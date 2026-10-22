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

#include "vim.h"
#include "xdiff/xdiff.h"

#အကယ်၍ defined(FEAT_DIFF)

static ကိန်းပြည့် diff_busy = FALSE;	    // using diff structs, don't change them
static ကိန်းပြည့် diff_need_update = FALSE; // ex_diffupdate needs to be called

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
static ကိန်းပြည့်	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

static long diff_algorithm = XDF_INDENT_HEURISTIC;
static ကိန်းပြည့် diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

static ကိန်းပြည့် diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# အကယ်၍ defined(MSWIN)
static ကိန်းပြည့် diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
typedef sa_nracanaa {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
typedef sa_nracanaa {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
typedef sa_nracanaa {
    linenr_T lnum_orig;
    long     count_orig;
    linenr_T lnum_new;
    long     count_new;
} diffhunk_T;

typedef ga_nanaa {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
typedef sa_nracanaa {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    ကိန်းပြည့်		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    ကိန်းပြည့်		    dio_ctxlen;   // unified diff context length
} diffio_T;

static ကိန်းပြည့် diff_buf_idx(buf_T *buf);
static ကိန်းပြည့် diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
static void diff_mark_adjust_tp(tabpage_T *tp, ကိန်းပြည့် idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
static void diff_check_unchanged(tabpage_T *tp, diff_T *dp);
static ကိန်းပြည့် diff_check_sanity(tabpage_T *tp, diff_T *dp);
static ကိန်းပြည့် check_external_diff(diffio_T *diffio);
static ကိန်းပြည့် diff_file(diffio_T *diffio);
static ကိန်းပြည့် diff_equal_entry(diff_T *dp, ကိန်းပြည့် idx1, ကိန်းပြည့် idx2);
static ကိန်းပြည့် diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
static void diff_fold_update(diff_T *dp, ကိန်းပြည့် skip_idx);
# endif
static void diff_read(ကိန်းပြည့် idx_orig, ကိန်းပြည့် idx_new, diffio_T *dio);
static void diff_copy_entry(diff_T *dprev, diff_T *dp, ကိန်းပြည့် idx_orig, ကိန်းပြည့် idx_new);
static diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
static ကိန်းပြည့် parse_diff_ed(char_u *line, diffhunk_T *hunk);
static ကိန်းပြည့် parse_diff_unified(char_u *line, diffhunk_T *hunk);
static ကိန်းပြည့် xdiff_out_indices(long start_a, long count_a, long start_b, long count_b, void *priv);
static ကိန်းပြည့် xdiff_out_unified(void *priv, mmbuffer_t *mb, ကိန်းပြည့် nbuf);
static ကိန်းပြည့် parse_diffanchors(ကိန်းပြည့် check_only, buf_T *buf, linenr_T *anchors, ကိန်းပြည့် *num_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    အတွက် ((dp) = (tp)->tp_first_diff; (dp) != NULL; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
static diff_T	    *diff_finger_dp = NULL;
static tabpage_T    *diff_finger_tp = NULL;
static ကိန်းပြည့်	    diff_finger_idx = -1;
static linenr_T	    diff_finger_lnum = 0;

    static void
clear_diffblock(diff_T *dp)
{
    အကယ်၍ (dp == diff_finger_dp)
	diff_finger_dp = NULL;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    static diff_T *
diff_scan_start(tabpage_T *tp, ကိန်းပြည့် idx, linenr_T lnum)
{
    အကယ်၍ (diff_finger_dp != NULL
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ပြန်ပေး diff_finger_dp;
    ပြန်ပေး tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    static void
diff_scan_remember(tabpage_T *tp, ကိန်းပြည့် idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != NULL ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    void
diff_buf_delete(buf_T *buf)
{
    ကိန်းပြည့်		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	အကယ်၍ (i != DB_COUNT)
	{
	    tp->tp_diffbuf[i] = NULL;
	    tp->tp_diff_invalid = TRUE;
	    အကယ်၍ (tp == curtab)
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
    void
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    ကိန်းပြည့်		i;

    အကယ်၍ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    အကယ်၍ (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		bha_nga;
	အကယ်၍ (wp == NULL)
	{
	    i = diff_buf_idx(win->w_buffer);
	    အကယ်၍ (i != DB_COUNT)
	    {
		curtab->tp_diffbuf[i] = NULL;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    မဟုတ်ပါက
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
    void
diff_buf_add(buf_T *buf)
{
    ကိန်းပြည့်		i;

    အကယ်၍ (diff_buf_idx(buf) != DB_COUNT)
	ပြန်ပေး;		// It's already there.

    အတွက် (i = 0; i < DB_COUNT; ++i)
	အကယ်၍ (curtab->tp_diffbuf[i] == NULL)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ပြန်ပေး;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_COUNT);
}

/*
 * Remove all buffers to make diffs for.
 */
    static void
diff_buf_clear(void)
{
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < DB_COUNT; ++i)
	အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
	{
	    curtab->tp_diffbuf[i] = NULL;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    static ကိန်းပြည့်
diff_buf_idx(buf_T *buf)
{
    ကိန်းပြည့်		idx;

    အတွက် (idx = 0; idx < DB_COUNT; ++idx)
	အကယ်၍ (curtab->tp_diffbuf[idx] == buf)
	    bha_nga;
    ပြန်ပေး idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    static ကိန်းပြည့်
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    ကိန်းပြည့်		idx;

    အတွက် (idx = 0; idx < DB_COUNT; ++idx)
	အကယ်၍ (tp->tp_diffbuf[idx] == buf)
	    bha_nga;
    ပြန်ပေး idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    void
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    ကိန်းပြည့်		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	အကယ်၍ (i != DB_COUNT)
	{
	    tp->tp_diff_invalid = TRUE;
	    အကယ်၍ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    void
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    ကိန်းပြည့်		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	အကယ်၍ (idx != DB_COUNT)
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
    static void
diff_mark_adjust_tp(
    tabpage_T	*tp,
    ကိန်းပြည့်		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		inserted, deleted;
    ကိန်းပြည့်		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    ကိန်းပြည့်		check_unchanged;

    diff_finger_dp = NULL;		// may shift df_lnum, invalidate the finger

    အကယ်၍ (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    အကယ်၍ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    မဟုတ်ပါက အကယ်၍ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    မဟုတ်ပါက
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    အတွက် (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	အကယ်၍ ((dp == NULL || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == NULL
		    || dprev->df_lnum[idx] + dprev->df_count[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    အကယ်၍ (dnext == NULL)
		ပြန်ပေး;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_count[idx] = inserted;
	    အတွက် (i = 0; i < DB_COUNT; ++i)
		အကယ်၍ (tp->tp_diffbuf[i] != NULL && i != idx)
		{
		    အကယ်၍ (dprev == NULL)
			dnext->df_lnum[i] = line1;
		    မဟုတ်ပါက
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_count[i])
			    - (dprev->df_lnum[idx] + dprev->df_count[idx]);
		    dnext->df_count[i] = deleted;
		}
	}

	// if at end of the list, quit
	အကယ်၍ (dp == NULL)
	    bha_nga;

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
	last = dp->df_lnum[idx] + dp->df_count[idx] - 1;

	// 1. change completely above line1: nothing to do
	အကယ်၍ (last >= line1 - 1)
	{
	    အကယ်၍ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		အကယ်၍ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		continue;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    အကယ်၍ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		အကယ်၍ (amount_after == 0)
		    bha_nga;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    မဟုတ်ပါက
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		အကယ်၍ (deleted > 0)
		{
		    off = 0;
		    အကယ်၍ (dp->df_lnum[idx] >= line1)
		    {
			အကယ်၍ (last <= line2)
			{
			    // 4. delete all lines of diff
			    အကယ်၍ (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_count[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    မဟုတ်ပါက
				n = deleted - dp->df_count[idx];
			    dp->df_count[idx] = 0;
			}
			မဟုတ်ပါက
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_count[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    မဟုတ်ပါက
		    {
			အကယ်၍ (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_count[idx] -= last - lnum_deleted + 1;
			    အကယ်၍ (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    မဟုတ်ပါက
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			မဟုတ်ပါက
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_count[idx] -= deleted;
			}
		    }

		    အတွက် (i = 0; i < DB_COUNT; ++i)
			အကယ်၍ (tp->tp_diffbuf[i] != NULL && i != idx)
			{
			    အကယ်၍ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    မဟုတ်ပါက
				dp->df_lnum[i] = 1;
			    dp->df_count[i] += n;
			}
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_count[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    မဟုတ်ပါက
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		အကယ်၍ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	အကယ်၍ (dprev != NULL && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_count[idx]
							== dp->df_lnum[idx])
	{
	    အတွက် (i = 0; i < DB_COUNT; ++i)
		အကယ်၍ (tp->tp_diffbuf[i] != NULL)
		    dprev->df_count[i] += dp->df_count[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	မဟုတ်ပါက
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    နေစဉ် (dp != NULL)
    {
	// All counts are zero, remove this entry.
	အတွက် (i = 0; i < DB_COUNT; ++i)
	    အကယ်၍ (tp->tp_diffbuf[i] != NULL && dp->df_count[i] != 0)
		bha_nga;
	အကယ်၍ (i == DB_COUNT)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    အကယ်၍ (dprev == NULL)
		tp->tp_first_diff = dnext;
	    မဟုတ်ပါက
		dprev->df_next = dnext;
	}
	မဟုတ်ပါက
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    အကယ်၍ (tp == curtab)
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
    static diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    အကယ်၍ (dnew == NULL)
	ပြန်ပေး NULL;
    diff_finger_dp = NULL;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    အကယ်၍ (dprev == NULL)
	tp->tp_first_diff = dnew;
    မဟုတ်ပါက
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, parimaa_na(diffline_change_T), 20);
    ပြန်ပေး dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    static void
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    ကိန်းပြည့်		i_org;
    ကိန်းပြည့်		i_new;
    ကိန်းပြည့်		off_org, off_new;
    char_u	*line_org;
    ကိန်းပြည့်		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    အတွက် (i_org = 0; i_org < DB_COUNT; ++i_org)
	အကယ်၍ (tp->tp_diffbuf[i_org] != NULL)
	    bha_nga;
    အကယ်၍ (i_org == DB_COUNT)	// safety check
	ပြန်ပေး;

    အကယ်၍ (diff_check_sanity(tp, dp) == FAIL)
	ပြန်ပေး;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    အတွက် (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	နေစဉ် (dp->df_count[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    အကယ်၍ (dir == BACKWARD)
		off_org = dp->df_count[i_org] - 1;
	    line_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    အကယ်၍ (line_org == NULL)
		ပြန်ပေး;
	    အတွက် (i_new = i_org + 1; i_new < DB_COUNT; ++i_new)
	    {
		အကယ်၍ (tp->tp_diffbuf[i_new] == NULL)
		    continue;
		အကယ်၍ (dir == BACKWARD)
		    off_new = dp->df_count[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		အကယ်၍ (off_new < 0 || off_new >= dp->df_count[i_new])
		    bha_nga;
		အကယ်၍ (diff_cmp(line_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    bha_nga;
	    }
	    vim_free(line_org);

	    // Stop when a line isn't equal in all diff buffers.
	    အကယ်၍ (i_new != DB_COUNT)
		bha_nga;

	    // Line matched in all buffers, remove it from the diff.
	    အတွက် (i_new = i_org; i_new < DB_COUNT; ++i_new)
		အကယ်၍ (tp->tp_diffbuf[i_new] != NULL)
		{
		    အကယ်၍ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_count[i_new];
		}
	}
	အကယ်၍ (dir == BACKWARD)
	    bha_nga;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    static ကိန်းပြည့်
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < DB_COUNT; ++i)
	အကယ်၍ (tp->tp_diffbuf[i] != NULL)
	    အကယ်၍ (dp->df_lnum[i] + dp->df_count[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_line_count)
		ပြန်ပေး FAIL;
    ပြန်ပေး OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    void
diff_redraw(
    ကိန်းပြည့်		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = NULL;
    ကိန်းပြည့်		used_max_fill_other = FALSE;
    ကိန်းပြည့်		used_max_fill_curwin = FALSE;
    ကိန်းပြည့်		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	အကယ်၍ (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    continue;

	redraw_win_later(wp, UPD_SOME_VALID);
	အကယ်၍ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	အကယ်၍ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	အကယ်၍ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    အကယ်၍ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    မဟုတ်ပါက အကယ်၍ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		အကယ်၍ (wp == curwin)
		    used_max_fill_curwin = TRUE;
		မဟုတ်ပါက အကယ်၍ (wp_other != NULL)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    အကယ်၍ (wp_other != NULL && curwin->w_p_scb)
    {
	အကယ်၍ (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	မဟုတ်ပါက အကယ်၍ (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    static void
clear_diffin(diffin_T *din)
{
    အကယ်၍ (din->din_fname == NULL)
	VIM_CLEAR(din->din_mmfile.ptr);
    မဟုတ်ပါက
	mch_remove(din->din_fname);
}

    static void
clear_diffout(diffout_T *dout)
{
    အကယ်၍ (dout->dout_fname == NULL)
	ga_clear_strings(&dout->dout_ga);
    မဟုတ်ပါက
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    static ကိန်းပြည့်
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    အကယ်၍ (end < 0)
	end = buf->b_ml.ml_line_count;

    အကယ်၍ (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = NULL;
	din->din_mmfile.size = 0;
	ပြန်ပေး OK;
    }

    // xdiff requires one big block of memory with all the text.
    အတွက် (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    အကယ်၍ (ptr == NULL)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	အကယ်၍ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ပြန်ပေး FAIL;
    }
    din->din_mmfile.ptr = (var_na *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    အတွက် (lnum = start; lnum <= end; ++lnum)
    {
	အတွက် (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    အကယ်၍ (diff_flags & DIFF_ICASE)
	    {
		ကိန်းပြည့် c;
		ကိန်းပြည့်	orig_len;
		ကိန်းပြည့်	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		အကယ်၍ (*s == NL)
		    c = NUL;
		မဟုတ်ပါက
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		အကယ်၍ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		မဟုတ်ပါက
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    အကယ်၍ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    မဟုတ်ပါက
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ပြန်ပေး OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    static ကိန်းပြည့်
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    ကိန်းပြည့်		r;
    ကိန်းပြည့်		save_ml_flags;
    char_u	*save_ff;
    ကိန်းပြည့်		save_cmod_flags;

    အကယ်၍ (din->din_fname == NULL)
	ပြန်ပေး diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    အကယ်၍ (frames_locked())
	ပြန်ပေး FAIL;

    အကယ်၍ (end < 0)
	end = buf->b_ml.ml_line_count;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    အကယ်၍ (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, NULL,
			start, end,
			NULL, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    ပြန်ပေး r;
}

    static ကိန်းပြည့်
lnum_compare(const void *s1, const void *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    အကယ်၍ (lnum1 < lnum2)
	ပြန်ပေး -1;
    အကယ်၍ (lnum1 > lnum2)
	ပြန်ပေး 1;
    ပြန်ပေး 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    static void
diff_try_update(
	diffio_T    *dio,
	ကိန်းပြည့်	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    ကိန်းပြည့်		idx_new;

    အကယ်၍ (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, parimaa_na(var_na *), 1000);
    }
    မဟုတ်ပါက
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	အကယ်၍ (dio->dio_orig.din_fname == NULL
		|| dio->dio_new.din_fname == NULL
		|| dio->dio_diff.dout_fname == NULL)
	    goto theend;
    }

    // Check external diff is actually working.
    အကယ်၍ (!dio->dio_internal && check_external_diff(dio) == FAIL)
	goto theend;

    // :diffupdate!
    အကယ်၍ (eap != NULL && eap->forceit)
	အတွက် (idx_new = idx_orig; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    အကယ်၍ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    ကိန်းပြည့် num_anchors = INT_MAX;
    linenr_T anchors[DB_COUNT][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    အကယ်၍ (diff_flags & DIFF_ANCHOR)
    {
	အတွက် (ကိန်းပြည့် idx = 0; idx < DB_COUNT; idx++)
	{
	    အကယ်၍ (curtab->tp_diffbuf[idx] == NULL)
		continue;
	    ကိန်းပြည့် buf_num_anchors = 0;
	    အကယ်၍ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_num_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		num_anchors = 0;
		CLEAR_FIELD(anchors);
		bha_nga;
	    }
	    အကယ်၍ (buf_num_anchors < num_anchors)
		num_anchors = buf_num_anchors;

	    အကယ်၍ (buf_num_anchors > 0)
		qsort((void *)anchors[idx],
			(size_t)buf_num_anchors,
			parimaa_na(linenr_T),
			lnum_compare);
	}
    }
    အကယ်၍ (num_anchors == INT_MAX)
	num_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    အတွက် (ကိန်းပြည့် anchor_i = 0; anchor_i <= num_anchors; anchor_i++)
    {
	diff_T	*orig_diff = NULL;
	အကယ်၍ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = NULL;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == num_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	အကယ်၍ (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    အကယ်၍ (orig_diff != NULL)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    goto theend;
	}

	// Make a difference between the first buffer and every other.
	အတွက် (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    အကယ်၍ (buf == NULL || buf->b_ml.ml_mfp == NULL)
		continue; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == num_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    အကယ်၍ (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		continue;
	    အကယ်၍ (diff_file(dio) == FAIL)
		continue;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	အကယ်၍ (anchor_i != 0)
	{
	    diff_finger_dp = NULL;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    အတွက် (diff_T *dp = curtab->tp_first_diff; dp != NULL; dp = dp->df_next)
	    {
		အတွက် (ကိန်းပြည့် idx = 0; idx < DB_COUNT; idx++)
		{
		    အကယ်၍ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    အကယ်၍ (orig_diff != NULL)
	    {
		diff_T *last_diff = orig_diff;
		နေစဉ် (last_diff->df_next != NULL)
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
    ကိန်းပြည့်
diff_internal(void)
{
    ပြန်ပေး (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    static ကိန်းပြည့်
diff_internal_failed(void)
{
    ကိန်းပြည့် idx;

    // Only need to do something when there is another buffer.
    အတွက် (idx = 0; idx < DB_COUNT; ++idx)
	အကယ်၍ (curtab->tp_diffbuf[idx] != NULL
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    void
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    ကိန်းပြည့်		idx_orig;
    ကိန်းပြည့်		idx_new;
    diffio_T	diffio;
    ကိန်းပြည့်		had_diffs = curtab->tp_first_diff != NULL;

    အကယ်၍ (diff_busy)
    {
	diff_need_update = TRUE;
	ပြန်ပေး;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    အတွက် (idx_orig = 0; idx_orig < DB_COUNT; ++idx_orig)
	အကယ်၍ (curtab->tp_diffbuf[idx_orig] != NULL)
	    bha_nga;
    အကယ်၍ (idx_orig == DB_COUNT)
	goto theend;

    // Only need to do something when there is another buffer.
    အတွက် (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	အကယ်၍ (curtab->tp_diffbuf[idx_new] != NULL)
	    bha_nga;
    အကယ်၍ (idx_new == DB_COUNT)
	goto theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    အကယ်၍ (diffio.dio_internal && diff_internal_failed())
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
    အကယ်၍ (had_diffs || curtab->tp_first_diff != NULL)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, NULL, NULL, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    static ကိန်းပြည့်
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    ကိန်းပြည့်		ok;
    ကိန်းပြည့်		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    အတွက် (;;)
    {
	ok = FALSE;
	fd = mch_fopen((var_na *)diffio->dio_orig.din_fname, "w");
	အကယ်၍ (fd == NULL)
	    io_error = TRUE;
	မဟုတ်ပါက
	{
	    အကယ်၍ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((var_na *)diffio->dio_new.din_fname, "w");
	    အကယ်၍ (fd == NULL)
		io_error = TRUE;
	    မဟုတ်ပါက
	    {
		အကယ်၍ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = NULL;
		အကယ်၍ (diff_file(diffio) == OK)
		    fd = mch_fopen((var_na *)diffio->dio_diff.dout_fname, "r");
		အကယ်၍ (fd == NULL)
		    io_error = TRUE;
		မဟုတ်ပါက
		{
		    char_u	linebuf[LBUFLEN];

		    အတွက် (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			အကယ်၍ (vim_fgets(linebuf, LBUFLEN, fd))
			    bha_nga;
			အကယ်၍ (STRNCMP(linebuf, "1c1", 3) == 0
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
	အကယ်၍ (*p_dex != NUL)
	    bha_nga;
# endif

# အကယ်၍ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	အကယ်၍ (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    continue;
	}
	အကယ်၍ (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	အကယ်၍ (diff_a_works != MAYBE)
	    bha_nga;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	အကယ်၍ (ok)
	    bha_nga;
    }
    အကယ်၍ (!ok)
    {
	အကယ်၍ (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# အကယ်၍ defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

/*
 * Invoke the xdiff function.
 */
    static ကိန်းပြည့်
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    အကယ်၍ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    အကယ်၍ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    အကယ်၍ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    အကယ်၍ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_LINES;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    အကယ်၍ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    မဟုတ်ပါက
	emit_cb.out_line = xdiff_out_unified;
    အကယ်၍ (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    static ကိန်းပြည့်
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    အကယ်၍ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ပြန်ပေး OK;
    }
    မဟုတ်ပါက
# endif
    // Use xdiff for generating the diff.
    အကယ်၍ (dio->dio_internal)
	ပြန်ပေး diff_file_internal(dio);

    အကယ်၍ (check_restricted())
	ပြန်ပေး FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    အကယ်၍ (cmd == NULL)
	ပြန်ပေး FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    အကယ်၍ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((var_na *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# အကယ်၍ defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# မဟုတ်ပါက
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (ကိန်းပြည့်)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (void)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ပြန်ပေး OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    void
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = NULL;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = NULL;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = NULL;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = NULL;
    ကိန်းပြည့်		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = NULL;

# ifdef FEAT_BROWSE
    အကယ်၍ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, NULL, NULL,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), NULL);
	အကယ်၍ (browseFile == NULL)
	    ပြန်ပေး;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    အကယ်၍ (tmp_orig == NULL || tmp_new == NULL)
	goto theend;

    // Write the current buffer to "tmp_orig".
    အကယ်၍ (buf_write(curbuf, tmp_orig, NULL,
		(linenr_T)1, curbuf->b_ml.ml_line_count,
				     NULL, FALSE, FALSE, FALSE, TRUE) == FAIL)
	goto theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != NULL ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    အကယ်၍ (esc_name == NULL)
	goto theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    အကယ်၍ (buf == NULL)
	goto theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    အကယ်၍ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((var_na *)dirbuf) != 0)
	dirbuf[0] = NUL;
    မဟုတ်ပါက
    {
#  ifdef TEMPDIRNAMES
	အကယ်၍ (vim_tempdir != NULL)
	    vim_ignored = mch_chdir((var_na *)vim_tempdir);
	မဟုတ်ပါက
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    အကယ်၍ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != NULL ? fullname :
#  endif
		eap->arg, tmp_new);
    မဟုတ်ပါက
# endif
    {
	အကယ်၍ (check_restricted())
	    goto theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((var_na *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(void)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    အကယ်၍ (dirbuf[0] != NUL)
    {
	အကယ်၍ (mch_chdir((var_na *)dirbuf) != 0)
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
    အကယ်၍ (mch_stat((var_na *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    မဟုတ်ပါက
    {
	အကယ်၍ (curbuf->b_fname != NULL)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    အကယ်၍ (newname != NULL)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	အကယ်၍ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    အကယ်၍ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		အကယ်၍ (newname != NULL)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    အကယ်၍ (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    အကယ်၍ (tmp_orig != NULL)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    အကယ်၍ (tmp_new != NULL)
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
    void
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

    အကယ်၍ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ပြန်ပေး;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    အကယ်၍ (curwin == old_curwin)		// split didn't work
	ပြန်ပေး;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    အကယ်၍ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	အကယ်၍ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_line(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    void
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    static void
set_diff_option(win_T *wp, ကိန်းပြည့် value)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_value_give_err((char_u *)"diff", (long)value, NULL, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    void
diff_win_options(
    win_T	*wp,
    ကိန်းပြည့်		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    အကယ်၍ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    အကယ်၍ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    အကယ်၍ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	အကယ်၍ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    အကယ်၍ (!wp->w_p_diff)
    {
	အကယ်၍ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    အကယ်၍ (!wp->w_p_diff)
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
    အကယ်၍ (vim_strchr(p_sbo, 'h') == NULL)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    အကယ်၍ (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    void
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    ကိန်းပြည့်		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	အကယ်၍ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    အကယ်၍ (wp->w_p_diff_saved)
	    {

		အကယ်၍ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		အကယ်၍ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		အကယ်၍ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    အကယ်၍ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		အကယ်၍ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		အကယ်၍ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		အကယ်၍ (wp->w_p_fen)
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
    အကယ်၍ (eap->forceit)
	diff_buf_clear();

    အကယ်၍ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    အကယ်၍ (!diffwin && vim_strchr(p_sbo, 'h') != NULL)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    static void
diff_read(
    ကိန်းပြည့်		idx_orig,	// idx of original file
    ကိန်းပြည့်		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = NULL;
    ကိန်းပြည့်		line_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = NULL;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*line;
    long	off;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = NULL;	    // init to avoid gcc warning

    ga_nanaa {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    အကယ်၍ (dout->dout_fname == NULL)
    {
	diffstyle = DIFF_UNIFIED;
    }
    မဟုတ်ပါက
    {
	fd = mch_fopen((var_na *)dout->dout_fname, "r");
	အကယ်၍ (fd == NULL)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ပြန်ပေး;
	}
    }

    အကယ်၍ (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	အကယ်၍ (hunk == NULL)
	{
	    အကယ်၍ (fd != NULL)
		fclose(fd);
	    ပြန်ပေး;
	}
    }

    အတွက် (;;)
    {
	အကယ်၍ (dio->dio_internal)
	{
	    အကယ်၍ (line_hunk_idx >= dout->dout_ga.ga_len)
		bha_nga;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[line_hunk_idx++];
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (fd == NULL)
	    {
		အကယ်၍ (line_hunk_idx >= dout->dout_ga.ga_len)
		    bha_nga;	    // did last line
		line = ((char_u **)dout->dout_ga.ga_data)[line_hunk_idx++];
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (vim_fgets(linebuf, LBUFLEN, fd))
		    bha_nga;		// end of file
		line = linebuf;
	    }

	    အကယ်၍ (diffstyle == DIFF_NONE)
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
		အကယ်၍ (SAFE_isdigit(*line))
		    diffstyle = DIFF_ED;
		မဟုတ်ပါက အကယ်၍ ((STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		မဟုတ်ပါက အကယ်၍ ((STRNCMP(line, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		မဟုတ်ပါက
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    continue;
	    }

	    အကယ်၍ (diffstyle == DIFF_ED)
	    {
		အကယ်၍ (!SAFE_isdigit(*line))
		    continue;	// not the start of a diff block
		အကယ်၍ (parse_diff_ed(line, hunk) == FAIL)
		    continue;
	    }
	    မဟုတ်ပါက အကယ်၍ (diffstyle == DIFF_UNIFIED)
	    {
		အကယ်၍ (STRNCMP(line, "@@ ", 3)  != 0)
		    continue;	// not the start of a diff block
		အကယ်၍ (parse_diff_unified(line, hunk) == FAIL)
		    continue;
	    }
	    မဟုတ်ပါက
	    {
		emsg(_(e_invalid_diff_format));
		bha_nga;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	နေစဉ် (dp != NULL
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_count[idx_orig])
	{
	    အကယ်၍ (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	အကယ်၍ (dp != NULL
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_count[idx_orig]
		&& hunk->lnum_orig + hunk->count_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    အတွက် (dpl = dp; dpl->df_next != NULL; dpl = dpl->df_next)
		အကယ်၍ (hunk->lnum_orig + hunk->count_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    bha_nga;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    အကယ်၍ (off > 0)
	    {
		အတွက် (i = idx_orig; i < idx_new; ++i)
		    အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
		    {
			dp->df_lnum[i] -= off;
			dp->df_count[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_count[idx_new] = hunk->count_new;
	    }
	    မဟုတ်ပါက အကယ်၍ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_count[idx_new] = hunk->count_new - off;
	    }
	    မဟုတ်ပါက
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		ကိန်းပြည့် orig_size_in_dp = MIN(hunk->count_orig,
			dp->df_lnum[idx_orig] +
			dp->df_count[idx_orig] - hunk->lnum_orig);
		ကိန်းပြည့် size_diff = hunk->count_new - orig_size_in_dp;
		dp->df_count[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->count_new
		    - (dp->df_lnum[idx_new] + dp->df_count[idx_new]);
		အကယ်၍ (off > 0)
		    dp->df_count[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->count_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_count[idx_orig]);
	    အကယ်၍ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		အကယ်၍ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_count[idx_new] += -off;
		}
		off = 0;
	    }
	    အတွက် (i = idx_orig; i < idx_new; ++i)
		အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
		    dp->df_count[i] = dpl->df_lnum[i] + dpl->df_count[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    နေစဉ် (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	မဟုတ်ပါက
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    အကယ်၍ (dp == NULL)
		goto done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_count[idx_orig] = hunk->count_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_count[idx_new] = hunk->count_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    အတွက် (i = idx_orig + 1; i < idx_new; ++i)
		အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    နေစဉ် (dp != NULL)
    {
	အကယ်၍ (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    အကယ်၍ (!dio->dio_internal)
	vim_free(hunk);

    အကယ်၍ (fd != NULL)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    static void
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    ကိန်းပြည့်		idx_orig,
    ကိန်းပြည့်		idx_new)
{
    long	off;

    အကယ်၍ (dprev == NULL)
	off = 0;
    မဟုတ်ပါက
	off = (dprev->df_lnum[idx_orig] + dprev->df_count[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_count[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_count[idx_new] = dp->df_count[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    void
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    အတွက် (p = tp->tp_first_diff; p != NULL; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = NULL;
}

/*
 *  return true if the options are set to use diff linematch
 */
    static ကိန်းပြည့်
diff_linematch(diff_T *dp)
{
    အကယ်၍ (!(diff_flags & DIFF_LINEMATCH))
	ပြန်ပေး 0;

    // are there more than three diff buffers?
    ကိန်းပြည့် tsize = 0;
    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
    {
	အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    အကယ်၍ (dp->df_count[i] < 0)
		ပြန်ပေး FALSE;
	    tsize += dp->df_count[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ပြန်ပေး tsize <= linematch_lines;
}

    static ကိန်းပြည့်
get_max_diff_length(const diff_T *dp)
{
    ကိန်းပြည့် maxlength = 0;

    အတွက် (ကိန်းပြည့် k = 0; k < DB_COUNT; k++)
    {
	အကယ်၍ (curtab->tp_diffbuf[k] != NULL)
	{
	    အကယ်၍ (dp->df_count[k] > maxlength)
		maxlength = dp->df_count[k];
	}
    }
    ပြန်ပေး maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    static void
find_top_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    ကိန်းပြည့်		fromidx,
    ကိန်းပြည့်		topline)
{
    diff_T	*topdiff = NULL;
    diff_T	*localtopdiff = NULL;
    ကိန်းပြည့်		topdiffchange = 0;

    အတွက် (topdiff = curtab->tp_first_diff; topdiff != NULL;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	အကယ်၍ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	အကယ်၍ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_count[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    အကယ်၍ ((*thistopdiff) == NULL)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	အကယ်၍ (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_count[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    အကယ်၍ (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		bha_nga;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    static void
calculate_topfill_and_topline(
    const ကိန်းပြည့်	fromidx,
    const ကိန်းပြည့်	toidx,
    const ကိန်းပြည့်	from_topline,
    const ကိန်းပြည့်	from_topfill,
    ကိန်းပြည့်		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = NULL;
    diff_T	*next_adjacent_blocks = NULL;
    ကိန်းပြည့်		virtual_lines_passed = 0;
    ကိန်းပြည့်		curlinenum_to = 1;

    find_top_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    နေစဉ် (curdif && (curdif->df_lnum[fromidx] + curdif->df_count[fromidx])
							<= from_topline)
    {
	virtual_lines_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    အကယ်၍ (curdif != next_adjacent_blocks)
	virtual_lines_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_lines_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    အကယ်၍ (virtual_lines_passed < 0)
	virtual_lines_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    အကယ်၍ (thistopdiff != NULL) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    ကိန်းပြည့် virt_lines_left = virtual_lines_passed;
    curdif = thistopdiff;
    နေစဉ် (virt_lines_left > 0 && curdif != NULL && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_lines_left, curdif->df_count[toidx]);
	virt_lines_left -= MIN(virt_lines_left, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    ကိန်းပြည့် max_virt_lines = 0;
    အတွက် (diff_T *dp = thistopdiff; dp != NULL; dp = dp->df_next)
    {
	အကယ်၍ (dp->df_lnum[toidx] + dp->df_count[toidx] <= curlinenum_to)
	    max_virt_lines += get_max_diff_length(dp);
	မဟုတ်ပါက
	{
	    အကယ်၍ (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_lines += curlinenum_to - dp->df_lnum[toidx];
	    bha_nga;
	}
    }

    အကယ်၍ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_lines - virtual_lines_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    static void
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    const ကိန်းပြည့်	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    ကိန်းပြည့်		line_numbers[DB_COUNT];
    ကိန်းပြည့်		outputmap[DB_COUNT];
    size_t	ndiffs = 0;

    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
    {
	အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
	{
	    line_numbers[i] = dp->df_lnum[i];
	    dp->df_count[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    အတွက် (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	အကယ်၍ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    အတွက် (ကိန်းပြည့် j = 0; j < DB_COUNT; j++)
	    {
		အကယ်၍ (curtab->tp_diffbuf[j] != NULL)
		{
		    dp_s->df_lnum[j] = line_numbers[j];
		    dp_s->df_count[j] = 0;
		}
	    }
	}
	အတွက် (size_t j = 0; j < ndiffs; j++)
	{
	    အကယ်၍ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_count[outputmap[j]]++;
		line_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    static void
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_COUNT];
    const mmfile_t	*diffbufs[DB_COUNT];
    ကိန်းပြည့်			diff_length[DB_COUNT];
    size_t		ndiffs = 0;

    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
    {
	အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    အကယ်၍ (dp->df_count[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_count[i] - 1);
	    }
	    မဟုတ်ပါက
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = NULL;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_count[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    ကိန်းပြည့် *decisions = NULL;
    const ကိန်းပြည့် iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    အတွက် (size_t i = 0; i < ndiffs; i++)
	free(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    free(decisions);
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
    ကိန်းပြည့်
diff_check_with_linestatus(win_T *wp, linenr_T lnum, ကိန်းပြည့် *linestatus)
{
    ကိန်းပြည့်		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    ကိန်းပြည့်		maxcount;
    ကိန်းပြည့်		i;
    buf_T	*buf = wp->w_buffer;
    ကိန်းပြည့်		cmp;

    အကယ်၍ (linestatus != NULL)
	*linestatus = 0;

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    အကယ်၍ (curtab->tp_first_diff == NULL || !wp->w_p_diff)	// no diffs at all
	ပြန်ပေး 0;

    // safety check: "lnum" must be a buffer line
    အကယ်၍ (lnum < 1 || lnum > buf->b_ml.ml_line_count + 1)
	ပြန်ပေး 0;

    idx = diff_buf_idx(buf);
    အကယ်၍ (idx == DB_COUNT)
	ပြန်ပေး 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    အကယ်၍ (hasFoldingWin(wp, lnum, NULL, NULL, TRUE, NULL))
	ပြန်ပေး 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    အတွက် (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	အကယ်၍ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	အကယ်၍ (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    bha_nga;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    အကယ်၍ (dp == NULL || lnum < dp->df_lnum[idx])
	ပြန်ပေး 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    အကယ်၍ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    ကိန်းပြည့် num_fill = 0;
    နေစဉ် (lnum == dp->df_lnum[idx] + dp->df_count[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	အကယ်၍ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    num_fill += maxcount - dp->df_count[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	အကယ်၍ (dp->df_next != NULL
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_count[idx])
	    dp = dp->df_next;
	မဟုတ်ပါက
	    bha_nga;
    }

    အကယ်၍ (lnum < dp->df_lnum[idx] + dp->df_count[idx])
    {
	ကိန်းပြည့်	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	အတွက် (i = 0; i < DB_COUNT; ++i)
	    အကယ်၍ (i != idx && curtab->tp_diffbuf[i] != NULL)
	    {
		အကယ်၍ (dp->df_count[i] == 0)
		    zero = TRUE;
		မဟုတ်ပါက
		{
		    အကယ်၍ (dp->df_count[i] != dp->df_count[idx])
		    {
			အကယ်၍ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ပြန်ပေး num_fill;
		    }
		    cmp = TRUE;
		}
	    }
	အကယ်၍ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    အတွက် (i = 0; i < DB_COUNT; ++i)
		အကယ်၍ (i != idx && curtab->tp_diffbuf[i] != NULL
						      && dp->df_count[i] != 0)
		    အကယ်၍ (!diff_equal_entry(dp, idx, i))
		    {
			အကယ်၍ (linestatus)
			    *linestatus = -1;
			ပြန်ပေး num_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	အကယ်၍ (zero == FALSE)
	    ပြန်ပေး num_fill;
	အကယ်၍ (linestatus)
	    *linestatus = -2;
	ပြန်ပေး num_fill;
    }
    ပြန်ပေး num_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    static ကိန်းပြည့်
diff_equal_entry(diff_T *dp, ကိန်းပြည့် idx1, ကိန်းပြည့် idx2)
{
    ကိန်းပြည့်		i;
    char_u	*line;
    ကိန်းပြည့်		cmp;

    အကယ်၍ (dp->df_count[idx1] != dp->df_count[idx2])
	ပြန်ပေး FALSE;
    အကယ်၍ (diff_check_sanity(curtab, dp) == FAIL)
	ပြန်ပေး FALSE;
    အတွက် (i = 0; i < dp->df_count[idx1]; ++i)
    {
	line = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	အကယ်၍ (line == NULL)
	    ပြန်ပေး FALSE;
	cmp = diff_cmp(line, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(line);
	အကယ်၍ (cmp != 0)
	    ပြန်ပေး FALSE;
    }
    ပြန်ပေး TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    static ကိန်းပြည့်
diff_equal_char(char_u *p1, char_u *p2, ကိန်းပြည့် *len)
{
    ကိန်းပြည့် l  = (*mb_ptr2len)(p1);

    အကယ်၍ (l != (*mb_ptr2len)(p2))
	ပြန်ပေး FALSE;
    အကယ်၍ (l > 1)
    {
	အကယ်၍ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ပြန်ပေး FALSE;
	*len = l;
    }
    မဟုတ်ပါက
    {
	အကယ်၍ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ပြန်ပေး FALSE;
	*len = 1;
    }
    ပြန်ပေး TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    static ကိန်းပြည့်
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    ကိန်းပြည့်		l;

    အကယ်၍ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ပြန်ပေး 0;

    အကယ်၍ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ပြန်ပေး STRCMP(s1, s2);
    အကယ်၍ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ပြန်ပေး MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    နေစဉ် (*p1 != NUL && *p2 != NUL)
    {
	အကယ်၍ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (!diff_equal_char(p1, p2, &l))
		bha_nga;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    အကယ်၍ (*p1 != NUL || *p2 != NUL)
	ပြန်ပေး 1;
    ပြန်ပေး 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    ကိန်းပြည့်
diff_check_fill(win_T *wp, linenr_T lnum)
{
    ကိန်းပြည့်		n;

    // be quick when there are no filler lines
    အကယ်၍ (!(diff_flags & DIFF_FILLER))
	ပြန်ပေး 0;
    n = diff_check_with_linestatus(wp, lnum, NULL);
    အကယ်၍ (n <= 0)
	ပြန်ပေး 0;
    ပြန်ပေး n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    void
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    ကိန်းပြည့်		fromidx;
    ကိန်းပြည့်		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    အကယ်၍ (fromidx == DB_COUNT)
	ပြန်ပေး;		// safety check

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	အကယ်၍ (lnum <= dp->df_lnum[fromidx] + dp->df_count[fromidx])
	    bha_nga;
    အကယ်၍ (dp == NULL)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count
				       - (frombuf->b_ml.ml_line_count - lnum);
    }
    မဟုတ်ပါက
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	အကယ်၍ (toidx == DB_COUNT)
	    ပြန်ပေး;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	အကယ်၍ (lnum >= dp->df_lnum[fromidx])
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
    အကယ်၍ (towin->w_topline > towin->w_buffer->b_ml.ml_line_count)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count;
	towin->w_botfill = true;
    }
    အကယ်၍ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_line_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (void)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    NULL, TRUE, NULL);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    static ကိန်းပြည့်
parse_diffanchors(
    ကိန်းပြည့်		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    ကိန်းပြည့်		*num_anchors)
{
    ကိန်းပြည့် i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = NULL;
    အကယ်၍ (check_only)
	bufwin = curwin;
    မဟုတ်ပါက
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    အကယ်၍ (bufwin->w_buffer == buf && bufwin->w_p_diff)
		bha_nga;
	အကယ်၍ (bufwin == NULL && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ပြန်ပေး FAIL;
	}
    }

    အတွက် (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	အကယ်၍ (*dia == ',') // don't allow empty values
	    ပြန်ပေး FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(NULL, &dia, ADDR_LINES, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	အကယ်၍ (dia == NULL) // error detected
	    ပြန်ပေး FAIL;
	အကယ်၍ (*dia != ',' && *dia != NUL)
	    ပြန်ပေး FAIL;

	အကယ်၍ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_line_count + 1))
	{
	    emsg(_(e_invalid_range));
	    ပြန်ပေး FAIL;
	}

	အကယ်၍ (anchors != NULL)
	    anchors[i] = lnum;

	အကယ်၍ (*dia == ',')
	    dia++;
    }
    အကယ်၍ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (num_anchors != NULL)
	*num_anchors = i;
    ပြန်ပေး OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    ကိန်းပြည့်
diffanchors_changed(ကိန်းပြည့် buflocal)
{
    ကိန်းပြည့် result = parse_diffanchors(TRUE, curbuf, NULL, NULL);
    အကယ်၍ (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    အကယ်၍ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    မဟုတ်ပါက
	    {
		အတွက် (ကိန်းပြည့် idx = 0; idx < DB_COUNT; ++idx)
		    အကယ်၍ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			bha_nga;
		    }
	    }
	}
    }
    ပြန်ပေး result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    ကိန်းပြည့်
diffopt_changed(void)
{
    char_u	*p;
    ကိန်းပြည့်		diff_context_new = 6;
    ကိန်းပြည့်		linematch_lines_new = 0;
    ကိန်းပြည့်		diff_flags_new = 0;
    ကိန်းပြည့်		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    နေစဉ် (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	အကယ်၍ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    အကယ်၍ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    မဟုတ်ပါက
		ပြန်ပေး FAIL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    အကယ်၍ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    မဟုတ်ပါက
		ပြန်ပေး FAIL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_lines_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	အကယ်၍ (*p != ',' && *p != NUL)
	    ပြန်ပေး FAIL;
	အကယ်၍ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    အကယ်၍ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ပြန်ပေး FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    အကယ်၍ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_lines = linematch_lines_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    ပြန်ပေး OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    ကိန်းပြည့်
diffopt_horizontal(void)
{
    ပြန်ပေး (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    ကိန်းပြည့်
diffopt_hiddenoff(void)
{
    ပြန်ပေး (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    ကိန်းပြည့်
diffopt_closeoff(void)
{
    ပြန်ပေး (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    void
diff_update_line(linenr_T lnum)
{
    ကိန်းပြည့်		idx;
    diff_T	*dp;

    အကယ်၍ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ပြန်ပေး;

    idx = diff_buf_idx(curbuf);
    အကယ်၍ (idx == DB_COUNT)
	ပြန်ပေး;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	အကယ်၍ (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    bha_nga;

    // clear the inline change cache as it's invalid
    အကယ်၍ (dp != NULL)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

static diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    ကိန်းပြည့်
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    ကိန်းပြည့် *change_start,
    ကိန်းပြည့် *change_end)
{
    အကယ်၍ (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    မဟုတ်ပါက
	*change_start = change->dc_start[diffline->bufidx];
    အကယ်၍ (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    မဟုတ်ပါက
	*change_end = change->dc_end[diffline->bufidx];

    အကယ်၍ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ပြန်ပေး FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
    {
	အကယ်၍ (i == diffline->bufidx)
	    continue;
	အကယ်၍ (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    ပြန်ပေး FALSE;
	}
    }
    ပြန်ပေး TRUE;
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
    static ကိန်းပြည့်
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    ကိန်းပြည့်		idx,
    ကိန်းပြည့်		*startp,	// first char of the change
    ကိန်းပြည့်		*endp)		// last char of the change
{
    char_u	*line_org;
    char_u	*line_new;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		si_org, si_new;
    ကိန်းပြည့်		ei_org, ei_new;
    ကိန်းပြည့်		off;
    ကိန်းပြည့်		added = TRUE;
    char_u	*p1, *p2;
    ကိန်းပြည့်		l;

    အကယ်၍ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	line_org = NULL;
    }
    မဟုတ်ပါက
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	line_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	အကယ်၍ (line_org == NULL)
	    ပြန်ပေး FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    အတွက် (i = 0; i < DB_COUNT; ++i)
	အကယ်၍ (curtab->tp_diffbuf[i] != NULL && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    အကယ်၍ (off >= dp->df_count[i])
		continue;
	    added = FALSE;
	    အကယ်၍ (diff_flags & DIFF_INLINE_NONE)
		bha_nga; // early terminate as we only care about the return value

	    line_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    နေစဉ် (line_org[si_org] != NUL)
	    {
		အကယ်၍ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(line_org[si_org])
					      && VIM_ISWHITE(line_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(line_org[si_org])
					    || VIM_ISWHITE(line_new[si_new]))))
		{
		    si_org = (ကိန်းပြည့်)(skipwhite(line_org + si_org) - line_org);
		    si_new = (ကိန်းပြည့်)(skipwhite(line_new + si_new) - line_new);
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (!diff_equal_char(line_org + si_org, line_new + si_new,
									   &l))
			bha_nga;
		    si_org += l;
		    si_new += l;
		}
	    }
	    အကယ်၍ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(line_org, line_org + si_org);
		si_new -= (*mb_head_off)(line_new, line_new + si_new);
	    }
	    အကယ်၍ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    အကယ်၍ (line_org[si_org] != NUL || line_new[si_new] != NUL)
	    {
		ei_org = (ကိန်းပြည့်)STRLEN(line_org);
		ei_new = (ကိန်းပြည့်)STRLEN(line_new);
		နေစဉ် (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    အကယ်၍ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(line_org[ei_org])
					      && VIM_ISWHITE(line_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(line_org[ei_org])
					    || VIM_ISWHITE(line_new[ei_new]))))
		    {
			နေစဉ် (ei_org >= *startp
					     && VIM_ISWHITE(line_org[ei_org]))
			    --ei_org;
			နေစဉ် (ei_new >= si_new
					     && VIM_ISWHITE(line_new[ei_new]))
			    --ei_new;
		    }
		    မဟုတ်ပါက
		    {
			p1 = line_org + ei_org;
			p2 = line_new + ei_new;
			p1 -= (*mb_head_off)(line_org, p1);
			p2 -= (*mb_head_off)(line_new, p2);
			အကယ်၍ (!diff_equal_char(p1, p2, &l))
			    bha_nga;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		အကယ်၍ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(line_org);
    ပြန်ပေး added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
typedef sa_nracanaa
{
    long byte_start;
    long num_bytes;
    ကိန်းပြည့် lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    static void
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, ကိန်းပြည့် idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    ကိန်းပြည့် pass = 1;
    do
    {
	ကိန်းပြည့် has_unmerged_gaps = FALSE;
	ကိန်းပြည့် has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	နေစဉ် (dp!= NULL && dp->df_next != NULL)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    အကယ်၍ (dp->df_lnum[idx1] + dp->df_count[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_count[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    အကယ်၍ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_count[idx1]);
	    အကယ်၍ (gap <= 3)
	    {
		linenr_T max_df_count = 0;
		အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
		    max_df_count = MAX(max_df_count, dp->df_count[i] + dp->df_next->df_count[i]);

		အကယ်၍ (max_df_count >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
		    {
			dp->df_count[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_count[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    continue;
		}
		မဟုတ်ပါက
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	အကယ်၍ (!has_unmerged_gaps || !has_merged_gaps)
	    bha_nga;
    } နေစဉ် (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    static void
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, ကိန်းပြည့် idx1,
				  linenr_T start_lnum)
{
    ကိန်းပြည့် pass = 1;
    do
    {
	diff_T *dp = dp_orig;

	နေစဉ် (dp != NULL && dp->df_next != NULL)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_count[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    အကယ်၍ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		continue;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    အကယ်၍ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // Calculate the gap between blocks
	    ကိန်းပြည့် gap_start = entry1->byte_start + entry1->num_bytes;
	    ကိန်းပြည့် gap_end = entry2->byte_start;
	    ကိန်းပြည့် gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    အကယ်၍ (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // Get the text between the two blocks
	    char_u *line = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = line + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    အတွက် (ကိန်းပြည့် i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		ကိန်းပြည့် char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		အကယ်၍ (char_class == 2)
		{
		    only_non_word = false;
		    bha_nga;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    အကယ်၍ (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
		{
		    အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
		    {
			// count bytes in the first block
			အတွက် (ကိန်းပြည့် k = 0; k < dp->df_count[i]; k++)
			{
			    ကိန်းပြည့် idx = dp->df_lnum[i] + k - 1;
			    အကယ်၍ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
			// count bytes in the next block
			အတွက် (ကိန်းပြည့် k = 0; k < dp->df_next->df_count[i]; k++)
			{
			    ကိန်းပြည့် idx = dp->df_next->df_lnum[i] + k - 1;
			    အကယ်၍ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
		    }
		}

		အကယ်၍ (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
		    {
			အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
			{
			    dp->df_count[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_count[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    continue;
		}
	    }

	    dp = dp->df_next;
	}
    } နေစဉ် (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    static void
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_COUNT];
    garray_T	file1_str;
    garray_T	file2_str;
    ကိန်းပြည့်		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, parimaa_na(var_na *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = NULL;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_COUNT]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, parimaa_na(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
	ga_init2(&linemap[i], parimaa_na(linemap_entry_T), 128);

    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	အကယ်၍ (buf == NULL || buf->b_ml.ml_mfp == NULL)
	    continue; // skip buffer that isn't loaded

	အကယ်၍ (dp->df_count[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = NULL;
	    continue;
	}

	အကယ်၍ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	အတွက် (ကိန်းပြည့် off = 0; off < dp->df_count[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    ကိန်းပြည့် in_keyword = FALSE;

	    // iwhiteeol support vars
	    ကိန်းပြည့် last_white = FALSE;
	    ကိန်းပြည့် eol_ga_len = -1;
	    ကိန်းပြည့် eol_linemap_len = -1;
	    ကိန်းပြည့် eol_numlines = -1;

	    char_u *s;
	    အတွက် (s = curline; *s != NUL;)
	    {
		ကိန်းပြည့် new_in_keyword = FALSE;
		အကယ်၍ (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		အကယ်၍ (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		အကယ်၍ (VIM_ISWHITE(*s))
		{
		    အကယ်၍ (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			continue;
		    }
		    မဟုတ်ပါက အကယ်၍ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			အကယ်၍ (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		ကိန်းပြည့် char_len = 1;
		အကယ်၍ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		မဟုတ်ပါက
		{
		    char_len = mb_ptr2len(s);

		    အကယ်၍ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    အကယ်၍ (diff_flags & DIFF_ICASE)
		    {
			ကိန်းပြည့် c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			ကိန်းပြည့် c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			ကိန်းပြည့် c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			အကယ်၍ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    မဟုတ်ပါက
			ga_concat_len(curstr, s, char_len);
		}

		အကယ်၍ (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		အကယ်၍ (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.num_bytes = char_len;
		    အကယ်၍ (ga_grow(&linemap[i], 1) != OK)
			goto done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		မဟုတ်ပါက
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].num_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    အကယ်၍ (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    အကယ်၍ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		အကယ်၍ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    အကယ်၍ (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.num_bytes = parimaa_na(NL);
		အကယ်၍ (ga_grow(&linemap[i], 1) != OK)
		    goto done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	အကယ်၍ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (var_na *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	မဟုတ်ပါက
	{
	    dio.dio_orig.din_mmfile.ptr = (var_na *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	အကယ်၍ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    ကိန်းပြည့် diff_status = diff_file_internal(&dio);
	    အကယ်၍ (diff_status == FAIL)
		goto done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    အကယ်၍ (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    မဟုတ်ပါက အကယ်၍ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    အတွက် (; new_diff != NULL; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
	{
	    အကယ်၍ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		continue;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_count[i];

	    အကယ်၍ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    မဟုတ်ပါက
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    အကယ်၍ (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    မဟုတ်ပါက အကယ်၍ (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    မဟုတ်ပါက
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].num_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	အကယ်၍ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    goto done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, parimaa_na(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ကိန်းပြည့်
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    ကိန်းပြည့်		idx;
    ကိန်းပြည့်		off;

    idx = diff_buf_idx(wp->w_buffer);
    အကယ်၍ (idx == DB_COUNT)	// cannot happen
	ပြန်ပေး FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	အကယ်၍ (lnum < dp->df_lnum[idx] + dp->df_count[idx])
	    bha_nga;
    အကယ်၍ (dp == NULL || diff_check_sanity(curtab, dp) == FAIL)
	ပြန်ပေး FALSE;

    အကယ်၍ (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	ပြန်ပေး FALSE;
    off = lnum - dp->df_lnum[idx];

    အကယ်၍ (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	ကိန်းပြည့်	change_start = MAXCOL;	// first col of changed area
	ကိန်းပြည့်	change_end = -1;	// last col of changed area
	ကိန်းပြည့်	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->num_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	ပြန်ပေး ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    အကယ်၍ (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    ကိန်းပြည့် num_changes = 0;
    ကိန်းပြည့် change_idx = 0;
    diffline->changes = NULL;
    အတွက် (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	အကယ်၍ (change->dc_end_lnum_off[idx] < off)
	    continue;
	အကယ်၍ (change->dc_start_lnum_off[idx] > off)
	    bha_nga;
	အကယ်၍ (diffline->changes == NULL)
	    diffline->changes = change;
	num_changes++;
    }
    diffline->num_changes = num_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    ကိန်းပြည့် added = FALSE;
    အကယ်၍ (num_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	အတွက် (ကိန်းပြည့် i = 0; i < DB_COUNT; i++)
	{
	    အကယ်၍ (idx == i)
		continue;
	    အကယ်၍ (curtab->tp_diffbuf[i] == NULL)
		continue;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    အကယ်၍ (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		bha_nga;
	    }
	}
    }
    ပြန်ပေး added;
}

# အကယ်၍ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    ကိန်းပြည့်
diff_infold(win_T *wp, linenr_T lnum)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		idx = -1;
    ကိန်းပြည့်		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    အကယ်၍ (!wp->w_p_diff)
	ပြန်ပေး FALSE;

    အတွက် (i = 0; i < DB_COUNT; ++i)
    {
	အကယ်၍ (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	မဟုတ်ပါက အကယ်၍ (curtab->tp_diffbuf[i] != NULL)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    အကယ်၍ (idx == -1 || !other)
	ပြန်ပေး FALSE;

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    အကယ်၍ (curtab->tp_first_diff == NULL)
	ပြန်ပေး TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    ကိန်းပြည့်		result = TRUE;
    အတွက် (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	အကယ်၍ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	အကယ်၍ (dp->df_lnum[idx] - diff_context > lnum)
	    bha_nga;
	// If this change ends before the line we have a match.
	အကယ်၍ (dp->df_lnum[idx] + dp->df_count[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    bha_nga;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ပြန်ပေး result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    void
nv_diffgetput(ကိန်းပြည့် put, long count)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    အကယ်၍ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ပြန်ပေး;
    }
# endif
    အကယ်၍ (count == 0)
	ea.arg = (char_u *)"";
    မဟုတ်ပါက
    {
	vim_snprintf((var_na *)buf, 30, "%ld", count);
	ea.arg = buf;
    }
    အကယ်၍ (put)
	ea.cmdidx = CMD_diffput;
    မဟုတ်ပါက
	ea.cmdidx = CMD_diffget;
    ea.addr_count = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    static ကိန်းပြည့်
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	အကယ်၍ (dp == diff)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    void
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    ကိန်းပြည့်		count;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    ကိန်းပြည့်		idx_cur;
    ကိန်းပြည့်		idx_other;
    ကိန်းပြည့်		idx_from;
    ကိန်းပြည့်		idx_to;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    ကိန်းပြည့်		start_skip, end_skip;
    ကိန်းပြည့်		new_count;
    ကိန်းပြည့်		buf_empty;
    ကိန်းပြည့်		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    အကယ်၍ (idx_cur == DB_COUNT)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	ပြန်ပေး;
    }

    အကယ်၍ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	အတွက် (idx_other = 0; idx_other < DB_COUNT; ++idx_other)
	    အကယ်၍ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != NULL)
	    {
		အကယ်၍ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    bha_nga;
		found_not_ma = TRUE;
	    }
	အကယ်၍ (idx_other == DB_COUNT)
	{
	    အကယ်၍ (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    မဟုတ်ပါက
		emsg(_(e_no_other_buffer_in_diff_mode));
	    ပြန်ပေး;
	}

	// Check that there isn't a third buffer in the list
	အတွက် (i = idx_other + 1; i < DB_COUNT; ++i)
	    အကယ်၍ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != NULL
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		ပြန်ပေး;
	    }
    }
    မဟုတ်ပါက
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	နေစဉ် (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	အတွက် (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	အကယ်၍ (eap->arg + i == p)	    // digits only
	    i = atol((var_na *)eap->arg);
	မဟုတ်ပါက
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    အကယ်၍ (i < 0)
		ပြန်ပေး;		// error message already given
	}
	buf = buflist_findnr(i);
	အကယ်၍ (buf == NULL)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    ပြန်ပေး;
	}
	အကယ်၍ (buf == curbuf)
	    ပြန်ပေး;		// nothing to do
	idx_other = diff_buf_idx(buf);
	အကယ်၍ (idx_other == DB_COUNT)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    ပြန်ပေး;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    အကယ်၍ (eap->addr_count == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	ကိန်းပြည့် linestatus = 0;
	အကယ်၍ (eap->line1 == curbuf->b_ml.ml_line_count
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	မဟုတ်ပါက အကယ်၍ (eap->line1 > 0)
	    --eap->line1;
    }

    အကယ်၍ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    မဟုတ်ပါက
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	အကယ်၍ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    goto theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    အကယ်၍ (!curbuf->b_changed)
    {
	change_warning(0);
	အကယ်၍ (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    goto theend;
	}
    }

    dprev = NULL;
    အတွက် (dp = curtab->tp_first_diff; dp != NULL; )
    {
	အကယ်၍ (!eap->addr_count)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    နေစဉ် (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_count[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	အကယ်၍ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    bha_nga;	// past the range that was specified

	dfree = NULL;
	lnum = dp->df_lnum[idx_to];
	count = dp->df_count[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + count + (count == 0 && BUFEMPTY() ? 1 : 0);

	အကယ်၍ (dp->df_lnum[idx_cur] + dp->df_count[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    အကယ်၍ (eap->addr_count > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		အကယ်၍ (start_skip > 0)
		{
		    // range starts below start of current diff block
		    အကယ်၍ (start_skip > count)
		    {
			lnum += count;
			count = 0;
		    }
		    မဟုတ်ပါက
		    {
			count -= start_skip;
			lnum += start_skip;
		    }
		}
		မဟုတ်ပါက
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_count[idx_cur] - 1
							 - (eap->line2 + off);
		အကယ်၍ (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    အကယ်၍ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_count[idx_cur] - start_skip - end_skip;
			အကယ်၍ (count > i)
			    count = i;
		    }
		    မဟုတ်ပါက			// :diffget
		    {
			count -= end_skip;
			end_skip = dp->df_count[idx_from] - start_skip - count;
			အကယ်၍ (end_skip < 0)
			    end_skip = 0;
		    }
		}
		မဟုတ်ပါက
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    အတွက် (i = 0; i < count; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_line_count == 1;
		အကယ်၍ (ml_delete(lnum) == OK)
		    --added;
	    }
	    အတွက် (i = 0; i < dp->df_count[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		အကယ်၍ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_line_count)
		    bha_nga;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		အကယ်၍ (p != NULL)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    အကယ်၍ (buf_empty && curbuf->b_ml.ml_line_count == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_count = dp->df_count[idx_to] + added;
	    dp->df_count[idx_to] = new_count;

	    အကယ်၍ (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		အတွက် (i = 0; i < DB_COUNT; ++i)
		    အကယ်၍ (curtab->tp_diffbuf[i] != NULL && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			bha_nga;
		အကယ်၍ (i == DB_COUNT)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    အကယ်၍ (dprev == NULL)
			curtab->tp_first_diff = dp;
		    မဟုတ်ပါက
			dprev->df_next = dp;
		}
	    }

	    အကယ်၍ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + count - 1, (long)MAXLNUM, (long)added);
		အကယ်၍ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    အကယ်၍ (curwin->w_cursor.lnum >= lnum + count)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_line_count);
		    }
		    မဟုတ်ပါက အကယ်၍ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_lines(lnum, 0, lnum + count, (long)added);

	    အကယ်၍ (dfree != NULL)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    အကယ်၍ (added != 0 && !valid_diff(dp))
		bha_nga;

	    အကယ်၍ (dfree == NULL)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_count[idx_to] = new_count;

	    // When changing the current buffer, keep track of line numbers
	    အကယ်၍ (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	အကယ်၍ (dfree == NULL)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    အကယ်၍ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	အကယ်၍ (KeyTyped)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    အကယ်၍ (diff_need_update)
	ex_diffupdate(NULL);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_line_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    အကယ်၍ (curtab->tp_first_diff == NULL)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    အကယ်၍ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    အကယ်၍ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    မဟုတ်ပါက
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, NULL, NULL, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    static void
diff_fold_update(diff_T *dp, ကိန်းပြည့် skip_idx)
{
    ကိန်းပြည့်		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	အတွက် (i = 0; i < DB_COUNT; ++i)
	    အကယ်၍ (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_count[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    ကိန်းပြည့်
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	အကယ်၍ (diff_buf_idx_tp(buf, tp) != DB_COUNT)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    ကိန်းပြည့်
diff_move_to(ကိန်းပြည့် dir, long count)
{
    ကိန်းပြည့်		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    အကယ်၍ (idx == DB_COUNT || curtab->tp_first_diff == NULL)
	ပြန်ပေး FAIL;

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    အကယ်၍ (curtab->tp_first_diff == NULL)		// no diffs today
	ပြန်ပေး FAIL;

    နေစဉ် (--count >= 0)
    {
	// Check if already before first diff.
	အကယ်၍ (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    bha_nga;

	အတွက် (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    အကယ်၍ (dp == NULL)
		bha_nga;
	    အကယ်၍ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == NULL
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		bha_nga;
	    }
	}
    }

    // don't end up past the end of the file
    အကယ်၍ (lnum > curbuf->b_ml.ml_line_count)
	lnum = curbuf->b_ml.ml_line_count;

    // When the cursor didn't move at all we fail.
    အကယ်၍ (lnum == curwin->w_cursor.lnum)
	ပြန်ပေး FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    ပြန်ပေး OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    static linenr_T
diff_get_corresponding_line_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    ကိန်းပြည့်		idx1;
    ကိန်းပြည့်		idx2;
    diff_T	*dp;
    ကိန်းပြည့်		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    အကယ်၍ (idx1 == DB_COUNT || idx2 == DB_COUNT || curtab->tp_first_diff == NULL)
	ပြန်ပေး lnum1;

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    အကယ်၍ (curtab->tp_first_diff == NULL)		// no diffs today
	ပြန်ပေး lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	အကယ်၍ (dp->df_lnum[idx1] > lnum1)
	    ပြန်ပေး lnum1 - baseline;
	အကယ်၍ ((dp->df_lnum[idx1] + dp->df_count[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    အကယ်၍ (baseline > dp->df_count[idx2])
		baseline = dp->df_count[idx2];

	    ပြန်ပေး dp->df_lnum[idx2] + baseline;
	}
	အကယ်၍ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_count[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_count[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    ပြန်ပေး curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_count[idx1])
				   - (dp->df_lnum[idx2] + dp->df_count[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ပြန်ပေး lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_line(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_line_int(buf1, lnum1);

    // don't end up past the end of the file
    အကယ်၍ (lnum > curbuf->b_ml.ml_line_count)
	ပြန်ပေး curbuf->b_ml.ml_line_count;
    ပြန်ပေး lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    ကိန်းပြည့်		idx;
    ကိန်းပြည့်		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    အကယ်၍ (idx == DB_COUNT)		// safety check
	ပြန်ပေး (linenr_T)0;

    အကယ်၍ (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	အကယ်၍ (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    bha_nga;

    // When after the last change, compute relative to the last line number.
    အကယ်၍ (dp == NULL)
	ပြန်ပေး wp->w_buffer->b_ml.ml_line_count
					- (curbuf->b_ml.ml_line_count - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    အကယ်၍ (i == DB_COUNT)			// safety check
	ပြန်ပေး (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    အကယ်၍ (n > dp->df_lnum[i] + dp->df_count[i])
	n = dp->df_lnum[i] + dp->df_count[i];
    ပြန်ပေး n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    static ကိန်းပြည့်
parse_diff_ed(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    ကိန်းပြည့်	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = line;
    f1 = getdigits(&p);
    အကယ်၍ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    မဟုတ်ပါက
	l1 = f1;
    အကယ်၍ (*p != 'a' && *p != 'c' && *p != 'd')
	ပြန်ပေး FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    အကယ်၍ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    မဟုတ်ပါက
	l2 = f2;
    အကယ်၍ (l1 < f1 || l2 < f2)
	ပြန်ပေး FAIL;

    အကယ်၍ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->count_orig = 0;
    }
    မဟုတ်ပါက
    {
	hunk->lnum_orig = f1;
	hunk->count_orig = l1 - f1 + 1;
    }
    အကယ်၍ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->count_new = 0;
    }
    မဟုတ်ပါက
    {
	hunk->lnum_new = f2;
	hunk->count_new = l2 - f2 + 1;
    }
    ပြန်ပေး OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    static ကိန်းပြည့်
parse_diff_unified(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = line;
    အကယ်၍ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	အကယ်၍ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	မဟုတ်ပါက
	    oldcount = 1;
	အကယ်၍ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    အကယ်၍ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    မဟုတ်ပါက
		newcount = 1;
	}
	မဟုတ်ပါက
	    ပြန်ပေး FAIL;	// invalid diff format

	အကယ်၍ (oldcount == 0)
	    oldline += 1;
	အကယ်၍ (newcount == 0)
	    newline += 1;
	အကယ်၍ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->count_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->count_new = newcount;

	ပြန်ပေး OK;
    }

    ပြန်ပေး FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    static ကိန်းပြည့်
xdiff_out_indices(
	long start_a,
	long count_a,
	long start_b,
	long count_b,
	void *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    အကယ်၍ (p == NULL)
	ပြန်ပေး -1;

    အကယ်၍ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ပြန်ပေး -1;
    }

    p->lnum_orig  = start_a + 1;
    p->count_orig = count_a;
    p->lnum_new   = start_b + 1;
    p->count_new  = count_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    ပြန်ပေး 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    static ကိန်းပြည့်
xdiff_out_unified(
	void *priv,
	mmbuffer_t *mb,
	ကိန်းပြည့် nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    ပြန်ပေး 0;
}

#endif	// FEAT_DIFF

#အကယ်၍ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    void
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    အကယ်၍ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    rettv->vval.v_number = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    void
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    static linenr_T	prev_lnum = 0;
    static varnumber_T	changedtick = 0;
    static ကိန်းပြည့်		fnum = 0;
    static ကိန်းပြည့်		prev_diff_flags = 0;
    static ကိန်းပြည့်		change_start = 0;
    static ကိန်းပြည့်		change_end = 0;
    static hlf_T	hlID = (hlf_T)0;
    ကိန်းပြည့်			cache_results = TRUE;
    ကိန်းပြည့်			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    အကယ်၍ (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    အကယ်၍ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    အကယ်၍ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    အကယ်၍ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	ကိန်းပြည့် linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	အကယ်၍ (linestatus < 0)
	{
	    အကယ်၍ (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		အကယ်၍ (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		မဟုတ်ပါက
		{
		    hlID = HLF_CHD;	// changed line
		    အကယ်၍ (diffline.num_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    မဟုတ်ပါက
		hlID = HLF_ADD;	// added line
	}
	မဟုတ်ပါက
	    hlID = (hlf_T)0;

	အကယ်၍ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    အကယ်၍ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_number(&argvars[1]) - 1; // ignore type error in {col}
	အကယ်၍ (cache_results)
	{
	    အကယ်၍ (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    မဟုတ်ပါက
		hlID = HLF_CHD;			// changed line
	}
	မဟုတ်ပါက
	{
	    hlID = HLF_CHD;
	    အတွက် (ကိန်းပြည့် i = 0; i < diffline.num_changes; i++)
	    {
		ကိန်းပြည့် added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		အကယ်၍ (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    bha_nga;
		}
		အကယ်၍ (col < change_start)
		    // the remaining changes are past this column and not relevant
		    bha_nga;
	    }
	}
    }
    rettv->vval.v_number = hlID == (hlf_T)0 ? 0 : (ကိန်းပြည့်)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    static ကိန်းပြည့်
parse_diff_optarg(
    typval_T	    *opts,
    ကိန်းပြည့်		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    ကိန်းပြည့်		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    အကယ်၍ (algo != NULL)
    {
	အကယ်၍ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	မဟုတ်ပါက အကယ်၍ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	မဟုတ်ပါက အကယ်၍ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	မဟုတ်ပါက အကယ်၍ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    အကယ်၍ (output_fmt != NULL)
    {
	အကယ်၍ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	မဟုတ်ပါက အကယ်၍ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	မဟုတ်ပါက
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ပြန်ပေး FAIL;
	}
    }

    *diff_ctxlen = dict_get_number_def(d, "context", 0);
    အကယ်၍ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    အကယ်၍ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    အကယ်၍ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    အကယ်၍ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    အကယ်၍ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    အကယ်၍ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    အကယ်၍ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ပြန်ပေး OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    static void
list_to_diffin(list_T *l, diffin_T *din, ကိန်းပြည့် icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	str = tv_get_string(&li->li_tv);
	အကယ်၍ (icase)
	{
	    str = strlow_save(str);
	    အကယ်၍ (str == NULL)
		continue;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	အကယ်၍ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (var_na *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    static dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    အကယ်၍ (hunk_dict == NULL)
	ပြန်ပေး NULL;

    dict_add_number(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_number(hunk_dict, "from_count", hunk->count_orig);
    dict_add_number(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_number(hunk_dict, "to_count", hunk->count_new);

    ပြန်ပေး hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    void
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    အကယ်၍ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ပြန်ပေး;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, parimaa_na(var_na *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    ကိန်းပြည့်		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    အကယ်၍ (argvars[2].v_type != VAR_UNKNOWN)
	အကယ်၍ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ပြန်ပေး;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    ကိန်းပြည့်	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    အကယ်၍ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    ကိန်းပြည့် diff_status = diff_file(&dio);

    // restore 'diffexpr'
    အကယ်၍ (restore_diffexpr)
	*p_dex = cc;

    အကယ်၍ (diff_status == FAIL)
	goto done;

    ကိန်းပြည့်		hunk_idx = 0;
    dict_T	*hunk_dict;

    အကယ်၍ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	အကယ်၍ (rettv_list_alloc(rettv) != OK)
	    goto done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = NULL;
	နေစဉ် (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    အကယ်၍ (hunk_dict == NULL)
		goto done;

	    အကယ်၍ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    မဟုတ်ပါက
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    အကယ်၍ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    မဟုတ်ပါက
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
