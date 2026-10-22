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

#nếu defined(FEAT_DIFF)

static số_nguyên diff_busy = FALSE;	    // using diff structs, don't change them
static số_nguyên diff_need_update = FALSE; // ex_diffupdate needs to be called

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
static số_nguyên	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

static long diff_algorithm = XDF_INDENT_HEURISTIC;
static số_nguyên diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

static số_nguyên diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# nếu defined(MSWIN)
static số_nguyên diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
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
    số_nguyên		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    số_nguyên		    dio_ctxlen;   // unified diff context length
} diffio_T;

static số_nguyên diff_buf_idx(buf_T *buf);
static số_nguyên diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
static void diff_mark_adjust_tp(tabpage_T *tp, số_nguyên idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
static void diff_check_unchanged(tabpage_T *tp, diff_T *dp);
static số_nguyên diff_check_sanity(tabpage_T *tp, diff_T *dp);
static số_nguyên check_external_diff(diffio_T *diffio);
static số_nguyên diff_file(diffio_T *diffio);
static số_nguyên diff_equal_entry(diff_T *dp, số_nguyên idx1, số_nguyên idx2);
static số_nguyên diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
static void diff_fold_update(diff_T *dp, số_nguyên skip_idx);
# endif
static void diff_read(số_nguyên idx_orig, số_nguyên idx_new, diffio_T *dio);
static void diff_copy_entry(diff_T *dprev, diff_T *dp, số_nguyên idx_orig, số_nguyên idx_new);
static diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
static số_nguyên parse_diff_ed(char_u *line, diffhunk_T *hunk);
static số_nguyên parse_diff_unified(char_u *line, diffhunk_T *hunk);
static số_nguyên xdiff_out_indices(long start_a, long count_a, long start_b, long count_b, void *priv);
static số_nguyên xdiff_out_unified(void *priv, mmbuffer_t *mb, số_nguyên nbuf);
static số_nguyên parse_diffanchors(số_nguyên check_only, buf_T *buf, linenr_T *anchors, số_nguyên *num_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    cho ((dp) = (tp)->tp_first_diff; (dp) != NULL; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
static diff_T	    *diff_finger_dp = NULL;
static tabpage_T    *diff_finger_tp = NULL;
static số_nguyên	    diff_finger_idx = -1;
static linenr_T	    diff_finger_lnum = 0;

    static void
clear_diffblock(diff_T *dp)
{
    nếu (dp == diff_finger_dp)
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
diff_scan_start(tabpage_T *tp, số_nguyên idx, linenr_T lnum)
{
    nếu (diff_finger_dp != NULL
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	trả_về diff_finger_dp;
    trả_về tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    static void
diff_scan_remember(tabpage_T *tp, số_nguyên idx, linenr_T lnum, diff_T *dp)
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
    số_nguyên		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	nếu (i != DB_COUNT)
	{
	    tp->tp_diffbuf[i] = NULL;
	    tp->tp_diff_invalid = TRUE;
	    nếu (tp == curtab)
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
    số_nguyên		i;

    nếu (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    nếu (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		break;
	nếu (wp == NULL)
	{
	    i = diff_buf_idx(win->w_buffer);
	    nếu (i != DB_COUNT)
	    {
		curtab->tp_diffbuf[i] = NULL;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    không_thì
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
    số_nguyên		i;

    nếu (diff_buf_idx(buf) != DB_COUNT)
	trả_về;		// It's already there.

    cho (i = 0; i < DB_COUNT; ++i)
	nếu (curtab->tp_diffbuf[i] == NULL)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    trả_về;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_COUNT);
}

/*
 * Remove all buffers to make diffs for.
 */
    static void
diff_buf_clear(void)
{
    số_nguyên		i;

    cho (i = 0; i < DB_COUNT; ++i)
	nếu (curtab->tp_diffbuf[i] != NULL)
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
    static số_nguyên
diff_buf_idx(buf_T *buf)
{
    số_nguyên		idx;

    cho (idx = 0; idx < DB_COUNT; ++idx)
	nếu (curtab->tp_diffbuf[idx] == buf)
	    break;
    trả_về idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    static số_nguyên
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    số_nguyên		idx;

    cho (idx = 0; idx < DB_COUNT; ++idx)
	nếu (tp->tp_diffbuf[idx] == buf)
	    break;
    trả_về idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    void
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    số_nguyên		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	nếu (i != DB_COUNT)
	{
	    tp->tp_diff_invalid = TRUE;
	    nếu (tp == curtab)
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
    số_nguyên		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	nếu (idx != DB_COUNT)
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
    số_nguyên		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    số_nguyên		i;
    số_nguyên		inserted, deleted;
    số_nguyên		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    số_nguyên		check_unchanged;

    diff_finger_dp = NULL;		// may shift df_lnum, invalidate the finger

    nếu (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    nếu (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    không_thì nếu (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    không_thì
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    cho (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	nếu ((dp == NULL || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == NULL
		    || dprev->df_lnum[idx] + dprev->df_count[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    nếu (dnext == NULL)
		trả_về;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_count[idx] = inserted;
	    cho (i = 0; i < DB_COUNT; ++i)
		nếu (tp->tp_diffbuf[i] != NULL && i != idx)
		{
		    nếu (dprev == NULL)
			dnext->df_lnum[i] = line1;
		    không_thì
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_count[i])
			    - (dprev->df_lnum[idx] + dprev->df_count[idx]);
		    dnext->df_count[i] = deleted;
		}
	}

	// if at end of the list, quit
	nếu (dp == NULL)
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
	nếu (last >= line1 - 1)
	{
	    nếu (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		nếu (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		continue;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    nếu (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		nếu (amount_after == 0)
		    break;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    không_thì
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		nếu (deleted > 0)
		{
		    off = 0;
		    nếu (dp->df_lnum[idx] >= line1)
		    {
			nếu (last <= line2)
			{
			    // 4. delete all lines of diff
			    nếu (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_count[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    không_thì
				n = deleted - dp->df_count[idx];
			    dp->df_count[idx] = 0;
			}
			không_thì
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_count[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    không_thì
		    {
			nếu (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_count[idx] -= last - lnum_deleted + 1;
			    nếu (dp->df_next != NULL
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    không_thì
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			không_thì
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_count[idx] -= deleted;
			}
		    }

		    cho (i = 0; i < DB_COUNT; ++i)
			nếu (tp->tp_diffbuf[i] != NULL && i != idx)
			{
			    nếu (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    không_thì
				dp->df_lnum[i] = 1;
			    dp->df_count[i] += n;
			}
		}
		không_thì
		{
		    nếu (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_count[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    không_thì
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		nếu (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	nếu (dprev != NULL && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_count[idx]
							== dp->df_lnum[idx])
	{
	    cho (i = 0; i < DB_COUNT; ++i)
		nếu (tp->tp_diffbuf[i] != NULL)
		    dprev->df_count[i] += dp->df_count[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	không_thì
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = NULL;
    dp = tp->tp_first_diff;
    trong_khi (dp != NULL)
    {
	// All counts are zero, remove this entry.
	cho (i = 0; i < DB_COUNT; ++i)
	    nếu (tp->tp_diffbuf[i] != NULL && dp->df_count[i] != 0)
		break;
	nếu (i == DB_COUNT)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    nếu (dprev == NULL)
		tp->tp_first_diff = dnext;
	    không_thì
		dprev->df_next = dnext;
	}
	không_thì
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    nếu (tp == curtab)
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
    nếu (dnew == NULL)
	trả_về NULL;
    diff_finger_dp = NULL;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    nếu (dprev == NULL)
	tp->tp_first_diff = dnew;
    không_thì
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, sizeof(diffline_change_T), 20);
    trả_về dnew;
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
    số_nguyên		i_org;
    số_nguyên		i_new;
    số_nguyên		off_org, off_new;
    char_u	*line_org;
    số_nguyên		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    cho (i_org = 0; i_org < DB_COUNT; ++i_org)
	nếu (tp->tp_diffbuf[i_org] != NULL)
	    break;
    nếu (i_org == DB_COUNT)	// safety check
	trả_về;

    nếu (diff_check_sanity(tp, dp) == FAIL)
	trả_về;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    cho (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	trong_khi (dp->df_count[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    nếu (dir == BACKWARD)
		off_org = dp->df_count[i_org] - 1;
	    line_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    nếu (line_org == NULL)
		trả_về;
	    cho (i_new = i_org + 1; i_new < DB_COUNT; ++i_new)
	    {
		nếu (tp->tp_diffbuf[i_new] == NULL)
		    continue;
		nếu (dir == BACKWARD)
		    off_new = dp->df_count[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		nếu (off_new < 0 || off_new >= dp->df_count[i_new])
		    break;
		nếu (diff_cmp(line_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    break;
	    }
	    vim_free(line_org);

	    // Stop when a line isn't equal in all diff buffers.
	    nếu (i_new != DB_COUNT)
		break;

	    // Line matched in all buffers, remove it from the diff.
	    cho (i_new = i_org; i_new < DB_COUNT; ++i_new)
		nếu (tp->tp_diffbuf[i_new] != NULL)
		{
		    nếu (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_count[i_new];
		}
	}
	nếu (dir == BACKWARD)
	    break;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    static số_nguyên
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    số_nguyên		i;

    cho (i = 0; i < DB_COUNT; ++i)
	nếu (tp->tp_diffbuf[i] != NULL)
	    nếu (dp->df_lnum[i] + dp->df_count[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_line_count)
		trả_về FAIL;
    trả_về OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    void
diff_redraw(
    số_nguyên		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = NULL;
    số_nguyên		used_max_fill_other = FALSE;
    số_nguyên		used_max_fill_curwin = FALSE;
    số_nguyên		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	nếu (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    continue;

	redraw_win_later(wp, UPD_SOME_VALID);
	nếu (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	nếu (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	nếu ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    nếu (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    không_thì nếu (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		nếu (wp == curwin)
		    used_max_fill_curwin = TRUE;
		không_thì nếu (wp_other != NULL)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    nếu (wp_other != NULL && curwin->w_p_scb)
    {
	nếu (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	không_thì nếu (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    static void
clear_diffin(diffin_T *din)
{
    nếu (din->din_fname == NULL)
	VIM_CLEAR(din->din_mmfile.ptr);
    không_thì
	mch_remove(din->din_fname);
}

    static void
clear_diffout(diffout_T *dout)
{
    nếu (dout->dout_fname == NULL)
	ga_clear_strings(&dout->dout_ga);
    không_thì
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    static số_nguyên
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    nếu (end < 0)
	end = buf->b_ml.ml_line_count;

    nếu (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = NULL;
	din->din_mmfile.size = 0;
	trả_về OK;
    }

    // xdiff requires one big block of memory with all the text.
    cho (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    nếu (ptr == NULL)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	nếu (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	trả_về FAIL;
    }
    din->din_mmfile.ptr = (char *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    cho (lnum = start; lnum <= end; ++lnum)
    {
	cho (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    nếu (diff_flags & DIFF_ICASE)
	    {
		số_nguyên c;
		số_nguyên	orig_len;
		số_nguyên	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		nếu (*s == NL)
		    c = NUL;
		không_thì
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		nếu (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		không_thì
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    nếu (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    không_thì
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    trả_về OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    static số_nguyên
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    số_nguyên		r;
    số_nguyên		save_ml_flags;
    char_u	*save_ff;
    số_nguyên		save_cmod_flags;

    nếu (din->din_fname == NULL)
	trả_về diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    nếu (frames_locked())
	trả_về FAIL;

    nếu (end < 0)
	end = buf->b_ml.ml_line_count;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    nếu (end < start)
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
    trả_về r;
}

    static số_nguyên
lnum_compare(const void *s1, const void *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    nếu (lnum1 < lnum2)
	trả_về -1;
    nếu (lnum1 > lnum2)
	trả_về 1;
    trả_về 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    static void
diff_try_update(
	diffio_T    *dio,
	số_nguyên	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    số_nguyên		idx_new;

    nếu (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, sizeof(char *), 1000);
    }
    không_thì
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	nếu (dio->dio_orig.din_fname == NULL
		|| dio->dio_new.din_fname == NULL
		|| dio->dio_diff.dout_fname == NULL)
	    goto theend;
    }

    // Check external diff is actually working.
    nếu (!dio->dio_internal && check_external_diff(dio) == FAIL)
	goto theend;

    // :diffupdate!
    nếu (eap != NULL && eap->forceit)
	cho (idx_new = idx_orig; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    nếu (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    số_nguyên num_anchors = INT_MAX;
    linenr_T anchors[DB_COUNT][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    nếu (diff_flags & DIFF_ANCHOR)
    {
	cho (số_nguyên idx = 0; idx < DB_COUNT; idx++)
	{
	    nếu (curtab->tp_diffbuf[idx] == NULL)
		continue;
	    số_nguyên buf_num_anchors = 0;
	    nếu (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_num_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		num_anchors = 0;
		CLEAR_FIELD(anchors);
		break;
	    }
	    nếu (buf_num_anchors < num_anchors)
		num_anchors = buf_num_anchors;

	    nếu (buf_num_anchors > 0)
		qsort((void *)anchors[idx],
			(size_t)buf_num_anchors,
			sizeof(linenr_T),
			lnum_compare);
	}
    }
    nếu (num_anchors == INT_MAX)
	num_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    cho (số_nguyên anchor_i = 0; anchor_i <= num_anchors; anchor_i++)
    {
	diff_T	*orig_diff = NULL;
	nếu (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = NULL;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == num_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	nếu (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    nếu (orig_diff != NULL)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    goto theend;
	}

	// Make a difference between the first buffer and every other.
	cho (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    nếu (buf == NULL || buf->b_ml.ml_mfp == NULL)
		continue; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == num_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    nếu (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		continue;
	    nếu (diff_file(dio) == FAIL)
		continue;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	nếu (anchor_i != 0)
	{
	    diff_finger_dp = NULL;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    cho (diff_T *dp = curtab->tp_first_diff; dp != NULL; dp = dp->df_next)
	    {
		cho (số_nguyên idx = 0; idx < DB_COUNT; idx++)
		{
		    nếu (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    nếu (orig_diff != NULL)
	    {
		diff_T *last_diff = orig_diff;
		trong_khi (last_diff->df_next != NULL)
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
    số_nguyên
diff_internal(void)
{
    trả_về (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    static số_nguyên
diff_internal_failed(void)
{
    số_nguyên idx;

    // Only need to do something when there is another buffer.
    cho (idx = 0; idx < DB_COUNT; ++idx)
	nếu (curtab->tp_diffbuf[idx] != NULL
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    trả_về TRUE;
    trả_về FALSE;
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
    số_nguyên		idx_orig;
    số_nguyên		idx_new;
    diffio_T	diffio;
    số_nguyên		had_diffs = curtab->tp_first_diff != NULL;

    nếu (diff_busy)
    {
	diff_need_update = TRUE;
	trả_về;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    cho (idx_orig = 0; idx_orig < DB_COUNT; ++idx_orig)
	nếu (curtab->tp_diffbuf[idx_orig] != NULL)
	    break;
    nếu (idx_orig == DB_COUNT)
	goto theend;

    // Only need to do something when there is another buffer.
    cho (idx_new = idx_orig + 1; idx_new < DB_COUNT; ++idx_new)
	nếu (curtab->tp_diffbuf[idx_new] != NULL)
	    break;
    nếu (idx_new == DB_COUNT)
	goto theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    nếu (diffio.dio_internal && diff_internal_failed())
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
    nếu (had_diffs || curtab->tp_first_diff != NULL)
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
    static số_nguyên
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    số_nguyên		ok;
    số_nguyên		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    cho (;;)
    {
	ok = FALSE;
	fd = mch_fopen((char *)diffio->dio_orig.din_fname, "w");
	nếu (fd == NULL)
	    io_error = TRUE;
	không_thì
	{
	    nếu (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((char *)diffio->dio_new.din_fname, "w");
	    nếu (fd == NULL)
		io_error = TRUE;
	    không_thì
	    {
		nếu (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = NULL;
		nếu (diff_file(diffio) == OK)
		    fd = mch_fopen((char *)diffio->dio_diff.dout_fname, "r");
		nếu (fd == NULL)
		    io_error = TRUE;
		không_thì
		{
		    char_u	linebuf[LBUFLEN];

		    cho (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			nếu (vim_fgets(linebuf, LBUFLEN, fd))
			    break;
			nếu (STRNCMP(linebuf, "1c1", 3) == 0
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
	nếu (*p_dex != NUL)
	    break;
# endif

# nếu defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	nếu (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    continue;
	}
	nếu (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	nếu (diff_a_works != MAYBE)
	    break;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	nếu (ok)
	    break;
    }
    nếu (!ok)
    {
	nếu (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# nếu defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Invoke the xdiff function.
 */
    static số_nguyên
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    nếu (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    nếu (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    nếu (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    nếu (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_LINES;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    nếu (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    không_thì
	emit_cb.out_line = xdiff_out_unified;
    nếu (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	trả_về FAIL;
    }
    nếu (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    static số_nguyên
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    nếu (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	trả_về OK;
    }
    không_thì
# endif
    // Use xdiff for generating the diff.
    nếu (dio->dio_internal)
	trả_về diff_file_internal(dio);

    nếu (check_restricted())
	trả_về FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    nếu (cmd == NULL)
	trả_về FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    nếu (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((char *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# nếu defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# không_thì
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (số_nguyên)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (void)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    trả_về OK;
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
    số_nguyên		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = NULL;

# ifdef FEAT_BROWSE
    nếu (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, NULL, NULL,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), NULL);
	nếu (browseFile == NULL)
	    trả_về;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    nếu (tmp_orig == NULL || tmp_new == NULL)
	goto theend;

    // Write the current buffer to "tmp_orig".
    nếu (buf_write(curbuf, tmp_orig, NULL,
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
    nếu (esc_name == NULL)
	goto theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    nếu (buf == NULL)
	goto theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    nếu (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((char *)dirbuf) != 0)
	dirbuf[0] = NUL;
    không_thì
    {
#  ifdef TEMPDIRNAMES
	nếu (vim_tempdir != NULL)
	    vim_ignored = mch_chdir((char *)vim_tempdir);
	không_thì
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    nếu (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != NULL ? fullname :
#  endif
		eap->arg, tmp_new);
    không_thì
# endif
    {
	nếu (check_restricted())
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
    nếu (dirbuf[0] != NUL)
    {
	nếu (mch_chdir((char *)dirbuf) != 0)
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
    nếu (mch_stat((char *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    không_thì
    {
	nếu (curbuf->b_fname != NULL)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    nếu (newname != NULL)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	nếu (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    nếu (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		nếu (newname != NULL)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    nếu (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    nếu (tmp_orig != NULL)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    nếu (tmp_new != NULL)
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

    nếu (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	trả_về;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    nếu (curwin == old_curwin)		// split didn't work
	trả_về;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    nếu (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	nếu (bufref_valid(&old_curbuf))
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
set_diff_option(win_T *wp, số_nguyên value)
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
    số_nguyên		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    nếu (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    nếu (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    nếu (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	nếu (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    nếu (!wp->w_p_diff)
    {
	nếu (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    nếu (!wp->w_p_diff)
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
    nếu (vim_strchr(p_sbo, 'h') == NULL)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    nếu (addbuf)
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
    số_nguyên		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	nếu (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    nếu (wp->w_p_diff_saved)
	    {

		nếu (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		nếu (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		nếu (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    nếu (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		nếu (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		nếu (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		nếu (wp->w_p_fen)
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
    nếu (eap->forceit)
	diff_buf_clear();

    nếu (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    nếu (!diffwin && vim_strchr(p_sbo, 'h') != NULL)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    static void
diff_read(
    số_nguyên		idx_orig,	// idx of original file
    số_nguyên		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = NULL;
    số_nguyên		line_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = NULL;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*line;
    long	off;
    số_nguyên		i;
    số_nguyên		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = NULL;	    // init to avoid gcc warning

    enum {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    nếu (dout->dout_fname == NULL)
    {
	diffstyle = DIFF_UNIFIED;
    }
    không_thì
    {
	fd = mch_fopen((char *)dout->dout_fname, "r");
	nếu (fd == NULL)
	{
	    emsg(_(e_cannot_read_diff_output));
	    trả_về;
	}
    }

    nếu (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	nếu (hunk == NULL)
	{
	    nếu (fd != NULL)
		fclose(fd);
	    trả_về;
	}
    }

    cho (;;)
    {
	nếu (dio->dio_internal)
	{
	    nếu (line_hunk_idx >= dout->dout_ga.ga_len)
		break;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[line_hunk_idx++];
	}
	không_thì
	{
	    nếu (fd == NULL)
	    {
		nếu (line_hunk_idx >= dout->dout_ga.ga_len)
		    break;	    // did last line
		line = ((char_u **)dout->dout_ga.ga_data)[line_hunk_idx++];
	    }
	    không_thì
	    {
		nếu (vim_fgets(linebuf, LBUFLEN, fd))
		    break;		// end of file
		line = linebuf;
	    }

	    nếu (diffstyle == DIFF_NONE)
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
		nếu (SAFE_isdigit(*line))
		    diffstyle = DIFF_ED;
		không_thì nếu ((STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		không_thì nếu ((STRNCMP(line, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(line, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		không_thì
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    continue;
	    }

	    nếu (diffstyle == DIFF_ED)
	    {
		nếu (!SAFE_isdigit(*line))
		    continue;	// not the start of a diff block
		nếu (parse_diff_ed(line, hunk) == FAIL)
		    continue;
	    }
	    không_thì nếu (diffstyle == DIFF_UNIFIED)
	    {
		nếu (STRNCMP(line, "@@ ", 3)  != 0)
		    continue;	// not the start of a diff block
		nếu (parse_diff_unified(line, hunk) == FAIL)
		    continue;
	    }
	    không_thì
	    {
		emsg(_(e_invalid_diff_format));
		break;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	trong_khi (dp != NULL
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_count[idx_orig])
	{
	    nếu (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	nếu (dp != NULL
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_count[idx_orig]
		&& hunk->lnum_orig + hunk->count_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    cho (dpl = dp; dpl->df_next != NULL; dpl = dpl->df_next)
		nếu (hunk->lnum_orig + hunk->count_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    break;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    nếu (off > 0)
	    {
		cho (i = idx_orig; i < idx_new; ++i)
		    nếu (curtab->tp_diffbuf[i] != NULL)
		    {
			dp->df_lnum[i] -= off;
			dp->df_count[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_count[idx_new] = hunk->count_new;
	    }
	    không_thì nếu (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_count[idx_new] = hunk->count_new - off;
	    }
	    không_thì
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		số_nguyên orig_size_in_dp = MIN(hunk->count_orig,
			dp->df_lnum[idx_orig] +
			dp->df_count[idx_orig] - hunk->lnum_orig);
		số_nguyên size_diff = hunk->count_new - orig_size_in_dp;
		dp->df_count[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->count_new
		    - (dp->df_lnum[idx_new] + dp->df_count[idx_new]);
		nếu (off > 0)
		    dp->df_count[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->count_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_count[idx_orig]);
	    nếu (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		nếu (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_count[idx_new] += -off;
		}
		off = 0;
	    }
	    cho (i = idx_orig; i < idx_new; ++i)
		nếu (curtab->tp_diffbuf[i] != NULL)
		    dp->df_count[i] = dpl->df_lnum[i] + dpl->df_count[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    trong_khi (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	không_thì
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    nếu (dp == NULL)
		goto done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_count[idx_orig] = hunk->count_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_count[idx_new] = hunk->count_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    cho (i = idx_orig + 1; i < idx_new; ++i)
		nếu (curtab->tp_diffbuf[i] != NULL)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    trong_khi (dp != NULL)
    {
	nếu (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    nếu (!dio->dio_internal)
	vim_free(hunk);

    nếu (fd != NULL)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    static void
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    số_nguyên		idx_orig,
    số_nguyên		idx_new)
{
    long	off;

    nếu (dprev == NULL)
	off = 0;
    không_thì
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

    cho (p = tp->tp_first_diff; p != NULL; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = NULL;
}

/*
 *  return true if the options are set to use diff linematch
 */
    static số_nguyên
diff_linematch(diff_T *dp)
{
    nếu (!(diff_flags & DIFF_LINEMATCH))
	trả_về 0;

    // are there more than three diff buffers?
    số_nguyên tsize = 0;
    cho (số_nguyên i = 0; i < DB_COUNT; i++)
    {
	nếu (curtab->tp_diffbuf[i] != NULL)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    nếu (dp->df_count[i] < 0)
		trả_về FALSE;
	    tsize += dp->df_count[i];
	}
    }

    // avoid allocating a huge array because it will lag
    trả_về tsize <= linematch_lines;
}

    static số_nguyên
get_max_diff_length(const diff_T *dp)
{
    số_nguyên maxlength = 0;

    cho (số_nguyên k = 0; k < DB_COUNT; k++)
    {
	nếu (curtab->tp_diffbuf[k] != NULL)
	{
	    nếu (dp->df_count[k] > maxlength)
		maxlength = dp->df_count[k];
	}
    }
    trả_về maxlength;
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
    số_nguyên		fromidx,
    số_nguyên		topline)
{
    diff_T	*topdiff = NULL;
    diff_T	*localtopdiff = NULL;
    số_nguyên		topdiffchange = 0;

    cho (topdiff = curtab->tp_first_diff; topdiff != NULL;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	nếu (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	nếu (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_count[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    nếu ((*thistopdiff) == NULL)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	nếu (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_count[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    nếu (*thistopdiff)
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
    const số_nguyên	fromidx,
    const số_nguyên	toidx,
    const số_nguyên	from_topline,
    const số_nguyên	from_topfill,
    số_nguyên		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = NULL;
    diff_T	*next_adjacent_blocks = NULL;
    số_nguyên		virtual_lines_passed = 0;
    số_nguyên		curlinenum_to = 1;

    find_top_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    trong_khi (curdif && (curdif->df_lnum[fromidx] + curdif->df_count[fromidx])
							<= from_topline)
    {
	virtual_lines_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    nếu (curdif != next_adjacent_blocks)
	virtual_lines_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_lines_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    nếu (virtual_lines_passed < 0)
	virtual_lines_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    nếu (thistopdiff != NULL) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    số_nguyên virt_lines_left = virtual_lines_passed;
    curdif = thistopdiff;
    trong_khi (virt_lines_left > 0 && curdif != NULL && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_lines_left, curdif->df_count[toidx]);
	virt_lines_left -= MIN(virt_lines_left, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    số_nguyên max_virt_lines = 0;
    cho (diff_T *dp = thistopdiff; dp != NULL; dp = dp->df_next)
    {
	nếu (dp->df_lnum[toidx] + dp->df_count[toidx] <= curlinenum_to)
	    max_virt_lines += get_max_diff_length(dp);
	không_thì
	{
	    nếu (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_lines += curlinenum_to - dp->df_lnum[toidx];
	    break;
	}
    }

    nếu (diff_flags & DIFF_FILLER)
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
    const số_nguyên	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    số_nguyên		line_numbers[DB_COUNT];
    số_nguyên		outputmap[DB_COUNT];
    size_t	ndiffs = 0;

    cho (số_nguyên i = 0; i < DB_COUNT; i++)
    {
	nếu (curtab->tp_diffbuf[i] != NULL)
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
    cho (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	nếu (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    cho (số_nguyên j = 0; j < DB_COUNT; j++)
	    {
		nếu (curtab->tp_diffbuf[j] != NULL)
		{
		    dp_s->df_lnum[j] = line_numbers[j];
		    dp_s->df_count[j] = 0;
		}
	    }
	}
	cho (size_t j = 0; j < ndiffs; j++)
	{
	    nếu (decisions[i] & (1 << j))
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
    số_nguyên			diff_length[DB_COUNT];
    size_t		ndiffs = 0;

    cho (số_nguyên i = 0; i < DB_COUNT; i++)
    {
	nếu (curtab->tp_diffbuf[i] != NULL)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    nếu (dp->df_count[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_count[i] - 1);
	    }
	    không_thì
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
    số_nguyên *decisions = NULL;
    const số_nguyên iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    cho (size_t i = 0; i < ndiffs; i++)
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
    số_nguyên
diff_check_with_linestatus(win_T *wp, linenr_T lnum, số_nguyên *linestatus)
{
    số_nguyên		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    số_nguyên		maxcount;
    số_nguyên		i;
    buf_T	*buf = wp->w_buffer;
    số_nguyên		cmp;

    nếu (linestatus != NULL)
	*linestatus = 0;

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    nếu (curtab->tp_first_diff == NULL || !wp->w_p_diff)	// no diffs at all
	trả_về 0;

    // safety check: "lnum" must be a buffer line
    nếu (lnum < 1 || lnum > buf->b_ml.ml_line_count + 1)
	trả_về 0;

    idx = diff_buf_idx(buf);
    nếu (idx == DB_COUNT)
	trả_về 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    nếu (hasFoldingWin(wp, lnum, NULL, NULL, TRUE, NULL))
	trả_về 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    cho (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	nếu (dp->df_lnum[idx] <= lnum)
	    le = dp;
	nếu (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    nếu (dp == NULL || lnum < dp->df_lnum[idx])
	trả_về 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    nếu (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    số_nguyên num_fill = 0;
    trong_khi (lnum == dp->df_lnum[idx] + dp->df_count[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	nếu (diff_flags & DIFF_FILLER)
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
	nếu (dp->df_next != NULL
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_count[idx])
	    dp = dp->df_next;
	không_thì
	    break;
    }

    nếu (lnum < dp->df_lnum[idx] + dp->df_count[idx])
    {
	số_nguyên	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	cho (i = 0; i < DB_COUNT; ++i)
	    nếu (i != idx && curtab->tp_diffbuf[i] != NULL)
	    {
		nếu (dp->df_count[i] == 0)
		    zero = TRUE;
		không_thì
		{
		    nếu (dp->df_count[i] != dp->df_count[idx])
		    {
			nếu (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			trả_về num_fill;
		    }
		    cmp = TRUE;
		}
	    }
	nếu (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    cho (i = 0; i < DB_COUNT; ++i)
		nếu (i != idx && curtab->tp_diffbuf[i] != NULL
						      && dp->df_count[i] != 0)
		    nếu (!diff_equal_entry(dp, idx, i))
		    {
			nếu (linestatus)
			    *linestatus = -1;
			trả_về num_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	nếu (zero == FALSE)
	    trả_về num_fill;
	nếu (linestatus)
	    *linestatus = -2;
	trả_về num_fill;
    }
    trả_về num_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    static số_nguyên
diff_equal_entry(diff_T *dp, số_nguyên idx1, số_nguyên idx2)
{
    số_nguyên		i;
    char_u	*line;
    số_nguyên		cmp;

    nếu (dp->df_count[idx1] != dp->df_count[idx2])
	trả_về FALSE;
    nếu (diff_check_sanity(curtab, dp) == FAIL)
	trả_về FALSE;
    cho (i = 0; i < dp->df_count[idx1]; ++i)
    {
	line = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	nếu (line == NULL)
	    trả_về FALSE;
	cmp = diff_cmp(line, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(line);
	nếu (cmp != 0)
	    trả_về FALSE;
    }
    trả_về TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    static số_nguyên
diff_equal_char(char_u *p1, char_u *p2, số_nguyên *len)
{
    số_nguyên l  = (*mb_ptr2len)(p1);

    nếu (l != (*mb_ptr2len)(p2))
	trả_về FALSE;
    nếu (l > 1)
    {
	nếu (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    trả_về FALSE;
	*len = l;
    }
    không_thì
    {
	nếu ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    trả_về FALSE;
	*len = 1;
    }
    trả_về TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    static số_nguyên
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    số_nguyên		l;

    nếu ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	trả_về 0;

    nếu ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	trả_về STRCMP(s1, s2);
    nếu ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	trả_về MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    trong_khi (*p1 != NUL && *p2 != NUL)
    {
	nếu (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	không_thì
	{
	    nếu (!diff_equal_char(p1, p2, &l))
		break;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    nếu (*p1 != NUL || *p2 != NUL)
	trả_về 1;
    trả_về 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    số_nguyên
diff_check_fill(win_T *wp, linenr_T lnum)
{
    số_nguyên		n;

    // be quick when there are no filler lines
    nếu (!(diff_flags & DIFF_FILLER))
	trả_về 0;
    n = diff_check_with_linestatus(wp, lnum, NULL);
    nếu (n <= 0)
	trả_về 0;
    trả_về n;
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
    số_nguyên		fromidx;
    số_nguyên		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    nếu (fromidx == DB_COUNT)
	trả_về;		// safety check

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	nếu (lnum <= dp->df_lnum[fromidx] + dp->df_count[fromidx])
	    break;
    nếu (dp == NULL)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count
				       - (frombuf->b_ml.ml_line_count - lnum);
    }
    không_thì
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	nếu (toidx == DB_COUNT)
	    trả_về;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	nếu (lnum >= dp->df_lnum[fromidx])
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
    nếu (towin->w_topline > towin->w_buffer->b_ml.ml_line_count)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_line_count;
	towin->w_botfill = true;
    }
    nếu (towin->w_topline < 1)
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
    static số_nguyên
parse_diffanchors(
    số_nguyên		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    số_nguyên		*num_anchors)
{
    số_nguyên i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = NULL;
    nếu (check_only)
	bufwin = curwin;
    không_thì
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    nếu (bufwin->w_buffer == buf && bufwin->w_p_diff)
		break;
	nếu (bufwin == NULL && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    trả_về FAIL;
	}
    }

    cho (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	nếu (*dia == ',') // don't allow empty values
	    trả_về FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(NULL, &dia, ADDR_LINES, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	nếu (dia == NULL) // error detected
	    trả_về FAIL;
	nếu (*dia != ',' && *dia != NUL)
	    trả_về FAIL;

	nếu (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_line_count + 1))
	{
	    emsg(_(e_invalid_range));
	    trả_về FAIL;
	}

	nếu (anchors != NULL)
	    anchors[i] = lnum;

	nếu (*dia == ',')
	    dia++;
    }
    nếu (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	trả_về FAIL;
    }
    nếu (num_anchors != NULL)
	*num_anchors = i;
    trả_về OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    số_nguyên
diffanchors_changed(số_nguyên buflocal)
{
    số_nguyên result = parse_diffanchors(TRUE, curbuf, NULL, NULL);
    nếu (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    nếu (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    không_thì
	    {
		cho (số_nguyên idx = 0; idx < DB_COUNT; ++idx)
		    nếu (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			break;
		    }
	    }
	}
    }
    trả_về result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    số_nguyên
diffopt_changed(void)
{
    char_u	*p;
    số_nguyên		diff_context_new = 6;
    số_nguyên		linematch_lines_new = 0;
    số_nguyên		diff_flags_new = 0;
    số_nguyên		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    trong_khi (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	nếu (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	không_thì nếu (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	không_thì nếu (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	không_thì nếu (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	không_thì nếu (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	không_thì nếu (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	không_thì nếu (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	không_thì nếu (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	không_thì nếu (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	không_thì nếu (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	không_thì nếu (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	không_thì nếu (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	không_thì nếu (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	không_thì nếu (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	không_thì nếu (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	không_thì nếu (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	không_thì nếu (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    nếu (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    không_thì nếu (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    không_thì nếu (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    không_thì nếu (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    không_thì
		trả_về FAIL;
	}
	không_thì nếu (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    nếu (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    không_thì nếu (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    không_thì nếu (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    không_thì nếu (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    không_thì
		trả_về FAIL;
	}
	không_thì nếu (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_lines_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	nếu (*p != ',' && *p != NUL)
	    trả_về FAIL;
	nếu (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    nếu ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	trả_về FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    nếu (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
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

    trả_về OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    số_nguyên
diffopt_horizontal(void)
{
    trả_về (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    số_nguyên
diffopt_hiddenoff(void)
{
    trả_về (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    số_nguyên
diffopt_closeoff(void)
{
    trả_về (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    void
diff_update_line(linenr_T lnum)
{
    số_nguyên		idx;
    diff_T	*dp;

    nếu (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	trả_về;

    idx = diff_buf_idx(curbuf);
    nếu (idx == DB_COUNT)
	trả_về;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	nếu (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;

    // clear the inline change cache as it's invalid
    nếu (dp != NULL)
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
    số_nguyên
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    số_nguyên *change_start,
    số_nguyên *change_end)
{
    nếu (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    không_thì
	*change_start = change->dc_start[diffline->bufidx];
    nếu (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    không_thì
	*change_end = change->dc_end[diffline->bufidx];

    nếu (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	trả_về FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    cho (số_nguyên i = 0; i < DB_COUNT; i++)
    {
	nếu (i == diffline->bufidx)
	    continue;
	nếu (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    trả_về FALSE;
	}
    }
    trả_về TRUE;
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
    static số_nguyên
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    số_nguyên		idx,
    số_nguyên		*startp,	// first char of the change
    số_nguyên		*endp)		// last char of the change
{
    char_u	*line_org;
    char_u	*line_new;
    số_nguyên		i;
    số_nguyên		si_org, si_new;
    số_nguyên		ei_org, ei_new;
    số_nguyên		off;
    số_nguyên		added = TRUE;
    char_u	*p1, *p2;
    số_nguyên		l;

    nếu (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	line_org = NULL;
    }
    không_thì
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	line_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	nếu (line_org == NULL)
	    trả_về FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    cho (i = 0; i < DB_COUNT; ++i)
	nếu (curtab->tp_diffbuf[i] != NULL && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    nếu (off >= dp->df_count[i])
		continue;
	    added = FALSE;
	    nếu (diff_flags & DIFF_INLINE_NONE)
		break; // early terminate as we only care about the return value

	    line_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    trong_khi (line_org[si_org] != NUL)
	    {
		nếu (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(line_org[si_org])
					      && VIM_ISWHITE(line_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(line_org[si_org])
					    || VIM_ISWHITE(line_new[si_new]))))
		{
		    si_org = (số_nguyên)(skipwhite(line_org + si_org) - line_org);
		    si_new = (số_nguyên)(skipwhite(line_new + si_new) - line_new);
		}
		không_thì
		{
		    nếu (!diff_equal_char(line_org + si_org, line_new + si_new,
									   &l))
			break;
		    si_org += l;
		    si_new += l;
		}
	    }
	    nếu (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(line_org, line_org + si_org);
		si_new -= (*mb_head_off)(line_new, line_new + si_new);
	    }
	    nếu (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    nếu (line_org[si_org] != NUL || line_new[si_new] != NUL)
	    {
		ei_org = (số_nguyên)STRLEN(line_org);
		ei_new = (số_nguyên)STRLEN(line_new);
		trong_khi (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    nếu (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(line_org[ei_org])
					      && VIM_ISWHITE(line_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(line_org[ei_org])
					    || VIM_ISWHITE(line_new[ei_new]))))
		    {
			trong_khi (ei_org >= *startp
					     && VIM_ISWHITE(line_org[ei_org]))
			    --ei_org;
			trong_khi (ei_new >= si_new
					     && VIM_ISWHITE(line_new[ei_new]))
			    --ei_new;
		    }
		    không_thì
		    {
			p1 = line_org + ei_org;
			p2 = line_new + ei_new;
			p1 -= (*mb_head_off)(line_org, p1);
			p2 -= (*mb_head_off)(line_new, p2);
			nếu (!diff_equal_char(p1, p2, &l))
			    break;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		nếu (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(line_org);
    trả_về added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
typedef struct
{
    long byte_start;
    long num_bytes;
    số_nguyên lineoff;
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
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, số_nguyên idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    số_nguyên pass = 1;
    do
    {
	số_nguyên has_unmerged_gaps = FALSE;
	số_nguyên has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	trong_khi (dp!= NULL && dp->df_next != NULL)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    nếu (dp->df_lnum[idx1] + dp->df_count[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_count[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    nếu (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_count[idx1]);
	    nếu (gap <= 3)
	    {
		linenr_T max_df_count = 0;
		cho (số_nguyên i = 0; i < DB_COUNT; i++)
		    max_df_count = MAX(max_df_count, dp->df_count[i] + dp->df_next->df_count[i]);

		nếu (max_df_count >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    cho (số_nguyên i = 0; i < DB_COUNT; i++)
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
		không_thì
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	nếu (!has_unmerged_gaps || !has_merged_gaps)
	    break;
    } trong_khi (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    static void
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, số_nguyên idx1,
				  linenr_T start_lnum)
{
    số_nguyên pass = 1;
    do
    {
	diff_T *dp = dp_orig;

	trong_khi (dp != NULL && dp->df_next != NULL)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_count[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    nếu (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
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
	    nếu (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		continue;
	    }

	    // Calculate the gap between blocks
	    số_nguyên gap_start = entry1->byte_start + entry1->num_bytes;
	    số_nguyên gap_end = entry2->byte_start;
	    số_nguyên gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    nếu (gap_size <= 0 || gap_size > diff_word_gap)
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

	    cho (số_nguyên i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		số_nguyên char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		nếu (char_class == 2)
		{
		    only_non_word = false;
		    break;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    nếu (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		cho (số_nguyên i = 0; i < DB_COUNT; i++)
		{
		    nếu (curtab->tp_diffbuf[i] != NULL)
		    {
			// count bytes in the first block
			cho (số_nguyên k = 0; k < dp->df_count[i]; k++)
			{
			    số_nguyên idx = dp->df_lnum[i] + k - 1;
			    nếu (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
			// count bytes in the next block
			cho (số_nguyên k = 0; k < dp->df_next->df_count[i]; k++)
			{
			    số_nguyên idx = dp->df_next->df_lnum[i] + k - 1;
			    nếu (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].num_bytes;
			}
		    }
		}

		nếu (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    cho (số_nguyên i = 0; i < DB_COUNT; i++)
		    {
			nếu (curtab->tp_diffbuf[i] != NULL)
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
    } trong_khi (pass++ < 4);  // use limited number of passes to avoid excessive looping
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
    số_nguyên		file1_idx = -1;

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
    cho (số_nguyên i = 0; i < DB_COUNT; i++)
	ga_init2(&linemap[i], sizeof(linemap_entry_T), 128);

    cho (số_nguyên i = 0; i < DB_COUNT; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	nếu (buf == NULL || buf->b_ml.ml_mfp == NULL)
	    continue; // skip buffer that isn't loaded

	nếu (dp->df_count[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = NULL;
	    continue;
	}

	nếu (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	cho (số_nguyên off = 0; off < dp->df_count[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    số_nguyên in_keyword = FALSE;

	    // iwhiteeol support vars
	    số_nguyên last_white = FALSE;
	    số_nguyên eol_ga_len = -1;
	    số_nguyên eol_linemap_len = -1;
	    số_nguyên eol_numlines = -1;

	    char_u *s;
	    cho (s = curline; *s != NUL;)
	    {
		số_nguyên new_in_keyword = FALSE;
		nếu (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		nếu (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		nếu (VIM_ISWHITE(*s))
		{
		    nếu (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			continue;
		    }
		    không_thì nếu ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			nếu (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		không_thì
		{
		    nếu ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		số_nguyên char_len = 1;
		nếu (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		không_thì
		{
		    char_len = mb_ptr2len(s);

		    nếu (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    nếu (diff_flags & DIFF_ICASE)
		    {
			số_nguyên c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			số_nguyên c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			số_nguyên c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			nếu (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    không_thì
			ga_concat_len(curstr, s, char_len);
		}

		nếu (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		nếu (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.num_bytes = char_len;
		    nếu (ga_grow(&linemap[i], 1) != OK)
			goto done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		không_thì
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
	    nếu (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    nếu ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		nếu (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    nếu (!(diff_flags & DIFF_IWHITEALL))
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
		nếu (ga_grow(&linemap[i], 1) != OK)
		    goto done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	nếu (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (char *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	không_thì
	{
	    dio.dio_orig.din_mmfile.ptr = (char *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	nếu (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    số_nguyên diff_status = diff_file_internal(&dio);
	    nếu (diff_status == FAIL)
		goto done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    nếu (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    không_thì nếu (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    cho (; new_diff != NULL; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	cho (số_nguyên i = 0; i < DB_COUNT; i++)
	{
	    nếu (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		continue;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_count[i];

	    nếu (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    không_thì
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    nếu (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    không_thì nếu (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    không_thì
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].num_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	nếu (ga_grow(&dp->df_changes, 1) != OK)
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
    cho (số_nguyên i = 0; i < DB_COUNT; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    số_nguyên
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    số_nguyên		idx;
    số_nguyên		off;

    idx = diff_buf_idx(wp->w_buffer);
    nếu (idx == DB_COUNT)	// cannot happen
	trả_về FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	nếu (lnum < dp->df_lnum[idx] + dp->df_count[idx])
	    break;
    nếu (dp == NULL || diff_check_sanity(curtab, dp) == FAIL)
	trả_về FALSE;

    nếu (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	trả_về FALSE;
    off = lnum - dp->df_lnum[idx];

    nếu (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	số_nguyên	change_start = MAXCOL;	// first col of changed area
	số_nguyên	change_end = -1;	// last col of changed area
	số_nguyên	ret;

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
	trả_về ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    nếu (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    số_nguyên num_changes = 0;
    số_nguyên change_idx = 0;
    diffline->changes = NULL;
    cho (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	nếu (change->dc_end_lnum_off[idx] < off)
	    continue;
	nếu (change->dc_start_lnum_off[idx] > off)
	    break;
	nếu (diffline->changes == NULL)
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
    số_nguyên added = FALSE;
    nếu (num_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	cho (số_nguyên i = 0; i < DB_COUNT; i++)
	{
	    nếu (idx == i)
		continue;
	    nếu (curtab->tp_diffbuf[i] == NULL)
		continue;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    nếu (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		break;
	    }
	}
    }
    trả_về added;
}

# nếu defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    số_nguyên
diff_infold(win_T *wp, linenr_T lnum)
{
    số_nguyên		i;
    số_nguyên		idx = -1;
    số_nguyên		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    nếu (!wp->w_p_diff)
	trả_về FALSE;

    cho (i = 0; i < DB_COUNT; ++i)
    {
	nếu (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	không_thì nếu (curtab->tp_diffbuf[i] != NULL)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    nếu (idx == -1 || !other)
	trả_về FALSE;

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    nếu (curtab->tp_first_diff == NULL)
	trả_về TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = NULL;		// last block with df_lnum[idx] <= lnum
    số_nguyên		result = TRUE;
    cho (dp = diff_scan_start(curtab, idx, lnum); dp != NULL; dp = dp->df_next)
    {
	nếu (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	nếu (dp->df_lnum[idx] - diff_context > lnum)
	    break;
	// If this change ends before the line we have a match.
	nếu (dp->df_lnum[idx] + dp->df_count[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    break;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    trả_về result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    void
nv_diffgetput(số_nguyên put, long count)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    nếu (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	trả_về;
    }
# endif
    nếu (count == 0)
	ea.arg = (char_u *)"";
    không_thì
    {
	vim_snprintf((char *)buf, 30, "%ld", count);
	ea.arg = buf;
    }
    nếu (put)
	ea.cmdidx = CMD_diffput;
    không_thì
	ea.cmdidx = CMD_diffget;
    ea.addr_count = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    static số_nguyên
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	nếu (dp == diff)
	    trả_về TRUE;
    trả_về FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    void
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    số_nguyên		count;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    số_nguyên		idx_cur;
    số_nguyên		idx_other;
    số_nguyên		idx_from;
    số_nguyên		idx_to;
    số_nguyên		i;
    số_nguyên		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    số_nguyên		start_skip, end_skip;
    số_nguyên		new_count;
    số_nguyên		buf_empty;
    số_nguyên		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    nếu (idx_cur == DB_COUNT)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	trả_về;
    }

    nếu (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	cho (idx_other = 0; idx_other < DB_COUNT; ++idx_other)
	    nếu (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != NULL)
	    {
		nếu (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    break;
		found_not_ma = TRUE;
	    }
	nếu (idx_other == DB_COUNT)
	{
	    nếu (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    không_thì
		emsg(_(e_no_other_buffer_in_diff_mode));
	    trả_về;
	}

	// Check that there isn't a third buffer in the list
	cho (i = idx_other + 1; i < DB_COUNT; ++i)
	    nếu (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != NULL
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		trả_về;
	    }
    }
    không_thì
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	trong_khi (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	cho (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	nếu (eap->arg + i == p)	    // digits only
	    i = atol((char *)eap->arg);
	không_thì
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    nếu (i < 0)
		trả_về;		// error message already given
	}
	buf = buflist_findnr(i);
	nếu (buf == NULL)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    trả_về;
	}
	nếu (buf == curbuf)
	    trả_về;		// nothing to do
	idx_other = diff_buf_idx(buf);
	nếu (idx_other == DB_COUNT)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    trả_về;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    nếu (eap->addr_count == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	số_nguyên linestatus = 0;
	nếu (eap->line1 == curbuf->b_ml.ml_line_count
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	không_thì nếu (eap->line1 > 0)
	    --eap->line1;
    }

    nếu (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    không_thì
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	nếu (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    goto theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    nếu (!curbuf->b_changed)
    {
	change_warning(0);
	nếu (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    goto theend;
	}
    }

    dprev = NULL;
    cho (dp = curtab->tp_first_diff; dp != NULL; )
    {
	nếu (!eap->addr_count)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    trong_khi (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_count[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	nếu (dp->df_lnum[idx_cur] > eap->line2 + off)
	    break;	// past the range that was specified

	dfree = NULL;
	lnum = dp->df_lnum[idx_to];
	count = dp->df_count[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + count + (count == 0 && BUFEMPTY() ? 1 : 0);

	nếu (dp->df_lnum[idx_cur] + dp->df_count[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    nếu (eap->addr_count > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		nếu (start_skip > 0)
		{
		    // range starts below start of current diff block
		    nếu (start_skip > count)
		    {
			lnum += count;
			count = 0;
		    }
		    không_thì
		    {
			count -= start_skip;
			lnum += start_skip;
		    }
		}
		không_thì
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_count[idx_cur] - 1
							 - (eap->line2 + off);
		nếu (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    nếu (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_count[idx_cur] - start_skip - end_skip;
			nếu (count > i)
			    count = i;
		    }
		    không_thì			// :diffget
		    {
			count -= end_skip;
			end_skip = dp->df_count[idx_from] - start_skip - count;
			nếu (end_skip < 0)
			    end_skip = 0;
		    }
		}
		không_thì
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    cho (i = 0; i < count; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_line_count == 1;
		nếu (ml_delete(lnum) == OK)
		    --added;
	    }
	    cho (i = 0; i < dp->df_count[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		nếu (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_line_count)
		    break;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		nếu (p != NULL)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    nếu (buf_empty && curbuf->b_ml.ml_line_count == 2)
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

	    nếu (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		cho (i = 0; i < DB_COUNT; ++i)
		    nếu (curtab->tp_diffbuf[i] != NULL && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			break;
		nếu (i == DB_COUNT)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    nếu (dprev == NULL)
			curtab->tp_first_diff = dp;
		    không_thì
			dprev->df_next = dp;
		}
	    }

	    nếu (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + count - 1, (long)MAXLNUM, (long)added);
		nếu (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    nếu (curwin->w_cursor.lnum >= lnum + count)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_line_count);
		    }
		    không_thì nếu (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_lines(lnum, 0, lnum + count, (long)added);

	    nếu (dfree != NULL)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    nếu (added != 0 && !valid_diff(dp))
		break;

	    nếu (dfree == NULL)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_count[idx_to] = new_count;

	    // When changing the current buffer, keep track of line numbers
	    nếu (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	nếu (dfree == NULL)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    nếu (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	nếu (KeyTyped)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    nếu (diff_need_update)
	ex_diffupdate(NULL);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_line_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    nếu (curtab->tp_first_diff == NULL)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    nếu (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    nếu (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    không_thì
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
diff_fold_update(diff_T *dp, số_nguyên skip_idx)
{
    số_nguyên		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	cho (i = 0; i < DB_COUNT; ++i)
	    nếu (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_count[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    số_nguyên
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	nếu (diff_buf_idx_tp(buf, tp) != DB_COUNT)
	    trả_về TRUE;
    trả_về FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    số_nguyên
diff_move_to(số_nguyên dir, long count)
{
    số_nguyên		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    nếu (idx == DB_COUNT || curtab->tp_first_diff == NULL)
	trả_về FAIL;

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    nếu (curtab->tp_first_diff == NULL)		// no diffs today
	trả_về FAIL;

    trong_khi (--count >= 0)
    {
	// Check if already before first diff.
	nếu (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    break;

	cho (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    nếu (dp == NULL)
		break;
	    nếu ((dir == FORWARD && lnum < dp->df_lnum[idx])
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
    nếu (lnum > curbuf->b_ml.ml_line_count)
	lnum = curbuf->b_ml.ml_line_count;

    // When the cursor didn't move at all we fail.
    nếu (lnum == curwin->w_cursor.lnum)
	trả_về FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    trả_về OK;
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
    số_nguyên		idx1;
    số_nguyên		idx2;
    diff_T	*dp;
    số_nguyên		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    nếu (idx1 == DB_COUNT || idx2 == DB_COUNT || curtab->tp_first_diff == NULL)
	trả_về lnum1;

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    nếu (curtab->tp_first_diff == NULL)		// no diffs today
	trả_về lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	nếu (dp->df_lnum[idx1] > lnum1)
	    trả_về lnum1 - baseline;
	nếu ((dp->df_lnum[idx1] + dp->df_count[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    nếu (baseline > dp->df_count[idx2])
		baseline = dp->df_count[idx2];

	    trả_về dp->df_lnum[idx2] + baseline;
	}
	nếu (    (dp->df_lnum[idx1] == lnum1)
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
	    trả_về curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_count[idx1])
				   - (dp->df_lnum[idx2] + dp->df_count[idx2]);
    }

    // If we get here then the cursor is after the last diff
    trả_về lnum1 - baseline;
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
    nếu (lnum > curbuf->b_ml.ml_line_count)
	trả_về curbuf->b_ml.ml_line_count;
    trả_về lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    số_nguyên		idx;
    số_nguyên		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    nếu (idx == DB_COUNT)		// safety check
	trả_về (linenr_T)0;

    nếu (curtab->tp_diff_invalid)
	ex_diffupdate(NULL);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	nếu (lnum <= dp->df_lnum[idx] + dp->df_count[idx])
	    break;

    // When after the last change, compute relative to the last line number.
    nếu (dp == NULL)
	trả_về wp->w_buffer->b_ml.ml_line_count
					- (curbuf->b_ml.ml_line_count - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    nếu (i == DB_COUNT)			// safety check
	trả_về (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    nếu (n > dp->df_lnum[i] + dp->df_count[i])
	n = dp->df_lnum[i] + dp->df_count[i];
    trả_về n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    static số_nguyên
parse_diff_ed(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    số_nguyên	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = line;
    f1 = getdigits(&p);
    nếu (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    không_thì
	l1 = f1;
    nếu (*p != 'a' && *p != 'c' && *p != 'd')
	trả_về FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    nếu (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    không_thì
	l2 = f2;
    nếu (l1 < f1 || l2 < f2)
	trả_về FAIL;

    nếu (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->count_orig = 0;
    }
    không_thì
    {
	hunk->lnum_orig = f1;
	hunk->count_orig = l1 - f1 + 1;
    }
    nếu (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->count_new = 0;
    }
    không_thì
    {
	hunk->lnum_new = f2;
	hunk->count_new = l2 - f2 + 1;
    }
    trả_về OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    static số_nguyên
parse_diff_unified(
	char_u	    *line,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = line;
    nếu (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	nếu (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	không_thì
	    oldcount = 1;
	nếu (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    nếu (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    không_thì
		newcount = 1;
	}
	không_thì
	    trả_về FAIL;	// invalid diff format

	nếu (oldcount == 0)
	    oldline += 1;
	nếu (newcount == 0)
	    newline += 1;
	nếu (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->count_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->count_new = newcount;

	trả_về OK;
    }

    trả_về FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    static số_nguyên
xdiff_out_indices(
	long start_a,
	long count_a,
	long start_b,
	long count_b,
	void *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    nếu (p == NULL)
	trả_về -1;

    nếu (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	trả_về -1;
    }

    p->lnum_orig  = start_a + 1;
    p->count_orig = count_a;
    p->lnum_new   = start_b + 1;
    p->count_new  = count_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    trả_về 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    static số_nguyên
xdiff_out_unified(
	void *priv,
	mmbuffer_t *mb,
	số_nguyên nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    số_nguyên		i;

    cho (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    trả_về 0;
}

#endif	// FEAT_DIFF

#nếu defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    void
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    nếu (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	trả_về;

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
    static số_nguyên		fnum = 0;
    static số_nguyên		prev_diff_flags = 0;
    static số_nguyên		change_start = 0;
    static số_nguyên		change_end = 0;
    static hlf_T	hlID = (hlf_T)0;
    số_nguyên			cache_results = TRUE;
    số_nguyên			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    nếu (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	trả_về;

    nếu (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    nếu (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    nếu (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	số_nguyên linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	nếu (linestatus < 0)
	{
	    nếu (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		nếu (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		không_thì
		{
		    hlID = HLF_CHD;	// changed line
		    nếu (diffline.num_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    không_thì
		hlID = HLF_ADD;	// added line
	}
	không_thì
	    hlID = (hlf_T)0;

	nếu (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    nếu (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_number(&argvars[1]) - 1; // ignore type error in {col}
	nếu (cache_results)
	{
	    nếu (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    không_thì
		hlID = HLF_CHD;			// changed line
	}
	không_thì
	{
	    hlID = HLF_CHD;
	    cho (số_nguyên i = 0; i < diffline.num_changes; i++)
	    {
		số_nguyên added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		nếu (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    break;
		}
		nếu (col < change_start)
		    // the remaining changes are past this column and not relevant
		    break;
	    }
	}
    }
    rettv->vval.v_number = hlID == (hlf_T)0 ? 0 : (số_nguyên)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    static số_nguyên
parse_diff_optarg(
    typval_T	    *opts,
    số_nguyên		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    số_nguyên		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    nếu (algo != NULL)
    {
	nếu (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	không_thì nếu (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	không_thì nếu (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	không_thì nếu (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    nếu (output_fmt != NULL)
    {
	nếu (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	không_thì nếu (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	không_thì
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    trả_về FAIL;
	}
    }

    *diff_ctxlen = dict_get_number_def(d, "context", 0);
    nếu (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    nếu (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    nếu (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    nếu (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    nếu (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    nếu (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    nếu (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    trả_về OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    static void
list_to_diffin(list_T *l, diffin_T *din, số_nguyên icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ITEMS(l, li)
    {
	str = tv_get_string(&li->li_tv);
	nếu (icase)
	{
	    str = strlow_save(str);
	    nếu (str == NULL)
		continue;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	nếu (icase)
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
    nếu (hunk_dict == NULL)
	trả_về NULL;

    dict_add_number(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_number(hunk_dict, "from_count", hunk->count_orig);
    dict_add_number(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_number(hunk_dict, "to_count", hunk->count_new);

    trả_về hunk_dict;
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

    nếu (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	trả_về;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, sizeof(char *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    số_nguyên		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    nếu (argvars[2].v_type != VAR_UNKNOWN)
	nếu (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    trả_về;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    số_nguyên	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    nếu (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    số_nguyên diff_status = diff_file(&dio);

    // restore 'diffexpr'
    nếu (restore_diffexpr)
	*p_dex = cc;

    nếu (diff_status == FAIL)
	goto done;

    số_nguyên		hunk_idx = 0;
    dict_T	*hunk_dict;

    nếu (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	nếu (rettv_list_alloc(rettv) != OK)
	    goto done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = NULL;
	trong_khi (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    nếu (hunk_dict == NULL)
		goto done;

	    nếu (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    không_thì
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    nếu (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    không_thì
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
