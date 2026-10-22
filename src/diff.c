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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "xdiff/xdiff.h"

#ଯଦି defined(FEAT_DIFF)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_busy = FALSE;	    // using diff structs, don't change them
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_need_update = FALSE; // ex_diffupdate needs to be called

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

ସ୍ଥିର long diff_algorithm = XDF_INDENT_HEURISTIC;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# ଯଦି defined(MSWIN)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    linenr_T lnum_orig;
    long     ଗଣନା_orig;
    linenr_T lnum_new;
    long     ଗଣନା_new;
} diffhunk_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dio_ctxlen;   // unified diff context length
} diffio_T;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_buf_idx(buf_T *buf);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
ସ୍ଥିର ଶୂନ୍ୟ diff_mark_adjust_tp(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
ସ୍ଥିର ଶୂନ୍ୟ diff_check_unchanged(tabpage_T *tp, diff_T *dp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_check_sanity(tabpage_T *tp, diff_T *dp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_external_diff(diffio_T *diffio);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_file(diffio_T *diffio);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_equal_entry(diff_T *dp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx2);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
ସ୍ଥିର ଶୂନ୍ୟ diff_fold_update(diff_T *dp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_idx);
# endif
ସ୍ଥିର ଶୂନ୍ୟ diff_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_orig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_new, diffio_T *dio);
ସ୍ଥିର ଶୂନ୍ୟ diff_copy_entry(diff_T *dprev, diff_T *dp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_orig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx_new);
ସ୍ଥିର diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_diff_ed(char_u *ଧାଡ଼ି, diffhunk_T *hunk);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_diff_unified(char_u *ଧାଡ଼ି, diffhunk_T *hunk);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdiff_out_indices(long start_a, long ଗଣନା_a, long start_b, long ଗଣନା_b, ଶୂନ୍ୟ *priv);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdiff_out_unified(ଶୂନ୍ୟ *priv, mmbuffer_t *mb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_diffanchors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_only, buf_T *buf, linenr_T *anchors, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    ପାଇଁ ((dp) = (tp)->tp_first_diff; (dp) != ଖାଲି; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
ସ୍ଥିର diff_T	    *diff_finger_dp = ଖାଲି;
ସ୍ଥିର tabpage_T    *diff_finger_tp = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    diff_finger_idx = -1;
ସ୍ଥିର linenr_T	    diff_finger_lnum = 0;

    ସ୍ଥିର ଶୂନ୍ୟ
clear_diffblock(diff_T *dp)
{
    ଯଦି (dp == diff_finger_dp)
	diff_finger_dp = ଖାଲି;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    ସ୍ଥିର diff_T *
diff_scan_start(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, linenr_T lnum)
{
    ଯଦି (diff_finger_dp != ଖାଲି
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	ଫେରନ୍ତୁ diff_finger_dp;
    ଫେରନ୍ତୁ tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_scan_remember(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ଖାଲି ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    ଶୂନ୍ୟ
diff_buf_delete(buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ଯଦି (i != DB_ଗଣନା)
	{
	    tp->tp_diffbuf[i] = ଖାଲି;
	    tp->tp_diff_invalid = TRUE;
	    ଯଦି (tp == curtab)
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
    ଶୂନ୍ୟ
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    ଯଦି (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		ଭାଙ୍ଗ;
	ଯଦି (wp == ଖାଲି)
	{
	    i = diff_buf_idx(win->w_buffer);
	    ଯଦି (i != DB_ଗଣନା)
	    {
		curtab->tp_diffbuf[i] = ଖାଲି;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
diff_buf_add(buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (diff_buf_idx(buf) != DB_ଗଣନା)
	ଫେରନ୍ତୁ;		// It's already there.

    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	ଯଦି (curtab->tp_diffbuf[i] == ଖାଲି)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    ଫେରନ୍ତୁ;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_ଗଣନା);
}

/*
 * Remove all buffers to make diffs for.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_buf_clear(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
	{
	    curtab->tp_diffbuf[i] = ଖାଲି;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_buf_idx(buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ପାଇଁ (idx = 0; idx < DB_ଗଣନା; ++idx)
	ଯଦି (curtab->tp_diffbuf[idx] == buf)
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ପାଇଁ (idx = 0; idx < DB_ଗଣନା; ++idx)
	ଯଦି (tp->tp_diffbuf[idx] == buf)
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    ଶୂନ୍ୟ
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ଯଦି (i != DB_ଗଣନା)
	{
	    tp->tp_diff_invalid = TRUE;
	    ଯଦି (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    ଶୂନ୍ୟ
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	ଯଦି (idx != DB_ଗଣନା)
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
    ସ୍ଥିର ଶୂନ୍ୟ
diff_mark_adjust_tp(
    tabpage_T	*tp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inserted, deleted;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_unchanged;

    diff_finger_dp = ଖାଲି;		// may shift df_lnum, invalidate the finger

    ଯଦି (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    ଯଦି (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    ଅନ୍ୟଥା ଯଦି (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    ଅନ୍ୟଥା
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ଖାଲି;
    dp = tp->tp_first_diff;
    ପାଇଁ (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	ଯଦି ((dp == ଖାଲି || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ଖାଲି
		    || dprev->df_lnum[idx] + dprev->df_ଗଣନା[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    ଯଦି (dnext == ଖାଲି)
		ଫେରନ୍ତୁ;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_ଗଣନା[idx] = inserted;
	    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
		ଯଦି (tp->tp_diffbuf[i] != ଖାଲି && i != idx)
		{
		    ଯଦି (dprev == ଖାଲି)
			dnext->df_lnum[i] = line1;
		    ଅନ୍ୟଥା
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_ଗଣନା[i])
			    - (dprev->df_lnum[idx] + dprev->df_ଗଣନା[idx]);
		    dnext->df_ଗଣନା[i] = deleted;
		}
	}

	// if at end of the list, quit
	ଯଦି (dp == ଖାଲି)
	    ଭାଙ୍ଗ;

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
	last = dp->df_lnum[idx] + dp->df_ଗଣନା[idx] - 1;

	// 1. change completely above line1: nothing to do
	ଯଦି (last >= line1 - 1)
	{
	    ଯଦି (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		ଯଦି (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		ଜାରି;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    ଯଦି (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		ଯଦି (amount_after == 0)
		    ଭାଙ୍ଗ;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    ଅନ୍ୟଥା
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		ଯଦି (deleted > 0)
		{
		    off = 0;
		    ଯଦି (dp->df_lnum[idx] >= line1)
		    {
			ଯଦି (last <= line2)
			{
			    // 4. delete all lines of diff
			    ଯଦି (dp->df_next != ଖାଲି
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_ଗଣନା[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    ଅନ୍ୟଥା
				n = deleted - dp->df_ଗଣନା[idx];
			    dp->df_ଗଣନା[idx] = 0;
			}
			ଅନ୍ୟଥା
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_ଗଣନା[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_ଗଣନା[idx] -= last - lnum_deleted + 1;
			    ଯଦି (dp->df_next != ଖାଲି
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    ଅନ୍ୟଥା
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			ଅନ୍ୟଥା
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_ଗଣନା[idx] -= deleted;
			}
		    }

		    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
			ଯଦି (tp->tp_diffbuf[i] != ଖାଲି && i != idx)
			{
			    ଯଦି (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    ଅନ୍ୟଥା
				dp->df_lnum[i] = 1;
			    dp->df_ଗଣନା[i] += n;
			}
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_ଗଣନା[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    ଅନ୍ୟଥା
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		ଯଦି (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	ଯଦି (dprev != ଖାଲି && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_ଗଣନା[idx]
							== dp->df_lnum[idx])
	{
	    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
		ଯଦି (tp->tp_diffbuf[i] != ଖାଲି)
		    dprev->df_ଗଣନା[i] += dp->df_ଗଣନା[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	ଅନ୍ୟଥା
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = ଖାଲି;
    dp = tp->tp_first_diff;
    ଯେତେବେଳେ (dp != ଖାଲି)
    {
	// All counts are zero, remove this entry.
	ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	    ଯଦି (tp->tp_diffbuf[i] != ଖାଲି && dp->df_ଗଣନା[i] != 0)
		ଭାଙ୍ଗ;
	ଯଦି (i == DB_ଗଣନା)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    ଯଦି (dprev == ଖାଲି)
		tp->tp_first_diff = dnext;
	    ଅନ୍ୟଥା
		dprev->df_next = dnext;
	}
	ଅନ୍ୟଥା
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    ଯଦି (tp == curtab)
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
    ସ୍ଥିର diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    ଯଦି (dnew == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    diff_finger_dp = ଖାଲି;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    ଯଦି (dprev == ଖାଲି)
	tp->tp_first_diff = dnew;
    ଅନ୍ୟଥା
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, ଆକାର(diffline_change_T), 20);
    ଫେରନ୍ତୁ dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i_org;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i_new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off_org, off_new;
    char_u	*ଧାଡ଼ି_org;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    ପାଇଁ (i_org = 0; i_org < DB_ଗଣନା; ++i_org)
	ଯଦି (tp->tp_diffbuf[i_org] != ଖାଲି)
	    ଭାଙ୍ଗ;
    ଯଦି (i_org == DB_ଗଣନା)	// safety check
	ଫେରନ୍ତୁ;

    ଯଦି (diff_check_sanity(tp, dp) == FAIL)
	ଫେରନ୍ତୁ;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    ପାଇଁ (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ଯେତେବେଳେ (dp->df_ଗଣନା[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    ଯଦି (dir == BACKWARD)
		off_org = dp->df_ଗଣନା[i_org] - 1;
	    ଧାଡ଼ି_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    ଯଦି (ଧାଡ଼ି_org == ଖାଲି)
		ଫେରନ୍ତୁ;
	    ପାଇଁ (i_new = i_org + 1; i_new < DB_ଗଣନା; ++i_new)
	    {
		ଯଦି (tp->tp_diffbuf[i_new] == ଖାଲି)
		    ଜାରି;
		ଯଦି (dir == BACKWARD)
		    off_new = dp->df_ଗଣନା[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		ଯଦି (off_new < 0 || off_new >= dp->df_ଗଣନା[i_new])
		    ଭାଙ୍ଗ;
		ଯଦି (diff_cmp(ଧାଡ଼ି_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ଭାଙ୍ଗ;
	    }
	    vim_free(ଧାଡ଼ି_org);

	    // Stop when a line isn't equal in all diff buffers.
	    ଯଦି (i_new != DB_ଗଣନା)
		ଭାଙ୍ଗ;

	    // Line matched in all buffers, remove it from the diff.
	    ପାଇଁ (i_new = i_org; i_new < DB_ଗଣନା; ++i_new)
		ଯଦି (tp->tp_diffbuf[i_new] != ଖାଲି)
		{
		    ଯଦି (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_ଗଣନା[i_new];
		}
	}
	ଯଦି (dir == BACKWARD)
	    ଭାଙ୍ଗ;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	ଯଦି (tp->tp_diffbuf[i] != ଖାଲି)
	    ଯଦି (dp->df_lnum[i] + dp->df_ଗଣନା[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    ଶୂନ୍ୟ
diff_redraw(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_max_fill_other = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_max_fill_curwin = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	ଯଦି (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    ଜାରି;

	redraw_win_later(wp, UPD_SOME_VALID);
	ଯଦି (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	ଯଦି (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	ଯଦି ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    ଯଦି (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    ଅନ୍ୟଥା ଯଦି (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		ଯଦି (wp == curwin)
		    used_max_fill_curwin = TRUE;
		ଅନ୍ୟଥା ଯଦି (wp_other != ଖାଲି)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    ଯଦି (wp_other != ଖାଲି && curwin->w_p_scb)
    {
	ଯଦି (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	ଅନ୍ୟଥା ଯଦି (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_diffin(diffin_T *din)
{
    ଯଦି (din->din_fname == ଖାଲି)
	VIM_CLEAR(din->din_mmfile.ptr);
    ଅନ୍ୟଥା
	mch_remove(din->din_fname);
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_diffout(diffout_T *dout)
{
    ଯଦି (dout->dout_fname == ଖାଲି)
	ga_clear_strings(&dout->dout_ga);
    ଅନ୍ୟଥା
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    ଯଦି (end < 0)
	end = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = ଖାଲି;
	din->din_mmfile.size = 0;
	ଫେରନ୍ତୁ OK;
    }

    // xdiff requires one big block of memory with all the text.
    ପାଇଁ (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    ଯଦି (ptr == ଖାଲି)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	ଫେରନ୍ତୁ FAIL;
    }
    din->din_mmfile.ptr = (ଅକ୍ଷର *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    ପାଇଁ (lnum = start; lnum <= end; ++lnum)
    {
	ପାଇଁ (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    ଯଦି (diff_flags & DIFF_ICASE)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		ଯଦି (*s == NL)
		    c = NUL;
		ଅନ୍ୟଥା
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		ଯଦି (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		ଅନ୍ୟଥା
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    ଯଦି (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    ଅନ୍ୟଥା
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ml_flags;
    char_u	*save_ff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags;

    ଯଦି (din->din_fname == ଖାଲି)
	ଫେରନ୍ତୁ diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    ଯଦି (frames_locked())
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (end < 0)
	end = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    ଯଦି (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, ଖାଲି,
			start, end,
			ଖାଲି, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    ଫେରନ୍ତୁ r;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lnum_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    ଯଦି (lnum1 < lnum2)
	ଫେରନ୍ତୁ -1;
    ଯଦି (lnum1 > lnum2)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_try_update(
	diffio_T    *dio,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_new;

    ଯଦି (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, ଆକାର(ଅକ୍ଷର *), 1000);
    }
    ଅନ୍ୟଥା
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	ଯଦି (dio->dio_orig.din_fname == ଖାଲି
		|| dio->dio_new.din_fname == ଖାଲି
		|| dio->dio_diff.dout_fname == ଖାଲି)
	    ଯାଅ theend;
    }

    // Check external diff is actually working.
    ଯଦି (!dio->dio_internal && check_external_diff(dio) == FAIL)
	ଯାଅ theend;

    // :diffupdate!
    ଯଦି (eap != ଖାଲି && eap->forceit)
	ପାଇଁ (idx_new = idx_orig; idx_new < DB_ଗଣନା; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ଯଦି (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_anchors = INT_MAX;
    linenr_T anchors[DB_ଗଣନା][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    ଯଦି (diff_flags & DIFF_ANCHOR)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < DB_ଗଣନା; idx++)
	{
	    ଯଦି (curtab->tp_diffbuf[idx] == ଖାଲି)
		ଜାରି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_ଅଙ୍କ_anchors = 0;
	    ଯଦି (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_ଅଙ୍କ_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		ଅଙ୍କ_anchors = 0;
		CLEAR_FIELD(anchors);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (buf_ଅଙ୍କ_anchors < ଅଙ୍କ_anchors)
		ଅଙ୍କ_anchors = buf_ଅଙ୍କ_anchors;

	    ଯଦି (buf_ଅଙ୍କ_anchors > 0)
		qsort((ଶୂନ୍ୟ *)anchors[idx],
			(size_t)buf_ଅଙ୍କ_anchors,
			ଆକାର(linenr_T),
			lnum_compare);
	}
    }
    ଯଦି (ଅଙ୍କ_anchors == INT_MAX)
	ଅଙ୍କ_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା anchor_i = 0; anchor_i <= ଅଙ୍କ_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ଖାଲି;
	ଯଦି (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = ଖାଲି;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == ଅଙ୍କ_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	ଯଦି (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    ଯଦି (orig_diff != ଖାଲି)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    ଯାଅ theend;
	}

	// Make a difference between the first buffer and every other.
	ପାଇଁ (idx_new = idx_orig + 1; idx_new < DB_ଗଣନା; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
		ଜାରି; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == ଅଙ୍କ_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    ଯଦି (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		ଜାରି;
	    ଯଦି (diff_file(dio) == FAIL)
		ଜାରି;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	ଯଦି (anchor_i != 0)
	{
	    diff_finger_dp = ଖାଲି;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    ପାଇଁ (diff_T *dp = curtab->tp_first_diff; dp != ଖାଲି; dp = dp->df_next)
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < DB_ଗଣନା; idx++)
		{
		    ଯଦି (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    ଯଦି (orig_diff != ଖାଲି)
	    {
		diff_T *last_diff = orig_diff;
		ଯେତେବେଳେ (last_diff->df_next != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_internal(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_internal_failed(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

    // Only need to do something when there is another buffer.
    ପାଇଁ (idx = 0; idx < DB_ଗଣନା; ++idx)
	ଯଦି (curtab->tp_diffbuf[idx] != ଖାଲି
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    ଶୂନ୍ୟ
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_orig;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_new;
    diffio_T	diffio;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_diffs = curtab->tp_first_diff != ଖାଲି;

    ଯଦି (diff_busy)
    {
	diff_need_update = TRUE;
	ଫେରନ୍ତୁ;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    ପାଇଁ (idx_orig = 0; idx_orig < DB_ଗଣନା; ++idx_orig)
	ଯଦି (curtab->tp_diffbuf[idx_orig] != ଖାଲି)
	    ଭାଙ୍ଗ;
    ଯଦି (idx_orig == DB_ଗଣନା)
	ଯାଅ theend;

    // Only need to do something when there is another buffer.
    ପାଇଁ (idx_new = idx_orig + 1; idx_new < DB_ଗଣନା; ++idx_new)
	ଯଦି (curtab->tp_diffbuf[idx_new] != ଖାଲି)
	    ଭାଙ୍ଗ;
    ଯଦି (idx_new == DB_ଗଣନା)
	ଯାଅ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    ଯଦି (diffio.dio_internal && diff_internal_failed())
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
    ଯଦି (had_diffs || curtab->tp_first_diff != ଖାଲି)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ଖାଲି, ଖାଲି, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ok;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    ପାଇଁ (;;)
    {
	ok = FALSE;
	fd = mch_fopen((ଅକ୍ଷର *)diffio->dio_orig.din_fname, "w");
	ଯଦି (fd == ଖାଲି)
	    io_error = TRUE;
	ଅନ୍ୟଥା
	{
	    ଯଦି (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((ଅକ୍ଷର *)diffio->dio_new.din_fname, "w");
	    ଯଦି (fd == ଖାଲି)
		io_error = TRUE;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ଖାଲି;
		ଯଦି (diff_file(diffio) == OK)
		    fd = mch_fopen((ଅକ୍ଷର *)diffio->dio_diff.dout_fname, "r");
		ଯଦି (fd == ଖାଲି)
		    io_error = TRUE;
		ଅନ୍ୟଥା
		{
		    char_u	linebuf[LBUFLEN];

		    ପାଇଁ (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			ଯଦି (vim_fgets(linebuf, LBUFLEN, fd))
			    ଭାଙ୍ଗ;
			ଯଦି (STRNCMP(linebuf, "1c1", 3) == 0
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
	ଯଦି (*p_dex != NUL)
	    ଭାଙ୍ଗ;
# endif

# ଯଦି defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	ଯଦି (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    ଜାରି;
	}
	ଯଦି (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	ଯଦି (diff_a_works != MAYBE)
	    ଭାଙ୍ଗ;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	ଯଦି (ok)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (!ok)
    {
	ଯଦି (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# ଯଦି defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Invoke the xdiff function.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    ଯଦି (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    ଯଦି (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    ଯଦି (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    ଯଦି (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_ଧାଡ଼ିଗୁଡ଼ିକ;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    ଯଦି (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    ଅନ୍ୟଥା
	emit_cb.out_ଧାଡ଼ି = xdiff_out_unified;
    ଯଦି (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    ଯଦି (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
# endif
    // Use xdiff for generating the diff.
    ଯଦି (dio->dio_internal)
	ଫେରନ୍ତୁ diff_file_internal(dio);

    ଯଦି (check_restricted())
	ଫେରନ୍ତୁ FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    ଯଦି (cmd == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    ଯଦି (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((ଅକ୍ଷର *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# ଯଦି defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# ଅନ୍ୟଥା
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (ଶୂନ୍ୟ)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    ଫେରନ୍ତୁ OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    ଶୂନ୍ୟ
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ଖାଲି;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ଖାଲି;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ଖାଲି;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ଖାଲି;

# ifdef FEAT_BROWSE
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ଖାଲି, ଖାଲି,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ଖାଲି);
	ଯଦି (browseFile == ଖାଲି)
	    ଫେରନ୍ତୁ;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    ଯଦି (tmp_orig == ଖାଲି || tmp_new == ଖାଲି)
	ଯାଅ theend;

    // Write the current buffer to "tmp_orig".
    ଯଦି (buf_write(curbuf, tmp_orig, ଖାଲି,
		(linenr_T)1, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
				     ଖାଲି, FALSE, FALSE, FALSE, TRUE) == FAIL)
	ଯାଅ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ଖାଲି ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    ଯଦି (esc_name == ଖାଲି)
	ଯାଅ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    ଯଦି (buf == ଖାଲି)
	ଯାଅ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    ଯଦି (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((ଅକ୍ଷର *)dirbuf) != 0)
	dirbuf[0] = NUL;
    ଅନ୍ୟଥା
    {
#  ifdef TEMPDIRNAMES
	ଯଦି (vim_tempdir != ଖାଲି)
	    vim_ignored = mch_chdir((ଅକ୍ଷର *)vim_tempdir);
	ଅନ୍ୟଥା
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    ଯଦି (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ଖାଲି ? fullname :
#  endif
		eap->arg, tmp_new);
    ଅନ୍ୟଥା
# endif
    {
	ଯଦି (check_restricted())
	    ଯାଅ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((ଅକ୍ଷର *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(ଶୂନ୍ୟ)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    ଯଦି (dirbuf[0] != NUL)
    {
	ଯଦି (mch_chdir((ଅକ୍ଷର *)dirbuf) != 0)
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
    ଯଦି (mch_stat((ଅକ୍ଷର *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    ଅନ୍ୟଥା
    {
	ଯଦି (curbuf->b_fname != ଖାଲି)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    ଯଦି (newname != ଖାଲି)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	ଯଦି (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    ଯଦି (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		ଯଦି (newname != ଖାଲି)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    ଯଦି (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    ଯଦି (tmp_orig != ଖାଲି)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    ଯଦି (tmp_new != ଖାଲି)
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
    ଶୂନ୍ୟ
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

    ଯଦି (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	ଫେରନ୍ତୁ;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    ଯଦି (curwin == old_curwin)		// split didn't work
	ଫେରନ୍ତୁ;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    ଯଦି (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	ଯଦି (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_ଧାଡ଼ି(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    ଶୂନ୍ୟ
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_diff_option(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"diff", (long)ମୂଲ୍ୟ, ଖାଲି, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    ଶୂନ୍ୟ
diff_win_options(
    win_T	*wp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    ଯଦି (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    ଯଦି (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    ଯଦି (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	ଯଦି (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    ଯଦି (!wp->w_p_diff)
    {
	ଯଦି (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    ଯଦି (!wp->w_p_diff)
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
    ଯଦି (vim_strchr(p_sbo, 'h') == ଖାଲି)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    ଯଦି (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    ଶୂନ୍ୟ
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    ଯଦି (wp->w_p_diff_saved)
	    {

		ଯଦି (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		ଯଦି (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		ଯଦି (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    ଯଦି (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		ଯଦି (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		ଯଦି (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		ଯଦି (wp->w_p_fen)
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
    ଯଦି (eap->forceit)
	diff_buf_clear();

    ଯଦି (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    ଯଦି (!diffwin && vim_strchr(p_sbo, 'h') != ଖାଲି)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_read(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_orig,	// idx of original file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ଖାଲି;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*ଧାଡ଼ି;
    long	off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ଖାଲି;	    // init to avoid gcc warning

    ଗଣନା {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    ଯଦି (dout->dout_fname == ଖାଲି)
    {
	diffstyle = DIFF_UNIFIED;
    }
    ଅନ୍ୟଥା
    {
	fd = mch_fopen((ଅକ୍ଷର *)dout->dout_fname, "r");
	ଯଦି (fd == ଖାଲି)
	{
	    emsg(_(e_cannot_read_diff_output));
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	ଯଦି (hunk == ଖାଲି)
	{
	    ଯଦି (fd != ଖାଲି)
		fclose(fd);
	    ଫେରନ୍ତୁ;
	}
    }

    ପାଇଁ (;;)
    {
	ଯଦି (dio->dio_internal)
	{
	    ଯଦି (ଧାଡ଼ି_hunk_idx >= dout->dout_ga.ga_len)
		ଭାଙ୍ଗ;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[ଧାଡ଼ି_hunk_idx++];
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (fd == ଖାଲି)
	    {
		ଯଦି (ଧାଡ଼ି_hunk_idx >= dout->dout_ga.ga_len)
		    ଭାଙ୍ଗ;	    // did last line
		ଧାଡ଼ି = ((char_u **)dout->dout_ga.ga_data)[ଧାଡ଼ି_hunk_idx++];
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (vim_fgets(linebuf, LBUFLEN, fd))
		    ଭାଙ୍ଗ;		// end of file
		ଧାଡ଼ି = linebuf;
	    }

	    ଯଦି (diffstyle == DIFF_NONE)
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
		ଯଦି (SAFE_isdigit(*ଧାଡ଼ି))
		    diffstyle = DIFF_ED;
		ଅନ୍ୟଥା ଯଦି ((STRNCMP(ଧାଡ଼ି, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ଅନ୍ୟଥା ଯଦି ((STRNCMP(ଧାଡ଼ି, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ଧାଡ଼ି, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ଧାଡ଼ି, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		ଅନ୍ୟଥା
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    ଜାରି;
	    }

	    ଯଦି (diffstyle == DIFF_ED)
	    {
		ଯଦି (!SAFE_isdigit(*ଧାଡ଼ି))
		    ଜାରି;	// not the start of a diff block
		ଯଦି (parse_diff_ed(ଧାଡ଼ି, hunk) == FAIL)
		    ଜାରି;
	    }
	    ଅନ୍ୟଥା ଯଦି (diffstyle == DIFF_UNIFIED)
	    {
		ଯଦି (STRNCMP(ଧାଡ଼ି, "@@ ", 3)  != 0)
		    ଜାରି;	// not the start of a diff block
		ଯଦି (parse_diff_unified(ଧାଡ଼ି, hunk) == FAIL)
		    ଜାରି;
	    }
	    ଅନ୍ୟଥା
	    {
		emsg(_(e_invalid_diff_format));
		ଭାଙ୍ଗ;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ଯେତେବେଳେ (dp != ଖାଲି
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_ଗଣନା[idx_orig])
	{
	    ଯଦି (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	ଯଦି (dp != ଖାଲି
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_ଗଣନା[idx_orig]
		&& hunk->lnum_orig + hunk->ଗଣନା_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    ପାଇଁ (dpl = dp; dpl->df_next != ଖାଲି; dpl = dpl->df_next)
		ଯଦି (hunk->lnum_orig + hunk->ଗଣନା_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    ଭାଙ୍ଗ;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    ଯଦି (off > 0)
	    {
		ପାଇଁ (i = idx_orig; i < idx_new; ++i)
		    ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
		    {
			dp->df_lnum[i] -= off;
			dp->df_ଗଣନା[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_ଗଣନା[idx_new] = hunk->ଗଣନା_new;
	    }
	    ଅନ୍ୟଥା ଯଦି (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_ଗଣନା[idx_new] = hunk->ଗଣନା_new - off;
	    }
	    ଅନ୍ୟଥା
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା orig_size_in_dp = MIN(hunk->ଗଣନା_orig,
			dp->df_lnum[idx_orig] +
			dp->df_ଗଣନା[idx_orig] - hunk->lnum_orig);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size_diff = hunk->ଗଣନା_new - orig_size_in_dp;
		dp->df_ଗଣନା[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->ଗଣନା_new
		    - (dp->df_lnum[idx_new] + dp->df_ଗଣନା[idx_new]);
		ଯଦି (off > 0)
		    dp->df_ଗଣନା[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->ଗଣନା_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_ଗଣନା[idx_orig]);
	    ଯଦି (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		ଯଦି (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_ଗଣନା[idx_new] += -off;
		}
		off = 0;
	    }
	    ପାଇଁ (i = idx_orig; i < idx_new; ++i)
		ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
		    dp->df_ଗଣନା[i] = dpl->df_lnum[i] + dpl->df_ଗଣନା[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    ଯେତେବେଳେ (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    ଯଦି (dp == ଖାଲି)
		ଯାଅ done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_ଗଣନା[idx_orig] = hunk->ଗଣନା_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_ଗଣନା[idx_new] = hunk->ଗଣନା_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    ପାଇଁ (i = idx_orig + 1; i < idx_new; ++i)
		ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ଯେତେବେଳେ (dp != ଖାଲି)
    {
	ଯଦି (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    ଯଦି (!dio->dio_internal)
	vim_free(hunk);

    ଯଦି (fd != ଖାଲି)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_orig,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_new)
{
    long	off;

    ଯଦି (dprev == ଖାଲି)
	off = 0;
    ଅନ୍ୟଥା
	off = (dprev->df_lnum[idx_orig] + dprev->df_ଗଣନା[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_ଗଣନା[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_ଗଣନା[idx_new] = dp->df_ଗଣନା[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    ଶୂନ୍ୟ
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    ପାଇଁ (p = tp->tp_first_diff; p != ଖାଲି; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = ଖାଲି;
}

/*
 *  return true if the options are set to use diff linematch
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_linematch(diff_T *dp)
{
    ଯଦି (!(diff_flags & DIFF_LINEMATCH))
	ଫେରନ୍ତୁ 0;

    // are there more than three diff buffers?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tsize = 0;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
    {
	ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    ଯଦି (dp->df_ଗଣନା[i] < 0)
		ଫେରନ୍ତୁ FALSE;
	    tsize += dp->df_ଗଣନା[i];
	}
    }

    // avoid allocating a huge array because it will lag
    ଫେରନ୍ତୁ tsize <= linematch_ଧାଡ଼ିଗୁଡ଼ିକ;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_max_diff_length(ଅଚଳ diff_T *dp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlength = 0;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < DB_ଗଣନା; k++)
    {
	ଯଦି (curtab->tp_diffbuf[k] != ଖାଲି)
	{
	    ଯଦି (dp->df_ଗଣନା[k] > maxlength)
		maxlength = dp->df_ଗଣନା[k];
	}
    }
    ଫେରନ୍ତୁ maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_ଶୀର୍ଷ_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fromidx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		topline)
{
    diff_T	*topdiff = ଖାଲି;
    diff_T	*localtopdiff = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		topdiffchange = 0;

    ପାଇଁ (topdiff = curtab->tp_first_diff; topdiff != ଖାଲି;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	ଯଦି (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	ଯଦି (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_ଗଣନା[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    ଯଦି ((*thistopdiff) == ଖାଲି)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	ଯଦି (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_ଗଣନା[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    ଯଦି (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		ଭାଙ୍ଗ;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
calculate_topfill_and_topline(
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fromidx,
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	toidx,
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	from_topline,
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	from_topfill,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ଖାଲି;
    diff_T	*next_adjacent_blocks = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curlinenum_to = 1;

    find_ଶୀର୍ଷ_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ଯେତେବେଳେ (curdif && (curdif->df_lnum[fromidx] + curdif->df_ଗଣନା[fromidx])
							<= from_topline)
    {
	virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    ଯଦି (curdif != next_adjacent_blocks)
	virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    ଯଦି (virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed < 0)
	virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    ଯଦି (thistopdiff != ଖାଲି) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା virt_ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed;
    curdif = thistopdiff;
    ଯେତେବେଳେ (virt_ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ > 0 && curdif != ଖାଲି && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ, curdif->df_ଗଣନା[toidx]);
	virt_ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ -= MIN(virt_ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_virt_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
    ପାଇଁ (diff_T *dp = thistopdiff; dp != ଖାଲି; dp = dp->df_next)
    {
	ଯଦି (dp->df_lnum[toidx] + dp->df_ଗଣନା[toidx] <= curlinenum_to)
	    max_virt_ଧାଡ଼ିଗୁଡ଼ିକ += get_max_diff_length(dp);
	ଅନ୍ୟଥା
	{
	    ଯଦି (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_ଧାଡ଼ିଗୁଡ଼ିକ += curlinenum_to - dp->df_lnum[toidx];
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_ଧାଡ଼ିଗୁଡ଼ିକ - virtual_ଧାଡ଼ିଗୁଡ଼ିକ_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    ସ୍ଥିର ଶୂନ୍ୟ
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_numbers[DB_ଗଣନା];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		outputmap[DB_ଗଣନା];
    size_t	ndiffs = 0;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
    {
	ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
	{
	    ଧାଡ଼ି_numbers[i] = dp->df_lnum[i];
	    dp->df_ଗଣନା[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    ପାଇଁ (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	ଯଦି (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < DB_ଗଣନା; j++)
	    {
		ଯଦି (curtab->tp_diffbuf[j] != ଖାଲି)
		{
		    dp_s->df_lnum[j] = ଧାଡ଼ି_numbers[j];
		    dp_s->df_ଗଣନା[j] = 0;
		}
	    }
	}
	ପାଇଁ (size_t j = 0; j < ndiffs; j++)
	{
	    ଯଦି (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_ଗଣନା[outputmap[j]]++;
		ଧାଡ଼ି_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_ଗଣନା];
    ଅଚଳ mmfile_t	*diffbufs[DB_ଗଣନା];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			diff_length[DB_ଗଣନା];
    size_t		ndiffs = 0;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
    {
	ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    ଯଦି (dp->df_ଗଣନା[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_ଗଣନା[i] - 1);
	    }
	    ଅନ୍ୟଥା
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ଖାଲି;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_ଗଣନା[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *decisions = ଖାଲି;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    ପାଇଁ (size_t i = 0; i < ndiffs; i++)
	ମୁକ୍ତ(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    ମୁକ୍ତ(decisions);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_check_with_linestatus(win_T *wp, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *linestatus)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    buf_T	*buf = wp->w_buffer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmp;

    ଯଦି (linestatus != ଖାଲି)
	*linestatus = 0;

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    ଯଦି (curtab->tp_first_diff == ଖାଲି || !wp->w_p_diff)	// no diffs at all
	ଫେରନ୍ତୁ 0;

    // safety check: "lnum" must be a buffer line
    ଯଦି (lnum < 1 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	ଫେରନ୍ତୁ 0;

    idx = diff_buf_idx(buf);
    ଯଦି (idx == DB_ଗଣନା)
	ଫେରନ୍ତୁ 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    ଯଦି (hasFoldingWin(wp, lnum, ଖାଲି, ଖାଲି, TRUE, ଖାଲି))
	ଫେରନ୍ତୁ 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ଖାଲି;		// last block with df_lnum[idx] <= lnum
    ପାଇଁ (dp = diff_scan_start(curtab, idx, lnum); dp != ଖାଲି; dp = dp->df_next)
    {
	ଯଦି (dp->df_lnum[idx] <= lnum)
	    le = dp;
	ଯଦି (lnum <= dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
	    ଭାଙ୍ଗ;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ଯଦି (dp == ଖାଲି || lnum < dp->df_lnum[idx])
	ଫେରନ୍ତୁ 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    ଯଦି (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_fill = 0;
    ଯେତେବେଳେ (lnum == dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	ଯଦି (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    ଅଙ୍କ_fill += maxcount - dp->df_ଗଣନା[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	ଯଦି (dp->df_next != ଖାଲି
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_ଗଣନା[idx])
	    dp = dp->df_next;
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    ଯଦି (lnum < dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	    ଯଦି (i != idx && curtab->tp_diffbuf[i] != ଖାଲି)
	    {
		ଯଦି (dp->df_ଗଣନା[i] == 0)
		    zero = TRUE;
		ଅନ୍ୟଥା
		{
		    ଯଦି (dp->df_ଗଣନା[i] != dp->df_ଗଣନା[idx])
		    {
			ଯଦି (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			ଫେରନ୍ତୁ ଅଙ୍କ_fill;
		    }
		    cmp = TRUE;
		}
	    }
	ଯଦି (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
		ଯଦି (i != idx && curtab->tp_diffbuf[i] != ଖାଲି
						      && dp->df_ଗଣନା[i] != 0)
		    ଯଦି (!diff_equal_entry(dp, idx, i))
		    {
			ଯଦି (linestatus)
			    *linestatus = -1;
			ଫେରନ୍ତୁ ଅଙ୍କ_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	ଯଦି (zero == FALSE)
	    ଫେରନ୍ତୁ ଅଙ୍କ_fill;
	ଯଦି (linestatus)
	    *linestatus = -2;
	ଫେରନ୍ତୁ ଅଙ୍କ_fill;
    }
    ଫେରନ୍ତୁ ଅଙ୍କ_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_equal_entry(diff_T *dp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmp;

    ଯଦି (dp->df_ଗଣନା[idx1] != dp->df_ଗଣନା[idx2])
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (diff_check_sanity(curtab, dp) == FAIL)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (i = 0; i < dp->df_ଗଣନା[idx1]; ++i)
    {
	ଧାଡ଼ି = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	ଯଦି (ଧାଡ଼ି == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
	cmp = diff_cmp(ଧାଡ଼ି, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(ଧାଡ଼ି);
	ଯଦି (cmp != 0)
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_equal_char(char_u *p1, char_u *p2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l  = (*mb_ptr2len)(p1);

    ଯଦି (l != (*mb_ptr2len)(p2))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (l > 1)
    {
	ଯଦି (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    ଫେରନ୍ତୁ FALSE;
	*len = l;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    ଫେରନ୍ତୁ FALSE;
	*len = 1;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	ଫେରନ୍ତୁ 0;

    ଯଦି ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	ଫେରନ୍ତୁ STRCMP(s1, s2);
    ଯଦି ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	ଫେରନ୍ତୁ MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ଯେତେବେଳେ (*p1 != NUL && *p2 != NUL)
    {
	ଯଦି (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!diff_equal_char(p1, p2, &l))
		ଭାଙ୍ଗ;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    ଯଦି (*p1 != NUL || *p2 != NUL)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_check_fill(win_T *wp, linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    // be quick when there are no filler lines
    ଯଦି (!(diff_flags & DIFF_FILLER))
	ଫେରନ୍ତୁ 0;
    n = diff_check_with_linestatus(wp, lnum, ଖାଲି);
    ଯଦି (n <= 0)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    ଶୂନ୍ୟ
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fromidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    ଯଦି (fromidx == DB_ଗଣନା)
	ଫେରନ୍ତୁ;		// safety check

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ଯଦି (lnum <= dp->df_lnum[fromidx] + dp->df_ଗଣନା[fromidx])
	    ଭାଙ୍ଗ;
    ଯଦି (dp == ଖାଲି)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
				       - (frombuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum);
    }
    ଅନ୍ୟଥା
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	ଯଦି (toidx == DB_ଗଣନା)
	    ଫେରନ୍ତୁ;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	ଯଦି (lnum >= dp->df_lnum[fromidx])
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
    ଯଦି (towin->w_topline > towin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	towin->w_botfill = true;
    }
    ଯଦି (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_ଧାଡ଼ି_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (ଶୂନ୍ୟ)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ଖାଲି, TRUE, ଖାଲି);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_diffanchors(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_anchors)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ଖାଲି;
    ଯଦି (check_only)
	bufwin = curwin;
    ଅନ୍ୟଥା
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    ଯଦି (bufwin->w_buffer == buf && bufwin->w_p_diff)
		ଭାଙ୍ଗ;
	ଯଦି (bufwin == ଖାଲି && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ପାଇଁ (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	ଯଦି (*dia == ',') // don't allow empty values
	    ଫେରନ୍ତୁ FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(ଖାଲି, &dia, ADDR_ଧାଡ଼ିଗୁଡ଼ିକ, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	ଯଦି (dia == ଖାଲି) // error detected
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*dia != ',' && *dia != NUL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1))
	{
	    emsg(_(e_invalid_range));
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (anchors != ଖାଲି)
	    anchors[i] = lnum;

	ଯଦି (*dia == ',')
	    dia++;
    }
    ଯଦି (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (ଅଙ୍କ_anchors != ଖାଲି)
	*ଅଙ୍କ_anchors = i;
    ଫେରନ୍ତୁ OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diffanchors_changed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflocal)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = parse_diffanchors(TRUE, curbuf, ଖାଲି, ଖାଲି);
    ଯଦି (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    ଯଦି (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < DB_ଗଣନା; ++idx)
		    ଯଦି (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ଭାଙ୍ଗ;
		    }
	    }
	}
    }
    ଫେରନ୍ତୁ result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diffopt_changed(ଶୂନ୍ୟ)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diff_context_new = 6;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linematch_ଧାଡ଼ିଗୁଡ଼ିକ_new = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diff_flags_new = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ଯେତେବେଳେ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	ଯଦି (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    ଯଦି (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    ଯଦି (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_ଧାଡ଼ିଗୁଡ଼ିକ_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    ଯଦି ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	ଫେରନ୍ତୁ FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    ଯଦି (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_ଧାଡ଼ିଗୁଡ଼ିକ = linematch_ଧାଡ଼ିଗୁଡ଼ିକ_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diffopt_horizontal(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diffopt_hiddenoff(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diffopt_closeoff(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    ଶୂନ୍ୟ
diff_update_ଧାଡ଼ି(linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    diff_T	*dp;

    ଯଦି (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	ଫେରନ୍ତୁ;

    idx = diff_buf_idx(curbuf);
    ଯଦି (idx == DB_ଗଣନା)
	ଫେରନ୍ତୁ;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ଯଦି (lnum <= dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
	    ଭାଙ୍ଗ;

    // clear the inline change cache as it's invalid
    ଯଦି (dp != ଖାଲି)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

ସ୍ଥିର diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *change_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *change_end)
{
    ଯଦି (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    ଅନ୍ୟଥା
	*change_start = change->dc_start[diffline->bufidx];
    ଯଦି (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    ଅନ୍ୟଥା
	*change_end = change->dc_end[diffline->bufidx];

    ଯଦି (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	ଫେରନ୍ତୁ FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
    {
	ଯଦି (i == diffline->bufidx)
	    ଜାରି;
	ଯଦି (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*startp,	// first char of the change
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*endp)		// last char of the change
{
    char_u	*ଧାଡ଼ି_org;
    char_u	*ଧାଡ଼ି_new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_org, si_new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ei_org, ei_new;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added = TRUE;
    char_u	*p1, *p2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	ଧାଡ଼ି_org = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	ଧାଡ଼ି_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	ଯଦି (ଧାଡ଼ି_org == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    ଯଦି (off >= dp->df_ଗଣନା[i])
		ଜାରି;
	    added = FALSE;
	    ଯଦି (diff_flags & DIFF_INLINE_NONE)
		ଭାଙ୍ଗ; // early terminate as we only care about the return value

	    ଧାଡ଼ି_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ଯେତେବେଳେ (ଧାଡ଼ି_org[si_org] != NUL)
	    {
		ଯଦି (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(ଧାଡ଼ି_org[si_org])
					      && VIM_ISWHITE(ଧାଡ଼ି_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(ଧାଡ଼ି_org[si_org])
					    || VIM_ISWHITE(ଧାଡ଼ି_new[si_new]))))
		{
		    si_org = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skipwhite(ଧାଡ଼ି_org + si_org) - ଧାଡ଼ି_org);
		    si_new = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(skipwhite(ଧାଡ଼ି_new + si_new) - ଧାଡ଼ି_new);
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (!diff_equal_char(ଧାଡ଼ି_org + si_org, ଧାଡ଼ି_new + si_new,
									   &l))
			ଭାଙ୍ଗ;
		    si_org += l;
		    si_new += l;
		}
	    }
	    ଯଦି (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(ଧାଡ଼ି_org, ଧାଡ଼ି_org + si_org);
		si_new -= (*mb_head_off)(ଧାଡ଼ି_new, ଧାଡ଼ି_new + si_new);
	    }
	    ଯଦି (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    ଯଦି (ଧାଡ଼ି_org[si_org] != NUL || ଧାଡ଼ି_new[si_new] != NUL)
	    {
		ei_org = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି_org);
		ei_new = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି_new);
		ଯେତେବେଳେ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    ଯଦି (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(ଧାଡ଼ି_org[ei_org])
					      && VIM_ISWHITE(ଧାଡ଼ି_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(ଧାଡ଼ି_org[ei_org])
					    || VIM_ISWHITE(ଧାଡ଼ି_new[ei_new]))))
		    {
			ଯେତେବେଳେ (ei_org >= *startp
					     && VIM_ISWHITE(ଧାଡ଼ି_org[ei_org]))
			    --ei_org;
			ଯେତେବେଳେ (ei_new >= si_new
					     && VIM_ISWHITE(ଧାଡ଼ି_new[ei_new]))
			    --ei_new;
		    }
		    ଅନ୍ୟଥା
		    {
			p1 = ଧାଡ଼ି_org + ei_org;
			p2 = ଧାଡ଼ି_new + ei_new;
			p1 -= (*mb_head_off)(ଧାଡ଼ି_org, p1);
			p2 -= (*mb_head_off)(ଧାଡ଼ି_new, p2);
			ଯଦି (!diff_equal_char(p1, p2, &l))
			    ଭାଙ୍ଗ;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		ଯଦି (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(ଧାଡ଼ି_org);
    ଫେରନ୍ତୁ added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    long byte_start;
    long ଅଙ୍କ_bytes;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pass = 1;
    କର
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_unmerged_gaps = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ଯେତେବେଳେ (dp!= ଖାଲି && dp->df_next != ଖାଲି)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    ଯଦି (dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ଜାରି;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    ଯଦି (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ଜାରି;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1]);
	    ଯଦି (gap <= 3)
	    {
		linenr_T max_df_ଗଣନା = 0;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
		    max_df_ଗଣନା = MAX(max_df_ଗଣନା, dp->df_ଗଣନା[i] + dp->df_next->df_ଗଣନା[i]);

		ଯଦି (max_df_ଗଣନା >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
		    {
			dp->df_ଗଣନା[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_ଗଣନା[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    ଜାରି;
		}
		ଅନ୍ୟଥା
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	ଯଦି (!has_unmerged_gaps || !has_merged_gaps)
	    ଭାଙ୍ଗ;
    } ଯେତେବେଳେ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1,
				  linenr_T start_lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pass = 1;
    କର
    {
	diff_T *dp = dp_orig;

	ଯେତେବେଳେ (dp != ଖାଲି && dp->df_next != ଖାଲି)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    ଯଦି (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		ଜାରି;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    ଯଦି (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		ଜାରି;
	    }

	    // Calculate the gap between blocks
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା gap_start = entry1->byte_start + entry1->ଅଙ୍କ_bytes;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା gap_end = entry2->byte_start;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    ଯଦି (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		ଜାରି;
	    }

	    // Get the text between the two blocks
	    char_u *ଧାଡ଼ି = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = ଧାଡ଼ି + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		ଯଦି (char_class == 2)
		{
		    only_non_word = false;
		    ଭାଙ୍ଗ;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    ଯଦି (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
		{
		    ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
		    {
			// count bytes in the first block
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < dp->df_ଗଣନା[i]; k++)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = dp->df_lnum[i] + k - 1;
			    ଯଦି (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].ଅଙ୍କ_bytes;
			}
			// count bytes in the next block
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < dp->df_next->df_ଗଣନା[i]; k++)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = dp->df_next->df_lnum[i] + k - 1;
			    ଯଦି (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].ଅଙ୍କ_bytes;
			}
		    }
		}

		ଯଦି (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
		    {
			ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
			{
			    dp->df_ଗଣନା[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_ଗଣନା[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    ଜାରି;
		}
	    }

	    dp = dp->df_next;
	}
    } ଯେତେବେଳେ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_ଗଣନା];
    garray_T	file1_str;
    garray_T	file2_str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, ଆକାର(ଅକ୍ଷର *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = ଖାଲି;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_ଗଣନା]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, ଆକାର(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
	ga_init2(&linemap[i], ଆକାର(linemap_entry_T), 128);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
	    ଜାରି; // skip buffer that isn't loaded

	ଯଦି (dp->df_ଗଣନା[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ଖାଲି;
	    ଜାରି;
	}

	ଯଦି (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = 0; off < dp->df_ଗଣନା[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_keyword = FALSE;

	    // iwhiteeol support vars
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_white = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eol_ga_len = -1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eol_linemap_len = -1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eol_numlines = -1;

	    char_u *s;
	    ପାଇଁ (s = curline; *s != NUL;)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_in_keyword = FALSE;
		ଯଦି (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		ଯଦି (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ଯଦି (VIM_ISWHITE(*s))
		{
		    ଯଦି (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			ଜାରି;
		    }
		    ଅନ୍ୟଥା ଯଦି ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ଯଦି (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_len = 1;
		ଯଦି (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		ଅନ୍ୟଥା
		{
		    char_len = mb_ptr2len(s);

		    ଯଦି (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    ଯଦି (diff_flags & DIFF_ICASE)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			ଯଦି (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    ଅନ୍ୟଥା
			ga_concat_len(curstr, s, char_len);
		}

		ଯଦି (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ଯଦି (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.ଅଙ୍କ_bytes = char_len;
		    ଯଦି (ga_grow(&linemap[i], 1) != OK)
			ଯାଅ done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		ଅନ୍ୟଥା
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].ଅଙ୍କ_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    ଯଦି (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    ଯଦି ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		ଯଦି (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    ଯଦି (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.ଅଙ୍କ_bytes = ଆକାର(NL);
		ଯଦି (ga_grow(&linemap[i], 1) != OK)
		    ଯାଅ done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	ଯଦି (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (ଅକ୍ଷର *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	ଅନ୍ୟଥା
	{
	    dio.dio_orig.din_mmfile.ptr = (ଅକ୍ଷର *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	ଯଦି (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_status = diff_file_internal(&dio);
	    ଯଦି (diff_status == FAIL)
		ଯାଅ done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    ଯଦି (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    ଅନ୍ୟଥା ଯଦି (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    ପାଇଁ (; new_diff != ଖାଲି; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
	{
	    ଯଦି (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		ଜାରି;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_ଗଣନା[i];

	    ଯଦି (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    ଅନ୍ୟଥା
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    ଯଦି (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    ଅନ୍ୟଥା ଯଦି (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    ଅନ୍ୟଥା
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].ଅଙ୍କ_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	ଯଦି (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    ଯାଅ done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, ଆକାର(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;

    idx = diff_buf_idx(wp->w_buffer);
    ଯଦି (idx == DB_ଗଣନା)	// cannot happen
	ଫେରନ୍ତୁ FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ଯଦି (lnum < dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
	    ଭାଙ୍ଗ;
    ଯଦି (dp == ଖାଲି || diff_check_sanity(curtab, dp) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	ଫେରନ୍ତୁ FALSE;
    off = lnum - dp->df_lnum[idx];

    ଯଦି (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	change_start = MAXCOL;	// first col of changed area
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	change_end = -1;	// last col of changed area
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->ଅଙ୍କ_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	ଫେରନ୍ତୁ ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    ଯଦି (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_changes = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା change_idx = 0;
    diffline->changes = ଖାଲି;
    ପାଇଁ (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	ଯଦି (change->dc_end_lnum_off[idx] < off)
	    ଜାରି;
	ଯଦି (change->dc_start_lnum_off[idx] > off)
	    ଭାଙ୍ଗ;
	ଯଦି (diffline->changes == ଖାଲି)
	    diffline->changes = change;
	ଅଙ୍କ_changes++;
    }
    diffline->ଅଙ୍କ_changes = ଅଙ୍କ_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = FALSE;
    ଯଦି (ଅଙ୍କ_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < DB_ଗଣନା; i++)
	{
	    ଯଦି (idx == i)
		ଜାରି;
	    ଯଦି (curtab->tp_diffbuf[i] == ଖାଲି)
		ଜାରି;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    ଯଦି (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		ଭାଙ୍ଗ;
	    }
	}
    }
    ଫେରନ୍ତୁ added;
}

# ଯଦି defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_infold(win_T *wp, linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    ଯଦି (!wp->w_p_diff)
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
    {
	ଯଦି (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	ଅନ୍ୟଥା ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    ଯଦି (idx == -1 || !other)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    ଯଦି (curtab->tp_first_diff == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ଖାଲି;		// last block with df_lnum[idx] <= lnum
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = TRUE;
    ପାଇଁ (dp = diff_scan_start(curtab, idx, lnum); dp != ଖାଲି; dp = dp->df_next)
    {
	ଯଦି (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	ଯଦି (dp->df_lnum[idx] - diff_context > lnum)
	    ଭାଙ୍ଗ;
	// If this change ends before the line we have a match.
	ଯଦି (dp->df_lnum[idx] + dp->df_ଗଣନା[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    ଭାଙ୍ଗ;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    ଫେରନ୍ତୁ result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    ଶୂନ୍ୟ
nv_diffgetput(ପୂର୍ଣ୍ଣସଂଖ୍ୟା put, long ଗଣନା_2)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    ଯଦି (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	ଫେରନ୍ତୁ;
    }
# endif
    ଯଦି (ଗଣନା_2 == 0)
	ea.arg = (char_u *)"";
    ଅନ୍ୟଥା
    {
	vim_snprintf((ଅକ୍ଷର *)buf, 30, "%ld", ଗଣନା_2);
	ea.arg = buf;
    }
    ଯଦି (put)
	ea.cmdidx = CMD_diffput;
    ଅନ୍ୟଥା
	ea.cmdidx = CMD_diffget;
    ea.addr_ଗଣନା = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ଯଦି (dp == diff)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    ଶୂନ୍ୟ
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_cur;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_other;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_from;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx_to;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_skip, end_skip;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buf_empty;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    ଯଦି (idx_cur == DB_ଗଣନା)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	ପାଇଁ (idx_other = 0; idx_other < DB_ଗଣନା; ++idx_other)
	    ଯଦି (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ଖାଲି)
	    {
		ଯଦି (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ଭାଙ୍ଗ;
		found_not_ma = TRUE;
	    }
	ଯଦି (idx_other == DB_ଗଣନା)
	{
	    ଯଦି (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    ଅନ୍ୟଥା
		emsg(_(e_no_other_buffer_in_diff_mode));
	    ଫେରନ୍ତୁ;
	}

	// Check that there isn't a third buffer in the list
	ପାଇଁ (i = idx_other + 1; i < DB_ଗଣନା; ++i)
	    ଯଦି (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ଖାଲି
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		ଫେରନ୍ତୁ;
	    }
    }
    ଅନ୍ୟଥା
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ଯେତେବେଳେ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	ପାଇଁ (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	ଯଦି (eap->arg + i == p)	    // digits only
	    i = atol((ଅକ୍ଷର *)eap->arg);
	ଅନ୍ୟଥା
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    ଯଦି (i < 0)
		ଫେରନ୍ତୁ;		// error message already given
	}
	buf = buflist_findnr(i);
	ଯଦି (buf == ଖାଲି)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (buf == curbuf)
	    ଫେରନ୍ତୁ;		// nothing to do
	idx_other = diff_buf_idx(buf);
	ଯଦି (idx_other == DB_ଗଣନା)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    ଫେରନ୍ତୁ;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    ଯଦି (eap->addr_ଗଣନା == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା linestatus = 0;
	ଯଦି (eap->line1 == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	ଅନ୍ୟଥା ଯଦି (eap->line1 > 0)
	    --eap->line1;
    }

    ଯଦି (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    ଅନ୍ୟଥା
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	ଯଦି (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    ଯାଅ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    ଯଦି (!curbuf->b_changed)
    {
	change_warning(0);
	ଯଦି (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    ଯାଅ theend;
	}
    }

    dprev = ଖାଲି;
    ପାଇଁ (dp = curtab->tp_first_diff; dp != ଖାଲି; )
    {
	ଯଦି (!eap->addr_ଗଣନା)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ଯେତେବେଳେ (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_ଗଣନା[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	ଯଦି (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ଭାଙ୍ଗ;	// past the range that was specified

	dfree = ଖାଲି;
	lnum = dp->df_lnum[idx_to];
	ଗଣନା_2 = dp->df_ଗଣନା[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + ଗଣନା_2 + (ଗଣନା_2 == 0 && BUFEMPTY() ? 1 : 0);

	ଯଦି (dp->df_lnum[idx_cur] + dp->df_ଗଣନା[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    ଯଦି (eap->addr_ଗଣନା > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		ଯଦି (start_skip > 0)
		{
		    // range starts below start of current diff block
		    ଯଦି (start_skip > ଗଣନା_2)
		    {
			lnum += ଗଣନା_2;
			ଗଣନା_2 = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			ଗଣନା_2 -= start_skip;
			lnum += start_skip;
		    }
		}
		ଅନ୍ୟଥା
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_ଗଣନା[idx_cur] - 1
							 - (eap->line2 + off);
		ଯଦି (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    ଯଦି (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_ଗଣନା[idx_cur] - start_skip - end_skip;
			ଯଦି (ଗଣନା_2 > i)
			    ଗଣନା_2 = i;
		    }
		    ଅନ୍ୟଥା			// :diffget
		    {
			ଗଣନା_2 -= end_skip;
			end_skip = dp->df_ଗଣନା[idx_from] - start_skip - ଗଣନା_2;
			ଯଦି (end_skip < 0)
			    end_skip = 0;
		    }
		}
		ଅନ୍ୟଥା
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା == 1;
		ଯଦି (ml_delete(lnum) == OK)
		    --added;
	    }
	    ପାଇଁ (i = 0; i < dp->df_ଗଣନା[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		ଯଦି (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		    ଭାଙ୍ଗ;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		ଯଦି (p != ଖାଲି)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    ଯଦି (buf_empty && curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_ଗଣନା = dp->df_ଗଣନା[idx_to] + added;
	    dp->df_ଗଣନା[idx_to] = new_ଗଣନା;

	    ଯଦି (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
		    ଯଦି (curtab->tp_diffbuf[i] != ଖାଲି && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			ଭାଙ୍ଗ;
		ଯଦି (i == DB_ଗଣନା)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    ଯଦି (dprev == ଖାଲି)
			curtab->tp_first_diff = dp;
		    ଅନ୍ୟଥା
			dprev->df_next = dp;
		}
	    }

	    ଯଦି (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + ଗଣନା_2 - 1, (long)MAXLNUM, (long)added);
		ଯଦି (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    ଯଦି (curwin->w_cursor.lnum >= lnum + ଗଣନା_2)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
		    }
		    ଅନ୍ୟଥା ଯଦି (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_ଧାଡ଼ିଗୁଡ଼ିକ(lnum, 0, lnum + ଗଣନା_2, (long)added);

	    ଯଦି (dfree != ଖାଲି)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    ଯଦି (added != 0 && !valid_diff(dp))
		ଭାଙ୍ଗ;

	    ଯଦି (dfree == ଖାଲି)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_ଗଣନା[idx_to] = new_ଗଣନା;

	    // When changing the current buffer, keep track of line numbers
	    ଯଦି (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	ଯଦି (dfree == ଖାଲି)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    ଯଦି (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	ଯଦି (ଚାବି_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    ଯଦି (diff_need_update)
	ex_diffupdate(ଖାଲି);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_ଧାଡ଼ି_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    ଯଦି (curtab->tp_first_diff == ଖାଲି)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    ଯଦି (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    ଯଦି (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    ଅନ୍ୟଥା
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ଖାଲି, ଖାଲି, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
diff_fold_update(diff_T *dp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ପାଇଁ (i = 0; i < DB_ଗଣନା; ++i)
	    ଯଦି (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_ଗଣନା[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ଯଦି (diff_buf_idx_tp(buf, tp) != DB_ଗଣନା)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
diff_move_to(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, long ଗଣନା_2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    ଯଦି (idx == DB_ଗଣନା || curtab->tp_first_diff == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    ଯଦି (curtab->tp_first_diff == ଖାଲି)		// no diffs today
	ଫେରନ୍ତୁ FAIL;

    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
    {
	// Check if already before first diff.
	ଯଦି (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    ଭାଙ୍ଗ;

	ପାଇଁ (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    ଯଦି (dp == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଯଦି ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == ଖାଲି
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ଭାଙ୍ଗ;
	    }
	}
    }

    // don't end up past the end of the file
    ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // When the cursor didn't move at all we fail.
    ଯଦି (lnum == curwin->w_cursor.lnum)
	ଫେରନ୍ତୁ FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    ଫେରନ୍ତୁ OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    ସ୍ଥିର linenr_T
diff_get_corresponding_ଧାଡ଼ି_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx2;
    diff_T	*dp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    ଯଦି (idx1 == DB_ଗଣନା || idx2 == DB_ଗଣନା || curtab->tp_first_diff == ଖାଲି)
	ଫେରନ୍ତୁ lnum1;

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    ଯଦି (curtab->tp_first_diff == ଖାଲି)		// no diffs today
	ଫେରନ୍ତୁ lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	ଯଦି (dp->df_lnum[idx1] > lnum1)
	    ଫେରନ୍ତୁ lnum1 - baseline;
	ଯଦି ((dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    ଯଦି (baseline > dp->df_ଗଣନା[idx2])
		baseline = dp->df_ଗଣନା[idx2];

	    ଫେରନ୍ତୁ dp->df_lnum[idx2] + baseline;
	}
	ଯଦି (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_ଗଣନା[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_ଗଣନା[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    ଫେରନ୍ତୁ curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_ଗଣନା[idx1])
				   - (dp->df_lnum[idx2] + dp->df_ଗଣନା[idx2]);
    }

    // If we get here then the cursor is after the last diff
    ଫେରନ୍ତୁ lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_ଧାଡ଼ି(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_ଧାଡ଼ି_int(buf1, lnum1);

    // don't end up past the end of the file
    ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଫେରନ୍ତୁ lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    ଯଦି (idx == DB_ଗଣନା)		// safety check
	ଫେରନ୍ତୁ (linenr_T)0;

    ଯଦି (curtab->tp_diff_invalid)
	ex_diffupdate(ଖାଲି);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	ଯଦି (lnum <= dp->df_lnum[idx] + dp->df_ଗଣନା[idx])
	    ଭାଙ୍ଗ;

    // When after the last change, compute relative to the last line number.
    ଯଦି (dp == ଖାଲି)
	ଫେରନ୍ତୁ wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
					- (curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    ଯଦି (i == DB_ଗଣନା)			// safety check
	ଫେରନ୍ତୁ (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    ଯଦି (n > dp->df_lnum[i] + dp->df_ଗଣନା[i])
	n = dp->df_lnum[i] + dp->df_ଗଣନା[i];
    ଫେରନ୍ତୁ n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_diff_ed(
	char_u	    *ଧାଡ଼ି,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = ଧାଡ଼ି;
    f1 = getdigits(&p);
    ଯଦି (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    ଅନ୍ୟଥା
	l1 = f1;
    ଯଦି (*p != 'a' && *p != 'c' && *p != 'd')
	ଫେରନ୍ତୁ FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    ଯଦି (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    ଅନ୍ୟଥା
	l2 = f2;
    ଯଦି (l1 < f1 || l2 < f2)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->ଗଣନା_orig = 0;
    }
    ଅନ୍ୟଥା
    {
	hunk->lnum_orig = f1;
	hunk->ଗଣନା_orig = l1 - f1 + 1;
    }
    ଯଦି (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->ଗଣନା_new = 0;
    }
    ଅନ୍ୟଥା
    {
	hunk->lnum_new = f2;
	hunk->ଗଣନା_new = l2 - f2 + 1;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_diff_unified(
	char_u	    *ଧାଡ଼ି,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = ଧାଡ଼ି;
    ଯଦି (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	ଯଦି (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	ଅନ୍ୟଥା
	    oldcount = 1;
	ଯଦି (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    ଯଦି (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    ଅନ୍ୟଥା
		newcount = 1;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;	// invalid diff format

	ଯଦି (oldcount == 0)
	    oldline += 1;
	ଯଦି (newcount == 0)
	    newline += 1;
	ଯଦି (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->ଗଣନା_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->ଗଣନା_new = newcount;

	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xdiff_out_indices(
	long start_a,
	long ଗଣନା_a,
	long start_b,
	long ଗଣନା_b,
	ଶୂନ୍ୟ *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	ଫେରନ୍ତୁ -1;
    }

    p->lnum_orig  = start_a + 1;
    p->ଗଣନା_orig = ଗଣନା_a;
    p->lnum_new   = start_b + 1;
    p->ଗଣନା_new  = ଗଣନା_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    ଫେରନ୍ତୁ 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xdiff_out_unified(
	ଶୂନ୍ୟ *priv,
	mmbuffer_t *mb,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    ଫେରନ୍ତୁ 0;
}

#endif	// FEAT_DIFF

#ଯଦି defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    ଶୂନ୍ୟ
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    ଶୂନ୍ୟ
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    ସ୍ଥିର linenr_T	prev_lnum = 0;
    ସ୍ଥିର varnumber_T	changedtick = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_diff_flags = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_start = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_end = 0;
    ସ୍ଥିର hlf_T	hlID = (hlf_T)0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			cache_results = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    ଯଦି (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    ଯଦି (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	ଯଦି (linestatus < 0)
	{
	    ଯଦି (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		ଯଦି (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		ଅନ୍ୟଥା
		{
		    hlID = HLF_CHD;	// changed line
		    ଯଦି (diffline.ଅଙ୍କ_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    ଅନ୍ୟଥା
		hlID = HLF_ADD;	// added line
	}
	ଅନ୍ୟଥା
	    hlID = (hlf_T)0;

	ଯଦି (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    ଯଦି (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_ସଂଖ୍ୟା(&argvars[1]) - 1; // ignore type error in {col}
	ଯଦି (cache_results)
	{
	    ଯଦି (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    ଅନ୍ୟଥା
		hlID = HLF_CHD;			// changed line
	}
	ଅନ୍ୟଥା
	{
	    hlID = HLF_CHD;
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < diffline.ଅଙ୍କ_changes; i++)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		ଯଦି (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ଭାଙ୍ଗ;
		}
		ଯଦି (col < change_start)
		    // the remaining changes are past this column and not relevant
		    ଭାଙ୍ଗ;
	    }
	}
    }
    rettv->vval.v_ସଂଖ୍ୟା = hlID == (hlf_T)0 ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_diff_optarg(
    typval_T	    *opts,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    ଯଦି (algo != ଖାଲି)
    {
	ଯଦି (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    ଯଦି (output_fmt != ଖାଲି)
    {
	ଯଦି (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	ଅନ୍ୟଥା
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    *diff_ctxlen = dict_get_ସଂଖ୍ୟା_def(d, "context", 0);
    ଯଦି (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    ଯଦି (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    ଯଦି (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    ଯଦି (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    ଯଦି (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    ଯଦି (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    ଯଦି (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    ଫେରନ୍ତୁ OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_to_diffin(list_T *l, diffin_T *din, ପୂର୍ଣ୍ଣସଂଖ୍ୟା icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	str = tv_get_string(&li->li_tv);
	ଯଦି (icase)
	{
	    str = strlow_save(str);
	    ଯଦି (str == ଖାଲି)
		ଜାରି;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	ଯଦି (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (ଅକ୍ଷର *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    ସ୍ଥିର dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    ଯଦି (hunk_dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    dict_add_ସଂଖ୍ୟା(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_ସଂଖ୍ୟା(hunk_dict, "from_count", hunk->ଗଣନା_orig);
    dict_add_ସଂଖ୍ୟା(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_ସଂଖ୍ୟା(hunk_dict, "to_count", hunk->ଗଣନା_new);

    ଫେରନ୍ତୁ hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    ଶୂନ୍ୟ
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    ଯଦି (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	ଫେରନ୍ତୁ;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, ଆକାର(ଅକ୍ଷର *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	ଯଦି (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    ଫେରନ୍ତୁ;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    ଯଦି (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff_status = diff_file(&dio);

    // restore 'diffexpr'
    ଯଦି (restore_diffexpr)
	*p_dex = cc;

    ଯଦି (diff_status == FAIL)
	ଯାଅ done;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hunk_idx = 0;
    dict_T	*hunk_dict;

    ଯଦି (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	ଯଦି (rettv_list_alloc(rettv) != OK)
	    ଯାଅ done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ଖାଲି;
	ଯେତେବେଳେ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    ଯଦି (hunk_dict == ଖାଲି)
		ଯାଅ done;

	    ଯଦି (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    ଅନ୍ୟଥା
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    ଯଦି (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    ଅନ୍ୟଥା
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
