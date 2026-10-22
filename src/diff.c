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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "xdiff/xdiff.h"

#ఒకవేళ defined(FEAT_DIFF)

స్థిర పూర్ణాంకం diff_busy = FALSE;	    // using diff structs, don't change them
స్థిర పూర్ణాంకం diff_need_update = FALSE; // ex_diffupdate needs to be called

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
# define DIFF_అంతర్గత	0x200	// use internal xdiff algorithm
# define DIFF_CLOSE_OFF	0x400	// diffoff when closing window
# define DIFF_FOLLOWWRAP	0x800	// follow the wrap option
# define DIFF_LINEMATCH  0x1000  // match most similar lines within diff
# define DIFF_INLINE_NONE    0x2000  // no inline highlight
# define DIFF_INLINE_SIMPLE  0x4000  // inline highlight with simple algorithm
# define DIFF_INLINE_CHAR    0x8000  // inline highlight with character diff
# define DIFF_INLINE_మాట    0x10000 // inline highlight with word diff
# define DIFF_ANCHOR	0x20000	// use 'diffanchors' to anchor the diff
# define ALL_WHITE_DIFF (DIFF_IWHITE | DIFF_IWHITEALL | DIFF_IWHITEEOL)
# define ALL_INLINE (DIFF_INLINE_NONE | DIFF_INLINE_SIMPLE | DIFF_INLINE_CHAR | DIFF_INLINE_మాట)
# define ALL_INLINE_DIFF (DIFF_INLINE_CHAR | DIFF_INLINE_మాట)
స్థిర పూర్ణాంకం	diff_flags = DIFF_అంతర్గత | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

స్థిర దీర్ఘ diff_algorithm = XDF_INDENT_HEURISTIC;
స్థిర పూర్ణాంకం diff_మాట_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_కొలత (1024L * 1024 * 1023)

స్థిర పూర్ణాంకం diff_a_పనిచేస్తుంది = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# ఒకవేళ defined(MSWIN)
స్థిర పూర్ణాంకం diff_bin_పనిచేస్తుంది = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
రకనిర్వచనం నిర్మాణం {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
రకనిర్వచనం నిర్మాణం {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
రకనిర్వచనం నిర్మాణం {
    linenr_T lnum_orig;
    దీర్ఘ     లెక్క_orig;
    linenr_T lnum_new;
    దీర్ఘ     లెక్క_new;
} diffhunk_T;

రకనిర్వచనం గణన {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
రకనిర్వచనం నిర్మాణం {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    పూర్ణాంకం		    dio_అంతర్గత; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    పూర్ణాంకం		    dio_ctxlen;   // unified diff context length
} diffio_T;

స్థిర పూర్ణాంకం diff_buf_idx(buf_T *buf);
స్థిర పూర్ణాంకం diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
స్థిర శూన్య diff_mark_adjust_tp(tabpage_T *tp, పూర్ణాంకం idx, linenr_T line1, linenr_T line2, దీర్ఘ amount, దీర్ఘ amount_after);
స్థిర శూన్య diff_check_unchanged(tabpage_T *tp, diff_T *dp);
స్థిర పూర్ణాంకం diff_check_sanity(tabpage_T *tp, diff_T *dp);
స్థిర పూర్ణాంకం check_external_diff(diffio_T *diffio);
స్థిర పూర్ణాంకం diff_file(diffio_T *diffio);
స్థిర పూర్ణాంకం diff_equal_నమోదు(diff_T *dp, పూర్ణాంకం idx1, పూర్ణాంకం idx2);
స్థిర పూర్ణాంకం diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
స్థిర శూన్య diff_fold_update(diff_T *dp, పూర్ణాంకం skip_idx);
# endif
స్థిర శూన్య diff_read(పూర్ణాంకం idx_orig, పూర్ణాంకం idx_new, diffio_T *dio);
స్థిర శూన్య diff_copy_నమోదు(diff_T *dprev, diff_T *dp, పూర్ణాంకం idx_orig, పూర్ణాంకం idx_new);
స్థిర diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
స్థిర పూర్ణాంకం parse_diff_ed(char_u *పంక్తి, diffhunk_T *hunk);
స్థిర పూర్ణాంకం parse_diff_unified(char_u *పంక్తి, diffhunk_T *hunk);
స్థిర పూర్ణాంకం xdiff_బహిర్గతం_indices(దీర్ఘ ప్రారంభం_a, దీర్ఘ లెక్క_a, దీర్ఘ ప్రారంభం_b, దీర్ఘ లెక్క_b, శూన్య *priv);
స్థిర పూర్ణాంకం xdiff_బహిర్గతం_unified(శూన్య *priv, mmbuffer_t *mb, పూర్ణాంకం nbuf);
స్థిర పూర్ణాంకం parse_diffanchors(పూర్ణాంకం check_only, buf_T *buf, linenr_T *anchors, పూర్ణాంకం *అంకె_anchors);

# define FOR_ALL_DIFFBLOCKS_లోపల_TAB(tp, dp) \
    కొరకు ((dp) = (tp)->tp_మొదటి_diff; (dp) != ఖాళీ; (dp) = (dp)->df_తదుపరి)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
స్థిర diff_T	    *diff_finger_dp = ఖాళీ;
స్థిర tabpage_T    *diff_finger_tp = ఖాళీ;
స్థిర పూర్ణాంకం	    diff_finger_idx = -1;
స్థిర linenr_T	    diff_finger_lnum = 0;

    స్థిర శూన్య
clear_diffblock(diff_T *dp)
{
    ఒకవేళ (dp == diff_finger_dp)
	diff_finger_dp = ఖాళీ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    స్థిర diff_T *
diff_పరిశీలన_ప్రారంభం(tabpage_T *tp, పూర్ణాంకం idx, linenr_T lnum)
{
    ఒకవేళ (diff_finger_dp != ఖాళీ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	తిరిగి diff_finger_dp;
    తిరిగి tp->tp_మొదటి_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    స్థిర శూన్య
diff_పరిశీలన_remember(tabpage_T *tp, పూర్ణాంకం idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ఖాళీ ? dp : tp->tp_మొదటి_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    శూన్య
diff_buf_delete(buf_T *buf)
{
    పూర్ణాంకం		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ఒకవేళ (i != DB_లెక్క)
	{
	    tp->tp_diffbuf[i] = ఖాళీ;
	    tp->tp_diff_invalid = TRUE;
	    ఒకవేళ (tp == curtab)
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
    శూన్య
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    పూర్ణాంకం		i;

    ఒకవేళ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_బఫరు == win->w_బఫరు && wp->w_p_diff)
		ఆపు;
	ఒకవేళ (wp == ఖాళీ)
	{
	    i = diff_buf_idx(win->w_బఫరు);
	    ఒకవేళ (i != DB_లెక్క)
	    {
		curtab->tp_diffbuf[i] = ఖాళీ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    లేకపోతే
	diff_buf_కూడు(win->w_బఫరు);
}

/*
 * Add a buffer to make diffs for.
 * Call this when a new buffer is being edited in the current window where
 * 'diff' is set.
 * Marks the current buffer as being part of the diff and requiring updating.
 * This must be done before any autocmd, because a command may use info
 * about the screen contents.
 */
    శూన్య
diff_buf_కూడు(buf_T *buf)
{
    పూర్ణాంకం		i;

    ఒకవేళ (diff_buf_idx(buf) != DB_లెక్క)
	తిరిగి;		// It's already there.

    కొరకు (i = 0; i < DB_లెక్క; ++i)
	ఒకవేళ (curtab->tp_diffbuf[i] == ఖాళీ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    తిరిగి;
	}

    semsg(_(e_cannot_diff_మరిన్ని_than_nr_buffers), DB_లెక్క);
}

/*
 * Remove all buffers to make diffs for.
 */
    స్థిర శూన్య
diff_buf_clear(శూన్య)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < DB_లెక్క; ++i)
	ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
	{
	    curtab->tp_diffbuf[i] = ఖాళీ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    స్థిర పూర్ణాంకం
diff_buf_idx(buf_T *buf)
{
    పూర్ణాంకం		idx;

    కొరకు (idx = 0; idx < DB_లెక్క; ++idx)
	ఒకవేళ (curtab->tp_diffbuf[idx] == buf)
	    ఆపు;
    తిరిగి idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    స్థిర పూర్ణాంకం
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    పూర్ణాంకం		idx;

    కొరకు (idx = 0; idx < DB_లెక్క; ++idx)
	ఒకవేళ (tp->tp_diffbuf[idx] == buf)
	    ఆపు;
    తిరిగి idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    శూన్య
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    పూర్ణాంకం		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	ఒకవేళ (i != DB_లెక్క)
	{
	    tp->tp_diff_invalid = TRUE;
	    ఒకవేళ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    శూన్య
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    పూర్ణాంకం		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	ఒకవేళ (idx != DB_లెక్క)
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
    స్థిర శూన్య
diff_mark_adjust_tp(
    tabpage_T	*tp,
    పూర్ణాంకం		idx,
    linenr_T	line1,
    linenr_T	line2,
    దీర్ఘ	amount,
    దీర్ఘ	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    పూర్ణాంకం		i;
    పూర్ణాంకం		inserted, deleted;
    పూర్ణాంకం		n, off;
    linenr_T	చివరి;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    పూర్ణాంకం		check_unchanged;

    diff_finger_dp = ఖాళీ;		// may shift df_lnum, invalidate the finger

    ఒకవేళ (diff_అంతర్గత())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    ఒకవేళ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    లేకపోతే ఒకవేళ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    లేకపోతే
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ఖాళీ;
    dp = tp->tp_మొదటి_diff;
    కొరకు (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	ఒకవేళ ((dp == ఖాళీ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ఖాళీ
		    || dprev->df_lnum[idx] + dprev->df_లెక్క[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    ఒకవేళ (dnext == ఖాళీ)
		తిరిగి;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_లెక్క[idx] = inserted;
	    కొరకు (i = 0; i < DB_లెక్క; ++i)
		ఒకవేళ (tp->tp_diffbuf[i] != ఖాళీ && i != idx)
		{
		    ఒకవేళ (dprev == ఖాళీ)
			dnext->df_lnum[i] = line1;
		    లేకపోతే
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_లెక్క[i])
			    - (dprev->df_lnum[idx] + dprev->df_లెక్క[idx]);
		    dnext->df_లెక్క[i] = deleted;
		}
	}

	// if at end of the list, quit
	ఒకవేళ (dp == ఖాళీ)
	    ఆపు;

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
	చివరి = dp->df_lnum[idx] + dp->df_లెక్క[idx] - 1;

	// 1. change completely above line1: nothing to do
	ఒకవేళ (చివరి >= line1 - 1)
	{
	    ఒకవేళ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		ఒకవేళ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    ఒకవేళ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		ఒకవేళ (amount_after == 0)
		    ఆపు;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    లేకపోతే
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		ఒకవేళ (deleted > 0)
		{
		    off = 0;
		    ఒకవేళ (dp->df_lnum[idx] >= line1)
		    {
			ఒకవేళ (చివరి <= line2)
			{
			    // 4. delete all lines of diff
			    ఒకవేళ (dp->df_తదుపరి != ఖాళీ
				    && dp->df_తదుపరి->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_తదుపరి->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_లెక్క[idx];
				lnum_deleted = dp->df_తదుపరి->df_lnum[idx];
			    }
			    లేకపోతే
				n = deleted - dp->df_లెక్క[idx];
			    dp->df_లెక్క[idx] = 0;
			}
			లేకపోతే
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_లెక్క[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (చివరి < line2)
			{
			    // 2. delete at end of diff
			    dp->df_లెక్క[idx] -= చివరి - lnum_deleted + 1;
			    ఒకవేళ (dp->df_తదుపరి != ఖాళీ
				    && dp->df_తదుపరి->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_తదుపరి->df_lnum[idx] - 1 - చివరి;
				deleted -= dp->df_తదుపరి->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_తదుపరి->df_lnum[idx];
			    }
			    లేకపోతే
				n = line2 - చివరి;
			    check_unchanged = TRUE;
			}
			లేకపోతే
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_లెక్క[idx] -= deleted;
			}
		    }

		    కొరకు (i = 0; i < DB_లెక్క; ++i)
			ఒకవేళ (tp->tp_diffbuf[i] != ఖాళీ && i != idx)
			{
			    ఒకవేళ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    లేకపోతే
				dp->df_lnum[i] = 1;
			    dp->df_లెక్క[i] += n;
			}
		}
		లేకపోతే
		{
		    ఒకవేళ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_లెక్క[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    లేకపోతే
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		ఒకవేళ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	ఒకవేళ (dprev != ఖాళీ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_లెక్క[idx]
							== dp->df_lnum[idx])
	{
	    కొరకు (i = 0; i < DB_లెక్క; ++i)
		ఒకవేళ (tp->tp_diffbuf[i] != ఖాళీ)
		    dprev->df_లెక్క[i] += dp->df_లెక్క[i];
	    dprev->df_తదుపరి = dp->df_తదుపరి;
	    clear_diffblock(dp);
	    dp = dprev->df_తదుపరి;
	}
	లేకపోతే
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_తదుపరి;
	}
    }

    dprev = ఖాళీ;
    dp = tp->tp_మొదటి_diff;
    అయితే (dp != ఖాళీ)
    {
	// All counts are zero, remove this entry.
	కొరకు (i = 0; i < DB_లెక్క; ++i)
	    ఒకవేళ (tp->tp_diffbuf[i] != ఖాళీ && dp->df_లెక్క[i] != 0)
		ఆపు;
	ఒకవేళ (i == DB_లెక్క)
	{
	    dnext = dp->df_తదుపరి;
	    clear_diffblock(dp);
	    dp = dnext;
	    ఒకవేళ (dprev == ఖాళీ)
		tp->tp_మొదటి_diff = dnext;
	    లేకపోతే
		dprev->df_తదుపరి = dnext;
	}
	లేకపోతే
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_తదుపరి;
	}

    }

    ఒకవేళ (tp == curtab)
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
    స్థిర diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    ఒకవేళ (dnew == ఖాళీ)
	తిరిగి ఖాళీ;
    diff_finger_dp = ఖాళీ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_తదుపరి = dp;
    ఒకవేళ (dprev == ఖాళీ)
	tp->tp_మొదటి_diff = dnew;
    లేకపోతే
	dprev->df_తదుపరి = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, పరిమాణం(diffline_change_T), 20);
    తిరిగి dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    స్థిర శూన్య
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    పూర్ణాంకం		i_org;
    పూర్ణాంకం		i_new;
    పూర్ణాంకం		off_org, off_new;
    char_u	*పంక్తి_org;
    పూర్ణాంకం		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    కొరకు (i_org = 0; i_org < DB_లెక్క; ++i_org)
	ఒకవేళ (tp->tp_diffbuf[i_org] != ఖాళీ)
	    ఆపు;
    ఒకవేళ (i_org == DB_లెక్క)	// safety check
	తిరిగి;

    ఒకవేళ (diff_check_sanity(tp, dp) == FAIL)
	తిరిగి;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    కొరకు (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	అయితే (dp->df_లెక్క[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    ఒకవేళ (dir == BACKWARD)
		off_org = dp->df_లెక్క[i_org] - 1;
	    పంక్తి_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    ఒకవేళ (పంక్తి_org == ఖాళీ)
		తిరిగి;
	    కొరకు (i_new = i_org + 1; i_new < DB_లెక్క; ++i_new)
	    {
		ఒకవేళ (tp->tp_diffbuf[i_new] == ఖాళీ)
		    కొనసాగు;
		ఒకవేళ (dir == BACKWARD)
		    off_new = dp->df_లెక్క[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		ఒకవేళ (off_new < 0 || off_new >= dp->df_లెక్క[i_new])
		    ఆపు;
		ఒకవేళ (diff_cmp(పంక్తి_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    ఆపు;
	    }
	    vim_free(పంక్తి_org);

	    // Stop when a line isn't equal in all diff buffers.
	    ఒకవేళ (i_new != DB_లెక్క)
		ఆపు;

	    // Line matched in all buffers, remove it from the diff.
	    కొరకు (i_new = i_org; i_new < DB_లెక్క; ++i_new)
		ఒకవేళ (tp->tp_diffbuf[i_new] != ఖాళీ)
		{
		    ఒకవేళ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_లెక్క[i_new];
		}
	}
	ఒకవేళ (dir == BACKWARD)
	    ఆపు;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    స్థిర పూర్ణాంకం
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < DB_లెక్క; ++i)
	ఒకవేళ (tp->tp_diffbuf[i] != ఖాళీ)
	    ఒకవేళ (dp->df_lnum[i] + dp->df_లెక్క[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_పంక్తి_లెక్క)
		తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    శూన్య
diff_redraw(
    పూర్ణాంకం		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ఖాళీ;
    పూర్ణాంకం		వాడిన_max_fill_other = FALSE;
    పూర్ణాంకం		వాడిన_max_fill_curwin = FALSE;
    పూర్ణాంకం		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	ఒకవేళ (!wp->w_p_diff || !buf_valid(wp->w_బఫరు))
	    కొనసాగు;

	redraw_win_later(wp, UPD_SOME_VALID);
	ఒకవేళ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	ఒకవేళ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	ఒకవేళ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    ఒకవేళ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    లేకపోతే ఒకవేళ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		ఒకవేళ (wp == curwin)
		    వాడిన_max_fill_curwin = TRUE;
		లేకపోతే ఒకవేళ (wp_other != ఖాళీ)
		    వాడిన_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    ఒకవేళ (wp_other != ఖాళీ && curwin->w_p_scb)
    {
	ఒకవేళ (వాడిన_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_సమితి_topline(wp_other, curwin);
	లేకపోతే ఒకవేళ (వాడిన_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_సమితి_topline(curwin, wp_other);
    }
}

    స్థిర శూన్య
clear_diffin(diffin_T *din)
{
    ఒకవేళ (din->din_fname == ఖాళీ)
	VIM_CLEAR(din->din_mmfile.ptr);
    లేకపోతే
	mch_remove(din->din_fname);
}

    స్థిర శూన్య
clear_diffout(diffout_T *dout)
{
    ఒకవేళ (dout->dout_fname == ఖాళీ)
	ga_clear_అక్షరశ్రేణులు(&dout->dout_ga);
    లేకపోతే
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    స్థిర పూర్ణాంకం
diff_write_బఫరు(buf_T *buf, diffin_T *din, linenr_T ప్రారంభం, linenr_T చివర)
{
    linenr_T	lnum;
    char_u	*s;
    దీర్ఘ	len = 0;
    char_u	*ptr;

    ఒకవేళ (చివర < 0)
	చివర = buf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (buf->b_ml.ml_flags & ML_ఖాళీలు || చివర < ప్రారంభం)
    {
	din->din_mmfile.ptr = ఖాళీ;
	din->din_mmfile.కొలత = 0;
	తిరిగి OK;
    }

    // xdiff requires one big block of memory with all the text.
    కొరకు (lnum = ప్రారంభం; lnum <= చివర; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    ఒకవేళ (ptr == ఖాళీ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	తిరిగి FAIL;
    }
    din->din_mmfile.ptr = (అక్షరం *)ptr;
    din->din_mmfile.కొలత = len;

    len = 0;
    కొరకు (lnum = ప్రారంభం; lnum <= చివర; ++lnum)
    {
	కొరకు (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    ఒకవేళ (diff_flags & DIFF_ICASE)
	    {
		పూర్ణాంకం c;
		పూర్ణాంకం	orig_len;
		పూర్ణాంకం	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		ఒకవేళ (*s == NL)
		    c = NUL;
		లేకపోతే
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		ఒకవేళ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		లేకపోతే
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    ఒకవేళ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    లేకపోతే
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    తిరిగి OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    స్థిర పూర్ణాంకం
diff_write(buf_T *buf, diffin_T *din, linenr_T ప్రారంభం, linenr_T చివర)
{
    పూర్ణాంకం		r;
    పూర్ణాంకం		save_ml_flags;
    char_u	*save_ff;
    పూర్ణాంకం		save_cmod_flags;

    ఒకవేళ (din->din_fname == ఖాళీ)
	తిరిగి diff_write_బఫరు(buf, din, ప్రారంభం, చివర);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    ఒకవేళ (చట్రాలు_locked())
	తిరిగి FAIL;

    ఒకవేళ (చివర < 0)
	చివర = buf->b_ml.ml_పంక్తి_లెక్క;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    ఒకవేళ (చివర < ప్రారంభం)
    {
	// The line range specifies a completely empty file.
	చివర = ప్రారంభం;
	buf->b_ml.ml_flags |= ML_ఖాళీలు;
    }
    r = buf_write(buf, din->din_fname, ఖాళీ,
			ప్రారంభం, చివర,
			ఖాళీ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_ఖాళీలు) | (save_ml_flags & ML_ఖాళీలు);
    తిరిగి r;
}

    స్థిర పూర్ణాంకం
lnum_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    ఒకవేళ (lnum1 < lnum2)
	తిరిగి -1;
    ఒకవేళ (lnum1 > lnum2)
	తిరిగి 1;
    తిరిగి 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    స్థిర శూన్య
diff_try_update(
	diffio_T    *dio,
	పూర్ణాంకం	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    పూర్ణాంకం		idx_new;

    ఒకవేళ (dio->dio_అంతర్గత)
    {
	ga_init2(&dio->dio_diff.dout_ga, పరిమాణం(అక్షరం *), 1000);
    }
    లేకపోతే
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	ఒకవేళ (dio->dio_orig.din_fname == ఖాళీ
		|| dio->dio_new.din_fname == ఖాళీ
		|| dio->dio_diff.dout_fname == ఖాళీ)
	    వెళ్ళు theend;
    }

    // Check external diff is actually working.
    ఒకవేళ (!dio->dio_అంతర్గత && check_external_diff(dio) == FAIL)
	వెళ్ళు theend;

    // :diffupdate!
    ఒకవేళ (eap != ఖాళీ && eap->forceit)
	కొరకు (idx_new = idx_orig; idx_new < DB_లెక్క; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ఒకవేళ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    పూర్ణాంకం అంకె_anchors = గరిష్ఠ_పూర్ణాంకం;
    linenr_T anchors[DB_లెక్క][MAX_DIFF_ANCHORS];
    CLEAR_క్షేత్రం(anchors);
    ఒకవేళ (diff_flags & DIFF_ANCHOR)
    {
	కొరకు (పూర్ణాంకం idx = 0; idx < DB_లెక్క; idx++)
	{
	    ఒకవేళ (curtab->tp_diffbuf[idx] == ఖాళీ)
		కొనసాగు;
	    పూర్ణాంకం buf_అంకె_anchors = 0;
	    ఒకవేళ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_అంకె_anchors) != OK)
	    {
		emsg(_(e_failed_గమ్యం_కనుగొను_all_diff_anchors));
		అంకె_anchors = 0;
		CLEAR_క్షేత్రం(anchors);
		ఆపు;
	    }
	    ఒకవేళ (buf_అంకె_anchors < అంకె_anchors)
		అంకె_anchors = buf_అంకె_anchors;

	    ఒకవేళ (buf_అంకె_anchors > 0)
		qsort((శూన్య *)anchors[idx],
			(size_t)buf_అంకె_anchors,
			పరిమాణం(linenr_T),
			lnum_compare);
	}
    }
    ఒకవేళ (అంకె_anchors == గరిష్ఠ_పూర్ణాంకం)
	అంకె_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    కొరకు (పూర్ణాంకం anchor_i = 0; anchor_i <= అంకె_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ఖాళీ;
	ఒకవేళ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_మొదటి_diff;
	    curtab->tp_మొదటి_diff = ఖాళీ;
	}
	linenr_T lnum_ప్రారంభం = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_చివర = (anchor_i == అంకె_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	ఒకవేళ (diff_write(buf, &dio->dio_orig, lnum_ప్రారంభం, lnum_చివర) == FAIL)
	{
	    ఒకవేళ (orig_diff != ఖాళీ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_మొదటి_diff = orig_diff;
		diff_clear(curtab);
	    }
	    వెళ్ళు theend;
	}

	// Make a difference between the first buffer and every other.
	కొరకు (idx_new = idx_orig + 1; idx_new < DB_లెక్క; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
		కొనసాగు; // skip buffer that isn't loaded

	    lnum_ప్రారంభం = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_చివర = anchor_i == అంకె_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    ఒకవేళ (diff_write(buf, &dio->dio_new, lnum_ప్రారంభం, lnum_చివర) == FAIL)
		కొనసాగు;
	    ఒకవేళ (diff_file(dio) == FAIL)
		కొనసాగు;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	ఒకవేళ (anchor_i != 0)
	{
	    diff_finger_dp = ఖాళీ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    కొరకు (diff_T *dp = curtab->tp_మొదటి_diff; dp != ఖాళీ; dp = dp->df_తదుపరి)
	    {
		కొరకు (పూర్ణాంకం idx = 0; idx < DB_లెక్క; idx++)
		{
		    ఒకవేళ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    ఒకవేళ (orig_diff != ఖాళీ)
	    {
		diff_T *చివరి_diff = orig_diff;
		అయితే (చివరి_diff->df_తదుపరి != ఖాళీ)
		    చివరి_diff = చివరి_diff->df_తదుపరి;
		చివరి_diff->df_తదుపరి = curtab->tp_మొదటి_diff;
		curtab->tp_మొదటి_diff = orig_diff;
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
    పూర్ణాంకం
diff_అంతర్గత(శూన్య)
{
    తిరిగి (diff_flags & DIFF_అంతర్గత) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    స్థిర పూర్ణాంకం
diff_అంతర్గత_failed(శూన్య)
{
    పూర్ణాంకం idx;

    // Only need to do something when there is another buffer.
    కొరకు (idx = 0; idx < DB_లెక్క; ++idx)
	ఒకవేళ (curtab->tp_diffbuf[idx] != ఖాళీ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    శూన్య
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    పూర్ణాంకం		idx_orig;
    పూర్ణాంకం		idx_new;
    diffio_T	diffio;
    పూర్ణాంకం		had_diffs = curtab->tp_మొదటి_diff != ఖాళీ;

    ఒకవేళ (diff_busy)
    {
	diff_need_update = TRUE;
	తిరిగి;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    కొరకు (idx_orig = 0; idx_orig < DB_లెక్క; ++idx_orig)
	ఒకవేళ (curtab->tp_diffbuf[idx_orig] != ఖాళీ)
	    ఆపు;
    ఒకవేళ (idx_orig == DB_లెక్క)
	వెళ్ళు theend;

    // Only need to do something when there is another buffer.
    కొరకు (idx_new = idx_orig + 1; idx_new < DB_లెక్క; ++idx_new)
	ఒకవేళ (curtab->tp_diffbuf[idx_new] != ఖాళీ)
	    ఆపు;
    ఒకవేళ (idx_new == DB_లెక్క)
	వెళ్ళు theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_క్షేత్రం(diffio);
    diffio.dio_అంతర్గత = diff_అంతర్గత() && !diff_అంతర్గత_failed();

    diff_try_update(&diffio, idx_orig, eap);
    ఒకవేళ (diffio.dio_అంతర్గత && diff_అంతర్గత_failed())
    {
	// Internal diff failed, use external diff instead.
	CLEAR_క్షేత్రం(diffio);
	diff_try_update(&diffio, idx_orig, eap);
    }

    // force updating cursor position on screen
    curwin->w_valid_cursor.lnum = 0;

theend:
    // A redraw is needed if there were diffs and they were cleared, or there
    // are diffs now, which means they got updated.
    ఒకవేళ (had_diffs || curtab->tp_మొదటి_diff != ఖాళీ)
    {
	diff_redraw(TRUE);
	కిటికీ_layout_తాళం();
	apply_autocmds(EVENT_DIFFUPDATED, ఖాళీ, ఖాళీ, FALSE, curbuf);
	కిటికీ_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    స్థిర పూర్ణాంకం
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    పూర్ణాంకం		ok;
    పూర్ణాంకం		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    కొరకు (;;)
    {
	ok = FALSE;
	fd = mch_fopen((అక్షరం *)diffio->dio_orig.din_fname, "w");
	ఒకవేళ (fd == ఖాళీ)
	    io_error = TRUE;
	లేకపోతే
	{
	    ఒకవేళ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((అక్షరం *)diffio->dio_new.din_fname, "w");
	    ఒకవేళ (fd == ఖాళీ)
		io_error = TRUE;
	    లేకపోతే
	    {
		ఒకవేళ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ఖాళీ;
		ఒకవేళ (diff_file(diffio) == OK)
		    fd = mch_fopen((అక్షరం *)diffio->dio_diff.dout_fname, "r");
		ఒకవేళ (fd == ఖాళీ)
		    io_error = TRUE;
		లేకపోతే
		{
		    char_u	linebuf[LBUFLEN];

		    కొరకు (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			ఒకవేళ (vim_fgets(linebuf, LBUFLEN, fd))
			    ఆపు;
			ఒకవేళ (STRNCMP(linebuf, "1c1", 3) == 0
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
	ఒకవేళ (*p_dex != NUL)
	    ఆపు;
# endif

# ఒకవేళ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	ఒకవేళ (ok && diff_a_పనిచేస్తుంది == MAYBE && diff_bin_పనిచేస్తుంది == MAYBE)
	{
	    diff_a_పనిచేస్తుంది = TRUE;
	    diff_bin_పనిచేస్తుంది = TRUE;
	    కొనసాగు;
	}
	ఒకవేళ (!ok && diff_a_పనిచేస్తుంది == TRUE && diff_bin_పనిచేస్తుంది == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_పనిచేస్తుంది = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	ఒకవేళ (diff_a_పనిచేస్తుంది != MAYBE)
	    ఆపు;
	diff_a_పనిచేస్తుంది = ok;

	// If "-a" works break here, otherwise retry without "-a".
	ఒకవేళ (ok)
	    ఆపు;
    }
    ఒకవేళ (!ok)
    {
	ఒకవేళ (io_error)
	    emsg(_(e_cannot_read_or_write_తాత్కాలిక_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_పనిచేస్తుంది = MAYBE;
# ఒకవేళ defined(MSWIN)
	diff_bin_పనిచేస్తుంది = MAYBE;
# endif
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Invoke the xdiff function.
 */
    స్థిర పూర్ణాంకం
diff_file_అంతర్గత(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_క్షేత్రం(param);
    CLEAR_క్షేత్రం(emit_cfg);
    CLEAR_క్షేత్రం(emit_cb);

    param.flags = diff_algorithm;

    ఒకవేళ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    ఒకవేళ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    ఒకవేళ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    ఒకవేళ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_పంక్తులు;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    ఒకవేళ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_బహిర్గతం_indices;
    లేకపోతే
	emit_cb.బహిర్గతం_పంక్తి = xdiff_బహిర్గతం_unified;
    ఒకవేళ (diffio->dio_orig.din_mmfile.కొలత > MAX_XDIFF_కొలత ||
	diffio->dio_new.din_mmfile.కొలత > MAX_XDIFF_కొలత)
    {
	emsg(_(e_problem_creating_అంతర్గత_diff));
	తిరిగి FAIL;
    }
    ఒకవేళ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_అంతర్గత_diff));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    స్థిర పూర్ణాంకం
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    ఒకవేళ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	తిరిగి OK;
    }
    లేకపోతే
# endif
    // Use xdiff for generating the diff.
    ఒకవేళ (dio->dio_అంతర్గత)
	తిరిగి diff_file_అంతర్గత(dio);

    ఒకవేళ (check_restricted())
	తిరిగి FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    ఒకవేళ (cmd == ఖాళీ)
	తిరిగి FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    ఒకవేళ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((అక్షరం *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_పనిచేస్తుంది == FALSE ? "" : "-a ",
# ఒకవేళ defined(MSWIN)
	    diff_bin_పనిచేస్తుంది == TRUE ? "--binary " : "",
# లేకపోతే
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (పూర్ణాంకం)len, p_srr, tmp_diff);
    ఖండం_autocmds();	// avoid ShellCmdPost stuff
    (శూన్య)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    తిరిగి OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    శూన్య
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ఖాళీ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ఖాళీ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ఖాళీ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ఖాళీ;
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ఖాళీ;

# ifdef FEAT_BROWSE
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ఖాళీ, ఖాళీ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ఖాళీ);
	ఒకవేళ (browseFile == ఖాళీ)
	    తిరిగి;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    ఒకవేళ (tmp_orig == ఖాళీ || tmp_new == ఖాళీ)
	వెళ్ళు theend;

    // Write the current buffer to "tmp_orig".
    ఒకవేళ (buf_write(curbuf, tmp_orig, ఖాళీ,
		(linenr_T)1, curbuf->b_ml.ml_పంక్తి_లెక్క,
				     ఖాళీ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	వెళ్ళు theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = నిండిన_Name_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ఖాళీ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    ఒకవేళ (esc_name == ఖాళీ)
	వెళ్ళు theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    ఒకవేళ (buf == ఖాళీ)
	వెళ్ళు theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    ఒకవేళ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((అక్షరం *)dirbuf) != 0)
	dirbuf[0] = NUL;
    లేకపోతే
    {
#  ifdef TEMPDIRNAMES
	ఒకవేళ (vim_tempdir != ఖాళీ)
	    vim_ignored = mch_chdir((అక్షరం *)vim_tempdir);
	లేకపోతే
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    ఒకవేళ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ఖాళీ ? fullname :
#  endif
		eap->arg, tmp_new);
    లేకపోతే
# endif
    {
	ఒకవేళ (check_restricted())
	    వెళ్ళు theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((అక్షరం *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	ఖండం_autocmds();	// Avoid ShellCmdPost stuff
	(శూన్య)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    ఒకవేళ (dirbuf[0] != NUL)
    {
	ఒకవేళ (mch_chdir((అక్షరం *)dirbuf) != 0)
	    emsg(_(e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory));
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
    ఒకవేళ (mch_stat((అక్షరం *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    లేకపోతే
    {
	ఒకవేళ (curbuf->b_fname != ఖాళీ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    ఒకవేళ (newname != ఖాళీ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	ఒకవేళ (win_విభజించు(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_విభజించు;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    ఒకవేళ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		ఒకవేళ (newname != ఖాళీ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    ఒకవేళ (au_has_సమూహం((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    ఒకవేళ (tmp_orig != ఖాళీ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    ఒకవేళ (tmp_new != ఖాళీ)
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
    శూన్య
ex_diffsplit(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    bufref_T	old_curbuf;

    సమితి_bufref(&old_curbuf, curbuf);
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif
    // Need to compute w_fraction when no redraw happened yet.
    validate_cursor();
    సమితి_fraction(curwin);

    // don't use a new tab page, each tab page has its own diffs
    cmdmod.cmod_tab = 0;

    ఒకవేళ (win_విభజించు(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	తిరిగి;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_విభజించు;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    ఒకవేళ (curwin == old_curwin)		// split didn't work
	తిరిగి;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    ఒకవేళ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	ఒకవేళ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_పంక్తి(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_గమ్యం_fraction(curwin, curwin->w_ఎత్తు);
}

/*
 * Set options to show diffs for the current window.
 */
    శూన్య
ex_diffthis(exarg_T *eap వాడని)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    స్థిర శూన్య
సమితి_diff_option(win_T *wp, పూర్ణాంకం విలువ)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_బఫరు;
    ++curbuf_తాళం;
    సమితి_option_విలువ_give_err((char_u *)"diff", (దీర్ఘ)విలువ, ఖాళీ, OPT_LOCAL);
    --curbuf_తాళం;
    curwin = old_curwin;
    curbuf = curwin->w_బఫరు;
}

/*
 * Set options in window "wp" for diff mode.
 */
    శూన్య
diff_win_options(
    win_T	*wp,
    పూర్ణాంకం		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    ఒకవేళ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    ఒకవేళ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    ఒకవేళ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	ఒకవేళ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    ఒకవేళ (!wp->w_p_diff)
    {
	ఒకవేళ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    సమితి_string_option_ప్రత్యక్ష_లోపల_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    ఒకవేళ (!wp->w_p_diff)
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
    మారింది_కిటికీ_setting_win(wp);
# endif
    ఒకవేళ (vim_strchr(p_sbo, 'h') == ఖాళీ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    సమితి_diff_option(wp, TRUE);

    ఒకవేళ (addbuf)
	diff_buf_కూడు(wp->w_బఫరు);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    శూన్య
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    పూర్ణాంకం		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    సమితి_diff_option(wp, FALSE);

	    ఒకవేళ (wp->w_p_diff_saved)
	    {

		ఒకవేళ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		ఒకవేళ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		ఒకవేళ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    ఒకవేళ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		ఒకవేళ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		ఒకవేళ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		ఒకవేళ (wp->w_p_fen)
		    wp->w_p_fen = foldmethodIsManual(wp) ? FALSE
							 : wp->w_p_fen_save;

		foldUpdateAll(wp);
# endif
	    }
	    // remove filler lines
	    wp->w_topfill = 0;

	    // make sure topline is not halfway a fold and cursor is
	    // invalidated
	    మారింది_కిటికీ_setting_win(wp);

	    // Note: 'sbo' is not restored, it's a global option.
	    diff_buf_adjust(wp);
	}
	diffwin |= wp->w_p_diff;
    }

    // Also remove hidden buffers from the list.
    ఒకవేళ (eap->forceit)
	diff_buf_clear();

    ఒకవేళ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    ఒకవేళ (!diffwin && vim_strchr(p_sbo, 'h') != ఖాళీ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    స్థిర శూన్య
diff_read(
    పూర్ణాంకం		idx_orig,	// idx of original file
    పూర్ణాంకం		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ఖాళీ;
    పూర్ణాంకం		పంక్తి_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ఖాళీ;
    diff_T	*dp = curtab->tp_మొదటి_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*పంక్తి;
    దీర్ఘ	off;
    పూర్ణాంకం		i;
    పూర్ణాంకం		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ఖాళీ;	    // init to avoid gcc warning

    గణన {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    ఒకవేళ (dout->dout_fname == ఖాళీ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    లేకపోతే
    {
	fd = mch_fopen((అక్షరం *)dout->dout_fname, "r");
	ఒకవేళ (fd == ఖాళీ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    తిరిగి;
	}
    }

    ఒకవేళ (!dio->dio_అంతర్గత)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	ఒకవేళ (hunk == ఖాళీ)
	{
	    ఒకవేళ (fd != ఖాళీ)
		fclose(fd);
	    తిరిగి;
	}
    }

    కొరకు (;;)
    {
	ఒకవేళ (dio->dio_అంతర్గత)
	{
	    ఒకవేళ (పంక్తి_hunk_idx >= dout->dout_ga.ga_len)
		ఆపు;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_దత్తాంశం)[పంక్తి_hunk_idx++];
	}
	లేకపోతే
	{
	    ఒకవేళ (fd == ఖాళీ)
	    {
		ఒకవేళ (పంక్తి_hunk_idx >= dout->dout_ga.ga_len)
		    ఆపు;	    // did last line
		పంక్తి = ((char_u **)dout->dout_ga.ga_దత్తాంశం)[పంక్తి_hunk_idx++];
	    }
	    లేకపోతే
	    {
		ఒకవేళ (vim_fgets(linebuf, LBUFLEN, fd))
		    ఆపు;		// end of file
		పంక్తి = linebuf;
	    }

	    ఒకవేళ (diffstyle == DIFF_NONE)
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
		ఒకవేళ (సురక్షిత_isdigit(*పంక్తి))
		    diffstyle = DIFF_ED;
		లేకపోతే ఒకవేళ ((STRNCMP(పంక్తి, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		లేకపోతే ఒకవేళ ((STRNCMP(పంక్తి, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(పంక్తి, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(పంక్తి, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		లేకపోతే
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    కొనసాగు;
	    }

	    ఒకవేళ (diffstyle == DIFF_ED)
	    {
		ఒకవేళ (!సురక్షిత_isdigit(*పంక్తి))
		    కొనసాగు;	// not the start of a diff block
		ఒకవేళ (parse_diff_ed(పంక్తి, hunk) == FAIL)
		    కొనసాగు;
	    }
	    లేకపోతే ఒకవేళ (diffstyle == DIFF_UNIFIED)
	    {
		ఒకవేళ (STRNCMP(పంక్తి, "@@ ", 3)  != 0)
		    కొనసాగు;	// not the start of a diff block
		ఒకవేళ (parse_diff_unified(పంక్తి, hunk) == FAIL)
		    కొనసాగు;
	    }
	    లేకపోతే
	    {
		emsg(_(e_invalid_diff_format));
		ఆపు;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	అయితే (dp != ఖాళీ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_లెక్క[idx_orig])
	{
	    ఒకవేళ (notset)
		diff_copy_నమోదు(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_తదుపరి;
	    notset = TRUE;
	}

	ఒకవేళ (dp != ఖాళీ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_లెక్క[idx_orig]
		&& hunk->lnum_orig + hunk->లెక్క_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    కొరకు (dpl = dp; dpl->df_తదుపరి != ఖాళీ; dpl = dpl->df_తదుపరి)
		ఒకవేళ (hunk->lnum_orig + hunk->లెక్క_orig
					     < dpl->df_తదుపరి->df_lnum[idx_orig])
		    ఆపు;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    ఒకవేళ (off > 0)
	    {
		కొరకు (i = idx_orig; i < idx_new; ++i)
		    ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_లెక్క[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_లెక్క[idx_new] = hunk->లెక్క_new;
	    }
	    లేకపోతే ఒకవేళ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_లెక్క[idx_new] = hunk->లెక్క_new - off;
	    }
	    లేకపోతే
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		పూర్ణాంకం orig_కొలత_లోపల_dp = MIN(hunk->లెక్క_orig,
			dp->df_lnum[idx_orig] +
			dp->df_లెక్క[idx_orig] - hunk->lnum_orig);
		పూర్ణాంకం కొలత_diff = hunk->లెక్క_new - orig_కొలత_లోపల_dp;
		dp->df_లెక్క[idx_new] += కొలత_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->లెక్క_new
		    - (dp->df_lnum[idx_new] + dp->df_లెక్క[idx_new]);
		ఒకవేళ (off > 0)
		    dp->df_లెక్క[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->లెక్క_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_లెక్క[idx_orig]);
	    ఒకవేళ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		ఒకవేళ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_లెక్క[idx_new] += -off;
		}
		off = 0;
	    }
	    కొరకు (i = idx_orig; i < idx_new; ++i)
		ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
		    dp->df_లెక్క[i] = dpl->df_lnum[i] + dpl->df_లెక్క[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_తదుపరి;
	    dp->df_తదుపరి = dpl->df_తదుపరి;
	    అయితే (dn != dp->df_తదుపరి)
	    {
		dpl = dn->df_తదుపరి;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	లేకపోతే
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    ఒకవేళ (dp == ఖాళీ)
		వెళ్ళు పూర్తి;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_లెక్క[idx_orig] = hunk->లెక్క_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_లెక్క[idx_new] = hunk->లెక్క_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    కొరకు (i = idx_orig + 1; i < idx_new; ++i)
		ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
		    diff_copy_నమోదు(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    అయితే (dp != ఖాళీ)
    {
	ఒకవేళ (notset)
	    diff_copy_నమోదు(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_తదుపరి;
	notset = TRUE;
    }

పూర్తి:
    ఒకవేళ (!dio->dio_అంతర్గత)
	vim_free(hunk);

    ఒకవేళ (fd != ఖాళీ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    స్థిర శూన్య
diff_copy_నమోదు(
    diff_T	*dprev,
    diff_T	*dp,
    పూర్ణాంకం		idx_orig,
    పూర్ణాంకం		idx_new)
{
    దీర్ఘ	off;

    ఒకవేళ (dprev == ఖాళీ)
	off = 0;
    లేకపోతే
	off = (dprev->df_lnum[idx_orig] + dprev->df_లెక్క[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_లెక్క[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_లెక్క[idx_new] = dp->df_లెక్క[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    శూన్య
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *తదుపరి_p;

    కొరకు (p = tp->tp_మొదటి_diff; p != ఖాళీ; p = తదుపరి_p)
    {
	తదుపరి_p = p->df_తదుపరి;
	clear_diffblock(p);
    }
    tp->tp_మొదటి_diff = ఖాళీ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    స్థిర పూర్ణాంకం
diff_linematch(diff_T *dp)
{
    ఒకవేళ (!(diff_flags & DIFF_LINEMATCH))
	తిరిగి 0;

    // are there more than three diff buffers?
    పూర్ణాంకం tsize = 0;
    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
    {
	ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    ఒకవేళ (dp->df_లెక్క[i] < 0)
		తిరిగి FALSE;
	    tsize += dp->df_లెక్క[i];
	}
    }

    // avoid allocating a huge array because it will lag
    తిరిగి tsize <= linematch_పంక్తులు;
}

    స్థిర పూర్ణాంకం
get_max_diff_నిడివి(స్థిరరాశి diff_T *dp)
{
    పూర్ణాంకం maxlength = 0;

    కొరకు (పూర్ణాంకం k = 0; k < DB_లెక్క; k++)
    {
	ఒకవేళ (curtab->tp_diffbuf[k] != ఖాళీ)
	{
	    ఒకవేళ (dp->df_లెక్క[k] > maxlength)
		maxlength = dp->df_లెక్క[k];
	}
    }
    తిరిగి maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    స్థిర శూన్య
కనుగొను_శిఖరం_diff_ఖండం(
    diff_T	**thistopdiff,
    diff_T	**తదుపరి_adjacent_ఖండాలు,
    పూర్ణాంకం		fromidx,
    పూర్ణాంకం		topline)
{
    diff_T	*topdiff = ఖాళీ;
    diff_T	*localtopdiff = ఖాళీ;
    పూర్ణాంకం		topdiffchange = 0;

    కొరకు (topdiff = curtab->tp_మొదటి_diff; topdiff != ఖాళీ;
						topdiff = topdiff->df_తదుపరి)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	ఒకవేళ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	ఒకవేళ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_లెక్క[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    ఒకవేళ ((*thistopdiff) == ఖాళీ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	ఒకవేళ (!(topdiff->df_తదుపరి && (topdiff->df_తదుపరి->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_లెక్క[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    ఒకవేళ (*thistopdiff)
	    {
		(*తదుపరి_adjacent_ఖండాలు) = topdiff->df_తదుపరి;
		ఆపు;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    స్థిర శూన్య
calculate_topfill_and_topline(
    స్థిరరాశి పూర్ణాంకం	fromidx,
    స్థిరరాశి పూర్ణాంకం	toidx,
    స్థిరరాశి పూర్ణాంకం	from_topline,
    స్థిరరాశి పూర్ణాంకం	from_topfill,
    పూర్ణాంకం		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ఖాళీ;
    diff_T	*తదుపరి_adjacent_ఖండాలు = ఖాళీ;
    పూర్ణాంకం		virtual_పంక్తులు_passed = 0;
    పూర్ణాంకం		curlinenum_గమ్యం = 1;

    కనుగొను_శిఖరం_diff_ఖండం(&thistopdiff, &తదుపరి_adjacent_ఖండాలు, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    అయితే (curdif && (curdif->df_lnum[fromidx] + curdif->df_లెక్క[fromidx])
							<= from_topline)
    {
	virtual_పంక్తులు_passed += get_max_diff_నిడివి(curdif);

	curdif = curdif->df_తదుపరి;
    }

    ఒకవేళ (curdif != తదుపరి_adjacent_ఖండాలు)
	virtual_పంక్తులు_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_పంక్తులు_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    ఒకవేళ (virtual_పంక్తులు_passed < 0)
	virtual_పంక్తులు_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    ఒకవేళ (thistopdiff != ఖాళీ) // this should not be null, but just for safety
	curlinenum_గమ్యం = thistopdiff->df_lnum[toidx];

    పూర్ణాంకం virt_పంక్తులు_ఎడమ = virtual_పంక్తులు_passed;
    curdif = thistopdiff;
    అయితే (virt_పంక్తులు_ఎడమ > 0 && curdif != ఖాళీ && curdif != తదుపరి_adjacent_ఖండాలు)
    {
	curlinenum_గమ్యం += MIN(virt_పంక్తులు_ఎడమ, curdif->df_లెక్క[toidx]);
	virt_పంక్తులు_ఎడమ -= MIN(virt_పంక్తులు_ఎడమ, get_max_diff_నిడివి(curdif));
	curdif = curdif->df_తదుపరి;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    పూర్ణాంకం max_virt_పంక్తులు = 0;
    కొరకు (diff_T *dp = thistopdiff; dp != ఖాళీ; dp = dp->df_తదుపరి)
    {
	ఒకవేళ (dp->df_lnum[toidx] + dp->df_లెక్క[toidx] <= curlinenum_గమ్యం)
	    max_virt_పంక్తులు += get_max_diff_నిడివి(dp);
	లేకపోతే
	{
	    ఒకవేళ (dp->df_lnum[toidx] <= curlinenum_గమ్యం)
		max_virt_పంక్తులు += curlinenum_గమ్యం - dp->df_lnum[toidx];
	    ఆపు;
	}
    }

    ఒకవేళ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_పంక్తులు - virtual_పంక్తులు_passed;
    (*topline) = curlinenum_గమ్యం;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    స్థిర శూన్య
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_నిడివి,
    స్థిరరాశి పూర్ణాంకం	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    పూర్ణాంకం		పంక్తి_numbers[DB_లెక్క];
    పూర్ణాంకం		outputmap[DB_లెక్క];
    size_t	ndiffs = 0;

    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
    {
	ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
	{
	    పంక్తి_numbers[i] = dp->df_lnum[i];
	    dp->df_లెక్క[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    కొరకు (size_t i = 0; i < decisions_నిడివి; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	ఒకవేళ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_తదుపరి);
	    dp_s->is_linematched = TRUE;
	    కొరకు (పూర్ణాంకం j = 0; j < DB_లెక్క; j++)
	    {
		ఒకవేళ (curtab->tp_diffbuf[j] != ఖాళీ)
		{
		    dp_s->df_lnum[j] = పంక్తి_numbers[j];
		    dp_s->df_లెక్క[j] = 0;
		}
	    }
	}
	కొరకు (size_t j = 0; j < ndiffs; j++)
	{
	    ఒకవేళ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_లెక్క[outputmap[j]]++;
		పంక్తి_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    స్థిర శూన్య
నిరంతరం_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_లెక్క];
    స్థిరరాశి mmfile_t	*diffbufs[DB_లెక్క];
    పూర్ణాంకం			diff_నిడివి[DB_లెక్క];
    size_t		ndiffs = 0;

    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
    {
	ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    ఒకవేళ (dp->df_లెక్క[i] > 0)
	    {
		diff_write_బఫరు(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_లెక్క[i] - 1);
	    }
	    లేకపోతే
	    {
		diffbufs_mm[ndiffs].din_mmfile.కొలత = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ఖాళీ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_నిడివి[ndiffs] = dp->df_లెక్క[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    పూర్ణాంకం *decisions = ఖాళీ;
    స్థిరరాశి పూర్ణాంకం iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_నిడివి =
	linematch_nbuffers(diffbufs, diff_నిడివి, ndiffs, &decisions, iwhite);

    కొరకు (size_t i = 0; i < ndiffs; i++)
	విడుదల(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_నిడివి, decisions);

    విడుదల(decisions);
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
    పూర్ణాంకం
diff_check_with_linestatus(win_T *wp, linenr_T lnum, పూర్ణాంకం *linestatus)
{
    పూర్ణాంకం		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    పూర్ణాంకం		maxcount;
    పూర్ణాంకం		i;
    buf_T	*buf = wp->w_బఫరు;
    పూర్ణాంకం		cmp;

    ఒకవేళ (linestatus != ఖాళీ)
	*linestatus = 0;

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    ఒకవేళ (curtab->tp_మొదటి_diff == ఖాళీ || !wp->w_p_diff)	// no diffs at all
	తిరిగి 0;

    // safety check: "lnum" must be a buffer line
    ఒకవేళ (lnum < 1 || lnum > buf->b_ml.ml_పంక్తి_లెక్క + 1)
	తిరిగి 0;

    idx = diff_buf_idx(buf);
    ఒకవేళ (idx == DB_లెక్క)
	తిరిగి 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    ఒకవేళ (hasFoldingWin(wp, lnum, ఖాళీ, ఖాళీ, TRUE, ఖాళీ))
	తిరిగి 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ఖాళీ;		// last block with df_lnum[idx] <= lnum
    కొరకు (dp = diff_పరిశీలన_ప్రారంభం(curtab, idx, lnum); dp != ఖాళీ; dp = dp->df_తదుపరి)
    {
	ఒకవేళ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	ఒకవేళ (lnum <= dp->df_lnum[idx] + dp->df_లెక్క[idx])
	    ఆపు;
    }
    diff_పరిశీలన_remember(curtab, idx, lnum, le);
    ఒకవేళ (dp == ఖాళీ || lnum < dp->df_lnum[idx])
	తిరిగి 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    ఒకవేళ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	నిరంతరం_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    పూర్ణాంకం అంకె_fill = 0;
    అయితే (lnum == dp->df_lnum[idx] + dp->df_లెక్క[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	ఒకవేళ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_నిడివి(dp);
	    అంకె_fill += maxcount - dp->df_లెక్క[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	ఒకవేళ (dp->df_తదుపరి != ఖాళీ
		&& lnum >= dp->df_తదుపరి->df_lnum[idx]
		&& lnum <= dp->df_తదుపరి->df_lnum[idx] + dp->df_తదుపరి->df_లెక్క[idx])
	    dp = dp->df_తదుపరి;
	లేకపోతే
	    ఆపు;
    }

    ఒకవేళ (lnum < dp->df_lnum[idx] + dp->df_లెక్క[idx])
    {
	పూర్ణాంకం	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	కొరకు (i = 0; i < DB_లెక్క; ++i)
	    ఒకవేళ (i != idx && curtab->tp_diffbuf[i] != ఖాళీ)
	    {
		ఒకవేళ (dp->df_లెక్క[i] == 0)
		    zero = TRUE;
		లేకపోతే
		{
		    ఒకవేళ (dp->df_లెక్క[i] != dp->df_లెక్క[idx])
		    {
			ఒకవేళ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			తిరిగి అంకె_fill;
		    }
		    cmp = TRUE;
		}
	    }
	ఒకవేళ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    కొరకు (i = 0; i < DB_లెక్క; ++i)
		ఒకవేళ (i != idx && curtab->tp_diffbuf[i] != ఖాళీ
						      && dp->df_లెక్క[i] != 0)
		    ఒకవేళ (!diff_equal_నమోదు(dp, idx, i))
		    {
			ఒకవేళ (linestatus)
			    *linestatus = -1;
			తిరిగి అంకె_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	ఒకవేళ (zero == FALSE)
	    తిరిగి అంకె_fill;
	ఒకవేళ (linestatus)
	    *linestatus = -2;
	తిరిగి అంకె_fill;
    }
    తిరిగి అంకె_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    స్థిర పూర్ణాంకం
diff_equal_నమోదు(diff_T *dp, పూర్ణాంకం idx1, పూర్ణాంకం idx2)
{
    పూర్ణాంకం		i;
    char_u	*పంక్తి;
    పూర్ణాంకం		cmp;

    ఒకవేళ (dp->df_లెక్క[idx1] != dp->df_లెక్క[idx2])
	తిరిగి FALSE;
    ఒకవేళ (diff_check_sanity(curtab, dp) == FAIL)
	తిరిగి FALSE;
    కొరకు (i = 0; i < dp->df_లెక్క[idx1]; ++i)
    {
	పంక్తి = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	ఒకవేళ (పంక్తి == ఖాళీ)
	    తిరిగి FALSE;
	cmp = diff_cmp(పంక్తి, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(పంక్తి);
	ఒకవేళ (cmp != 0)
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    స్థిర పూర్ణాంకం
diff_equal_char(char_u *p1, char_u *p2, పూర్ణాంకం *len)
{
    పూర్ణాంకం l  = (*mb_ptr2len)(p1);

    ఒకవేళ (l != (*mb_ptr2len)(p2))
	తిరిగి FALSE;
    ఒకవేళ (l > 1)
    {
	ఒకవేళ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    తిరిగి FALSE;
	*len = l;
    }
    లేకపోతే
    {
	ఒకవేళ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    తిరిగి FALSE;
	*len = 1;
    }
    తిరిగి TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    స్థిర పూర్ణాంకం
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    పూర్ణాంకం		l;

    ఒకవేళ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	తిరిగి 0;

    ఒకవేళ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	తిరిగి STRCMP(s1, s2);
    ఒకవేళ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	తిరిగి MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    అయితే (*p1 != NUL && *p2 != NUL)
    {
	ఒకవేళ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	లేకపోతే
	{
	    ఒకవేళ (!diff_equal_char(p1, p2, &l))
		ఆపు;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    ఒకవేళ (*p1 != NUL || *p2 != NUL)
	తిరిగి 1;
    తిరిగి 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    పూర్ణాంకం
diff_check_fill(win_T *wp, linenr_T lnum)
{
    పూర్ణాంకం		n;

    // be quick when there are no filler lines
    ఒకవేళ (!(diff_flags & DIFF_FILLER))
	తిరిగి 0;
    n = diff_check_with_linestatus(wp, lnum, ఖాళీ);
    ఒకవేళ (n <= 0)
	తిరిగి 0;
    తిరిగి n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    శూన్య
diff_సమితి_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_బఫరు;
    linenr_T	lnum;
    పూర్ణాంకం		fromidx;
    పూర్ణాంకం		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    ఒకవేళ (fromidx == DB_లెక్క)
	తిరిగి;		// safety check

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
	ఒకవేళ (lnum <= dp->df_lnum[fromidx] + dp->df_లెక్క[fromidx])
	    ఆపు;
    ఒకవేళ (dp == ఖాళీ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క
				       - (frombuf->b_ml.ml_పంక్తి_లెక్క - lnum);
    }
    లేకపోతే
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_బఫరు);
	ఒకవేళ (toidx == DB_లెక్క)
	    తిరిగి;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	ఒకవేళ (lnum >= dp->df_lnum[fromidx])
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
    ఒకవేళ (towin->w_topline > towin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	towin->w_topline = towin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	towin->w_botfill = true;
    }
    ఒకవేళ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    మారింది_పంక్తి_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (శూన్య)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ఖాళీ, TRUE, ఖాళీ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    స్థిర పూర్ణాంకం
parse_diffanchors(
    పూర్ణాంకం		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    పూర్ణాంకం		*అంకె_anchors)
{
    పూర్ణాంకం i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ఖాళీ;
    ఒకవేళ (check_only)
	bufwin = curwin;
    లేకపోతే
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    ఒకవేళ (bufwin->w_బఫరు == buf && bufwin->w_p_diff)
		ఆపు;
	ఒకవేళ (bufwin == ఖాళీ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    తిరిగి FAIL;
	}
    }

    కొరకు (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	ఒకవేళ (*dia == ',') // don't allow empty values
	    తిరిగి FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_చిరునామా(ఖాళీ, &dia, ADDR_పంక్తులు, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	ఒకవేళ (dia == ఖాళీ) // error detected
	    తిరిగి FAIL;
	ఒకవేళ (*dia != ',' && *dia != NUL)
	    తిరిగి FAIL;

	ఒకవేళ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_పంక్తి_లెక్క + 1))
	{
	    emsg(_(e_invalid_range));
	    తిరిగి FAIL;
	}

	ఒకవేళ (anchors != ఖాళీ)
	    anchors[i] = lnum;

	ఒకవేళ (*dia == ',')
	    dia++;
    }
    ఒకవేళ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_మరిన్ని_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	తిరిగి FAIL;
    }
    ఒకవేళ (అంకె_anchors != ఖాళీ)
	*అంకె_anchors = i;
    తిరిగి OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    పూర్ణాంకం
diffanchors_మారింది(పూర్ణాంకం buflocal)
{
    పూర్ణాంకం ఫలితం = parse_diffanchors(TRUE, curbuf, ఖాళీ, ఖాళీ);
    ఒకవేళ (ఫలితం == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    ఒకవేళ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    లేకపోతే
	    {
		కొరకు (పూర్ణాంకం idx = 0; idx < DB_లెక్క; ++idx)
		    ఒకవేళ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			ఆపు;
		    }
	    }
	}
    }
    తిరిగి ఫలితం;
}

/*
 * This is called when 'diffopt' is changed.
 */
    పూర్ణాంకం
diffopt_మారింది(శూన్య)
{
    char_u	*p;
    పూర్ణాంకం		diff_context_new = 6;
    పూర్ణాంకం		linematch_పంక్తులు_new = 0;
    పూర్ణాంకం		diff_flags_new = 0;
    పూర్ణాంకం		diff_foldcolumn_new = 2;
    దీర్ఘ	diff_algorithm_new = 0;
    దీర్ఘ	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    అయితే (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	ఒకవేళ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_అంతర్గత;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    ఒకవేళ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_కనీస;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    లేకపోతే
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    ఒకవేళ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    లేకపోతే ఒకవేళ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_మాట;
	    }
	    లేకపోతే
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_పంక్తులు_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    ఒకవేళ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	తిరిగి FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    ఒకవేళ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_పంక్తులు = linematch_పంక్తులు_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    తిరిగి OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    పూర్ణాంకం
diffopt_horizontal(శూన్య)
{
    తిరిగి (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    పూర్ణాంకం
diffopt_hiddenoff(శూన్య)
{
    తిరిగి (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    పూర్ణాంకం
diffopt_closeoff(శూన్య)
{
    తిరిగి (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    శూన్య
diff_update_పంక్తి(linenr_T lnum)
{
    పూర్ణాంకం		idx;
    diff_T	*dp;

    ఒకవేళ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	తిరిగి;

    idx = diff_buf_idx(curbuf);
    ఒకవేళ (idx == DB_లెక్క)
	తిరిగి;
    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
	ఒకవేళ (lnum <= dp->df_lnum[idx] + dp->df_లెక్క[idx])
	    ఆపు;

    // clear the inline change cache as it's invalid
    ఒకవేళ (dp != ఖాళీ)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

స్థిర diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    పూర్ణాంకం
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    పూర్ణాంకం *change_ప్రారంభం,
    పూర్ణాంకం *change_చివర)
{
    ఒకవేళ (change->dc_ప్రారంభం_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_ప్రారంభం = 0;
    లేకపోతే
	*change_ప్రారంభం = change->dc_ప్రారంభం[diffline->bufidx];
    ఒకవేళ (change->dc_చివర_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_చివర = గరిష్ఠ_పూర్ణాంకం;
    లేకపోతే
	*change_చివర = change->dc_చివర[diffline->bufidx];

    ఒకవేళ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	తిరిగి FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
    {
	ఒకవేళ (i == diffline->bufidx)
	    కొనసాగు;
	ఒకవేళ (change->dc_ప్రారంభం[i] != change->dc_చివర[i]
		|| change->dc_చివర_lnum_off[i] != change->dc_ప్రారంభం_lnum_off[i])
	{
	    తిరిగి FALSE;
	}
    }
    తిరిగి TRUE;
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
    స్థిర పూర్ణాంకం
diff_కనుగొను_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		*startp,	// first char of the change
    పూర్ణాంకం		*endp)		// last char of the change
{
    char_u	*పంక్తి_org;
    char_u	*పంక్తి_new;
    పూర్ణాంకం		i;
    పూర్ణాంకం		si_org, si_new;
    పూర్ణాంకం		ei_org, ei_new;
    పూర్ణాంకం		off;
    పూర్ణాంకం		added = TRUE;
    char_u	*p1, *p2;
    పూర్ణాంకం		l;

    ఒకవేళ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	పంక్తి_org = ఖాళీ;
    }
    లేకపోతే
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	పంక్తి_org = vim_strsave(ml_get_buf(wp->w_బఫరు, lnum, FALSE));
	ఒకవేళ (పంక్తి_org == ఖాళీ)
	    తిరిగి FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    కొరకు (i = 0; i < DB_లెక్క; ++i)
	ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    ఒకవేళ (off >= dp->df_లెక్క[i])
		కొనసాగు;
	    added = FALSE;
	    ఒకవేళ (diff_flags & DIFF_INLINE_NONE)
		ఆపు; // early terminate as we only care about the return value

	    పంక్తి_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    అయితే (పంక్తి_org[si_org] != NUL)
	    {
		ఒకవేళ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(పంక్తి_org[si_org])
					      && VIM_ISWHITE(పంక్తి_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(పంక్తి_org[si_org])
					    || VIM_ISWHITE(పంక్తి_new[si_new]))))
		{
		    si_org = (పూర్ణాంకం)(skipwhite(పంక్తి_org + si_org) - పంక్తి_org);
		    si_new = (పూర్ణాంకం)(skipwhite(పంక్తి_new + si_new) - పంక్తి_new);
		}
		లేకపోతే
		{
		    ఒకవేళ (!diff_equal_char(పంక్తి_org + si_org, పంక్తి_new + si_new,
									   &l))
			ఆపు;
		    si_org += l;
		    si_new += l;
		}
	    }
	    ఒకవేళ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_తల_off)(పంక్తి_org, పంక్తి_org + si_org);
		si_new -= (*mb_తల_off)(పంక్తి_new, పంక్తి_new + si_new);
	    }
	    ఒకవేళ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    ఒకవేళ (పంక్తి_org[si_org] != NUL || పంక్తి_new[si_new] != NUL)
	    {
		ei_org = (పూర్ణాంకం)STRLEN(పంక్తి_org);
		ei_new = (పూర్ణాంకం)STRLEN(పంక్తి_new);
		అయితే (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    ఒకవేళ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(పంక్తి_org[ei_org])
					      && VIM_ISWHITE(పంక్తి_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(పంక్తి_org[ei_org])
					    || VIM_ISWHITE(పంక్తి_new[ei_new]))))
		    {
			అయితే (ei_org >= *startp
					     && VIM_ISWHITE(పంక్తి_org[ei_org]))
			    --ei_org;
			అయితే (ei_new >= si_new
					     && VIM_ISWHITE(పంక్తి_new[ei_new]))
			    --ei_new;
		    }
		    లేకపోతే
		    {
			p1 = పంక్తి_org + ei_org;
			p2 = పంక్తి_new + ei_new;
			p1 -= (*mb_తల_off)(పంక్తి_org, p1);
			p2 -= (*mb_తల_off)(పంక్తి_new, p2);
			ఒకవేళ (!diff_equal_char(p1, p2, &l))
			    ఆపు;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		ఒకవేళ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(పంక్తి_org);
    తిరిగి added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
రకనిర్వచనం నిర్మాణం
{
    దీర్ఘ byte_ప్రారంభం;
    దీర్ఘ అంకె_బైట్లు;
    పూర్ణాంకం lineoff;
} linemap_నమోదు_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    స్థిర శూన్య
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, పూర్ణాంకం idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    పూర్ణాంకం pass = 1;
    చేయి
    {
	పూర్ణాంకం has_unmerged_gaps = FALSE;
	పూర్ణాంకం has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	అయితే (dp!= ఖాళీ && dp->df_తదుపరి != ఖాళీ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    ఒకవేళ (dp->df_lnum[idx1] + dp->df_లెక్క[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_తదుపరి->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_నమోదు_T *entry1 = &((linemap_నమోదు_T*)linemap[idx1].ga_దత్తాంశం)[dp->df_lnum[idx1] + dp->df_లెక్క[idx1] - 1];
	    linemap_నమోదు_T *entry2 = &((linemap_నమోదు_T*)linemap[idx1].ga_దత్తాంశం)[dp->df_తదుపరి->df_lnum[idx1] - 1];
	    ఒకవేళ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    linenr_T gap = dp->df_తదుపరి->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_లెక్క[idx1]);
	    ఒకవేళ (gap <= 3)
	    {
		linenr_T max_df_లెక్క = 0;
		కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
		    max_df_లెక్క = MAX(max_df_లెక్క, dp->df_లెక్క[i] + dp->df_తదుపరి->df_లెక్క[i]);

		ఒకవేళ (max_df_లెక్క >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
		    {
			dp->df_లెక్క[i] = dp->df_తదుపరి->df_lnum[i]
			    + dp->df_తదుపరి->df_లెక్క[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_తదుపరి = dp->df_తదుపరి;
		    dp->df_తదుపరి = dp_తదుపరి->df_తదుపరి;
		    clear_diffblock(dp_తదుపరి);
		    has_merged_gaps = TRUE;
		    కొనసాగు;
		}
		లేకపోతే
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_తదుపరి;
	}
	ఒకవేళ (!has_unmerged_gaps || !has_merged_gaps)
	    ఆపు;
    } అయితే (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    స్థిర శూన్య
diff_refine_inline_మాట_highlight(diff_T *dp_orig, garray_T *linemap, పూర్ణాంకం idx1,
				  linenr_T ప్రారంభం_lnum)
{
    పూర్ణాంకం pass = 1;
    చేయి
    {
	diff_T *dp = dp_orig;

	అయితే (dp != ఖాళీ && dp->df_తదుపరి != ఖాళీ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_లెక్క[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_తదుపరి->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    ఒకవేళ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    linemap_నమోదు_T *entry1 =
		&((linemap_నమోదు_T *)linemap[idx1].ga_దత్తాంశం)[idx_entry1];
	    linemap_నమోదు_T *entry2 =
		&((linemap_నమోదు_T *)linemap[idx1].ga_దత్తాంశం)[idx_entry2];

	    // Skip if blocks are on different lines
	    ఒకవేళ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    // Calculate the gap between blocks
	    పూర్ణాంకం gap_ప్రారంభం = entry1->byte_ప్రారంభం + entry1->అంకె_బైట్లు;
	    పూర్ణాంకం gap_చివర = entry2->byte_ప్రారంభం;
	    పూర్ణాంకం gap_కొలత = gap_చివర - gap_ప్రారంభం;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    ఒకవేళ (gap_కొలత <= 0 || gap_కొలత > diff_మాట_gap)
	    {
		dp = dp->df_తదుపరి;
		కొనసాగు;
	    }

	    // Get the text between the two blocks
	    char_u *పంక్తి = ml_get_buf(curtab->tp_diffbuf[idx1],
		    ప్రారంభం_lnum + entry1->lineoff, FALSE);
	    char_u *gap_పాఠ్యం = పంక్తి + gap_ప్రారంభం;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_మాట = true;
	    bool has_content = false;

	    కొరకు (పూర్ణాంకం i = 0; i < gap_కొలత && gap_పాఠ్యం[i] != NUL; i++)
	    {
		has_content = true;
		పూర్ణాంకం char_class = mb_get_class_buf(gap_పాఠ్యం + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		ఒకవేళ (char_class == 2)
		{
		    only_non_మాట = false;
		    ఆపు;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    ఒకవేళ (has_content && only_non_మాట)
	    {
		దీర్ఘ మొత్తం_change_బైట్లు = 0;
		కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
		{
		    ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
		    {
			// count bytes in the first block
			కొరకు (పూర్ణాంకం k = 0; k < dp->df_లెక్క[i]; k++)
			{
			    పూర్ణాంకం idx = dp->df_lnum[i] + k - 1;
			    ఒకవేళ (idx < linemap[i].ga_len)
				మొత్తం_change_బైట్లు +=
				    ((linemap_నమోదు_T *)linemap[i].ga_దత్తాంశం)[idx].అంకె_బైట్లు;
			}
			// count bytes in the next block
			కొరకు (పూర్ణాంకం k = 0; k < dp->df_తదుపరి->df_లెక్క[i]; k++)
			{
			    పూర్ణాంకం idx = dp->df_తదుపరి->df_lnum[i] + k - 1;
			    ఒకవేళ (idx < linemap[i].ga_len)
				మొత్తం_change_బైట్లు +=
				    ((linemap_నమోదు_T *)linemap[i].ga_దత్తాంశం)[idx].అంకె_బైట్లు;
			}
		    }
		}

		ఒకవేళ (మొత్తం_change_బైట్లు >= gap_కొలత * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
		    {
			ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
			{
			    dp->df_లెక్క[i] = dp->df_తదుపరి->df_lnum[i]
				+ dp->df_తదుపరి->df_లెక్క[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_తదుపరి = dp->df_తదుపరి;
		    dp->df_తదుపరి = dp_తదుపరి->df_తదుపరి;
		    clear_diffblock(dp_తదుపరి);

		    // Don't advance dp, check if can merge with the next block too
		    కొనసాగు;
		}
	    }

	    dp = dp->df_తదుపరి;
	}
    } అయితే (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    స్థిర శూన్య
diff_కనుగొను_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_లెక్క];
    garray_T	file1_str;
    garray_T	file2_str;
    పూర్ణాంకం		file1_idx = -1;

    దీర్ఘ	save_diff_algorithm = diff_algorithm;

    CLEAR_క్షేత్రం(dio);
    ga_init2(&dio.dio_diff.dout_ga, పరిమాణం(అక్షరం *), 1000);

    // inline diff only supports internal algo
    dio.dio_అంతర్గత = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_మొదటి_diff;
    curtab->tp_మొదటి_diff = ఖాళీ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    //బఫరు
    buf_T	*(orig_diffbuf[DB_లెక్క]);
    జ్ఞాపకం_నకలు(orig_diffbuf, curtab->tp_diffbuf, పరిమాణం(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
	ga_init2(&linemap[i], పరిమాణం(linemap_నమోదు_T), 128);

    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
	    కొనసాగు; // skip buffer that isn't loaded

	ఒకవేళ (dp->df_లెక్క[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ఖాళీ;
	    కొనసాగు;
	}

	ఒకవేళ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	కొరకు (పూర్ణాంకం off = 0; off < dp->df_లెక్క[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    పూర్ణాంకం లోపల_keyword = FALSE;

	    // iwhiteeol support vars
	    పూర్ణాంకం చివరి_white = FALSE;
	    పూర్ణాంకం eol_ga_len = -1;
	    పూర్ణాంకం eol_linemap_len = -1;
	    పూర్ణాంకం eol_numlines = -1;

	    char_u *s;
	    కొరకు (s = curline; *s != NUL;)
	    {
		పూర్ణాంకం new_లోపల_keyword = FALSE;
		ఒకవేళ (diff_flags & DIFF_INLINE_మాట)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_లోపల_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		ఒకవేళ (లోపల_keyword && !new_లోపల_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ఒకవేళ (VIM_ISWHITE(*s))
		{
		    ఒకవేళ (diff_flags & DIFF_IWHITEALL)
		    {
			లోపల_keyword = FALSE;
			s = skipwhite(s);
			కొనసాగు;
		    }
		    లేకపోతే ఒకవేళ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			ఒకవేళ (!చివరి_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    చివరి_white = TRUE;
			}
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			చివరి_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		పూర్ణాంకం char_len = 1;
		ఒకవేళ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		లేకపోతే
		{
		    char_len = mb_ptr2len(s);

		    ఒకవేళ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    ఒకవేళ (diff_flags & DIFF_ICASE)
		    {
			పూర్ణాంకం c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			పూర్ణాంకం c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			పూర్ణాంకం c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			ఒకవేళ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    లేకపోతే
			ga_concat_len(curstr, s, char_len);
		}

		ఒకవేళ (!new_లోపల_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		ఒకవేళ (!new_లోపల_keyword || (new_లోపల_keyword && !లోపల_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_నమోదు_T linemap_నమోదు;
		    linemap_నమోదు.lineoff = off;
		    linemap_నమోదు.byte_ప్రారంభం = s - curline;
		    linemap_నమోదు.అంకె_బైట్లు = char_len;
		    ఒకవేళ (ga_grow(&linemap[i], 1) != OK)
			వెళ్ళు పూర్తి;
		    ((linemap_నమోదు_T*)(linemap[i].ga_దత్తాంశం))[linemap[i].ga_len]
			= linemap_నమోదు;
		    linemap[i].ga_len += 1;
		}
		లేకపోతే
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[linemap[i].ga_len-1].అంకె_బైట్లు
			+= char_len;
		}

		లోపల_keyword = new_లోపల_keyword;
		s += char_len;
	    }
	    ఒకవేళ (లోపల_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    ఒకవేళ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		ఒకవేళ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    ఒకవేళ (!(diff_flags & DIFF_IWHITEALL))
	    {
		// Add an empty line token mapped to the end-of-line in the
		// original file. This helps diff newline differences among
		// files, which will be visualized when using 'list' as the eol
		// listchar will be highlighted.
		ga_append(curstr, NL);
		numlines++;

		linemap_నమోదు_T linemap_నమోదు;
		linemap_నమోదు.lineoff = off;
		linemap_నమోదు.byte_ప్రారంభం = s - curline;
		linemap_నమోదు.అంకె_బైట్లు = పరిమాణం(NL);
		ఒకవేళ (ga_grow(&linemap[i], 1) != OK)
		    వెళ్ళు పూర్తి;
		((linemap_నమోదు_T*)(linemap[i].ga_దత్తాంశం))[linemap[i].ga_len]
		    = linemap_నమోదు;
		linemap[i].ga_len += 1;
	    }
	}

	ఒకవేళ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (అక్షరం *)curstr->ga_దత్తాంశం;
	    dio.dio_new.din_mmfile.కొలత = curstr->ga_len;
	}
	లేకపోతే
	{
	    dio.dio_orig.din_mmfile.ptr = (అక్షరం *)curstr->ga_దత్తాంశం;
	    dio.dio_orig.din_mmfile.కొలత = curstr->ga_len;
	}
	ఒకవేళ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    పూర్ణాంకం diff_స్థితి = diff_file_అంతర్గత(&dio);
	    ఒకవేళ (diff_స్థితి == FAIL)
		వెళ్ళు పూర్తి;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_మొదటి_diff;

    ఒకవేళ (diff_flags & DIFF_INLINE_మాట && file1_idx != -1)
	diff_refine_inline_మాట_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    లేకపోతే ఒకవేళ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    కొరకు (; new_diff != ఖాళీ; new_diff = new_diff->df_తదుపరి)
    {
	diffline_change_T change;
	CLEAR_క్షేత్రం(change);
	కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
	{
	    ఒకవేళ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		కొనసాగు;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_చివర = diff_lnum + new_diff->df_లెక్క[i];

	    ఒకవేళ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_ప్రారంభం[i] = MAXCOL;
		change.dc_ప్రారంభం_lnum_off[i] = గరిష్ఠ_పూర్ణాంకం;
	    }
	    లేకపోతే
	    {
		change.dc_ప్రారంభం[i] = ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[diff_lnum].byte_ప్రారంభం;
		change.dc_ప్రారంభం_lnum_off[i] = ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[diff_lnum].lineoff;
	    }

	    ఒకవేళ (diff_lnum == diff_lnum_చివర)
	    {
		change.dc_చివర[i] = change.dc_ప్రారంభం[i];
		change.dc_చివర_lnum_off[i] = change.dc_ప్రారంభం_lnum_off[i];
	    }
	    లేకపోతే ఒకవేళ (diff_lnum_చివర - 1 >= linemap[i].ga_len)
	    {
		change.dc_చివర[i] = MAXCOL;
		change.dc_చివర_lnum_off[i] = గరిష్ఠ_పూర్ణాంకం;
	    }
	    లేకపోతే
	    {
		change.dc_చివర[i] = ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[diff_lnum_చివర-1].byte_ప్రారంభం +
		    ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[diff_lnum_చివర-1].అంకె_బైట్లు;
		change.dc_చివర_lnum_off[i] = ((linemap_నమోదు_T*)linemap[i].ga_దత్తాంశం)[diff_lnum_చివర-1].lineoff;
	    }
	}
	ఒకవేళ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    వెళ్ళు పూర్తి;
	}
	((diffline_change_T*)(dp->df_changes.ga_దత్తాంశం))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

పూర్తి:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_మొదటి_diff = orig_diff;
    జ్ఞాపకం_నకలు(curtab->tp_diffbuf, orig_diffbuf, పరిమాణం(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    పూర్ణాంకం
diff_కనుగొను_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		off;

    idx = diff_buf_idx(wp->w_బఫరు);
    ఒకవేళ (idx == DB_లెక్క)	// cannot happen
	తిరిగి FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
	ఒకవేళ (lnum < dp->df_lnum[idx] + dp->df_లెక్క[idx])
	    ఆపు;
    ఒకవేళ (dp == ఖాళీ || diff_check_sanity(curtab, dp) == FAIL)
	తిరిగి FALSE;

    ఒకవేళ (lnum - dp->df_lnum[idx] > గరిష్ఠ_పూర్ణాంకం)
	// Integer overflow protection
	తిరిగి FALSE;
    off = lnum - dp->df_lnum[idx];

    ఒకవేళ (!(diff_flags & ALL_INLINE_DIFF) || diff_అంతర్గత_failed())
    {
	// Use simple algorithm
	పూర్ణాంకం	change_ప్రారంభం = MAXCOL;	// first col of changed area
	పూర్ణాంకం	change_చివర = -1;	// last col of changed area
	పూర్ణాంకం	ret;

	ret = diff_కనుగొను_change_simple(wp, lnum, dp, idx, &change_ప్రారంభం, &change_చివర);

	// convert from inclusive end to exclusive end per diffline's contract
	change_చివర += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_క్షేత్రం(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->అంకె_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_ప్రారంభం[idx] = change_ప్రారంభం;
	simple_diffline_change.dc_చివర[idx] = change_చివర;
	simple_diffline_change.dc_ప్రారంభం_lnum_off[idx] = off;
	simple_diffline_change.dc_చివర_lnum_off[idx] = off;
	తిరిగి ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    ఒకవేళ (!dp->has_changes)
	diff_కనుగొను_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    పూర్ణాంకం అంకె_changes = 0;
    పూర్ణాంకం change_idx = 0;
    diffline->changes = ఖాళీ;
    కొరకు (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_దత్తాంశం)[change_idx];
	ఒకవేళ (change->dc_చివర_lnum_off[idx] < off)
	    కొనసాగు;
	ఒకవేళ (change->dc_ప్రారంభం_lnum_off[idx] > off)
	    ఆపు;
	ఒకవేళ (diffline->changes == ఖాళీ)
	    diffline->changes = change;
	అంకె_changes++;
    }
    diffline->అంకె_changes = అంకె_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    పూర్ణాంకం added = FALSE;
    ఒకవేళ (అంకె_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	కొరకు (పూర్ణాంకం i = 0; i < DB_లెక్క; i++)
	{
	    ఒకవేళ (idx == i)
		కొనసాగు;
	    ఒకవేళ (curtab->tp_diffbuf[i] == ఖాళీ)
		కొనసాగు;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_దత్తాంశం)[dp->df_changes.ga_len-1];
	    ఒకవేళ (change->dc_ప్రారంభం_lnum_off[i] != గరిష్ఠ_పూర్ణాంకం)
	    {
		added = FALSE;
		ఆపు;
	    }
	}
    }
    తిరిగి added;
}

# ఒకవేళ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    పూర్ణాంకం
diff_infold(win_T *wp, linenr_T lnum)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		idx = -1;
    పూర్ణాంకం		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    ఒకవేళ (!wp->w_p_diff)
	తిరిగి FALSE;

    కొరకు (i = 0; i < DB_లెక్క; ++i)
    {
	ఒకవేళ (curtab->tp_diffbuf[i] == wp->w_బఫరు)
	    idx = i;
	లేకపోతే ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    ఒకవేళ (idx == -1 || !other)
	తిరిగి FALSE;

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    ఒకవేళ (curtab->tp_మొదటి_diff == ఖాళీ)
	తిరిగి TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ఖాళీ;		// last block with df_lnum[idx] <= lnum
    పూర్ణాంకం		ఫలితం = TRUE;
    కొరకు (dp = diff_పరిశీలన_ప్రారంభం(curtab, idx, lnum); dp != ఖాళీ; dp = dp->df_తదుపరి)
    {
	ఒకవేళ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	ఒకవేళ (dp->df_lnum[idx] - diff_context > lnum)
	    ఆపు;
	// If this change ends before the line we have a match.
	ఒకవేళ (dp->df_lnum[idx] + dp->df_లెక్క[idx] + diff_context > lnum)
	{
	    ఫలితం = FALSE;
	    ఆపు;
	}
    }
    diff_పరిశీలన_remember(curtab, idx, lnum, le);
    తిరిగి ఫలితం;
}
# endif

/*
 * "dp" and "do" commands.
 */
    శూన్య
nv_diffgetput(పూర్ణాంకం put, దీర్ఘ లెక్క)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_పని_CHANNEL
    ఒకవేళ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	తిరిగి;
    }
# endif
    ఒకవేళ (లెక్క == 0)
	ea.arg = (char_u *)"";
    లేకపోతే
    {
	vim_snprintf((అక్షరం *)buf, 30, "%ld", లెక్క);
	ea.arg = buf;
    }
    ఒకవేళ (put)
	ea.cmdidx = CMD_diffput;
    లేకపోతే
	ea.cmdidx = CMD_diffget;
    ea.addr_లెక్క = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    స్థిర పూర్ణాంకం
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
	ఒకవేళ (dp == diff)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    శూన్య
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    పూర్ణాంకం		లెక్క;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    పూర్ణాంకం		idx_cur;
    పూర్ణాంకం		idx_other;
    పూర్ణాంకం		idx_from;
    పూర్ణాంకం		idx_గమ్యం;
    పూర్ణాంకం		i;
    పూర్ణాంకం		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    పూర్ణాంకం		ప్రారంభం_skip, చివర_skip;
    పూర్ణాంకం		new_లెక్క;
    పూర్ణాంకం		buf_ఖాళీలు;
    పూర్ణాంకం		దొరికింది_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    ఒకవేళ (idx_cur == DB_లెక్క)
    {
	emsg(_(e_current_బఫరు_is_not_లోపల_diff_బహుళకం));
	తిరిగి;
    }

    ఒకవేళ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	కొరకు (idx_other = 0; idx_other < DB_లెక్క; ++idx_other)
	    ఒకవేళ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ఖాళీ)
	    {
		ఒకవేళ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    ఆపు;
		దొరికింది_not_ma = TRUE;
	    }
	ఒకవేళ (idx_other == DB_లెక్క)
	{
	    ఒకవేళ (దొరికింది_not_ma)
		emsg(_(e_no_other_బఫరు_లోపల_diff_బహుళకం_is_modifiable));
	    లేకపోతే
		emsg(_(e_no_other_బఫరు_లోపల_diff_బహుళకం));
	    తిరిగి;
	}

	// Check that there isn't a third buffer in the list
	కొరకు (i = idx_other + 1; i < DB_లెక్క; ++i)
	    ఒకవేళ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ఖాళీ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_మరిన్ని_than_two_buffers_లోపల_diff_బహుళకం_dont_know_which_one_గమ్యం_use));
		తిరిగి;
	    }
    }
    లేకపోతే
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	అయితే (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	కొరకు (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	ఒకవేళ (eap->arg + i == p)	    // digits only
	    i = atol((అక్షరం *)eap->arg);
	లేకపోతే
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    ఒకవేళ (i < 0)
		తిరిగి;		// error message already given
	}
	buf = buflist_findnr(i);
	ఒకవేళ (buf == ఖాళీ)
	{
	    semsg(_(e_cant_కనుగొను_బఫరు_str), eap->arg);
	    తిరిగి;
	}
	ఒకవేళ (buf == curbuf)
	    తిరిగి;		// nothing to do
	idx_other = diff_buf_idx(buf);
	ఒకవేళ (idx_other == DB_లెక్క)
	{
	    semsg(_(e_బఫరు_str_is_not_లోపల_diff_బహుళకం), eap->arg);
	    తిరిగి;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    ఒకవేళ (eap->addr_లెక్క == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	పూర్ణాంకం linestatus = 0;
	ఒకవేళ (eap->line1 == curbuf->b_ml.ml_పంక్తి_లెక్క
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	లేకపోతే ఒకవేళ (eap->line1 > 0)
	    --eap->line1;
    }

    ఒకవేళ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_గమ్యం = idx_cur;
    }
    లేకపోతే
    {
	idx_from = idx_cur;
	idx_గమ్యం = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	ఒకవేళ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    వెళ్ళు theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    ఒకవేళ (!curbuf->b_మారింది)
    {
	change_warning(0);
	ఒకవేళ (diff_buf_idx(curbuf) != idx_గమ్యం)
	{
	    emsg(_(e_బఫరు_మారింది_unexpectedly));
	    వెళ్ళు theend;
	}
    }

    dprev = ఖాళీ;
    కొరకు (dp = curtab->tp_మొదటి_diff; dp != ఖాళీ; )
    {
	ఒకవేళ (!eap->addr_లెక్క)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    అయితే (dp->df_తదుపరి
		    && dp->df_తదుపరి->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_లెక్క[idx_cur]
		    && dp->df_తదుపరి->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_తదుపరి;
	    }
	}
	ఒకవేళ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    ఆపు;	// past the range that was specified

	dfree = ఖాళీ;
	lnum = dp->df_lnum[idx_గమ్యం];
	లెక్క = dp->df_లెక్క[idx_గమ్యం];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + లెక్క + (లెక్క == 0 && BUFEMPTY() ? 1 : 0);

	ఒకవేళ (dp->df_lnum[idx_cur] + dp->df_లెక్క[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    ప్రారంభం_skip = 0;
	    చివర_skip = 0;
	    ఒకవేళ (eap->addr_లెక్క > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		ప్రారంభం_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		ఒకవేళ (ప్రారంభం_skip > 0)
		{
		    // range starts below start of current diff block
		    ఒకవేళ (ప్రారంభం_skip > లెక్క)
		    {
			lnum += లెక్క;
			లెక్క = 0;
		    }
		    లేకపోతే
		    {
			లెక్క -= ప్రారంభం_skip;
			lnum += ప్రారంభం_skip;
		    }
		}
		లేకపోతే
		    ప్రారంభం_skip = 0;

		చివర_skip = dp->df_lnum[idx_cur] + dp->df_లెక్క[idx_cur] - 1
							 - (eap->line2 + off);
		ఒకవేళ (చివర_skip > 0)
		{
		    // range ends above end of current/from diff block
		    ఒకవేళ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_లెక్క[idx_cur] - ప్రారంభం_skip - చివర_skip;
			ఒకవేళ (లెక్క > i)
			    లెక్క = i;
		    }
		    లేకపోతే			// :diffget
		    {
			లెక్క -= చివర_skip;
			చివర_skip = dp->df_లెక్క[idx_from] - ప్రారంభం_skip - లెక్క;
			ఒకవేళ (చివర_skip < 0)
			    చివర_skip = 0;
		    }
		}
		లేకపోతే
		    చివర_skip = 0;
	    }

	    buf_ఖాళీలు = BUFEMPTY();
	    added = 0;
	    కొరకు (i = 0; i < లెక్క; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_ఖాళీలు = curbuf->b_ml.ml_పంక్తి_లెక్క == 1;
		ఒకవేళ (ml_delete(lnum) == OK)
		    --added;
	    }
	    కొరకు (i = 0; i < dp->df_లెక్క[idx_from] - ప్రారంభం_skip - చివర_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + ప్రారంభం_skip + i;
		ఒకవేళ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_పంక్తి_లెక్క)
		    ఆపు;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		ఒకవేళ (p != ఖాళీ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    ఒకవేళ (buf_ఖాళీలు && curbuf->b_ml.ml_పంక్తి_లెక్క == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_ఖాళీలు = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_లెక్క = dp->df_లెక్క[idx_గమ్యం] + added;
	    dp->df_లెక్క[idx_గమ్యం] = new_లెక్క;

	    ఒకవేళ (ప్రారంభం_skip == 0 && చివర_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		కొరకు (i = 0; i < DB_లెక్క; ++i)
		    ఒకవేళ (curtab->tp_diffbuf[i] != ఖాళీ && i != idx_from
								&& i != idx_గమ్యం
			    && !diff_equal_నమోదు(dp, idx_from, i))
			ఆపు;
		ఒకవేళ (i == DB_లెక్క)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_తదుపరి;
		    ఒకవేళ (dprev == ఖాళీ)
			curtab->tp_మొదటి_diff = dp;
		    లేకపోతే
			dprev->df_తదుపరి = dp;
		}
	    }

	    ఒకవేళ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + లెక్క - 1, (దీర్ఘ)MAXLNUM, (దీర్ఘ)added);
		ఒకవేళ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    ఒకవేళ (curwin->w_cursor.lnum >= lnum + లెక్క)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_పంక్తి_లెక్క);
		    }
		    లేకపోతే ఒకవేళ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    మారింది_పంక్తులు(lnum, 0, lnum + లెక్క, (దీర్ఘ)added);

	    ఒకవేళ (dfree != ఖాళీ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_గమ్యం);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    ఒకవేళ (added != 0 && !valid_diff(dp))
		ఆపు;

	    ఒకవేళ (dfree == ఖాళీ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_లెక్క[idx_గమ్యం] = new_లెక్క;

	    // When changing the current buffer, keep track of line numbers
	    ఒకవేళ (idx_cur == idx_గమ్యం)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	ఒకవేళ (dfree == ఖాళీ)
	{
	    dprev = dp;
	    dp = dp->df_తదుపరి;
	}
    }

    // restore curwin/curbuf and a few other things
    ఒకవేళ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	ఒకవేళ (కీలకం_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    ఒకవేళ (diff_need_update)
	ex_diffupdate(ఖాళీ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    మారింది_పంక్తి_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    ఒకవేళ (curtab->tp_మొదటి_diff == ఖాళీ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_లోపల_TAB(curtab, wp)
	    ఒకవేళ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    ఒకవేళ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    లేకపోతే
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	కిటికీ_layout_తాళం();
	apply_autocmds(EVENT_DIFFUPDATED, ఖాళీ, ఖాళీ, FALSE, curbuf);
	కిటికీ_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    స్థిర శూన్య
diff_fold_update(diff_T *dp, పూర్ణాంకం skip_idx)
{
    పూర్ణాంకం		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	కొరకు (i = 0; i < DB_లెక్క; ++i)
	    ఒకవేళ (curtab->tp_diffbuf[i] == wp->w_బఫరు && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_లెక్క[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    పూర్ణాంకం
diff_బహుళకం_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ఒకవేళ (diff_buf_idx_tp(buf, tp) != DB_లెక్క)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    పూర్ణాంకం
diff_move_గమ్యం(పూర్ణాంకం dir, దీర్ఘ లెక్క)
{
    పూర్ణాంకం		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    ఒకవేళ (idx == DB_లెక్క || curtab->tp_మొదటి_diff == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    ఒకవేళ (curtab->tp_మొదటి_diff == ఖాళీ)		// no diffs today
	తిరిగి FAIL;

    అయితే (--లెక్క >= 0)
    {
	// Check if already before first diff.
	ఒకవేళ (dir == BACKWARD && lnum <= curtab->tp_మొదటి_diff->df_lnum[idx])
	    ఆపు;

	కొరకు (dp = curtab->tp_మొదటి_diff; ; dp = dp->df_తదుపరి)
	{
	    ఒకవేళ (dp == ఖాళీ)
		ఆపు;
	    ఒకవేళ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_తదుపరి == ఖాళీ
			    || lnum <= dp->df_తదుపరి->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		ఆపు;
	    }
	}
    }

    // don't end up past the end of the file
    ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;

    // When the cursor didn't move at all we fail.
    ఒకవేళ (lnum == curwin->w_cursor.lnum)
	తిరిగి FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.నిలువు = 0;

    తిరిగి OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    స్థిర linenr_T
diff_get_corresponding_పంక్తి_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    పూర్ణాంకం		idx1;
    పూర్ణాంకం		idx2;
    diff_T	*dp;
    పూర్ణాంకం		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    ఒకవేళ (idx1 == DB_లెక్క || idx2 == DB_లెక్క || curtab->tp_మొదటి_diff == ఖాళీ)
	తిరిగి lnum1;

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    ఒకవేళ (curtab->tp_మొదటి_diff == ఖాళీ)		// no diffs today
	తిరిగి lnum1;

    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
    {
	ఒకవేళ (dp->df_lnum[idx1] > lnum1)
	    తిరిగి lnum1 - baseline;
	ఒకవేళ ((dp->df_lnum[idx1] + dp->df_లెక్క[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    ఒకవేళ (baseline > dp->df_లెక్క[idx2])
		baseline = dp->df_లెక్క[idx2];

	    తిరిగి dp->df_lnum[idx2] + baseline;
	}
	ఒకవేళ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_లెక్క[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_లెక్క[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    తిరిగి curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_లెక్క[idx1])
				   - (dp->df_lnum[idx2] + dp->df_లెక్క[idx2]);
    }

    // If we get here then the cursor is after the last diff
    తిరిగి lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_పంక్తి(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_పంక్తి_int(buf1, lnum1);

    // don't end up past the end of the file
    ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి curbuf->b_ml.ml_పంక్తి_లెక్క;
    తిరిగి lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    ఒకవేళ (idx == DB_లెక్క)		// safety check
	తిరిగి (linenr_T)0;

    ఒకవేళ (curtab->tp_diff_invalid)
	ex_diffupdate(ఖాళీ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_లోపల_TAB(curtab, dp)
	ఒకవేళ (lnum <= dp->df_lnum[idx] + dp->df_లెక్క[idx])
	    ఆపు;

    // When after the last change, compute relative to the last line number.
    ఒకవేళ (dp == ఖాళీ)
	తిరిగి wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క
					- (curbuf->b_ml.ml_పంక్తి_లెక్క - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_బఫరు);
    ఒకవేళ (i == DB_లెక్క)			// safety check
	తిరిగి (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    ఒకవేళ (n > dp->df_lnum[i] + dp->df_లెక్క[i])
	n = dp->df_lnum[i] + dp->df_లెక్క[i];
    తిరిగి n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    స్థిర పూర్ణాంకం
parse_diff_ed(
	char_u	    *పంక్తి,
	diffhunk_T  *hunk)
{
    char_u *p;
    దీర్ఘ    f1, l1, f2, l2;
    పూర్ణాంకం	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = పంక్తి;
    f1 = getdigits(&p);
    ఒకవేళ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    లేకపోతే
	l1 = f1;
    ఒకవేళ (*p != 'a' && *p != 'c' && *p != 'd')
	తిరిగి FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    ఒకవేళ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    లేకపోతే
	l2 = f2;
    ఒకవేళ (l1 < f1 || l2 < f2)
	తిరిగి FAIL;

    ఒకవేళ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->లెక్క_orig = 0;
    }
    లేకపోతే
    {
	hunk->lnum_orig = f1;
	hunk->లెక్క_orig = l1 - f1 + 1;
    }
    ఒకవేళ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->లెక్క_new = 0;
    }
    లేకపోతే
    {
	hunk->lnum_new = f2;
	hunk->లెక్క_new = l2 - f2 + 1;
    }
    తిరిగి OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    స్థిర పూర్ణాంకం
parse_diff_unified(
	char_u	    *పంక్తి,
	diffhunk_T  *hunk)
{
    char_u *p;
    దీర్ఘ    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = పంక్తి;
    ఒకవేళ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	ఒకవేళ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	లేకపోతే
	    oldcount = 1;
	ఒకవేళ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    ఒకవేళ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    లేకపోతే
		newcount = 1;
	}
	లేకపోతే
	    తిరిగి FAIL;	// invalid diff format

	ఒకవేళ (oldcount == 0)
	    oldline += 1;
	ఒకవేళ (newcount == 0)
	    newline += 1;
	ఒకవేళ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->లెక్క_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->లెక్క_new = newcount;

	తిరిగి OK;
    }

    తిరిగి FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    స్థిర పూర్ణాంకం
xdiff_బహిర్గతం_indices(
	దీర్ఘ ప్రారంభం_a,
	దీర్ఘ లెక్క_a,
	దీర్ఘ ప్రారంభం_b,
	దీర్ఘ లెక్క_b,
	శూన్య *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	తిరిగి -1;
    }

    p->lnum_orig  = ప్రారంభం_a + 1;
    p->లెక్క_orig = లెక్క_a;
    p->lnum_new   = ప్రారంభం_b + 1;
    p->లెక్క_new  = లెక్క_b;
    ((diffhunk_T **)dout->dout_ga.ga_దత్తాంశం)[dout->dout_ga.ga_len++] = p;
    తిరిగి 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    స్థిర పూర్ణాంకం
xdiff_బహిర్గతం_unified(
	శూన్య *priv,
	mmbuffer_t *mb,
	పూర్ణాంకం nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].కొలత);

    తిరిగి 0;
}

#అయితే_ముగింపు	// FEAT_DIFF

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    శూన్య
f_diff_filler(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_DIFF
    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    శూన్య
f_diff_hlID(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    స్థిర linenr_T	prev_lnum = 0;
    స్థిర varnumber_T	changedtick = 0;
    స్థిర పూర్ణాంకం		fnum = 0;
    స్థిర పూర్ణాంకం		prev_diff_flags = 0;
    స్థిర పూర్ణాంకం		change_ప్రారంభం = 0;
    స్థిర పూర్ణాంకం		change_చివర = 0;
    స్థిర hlf_T	hlID = (hlf_T)0;
    పూర్ణాంకం			cache_results = TRUE;
    పూర్ణాంకం			నిలువు;
    diffline_T		diffline;

    CLEAR_క్షేత్రం(diffline);

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    ఒకవేళ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	పూర్ణాంకం linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	ఒకవేళ (linestatus < 0)
	{
	    ఒకవేళ (linestatus == -1)
	    {
		change_ప్రారంభం = MAXCOL;
		change_చివర = -1;
		ఒకవేళ (diff_కనుగొను_change(curwin, lnum, &diffline))
		    hlID = HLF_కూడు;	// added line
		లేకపోతే
		{
		    hlID = HLF_CHD;	// changed line
		    ఒకవేళ (diffline.అంకె_changes > 0 && cache_results)
		    {
			change_ప్రారంభం = diffline.changes[0].dc_ప్రారంభం[diffline.bufidx];
			change_చివర = diffline.changes[0].dc_చివర[diffline.bufidx];
		    }
		}
	    }
	    లేకపోతే
		hlID = HLF_కూడు;	// added line
	}
	లేకపోతే
	    hlID = (hlf_T)0;

	ఒకవేళ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    ఒకవేళ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	నిలువు = tv_get_సంఖ్య(&argvars[1]) - 1; // ignore type error in {col}
	ఒకవేళ (cache_results)
	{
	    ఒకవేళ (నిలువు >= change_ప్రారంభం && నిలువు < change_చివర)
		hlID = HLF_TXD;			// changed text
	    లేకపోతే
		hlID = HLF_CHD;			// changed line
	}
	లేకపోతే
	{
	    hlID = HLF_CHD;
	    కొరకు (పూర్ణాంకం i = 0; i < diffline.అంకె_changes; i++)
	    {
		పూర్ణాంకం added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_ప్రారంభం, &change_చివర);
		ఒకవేళ (నిలువు >= change_ప్రారంభం && నిలువు < change_చివర)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    ఆపు;
		}
		ఒకవేళ (నిలువు < change_ప్రారంభం)
		    // the remaining changes are past this column and not relevant
		    ఆపు;
	    }
	}
    }
    rettv->vval.v_సంఖ్య = hlID == (hlf_T)0 ? 0 : (పూర్ణాంకం)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    స్థిర పూర్ణాంకం
parse_diff_optarg(
    typval_T	    *opts,
    పూర్ణాంకం		    *diffopts,
    దీర్ఘ	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    పూర్ణాంకం		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    ఒకవేళ (algo != ఖాళీ)
    {
	ఒకవేళ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	లేకపోతే ఒకవేళ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_కనీస;
	లేకపోతే ఒకవేళ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	లేకపోతే ఒకవేళ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    ఒకవేళ (output_fmt != ఖాళీ)
    {
	ఒకవేళ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	లేకపోతే ఒకవేళ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	లేకపోతే
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    తిరిగి FAIL;
	}
    }

    *diff_ctxlen = dict_get_సంఖ్య_def(d, "context", 0);
    ఒకవేళ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    ఒకవేళ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    ఒకవేళ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    ఒకవేళ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    ఒకవేళ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    ఒకవేళ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    ఒకవేళ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    తిరిగి OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    స్థిర శూన్య
list_గమ్యం_diffin(list_T *l, diffin_T *din, పూర్ణాంకం icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	str = tv_get_string(&li->li_tv);
	ఒకవేళ (icase)
	{
	    str = strlow_save(str);
	    ఒకవేళ (str == ఖాళీ)
		కొనసాగు;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	ఒకవేళ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (అక్షరం *)ga.ga_దత్తాంశం;
    din->din_mmfile.కొలత = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    స్థిర dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    ఒకవేళ (hunk_dict == ఖాళీ)
	తిరిగి ఖాళీ;

    dict_కూడు_సంఖ్య(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_కూడు_సంఖ్య(hunk_dict, "from_count", hunk->లెక్క_orig);
    dict_కూడు_సంఖ్య(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_కూడు_సంఖ్య(hunk_dict, "to_count", hunk->లెక్క_new);

    తిరిగి hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    శూన్య
f_diff(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    ఒకవేళ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	తిరిగి;

    CLEAR_క్షేత్రం(dio);
    dio.dio_అంతర్గత = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, పరిమాణం(అక్షరం *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    పూర్ణాంకం		save_diff_flags = diff_flags;
    దీర్ఘ	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_అంతర్గత;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	ఒకవేళ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    తిరిగి;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_గమ్యం_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_గమ్యం_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    పూర్ణాంకం	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    ఒకవేళ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    పూర్ణాంకం diff_స్థితి = diff_file(&dio);

    // restore 'diffexpr'
    ఒకవేళ (restore_diffexpr)
	*p_dex = cc;

    ఒకవేళ (diff_స్థితి == FAIL)
	వెళ్ళు పూర్తి;

    పూర్ణాంకం		hunk_idx = 0;
    dict_T	*hunk_dict;

    ఒకవేళ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	ఒకవేళ (rettv_list_alloc(rettv) != OK)
	    వెళ్ళు పూర్తి;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ఖాళీ;
	అయితే (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_దత్తాంశం)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    ఒకవేళ (hunk_dict == ఖాళీ)
		వెళ్ళు పూర్తి;

	    ఒకవేళ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    లేకపోతే
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_దత్తాంశం);
    }

పూర్తి:
    clear_diffin(&dio.dio_new);
    ఒకవేళ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    లేకపోతే
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#అయితే_ముగింపు
