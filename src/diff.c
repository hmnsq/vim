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

#ضمن "vim.h"
#ضمن "xdiff/xdiff.h"

#إذا defined(FEAT_DIFF)

ساكن صحيح diff_busy = FALSE;	    // using diff structs, don't change them
ساكن صحيح diff_need_update = FALSE; // ex_diffupdate needs to be called

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
# define DIFF_دننی	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_کلمه    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_کلمه)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_کلمه)
ساكن صحيح	diff_flags = DIFF_دننی | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ساكن اوږد diff_algorithm = XDF_INDENT_HEURISTIC;
ساكن صحيح diff_کلمه_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_اندازه (1024L * 1024 * 1023)

ساكن صحيح diff_a_کار_کوي = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# إذا defined(MSWIN)
ساكن صحيح diff_bin_کار_کوي = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
تعريفنوع بنية {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
تعريفنوع بنية {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
تعريفنوع بنية {
    linenr_T lnum_orig;
    اوږد     شمېر_orig;
    linenr_T lnum_new;
    اوږد     شمېر_new;
} diffhunk_T;

تعريفنوع تعداد {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
تعريفنوع بنية {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    صحيح		    dio_دننی; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    صحيح		    dio_ctxlen;   // unified diff context length
} diffio_T;

ساكن صحيح diff_buf_idx(buf_T *buf);
ساكن صحيح diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ساكن فراغ diff_mark_adjust_tp(tabpage_T *tp, صحيح idx, linenr_T line1, linenr_T line2, اوږد amount, اوږد amount_after);
ساكن فراغ diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ساكن صحيح diff_check_sanity(tabpage_T *tp, diff_T *dp);
ساكن صحيح check_external_diff(diffio_T *diffio);
ساكن صحيح diff_file(diffio_T *diffio);
ساكن صحيح diff_equal_ثبت(diff_T *dp, صحيح idx1, صحيح idx2);
ساكن صحيح diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ساكن فراغ diff_fold_update(diff_T *dp, صحيح skip_idx);
# endif
ساكن فراغ diff_read(صحيح idx_orig, صحيح idx_new, diffio_T *dio);
ساكن فراغ diff_copy_ثبت(diff_T *dprev, diff_T *dp, صحيح idx_orig, صحيح idx_new);
ساكن diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ساكن صحيح parse_diff_ed(char_u *کرښه, diffhunk_T *hunk);
ساكن صحيح parse_diff_unified(char_u *کرښه, diffhunk_T *hunk);
ساكن صحيح xdiff_بهر_indices(اوږد پیل_a, اوږد شمېر_a, اوږد پیل_b, اوږد شمېر_b, فراغ *priv);
ساكن صحيح xdiff_بهر_unified(فراغ *priv, mmbuffer_t *mb, صحيح nbuf);
ساكن صحيح parse_diffanchors(صحيح check_only, buf_T *buf, linenr_T *anchors, صحيح *عدد_anchors);

# define FOR_ALL_DIFFBLOCKS_دننه_TAB(tp, dp) \
    لكل ((dp) = (tp)->tp_لومړی_diff; (dp) != عدم; (dp) = (dp)->df_بل)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ساكن diff_T	    *diff_finger_dp = عدم;
ساكن tabpage_T    *diff_finger_tp = عدم;
ساكن صحيح	    diff_finger_idx = -1;
ساكن linenr_T	    diff_finger_lnum = 0;

    ساكن فراغ
clear_diffblock(diff_T *dp)
{
    إذا (dp == diff_finger_dp)
	diff_finger_dp = عدم;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ساكن diff_T *
diff_لټون_پیل(tabpage_T *tp, صحيح idx, linenr_T lnum)
{
    إذا (diff_finger_dp != عدم
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	أرجع diff_finger_dp;
    أرجع tp->tp_لومړی_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ساكن فراغ
diff_لټون_remember(tabpage_T *tp, صحيح idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != عدم ? dp : tp->tp_لومړی_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    فراغ
diff_buf_delete(buf_T *buf)
{
    صحيح		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	إذا (i != DB_شمېر)
	{
	    tp->tp_diffbuf[i] = عدم;
	    tp->tp_diff_invalid = TRUE;
	    إذا (tp == curtab)
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
    فراغ
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    صحيح		i;

    إذا (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_بفر == win->w_بفر && wp->w_p_diff)
		اكسر;
	إذا (wp == عدم)
	{
	    i = diff_buf_idx(win->w_بفر);
	    إذا (i != DB_شمېر)
	    {
		curtab->tp_diffbuf[i] = عدم;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    وإلا
	diff_buf_جمع_کړه(win->w_بفر);
}

/*
 * Add a buffer to make diffs for.
 * Call this when a new buffer is being edited in the current window where
 * 'diff' is set.
 * Marks the current buffer as being part of the diff and requiring updating.
 * This must be done before any autocmd, because a command may use info
 * about the screen contents.
 */
    فراغ
diff_buf_جمع_کړه(buf_T *buf)
{
    صحيح		i;

    إذا (diff_buf_idx(buf) != DB_شمېر)
	أرجع;		// It's already there.

    لكل (i = 0; i < DB_شمېر; ++i)
	إذا (curtab->tp_diffbuf[i] == عدم)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    أرجع;
	}

    semsg(_(e_cannot_diff_نور_than_nr_buffers), DB_شمېر);
}

/*
 * Remove all buffers to make diffs for.
 */
    ساكن فراغ
diff_buf_clear(فراغ)
{
    صحيح		i;

    لكل (i = 0; i < DB_شمېر; ++i)
	إذا (curtab->tp_diffbuf[i] != عدم)
	{
	    curtab->tp_diffbuf[i] = عدم;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ساكن صحيح
diff_buf_idx(buf_T *buf)
{
    صحيح		idx;

    لكل (idx = 0; idx < DB_شمېر; ++idx)
	إذا (curtab->tp_diffbuf[idx] == buf)
	    اكسر;
    أرجع idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ساكن صحيح
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    صحيح		idx;

    لكل (idx = 0; idx < DB_شمېر; ++idx)
	إذا (tp->tp_diffbuf[idx] == buf)
	    اكسر;
    أرجع idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    فراغ
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    صحيح		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	إذا (i != DB_شمېر)
	{
	    tp->tp_diff_invalid = TRUE;
	    إذا (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    فراغ
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    اوږد	amount,
    اوږد	amount_after)
{
    صحيح		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	إذا (idx != DB_شمېر)
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
    ساكن فراغ
diff_mark_adjust_tp(
    tabpage_T	*tp,
    صحيح		idx,
    linenr_T	line1,
    linenr_T	line2,
    اوږد	amount,
    اوږد	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    صحيح		i;
    صحيح		inserted, deleted;
    صحيح		n, off;
    linenr_T	وروستی;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    صحيح		check_unchanged;

    diff_finger_dp = عدم;		// may shift df_lnum, invalidate the finger

    إذا (diff_دننی())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    إذا (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    وإلا إذا (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    وإلا
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = عدم;
    dp = tp->tp_لومړی_diff;
    لكل (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	إذا ((dp == عدم || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == عدم
		    || dprev->df_lnum[idx] + dprev->df_شمېر[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    إذا (dnext == عدم)
		أرجع;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_شمېر[idx] = inserted;
	    لكل (i = 0; i < DB_شمېر; ++i)
		إذا (tp->tp_diffbuf[i] != عدم && i != idx)
		{
		    إذا (dprev == عدم)
			dnext->df_lnum[i] = line1;
		    وإلا
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_شمېر[i])
			    - (dprev->df_lnum[idx] + dprev->df_شمېر[idx]);
		    dnext->df_شمېر[i] = deleted;
		}
	}

	// if at end of the list, quit
	إذا (dp == عدم)
	    اكسر;

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
	وروستی = dp->df_lnum[idx] + dp->df_شمېر[idx] - 1;

	// 1. change completely above line1: nothing to do
	إذا (وروستی >= line1 - 1)
	{
	    إذا (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		إذا (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_بل;
		استمر;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    إذا (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		إذا (amount_after == 0)
		    اكسر;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    وإلا
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		إذا (deleted > 0)
		{
		    off = 0;
		    إذا (dp->df_lnum[idx] >= line1)
		    {
			إذا (وروستی <= line2)
			{
			    // 4. delete all lines of diff
			    إذا (dp->df_بل != عدم
				    && dp->df_بل->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_بل->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_شمېر[idx];
				lnum_deleted = dp->df_بل->df_lnum[idx];
			    }
			    وإلا
				n = deleted - dp->df_شمېر[idx];
			    dp->df_شمېر[idx] = 0;
			}
			وإلا
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_شمېر[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    وإلا
		    {
			إذا (وروستی < line2)
			{
			    // 2. delete at end of diff
			    dp->df_شمېر[idx] -= وروستی - lnum_deleted + 1;
			    إذا (dp->df_بل != عدم
				    && dp->df_بل->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_بل->df_lnum[idx] - 1 - وروستی;
				deleted -= dp->df_بل->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_بل->df_lnum[idx];
			    }
			    وإلا
				n = line2 - وروستی;
			    check_unchanged = TRUE;
			}
			وإلا
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_شمېر[idx] -= deleted;
			}
		    }

		    لكل (i = 0; i < DB_شمېر; ++i)
			إذا (tp->tp_diffbuf[i] != عدم && i != idx)
			{
			    إذا (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    وإلا
				dp->df_lnum[i] = 1;
			    dp->df_شمېر[i] += n;
			}
		}
		وإلا
		{
		    إذا (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_شمېر[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    وإلا
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		إذا (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	إذا (dprev != عدم && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_شمېر[idx]
							== dp->df_lnum[idx])
	{
	    لكل (i = 0; i < DB_شمېر; ++i)
		إذا (tp->tp_diffbuf[i] != عدم)
		    dprev->df_شمېر[i] += dp->df_شمېر[i];
	    dprev->df_بل = dp->df_بل;
	    clear_diffblock(dp);
	    dp = dprev->df_بل;
	}
	وإلا
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_بل;
	}
    }

    dprev = عدم;
    dp = tp->tp_لومړی_diff;
    طالما (dp != عدم)
    {
	// All counts are zero, remove this entry.
	لكل (i = 0; i < DB_شمېر; ++i)
	    إذا (tp->tp_diffbuf[i] != عدم && dp->df_شمېر[i] != 0)
		اكسر;
	إذا (i == DB_شمېر)
	{
	    dnext = dp->df_بل;
	    clear_diffblock(dp);
	    dp = dnext;
	    إذا (dprev == عدم)
		tp->tp_لومړی_diff = dnext;
	    وإلا
		dprev->df_بل = dnext;
	}
	وإلا
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_بل;
	}

    }

    إذا (tp == curtab)
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
    ساكن diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    إذا (dnew == عدم)
	أرجع عدم;
    diff_finger_dp = عدم;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_بل = dp;
    إذا (dprev == عدم)
	tp->tp_لومړی_diff = dnew;
    وإلا
	dprev->df_بل = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, حجم(diffline_change_T), 20);
    أرجع dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ساكن فراغ
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    صحيح		i_org;
    صحيح		i_new;
    صحيح		off_org, off_new;
    char_u	*کرښه_org;
    صحيح		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    لكل (i_org = 0; i_org < DB_شمېر; ++i_org)
	إذا (tp->tp_diffbuf[i_org] != عدم)
	    اكسر;
    إذا (i_org == DB_شمېر)	// safety check
	أرجع;

    إذا (diff_check_sanity(tp, dp) == FAIL)
	أرجع;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    لكل (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	طالما (dp->df_شمېر[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    إذا (dir == BACKWARD)
		off_org = dp->df_شمېر[i_org] - 1;
	    کرښه_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    إذا (کرښه_org == عدم)
		أرجع;
	    لكل (i_new = i_org + 1; i_new < DB_شمېر; ++i_new)
	    {
		إذا (tp->tp_diffbuf[i_new] == عدم)
		    استمر;
		إذا (dir == BACKWARD)
		    off_new = dp->df_شمېر[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		إذا (off_new < 0 || off_new >= dp->df_شمېر[i_new])
		    اكسر;
		إذا (diff_cmp(کرښه_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    اكسر;
	    }
	    vim_free(کرښه_org);

	    // Stop when a line isn't equal in all diff buffers.
	    إذا (i_new != DB_شمېر)
		اكسر;

	    // Line matched in all buffers, remove it from the diff.
	    لكل (i_new = i_org; i_new < DB_شمېر; ++i_new)
		إذا (tp->tp_diffbuf[i_new] != عدم)
		{
		    إذا (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_شمېر[i_new];
		}
	}
	إذا (dir == BACKWARD)
	    اكسر;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ساكن صحيح
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    صحيح		i;

    لكل (i = 0; i < DB_شمېر; ++i)
	إذا (tp->tp_diffbuf[i] != عدم)
	    إذا (dp->df_lnum[i] + dp->df_شمېر[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_کرښه_شمېر)
		أرجع FAIL;
    أرجع OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    فراغ
diff_redraw(
    صحيح		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = عدم;
    صحيح		کارول_شوی_max_fill_other = FALSE;
    صحيح		کارول_شوی_max_fill_curwin = FALSE;
    صحيح		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	إذا (!wp->w_p_diff || !buf_valid(wp->w_بفر))
	    استمر;

	redraw_win_later(wp, UPD_SOME_VALID);
	إذا (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	إذا (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	إذا ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    إذا (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    وإلا إذا (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		إذا (wp == curwin)
		    کارول_شوی_max_fill_curwin = TRUE;
		وإلا إذا (wp_other != عدم)
		    کارول_شوی_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    إذا (wp_other != عدم && curwin->w_p_scb)
    {
	إذا (کارول_شوی_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_سېټ_topline(wp_other, curwin);
	وإلا إذا (کارول_شوی_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_سېټ_topline(curwin, wp_other);
    }
}

    ساكن فراغ
clear_diffin(diffin_T *din)
{
    إذا (din->din_fname == عدم)
	VIM_CLEAR(din->din_mmfile.ptr);
    وإلا
	mch_remove(din->din_fname);
}

    ساكن فراغ
clear_diffout(diffout_T *dout)
{
    إذا (dout->dout_fname == عدم)
	ga_clear_مزي_ګانې(&dout->dout_ga);
    وإلا
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ساكن صحيح
diff_write_بفر(buf_T *buf, diffin_T *din, linenr_T پیل, linenr_T پای_ټکی)
{
    linenr_T	lnum;
    char_u	*s;
    اوږد	len = 0;
    char_u	*ptr;

    إذا (پای_ټکی < 0)
	پای_ټکی = buf->b_ml.ml_کرښه_شمېر;

    إذا (buf->b_ml.ml_flags & ML_تشه || پای_ټکی < پیل)
    {
	din->din_mmfile.ptr = عدم;
	din->din_mmfile.اندازه = 0;
	أرجع OK;
    }

    // xdiff requires one big block of memory with all the text.
    لكل (lnum = پیل; lnum <= پای_ټکی; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    إذا (ptr == عدم)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	أرجع FAIL;
    }
    din->din_mmfile.ptr = (حرف *)ptr;
    din->din_mmfile.اندازه = len;

    len = 0;
    لكل (lnum = پیل; lnum <= پای_ټکی; ++lnum)
    {
	لكل (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    إذا (diff_flags & DIFF_ICASE)
	    {
		صحيح c;
		صحيح	orig_len;
		صحيح	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		إذا (*s == NL)
		    c = NUL;
		وإلا
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		إذا (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		وإلا
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    إذا (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    وإلا
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    أرجع OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ساكن صحيح
diff_write(buf_T *buf, diffin_T *din, linenr_T پیل, linenr_T پای_ټکی)
{
    صحيح		r;
    صحيح		save_ml_flags;
    char_u	*save_ff;
    صحيح		save_cmod_flags;

    إذا (din->din_fname == عدم)
	أرجع diff_write_بفر(buf, din, پیل, پای_ټکی);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    إذا (چوکاټونه_locked())
	أرجع FAIL;

    إذا (پای_ټکی < 0)
	پای_ټکی = buf->b_ml.ml_کرښه_شمېر;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    إذا (پای_ټکی < پیل)
    {
	// The line range specifies a completely empty file.
	پای_ټکی = پیل;
	buf->b_ml.ml_flags |= ML_تشه;
    }
    r = buf_write(buf, din->din_fname, عدم,
			پیل, پای_ټکی,
			عدم, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_تشه) | (save_ml_flags & ML_تشه);
    أرجع r;
}

    ساكن صحيح
lnum_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    إذا (lnum1 < lnum2)
	أرجع -1;
    إذا (lnum1 > lnum2)
	أرجع 1;
    أرجع 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ساكن فراغ
diff_try_update(
	diffio_T    *dio,
	صحيح	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    صحيح		idx_new;

    إذا (dio->dio_دننی)
    {
	ga_init2(&dio->dio_diff.dout_ga, حجم(حرف *), 1000);
    }
    وإلا
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	إذا (dio->dio_orig.din_fname == عدم
		|| dio->dio_new.din_fname == عدم
		|| dio->dio_diff.dout_fname == عدم)
	    اذهب theend;
    }

    // Check external diff is actually working.
    إذا (!dio->dio_دننی && check_external_diff(dio) == FAIL)
	اذهب theend;

    // :diffupdate!
    إذا (eap != عدم && eap->forceit)
	لكل (idx_new = idx_orig; idx_new < DB_شمېر; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    إذا (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    صحيح عدد_anchors = لوی_عدد;
    linenr_T anchors[DB_شمېر][MAX_DIFF_ANCHORS];
    CLEAR_ډګر(anchors);
    إذا (diff_flags & DIFF_ANCHOR)
    {
	لكل (صحيح idx = 0; idx < DB_شمېر; idx++)
	{
	    إذا (curtab->tp_diffbuf[idx] == عدم)
		استمر;
	    صحيح buf_عدد_anchors = 0;
	    إذا (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_عدد_anchors) != OK)
	    {
		emsg(_(e_failed_منزل_پیدا_کړه_all_diff_anchors));
		عدد_anchors = 0;
		CLEAR_ډګر(anchors);
		اكسر;
	    }
	    إذا (buf_عدد_anchors < عدد_anchors)
		عدد_anchors = buf_عدد_anchors;

	    إذا (buf_عدد_anchors > 0)
		qsort((فراغ *)anchors[idx],
			(size_t)buf_عدد_anchors,
			حجم(linenr_T),
			lnum_compare);
	}
    }
    إذا (عدد_anchors == لوی_عدد)
	عدد_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    لكل (صحيح anchor_i = 0; anchor_i <= عدد_anchors; anchor_i++)
    {
	diff_T	*orig_diff = عدم;
	إذا (anchor_i != 0)
	{
	    orig_diff = curtab->tp_لومړی_diff;
	    curtab->tp_لومړی_diff = عدم;
	}
	linenr_T lnum_پیل = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_پای_ټکی = (anchor_i == عدد_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	إذا (diff_write(buf, &dio->dio_orig, lnum_پیل, lnum_پای_ټکی) == FAIL)
	{
	    إذا (orig_diff != عدم)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_لومړی_diff = orig_diff;
		diff_clear(curtab);
	    }
	    اذهب theend;
	}

	// Make a difference between the first buffer and every other.
	لكل (idx_new = idx_orig + 1; idx_new < DB_شمېر; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
		استمر; // skip buffer that isn't loaded

	    lnum_پیل = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_پای_ټکی = anchor_i == عدد_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    إذا (diff_write(buf, &dio->dio_new, lnum_پیل, lnum_پای_ټکی) == FAIL)
		استمر;
	    إذا (diff_file(dio) == FAIL)
		استمر;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	إذا (anchor_i != 0)
	{
	    diff_finger_dp = عدم;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    لكل (diff_T *dp = curtab->tp_لومړی_diff; dp != عدم; dp = dp->df_بل)
	    {
		لكل (صحيح idx = 0; idx < DB_شمېر; idx++)
		{
		    إذا (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    إذا (orig_diff != عدم)
	    {
		diff_T *وروستی_diff = orig_diff;
		طالما (وروستی_diff->df_بل != عدم)
		    وروستی_diff = وروستی_diff->df_بل;
		وروستی_diff->df_بل = curtab->tp_لومړی_diff;
		curtab->tp_لومړی_diff = orig_diff;
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
    صحيح
diff_دننی(فراغ)
{
    أرجع (diff_flags & DIFF_دننی) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ساكن صحيح
diff_دننی_failed(فراغ)
{
    صحيح idx;

    // Only need to do something when there is another buffer.
    لكل (idx = 0; idx < DB_شمېر; ++idx)
	إذا (curtab->tp_diffbuf[idx] != عدم
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    فراغ
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    صحيح		idx_orig;
    صحيح		idx_new;
    diffio_T	diffio;
    صحيح		had_diffs = curtab->tp_لومړی_diff != عدم;

    إذا (diff_busy)
    {
	diff_need_update = TRUE;
	أرجع;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    لكل (idx_orig = 0; idx_orig < DB_شمېر; ++idx_orig)
	إذا (curtab->tp_diffbuf[idx_orig] != عدم)
	    اكسر;
    إذا (idx_orig == DB_شمېر)
	اذهب theend;

    // Only need to do something when there is another buffer.
    لكل (idx_new = idx_orig + 1; idx_new < DB_شمېر; ++idx_new)
	إذا (curtab->tp_diffbuf[idx_new] != عدم)
	    اكسر;
    إذا (idx_new == DB_شمېر)
	اذهب theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_ډګر(diffio);
    diffio.dio_دننی = diff_دننی() && !diff_دننی_failed();

    diff_try_update(&diffio, idx_orig, eap);
    إذا (diffio.dio_دننی && diff_دننی_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_ډګر(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    إذا (had_diffs || curtab->tp_لومړی_diff != عدم)
    {
	diff_redraw(TRUE);
	کړکۍ_layout_قلف();
	apply_autocmds(EVENT_DIFFUPDATED, عدم, عدم, FALSE, curbuf);
	کړکۍ_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ساكن صحيح
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    صحيح		ok;
    صحيح		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    لكل (;;)
    {
	ok = FALSE;
	fd = mch_fopen((حرف *)diffio->dio_orig.din_fname, "w");
	إذا (fd == عدم)
	    io_error = TRUE;
	وإلا
	{
	    إذا (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((حرف *)diffio->dio_new.din_fname, "w");
	    إذا (fd == عدم)
		io_error = TRUE;
	    وإلا
	    {
		إذا (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = عدم;
		إذا (diff_file(diffio) == OK)
		    fd = mch_fopen((حرف *)diffio->dio_diff.dout_fname, "r");
		إذا (fd == عدم)
		    io_error = TRUE;
		وإلا
		{
		    char_u	linebuf[LBUFLEN];

		    لكل (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			إذا (vim_fgets(linebuf, LBUFLEN, fd))
			    اكسر;
			إذا (STRNCMP(linebuf, "1c1", 3) == 0
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
	إذا (*p_dex != NUL)
	    اكسر;
# endif

# إذا defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	إذا (ok && diff_a_کار_کوي == MAYBE && diff_bin_کار_کوي == MAYBE)
	{
	    diff_a_کار_کوي = TRUE;
	    diff_bin_کار_کوي = TRUE;
	    استمر;
	}
	إذا (!ok && diff_a_کار_کوي == TRUE && diff_bin_کار_کوي == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_کار_کوي = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	إذا (diff_a_کار_کوي != MAYBE)
	    اكسر;
	diff_a_کار_کوي = ok;

	// If "-a" works break here, otherwise retry without "-a".
	إذا (ok)
	    اكسر;
    }
    إذا (!ok)
    {
	إذا (io_error)
	    emsg(_(e_cannot_read_or_write_لنډمهاله_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_کار_کوي = MAYBE;
# إذا defined(MSWIN)
	diff_bin_کار_کوي = MAYBE;
# endif
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Invoke the xdiff function.
 */
    ساكن صحيح
diff_file_دننی(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_ډګر(param);
    CLEAR_ډګر(emit_cfg);
    CLEAR_ډګر(emit_cb);

    param.flags = diff_algorithm;

    إذا (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    إذا (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    إذا (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    إذا (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_کرښې;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    إذا (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_بهر_indices;
    وإلا
	emit_cb.بهر_کرښه = xdiff_بهر_unified;
    إذا (diffio->dio_orig.din_mmfile.اندازه > MAX_XDIFF_اندازه ||
	diffio->dio_new.din_mmfile.اندازه > MAX_XDIFF_اندازه)
    {
	emsg(_(e_problem_creating_دننی_diff));
	أرجع FAIL;
    }
    إذا (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_دننی_diff));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ساكن صحيح
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    إذا (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	أرجع OK;
    }
    وإلا
# endif
    // Use xdiff for generating the diff.
    إذا (dio->dio_دننی)
	أرجع diff_file_دننی(dio);

    إذا (check_restricted())
	أرجع FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    إذا (cmd == عدم)
	أرجع FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    إذا (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((حرف *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_کار_کوي == FALSE ? "" : "-a ",
# إذا defined(MSWIN)
	    diff_bin_کار_کوي == TRUE ? "--binary " : "",
# وإلا
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (صحيح)len, p_srr, tmp_diff);
    بلاک_autocmds();	// avoid ShellCmdPost stuff
    (فراغ)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    أرجع OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    فراغ
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = عدم;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = عدم;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = عدم;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = عدم;
    صحيح		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = عدم;

# ifdef FEAT_BROWSE
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, عدم, عدم,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), عدم);
	إذا (browseFile == عدم)
	    أرجع;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    إذا (tmp_orig == عدم || tmp_new == عدم)
	اذهب theend;

    // Write the current buffer to "tmp_orig".
    إذا (buf_write(curbuf, tmp_orig, عدم,
		(linenr_T)1, curbuf->b_ml.ml_کرښه_شمېر,
				     عدم, FALSE, FALSE, FALSE, TRUE) == FAIL)
	اذهب theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = ډک_Name_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != عدم ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    إذا (esc_name == عدم)
	اذهب theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    إذا (buf == عدم)
	اذهب theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    إذا (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((حرف *)dirbuf) != 0)
	dirbuf[0] = NUL;
    وإلا
    {
#  ifdef TEMPDIRNAMES
	إذا (vim_tempdir != عدم)
	    vim_ignored = mch_chdir((حرف *)vim_tempdir);
	وإلا
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    إذا (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != عدم ? fullname :
#  endif
		eap->arg, tmp_new);
    وإلا
# endif
    {
	إذا (check_restricted())
	    اذهب theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((حرف *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	بلاک_autocmds();	// Avoid ShellCmdPost stuff
	(فراغ)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    إذا (dirbuf[0] != NUL)
    {
	إذا (mch_chdir((حرف *)dirbuf) != 0)
	    emsg(_(e_cannot_go_شا_منزل_پخوانی_directory));
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
    إذا (mch_stat((حرف *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    وإلا
    {
	إذا (curbuf->b_fname != عدم)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    إذا (newname != عدم)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	إذا (win_وېشل(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_وېشل;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    إذا (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		إذا (newname != عدم)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    إذا (au_has_ډله((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    إذا (tmp_orig != عدم)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    إذا (tmp_new != عدم)
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
    فراغ
ex_diffsplit(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    bufref_T	old_curbuf;

    سېټ_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    سېټ_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    إذا (win_وېشل(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	أرجع;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_وېشل;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    إذا (curwin == old_curwin)		// split didn't work
	أرجع;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    إذا (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	إذا (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_کرښه(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_منزل_fraction(curwin, curwin->w_لوړوالی);
}

/*
 * Set options to show diffs for the current window.
 */
    فراغ
ex_diffthis(exarg_T *eap ناکارول_شوی)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ساكن فراغ
سېټ_diff_option(win_T *wp, صحيح ارزښت)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_بفر;
    ++curbuf_قلف;
    سېټ_option_ارزښت_give_err((char_u *)"diff", (اوږد)ارزښت, عدم, OPT_LOCAL);
    --curbuf_قلف;
    curwin = old_curwin;
    curbuf = curwin->w_بفر;
}

/*
 * Set options in window "wp" for diff mode.
 */
    فراغ
diff_win_options(
    win_T	*wp,
    صحيح		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    إذا (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    إذا (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    إذا (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	إذا (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    إذا (!wp->w_p_diff)
    {
	إذا (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    سېټ_string_option_مستقیم_دننه_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    إذا (!wp->w_p_diff)
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
    بدل_شوی_کړکۍ_setting_win(wp);
# endif
    إذا (vim_strchr(p_sbo, 'h') == عدم)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    سېټ_diff_option(wp, TRUE);

    إذا (addbuf)
	diff_buf_جمع_کړه(wp->w_بفر);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    فراغ
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    صحيح		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	إذا (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    سېټ_diff_option(wp, FALSE);

	    إذا (wp->w_p_diff_saved)
	    {

		إذا (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		إذا (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		إذا (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    إذا (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		إذا (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		إذا (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		إذا (wp->w_p_fen)
		    wp->w_p_fen = foldmethodIsManual(wp) ? FALSE
							 : wp->w_p_fen_save;

		foldUpdateAll(wp);
# endif
	    }
	    // remove filler lines
	    wp->w_topfill = 0;

	    // make sure topline is not halfway a fold and cursor is
	    // invalidated
	    بدل_شوی_کړکۍ_setting_win(wp);

	    // Note: 'sbo' is not restored, it's a global option.
	    diff_buf_adjust(wp);
	}
	diffwin |= wp->w_p_diff;
    }

    // Also remove hidden buffers from the list.
    إذا (eap->forceit)
	diff_buf_clear();

    إذا (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    إذا (!diffwin && vim_strchr(p_sbo, 'h') != عدم)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ساكن فراغ
diff_read(
    صحيح		idx_orig,	// idx of original file
    صحيح		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = عدم;
    صحيح		کرښه_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = عدم;
    diff_T	*dp = curtab->tp_لومړی_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*کرښه;
    اوږد	off;
    صحيح		i;
    صحيح		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = عدم;	    // init to avoid gcc warning

    تعداد {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    إذا (dout->dout_fname == عدم)
    {
	diffstyle = DIFF_UNIFIED;
    }
    وإلا
    {
	fd = mch_fopen((حرف *)dout->dout_fname, "r");
	إذا (fd == عدم)
	{
	    emsg(_(e_cannot_read_diff_output));
	    أرجع;
	}
    }

    إذا (!dio->dio_دننی)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	إذا (hunk == عدم)
	{
	    إذا (fd != عدم)
		fclose(fd);
	    أرجع;
	}
    }

    لكل (;;)
    {
	إذا (dio->dio_دننی)
	{
	    إذا (کرښه_hunk_idx >= dout->dout_ga.ga_len)
		اكسر;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_معلومات)[کرښه_hunk_idx++];
	}
	وإلا
	{
	    إذا (fd == عدم)
	    {
		إذا (کرښه_hunk_idx >= dout->dout_ga.ga_len)
		    اكسر;	    // did last line
		کرښه = ((char_u **)dout->dout_ga.ga_معلومات)[کرښه_hunk_idx++];
	    }
	    وإلا
	    {
		إذا (vim_fgets(linebuf, LBUFLEN, fd))
		    اكسر;		// end of file
		کرښه = linebuf;
	    }

	    إذا (diffstyle == DIFF_NONE)
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
		إذا (خوندي_isdigit(*کرښه))
		    diffstyle = DIFF_ED;
		وإلا إذا ((STRNCMP(کرښه, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		وإلا إذا ((STRNCMP(کرښه, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(کرښه, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(کرښه, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		وإلا
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    استمر;
	    }

	    إذا (diffstyle == DIFF_ED)
	    {
		إذا (!خوندي_isdigit(*کرښه))
		    استمر;	// not the start of a diff block
		إذا (parse_diff_ed(کرښه, hunk) == FAIL)
		    استمر;
	    }
	    وإلا إذا (diffstyle == DIFF_UNIFIED)
	    {
		إذا (STRNCMP(کرښه, "@@ ", 3)  != 0)
		    استمر;	// not the start of a diff block
		إذا (parse_diff_unified(کرښه, hunk) == FAIL)
		    استمر;
	    }
	    وإلا
	    {
		emsg(_(e_invalid_diff_format));
		اكسر;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	طالما (dp != عدم
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_شمېر[idx_orig])
	{
	    إذا (notset)
		diff_copy_ثبت(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_بل;
	    notset = TRUE;
	}

	إذا (dp != عدم
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_شمېر[idx_orig]
		&& hunk->lnum_orig + hunk->شمېر_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    لكل (dpl = dp; dpl->df_بل != عدم; dpl = dpl->df_بل)
		إذا (hunk->lnum_orig + hunk->شمېر_orig
					     < dpl->df_بل->df_lnum[idx_orig])
		    اكسر;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    إذا (off > 0)
	    {
		لكل (i = idx_orig; i < idx_new; ++i)
		    إذا (curtab->tp_diffbuf[i] != عدم)
		    {
			dp->df_lnum[i] -= off;
			dp->df_شمېر[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_شمېر[idx_new] = hunk->شمېر_new;
	    }
	    وإلا إذا (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_شمېر[idx_new] = hunk->شمېر_new - off;
	    }
	    وإلا
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		صحيح orig_اندازه_دننه_dp = MIN(hunk->شمېر_orig,
			dp->df_lnum[idx_orig] +
			dp->df_شمېر[idx_orig] - hunk->lnum_orig);
		صحيح اندازه_diff = hunk->شمېر_new - orig_اندازه_دننه_dp;
		dp->df_شمېر[idx_new] += اندازه_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->شمېر_new
		    - (dp->df_lnum[idx_new] + dp->df_شمېر[idx_new]);
		إذا (off > 0)
		    dp->df_شمېر[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->شمېر_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_شمېر[idx_orig]);
	    إذا (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		إذا (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_شمېر[idx_new] += -off;
		}
		off = 0;
	    }
	    لكل (i = idx_orig; i < idx_new; ++i)
		إذا (curtab->tp_diffbuf[i] != عدم)
		    dp->df_شمېر[i] = dpl->df_lnum[i] + dpl->df_شمېر[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_بل;
	    dp->df_بل = dpl->df_بل;
	    طالما (dn != dp->df_بل)
	    {
		dpl = dn->df_بل;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	وإلا
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    إذا (dp == عدم)
		اذهب بشپړ;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_شمېر[idx_orig] = hunk->شمېر_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_شمېر[idx_new] = hunk->شمېر_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    لكل (i = idx_orig + 1; i < idx_new; ++i)
		إذا (curtab->tp_diffbuf[i] != عدم)
		    diff_copy_ثبت(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    طالما (dp != عدم)
    {
	إذا (notset)
	    diff_copy_ثبت(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_بل;
	notset = TRUE;
    }

بشپړ:
    إذا (!dio->dio_دننی)
	vim_free(hunk);

    إذا (fd != عدم)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ساكن فراغ
diff_copy_ثبت(
    diff_T	*dprev,
    diff_T	*dp,
    صحيح		idx_orig,
    صحيح		idx_new)
{
    اوږد	off;

    إذا (dprev == عدم)
	off = 0;
    وإلا
	off = (dprev->df_lnum[idx_orig] + dprev->df_شمېر[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_شمېر[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_شمېر[idx_new] = dp->df_شمېر[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    فراغ
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *بل_p;

    لكل (p = tp->tp_لومړی_diff; p != عدم; p = بل_p)
    {
	بل_p = p->df_بل;
	clear_diffblock(p);
    }
    tp->tp_لومړی_diff = عدم;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ساكن صحيح
diff_linematch(diff_T *dp)
{
    إذا (!(diff_flags & DIFF_LINEMATCH))
	أرجع 0;

    // are there more than three diff buffers?
    صحيح tsize = 0;
    لكل (صحيح i = 0; i < DB_شمېر; i++)
    {
	إذا (curtab->tp_diffbuf[i] != عدم)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    إذا (dp->df_شمېر[i] < 0)
		أرجع FALSE;
	    tsize += dp->df_شمېر[i];
	}
    }

    // avoid allocating a huge array because it will lag
    أرجع tsize <= linematch_کرښې;
}

    ساكن صحيح
get_max_diff_اوږدتیا(ثابت diff_T *dp)
{
    صحيح maxlength = 0;

    لكل (صحيح k = 0; k < DB_شمېر; k++)
    {
	إذا (curtab->tp_diffbuf[k] != عدم)
	{
	    إذا (dp->df_شمېر[k] > maxlength)
		maxlength = dp->df_شمېر[k];
	}
    }
    أرجع maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ساكن فراغ
پیدا_کړه_سر_diff_بلاک(
    diff_T	**thistopdiff,
    diff_T	**بل_adjacent_بلاکونه,
    صحيح		fromidx,
    صحيح		topline)
{
    diff_T	*topdiff = عدم;
    diff_T	*localtopdiff = عدم;
    صحيح		topdiffchange = 0;

    لكل (topdiff = curtab->tp_لومړی_diff; topdiff != عدم;
						topdiff = topdiff->df_بل)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	إذا (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	إذا (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_شمېر[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    إذا ((*thistopdiff) == عدم)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	إذا (!(topdiff->df_بل && (topdiff->df_بل->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_شمېر[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    إذا (*thistopdiff)
	    {
		(*بل_adjacent_بلاکونه) = topdiff->df_بل;
		اكسر;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ساكن فراغ
calculate_topfill_and_topline(
    ثابت صحيح	fromidx,
    ثابت صحيح	toidx,
    ثابت صحيح	from_topline,
    ثابت صحيح	from_topfill,
    صحيح		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = عدم;
    diff_T	*بل_adjacent_بلاکونه = عدم;
    صحيح		virtual_کرښې_passed = 0;
    صحيح		curlinenum_منزل = 1;

    پیدا_کړه_سر_diff_بلاک(&thistopdiff, &بل_adjacent_بلاکونه, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    طالما (curdif && (curdif->df_lnum[fromidx] + curdif->df_شمېر[fromidx])
							<= from_topline)
    {
	virtual_کرښې_passed += get_max_diff_اوږدتیا(curdif);

	curdif = curdif->df_بل;
    }

    إذا (curdif != بل_adjacent_بلاکونه)
	virtual_کرښې_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_کرښې_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    إذا (virtual_کرښې_passed < 0)
	virtual_کرښې_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    إذا (thistopdiff != عدم) // this should not be null, but just for safety
	curlinenum_منزل = thistopdiff->df_lnum[toidx];

    صحيح virt_کرښې_کیڼ = virtual_کرښې_passed;
    curdif = thistopdiff;
    طالما (virt_کرښې_کیڼ > 0 && curdif != عدم && curdif != بل_adjacent_بلاکونه)
    {
	curlinenum_منزل += MIN(virt_کرښې_کیڼ, curdif->df_شمېر[toidx]);
	virt_کرښې_کیڼ -= MIN(virt_کرښې_کیڼ, get_max_diff_اوږدتیا(curdif));
	curdif = curdif->df_بل;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    صحيح max_virt_کرښې = 0;
    لكل (diff_T *dp = thistopdiff; dp != عدم; dp = dp->df_بل)
    {
	إذا (dp->df_lnum[toidx] + dp->df_شمېر[toidx] <= curlinenum_منزل)
	    max_virt_کرښې += get_max_diff_اوږدتیا(dp);
	وإلا
	{
	    إذا (dp->df_lnum[toidx] <= curlinenum_منزل)
		max_virt_کرښې += curlinenum_منزل - dp->df_lnum[toidx];
	    اكسر;
	}
    }

    إذا (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_کرښې - virtual_کرښې_passed;
    (*topline) = curlinenum_منزل;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ساكن فراغ
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_اوږدتیا,
    ثابت صحيح	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    صحيح		کرښه_numbers[DB_شمېر];
    صحيح		outputmap[DB_شمېر];
    size_t	ndiffs = 0;

    لكل (صحيح i = 0; i < DB_شمېر; i++)
    {
	إذا (curtab->tp_diffbuf[i] != عدم)
	{
	    کرښه_numbers[i] = dp->df_lnum[i];
	    dp->df_شمېر[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    لكل (size_t i = 0; i < decisions_اوږدتیا; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	إذا (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_بل);
	    dp_s->is_linematched = TRUE;
	    لكل (صحيح j = 0; j < DB_شمېر; j++)
	    {
		إذا (curtab->tp_diffbuf[j] != عدم)
		{
		    dp_s->df_lnum[j] = کرښه_numbers[j];
		    dp_s->df_شمېر[j] = 0;
		}
	    }
	}
	لكل (size_t j = 0; j < ndiffs; j++)
	{
	    إذا (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_شمېر[outputmap[j]]++;
		کرښه_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ساكن فراغ
پرلپسې_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_شمېر];
    ثابت mmfile_t	*diffbufs[DB_شمېر];
    صحيح			diff_اوږدتیا[DB_شمېر];
    size_t		ndiffs = 0;

    لكل (صحيح i = 0; i < DB_شمېر; i++)
    {
	إذا (curtab->tp_diffbuf[i] != عدم)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    إذا (dp->df_شمېر[i] > 0)
	    {
		diff_write_بفر(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_شمېر[i] - 1);
	    }
	    وإلا
	    {
		diffbufs_mm[ndiffs].din_mmfile.اندازه = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = عدم;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_اوږدتیا[ndiffs] = dp->df_شمېر[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    صحيح *decisions = عدم;
    ثابت صحيح iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_اوږدتیا =
	linematch_nbuffers(diffbufs, diff_اوږدتیا, ndiffs, &decisions, iwhite);

    لكل (size_t i = 0; i < ndiffs; i++)
	حرر(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_اوږدتیا, decisions);

    حرر(decisions);
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
    صحيح
diff_check_with_linestatus(win_T *wp, linenr_T lnum, صحيح *linestatus)
{
    صحيح		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    صحيح		maxcount;
    صحيح		i;
    buf_T	*buf = wp->w_بفر;
    صحيح		cmp;

    إذا (linestatus != عدم)
	*linestatus = 0;

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    إذا (curtab->tp_لومړی_diff == عدم || !wp->w_p_diff)	// no diffs at all
	أرجع 0;

    // safety check: "lnum" must be a buffer line
    إذا (lnum < 1 || lnum > buf->b_ml.ml_کرښه_شمېر + 1)
	أرجع 0;

    idx = diff_buf_idx(buf);
    إذا (idx == DB_شمېر)
	أرجع 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    إذا (hasFoldingWin(wp, lnum, عدم, عدم, TRUE, عدم))
	أرجع 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = عدم;		// last block with df_lnum[idx] <= lnum
    لكل (dp = diff_لټون_پیل(curtab, idx, lnum); dp != عدم; dp = dp->df_بل)
    {
	إذا (dp->df_lnum[idx] <= lnum)
	    le = dp;
	إذا (lnum <= dp->df_lnum[idx] + dp->df_شمېر[idx])
	    اكسر;
    }
    diff_لټون_remember(curtab, idx, lnum, le);
    إذا (dp == عدم || lnum < dp->df_lnum[idx])
	أرجع 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    إذا (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	پرلپسې_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    صحيح عدد_fill = 0;
    طالما (lnum == dp->df_lnum[idx] + dp->df_شمېر[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	إذا (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_اوږدتیا(dp);
	    عدد_fill += maxcount - dp->df_شمېر[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	إذا (dp->df_بل != عدم
		&& lnum >= dp->df_بل->df_lnum[idx]
		&& lnum <= dp->df_بل->df_lnum[idx] + dp->df_بل->df_شمېر[idx])
	    dp = dp->df_بل;
	وإلا
	    اكسر;
    }

    إذا (lnum < dp->df_lnum[idx] + dp->df_شمېر[idx])
    {
	صحيح	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	لكل (i = 0; i < DB_شمېر; ++i)
	    إذا (i != idx && curtab->tp_diffbuf[i] != عدم)
	    {
		إذا (dp->df_شمېر[i] == 0)
		    zero = TRUE;
		وإلا
		{
		    إذا (dp->df_شمېر[i] != dp->df_شمېر[idx])
		    {
			إذا (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			أرجع عدد_fill;
		    }
		    cmp = TRUE;
		}
	    }
	إذا (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    لكل (i = 0; i < DB_شمېر; ++i)
		إذا (i != idx && curtab->tp_diffbuf[i] != عدم
						      && dp->df_شمېر[i] != 0)
		    إذا (!diff_equal_ثبت(dp, idx, i))
		    {
			إذا (linestatus)
			    *linestatus = -1;
			أرجع عدد_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	إذا (zero == FALSE)
	    أرجع عدد_fill;
	إذا (linestatus)
	    *linestatus = -2;
	أرجع عدد_fill;
    }
    أرجع عدد_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ساكن صحيح
diff_equal_ثبت(diff_T *dp, صحيح idx1, صحيح idx2)
{
    صحيح		i;
    char_u	*کرښه;
    صحيح		cmp;

    إذا (dp->df_شمېر[idx1] != dp->df_شمېر[idx2])
	أرجع FALSE;
    إذا (diff_check_sanity(curtab, dp) == FAIL)
	أرجع FALSE;
    لكل (i = 0; i < dp->df_شمېر[idx1]; ++i)
    {
	کرښه = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	إذا (کرښه == عدم)
	    أرجع FALSE;
	cmp = diff_cmp(کرښه, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(کرښه);
	إذا (cmp != 0)
	    أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ساكن صحيح
diff_equal_char(char_u *p1, char_u *p2, صحيح *len)
{
    صحيح l  = (*mb_ptr2len)(p1);

    إذا (l != (*mb_ptr2len)(p2))
	أرجع FALSE;
    إذا (l > 1)
    {
	إذا (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    أرجع FALSE;
	*len = l;
    }
    وإلا
    {
	إذا ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    أرجع FALSE;
	*len = 1;
    }
    أرجع TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ساكن صحيح
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    صحيح		l;

    إذا ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	أرجع 0;

    إذا ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	أرجع STRCMP(s1, s2);
    إذا ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	أرجع MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    طالما (*p1 != NUL && *p2 != NUL)
    {
	إذا (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	وإلا
	{
	    إذا (!diff_equal_char(p1, p2, &l))
		اكسر;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    إذا (*p1 != NUL || *p2 != NUL)
	أرجع 1;
    أرجع 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    صحيح
diff_check_fill(win_T *wp, linenr_T lnum)
{
    صحيح		n;

    // be quick when there are no filler lines
    إذا (!(diff_flags & DIFF_FILLER))
	أرجع 0;
    n = diff_check_with_linestatus(wp, lnum, عدم);
    إذا (n <= 0)
	أرجع 0;
    أرجع n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    فراغ
diff_سېټ_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_بفر;
    linenr_T	lnum;
    صحيح		fromidx;
    صحيح		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    إذا (fromidx == DB_شمېر)
	أرجع;		// safety check

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
	إذا (lnum <= dp->df_lnum[fromidx] + dp->df_شمېر[fromidx])
	    اكسر;
    إذا (dp == عدم)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_بفر->b_ml.ml_کرښه_شمېر
				       - (frombuf->b_ml.ml_کرښه_شمېر - lnum);
    }
    وإلا
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_بفر);
	إذا (toidx == DB_شمېر)
	    أرجع;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	إذا (lnum >= dp->df_lnum[fromidx])
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
    إذا (towin->w_topline > towin->w_بفر->b_ml.ml_کرښه_شمېر)
    {
	towin->w_topline = towin->w_بفر->b_ml.ml_کرښه_شمېر;
	towin->w_botfill = true;
    }
    إذا (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    بدل_شوی_کرښه_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (فراغ)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    عدم, TRUE, عدم);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ساكن صحيح
parse_diffanchors(
    صحيح		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    صحيح		*عدد_anchors)
{
    صحيح i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = عدم;
    إذا (check_only)
	bufwin = curwin;
    وإلا
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    إذا (bufwin->w_بفر == buf && bufwin->w_p_diff)
		اكسر;
	إذا (bufwin == عدم && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    أرجع FAIL;
	}
    }

    لكل (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	إذا (*dia == ',') // don't allow empty values
	    أرجع FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_پته(عدم, &dia, ADDR_کرښې, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	إذا (dia == عدم) // error detected
	    أرجع FAIL;
	إذا (*dia != ',' && *dia != NUL)
	    أرجع FAIL;

	إذا (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_کرښه_شمېر + 1))
	{
	    emsg(_(e_invalid_range));
	    أرجع FAIL;
	}

	إذا (anchors != عدم)
	    anchors[i] = lnum;

	إذا (*dia == ',')
	    dia++;
    }
    إذا (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_نور_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	أرجع FAIL;
    }
    إذا (عدد_anchors != عدم)
	*عدد_anchors = i;
    أرجع OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    صحيح
diffanchors_بدل_شوی(صحيح buflocal)
{
    صحيح پایله = parse_diffanchors(TRUE, curbuf, عدم, عدم);
    إذا (پایله == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    إذا (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    وإلا
	    {
		لكل (صحيح idx = 0; idx < DB_شمېر; ++idx)
		    إذا (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			اكسر;
		    }
	    }
	}
    }
    أرجع پایله;
}

/*
 * This is called when 'diffopt' is changed.
 */
    صحيح
diffopt_بدل_شوی(فراغ)
{
    char_u	*p;
    صحيح		diff_context_new = 6;
    صحيح		linematch_کرښې_new = 0;
    صحيح		diff_flags_new = 0;
    صحيح		diff_foldcolumn_new = 2;
    اوږد	diff_algorithm_new = 0;
    اوږد	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    طالما (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	إذا (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	وإلا إذا (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	وإلا إذا (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	وإلا إذا (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	وإلا إذا (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	وإلا إذا (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	وإلا إذا (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	وإلا إذا (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	وإلا إذا (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	وإلا إذا (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	وإلا إذا (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	وإلا إذا (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	وإلا إذا (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	وإلا إذا (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	وإلا إذا (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	وإلا إذا (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_دننی;
	}
	وإلا إذا (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    إذا (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    وإلا إذا (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_اصغري;
	    }
	    وإلا إذا (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    وإلا إذا (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    وإلا
		أرجع FAIL;
	}
	وإلا إذا (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    إذا (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    وإلا إذا (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    وإلا إذا (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    وإلا إذا (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_کلمه;
	    }
	    وإلا
		أرجع FAIL;
	}
	وإلا إذا (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_کرښې_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    إذا ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	أرجع FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    إذا (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_کرښې = linematch_کرښې_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    أرجع OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    صحيح
diffopt_horizontal(فراغ)
{
    أرجع (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    صحيح
diffopt_hiddenoff(فراغ)
{
    أرجع (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    صحيح
diffopt_closeoff(فراغ)
{
    أرجع (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    فراغ
diff_update_کرښه(linenr_T lnum)
{
    صحيح		idx;
    diff_T	*dp;

    إذا (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	أرجع;

    idx = diff_buf_idx(curbuf);
    إذا (idx == DB_شمېر)
	أرجع;
    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
	إذا (lnum <= dp->df_lnum[idx] + dp->df_شمېر[idx])
	    اكسر;

    // clear the inline change cache as it's invalid
    إذا (dp != عدم)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ساكن diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    صحيح
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    صحيح *change_پیل,
    صحيح *change_پای_ټکی)
{
    إذا (change->dc_پیل_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_پیل = 0;
    وإلا
	*change_پیل = change->dc_پیل[diffline->bufidx];
    إذا (change->dc_پای_ټکی_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_پای_ټکی = لوی_عدد;
    وإلا
	*change_پای_ټکی = change->dc_پای_ټکی[diffline->bufidx];

    إذا (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	أرجع FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    لكل (صحيح i = 0; i < DB_شمېر; i++)
    {
	إذا (i == diffline->bufidx)
	    استمر;
	إذا (change->dc_پیل[i] != change->dc_پای_ټکی[i]
		|| change->dc_پای_ټکی_lnum_off[i] != change->dc_پیل_lnum_off[i])
	{
	    أرجع FALSE;
	}
    }
    أرجع TRUE;
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
    ساكن صحيح
diff_پیدا_کړه_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    صحيح		idx,
    صحيح		*startp,	// first char of the change
    صحيح		*endp)		// last char of the change
{
    char_u	*کرښه_org;
    char_u	*کرښه_new;
    صحيح		i;
    صحيح		si_org, si_new;
    صحيح		ei_org, ei_new;
    صحيح		off;
    صحيح		added = TRUE;
    char_u	*p1, *p2;
    صحيح		l;

    إذا (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	کرښه_org = عدم;
    }
    وإلا
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	کرښه_org = vim_strsave(ml_get_buf(wp->w_بفر, lnum, FALSE));
	إذا (کرښه_org == عدم)
	    أرجع FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    لكل (i = 0; i < DB_شمېر; ++i)
	إذا (curtab->tp_diffbuf[i] != عدم && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    إذا (off >= dp->df_شمېر[i])
		استمر;
	    added = FALSE;
	    إذا (diff_flags & DIFF_INLINE_NONE)
		اكسر; // early terminate as we only care about the return value

	    کرښه_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    طالما (کرښه_org[si_org] != NUL)
	    {
		إذا (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(کرښه_org[si_org])
					      && VIM_ISWHITE(کرښه_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(کرښه_org[si_org])
					    || VIM_ISWHITE(کرښه_new[si_new]))))
		{
		    si_org = (صحيح)(skipwhite(کرښه_org + si_org) - کرښه_org);
		    si_new = (صحيح)(skipwhite(کرښه_new + si_new) - کرښه_new);
		}
		وإلا
		{
		    إذا (!diff_equal_char(کرښه_org + si_org, کرښه_new + si_new,
									   &l))
			اكسر;
		    si_org += l;
		    si_new += l;
		}
	    }
	    إذا (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_سر_off)(کرښه_org, کرښه_org + si_org);
		si_new -= (*mb_سر_off)(کرښه_new, کرښه_new + si_new);
	    }
	    إذا (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    إذا (کرښه_org[si_org] != NUL || کرښه_new[si_new] != NUL)
	    {
		ei_org = (صحيح)STRLEN(کرښه_org);
		ei_new = (صحيح)STRLEN(کرښه_new);
		طالما (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    إذا (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(کرښه_org[ei_org])
					      && VIM_ISWHITE(کرښه_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(کرښه_org[ei_org])
					    || VIM_ISWHITE(کرښه_new[ei_new]))))
		    {
			طالما (ei_org >= *startp
					     && VIM_ISWHITE(کرښه_org[ei_org]))
			    --ei_org;
			طالما (ei_new >= si_new
					     && VIM_ISWHITE(کرښه_new[ei_new]))
			    --ei_new;
		    }
		    وإلا
		    {
			p1 = کرښه_org + ei_org;
			p2 = کرښه_new + ei_new;
			p1 -= (*mb_سر_off)(کرښه_org, p1);
			p2 -= (*mb_سر_off)(کرښه_new, p2);
			إذا (!diff_equal_char(p1, p2, &l))
			    اكسر;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		إذا (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(کرښه_org);
    أرجع added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
تعريفنوع بنية
{
    اوږد byte_پیل;
    اوږد عدد_بایټونه;
    صحيح lineoff;
} linemap_ثبت_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ساكن فراغ
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, صحيح idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    صحيح pass = 1;
    افعل
    {
	صحيح has_unmerged_gaps = FALSE;
	صحيح has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	طالما (dp!= عدم && dp->df_بل != عدم)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    إذا (dp->df_lnum[idx1] + dp->df_شمېر[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_بل->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_بل;
		استمر;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_ثبت_T *entry1 = &((linemap_ثبت_T*)linemap[idx1].ga_معلومات)[dp->df_lnum[idx1] + dp->df_شمېر[idx1] - 1];
	    linemap_ثبت_T *entry2 = &((linemap_ثبت_T*)linemap[idx1].ga_معلومات)[dp->df_بل->df_lnum[idx1] - 1];
	    إذا (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_بل;
		استمر;
	    }

	    linenr_T gap = dp->df_بل->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_شمېر[idx1]);
	    إذا (gap <= 3)
	    {
		linenr_T max_df_شمېر = 0;
		لكل (صحيح i = 0; i < DB_شمېر; i++)
		    max_df_شمېر = MAX(max_df_شمېر, dp->df_شمېر[i] + dp->df_بل->df_شمېر[i]);

		إذا (max_df_شمېر >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    لكل (صحيح i = 0; i < DB_شمېر; i++)
		    {
			dp->df_شمېر[i] = dp->df_بل->df_lnum[i]
			    + dp->df_بل->df_شمېر[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_بل = dp->df_بل;
		    dp->df_بل = dp_بل->df_بل;
		    clear_diffblock(dp_بل);
		    has_merged_gaps = TRUE;
		    استمر;
		}
		وإلا
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_بل;
	}
	إذا (!has_unmerged_gaps || !has_merged_gaps)
	    اكسر;
    } طالما (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ساكن فراغ
diff_refine_inline_کلمه_highlight(diff_T *dp_orig, garray_T *linemap, صحيح idx1,
				  linenr_T پیل_lnum)
{
    صحيح pass = 1;
    افعل
    {
	diff_T *dp = dp_orig;

	طالما (dp != عدم && dp->df_بل != عدم)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_شمېر[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_بل->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    إذا (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_بل;
		استمر;
	    }

	    linemap_ثبت_T *entry1 =
		&((linemap_ثبت_T *)linemap[idx1].ga_معلومات)[idx_entry1];
	    linemap_ثبت_T *entry2 =
		&((linemap_ثبت_T *)linemap[idx1].ga_معلومات)[idx_entry2];

	    // Skip if blocks are on different lines
	    إذا (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_بل;
		استمر;
	    }

	    // Calculate the gap between blocks
	    صحيح gap_پیل = entry1->byte_پیل + entry1->عدد_بایټونه;
	    صحيح gap_پای_ټکی = entry2->byte_پیل;
	    صحيح gap_اندازه = gap_پای_ټکی - gap_پیل;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    إذا (gap_اندازه <= 0 || gap_اندازه > diff_کلمه_gap)
	    {
		dp = dp->df_بل;
		استمر;
	    }

	    // Get the text between the two blocks
	    char_u *کرښه = ml_get_buf(curtab->tp_diffbuf[idx1],
		    پیل_lnum + entry1->lineoff, FALSE);
	    char_u *gap_متن = کرښه + gap_پیل;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_کلمه = true;
	    bool has_content = false;

	    لكل (صحيح i = 0; i < gap_اندازه && gap_متن[i] != NUL; i++)
	    {
		has_content = true;
		صحيح char_class = mb_get_class_buf(gap_متن + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		إذا (char_class == 2)
		{
		    only_non_کلمه = false;
		    اكسر;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    إذا (has_content && only_non_کلمه)
	    {
		اوږد ټول_change_بایټونه = 0;
		لكل (صحيح i = 0; i < DB_شمېر; i++)
		{
		    إذا (curtab->tp_diffbuf[i] != عدم)
		    {
			// count bytes in the first block
			لكل (صحيح k = 0; k < dp->df_شمېر[i]; k++)
			{
			    صحيح idx = dp->df_lnum[i] + k - 1;
			    إذا (idx < linemap[i].ga_len)
				ټول_change_بایټونه +=
				    ((linemap_ثبت_T *)linemap[i].ga_معلومات)[idx].عدد_بایټونه;
			}
			// count bytes in the next block
			لكل (صحيح k = 0; k < dp->df_بل->df_شمېر[i]; k++)
			{
			    صحيح idx = dp->df_بل->df_lnum[i] + k - 1;
			    إذا (idx < linemap[i].ga_len)
				ټول_change_بایټونه +=
				    ((linemap_ثبت_T *)linemap[i].ga_معلومات)[idx].عدد_بایټونه;
			}
		    }
		}

		إذا (ټول_change_بایټونه >= gap_اندازه * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    لكل (صحيح i = 0; i < DB_شمېر; i++)
		    {
			إذا (curtab->tp_diffbuf[i] != عدم)
			{
			    dp->df_شمېر[i] = dp->df_بل->df_lnum[i]
				+ dp->df_بل->df_شمېر[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_بل = dp->df_بل;
		    dp->df_بل = dp_بل->df_بل;
		    clear_diffblock(dp_بل);

		    // Don't advance dp, check if can merge with the next block too
		    استمر;
		}
	    }

	    dp = dp->df_بل;
	}
    } طالما (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ساكن فراغ
diff_پیدا_کړه_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_شمېر];
    garray_T	file1_str;
    garray_T	file2_str;
    صحيح		file1_idx = -1;

    اوږد	save_diff_algorithm = diff_algorithm;

    CLEAR_ډګر(dio);
    ga_init2(&dio.dio_diff.dout_ga, حجم(حرف *), 1000);

    // inline diff only supports internal algo
    dio.dio_دننی = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_لومړی_diff;
    curtab->tp_لومړی_diff = عدم;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    //بفر
    buf_T	*(orig_diffbuf[DB_شمېر]);
    حافظه_نقل(orig_diffbuf, curtab->tp_diffbuf, حجم(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    لكل (صحيح i = 0; i < DB_شمېر; i++)
	ga_init2(&linemap[i], حجم(linemap_ثبت_T), 128);

    لكل (صحيح i = 0; i < DB_شمېر; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
	    استمر; // skip buffer that isn't loaded

	إذا (dp->df_شمېر[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = عدم;
	    استمر;
	}

	إذا (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	لكل (صحيح off = 0; off < dp->df_شمېر[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    صحيح دننه_keyword = FALSE;

	    // iwhiteeol support vars
	    صحيح وروستی_white = FALSE;
	    صحيح eol_ga_len = -1;
	    صحيح eol_linemap_len = -1;
	    صحيح eol_numlines = -1;

	    char_u *s;
	    لكل (s = curline; *s != NUL;)
	    {
		صحيح new_دننه_keyword = FALSE;
		إذا (diff_flags & DIFF_INLINE_کلمه)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_دننه_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		إذا (دننه_keyword && !new_دننه_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		إذا (VIM_ISWHITE(*s))
		{
		    إذا (diff_flags & DIFF_IWHITEALL)
		    {
			دننه_keyword = FALSE;
			s = skipwhite(s);
			استمر;
		    }
		    وإلا إذا ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			إذا (!وروستی_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    وروستی_white = TRUE;
			}
		    }
		}
		وإلا
		{
		    إذا ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			وروستی_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		صحيح char_len = 1;
		إذا (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		وإلا
		{
		    char_len = mb_ptr2len(s);

		    إذا (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    إذا (diff_flags & DIFF_ICASE)
		    {
			صحيح c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			صحيح c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			صحيح c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			إذا (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    وإلا
			ga_concat_len(curstr, s, char_len);
		}

		إذا (!new_دننه_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		إذا (!new_دننه_keyword || (new_دننه_keyword && !دننه_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_ثبت_T linemap_ثبت;
		    linemap_ثبت.lineoff = off;
		    linemap_ثبت.byte_پیل = s - curline;
		    linemap_ثبت.عدد_بایټونه = char_len;
		    إذا (ga_grow(&linemap[i], 1) != OK)
			اذهب بشپړ;
		    ((linemap_ثبت_T*)(linemap[i].ga_معلومات))[linemap[i].ga_len]
			= linemap_ثبت;
		    linemap[i].ga_len += 1;
		}
		وإلا
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_ثبت_T*)linemap[i].ga_معلومات)[linemap[i].ga_len-1].عدد_بایټونه
			+= char_len;
		}

		دننه_keyword = new_دننه_keyword;
		s += char_len;
	    }
	    إذا (دننه_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    إذا ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		إذا (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    إذا (!(diff_flags & DIFF_IWHITEALL))
	    {
		// Add an empty line token mapped to the end-of-line in the
		// original file. This helps diff newline differences among
		// files, which will be visualized when using 'list' as the eol
		// listchar will be highlighted.
		ga_append(curstr, NL);
		numlines++;

		linemap_ثبت_T linemap_ثبت;
		linemap_ثبت.lineoff = off;
		linemap_ثبت.byte_پیل = s - curline;
		linemap_ثبت.عدد_بایټونه = حجم(NL);
		إذا (ga_grow(&linemap[i], 1) != OK)
		    اذهب بشپړ;
		((linemap_ثبت_T*)(linemap[i].ga_معلومات))[linemap[i].ga_len]
		    = linemap_ثبت;
		linemap[i].ga_len += 1;
	    }
	}

	إذا (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (حرف *)curstr->ga_معلومات;
	    dio.dio_new.din_mmfile.اندازه = curstr->ga_len;
	}
	وإلا
	{
	    dio.dio_orig.din_mmfile.ptr = (حرف *)curstr->ga_معلومات;
	    dio.dio_orig.din_mmfile.اندازه = curstr->ga_len;
	}
	إذا (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    صحيح diff_حالت = diff_file_دننی(&dio);
	    إذا (diff_حالت == FAIL)
		اذهب بشپړ;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_لومړی_diff;

    إذا (diff_flags & DIFF_INLINE_کلمه && file1_idx != -1)
	diff_refine_inline_کلمه_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    وإلا إذا (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    لكل (; new_diff != عدم; new_diff = new_diff->df_بل)
    {
	diffline_change_T change;
	CLEAR_ډګر(change);
	لكل (صحيح i = 0; i < DB_شمېر; i++)
	{
	    إذا (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		استمر;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_پای_ټکی = diff_lnum + new_diff->df_شمېر[i];

	    إذا (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_پیل[i] = MAXCOL;
		change.dc_پیل_lnum_off[i] = لوی_عدد;
	    }
	    وإلا
	    {
		change.dc_پیل[i] = ((linemap_ثبت_T*)linemap[i].ga_معلومات)[diff_lnum].byte_پیل;
		change.dc_پیل_lnum_off[i] = ((linemap_ثبت_T*)linemap[i].ga_معلومات)[diff_lnum].lineoff;
	    }

	    إذا (diff_lnum == diff_lnum_پای_ټکی)
	    {
		change.dc_پای_ټکی[i] = change.dc_پیل[i];
		change.dc_پای_ټکی_lnum_off[i] = change.dc_پیل_lnum_off[i];
	    }
	    وإلا إذا (diff_lnum_پای_ټکی - 1 >= linemap[i].ga_len)
	    {
		change.dc_پای_ټکی[i] = MAXCOL;
		change.dc_پای_ټکی_lnum_off[i] = لوی_عدد;
	    }
	    وإلا
	    {
		change.dc_پای_ټکی[i] = ((linemap_ثبت_T*)linemap[i].ga_معلومات)[diff_lnum_پای_ټکی-1].byte_پیل +
		    ((linemap_ثبت_T*)linemap[i].ga_معلومات)[diff_lnum_پای_ټکی-1].عدد_بایټونه;
		change.dc_پای_ټکی_lnum_off[i] = ((linemap_ثبت_T*)linemap[i].ga_معلومات)[diff_lnum_پای_ټکی-1].lineoff;
	    }
	}
	إذا (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    اذهب بشپړ;
	}
	((diffline_change_T*)(dp->df_changes.ga_معلومات))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

بشپړ:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_لومړی_diff = orig_diff;
    حافظه_نقل(curtab->tp_diffbuf, orig_diffbuf, حجم(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    لكل (صحيح i = 0; i < DB_شمېر; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    صحيح
diff_پیدا_کړه_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    صحيح		idx;
    صحيح		off;

    idx = diff_buf_idx(wp->w_بفر);
    إذا (idx == DB_شمېر)	// cannot happen
	أرجع FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
	إذا (lnum < dp->df_lnum[idx] + dp->df_شمېر[idx])
	    اكسر;
    إذا (dp == عدم || diff_check_sanity(curtab, dp) == FAIL)
	أرجع FALSE;

    إذا (lnum - dp->df_lnum[idx] > لوی_عدد)
	// Integer overflow protection
	أرجع FALSE;
    off = lnum - dp->df_lnum[idx];

    إذا (!(diff_flags & ALL_INLINE_DIFF) || diff_دننی_failed())
    {
	// Use simple algorithm
	صحيح	change_پیل = MAXCOL;	// first col of changed area
	صحيح	change_پای_ټکی = -1;	// last col of changed area
	صحيح	ret;

	ret = diff_پیدا_کړه_change_simple(wp, lnum, dp, idx, &change_پیل, &change_پای_ټکی);

	// convert from inclusive end to exclusive end per diffline's contract
	change_پای_ټکی += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_ډګر(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->عدد_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_پیل[idx] = change_پیل;
	simple_diffline_change.dc_پای_ټکی[idx] = change_پای_ټکی;
	simple_diffline_change.dc_پیل_lnum_off[idx] = off;
	simple_diffline_change.dc_پای_ټکی_lnum_off[idx] = off;
	أرجع ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    إذا (!dp->has_changes)
	diff_پیدا_کړه_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    صحيح عدد_changes = 0;
    صحيح change_idx = 0;
    diffline->changes = عدم;
    لكل (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_معلومات)[change_idx];
	إذا (change->dc_پای_ټکی_lnum_off[idx] < off)
	    استمر;
	إذا (change->dc_پیل_lnum_off[idx] > off)
	    اكسر;
	إذا (diffline->changes == عدم)
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
    صحيح added = FALSE;
    إذا (عدد_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	لكل (صحيح i = 0; i < DB_شمېر; i++)
	{
	    إذا (idx == i)
		استمر;
	    إذا (curtab->tp_diffbuf[i] == عدم)
		استمر;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_معلومات)[dp->df_changes.ga_len-1];
	    إذا (change->dc_پیل_lnum_off[i] != لوی_عدد)
	    {
		added = FALSE;
		اكسر;
	    }
	}
    }
    أرجع added;
}

# إذا defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    صحيح
diff_infold(win_T *wp, linenr_T lnum)
{
    صحيح		i;
    صحيح		idx = -1;
    صحيح		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    إذا (!wp->w_p_diff)
	أرجع FALSE;

    لكل (i = 0; i < DB_شمېر; ++i)
    {
	إذا (curtab->tp_diffbuf[i] == wp->w_بفر)
	    idx = i;
	وإلا إذا (curtab->tp_diffbuf[i] != عدم)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    إذا (idx == -1 || !other)
	أرجع FALSE;

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    إذا (curtab->tp_لومړی_diff == عدم)
	أرجع TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = عدم;		// last block with df_lnum[idx] <= lnum
    صحيح		پایله = TRUE;
    لكل (dp = diff_لټون_پیل(curtab, idx, lnum); dp != عدم; dp = dp->df_بل)
    {
	إذا (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	إذا (dp->df_lnum[idx] - diff_context > lnum)
	    اكسر;
	// If this change ends before the line we have a match.
	إذا (dp->df_lnum[idx] + dp->df_شمېر[idx] + diff_context > lnum)
	{
	    پایله = FALSE;
	    اكسر;
	}
    }
    diff_لټون_remember(curtab, idx, lnum, le);
    أرجع پایله;
}
# endif

/*
 * "dp" and "do" commands.
 */
    فراغ
nv_diffgetput(صحيح put, اوږد شمېر)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_دنده_CHANNEL
    إذا (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	أرجع;
    }
# endif
    إذا (شمېر == 0)
	ea.arg = (char_u *)"";
    وإلا
    {
	vim_snprintf((حرف *)buf, 30, "%ld", شمېر);
	ea.arg = buf;
    }
    إذا (put)
	ea.cmdidx = CMD_diffput;
    وإلا
	ea.cmdidx = CMD_diffget;
    ea.addr_شمېر = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ساكن صحيح
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
	إذا (dp == diff)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    فراغ
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    صحيح		شمېر;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    صحيح		idx_cur;
    صحيح		idx_other;
    صحيح		idx_from;
    صحيح		idx_منزل;
    صحيح		i;
    صحيح		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    صحيح		پیل_skip, پای_ټکی_skip;
    صحيح		new_شمېر;
    صحيح		buf_تشه;
    صحيح		وموندل_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    إذا (idx_cur == DB_شمېر)
    {
	emsg(_(e_current_بفر_is_not_دننه_diff_ډېرپېښ));
	أرجع;
    }

    إذا (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	لكل (idx_other = 0; idx_other < DB_شمېر; ++idx_other)
	    إذا (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != عدم)
	    {
		إذا (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    اكسر;
		وموندل_not_ma = TRUE;
	    }
	إذا (idx_other == DB_شمېر)
	{
	    إذا (وموندل_not_ma)
		emsg(_(e_no_other_بفر_دننه_diff_ډېرپېښ_is_modifiable));
	    وإلا
		emsg(_(e_no_other_بفر_دننه_diff_ډېرپېښ));
	    أرجع;
	}

	// Check that there isn't a third buffer in the list
	لكل (i = idx_other + 1; i < DB_شمېر; ++i)
	    إذا (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != عدم
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_نور_than_two_buffers_دننه_diff_ډېرپېښ_dont_know_which_one_منزل_use));
		أرجع;
	    }
    }
    وإلا
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	طالما (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	لكل (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	إذا (eap->arg + i == p)	    // digits only
	    i = atol((حرف *)eap->arg);
	وإلا
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    إذا (i < 0)
		أرجع;		// error message already given
	}
	buf = buflist_findnr(i);
	إذا (buf == عدم)
	{
	    semsg(_(e_cant_پیدا_کړه_بفر_str), eap->arg);
	    أرجع;
	}
	إذا (buf == curbuf)
	    أرجع;		// nothing to do
	idx_other = diff_buf_idx(buf);
	إذا (idx_other == DB_شمېر)
	{
	    semsg(_(e_بفر_str_is_not_دننه_diff_ډېرپېښ), eap->arg);
	    أرجع;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    إذا (eap->addr_شمېر == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	صحيح linestatus = 0;
	إذا (eap->line1 == curbuf->b_ml.ml_کرښه_شمېر
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	وإلا إذا (eap->line1 > 0)
	    --eap->line1;
    }

    إذا (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_منزل = idx_cur;
    }
    وإلا
    {
	idx_from = idx_cur;
	idx_منزل = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	إذا (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    اذهب theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    إذا (!curbuf->b_بدل_شوی)
    {
	change_warning(0);
	إذا (diff_buf_idx(curbuf) != idx_منزل)
	{
	    emsg(_(e_بفر_بدل_شوی_unexpectedly));
	    اذهب theend;
	}
    }

    dprev = عدم;
    لكل (dp = curtab->tp_لومړی_diff; dp != عدم; )
    {
	إذا (!eap->addr_شمېر)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    طالما (dp->df_بل
		    && dp->df_بل->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_شمېر[idx_cur]
		    && dp->df_بل->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_بل;
	    }
	}
	إذا (dp->df_lnum[idx_cur] > eap->line2 + off)
	    اكسر;	// past the range that was specified

	dfree = عدم;
	lnum = dp->df_lnum[idx_منزل];
	شمېر = dp->df_شمېر[idx_منزل];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + شمېر + (شمېر == 0 && BUFEMPTY() ? 1 : 0);

	إذا (dp->df_lnum[idx_cur] + dp->df_شمېر[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    پیل_skip = 0;
	    پای_ټکی_skip = 0;
	    إذا (eap->addr_شمېر > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		پیل_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		إذا (پیل_skip > 0)
		{
		    // range starts below start of current diff block
		    إذا (پیل_skip > شمېر)
		    {
			lnum += شمېر;
			شمېر = 0;
		    }
		    وإلا
		    {
			شمېر -= پیل_skip;
			lnum += پیل_skip;
		    }
		}
		وإلا
		    پیل_skip = 0;

		پای_ټکی_skip = dp->df_lnum[idx_cur] + dp->df_شمېر[idx_cur] - 1
							 - (eap->line2 + off);
		إذا (پای_ټکی_skip > 0)
		{
		    // range ends above end of current/from diff block
		    إذا (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_شمېر[idx_cur] - پیل_skip - پای_ټکی_skip;
			إذا (شمېر > i)
			    شمېر = i;
		    }
		    وإلا			// :diffget
		    {
			شمېر -= پای_ټکی_skip;
			پای_ټکی_skip = dp->df_شمېر[idx_from] - پیل_skip - شمېر;
			إذا (پای_ټکی_skip < 0)
			    پای_ټکی_skip = 0;
		    }
		}
		وإلا
		    پای_ټکی_skip = 0;
	    }

	    buf_تشه = BUFEMPTY();
	    added = 0;
	    لكل (i = 0; i < شمېر; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_تشه = curbuf->b_ml.ml_کرښه_شمېر == 1;
		إذا (ml_delete(lnum) == OK)
		    --added;
	    }
	    لكل (i = 0; i < dp->df_شمېر[idx_from] - پیل_skip - پای_ټکی_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + پیل_skip + i;
		إذا (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_کرښه_شمېر)
		    اكسر;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		إذا (p != عدم)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    إذا (buf_تشه && curbuf->b_ml.ml_کرښه_شمېر == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_تشه = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_شمېر = dp->df_شمېر[idx_منزل] + added;
	    dp->df_شمېر[idx_منزل] = new_شمېر;

	    إذا (پیل_skip == 0 && پای_ټکی_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		لكل (i = 0; i < DB_شمېر; ++i)
		    إذا (curtab->tp_diffbuf[i] != عدم && i != idx_from
								&& i != idx_منزل
			    && !diff_equal_ثبت(dp, idx_from, i))
			اكسر;
		إذا (i == DB_شمېر)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_بل;
		    إذا (dprev == عدم)
			curtab->tp_لومړی_diff = dp;
		    وإلا
			dprev->df_بل = dp;
		}
	    }

	    إذا (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + شمېر - 1, (اوږد)MAXLNUM, (اوږد)added);
		إذا (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    إذا (curwin->w_cursor.lnum >= lnum + شمېر)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_کرښه_شمېر);
		    }
		    وإلا إذا (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    بدل_شوی_کرښې(lnum, 0, lnum + شمېر, (اوږد)added);

	    إذا (dfree != عدم)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_منزل);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    إذا (added != 0 && !valid_diff(dp))
		اكسر;

	    إذا (dfree == عدم)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_شمېر[idx_منزل] = new_شمېر;

	    // When changing the current buffer, keep track of line numbers
	    إذا (idx_cur == idx_منزل)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	إذا (dfree == عدم)
	{
	    dprev = dp;
	    dp = dp->df_بل;
	}
    }

    // restore curwin/curbuf and a few other things
    إذا (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	إذا (کلی_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    إذا (diff_need_update)
	ex_diffupdate(عدم);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    بدل_شوی_کرښه_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    إذا (curtab->tp_لومړی_diff == عدم)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_دننه_TAB(curtab, wp)
	    إذا (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    إذا (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    وإلا
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	کړکۍ_layout_قلف();
	apply_autocmds(EVENT_DIFFUPDATED, عدم, عدم, FALSE, curbuf);
	کړکۍ_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ساكن فراغ
diff_fold_update(diff_T *dp, صحيح skip_idx)
{
    صحيح		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	لكل (i = 0; i < DB_شمېر; ++i)
	    إذا (curtab->tp_diffbuf[i] == wp->w_بفر && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_شمېر[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    صحيح
diff_ډېرپېښ_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	إذا (diff_buf_idx_tp(buf, tp) != DB_شمېر)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    صحيح
diff_move_منزل(صحيح dir, اوږد شمېر)
{
    صحيح		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    إذا (idx == DB_شمېر || curtab->tp_لومړی_diff == عدم)
	أرجع FAIL;

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    إذا (curtab->tp_لومړی_diff == عدم)		// no diffs today
	أرجع FAIL;

    طالما (--شمېر >= 0)
    {
	// Check if already before first diff.
	إذا (dir == BACKWARD && lnum <= curtab->tp_لومړی_diff->df_lnum[idx])
	    اكسر;

	لكل (dp = curtab->tp_لومړی_diff; ; dp = dp->df_بل)
	{
	    إذا (dp == عدم)
		اكسر;
	    إذا ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_بل == عدم
			    || lnum <= dp->df_بل->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		اكسر;
	    }
	}
    }

    // don't end up past the end of the file
    إذا (lnum > curbuf->b_ml.ml_کرښه_شمېر)
	lnum = curbuf->b_ml.ml_کرښه_شمېر;

    // When the cursor didn't move at all we fail.
    إذا (lnum == curwin->w_cursor.lnum)
	أرجع FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ستون = 0;

    أرجع OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ساكن linenr_T
diff_get_corresponding_کرښه_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    صحيح		idx1;
    صحيح		idx2;
    diff_T	*dp;
    صحيح		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    إذا (idx1 == DB_شمېر || idx2 == DB_شمېر || curtab->tp_لومړی_diff == عدم)
	أرجع lnum1;

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    إذا (curtab->tp_لومړی_diff == عدم)		// no diffs today
	أرجع lnum1;

    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
    {
	إذا (dp->df_lnum[idx1] > lnum1)
	    أرجع lnum1 - baseline;
	إذا ((dp->df_lnum[idx1] + dp->df_شمېر[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    إذا (baseline > dp->df_شمېر[idx2])
		baseline = dp->df_شمېر[idx2];

	    أرجع dp->df_lnum[idx2] + baseline;
	}
	إذا (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_شمېر[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_شمېر[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    أرجع curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_شمېر[idx1])
				   - (dp->df_lnum[idx2] + dp->df_شمېر[idx2]);
    }

    // If we get here then the cursor is after the last diff
    أرجع lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_کرښه(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_کرښه_int(buf1, lnum1);

    // don't end up past the end of the file
    إذا (lnum > curbuf->b_ml.ml_کرښه_شمېر)
	أرجع curbuf->b_ml.ml_کرښه_شمېر;
    أرجع lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    صحيح		idx;
    صحيح		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    إذا (idx == DB_شمېر)		// safety check
	أرجع (linenr_T)0;

    إذا (curtab->tp_diff_invalid)
	ex_diffupdate(عدم);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_دننه_TAB(curtab, dp)
	إذا (lnum <= dp->df_lnum[idx] + dp->df_شمېر[idx])
	    اكسر;

    // When after the last change, compute relative to the last line number.
    إذا (dp == عدم)
	أرجع wp->w_بفر->b_ml.ml_کرښه_شمېر
					- (curbuf->b_ml.ml_کرښه_شمېر - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_بفر);
    إذا (i == DB_شمېر)			// safety check
	أرجع (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    إذا (n > dp->df_lnum[i] + dp->df_شمېر[i])
	n = dp->df_lnum[i] + dp->df_شمېر[i];
    أرجع n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ساكن صحيح
parse_diff_ed(
	char_u	    *کرښه,
	diffhunk_T  *hunk)
{
    char_u *p;
    اوږد    f1, l1, f2, l2;
    صحيح	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = کرښه;
    f1 = getdigits(&p);
    إذا (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    وإلا
	l1 = f1;
    إذا (*p != 'a' && *p != 'c' && *p != 'd')
	أرجع FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    إذا (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    وإلا
	l2 = f2;
    إذا (l1 < f1 || l2 < f2)
	أرجع FAIL;

    إذا (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->شمېر_orig = 0;
    }
    وإلا
    {
	hunk->lnum_orig = f1;
	hunk->شمېر_orig = l1 - f1 + 1;
    }
    إذا (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->شمېر_new = 0;
    }
    وإلا
    {
	hunk->lnum_new = f2;
	hunk->شمېر_new = l2 - f2 + 1;
    }
    أرجع OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ساكن صحيح
parse_diff_unified(
	char_u	    *کرښه,
	diffhunk_T  *hunk)
{
    char_u *p;
    اوږد    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = کرښه;
    إذا (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	إذا (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	وإلا
	    oldcount = 1;
	إذا (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    إذا (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    وإلا
		newcount = 1;
	}
	وإلا
	    أرجع FAIL;	// invalid diff format

	إذا (oldcount == 0)
	    oldline += 1;
	إذا (newcount == 0)
	    newline += 1;
	إذا (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->شمېر_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->شمېر_new = newcount;

	أرجع OK;
    }

    أرجع FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ساكن صحيح
xdiff_بهر_indices(
	اوږد پیل_a,
	اوږد شمېر_a,
	اوږد پیل_b,
	اوږد شمېر_b,
	فراغ *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    إذا (p == عدم)
	أرجع -1;

    إذا (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	أرجع -1;
    }

    p->lnum_orig  = پیل_a + 1;
    p->شمېر_orig = شمېر_a;
    p->lnum_new   = پیل_b + 1;
    p->شمېر_new  = شمېر_b;
    ((diffhunk_T **)dout->dout_ga.ga_معلومات)[dout->dout_ga.ga_len++] = p;
    أرجع 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ساكن صحيح
xdiff_بهر_unified(
	فراغ *priv,
	mmbuffer_t *mb,
	صحيح nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    صحيح		i;

    لكل (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].اندازه);

    أرجع 0;
}

#د_که_پای	// FEAT_DIFF

#إذا defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    فراغ
f_diff_filler(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
# ifdef FEAT_DIFF
    إذا (دننه_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_شمېره = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    فراغ
f_diff_hlID(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ساكن linenr_T	prev_lnum = 0;
    ساكن varnumber_T	changedtick = 0;
    ساكن صحيح		fnum = 0;
    ساكن صحيح		prev_diff_flags = 0;
    ساكن صحيح		change_پیل = 0;
    ساكن صحيح		change_پای_ټکی = 0;
    ساكن hlf_T	hlID = (hlf_T)0;
    صحيح			cache_results = TRUE;
    صحيح			ستون;
    diffline_T		diffline;

    CLEAR_ډګر(diffline);

    إذا (دننه_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    إذا (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    إذا (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	صحيح linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	إذا (linestatus < 0)
	{
	    إذا (linestatus == -1)
	    {
		change_پیل = MAXCOL;
		change_پای_ټکی = -1;
		إذا (diff_پیدا_کړه_change(curwin, lnum, &diffline))
		    hlID = HLF_جمع_کړه;	// added line
		وإلا
		{
		    hlID = HLF_CHD;	// changed line
		    إذا (diffline.عدد_changes > 0 && cache_results)
		    {
			change_پیل = diffline.changes[0].dc_پیل[diffline.bufidx];
			change_پای_ټکی = diffline.changes[0].dc_پای_ټکی[diffline.bufidx];
		    }
		}
	    }
	    وإلا
		hlID = HLF_جمع_کړه;	// added line
	}
	وإلا
	    hlID = (hlf_T)0;

	إذا (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    إذا (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	ستون = tv_get_شمېره(&argvars[1]) - 1; // ignore type error in {col}
	إذا (cache_results)
	{
	    إذا (ستون >= change_پیل && ستون < change_پای_ټکی)
		hlID = HLF_TXD;			// changed text
	    وإلا
		hlID = HLF_CHD;			// changed line
	}
	وإلا
	{
	    hlID = HLF_CHD;
	    لكل (صحيح i = 0; i < diffline.عدد_changes; i++)
	    {
		صحيح added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_پیل, &change_پای_ټکی);
		إذا (ستون >= change_پیل && ستون < change_پای_ټکی)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    اكسر;
		}
		إذا (ستون < change_پیل)
		    // the remaining changes are past this column and not relevant
		    اكسر;
	    }
	}
    }
    rettv->vval.v_شمېره = hlID == (hlf_T)0 ? 0 : (صحيح)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ساكن صحيح
parse_diff_optarg(
    typval_T	    *opts,
    صحيح		    *diffopts,
    اوږد	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    صحيح		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    إذا (algo != عدم)
    {
	إذا (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	وإلا إذا (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_اصغري;
	وإلا إذا (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	وإلا إذا (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    إذا (output_fmt != عدم)
    {
	إذا (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	وإلا إذا (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	وإلا
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    أرجع FAIL;
	}
    }

    *diff_ctxlen = dict_get_شمېره_def(d, "context", 0);
    إذا (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    إذا (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    إذا (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    إذا (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    إذا (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    إذا (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    إذا (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    أرجع OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ساكن فراغ
list_منزل_diffin(list_T *l, diffin_T *din, صحيح icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_توکي(l, li)
    {
	str = tv_get_string(&li->li_tv);
	إذا (icase)
	{
	    str = strlow_save(str);
	    إذا (str == عدم)
		استمر;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	إذا (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (حرف *)ga.ga_معلومات;
    din->din_mmfile.اندازه = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ساكن dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    إذا (hunk_dict == عدم)
	أرجع عدم;

    dict_جمع_کړه_شمېره(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_جمع_کړه_شمېره(hunk_dict, "from_count", hunk->شمېر_orig);
    dict_جمع_کړه_شمېره(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_جمع_کړه_شمېره(hunk_dict, "to_count", hunk->شمېر_new);

    أرجع hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    فراغ
f_diff(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    إذا (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	أرجع;

    CLEAR_ډګر(dio);
    dio.dio_دننی = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, حجم(حرف *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    صحيح		save_diff_flags = diff_flags;
    اوږد	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_دننی;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
	إذا (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    أرجع;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_منزل_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_منزل_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    صحيح	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    إذا (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    صحيح diff_حالت = diff_file(&dio);

    // restore 'diffexpr'
    إذا (restore_diffexpr)
	*p_dex = cc;

    إذا (diff_حالت == FAIL)
	اذهب بشپړ;

    صحيح		hunk_idx = 0;
    dict_T	*hunk_dict;

    إذا (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	إذا (rettv_list_alloc(rettv) != OK)
	    اذهب بشپړ;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = عدم;
	طالما (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_معلومات)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    إذا (hunk_dict == عدم)
		اذهب بشپړ;

	    إذا (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    وإلا
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_معلومات);
    }

بشپړ:
    clear_diffin(&dio.dio_new);
    إذا (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    وإلا
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#د_که_پای
