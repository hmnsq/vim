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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "xdiff/xdiff.h"

#எனில் defined(FEAT_DIFF)

நிலையான முழு diff_busy = FALSE;	    // using diff structs, don't change them
நிலையான முழு diff_need_update = FALSE; // ex_diffupdate needs to be called

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
நிலையான முழு	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

நிலையான long diff_algorithm = XDF_INDENT_HEURISTIC;
நிலையான முழு diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

நிலையான முழு diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# எனில் defined(MSWIN)
நிலையான முழு diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
வகைவரை கட்டமைப்பு {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
வகைவரை கட்டமைப்பு {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
வகைவரை கட்டமைப்பு {
    linenr_T lnum_orig;
    long     மொத்தம்_orig;
    linenr_T lnum_new;
    long     மொத்தம்_new;
} diffhunk_T;

வகைவரை எண்ணிக்கை {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
வகைவரை கட்டமைப்பு {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    முழு		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    முழு		    dio_ctxlen;   // unified diff context length
} diffio_T;

நிலையான முழு diff_buf_idx(buf_T *buf);
நிலையான முழு diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
நிலையான வெறுமை diff_mark_adjust_tp(tabpage_T *tp, முழு idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
நிலையான வெறுமை diff_check_unchanged(tabpage_T *tp, diff_T *dp);
நிலையான முழு diff_check_sanity(tabpage_T *tp, diff_T *dp);
நிலையான முழு check_external_diff(diffio_T *diffio);
நிலையான முழு diff_file(diffio_T *diffio);
நிலையான முழு diff_equal_entry(diff_T *dp, முழு idx1, முழு idx2);
நிலையான முழு diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
நிலையான வெறுமை diff_fold_update(diff_T *dp, முழு skip_idx);
# endif
நிலையான வெறுமை diff_read(முழு idx_orig, முழு idx_new, diffio_T *dio);
நிலையான வெறுமை diff_copy_entry(diff_T *dprev, diff_T *dp, முழு idx_orig, முழு idx_new);
நிலையான diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
நிலையான முழு parse_diff_ed(char_u *வரி, diffhunk_T *hunk);
நிலையான முழு parse_diff_unified(char_u *வரி, diffhunk_T *hunk);
நிலையான முழு xdiff_out_indices(long start_a, long மொத்தம்_a, long start_b, long மொத்தம்_b, வெறுமை *priv);
நிலையான முழு xdiff_out_unified(வெறுமை *priv, mmbuffer_t *mb, முழு nbuf);
நிலையான முழு parse_diffanchors(முழு check_only, buf_T *buf, linenr_T *anchors, முழு *இலக்கம்_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    க்கு ((dp) = (tp)->tp_first_diff; (dp) != வெற்று; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
நிலையான diff_T	    *diff_finger_dp = வெற்று;
நிலையான tabpage_T    *diff_finger_tp = வெற்று;
நிலையான முழு	    diff_finger_idx = -1;
நிலையான linenr_T	    diff_finger_lnum = 0;

    நிலையான வெறுமை
clear_diffblock(diff_T *dp)
{
    எனில் (dp == diff_finger_dp)
	diff_finger_dp = வெற்று;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    நிலையான diff_T *
diff_scan_start(tabpage_T *tp, முழு idx, linenr_T lnum)
{
    எனில் (diff_finger_dp != வெற்று
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	திரும்பு diff_finger_dp;
    திரும்பு tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    நிலையான வெறுமை
diff_scan_remember(tabpage_T *tp, முழு idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != வெற்று ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    வெறுமை
diff_buf_delete(buf_T *buf)
{
    முழு		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	எனில் (i != DB_மொத்தம்)
	{
	    tp->tp_diffbuf[i] = வெற்று;
	    tp->tp_diff_invalid = TRUE;
	    எனில் (tp == curtab)
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
    வெறுமை
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    முழு		i;

    எனில் (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		நிறுத்து;
	எனில் (wp == வெற்று)
	{
	    i = diff_buf_idx(win->w_buffer);
	    எனில் (i != DB_மொத்தம்)
	    {
		curtab->tp_diffbuf[i] = வெற்று;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    இல்லையெனில்
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
    வெறுமை
diff_buf_add(buf_T *buf)
{
    முழு		i;

    எனில் (diff_buf_idx(buf) != DB_மொத்தம்)
	திரும்பு;		// It's already there.

    க்கு (i = 0; i < DB_மொத்தம்; ++i)
	எனில் (curtab->tp_diffbuf[i] == வெற்று)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    திரும்பு;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_மொத்தம்);
}

/*
 * Remove all buffers to make diffs for.
 */
    நிலையான வெறுமை
diff_buf_clear(வெறுமை)
{
    முழு		i;

    க்கு (i = 0; i < DB_மொத்தம்; ++i)
	எனில் (curtab->tp_diffbuf[i] != வெற்று)
	{
	    curtab->tp_diffbuf[i] = வெற்று;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    நிலையான முழு
diff_buf_idx(buf_T *buf)
{
    முழு		idx;

    க்கு (idx = 0; idx < DB_மொத்தம்; ++idx)
	எனில் (curtab->tp_diffbuf[idx] == buf)
	    நிறுத்து;
    திரும்பு idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    நிலையான முழு
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    முழு		idx;

    க்கு (idx = 0; idx < DB_மொத்தம்; ++idx)
	எனில் (tp->tp_diffbuf[idx] == buf)
	    நிறுத்து;
    திரும்பு idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    வெறுமை
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    முழு		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	எனில் (i != DB_மொத்தம்)
	{
	    tp->tp_diff_invalid = TRUE;
	    எனில் (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    வெறுமை
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    முழு		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	எனில் (idx != DB_மொத்தம்)
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
    நிலையான வெறுமை
diff_mark_adjust_tp(
    tabpage_T	*tp,
    முழு		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    முழு		i;
    முழு		inserted, deleted;
    முழு		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    முழு		check_unchanged;

    diff_finger_dp = வெற்று;		// may shift df_lnum, invalidate the finger

    எனில் (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    எனில் (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    இல்லையெனில் எனில் (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    இல்லையெனில்
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = வெற்று;
    dp = tp->tp_first_diff;
    க்கு (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	எனில் ((dp == வெற்று || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == வெற்று
		    || dprev->df_lnum[idx] + dprev->df_மொத்தம்[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    எனில் (dnext == வெற்று)
		திரும்பு;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_மொத்தம்[idx] = inserted;
	    க்கு (i = 0; i < DB_மொத்தம்; ++i)
		எனில் (tp->tp_diffbuf[i] != வெற்று && i != idx)
		{
		    எனில் (dprev == வெற்று)
			dnext->df_lnum[i] = line1;
		    இல்லையெனில்
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_மொத்தம்[i])
			    - (dprev->df_lnum[idx] + dprev->df_மொத்தம்[idx]);
		    dnext->df_மொத்தம்[i] = deleted;
		}
	}

	// if at end of the list, quit
	எனில் (dp == வெற்று)
	    நிறுத்து;

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
	last = dp->df_lnum[idx] + dp->df_மொத்தம்[idx] - 1;

	// 1. change completely above line1: nothing to do
	எனில் (last >= line1 - 1)
	{
	    எனில் (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		எனில் (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		தொடர்;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    எனில் (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		எனில் (amount_after == 0)
		    நிறுத்து;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    இல்லையெனில்
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		எனில் (deleted > 0)
		{
		    off = 0;
		    எனில் (dp->df_lnum[idx] >= line1)
		    {
			எனில் (last <= line2)
			{
			    // 4. delete all lines of diff
			    எனில் (dp->df_next != வெற்று
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_மொத்தம்[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    இல்லையெனில்
				n = deleted - dp->df_மொத்தம்[idx];
			    dp->df_மொத்தம்[idx] = 0;
			}
			இல்லையெனில்
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_மொத்தம்[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    இல்லையெனில்
		    {
			எனில் (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_மொத்தம்[idx] -= last - lnum_deleted + 1;
			    எனில் (dp->df_next != வெற்று
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    இல்லையெனில்
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			இல்லையெனில்
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_மொத்தம்[idx] -= deleted;
			}
		    }

		    க்கு (i = 0; i < DB_மொத்தம்; ++i)
			எனில் (tp->tp_diffbuf[i] != வெற்று && i != idx)
			{
			    எனில் (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    இல்லையெனில்
				dp->df_lnum[i] = 1;
			    dp->df_மொத்தம்[i] += n;
			}
		}
		இல்லையெனில்
		{
		    எனில் (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_மொத்தம்[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    இல்லையெனில்
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		எனில் (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	எனில் (dprev != வெற்று && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_மொத்தம்[idx]
							== dp->df_lnum[idx])
	{
	    க்கு (i = 0; i < DB_மொத்தம்; ++i)
		எனில் (tp->tp_diffbuf[i] != வெற்று)
		    dprev->df_மொத்தம்[i] += dp->df_மொத்தம்[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	இல்லையெனில்
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = வெற்று;
    dp = tp->tp_first_diff;
    போது (dp != வெற்று)
    {
	// All counts are zero, remove this entry.
	க்கு (i = 0; i < DB_மொத்தம்; ++i)
	    எனில் (tp->tp_diffbuf[i] != வெற்று && dp->df_மொத்தம்[i] != 0)
		நிறுத்து;
	எனில் (i == DB_மொத்தம்)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    எனில் (dprev == வெற்று)
		tp->tp_first_diff = dnext;
	    இல்லையெனில்
		dprev->df_next = dnext;
	}
	இல்லையெனில்
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    எனில் (tp == curtab)
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
    நிலையான diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    எனில் (dnew == வெற்று)
	திரும்பு வெற்று;
    diff_finger_dp = வெற்று;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    எனில் (dprev == வெற்று)
	tp->tp_first_diff = dnew;
    இல்லையெனில்
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, அளவு(diffline_change_T), 20);
    திரும்பு dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    நிலையான வெறுமை
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    முழு		i_org;
    முழு		i_new;
    முழு		off_org, off_new;
    char_u	*வரி_org;
    முழு		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    க்கு (i_org = 0; i_org < DB_மொத்தம்; ++i_org)
	எனில் (tp->tp_diffbuf[i_org] != வெற்று)
	    நிறுத்து;
    எனில் (i_org == DB_மொத்தம்)	// safety check
	திரும்பு;

    எனில் (diff_check_sanity(tp, dp) == FAIL)
	திரும்பு;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    க்கு (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	போது (dp->df_மொத்தம்[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    எனில் (dir == BACKWARD)
		off_org = dp->df_மொத்தம்[i_org] - 1;
	    வரி_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    எனில் (வரி_org == வெற்று)
		திரும்பு;
	    க்கு (i_new = i_org + 1; i_new < DB_மொத்தம்; ++i_new)
	    {
		எனில் (tp->tp_diffbuf[i_new] == வெற்று)
		    தொடர்;
		எனில் (dir == BACKWARD)
		    off_new = dp->df_மொத்தம்[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		எனில் (off_new < 0 || off_new >= dp->df_மொத்தம்[i_new])
		    நிறுத்து;
		எனில் (diff_cmp(வரி_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    நிறுத்து;
	    }
	    vim_free(வரி_org);

	    // Stop when a line isn't equal in all diff buffers.
	    எனில் (i_new != DB_மொத்தம்)
		நிறுத்து;

	    // Line matched in all buffers, remove it from the diff.
	    க்கு (i_new = i_org; i_new < DB_மொத்தம்; ++i_new)
		எனில் (tp->tp_diffbuf[i_new] != வெற்று)
		{
		    எனில் (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_மொத்தம்[i_new];
		}
	}
	எனில் (dir == BACKWARD)
	    நிறுத்து;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    நிலையான முழு
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    முழு		i;

    க்கு (i = 0; i < DB_மொத்தம்; ++i)
	எனில் (tp->tp_diffbuf[i] != வெற்று)
	    எனில் (dp->df_lnum[i] + dp->df_மொத்தம்[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_வரி_மொத்தம்)
		திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    வெறுமை
diff_redraw(
    முழு		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = வெற்று;
    முழு		used_max_fill_other = FALSE;
    முழு		used_max_fill_curwin = FALSE;
    முழு		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	எனில் (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    தொடர்;

	redraw_win_later(wp, UPD_SOME_VALID);
	எனில் (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	எனில் (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	எனில் ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    எனில் (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    இல்லையெனில் எனில் (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		எனில் (wp == curwin)
		    used_max_fill_curwin = TRUE;
		இல்லையெனில் எனில் (wp_other != வெற்று)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    எனில் (wp_other != வெற்று && curwin->w_p_scb)
    {
	எனில் (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	இல்லையெனில் எனில் (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    நிலையான வெறுமை
clear_diffin(diffin_T *din)
{
    எனில் (din->din_fname == வெற்று)
	VIM_CLEAR(din->din_mmfile.ptr);
    இல்லையெனில்
	mch_remove(din->din_fname);
}

    நிலையான வெறுமை
clear_diffout(diffout_T *dout)
{
    எனில் (dout->dout_fname == வெற்று)
	ga_clear_strings(&dout->dout_ga);
    இல்லையெனில்
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    நிலையான முழு
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    எனில் (end < 0)
	end = buf->b_ml.ml_வரி_மொத்தம்;

    எனில் (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = வெற்று;
	din->din_mmfile.size = 0;
	திரும்பு OK;
    }

    // xdiff requires one big block of memory with all the text.
    க்கு (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    எனில் (ptr == வெற்று)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	திரும்பு FAIL;
    }
    din->din_mmfile.ptr = (எழுத்து *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    க்கு (lnum = start; lnum <= end; ++lnum)
    {
	க்கு (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    எனில் (diff_flags & DIFF_ICASE)
	    {
		முழு c;
		முழு	orig_len;
		முழு	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		எனில் (*s == NL)
		    c = NUL;
		இல்லையெனில்
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		எனில் (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		இல்லையெனில்
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    எனில் (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    இல்லையெனில்
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    திரும்பு OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    நிலையான முழு
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    முழு		r;
    முழு		save_ml_flags;
    char_u	*save_ff;
    முழு		save_cmod_flags;

    எனில் (din->din_fname == வெற்று)
	திரும்பு diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    எனில் (frames_locked())
	திரும்பு FAIL;

    எனில் (end < 0)
	end = buf->b_ml.ml_வரி_மொத்தம்;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    எனில் (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, வெற்று,
			start, end,
			வெற்று, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    திரும்பு r;
}

    நிலையான முழு
lnum_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    எனில் (lnum1 < lnum2)
	திரும்பு -1;
    எனில் (lnum1 > lnum2)
	திரும்பு 1;
    திரும்பு 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    நிலையான வெறுமை
diff_try_update(
	diffio_T    *dio,
	முழு	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    முழு		idx_new;

    எனில் (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, அளவு(எழுத்து *), 1000);
    }
    இல்லையெனில்
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	எனில் (dio->dio_orig.din_fname == வெற்று
		|| dio->dio_new.din_fname == வெற்று
		|| dio->dio_diff.dout_fname == வெற்று)
	    செல் theend;
    }

    // Check external diff is actually working.
    எனில் (!dio->dio_internal && check_external_diff(dio) == FAIL)
	செல் theend;

    // :diffupdate!
    எனில் (eap != வெற்று && eap->forceit)
	க்கு (idx_new = idx_orig; idx_new < DB_மொத்தம்; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    எனில் (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    முழு இலக்கம்_anchors = INT_MAX;
    linenr_T anchors[DB_மொத்தம்][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    எனில் (diff_flags & DIFF_ANCHOR)
    {
	க்கு (முழு idx = 0; idx < DB_மொத்தம்; idx++)
	{
	    எனில் (curtab->tp_diffbuf[idx] == வெற்று)
		தொடர்;
	    முழு buf_இலக்கம்_anchors = 0;
	    எனில் (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_இலக்கம்_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		இலக்கம்_anchors = 0;
		CLEAR_FIELD(anchors);
		நிறுத்து;
	    }
	    எனில் (buf_இலக்கம்_anchors < இலக்கம்_anchors)
		இலக்கம்_anchors = buf_இலக்கம்_anchors;

	    எனில் (buf_இலக்கம்_anchors > 0)
		qsort((வெறுமை *)anchors[idx],
			(size_t)buf_இலக்கம்_anchors,
			அளவு(linenr_T),
			lnum_compare);
	}
    }
    எனில் (இலக்கம்_anchors == INT_MAX)
	இலக்கம்_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    க்கு (முழு anchor_i = 0; anchor_i <= இலக்கம்_anchors; anchor_i++)
    {
	diff_T	*orig_diff = வெற்று;
	எனில் (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = வெற்று;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == இலக்கம்_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	எனில் (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    எனில் (orig_diff != வெற்று)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    செல் theend;
	}

	// Make a difference between the first buffer and every other.
	க்கு (idx_new = idx_orig + 1; idx_new < DB_மொத்தம்; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
		தொடர்; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == இலக்கம்_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    எனில் (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		தொடர்;
	    எனில் (diff_file(dio) == FAIL)
		தொடர்;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	எனில் (anchor_i != 0)
	{
	    diff_finger_dp = வெற்று;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    க்கு (diff_T *dp = curtab->tp_first_diff; dp != வெற்று; dp = dp->df_next)
	    {
		க்கு (முழு idx = 0; idx < DB_மொத்தம்; idx++)
		{
		    எனில் (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    எனில் (orig_diff != வெற்று)
	    {
		diff_T *last_diff = orig_diff;
		போது (last_diff->df_next != வெற்று)
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
    முழு
diff_internal(வெறுமை)
{
    திரும்பு (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    நிலையான முழு
diff_internal_failed(வெறுமை)
{
    முழு idx;

    // Only need to do something when there is another buffer.
    க்கு (idx = 0; idx < DB_மொத்தம்; ++idx)
	எனில் (curtab->tp_diffbuf[idx] != வெற்று
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    வெறுமை
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    முழு		idx_orig;
    முழு		idx_new;
    diffio_T	diffio;
    முழு		had_diffs = curtab->tp_first_diff != வெற்று;

    எனில் (diff_busy)
    {
	diff_need_update = TRUE;
	திரும்பு;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    க்கு (idx_orig = 0; idx_orig < DB_மொத்தம்; ++idx_orig)
	எனில் (curtab->tp_diffbuf[idx_orig] != வெற்று)
	    நிறுத்து;
    எனில் (idx_orig == DB_மொத்தம்)
	செல் theend;

    // Only need to do something when there is another buffer.
    க்கு (idx_new = idx_orig + 1; idx_new < DB_மொத்தம்; ++idx_new)
	எனில் (curtab->tp_diffbuf[idx_new] != வெற்று)
	    நிறுத்து;
    எனில் (idx_new == DB_மொத்தம்)
	செல் theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    எனில் (diffio.dio_internal && diff_internal_failed())
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
    எனில் (had_diffs || curtab->tp_first_diff != வெற்று)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, வெற்று, வெற்று, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    நிலையான முழு
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    முழு		ok;
    முழு		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    க்கு (;;)
    {
	ok = FALSE;
	fd = mch_fopen((எழுத்து *)diffio->dio_orig.din_fname, "w");
	எனில் (fd == வெற்று)
	    io_error = TRUE;
	இல்லையெனில்
	{
	    எனில் (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((எழுத்து *)diffio->dio_new.din_fname, "w");
	    எனில் (fd == வெற்று)
		io_error = TRUE;
	    இல்லையெனில்
	    {
		எனில் (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = வெற்று;
		எனில் (diff_file(diffio) == OK)
		    fd = mch_fopen((எழுத்து *)diffio->dio_diff.dout_fname, "r");
		எனில் (fd == வெற்று)
		    io_error = TRUE;
		இல்லையெனில்
		{
		    char_u	linebuf[LBUFLEN];

		    க்கு (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			எனில் (vim_fgets(linebuf, LBUFLEN, fd))
			    நிறுத்து;
			எனில் (STRNCMP(linebuf, "1c1", 3) == 0
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
	எனில் (*p_dex != NUL)
	    நிறுத்து;
# endif

# எனில் defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	எனில் (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    தொடர்;
	}
	எனில் (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	எனில் (diff_a_works != MAYBE)
	    நிறுத்து;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	எனில் (ok)
	    நிறுத்து;
    }
    எனில் (!ok)
    {
	எனில் (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# எனில் defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Invoke the xdiff function.
 */
    நிலையான முழு
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    எனில் (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    எனில் (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    எனில் (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    எனில் (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_வரிகள்;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    எனில் (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    இல்லையெனில்
	emit_cb.out_வரி = xdiff_out_unified;
    எனில் (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	திரும்பு FAIL;
    }
    எனில் (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    நிலையான முழு
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    எனில் (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	திரும்பு OK;
    }
    இல்லையெனில்
# endif
    // Use xdiff for generating the diff.
    எனில் (dio->dio_internal)
	திரும்பு diff_file_internal(dio);

    எனில் (check_restricted())
	திரும்பு FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    எனில் (cmd == வெற்று)
	திரும்பு FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    எனில் (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((எழுத்து *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# எனில் defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# இல்லையெனில்
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (முழு)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (வெறுமை)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    திரும்பு OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    வெறுமை
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = வெற்று;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = வெற்று;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = வெற்று;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = வெற்று;
    முழு		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = வெற்று;

# ifdef FEAT_BROWSE
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, வெற்று, வெற்று,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), வெற்று);
	எனில் (browseFile == வெற்று)
	    திரும்பு;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    எனில் (tmp_orig == வெற்று || tmp_new == வெற்று)
	செல் theend;

    // Write the current buffer to "tmp_orig".
    எனில் (buf_write(curbuf, tmp_orig, வெற்று,
		(linenr_T)1, curbuf->b_ml.ml_வரி_மொத்தம்,
				     வெற்று, FALSE, FALSE, FALSE, TRUE) == FAIL)
	செல் theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != வெற்று ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    எனில் (esc_name == வெற்று)
	செல் theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    எனில் (buf == வெற்று)
	செல் theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    எனில் (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((எழுத்து *)dirbuf) != 0)
	dirbuf[0] = NUL;
    இல்லையெனில்
    {
#  ifdef TEMPDIRNAMES
	எனில் (vim_tempdir != வெற்று)
	    vim_ignored = mch_chdir((எழுத்து *)vim_tempdir);
	இல்லையெனில்
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    எனில் (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != வெற்று ? fullname :
#  endif
		eap->arg, tmp_new);
    இல்லையெனில்
# endif
    {
	எனில் (check_restricted())
	    செல் theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((எழுத்து *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(வெறுமை)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    எனில் (dirbuf[0] != NUL)
    {
	எனில் (mch_chdir((எழுத்து *)dirbuf) != 0)
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
    எனில் (mch_stat((எழுத்து *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    இல்லையெனில்
    {
	எனில் (curbuf->b_fname != வெற்று)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    எனில் (newname != வெற்று)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	எனில் (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    எனில் (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		எனில் (newname != வெற்று)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    எனில் (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    எனில் (tmp_orig != வெற்று)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    எனில் (tmp_new != வெற்று)
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
    வெறுமை
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

    எனில் (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	திரும்பு;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    எனில் (curwin == old_curwin)		// split didn't work
	திரும்பு;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    எனில் (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	எனில் (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_வரி(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    வெறுமை
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    நிலையான வெறுமை
set_diff_option(win_T *wp, முழு மதிப்பு)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_மதிப்பு_give_err((char_u *)"diff", (long)மதிப்பு, வெற்று, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    வெறுமை
diff_win_options(
    win_T	*wp,
    முழு		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    எனில் (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    எனில் (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    எனில் (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	எனில் (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    எனில் (!wp->w_p_diff)
    {
	எனில் (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    எனில் (!wp->w_p_diff)
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
    எனில் (vim_strchr(p_sbo, 'h') == வெற்று)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    எனில் (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    வெறுமை
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    முழு		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	எனில் (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    எனில் (wp->w_p_diff_saved)
	    {

		எனில் (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		எனில் (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		எனில் (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    எனில் (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		எனில் (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		எனில் (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		எனில் (wp->w_p_fen)
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
    எனில் (eap->forceit)
	diff_buf_clear();

    எனில் (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    எனில் (!diffwin && vim_strchr(p_sbo, 'h') != வெற்று)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    நிலையான வெறுமை
diff_read(
    முழு		idx_orig,	// idx of original file
    முழு		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = வெற்று;
    முழு		வரி_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = வெற்று;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*வரி;
    long	off;
    முழு		i;
    முழு		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = வெற்று;	    // init to avoid gcc warning

    எண்ணிக்கை {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    எனில் (dout->dout_fname == வெற்று)
    {
	diffstyle = DIFF_UNIFIED;
    }
    இல்லையெனில்
    {
	fd = mch_fopen((எழுத்து *)dout->dout_fname, "r");
	எனில் (fd == வெற்று)
	{
	    emsg(_(e_cannot_read_diff_output));
	    திரும்பு;
	}
    }

    எனில் (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	எனில் (hunk == வெற்று)
	{
	    எனில் (fd != வெற்று)
		fclose(fd);
	    திரும்பு;
	}
    }

    க்கு (;;)
    {
	எனில் (dio->dio_internal)
	{
	    எனில் (வரி_hunk_idx >= dout->dout_ga.ga_len)
		நிறுத்து;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[வரி_hunk_idx++];
	}
	இல்லையெனில்
	{
	    எனில் (fd == வெற்று)
	    {
		எனில் (வரி_hunk_idx >= dout->dout_ga.ga_len)
		    நிறுத்து;	    // did last line
		வரி = ((char_u **)dout->dout_ga.ga_data)[வரி_hunk_idx++];
	    }
	    இல்லையெனில்
	    {
		எனில் (vim_fgets(linebuf, LBUFLEN, fd))
		    நிறுத்து;		// end of file
		வரி = linebuf;
	    }

	    எனில் (diffstyle == DIFF_NONE)
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
		எனில் (SAFE_isdigit(*வரி))
		    diffstyle = DIFF_ED;
		இல்லையெனில் எனில் ((STRNCMP(வரி, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		இல்லையெனில் எனில் ((STRNCMP(வரி, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(வரி, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(வரி, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		இல்லையெனில்
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    தொடர்;
	    }

	    எனில் (diffstyle == DIFF_ED)
	    {
		எனில் (!SAFE_isdigit(*வரி))
		    தொடர்;	// not the start of a diff block
		எனில் (parse_diff_ed(வரி, hunk) == FAIL)
		    தொடர்;
	    }
	    இல்லையெனில் எனில் (diffstyle == DIFF_UNIFIED)
	    {
		எனில் (STRNCMP(வரி, "@@ ", 3)  != 0)
		    தொடர்;	// not the start of a diff block
		எனில் (parse_diff_unified(வரி, hunk) == FAIL)
		    தொடர்;
	    }
	    இல்லையெனில்
	    {
		emsg(_(e_invalid_diff_format));
		நிறுத்து;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	போது (dp != வெற்று
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_மொத்தம்[idx_orig])
	{
	    எனில் (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	எனில் (dp != வெற்று
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_மொத்தம்[idx_orig]
		&& hunk->lnum_orig + hunk->மொத்தம்_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    க்கு (dpl = dp; dpl->df_next != வெற்று; dpl = dpl->df_next)
		எனில் (hunk->lnum_orig + hunk->மொத்தம்_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    நிறுத்து;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    எனில் (off > 0)
	    {
		க்கு (i = idx_orig; i < idx_new; ++i)
		    எனில் (curtab->tp_diffbuf[i] != வெற்று)
		    {
			dp->df_lnum[i] -= off;
			dp->df_மொத்தம்[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_மொத்தம்[idx_new] = hunk->மொத்தம்_new;
	    }
	    இல்லையெனில் எனில் (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_மொத்தம்[idx_new] = hunk->மொத்தம்_new - off;
	    }
	    இல்லையெனில்
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		முழு orig_size_in_dp = MIN(hunk->மொத்தம்_orig,
			dp->df_lnum[idx_orig] +
			dp->df_மொத்தம்[idx_orig] - hunk->lnum_orig);
		முழு size_diff = hunk->மொத்தம்_new - orig_size_in_dp;
		dp->df_மொத்தம்[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->மொத்தம்_new
		    - (dp->df_lnum[idx_new] + dp->df_மொத்தம்[idx_new]);
		எனில் (off > 0)
		    dp->df_மொத்தம்[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->மொத்தம்_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_மொத்தம்[idx_orig]);
	    எனில் (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		எனில் (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_மொத்தம்[idx_new] += -off;
		}
		off = 0;
	    }
	    க்கு (i = idx_orig; i < idx_new; ++i)
		எனில் (curtab->tp_diffbuf[i] != வெற்று)
		    dp->df_மொத்தம்[i] = dpl->df_lnum[i] + dpl->df_மொத்தம்[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    போது (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	இல்லையெனில்
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    எனில் (dp == வெற்று)
		செல் done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_மொத்தம்[idx_orig] = hunk->மொத்தம்_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_மொத்தம்[idx_new] = hunk->மொத்தம்_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    க்கு (i = idx_orig + 1; i < idx_new; ++i)
		எனில் (curtab->tp_diffbuf[i] != வெற்று)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    போது (dp != வெற்று)
    {
	எனில் (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    எனில் (!dio->dio_internal)
	vim_free(hunk);

    எனில் (fd != வெற்று)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    நிலையான வெறுமை
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    முழு		idx_orig,
    முழு		idx_new)
{
    long	off;

    எனில் (dprev == வெற்று)
	off = 0;
    இல்லையெனில்
	off = (dprev->df_lnum[idx_orig] + dprev->df_மொத்தம்[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_மொத்தம்[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_மொத்தம்[idx_new] = dp->df_மொத்தம்[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    வெறுமை
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    க்கு (p = tp->tp_first_diff; p != வெற்று; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = வெற்று;
}

/*
 *  return true if the options are set to use diff linematch
 */
    நிலையான முழு
diff_linematch(diff_T *dp)
{
    எனில் (!(diff_flags & DIFF_LINEMATCH))
	திரும்பு 0;

    // are there more than three diff buffers?
    முழு tsize = 0;
    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
    {
	எனில் (curtab->tp_diffbuf[i] != வெற்று)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    எனில் (dp->df_மொத்தம்[i] < 0)
		திரும்பு FALSE;
	    tsize += dp->df_மொத்தம்[i];
	}
    }

    // avoid allocating a huge array because it will lag
    திரும்பு tsize <= linematch_வரிகள்;
}

    நிலையான முழு
get_max_diff_length(மாறிலி diff_T *dp)
{
    முழு maxlength = 0;

    க்கு (முழு k = 0; k < DB_மொத்தம்; k++)
    {
	எனில் (curtab->tp_diffbuf[k] != வெற்று)
	{
	    எனில் (dp->df_மொத்தம்[k] > maxlength)
		maxlength = dp->df_மொத்தம்[k];
	}
    }
    திரும்பு maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    நிலையான வெறுமை
find_உச்சி_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    முழு		fromidx,
    முழு		topline)
{
    diff_T	*topdiff = வெற்று;
    diff_T	*localtopdiff = வெற்று;
    முழு		topdiffchange = 0;

    க்கு (topdiff = curtab->tp_first_diff; topdiff != வெற்று;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	எனில் (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	எனில் (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_மொத்தம்[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    எனில் ((*thistopdiff) == வெற்று)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	எனில் (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_மொத்தம்[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    எனில் (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		நிறுத்து;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    நிலையான வெறுமை
calculate_topfill_and_topline(
    மாறிலி முழு	fromidx,
    மாறிலி முழு	toidx,
    மாறிலி முழு	from_topline,
    மாறிலி முழு	from_topfill,
    முழு		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = வெற்று;
    diff_T	*next_adjacent_blocks = வெற்று;
    முழு		virtual_வரிகள்_passed = 0;
    முழு		curlinenum_to = 1;

    find_உச்சி_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    போது (curdif && (curdif->df_lnum[fromidx] + curdif->df_மொத்தம்[fromidx])
							<= from_topline)
    {
	virtual_வரிகள்_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    எனில் (curdif != next_adjacent_blocks)
	virtual_வரிகள்_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_வரிகள்_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    எனில் (virtual_வரிகள்_passed < 0)
	virtual_வரிகள்_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    எனில் (thistopdiff != வெற்று) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    முழு virt_வரிகள்_இடது = virtual_வரிகள்_passed;
    curdif = thistopdiff;
    போது (virt_வரிகள்_இடது > 0 && curdif != வெற்று && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_வரிகள்_இடது, curdif->df_மொத்தம்[toidx]);
	virt_வரிகள்_இடது -= MIN(virt_வரிகள்_இடது, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    முழு max_virt_வரிகள் = 0;
    க்கு (diff_T *dp = thistopdiff; dp != வெற்று; dp = dp->df_next)
    {
	எனில் (dp->df_lnum[toidx] + dp->df_மொத்தம்[toidx] <= curlinenum_to)
	    max_virt_வரிகள் += get_max_diff_length(dp);
	இல்லையெனில்
	{
	    எனில் (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_வரிகள் += curlinenum_to - dp->df_lnum[toidx];
	    நிறுத்து;
	}
    }

    எனில் (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_வரிகள் - virtual_வரிகள்_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    நிலையான வெறுமை
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    மாறிலி முழு	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    முழு		வரி_numbers[DB_மொத்தம்];
    முழு		outputmap[DB_மொத்தம்];
    size_t	ndiffs = 0;

    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
    {
	எனில் (curtab->tp_diffbuf[i] != வெற்று)
	{
	    வரி_numbers[i] = dp->df_lnum[i];
	    dp->df_மொத்தம்[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    க்கு (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	எனில் (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    க்கு (முழு j = 0; j < DB_மொத்தம்; j++)
	    {
		எனில் (curtab->tp_diffbuf[j] != வெற்று)
		{
		    dp_s->df_lnum[j] = வரி_numbers[j];
		    dp_s->df_மொத்தம்[j] = 0;
		}
	    }
	}
	க்கு (size_t j = 0; j < ndiffs; j++)
	{
	    எனில் (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_மொத்தம்[outputmap[j]]++;
		வரி_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    நிலையான வெறுமை
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_மொத்தம்];
    மாறிலி mmfile_t	*diffbufs[DB_மொத்தம்];
    முழு			diff_length[DB_மொத்தம்];
    size_t		ndiffs = 0;

    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
    {
	எனில் (curtab->tp_diffbuf[i] != வெற்று)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    எனில் (dp->df_மொத்தம்[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_மொத்தம்[i] - 1);
	    }
	    இல்லையெனில்
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = வெற்று;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_மொத்தம்[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    முழு *decisions = வெற்று;
    மாறிலி முழு iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    க்கு (size_t i = 0; i < ndiffs; i++)
	விடுவி(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    விடுவி(decisions);
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
    முழு
diff_check_with_linestatus(win_T *wp, linenr_T lnum, முழு *linestatus)
{
    முழு		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    முழு		maxcount;
    முழு		i;
    buf_T	*buf = wp->w_buffer;
    முழு		cmp;

    எனில் (linestatus != வெற்று)
	*linestatus = 0;

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    எனில் (curtab->tp_first_diff == வெற்று || !wp->w_p_diff)	// no diffs at all
	திரும்பு 0;

    // safety check: "lnum" must be a buffer line
    எனில் (lnum < 1 || lnum > buf->b_ml.ml_வரி_மொத்தம் + 1)
	திரும்பு 0;

    idx = diff_buf_idx(buf);
    எனில் (idx == DB_மொத்தம்)
	திரும்பு 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    எனில் (hasFoldingWin(wp, lnum, வெற்று, வெற்று, TRUE, வெற்று))
	திரும்பு 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = வெற்று;		// last block with df_lnum[idx] <= lnum
    க்கு (dp = diff_scan_start(curtab, idx, lnum); dp != வெற்று; dp = dp->df_next)
    {
	எனில் (dp->df_lnum[idx] <= lnum)
	    le = dp;
	எனில் (lnum <= dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
	    நிறுத்து;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    எனில் (dp == வெற்று || lnum < dp->df_lnum[idx])
	திரும்பு 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    எனில் (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    முழு இலக்கம்_fill = 0;
    போது (lnum == dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	எனில் (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    இலக்கம்_fill += maxcount - dp->df_மொத்தம்[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	எனில் (dp->df_next != வெற்று
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_மொத்தம்[idx])
	    dp = dp->df_next;
	இல்லையெனில்
	    நிறுத்து;
    }

    எனில் (lnum < dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
    {
	முழு	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	க்கு (i = 0; i < DB_மொத்தம்; ++i)
	    எனில் (i != idx && curtab->tp_diffbuf[i] != வெற்று)
	    {
		எனில் (dp->df_மொத்தம்[i] == 0)
		    zero = TRUE;
		இல்லையெனில்
		{
		    எனில் (dp->df_மொத்தம்[i] != dp->df_மொத்தம்[idx])
		    {
			எனில் (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			திரும்பு இலக்கம்_fill;
		    }
		    cmp = TRUE;
		}
	    }
	எனில் (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    க்கு (i = 0; i < DB_மொத்தம்; ++i)
		எனில் (i != idx && curtab->tp_diffbuf[i] != வெற்று
						      && dp->df_மொத்தம்[i] != 0)
		    எனில் (!diff_equal_entry(dp, idx, i))
		    {
			எனில் (linestatus)
			    *linestatus = -1;
			திரும்பு இலக்கம்_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	எனில் (zero == FALSE)
	    திரும்பு இலக்கம்_fill;
	எனில் (linestatus)
	    *linestatus = -2;
	திரும்பு இலக்கம்_fill;
    }
    திரும்பு இலக்கம்_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    நிலையான முழு
diff_equal_entry(diff_T *dp, முழு idx1, முழு idx2)
{
    முழு		i;
    char_u	*வரி;
    முழு		cmp;

    எனில் (dp->df_மொத்தம்[idx1] != dp->df_மொத்தம்[idx2])
	திரும்பு FALSE;
    எனில் (diff_check_sanity(curtab, dp) == FAIL)
	திரும்பு FALSE;
    க்கு (i = 0; i < dp->df_மொத்தம்[idx1]; ++i)
    {
	வரி = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	எனில் (வரி == வெற்று)
	    திரும்பு FALSE;
	cmp = diff_cmp(வரி, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(வரி);
	எனில் (cmp != 0)
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    நிலையான முழு
diff_equal_char(char_u *p1, char_u *p2, முழு *len)
{
    முழு l  = (*mb_ptr2len)(p1);

    எனில் (l != (*mb_ptr2len)(p2))
	திரும்பு FALSE;
    எனில் (l > 1)
    {
	எனில் (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    திரும்பு FALSE;
	*len = l;
    }
    இல்லையெனில்
    {
	எனில் ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    திரும்பு FALSE;
	*len = 1;
    }
    திரும்பு TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    நிலையான முழு
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    முழு		l;

    எனில் ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	திரும்பு 0;

    எனில் ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	திரும்பு STRCMP(s1, s2);
    எனில் ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	திரும்பு MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    போது (*p1 != NUL && *p2 != NUL)
    {
	எனில் (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	இல்லையெனில்
	{
	    எனில் (!diff_equal_char(p1, p2, &l))
		நிறுத்து;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    எனில் (*p1 != NUL || *p2 != NUL)
	திரும்பு 1;
    திரும்பு 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    முழு
diff_check_fill(win_T *wp, linenr_T lnum)
{
    முழு		n;

    // be quick when there are no filler lines
    எனில் (!(diff_flags & DIFF_FILLER))
	திரும்பு 0;
    n = diff_check_with_linestatus(wp, lnum, வெற்று);
    எனில் (n <= 0)
	திரும்பு 0;
    திரும்பு n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    வெறுமை
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    முழு		fromidx;
    முழு		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    எனில் (fromidx == DB_மொத்தம்)
	திரும்பு;		// safety check

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	எனில் (lnum <= dp->df_lnum[fromidx] + dp->df_மொத்தம்[fromidx])
	    நிறுத்து;
    எனில் (dp == வெற்று)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_வரி_மொத்தம்
				       - (frombuf->b_ml.ml_வரி_மொத்தம் - lnum);
    }
    இல்லையெனில்
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	எனில் (toidx == DB_மொத்தம்)
	    திரும்பு;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	எனில் (lnum >= dp->df_lnum[fromidx])
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
    எனில் (towin->w_topline > towin->w_buffer->b_ml.ml_வரி_மொத்தம்)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_வரி_மொத்தம்;
	towin->w_botfill = true;
    }
    எனில் (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_வரி_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (வெறுமை)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    வெற்று, TRUE, வெற்று);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    நிலையான முழு
parse_diffanchors(
    முழு		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    முழு		*இலக்கம்_anchors)
{
    முழு i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = வெற்று;
    எனில் (check_only)
	bufwin = curwin;
    இல்லையெனில்
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    எனில் (bufwin->w_buffer == buf && bufwin->w_p_diff)
		நிறுத்து;
	எனில் (bufwin == வெற்று && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    திரும்பு FAIL;
	}
    }

    க்கு (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	எனில் (*dia == ',') // don't allow empty values
	    திரும்பு FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(வெற்று, &dia, ADDR_வரிகள், check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	எனில் (dia == வெற்று) // error detected
	    திரும்பு FAIL;
	எனில் (*dia != ',' && *dia != NUL)
	    திரும்பு FAIL;

	எனில் (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_வரி_மொத்தம் + 1))
	{
	    emsg(_(e_invalid_range));
	    திரும்பு FAIL;
	}

	எனில் (anchors != வெற்று)
	    anchors[i] = lnum;

	எனில் (*dia == ',')
	    dia++;
    }
    எனில் (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	திரும்பு FAIL;
    }
    எனில் (இலக்கம்_anchors != வெற்று)
	*இலக்கம்_anchors = i;
    திரும்பு OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    முழு
diffanchors_changed(முழு buflocal)
{
    முழு result = parse_diffanchors(TRUE, curbuf, வெற்று, வெற்று);
    எனில் (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    எனில் (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    இல்லையெனில்
	    {
		க்கு (முழு idx = 0; idx < DB_மொத்தம்; ++idx)
		    எனில் (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			நிறுத்து;
		    }
	    }
	}
    }
    திரும்பு result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    முழு
diffopt_changed(வெறுமை)
{
    char_u	*p;
    முழு		diff_context_new = 6;
    முழு		linematch_வரிகள்_new = 0;
    முழு		diff_flags_new = 0;
    முழு		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    போது (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	எனில் (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	இல்லையெனில் எனில் (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	இல்லையெனில் எனில் (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    எனில் (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    இல்லையெனில்
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    எனில் (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    இல்லையெனில்
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_வரிகள்_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    எனில் ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	திரும்பு FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    எனில் (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_வரிகள் = linematch_வரிகள்_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    திரும்பு OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    முழு
diffopt_horizontal(வெறுமை)
{
    திரும்பு (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    முழு
diffopt_hiddenoff(வெறுமை)
{
    திரும்பு (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    முழு
diffopt_closeoff(வெறுமை)
{
    திரும்பு (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    வெறுமை
diff_update_வரி(linenr_T lnum)
{
    முழு		idx;
    diff_T	*dp;

    எனில் (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	திரும்பு;

    idx = diff_buf_idx(curbuf);
    எனில் (idx == DB_மொத்தம்)
	திரும்பு;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	எனில் (lnum <= dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
	    நிறுத்து;

    // clear the inline change cache as it's invalid
    எனில் (dp != வெற்று)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

நிலையான diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    முழு
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    முழு *change_start,
    முழு *change_end)
{
    எனில் (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    இல்லையெனில்
	*change_start = change->dc_start[diffline->bufidx];
    எனில் (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    இல்லையெனில்
	*change_end = change->dc_end[diffline->bufidx];

    எனில் (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	திரும்பு FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
    {
	எனில் (i == diffline->bufidx)
	    தொடர்;
	எனில் (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    திரும்பு FALSE;
	}
    }
    திரும்பு TRUE;
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
    நிலையான முழு
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    முழு		idx,
    முழு		*startp,	// first char of the change
    முழு		*endp)		// last char of the change
{
    char_u	*வரி_org;
    char_u	*வரி_new;
    முழு		i;
    முழு		si_org, si_new;
    முழு		ei_org, ei_new;
    முழு		off;
    முழு		added = TRUE;
    char_u	*p1, *p2;
    முழு		l;

    எனில் (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	வரி_org = வெற்று;
    }
    இல்லையெனில்
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	வரி_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	எனில் (வரி_org == வெற்று)
	    திரும்பு FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    க்கு (i = 0; i < DB_மொத்தம்; ++i)
	எனில் (curtab->tp_diffbuf[i] != வெற்று && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    எனில் (off >= dp->df_மொத்தம்[i])
		தொடர்;
	    added = FALSE;
	    எனில் (diff_flags & DIFF_INLINE_NONE)
		நிறுத்து; // early terminate as we only care about the return value

	    வரி_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    போது (வரி_org[si_org] != NUL)
	    {
		எனில் (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(வரி_org[si_org])
					      && VIM_ISWHITE(வரி_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(வரி_org[si_org])
					    || VIM_ISWHITE(வரி_new[si_new]))))
		{
		    si_org = (முழு)(skipwhite(வரி_org + si_org) - வரி_org);
		    si_new = (முழு)(skipwhite(வரி_new + si_new) - வரி_new);
		}
		இல்லையெனில்
		{
		    எனில் (!diff_equal_char(வரி_org + si_org, வரி_new + si_new,
									   &l))
			நிறுத்து;
		    si_org += l;
		    si_new += l;
		}
	    }
	    எனில் (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(வரி_org, வரி_org + si_org);
		si_new -= (*mb_head_off)(வரி_new, வரி_new + si_new);
	    }
	    எனில் (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    எனில் (வரி_org[si_org] != NUL || வரி_new[si_new] != NUL)
	    {
		ei_org = (முழு)STRLEN(வரி_org);
		ei_new = (முழு)STRLEN(வரி_new);
		போது (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    எனில் (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(வரி_org[ei_org])
					      && VIM_ISWHITE(வரி_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(வரி_org[ei_org])
					    || VIM_ISWHITE(வரி_new[ei_new]))))
		    {
			போது (ei_org >= *startp
					     && VIM_ISWHITE(வரி_org[ei_org]))
			    --ei_org;
			போது (ei_new >= si_new
					     && VIM_ISWHITE(வரி_new[ei_new]))
			    --ei_new;
		    }
		    இல்லையெனில்
		    {
			p1 = வரி_org + ei_org;
			p2 = வரி_new + ei_new;
			p1 -= (*mb_head_off)(வரி_org, p1);
			p2 -= (*mb_head_off)(வரி_new, p2);
			எனில் (!diff_equal_char(p1, p2, &l))
			    நிறுத்து;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		எனில் (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(வரி_org);
    திரும்பு added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
வகைவரை கட்டமைப்பு
{
    long byte_start;
    long இலக்கம்_bytes;
    முழு lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    நிலையான வெறுமை
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, முழு idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    முழு pass = 1;
    செய்
    {
	முழு has_unmerged_gaps = FALSE;
	முழு has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	போது (dp!= வெற்று && dp->df_next != வெற்று)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    எனில் (dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		தொடர்;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    எனில் (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		தொடர்;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1]);
	    எனில் (gap <= 3)
	    {
		linenr_T max_df_மொத்தம் = 0;
		க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
		    max_df_மொத்தம் = MAX(max_df_மொத்தம், dp->df_மொத்தம்[i] + dp->df_next->df_மொத்தம்[i]);

		எனில் (max_df_மொத்தம் >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
		    {
			dp->df_மொத்தம்[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_மொத்தம்[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    தொடர்;
		}
		இல்லையெனில்
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	எனில் (!has_unmerged_gaps || !has_merged_gaps)
	    நிறுத்து;
    } போது (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    நிலையான வெறுமை
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, முழு idx1,
				  linenr_T start_lnum)
{
    முழு pass = 1;
    செய்
    {
	diff_T *dp = dp_orig;

	போது (dp != வெற்று && dp->df_next != வெற்று)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    எனில் (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		தொடர்;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    எனில் (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		தொடர்;
	    }

	    // Calculate the gap between blocks
	    முழு gap_start = entry1->byte_start + entry1->இலக்கம்_bytes;
	    முழு gap_end = entry2->byte_start;
	    முழு gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    எனில் (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		தொடர்;
	    }

	    // Get the text between the two blocks
	    char_u *வரி = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = வரி + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    க்கு (முழு i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		முழு char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		எனில் (char_class == 2)
		{
		    only_non_word = false;
		    நிறுத்து;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    எனில் (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
		{
		    எனில் (curtab->tp_diffbuf[i] != வெற்று)
		    {
			// count bytes in the first block
			க்கு (முழு k = 0; k < dp->df_மொத்தம்[i]; k++)
			{
			    முழு idx = dp->df_lnum[i] + k - 1;
			    எனில் (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].இலக்கம்_bytes;
			}
			// count bytes in the next block
			க்கு (முழு k = 0; k < dp->df_next->df_மொத்தம்[i]; k++)
			{
			    முழு idx = dp->df_next->df_lnum[i] + k - 1;
			    எனில் (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].இலக்கம்_bytes;
			}
		    }
		}

		எனில் (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
		    {
			எனில் (curtab->tp_diffbuf[i] != வெற்று)
			{
			    dp->df_மொத்தம்[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_மொத்தம்[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    தொடர்;
		}
	    }

	    dp = dp->df_next;
	}
    } போது (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    நிலையான வெறுமை
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_மொத்தம்];
    garray_T	file1_str;
    garray_T	file2_str;
    முழு		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, அளவு(எழுத்து *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = வெற்று;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_மொத்தம்]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, அளவு(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
	ga_init2(&linemap[i], அளவு(linemap_entry_T), 128);

    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
	    தொடர்; // skip buffer that isn't loaded

	எனில் (dp->df_மொத்தம்[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = வெற்று;
	    தொடர்;
	}

	எனில் (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	க்கு (முழு off = 0; off < dp->df_மொத்தம்[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    முழு in_keyword = FALSE;

	    // iwhiteeol support vars
	    முழு last_white = FALSE;
	    முழு eol_ga_len = -1;
	    முழு eol_linemap_len = -1;
	    முழு eol_numlines = -1;

	    char_u *s;
	    க்கு (s = curline; *s != NUL;)
	    {
		முழு new_in_keyword = FALSE;
		எனில் (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		எனில் (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		எனில் (VIM_ISWHITE(*s))
		{
		    எனில் (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			தொடர்;
		    }
		    இல்லையெனில் எனில் ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			எனில் (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		இல்லையெனில்
		{
		    எனில் ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		முழு char_len = 1;
		எனில் (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		இல்லையெனில்
		{
		    char_len = mb_ptr2len(s);

		    எனில் (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    எனில் (diff_flags & DIFF_ICASE)
		    {
			முழு c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			முழு c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			முழு c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			எனில் (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    இல்லையெனில்
			ga_concat_len(curstr, s, char_len);
		}

		எனில் (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		எனில் (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.இலக்கம்_bytes = char_len;
		    எனில் (ga_grow(&linemap[i], 1) != OK)
			செல் done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		இல்லையெனில்
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].இலக்கம்_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    எனில் (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    எனில் ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		எனில் (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    எனில் (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.இலக்கம்_bytes = அளவு(NL);
		எனில் (ga_grow(&linemap[i], 1) != OK)
		    செல் done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	எனில் (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (எழுத்து *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	இல்லையெனில்
	{
	    dio.dio_orig.din_mmfile.ptr = (எழுத்து *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	எனில் (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    முழு diff_status = diff_file_internal(&dio);
	    எனில் (diff_status == FAIL)
		செல் done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    எனில் (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    இல்லையெனில் எனில் (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    க்கு (; new_diff != வெற்று; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
	{
	    எனில் (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		தொடர்;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_மொத்தம்[i];

	    எனில் (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    இல்லையெனில்
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    எனில் (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    இல்லையெனில் எனில் (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    இல்லையெனில்
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].இலக்கம்_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	எனில் (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    செல் done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, அளவு(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    முழு
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    முழு		idx;
    முழு		off;

    idx = diff_buf_idx(wp->w_buffer);
    எனில் (idx == DB_மொத்தம்)	// cannot happen
	திரும்பு FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	எனில் (lnum < dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
	    நிறுத்து;
    எனில் (dp == வெற்று || diff_check_sanity(curtab, dp) == FAIL)
	திரும்பு FALSE;

    எனில் (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	திரும்பு FALSE;
    off = lnum - dp->df_lnum[idx];

    எனில் (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	முழு	change_start = MAXCOL;	// first col of changed area
	முழு	change_end = -1;	// last col of changed area
	முழு	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->இலக்கம்_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	திரும்பு ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    எனில் (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    முழு இலக்கம்_changes = 0;
    முழு change_idx = 0;
    diffline->changes = வெற்று;
    க்கு (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	எனில் (change->dc_end_lnum_off[idx] < off)
	    தொடர்;
	எனில் (change->dc_start_lnum_off[idx] > off)
	    நிறுத்து;
	எனில் (diffline->changes == வெற்று)
	    diffline->changes = change;
	இலக்கம்_changes++;
    }
    diffline->இலக்கம்_changes = இலக்கம்_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    முழு added = FALSE;
    எனில் (இலக்கம்_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	க்கு (முழு i = 0; i < DB_மொத்தம்; i++)
	{
	    எனில் (idx == i)
		தொடர்;
	    எனில் (curtab->tp_diffbuf[i] == வெற்று)
		தொடர்;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    எனில் (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		நிறுத்து;
	    }
	}
    }
    திரும்பு added;
}

# எனில் defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    முழு
diff_infold(win_T *wp, linenr_T lnum)
{
    முழு		i;
    முழு		idx = -1;
    முழு		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    எனில் (!wp->w_p_diff)
	திரும்பு FALSE;

    க்கு (i = 0; i < DB_மொத்தம்; ++i)
    {
	எனில் (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	இல்லையெனில் எனில் (curtab->tp_diffbuf[i] != வெற்று)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    எனில் (idx == -1 || !other)
	திரும்பு FALSE;

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    எனில் (curtab->tp_first_diff == வெற்று)
	திரும்பு TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = வெற்று;		// last block with df_lnum[idx] <= lnum
    முழு		result = TRUE;
    க்கு (dp = diff_scan_start(curtab, idx, lnum); dp != வெற்று; dp = dp->df_next)
    {
	எனில் (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	எனில் (dp->df_lnum[idx] - diff_context > lnum)
	    நிறுத்து;
	// If this change ends before the line we have a match.
	எனில் (dp->df_lnum[idx] + dp->df_மொத்தம்[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    நிறுத்து;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    திரும்பு result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    வெறுமை
nv_diffgetput(முழு put, long மொத்தம்)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    எனில் (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	திரும்பு;
    }
# endif
    எனில் (மொத்தம் == 0)
	ea.arg = (char_u *)"";
    இல்லையெனில்
    {
	vim_snprintf((எழுத்து *)buf, 30, "%ld", மொத்தம்);
	ea.arg = buf;
    }
    எனில் (put)
	ea.cmdidx = CMD_diffput;
    இல்லையெனில்
	ea.cmdidx = CMD_diffget;
    ea.addr_மொத்தம் = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    நிலையான முழு
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	எனில் (dp == diff)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    வெறுமை
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    முழு		மொத்தம்;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    முழு		idx_cur;
    முழு		idx_other;
    முழு		idx_from;
    முழு		idx_to;
    முழு		i;
    முழு		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    முழு		start_skip, end_skip;
    முழு		new_மொத்தம்;
    முழு		buf_empty;
    முழு		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    எனில் (idx_cur == DB_மொத்தம்)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	திரும்பு;
    }

    எனில் (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	க்கு (idx_other = 0; idx_other < DB_மொத்தம்; ++idx_other)
	    எனில் (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != வெற்று)
	    {
		எனில் (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    நிறுத்து;
		found_not_ma = TRUE;
	    }
	எனில் (idx_other == DB_மொத்தம்)
	{
	    எனில் (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    இல்லையெனில்
		emsg(_(e_no_other_buffer_in_diff_mode));
	    திரும்பு;
	}

	// Check that there isn't a third buffer in the list
	க்கு (i = idx_other + 1; i < DB_மொத்தம்; ++i)
	    எனில் (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != வெற்று
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		திரும்பு;
	    }
    }
    இல்லையெனில்
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	போது (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	க்கு (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	எனில் (eap->arg + i == p)	    // digits only
	    i = atol((எழுத்து *)eap->arg);
	இல்லையெனில்
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    எனில் (i < 0)
		திரும்பு;		// error message already given
	}
	buf = buflist_findnr(i);
	எனில் (buf == வெற்று)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    திரும்பு;
	}
	எனில் (buf == curbuf)
	    திரும்பு;		// nothing to do
	idx_other = diff_buf_idx(buf);
	எனில் (idx_other == DB_மொத்தம்)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    திரும்பு;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    எனில் (eap->addr_மொத்தம் == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	முழு linestatus = 0;
	எனில் (eap->line1 == curbuf->b_ml.ml_வரி_மொத்தம்
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	இல்லையெனில் எனில் (eap->line1 > 0)
	    --eap->line1;
    }

    எனில் (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    இல்லையெனில்
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	எனில் (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    செல் theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    எனில் (!curbuf->b_changed)
    {
	change_warning(0);
	எனில் (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    செல் theend;
	}
    }

    dprev = வெற்று;
    க்கு (dp = curtab->tp_first_diff; dp != வெற்று; )
    {
	எனில் (!eap->addr_மொத்தம்)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    போது (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_மொத்தம்[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	எனில் (dp->df_lnum[idx_cur] > eap->line2 + off)
	    நிறுத்து;	// past the range that was specified

	dfree = வெற்று;
	lnum = dp->df_lnum[idx_to];
	மொத்தம் = dp->df_மொத்தம்[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + மொத்தம் + (மொத்தம் == 0 && BUFEMPTY() ? 1 : 0);

	எனில் (dp->df_lnum[idx_cur] + dp->df_மொத்தம்[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    எனில் (eap->addr_மொத்தம் > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		எனில் (start_skip > 0)
		{
		    // range starts below start of current diff block
		    எனில் (start_skip > மொத்தம்)
		    {
			lnum += மொத்தம்;
			மொத்தம் = 0;
		    }
		    இல்லையெனில்
		    {
			மொத்தம் -= start_skip;
			lnum += start_skip;
		    }
		}
		இல்லையெனில்
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_மொத்தம்[idx_cur] - 1
							 - (eap->line2 + off);
		எனில் (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    எனில் (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_மொத்தம்[idx_cur] - start_skip - end_skip;
			எனில் (மொத்தம் > i)
			    மொத்தம் = i;
		    }
		    இல்லையெனில்			// :diffget
		    {
			மொத்தம் -= end_skip;
			end_skip = dp->df_மொத்தம்[idx_from] - start_skip - மொத்தம்;
			எனில் (end_skip < 0)
			    end_skip = 0;
		    }
		}
		இல்லையெனில்
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    க்கு (i = 0; i < மொத்தம்; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_வரி_மொத்தம் == 1;
		எனில் (ml_delete(lnum) == OK)
		    --added;
	    }
	    க்கு (i = 0; i < dp->df_மொத்தம்[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		எனில் (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_வரி_மொத்தம்)
		    நிறுத்து;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		எனில் (p != வெற்று)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    எனில் (buf_empty && curbuf->b_ml.ml_வரி_மொத்தம் == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_மொத்தம் = dp->df_மொத்தம்[idx_to] + added;
	    dp->df_மொத்தம்[idx_to] = new_மொத்தம்;

	    எனில் (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		க்கு (i = 0; i < DB_மொத்தம்; ++i)
		    எனில் (curtab->tp_diffbuf[i] != வெற்று && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			நிறுத்து;
		எனில் (i == DB_மொத்தம்)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    எனில் (dprev == வெற்று)
			curtab->tp_first_diff = dp;
		    இல்லையெனில்
			dprev->df_next = dp;
		}
	    }

	    எனில் (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + மொத்தம் - 1, (long)MAXLNUM, (long)added);
		எனில் (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    எனில் (curwin->w_cursor.lnum >= lnum + மொத்தம்)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_வரி_மொத்தம்);
		    }
		    இல்லையெனில் எனில் (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_வரிகள்(lnum, 0, lnum + மொத்தம், (long)added);

	    எனில் (dfree != வெற்று)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    எனில் (added != 0 && !valid_diff(dp))
		நிறுத்து;

	    எனில் (dfree == வெற்று)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_மொத்தம்[idx_to] = new_மொத்தம்;

	    // When changing the current buffer, keep track of line numbers
	    எனில் (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	எனில் (dfree == வெற்று)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    எனில் (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	எனில் (சாவி_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    எனில் (diff_need_update)
	ex_diffupdate(வெற்று);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_வரி_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    எனில் (curtab->tp_first_diff == வெற்று)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    எனில் (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    எனில் (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    இல்லையெனில்
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, வெற்று, வெற்று, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    நிலையான வெறுமை
diff_fold_update(diff_T *dp, முழு skip_idx)
{
    முழு		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	க்கு (i = 0; i < DB_மொத்தம்; ++i)
	    எனில் (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_மொத்தம்[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    முழு
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	எனில் (diff_buf_idx_tp(buf, tp) != DB_மொத்தம்)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    முழு
diff_move_to(முழு dir, long மொத்தம்)
{
    முழு		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    எனில் (idx == DB_மொத்தம் || curtab->tp_first_diff == வெற்று)
	திரும்பு FAIL;

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    எனில் (curtab->tp_first_diff == வெற்று)		// no diffs today
	திரும்பு FAIL;

    போது (--மொத்தம் >= 0)
    {
	// Check if already before first diff.
	எனில் (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    நிறுத்து;

	க்கு (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    எனில் (dp == வெற்று)
		நிறுத்து;
	    எனில் ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == வெற்று
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		நிறுத்து;
	    }
	}
    }

    // don't end up past the end of the file
    எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	lnum = curbuf->b_ml.ml_வரி_மொத்தம்;

    // When the cursor didn't move at all we fail.
    எனில் (lnum == curwin->w_cursor.lnum)
	திரும்பு FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    திரும்பு OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    நிலையான linenr_T
diff_get_corresponding_வரி_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    முழு		idx1;
    முழு		idx2;
    diff_T	*dp;
    முழு		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    எனில் (idx1 == DB_மொத்தம் || idx2 == DB_மொத்தம் || curtab->tp_first_diff == வெற்று)
	திரும்பு lnum1;

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    எனில் (curtab->tp_first_diff == வெற்று)		// no diffs today
	திரும்பு lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	எனில் (dp->df_lnum[idx1] > lnum1)
	    திரும்பு lnum1 - baseline;
	எனில் ((dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    எனில் (baseline > dp->df_மொத்தம்[idx2])
		baseline = dp->df_மொத்தம்[idx2];

	    திரும்பு dp->df_lnum[idx2] + baseline;
	}
	எனில் (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_மொத்தம்[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_மொத்தம்[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    திரும்பு curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_மொத்தம்[idx1])
				   - (dp->df_lnum[idx2] + dp->df_மொத்தம்[idx2]);
    }

    // If we get here then the cursor is after the last diff
    திரும்பு lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_வரி(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_வரி_int(buf1, lnum1);

    // don't end up past the end of the file
    எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு curbuf->b_ml.ml_வரி_மொத்தம்;
    திரும்பு lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    முழு		idx;
    முழு		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    எனில் (idx == DB_மொத்தம்)		// safety check
	திரும்பு (linenr_T)0;

    எனில் (curtab->tp_diff_invalid)
	ex_diffupdate(வெற்று);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	எனில் (lnum <= dp->df_lnum[idx] + dp->df_மொத்தம்[idx])
	    நிறுத்து;

    // When after the last change, compute relative to the last line number.
    எனில் (dp == வெற்று)
	திரும்பு wp->w_buffer->b_ml.ml_வரி_மொத்தம்
					- (curbuf->b_ml.ml_வரி_மொத்தம் - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    எனில் (i == DB_மொத்தம்)			// safety check
	திரும்பு (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    எனில் (n > dp->df_lnum[i] + dp->df_மொத்தம்[i])
	n = dp->df_lnum[i] + dp->df_மொத்தம்[i];
    திரும்பு n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    நிலையான முழு
parse_diff_ed(
	char_u	    *வரி,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    முழு	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = வரி;
    f1 = getdigits(&p);
    எனில் (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    இல்லையெனில்
	l1 = f1;
    எனில் (*p != 'a' && *p != 'c' && *p != 'd')
	திரும்பு FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    எனில் (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    இல்லையெனில்
	l2 = f2;
    எனில் (l1 < f1 || l2 < f2)
	திரும்பு FAIL;

    எனில் (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->மொத்தம்_orig = 0;
    }
    இல்லையெனில்
    {
	hunk->lnum_orig = f1;
	hunk->மொத்தம்_orig = l1 - f1 + 1;
    }
    எனில் (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->மொத்தம்_new = 0;
    }
    இல்லையெனில்
    {
	hunk->lnum_new = f2;
	hunk->மொத்தம்_new = l2 - f2 + 1;
    }
    திரும்பு OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    நிலையான முழு
parse_diff_unified(
	char_u	    *வரி,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = வரி;
    எனில் (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	எனில் (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	இல்லையெனில்
	    oldcount = 1;
	எனில் (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    எனில் (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    இல்லையெனில்
		newcount = 1;
	}
	இல்லையெனில்
	    திரும்பு FAIL;	// invalid diff format

	எனில் (oldcount == 0)
	    oldline += 1;
	எனில் (newcount == 0)
	    newline += 1;
	எனில் (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->மொத்தம்_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->மொத்தம்_new = newcount;

	திரும்பு OK;
    }

    திரும்பு FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    நிலையான முழு
xdiff_out_indices(
	long start_a,
	long மொத்தம்_a,
	long start_b,
	long மொத்தம்_b,
	வெறுமை *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    எனில் (p == வெற்று)
	திரும்பு -1;

    எனில் (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	திரும்பு -1;
    }

    p->lnum_orig  = start_a + 1;
    p->மொத்தம்_orig = மொத்தம்_a;
    p->lnum_new   = start_b + 1;
    p->மொத்தம்_new  = மொத்தம்_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    திரும்பு 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    நிலையான முழு
xdiff_out_unified(
	வெறுமை *priv,
	mmbuffer_t *mb,
	முழு nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    முழு		i;

    க்கு (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    திரும்பு 0;
}

#endif	// FEAT_DIFF

#எனில் defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    வெறுமை
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    வெறுமை
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    நிலையான linenr_T	prev_lnum = 0;
    நிலையான varnumber_T	changedtick = 0;
    நிலையான முழு		fnum = 0;
    நிலையான முழு		prev_diff_flags = 0;
    நிலையான முழு		change_start = 0;
    நிலையான முழு		change_end = 0;
    நிலையான hlf_T	hlID = (hlf_T)0;
    முழு			cache_results = TRUE;
    முழு			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    எனில் (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    எனில் (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	முழு linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	எனில் (linestatus < 0)
	{
	    எனில் (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		எனில் (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		இல்லையெனில்
		{
		    hlID = HLF_CHD;	// changed line
		    எனில் (diffline.இலக்கம்_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    இல்லையெனில்
		hlID = HLF_ADD;	// added line
	}
	இல்லையெனில்
	    hlID = (hlf_T)0;

	எனில் (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    எனில் (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_எண்(&argvars[1]) - 1; // ignore type error in {col}
	எனில் (cache_results)
	{
	    எனில் (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    இல்லையெனில்
		hlID = HLF_CHD;			// changed line
	}
	இல்லையெனில்
	{
	    hlID = HLF_CHD;
	    க்கு (முழு i = 0; i < diffline.இலக்கம்_changes; i++)
	    {
		முழு added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		எனில் (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    நிறுத்து;
		}
		எனில் (col < change_start)
		    // the remaining changes are past this column and not relevant
		    நிறுத்து;
	    }
	}
    }
    rettv->vval.v_எண் = hlID == (hlf_T)0 ? 0 : (முழு)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    நிலையான முழு
parse_diff_optarg(
    typval_T	    *opts,
    முழு		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    முழு		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    எனில் (algo != வெற்று)
    {
	எனில் (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	இல்லையெனில் எனில் (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	இல்லையெனில் எனில் (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	இல்லையெனில் எனில் (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    எனில் (output_fmt != வெற்று)
    {
	எனில் (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	இல்லையெனில் எனில் (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	இல்லையெனில்
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    திரும்பு FAIL;
	}
    }

    *diff_ctxlen = dict_get_எண்_def(d, "context", 0);
    எனில் (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    எனில் (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    எனில் (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    எனில் (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    எனில் (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    எனில் (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    எனில் (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    திரும்பு OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    நிலையான வெறுமை
list_to_diffin(list_T *l, diffin_T *din, முழு icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	str = tv_get_string(&li->li_tv);
	எனில் (icase)
	{
	    str = strlow_save(str);
	    எனில் (str == வெற்று)
		தொடர்;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	எனில் (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (எழுத்து *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    நிலையான dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    எனில் (hunk_dict == வெற்று)
	திரும்பு வெற்று;

    dict_add_எண்(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_எண்(hunk_dict, "from_count", hunk->மொத்தம்_orig);
    dict_add_எண்(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_எண்(hunk_dict, "to_count", hunk->மொத்தம்_new);

    திரும்பு hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    வெறுமை
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    எனில் (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	திரும்பு;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, அளவு(எழுத்து *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    முழு		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	எனில் (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    திரும்பு;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    முழு	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    எனில் (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    முழு diff_status = diff_file(&dio);

    // restore 'diffexpr'
    எனில் (restore_diffexpr)
	*p_dex = cc;

    எனில் (diff_status == FAIL)
	செல் done;

    முழு		hunk_idx = 0;
    dict_T	*hunk_dict;

    எனில் (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	எனில் (rettv_list_alloc(rettv) != OK)
	    செல் done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = வெற்று;
	போது (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    எனில் (hunk_dict == வெற்று)
		செல் done;

	    எனில் (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    இல்லையெனில்
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    எனில் (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    இல்லையெனில்
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
