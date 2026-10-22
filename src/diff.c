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

ساکن صحیح diff_busy = FALSE;	    // using diff structs, don't change them
ساکن صحیح diff_need_update = FALSE; // ex_diffupdate needs to be called

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
# define DIFF_اندرونی	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_لفظ    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_لفظ)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_لفظ)
ساکن صحیح	diff_flags = DIFF_اندرونی | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ساکن طویل diff_algorithm = XDF_INDENT_HEURISTIC;
ساکن صحیح diff_لفظ_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_سائز (1024L * 1024 * 1023)

ساکن صحیح diff_a_چلتا = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# اگر defined(MSWIN)
ساکن صحیح diff_bin_چلتا = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
قسمتعریف ساخت {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
قسمتعریف ساخت {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
قسمتعریف ساخت {
    linenr_T lnum_orig;
    طویل     گنتی_orig;
    linenr_T lnum_new;
    طویل     گنتی_new;
} diffhunk_T;

قسمتعریف شمار {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
قسمتعریف ساخت {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    صحیح		    dio_اندرونی; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    صحیح		    dio_ctxlen;   // unified diff context length
} diffio_T;

ساکن صحیح diff_buf_idx(buf_T *buf);
ساکن صحیح diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ساکن خالی diff_mark_adjust_tp(tabpage_T *tp, صحیح idx, linenr_T line1, linenr_T line2, طویل amount, طویل amount_after);
ساکن خالی diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ساکن صحیح diff_check_sanity(tabpage_T *tp, diff_T *dp);
ساکن صحیح check_external_diff(diffio_T *diffio);
ساکن صحیح diff_file(diffio_T *diffio);
ساکن صحیح diff_equal_اندراج(diff_T *dp, صحیح idx1, صحیح idx2);
ساکن صحیح diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ساکن خالی diff_fold_update(diff_T *dp, صحیح skip_idx);
# endif
ساکن خالی diff_read(صحیح idx_orig, صحیح idx_new, diffio_T *dio);
ساکن خالی diff_copy_اندراج(diff_T *dprev, diff_T *dp, صحیح idx_orig, صحیح idx_new);
ساکن diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ساکن صحیح parse_diff_ed(char_u *سطر, diffhunk_T *hunk);
ساکن صحیح parse_diff_unified(char_u *سطر, diffhunk_T *hunk);
ساکن صحیح xdiff_برآمد_indices(طویل آغاز_a, طویل گنتی_a, طویل آغاز_b, طویل گنتی_b, خالی *priv);
ساکن صحیح xdiff_برآمد_unified(خالی *priv, mmbuffer_t *mb, صحیح nbuf);
ساکن صحیح parse_diffanchors(صحیح check_only, buf_T *buf, linenr_T *anchors, صحیح *عدد_anchors);

# define FOR_ALL_DIFFBLOCKS_اندر_TAB(tp, dp) \
    برائے ((dp) = (tp)->tp_پہلا_diff; (dp) != تہی; (dp) = (dp)->df_اگلا)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ساکن diff_T	    *diff_finger_dp = تہی;
ساکن tabpage_T    *diff_finger_tp = تہی;
ساکن صحیح	    diff_finger_idx = -1;
ساکن linenr_T	    diff_finger_lnum = 0;

    ساکن خالی
clear_diffblock(diff_T *dp)
{
    اگر (dp == diff_finger_dp)
	diff_finger_dp = تہی;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ساکن diff_T *
diff_جائزہ_آغاز(tabpage_T *tp, صحیح idx, linenr_T lnum)
{
    اگر (diff_finger_dp != تہی
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	واپس diff_finger_dp;
    واپس tp->tp_پہلا_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ساکن خالی
diff_جائزہ_remember(tabpage_T *tp, صحیح idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != تہی ? dp : tp->tp_پہلا_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    خالی
diff_buf_delete(buf_T *buf)
{
    صحیح		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	اگر (i != DB_گنتی)
	{
	    tp->tp_diffbuf[i] = تہی;
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
    خالی
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    صحیح		i;

    اگر (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_بفر == win->w_بفر && wp->w_p_diff)
		توڑو;
	اگر (wp == تہی)
	{
	    i = diff_buf_idx(win->w_بفر);
	    اگر (i != DB_گنتی)
	    {
		curtab->tp_diffbuf[i] = تہی;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    ورنہ
	diff_buf_جوڑو(win->w_بفر);
}

/*
 * Add a buffer to make diffs for.
 * Call this when a new buffer is being edited in the current window where
 * 'diff' is set.
 * Marks the current buffer as being part of the diff and requiring updating.
 * This must be done before any autocmd, because a command may use info
 * about the screen contents.
 */
    خالی
diff_buf_جوڑو(buf_T *buf)
{
    صحیح		i;

    اگر (diff_buf_idx(buf) != DB_گنتی)
	واپس;		// It's already there.

    برائے (i = 0; i < DB_گنتی; ++i)
	اگر (curtab->tp_diffbuf[i] == تہی)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    واپس;
	}

    semsg(_(e_cannot_diff_مزید_than_nr_buffers), DB_گنتی);
}

/*
 * Remove all buffers to make diffs for.
 */
    ساکن خالی
diff_buf_clear(خالی)
{
    صحیح		i;

    برائے (i = 0; i < DB_گنتی; ++i)
	اگر (curtab->tp_diffbuf[i] != تہی)
	{
	    curtab->tp_diffbuf[i] = تہی;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ساکن صحیح
diff_buf_idx(buf_T *buf)
{
    صحیح		idx;

    برائے (idx = 0; idx < DB_گنتی; ++idx)
	اگر (curtab->tp_diffbuf[idx] == buf)
	    توڑو;
    واپس idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ساکن صحیح
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    صحیح		idx;

    برائے (idx = 0; idx < DB_گنتی; ++idx)
	اگر (tp->tp_diffbuf[idx] == buf)
	    توڑو;
    واپس idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    خالی
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    صحیح		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	اگر (i != DB_گنتی)
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
    خالی
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    صحیح		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	اگر (idx != DB_گنتی)
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
    ساکن خالی
diff_mark_adjust_tp(
    tabpage_T	*tp,
    صحیح		idx,
    linenr_T	line1,
    linenr_T	line2,
    طویل	amount,
    طویل	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    صحیح		i;
    صحیح		inserted, deleted;
    صحیح		n, off;
    linenr_T	آخری;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    صحیح		check_unchanged;

    diff_finger_dp = تہی;		// may shift df_lnum, invalidate the finger

    اگر (diff_اندرونی())
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
    ورنہ اگر (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    ورنہ
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = تہی;
    dp = tp->tp_پہلا_diff;
    برائے (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	اگر ((dp == تہی || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == تہی
		    || dprev->df_lnum[idx] + dprev->df_گنتی[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    اگر (dnext == تہی)
		واپس;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_گنتی[idx] = inserted;
	    برائے (i = 0; i < DB_گنتی; ++i)
		اگر (tp->tp_diffbuf[i] != تہی && i != idx)
		{
		    اگر (dprev == تہی)
			dnext->df_lnum[i] = line1;
		    ورنہ
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_گنتی[i])
			    - (dprev->df_lnum[idx] + dprev->df_گنتی[idx]);
		    dnext->df_گنتی[i] = deleted;
		}
	}

	// if at end of the list, quit
	اگر (dp == تہی)
	    توڑو;

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
	آخری = dp->df_lnum[idx] + dp->df_گنتی[idx] - 1;

	// 1. change completely above line1: nothing to do
	اگر (آخری >= line1 - 1)
	{
	    اگر (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		اگر (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_اگلا;
		جاری;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    اگر (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		اگر (amount_after == 0)
		    توڑو;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    ورنہ
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		اگر (deleted > 0)
		{
		    off = 0;
		    اگر (dp->df_lnum[idx] >= line1)
		    {
			اگر (آخری <= line2)
			{
			    // 4. delete all lines of diff
			    اگر (dp->df_اگلا != تہی
				    && dp->df_اگلا->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_اگلا->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_گنتی[idx];
				lnum_deleted = dp->df_اگلا->df_lnum[idx];
			    }
			    ورنہ
				n = deleted - dp->df_گنتی[idx];
			    dp->df_گنتی[idx] = 0;
			}
			ورنہ
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_گنتی[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    ورنہ
		    {
			اگر (آخری < line2)
			{
			    // 2. delete at end of diff
			    dp->df_گنتی[idx] -= آخری - lnum_deleted + 1;
			    اگر (dp->df_اگلا != تہی
				    && dp->df_اگلا->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_اگلا->df_lnum[idx] - 1 - آخری;
				deleted -= dp->df_اگلا->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_اگلا->df_lnum[idx];
			    }
			    ورنہ
				n = line2 - آخری;
			    check_unchanged = TRUE;
			}
			ورنہ
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_گنتی[idx] -= deleted;
			}
		    }

		    برائے (i = 0; i < DB_گنتی; ++i)
			اگر (tp->tp_diffbuf[i] != تہی && i != idx)
			{
			    اگر (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    ورنہ
				dp->df_lnum[i] = 1;
			    dp->df_گنتی[i] += n;
			}
		}
		ورنہ
		{
		    اگر (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_گنتی[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    ورنہ
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
	اگر (dprev != تہی && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_گنتی[idx]
							== dp->df_lnum[idx])
	{
	    برائے (i = 0; i < DB_گنتی; ++i)
		اگر (tp->tp_diffbuf[i] != تہی)
		    dprev->df_گنتی[i] += dp->df_گنتی[i];
	    dprev->df_اگلا = dp->df_اگلا;
	    clear_diffblock(dp);
	    dp = dprev->df_اگلا;
	}
	ورنہ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_اگلا;
	}
    }

    dprev = تہی;
    dp = tp->tp_پہلا_diff;
    جب (dp != تہی)
    {
	// All counts are zero, remove this entry.
	برائے (i = 0; i < DB_گنتی; ++i)
	    اگر (tp->tp_diffbuf[i] != تہی && dp->df_گنتی[i] != 0)
		توڑو;
	اگر (i == DB_گنتی)
	{
	    dnext = dp->df_اگلا;
	    clear_diffblock(dp);
	    dp = dnext;
	    اگر (dprev == تہی)
		tp->tp_پہلا_diff = dnext;
	    ورنہ
		dprev->df_اگلا = dnext;
	}
	ورنہ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_اگلا;
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
    ساکن diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    اگر (dnew == تہی)
	واپس تہی;
    diff_finger_dp = تہی;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_اگلا = dp;
    اگر (dprev == تہی)
	tp->tp_پہلا_diff = dnew;
    ورنہ
	dprev->df_اگلا = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, حجم(diffline_change_T), 20);
    واپس dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ساکن خالی
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    صحیح		i_org;
    صحیح		i_new;
    صحیح		off_org, off_new;
    char_u	*سطر_org;
    صحیح		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    برائے (i_org = 0; i_org < DB_گنتی; ++i_org)
	اگر (tp->tp_diffbuf[i_org] != تہی)
	    توڑو;
    اگر (i_org == DB_گنتی)	// safety check
	واپس;

    اگر (diff_check_sanity(tp, dp) == FAIL)
	واپس;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    برائے (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	جب (dp->df_گنتی[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    اگر (dir == BACKWARD)
		off_org = dp->df_گنتی[i_org] - 1;
	    سطر_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    اگر (سطر_org == تہی)
		واپس;
	    برائے (i_new = i_org + 1; i_new < DB_گنتی; ++i_new)
	    {
		اگر (tp->tp_diffbuf[i_new] == تہی)
		    جاری;
		اگر (dir == BACKWARD)
		    off_new = dp->df_گنتی[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		اگر (off_new < 0 || off_new >= dp->df_گنتی[i_new])
		    توڑو;
		اگر (diff_cmp(سطر_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    توڑو;
	    }
	    vim_free(سطر_org);

	    // Stop when a line isn't equal in all diff buffers.
	    اگر (i_new != DB_گنتی)
		توڑو;

	    // Line matched in all buffers, remove it from the diff.
	    برائے (i_new = i_org; i_new < DB_گنتی; ++i_new)
		اگر (tp->tp_diffbuf[i_new] != تہی)
		{
		    اگر (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_گنتی[i_new];
		}
	}
	اگر (dir == BACKWARD)
	    توڑو;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ساکن صحیح
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    صحیح		i;

    برائے (i = 0; i < DB_گنتی; ++i)
	اگر (tp->tp_diffbuf[i] != تہی)
	    اگر (dp->df_lnum[i] + dp->df_گنتی[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_سطر_گنتی)
		واپس FAIL;
    واپس OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    خالی
diff_redraw(
    صحیح		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = تہی;
    صحیح		مستعمل_max_fill_other = FALSE;
    صحیح		مستعمل_max_fill_curwin = FALSE;
    صحیح		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	اگر (!wp->w_p_diff || !buf_valid(wp->w_بفر))
	    جاری;

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
	    ورنہ اگر (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		اگر (wp == curwin)
		    مستعمل_max_fill_curwin = TRUE;
		ورنہ اگر (wp_other != تہی)
		    مستعمل_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    اگر (wp_other != تہی && curwin->w_p_scb)
    {
	اگر (مستعمل_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_سیٹ_topline(wp_other, curwin);
	ورنہ اگر (مستعمل_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_سیٹ_topline(curwin, wp_other);
    }
}

    ساکن خالی
clear_diffin(diffin_T *din)
{
    اگر (din->din_fname == تہی)
	VIM_CLEAR(din->din_mmfile.ptr);
    ورنہ
	mch_remove(din->din_fname);
}

    ساکن خالی
clear_diffout(diffout_T *dout)
{
    اگر (dout->dout_fname == تہی)
	ga_clear_لڑیاں(&dout->dout_ga);
    ورنہ
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ساکن صحیح
diff_write_بفر(buf_T *buf, diffin_T *din, linenr_T آغاز, linenr_T انتہا)
{
    linenr_T	lnum;
    char_u	*s;
    طویل	len = 0;
    char_u	*ptr;

    اگر (انتہا < 0)
	انتہا = buf->b_ml.ml_سطر_گنتی;

    اگر (buf->b_ml.ml_flags & ML_خالی_خانے || انتہا < آغاز)
    {
	din->din_mmfile.ptr = تہی;
	din->din_mmfile.سائز = 0;
	واپس OK;
    }

    // xdiff requires one big block of memory with all the text.
    برائے (lnum = آغاز; lnum <= انتہا; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    اگر (ptr == تہی)
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
	واپس FAIL;
    }
    din->din_mmfile.ptr = (حرف *)ptr;
    din->din_mmfile.سائز = len;

    len = 0;
    برائے (lnum = آغاز; lnum <= انتہا; ++lnum)
    {
	برائے (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    اگر (diff_flags & DIFF_ICASE)
	    {
		صحیح c;
		صحیح	orig_len;
		صحیح	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		اگر (*s == NL)
		    c = NUL;
		ورنہ
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
		ورنہ
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
	    ورنہ
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    واپس OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ساکن صحیح
diff_write(buf_T *buf, diffin_T *din, linenr_T آغاز, linenr_T انتہا)
{
    صحیح		r;
    صحیح		save_ml_flags;
    char_u	*save_ff;
    صحیح		save_cmod_flags;

    اگر (din->din_fname == تہی)
	واپس diff_write_بفر(buf, din, آغاز, انتہا);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    اگر (چوکھٹے_locked())
	واپس FAIL;

    اگر (انتہا < 0)
	انتہا = buf->b_ml.ml_سطر_گنتی;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    اگر (انتہا < آغاز)
    {
	// The line range specifies a completely empty file.
	انتہا = آغاز;
	buf->b_ml.ml_flags |= ML_خالی_خانے;
    }
    r = buf_write(buf, din->din_fname, تہی,
			آغاز, انتہا,
			تہی, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_خالی_خانے) | (save_ml_flags & ML_خالی_خانے);
    واپس r;
}

    ساکن صحیح
lnum_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    اگر (lnum1 < lnum2)
	واپس -1;
    اگر (lnum1 > lnum2)
	واپس 1;
    واپس 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ساکن خالی
diff_try_update(
	diffio_T    *dio,
	صحیح	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    صحیح		idx_new;

    اگر (dio->dio_اندرونی)
    {
	ga_init2(&dio->dio_diff.dout_ga, حجم(حرف *), 1000);
    }
    ورنہ
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	اگر (dio->dio_orig.din_fname == تہی
		|| dio->dio_new.din_fname == تہی
		|| dio->dio_diff.dout_fname == تہی)
	    جاؤ theend;
    }

    // Check external diff is actually working.
    اگر (!dio->dio_اندرونی && check_external_diff(dio) == FAIL)
	جاؤ theend;

    // :diffupdate!
    اگر (eap != تہی && eap->forceit)
	برائے (idx_new = idx_orig; idx_new < DB_گنتی; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    اگر (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    صحیح عدد_anchors = زیادہ_سے_زیادہ_عدد;
    linenr_T anchors[DB_گنتی][MAX_DIFF_ANCHORS];
    CLEAR_میدان(anchors);
    اگر (diff_flags & DIFF_ANCHOR)
    {
	برائے (صحیح idx = 0; idx < DB_گنتی; idx++)
	{
	    اگر (curtab->tp_diffbuf[idx] == تہی)
		جاری;
	    صحیح buf_عدد_anchors = 0;
	    اگر (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_عدد_anchors) != OK)
	    {
		emsg(_(e_failed_مقصد_ڈھونڈو_all_diff_anchors));
		عدد_anchors = 0;
		CLEAR_میدان(anchors);
		توڑو;
	    }
	    اگر (buf_عدد_anchors < عدد_anchors)
		عدد_anchors = buf_عدد_anchors;

	    اگر (buf_عدد_anchors > 0)
		qsort((خالی *)anchors[idx],
			(size_t)buf_عدد_anchors,
			حجم(linenr_T),
			lnum_compare);
	}
    }
    اگر (عدد_anchors == زیادہ_سے_زیادہ_عدد)
	عدد_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    برائے (صحیح anchor_i = 0; anchor_i <= عدد_anchors; anchor_i++)
    {
	diff_T	*orig_diff = تہی;
	اگر (anchor_i != 0)
	{
	    orig_diff = curtab->tp_پہلا_diff;
	    curtab->tp_پہلا_diff = تہی;
	}
	linenr_T lnum_آغاز = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_انتہا = (anchor_i == عدد_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	اگر (diff_write(buf, &dio->dio_orig, lnum_آغاز, lnum_انتہا) == FAIL)
	{
	    اگر (orig_diff != تہی)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_پہلا_diff = orig_diff;
		diff_clear(curtab);
	    }
	    جاؤ theend;
	}

	// Make a difference between the first buffer and every other.
	برائے (idx_new = idx_orig + 1; idx_new < DB_گنتی; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    اگر (buf == تہی || buf->b_ml.ml_mfp == تہی)
		جاری; // skip buffer that isn't loaded

	    lnum_آغاز = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_انتہا = anchor_i == عدد_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    اگر (diff_write(buf, &dio->dio_new, lnum_آغاز, lnum_انتہا) == FAIL)
		جاری;
	    اگر (diff_file(dio) == FAIL)
		جاری;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	اگر (anchor_i != 0)
	{
	    diff_finger_dp = تہی;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    برائے (diff_T *dp = curtab->tp_پہلا_diff; dp != تہی; dp = dp->df_اگلا)
	    {
		برائے (صحیح idx = 0; idx < DB_گنتی; idx++)
		{
		    اگر (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    اگر (orig_diff != تہی)
	    {
		diff_T *آخری_diff = orig_diff;
		جب (آخری_diff->df_اگلا != تہی)
		    آخری_diff = آخری_diff->df_اگلا;
		آخری_diff->df_اگلا = curtab->tp_پہلا_diff;
		curtab->tp_پہلا_diff = orig_diff;
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
diff_اندرونی(خالی)
{
    واپس (diff_flags & DIFF_اندرونی) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ساکن صحیح
diff_اندرونی_failed(خالی)
{
    صحیح idx;

    // Only need to do something when there is another buffer.
    برائے (idx = 0; idx < DB_گنتی; ++idx)
	اگر (curtab->tp_diffbuf[idx] != تہی
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    خالی
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    صحیح		idx_orig;
    صحیح		idx_new;
    diffio_T	diffio;
    صحیح		had_diffs = curtab->tp_پہلا_diff != تہی;

    اگر (diff_busy)
    {
	diff_need_update = TRUE;
	واپس;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    برائے (idx_orig = 0; idx_orig < DB_گنتی; ++idx_orig)
	اگر (curtab->tp_diffbuf[idx_orig] != تہی)
	    توڑو;
    اگر (idx_orig == DB_گنتی)
	جاؤ theend;

    // Only need to do something when there is another buffer.
    برائے (idx_new = idx_orig + 1; idx_new < DB_گنتی; ++idx_new)
	اگر (curtab->tp_diffbuf[idx_new] != تہی)
	    توڑو;
    اگر (idx_new == DB_گنتی)
	جاؤ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_میدان(diffio);
    diffio.dio_اندرونی = diff_اندرونی() && !diff_اندرونی_failed();

    diff_try_update(&diffio, idx_orig, eap);
    اگر (diffio.dio_اندرونی && diff_اندرونی_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_میدان(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    اگر (had_diffs || curtab->tp_پہلا_diff != تہی)
    {
	diff_redraw(TRUE);
	کھڑکی_layout_تالا();
	apply_autocmds(EVENT_DIFFUPDATED, تہی, تہی, FALSE, curbuf);
	کھڑکی_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ساکن صحیح
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    صحیح		ok;
    صحیح		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    برائے (;;)
    {
	ok = FALSE;
	fd = mch_fopen((حرف *)diffio->dio_orig.din_fname, "w");
	اگر (fd == تہی)
	    io_error = TRUE;
	ورنہ
	{
	    اگر (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((حرف *)diffio->dio_new.din_fname, "w");
	    اگر (fd == تہی)
		io_error = TRUE;
	    ورنہ
	    {
		اگر (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = تہی;
		اگر (diff_file(diffio) == OK)
		    fd = mch_fopen((حرف *)diffio->dio_diff.dout_fname, "r");
		اگر (fd == تہی)
		    io_error = TRUE;
		ورنہ
		{
		    char_u	linebuf[LBUFLEN];

		    برائے (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			اگر (vim_fgets(linebuf, LBUFLEN, fd))
			    توڑو;
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
	    توڑو;
# endif

# اگر defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	اگر (ok && diff_a_چلتا == MAYBE && diff_bin_چلتا == MAYBE)
	{
	    diff_a_چلتا = TRUE;
	    diff_bin_چلتا = TRUE;
	    جاری;
	}
	اگر (!ok && diff_a_چلتا == TRUE && diff_bin_چلتا == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_چلتا = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	اگر (diff_a_چلتا != MAYBE)
	    توڑو;
	diff_a_چلتا = ok;

	// If "-a" works break here, otherwise retry without "-a".
	اگر (ok)
	    توڑو;
    }
    اگر (!ok)
    {
	اگر (io_error)
	    emsg(_(e_cannot_read_or_write_عارضی_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_چلتا = MAYBE;
# اگر defined(MSWIN)
	diff_bin_چلتا = MAYBE;
# endif
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Invoke the xdiff function.
 */
    ساکن صحیح
diff_file_اندرونی(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_میدان(param);
    CLEAR_میدان(emit_cfg);
    CLEAR_میدان(emit_cb);

    param.flags = diff_algorithm;

    اگر (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    اگر (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    اگر (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    اگر (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_سطریں;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    اگر (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_برآمد_indices;
    ورنہ
	emit_cb.برآمد_سطر = xdiff_برآمد_unified;
    اگر (diffio->dio_orig.din_mmfile.سائز > MAX_XDIFF_سائز ||
	diffio->dio_new.din_mmfile.سائز > MAX_XDIFF_سائز)
    {
	emsg(_(e_problem_creating_اندرونی_diff));
	واپس FAIL;
    }
    اگر (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_اندرونی_diff));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ساکن صحیح
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
	واپس OK;
    }
    ورنہ
# endif
    // Use xdiff for generating the diff.
    اگر (dio->dio_اندرونی)
	واپس diff_file_اندرونی(dio);

    اگر (check_restricted())
	واپس FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    اگر (cmd == تہی)
	واپس FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    اگر (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((حرف *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_چلتا == FALSE ? "" : "-a ",
# اگر defined(MSWIN)
	    diff_bin_چلتا == TRUE ? "--binary " : "",
# ورنہ
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (صحیح)len, p_srr, tmp_diff);
    بلاک_autocmds();	// avoid ShellCmdPost stuff
    (خالی)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    واپس OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    خالی
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = تہی;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = تہی;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = تہی;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = تہی;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = تہی;

# ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, تہی, تہی,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), تہی);
	اگر (browseFile == تہی)
	    واپس;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    اگر (tmp_orig == تہی || tmp_new == تہی)
	جاؤ theend;

    // Write the current buffer to "tmp_orig".
    اگر (buf_write(curbuf, tmp_orig, تہی,
		(linenr_T)1, curbuf->b_ml.ml_سطر_گنتی,
				     تہی, FALSE, FALSE, FALSE, TRUE) == FAIL)
	جاؤ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = بھرا_Name_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != تہی ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    اگر (esc_name == تہی)
	جاؤ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    اگر (buf == تہی)
	جاؤ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    اگر (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((حرف *)dirbuf) != 0)
	dirbuf[0] = NUL;
    ورنہ
    {
#  ifdef TEMPDIRNAMES
	اگر (vim_tempdir != تہی)
	    vim_ignored = mch_chdir((حرف *)vim_tempdir);
	ورنہ
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
		fullname != تہی ? fullname :
#  endif
		eap->arg, tmp_new);
    ورنہ
# endif
    {
	اگر (check_restricted())
	    جاؤ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((حرف *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	بلاک_autocmds();	// Avoid ShellCmdPost stuff
	(خالی)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    اگر (dirbuf[0] != NUL)
    {
	اگر (mch_chdir((حرف *)dirbuf) != 0)
	    emsg(_(e_cannot_go_عقب_مقصد_پچھلا_directory));
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
    اگر (mch_stat((حرف *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    ورنہ
    {
	اگر (curbuf->b_fname != تہی)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    اگر (newname != تہی)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	اگر (win_بانٹو(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_بانٹو;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    اگر (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		اگر (newname != تہی)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    اگر (au_has_گروہ((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    اگر (tmp_orig != تہی)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    اگر (tmp_new != تہی)
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
    خالی
ex_diffsplit(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    bufref_T	old_curbuf;

    سیٹ_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    سیٹ_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    اگر (win_بانٹو(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	واپس;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_بانٹو;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    اگر (curwin == old_curwin)		// split didn't work
	واپس;

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
    scroll_مقصد_fraction(curwin, curwin->w_اونچائی);
}

/*
 * Set options to show diffs for the current window.
 */
    خالی
ex_diffthis(exarg_T *eap غیر_مستعمل)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ساکن خالی
سیٹ_diff_option(win_T *wp, صحیح قدر)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_بفر;
    ++curbuf_تالا;
    سیٹ_option_قدر_give_err((char_u *)"diff", (طویل)قدر, تہی, OPT_LOCAL);
    --curbuf_تالا;
    curwin = old_curwin;
    curbuf = curwin->w_بفر;
}

/*
 * Set options in window "wp" for diff mode.
 */
    خالی
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
    سیٹ_string_option_براہ_راست_اندر_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
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
    بدلا_کھڑکی_setting_win(wp);
# endif
    اگر (vim_strchr(p_sbo, 'h') == تہی)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    سیٹ_diff_option(wp, TRUE);

    اگر (addbuf)
	diff_buf_جوڑو(wp->w_بفر);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    خالی
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
	    سیٹ_diff_option(wp, FALSE);

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
	    بدلا_کھڑکی_setting_win(wp);

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
    اگر (!diffwin && vim_strchr(p_sbo, 'h') != تہی)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ساکن خالی
diff_read(
    صحیح		idx_orig,	// idx of original file
    صحیح		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = تہی;
    صحیح		سطر_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = تہی;
    diff_T	*dp = curtab->tp_پہلا_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*سطر;
    طویل	off;
    صحیح		i;
    صحیح		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = تہی;	    // init to avoid gcc warning

    شمار {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    اگر (dout->dout_fname == تہی)
    {
	diffstyle = DIFF_UNIFIED;
    }
    ورنہ
    {
	fd = mch_fopen((حرف *)dout->dout_fname, "r");
	اگر (fd == تہی)
	{
	    emsg(_(e_cannot_read_diff_output));
	    واپس;
	}
    }

    اگر (!dio->dio_اندرونی)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	اگر (hunk == تہی)
	{
	    اگر (fd != تہی)
		fclose(fd);
	    واپس;
	}
    }

    برائے (;;)
    {
	اگر (dio->dio_اندرونی)
	{
	    اگر (سطر_hunk_idx >= dout->dout_ga.ga_len)
		توڑو;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_ڈیٹا)[سطر_hunk_idx++];
	}
	ورنہ
	{
	    اگر (fd == تہی)
	    {
		اگر (سطر_hunk_idx >= dout->dout_ga.ga_len)
		    توڑو;	    // did last line
		سطر = ((char_u **)dout->dout_ga.ga_ڈیٹا)[سطر_hunk_idx++];
	    }
	    ورنہ
	    {
		اگر (vim_fgets(linebuf, LBUFLEN, fd))
		    توڑو;		// end of file
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
		اگر (محفوظ_isdigit(*سطر))
		    diffstyle = DIFF_ED;
		ورنہ اگر ((STRNCMP(سطر, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ورنہ اگر ((STRNCMP(سطر, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(سطر, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(سطر, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ورنہ
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    جاری;
	    }

	    اگر (diffstyle == DIFF_ED)
	    {
		اگر (!محفوظ_isdigit(*سطر))
		    جاری;	// not the start of a diff block
		اگر (parse_diff_ed(سطر, hunk) == FAIL)
		    جاری;
	    }
	    ورنہ اگر (diffstyle == DIFF_UNIFIED)
	    {
		اگر (STRNCMP(سطر, "@@ ", 3)  != 0)
		    جاری;	// not the start of a diff block
		اگر (parse_diff_unified(سطر, hunk) == FAIL)
		    جاری;
	    }
	    ورنہ
	    {
		emsg(_(e_invalid_diff_format));
		توڑو;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	جب (dp != تہی
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_گنتی[idx_orig])
	{
	    اگر (notset)
		diff_copy_اندراج(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_اگلا;
	    notset = TRUE;
	}

	اگر (dp != تہی
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_گنتی[idx_orig]
		&& hunk->lnum_orig + hunk->گنتی_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    برائے (dpl = dp; dpl->df_اگلا != تہی; dpl = dpl->df_اگلا)
		اگر (hunk->lnum_orig + hunk->گنتی_orig
					     < dpl->df_اگلا->df_lnum[idx_orig])
		    توڑو;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    اگر (off > 0)
	    {
		برائے (i = idx_orig; i < idx_new; ++i)
		    اگر (curtab->tp_diffbuf[i] != تہی)
		    {
			dp->df_lnum[i] -= off;
			dp->df_گنتی[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_گنتی[idx_new] = hunk->گنتی_new;
	    }
	    ورنہ اگر (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_گنتی[idx_new] = hunk->گنتی_new - off;
	    }
	    ورنہ
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		صحیح orig_سائز_اندر_dp = MIN(hunk->گنتی_orig,
			dp->df_lnum[idx_orig] +
			dp->df_گنتی[idx_orig] - hunk->lnum_orig);
		صحیح سائز_diff = hunk->گنتی_new - orig_سائز_اندر_dp;
		dp->df_گنتی[idx_new] += سائز_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->گنتی_new
		    - (dp->df_lnum[idx_new] + dp->df_گنتی[idx_new]);
		اگر (off > 0)
		    dp->df_گنتی[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->گنتی_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_گنتی[idx_orig]);
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
		    dp->df_گنتی[idx_new] += -off;
		}
		off = 0;
	    }
	    برائے (i = idx_orig; i < idx_new; ++i)
		اگر (curtab->tp_diffbuf[i] != تہی)
		    dp->df_گنتی[i] = dpl->df_lnum[i] + dpl->df_گنتی[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_اگلا;
	    dp->df_اگلا = dpl->df_اگلا;
	    جب (dn != dp->df_اگلا)
	    {
		dpl = dn->df_اگلا;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	ورنہ
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    اگر (dp == تہی)
		جاؤ مکمل;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_گنتی[idx_orig] = hunk->گنتی_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_گنتی[idx_new] = hunk->گنتی_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    برائے (i = idx_orig + 1; i < idx_new; ++i)
		اگر (curtab->tp_diffbuf[i] != تہی)
		    diff_copy_اندراج(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    جب (dp != تہی)
    {
	اگر (notset)
	    diff_copy_اندراج(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_اگلا;
	notset = TRUE;
    }

مکمل:
    اگر (!dio->dio_اندرونی)
	vim_free(hunk);

    اگر (fd != تہی)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ساکن خالی
diff_copy_اندراج(
    diff_T	*dprev,
    diff_T	*dp,
    صحیح		idx_orig,
    صحیح		idx_new)
{
    طویل	off;

    اگر (dprev == تہی)
	off = 0;
    ورنہ
	off = (dprev->df_lnum[idx_orig] + dprev->df_گنتی[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_گنتی[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_گنتی[idx_new] = dp->df_گنتی[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    خالی
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *اگلا_p;

    برائے (p = tp->tp_پہلا_diff; p != تہی; p = اگلا_p)
    {
	اگلا_p = p->df_اگلا;
	clear_diffblock(p);
    }
    tp->tp_پہلا_diff = تہی;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ساکن صحیح
diff_linematch(diff_T *dp)
{
    اگر (!(diff_flags & DIFF_LINEMATCH))
	واپس 0;

    // are there more than three diff buffers?
    صحیح tsize = 0;
    برائے (صحیح i = 0; i < DB_گنتی; i++)
    {
	اگر (curtab->tp_diffbuf[i] != تہی)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    اگر (dp->df_گنتی[i] < 0)
		واپس FALSE;
	    tsize += dp->df_گنتی[i];
	}
    }

    // avoid allocating a huge array because it will lag
    واپس tsize <= linematch_سطریں;
}

    ساکن صحیح
get_max_diff_لمبائی(ثابت diff_T *dp)
{
    صحیح maxlength = 0;

    برائے (صحیح k = 0; k < DB_گنتی; k++)
    {
	اگر (curtab->tp_diffbuf[k] != تہی)
	{
	    اگر (dp->df_گنتی[k] > maxlength)
		maxlength = dp->df_گنتی[k];
	}
    }
    واپس maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ساکن خالی
ڈھونڈو_چوٹی_diff_بلاک(
    diff_T	**thistopdiff,
    diff_T	**اگلا_adjacent_بلاکیں,
    صحیح		fromidx,
    صحیح		topline)
{
    diff_T	*topdiff = تہی;
    diff_T	*localtopdiff = تہی;
    صحیح		topdiffchange = 0;

    برائے (topdiff = curtab->tp_پہلا_diff; topdiff != تہی;
						topdiff = topdiff->df_اگلا)
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
		(topdiff->df_lnum[fromidx] + topdiff->df_گنتی[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    اگر ((*thistopdiff) == تہی)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	اگر (!(topdiff->df_اگلا && (topdiff->df_اگلا->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_گنتی[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    اگر (*thistopdiff)
	    {
		(*اگلا_adjacent_بلاکیں) = topdiff->df_اگلا;
		توڑو;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ساکن خالی
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
    diff_T	*thistopdiff = تہی;
    diff_T	*اگلا_adjacent_بلاکیں = تہی;
    صحیح		virtual_سطریں_passed = 0;
    صحیح		curlinenum_مقصد = 1;

    ڈھونڈو_چوٹی_diff_بلاک(&thistopdiff, &اگلا_adjacent_بلاکیں, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    جب (curdif && (curdif->df_lnum[fromidx] + curdif->df_گنتی[fromidx])
							<= from_topline)
    {
	virtual_سطریں_passed += get_max_diff_لمبائی(curdif);

	curdif = curdif->df_اگلا;
    }

    اگر (curdif != اگلا_adjacent_بلاکیں)
	virtual_سطریں_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_سطریں_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    اگر (virtual_سطریں_passed < 0)
	virtual_سطریں_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    اگر (thistopdiff != تہی) // this should not be null, but just for safety
	curlinenum_مقصد = thistopdiff->df_lnum[toidx];

    صحیح virt_سطریں_بایاں = virtual_سطریں_passed;
    curdif = thistopdiff;
    جب (virt_سطریں_بایاں > 0 && curdif != تہی && curdif != اگلا_adjacent_بلاکیں)
    {
	curlinenum_مقصد += MIN(virt_سطریں_بایاں, curdif->df_گنتی[toidx]);
	virt_سطریں_بایاں -= MIN(virt_سطریں_بایاں, get_max_diff_لمبائی(curdif));
	curdif = curdif->df_اگلا;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    صحیح max_virt_سطریں = 0;
    برائے (diff_T *dp = thistopdiff; dp != تہی; dp = dp->df_اگلا)
    {
	اگر (dp->df_lnum[toidx] + dp->df_گنتی[toidx] <= curlinenum_مقصد)
	    max_virt_سطریں += get_max_diff_لمبائی(dp);
	ورنہ
	{
	    اگر (dp->df_lnum[toidx] <= curlinenum_مقصد)
		max_virt_سطریں += curlinenum_مقصد - dp->df_lnum[toidx];
	    توڑو;
	}
    }

    اگر (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_سطریں - virtual_سطریں_passed;
    (*topline) = curlinenum_مقصد;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ساکن خالی
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_لمبائی,
    ثابت صحیح	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    صحیح		سطر_numbers[DB_گنتی];
    صحیح		outputmap[DB_گنتی];
    size_t	ndiffs = 0;

    برائے (صحیح i = 0; i < DB_گنتی; i++)
    {
	اگر (curtab->tp_diffbuf[i] != تہی)
	{
	    سطر_numbers[i] = dp->df_lnum[i];
	    dp->df_گنتی[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    برائے (size_t i = 0; i < decisions_لمبائی; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	اگر (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_اگلا);
	    dp_s->is_linematched = TRUE;
	    برائے (صحیح j = 0; j < DB_گنتی; j++)
	    {
		اگر (curtab->tp_diffbuf[j] != تہی)
		{
		    dp_s->df_lnum[j] = سطر_numbers[j];
		    dp_s->df_گنتی[j] = 0;
		}
	    }
	}
	برائے (size_t j = 0; j < ndiffs; j++)
	{
	    اگر (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_گنتی[outputmap[j]]++;
		سطر_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ساکن خالی
تسلسل_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_گنتی];
    ثابت mmfile_t	*diffbufs[DB_گنتی];
    صحیح			diff_لمبائی[DB_گنتی];
    size_t		ndiffs = 0;

    برائے (صحیح i = 0; i < DB_گنتی; i++)
    {
	اگر (curtab->tp_diffbuf[i] != تہی)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    اگر (dp->df_گنتی[i] > 0)
	    {
		diff_write_بفر(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_گنتی[i] - 1);
	    }
	    ورنہ
	    {
		diffbufs_mm[ndiffs].din_mmfile.سائز = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = تہی;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_لمبائی[ndiffs] = dp->df_گنتی[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    صحیح *decisions = تہی;
    ثابت صحیح iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_لمبائی =
	linematch_nbuffers(diffbufs, diff_لمبائی, ndiffs, &decisions, iwhite);

    برائے (size_t i = 0; i < ndiffs; i++)
	آزاد(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_لمبائی, decisions);

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
    buf_T	*buf = wp->w_بفر;
    صحیح		cmp;

    اگر (linestatus != تہی)
	*linestatus = 0;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    اگر (curtab->tp_پہلا_diff == تہی || !wp->w_p_diff)	// no diffs at all
	واپس 0;

    // safety check: "lnum" must be a buffer line
    اگر (lnum < 1 || lnum > buf->b_ml.ml_سطر_گنتی + 1)
	واپس 0;

    idx = diff_buf_idx(buf);
    اگر (idx == DB_گنتی)
	واپس 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    اگر (hasFoldingWin(wp, lnum, تہی, تہی, TRUE, تہی))
	واپس 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = تہی;		// last block with df_lnum[idx] <= lnum
    برائے (dp = diff_جائزہ_آغاز(curtab, idx, lnum); dp != تہی; dp = dp->df_اگلا)
    {
	اگر (dp->df_lnum[idx] <= lnum)
	    le = dp;
	اگر (lnum <= dp->df_lnum[idx] + dp->df_گنتی[idx])
	    توڑو;
    }
    diff_جائزہ_remember(curtab, idx, lnum, le);
    اگر (dp == تہی || lnum < dp->df_lnum[idx])
	واپس 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    اگر (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	تسلسل_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    صحیح عدد_fill = 0;
    جب (lnum == dp->df_lnum[idx] + dp->df_گنتی[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	اگر (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_لمبائی(dp);
	    عدد_fill += maxcount - dp->df_گنتی[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	اگر (dp->df_اگلا != تہی
		&& lnum >= dp->df_اگلا->df_lnum[idx]
		&& lnum <= dp->df_اگلا->df_lnum[idx] + dp->df_اگلا->df_گنتی[idx])
	    dp = dp->df_اگلا;
	ورنہ
	    توڑو;
    }

    اگر (lnum < dp->df_lnum[idx] + dp->df_گنتی[idx])
    {
	صحیح	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	برائے (i = 0; i < DB_گنتی; ++i)
	    اگر (i != idx && curtab->tp_diffbuf[i] != تہی)
	    {
		اگر (dp->df_گنتی[i] == 0)
		    zero = TRUE;
		ورنہ
		{
		    اگر (dp->df_گنتی[i] != dp->df_گنتی[idx])
		    {
			اگر (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			واپس عدد_fill;
		    }
		    cmp = TRUE;
		}
	    }
	اگر (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    برائے (i = 0; i < DB_گنتی; ++i)
		اگر (i != idx && curtab->tp_diffbuf[i] != تہی
						      && dp->df_گنتی[i] != 0)
		    اگر (!diff_equal_اندراج(dp, idx, i))
		    {
			اگر (linestatus)
			    *linestatus = -1;
			واپس عدد_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	اگر (zero == FALSE)
	    واپس عدد_fill;
	اگر (linestatus)
	    *linestatus = -2;
	واپس عدد_fill;
    }
    واپس عدد_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ساکن صحیح
diff_equal_اندراج(diff_T *dp, صحیح idx1, صحیح idx2)
{
    صحیح		i;
    char_u	*سطر;
    صحیح		cmp;

    اگر (dp->df_گنتی[idx1] != dp->df_گنتی[idx2])
	واپس FALSE;
    اگر (diff_check_sanity(curtab, dp) == FAIL)
	واپس FALSE;
    برائے (i = 0; i < dp->df_گنتی[idx1]; ++i)
    {
	سطر = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	اگر (سطر == تہی)
	    واپس FALSE;
	cmp = diff_cmp(سطر, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(سطر);
	اگر (cmp != 0)
	    واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ساکن صحیح
diff_equal_char(char_u *p1, char_u *p2, صحیح *len)
{
    صحیح l  = (*mb_ptr2len)(p1);

    اگر (l != (*mb_ptr2len)(p2))
	واپس FALSE;
    اگر (l > 1)
    {
	اگر (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    واپس FALSE;
	*len = l;
    }
    ورنہ
    {
	اگر ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    واپس FALSE;
	*len = 1;
    }
    واپس TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ساکن صحیح
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    صحیح		l;

    اگر ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	واپس 0;

    اگر ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	واپس STRCMP(s1, s2);
    اگر ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	واپس MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    جب (*p1 != NUL && *p2 != NUL)
    {
	اگر (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	ورنہ
	{
	    اگر (!diff_equal_char(p1, p2, &l))
		توڑو;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    اگر (*p1 != NUL || *p2 != NUL)
	واپس 1;
    واپس 0;
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
	واپس 0;
    n = diff_check_with_linestatus(wp, lnum, تہی);
    اگر (n <= 0)
	واپس 0;
    واپس n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    خالی
diff_سیٹ_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_بفر;
    linenr_T	lnum;
    صحیح		fromidx;
    صحیح		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    اگر (fromidx == DB_گنتی)
	واپس;		// safety check

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[fromidx] + dp->df_گنتی[fromidx])
	    توڑو;
    اگر (dp == تہی)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_بفر->b_ml.ml_سطر_گنتی
				       - (frombuf->b_ml.ml_سطر_گنتی - lnum);
    }
    ورنہ
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_بفر);
	اگر (toidx == DB_گنتی)
	    واپس;		// safety check

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
    اگر (towin->w_topline > towin->w_بفر->b_ml.ml_سطر_گنتی)
    {
	towin->w_topline = towin->w_بفر->b_ml.ml_سطر_گنتی;
	towin->w_botfill = true;
    }
    اگر (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    بدلا_سطر_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (خالی)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    تہی, TRUE, تہی);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ساکن صحیح
parse_diffanchors(
    صحیح		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    صحیح		*عدد_anchors)
{
    صحیح i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = تہی;
    اگر (check_only)
	bufwin = curwin;
    ورنہ
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    اگر (bufwin->w_بفر == buf && bufwin->w_p_diff)
		توڑو;
	اگر (bufwin == تہی && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    واپس FAIL;
	}
    }

    برائے (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	اگر (*dia == ',') // don't allow empty values
	    واپس FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_پتہ(تہی, &dia, ADDR_سطریں, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	اگر (dia == تہی) // error detected
	    واپس FAIL;
	اگر (*dia != ',' && *dia != NUL)
	    واپس FAIL;

	اگر (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_سطر_گنتی + 1))
	{
	    emsg(_(e_invalid_range));
	    واپس FAIL;
	}

	اگر (anchors != تہی)
	    anchors[i] = lnum;

	اگر (*dia == ',')
	    dia++;
    }
    اگر (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_مزید_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	واپس FAIL;
    }
    اگر (عدد_anchors != تہی)
	*عدد_anchors = i;
    واپس OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    صحیح
diffanchors_بدلا(صحیح buflocal)
{
    صحیح نتیجہ = parse_diffanchors(TRUE, curbuf, تہی, تہی);
    اگر (نتیجہ == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    اگر (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    ورنہ
	    {
		برائے (صحیح idx = 0; idx < DB_گنتی; ++idx)
		    اگر (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			توڑو;
		    }
	    }
	}
    }
    واپس نتیجہ;
}

/*
 * This is called when 'diffopt' is changed.
 */
    صحیح
diffopt_بدلا(خالی)
{
    char_u	*p;
    صحیح		diff_context_new = 6;
    صحیح		linematch_سطریں_new = 0;
    صحیح		diff_flags_new = 0;
    صحیح		diff_foldcolumn_new = 2;
    طویل	diff_algorithm_new = 0;
    طویل	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    جب (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	اگر (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	ورنہ اگر (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	ورنہ اگر (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	ورنہ اگر (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	ورنہ اگر (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	ورنہ اگر (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	ورنہ اگر (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	ورنہ اگر (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	ورنہ اگر (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	ورنہ اگر (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	ورنہ اگر (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	ورنہ اگر (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	ورنہ اگر (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	ورنہ اگر (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	ورنہ اگر (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	ورنہ اگر (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_اندرونی;
	}
	ورنہ اگر (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    اگر (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    ورنہ اگر (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_اقل;
	    }
	    ورنہ اگر (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    ورنہ اگر (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    ورنہ
		واپس FAIL;
	}
	ورنہ اگر (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    اگر (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    ورنہ اگر (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    ورنہ اگر (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    ورنہ اگر (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_لفظ;
	    }
	    ورنہ
		واپس FAIL;
	}
	ورنہ اگر (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_سطریں_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    اگر ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	واپس FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    اگر (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_سطریں = linematch_سطریں_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    واپس OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    صحیح
diffopt_horizontal(خالی)
{
    واپس (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    صحیح
diffopt_hiddenoff(خالی)
{
    واپس (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    صحیح
diffopt_closeoff(خالی)
{
    واپس (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    خالی
diff_update_سطر(linenr_T lnum)
{
    صحیح		idx;
    diff_T	*dp;

    اگر (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	واپس;

    idx = diff_buf_idx(curbuf);
    اگر (idx == DB_گنتی)
	واپس;
    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[idx] + dp->df_گنتی[idx])
	    توڑو;

    // clear the inline change cache as it's invalid
    اگر (dp != تہی)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ساکن diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    صحیح
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    صحیح *change_آغاز,
    صحیح *change_انتہا)
{
    اگر (change->dc_آغاز_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_آغاز = 0;
    ورنہ
	*change_آغاز = change->dc_آغاز[diffline->bufidx];
    اگر (change->dc_انتہا_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_انتہا = زیادہ_سے_زیادہ_عدد;
    ورنہ
	*change_انتہا = change->dc_انتہا[diffline->bufidx];

    اگر (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	واپس FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    برائے (صحیح i = 0; i < DB_گنتی; i++)
    {
	اگر (i == diffline->bufidx)
	    جاری;
	اگر (change->dc_آغاز[i] != change->dc_انتہا[i]
		|| change->dc_انتہا_lnum_off[i] != change->dc_آغاز_lnum_off[i])
	{
	    واپس FALSE;
	}
    }
    واپس TRUE;
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
    ساکن صحیح
diff_ڈھونڈو_change_simple(
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
	سطر_org = تہی;
    }
    ورنہ
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	سطر_org = vim_strsave(ml_get_buf(wp->w_بفر, lnum, FALSE));
	اگر (سطر_org == تہی)
	    واپس FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    برائے (i = 0; i < DB_گنتی; ++i)
	اگر (curtab->tp_diffbuf[i] != تہی && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    اگر (off >= dp->df_گنتی[i])
		جاری;
	    added = FALSE;
	    اگر (diff_flags & DIFF_INLINE_NONE)
		توڑو; // early terminate as we only care about the return value

	    سطر_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    جب (سطر_org[si_org] != NUL)
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
		ورنہ
		{
		    اگر (!diff_equal_char(سطر_org + si_org, سطر_new + si_new,
									   &l))
			توڑو;
		    si_org += l;
		    si_new += l;
		}
	    }
	    اگر (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_سر_off)(سطر_org, سطر_org + si_org);
		si_new -= (*mb_سر_off)(سطر_new, سطر_new + si_new);
	    }
	    اگر (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    اگر (سطر_org[si_org] != NUL || سطر_new[si_new] != NUL)
	    {
		ei_org = (صحیح)STRLEN(سطر_org);
		ei_new = (صحیح)STRLEN(سطر_new);
		جب (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    اگر (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(سطر_org[ei_org])
					      && VIM_ISWHITE(سطر_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(سطر_org[ei_org])
					    || VIM_ISWHITE(سطر_new[ei_new]))))
		    {
			جب (ei_org >= *startp
					     && VIM_ISWHITE(سطر_org[ei_org]))
			    --ei_org;
			جب (ei_new >= si_new
					     && VIM_ISWHITE(سطر_new[ei_new]))
			    --ei_new;
		    }
		    ورنہ
		    {
			p1 = سطر_org + ei_org;
			p2 = سطر_new + ei_new;
			p1 -= (*mb_سر_off)(سطر_org, p1);
			p2 -= (*mb_سر_off)(سطر_new, p2);
			اگر (!diff_equal_char(p1, p2, &l))
			    توڑو;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		اگر (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(سطر_org);
    واپس added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
قسمتعریف ساخت
{
    طویل byte_آغاز;
    طویل عدد_بائٹیں;
    صحیح lineoff;
} linemap_اندراج_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ساکن خالی
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, صحیح idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    صحیح pass = 1;
    کرو
    {
	صحیح has_unmerged_gaps = FALSE;
	صحیح has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	جب (dp!= تہی && dp->df_اگلا != تہی)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    اگر (dp->df_lnum[idx1] + dp->df_گنتی[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_اگلا->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_اگلا;
		جاری;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_اندراج_T *entry1 = &((linemap_اندراج_T*)linemap[idx1].ga_ڈیٹا)[dp->df_lnum[idx1] + dp->df_گنتی[idx1] - 1];
	    linemap_اندراج_T *entry2 = &((linemap_اندراج_T*)linemap[idx1].ga_ڈیٹا)[dp->df_اگلا->df_lnum[idx1] - 1];
	    اگر (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_اگلا;
		جاری;
	    }

	    linenr_T gap = dp->df_اگلا->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_گنتی[idx1]);
	    اگر (gap <= 3)
	    {
		linenr_T max_df_گنتی = 0;
		برائے (صحیح i = 0; i < DB_گنتی; i++)
		    max_df_گنتی = MAX(max_df_گنتی, dp->df_گنتی[i] + dp->df_اگلا->df_گنتی[i]);

		اگر (max_df_گنتی >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    برائے (صحیح i = 0; i < DB_گنتی; i++)
		    {
			dp->df_گنتی[i] = dp->df_اگلا->df_lnum[i]
			    + dp->df_اگلا->df_گنتی[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_اگلا = dp->df_اگلا;
		    dp->df_اگلا = dp_اگلا->df_اگلا;
		    clear_diffblock(dp_اگلا);
		    has_merged_gaps = TRUE;
		    جاری;
		}
		ورنہ
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_اگلا;
	}
	اگر (!has_unmerged_gaps || !has_merged_gaps)
	    توڑو;
    } جب (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ساکن خالی
diff_refine_inline_لفظ_highlight(diff_T *dp_orig, garray_T *linemap, صحیح idx1,
				  linenr_T آغاز_lnum)
{
    صحیح pass = 1;
    کرو
    {
	diff_T *dp = dp_orig;

	جب (dp != تہی && dp->df_اگلا != تہی)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_گنتی[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_اگلا->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    اگر (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_اگلا;
		جاری;
	    }

	    linemap_اندراج_T *entry1 =
		&((linemap_اندراج_T *)linemap[idx1].ga_ڈیٹا)[idx_entry1];
	    linemap_اندراج_T *entry2 =
		&((linemap_اندراج_T *)linemap[idx1].ga_ڈیٹا)[idx_entry2];

	    // Skip if blocks are on different lines
	    اگر (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_اگلا;
		جاری;
	    }

	    // Calculate the gap between blocks
	    صحیح gap_آغاز = entry1->byte_آغاز + entry1->عدد_بائٹیں;
	    صحیح gap_انتہا = entry2->byte_آغاز;
	    صحیح gap_سائز = gap_انتہا - gap_آغاز;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    اگر (gap_سائز <= 0 || gap_سائز > diff_لفظ_gap)
	    {
		dp = dp->df_اگلا;
		جاری;
	    }

	    // Get the text between the two blocks
	    char_u *سطر = ml_get_buf(curtab->tp_diffbuf[idx1],
		    آغاز_lnum + entry1->lineoff, FALSE);
	    char_u *gap_متن = سطر + gap_آغاز;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_لفظ = true;
	    bool has_content = false;

	    برائے (صحیح i = 0; i < gap_سائز && gap_متن[i] != NUL; i++)
	    {
		has_content = true;
		صحیح char_class = mb_get_class_buf(gap_متن + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		اگر (char_class == 2)
		{
		    only_non_لفظ = false;
		    توڑو;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    اگر (has_content && only_non_لفظ)
	    {
		طویل کل_change_بائٹیں = 0;
		برائے (صحیح i = 0; i < DB_گنتی; i++)
		{
		    اگر (curtab->tp_diffbuf[i] != تہی)
		    {
			// count bytes in the first block
			برائے (صحیح k = 0; k < dp->df_گنتی[i]; k++)
			{
			    صحیح idx = dp->df_lnum[i] + k - 1;
			    اگر (idx < linemap[i].ga_len)
				کل_change_بائٹیں +=
				    ((linemap_اندراج_T *)linemap[i].ga_ڈیٹا)[idx].عدد_بائٹیں;
			}
			// count bytes in the next block
			برائے (صحیح k = 0; k < dp->df_اگلا->df_گنتی[i]; k++)
			{
			    صحیح idx = dp->df_اگلا->df_lnum[i] + k - 1;
			    اگر (idx < linemap[i].ga_len)
				کل_change_بائٹیں +=
				    ((linemap_اندراج_T *)linemap[i].ga_ڈیٹا)[idx].عدد_بائٹیں;
			}
		    }
		}

		اگر (کل_change_بائٹیں >= gap_سائز * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    برائے (صحیح i = 0; i < DB_گنتی; i++)
		    {
			اگر (curtab->tp_diffbuf[i] != تہی)
			{
			    dp->df_گنتی[i] = dp->df_اگلا->df_lnum[i]
				+ dp->df_اگلا->df_گنتی[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_اگلا = dp->df_اگلا;
		    dp->df_اگلا = dp_اگلا->df_اگلا;
		    clear_diffblock(dp_اگلا);

		    // Don't advance dp, check if can merge with the next block too
		    جاری;
		}
	    }

	    dp = dp->df_اگلا;
	}
    } جب (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ساکن خالی
diff_ڈھونڈو_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_گنتی];
    garray_T	file1_str;
    garray_T	file2_str;
    صحیح		file1_idx = -1;

    طویل	save_diff_algorithm = diff_algorithm;

    CLEAR_میدان(dio);
    ga_init2(&dio.dio_diff.dout_ga, حجم(حرف *), 1000);

    // inline diff only supports internal algo
    dio.dio_اندرونی = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_پہلا_diff;
    curtab->tp_پہلا_diff = تہی;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    //بفر
    buf_T	*(orig_diffbuf[DB_گنتی]);
    حافظہ_نقل(orig_diffbuf, curtab->tp_diffbuf, حجم(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    برائے (صحیح i = 0; i < DB_گنتی; i++)
	ga_init2(&linemap[i], حجم(linemap_اندراج_T), 128);

    برائے (صحیح i = 0; i < DB_گنتی; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	اگر (buf == تہی || buf->b_ml.ml_mfp == تہی)
	    جاری; // skip buffer that isn't loaded

	اگر (dp->df_گنتی[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = تہی;
	    جاری;
	}

	اگر (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	برائے (صحیح off = 0; off < dp->df_گنتی[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    صحیح اندر_keyword = FALSE;

	    // iwhiteeol support vars
	    صحیح آخری_white = FALSE;
	    صحیح eol_ga_len = -1;
	    صحیح eol_linemap_len = -1;
	    صحیح eol_numlines = -1;

	    char_u *s;
	    برائے (s = curline; *s != NUL;)
	    {
		صحیح new_اندر_keyword = FALSE;
		اگر (diff_flags & DIFF_INLINE_لفظ)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_اندر_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		اگر (اندر_keyword && !new_اندر_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		اگر (VIM_ISWHITE(*s))
		{
		    اگر (diff_flags & DIFF_IWHITEALL)
		    {
			اندر_keyword = FALSE;
			s = skipwhite(s);
			جاری;
		    }
		    ورنہ اگر ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			اگر (!آخری_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    آخری_white = TRUE;
			}
		    }
		}
		ورنہ
		{
		    اگر ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			آخری_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		صحیح char_len = 1;
		اگر (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		ورنہ
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
		    ورنہ
			ga_concat_len(curstr, s, char_len);
		}

		اگر (!new_اندر_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		اگر (!new_اندر_keyword || (new_اندر_keyword && !اندر_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_اندراج_T linemap_اندراج;
		    linemap_اندراج.lineoff = off;
		    linemap_اندراج.byte_آغاز = s - curline;
		    linemap_اندراج.عدد_بائٹیں = char_len;
		    اگر (ga_grow(&linemap[i], 1) != OK)
			جاؤ مکمل;
		    ((linemap_اندراج_T*)(linemap[i].ga_ڈیٹا))[linemap[i].ga_len]
			= linemap_اندراج;
		    linemap[i].ga_len += 1;
		}
		ورنہ
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[linemap[i].ga_len-1].عدد_بائٹیں
			+= char_len;
		}

		اندر_keyword = new_اندر_keyword;
		s += char_len;
	    }
	    اگر (اندر_keyword)
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

		linemap_اندراج_T linemap_اندراج;
		linemap_اندراج.lineoff = off;
		linemap_اندراج.byte_آغاز = s - curline;
		linemap_اندراج.عدد_بائٹیں = حجم(NL);
		اگر (ga_grow(&linemap[i], 1) != OK)
		    جاؤ مکمل;
		((linemap_اندراج_T*)(linemap[i].ga_ڈیٹا))[linemap[i].ga_len]
		    = linemap_اندراج;
		linemap[i].ga_len += 1;
	    }
	}

	اگر (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (حرف *)curstr->ga_ڈیٹا;
	    dio.dio_new.din_mmfile.سائز = curstr->ga_len;
	}
	ورنہ
	{
	    dio.dio_orig.din_mmfile.ptr = (حرف *)curstr->ga_ڈیٹا;
	    dio.dio_orig.din_mmfile.سائز = curstr->ga_len;
	}
	اگر (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    صحیح diff_حالت = diff_file_اندرونی(&dio);
	    اگر (diff_حالت == FAIL)
		جاؤ مکمل;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_پہلا_diff;

    اگر (diff_flags & DIFF_INLINE_لفظ && file1_idx != -1)
	diff_refine_inline_لفظ_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    ورنہ اگر (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    برائے (; new_diff != تہی; new_diff = new_diff->df_اگلا)
    {
	diffline_change_T change;
	CLEAR_میدان(change);
	برائے (صحیح i = 0; i < DB_گنتی; i++)
	{
	    اگر (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		جاری;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_انتہا = diff_lnum + new_diff->df_گنتی[i];

	    اگر (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_آغاز[i] = MAXCOL;
		change.dc_آغاز_lnum_off[i] = زیادہ_سے_زیادہ_عدد;
	    }
	    ورنہ
	    {
		change.dc_آغاز[i] = ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[diff_lnum].byte_آغاز;
		change.dc_آغاز_lnum_off[i] = ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[diff_lnum].lineoff;
	    }

	    اگر (diff_lnum == diff_lnum_انتہا)
	    {
		change.dc_انتہا[i] = change.dc_آغاز[i];
		change.dc_انتہا_lnum_off[i] = change.dc_آغاز_lnum_off[i];
	    }
	    ورنہ اگر (diff_lnum_انتہا - 1 >= linemap[i].ga_len)
	    {
		change.dc_انتہا[i] = MAXCOL;
		change.dc_انتہا_lnum_off[i] = زیادہ_سے_زیادہ_عدد;
	    }
	    ورنہ
	    {
		change.dc_انتہا[i] = ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[diff_lnum_انتہا-1].byte_آغاز +
		    ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[diff_lnum_انتہا-1].عدد_بائٹیں;
		change.dc_انتہا_lnum_off[i] = ((linemap_اندراج_T*)linemap[i].ga_ڈیٹا)[diff_lnum_انتہا-1].lineoff;
	    }
	}
	اگر (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    جاؤ مکمل;
	}
	((diffline_change_T*)(dp->df_changes.ga_ڈیٹا))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

مکمل:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_پہلا_diff = orig_diff;
    حافظہ_نقل(curtab->tp_diffbuf, orig_diffbuf, حجم(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    برائے (صحیح i = 0; i < DB_گنتی; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    صحیح
diff_ڈھونڈو_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    صحیح		idx;
    صحیح		off;

    idx = diff_buf_idx(wp->w_بفر);
    اگر (idx == DB_گنتی)	// cannot happen
	واپس FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
	اگر (lnum < dp->df_lnum[idx] + dp->df_گنتی[idx])
	    توڑو;
    اگر (dp == تہی || diff_check_sanity(curtab, dp) == FAIL)
	واپس FALSE;

    اگر (lnum - dp->df_lnum[idx] > زیادہ_سے_زیادہ_عدد)
	// Integer overflow protection
	واپس FALSE;
    off = lnum - dp->df_lnum[idx];

    اگر (!(diff_flags & ALL_INLINE_DIFF) || diff_اندرونی_failed())
    {
	// Use simple algorithm
	صحیح	change_آغاز = MAXCOL;	// first col of changed area
	صحیح	change_انتہا = -1;	// last col of changed area
	صحیح	ret;

	ret = diff_ڈھونڈو_change_simple(wp, lnum, dp, idx, &change_آغاز, &change_انتہا);

	// convert from inclusive end to exclusive end per diffline's contract
	change_انتہا += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_میدان(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->عدد_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_آغاز[idx] = change_آغاز;
	simple_diffline_change.dc_انتہا[idx] = change_انتہا;
	simple_diffline_change.dc_آغاز_lnum_off[idx] = off;
	simple_diffline_change.dc_انتہا_lnum_off[idx] = off;
	واپس ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    اگر (!dp->has_changes)
	diff_ڈھونڈو_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    صحیح عدد_changes = 0;
    صحیح change_idx = 0;
    diffline->changes = تہی;
    برائے (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ڈیٹا)[change_idx];
	اگر (change->dc_انتہا_lnum_off[idx] < off)
	    جاری;
	اگر (change->dc_آغاز_lnum_off[idx] > off)
	    توڑو;
	اگر (diffline->changes == تہی)
	    diffline->changes = change;
	عدد_changes++;
    }
    diffline->عدد_changes = عدد_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    صحیح added = FALSE;
    اگر (عدد_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	برائے (صحیح i = 0; i < DB_گنتی; i++)
	{
	    اگر (idx == i)
		جاری;
	    اگر (curtab->tp_diffbuf[i] == تہی)
		جاری;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_ڈیٹا)[dp->df_changes.ga_len-1];
	    اگر (change->dc_آغاز_lnum_off[i] != زیادہ_سے_زیادہ_عدد)
	    {
		added = FALSE;
		توڑو;
	    }
	}
    }
    واپس added;
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
	واپس FALSE;

    برائے (i = 0; i < DB_گنتی; ++i)
    {
	اگر (curtab->tp_diffbuf[i] == wp->w_بفر)
	    idx = i;
	ورنہ اگر (curtab->tp_diffbuf[i] != تہی)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    اگر (idx == -1 || !other)
	واپس FALSE;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    اگر (curtab->tp_پہلا_diff == تہی)
	واپس TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = تہی;		// last block with df_lnum[idx] <= lnum
    صحیح		نتیجہ = TRUE;
    برائے (dp = diff_جائزہ_آغاز(curtab, idx, lnum); dp != تہی; dp = dp->df_اگلا)
    {
	اگر (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	اگر (dp->df_lnum[idx] - diff_context > lnum)
	    توڑو;
	// If this change ends before the line we have a match.
	اگر (dp->df_lnum[idx] + dp->df_گنتی[idx] + diff_context > lnum)
	{
	    نتیجہ = FALSE;
	    توڑو;
	}
    }
    diff_جائزہ_remember(curtab, idx, lnum, le);
    واپس نتیجہ;
}
# endif

/*
 * "dp" and "do" commands.
 */
    خالی
nv_diffgetput(صحیح put, طویل گنتی)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_کام_CHANNEL
    اگر (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	واپس;
    }
# endif
    اگر (گنتی == 0)
	ea.arg = (char_u *)"";
    ورنہ
    {
	vim_snprintf((حرف *)buf, 30, "%ld", گنتی);
	ea.arg = buf;
    }
    اگر (put)
	ea.cmdidx = CMD_diffput;
    ورنہ
	ea.cmdidx = CMD_diffget;
    ea.addr_گنتی = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ساکن صحیح
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
	اگر (dp == diff)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    خالی
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    صحیح		گنتی;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    صحیح		idx_cur;
    صحیح		idx_other;
    صحیح		idx_from;
    صحیح		idx_مقصد;
    صحیح		i;
    صحیح		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    صحیح		آغاز_skip, انتہا_skip;
    صحیح		new_گنتی;
    صحیح		buf_خالی_خانے;
    صحیح		ملا_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    اگر (idx_cur == DB_گنتی)
    {
	emsg(_(e_current_بفر_is_not_اندر_diff_منوال));
	واپس;
    }

    اگر (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	برائے (idx_other = 0; idx_other < DB_گنتی; ++idx_other)
	    اگر (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != تہی)
	    {
		اگر (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    توڑو;
		ملا_not_ma = TRUE;
	    }
	اگر (idx_other == DB_گنتی)
	{
	    اگر (ملا_not_ma)
		emsg(_(e_no_other_بفر_اندر_diff_منوال_is_modifiable));
	    ورنہ
		emsg(_(e_no_other_بفر_اندر_diff_منوال));
	    واپس;
	}

	// Check that there isn't a third buffer in the list
	برائے (i = idx_other + 1; i < DB_گنتی; ++i)
	    اگر (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != تہی
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_مزید_than_two_buffers_اندر_diff_منوال_dont_know_which_one_مقصد_use));
		واپس;
	    }
    }
    ورنہ
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	جب (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	برائے (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	اگر (eap->arg + i == p)	    // digits only
	    i = atol((حرف *)eap->arg);
	ورنہ
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    اگر (i < 0)
		واپس;		// error message already given
	}
	buf = buflist_findnr(i);
	اگر (buf == تہی)
	{
	    semsg(_(e_cant_ڈھونڈو_بفر_str), eap->arg);
	    واپس;
	}
	اگر (buf == curbuf)
	    واپس;		// nothing to do
	idx_other = diff_buf_idx(buf);
	اگر (idx_other == DB_گنتی)
	{
	    semsg(_(e_بفر_str_is_not_اندر_diff_منوال), eap->arg);
	    واپس;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    اگر (eap->addr_گنتی == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	صحیح linestatus = 0;
	اگر (eap->line1 == curbuf->b_ml.ml_سطر_گنتی
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	ورنہ اگر (eap->line1 > 0)
	    --eap->line1;
    }

    اگر (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_مقصد = idx_cur;
    }
    ورنہ
    {
	idx_from = idx_cur;
	idx_مقصد = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	اگر (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    جاؤ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    اگر (!curbuf->b_بدلا)
    {
	change_warning(0);
	اگر (diff_buf_idx(curbuf) != idx_مقصد)
	{
	    emsg(_(e_بفر_بدلا_unexpectedly));
	    جاؤ theend;
	}
    }

    dprev = تہی;
    برائے (dp = curtab->tp_پہلا_diff; dp != تہی; )
    {
	اگر (!eap->addr_گنتی)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    جب (dp->df_اگلا
		    && dp->df_اگلا->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_گنتی[idx_cur]
		    && dp->df_اگلا->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_اگلا;
	    }
	}
	اگر (dp->df_lnum[idx_cur] > eap->line2 + off)
	    توڑو;	// past the range that was specified

	dfree = تہی;
	lnum = dp->df_lnum[idx_مقصد];
	گنتی = dp->df_گنتی[idx_مقصد];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + گنتی + (گنتی == 0 && BUFEMPTY() ? 1 : 0);

	اگر (dp->df_lnum[idx_cur] + dp->df_گنتی[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    آغاز_skip = 0;
	    انتہا_skip = 0;
	    اگر (eap->addr_گنتی > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		آغاز_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		اگر (آغاز_skip > 0)
		{
		    // range starts below start of current diff block
		    اگر (آغاز_skip > گنتی)
		    {
			lnum += گنتی;
			گنتی = 0;
		    }
		    ورنہ
		    {
			گنتی -= آغاز_skip;
			lnum += آغاز_skip;
		    }
		}
		ورنہ
		    آغاز_skip = 0;

		انتہا_skip = dp->df_lnum[idx_cur] + dp->df_گنتی[idx_cur] - 1
							 - (eap->line2 + off);
		اگر (انتہا_skip > 0)
		{
		    // range ends above end of current/from diff block
		    اگر (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_گنتی[idx_cur] - آغاز_skip - انتہا_skip;
			اگر (گنتی > i)
			    گنتی = i;
		    }
		    ورنہ			// :diffget
		    {
			گنتی -= انتہا_skip;
			انتہا_skip = dp->df_گنتی[idx_from] - آغاز_skip - گنتی;
			اگر (انتہا_skip < 0)
			    انتہا_skip = 0;
		    }
		}
		ورنہ
		    انتہا_skip = 0;
	    }

	    buf_خالی_خانے = BUFEMPTY();
	    added = 0;
	    برائے (i = 0; i < گنتی; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_خالی_خانے = curbuf->b_ml.ml_سطر_گنتی == 1;
		اگر (ml_delete(lnum) == OK)
		    --added;
	    }
	    برائے (i = 0; i < dp->df_گنتی[idx_from] - آغاز_skip - انتہا_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + آغاز_skip + i;
		اگر (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_سطر_گنتی)
		    توڑو;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		اگر (p != تہی)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    اگر (buf_خالی_خانے && curbuf->b_ml.ml_سطر_گنتی == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_خالی_خانے = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_گنتی = dp->df_گنتی[idx_مقصد] + added;
	    dp->df_گنتی[idx_مقصد] = new_گنتی;

	    اگر (آغاز_skip == 0 && انتہا_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		برائے (i = 0; i < DB_گنتی; ++i)
		    اگر (curtab->tp_diffbuf[i] != تہی && i != idx_from
								&& i != idx_مقصد
			    && !diff_equal_اندراج(dp, idx_from, i))
			توڑو;
		اگر (i == DB_گنتی)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_اگلا;
		    اگر (dprev == تہی)
			curtab->tp_پہلا_diff = dp;
		    ورنہ
			dprev->df_اگلا = dp;
		}
	    }

	    اگر (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + گنتی - 1, (طویل)MAXLNUM, (طویل)added);
		اگر (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    اگر (curwin->w_cursor.lnum >= lnum + گنتی)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_سطر_گنتی);
		    }
		    ورنہ اگر (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    بدلا_سطریں(lnum, 0, lnum + گنتی, (طویل)added);

	    اگر (dfree != تہی)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_مقصد);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    اگر (added != 0 && !valid_diff(dp))
		توڑو;

	    اگر (dfree == تہی)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_گنتی[idx_مقصد] = new_گنتی;

	    // When changing the current buffer, keep track of line numbers
	    اگر (idx_cur == idx_مقصد)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	اگر (dfree == تہی)
	{
	    dprev = dp;
	    dp = dp->df_اگلا;
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
	ex_diffupdate(تہی);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    بدلا_سطر_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    اگر (curtab->tp_پہلا_diff == تہی)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_اندر_TAB(curtab, wp)
	    اگر (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    اگر (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    ورنہ
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	کھڑکی_layout_تالا();
	apply_autocmds(EVENT_DIFFUPDATED, تہی, تہی, FALSE, curbuf);
	کھڑکی_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ساکن خالی
diff_fold_update(diff_T *dp, صحیح skip_idx)
{
    صحیح		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	برائے (i = 0; i < DB_گنتی; ++i)
	    اگر (curtab->tp_diffbuf[i] == wp->w_بفر && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_گنتی[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    صحیح
diff_منوال_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	اگر (diff_buf_idx_tp(buf, tp) != DB_گنتی)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    صحیح
diff_move_مقصد(صحیح dir, طویل گنتی)
{
    صحیح		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    اگر (idx == DB_گنتی || curtab->tp_پہلا_diff == تہی)
	واپس FAIL;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    اگر (curtab->tp_پہلا_diff == تہی)		// no diffs today
	واپس FAIL;

    جب (--گنتی >= 0)
    {
	// Check if already before first diff.
	اگر (dir == BACKWARD && lnum <= curtab->tp_پہلا_diff->df_lnum[idx])
	    توڑو;

	برائے (dp = curtab->tp_پہلا_diff; ; dp = dp->df_اگلا)
	{
	    اگر (dp == تہی)
		توڑو;
	    اگر ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_اگلا == تہی
			    || lnum <= dp->df_اگلا->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		توڑو;
	    }
	}
    }

    // don't end up past the end of the file
    اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	lnum = curbuf->b_ml.ml_سطر_گنتی;

    // When the cursor didn't move at all we fail.
    اگر (lnum == curwin->w_cursor.lnum)
	واپس FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;

    واپس OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ساکن linenr_T
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
    اگر (idx1 == DB_گنتی || idx2 == DB_گنتی || curtab->tp_پہلا_diff == تہی)
	واپس lnum1;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    اگر (curtab->tp_پہلا_diff == تہی)		// no diffs today
	واپس lnum1;

    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
    {
	اگر (dp->df_lnum[idx1] > lnum1)
	    واپس lnum1 - baseline;
	اگر ((dp->df_lnum[idx1] + dp->df_گنتی[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    اگر (baseline > dp->df_گنتی[idx2])
		baseline = dp->df_گنتی[idx2];

	    واپس dp->df_lnum[idx2] + baseline;
	}
	اگر (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_گنتی[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_گنتی[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    واپس curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_گنتی[idx1])
				   - (dp->df_lnum[idx2] + dp->df_گنتی[idx2]);
    }

    // If we get here then the cursor is after the last diff
    واپس lnum1 - baseline;
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
    اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	واپس curbuf->b_ml.ml_سطر_گنتی;
    واپس lnum;
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
    اگر (idx == DB_گنتی)		// safety check
	واپس (linenr_T)0;

    اگر (curtab->tp_diff_invalid)
	ex_diffupdate(تہی);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_اندر_TAB(curtab, dp)
	اگر (lnum <= dp->df_lnum[idx] + dp->df_گنتی[idx])
	    توڑو;

    // When after the last change, compute relative to the last line number.
    اگر (dp == تہی)
	واپس wp->w_بفر->b_ml.ml_سطر_گنتی
					- (curbuf->b_ml.ml_سطر_گنتی - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_بفر);
    اگر (i == DB_گنتی)			// safety check
	واپس (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    اگر (n > dp->df_lnum[i] + dp->df_گنتی[i])
	n = dp->df_lnum[i] + dp->df_گنتی[i];
    واپس n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ساکن صحیح
parse_diff_ed(
	char_u	    *سطر,
	diffhunk_T  *hunk)
{
    char_u *p;
    طویل    f1, l1, f2, l2;
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
    ورنہ
	l1 = f1;
    اگر (*p != 'a' && *p != 'c' && *p != 'd')
	واپس FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    اگر (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    ورنہ
	l2 = f2;
    اگر (l1 < f1 || l2 < f2)
	واپس FAIL;

    اگر (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->گنتی_orig = 0;
    }
    ورنہ
    {
	hunk->lnum_orig = f1;
	hunk->گنتی_orig = l1 - f1 + 1;
    }
    اگر (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->گنتی_new = 0;
    }
    ورنہ
    {
	hunk->lnum_new = f2;
	hunk->گنتی_new = l2 - f2 + 1;
    }
    واپس OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ساکن صحیح
parse_diff_unified(
	char_u	    *سطر,
	diffhunk_T  *hunk)
{
    char_u *p;
    طویل    oldline, oldcount, newline, newcount;

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
	ورنہ
	    oldcount = 1;
	اگر (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    اگر (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    ورنہ
		newcount = 1;
	}
	ورنہ
	    واپس FAIL;	// invalid diff format

	اگر (oldcount == 0)
	    oldline += 1;
	اگر (newcount == 0)
	    newline += 1;
	اگر (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->گنتی_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->گنتی_new = newcount;

	واپس OK;
    }

    واپس FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ساکن صحیح
xdiff_برآمد_indices(
	طویل آغاز_a,
	طویل گنتی_a,
	طویل آغاز_b,
	طویل گنتی_b,
	خالی *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    اگر (p == تہی)
	واپس -1;

    اگر (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	واپس -1;
    }

    p->lnum_orig  = آغاز_a + 1;
    p->گنتی_orig = گنتی_a;
    p->lnum_new   = آغاز_b + 1;
    p->گنتی_new  = گنتی_b;
    ((diffhunk_T **)dout->dout_ga.ga_ڈیٹا)[dout->dout_ga.ga_len++] = p;
    واپس 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ساکن صحیح
xdiff_برآمد_unified(
	خالی *priv,
	mmbuffer_t *mb,
	صحیح nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    صحیح		i;

    برائے (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].سائز);

    واپس 0;
}

#اگر_ختم	// FEAT_DIFF

#اگر defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    خالی
f_diff_filler(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_DIFF
    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    خالی
f_diff_hlID(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ساکن linenr_T	پچھلا_lnum = 0;
    ساکن varnumber_T	changedtick = 0;
    ساکن صحیح		fnum = 0;
    ساکن صحیح		پچھلا_diff_flags = 0;
    ساکن صحیح		change_آغاز = 0;
    ساکن صحیح		change_انتہا = 0;
    ساکن hlf_T	hlID = (hlf_T)0;
    صحیح			cache_results = TRUE;
    صحیح			ستون;
    diffline_T		diffline;

    CLEAR_میدان(diffline);

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

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
	    || lnum != پچھلا_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != پچھلا_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	صحیح linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	اگر (linestatus < 0)
	{
	    اگر (linestatus == -1)
	    {
		change_آغاز = MAXCOL;
		change_انتہا = -1;
		اگر (diff_ڈھونڈو_change(curwin, lnum, &diffline))
		    hlID = HLF_جوڑو;	// added line
		ورنہ
		{
		    hlID = HLF_CHD;	// changed line
		    اگر (diffline.عدد_changes > 0 && cache_results)
		    {
			change_آغاز = diffline.changes[0].dc_آغاز[diffline.bufidx];
			change_انتہا = diffline.changes[0].dc_انتہا[diffline.bufidx];
		    }
		}
	    }
	    ورنہ
		hlID = HLF_جوڑو;	// added line
	}
	ورنہ
	    hlID = (hlf_T)0;

	اگر (cache_results)
	{
	    پچھلا_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    پچھلا_diff_flags = diff_flags;
	}
    }

    اگر (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	ستون = tv_get_نمبر(&argvars[1]) - 1; // ignore type error in {col}
	اگر (cache_results)
	{
	    اگر (ستون >= change_آغاز && ستون < change_انتہا)
		hlID = HLF_TXD;			// changed text
	    ورنہ
		hlID = HLF_CHD;			// changed line
	}
	ورنہ
	{
	    hlID = HLF_CHD;
	    برائے (صحیح i = 0; i < diffline.عدد_changes; i++)
	    {
		صحیح added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_آغاز, &change_انتہا);
		اگر (ستون >= change_آغاز && ستون < change_انتہا)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    توڑو;
		}
		اگر (ستون < change_آغاز)
		    // the remaining changes are past this column and not relevant
		    توڑو;
	    }
	}
    }
    rettv->vval.v_نمبر = hlID == (hlf_T)0 ? 0 : (صحیح)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ساکن صحیح
parse_diff_optarg(
    typval_T	    *opts,
    صحیح		    *diffopts,
    طویل	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    صحیح		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    اگر (algo != تہی)
    {
	اگر (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	ورنہ اگر (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_اقل;
	ورنہ اگر (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	ورنہ اگر (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    اگر (output_fmt != تہی)
    {
	اگر (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	ورنہ اگر (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	ورنہ
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    واپس FAIL;
	}
    }

    *diff_ctxlen = dict_get_نمبر_def(d, "context", 0);
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

    واپس OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ساکن خالی
list_مقصد_diffin(list_T *l, diffin_T *din, صحیح icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_اشیاء(l, li)
    {
	str = tv_get_string(&li->li_tv);
	اگر (icase)
	{
	    str = strlow_save(str);
	    اگر (str == تہی)
		جاری;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	اگر (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (حرف *)ga.ga_ڈیٹا;
    din->din_mmfile.سائز = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ساکن dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    اگر (hunk_dict == تہی)
	واپس تہی;

    dict_جوڑو_نمبر(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_جوڑو_نمبر(hunk_dict, "from_count", hunk->گنتی_orig);
    dict_جوڑو_نمبر(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_جوڑو_نمبر(hunk_dict, "to_count", hunk->گنتی_new);

    واپس hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    خالی
f_diff(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    اگر (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	واپس;

    CLEAR_میدان(dio);
    dio.dio_اندرونی = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, حجم(حرف *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    صحیح		save_diff_flags = diff_flags;
    طویل	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_اندرونی;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
	اگر (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    واپس;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_مقصد_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_مقصد_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

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
    صحیح diff_حالت = diff_file(&dio);

    // restore 'diffexpr'
    اگر (restore_diffexpr)
	*p_dex = cc;

    اگر (diff_حالت == FAIL)
	جاؤ مکمل;

    صحیح		hunk_idx = 0;
    dict_T	*hunk_dict;

    اگر (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	اگر (rettv_list_alloc(rettv) != OK)
	    جاؤ مکمل;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = تہی;
	جب (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_ڈیٹا)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    اگر (hunk_dict == تہی)
		جاؤ مکمل;

	    اگر (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    ورنہ
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_ڈیٹا);
    }

مکمل:
    clear_diffin(&dio.dio_new);
    اگر (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    ورنہ
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#اگر_ختم
