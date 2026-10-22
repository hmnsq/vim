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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "xdiff/xdiff.h"

#എങ്കിൽ defined(FEAT_DIFF)

സ്ഥിതം പൂർണ്ണസംഖ്യ diff_busy = FALSE;	    // using diff structs, don't change them
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_need_update = FALSE; // ex_diffupdate needs to be called

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
സ്ഥിതം പൂർണ്ണസംഖ്യ	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

സ്ഥിതം long diff_algorithm = XDF_INDENT_HEURISTIC;
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

സ്ഥിതം പൂർണ്ണസംഖ്യ diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# എങ്കിൽ defined(MSWIN)
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
തരനിർവചനം ഘടന {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
തരനിർവചനം ഘടന {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
തരനിർവചനം ഘടന {
    linenr_T lnum_orig;
    long     എണ്ണം_orig;
    linenr_T lnum_new;
    long     എണ്ണം_new;
} diffhunk_T;

തരനിർവചനം എണ്ണൽ {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
തരനിർവചനം ഘടന {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    പൂർണ്ണസംഖ്യ		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    പൂർണ്ണസംഖ്യ		    dio_ctxlen;   // unified diff context length
} diffio_T;

സ്ഥിതം പൂർണ്ണസംഖ്യ diff_buf_idx(buf_T *buf);
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
സ്ഥിതം ശൂന്യം diff_mark_adjust_tp(tabpage_T *tp, പൂർണ്ണസംഖ്യ idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
സ്ഥിതം ശൂന്യം diff_check_unchanged(tabpage_T *tp, diff_T *dp);
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_check_sanity(tabpage_T *tp, diff_T *dp);
സ്ഥിതം പൂർണ്ണസംഖ്യ check_external_diff(diffio_T *diffio);
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_file(diffio_T *diffio);
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_equal_entry(diff_T *dp, പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2);
സ്ഥിതം പൂർണ്ണസംഖ്യ diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
സ്ഥിതം ശൂന്യം diff_fold_update(diff_T *dp, പൂർണ്ണസംഖ്യ skip_idx);
# endif
സ്ഥിതം ശൂന്യം diff_read(പൂർണ്ണസംഖ്യ idx_orig, പൂർണ്ണസംഖ്യ idx_new, diffio_T *dio);
സ്ഥിതം ശൂന്യം diff_copy_entry(diff_T *dprev, diff_T *dp, പൂർണ്ണസംഖ്യ idx_orig, പൂർണ്ണസംഖ്യ idx_new);
സ്ഥിതം diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_diff_ed(char_u *വരി, diffhunk_T *hunk);
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_diff_unified(char_u *വരി, diffhunk_T *hunk);
സ്ഥിതം പൂർണ്ണസംഖ്യ xdiff_out_indices(long start_a, long എണ്ണം_a, long start_b, long എണ്ണം_b, ശൂന്യം *priv);
സ്ഥിതം പൂർണ്ണസംഖ്യ xdiff_out_unified(ശൂന്യം *priv, mmbuffer_t *mb, പൂർണ്ണസംഖ്യ nbuf);
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_diffanchors(പൂർണ്ണസംഖ്യ check_only, buf_T *buf, linenr_T *anchors, പൂർണ്ണസംഖ്യ *അക്കം_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    വേണ്ടി ((dp) = (tp)->tp_first_diff; (dp) != ശൂന്യ; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
സ്ഥിതം diff_T	    *diff_finger_dp = ശൂന്യ;
സ്ഥിതം tabpage_T    *diff_finger_tp = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    diff_finger_idx = -1;
സ്ഥിതം linenr_T	    diff_finger_lnum = 0;

    സ്ഥിതം ശൂന്യം
clear_diffblock(diff_T *dp)
{
    എങ്കിൽ (dp == diff_finger_dp)
	diff_finger_dp = ശൂന്യ;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    സ്ഥിതം diff_T *
diff_scan_start(tabpage_T *tp, പൂർണ്ണസംഖ്യ idx, linenr_T lnum)
{
    എങ്കിൽ (diff_finger_dp != ശൂന്യ
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	തിരികെ diff_finger_dp;
    തിരികെ tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    സ്ഥിതം ശൂന്യം
diff_scan_remember(tabpage_T *tp, പൂർണ്ണസംഖ്യ idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != ശൂന്യ ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    ശൂന്യം
diff_buf_delete(buf_T *buf)
{
    പൂർണ്ണസംഖ്യ		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	എങ്കിൽ (i != DB_എണ്ണം)
	{
	    tp->tp_diffbuf[i] = ശൂന്യ;
	    tp->tp_diff_invalid = TRUE;
	    എങ്കിൽ (tp == curtab)
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
    ശൂന്യം
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		നിർത്തുക;
	എങ്കിൽ (wp == ശൂന്യ)
	{
	    i = diff_buf_idx(win->w_buffer);
	    എങ്കിൽ (i != DB_എണ്ണം)
	    {
		curtab->tp_diffbuf[i] = ശൂന്യ;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
diff_buf_add(buf_T *buf)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (diff_buf_idx(buf) != DB_എണ്ണം)
	തിരികെ;		// It's already there.

    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	എങ്കിൽ (curtab->tp_diffbuf[i] == ശൂന്യ)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    തിരികെ;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_എണ്ണം);
}

/*
 * Remove all buffers to make diffs for.
 */
    സ്ഥിതം ശൂന്യം
diff_buf_clear(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
	{
	    curtab->tp_diffbuf[i] = ശൂന്യ;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_buf_idx(buf_T *buf)
{
    പൂർണ്ണസംഖ്യ		idx;

    വേണ്ടി (idx = 0; idx < DB_എണ്ണം; ++idx)
	എങ്കിൽ (curtab->tp_diffbuf[idx] == buf)
	    നിർത്തുക;
    തിരികെ idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    പൂർണ്ണസംഖ്യ		idx;

    വേണ്ടി (idx = 0; idx < DB_എണ്ണം; ++idx)
	എങ്കിൽ (tp->tp_diffbuf[idx] == buf)
	    നിർത്തുക;
    തിരികെ idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    ശൂന്യം
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	എങ്കിൽ (i != DB_എണ്ണം)
	{
	    tp->tp_diff_invalid = TRUE;
	    എങ്കിൽ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    ശൂന്യം
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    പൂർണ്ണസംഖ്യ		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	എങ്കിൽ (idx != DB_എണ്ണം)
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
    സ്ഥിതം ശൂന്യം
diff_mark_adjust_tp(
    tabpage_T	*tp,
    പൂർണ്ണസംഖ്യ		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		inserted, deleted;
    പൂർണ്ണസംഖ്യ		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    പൂർണ്ണസംഖ്യ		check_unchanged;

    diff_finger_dp = ശൂന്യ;		// may shift df_lnum, invalidate the finger

    എങ്കിൽ (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    എങ്കിൽ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    അല്ലെങ്കിൽ
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = ശൂന്യ;
    dp = tp->tp_first_diff;
    വേണ്ടി (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	എങ്കിൽ ((dp == ശൂന്യ || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == ശൂന്യ
		    || dprev->df_lnum[idx] + dprev->df_എണ്ണം[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    എങ്കിൽ (dnext == ശൂന്യ)
		തിരികെ;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_എണ്ണം[idx] = inserted;
	    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
		എങ്കിൽ (tp->tp_diffbuf[i] != ശൂന്യ && i != idx)
		{
		    എങ്കിൽ (dprev == ശൂന്യ)
			dnext->df_lnum[i] = line1;
		    അല്ലെങ്കിൽ
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_എണ്ണം[i])
			    - (dprev->df_lnum[idx] + dprev->df_എണ്ണം[idx]);
		    dnext->df_എണ്ണം[i] = deleted;
		}
	}

	// if at end of the list, quit
	എങ്കിൽ (dp == ശൂന്യ)
	    നിർത്തുക;

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
	last = dp->df_lnum[idx] + dp->df_എണ്ണം[idx] - 1;

	// 1. change completely above line1: nothing to do
	എങ്കിൽ (last >= line1 - 1)
	{
	    എങ്കിൽ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		എങ്കിൽ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		തുടരുക;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    എങ്കിൽ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		എങ്കിൽ (amount_after == 0)
		    നിർത്തുക;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    അല്ലെങ്കിൽ
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		എങ്കിൽ (deleted > 0)
		{
		    off = 0;
		    എങ്കിൽ (dp->df_lnum[idx] >= line1)
		    {
			എങ്കിൽ (last <= line2)
			{
			    // 4. delete all lines of diff
			    എങ്കിൽ (dp->df_next != ശൂന്യ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_എണ്ണം[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    അല്ലെങ്കിൽ
				n = deleted - dp->df_എണ്ണം[idx];
			    dp->df_എണ്ണം[idx] = 0;
			}
			അല്ലെങ്കിൽ
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_എണ്ണം[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_എണ്ണം[idx] -= last - lnum_deleted + 1;
			    എങ്കിൽ (dp->df_next != ശൂന്യ
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    അല്ലെങ്കിൽ
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			അല്ലെങ്കിൽ
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_എണ്ണം[idx] -= deleted;
			}
		    }

		    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
			എങ്കിൽ (tp->tp_diffbuf[i] != ശൂന്യ && i != idx)
			{
			    എങ്കിൽ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    അല്ലെങ്കിൽ
				dp->df_lnum[i] = 1;
			    dp->df_എണ്ണം[i] += n;
			}
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_എണ്ണം[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    അല്ലെങ്കിൽ
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		എങ്കിൽ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	എങ്കിൽ (dprev != ശൂന്യ && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_എണ്ണം[idx]
							== dp->df_lnum[idx])
	{
	    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
		എങ്കിൽ (tp->tp_diffbuf[i] != ശൂന്യ)
		    dprev->df_എണ്ണം[i] += dp->df_എണ്ണം[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	അല്ലെങ്കിൽ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = ശൂന്യ;
    dp = tp->tp_first_diff;
    ആകുമ്പോൾ (dp != ശൂന്യ)
    {
	// All counts are zero, remove this entry.
	വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	    എങ്കിൽ (tp->tp_diffbuf[i] != ശൂന്യ && dp->df_എണ്ണം[i] != 0)
		നിർത്തുക;
	എങ്കിൽ (i == DB_എണ്ണം)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    എങ്കിൽ (dprev == ശൂന്യ)
		tp->tp_first_diff = dnext;
	    അല്ലെങ്കിൽ
		dprev->df_next = dnext;
	}
	അല്ലെങ്കിൽ
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    എങ്കിൽ (tp == curtab)
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
    സ്ഥിതം diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    എങ്കിൽ (dnew == ശൂന്യ)
	തിരികെ ശൂന്യ;
    diff_finger_dp = ശൂന്യ;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    എങ്കിൽ (dprev == ശൂന്യ)
	tp->tp_first_diff = dnew;
    അല്ലെങ്കിൽ
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, വലുപ്പം(diffline_change_T), 20);
    തിരികെ dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    സ്ഥിതം ശൂന്യം
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    പൂർണ്ണസംഖ്യ		i_org;
    പൂർണ്ണസംഖ്യ		i_new;
    പൂർണ്ണസംഖ്യ		off_org, off_new;
    char_u	*വരി_org;
    പൂർണ്ണസംഖ്യ		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    വേണ്ടി (i_org = 0; i_org < DB_എണ്ണം; ++i_org)
	എങ്കിൽ (tp->tp_diffbuf[i_org] != ശൂന്യ)
	    നിർത്തുക;
    എങ്കിൽ (i_org == DB_എണ്ണം)	// safety check
	തിരികെ;

    എങ്കിൽ (diff_check_sanity(tp, dp) == FAIL)
	തിരികെ;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    വേണ്ടി (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	ആകുമ്പോൾ (dp->df_എണ്ണം[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    എങ്കിൽ (dir == BACKWARD)
		off_org = dp->df_എണ്ണം[i_org] - 1;
	    വരി_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    എങ്കിൽ (വരി_org == ശൂന്യ)
		തിരികെ;
	    വേണ്ടി (i_new = i_org + 1; i_new < DB_എണ്ണം; ++i_new)
	    {
		എങ്കിൽ (tp->tp_diffbuf[i_new] == ശൂന്യ)
		    തുടരുക;
		എങ്കിൽ (dir == BACKWARD)
		    off_new = dp->df_എണ്ണം[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		എങ്കിൽ (off_new < 0 || off_new >= dp->df_എണ്ണം[i_new])
		    നിർത്തുക;
		എങ്കിൽ (diff_cmp(വരി_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    നിർത്തുക;
	    }
	    vim_free(വരി_org);

	    // Stop when a line isn't equal in all diff buffers.
	    എങ്കിൽ (i_new != DB_എണ്ണം)
		നിർത്തുക;

	    // Line matched in all buffers, remove it from the diff.
	    വേണ്ടി (i_new = i_org; i_new < DB_എണ്ണം; ++i_new)
		എങ്കിൽ (tp->tp_diffbuf[i_new] != ശൂന്യ)
		{
		    എങ്കിൽ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_എണ്ണം[i_new];
		}
	}
	എങ്കിൽ (dir == BACKWARD)
	    നിർത്തുക;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	എങ്കിൽ (tp->tp_diffbuf[i] != ശൂന്യ)
	    എങ്കിൽ (dp->df_lnum[i] + dp->df_എണ്ണം[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_വരി_എണ്ണം)
		തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    ശൂന്യം
diff_redraw(
    പൂർണ്ണസംഖ്യ		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		used_max_fill_other = FALSE;
    പൂർണ്ണസംഖ്യ		used_max_fill_curwin = FALSE;
    പൂർണ്ണസംഖ്യ		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	എങ്കിൽ (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    തുടരുക;

	redraw_win_later(wp, UPD_SOME_VALID);
	എങ്കിൽ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	എങ്കിൽ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	എങ്കിൽ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    എങ്കിൽ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    അല്ലെങ്കിൽ എങ്കിൽ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		എങ്കിൽ (wp == curwin)
		    used_max_fill_curwin = TRUE;
		അല്ലെങ്കിൽ എങ്കിൽ (wp_other != ശൂന്യ)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    എങ്കിൽ (wp_other != ശൂന്യ && curwin->w_p_scb)
    {
	എങ്കിൽ (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	അല്ലെങ്കിൽ എങ്കിൽ (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    സ്ഥിതം ശൂന്യം
clear_diffin(diffin_T *din)
{
    എങ്കിൽ (din->din_fname == ശൂന്യ)
	VIM_CLEAR(din->din_mmfile.ptr);
    അല്ലെങ്കിൽ
	mch_remove(din->din_fname);
}

    സ്ഥിതം ശൂന്യം
clear_diffout(diffout_T *dout)
{
    എങ്കിൽ (dout->dout_fname == ശൂന്യ)
	ga_clear_strings(&dout->dout_ga);
    അല്ലെങ്കിൽ
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    എങ്കിൽ (end < 0)
	end = buf->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = ശൂന്യ;
	din->din_mmfile.size = 0;
	തിരികെ OK;
    }

    // xdiff requires one big block of memory with all the text.
    വേണ്ടി (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    എങ്കിൽ (ptr == ശൂന്യ)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	എങ്കിൽ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	തിരികെ FAIL;
    }
    din->din_mmfile.ptr = (അക്ഷരം *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    വേണ്ടി (lnum = start; lnum <= end; ++lnum)
    {
	വേണ്ടി (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    എങ്കിൽ (diff_flags & DIFF_ICASE)
	    {
		പൂർണ്ണസംഖ്യ c;
		പൂർണ്ണസംഖ്യ	orig_len;
		പൂർണ്ണസംഖ്യ	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		എങ്കിൽ (*s == NL)
		    c = NUL;
		അല്ലെങ്കിൽ
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		എങ്കിൽ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		അല്ലെങ്കിൽ
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    എങ്കിൽ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    അല്ലെങ്കിൽ
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    തിരികെ OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		save_ml_flags;
    char_u	*save_ff;
    പൂർണ്ണസംഖ്യ		save_cmod_flags;

    എങ്കിൽ (din->din_fname == ശൂന്യ)
	തിരികെ diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    എങ്കിൽ (frames_locked())
	തിരികെ FAIL;

    എങ്കിൽ (end < 0)
	end = buf->b_ml.ml_വരി_എണ്ണം;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    എങ്കിൽ (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, ശൂന്യ,
			start, end,
			ശൂന്യ, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    തിരികെ r;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lnum_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    എങ്കിൽ (lnum1 < lnum2)
	തിരികെ -1;
    എങ്കിൽ (lnum1 > lnum2)
	തിരികെ 1;
    തിരികെ 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    സ്ഥിതം ശൂന്യം
diff_try_update(
	diffio_T    *dio,
	പൂർണ്ണസംഖ്യ	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		idx_new;

    എങ്കിൽ (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, വലുപ്പം(അക്ഷരം *), 1000);
    }
    അല്ലെങ്കിൽ
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	എങ്കിൽ (dio->dio_orig.din_fname == ശൂന്യ
		|| dio->dio_new.din_fname == ശൂന്യ
		|| dio->dio_diff.dout_fname == ശൂന്യ)
	    പോകുക theend;
    }

    // Check external diff is actually working.
    എങ്കിൽ (!dio->dio_internal && check_external_diff(dio) == FAIL)
	പോകുക theend;

    // :diffupdate!
    എങ്കിൽ (eap != ശൂന്യ && eap->forceit)
	വേണ്ടി (idx_new = idx_orig; idx_new < DB_എണ്ണം; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    എങ്കിൽ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    പൂർണ്ണസംഖ്യ അക്കം_anchors = INT_MAX;
    linenr_T anchors[DB_എണ്ണം][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    എങ്കിൽ (diff_flags & DIFF_ANCHOR)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < DB_എണ്ണം; idx++)
	{
	    എങ്കിൽ (curtab->tp_diffbuf[idx] == ശൂന്യ)
		തുടരുക;
	    പൂർണ്ണസംഖ്യ buf_അക്കം_anchors = 0;
	    എങ്കിൽ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_അക്കം_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		അക്കം_anchors = 0;
		CLEAR_FIELD(anchors);
		നിർത്തുക;
	    }
	    എങ്കിൽ (buf_അക്കം_anchors < അക്കം_anchors)
		അക്കം_anchors = buf_അക്കം_anchors;

	    എങ്കിൽ (buf_അക്കം_anchors > 0)
		qsort((ശൂന്യം *)anchors[idx],
			(size_t)buf_അക്കം_anchors,
			വലുപ്പം(linenr_T),
			lnum_compare);
	}
    }
    എങ്കിൽ (അക്കം_anchors == INT_MAX)
	അക്കം_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    വേണ്ടി (പൂർണ്ണസംഖ്യ anchor_i = 0; anchor_i <= അക്കം_anchors; anchor_i++)
    {
	diff_T	*orig_diff = ശൂന്യ;
	എങ്കിൽ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = ശൂന്യ;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == അക്കം_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	എങ്കിൽ (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    എങ്കിൽ (orig_diff != ശൂന്യ)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    പോകുക theend;
	}

	// Make a difference between the first buffer and every other.
	വേണ്ടി (idx_new = idx_orig + 1; idx_new < DB_എണ്ണം; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
		തുടരുക; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == അക്കം_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    എങ്കിൽ (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		തുടരുക;
	    എങ്കിൽ (diff_file(dio) == FAIL)
		തുടരുക;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	എങ്കിൽ (anchor_i != 0)
	{
	    diff_finger_dp = ശൂന്യ;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    വേണ്ടി (diff_T *dp = curtab->tp_first_diff; dp != ശൂന്യ; dp = dp->df_next)
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < DB_എണ്ണം; idx++)
		{
		    എങ്കിൽ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    എങ്കിൽ (orig_diff != ശൂന്യ)
	    {
		diff_T *last_diff = orig_diff;
		ആകുമ്പോൾ (last_diff->df_next != ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ
diff_internal(ശൂന്യം)
{
    തിരികെ (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_internal_failed(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ idx;

    // Only need to do something when there is another buffer.
    വേണ്ടി (idx = 0; idx < DB_എണ്ണം; ++idx)
	എങ്കിൽ (curtab->tp_diffbuf[idx] != ശൂന്യ
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    ശൂന്യം
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    പൂർണ്ണസംഖ്യ		idx_orig;
    പൂർണ്ണസംഖ്യ		idx_new;
    diffio_T	diffio;
    പൂർണ്ണസംഖ്യ		had_diffs = curtab->tp_first_diff != ശൂന്യ;

    എങ്കിൽ (diff_busy)
    {
	diff_need_update = TRUE;
	തിരികെ;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    വേണ്ടി (idx_orig = 0; idx_orig < DB_എണ്ണം; ++idx_orig)
	എങ്കിൽ (curtab->tp_diffbuf[idx_orig] != ശൂന്യ)
	    നിർത്തുക;
    എങ്കിൽ (idx_orig == DB_എണ്ണം)
	പോകുക theend;

    // Only need to do something when there is another buffer.
    വേണ്ടി (idx_new = idx_orig + 1; idx_new < DB_എണ്ണം; ++idx_new)
	എങ്കിൽ (curtab->tp_diffbuf[idx_new] != ശൂന്യ)
	    നിർത്തുക;
    എങ്കിൽ (idx_new == DB_എണ്ണം)
	പോകുക theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    എങ്കിൽ (diffio.dio_internal && diff_internal_failed())
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
    എങ്കിൽ (had_diffs || curtab->tp_first_diff != ശൂന്യ)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    പൂർണ്ണസംഖ്യ		ok;
    പൂർണ്ണസംഖ്യ		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    വേണ്ടി (;;)
    {
	ok = FALSE;
	fd = mch_fopen((അക്ഷരം *)diffio->dio_orig.din_fname, "w");
	എങ്കിൽ (fd == ശൂന്യ)
	    io_error = TRUE;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((അക്ഷരം *)diffio->dio_new.din_fname, "w");
	    എങ്കിൽ (fd == ശൂന്യ)
		io_error = TRUE;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = ശൂന്യ;
		എങ്കിൽ (diff_file(diffio) == OK)
		    fd = mch_fopen((അക്ഷരം *)diffio->dio_diff.dout_fname, "r");
		എങ്കിൽ (fd == ശൂന്യ)
		    io_error = TRUE;
		അല്ലെങ്കിൽ
		{
		    char_u	linebuf[LBUFLEN];

		    വേണ്ടി (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			എങ്കിൽ (vim_fgets(linebuf, LBUFLEN, fd))
			    നിർത്തുക;
			എങ്കിൽ (STRNCMP(linebuf, "1c1", 3) == 0
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
	എങ്കിൽ (*p_dex != NUL)
	    നിർത്തുക;
# endif

# എങ്കിൽ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	എങ്കിൽ (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    തുടരുക;
	}
	എങ്കിൽ (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	എങ്കിൽ (diff_a_works != MAYBE)
	    നിർത്തുക;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	എങ്കിൽ (ok)
	    നിർത്തുക;
    }
    എങ്കിൽ (!ok)
    {
	എങ്കിൽ (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# എങ്കിൽ defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Invoke the xdiff function.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    എങ്കിൽ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    എങ്കിൽ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    എങ്കിൽ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    എങ്കിൽ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_വരികൾ;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    എങ്കിൽ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    അല്ലെങ്കിൽ
	emit_cb.out_വരി = xdiff_out_unified;
    എങ്കിൽ (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	തിരികെ FAIL;
    }
    എങ്കിൽ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    എങ്കിൽ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	തിരികെ OK;
    }
    അല്ലെങ്കിൽ
# endif
    // Use xdiff for generating the diff.
    എങ്കിൽ (dio->dio_internal)
	തിരികെ diff_file_internal(dio);

    എങ്കിൽ (check_restricted())
	തിരികെ FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    എങ്കിൽ (cmd == ശൂന്യ)
	തിരികെ FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    എങ്കിൽ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((അക്ഷരം *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# എങ്കിൽ defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# അല്ലെങ്കിൽ
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (പൂർണ്ണസംഖ്യ)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (ശൂന്യം)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    തിരികെ OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    ശൂന്യം
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = ശൂന്യ;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = ശൂന്യ;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = ശൂന്യ;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = ശൂന്യ;

# ifdef FEAT_BROWSE
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, ശൂന്യ, ശൂന്യ,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), ശൂന്യ);
	എങ്കിൽ (browseFile == ശൂന്യ)
	    തിരികെ;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    എങ്കിൽ (tmp_orig == ശൂന്യ || tmp_new == ശൂന്യ)
	പോകുക theend;

    // Write the current buffer to "tmp_orig".
    എങ്കിൽ (buf_write(curbuf, tmp_orig, ശൂന്യ,
		(linenr_T)1, curbuf->b_ml.ml_വരി_എണ്ണം,
				     ശൂന്യ, FALSE, FALSE, FALSE, TRUE) == FAIL)
	പോകുക theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != ശൂന്യ ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    എങ്കിൽ (esc_name == ശൂന്യ)
	പോകുക theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    എങ്കിൽ (buf == ശൂന്യ)
	പോകുക theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    എങ്കിൽ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((അക്ഷരം *)dirbuf) != 0)
	dirbuf[0] = NUL;
    അല്ലെങ്കിൽ
    {
#  ifdef TEMPDIRNAMES
	എങ്കിൽ (vim_tempdir != ശൂന്യ)
	    vim_ignored = mch_chdir((അക്ഷരം *)vim_tempdir);
	അല്ലെങ്കിൽ
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    എങ്കിൽ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != ശൂന്യ ? fullname :
#  endif
		eap->arg, tmp_new);
    അല്ലെങ്കിൽ
# endif
    {
	എങ്കിൽ (check_restricted())
	    പോകുക theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((അക്ഷരം *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(ശൂന്യം)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    എങ്കിൽ (dirbuf[0] != NUL)
    {
	എങ്കിൽ (mch_chdir((അക്ഷരം *)dirbuf) != 0)
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
    എങ്കിൽ (mch_stat((അക്ഷരം *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (curbuf->b_fname != ശൂന്യ)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    എങ്കിൽ (newname != ശൂന്യ)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	എങ്കിൽ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    എങ്കിൽ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		എങ്കിൽ (newname != ശൂന്യ)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    എങ്കിൽ (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    എങ്കിൽ (tmp_orig != ശൂന്യ)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    എങ്കിൽ (tmp_new != ശൂന്യ)
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
    ശൂന്യം
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

    എങ്കിൽ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	തിരികെ;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    എങ്കിൽ (curwin == old_curwin)		// split didn't work
	തിരികെ;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    എങ്കിൽ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	എങ്കിൽ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_വരി(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    ശൂന്യം
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    സ്ഥിതം ശൂന്യം
set_diff_option(win_T *wp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_മൂല്യം_give_err((char_u *)"diff", (long)മൂല്യം, ശൂന്യ, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    ശൂന്യം
diff_win_options(
    win_T	*wp,
    പൂർണ്ണസംഖ്യ		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    എങ്കിൽ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    എങ്കിൽ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    എങ്കിൽ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	എങ്കിൽ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    എങ്കിൽ (!wp->w_p_diff)
    {
	എങ്കിൽ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    എങ്കിൽ (!wp->w_p_diff)
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
    എങ്കിൽ (vim_strchr(p_sbo, 'h') == ശൂന്യ)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    എങ്കിൽ (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    ശൂന്യം
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    എങ്കിൽ (wp->w_p_diff_saved)
	    {

		എങ്കിൽ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		എങ്കിൽ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		എങ്കിൽ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    എങ്കിൽ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		എങ്കിൽ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		എങ്കിൽ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		എങ്കിൽ (wp->w_p_fen)
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
    എങ്കിൽ (eap->forceit)
	diff_buf_clear();

    എങ്കിൽ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    എങ്കിൽ (!diffwin && vim_strchr(p_sbo, 'h') != ശൂന്യ)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    സ്ഥിതം ശൂന്യം
diff_read(
    പൂർണ്ണസംഖ്യ		idx_orig,	// idx of original file
    പൂർണ്ണസംഖ്യ		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		വരി_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = ശൂന്യ;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*വരി;
    long	off;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = ശൂന്യ;	    // init to avoid gcc warning

    എണ്ണൽ {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    എങ്കിൽ (dout->dout_fname == ശൂന്യ)
    {
	diffstyle = DIFF_UNIFIED;
    }
    അല്ലെങ്കിൽ
    {
	fd = mch_fopen((അക്ഷരം *)dout->dout_fname, "r");
	എങ്കിൽ (fd == ശൂന്യ)
	{
	    emsg(_(e_cannot_read_diff_output));
	    തിരികെ;
	}
    }

    എങ്കിൽ (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	എങ്കിൽ (hunk == ശൂന്യ)
	{
	    എങ്കിൽ (fd != ശൂന്യ)
		fclose(fd);
	    തിരികെ;
	}
    }

    വേണ്ടി (;;)
    {
	എങ്കിൽ (dio->dio_internal)
	{
	    എങ്കിൽ (വരി_hunk_idx >= dout->dout_ga.ga_len)
		നിർത്തുക;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[വരി_hunk_idx++];
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (fd == ശൂന്യ)
	    {
		എങ്കിൽ (വരി_hunk_idx >= dout->dout_ga.ga_len)
		    നിർത്തുക;	    // did last line
		വരി = ((char_u **)dout->dout_ga.ga_data)[വരി_hunk_idx++];
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (vim_fgets(linebuf, LBUFLEN, fd))
		    നിർത്തുക;		// end of file
		വരി = linebuf;
	    }

	    എങ്കിൽ (diffstyle == DIFF_NONE)
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
		എങ്കിൽ (SAFE_isdigit(*വരി))
		    diffstyle = DIFF_ED;
		അല്ലെങ്കിൽ എങ്കിൽ ((STRNCMP(വരി, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		അല്ലെങ്കിൽ എങ്കിൽ ((STRNCMP(വരി, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(വരി, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(വരി, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		അല്ലെങ്കിൽ
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    തുടരുക;
	    }

	    എങ്കിൽ (diffstyle == DIFF_ED)
	    {
		എങ്കിൽ (!SAFE_isdigit(*വരി))
		    തുടരുക;	// not the start of a diff block
		എങ്കിൽ (parse_diff_ed(വരി, hunk) == FAIL)
		    തുടരുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (diffstyle == DIFF_UNIFIED)
	    {
		എങ്കിൽ (STRNCMP(വരി, "@@ ", 3)  != 0)
		    തുടരുക;	// not the start of a diff block
		എങ്കിൽ (parse_diff_unified(വരി, hunk) == FAIL)
		    തുടരുക;
	    }
	    അല്ലെങ്കിൽ
	    {
		emsg(_(e_invalid_diff_format));
		നിർത്തുക;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	ആകുമ്പോൾ (dp != ശൂന്യ
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_എണ്ണം[idx_orig])
	{
	    എങ്കിൽ (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	എങ്കിൽ (dp != ശൂന്യ
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_എണ്ണം[idx_orig]
		&& hunk->lnum_orig + hunk->എണ്ണം_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    വേണ്ടി (dpl = dp; dpl->df_next != ശൂന്യ; dpl = dpl->df_next)
		എങ്കിൽ (hunk->lnum_orig + hunk->എണ്ണം_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    നിർത്തുക;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    എങ്കിൽ (off > 0)
	    {
		വേണ്ടി (i = idx_orig; i < idx_new; ++i)
		    എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
		    {
			dp->df_lnum[i] -= off;
			dp->df_എണ്ണം[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_എണ്ണം[idx_new] = hunk->എണ്ണം_new;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_എണ്ണം[idx_new] = hunk->എണ്ണം_new - off;
	    }
	    അല്ലെങ്കിൽ
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		പൂർണ്ണസംഖ്യ orig_size_in_dp = MIN(hunk->എണ്ണം_orig,
			dp->df_lnum[idx_orig] +
			dp->df_എണ്ണം[idx_orig] - hunk->lnum_orig);
		പൂർണ്ണസംഖ്യ size_diff = hunk->എണ്ണം_new - orig_size_in_dp;
		dp->df_എണ്ണം[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->എണ്ണം_new
		    - (dp->df_lnum[idx_new] + dp->df_എണ്ണം[idx_new]);
		എങ്കിൽ (off > 0)
		    dp->df_എണ്ണം[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->എണ്ണം_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_എണ്ണം[idx_orig]);
	    എങ്കിൽ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		എങ്കിൽ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_എണ്ണം[idx_new] += -off;
		}
		off = 0;
	    }
	    വേണ്ടി (i = idx_orig; i < idx_new; ++i)
		എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
		    dp->df_എണ്ണം[i] = dpl->df_lnum[i] + dpl->df_എണ്ണം[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    ആകുമ്പോൾ (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    എങ്കിൽ (dp == ശൂന്യ)
		പോകുക done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_എണ്ണം[idx_orig] = hunk->എണ്ണം_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_എണ്ണം[idx_new] = hunk->എണ്ണം_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    വേണ്ടി (i = idx_orig + 1; i < idx_new; ++i)
		എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    ആകുമ്പോൾ (dp != ശൂന്യ)
    {
	എങ്കിൽ (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    എങ്കിൽ (!dio->dio_internal)
	vim_free(hunk);

    എങ്കിൽ (fd != ശൂന്യ)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    സ്ഥിതം ശൂന്യം
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    പൂർണ്ണസംഖ്യ		idx_orig,
    പൂർണ്ണസംഖ്യ		idx_new)
{
    long	off;

    എങ്കിൽ (dprev == ശൂന്യ)
	off = 0;
    അല്ലെങ്കിൽ
	off = (dprev->df_lnum[idx_orig] + dprev->df_എണ്ണം[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_എണ്ണം[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_എണ്ണം[idx_new] = dp->df_എണ്ണം[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    ശൂന്യം
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    വേണ്ടി (p = tp->tp_first_diff; p != ശൂന്യ; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = ശൂന്യ;
}

/*
 *  return true if the options are set to use diff linematch
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_linematch(diff_T *dp)
{
    എങ്കിൽ (!(diff_flags & DIFF_LINEMATCH))
	തിരികെ 0;

    // are there more than three diff buffers?
    പൂർണ്ണസംഖ്യ tsize = 0;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
    {
	എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    എങ്കിൽ (dp->df_എണ്ണം[i] < 0)
		തിരികെ FALSE;
	    tsize += dp->df_എണ്ണം[i];
	}
    }

    // avoid allocating a huge array because it will lag
    തിരികെ tsize <= linematch_വരികൾ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_max_diff_length(സ്ഥിരം diff_T *dp)
{
    പൂർണ്ണസംഖ്യ maxlength = 0;

    വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < DB_എണ്ണം; k++)
    {
	എങ്കിൽ (curtab->tp_diffbuf[k] != ശൂന്യ)
	{
	    എങ്കിൽ (dp->df_എണ്ണം[k] > maxlength)
		maxlength = dp->df_എണ്ണം[k];
	}
    }
    തിരികെ maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    സ്ഥിതം ശൂന്യം
find_മുകൾ_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    പൂർണ്ണസംഖ്യ		fromidx,
    പൂർണ്ണസംഖ്യ		topline)
{
    diff_T	*topdiff = ശൂന്യ;
    diff_T	*localtopdiff = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		topdiffchange = 0;

    വേണ്ടി (topdiff = curtab->tp_first_diff; topdiff != ശൂന്യ;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	എങ്കിൽ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	എങ്കിൽ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_എണ്ണം[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    എങ്കിൽ ((*thistopdiff) == ശൂന്യ)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	എങ്കിൽ (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_എണ്ണം[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    എങ്കിൽ (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		നിർത്തുക;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    സ്ഥിതം ശൂന്യം
calculate_topfill_and_topline(
    സ്ഥിരം പൂർണ്ണസംഖ്യ	fromidx,
    സ്ഥിരം പൂർണ്ണസംഖ്യ	toidx,
    സ്ഥിരം പൂർണ്ണസംഖ്യ	from_topline,
    സ്ഥിരം പൂർണ്ണസംഖ്യ	from_topfill,
    പൂർണ്ണസംഖ്യ		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = ശൂന്യ;
    diff_T	*next_adjacent_blocks = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		virtual_വരികൾ_passed = 0;
    പൂർണ്ണസംഖ്യ		curlinenum_to = 1;

    find_മുകൾ_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    ആകുമ്പോൾ (curdif && (curdif->df_lnum[fromidx] + curdif->df_എണ്ണം[fromidx])
							<= from_topline)
    {
	virtual_വരികൾ_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    എങ്കിൽ (curdif != next_adjacent_blocks)
	virtual_വരികൾ_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_വരികൾ_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    എങ്കിൽ (virtual_വരികൾ_passed < 0)
	virtual_വരികൾ_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    എങ്കിൽ (thistopdiff != ശൂന്യ) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    പൂർണ്ണസംഖ്യ virt_വരികൾ_ഇടത് = virtual_വരികൾ_passed;
    curdif = thistopdiff;
    ആകുമ്പോൾ (virt_വരികൾ_ഇടത് > 0 && curdif != ശൂന്യ && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_വരികൾ_ഇടത്, curdif->df_എണ്ണം[toidx]);
	virt_വരികൾ_ഇടത് -= MIN(virt_വരികൾ_ഇടത്, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    പൂർണ്ണസംഖ്യ max_virt_വരികൾ = 0;
    വേണ്ടി (diff_T *dp = thistopdiff; dp != ശൂന്യ; dp = dp->df_next)
    {
	എങ്കിൽ (dp->df_lnum[toidx] + dp->df_എണ്ണം[toidx] <= curlinenum_to)
	    max_virt_വരികൾ += get_max_diff_length(dp);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_വരികൾ += curlinenum_to - dp->df_lnum[toidx];
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_വരികൾ - virtual_വരികൾ_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    സ്ഥിതം ശൂന്യം
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    സ്ഥിരം പൂർണ്ണസംഖ്യ	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    പൂർണ്ണസംഖ്യ		വരി_numbers[DB_എണ്ണം];
    പൂർണ്ണസംഖ്യ		outputmap[DB_എണ്ണം];
    size_t	ndiffs = 0;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
    {
	എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
	{
	    വരി_numbers[i] = dp->df_lnum[i];
	    dp->df_എണ്ണം[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    വേണ്ടി (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	എങ്കിൽ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < DB_എണ്ണം; j++)
	    {
		എങ്കിൽ (curtab->tp_diffbuf[j] != ശൂന്യ)
		{
		    dp_s->df_lnum[j] = വരി_numbers[j];
		    dp_s->df_എണ്ണം[j] = 0;
		}
	    }
	}
	വേണ്ടി (size_t j = 0; j < ndiffs; j++)
	{
	    എങ്കിൽ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_എണ്ണം[outputmap[j]]++;
		വരി_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    സ്ഥിതം ശൂന്യം
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_എണ്ണം];
    സ്ഥിരം mmfile_t	*diffbufs[DB_എണ്ണം];
    പൂർണ്ണസംഖ്യ			diff_length[DB_എണ്ണം];
    size_t		ndiffs = 0;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
    {
	എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    എങ്കിൽ (dp->df_എണ്ണം[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_എണ്ണം[i] - 1);
	    }
	    അല്ലെങ്കിൽ
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = ശൂന്യ;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_എണ്ണം[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    പൂർണ്ണസംഖ്യ *decisions = ശൂന്യ;
    സ്ഥിരം പൂർണ്ണസംഖ്യ iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    വേണ്ടി (size_t i = 0; i < ndiffs; i++)
	സ്വതന്ത്രം(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    സ്വതന്ത്രം(decisions);
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
    പൂർണ്ണസംഖ്യ
diff_check_with_linestatus(win_T *wp, linenr_T lnum, പൂർണ്ണസംഖ്യ *linestatus)
{
    പൂർണ്ണസംഖ്യ		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    പൂർണ്ണസംഖ്യ		maxcount;
    പൂർണ്ണസംഖ്യ		i;
    buf_T	*buf = wp->w_buffer;
    പൂർണ്ണസംഖ്യ		cmp;

    എങ്കിൽ (linestatus != ശൂന്യ)
	*linestatus = 0;

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    എങ്കിൽ (curtab->tp_first_diff == ശൂന്യ || !wp->w_p_diff)	// no diffs at all
	തിരികെ 0;

    // safety check: "lnum" must be a buffer line
    എങ്കിൽ (lnum < 1 || lnum > buf->b_ml.ml_വരി_എണ്ണം + 1)
	തിരികെ 0;

    idx = diff_buf_idx(buf);
    എങ്കിൽ (idx == DB_എണ്ണം)
	തിരികെ 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    എങ്കിൽ (hasFoldingWin(wp, lnum, ശൂന്യ, ശൂന്യ, TRUE, ശൂന്യ))
	തിരികെ 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ശൂന്യ;		// last block with df_lnum[idx] <= lnum
    വേണ്ടി (dp = diff_scan_start(curtab, idx, lnum); dp != ശൂന്യ; dp = dp->df_next)
    {
	എങ്കിൽ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	എങ്കിൽ (lnum <= dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
	    നിർത്തുക;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    എങ്കിൽ (dp == ശൂന്യ || lnum < dp->df_lnum[idx])
	തിരികെ 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    എങ്കിൽ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    പൂർണ്ണസംഖ്യ അക്കം_fill = 0;
    ആകുമ്പോൾ (lnum == dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	എങ്കിൽ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    അക്കം_fill += maxcount - dp->df_എണ്ണം[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	എങ്കിൽ (dp->df_next != ശൂന്യ
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_എണ്ണം[idx])
	    dp = dp->df_next;
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    എങ്കിൽ (lnum < dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
    {
	പൂർണ്ണസംഖ്യ	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	    എങ്കിൽ (i != idx && curtab->tp_diffbuf[i] != ശൂന്യ)
	    {
		എങ്കിൽ (dp->df_എണ്ണം[i] == 0)
		    zero = TRUE;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (dp->df_എണ്ണം[i] != dp->df_എണ്ണം[idx])
		    {
			എങ്കിൽ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			തിരികെ അക്കം_fill;
		    }
		    cmp = TRUE;
		}
	    }
	എങ്കിൽ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
		എങ്കിൽ (i != idx && curtab->tp_diffbuf[i] != ശൂന്യ
						      && dp->df_എണ്ണം[i] != 0)
		    എങ്കിൽ (!diff_equal_entry(dp, idx, i))
		    {
			എങ്കിൽ (linestatus)
			    *linestatus = -1;
			തിരികെ അക്കം_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	എങ്കിൽ (zero == FALSE)
	    തിരികെ അക്കം_fill;
	എങ്കിൽ (linestatus)
	    *linestatus = -2;
	തിരികെ അക്കം_fill;
    }
    തിരികെ അക്കം_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_equal_entry(diff_T *dp, പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		cmp;

    എങ്കിൽ (dp->df_എണ്ണം[idx1] != dp->df_എണ്ണം[idx2])
	തിരികെ FALSE;
    എങ്കിൽ (diff_check_sanity(curtab, dp) == FAIL)
	തിരികെ FALSE;
    വേണ്ടി (i = 0; i < dp->df_എണ്ണം[idx1]; ++i)
    {
	വരി = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	എങ്കിൽ (വരി == ശൂന്യ)
	    തിരികെ FALSE;
	cmp = diff_cmp(വരി, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(വരി);
	എങ്കിൽ (cmp != 0)
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_equal_char(char_u *p1, char_u *p2, പൂർണ്ണസംഖ്യ *len)
{
    പൂർണ്ണസംഖ്യ l  = (*mb_ptr2len)(p1);

    എങ്കിൽ (l != (*mb_ptr2len)(p2))
	തിരികെ FALSE;
    എങ്കിൽ (l > 1)
    {
	എങ്കിൽ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    തിരികെ FALSE;
	*len = l;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    തിരികെ FALSE;
	*len = 1;
    }
    തിരികെ TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	തിരികെ 0;

    എങ്കിൽ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	തിരികെ STRCMP(s1, s2);
    എങ്കിൽ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	തിരികെ MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    ആകുമ്പോൾ (*p1 != NUL && *p2 != NUL)
    {
	എങ്കിൽ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!diff_equal_char(p1, p2, &l))
		നിർത്തുക;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    എങ്കിൽ (*p1 != NUL || *p2 != NUL)
	തിരികെ 1;
    തിരികെ 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    പൂർണ്ണസംഖ്യ
diff_check_fill(win_T *wp, linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ		n;

    // be quick when there are no filler lines
    എങ്കിൽ (!(diff_flags & DIFF_FILLER))
	തിരികെ 0;
    n = diff_check_with_linestatus(wp, lnum, ശൂന്യ);
    എങ്കിൽ (n <= 0)
	തിരികെ 0;
    തിരികെ n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    ശൂന്യം
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		fromidx;
    പൂർണ്ണസംഖ്യ		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    എങ്കിൽ (fromidx == DB_എണ്ണം)
	തിരികെ;		// safety check

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	എങ്കിൽ (lnum <= dp->df_lnum[fromidx] + dp->df_എണ്ണം[fromidx])
	    നിർത്തുക;
    എങ്കിൽ (dp == ശൂന്യ)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_വരി_എണ്ണം
				       - (frombuf->b_ml.ml_വരി_എണ്ണം - lnum);
    }
    അല്ലെങ്കിൽ
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	എങ്കിൽ (toidx == DB_എണ്ണം)
	    തിരികെ;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	എങ്കിൽ (lnum >= dp->df_lnum[fromidx])
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
    എങ്കിൽ (towin->w_topline > towin->w_buffer->b_ml.ml_വരി_എണ്ണം)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_വരി_എണ്ണം;
	towin->w_botfill = true;
    }
    എങ്കിൽ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_വരി_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (ശൂന്യം)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    ശൂന്യ, TRUE, ശൂന്യ);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_diffanchors(
    പൂർണ്ണസംഖ്യ		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    പൂർണ്ണസംഖ്യ		*അക്കം_anchors)
{
    പൂർണ്ണസംഖ്യ i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = ശൂന്യ;
    എങ്കിൽ (check_only)
	bufwin = curwin;
    അല്ലെങ്കിൽ
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    എങ്കിൽ (bufwin->w_buffer == buf && bufwin->w_p_diff)
		നിർത്തുക;
	എങ്കിൽ (bufwin == ശൂന്യ && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    തിരികെ FAIL;
	}
    }

    വേണ്ടി (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	എങ്കിൽ (*dia == ',') // don't allow empty values
	    തിരികെ FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(ശൂന്യ, &dia, ADDR_വരികൾ, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	എങ്കിൽ (dia == ശൂന്യ) // error detected
	    തിരികെ FAIL;
	എങ്കിൽ (*dia != ',' && *dia != NUL)
	    തിരികെ FAIL;

	എങ്കിൽ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_വരി_എണ്ണം + 1))
	{
	    emsg(_(e_invalid_range));
	    തിരികെ FAIL;
	}

	എങ്കിൽ (anchors != ശൂന്യ)
	    anchors[i] = lnum;

	എങ്കിൽ (*dia == ',')
	    dia++;
    }
    എങ്കിൽ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	തിരികെ FAIL;
    }
    എങ്കിൽ (അക്കം_anchors != ശൂന്യ)
	*അക്കം_anchors = i;
    തിരികെ OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    പൂർണ്ണസംഖ്യ
diffanchors_changed(പൂർണ്ണസംഖ്യ buflocal)
{
    പൂർണ്ണസംഖ്യ result = parse_diffanchors(TRUE, curbuf, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    എങ്കിൽ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < DB_എണ്ണം; ++idx)
		    എങ്കിൽ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			നിർത്തുക;
		    }
	    }
	}
    }
    തിരികെ result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    പൂർണ്ണസംഖ്യ
diffopt_changed(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		diff_context_new = 6;
    പൂർണ്ണസംഖ്യ		linematch_വരികൾ_new = 0;
    പൂർണ്ണസംഖ്യ		diff_flags_new = 0;
    പൂർണ്ണസംഖ്യ		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    ആകുമ്പോൾ (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	എങ്കിൽ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    എങ്കിൽ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    അല്ലെങ്കിൽ
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    എങ്കിൽ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    അല്ലെങ്കിൽ
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_വരികൾ_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    എങ്കിൽ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	തിരികെ FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    എങ്കിൽ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_വരികൾ = linematch_വരികൾ_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    തിരികെ OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    പൂർണ്ണസംഖ്യ
diffopt_horizontal(ശൂന്യം)
{
    തിരികെ (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    പൂർണ്ണസംഖ്യ
diffopt_hiddenoff(ശൂന്യം)
{
    തിരികെ (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    പൂർണ്ണസംഖ്യ
diffopt_closeoff(ശൂന്യം)
{
    തിരികെ (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    ശൂന്യം
diff_update_വരി(linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ		idx;
    diff_T	*dp;

    എങ്കിൽ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	തിരികെ;

    idx = diff_buf_idx(curbuf);
    എങ്കിൽ (idx == DB_എണ്ണം)
	തിരികെ;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	എങ്കിൽ (lnum <= dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
	    നിർത്തുക;

    // clear the inline change cache as it's invalid
    എങ്കിൽ (dp != ശൂന്യ)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

സ്ഥിതം diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    പൂർണ്ണസംഖ്യ
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    പൂർണ്ണസംഖ്യ *change_start,
    പൂർണ്ണസംഖ്യ *change_end)
{
    എങ്കിൽ (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    അല്ലെങ്കിൽ
	*change_start = change->dc_start[diffline->bufidx];
    എങ്കിൽ (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    അല്ലെങ്കിൽ
	*change_end = change->dc_end[diffline->bufidx];

    എങ്കിൽ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	തിരികെ FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
    {
	എങ്കിൽ (i == diffline->bufidx)
	    തുടരുക;
	എങ്കിൽ (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    തിരികെ FALSE;
	}
    }
    തിരികെ TRUE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		*startp,	// first char of the change
    പൂർണ്ണസംഖ്യ		*endp)		// last char of the change
{
    char_u	*വരി_org;
    char_u	*വരി_new;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		si_org, si_new;
    പൂർണ്ണസംഖ്യ		ei_org, ei_new;
    പൂർണ്ണസംഖ്യ		off;
    പൂർണ്ണസംഖ്യ		added = TRUE;
    char_u	*p1, *p2;
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	വരി_org = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	വരി_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	എങ്കിൽ (വരി_org == ശൂന്യ)
	    തിരികെ FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    എങ്കിൽ (off >= dp->df_എണ്ണം[i])
		തുടരുക;
	    added = FALSE;
	    എങ്കിൽ (diff_flags & DIFF_INLINE_NONE)
		നിർത്തുക; // early terminate as we only care about the return value

	    വരി_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    ആകുമ്പോൾ (വരി_org[si_org] != NUL)
	    {
		എങ്കിൽ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(വരി_org[si_org])
					      && VIM_ISWHITE(വരി_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(വരി_org[si_org])
					    || VIM_ISWHITE(വരി_new[si_new]))))
		{
		    si_org = (പൂർണ്ണസംഖ്യ)(skipwhite(വരി_org + si_org) - വരി_org);
		    si_new = (പൂർണ്ണസംഖ്യ)(skipwhite(വരി_new + si_new) - വരി_new);
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (!diff_equal_char(വരി_org + si_org, വരി_new + si_new,
									   &l))
			നിർത്തുക;
		    si_org += l;
		    si_new += l;
		}
	    }
	    എങ്കിൽ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(വരി_org, വരി_org + si_org);
		si_new -= (*mb_head_off)(വരി_new, വരി_new + si_new);
	    }
	    എങ്കിൽ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    എങ്കിൽ (വരി_org[si_org] != NUL || വരി_new[si_new] != NUL)
	    {
		ei_org = (പൂർണ്ണസംഖ്യ)STRLEN(വരി_org);
		ei_new = (പൂർണ്ണസംഖ്യ)STRLEN(വരി_new);
		ആകുമ്പോൾ (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    എങ്കിൽ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(വരി_org[ei_org])
					      && VIM_ISWHITE(വരി_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(വരി_org[ei_org])
					    || VIM_ISWHITE(വരി_new[ei_new]))))
		    {
			ആകുമ്പോൾ (ei_org >= *startp
					     && VIM_ISWHITE(വരി_org[ei_org]))
			    --ei_org;
			ആകുമ്പോൾ (ei_new >= si_new
					     && VIM_ISWHITE(വരി_new[ei_new]))
			    --ei_new;
		    }
		    അല്ലെങ്കിൽ
		    {
			p1 = വരി_org + ei_org;
			p2 = വരി_new + ei_new;
			p1 -= (*mb_head_off)(വരി_org, p1);
			p2 -= (*mb_head_off)(വരി_new, p2);
			എങ്കിൽ (!diff_equal_char(p1, p2, &l))
			    നിർത്തുക;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		എങ്കിൽ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(വരി_org);
    തിരികെ added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
തരനിർവചനം ഘടന
{
    long byte_start;
    long അക്കം_bytes;
    പൂർണ്ണസംഖ്യ lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    സ്ഥിതം ശൂന്യം
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, പൂർണ്ണസംഖ്യ idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    പൂർണ്ണസംഖ്യ pass = 1;
    ചെയ്യുക
    {
	പൂർണ്ണസംഖ്യ has_unmerged_gaps = FALSE;
	പൂർണ്ണസംഖ്യ has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	ആകുമ്പോൾ (dp!= ശൂന്യ && dp->df_next != ശൂന്യ)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    എങ്കിൽ (dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		തുടരുക;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    എങ്കിൽ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		തുടരുക;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1]);
	    എങ്കിൽ (gap <= 3)
	    {
		linenr_T max_df_എണ്ണം = 0;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
		    max_df_എണ്ണം = MAX(max_df_എണ്ണം, dp->df_എണ്ണം[i] + dp->df_next->df_എണ്ണം[i]);

		എങ്കിൽ (max_df_എണ്ണം >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
		    {
			dp->df_എണ്ണം[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_എണ്ണം[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    തുടരുക;
		}
		അല്ലെങ്കിൽ
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	എങ്കിൽ (!has_unmerged_gaps || !has_merged_gaps)
	    നിർത്തുക;
    } ആകുമ്പോൾ (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    സ്ഥിതം ശൂന്യം
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, പൂർണ്ണസംഖ്യ idx1,
				  linenr_T start_lnum)
{
    പൂർണ്ണസംഖ്യ pass = 1;
    ചെയ്യുക
    {
	diff_T *dp = dp_orig;

	ആകുമ്പോൾ (dp != ശൂന്യ && dp->df_next != ശൂന്യ)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    എങ്കിൽ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		തുടരുക;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    എങ്കിൽ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		തുടരുക;
	    }

	    // Calculate the gap between blocks
	    പൂർണ്ണസംഖ്യ gap_start = entry1->byte_start + entry1->അക്കം_bytes;
	    പൂർണ്ണസംഖ്യ gap_end = entry2->byte_start;
	    പൂർണ്ണസംഖ്യ gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    എങ്കിൽ (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		തുടരുക;
	    }

	    // Get the text between the two blocks
	    char_u *വരി = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = വരി + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		പൂർണ്ണസംഖ്യ char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		എങ്കിൽ (char_class == 2)
		{
		    only_non_word = false;
		    നിർത്തുക;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    എങ്കിൽ (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
		{
		    എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
		    {
			// count bytes in the first block
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < dp->df_എണ്ണം[i]; k++)
			{
			    പൂർണ്ണസംഖ്യ idx = dp->df_lnum[i] + k - 1;
			    എങ്കിൽ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].അക്കം_bytes;
			}
			// count bytes in the next block
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < dp->df_next->df_എണ്ണം[i]; k++)
			{
			    പൂർണ്ണസംഖ്യ idx = dp->df_next->df_lnum[i] + k - 1;
			    എങ്കിൽ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].അക്കം_bytes;
			}
		    }
		}

		എങ്കിൽ (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
		    {
			എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
			{
			    dp->df_എണ്ണം[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_എണ്ണം[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    തുടരുക;
		}
	    }

	    dp = dp->df_next;
	}
    } ആകുമ്പോൾ (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    സ്ഥിതം ശൂന്യം
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_എണ്ണം];
    garray_T	file1_str;
    garray_T	file2_str;
    പൂർണ്ണസംഖ്യ		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, വലുപ്പം(അക്ഷരം *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = ശൂന്യ;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_എണ്ണം]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, വലുപ്പം(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
	ga_init2(&linemap[i], വലുപ്പം(linemap_entry_T), 128);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
	    തുടരുക; // skip buffer that isn't loaded

	എങ്കിൽ (dp->df_എണ്ണം[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = ശൂന്യ;
	    തുടരുക;
	}

	എങ്കിൽ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	വേണ്ടി (പൂർണ്ണസംഖ്യ off = 0; off < dp->df_എണ്ണം[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    പൂർണ്ണസംഖ്യ in_keyword = FALSE;

	    // iwhiteeol support vars
	    പൂർണ്ണസംഖ്യ last_white = FALSE;
	    പൂർണ്ണസംഖ്യ eol_ga_len = -1;
	    പൂർണ്ണസംഖ്യ eol_linemap_len = -1;
	    പൂർണ്ണസംഖ്യ eol_numlines = -1;

	    char_u *s;
	    വേണ്ടി (s = curline; *s != NUL;)
	    {
		പൂർണ്ണസംഖ്യ new_in_keyword = FALSE;
		എങ്കിൽ (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		എങ്കിൽ (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		എങ്കിൽ (VIM_ISWHITE(*s))
		{
		    എങ്കിൽ (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			തുടരുക;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			എങ്കിൽ (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		പൂർണ്ണസംഖ്യ char_len = 1;
		എങ്കിൽ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		അല്ലെങ്കിൽ
		{
		    char_len = mb_ptr2len(s);

		    എങ്കിൽ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    എങ്കിൽ (diff_flags & DIFF_ICASE)
		    {
			പൂർണ്ണസംഖ്യ c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			പൂർണ്ണസംഖ്യ c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			പൂർണ്ണസംഖ്യ c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			എങ്കിൽ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    അല്ലെങ്കിൽ
			ga_concat_len(curstr, s, char_len);
		}

		എങ്കിൽ (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		എങ്കിൽ (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.അക്കം_bytes = char_len;
		    എങ്കിൽ (ga_grow(&linemap[i], 1) != OK)
			പോകുക done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		അല്ലെങ്കിൽ
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].അക്കം_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    എങ്കിൽ (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    എങ്കിൽ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		എങ്കിൽ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    എങ്കിൽ (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.അക്കം_bytes = വലുപ്പം(NL);
		എങ്കിൽ (ga_grow(&linemap[i], 1) != OK)
		    പോകുക done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	എങ്കിൽ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (അക്ഷരം *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	അല്ലെങ്കിൽ
	{
	    dio.dio_orig.din_mmfile.ptr = (അക്ഷരം *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	എങ്കിൽ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    പൂർണ്ണസംഖ്യ diff_status = diff_file_internal(&dio);
	    എങ്കിൽ (diff_status == FAIL)
		പോകുക done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    എങ്കിൽ (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    അല്ലെങ്കിൽ എങ്കിൽ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    വേണ്ടി (; new_diff != ശൂന്യ; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
	{
	    എങ്കിൽ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		തുടരുക;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_എണ്ണം[i];

	    എങ്കിൽ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    അല്ലെങ്കിൽ
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    എങ്കിൽ (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    അല്ലെങ്കിൽ
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].അക്കം_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	എങ്കിൽ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    പോകുക done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, വലുപ്പം(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    പൂർണ്ണസംഖ്യ
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		off;

    idx = diff_buf_idx(wp->w_buffer);
    എങ്കിൽ (idx == DB_എണ്ണം)	// cannot happen
	തിരികെ FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	എങ്കിൽ (lnum < dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
	    നിർത്തുക;
    എങ്കിൽ (dp == ശൂന്യ || diff_check_sanity(curtab, dp) == FAIL)
	തിരികെ FALSE;

    എങ്കിൽ (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	തിരികെ FALSE;
    off = lnum - dp->df_lnum[idx];

    എങ്കിൽ (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	പൂർണ്ണസംഖ്യ	change_start = MAXCOL;	// first col of changed area
	പൂർണ്ണസംഖ്യ	change_end = -1;	// last col of changed area
	പൂർണ്ണസംഖ്യ	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->അക്കം_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	തിരികെ ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    എങ്കിൽ (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    പൂർണ്ണസംഖ്യ അക്കം_changes = 0;
    പൂർണ്ണസംഖ്യ change_idx = 0;
    diffline->changes = ശൂന്യ;
    വേണ്ടി (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	എങ്കിൽ (change->dc_end_lnum_off[idx] < off)
	    തുടരുക;
	എങ്കിൽ (change->dc_start_lnum_off[idx] > off)
	    നിർത്തുക;
	എങ്കിൽ (diffline->changes == ശൂന്യ)
	    diffline->changes = change;
	അക്കം_changes++;
    }
    diffline->അക്കം_changes = അക്കം_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    പൂർണ്ണസംഖ്യ added = FALSE;
    എങ്കിൽ (അക്കം_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < DB_എണ്ണം; i++)
	{
	    എങ്കിൽ (idx == i)
		തുടരുക;
	    എങ്കിൽ (curtab->tp_diffbuf[i] == ശൂന്യ)
		തുടരുക;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    എങ്കിൽ (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		നിർത്തുക;
	    }
	}
    }
    തിരികെ added;
}

# എങ്കിൽ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    പൂർണ്ണസംഖ്യ
diff_infold(win_T *wp, linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		idx = -1;
    പൂർണ്ണസംഖ്യ		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    എങ്കിൽ (!wp->w_p_diff)
	തിരികെ FALSE;

    വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
    {
	എങ്കിൽ (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	അല്ലെങ്കിൽ എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    എങ്കിൽ (idx == -1 || !other)
	തിരികെ FALSE;

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    എങ്കിൽ (curtab->tp_first_diff == ശൂന്യ)
	തിരികെ TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = ശൂന്യ;		// last block with df_lnum[idx] <= lnum
    പൂർണ്ണസംഖ്യ		result = TRUE;
    വേണ്ടി (dp = diff_scan_start(curtab, idx, lnum); dp != ശൂന്യ; dp = dp->df_next)
    {
	എങ്കിൽ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	എങ്കിൽ (dp->df_lnum[idx] - diff_context > lnum)
	    നിർത്തുക;
	// If this change ends before the line we have a match.
	എങ്കിൽ (dp->df_lnum[idx] + dp->df_എണ്ണം[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    നിർത്തുക;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    തിരികെ result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    ശൂന്യം
nv_diffgetput(പൂർണ്ണസംഖ്യ put, long എണ്ണം)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	തിരികെ;
    }
# endif
    എങ്കിൽ (എണ്ണം == 0)
	ea.arg = (char_u *)"";
    അല്ലെങ്കിൽ
    {
	vim_snprintf((അക്ഷരം *)buf, 30, "%ld", എണ്ണം);
	ea.arg = buf;
    }
    എങ്കിൽ (put)
	ea.cmdidx = CMD_diffput;
    അല്ലെങ്കിൽ
	ea.cmdidx = CMD_diffget;
    ea.addr_എണ്ണം = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	എങ്കിൽ (dp == diff)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    ശൂന്യം
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    പൂർണ്ണസംഖ്യ		idx_cur;
    പൂർണ്ണസംഖ്യ		idx_other;
    പൂർണ്ണസംഖ്യ		idx_from;
    പൂർണ്ണസംഖ്യ		idx_to;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		start_skip, end_skip;
    പൂർണ്ണസംഖ്യ		new_എണ്ണം;
    പൂർണ്ണസംഖ്യ		buf_empty;
    പൂർണ്ണസംഖ്യ		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    എങ്കിൽ (idx_cur == DB_എണ്ണം)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	തിരികെ;
    }

    എങ്കിൽ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	വേണ്ടി (idx_other = 0; idx_other < DB_എണ്ണം; ++idx_other)
	    എങ്കിൽ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != ശൂന്യ)
	    {
		എങ്കിൽ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    നിർത്തുക;
		found_not_ma = TRUE;
	    }
	എങ്കിൽ (idx_other == DB_എണ്ണം)
	{
	    എങ്കിൽ (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    അല്ലെങ്കിൽ
		emsg(_(e_no_other_buffer_in_diff_mode));
	    തിരികെ;
	}

	// Check that there isn't a third buffer in the list
	വേണ്ടി (i = idx_other + 1; i < DB_എണ്ണം; ++i)
	    എങ്കിൽ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != ശൂന്യ
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		തിരികെ;
	    }
    }
    അല്ലെങ്കിൽ
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	ആകുമ്പോൾ (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	വേണ്ടി (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	എങ്കിൽ (eap->arg + i == p)	    // digits only
	    i = atol((അക്ഷരം *)eap->arg);
	അല്ലെങ്കിൽ
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    എങ്കിൽ (i < 0)
		തിരികെ;		// error message already given
	}
	buf = buflist_findnr(i);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    തിരികെ;
	}
	എങ്കിൽ (buf == curbuf)
	    തിരികെ;		// nothing to do
	idx_other = diff_buf_idx(buf);
	എങ്കിൽ (idx_other == DB_എണ്ണം)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    തിരികെ;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    എങ്കിൽ (eap->addr_എണ്ണം == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	പൂർണ്ണസംഖ്യ linestatus = 0;
	എങ്കിൽ (eap->line1 == curbuf->b_ml.ml_വരി_എണ്ണം
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	അല്ലെങ്കിൽ എങ്കിൽ (eap->line1 > 0)
	    --eap->line1;
    }

    എങ്കിൽ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    അല്ലെങ്കിൽ
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	എങ്കിൽ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    പോകുക theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    എങ്കിൽ (!curbuf->b_changed)
    {
	change_warning(0);
	എങ്കിൽ (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    പോകുക theend;
	}
    }

    dprev = ശൂന്യ;
    വേണ്ടി (dp = curtab->tp_first_diff; dp != ശൂന്യ; )
    {
	എങ്കിൽ (!eap->addr_എണ്ണം)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    ആകുമ്പോൾ (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_എണ്ണം[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	എങ്കിൽ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    നിർത്തുക;	// past the range that was specified

	dfree = ശൂന്യ;
	lnum = dp->df_lnum[idx_to];
	എണ്ണം = dp->df_എണ്ണം[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + എണ്ണം + (എണ്ണം == 0 && BUFEMPTY() ? 1 : 0);

	എങ്കിൽ (dp->df_lnum[idx_cur] + dp->df_എണ്ണം[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    എങ്കിൽ (eap->addr_എണ്ണം > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		എങ്കിൽ (start_skip > 0)
		{
		    // range starts below start of current diff block
		    എങ്കിൽ (start_skip > എണ്ണം)
		    {
			lnum += എണ്ണം;
			എണ്ണം = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			എണ്ണം -= start_skip;
			lnum += start_skip;
		    }
		}
		അല്ലെങ്കിൽ
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_എണ്ണം[idx_cur] - 1
							 - (eap->line2 + off);
		എങ്കിൽ (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    എങ്കിൽ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_എണ്ണം[idx_cur] - start_skip - end_skip;
			എങ്കിൽ (എണ്ണം > i)
			    എണ്ണം = i;
		    }
		    അല്ലെങ്കിൽ			// :diffget
		    {
			എണ്ണം -= end_skip;
			end_skip = dp->df_എണ്ണം[idx_from] - start_skip - എണ്ണം;
			എങ്കിൽ (end_skip < 0)
			    end_skip = 0;
		    }
		}
		അല്ലെങ്കിൽ
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_വരി_എണ്ണം == 1;
		എങ്കിൽ (ml_delete(lnum) == OK)
		    --added;
	    }
	    വേണ്ടി (i = 0; i < dp->df_എണ്ണം[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		എങ്കിൽ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_വരി_എണ്ണം)
		    നിർത്തുക;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		എങ്കിൽ (p != ശൂന്യ)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    എങ്കിൽ (buf_empty && curbuf->b_ml.ml_വരി_എണ്ണം == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_എണ്ണം = dp->df_എണ്ണം[idx_to] + added;
	    dp->df_എണ്ണം[idx_to] = new_എണ്ണം;

	    എങ്കിൽ (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
		    എങ്കിൽ (curtab->tp_diffbuf[i] != ശൂന്യ && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			നിർത്തുക;
		എങ്കിൽ (i == DB_എണ്ണം)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    എങ്കിൽ (dprev == ശൂന്യ)
			curtab->tp_first_diff = dp;
		    അല്ലെങ്കിൽ
			dprev->df_next = dp;
		}
	    }

	    എങ്കിൽ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + എണ്ണം - 1, (long)MAXLNUM, (long)added);
		എങ്കിൽ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    എങ്കിൽ (curwin->w_cursor.lnum >= lnum + എണ്ണം)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_വരി_എണ്ണം);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_വരികൾ(lnum, 0, lnum + എണ്ണം, (long)added);

	    എങ്കിൽ (dfree != ശൂന്യ)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    എങ്കിൽ (added != 0 && !valid_diff(dp))
		നിർത്തുക;

	    എങ്കിൽ (dfree == ശൂന്യ)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_എണ്ണം[idx_to] = new_എണ്ണം;

	    // When changing the current buffer, keep track of line numbers
	    എങ്കിൽ (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	എങ്കിൽ (dfree == ശൂന്യ)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    എങ്കിൽ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	എങ്കിൽ (താക്കോൽ_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    എങ്കിൽ (diff_need_update)
	ex_diffupdate(ശൂന്യ);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_വരി_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    എങ്കിൽ (curtab->tp_first_diff == ശൂന്യ)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    എങ്കിൽ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    എങ്കിൽ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    അല്ലെങ്കിൽ
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    സ്ഥിതം ശൂന്യം
diff_fold_update(diff_T *dp, പൂർണ്ണസംഖ്യ skip_idx)
{
    പൂർണ്ണസംഖ്യ		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	വേണ്ടി (i = 0; i < DB_എണ്ണം; ++i)
	    എങ്കിൽ (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_എണ്ണം[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    പൂർണ്ണസംഖ്യ
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	എങ്കിൽ (diff_buf_idx_tp(buf, tp) != DB_എണ്ണം)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    പൂർണ്ണസംഖ്യ
diff_move_to(പൂർണ്ണസംഖ്യ dir, long എണ്ണം)
{
    പൂർണ്ണസംഖ്യ		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    എങ്കിൽ (idx == DB_എണ്ണം || curtab->tp_first_diff == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    എങ്കിൽ (curtab->tp_first_diff == ശൂന്യ)		// no diffs today
	തിരികെ FAIL;

    ആകുമ്പോൾ (--എണ്ണം >= 0)
    {
	// Check if already before first diff.
	എങ്കിൽ (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    നിർത്തുക;

	വേണ്ടി (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    എങ്കിൽ (dp == ശൂന്യ)
		നിർത്തുക;
	    എങ്കിൽ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == ശൂന്യ
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		നിർത്തുക;
	    }
	}
    }

    // don't end up past the end of the file
    എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	lnum = curbuf->b_ml.ml_വരി_എണ്ണം;

    // When the cursor didn't move at all we fail.
    എങ്കിൽ (lnum == curwin->w_cursor.lnum)
	തിരികെ FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    തിരികെ OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    സ്ഥിതം linenr_T
diff_get_corresponding_വരി_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    പൂർണ്ണസംഖ്യ		idx1;
    പൂർണ്ണസംഖ്യ		idx2;
    diff_T	*dp;
    പൂർണ്ണസംഖ്യ		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    എങ്കിൽ (idx1 == DB_എണ്ണം || idx2 == DB_എണ്ണം || curtab->tp_first_diff == ശൂന്യ)
	തിരികെ lnum1;

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    എങ്കിൽ (curtab->tp_first_diff == ശൂന്യ)		// no diffs today
	തിരികെ lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	എങ്കിൽ (dp->df_lnum[idx1] > lnum1)
	    തിരികെ lnum1 - baseline;
	എങ്കിൽ ((dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    എങ്കിൽ (baseline > dp->df_എണ്ണം[idx2])
		baseline = dp->df_എണ്ണം[idx2];

	    തിരികെ dp->df_lnum[idx2] + baseline;
	}
	എങ്കിൽ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_എണ്ണം[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_എണ്ണം[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    തിരികെ curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_എണ്ണം[idx1])
				   - (dp->df_lnum[idx2] + dp->df_എണ്ണം[idx2]);
    }

    // If we get here then the cursor is after the last diff
    തിരികെ lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_വരി(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_വരി_int(buf1, lnum1);

    // don't end up past the end of the file
    എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ curbuf->b_ml.ml_വരി_എണ്ണം;
    തിരികെ lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    എങ്കിൽ (idx == DB_എണ്ണം)		// safety check
	തിരികെ (linenr_T)0;

    എങ്കിൽ (curtab->tp_diff_invalid)
	ex_diffupdate(ശൂന്യ);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	എങ്കിൽ (lnum <= dp->df_lnum[idx] + dp->df_എണ്ണം[idx])
	    നിർത്തുക;

    // When after the last change, compute relative to the last line number.
    എങ്കിൽ (dp == ശൂന്യ)
	തിരികെ wp->w_buffer->b_ml.ml_വരി_എണ്ണം
					- (curbuf->b_ml.ml_വരി_എണ്ണം - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    എങ്കിൽ (i == DB_എണ്ണം)			// safety check
	തിരികെ (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    എങ്കിൽ (n > dp->df_lnum[i] + dp->df_എണ്ണം[i])
	n = dp->df_lnum[i] + dp->df_എണ്ണം[i];
    തിരികെ n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_diff_ed(
	char_u	    *വരി,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    പൂർണ്ണസംഖ്യ	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = വരി;
    f1 = getdigits(&p);
    എങ്കിൽ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    അല്ലെങ്കിൽ
	l1 = f1;
    എങ്കിൽ (*p != 'a' && *p != 'c' && *p != 'd')
	തിരികെ FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    എങ്കിൽ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    അല്ലെങ്കിൽ
	l2 = f2;
    എങ്കിൽ (l1 < f1 || l2 < f2)
	തിരികെ FAIL;

    എങ്കിൽ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->എണ്ണം_orig = 0;
    }
    അല്ലെങ്കിൽ
    {
	hunk->lnum_orig = f1;
	hunk->എണ്ണം_orig = l1 - f1 + 1;
    }
    എങ്കിൽ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->എണ്ണം_new = 0;
    }
    അല്ലെങ്കിൽ
    {
	hunk->lnum_new = f2;
	hunk->എണ്ണം_new = l2 - f2 + 1;
    }
    തിരികെ OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_diff_unified(
	char_u	    *വരി,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = വരി;
    എങ്കിൽ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	എങ്കിൽ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	അല്ലെങ്കിൽ
	    oldcount = 1;
	എങ്കിൽ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    എങ്കിൽ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    അല്ലെങ്കിൽ
		newcount = 1;
	}
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;	// invalid diff format

	എങ്കിൽ (oldcount == 0)
	    oldline += 1;
	എങ്കിൽ (newcount == 0)
	    newline += 1;
	എങ്കിൽ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->എണ്ണം_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->എണ്ണം_new = newcount;

	തിരികെ OK;
    }

    തിരികെ FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
xdiff_out_indices(
	long start_a,
	long എണ്ണം_a,
	long start_b,
	long എണ്ണം_b,
	ശൂന്യം *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	തിരികെ -1;
    }

    p->lnum_orig  = start_a + 1;
    p->എണ്ണം_orig = എണ്ണം_a;
    p->lnum_new   = start_b + 1;
    p->എണ്ണം_new  = എണ്ണം_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    തിരികെ 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
xdiff_out_unified(
	ശൂന്യം *priv,
	mmbuffer_t *mb,
	പൂർണ്ണസംഖ്യ nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    തിരികെ 0;
}

#endif	// FEAT_DIFF

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    ശൂന്യം
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    ശൂന്യം
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    സ്ഥിതം linenr_T	prev_lnum = 0;
    സ്ഥിതം varnumber_T	changedtick = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		fnum = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		prev_diff_flags = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		change_start = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		change_end = 0;
    സ്ഥിതം hlf_T	hlID = (hlf_T)0;
    പൂർണ്ണസംഖ്യ			cache_results = TRUE;
    പൂർണ്ണസംഖ്യ			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    എങ്കിൽ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	പൂർണ്ണസംഖ്യ linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	എങ്കിൽ (linestatus < 0)
	{
	    എങ്കിൽ (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		എങ്കിൽ (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		അല്ലെങ്കിൽ
		{
		    hlID = HLF_CHD;	// changed line
		    എങ്കിൽ (diffline.അക്കം_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    അല്ലെങ്കിൽ
		hlID = HLF_ADD;	// added line
	}
	അല്ലെങ്കിൽ
	    hlID = (hlf_T)0;

	എങ്കിൽ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    എങ്കിൽ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_സംഖ്യ(&argvars[1]) - 1; // ignore type error in {col}
	എങ്കിൽ (cache_results)
	{
	    എങ്കിൽ (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    അല്ലെങ്കിൽ
		hlID = HLF_CHD;			// changed line
	}
	അല്ലെങ്കിൽ
	{
	    hlID = HLF_CHD;
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < diffline.അക്കം_changes; i++)
	    {
		പൂർണ്ണസംഖ്യ added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		എങ്കിൽ (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    നിർത്തുക;
		}
		എങ്കിൽ (col < change_start)
		    // the remaining changes are past this column and not relevant
		    നിർത്തുക;
	    }
	}
    }
    rettv->vval.v_സംഖ്യ = hlID == (hlf_T)0 ? 0 : (പൂർണ്ണസംഖ്യ)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_diff_optarg(
    typval_T	    *opts,
    പൂർണ്ണസംഖ്യ		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    പൂർണ്ണസംഖ്യ		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    എങ്കിൽ (algo != ശൂന്യ)
    {
	എങ്കിൽ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    എങ്കിൽ (output_fmt != ശൂന്യ)
    {
	എങ്കിൽ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    തിരികെ FAIL;
	}
    }

    *diff_ctxlen = dict_get_സംഖ്യ_def(d, "context", 0);
    എങ്കിൽ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    എങ്കിൽ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    എങ്കിൽ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    എങ്കിൽ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    എങ്കിൽ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    എങ്കിൽ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    എങ്കിൽ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    തിരികെ OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    സ്ഥിതം ശൂന്യം
list_to_diffin(list_T *l, diffin_T *din, പൂർണ്ണസംഖ്യ icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	str = tv_get_string(&li->li_tv);
	എങ്കിൽ (icase)
	{
	    str = strlow_save(str);
	    എങ്കിൽ (str == ശൂന്യ)
		തുടരുക;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	എങ്കിൽ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (അക്ഷരം *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    സ്ഥിതം dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    എങ്കിൽ (hunk_dict == ശൂന്യ)
	തിരികെ ശൂന്യ;

    dict_add_സംഖ്യ(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_സംഖ്യ(hunk_dict, "from_count", hunk->എണ്ണം_orig);
    dict_add_സംഖ്യ(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_സംഖ്യ(hunk_dict, "to_count", hunk->എണ്ണം_new);

    തിരികെ hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    ശൂന്യം
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    എങ്കിൽ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	തിരികെ;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, വലുപ്പം(അക്ഷരം *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    പൂർണ്ണസംഖ്യ		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	എങ്കിൽ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    തിരികെ;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    പൂർണ്ണസംഖ്യ	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    എങ്കിൽ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    പൂർണ്ണസംഖ്യ diff_status = diff_file(&dio);

    // restore 'diffexpr'
    എങ്കിൽ (restore_diffexpr)
	*p_dex = cc;

    എങ്കിൽ (diff_status == FAIL)
	പോകുക done;

    പൂർണ്ണസംഖ്യ		hunk_idx = 0;
    dict_T	*hunk_dict;

    എങ്കിൽ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	എങ്കിൽ (rettv_list_alloc(rettv) != OK)
	    പോകുക done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = ശൂന്യ;
	ആകുമ്പോൾ (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    എങ്കിൽ (hunk_dict == ശൂന്യ)
		പോകുക done;

	    എങ്കിൽ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    അല്ലെങ്കിൽ
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    എങ്കിൽ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    അല്ലെങ്കിൽ
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
