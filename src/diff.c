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

#شامل "vim.h"
#شامل "xdiff/xdiff.h"

#اگر defined(FEAT_DIFF)

ایستا صحیح diff_busy = FALSE;	    // using diff structs, don't change them
ایستا صحیح diff_need_update = FALSE; // ex_diffupdate needs to be called

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
ایستا صحیح	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ایستا long diff_algorithm = XDF_INDENT_HEURISTIC;
ایستا صحیح diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

ایستا صحیح diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# اگر defined(MSWIN)
ایستا صحیح diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
تعریفنوع ساختار {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
تعریفنوع ساختار {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
تعریفنوع ساختار {
    linenr_T lnum_orig;
    long     شمار_orig;
    linenr_T lnum_new;
    long     شمار_new;
} diffhunk_T;

تعریفنوع شمارش {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
تعریفنوع ساختار {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    صحیح		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    صحیح		    dio_ctxlen;   // unified diff context length
} diffio_T;

ایستا صحیح diff_buf_idx(buf_T *buf);
ایستا صحیح diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ایستا تهی diff_mark_adjust_tp(tabpage_T *tp, صحیح idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
ایستا تهی diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ایستا صحیح diff_check_sanity(tabpage_T *tp, diff_T *dp);
ایستا صحیح check_external_diff(diffio_T *diffio);
ایستا صحیح diff_file(diffio_T *diffio);
ایستا صحیح diff_equal_entry(diff_T *dp, صحیح idx1, صحیح idx2);
ایستا صحیح diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ایستا تهی diff_fold_update(diff_T *dp, صحیح skip_idx);
# endif
ایستا تهی diff_read(صحیح idx_orig, صحیح idx_new, diffio_T *dio);
ایستا تهی diff_copy_entry(diff_T *dprev, diff_T *dp, صحیح idx_orig, صحیح idx_new);
ایستا diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ایستا صحیح parse_diff_ed(char_u *سطر, diffhunk_T *hunk);
ایستا صحیح parse_diff_unified(char_u *سطر, diffhunk_T *hunk);
ایستا صحیح xdiff_out_indices(long start_a, long شمار_a, long start_b, long شمار_b, تهی *priv);
ایستا صحیح xdiff_out_unified(تهی *priv, mmbuffer_t *mb, صحیح nbuf);
ایستا صحیح parse_diffanchors(صحیح check_only, buf_T *buf, linenr_T *anchors, صحیح *رقم_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    برای ((dp) = (tp)->tp_first_diff; (dp) != پوچ; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ایستا diff_T	    *diff_finger_dp = پوچ;
ایستا tabpage_T    *diff_finger_tp = پوچ;
ایستا صحیح	    diff_finger_idx = -1;
ایستا linenr_T	    diff_finger_lnum = 0;

    ایستا تهی
clear_diffblock(diff_T *dp)
{
    اگر (dp == diff_finger_dp)
	diff_finger_dp = پوچ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ایستا diff_T *
diff_scan_start(tabpage_T *tp, صحیح idx, linenr_T lnum)
{
    اگر (diff_finger_dp != پوچ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	بازگشت diff_finger_dp;
    بازگشت tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ایستا تهی
diff_scan_remember(tabpage_T *tp, صحیح idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != پوچ ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    تهی
diff_buf_delete(buf_T *buf)
{
    صحیح		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	اگر (i != DB_شمار)
	{
	    tp->tp_diffbuf[i] = پوچ;
	    tp->tp_diff_invalid = TRUE;
	    اگر (tp == curtab)
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
    تهی
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    صحیح		i;

    اگر (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		بشکن;
	اگر (wp == پوچ)
	{
	    i = diff_buf_idx(win->w_buffer);
	    اگر (i != DB_شمار)
	    {
		curtab->tp_diffbuf[i] = پوچ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    وگرنه
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
    تهی
diff_buf_add(buf_T *buf)
{
    صحیح		i;

    اگر (diff_buf_idx(buf) != DB_شمار)
	بازگشت;		// It's already there.

    برای (i = 0; i < DB_شمار; ++i)
	اگر (curtab->tp_diffbuf[i] == پوچ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    بازگشت;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_شمار);
}

/*
 * Remove all buffers to make diffs for.
 */
    ایستا تهی
diff_buf_clear(تهی)
{
    صحیح		i;

    برای (i = 0; i < DB_شمار; ++i)
	اگر (curtab->tp_diffbuf[i] != پوچ)
	{
	    curtab->tp_diffbuf[i] = پوچ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ایستا صحیح
diff_buf_idx(buf_T *buf)
{
    صحیح		idx;

    برای (idx = 0; idx < DB_شمار; ++idx)
	اگر (curtab->tp_diffbuf[idx] == buf)
	    بشکن;
    بازگشت idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ایستا صحیح
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    صحیح		idx;

    برای (idx = 0; idx < DB_شمار; ++idx)
	اگر (tp->tp_diffbuf[idx] == buf)
	    بشکن;
    بازگشت idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    تهی
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    صحیح		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	اگر (i != DB_شمار)
	{
	    tp->tp_diff_invalid = TRUE;
	    اگر (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    تهی
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    صحیح		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	اگر (idx != DB_شمار)
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
    ایستا تهی
diff_mark_adjust_tp(
    tabpage_T	*tp,
    صحیح		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    صحیح		i;
    صحیح		inserted, deleted;
    صحیح		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    صحیح		check_unchanged;

    diff_finger_dp = پوچ;		// may shift df_lnum, invalidate the finger

    اگر (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    اگر (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    وگرنه اگر (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    وگرنه
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = پوچ;
    dp = tp->tp_first_diff;
    برای (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	اگر ((dp == پوچ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == پوچ
		    || dprev->df_lnum[idx] + dprev->df_شمار[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    اگر (dnext == پوچ)
		بازگشت;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_شمار[idx] = inserted;
	    برای (i = 0; i < DB_شمار; ++i)
		اگر (tp->tp_diffbuf[i] != پوچ && i != idx)
		{
		    اگر (dprev == پوچ)
			dnext->df_lnum[i] = line1;
		    وگرنه
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_شمار[i])
			    - (dprev->df_lnum[idx] + dprev->df_شمار[idx]);
		    dnext->df_شمار[i] = deleted;
		}
	}

	// if at end of the list, quit
	اگر (dp == پوچ)
	    بشکن;

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
	last = dp->df_lnum[idx] + dp->df_شمار[idx] - 1;

	// 1. change completely above line1: nothing to do
	اگر (last >= line1 - 1)
	{
	    اگر (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		اگر (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		ادامه;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    اگر (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		اگر (amount_after == 0)
		    بشکن;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    وگرنه
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		اگر (deleted > 0)
		{
		    off = 0;
		    اگر (dp->df_lnum[idx] >= line1)
		    {
			اگر (last <= line2)
			{
			    // 4. delete all lines of diff
			    اگر (dp->df_next != پوچ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_شمار[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    وگرنه
				n = deleted - dp->df_شمار[idx];
			    dp->df_شمار[idx] = 0;
			}
			وگرنه
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_شمار[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    وگرنه
		    {
			اگر (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_شمار[idx] -= last - lnum_deleted + 1;
			    اگر (dp->df_next != پوچ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    وگرنه
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			وگرنه
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_شمار[idx] -= deleted;
			}
		    }

		    برای (i = 0; i < DB_شمار; ++i)
			اگر (tp->tp_diffbuf[i] != پوچ && i != idx)
			{
			    اگر (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    وگرنه
				dp->df_lnum[i] = 1;
			    dp->df_شمار[i] += n;
			}
		}
		وگرنه
		{
		    اگر (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_شمار[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    وگرنه
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		اگر (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	اگر (dprev != پوچ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_شمار[idx]
							== dp->df_lnum[idx])
	{
	    برای (i = 0; i < DB_شمار; ++i)
		اگر (tp->tp_diffbuf[i] != پوچ)
		    dprev->df_شمار[i] += dp->df_شمار[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	وگرنه
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = پوچ;
    dp = tp->tp_first_diff;
    وقتی (dp != پوچ)
    {
	// All counts are zero, remove this entry.
	برای (i = 0; i < DB_شمار; ++i)
	    اگر (tp->tp_diffbuf[i] != پوچ && dp->df_شمار[i] != 0)
		بشکن;
	اگر (i == DB_شمار)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    اگر (dprev == پوچ)
		tp->tp_first_diff = dnext;
	    وگرنه
		dprev->df_next = dnext;
	}
	وگرنه
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    اگر (tp == curtab)
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
    ایستا diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    اگر (dnew == پوچ)
	بازگشت پوچ;
    diff_finger_dp = پوچ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    اگر (dprev == پوچ)
	tp->tp_first_diff = dnew;
    وگرنه
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, اندازه(diffline_change_T), 20);
    بازگشت dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ایستا تهی
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    صحیح		i_org;
    صحیح		i_new;
    صحیح		off_org, off_new;
    char_u	*سطر_org;
    صحیح		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    برای (i_org = 0; i_org < DB_شمار; ++i_org)
	اگر (tp->tp_diffbuf[i_org] != پوچ)
	    بشکن;
    اگر (i_org == DB_شمار)	// safety check
	بازگشت;

    اگر (diff_check_sanity(tp, dp) == FAIL)
	بازگشت;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    برای (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	وقتی (dp->df_شمار[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    اگر (dir == BACKWARD)
		off_org = dp->df_شمار[i_org] - 1;
	    سطر_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    اگر (سطر_org == پوچ)
		بازگشت;
	    برای (i_new = i_org + 1; i_new < DB_شمار; ++i_new)
	    {
		اگر (tp->tp_diffbuf[i_new] == پوچ)
		    ادامه;
		اگر (dir == BACKWARD)
		    off_new = dp->df_شمار[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		اگر (off_new < 0 || off_new >= dp->df_شمار[i_new])
		    بشکن;
		اگر (diff_cmp(سطر_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    بشکن;
	    }
	    vim_free(سطر_org);

	    // Stop when a line isn't equal in all diff buffers.
	    اگر (i_new != DB_شمار)
		بشکن;

	    // Line matched in all buffers, remove it from the diff.
	    برای (i_new = i_org; i_new < DB_شمار; ++i_new)
		اگر (tp->tp_diffbuf[i_new] != پوچ)
		{
		    اگر (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_شمار[i_new];
		}
	}
	اگر (dir == BACKWARD)
	    بشکن;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ایستا صحیح
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    صحیح		i;

    برای (i = 0; i < DB_شمار; ++i)
	اگر (tp->tp_diffbuf[i] != پوچ)
	    اگر (dp->df_lnum[i] + dp->df_شمار[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_سطر_شمار)
		بازگشت FAIL;
    بازگشت OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    تهی
diff_redraw(
    صحیح		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = پوچ;
    صحیح		used_max_fill_other = FALSE;
    صحیح		used_max_fill_curwin = FALSE;
    صحیح		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	اگر (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    ادامه;

	redraw_win_later(wp, UPD_SOME_VALID);
	اگر (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	اگر (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	اگر ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    اگر (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    وگرنه اگر (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		اگر (wp == curwin)
		    used_max_fill_curwin = TRUE;
		وگرنه اگر (wp_other != پوچ)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    اگر (wp_other != پوچ && curwin->w_p_scb)
    {
	اگر (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	وگرنه اگر (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    ایستا تهی
clear_diffin(diffin_T *din)
{
    اگر (din->din_fname == پوچ)
	VIM_CLEAR(din->din_mmfile.ptr);
    وگرنه
	mch_remove(din->din_fname);
}

    ایستا تهی
clear_diffout(diffout_T *dout)
{
    اگر (dout->dout_fname == پوچ)
	ga_clear_strings(&dout->dout_ga);
    وگرنه
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ایستا صحیح
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    اگر (end < 0)
	end = buf->b_ml.ml_سطر_شمار;

    اگر (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = پوچ;
	din->din_mmfile.size = 0;
	بازگشت OK;
    }

    // xdiff requires one big block of memory with all the text.
    برای (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    اگر (ptr == پوچ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	بازگشت FAIL;
    }
    din->din_mmfile.ptr = (نویسه *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    برای (lnum = start; lnum <= end; ++lnum)
    {
	برای (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    اگر (diff_flags & DIFF_ICASE)
	    {
		صحیح c;
		صحیح	orig_len;
		صحیح	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		اگر (*s == NL)
		    c = NUL;
		وگرنه
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		اگر (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		وگرنه
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    اگر (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    وگرنه
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    بازگشت OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ایستا صحیح
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    صحیح		r;
    صحیح		save_ml_flags;
    char_u	*save_ff;
    صحیح		save_cmod_flags;

    اگر (din->din_fname == پوچ)
	بازگشت diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    اگر (frames_locked())
	بازگشت FAIL;

    اگر (end < 0)
	end = buf->b_ml.ml_سطر_شمار;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    اگر (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, پوچ,
			start, end,
			پوچ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    بازگشت r;
}

    ایستا صحیح
lnum_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    اگر (lnum1 < lnum2)
	بازگشت -1;
    اگر (lnum1 > lnum2)
	بازگشت 1;
    بازگشت 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ایستا تهی
diff_try_update(
	diffio_T    *dio,
	صحیح	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    صحیح		idx_new;

    اگر (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, اندازه(نویسه *), 1000);
    }
    وگرنه
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	اگر (dio->dio_orig.din_fname == پوچ
		|| dio->dio_new.din_fname == پوچ
		|| dio->dio_diff.dout_fname == پوچ)
	    برو theend;
    }

    // Check external diff is actually working.
    اگر (!dio->dio_internal && check_external_diff(dio) == FAIL)
	برو theend;

    // :diffupdate!
    اگر (eap != پوچ && eap->forceit)
	برای (idx_new = idx_orig; idx_new < DB_شمار; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    اگر (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    صحیح رقم_anchors = INT_MAX;
    linenr_T anchors[DB_شمار][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    اگر (diff_flags & DIFF_ANCHOR)
    {
	برای (صحیح idx = 0; idx < DB_شمار; idx++)
	{
	    اگر (curtab->tp_diffbuf[idx] == پوچ)
		ادامه;
	    صحیح buf_رقم_anchors = 0;
	    اگر (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_رقم_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		رقم_anchors = 0;
		CLEAR_FIELD(anchors);
		بشکن;
	    }
	    اگر (buf_رقم_anchors < رقم_anchors)
		رقم_anchors = buf_رقم_anchors;

	    اگر (buf_رقم_anchors > 0)
		qsort((تهی *)anchors[idx],
			(size_t)buf_رقم_anchors,
			اندازه(linenr_T),
			lnum_compare);
	}
    }
    اگر (رقم_anchors == INT_MAX)
	رقم_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    برای (صحیح anchor_i = 0; anchor_i <= رقم_anchors; anchor_i++)
    {
	diff_T	*orig_diff = پوچ;
	اگر (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = پوچ;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == رقم_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	اگر (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    اگر (orig_diff != پوچ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    برو theend;
	}

	// Make a difference between the first buffer and every other.
	برای (idx_new = idx_orig + 1; idx_new < DB_شمار; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
		ادامه; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == رقم_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    اگر (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		ادامه;
	    اگر (diff_file(dio) == FAIL)
		ادامه;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	اگر (anchor_i != 0)
	{
	    diff_finger_dp = پوچ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    برای (diff_T *dp = curtab->tp_first_diff; dp != پوچ; dp = dp->df_next)
	    {
		برای (صحیح idx = 0; idx < DB_شمار; idx++)
		{
		    اگر (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    اگر (orig_diff != پوچ)
	    {
		diff_T *last_diff = orig_diff;
		وقتی (last_diff->df_next != پوچ)
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
    صحیح
diff_internal(تهی)
{
    بازگشت (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ایستا صحیح
diff_internal_failed(تهی)
{
    صحیح idx;

    // Only need to do something when there is another buffer.
    برای (idx = 0; idx < DB_شمار; ++idx)
	اگر (curtab->tp_diffbuf[idx] != پوچ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    تهی
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    صحیح		idx_orig;
    صحیح		idx_new;
    diffio_T	diffio;
    صحیح		had_diffs = curtab->tp_first_diff != پوچ;

    اگر (diff_busy)
    {
	diff_need_update = TRUE;
	بازگشت;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    برای (idx_orig = 0; idx_orig < DB_شمار; ++idx_orig)
	اگر (curtab->tp_diffbuf[idx_orig] != پوچ)
	    بشکن;
    اگر (idx_orig == DB_شمار)
	برو theend;

    // Only need to do something when there is another buffer.
    برای (idx_new = idx_orig + 1; idx_new < DB_شمار; ++idx_new)
	اگر (curtab->tp_diffbuf[idx_new] != پوچ)
	    بشکن;
    اگر (idx_new == DB_شمار)
	برو theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    اگر (diffio.dio_internal && diff_internal_failed())
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
    اگر (had_diffs || curtab->tp_first_diff != پوچ)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, پوچ, پوچ, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ایستا صحیح
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    صحیح		ok;
    صحیح		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    برای (;;)
    {
	ok = FALSE;
	fd = mch_fopen((نویسه *)diffio->dio_orig.din_fname, "w");
	اگر (fd == پوچ)
	    io_error = TRUE;
	وگرنه
	{
	    اگر (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((نویسه *)diffio->dio_new.din_fname, "w");
	    اگر (fd == پوچ)
		io_error = TRUE;
	    وگرنه
	    {
		اگر (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = پوچ;
		اگر (diff_file(diffio) == OK)
		    fd = mch_fopen((نویسه *)diffio->dio_diff.dout_fname, "r");
		اگر (fd == پوچ)
		    io_error = TRUE;
		وگرنه
		{
		    char_u	linebuf[LBUFLEN];

		    برای (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			اگر (vim_fgets(linebuf, LBUFLEN, fd))
			    بشکن;
			اگر (STRNCMP(linebuf, "1c1", 3) == 0
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
	اگر (*p_dex != NUL)
	    بشکن;
# endif

# اگر defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	اگر (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    ادامه;
	}
	اگر (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	اگر (diff_a_works != MAYBE)
	    بشکن;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	اگر (ok)
	    بشکن;
    }
    اگر (!ok)
    {
	اگر (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# اگر defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Invoke the xdiff function.
 */
    ایستا صحیح
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    اگر (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    اگر (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    اگر (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    اگر (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_سطرها;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    اگر (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    وگرنه
	emit_cb.out_سطر = xdiff_out_unified;
    اگر (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	بازگشت FAIL;
    }
    اگر (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ایستا صحیح
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    اگر (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	بازگشت OK;
    }
    وگرنه
# endif
    // Use xdiff for generating the diff.
    اگر (dio->dio_internal)
	بازگشت diff_file_internal(dio);

    اگر (check_restricted())
	بازگشت FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    اگر (cmd == پوچ)
	بازگشت FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    اگر (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((نویسه *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# اگر defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# وگرنه
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (صحیح)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (تهی)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    بازگشت OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    تهی
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = پوچ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = پوچ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = پوچ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = پوچ;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = پوچ;

# ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, پوچ, پوچ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), پوچ);
	اگر (browseFile == پوچ)
	    بازگشت;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    اگر (tmp_orig == پوچ || tmp_new == پوچ)
	برو theend;

    // Write the current buffer to "tmp_orig".
    اگر (buf_write(curbuf, tmp_orig, پوچ,
		(linenr_T)1, curbuf->b_ml.ml_سطر_شمار,
				     پوچ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	برو theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != پوچ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    اگر (esc_name == پوچ)
	برو theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    اگر (buf == پوچ)
	برو theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    اگر (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((نویسه *)dirbuf) != 0)
	dirbuf[0] = NUL;
    وگرنه
    {
#  ifdef TEMPDIRNAMES
	اگر (vim_tempdir != پوچ)
	    vim_ignored = mch_chdir((نویسه *)vim_tempdir);
	وگرنه
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    اگر (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != پوچ ? fullname :
#  endif
		eap->arg, tmp_new);
    وگرنه
# endif
    {
	اگر (check_restricted())
	    برو theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((نویسه *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(تهی)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    اگر (dirbuf[0] != NUL)
    {
	اگر (mch_chdir((نویسه *)dirbuf) != 0)
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
    اگر (mch_stat((نویسه *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    وگرنه
    {
	اگر (curbuf->b_fname != پوچ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    اگر (newname != پوچ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	اگر (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    اگر (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		اگر (newname != پوچ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    اگر (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    اگر (tmp_orig != پوچ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    اگر (tmp_new != پوچ)
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
    تهی
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

    اگر (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	بازگشت;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    اگر (curwin == old_curwin)		// split didn't work
	بازگشت;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    اگر (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	اگر (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_سطر(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    تهی
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ایستا تهی
set_diff_option(win_T *wp, صحیح مقدار)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_مقدار_give_err((char_u *)"diff", (long)مقدار, پوچ, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    تهی
diff_win_options(
    win_T	*wp,
    صحیح		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    اگر (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    اگر (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    اگر (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	اگر (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    اگر (!wp->w_p_diff)
    {
	اگر (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    اگر (!wp->w_p_diff)
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
    اگر (vim_strchr(p_sbo, 'h') == پوچ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    اگر (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    تهی
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    صحیح		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    اگر (wp->w_p_diff_saved)
	    {

		اگر (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		اگر (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		اگر (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    اگر (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		اگر (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		اگر (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		اگر (wp->w_p_fen)
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
    اگر (eap->forceit)
	diff_buf_clear();

    اگر (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    اگر (!diffwin && vim_strchr(p_sbo, 'h') != پوچ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ایستا تهی
diff_read(
    صحیح		idx_orig,	// idx of original file
    صحیح		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = پوچ;
    صحیح		سطر_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = پوچ;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*سطر;
    long	off;
    صحیح		i;
    صحیح		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = پوچ;	    // init to avoid gcc warning

    شمارش {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    اگر (dout->dout_fname == پوچ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    وگرنه
    {
	fd = mch_fopen((نویسه *)dout->dout_fname, "r");
	اگر (fd == پوچ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    بازگشت;
	}
    }

    اگر (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	اگر (hunk == پوچ)
	{
	    اگر (fd != پوچ)
		fclose(fd);
	    بازگشت;
	}
    }

    برای (;;)
    {
	اگر (dio->dio_internal)
	{
	    اگر (سطر_hunk_idx >= dout->dout_ga.ga_len)
		بشکن;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[سطر_hunk_idx++];
	}
	وگرنه
	{
	    اگر (fd == پوچ)
	    {
		اگر (سطر_hunk_idx >= dout->dout_ga.ga_len)
		    بشکن;	    // did last line
		سطر = ((char_u **)dout->dout_ga.ga_data)[سطر_hunk_idx++];
	    }
	    وگرنه
	    {
		اگر (vim_fgets(linebuf, LBUFLEN, fd))
		    بشکن;		// end of file
		سطر = linebuf;
	    }

	    اگر (diffstyle == DIFF_NONE)
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
		اگر (SAFE_isdigit(*سطر))
		    diffstyle = DIFF_ED;
		وگرنه اگر ((STRNCMP(سطر, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		وگرنه اگر ((STRNCMP(سطر, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(سطر, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(سطر, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		وگرنه
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ادامه;
	    }

	    اگر (diffstyle == DIFF_ED)
	    {
		اگر (!SAFE_isdigit(*سطر))
		    ادامه;	// not the start of a diff block
		اگر (parse_diff_ed(سطر, hunk) == FAIL)
		    ادامه;
	    }
	    وگرنه اگر (diffstyle == DIFF_UNIFIED)
	    {
		اگر (STRNCMP(سطر, "@@ ", 3)  != 0)
		    ادامه;	// not the start of a diff block
		اگر (parse_diff_unified(سطر, hunk) == FAIL)
		    ادامه;
	    }
	    وگرنه
	    {
		emsg(_(e_invalid_diff_format));
		بشکن;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	وقتی (dp != پوچ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_شمار[idx_orig])
	{
	    اگر (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	اگر (dp != پوچ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_شمار[idx_orig]
		&& hunk->lnum_orig + hunk->شمار_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    برای (dpl = dp; dpl->df_next != پوچ; dpl = dpl->df_next)
		اگر (hunk->lnum_orig + hunk->شمار_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    بشکن;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    اگر (off > 0)
	    {
		برای (i = idx_orig; i < idx_new; ++i)
		    اگر (curtab->tp_diffbuf[i] != پوچ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_شمار[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_شمار[idx_new] = hunk->شمار_new;
	    }
	    وگرنه اگر (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_شمار[idx_new] = hunk->شمار_new - off;
	    }
	    وگرنه
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		صحیح orig_size_in_dp = MIN(hunk->شمار_orig,
			dp->df_lnum[idx_orig] +
			dp->df_شمار[idx_orig] - hunk->lnum_orig);
		صحیح size_diff = hunk->شمار_new - orig_size_in_dp;
		dp->df_شمار[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->شمار_new
		    - (dp->df_lnum[idx_new] + dp->df_شمار[idx_new]);
		اگر (off > 0)
		    dp->df_شمار[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->شمار_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_شمار[idx_orig]);
	    اگر (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		اگر (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_شمار[idx_new] += -off;
		}
		off = 0;
	    }
	    برای (i = idx_orig; i < idx_new; ++i)
		اگر (curtab->tp_diffbuf[i] != پوچ)
		    dp->df_شمار[i] = dpl->df_lnum[i] + dpl->df_شمار[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    وقتی (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	وگرنه
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    اگر (dp == پوچ)
		برو done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_شمار[idx_orig] = hunk->شمار_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_شمار[idx_new] = hunk->شمار_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    برای (i = idx_orig + 1; i < idx_new; ++i)
		اگر (curtab->tp_diffbuf[i] != پوچ)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    وقتی (dp != پوچ)
    {
	اگر (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    اگر (!dio->dio_internal)
	vim_free(hunk);

    اگر (fd != پوچ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ایستا تهی
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    صحیح		idx_orig,
    صحیح		idx_new)
{
    long	off;

    اگر (dprev == پوچ)
	off = 0;
    وگرنه
	off = (dprev->df_lnum[idx_orig] + dprev->df_شمار[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_شمار[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_شمار[idx_new] = dp->df_شمار[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    تهی
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    برای (p = tp->tp_first_diff; p != پوچ; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = پوچ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ایستا صحیح
diff_linematch(diff_T *dp)
{
    اگر (!(diff_flags & DIFF_LINEMATCH))
	بازگشت 0;

    // are there more than three diff buffers?
    صحیح tsize = 0;
    برای (صحیح i = 0; i < DB_شمار; i++)
    {
	اگر (curtab->tp_diffbuf[i] != پوچ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    اگر (dp->df_شمار[i] < 0)
		بازگشت FALSE;
	    tsize += dp->df_شمار[i];
	}
    }

    // avoid allocating a huge array because it will lag
    بازگشت tsize <= linematch_سطرها;
}

    ایستا صحیح
get_max_diff_length(ثابت diff_T *dp)
{
    صحیح maxlength = 0;

    برای (صحیح k = 0; k < DB_شمار; k++)
    {
	اگر (curtab->tp_diffbuf[k] != پوچ)
	{
	    اگر (dp->df_شمار[k] > maxlength)
		maxlength = dp->df_شمار[k];
	}
    }
    بازگشت maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ایستا تهی
find_بالاترین_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    صحیح		fromidx,
    صحیح		topline)
{
    diff_T	*topdiff = پوچ;
    diff_T	*localtopdiff = پوچ;
    صحیح		topdiffchange = 0;

    برای (topdiff = curtab->tp_first_diff; topdiff != پوچ;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	اگر (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	اگر (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_شمار[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    اگر ((*thistopdiff) == پوچ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	اگر (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_شمار[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    اگر (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		بشکن;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ایستا تهی
calculate_topfill_and_topline(
    ثابت صحیح	fromidx,
    ثابت صحیح	toidx,
    ثابت صحیح	from_topline,
    ثابت صحیح	from_topfill,
    صحیح		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = پوچ;
    diff_T	*next_adjacent_blocks = پوچ;
    صحیح		virtual_سطرها_passed = 0;
    صحیح		curlinenum_to = 1;

    find_بالاترین_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    وقتی (curdif && (curdif->df_lnum[fromidx] + curdif->df_شمار[fromidx])
							<= from_topline)
    {
	virtual_سطرها_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    اگر (curdif != next_adjacent_blocks)
	virtual_سطرها_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_سطرها_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    اگر (virtual_سطرها_passed < 0)
	virtual_سطرها_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    اگر (thistopdiff != پوچ) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    صحیح virt_سطرها_چپ = virtual_سطرها_passed;
    curdif = thistopdiff;
    وقتی (virt_سطرها_چپ > 0 && curdif != پوچ && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_سطرها_چپ, curdif->df_شمار[toidx]);
	virt_سطرها_چپ -= MIN(virt_سطرها_چپ, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    صحیح max_virt_سطرها = 0;
    برای (diff_T *dp = thistopdiff; dp != پوچ; dp = dp->df_next)
    {
	اگر (dp->df_lnum[toidx] + dp->df_شمار[toidx] <= curlinenum_to)
	    max_virt_سطرها += get_max_diff_length(dp);
	وگرنه
	{
	    اگر (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_سطرها += curlinenum_to - dp->df_lnum[toidx];
	    بشکن;
	}
    }

    اگر (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_سطرها - virtual_سطرها_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ایستا تهی
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    ثابت صحیح	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    صحیح		سطر_numbers[DB_شمار];
    صحیح		outputmap[DB_شمار];
    size_t	ndiffs = 0;

    برای (صحیح i = 0; i < DB_شمار; i++)
    {
	اگر (curtab->tp_diffbuf[i] != پوچ)
	{
	    سطر_numbers[i] = dp->df_lnum[i];
	    dp->df_شمار[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    برای (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	اگر (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    برای (صحیح j = 0; j < DB_شمار; j++)
	    {
		اگر (curtab->tp_diffbuf[j] != پوچ)
		{
		    dp_s->df_lnum[j] = سطر_numbers[j];
		    dp_s->df_شمار[j] = 0;
		}
	    }
	}
	برای (size_t j = 0; j < ndiffs; j++)
	{
	    اگر (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_شمار[outputmap[j]]++;
		سطر_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ایستا تهی
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_شمار];
    ثابت mmfile_t	*diffbufs[DB_شمار];
    صحیح			diff_length[DB_شمار];
    size_t		ndiffs = 0;

    برای (صحیح i = 0; i < DB_شمار; i++)
    {
	اگر (curtab->tp_diffbuf[i] != پوچ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    اگر (dp->df_شمار[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_شمار[i] - 1);
	    }
	    وگرنه
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = پوچ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_شمار[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    صحیح *decisions = پوچ;
    ثابت صحیح iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    برای (size_t i = 0; i < ndiffs; i++)
	آزاد(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    آزاد(decisions);
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
    صحیح
diff_check_with_linestatus(win_T *wp, linenr_T lnum, صحیح *linestatus)
{
    صحیح		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    صحیح		maxcount;
    صحیح		i;
    buf_T	*buf = wp->w_buffer;
    صحیح		cmp;

    اگر (linestatus != پوچ)
	*linestatus = 0;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    اگر (curtab->tp_first_diff == پوچ || !wp->w_p_diff)	// no diffs at all
	بازگشت 0;

    // safety check: "lnum" must be a buffer line
    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_شمار + 1)
	بازگشت 0;

    idx = diff_buf_idx(buf);
    اگر (idx == DB_شمار)
	بازگشت 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    اگر (hasFoldingWin(wp, lnum, پوچ, پوچ, TRUE, پوچ))
	بازگشت 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = پوچ;		// last block with df_lnum[idx] <= lnum
    برای (dp = diff_scan_start(curtab, idx, lnum); dp != پوچ; dp = dp->df_next)
    {
	اگر (dp->df_lnum[idx] <= lnum)
	    le = dp;
	اگر (lnum <= dp->df_lnum[idx] + dp->df_شمار[idx])
	    بشکن;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    اگر (dp == پوچ || lnum < dp->df_lnum[idx])
	بازگشت 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    اگر (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    صحیح رقم_fill = 0;
    وقتی (lnum == dp->df_lnum[idx] + dp->df_شمار[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	اگر (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    رقم_fill += maxcount - dp->df_شمار[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	اگر (dp->df_next != پوچ
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_شمار[idx])
	    dp = dp->df_next;
	وگرنه
	    بشکن;
    }

    اگر (lnum < dp->df_lnum[idx] + dp->df_شمار[idx])
    {
	صحیح	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	برای (i = 0; i < DB_شمار; ++i)
	    اگر (i != idx && curtab->tp_diffbuf[i] != پوچ)
	    {
		اگر (dp->df_شمار[i] == 0)
		    zero = TRUE;
		وگرنه
		{
		    اگر (dp->df_شمار[i] != dp->df_شمار[idx])
		    {
			اگر (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			بازگشت رقم_fill;
		    }
		    cmp = TRUE;
		}
	    }
	اگر (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    برای (i = 0; i < DB_شمار; ++i)
		اگر (i != idx && curtab->tp_diffbuf[i] != پوچ
						      && dp->df_شمار[i] != 0)
		    اگر (!diff_equal_entry(dp, idx, i))
		    {
			اگر (linestatus)
			    *linestatus = -1;
			بازگشت رقم_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	اگر (zero == FALSE)
	    بازگشت رقم_fill;
	اگر (linestatus)
	    *linestatus = -2;
	بازگشت رقم_fill;
    }
    بازگشت رقم_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ایستا صحیح
diff_equal_entry(diff_T *dp, صحیح idx1, صحیح idx2)
{
    صحیح		i;
    char_u	*سطر;
    صحیح		cmp;

    اگر (dp->df_شمار[idx1] != dp->df_شمار[idx2])
	بازگشت FALSE;
    اگر (diff_check_sanity(curtab, dp) == FAIL)
	بازگشت FALSE;
    برای (i = 0; i < dp->df_شمار[idx1]; ++i)
    {
	سطر = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	اگر (سطر == پوچ)
	    بازگشت FALSE;
	cmp = diff_cmp(سطر, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(سطر);
	اگر (cmp != 0)
	    بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ایستا صحیح
diff_equal_char(char_u *p1, char_u *p2, صحیح *len)
{
    صحیح l  = (*mb_ptr2len)(p1);

    اگر (l != (*mb_ptr2len)(p2))
	بازگشت FALSE;
    اگر (l > 1)
    {
	اگر (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    بازگشت FALSE;
	*len = l;
    }
    وگرنه
    {
	اگر ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    بازگشت FALSE;
	*len = 1;
    }
    بازگشت TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ایستا صحیح
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    صحیح		l;

    اگر ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	بازگشت 0;

    اگر ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	بازگشت STRCMP(s1, s2);
    اگر ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	بازگشت MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    وقتی (*p1 != NUL && *p2 != NUL)
    {
	اگر (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	وگرنه
	{
	    اگر (!diff_equal_char(p1, p2, &l))
		بشکن;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    اگر (*p1 != NUL || *p2 != NUL)
	بازگشت 1;
    بازگشت 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    صحیح
diff_check_fill(win_T *wp, linenr_T lnum)
{
    صحیح		n;

    // be quick when there are no filler lines
    اگر (!(diff_flags & DIFF_FILLER))
	بازگشت 0;
    n = diff_check_with_linestatus(wp, lnum, پوچ);
    اگر (n <= 0)
	بازگشت 0;
    بازگشت n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    تهی
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    صحیح		fromidx;
    صحیح		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    اگر (fromidx == DB_شمار)
	بازگشت;		// safety check

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[fromidx] + dp->df_شمار[fromidx])
	    بشکن;
    اگر (dp == پوچ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_سطر_شمار
				       - (frombuf->b_ml.ml_سطر_شمار - lnum);
    }
    وگرنه
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	اگر (toidx == DB_شمار)
	    بازگشت;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	اگر (lnum >= dp->df_lnum[fromidx])
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
    اگر (towin->w_topline > towin->w_buffer->b_ml.ml_سطر_شمار)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_سطر_شمار;
	towin->w_botfill = true;
    }
    اگر (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_سطر_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (تهی)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    پوچ, TRUE, پوچ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ایستا صحیح
parse_diffanchors(
    صحیح		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    صحیح		*رقم_anchors)
{
    صحیح i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = پوچ;
    اگر (check_only)
	bufwin = curwin;
    وگرنه
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    اگر (bufwin->w_buffer == buf && bufwin->w_p_diff)
		بشکن;
	اگر (bufwin == پوچ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    بازگشت FAIL;
	}
    }

    برای (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	اگر (*dia == ',') // don't allow empty values
	    بازگشت FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(پوچ, &dia, ADDR_سطرها, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	اگر (dia == پوچ) // error detected
	    بازگشت FAIL;
	اگر (*dia != ',' && *dia != NUL)
	    بازگشت FAIL;

	اگر (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_سطر_شمار + 1))
	{
	    emsg(_(e_invalid_range));
	    بازگشت FAIL;
	}

	اگر (anchors != پوچ)
	    anchors[i] = lnum;

	اگر (*dia == ',')
	    dia++;
    }
    اگر (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	بازگشت FAIL;
    }
    اگر (رقم_anchors != پوچ)
	*رقم_anchors = i;
    بازگشت OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    صحیح
diffanchors_changed(صحیح buflocal)
{
    صحیح result = parse_diffanchors(TRUE, curbuf, پوچ, پوچ);
    اگر (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    اگر (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    وگرنه
	    {
		برای (صحیح idx = 0; idx < DB_شمار; ++idx)
		    اگر (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			بشکن;
		    }
	    }
	}
    }
    بازگشت result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    صحیح
diffopt_changed(تهی)
{
    char_u	*p;
    صحیح		diff_context_new = 6;
    صحیح		linematch_سطرها_new = 0;
    صحیح		diff_flags_new = 0;
    صحیح		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    وقتی (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	اگر (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	وگرنه اگر (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	وگرنه اگر (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	وگرنه اگر (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	وگرنه اگر (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	وگرنه اگر (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	وگرنه اگر (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	وگرنه اگر (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	وگرنه اگر (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	وگرنه اگر (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	وگرنه اگر (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	وگرنه اگر (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	وگرنه اگر (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	وگرنه اگر (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	وگرنه اگر (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	وگرنه اگر (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	وگرنه اگر (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    اگر (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    وگرنه اگر (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    وگرنه اگر (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    وگرنه اگر (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    وگرنه
		بازگشت FAIL;
	}
	وگرنه اگر (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    اگر (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    وگرنه اگر (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    وگرنه اگر (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    وگرنه اگر (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    وگرنه
		بازگشت FAIL;
	}
	وگرنه اگر (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_سطرها_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    اگر ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	بازگشت FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    اگر (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_سطرها = linematch_سطرها_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    بازگشت OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    صحیح
diffopt_horizontal(تهی)
{
    بازگشت (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    صحیح
diffopt_hiddenoff(تهی)
{
    بازگشت (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    صحیح
diffopt_closeoff(تهی)
{
    بازگشت (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    تهی
diff_update_سطر(linenr_T lnum)
{
    صحیح		idx;
    diff_T	*dp;

    اگر (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	بازگشت;

    idx = diff_buf_idx(curbuf);
    اگر (idx == DB_شمار)
	بازگشت;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[idx] + dp->df_شمار[idx])
	    بشکن;

    // clear the inline change cache as it's invalid
    اگر (dp != پوچ)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ایستا diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    صحیح
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    صحیح *change_start,
    صحیح *change_end)
{
    اگر (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    وگرنه
	*change_start = change->dc_start[diffline->bufidx];
    اگر (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    وگرنه
	*change_end = change->dc_end[diffline->bufidx];

    اگر (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	بازگشت FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    برای (صحیح i = 0; i < DB_شمار; i++)
    {
	اگر (i == diffline->bufidx)
	    ادامه;
	اگر (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    بازگشت FALSE;
	}
    }
    بازگشت TRUE;
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
    ایستا صحیح
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    صحیح		idx,
    صحیح		*startp,	// first char of the change
    صحیح		*endp)		// last char of the change
{
    char_u	*سطر_org;
    char_u	*سطر_new;
    صحیح		i;
    صحیح		si_org, si_new;
    صحیح		ei_org, ei_new;
    صحیح		off;
    صحیح		added = TRUE;
    char_u	*p1, *p2;
    صحیح		l;

    اگر (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	سطر_org = پوچ;
    }
    وگرنه
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	سطر_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	اگر (سطر_org == پوچ)
	    بازگشت FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    برای (i = 0; i < DB_شمار; ++i)
	اگر (curtab->tp_diffbuf[i] != پوچ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    اگر (off >= dp->df_شمار[i])
		ادامه;
	    added = FALSE;
	    اگر (diff_flags & DIFF_INLINE_NONE)
		بشکن; // early terminate as we only care about the return value

	    سطر_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    وقتی (سطر_org[si_org] != NUL)
	    {
		اگر (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(سطر_org[si_org])
					      && VIM_ISWHITE(سطر_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(سطر_org[si_org])
					    || VIM_ISWHITE(سطر_new[si_new]))))
		{
		    si_org = (صحیح)(skipwhite(سطر_org + si_org) - سطر_org);
		    si_new = (صحیح)(skipwhite(سطر_new + si_new) - سطر_new);
		}
		وگرنه
		{
		    اگر (!diff_equal_char(سطر_org + si_org, سطر_new + si_new,
									   &l))
			بشکن;
		    si_org += l;
		    si_new += l;
		}
	    }
	    اگر (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(سطر_org, سطر_org + si_org);
		si_new -= (*mb_head_off)(سطر_new, سطر_new + si_new);
	    }
	    اگر (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    اگر (سطر_org[si_org] != NUL || سطر_new[si_new] != NUL)
	    {
		ei_org = (صحیح)STRLEN(سطر_org);
		ei_new = (صحیح)STRLEN(سطر_new);
		وقتی (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    اگر (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(سطر_org[ei_org])
					      && VIM_ISWHITE(سطر_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(سطر_org[ei_org])
					    || VIM_ISWHITE(سطر_new[ei_new]))))
		    {
			وقتی (ei_org >= *startp
					     && VIM_ISWHITE(سطر_org[ei_org]))
			    --ei_org;
			وقتی (ei_new >= si_new
					     && VIM_ISWHITE(سطر_new[ei_new]))
			    --ei_new;
		    }
		    وگرنه
		    {
			p1 = سطر_org + ei_org;
			p2 = سطر_new + ei_new;
			p1 -= (*mb_head_off)(سطر_org, p1);
			p2 -= (*mb_head_off)(سطر_new, p2);
			اگر (!diff_equal_char(p1, p2, &l))
			    بشکن;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		اگر (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(سطر_org);
    بازگشت added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
تعریفنوع ساختار
{
    long byte_start;
    long رقم_bytes;
    صحیح lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ایستا تهی
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, صحیح idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    صحیح pass = 1;
    انجامبده
    {
	صحیح has_unmerged_gaps = FALSE;
	صحیح has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	وقتی (dp!= پوچ && dp->df_next != پوچ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    اگر (dp->df_lnum[idx1] + dp->df_شمار[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ادامه;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_شمار[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    اگر (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ادامه;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_شمار[idx1]);
	    اگر (gap <= 3)
	    {
		linenr_T max_df_شمار = 0;
		برای (صحیح i = 0; i < DB_شمار; i++)
		    max_df_شمار = MAX(max_df_شمار, dp->df_شمار[i] + dp->df_next->df_شمار[i]);

		اگر (max_df_شمار >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    برای (صحیح i = 0; i < DB_شمار; i++)
		    {
			dp->df_شمار[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_شمار[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    ادامه;
		}
		وگرنه
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	اگر (!has_unmerged_gaps || !has_merged_gaps)
	    بشکن;
    } وقتی (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ایستا تهی
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, صحیح idx1,
				  linenr_T start_lnum)
{
    صحیح pass = 1;
    انجامبده
    {
	diff_T *dp = dp_orig;

	وقتی (dp != پوچ && dp->df_next != پوچ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_شمار[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    اگر (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ادامه;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    اگر (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ادامه;
	    }

	    // Calculate the gap between blocks
	    صحیح gap_start = entry1->byte_start + entry1->رقم_bytes;
	    صحیح gap_end = entry2->byte_start;
	    صحیح gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    اگر (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		ادامه;
	    }

	    // Get the text between the two blocks
	    char_u *سطر = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = سطر + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    برای (صحیح i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		صحیح char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		اگر (char_class == 2)
		{
		    only_non_word = false;
		    بشکن;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    اگر (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		برای (صحیح i = 0; i < DB_شمار; i++)
		{
		    اگر (curtab->tp_diffbuf[i] != پوچ)
		    {
			// count bytes in the first block
			برای (صحیح k = 0; k < dp->df_شمار[i]; k++)
			{
			    صحیح idx = dp->df_lnum[i] + k - 1;
			    اگر (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].رقم_bytes;
			}
			// count bytes in the next block
			برای (صحیح k = 0; k < dp->df_next->df_شمار[i]; k++)
			{
			    صحیح idx = dp->df_next->df_lnum[i] + k - 1;
			    اگر (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].رقم_bytes;
			}
		    }
		}

		اگر (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    برای (صحیح i = 0; i < DB_شمار; i++)
		    {
			اگر (curtab->tp_diffbuf[i] != پوچ)
			{
			    dp->df_شمار[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_شمار[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    ادامه;
		}
	    }

	    dp = dp->df_next;
	}
    } وقتی (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ایستا تهی
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_شمار];
    garray_T	file1_str;
    garray_T	file2_str;
    صحیح		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, اندازه(نویسه *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = پوچ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_شمار]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, اندازه(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    برای (صحیح i = 0; i < DB_شمار; i++)
	ga_init2(&linemap[i], اندازه(linemap_entry_T), 128);

    برای (صحیح i = 0; i < DB_شمار; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
	    ادامه; // skip buffer that isn't loaded

	اگر (dp->df_شمار[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = پوچ;
	    ادامه;
	}

	اگر (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	برای (صحیح off = 0; off < dp->df_شمار[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    صحیح in_keyword = FALSE;

	    // iwhiteeol support vars
	    صحیح last_white = FALSE;
	    صحیح eol_ga_len = -1;
	    صحیح eol_linemap_len = -1;
	    صحیح eol_numlines = -1;

	    char_u *s;
	    برای (s = curline; *s != NUL;)
	    {
		صحیح new_in_keyword = FALSE;
		اگر (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		اگر (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		اگر (VIM_ISWHITE(*s))
		{
		    اگر (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			ادامه;
		    }
		    وگرنه اگر ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			اگر (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		وگرنه
		{
		    اگر ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		صحیح char_len = 1;
		اگر (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		وگرنه
		{
		    char_len = mb_ptr2len(s);

		    اگر (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    اگر (diff_flags & DIFF_ICASE)
		    {
			صحیح c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			صحیح c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			صحیح c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			اگر (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    وگرنه
			ga_concat_len(curstr, s, char_len);
		}

		اگر (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		اگر (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.رقم_bytes = char_len;
		    اگر (ga_grow(&linemap[i], 1) != OK)
			برو done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		وگرنه
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].رقم_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    اگر (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    اگر ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		اگر (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    اگر (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.رقم_bytes = اندازه(NL);
		اگر (ga_grow(&linemap[i], 1) != OK)
		    برو done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	اگر (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (نویسه *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	وگرنه
	{
	    dio.dio_orig.din_mmfile.ptr = (نویسه *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	اگر (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    صحیح diff_status = diff_file_internal(&dio);
	    اگر (diff_status == FAIL)
		برو done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    اگر (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    وگرنه اگر (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    برای (; new_diff != پوچ; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	برای (صحیح i = 0; i < DB_شمار; i++)
	{
	    اگر (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ادامه;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_شمار[i];

	    اگر (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    وگرنه
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    اگر (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    وگرنه اگر (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    وگرنه
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].رقم_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	اگر (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    برو done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, اندازه(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    برای (صحیح i = 0; i < DB_شمار; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    صحیح
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    صحیح		idx;
    صحیح		off;

    idx = diff_buf_idx(wp->w_buffer);
    اگر (idx == DB_شمار)	// cannot happen
	بازگشت FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	اگر (lnum < dp->df_lnum[idx] + dp->df_شمار[idx])
	    بشکن;
    اگر (dp == پوچ || diff_check_sanity(curtab, dp) == FAIL)
	بازگشت FALSE;

    اگر (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	بازگشت FALSE;
    off = lnum - dp->df_lnum[idx];

    اگر (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	صحیح	change_start = MAXCOL;	// first col of changed area
	صحیح	change_end = -1;	// last col of changed area
	صحیح	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->رقم_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	بازگشت ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    اگر (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    صحیح رقم_changes = 0;
    صحیح change_idx = 0;
    diffline->changes = پوچ;
    برای (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	اگر (change->dc_end_lnum_off[idx] < off)
	    ادامه;
	اگر (change->dc_start_lnum_off[idx] > off)
	    بشکن;
	اگر (diffline->changes == پوچ)
	    diffline->changes = change;
	رقم_changes++;
    }
    diffline->رقم_changes = رقم_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    صحیح added = FALSE;
    اگر (رقم_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	برای (صحیح i = 0; i < DB_شمار; i++)
	{
	    اگر (idx == i)
		ادامه;
	    اگر (curtab->tp_diffbuf[i] == پوچ)
		ادامه;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    اگر (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		بشکن;
	    }
	}
    }
    بازگشت added;
}

# اگر defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    صحیح
diff_infold(win_T *wp, linenr_T lnum)
{
    صحیح		i;
    صحیح		idx = -1;
    صحیح		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    اگر (!wp->w_p_diff)
	بازگشت FALSE;

    برای (i = 0; i < DB_شمار; ++i)
    {
	اگر (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	وگرنه اگر (curtab->tp_diffbuf[i] != پوچ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    اگر (idx == -1 || !other)
	بازگشت FALSE;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    اگر (curtab->tp_first_diff == پوچ)
	بازگشت TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = پوچ;		// last block with df_lnum[idx] <= lnum
    صحیح		result = TRUE;
    برای (dp = diff_scan_start(curtab, idx, lnum); dp != پوچ; dp = dp->df_next)
    {
	اگر (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	اگر (dp->df_lnum[idx] - diff_context > lnum)
	    بشکن;
	// If this change ends before the line we have a match.
	اگر (dp->df_lnum[idx] + dp->df_شمار[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    بشکن;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    بازگشت result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    تهی
nv_diffgetput(صحیح put, long شمار)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    اگر (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	بازگشت;
    }
# endif
    اگر (شمار == 0)
	ea.arg = (char_u *)"";
    وگرنه
    {
	vim_snprintf((نویسه *)buf, 30, "%ld", شمار);
	ea.arg = buf;
    }
    اگر (put)
	ea.cmdidx = CMD_diffput;
    وگرنه
	ea.cmdidx = CMD_diffget;
    ea.addr_شمار = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ایستا صحیح
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	اگر (dp == diff)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    تهی
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    صحیح		شمار;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    صحیح		idx_cur;
    صحیح		idx_other;
    صحیح		idx_from;
    صحیح		idx_to;
    صحیح		i;
    صحیح		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    صحیح		start_skip, end_skip;
    صحیح		new_شمار;
    صحیح		buf_empty;
    صحیح		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    اگر (idx_cur == DB_شمار)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	بازگشت;
    }

    اگر (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	برای (idx_other = 0; idx_other < DB_شمار; ++idx_other)
	    اگر (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != پوچ)
	    {
		اگر (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    بشکن;
		found_not_ma = TRUE;
	    }
	اگر (idx_other == DB_شمار)
	{
	    اگر (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    وگرنه
		emsg(_(e_no_other_buffer_in_diff_mode));
	    بازگشت;
	}

	// Check that there isn't a third buffer in the list
	برای (i = idx_other + 1; i < DB_شمار; ++i)
	    اگر (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != پوچ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		بازگشت;
	    }
    }
    وگرنه
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	وقتی (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	برای (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	اگر (eap->arg + i == p)	    // digits only
	    i = atol((نویسه *)eap->arg);
	وگرنه
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    اگر (i < 0)
		بازگشت;		// error message already given
	}
	buf = buflist_findnr(i);
	اگر (buf == پوچ)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    بازگشت;
	}
	اگر (buf == curbuf)
	    بازگشت;		// nothing to do
	idx_other = diff_buf_idx(buf);
	اگر (idx_other == DB_شمار)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    بازگشت;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    اگر (eap->addr_شمار == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	صحیح linestatus = 0;
	اگر (eap->line1 == curbuf->b_ml.ml_سطر_شمار
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	وگرنه اگر (eap->line1 > 0)
	    --eap->line1;
    }

    اگر (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    وگرنه
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	اگر (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    برو theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    اگر (!curbuf->b_changed)
    {
	change_warning(0);
	اگر (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    برو theend;
	}
    }

    dprev = پوچ;
    برای (dp = curtab->tp_first_diff; dp != پوچ; )
    {
	اگر (!eap->addr_شمار)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    وقتی (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_شمار[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	اگر (dp->df_lnum[idx_cur] > eap->line2 + off)
	    بشکن;	// past the range that was specified

	dfree = پوچ;
	lnum = dp->df_lnum[idx_to];
	شمار = dp->df_شمار[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + شمار + (شمار == 0 && BUFEMPTY() ? 1 : 0);

	اگر (dp->df_lnum[idx_cur] + dp->df_شمار[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    اگر (eap->addr_شمار > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		اگر (start_skip > 0)
		{
		    // range starts below start of current diff block
		    اگر (start_skip > شمار)
		    {
			lnum += شمار;
			شمار = 0;
		    }
		    وگرنه
		    {
			شمار -= start_skip;
			lnum += start_skip;
		    }
		}
		وگرنه
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_شمار[idx_cur] - 1
							 - (eap->line2 + off);
		اگر (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    اگر (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_شمار[idx_cur] - start_skip - end_skip;
			اگر (شمار > i)
			    شمار = i;
		    }
		    وگرنه			// :diffget
		    {
			شمار -= end_skip;
			end_skip = dp->df_شمار[idx_from] - start_skip - شمار;
			اگر (end_skip < 0)
			    end_skip = 0;
		    }
		}
		وگرنه
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    برای (i = 0; i < شمار; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_سطر_شمار == 1;
		اگر (ml_delete(lnum) == OK)
		    --added;
	    }
	    برای (i = 0; i < dp->df_شمار[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		اگر (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_سطر_شمار)
		    بشکن;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		اگر (p != پوچ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    اگر (buf_empty && curbuf->b_ml.ml_سطر_شمار == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_شمار = dp->df_شمار[idx_to] + added;
	    dp->df_شمار[idx_to] = new_شمار;

	    اگر (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		برای (i = 0; i < DB_شمار; ++i)
		    اگر (curtab->tp_diffbuf[i] != پوچ && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			بشکن;
		اگر (i == DB_شمار)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    اگر (dprev == پوچ)
			curtab->tp_first_diff = dp;
		    وگرنه
			dprev->df_next = dp;
		}
	    }

	    اگر (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + شمار - 1, (long)MAXLNUM, (long)added);
		اگر (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    اگر (curwin->w_cursor.lnum >= lnum + شمار)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_سطر_شمار);
		    }
		    وگرنه اگر (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_سطرها(lnum, 0, lnum + شمار, (long)added);

	    اگر (dfree != پوچ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    اگر (added != 0 && !valid_diff(dp))
		بشکن;

	    اگر (dfree == پوچ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_شمار[idx_to] = new_شمار;

	    // When changing the current buffer, keep track of line numbers
	    اگر (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	اگر (dfree == پوچ)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    اگر (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	اگر (کلید_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    اگر (diff_need_update)
	ex_diffupdate(پوچ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_سطر_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    اگر (curtab->tp_first_diff == پوچ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    اگر (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    اگر (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    وگرنه
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, پوچ, پوچ, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ایستا تهی
diff_fold_update(diff_T *dp, صحیح skip_idx)
{
    صحیح		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	برای (i = 0; i < DB_شمار; ++i)
	    اگر (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_شمار[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    صحیح
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	اگر (diff_buf_idx_tp(buf, tp) != DB_شمار)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    صحیح
diff_move_to(صحیح dir, long شمار)
{
    صحیح		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    اگر (idx == DB_شمار || curtab->tp_first_diff == پوچ)
	بازگشت FAIL;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    اگر (curtab->tp_first_diff == پوچ)		// no diffs today
	بازگشت FAIL;

    وقتی (--شمار >= 0)
    {
	// Check if already before first diff.
	اگر (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    بشکن;

	برای (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    اگر (dp == پوچ)
		بشکن;
	    اگر ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == پوچ
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		بشکن;
	    }
	}
    }

    // don't end up past the end of the file
    اگر (lnum > curbuf->b_ml.ml_سطر_شمار)
	lnum = curbuf->b_ml.ml_سطر_شمار;

    // When the cursor didn't move at all we fail.
    اگر (lnum == curwin->w_cursor.lnum)
	بازگشت FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    بازگشت OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ایستا linenr_T
diff_get_corresponding_سطر_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    صحیح		idx1;
    صحیح		idx2;
    diff_T	*dp;
    صحیح		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    اگر (idx1 == DB_شمار || idx2 == DB_شمار || curtab->tp_first_diff == پوچ)
	بازگشت lnum1;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    اگر (curtab->tp_first_diff == پوچ)		// no diffs today
	بازگشت lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	اگر (dp->df_lnum[idx1] > lnum1)
	    بازگشت lnum1 - baseline;
	اگر ((dp->df_lnum[idx1] + dp->df_شمار[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    اگر (baseline > dp->df_شمار[idx2])
		baseline = dp->df_شمار[idx2];

	    بازگشت dp->df_lnum[idx2] + baseline;
	}
	اگر (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_شمار[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_شمار[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    بازگشت curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_شمار[idx1])
				   - (dp->df_lnum[idx2] + dp->df_شمار[idx2]);
    }

    // If we get here then the cursor is after the last diff
    بازگشت lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_سطر(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_سطر_int(buf1, lnum1);

    // don't end up past the end of the file
    اگر (lnum > curbuf->b_ml.ml_سطر_شمار)
	بازگشت curbuf->b_ml.ml_سطر_شمار;
    بازگشت lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    صحیح		idx;
    صحیح		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    اگر (idx == DB_شمار)		// safety check
	بازگشت (linenr_T)0;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(پوچ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[idx] + dp->df_شمار[idx])
	    بشکن;

    // When after the last change, compute relative to the last line number.
    اگر (dp == پوچ)
	بازگشت wp->w_buffer->b_ml.ml_سطر_شمار
					- (curbuf->b_ml.ml_سطر_شمار - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    اگر (i == DB_شمار)			// safety check
	بازگشت (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    اگر (n > dp->df_lnum[i] + dp->df_شمار[i])
	n = dp->df_lnum[i] + dp->df_شمار[i];
    بازگشت n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ایستا صحیح
parse_diff_ed(
	char_u	    *سطر,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    صحیح	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = سطر;
    f1 = getdigits(&p);
    اگر (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    وگرنه
	l1 = f1;
    اگر (*p != 'a' && *p != 'c' && *p != 'd')
	بازگشت FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    اگر (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    وگرنه
	l2 = f2;
    اگر (l1 < f1 || l2 < f2)
	بازگشت FAIL;

    اگر (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->شمار_orig = 0;
    }
    وگرنه
    {
	hunk->lnum_orig = f1;
	hunk->شمار_orig = l1 - f1 + 1;
    }
    اگر (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->شمار_new = 0;
    }
    وگرنه
    {
	hunk->lnum_new = f2;
	hunk->شمار_new = l2 - f2 + 1;
    }
    بازگشت OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ایستا صحیح
parse_diff_unified(
	char_u	    *سطر,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = سطر;
    اگر (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	اگر (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	وگرنه
	    oldcount = 1;
	اگر (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    اگر (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    وگرنه
		newcount = 1;
	}
	وگرنه
	    بازگشت FAIL;	// invalid diff format

	اگر (oldcount == 0)
	    oldline += 1;
	اگر (newcount == 0)
	    newline += 1;
	اگر (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->شمار_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->شمار_new = newcount;

	بازگشت OK;
    }

    بازگشت FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ایستا صحیح
xdiff_out_indices(
	long start_a,
	long شمار_a,
	long start_b,
	long شمار_b,
	تهی *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    اگر (p == پوچ)
	بازگشت -1;

    اگر (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	بازگشت -1;
    }

    p->lnum_orig  = start_a + 1;
    p->شمار_orig = شمار_a;
    p->lnum_new   = start_b + 1;
    p->شمار_new  = شمار_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    بازگشت 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ایستا صحیح
xdiff_out_unified(
	تهی *priv,
	mmbuffer_t *mb,
	صحیح nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    صحیح		i;

    برای (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    بازگشت 0;
}

#endif	// FEAT_DIFF

#اگر defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    تهی
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    تهی
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ایستا linenr_T	prev_lnum = 0;
    ایستا varnumber_T	changedtick = 0;
    ایستا صحیح		fnum = 0;
    ایستا صحیح		prev_diff_flags = 0;
    ایستا صحیح		change_start = 0;
    ایستا صحیح		change_end = 0;
    ایستا hlf_T	hlID = (hlf_T)0;
    صحیح			cache_results = TRUE;
    صحیح			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    اگر (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    اگر (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    اگر (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	صحیح linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	اگر (linestatus < 0)
	{
	    اگر (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		اگر (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		وگرنه
		{
		    hlID = HLF_CHD;	// changed line
		    اگر (diffline.رقم_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    وگرنه
		hlID = HLF_ADD;	// added line
	}
	وگرنه
	    hlID = (hlf_T)0;

	اگر (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    اگر (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_عدد(&argvars[1]) - 1; // ignore type error in {col}
	اگر (cache_results)
	{
	    اگر (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    وگرنه
		hlID = HLF_CHD;			// changed line
	}
	وگرنه
	{
	    hlID = HLF_CHD;
	    برای (صحیح i = 0; i < diffline.رقم_changes; i++)
	    {
		صحیح added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		اگر (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    بشکن;
		}
		اگر (col < change_start)
		    // the remaining changes are past this column and not relevant
		    بشکن;
	    }
	}
    }
    rettv->vval.v_عدد = hlID == (hlf_T)0 ? 0 : (صحیح)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ایستا صحیح
parse_diff_optarg(
    typval_T	    *opts,
    صحیح		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    صحیح		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    اگر (algo != پوچ)
    {
	اگر (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	وگرنه اگر (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	وگرنه اگر (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	وگرنه اگر (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    اگر (output_fmt != پوچ)
    {
	اگر (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	وگرنه اگر (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	وگرنه
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    بازگشت FAIL;
	}
    }

    *diff_ctxlen = dict_get_عدد_def(d, "context", 0);
    اگر (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    اگر (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    اگر (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    اگر (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    اگر (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    اگر (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    اگر (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    بازگشت OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ایستا تهی
list_to_diffin(list_T *l, diffin_T *din, صحیح icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_اقلام(l, li)
    {
	str = tv_get_string(&li->li_tv);
	اگر (icase)
	{
	    str = strlow_save(str);
	    اگر (str == پوچ)
		ادامه;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	اگر (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (نویسه *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ایستا dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    اگر (hunk_dict == پوچ)
	بازگشت پوچ;

    dict_add_عدد(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_عدد(hunk_dict, "from_count", hunk->شمار_orig);
    dict_add_عدد(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_عدد(hunk_dict, "to_count", hunk->شمار_new);

    بازگشت hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    تهی
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    اگر (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	بازگشت;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, اندازه(نویسه *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    صحیح		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
	اگر (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    بازگشت;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    صحیح	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    اگر (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    صحیح diff_status = diff_file(&dio);

    // restore 'diffexpr'
    اگر (restore_diffexpr)
	*p_dex = cc;

    اگر (diff_status == FAIL)
	برو done;

    صحیح		hunk_idx = 0;
    dict_T	*hunk_dict;

    اگر (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	اگر (rettv_list_alloc(rettv) != OK)
	    برو done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = پوچ;
	وقتی (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    اگر (hunk_dict == پوچ)
		برو done;

	    اگر (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    وگرنه
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    اگر (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    وگرنه
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
