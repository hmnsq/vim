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

#もし defined(FEAT_DIFF)

static 整数 diff_busy = FALSE;	    // using diff structs, don't change them
static 整数 diff_need_update = FALSE; // ex_diffupdate needs to be called

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
static 整数	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

static long diff_algorithm = XDF_INDENT_HEURISTIC;
static 整数 diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

static 整数 diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# もし defined(MSWIN)
static 整数 diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
typedef struct {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
typedef struct {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
typedef struct {
    linenr_T lnum_orig;
    long     count_orig;
    linenr_T lnum_new;
    long     count_new;
} diffhunk_T;

typedef enum {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
typedef struct {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    整数		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    整数		    dio_ctxlen;   // unified diff context length
} diffio_T;

static 整数 diff_buf_idx(buf_T *buf);
static 整数 diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
static void diff_mark_adjust_tp(tabpage_T *tp, 整数 idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
static void diff_check_unchanged(tabpage_T *tp, diff_T *dp);
static 整数 diff_check_sanity(tabpage_T *tp, diff_T *dp);
static 整数 check_external_diff(diffio_T *diffio);
static 整数 diff_file(diffio_T *diffio);
static 整数 diff_equal_entry(diff_T *dp, 整数 idx1, 整数 idx2);
static 整数 diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
static void diff_fold_update(diff_T *dp, 整数 skip_idx);
# endif
static void diff_read(整数 idx_orig, 整数 idx_new, diffio_T *dio);
static void diff_copy_entry(diff_T *dprev, diff_T *dp, 整数 idx_orig, 整数 idx_new);
static diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
static 整数 parse_diff_ed(char_u *line, diffhunk_T *hunk);
static 整数 parse_diff_unified(char_u *line, diffhunk_T *hunk);
static 整数 xdiff_out_indices(long start_a, long count_a, long start_b, long count_b, void *priv);
static 整数 xdiff_out_unified(void *priv, mmbuffer_t *mb, 整数 nbuf);
static 整数 parse_diffanchors(整数 check_only, buf_T *buf, linenr_T *anchors, 整数 *num_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    ために ((dp) = (tp)->tp_first_diff; (dp) != NULL; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
static diff_T	    *diff_finger_dp = NULL;
static tabpage_T    *diff_finger_tp = NULL;
static 整数	    diff_finger_idx = -1;
static linenr_T	    diff_finger_lnum = 0;

    static void
clear_diffblock(diff_T *dp)
{
    もし (dp == diff_finger_dp)
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
diff_scan_start(tabpage_T *tp, 整数 idx, linenr_T lnum)
{
    もし (diff_finger_dp != NULL
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	戻る diff_finger_dp;
    戻る tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    static void
diff_scan_remember(tabpage_T *tp, 整数 idx, linenr_T lnum, diff_T *dp)
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
    整数		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	もし (i != DB_COUNT)
	{
	    tp->tp_diffbuf[i] = NULL;
	    tp->tp_diff_invalid = TRUE;
	    もし (tp == curtab)
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
    整数		i;

    もし (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    もし (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		break;
	もし (wp == NULL)
	{
	    i = diff_buf_idx(win->w_buffer);
	    もし (i != DB_COUNT)
	    {
		curtab->tp_diffbuf[i] = NULL;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    否则
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
    整数		i;

    もし (diff_buf_idx(buf) != DB_COUNT)
	戻る;		// It's already there.

    ために (i = 0; i < DB_COUNT; ++i)
	もし (curtab->tp_diffbuf[i] == NULL)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    戻る;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_COUNT);
}

/*
 * Remove all buffers to make diffs for.
 */
    static void
diff_buf_clear(void)
{
    整数		i;

    ために (i = 0; i < DB_COUNT; ++i)
	もし (curtab->tp_diffbuf[i] != NULL)
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
    static 整数
diff_buf_idx(buf_T *buf)
{
    整数		idx;

    ために (idx = 0; idx < DB_COUNT; ++idx)
	もし (curtab->tp_diffbuf[idx] == buf)
	    break;
    戻る idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    static 整数
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    整数		idx;

    ために (idx = 0; idx < DB_COUNT; ++idx)
	もし (tp->tp_diffbuf[idx] == buf)
	    break;
    戻る idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    void
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    整数		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	もし (i != DB_COUNT)
	{
	    tp->tp_diff_invalid = TRUE;
	    もし (tp == curtab)
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
    整数		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	もし (idx != DB_COUNT)
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
    整数		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    整数		i;
    整数		inserted, deleted;
    整数		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    整数		check_unchanged;

    diff_finger_dp = NULL;		// may shift df_lnum, invalidate the finger

    もし (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    もし (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    否则 もし (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    否则
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    ために (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	もし ((dp == NULL || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == NULL
		    || dprev->df_lnum[idx] + dprev->df_count[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    もし (dnext == NULL)
		戻る;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_count[idx] = inserted;
	    ために (i = 0; i < DB_COUNT; ++i)
		もし (tp->tp_diffbuf[i] != NULL && i != idx)
		{
		    もし (dprev == NULL)
			dnext->df_lnum[i] = line1;
		    否则
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_count[i])
			    - (dprev->df_lnum[idx] + dprev->df_count[idx]);
		    dnext->df_count[i] = deleted;
		}
	}

	// if at end of the list, quit
	もし (dp == NULL)
	    break;

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
	もし (last >= line1 - 1)
	{
	    もし (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		もし (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		continue;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    もし (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		もし (amount_after == 0)
		    break;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    否则
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		もし (deleted > 0)
		{
		    off = 0;
		    もし (dp->df_lnum[idx] >= line1)
		    {
			もし (last <= line2)
			{
			    // 4. delete all lines of diff
			    もし (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_count[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    否则
				n = deleted - dp->df_count[idx];
			    dp->df_count[idx] = 0;
			}
			否则
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_count[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    否则
		    {
			もし (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_count[idx] -= last - lnum_deleted + 1;
			    もし (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    否则
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			否则
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_count[idx] -= deleted;
			}
		    }

		    ために (i = 0; i < DB_COUNT; ++i)
			もし (tp->tp_diffbuf[i] != NULL && i != idx)
			{
			    もし (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    否则
				dp->df_lnum[i] = 1;
			    dp->df_count[i] += n;
			}
		}
		否则
		{
		    もし (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_count[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    否则
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		もし (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	もし (dprev != NULL && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_count[idx]
							== dp->df_lnum[idx])
	{
	    ために (i = 0; i < DB_COUNT; ++i)
		もし (tp->tp_diffbuf[i] != NULL)
		    dprev->df_count[i] += dp->df_count[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	否则
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    ながら (dp != NULL)
    {
	// All counts are zero, remove this entry.
	ために (i = 0; i < DB_COUNT; ++i)
	    もし (tp->tp_diffbuf[i] != NULL && dp->df_count[i] != 0)
		break;
	もし (i == DB_COUNT)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    もし (dprev == NULL)
		tp->tp_first_diff = dnext;
	    否则
		dprev->df_next = dnext;
	}
	否则
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    もし (tp == curtab)
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
    もし (dnew == NULL)
	戻る NULL;
    diff_finger_dp = NULL;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    もし (dprev == NULL)
	tp->tp_first_diff = dnew;
    否则
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, sizeof(diffline_change_T), 20);
    戻る dnew;
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
    整数		i_org;
    整数		i_new;
    整数		off_org, off_new;
    char_u	*line_org;
    整数		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    ために (i_org = 0; i_org < DB_COUNT; ++i_org)
	もし (tp->tp_diffbuf[i_org] != NULL)
	    break;
    もし (i_org == DB_COUNT)	// safety check
	戻る;

    もし (diff_check_sanity(tp, dp) == FAIL)
	戻る;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    ために (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ながら (dp->df_count[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    もし (dir == BACKWARD)
		off_org = dp->df_count[i_org] - 1;
	    line_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    もし (line_org == NULL)
		戻る;
	    ために (i_new = i_org + 1; i_new < DB_COUNT; ++i_new)
	    {
		もし (tp->tp_diffbuf[i_new] == NULL)
		    continue;
		もし (dir == BACKWARD)
		    off_new = dp->df_count[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		もし (off_new < 0 || off_new >= dp->df_count[i_new])
		    break;
		もし (diff_cmp(line_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    break;
	    }
	    vim_free(line_org);

	    // Stop when a line isn't equal in all diff buffers.
	    もし (i_new != DB_COUNT)
		break;

	    // Line matched in all buffers, remove it from the diff.
	    ために (i_new = i_org; i_new < DB_COUNT; ++i_new)
		もし (tp->tp_diffbuf[i_new] != NULL)
		{
		    もし (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_count[i_new];
		}
	}
	もし (dir == BACKWARD)
	    break;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    static 整数
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    整数		i;

    ために (i = 0; i < DB_COUNT; ++i)
	もし (tp->tp_diffbuf[i] != NULL)
	    もし (dp->df_lnum[i] + dp->df_count[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_line_count)
		戻る FAIL;
    戻る OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    void
diff_redraw(
    整数		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = NULL;
    整数		used_max_fill_other = FALSE;
    整数		used_max_fill_curwin = FALSE;
    整数		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	もし (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    continue;

	redraw_win_later(wp, UPD_SOME_VALID);
	もし (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	もし (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	もし ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    もし (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    否则 もし (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		もし (wp == curwin)
		    used_max_fill_curwin = TRUE;
		否则 もし (wp_other != NULL)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    もし (wp_other != NULL && curwin->w_p_scb)
    {
	もし (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	否则 もし (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    static void
clear_diffin(diffin_T *din)
{
    もし (din->din_fname == NULL)
	VIM_CLEAR(din->din_mmfile.ptr);
    否则
	mch_remove(din->din_fname);
}

    static void
clear_diffout(diffout_T *dout)
{
    もし (dout->dout_fname == NULL)
	ga_clear_strings(&dout->dout_ga);
    否则
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    static 整数
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    もし (end < 0)
	end = buf->b_ml.ml_line_count;

    もし (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = NULL;
	din->din_mmfile.size = 0;
	戻る OK;
    }

    // xdiff requires one big block of memory with all the text.
    ために (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    もし (ptr == NULL)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	もし (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	戻る FAIL;
    }
    din->din_mmfile.ptr = (char *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    ために (lnum = start; lnum <= end; ++lnum)
    {
	ために (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    もし (diff_flags & DIFF_ICASE)
	    {
		整数 c;
		整数	orig_len;
		整数	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		もし (*s == NL)
		    c = NUL;
		否则
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		もし (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		否则
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    もし (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    否则
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    戻る OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    static 整数
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    整数		r;
    整数		save_ml_flags;
    char_u	*save_ff;
    整数		save_cmod_flags;

    もし (din->din_fname == NULL)
	戻る diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    もし (frames_locked())
	戻る FAIL;

    もし (end < 0)
	end = buf->b_ml.ml_line_count;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    もし (end < start)
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
    戻る r;
}

    static 整数
lnum_compare(const void *s1, const void *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    もし (lnum1 < lnum2)
	戻る -1;
    もし (lnum1 > lnum2)
	戻る 1;
    戻る 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    static void
diff_try_update(
	diffio_T    *dio,
	整数	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    整数		idx_new;

    もし (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, sizeof(char *), 1000);
    }
    否则
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	もし (dio->dio_orig.din_fname == NULL
		|| dio->dio_new.din_fname == NULL
		|| dio->dio_diff.dout_fname == NULL)
	    goto theend;
    }

    // Check external diff is actually working.
    もし (!dio->dio_internal && check_external_diff(dio) == FAIL)
	goto theend;

    // :diffupdate!
    もし (eap != NULL && eap->forceit)
	ために (idx_new = idx_orig; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    もし (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    整数 num_anchors = INT_MAX;
    linenr_T anchors[DB_COUNT][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    もし (diff_flags & DIFF_ANCHOR)
    {
	ために (整数 idx = 0; idx < DB_COUNT; idx++)
	{
	    もし (curtab->tp_diffbuf[idx] == NULL)
		continue;
	    整数 buf_num_anchors = 0;
	    もし (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_num_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		num_anchors = 0;
		CLEAR_FIELD(anchors);
		break;
	    }
	    もし (buf_num_anchors < num_anchors)
		num_anchors = buf_num_anchors;

	    もし (buf_num_anchors > 0)
		qsort((void *)anchors[idx],
			(size_t)buf_num_anchors,
			sizeof(linenr_T),
			lnum_compare);
	}
    }
    もし (num_anchors == INT_MAX)
	num_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    ために (整数 anchor_i = 0; anchor_i <= num_anchors; anchor_i++)
    {
	diff_T	*orig_diff = NULL;
	もし (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = NULL;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == num_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	もし (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    もし (orig_diff != NULL)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    goto theend;
	}

	// Make a difference between the first buffer and every other.
	ために (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    もし (buf == NULL || buf->b_ml.ml_mfp == NULL)
		continue; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == num_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    もし (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		continue;
	    もし (diff_file(dio) == FAIL)
		continue;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	もし (anchor_i != 0)
	{
	    diff_finger_dp = NULL;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    ために (diff_T *dp = curtab->tp_first_diff; dp != NULL; dp = dp->df_next)
	    {
		ために (整数 idx = 0; idx < DB_COUNT; idx++)
		{
		    もし (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    もし (orig_diff != NULL)
	    {
		diff_T *last_diff = orig_diff;
		ながら (last_diff->df_next != NULL)
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
    整数
diff_internal(void)
{
    戻る (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    static 整数
diff_internal_failed(void)
{
    整数 idx;

    // Only need to do something when there is another buffer.
    ために (idx = 0; idx < DB_COUNT; ++idx)
	もし (curtab->tp_diffbuf[idx] != NULL
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    戻る TRUE;
    戻る FALSE;
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
    整数		idx_orig;
    整数		idx_new;
    diffio_T	diffio;
    整数		had_diffs = curtab->tp_first_diff != NULL;

    もし (diff_busy)
    {
	diff_need_update = TRUE;
	戻る;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    ために (idx_orig = 0; idx_orig < DB_COUNT; ++idx_orig)
	もし (curtab->tp_diffbuf[idx_orig] != NULL)
	    break;
    もし (idx_orig == DB_COUNT)
	goto theend;

    // Only need to do something when there is another buffer.
    ために (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	もし (curtab->tp_diffbuf[idx_new] != NULL)
	    break;
    もし (idx_new == DB_COUNT)
	goto theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    もし (diffio.dio_internal && diff_internal_failed())
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
    もし (had_diffs || curtab->tp_first_diff != NULL)
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
    static 整数
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    整数		ok;
    整数		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    ために (;;)
    {
	ok = FALSE;
	fd = mch_fopen((char *)diffio->dio_orig.din_fname, "w");
	もし (fd == NULL)
	    io_error = TRUE;
	否则
	{
	    もし (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((char *)diffio->dio_new.din_fname, "w");
	    もし (fd == NULL)
		io_error = TRUE;
	    否则
	    {
		もし (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = NULL;
		もし (diff_file(diffio) == OK)
		    fd = mch_fopen((char *)diffio->dio_diff.dout_fname, "r");
		もし (fd == NULL)
		    io_error = TRUE;
		否则
		{
		    char_u	linebuf[LBUFLEN];

		    ために (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			もし (vim_fgets(linebuf, LBUFLEN, fd))
			    break;
			もし (STRNCMP(linebuf, "1c1", 3) == 0
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
	もし (*p_dex != NUL)
	    break;
# endif

# もし defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	もし (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    continue;
	}
	もし (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	もし (diff_a_works != MAYBE)
	    break;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	もし (ok)
	    break;
    }
    もし (!ok)
    {
	もし (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# もし defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Invoke the xdiff function.
 */
    static 整数
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    もし (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    もし (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    もし (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    もし (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_LINES;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    もし (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    否则
	emit_cb.out_line = xdiff_out_unified;
    もし (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	戻る FAIL;
    }
    もし (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	戻る FAIL;
    }
    戻る OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    static 整数
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    もし (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	戻る OK;
    }
    否则
# endif
    // Use xdiff for generating the diff.
    もし (dio->dio_internal)
	戻る diff_file_internal(dio);

    もし (check_restricted())
	戻る FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    もし (cmd == NULL)
	戻る FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    もし (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((char *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# もし defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# 否则
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (整数)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (void)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    戻る OK;
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
    整数		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = NULL;

# ifdef FEAT_BROWSE
    もし (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, NULL, NULL,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), NULL);
	もし (browseFile == NULL)
	    戻る;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    もし (tmp_orig == NULL || tmp_new == NULL)
	goto theend;

    // Write the current buffer to "tmp_orig".
    もし (buf_write(curbuf, tmp_orig, NULL,
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
    もし (esc_name == NULL)
	goto theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    もし (buf == NULL)
	goto theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    もし (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((char *)dirbuf) != 0)
	dirbuf[0] = NUL;
    否则
    {
#  ifdef TEMPDIRNAMES
	もし (vim_tempdir != NULL)
	    vim_ignored = mch_chdir((char *)vim_tempdir);
	否则
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    もし (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != NULL ? fullname :
#  endif
		eap->arg, tmp_new);
    否则
# endif
    {
	もし (check_restricted())
	    goto theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((char *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(void)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    もし (dirbuf[0] != NUL)
    {
	もし (mch_chdir((char *)dirbuf) != 0)
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
    もし (mch_stat((char *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    否则
    {
	もし (curbuf->b_fname != NULL)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    もし (newname != NULL)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	もし (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    もし (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		もし (newname != NULL)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    もし (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    もし (tmp_orig != NULL)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    もし (tmp_new != NULL)
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

    もし (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	戻る;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    もし (curwin == old_curwin)		// split didn't work
	戻る;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    もし (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	もし (bufref_valid(&old_curbuf))
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
set_diff_option(win_T *wp, 整数 value)
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
    整数		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    もし (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    もし (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    もし (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	もし (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    もし (!wp->w_p_diff)
    {
	もし (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    もし (!wp->w_p_diff)
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
    もし (vim_strchr(p_sbo, 'h') == NULL)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    もし (addbuf)
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
    整数		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	もし (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    もし (wp->w_p_diff_saved)
	    {

		もし (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		もし (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		もし (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    もし (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		もし (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		もし (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		もし (wp->w_p_fen)
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
    もし (eap->forceit)
	diff_buf_clear();

    もし (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    もし (!diffwin && vim_strchr(p_sbo, 'h') != NULL)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    static void
diff_read(
    整数		idx_orig,	// idx of original file
    整数		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = NULL;
    整数		line_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = NULL;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*line;
    long	off;
    整数		i;
    整数		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = NULL;	    // init to avoid gcc warning

    enum {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    もし (dout->dout_fname == NULL)
    {
	diffstyle = DIFF_UNIFIED;
    }
    否则
    {
	fd = mch_fopen((char *)dout->dout_fname, "r");
	もし (fd == NULL)
	{
	    emsg(_(e_cannot_read_diff_output));
	    戻る;
	}
    }

    もし (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	もし (hunk == NULL)
	{
	    もし (fd != NULL)
		fclose(fd);
	    戻る;
	}
    }

    ために (;;)
    {
	もし (dio->dio_internal)
	{
	    もし (line_hunk_idx >= dout->dout_ga.ga_len)
		break;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[line_hunk_idx++];
	}
	否则
	{
	    もし (fd == NULL)
	    {
		もし (line_hunk_idx >= dout->dout_ga.ga_len)
		    break;	    // did last line
		line = ((char_u **)dout->dout_ga.ga_data)[line_hunk_idx++];
	    }
	    否则
	    {
		もし (vim_fgets(linebuf, LBUFLEN, fd))
		    break;		// end of file
		line = linebuf;
	    }

	    もし (diffstyle == DIFF_NONE)
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
		もし (SAFE_isdigit(*line))
		    diffstyle = DIFF_ED;
		否则 もし ((STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		否则 もし ((STRNCMP(line, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		否则
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    continue;
	    }

	    もし (diffstyle == DIFF_ED)
	    {
		もし (!SAFE_isdigit(*line))
		    continue;	// not the start of a diff block
		もし (parse_diff_ed(line, hunk) == FAIL)
		    continue;
	    }
	    否则 もし (diffstyle == DIFF_UNIFIED)
	    {
		もし (STRNCMP(line, "@@ ", 3)  != 0)
		    continue;	// not the start of a diff block
		もし (parse_diff_unified(line, hunk) == FAIL)
		    continue;
	    }
	    否则
	    {
		emsg(_(e_invalid_diff_format));
		break;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ながら (dp != NULL
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_count[idx_orig])
	{
	    もし (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	もし (dp != NULL
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_count[idx_orig]
		&& hunk->lnum_orig + hunk->count_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    ために (dpl = dp; dpl->df_next != NULL; dpl = dpl->df_next)
		もし (hunk->lnum_orig + hunk->count_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    break;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    もし (off > 0)
	    {
		ために (i = idx_orig; i < idx_new; ++i)
		    もし (curtab->tp_diffbuf[i] != NULL)
		    {
			dp->df_lnum[i] -= off;
			dp->df_count[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_count[idx_new] = hunk->count_new;
	    }
	    否则 もし (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_count[idx_new] = hunk->count_new - off;
	    }
	    否则
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		整数 orig_size_in_dp = MIN(hunk->count_orig,
			dp->df_lnum[idx_orig] +
			dp->df_count[idx_orig] - hunk->lnum_orig);
		整数 size_diff = hunk->count_new - orig_size_in_dp;
		dp->df_count[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->count_new
		    - (dp->df_lnum[idx_new] + dp->df_count[idx_new]);
		もし (off > 0)
		    dp->df_count[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->count_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_count[idx_orig]);
	    もし (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		もし (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_count[idx_new] += -off;
		}
		off = 0;
	    }
	    ために (i = idx_orig; i < idx_new; ++i)
		もし (curtab->tp_diffbuf[i] != NULL)
		    dp->df_count[i] = dpl->df_lnum[i] + dpl->df_count[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    ながら (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	否则
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    もし (dp == NULL)
		goto done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_count[idx_orig] = hunk->count_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_count[idx_new] = hunk->count_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    ために (i = idx_orig + 1; i < idx_new; ++i)
		もし (curtab->tp_diffbuf[i] != NULL)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ながら (dp != NULL)
    {
	もし (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    もし (!dio->dio_internal)
	vim_free(hunk);

    もし (fd != NULL)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    static void
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    整数		idx_orig,
    整数		idx_new)
{
    long	off;

    もし (dprev == NULL)
	off = 0;
    否则
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

    ために (p = tp->tp_first_diff; p != NULL; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = NULL;
}

/*
 *  return true if the options are set to use diff linematch
 */
    static 整数
diff_linematch(diff_T *dp)
{
    もし (!(diff_flags & DIFF_LINEMATCH))
	戻る 0;

    // are there more than three diff buffers?
    整数 tsize = 0;
    ために (整数 i = 0; i < DB_COUNT; i++)
    {
	もし (curtab->tp_diffbuf[i] != NULL)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    もし (dp->df_count[i] < 0)
		戻る FALSE;
	    tsize += dp->df_count[i];
	}
    }

    // avoid allocating a huge array because it will lag
    戻る tsize <= linematch_lines;
}

    static 整数
get_max_diff_length(const diff_T *dp)
{
    整数 maxlength = 0;

    ために (整数 k = 0; k < DB_COUNT; k++)
    {
	もし (curtab->tp_diffbuf[k] != NULL)
	{
	    もし (dp->df_count[k] > maxlength)
		maxlength = dp->df_count[k];
	}
    }
    戻る maxlength;
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
    整数		fromidx,
    整数		topline)
{
    diff_T	*topdiff = NULL;
    diff_T	*localtopdiff = NULL;
    整数		topdiffchange = 0;

    ために (topdiff = curtab->tp_first_diff; topdiff != NULL;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	もし (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	もし (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_count[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    もし ((*thistopdiff) == NULL)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	もし (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_count[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    もし (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		break;
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
    const 整数	fromidx,
    const 整数	toidx,
    const 整数	from_topline,
    const 整数	from_topfill,
    整数		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = NULL;
    diff_T	*next_adjacent_blocks = NULL;
    整数		virtual_lines_passed = 0;
    整数		curlinenum_to = 1;

    find_top_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ながら (curdif && (curdif->df_lnum[fromidx] + curdif->df_count[fromidx])
							<= from_topline)
    {
	virtual_lines_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    もし (curdif != next_adjacent_blocks)
	virtual_lines_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_lines_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    もし (virtual_lines_passed < 0)
	virtual_lines_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    もし (thistopdiff != NULL) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    整数 virt_lines_left = virtual_lines_passed;
    curdif = thistopdiff;
    ながら (virt_lines_left > 0 && curdif != NULL && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_lines_left, curdif->df_count[toidx]);
	virt_lines_left -= MIN(virt_lines_left, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    整数 max_virt_lines = 0;
    ために (diff_T *dp = thistopdiff; dp != NULL; dp = dp->df_next)
    {
	もし (dp->df_lnum[toidx] + dp->df_count[toidx] <= curlinenum_to)
	    max_virt_lines += get_max_diff_length(dp);
	否则
	{
	    もし (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_lines += curlinenum_to - dp->df_lnum[toidx];
	    break;
	}
    }

    もし (diff_flags & DIFF_FILLER)
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
    const 整数	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    整数		line_numbers[DB_COUNT];
    整数		outputmap[DB_COUNT];
    size_t	ndiffs = 0;

    ために (整数 i = 0; i < DB_COUNT; i++)
    {
	もし (curtab->tp_diffbuf[i] != NULL)
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
    ために (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	もし (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    ために (整数 j = 0; j < DB_COUNT; j++)
	    {
		もし (curtab->tp_diffbuf[j] != NULL)
		{
		    dp_s->df_lnum[j] = line_numbers[j];
		    dp_s->df_count[j] = 0;
		}
	    }
	}
	ために (size_t j = 0; j < ndiffs; j++)
	{
	    もし (decisions[i] & (1 << j))
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
    整数			diff_length[DB_COUNT];
    size_t		ndiffs = 0;

    ために (整数 i = 0; i < DB_COUNT; i++)
    {
	もし (curtab->tp_diffbuf[i] != NULL)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    もし (dp->df_count[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_count[i] - 1);
	    }
	    否则
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
    整数 *decisions = NULL;
    const 整数 iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    ために (size_t i = 0; i < ndiffs; i++)
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
    整数
diff_check_with_linestatus(win_T *wp, linenr_T lnum, 整数 *linestatus)
{
    整数		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    整数		maxcount;
    整数		i;
    buf_T	*buf = wp->w_buffer;
    整数		cmp;

    もし (linestatus != NULL)
	*linestatus = 0;

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    もし (curtab->tp_first_diff == NULL || !wp->w_p_diff)	// no diffs at all
	戻る 0;

    // safety check: "lnum" must be a buffer line
    もし (lnum < 1 || lnum > buf->b_ml.ml_line_count + 1)
	戻る 0;

    idx = diff_buf_idx(buf);
    もし (idx == DB_COUNT)
	戻る 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    もし (hasFoldingWin(wp, lnum, NULL, NULL, TRUE, NULL))
	戻る 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    ために (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	もし (dp->df_lnum[idx] <= lnum)
	    le = dp;
	もし (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    もし (dp == NULL || lnum < dp->df_lnum[idx])
	戻る 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    もし (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    整数 num_fill = 0;
    ながら (lnum == dp->df_lnum[idx] + dp->df_count[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	もし (diff_flags & DIFF_FILLER)
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
	もし (dp->df_next != NULL
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_count[idx])
	    dp = dp->df_next;
	否则
	    break;
    }

    もし (lnum < dp->df_lnum[idx] + dp->df_count[idx])
    {
	整数	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	ために (i = 0; i < DB_COUNT; ++i)
	    もし (i != idx && curtab->tp_diffbuf[i] != NULL)
	    {
		もし (dp->df_count[i] == 0)
		    zero = TRUE;
		否则
		{
		    もし (dp->df_count[i] != dp->df_count[idx])
		    {
			もし (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			戻る num_fill;
		    }
		    cmp = TRUE;
		}
	    }
	もし (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    ために (i = 0; i < DB_COUNT; ++i)
		もし (i != idx && curtab->tp_diffbuf[i] != NULL
						      && dp->df_count[i] != 0)
		    もし (!diff_equal_entry(dp, idx, i))
		    {
			もし (linestatus)
			    *linestatus = -1;
			戻る num_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	もし (zero == FALSE)
	    戻る num_fill;
	もし (linestatus)
	    *linestatus = -2;
	戻る num_fill;
    }
    戻る num_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    static 整数
diff_equal_entry(diff_T *dp, 整数 idx1, 整数 idx2)
{
    整数		i;
    char_u	*line;
    整数		cmp;

    もし (dp->df_count[idx1] != dp->df_count[idx2])
	戻る FALSE;
    もし (diff_check_sanity(curtab, dp) == FAIL)
	戻る FALSE;
    ために (i = 0; i < dp->df_count[idx1]; ++i)
    {
	line = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	もし (line == NULL)
	    戻る FALSE;
	cmp = diff_cmp(line, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(line);
	もし (cmp != 0)
	    戻る FALSE;
    }
    戻る TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    static 整数
diff_equal_char(char_u *p1, char_u *p2, 整数 *len)
{
    整数 l  = (*mb_ptr2len)(p1);

    もし (l != (*mb_ptr2len)(p2))
	戻る FALSE;
    もし (l > 1)
    {
	もし (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    戻る FALSE;
	*len = l;
    }
    否则
    {
	もし ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    戻る FALSE;
	*len = 1;
    }
    戻る TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    static 整数
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    整数		l;

    もし ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	戻る 0;

    もし ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	戻る STRCMP(s1, s2);
    もし ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	戻る MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ながら (*p1 != NUL && *p2 != NUL)
    {
	もし (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	否则
	{
	    もし (!diff_equal_char(p1, p2, &l))
		break;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    もし (*p1 != NUL || *p2 != NUL)
	戻る 1;
    戻る 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    整数
diff_check_fill(win_T *wp, linenr_T lnum)
{
    整数		n;

    // be quick when there are no filler lines
    もし (!(diff_flags & DIFF_FILLER))
	戻る 0;
    n = diff_check_with_linestatus(wp, lnum, NULL);
    もし (n <= 0)
	戻る 0;
    戻る n;
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
    整数		fromidx;
    整数		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    もし (fromidx == DB_COUNT)
	戻る;		// safety check

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	もし (lnum <= dp->df_lnum[fromidx] + dp->df_count[fromidx])
	    break;
    もし (dp == NULL)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count
				       - (frombuf->b_ml.ml_line_count - lnum);
    }
    否则
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	もし (toidx == DB_COUNT)
	    戻る;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	もし (lnum >= dp->df_lnum[fromidx])
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
    もし (towin->w_topline > towin->w_buffer->b_ml.ml_line_count)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count;
	towin->w_botfill = true;
    }
    もし (towin->w_topline < 1)
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
    static 整数
parse_diffanchors(
    整数		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    整数		*num_anchors)
{
    整数 i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = NULL;
    もし (check_only)
	bufwin = curwin;
    否则
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    もし (bufwin->w_buffer == buf && bufwin->w_p_diff)
		break;
	もし (bufwin == NULL && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    戻る FAIL;
	}
    }

    ために (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	もし (*dia == ',') // don't allow empty values
	    戻る FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(NULL, &dia, ADDR_LINES, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	もし (dia == NULL) // error detected
	    戻る FAIL;
	もし (*dia != ',' && *dia != NUL)
	    戻る FAIL;

	もし (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_line_count + 1))
	{
	    emsg(_(e_invalid_range));
	    戻る FAIL;
	}

	もし (anchors != NULL)
	    anchors[i] = lnum;

	もし (*dia == ',')
	    dia++;
    }
    もし (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	戻る FAIL;
    }
    もし (num_anchors != NULL)
	*num_anchors = i;
    戻る OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    整数
diffanchors_changed(整数 buflocal)
{
    整数 result = parse_diffanchors(TRUE, curbuf, NULL, NULL);
    もし (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    もし (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    否则
	    {
		ために (整数 idx = 0; idx < DB_COUNT; ++idx)
		    もし (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			break;
		    }
	    }
	}
    }
    戻る result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    整数
diffopt_changed(void)
{
    char_u	*p;
    整数		diff_context_new = 6;
    整数		linematch_lines_new = 0;
    整数		diff_flags_new = 0;
    整数		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ながら (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	もし (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	否则 もし (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	否则 もし (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	否则 もし (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	否则 もし (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	否则 もし (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	否则 もし (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	否则 もし (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	否则 もし (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	否则 もし (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	否则 もし (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	否则 もし (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	否则 もし (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	否则 もし (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	否则 もし (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	否则 もし (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	否则 もし (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    もし (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    否则 もし (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    否则 もし (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    否则 もし (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    否则
		戻る FAIL;
	}
	否则 もし (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    もし (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    否则 もし (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    否则 もし (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    否则 もし (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    否则
		戻る FAIL;
	}
	否则 もし (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_lines_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	もし (*p != ',' && *p != NUL)
	    戻る FAIL;
	もし (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    もし ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	戻る FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    もし (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
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

    戻る OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    整数
diffopt_horizontal(void)
{
    戻る (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    整数
diffopt_hiddenoff(void)
{
    戻る (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    整数
diffopt_closeoff(void)
{
    戻る (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    void
diff_update_line(linenr_T lnum)
{
    整数		idx;
    diff_T	*dp;

    もし (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	戻る;

    idx = diff_buf_idx(curbuf);
    もし (idx == DB_COUNT)
	戻る;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	もし (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;

    // clear the inline change cache as it's invalid
    もし (dp != NULL)
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
    整数
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    整数 *change_start,
    整数 *change_end)
{
    もし (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    否则
	*change_start = change->dc_start[diffline->bufidx];
    もし (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    否则
	*change_end = change->dc_end[diffline->bufidx];

    もし (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	戻る FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    ために (整数 i = 0; i < DB_COUNT; i++)
    {
	もし (i == diffline->bufidx)
	    continue;
	もし (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    戻る FALSE;
	}
    }
    戻る TRUE;
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
    static 整数
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    整数		idx,
    整数		*startp,	// first char of the change
    整数		*endp)		// last char of the change
{
    char_u	*line_org;
    char_u	*line_new;
    整数		i;
    整数		si_org, si_new;
    整数		ei_org, ei_new;
    整数		off;
    整数		added = TRUE;
    char_u	*p1, *p2;
    整数		l;

    もし (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	line_org = NULL;
    }
    否则
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	line_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	もし (line_org == NULL)
	    戻る FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    ために (i = 0; i < DB_COUNT; ++i)
	もし (curtab->tp_diffbuf[i] != NULL && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    もし (off >= dp->df_count[i])
		continue;
	    added = FALSE;
	    もし (diff_flags & DIFF_INLINE_NONE)
		break; // early terminate as we only care about the return value

	    line_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ながら (line_org[si_org] != NUL)
	    {
		もし (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(line_org[si_org])
					      && VIM_ISWHITE(line_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(line_org[si_org])
					    || VIM_ISWHITE(line_new[si_new]))))
		{
		    si_org = (整数)(skipwhite(line_org + si_org) - line_org);
		    si_new = (整数)(skipwhite(line_new + si_new) - line_new);
		}
		否则
		{
		    もし (!diff_equal_char(line_org + si_org, line_new + si_new,
									   &l))
			break;
		    si_org += l;
		    si_new += l;
		}
	    }
	    もし (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(line_org, line_org + si_org);
		si_new -= (*mb_head_off)(line_new, line_new + si_new);
	    }
	    もし (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    もし (line_org[si_org] != NUL || line_new[si_new] != NUL)
	    {
		ei_org = (整数)STRLEN(line_org);
		ei_new = (整数)STRLEN(line_new);
		ながら (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    もし (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(line_org[ei_org])
					      && VIM_ISWHITE(line_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(line_org[ei_org])
					    || VIM_ISWHITE(line_new[ei_new]))))
		    {
			ながら (ei_org >= *startp
					     && VIM_ISWHITE(line_org[ei_org]))
			    --ei_org;
			ながら (ei_new >= si_new
					     && VIM_ISWHITE(line_new[ei_new]))
			    --ei_new;
		    }
		    否则
		    {
			p1 = line_org + ei_org;
			p2 = line_new + ei_new;
			p1 -= (*mb_head_off)(line_org, p1);
			p2 -= (*mb_head_off)(line_new, p2);
			もし (!diff_equal_char(p1, p2, &l))
			    break;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		もし (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(line_org);
    戻る added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
typedef struct
{
    long byte_start;
    long num_bytes;
    整数 lineoff;
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
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, 整数 idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    整数 pass = 1;
    do
    {
	整数 has_unmerged_gaps = FALSE;
	整数 has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ながら (dp!= NULL && dp->df_next != NULL)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    もし (dp->df_lnum[idx1] + dp->df_count[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_count[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    もし (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_count[idx1]);
	    もし (gap <= 3)
	    {
		linenr_T max_df_count = 0;
		ために (整数 i = 0; i < DB_COUNT; i++)
		    max_df_count = MAX(max_df_count, dp->df_count[i] + dp->df_next->df_count[i]);

		もし (max_df_count >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    ために (整数 i = 0; i < DB_COUNT; i++)
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
		否则
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	もし (!has_unmerged_gaps || !has_merged_gaps)
	    break;
    } ながら (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    static void
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, 整数 idx1,
				  linenr_T start_lnum)
{
    整数 pass = 1;
    do
    {
	diff_T *dp = dp_orig;

	ながら (dp != NULL && dp->df_next != NULL)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_count[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    もし (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
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
	    もし (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // Calculate the gap between blocks
	    整数 gap_start = entry1->byte_start + entry1->num_bytes;
	    整数 gap_end = entry2->byte_start;
	    整数 gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    もし (gap_size <= 0 || gap_size > diff_word_gap)
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

	    ために (整数 i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		整数 char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		もし (char_class == 2)
		{
		    only_non_word = false;
		    break;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    もし (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		ために (整数 i = 0; i < DB_COUNT; i++)
		{
		    もし (curtab->tp_diffbuf[i] != NULL)
		    {
			// count bytes in the first block
			ために (整数 k = 0; k < dp->df_count[i]; k++)
			{
			    整数 idx = dp->df_lnum[i] + k - 1;
			    もし (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
			// count bytes in the next block
			ために (整数 k = 0; k < dp->df_next->df_count[i]; k++)
			{
			    整数 idx = dp->df_next->df_lnum[i] + k - 1;
			    もし (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
		    }
		}

		もし (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    ために (整数 i = 0; i < DB_COUNT; i++)
		    {
			もし (curtab->tp_diffbuf[i] != NULL)
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
    } ながら (pass++ < 4);  // use limited number of passes to avoid excessive looping
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
    整数		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, sizeof(char *), 1000);

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
    memcpy(orig_diffbuf, curtab->tp_diffbuf, sizeof(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    ために (整数 i = 0; i < DB_COUNT; i++)
	ga_init2(&linemap[i], sizeof(linemap_entry_T), 128);

    ために (整数 i = 0; i < DB_COUNT; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	もし (buf == NULL || buf->b_ml.ml_mfp == NULL)
	    continue; // skip buffer that isn't loaded

	もし (dp->df_count[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = NULL;
	    continue;
	}

	もし (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	ために (整数 off = 0; off < dp->df_count[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    整数 in_keyword = FALSE;

	    // iwhiteeol support vars
	    整数 last_white = FALSE;
	    整数 eol_ga_len = -1;
	    整数 eol_linemap_len = -1;
	    整数 eol_numlines = -1;

	    char_u *s;
	    ために (s = curline; *s != NUL;)
	    {
		整数 new_in_keyword = FALSE;
		もし (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		もし (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		もし (VIM_ISWHITE(*s))
		{
		    もし (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			continue;
		    }
		    否则 もし ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			もし (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		否则
		{
		    もし ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		整数 char_len = 1;
		もし (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		否则
		{
		    char_len = mb_ptr2len(s);

		    もし (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    もし (diff_flags & DIFF_ICASE)
		    {
			整数 c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			整数 c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			整数 c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			もし (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    否则
			ga_concat_len(curstr, s, char_len);
		}

		もし (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		もし (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.num_bytes = char_len;
		    もし (ga_grow(&linemap[i], 1) != OK)
			goto done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		否则
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
	    もし (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    もし ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		もし (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    もし (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.num_bytes = sizeof(NL);
		もし (ga_grow(&linemap[i], 1) != OK)
		    goto done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	もし (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (char *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	否则
	{
	    dio.dio_orig.din_mmfile.ptr = (char *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	もし (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    整数 diff_status = diff_file_internal(&dio);
	    もし (diff_status == FAIL)
		goto done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    もし (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    否则 もし (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    ために (; new_diff != NULL; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	ために (整数 i = 0; i < DB_COUNT; i++)
	{
	    もし (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		continue;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_count[i];

	    もし (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    否则
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    もし (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    否则 もし (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    否则
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].num_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	もし (ga_grow(&dp->df_changes, 1) != OK)
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
    memcpy(curtab->tp_diffbuf, orig_diffbuf, sizeof(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    ために (整数 i = 0; i < DB_COUNT; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    整数
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    整数		idx;
    整数		off;

    idx = diff_buf_idx(wp->w_buffer);
    もし (idx == DB_COUNT)	// cannot happen
	戻る FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	もし (lnum < dp->df_lnum[idx] + dp->df_count[idx])
	    break;
    もし (dp == NULL || diff_check_sanity(curtab, dp) == FAIL)
	戻る FALSE;

    もし (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	戻る FALSE;
    off = lnum - dp->df_lnum[idx];

    もし (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	整数	change_start = MAXCOL;	// first col of changed area
	整数	change_end = -1;	// last col of changed area
	整数	ret;

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
	戻る ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    もし (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    整数 num_changes = 0;
    整数 change_idx = 0;
    diffline->changes = NULL;
    ために (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	もし (change->dc_end_lnum_off[idx] < off)
	    continue;
	もし (change->dc_start_lnum_off[idx] > off)
	    break;
	もし (diffline->changes == NULL)
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
    整数 added = FALSE;
    もし (num_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	ために (整数 i = 0; i < DB_COUNT; i++)
	{
	    もし (idx == i)
		continue;
	    もし (curtab->tp_diffbuf[i] == NULL)
		continue;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    もし (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		break;
	    }
	}
    }
    戻る added;
}

# もし defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    整数
diff_infold(win_T *wp, linenr_T lnum)
{
    整数		i;
    整数		idx = -1;
    整数		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    もし (!wp->w_p_diff)
	戻る FALSE;

    ために (i = 0; i < DB_COUNT; ++i)
    {
	もし (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	否则 もし (curtab->tp_diffbuf[i] != NULL)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    もし (idx == -1 || !other)
	戻る FALSE;

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    もし (curtab->tp_first_diff == NULL)
	戻る TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    整数		result = TRUE;
    ために (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	もし (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	もし (dp->df_lnum[idx] - diff_context > lnum)
	    break;
	// If this change ends before the line we have a match.
	もし (dp->df_lnum[idx] + dp->df_count[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    break;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    戻る result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    void
nv_diffgetput(整数 put, long count)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    もし (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	戻る;
    }
# endif
    もし (count == 0)
	ea.arg = (char_u *)"";
    否则
    {
	vim_snprintf((char *)buf, 30, "%ld", count);
	ea.arg = buf;
    }
    もし (put)
	ea.cmdidx = CMD_diffput;
    否则
	ea.cmdidx = CMD_diffget;
    ea.addr_count = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    static 整数
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	もし (dp == diff)
	    戻る TRUE;
    戻る FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    void
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    整数		count;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    整数		idx_cur;
    整数		idx_other;
    整数		idx_from;
    整数		idx_to;
    整数		i;
    整数		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    整数		start_skip, end_skip;
    整数		new_count;
    整数		buf_empty;
    整数		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    もし (idx_cur == DB_COUNT)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	戻る;
    }

    もし (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	ために (idx_other = 0; idx_other < DB_COUNT; ++idx_other)
	    もし (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != NULL)
	    {
		もし (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    break;
		found_not_ma = TRUE;
	    }
	もし (idx_other == DB_COUNT)
	{
	    もし (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    否则
		emsg(_(e_no_other_buffer_in_diff_mode));
	    戻る;
	}

	// Check that there isn't a third buffer in the list
	ために (i = idx_other + 1; i < DB_COUNT; ++i)
	    もし (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != NULL
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		戻る;
	    }
    }
    否则
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ながら (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	ために (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	もし (eap->arg + i == p)	    // digits only
	    i = atol((char *)eap->arg);
	否则
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    もし (i < 0)
		戻る;		// error message already given
	}
	buf = buflist_findnr(i);
	もし (buf == NULL)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    戻る;
	}
	もし (buf == curbuf)
	    戻る;		// nothing to do
	idx_other = diff_buf_idx(buf);
	もし (idx_other == DB_COUNT)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    戻る;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    もし (eap->addr_count == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	整数 linestatus = 0;
	もし (eap->line1 == curbuf->b_ml.ml_line_count
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	否则 もし (eap->line1 > 0)
	    --eap->line1;
    }

    もし (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    否则
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	もし (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    goto theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    もし (!curbuf->b_changed)
    {
	change_warning(0);
	もし (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    goto theend;
	}
    }

    dprev = NULL;
    ために (dp = curtab->tp_first_diff; dp != NULL; )
    {
	もし (!eap->addr_count)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ながら (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_count[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	もし (dp->df_lnum[idx_cur] > eap->line2 + off)
	    break;	// past the range that was specified

	dfree = NULL;
	lnum = dp->df_lnum[idx_to];
	count = dp->df_count[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + count + (count == 0 && BUFEMPTY() ? 1 : 0);

	もし (dp->df_lnum[idx_cur] + dp->df_count[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    もし (eap->addr_count > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		もし (start_skip > 0)
		{
		    // range starts below start of current diff block
		    もし (start_skip > count)
		    {
			lnum += count;
			count = 0;
		    }
		    否则
		    {
			count -= start_skip;
			lnum += start_skip;
		    }
		}
		否则
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_count[idx_cur] - 1
							 - (eap->line2 + off);
		もし (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    もし (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_count[idx_cur] - start_skip - end_skip;
			もし (count > i)
			    count = i;
		    }
		    否则			// :diffget
		    {
			count -= end_skip;
			end_skip = dp->df_count[idx_from] - start_skip - count;
			もし (end_skip < 0)
			    end_skip = 0;
		    }
		}
		否则
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    ために (i = 0; i < count; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_line_count == 1;
		もし (ml_delete(lnum) == OK)
		    --added;
	    }
	    ために (i = 0; i < dp->df_count[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		もし (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_line_count)
		    break;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		もし (p != NULL)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    もし (buf_empty && curbuf->b_ml.ml_line_count == 2)
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

	    もし (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		ために (i = 0; i < DB_COUNT; ++i)
		    もし (curtab->tp_diffbuf[i] != NULL && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			break;
		もし (i == DB_COUNT)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    もし (dprev == NULL)
			curtab->tp_first_diff = dp;
		    否则
			dprev->df_next = dp;
		}
	    }

	    もし (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + count - 1, (long)MAXLNUM, (long)added);
		もし (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    もし (curwin->w_cursor.lnum >= lnum + count)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_line_count);
		    }
		    否则 もし (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_lines(lnum, 0, lnum + count, (long)added);

	    もし (dfree != NULL)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    もし (added != 0 && !valid_diff(dp))
		break;

	    もし (dfree == NULL)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_count[idx_to] = new_count;

	    // When changing the current buffer, keep track of line numbers
	    もし (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	もし (dfree == NULL)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    もし (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	もし (KeyTyped)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    もし (diff_need_update)
	ex_diffupdate(NULL);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_line_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    もし (curtab->tp_first_diff == NULL)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    もし (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    もし (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    否则
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
diff_fold_update(diff_T *dp, 整数 skip_idx)
{
    整数		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ために (i = 0; i < DB_COUNT; ++i)
	    もし (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_count[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    整数
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	もし (diff_buf_idx_tp(buf, tp) != DB_COUNT)
	    戻る TRUE;
    戻る FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    整数
diff_move_to(整数 dir, long count)
{
    整数		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    もし (idx == DB_COUNT || curtab->tp_first_diff == NULL)
	戻る FAIL;

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    もし (curtab->tp_first_diff == NULL)		// no diffs today
	戻る FAIL;

    ながら (--count >= 0)
    {
	// Check if already before first diff.
	もし (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    break;

	ために (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    もし (dp == NULL)
		break;
	    もし ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == NULL
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		break;
	    }
	}
    }

    // don't end up past the end of the file
    もし (lnum > curbuf->b_ml.ml_line_count)
	lnum = curbuf->b_ml.ml_line_count;

    // When the cursor didn't move at all we fail.
    もし (lnum == curwin->w_cursor.lnum)
	戻る FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    戻る OK;
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
    整数		idx1;
    整数		idx2;
    diff_T	*dp;
    整数		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    もし (idx1 == DB_COUNT || idx2 == DB_COUNT || curtab->tp_first_diff == NULL)
	戻る lnum1;

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    もし (curtab->tp_first_diff == NULL)		// no diffs today
	戻る lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	もし (dp->df_lnum[idx1] > lnum1)
	    戻る lnum1 - baseline;
	もし ((dp->df_lnum[idx1] + dp->df_count[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    もし (baseline > dp->df_count[idx2])
		baseline = dp->df_count[idx2];

	    戻る dp->df_lnum[idx2] + baseline;
	}
	もし (    (dp->df_lnum[idx1] == lnum1)
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
	    戻る curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_count[idx1])
				   - (dp->df_lnum[idx2] + dp->df_count[idx2]);
    }

    // If we get here then the cursor is after the last diff
    戻る lnum1 - baseline;
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
    もし (lnum > curbuf->b_ml.ml_line_count)
	戻る curbuf->b_ml.ml_line_count;
    戻る lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    整数		idx;
    整数		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    もし (idx == DB_COUNT)		// safety check
	戻る (linenr_T)0;

    もし (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	もし (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;

    // When after the last change, compute relative to the last line number.
    もし (dp == NULL)
	戻る wp->w_buffer->b_ml.ml_line_count
					- (curbuf->b_ml.ml_line_count - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    もし (i == DB_COUNT)			// safety check
	戻る (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    もし (n > dp->df_lnum[i] + dp->df_count[i])
	n = dp->df_lnum[i] + dp->df_count[i];
    戻る n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    static 整数
parse_diff_ed(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    整数	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = line;
    f1 = getdigits(&p);
    もし (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    否则
	l1 = f1;
    もし (*p != 'a' && *p != 'c' && *p != 'd')
	戻る FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    もし (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    否则
	l2 = f2;
    もし (l1 < f1 || l2 < f2)
	戻る FAIL;

    もし (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->count_orig = 0;
    }
    否则
    {
	hunk->lnum_orig = f1;
	hunk->count_orig = l1 - f1 + 1;
    }
    もし (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->count_new = 0;
    }
    否则
    {
	hunk->lnum_new = f2;
	hunk->count_new = l2 - f2 + 1;
    }
    戻る OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    static 整数
parse_diff_unified(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = line;
    もし (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	もし (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	否则
	    oldcount = 1;
	もし (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    もし (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    否则
		newcount = 1;
	}
	否则
	    戻る FAIL;	// invalid diff format

	もし (oldcount == 0)
	    oldline += 1;
	もし (newcount == 0)
	    newline += 1;
	もし (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->count_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->count_new = newcount;

	戻る OK;
    }

    戻る FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    static 整数
xdiff_out_indices(
	long start_a,
	long count_a,
	long start_b,
	long count_b,
	void *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    もし (p == NULL)
	戻る -1;

    もし (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	戻る -1;
    }

    p->lnum_orig  = start_a + 1;
    p->count_orig = count_a;
    p->lnum_new   = start_b + 1;
    p->count_new  = count_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    戻る 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    static 整数
xdiff_out_unified(
	void *priv,
	mmbuffer_t *mb,
	整数 nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    整数		i;

    ために (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    戻る 0;
}

#endif	// FEAT_DIFF

#もし defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    void
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    もし (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	戻る;

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
    static 整数		fnum = 0;
    static 整数		prev_diff_flags = 0;
    static 整数		change_start = 0;
    static 整数		change_end = 0;
    static hlf_T	hlID = (hlf_T)0;
    整数			cache_results = TRUE;
    整数			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    もし (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    もし (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    もし (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    もし (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	整数 linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	もし (linestatus < 0)
	{
	    もし (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		もし (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		否则
		{
		    hlID = HLF_CHD;	// changed line
		    もし (diffline.num_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    否则
		hlID = HLF_ADD;	// added line
	}
	否则
	    hlID = (hlf_T)0;

	もし (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    もし (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_number(&argvars[1]) - 1; // ignore type error in {col}
	もし (cache_results)
	{
	    もし (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    否则
		hlID = HLF_CHD;			// changed line
	}
	否则
	{
	    hlID = HLF_CHD;
	    ために (整数 i = 0; i < diffline.num_changes; i++)
	    {
		整数 added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		もし (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    break;
		}
		もし (col < change_start)
		    // the remaining changes are past this column and not relevant
		    break;
	    }
	}
    }
    rettv->vval.v_number = hlID == (hlf_T)0 ? 0 : (整数)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    static 整数
parse_diff_optarg(
    typval_T	    *opts,
    整数		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    整数		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    もし (algo != NULL)
    {
	もし (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	否则 もし (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	否则 もし (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	否则 もし (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    もし (output_fmt != NULL)
    {
	もし (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	否则 もし (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	否则
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    戻る FAIL;
	}
    }

    *diff_ctxlen = dict_get_number_def(d, "context", 0);
    もし (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    もし (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    もし (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    もし (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    もし (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    もし (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    もし (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    戻る OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    static void
list_to_diffin(list_T *l, diffin_T *din, 整数 icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	str = tv_get_string(&li->li_tv);
	もし (icase)
	{
	    str = strlow_save(str);
	    もし (str == NULL)
		continue;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	もし (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (char *)ga.ga_data;
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
    もし (hunk_dict == NULL)
	戻る NULL;

    dict_add_number(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_number(hunk_dict, "from_count", hunk->count_orig);
    dict_add_number(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_number(hunk_dict, "to_count", hunk->count_new);

    戻る hunk_dict;
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

    もし (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	戻る;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, sizeof(char *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    整数		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    もし (argvars[2].v_type != VAR_UNKNOWN)
	もし (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    戻る;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    整数	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    もし (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    整数 diff_status = diff_file(&dio);

    // restore 'diffexpr'
    もし (restore_diffexpr)
	*p_dex = cc;

    もし (diff_status == FAIL)
	goto done;

    整数		hunk_idx = 0;
    dict_T	*hunk_dict;

    もし (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	もし (rettv_list_alloc(rettv) != OK)
	    goto done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = NULL;
	ながら (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    もし (hunk_dict == NULL)
		goto done;

	    もし (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    否则
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    もし (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    否则
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
