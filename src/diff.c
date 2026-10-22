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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "xdiff/xdiff.h"

#जइ defined(FEAT_DIFF)

स्थिर पूर्णाङ्क diff_busy = FALSE;	    // using diff structs, don't change them
स्थिर पूर्णाङ्क diff_need_update = FALSE; // ex_diffupdate needs to be called

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
स्थिर पूर्णाङ्क	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

स्थिर long diff_algorithm = XDF_INDENT_HEURISTIC;
स्थिर पूर्णाङ्क diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

स्थिर पूर्णाङ्क diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# जइ defined(MSWIN)
स्थिर पूर्णाङ्क diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
प्रकारनिर्देश संरचना {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
प्रकारनिर्देश संरचना {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
प्रकारनिर्देश संरचना {
    linenr_T lnum_orig;
    long     गणनम्_orig;
    linenr_T lnum_new;
    long     गणनम्_new;
} diffhunk_T;

प्रकारनिर्देश गणना {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
प्रकारनिर्देश संरचना {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    पूर्णाङ्क		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    पूर्णाङ्क		    dio_ctxlen;   // unified diff context length
} diffio_T;

स्थिर पूर्णाङ्क diff_buf_idx(buf_T *buf);
स्थिर पूर्णाङ्क diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
स्थिर शून्य diff_mark_adjust_tp(tabpage_T *tp, पूर्णाङ्क idx, linenr_T line1, linenr_T line2, long amount, long amount_after);
स्थिर शून्य diff_check_unchanged(tabpage_T *tp, diff_T *dp);
स्थिर पूर्णाङ्क diff_check_sanity(tabpage_T *tp, diff_T *dp);
स्थिर पूर्णाङ्क check_external_diff(diffio_T *diffio);
स्थिर पूर्णाङ्क diff_file(diffio_T *diffio);
स्थिर पूर्णाङ्क diff_equal_entry(diff_T *dp, पूर्णाङ्क idx1, पूर्णाङ्क idx2);
स्थिर पूर्णाङ्क diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
स्थिर शून्य diff_fold_update(diff_T *dp, पूर्णाङ्क skip_idx);
# endif
स्थिर शून्य diff_read(पूर्णाङ्क idx_orig, पूर्णाङ्क idx_new, diffio_T *dio);
स्थिर शून्य diff_copy_entry(diff_T *dprev, diff_T *dp, पूर्णाङ्क idx_orig, पूर्णाङ्क idx_new);
स्थिर diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
स्थिर पूर्णाङ्क parse_diff_ed(char_u *पङ्क्तिः, diffhunk_T *hunk);
स्थिर पूर्णाङ्क parse_diff_unified(char_u *पङ्क्तिः, diffhunk_T *hunk);
स्थिर पूर्णाङ्क xdiff_out_indices(long start_a, long गणनम्_a, long start_b, long गणनम्_b, शून्य *priv);
स्थिर पूर्णाङ्क xdiff_out_unified(शून्य *priv, mmbuffer_t *mb, पूर्णाङ्क nbuf);
स्थिर पूर्णाङ्क parse_diffanchors(पूर्णाङ्क check_only, buf_T *buf, linenr_T *anchors, पूर्णाङ्क *अङ्कः_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    प्रति ((dp) = (tp)->tp_first_diff; (dp) != रिक्त; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
स्थिर diff_T	    *diff_finger_dp = रिक्त;
स्थिर tabpage_T    *diff_finger_tp = रिक्त;
स्थिर पूर्णाङ्क	    diff_finger_idx = -1;
स्थिर linenr_T	    diff_finger_lnum = 0;

    स्थिर शून्य
clear_diffblock(diff_T *dp)
{
    जइ (dp == diff_finger_dp)
	diff_finger_dp = रिक्त;		// don't leave a dangling finger
    ga_clear(&dp->df_changes);
    vim_free(dp);
}

/*
 * Return the diff block to start a forward scan for line "lnum" in tab "tp",
 * buffer index "idx".  Resumes from the finger when it is still valid and
 * "lnum" did not move backwards, else starts at the first block.
 */
    स्थिर diff_T *
diff_scan_start(tabpage_T *tp, पूर्णाङ्क idx, linenr_T lnum)
{
    जइ (diff_finger_dp != रिक्त
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	प्रत्यागम diff_finger_dp;
    प्रत्यागम tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    स्थिर शून्य
diff_scan_remember(tabpage_T *tp, पूर्णाङ्क idx, linenr_T lnum, diff_T *dp)
{
    diff_finger_tp = tp;
    diff_finger_idx = idx;
    diff_finger_lnum = lnum;
    diff_finger_dp = dp != रिक्त ? dp : tp->tp_first_diff;
}

/*
 * Called when deleting or unloading a buffer: No longer make a diff with it.
 */
    शून्य
diff_buf_delete(buf_T *buf)
{
    पूर्णाङ्क		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	जइ (i != DB_गणनम्)
	{
	    tp->tp_diffbuf[i] = रिक्त;
	    tp->tp_diff_invalid = TRUE;
	    जइ (tp == curtab)
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
    शून्य
diff_buf_adjust(win_T *win)
{
    win_T	*wp;
    पूर्णाङ्क		i;

    जइ (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    जइ (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		भङ्ग;
	जइ (wp == रिक्त)
	{
	    i = diff_buf_idx(win->w_buffer);
	    जइ (i != DB_गणनम्)
	    {
		curtab->tp_diffbuf[i] = रिक्त;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    अण्णहा
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
    शून्य
diff_buf_add(buf_T *buf)
{
    पूर्णाङ्क		i;

    जइ (diff_buf_idx(buf) != DB_गणनम्)
	प्रत्यागम;		// It's already there.

    प्रति (i = 0; i < DB_गणनम्; ++i)
	जइ (curtab->tp_diffbuf[i] == रिक्त)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    प्रत्यागम;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_गणनम्);
}

/*
 * Remove all buffers to make diffs for.
 */
    स्थिर शून्य
diff_buf_clear(शून्य)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < DB_गणनम्; ++i)
	जइ (curtab->tp_diffbuf[i] != रिक्त)
	{
	    curtab->tp_diffbuf[i] = रिक्त;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	}
}

/*
 * Find buffer "buf" in the list of diff buffers for the current tab page.
 * Return its index or DB_COUNT if not found.
 */
    स्थिर पूर्णाङ्क
diff_buf_idx(buf_T *buf)
{
    पूर्णाङ्क		idx;

    प्रति (idx = 0; idx < DB_गणनम्; ++idx)
	जइ (curtab->tp_diffbuf[idx] == buf)
	    भङ्ग;
    प्रत्यागम idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    स्थिर पूर्णाङ्क
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    पूर्णाङ्क		idx;

    प्रति (idx = 0; idx < DB_गणनम्; ++idx)
	जइ (tp->tp_diffbuf[idx] == buf)
	    भङ्ग;
    प्रत्यागम idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    शून्य
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    पूर्णाङ्क		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	जइ (i != DB_गणनम्)
	{
	    tp->tp_diff_invalid = TRUE;
	    जइ (tp == curtab)
		diff_redraw(TRUE);
	}
    }
}

/*
 * Called by mark_adjust(): update line numbers in "curbuf".
 */
    शून्य
diff_mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    पूर्णाङ्क		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	जइ (idx != DB_गणनम्)
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
    स्थिर शून्य
diff_mark_adjust_tp(
    tabpage_T	*tp,
    पूर्णाङ्क		idx,
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    पूर्णाङ्क		i;
    पूर्णाङ्क		inserted, deleted;
    पूर्णाङ्क		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    पूर्णाङ्क		check_unchanged;

    diff_finger_dp = रिक्त;		// may shift df_lnum, invalidate the finger

    जइ (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    जइ (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    अण्णहा जइ (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    अण्णहा
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = रिक्त;
    dp = tp->tp_first_diff;
    प्रति (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	जइ ((dp == रिक्त || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == रिक्त
		    || dprev->df_lnum[idx] + dprev->df_गणनम्[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    जइ (dnext == रिक्त)
		प्रत्यागम;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_गणनम्[idx] = inserted;
	    प्रति (i = 0; i < DB_गणनम्; ++i)
		जइ (tp->tp_diffbuf[i] != रिक्त && i != idx)
		{
		    जइ (dprev == रिक्त)
			dnext->df_lnum[i] = line1;
		    अण्णहा
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_गणनम्[i])
			    - (dprev->df_lnum[idx] + dprev->df_गणनम्[idx]);
		    dnext->df_गणनम्[i] = deleted;
		}
	}

	// if at end of the list, quit
	जइ (dp == रिक्त)
	    भङ्ग;

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
	last = dp->df_lnum[idx] + dp->df_गणनम्[idx] - 1;

	// 1. change completely above line1: nothing to do
	जइ (last >= line1 - 1)
	{
	    जइ (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		जइ (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		अनुवर्त;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    जइ (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		जइ (amount_after == 0)
		    भङ्ग;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    अण्णहा
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		जइ (deleted > 0)
		{
		    off = 0;
		    जइ (dp->df_lnum[idx] >= line1)
		    {
			जइ (last <= line2)
			{
			    // 4. delete all lines of diff
			    जइ (dp->df_next != रिक्त
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_गणनम्[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    अण्णहा
				n = deleted - dp->df_गणनम्[idx];
			    dp->df_गणनम्[idx] = 0;
			}
			अण्णहा
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_गणनम्[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    अण्णहा
		    {
			जइ (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_गणनम्[idx] -= last - lnum_deleted + 1;
			    जइ (dp->df_next != रिक्त
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    अण्णहा
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			अण्णहा
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_गणनम्[idx] -= deleted;
			}
		    }

		    प्रति (i = 0; i < DB_गणनम्; ++i)
			जइ (tp->tp_diffbuf[i] != रिक्त && i != idx)
			{
			    जइ (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    अण्णहा
				dp->df_lnum[i] = 1;
			    dp->df_गणनम्[i] += n;
			}
		}
		अण्णहा
		{
		    जइ (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_गणनम्[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    अण्णहा
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		जइ (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	जइ (dprev != रिक्त && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_गणनम्[idx]
							== dp->df_lnum[idx])
	{
	    प्रति (i = 0; i < DB_गणनम्; ++i)
		जइ (tp->tp_diffbuf[i] != रिक्त)
		    dprev->df_गणनम्[i] += dp->df_गणनम्[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	अण्णहा
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = रिक्त;
    dp = tp->tp_first_diff;
    जाव (dp != रिक्त)
    {
	// All counts are zero, remove this entry.
	प्रति (i = 0; i < DB_गणनम्; ++i)
	    जइ (tp->tp_diffbuf[i] != रिक्त && dp->df_गणनम्[i] != 0)
		भङ्ग;
	जइ (i == DB_गणनम्)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    जइ (dprev == रिक्त)
		tp->tp_first_diff = dnext;
	    अण्णहा
		dprev->df_next = dnext;
	}
	अण्णहा
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    जइ (tp == curtab)
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
    स्थिर diff_T *
diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp)
{
    diff_T	*dnew;

    dnew = ALLOC_CLEAR_ONE(diff_T);
    जइ (dnew == रिक्त)
	प्रत्यागम रिक्त;
    diff_finger_dp = रिक्त;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    जइ (dprev == रिक्त)
	tp->tp_first_diff = dnew;
    अण्णहा
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, परिमाण(diffline_change_T), 20);
    प्रत्यागम dnew;
}

/*
 * Check if the diff block "dp" can be made smaller for lines at the start and
 * end that are equal.  Called after inserting lines.
 * This may result in a change where all buffers have zero lines, the caller
 * must take care of removing it.
 */
    स्थिर शून्य
diff_check_unchanged(tabpage_T *tp, diff_T *dp)
{
    पूर्णाङ्क		i_org;
    पूर्णाङ्क		i_new;
    पूर्णाङ्क		off_org, off_new;
    char_u	*पङ्क्तिः_org;
    पूर्णाङ्क		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    प्रति (i_org = 0; i_org < DB_गणनम्; ++i_org)
	जइ (tp->tp_diffbuf[i_org] != रिक्त)
	    भङ्ग;
    जइ (i_org == DB_गणनम्)	// safety check
	प्रत्यागम;

    जइ (diff_check_sanity(tp, dp) == FAIL)
	प्रत्यागम;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    प्रति (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	जाव (dp->df_गणनम्[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    जइ (dir == BACKWARD)
		off_org = dp->df_गणनम्[i_org] - 1;
	    पङ्क्तिः_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    जइ (पङ्क्तिः_org == रिक्त)
		प्रत्यागम;
	    प्रति (i_new = i_org + 1; i_new < DB_गणनम्; ++i_new)
	    {
		जइ (tp->tp_diffbuf[i_new] == रिक्त)
		    अनुवर्त;
		जइ (dir == BACKWARD)
		    off_new = dp->df_गणनम्[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		जइ (off_new < 0 || off_new >= dp->df_गणनम्[i_new])
		    भङ्ग;
		जइ (diff_cmp(पङ्क्तिः_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    भङ्ग;
	    }
	    vim_free(पङ्क्तिः_org);

	    // Stop when a line isn't equal in all diff buffers.
	    जइ (i_new != DB_गणनम्)
		भङ्ग;

	    // Line matched in all buffers, remove it from the diff.
	    प्रति (i_new = i_org; i_new < DB_गणनम्; ++i_new)
		जइ (tp->tp_diffbuf[i_new] != रिक्त)
		{
		    जइ (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_गणनम्[i_new];
		}
	}
	जइ (dir == BACKWARD)
	    भङ्ग;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    स्थिर पूर्णाङ्क
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < DB_गणनम्; ++i)
	जइ (tp->tp_diffbuf[i] != रिक्त)
	    जइ (dp->df_lnum[i] + dp->df_गणनम्[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_पङ्क्तिः_गणनम्)
		प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    शून्य
diff_redraw(
    पूर्णाङ्क		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = रिक्त;
    पूर्णाङ्क		used_max_fill_other = FALSE;
    पूर्णाङ्क		used_max_fill_curwin = FALSE;
    पूर्णाङ्क		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	जइ (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    अनुवर्त;

	redraw_win_later(wp, UPD_SOME_VALID);
	जइ (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	जइ (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	जइ ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    जइ (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    अण्णहा जइ (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		जइ (wp == curwin)
		    used_max_fill_curwin = TRUE;
		अण्णहा जइ (wp_other != रिक्त)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    जइ (wp_other != रिक्त && curwin->w_p_scb)
    {
	जइ (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	अण्णहा जइ (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    स्थिर शून्य
clear_diffin(diffin_T *din)
{
    जइ (din->din_fname == रिक्त)
	VIM_CLEAR(din->din_mmfile.ptr);
    अण्णहा
	mch_remove(din->din_fname);
}

    स्थिर शून्य
clear_diffout(diffout_T *dout)
{
    जइ (dout->dout_fname == रिक्त)
	ga_clear_strings(&dout->dout_ga);
    अण्णहा
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    स्थिर पूर्णाङ्क
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    long	len = 0;
    char_u	*ptr;

    जइ (end < 0)
	end = buf->b_ml.ml_पङ्क्तिः_गणनम्;

    जइ (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = रिक्त;
	din->din_mmfile.size = 0;
	प्रत्यागम OK;
    }

    // xdiff requires one big block of memory with all the text.
    प्रति (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    जइ (ptr == रिक्त)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	जइ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	प्रत्यागम FAIL;
    }
    din->din_mmfile.ptr = (वर्ण *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    प्रति (lnum = start; lnum <= end; ++lnum)
    {
	प्रति (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    जइ (diff_flags & DIFF_ICASE)
	    {
		पूर्णाङ्क c;
		पूर्णाङ्क	orig_len;
		पूर्णाङ्क	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		जइ (*s == NL)
		    c = NUL;
		अण्णहा
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		जइ (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		अण्णहा
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    जइ (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    अण्णहा
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    प्रत्यागम OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    स्थिर पूर्णाङ्क
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    पूर्णाङ्क		r;
    पूर्णाङ्क		save_ml_flags;
    char_u	*save_ff;
    पूर्णाङ्क		save_cmod_flags;

    जइ (din->din_fname == रिक्त)
	प्रत्यागम diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    जइ (frames_locked())
	प्रत्यागम FAIL;

    जइ (end < 0)
	end = buf->b_ml.ml_पङ्क्तिः_गणनम्;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    जइ (end < start)
    {
	// The line range specifies a completely empty file.
	end = start;
	buf->b_ml.ml_flags |= ML_EMPTY;
    }
    r = buf_write(buf, din->din_fname, रिक्त,
			start, end,
			रिक्त, FALSE, FALSE, FALSE, TRUE);
    cmdmod.cmod_flags = save_cmod_flags;
    free_string_option(buf->b_p_ff);
    buf->b_p_ff = save_ff;
    buf->b_ml.ml_flags =
		(buf->b_ml.ml_flags & ~ML_EMPTY) | (save_ml_flags & ML_EMPTY);
    प्रत्यागम r;
}

    स्थिर पूर्णाङ्क
lnum_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    जइ (lnum1 < lnum2)
	प्रत्यागम -1;
    जइ (lnum1 > lnum2)
	प्रत्यागम 1;
    प्रत्यागम 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    स्थिर शून्य
diff_try_update(
	diffio_T    *dio,
	पूर्णाङ्क	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    पूर्णाङ्क		idx_new;

    जइ (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, परिमाण(वर्ण *), 1000);
    }
    अण्णहा
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	जइ (dio->dio_orig.din_fname == रिक्त
		|| dio->dio_new.din_fname == रिक्त
		|| dio->dio_diff.dout_fname == रिक्त)
	    गच्छ theend;
    }

    // Check external diff is actually working.
    जइ (!dio->dio_internal && check_external_diff(dio) == FAIL)
	गच्छ theend;

    // :diffupdate!
    जइ (eap != रिक्त && eap->forceit)
	प्रति (idx_new = idx_orig; idx_new < DB_गणनम्; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    जइ (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    पूर्णाङ्क अङ्कः_anchors = INT_MAX;
    linenr_T anchors[DB_गणनम्][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    जइ (diff_flags & DIFF_ANCHOR)
    {
	प्रति (पूर्णाङ्क idx = 0; idx < DB_गणनम्; idx++)
	{
	    जइ (curtab->tp_diffbuf[idx] == रिक्त)
		अनुवर्त;
	    पूर्णाङ्क buf_अङ्कः_anchors = 0;
	    जइ (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_अङ्कः_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		अङ्कः_anchors = 0;
		CLEAR_FIELD(anchors);
		भङ्ग;
	    }
	    जइ (buf_अङ्कः_anchors < अङ्कः_anchors)
		अङ्कः_anchors = buf_अङ्कः_anchors;

	    जइ (buf_अङ्कः_anchors > 0)
		qsort((शून्य *)anchors[idx],
			(size_t)buf_अङ्कः_anchors,
			परिमाण(linenr_T),
			lnum_compare);
	}
    }
    जइ (अङ्कः_anchors == INT_MAX)
	अङ्कः_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    प्रति (पूर्णाङ्क anchor_i = 0; anchor_i <= अङ्कः_anchors; anchor_i++)
    {
	diff_T	*orig_diff = रिक्त;
	जइ (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = रिक्त;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == अङ्कः_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	जइ (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    जइ (orig_diff != रिक्त)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    गच्छ theend;
	}

	// Make a difference between the first buffer and every other.
	प्रति (idx_new = idx_orig + 1; idx_new < DB_गणनम्; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    जइ (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
		अनुवर्त; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == अङ्कः_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    जइ (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		अनुवर्त;
	    जइ (diff_file(dio) == FAIL)
		अनुवर्त;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	जइ (anchor_i != 0)
	{
	    diff_finger_dp = रिक्त;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    प्रति (diff_T *dp = curtab->tp_first_diff; dp != रिक्त; dp = dp->df_next)
	    {
		प्रति (पूर्णाङ्क idx = 0; idx < DB_गणनम्; idx++)
		{
		    जइ (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    जइ (orig_diff != रिक्त)
	    {
		diff_T *last_diff = orig_diff;
		जाव (last_diff->df_next != रिक्त)
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
    पूर्णाङ्क
diff_internal(शून्य)
{
    प्रत्यागम (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    स्थिर पूर्णाङ्क
diff_internal_failed(शून्य)
{
    पूर्णाङ्क idx;

    // Only need to do something when there is another buffer.
    प्रति (idx = 0; idx < DB_गणनम्; ++idx)
	जइ (curtab->tp_diffbuf[idx] != रिक्त
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Completely update the diffs for the buffers involved.
 * When using the external "diff" command the buffers are written to a file,
 * also for unmodified buffers (the file could have been produced by
 * autocommands, e.g. the netrw plugin).
 */
    शून्य
ex_diffupdate(exarg_T *eap)	// "eap" can be NULL
{
    पूर्णाङ्क		idx_orig;
    पूर्णाङ्क		idx_new;
    diffio_T	diffio;
    पूर्णाङ्क		had_diffs = curtab->tp_first_diff != रिक्त;

    जइ (diff_busy)
    {
	diff_need_update = TRUE;
	प्रत्यागम;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    प्रति (idx_orig = 0; idx_orig < DB_गणनम्; ++idx_orig)
	जइ (curtab->tp_diffbuf[idx_orig] != रिक्त)
	    भङ्ग;
    जइ (idx_orig == DB_गणनम्)
	गच्छ theend;

    // Only need to do something when there is another buffer.
    प्रति (idx_new = idx_orig + 1; idx_new < DB_गणनम्; ++idx_new)
	जइ (curtab->tp_diffbuf[idx_new] != रिक्त)
	    भङ्ग;
    जइ (idx_new == DB_गणनम्)
	गच्छ theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    जइ (diffio.dio_internal && diff_internal_failed())
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
    जइ (had_diffs || curtab->tp_first_diff != रिक्त)
    {
	diff_redraw(TRUE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, रिक्त, रिक्त, FALSE, curbuf);
	window_layout_unlock();
    }
}

/*
 * Do a quick test if "diff" really works.  Otherwise it looks like there
 * are no differences.  Can't use the return value, it's non-zero when
 * there are differences.
 */
    स्थिर पूर्णाङ्क
check_external_diff(diffio_T *diffio)
{
    FILE	*fd;
    पूर्णाङ्क		ok;
    पूर्णाङ्क		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    प्रति (;;)
    {
	ok = FALSE;
	fd = mch_fopen((वर्ण *)diffio->dio_orig.din_fname, "w");
	जइ (fd == रिक्त)
	    io_error = TRUE;
	अण्णहा
	{
	    जइ (fwrite("line1\n", (size_t)6, (size_t)1, fd) != 1)
		io_error = TRUE;
	    fclose(fd);
	    fd = mch_fopen((वर्ण *)diffio->dio_new.din_fname, "w");
	    जइ (fd == रिक्त)
		io_error = TRUE;
	    अण्णहा
	    {
		जइ (fwrite("line2\n", (size_t)6, (size_t)1, fd) != 1)
		    io_error = TRUE;
		fclose(fd);
		fd = रिक्त;
		जइ (diff_file(diffio) == OK)
		    fd = mch_fopen((वर्ण *)diffio->dio_diff.dout_fname, "r");
		जइ (fd == रिक्त)
		    io_error = TRUE;
		अण्णहा
		{
		    char_u	linebuf[LBUFLEN];

		    प्रति (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			जइ (vim_fgets(linebuf, LBUFLEN, fd))
			    भङ्ग;
			जइ (STRNCMP(linebuf, "1c1", 3) == 0
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
	जइ (*p_dex != NUL)
	    भङ्ग;
# endif

# जइ defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	जइ (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    अनुवर्त;
	}
	जइ (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	जइ (diff_a_works != MAYBE)
	    भङ्ग;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	जइ (ok)
	    भङ्ग;
    }
    जइ (!ok)
    {
	जइ (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# जइ defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Invoke the xdiff function.
 */
    स्थिर पूर्णाङ्क
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    जइ (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    जइ (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    जइ (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    जइ (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_पङ्क्तयः;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    जइ (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    अण्णहा
	emit_cb.out_पङ्क्तिः = xdiff_out_unified;
    जइ (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	प्रत्यागम FAIL;
    }
    जइ (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    स्थिर पूर्णाङ्क
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    size_t	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    जइ (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	प्रत्यागम OK;
    }
    अण्णहा
# endif
    // Use xdiff for generating the diff.
    जइ (dio->dio_internal)
	प्रत्यागम diff_file_internal(dio);

    जइ (check_restricted())
	प्रत्यागम FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    जइ (cmd == रिक्त)
	प्रत्यागम FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    जइ (getenv("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((वर्ण *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# जइ defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# अण्णहा
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (पूर्णाङ्क)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (शून्य)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    प्रत्यागम OK;
}

/*
 * Create a new version of a file from the current buffer and a diff file.
 * The buffer is written to a file, also for unmodified buffers (the file
 * could have been produced by autocommands, e.g. the netrw plugin).
 */
    शून्य
ex_diffpatch(exarg_T *eap)
{
    char_u	*tmp_orig;	// name of original temp file
    char_u	*tmp_new;	// name of patched temp file
    char_u	*buf = रिक्त;
    size_t	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = रिक्त;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = रिक्त;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = रिक्त;
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = रिक्त;

# ifdef FEAT_BROWSE
    जइ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, रिक्त, रिक्त,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), रिक्त);
	जइ (browseFile == रिक्त)
	    प्रत्यागम;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    जइ (tmp_orig == रिक्त || tmp_new == रिक्त)
	गच्छ theend;

    // Write the current buffer to "tmp_orig".
    जइ (buf_write(curbuf, tmp_orig, रिक्त,
		(linenr_T)1, curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
				     रिक्त, FALSE, FALSE, FALSE, TRUE) == FAIL)
	गच्छ theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != रिक्त ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    जइ (esc_name == रिक्त)
	गच्छ theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    जइ (buf == रिक्त)
	गच्छ theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    जइ (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((वर्ण *)dirbuf) != 0)
	dirbuf[0] = NUL;
    अण्णहा
    {
#  ifdef TEMPDIRNAMES
	जइ (vim_tempdir != रिक्त)
	    vim_ignored = mch_chdir((वर्ण *)vim_tempdir);
	अण्णहा
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    जइ (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != रिक्त ? fullname :
#  endif
		eap->arg, tmp_new);
    अण्णहा
# endif
    {
	जइ (check_restricted())
	    गच्छ theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((वर्ण *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(शून्य)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    जइ (dirbuf[0] != NUL)
    {
	जइ (mch_chdir((वर्ण *)dirbuf) != 0)
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
    जइ (mch_stat((वर्ण *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    अण्णहा
    {
	जइ (curbuf->b_fname != रिक्त)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    जइ (newname != रिक्त)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	जइ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    जइ (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		जइ (newname != रिक्त)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    जइ (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    जइ (tmp_orig != रिक्त)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    जइ (tmp_new != रिक्त)
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
    शून्य
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

    जइ (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	प्रत्यागम;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    जइ (curwin == old_curwin)		// split didn't work
	प्रत्यागम;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    जइ (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	जइ (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_पङ्क्तिः(
		    old_curbuf.br_buf, old_curwin->w_cursor.lnum);
    }
    // Now that lines are folded scroll to show the cursor at the same
    // relative position.
    scroll_to_fraction(curwin, curwin->w_height);
}

/*
 * Set options to show diffs for the current window.
 */
    शून्य
ex_diffthis(exarg_T *eap UNUSED)
{
    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
}

    स्थिर शून्य
set_diff_option(win_T *wp, पूर्णाङ्क मानम्)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_मानम्_give_err((char_u *)"diff", (long)मानम्, रिक्त, OPT_LOCAL);
    --curbuf_lock;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Set options in window "wp" for diff mode.
 */
    शून्य
diff_win_options(
    win_T	*wp,
    पूर्णाङ्क		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    जइ (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    जइ (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    जइ (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	जइ (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    जइ (!wp->w_p_diff)
    {
	जइ (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    जइ (!wp->w_p_diff)
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
    जइ (vim_strchr(p_sbo, 'h') == रिक्त)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    जइ (addbuf)
	diff_buf_add(wp->w_buffer);
    redraw_win_later(wp, UPD_NOT_VALID);
}

/*
 * Set options not to show diffs.  For the current window or all windows.
 * Only in the current tab page.
 */
    शून्य
ex_diffoff(exarg_T *eap)
{
    win_T	*wp;
    पूर्णाङ्क		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	जइ (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    जइ (wp->w_p_diff_saved)
	    {

		जइ (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		जइ (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		जइ (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    जइ (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		जइ (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		जइ (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		जइ (wp->w_p_fen)
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
    जइ (eap->forceit)
	diff_buf_clear();

    जइ (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    जइ (!diffwin && vim_strchr(p_sbo, 'h') != रिक्त)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    स्थिर शून्य
diff_read(
    पूर्णाङ्क		idx_orig,	// idx of original file
    पूर्णाङ्क		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    FILE	*fd = रिक्त;
    पूर्णाङ्क		पङ्क्तिः_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = रिक्त;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*पङ्क्तिः;
    long	off;
    पूर्णाङ्क		i;
    पूर्णाङ्क		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = रिक्त;	    // init to avoid gcc warning

    गणना {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    जइ (dout->dout_fname == रिक्त)
    {
	diffstyle = DIFF_UNIFIED;
    }
    अण्णहा
    {
	fd = mch_fopen((वर्ण *)dout->dout_fname, "r");
	जइ (fd == रिक्त)
	{
	    emsg(_(e_cannot_read_diff_output));
	    प्रत्यागम;
	}
    }

    जइ (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	जइ (hunk == रिक्त)
	{
	    जइ (fd != रिक्त)
		fclose(fd);
	    प्रत्यागम;
	}
    }

    प्रति (;;)
    {
	जइ (dio->dio_internal)
	{
	    जइ (पङ्क्तिः_hunk_idx >= dout->dout_ga.ga_len)
		भङ्ग;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[पङ्क्तिः_hunk_idx++];
	}
	अण्णहा
	{
	    जइ (fd == रिक्त)
	    {
		जइ (पङ्क्तिः_hunk_idx >= dout->dout_ga.ga_len)
		    भङ्ग;	    // did last line
		पङ्क्तिः = ((char_u **)dout->dout_ga.ga_data)[पङ्क्तिः_hunk_idx++];
	    }
	    अण्णहा
	    {
		जइ (vim_fgets(linebuf, LBUFLEN, fd))
		    भङ्ग;		// end of file
		पङ्क्तिः = linebuf;
	    }

	    जइ (diffstyle == DIFF_NONE)
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
		जइ (SAFE_isdigit(*पङ्क्तिः))
		    diffstyle = DIFF_ED;
		अण्णहा जइ ((STRNCMP(पङ्क्तिः, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		अण्णहा जइ ((STRNCMP(पङ्क्तिः, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(पङ्क्तिः, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(पङ्क्तिः, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		अण्णहा
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    अनुवर्त;
	    }

	    जइ (diffstyle == DIFF_ED)
	    {
		जइ (!SAFE_isdigit(*पङ्क्तिः))
		    अनुवर्त;	// not the start of a diff block
		जइ (parse_diff_ed(पङ्क्तिः, hunk) == FAIL)
		    अनुवर्त;
	    }
	    अण्णहा जइ (diffstyle == DIFF_UNIFIED)
	    {
		जइ (STRNCMP(पङ्क्तिः, "@@ ", 3)  != 0)
		    अनुवर्त;	// not the start of a diff block
		जइ (parse_diff_unified(पङ्क्तिः, hunk) == FAIL)
		    अनुवर्त;
	    }
	    अण्णहा
	    {
		emsg(_(e_invalid_diff_format));
		भङ्ग;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	जाव (dp != रिक्त
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_गणनम्[idx_orig])
	{
	    जइ (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	जइ (dp != रिक्त
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_गणनम्[idx_orig]
		&& hunk->lnum_orig + hunk->गणनम्_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    प्रति (dpl = dp; dpl->df_next != रिक्त; dpl = dpl->df_next)
		जइ (hunk->lnum_orig + hunk->गणनम्_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    भङ्ग;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    जइ (off > 0)
	    {
		प्रति (i = idx_orig; i < idx_new; ++i)
		    जइ (curtab->tp_diffbuf[i] != रिक्त)
		    {
			dp->df_lnum[i] -= off;
			dp->df_गणनम्[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_गणनम्[idx_new] = hunk->गणनम्_new;
	    }
	    अण्णहा जइ (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_गणनम्[idx_new] = hunk->गणनम्_new - off;
	    }
	    अण्णहा
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		पूर्णाङ्क orig_size_in_dp = MIN(hunk->गणनम्_orig,
			dp->df_lnum[idx_orig] +
			dp->df_गणनम्[idx_orig] - hunk->lnum_orig);
		पूर्णाङ्क size_diff = hunk->गणनम्_new - orig_size_in_dp;
		dp->df_गणनम्[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->गणनम्_new
		    - (dp->df_lnum[idx_new] + dp->df_गणनम्[idx_new]);
		जइ (off > 0)
		    dp->df_गणनम्[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->गणनम्_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_गणनम्[idx_orig]);
	    जइ (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		जइ (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_गणनम्[idx_new] += -off;
		}
		off = 0;
	    }
	    प्रति (i = idx_orig; i < idx_new; ++i)
		जइ (curtab->tp_diffbuf[i] != रिक्त)
		    dp->df_गणनम्[i] = dpl->df_lnum[i] + dpl->df_गणनम्[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    जाव (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	अण्णहा
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    जइ (dp == रिक्त)
		गच्छ done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_गणनम्[idx_orig] = hunk->गणनम्_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_गणनम्[idx_new] = hunk->गणनम्_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    प्रति (i = idx_orig + 1; i < idx_new; ++i)
		जइ (curtab->tp_diffbuf[i] != रिक्त)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    जाव (dp != रिक्त)
    {
	जइ (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    जइ (!dio->dio_internal)
	vim_free(hunk);

    जइ (fd != रिक्त)
	fclose(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    स्थिर शून्य
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    पूर्णाङ्क		idx_orig,
    पूर्णाङ्क		idx_new)
{
    long	off;

    जइ (dprev == रिक्त)
	off = 0;
    अण्णहा
	off = (dprev->df_lnum[idx_orig] + dprev->df_गणनम्[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_गणनम्[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_गणनम्[idx_new] = dp->df_गणनम्[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    शून्य
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    प्रति (p = tp->tp_first_diff; p != रिक्त; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = रिक्त;
}

/*
 *  return true if the options are set to use diff linematch
 */
    स्थिर पूर्णाङ्क
diff_linematch(diff_T *dp)
{
    जइ (!(diff_flags & DIFF_LINEMATCH))
	प्रत्यागम 0;

    // are there more than three diff buffers?
    पूर्णाङ्क tsize = 0;
    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
    {
	जइ (curtab->tp_diffbuf[i] != रिक्त)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    जइ (dp->df_गणनम्[i] < 0)
		प्रत्यागम FALSE;
	    tsize += dp->df_गणनम्[i];
	}
    }

    // avoid allocating a huge array because it will lag
    प्रत्यागम tsize <= linematch_पङ्क्तयः;
}

    स्थिर पूर्णाङ्क
get_max_diff_length(ध्रुव diff_T *dp)
{
    पूर्णाङ्क maxlength = 0;

    प्रति (पूर्णाङ्क k = 0; k < DB_गणनम्; k++)
    {
	जइ (curtab->tp_diffbuf[k] != रिक्त)
	{
	    जइ (dp->df_गणनम्[k] > maxlength)
		maxlength = dp->df_गणनम्[k];
	}
    }
    प्रत्यागम maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    स्थिर शून्य
find_शिखरम्_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    पूर्णाङ्क		fromidx,
    पूर्णाङ्क		topline)
{
    diff_T	*topdiff = रिक्त;
    diff_T	*localtopdiff = रिक्त;
    पूर्णाङ्क		topdiffchange = 0;

    प्रति (topdiff = curtab->tp_first_diff; topdiff != रिक्त;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	जइ (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	जइ (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_गणनम्[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    जइ ((*thistopdiff) == रिक्त)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	जइ (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_गणनम्[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    जइ (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		भङ्ग;
	    }
	}
    }
}

/*
 * Calculates topline/topfill of a target diff window to fit the source diff
 * window.
 */
    स्थिर शून्य
calculate_topfill_and_topline(
    ध्रुव पूर्णाङ्क	fromidx,
    ध्रुव पूर्णाङ्क	toidx,
    ध्रुव पूर्णाङ्क	from_topline,
    ध्रुव पूर्णाङ्क	from_topfill,
    पूर्णाङ्क		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = रिक्त;
    diff_T	*next_adjacent_blocks = रिक्त;
    पूर्णाङ्क		virtual_पङ्क्तयः_passed = 0;
    पूर्णाङ्क		curlinenum_to = 1;

    find_शिखरम्_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    जाव (curdif && (curdif->df_lnum[fromidx] + curdif->df_गणनम्[fromidx])
							<= from_topline)
    {
	virtual_पङ्क्तयः_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    जइ (curdif != next_adjacent_blocks)
	virtual_पङ्क्तयः_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_पङ्क्तयः_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    जइ (virtual_पङ्क्तयः_passed < 0)
	virtual_पङ्क्तयः_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    जइ (thistopdiff != रिक्त) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    पूर्णाङ्क virt_पङ्क्तयः_वामः = virtual_पङ्क्तयः_passed;
    curdif = thistopdiff;
    जाव (virt_पङ्क्तयः_वामः > 0 && curdif != रिक्त && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_पङ्क्तयः_वामः, curdif->df_गणनम्[toidx]);
	virt_पङ्क्तयः_वामः -= MIN(virt_पङ्क्तयः_वामः, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    पूर्णाङ्क max_virt_पङ्क्तयः = 0;
    प्रति (diff_T *dp = thistopdiff; dp != रिक्त; dp = dp->df_next)
    {
	जइ (dp->df_lnum[toidx] + dp->df_गणनम्[toidx] <= curlinenum_to)
	    max_virt_पङ्क्तयः += get_max_diff_length(dp);
	अण्णहा
	{
	    जइ (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_पङ्क्तयः += curlinenum_to - dp->df_lnum[toidx];
	    भङ्ग;
	}
    }

    जइ (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_पङ्क्तयः - virtual_पङ्क्तयः_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    स्थिर शून्य
apply_linematch_results(
    diff_T	*dp,
    size_t	decisions_length,
    ध्रुव पूर्णाङ्क	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    पूर्णाङ्क		पङ्क्तिः_numbers[DB_गणनम्];
    पूर्णाङ्क		outputmap[DB_गणनम्];
    size_t	ndiffs = 0;

    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
    {
	जइ (curtab->tp_diffbuf[i] != रिक्त)
	{
	    पङ्क्तिः_numbers[i] = dp->df_lnum[i];
	    dp->df_गणनम्[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    प्रति (size_t i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	जइ (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    प्रति (पूर्णाङ्क j = 0; j < DB_गणनम्; j++)
	    {
		जइ (curtab->tp_diffbuf[j] != रिक्त)
		{
		    dp_s->df_lnum[j] = पङ्क्तिः_numbers[j];
		    dp_s->df_गणनम्[j] = 0;
		}
	    }
	}
	प्रति (size_t j = 0; j < ndiffs; j++)
	{
	    जइ (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_गणनम्[outputmap[j]]++;
		पङ्क्तिः_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    स्थिर शून्य
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_गणनम्];
    ध्रुव mmfile_t	*diffbufs[DB_गणनम्];
    पूर्णाङ्क			diff_length[DB_गणनम्];
    size_t		ndiffs = 0;

    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
    {
	जइ (curtab->tp_diffbuf[i] != रिक्त)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    जइ (dp->df_गणनम्[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_गणनम्[i] - 1);
	    }
	    अण्णहा
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = रिक्त;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_गणनम्[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    पूर्णाङ्क *decisions = रिक्त;
    ध्रुव पूर्णाङ्क iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    size_t decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    प्रति (size_t i = 0; i < ndiffs; i++)
	मुक्त(diffbufs_mm[i].din_mmfile.ptr); // TODO should this be vim_free ?

    apply_linematch_results(dp, decisions_length, decisions);

    मुक्त(decisions);
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
    पूर्णाङ्क
diff_check_with_linestatus(win_T *wp, linenr_T lnum, पूर्णाङ्क *linestatus)
{
    पूर्णाङ्क		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    पूर्णाङ्क		maxcount;
    पूर्णाङ्क		i;
    buf_T	*buf = wp->w_buffer;
    पूर्णाङ्क		cmp;

    जइ (linestatus != रिक्त)
	*linestatus = 0;

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जइ (curtab->tp_first_diff == रिक्त || !wp->w_p_diff)	// no diffs at all
	प्रत्यागम 0;

    // safety check: "lnum" must be a buffer line
    जइ (lnum < 1 || lnum > buf->b_ml.ml_पङ्क्तिः_गणनम् + 1)
	प्रत्यागम 0;

    idx = diff_buf_idx(buf);
    जइ (idx == DB_गणनम्)
	प्रत्यागम 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    जइ (hasFoldingWin(wp, lnum, रिक्त, रिक्त, TRUE, रिक्त))
	प्रत्यागम 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = रिक्त;		// last block with df_lnum[idx] <= lnum
    प्रति (dp = diff_scan_start(curtab, idx, lnum); dp != रिक्त; dp = dp->df_next)
    {
	जइ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	जइ (lnum <= dp->df_lnum[idx] + dp->df_गणनम्[idx])
	    भङ्ग;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    जइ (dp == रिक्त || lnum < dp->df_lnum[idx])
	प्रत्यागम 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    जइ (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    पूर्णाङ्क अङ्कः_fill = 0;
    जाव (lnum == dp->df_lnum[idx] + dp->df_गणनम्[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	जइ (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    अङ्कः_fill += maxcount - dp->df_गणनम्[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	जइ (dp->df_next != रिक्त
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_गणनम्[idx])
	    dp = dp->df_next;
	अण्णहा
	    भङ्ग;
    }

    जइ (lnum < dp->df_lnum[idx] + dp->df_गणनम्[idx])
    {
	पूर्णाङ्क	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	प्रति (i = 0; i < DB_गणनम्; ++i)
	    जइ (i != idx && curtab->tp_diffbuf[i] != रिक्त)
	    {
		जइ (dp->df_गणनम्[i] == 0)
		    zero = TRUE;
		अण्णहा
		{
		    जइ (dp->df_गणनम्[i] != dp->df_गणनम्[idx])
		    {
			जइ (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			प्रत्यागम अङ्कः_fill;
		    }
		    cmp = TRUE;
		}
	    }
	जइ (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    प्रति (i = 0; i < DB_गणनम्; ++i)
		जइ (i != idx && curtab->tp_diffbuf[i] != रिक्त
						      && dp->df_गणनम्[i] != 0)
		    जइ (!diff_equal_entry(dp, idx, i))
		    {
			जइ (linestatus)
			    *linestatus = -1;
			प्रत्यागम अङ्कः_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	जइ (zero == FALSE)
	    प्रत्यागम अङ्कः_fill;
	जइ (linestatus)
	    *linestatus = -2;
	प्रत्यागम अङ्कः_fill;
    }
    प्रत्यागम अङ्कः_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    स्थिर पूर्णाङ्क
diff_equal_entry(diff_T *dp, पूर्णाङ्क idx1, पूर्णाङ्क idx2)
{
    पूर्णाङ्क		i;
    char_u	*पङ्क्तिः;
    पूर्णाङ्क		cmp;

    जइ (dp->df_गणनम्[idx1] != dp->df_गणनम्[idx2])
	प्रत्यागम FALSE;
    जइ (diff_check_sanity(curtab, dp) == FAIL)
	प्रत्यागम FALSE;
    प्रति (i = 0; i < dp->df_गणनम्[idx1]; ++i)
    {
	पङ्क्तिः = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	जइ (पङ्क्तिः == रिक्त)
	    प्रत्यागम FALSE;
	cmp = diff_cmp(पङ्क्तिः, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(पङ्क्तिः);
	जइ (cmp != 0)
	    प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    स्थिर पूर्णाङ्क
diff_equal_char(char_u *p1, char_u *p2, पूर्णाङ्क *len)
{
    पूर्णाङ्क l  = (*mb_ptr2len)(p1);

    जइ (l != (*mb_ptr2len)(p2))
	प्रत्यागम FALSE;
    जइ (l > 1)
    {
	जइ (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    प्रत्यागम FALSE;
	*len = l;
    }
    अण्णहा
    {
	जइ ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    प्रत्यागम FALSE;
	*len = 1;
    }
    प्रत्यागम TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    स्थिर पूर्णाङ्क
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    पूर्णाङ्क		l;

    जइ ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	प्रत्यागम 0;

    जइ ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	प्रत्यागम STRCMP(s1, s2);
    जइ ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	प्रत्यागम MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    जाव (*p1 != NUL && *p2 != NUL)
    {
	जइ (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	अण्णहा
	{
	    जइ (!diff_equal_char(p1, p2, &l))
		भङ्ग;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    जइ (*p1 != NUL || *p2 != NUL)
	प्रत्यागम 1;
    प्रत्यागम 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    पूर्णाङ्क
diff_check_fill(win_T *wp, linenr_T lnum)
{
    पूर्णाङ्क		n;

    // be quick when there are no filler lines
    जइ (!(diff_flags & DIFF_FILLER))
	प्रत्यागम 0;
    n = diff_check_with_linestatus(wp, lnum, रिक्त);
    जइ (n <= 0)
	प्रत्यागम 0;
    प्रत्यागम n;
}

/*
 * Set the topline of "towin" to match the position in "fromwin", so that they
 * show the same diff'ed lines.
 */
    शून्य
diff_set_topline(win_T *fromwin, win_T *towin)
{
    buf_T	*frombuf = fromwin->w_buffer;
    linenr_T	lnum;
    पूर्णाङ्क		fromidx;
    पूर्णाङ्क		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    जइ (fromidx == DB_गणनम्)
	प्रत्यागम;		// safety check

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जइ (lnum <= dp->df_lnum[fromidx] + dp->df_गणनम्[fromidx])
	    भङ्ग;
    जइ (dp == रिक्त)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्
				       - (frombuf->b_ml.ml_पङ्क्तिः_गणनम् - lnum);
    }
    अण्णहा
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	जइ (toidx == DB_गणनम्)
	    प्रत्यागम;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	जइ (lnum >= dp->df_lnum[fromidx])
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
    जइ (towin->w_topline > towin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
	towin->w_botfill = true;
    }
    जइ (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_पङ्क्तिः_abv_curs_win(towin);

    check_topfill(towin, FALSE);
# ifdef FEAT_FOLDING
    (शून्य)hasFoldingWin(towin, towin->w_topline, &towin->w_topline,
							    रिक्त, TRUE, रिक्त);
# endif
}

/*
 * Parse the diff anchors. If "check_only" is set, will only make sure the
 * syntax is correct.
 */
    स्थिर पूर्णाङ्क
parse_diffanchors(
    पूर्णाङ्क		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    पूर्णाङ्क		*अङ्कः_anchors)
{
    पूर्णाङ्क i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = रिक्त;
    जइ (check_only)
	bufwin = curwin;
    अण्णहा
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    जइ (bufwin->w_buffer == buf && bufwin->w_p_diff)
		भङ्ग;
	जइ (bufwin == रिक्त && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    प्रत्यागम FAIL;
	}
    }

    प्रति (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	जइ (*dia == ',') // don't allow empty values
	    प्रत्यागम FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(रिक्त, &dia, ADDR_पङ्क्तयः, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	जइ (dia == रिक्त) // error detected
	    प्रत्यागम FAIL;
	जइ (*dia != ',' && *dia != NUL)
	    प्रत्यागम FAIL;

	जइ (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_पङ्क्तिः_गणनम् + 1))
	{
	    emsg(_(e_invalid_range));
	    प्रत्यागम FAIL;
	}

	जइ (anchors != रिक्त)
	    anchors[i] = lnum;

	जइ (*dia == ',')
	    dia++;
    }
    जइ (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	प्रत्यागम FAIL;
    }
    जइ (अङ्कः_anchors != रिक्त)
	*अङ्कः_anchors = i;
    प्रत्यागम OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    पूर्णाङ्क
diffanchors_changed(पूर्णाङ्क buflocal)
{
    पूर्णाङ्क result = parse_diffanchors(TRUE, curbuf, रिक्त, रिक्त);
    जइ (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    जइ (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    अण्णहा
	    {
		प्रति (पूर्णाङ्क idx = 0; idx < DB_गणनम्; ++idx)
		    जइ (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			भङ्ग;
		    }
	    }
	}
    }
    प्रत्यागम result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    पूर्णाङ्क
diffopt_changed(शून्य)
{
    char_u	*p;
    पूर्णाङ्क		diff_context_new = 6;
    पूर्णाङ्क		linematch_पङ्क्तयः_new = 0;
    पूर्णाङ्क		diff_flags_new = 0;
    पूर्णाङ्क		diff_foldcolumn_new = 2;
    long	diff_algorithm_new = 0;
    long	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    जाव (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	जइ (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	अण्णहा जइ (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	अण्णहा जइ (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	अण्णहा जइ (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	अण्णहा जइ (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	अण्णहा जइ (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	अण्णहा जइ (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	अण्णहा जइ (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	अण्णहा जइ (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	अण्णहा जइ (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	अण्णहा जइ (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	अण्णहा जइ (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	अण्णहा जइ (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	अण्णहा जइ (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	अण्णहा जइ (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	अण्णहा जइ (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	अण्णहा जइ (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    जइ (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    अण्णहा जइ (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    अण्णहा जइ (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    अण्णहा जइ (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    अण्णहा
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    जइ (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    अण्णहा जइ (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    अण्णहा जइ (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    अण्णहा जइ (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    अण्णहा
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_पङ्क्तयः_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	जइ (*p != ',' && *p != NUL)
	    प्रत्यागम FAIL;
	जइ (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    जइ ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	प्रत्यागम FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    जइ (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_पङ्क्तयः = linematch_पङ्क्तयः_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    प्रत्यागम OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    पूर्णाङ्क
diffopt_horizontal(शून्य)
{
    प्रत्यागम (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    पूर्णाङ्क
diffopt_hiddenoff(शून्य)
{
    प्रत्यागम (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    पूर्णाङ्क
diffopt_closeoff(शून्य)
{
    प्रत्यागम (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    शून्य
diff_update_पङ्क्तिः(linenr_T lnum)
{
    पूर्णाङ्क		idx;
    diff_T	*dp;

    जइ (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	प्रत्यागम;

    idx = diff_buf_idx(curbuf);
    जइ (idx == DB_गणनम्)
	प्रत्यागम;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जइ (lnum <= dp->df_lnum[idx] + dp->df_गणनम्[idx])
	    भङ्ग;

    // clear the inline change cache as it's invalid
    जइ (dp != रिक्त)
    {
	dp->has_changes = FALSE;
	dp->df_changes.ga_len = 0;
    }
}

स्थिर diffline_change_T simple_diffline_change; // used for simple inline diff algorithm

/*
 * Parse a diffline struct and returns the [start,end] byte offsets
 *
 * Returns TRUE if this change was added, no other buffer has it.
 */
    पूर्णाङ्क
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    पूर्णाङ्क *change_start,
    पूर्णाङ्क *change_end)
{
    जइ (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    अण्णहा
	*change_start = change->dc_start[diffline->bufidx];
    जइ (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = INT_MAX;
    अण्णहा
	*change_end = change->dc_end[diffline->bufidx];

    जइ (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	प्रत्यागम FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
    {
	जइ (i == diffline->bufidx)
	    अनुवर्त;
	जइ (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    प्रत्यागम FALSE;
	}
    }
    प्रत्यागम TRUE;
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
    स्थिर पूर्णाङ्क
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		*startp,	// first char of the change
    पूर्णाङ्क		*endp)		// last char of the change
{
    char_u	*पङ्क्तिः_org;
    char_u	*पङ्क्तिः_new;
    पूर्णाङ्क		i;
    पूर्णाङ्क		si_org, si_new;
    पूर्णाङ्क		ei_org, ei_new;
    पूर्णाङ्क		off;
    पूर्णाङ्क		added = TRUE;
    char_u	*p1, *p2;
    पूर्णाङ्क		l;

    जइ (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	पङ्क्तिः_org = रिक्त;
    }
    अण्णहा
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	पङ्क्तिः_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	जइ (पङ्क्तिः_org == रिक्त)
	    प्रत्यागम FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    प्रति (i = 0; i < DB_गणनम्; ++i)
	जइ (curtab->tp_diffbuf[i] != रिक्त && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    जइ (off >= dp->df_गणनम्[i])
		अनुवर्त;
	    added = FALSE;
	    जइ (diff_flags & DIFF_INLINE_NONE)
		भङ्ग; // early terminate as we only care about the return value

	    पङ्क्तिः_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    जाव (पङ्क्तिः_org[si_org] != NUL)
	    {
		जइ (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(पङ्क्तिः_org[si_org])
					      && VIM_ISWHITE(पङ्क्तिः_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(पङ्क्तिः_org[si_org])
					    || VIM_ISWHITE(पङ्क्तिः_new[si_new]))))
		{
		    si_org = (पूर्णाङ्क)(skipwhite(पङ्क्तिः_org + si_org) - पङ्क्तिः_org);
		    si_new = (पूर्णाङ्क)(skipwhite(पङ्क्तिः_new + si_new) - पङ्क्तिः_new);
		}
		अण्णहा
		{
		    जइ (!diff_equal_char(पङ्क्तिः_org + si_org, पङ्क्तिः_new + si_new,
									   &l))
			भङ्ग;
		    si_org += l;
		    si_new += l;
		}
	    }
	    जइ (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(पङ्क्तिः_org, पङ्क्तिः_org + si_org);
		si_new -= (*mb_head_off)(पङ्क्तिः_new, पङ्क्तिः_new + si_new);
	    }
	    जइ (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    जइ (पङ्क्तिः_org[si_org] != NUL || पङ्क्तिः_new[si_new] != NUL)
	    {
		ei_org = (पूर्णाङ्क)STRLEN(पङ्क्तिः_org);
		ei_new = (पूर्णाङ्क)STRLEN(पङ्क्तिः_new);
		जाव (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    जइ (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(पङ्क्तिः_org[ei_org])
					      && VIM_ISWHITE(पङ्क्तिः_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(पङ्क्तिः_org[ei_org])
					    || VIM_ISWHITE(पङ्क्तिः_new[ei_new]))))
		    {
			जाव (ei_org >= *startp
					     && VIM_ISWHITE(पङ्क्तिः_org[ei_org]))
			    --ei_org;
			जाव (ei_new >= si_new
					     && VIM_ISWHITE(पङ्क्तिः_new[ei_new]))
			    --ei_new;
		    }
		    अण्णहा
		    {
			p1 = पङ्क्तिः_org + ei_org;
			p2 = पङ्क्तिः_new + ei_new;
			p1 -= (*mb_head_off)(पङ्क्तिः_org, p1);
			p2 -= (*mb_head_off)(पङ्क्तिः_new, p2);
			जइ (!diff_equal_char(p1, p2, &l))
			    भङ्ग;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		जइ (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(पङ्क्तिः_org);
    प्रत्यागम added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
प्रकारनिर्देश संरचना
{
    long byte_start;
    long अङ्कः_bytes;
    पूर्णाङ्क lineoff;
} linemap_entry_T;

/*
 * Refine inline character-wise diff blocks to create a more human readable
 * highlight. Otherwise a naive diff under existing algorithms tends to create
 * a messy output with lots of small gaps.
 * It does this by merging adjacent long diff blocks if they are only separated
 * by a couple characters.
 * These are done by heuristics and can be further tuned.
 */
    स्थिर शून्य
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, पूर्णाङ्क idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    पूर्णाङ्क pass = 1;
    करेहि
    {
	पूर्णाङ्क has_unmerged_gaps = FALSE;
	पूर्णाङ्क has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	जाव (dp!= रिक्त && dp->df_next != रिक्त)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    जइ (dp->df_lnum[idx1] + dp->df_गणनम्[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		अनुवर्त;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_गणनम्[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    जइ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		अनुवर्त;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_गणनम्[idx1]);
	    जइ (gap <= 3)
	    {
		linenr_T max_df_गणनम् = 0;
		प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
		    max_df_गणनम् = MAX(max_df_गणनम्, dp->df_गणनम्[i] + dp->df_next->df_गणनम्[i]);

		जइ (max_df_गणनम् >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
		    {
			dp->df_गणनम्[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_गणनम्[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    अनुवर्त;
		}
		अण्णहा
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	जइ (!has_unmerged_gaps || !has_merged_gaps)
	    भङ्ग;
    } जाव (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    स्थिर शून्य
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, पूर्णाङ्क idx1,
				  linenr_T start_lnum)
{
    पूर्णाङ्क pass = 1;
    करेहि
    {
	diff_T *dp = dp_orig;

	जाव (dp != रिक्त && dp->df_next != रिक्त)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_गणनम्[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    जइ (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		अनुवर्त;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    जइ (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		अनुवर्त;
	    }

	    // Calculate the gap between blocks
	    पूर्णाङ्क gap_start = entry1->byte_start + entry1->अङ्कः_bytes;
	    पूर्णाङ्क gap_end = entry2->byte_start;
	    पूर्णाङ्क gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    जइ (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		अनुवर्त;
	    }

	    // Get the text between the two blocks
	    char_u *पङ्क्तिः = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = पङ्क्तिः + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    प्रति (पूर्णाङ्क i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		पूर्णाङ्क char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		जइ (char_class == 2)
		{
		    only_non_word = false;
		    भङ्ग;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    जइ (has_content && only_non_word)
	    {
		long total_change_bytes = 0;
		प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
		{
		    जइ (curtab->tp_diffbuf[i] != रिक्त)
		    {
			// count bytes in the first block
			प्रति (पूर्णाङ्क k = 0; k < dp->df_गणनम्[i]; k++)
			{
			    पूर्णाङ्क idx = dp->df_lnum[i] + k - 1;
			    जइ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].अङ्कः_bytes;
			}
			// count bytes in the next block
			प्रति (पूर्णाङ्क k = 0; k < dp->df_next->df_गणनम्[i]; k++)
			{
			    पूर्णाङ्क idx = dp->df_next->df_lnum[i] + k - 1;
			    जइ (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].अङ्कः_bytes;
			}
		    }
		}

		जइ (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
		    {
			जइ (curtab->tp_diffbuf[i] != रिक्त)
			{
			    dp->df_गणनम्[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_गणनम्[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    अनुवर्त;
		}
	    }

	    dp = dp->df_next;
	}
    } जाव (pass++ < 4);  // use limited number of passes to avoid excessive looping
}

/*
 * Find the inline difference within a diff block among different buffers.  Do
 * this by splitting each block's content into characters or words, and then
 * use internal xdiff to calculate the per-character/word diff.  The result is
 * stored in dp instead of returned by the function.
 */
    स्थिर शून्य
diff_find_change_inline_diff(
    diff_T	*dp)
{
    diffio_T	dio;
    garray_T	linemap[DB_गणनम्];
    garray_T	file1_str;
    garray_T	file2_str;
    पूर्णाङ्क		file1_idx = -1;

    long	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, परिमाण(वर्ण *), 1000);

    // inline diff only supports internal algo
    dio.dio_internal = TRUE;

    // always use indent-heuristics to slide diff splits along
    // whitespace
    diff_algorithm |= XDF_INDENT_HEURISTIC;

    // diff_read() has an implicit dependency on curtab->tp_first_diff
    diff_T	*orig_diff = curtab->tp_first_diff;
    curtab->tp_first_diff = रिक्त;

    // diff_read() also uses curtab->tp_diffbuf to determine what's an active
    // buffer
    buf_T	*(orig_diffbuf[DB_गणनम्]);
    memcpy(orig_diffbuf, curtab->tp_diffbuf, परिमाण(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
	ga_init2(&linemap[i], परिमाण(linemap_entry_T), 128);

    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	जइ (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	    अनुवर्त; // skip buffer that isn't loaded

	जइ (dp->df_गणनम्[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = रिक्त;
	    अनुवर्त;
	}

	जइ (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	प्रति (पूर्णाङ्क off = 0; off < dp->df_गणनम्[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    पूर्णाङ्क in_keyword = FALSE;

	    // iwhiteeol support vars
	    पूर्णाङ्क last_white = FALSE;
	    पूर्णाङ्क eol_ga_len = -1;
	    पूर्णाङ्क eol_linemap_len = -1;
	    पूर्णाङ्क eol_numlines = -1;

	    char_u *s;
	    प्रति (s = curline; *s != NUL;)
	    {
		पूर्णाङ्क new_in_keyword = FALSE;
		जइ (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		जइ (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		जइ (VIM_ISWHITE(*s))
		{
		    जइ (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			अनुवर्त;
		    }
		    अण्णहा जइ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			जइ (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		अण्णहा
		{
		    जइ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		पूर्णाङ्क char_len = 1;
		जइ (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		अण्णहा
		{
		    char_len = mb_ptr2len(s);

		    जइ (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    जइ (diff_flags & DIFF_ICASE)
		    {
			पूर्णाङ्क c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			पूर्णाङ्क c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			पूर्णाङ्क c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			जइ (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    अण्णहा
			ga_concat_len(curstr, s, char_len);
		}

		जइ (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		जइ (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.अङ्कः_bytes = char_len;
		    जइ (ga_grow(&linemap[i], 1) != OK)
			गच्छ done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		अण्णहा
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].अङ्कः_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    जइ (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    जइ ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		जइ (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    जइ (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.अङ्कः_bytes = परिमाण(NL);
		जइ (ga_grow(&linemap[i], 1) != OK)
		    गच्छ done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	जइ (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (वर्ण *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	अण्णहा
	{
	    dio.dio_orig.din_mmfile.ptr = (वर्ण *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	जइ (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    पूर्णाङ्क diff_status = diff_file_internal(&dio);
	    जइ (diff_status == FAIL)
		गच्छ done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    जइ (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    अण्णहा जइ (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    प्रति (; new_diff != रिक्त; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
	{
	    जइ (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		अनुवर्त;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_गणनम्[i];

	    जइ (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = INT_MAX;
	    }
	    अण्णहा
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    जइ (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    अण्णहा जइ (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = INT_MAX;
	    }
	    अण्णहा
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].अङ्कः_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	जइ (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    गच्छ done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    memcpy(curtab->tp_diffbuf, orig_diffbuf, परिमाण(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    पूर्णाङ्क
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		off;

    idx = diff_buf_idx(wp->w_buffer);
    जइ (idx == DB_गणनम्)	// cannot happen
	प्रत्यागम FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जइ (lnum < dp->df_lnum[idx] + dp->df_गणनम्[idx])
	    भङ्ग;
    जइ (dp == रिक्त || diff_check_sanity(curtab, dp) == FAIL)
	प्रत्यागम FALSE;

    जइ (lnum - dp->df_lnum[idx] > INT_MAX)
	// Integer overflow protection
	प्रत्यागम FALSE;
    off = lnum - dp->df_lnum[idx];

    जइ (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	पूर्णाङ्क	change_start = MAXCOL;	// first col of changed area
	पूर्णाङ्क	change_end = -1;	// last col of changed area
	पूर्णाङ्क	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->अङ्कः_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	प्रत्यागम ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    जइ (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    पूर्णाङ्क अङ्कः_changes = 0;
    पूर्णाङ्क change_idx = 0;
    diffline->changes = रिक्त;
    प्रति (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	जइ (change->dc_end_lnum_off[idx] < off)
	    अनुवर्त;
	जइ (change->dc_start_lnum_off[idx] > off)
	    भङ्ग;
	जइ (diffline->changes == रिक्त)
	    diffline->changes = change;
	अङ्कः_changes++;
    }
    diffline->अङ्कः_changes = अङ्कः_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    पूर्णाङ्क added = FALSE;
    जइ (अङ्कः_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	प्रति (पूर्णाङ्क i = 0; i < DB_गणनम्; i++)
	{
	    जइ (idx == i)
		अनुवर्त;
	    जइ (curtab->tp_diffbuf[i] == रिक्त)
		अनुवर्त;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    जइ (change->dc_start_lnum_off[i] != INT_MAX)
	    {
		added = FALSE;
		भङ्ग;
	    }
	}
    }
    प्रत्यागम added;
}

# जइ defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    पूर्णाङ्क
diff_infold(win_T *wp, linenr_T lnum)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		idx = -1;
    पूर्णाङ्क		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    जइ (!wp->w_p_diff)
	प्रत्यागम FALSE;

    प्रति (i = 0; i < DB_गणनम्; ++i)
    {
	जइ (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	अण्णहा जइ (curtab->tp_diffbuf[i] != रिक्त)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    जइ (idx == -1 || !other)
	प्रत्यागम FALSE;

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    जइ (curtab->tp_first_diff == रिक्त)
	प्रत्यागम TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = रिक्त;		// last block with df_lnum[idx] <= lnum
    पूर्णाङ्क		result = TRUE;
    प्रति (dp = diff_scan_start(curtab, idx, lnum); dp != रिक्त; dp = dp->df_next)
    {
	जइ (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	जइ (dp->df_lnum[idx] - diff_context > lnum)
	    भङ्ग;
	// If this change ends before the line we have a match.
	जइ (dp->df_lnum[idx] + dp->df_गणनम्[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    भङ्ग;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    प्रत्यागम result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    शून्य
nv_diffgetput(पूर्णाङ्क put, long गणनम्)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    जइ (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	प्रत्यागम;
    }
# endif
    जइ (गणनम् == 0)
	ea.arg = (char_u *)"";
    अण्णहा
    {
	vim_snprintf((वर्ण *)buf, 30, "%ld", गणनम्);
	ea.arg = buf;
    }
    जइ (put)
	ea.cmdidx = CMD_diffput;
    अण्णहा
	ea.cmdidx = CMD_diffget;
    ea.addr_गणनम् = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    स्थिर पूर्णाङ्क
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जइ (dp == diff)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    शून्य
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    पूर्णाङ्क		गणनम्;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    पूर्णाङ्क		idx_cur;
    पूर्णाङ्क		idx_other;
    पूर्णाङ्क		idx_from;
    पूर्णाङ्क		idx_to;
    पूर्णाङ्क		i;
    पूर्णाङ्क		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    पूर्णाङ्क		start_skip, end_skip;
    पूर्णाङ्क		new_गणनम्;
    पूर्णाङ्क		buf_empty;
    पूर्णाङ्क		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    जइ (idx_cur == DB_गणनम्)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	प्रत्यागम;
    }

    जइ (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	प्रति (idx_other = 0; idx_other < DB_गणनम्; ++idx_other)
	    जइ (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != रिक्त)
	    {
		जइ (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    भङ्ग;
		found_not_ma = TRUE;
	    }
	जइ (idx_other == DB_गणनम्)
	{
	    जइ (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    अण्णहा
		emsg(_(e_no_other_buffer_in_diff_mode));
	    प्रत्यागम;
	}

	// Check that there isn't a third buffer in the list
	प्रति (i = idx_other + 1; i < DB_गणनम्; ++i)
	    जइ (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != रिक्त
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		प्रत्यागम;
	    }
    }
    अण्णहा
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	जाव (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	प्रति (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	जइ (eap->arg + i == p)	    // digits only
	    i = atol((वर्ण *)eap->arg);
	अण्णहा
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    जइ (i < 0)
		प्रत्यागम;		// error message already given
	}
	buf = buflist_findnr(i);
	जइ (buf == रिक्त)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    प्रत्यागम;
	}
	जइ (buf == curbuf)
	    प्रत्यागम;		// nothing to do
	idx_other = diff_buf_idx(buf);
	जइ (idx_other == DB_गणनम्)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    प्रत्यागम;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    जइ (eap->addr_गणनम् == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	पूर्णाङ्क linestatus = 0;
	जइ (eap->line1 == curbuf->b_ml.ml_पङ्क्तिः_गणनम्
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	अण्णहा जइ (eap->line1 > 0)
	    --eap->line1;
    }

    जइ (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    अण्णहा
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	जइ (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    गच्छ theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    जइ (!curbuf->b_changed)
    {
	change_warning(0);
	जइ (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    गच्छ theend;
	}
    }

    dprev = रिक्त;
    प्रति (dp = curtab->tp_first_diff; dp != रिक्त; )
    {
	जइ (!eap->addr_गणनम्)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    जाव (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_गणनम्[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	जइ (dp->df_lnum[idx_cur] > eap->line2 + off)
	    भङ्ग;	// past the range that was specified

	dfree = रिक्त;
	lnum = dp->df_lnum[idx_to];
	गणनम् = dp->df_गणनम्[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + गणनम् + (गणनम् == 0 && BUFEMPTY() ? 1 : 0);

	जइ (dp->df_lnum[idx_cur] + dp->df_गणनम्[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    जइ (eap->addr_गणनम् > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		जइ (start_skip > 0)
		{
		    // range starts below start of current diff block
		    जइ (start_skip > गणनम्)
		    {
			lnum += गणनम्;
			गणनम् = 0;
		    }
		    अण्णहा
		    {
			गणनम् -= start_skip;
			lnum += start_skip;
		    }
		}
		अण्णहा
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_गणनम्[idx_cur] - 1
							 - (eap->line2 + off);
		जइ (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    जइ (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_गणनम्[idx_cur] - start_skip - end_skip;
			जइ (गणनम् > i)
			    गणनम् = i;
		    }
		    अण्णहा			// :diffget
		    {
			गणनम् -= end_skip;
			end_skip = dp->df_गणनम्[idx_from] - start_skip - गणनम्;
			जइ (end_skip < 0)
			    end_skip = 0;
		    }
		}
		अण्णहा
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    प्रति (i = 0; i < गणनम्; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_पङ्क्तिः_गणनम् == 1;
		जइ (ml_delete(lnum) == OK)
		    --added;
	    }
	    प्रति (i = 0; i < dp->df_गणनम्[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		जइ (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_पङ्क्तिः_गणनम्)
		    भङ्ग;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		जइ (p != रिक्त)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    जइ (buf_empty && curbuf->b_ml.ml_पङ्क्तिः_गणनम् == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_गणनम् = dp->df_गणनम्[idx_to] + added;
	    dp->df_गणनम्[idx_to] = new_गणनम्;

	    जइ (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		प्रति (i = 0; i < DB_गणनम्; ++i)
		    जइ (curtab->tp_diffbuf[i] != रिक्त && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			भङ्ग;
		जइ (i == DB_गणनम्)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    जइ (dprev == रिक्त)
			curtab->tp_first_diff = dp;
		    अण्णहा
			dprev->df_next = dp;
		}
	    }

	    जइ (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + गणनम् - 1, (long)MAXLNUM, (long)added);
		जइ (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    जइ (curwin->w_cursor.lnum >= lnum + गणनम्)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_पङ्क्तिः_गणनम्);
		    }
		    अण्णहा जइ (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_पङ्क्तयः(lnum, 0, lnum + गणनम्, (long)added);

	    जइ (dfree != रिक्त)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    जइ (added != 0 && !valid_diff(dp))
		भङ्ग;

	    जइ (dfree == रिक्त)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_गणनम्[idx_to] = new_गणनम्;

	    // When changing the current buffer, keep track of line numbers
	    जइ (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	जइ (dfree == रिक्त)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    जइ (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	जइ (कुञ्चिका_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    जइ (diff_need_update)
	ex_diffupdate(रिक्त);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_पङ्क्तिः_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    जइ (curtab->tp_first_diff == रिक्त)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    जइ (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    जइ (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    अण्णहा
    {
	// Also need to redraw the other buffers.
	diff_redraw(FALSE);
	window_layout_lock();
	apply_autocmds(EVENT_DIFFUPDATED, रिक्त, रिक्त, FALSE, curbuf);
	window_layout_unlock();
    }
}

# ifdef FEAT_FOLDING
/*
 * Update folds for all diff buffers for entry "dp".
 * Skip buffer with index "skip_idx".
 * When there are no diffs, all folds are removed.
 */
    स्थिर शून्य
diff_fold_update(diff_T *dp, पूर्णाङ्क skip_idx)
{
    पूर्णाङ्क		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	प्रति (i = 0; i < DB_गणनम्; ++i)
	    जइ (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_गणनम्[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    पूर्णाङ्क
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	जइ (diff_buf_idx_tp(buf, tp) != DB_गणनम्)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    पूर्णाङ्क
diff_move_to(पूर्णाङ्क dir, long गणनम्)
{
    पूर्णाङ्क		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    जइ (idx == DB_गणनम् || curtab->tp_first_diff == रिक्त)
	प्रत्यागम FAIL;

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जइ (curtab->tp_first_diff == रिक्त)		// no diffs today
	प्रत्यागम FAIL;

    जाव (--गणनम् >= 0)
    {
	// Check if already before first diff.
	जइ (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    भङ्ग;

	प्रति (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    जइ (dp == रिक्त)
		भङ्ग;
	    जइ ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == रिक्त
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		भङ्ग;
	    }
	}
    }

    // don't end up past the end of the file
    जइ (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;

    // When the cursor didn't move at all we fail.
    जइ (lnum == curwin->w_cursor.lnum)
	प्रत्यागम FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    प्रत्यागम OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    स्थिर linenr_T
diff_get_corresponding_पङ्क्तिः_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    पूर्णाङ्क		idx1;
    पूर्णाङ्क		idx2;
    diff_T	*dp;
    पूर्णाङ्क		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    जइ (idx1 == DB_गणनम् || idx2 == DB_गणनम् || curtab->tp_first_diff == रिक्त)
	प्रत्यागम lnum1;

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जइ (curtab->tp_first_diff == रिक्त)		// no diffs today
	प्रत्यागम lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	जइ (dp->df_lnum[idx1] > lnum1)
	    प्रत्यागम lnum1 - baseline;
	जइ ((dp->df_lnum[idx1] + dp->df_गणनम्[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    जइ (baseline > dp->df_गणनम्[idx2])
		baseline = dp->df_गणनम्[idx2];

	    प्रत्यागम dp->df_lnum[idx2] + baseline;
	}
	जइ (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_गणनम्[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_गणनम्[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    प्रत्यागम curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_गणनम्[idx1])
				   - (dp->df_lnum[idx2] + dp->df_गणनम्[idx2]);
    }

    // If we get here then the cursor is after the last diff
    प्रत्यागम lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_पङ्क्तिः(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_पङ्क्तिः_int(buf1, lnum1);

    // don't end up past the end of the file
    जइ (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	प्रत्यागम curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    प्रत्यागम lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    जइ (idx == DB_गणनम्)		// safety check
	प्रत्यागम (linenr_T)0;

    जइ (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जइ (lnum <= dp->df_lnum[idx] + dp->df_गणनम्[idx])
	    भङ्ग;

    // When after the last change, compute relative to the last line number.
    जइ (dp == रिक्त)
	प्रत्यागम wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्
					- (curbuf->b_ml.ml_पङ्क्तिः_गणनम् - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    जइ (i == DB_गणनम्)			// safety check
	प्रत्यागम (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    जइ (n > dp->df_lnum[i] + dp->df_गणनम्[i])
	n = dp->df_lnum[i] + dp->df_गणनम्[i];
    प्रत्यागम n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    स्थिर पूर्णाङ्क
parse_diff_ed(
	char_u	    *पङ्क्तिः,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    f1, l1, f2, l2;
    पूर्णाङ्क	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = पङ्क्तिः;
    f1 = getdigits(&p);
    जइ (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    अण्णहा
	l1 = f1;
    जइ (*p != 'a' && *p != 'c' && *p != 'd')
	प्रत्यागम FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    जइ (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    अण्णहा
	l2 = f2;
    जइ (l1 < f1 || l2 < f2)
	प्रत्यागम FAIL;

    जइ (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->गणनम्_orig = 0;
    }
    अण्णहा
    {
	hunk->lnum_orig = f1;
	hunk->गणनम्_orig = l1 - f1 + 1;
    }
    जइ (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->गणनम्_new = 0;
    }
    अण्णहा
    {
	hunk->lnum_new = f2;
	hunk->गणनम्_new = l2 - f2 + 1;
    }
    प्रत्यागम OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    स्थिर पूर्णाङ्क
parse_diff_unified(
	char_u	    *पङ्क्तिः,
	diffhunk_T  *hunk)
{
    char_u *p;
    long    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = पङ्क्तिः;
    जइ (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	जइ (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	अण्णहा
	    oldcount = 1;
	जइ (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    जइ (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    अण्णहा
		newcount = 1;
	}
	अण्णहा
	    प्रत्यागम FAIL;	// invalid diff format

	जइ (oldcount == 0)
	    oldline += 1;
	जइ (newcount == 0)
	    newline += 1;
	जइ (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->गणनम्_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->गणनम्_new = newcount;

	प्रत्यागम OK;
    }

    प्रत्यागम FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    स्थिर पूर्णाङ्क
xdiff_out_indices(
	long start_a,
	long गणनम्_a,
	long start_b,
	long गणनम्_b,
	शून्य *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    जइ (p == रिक्त)
	प्रत्यागम -1;

    जइ (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	प्रत्यागम -1;
    }

    p->lnum_orig  = start_a + 1;
    p->गणनम्_orig = गणनम्_a;
    p->lnum_new   = start_b + 1;
    p->गणनम्_new  = गणनम्_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    प्रत्यागम 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    स्थिर पूर्णाङ्क
xdiff_out_unified(
	शून्य *priv,
	mmbuffer_t *mb,
	पूर्णाङ्क nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    प्रत्यागम 0;
}

#endif	// FEAT_DIFF

#जइ defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    शून्य
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    जइ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = diff_check_fill(curwin, tv_get_lnum(argvars));
# endif
}

/*
 * "diff_hlID()" function
 */
    शून्य
f_diff_hlID(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    linenr_T		lnum;
    स्थिर linenr_T	prev_lnum = 0;
    स्थिर varnumber_T	changedtick = 0;
    स्थिर पूर्णाङ्क		fnum = 0;
    स्थिर पूर्णाङ्क		prev_diff_flags = 0;
    स्थिर पूर्णाङ्क		change_start = 0;
    स्थिर पूर्णाङ्क		change_end = 0;
    स्थिर hlf_T	hlID = (hlf_T)0;
    पूर्णाङ्क			cache_results = TRUE;
    पूर्णाङ्क			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    जइ (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    जइ (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    जइ (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    जइ (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	पूर्णाङ्क linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	जइ (linestatus < 0)
	{
	    जइ (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		जइ (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		अण्णहा
		{
		    hlID = HLF_CHD;	// changed line
		    जइ (diffline.अङ्कः_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    अण्णहा
		hlID = HLF_ADD;	// added line
	}
	अण्णहा
	    hlID = (hlf_T)0;

	जइ (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    जइ (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_सङ्ख्या(&argvars[1]) - 1; // ignore type error in {col}
	जइ (cache_results)
	{
	    जइ (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    अण्णहा
		hlID = HLF_CHD;			// changed line
	}
	अण्णहा
	{
	    hlID = HLF_CHD;
	    प्रति (पूर्णाङ्क i = 0; i < diffline.अङ्कः_changes; i++)
	    {
		पूर्णाङ्क added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		जइ (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    भङ्ग;
		}
		जइ (col < change_start)
		    // the remaining changes are past this column and not relevant
		    भङ्ग;
	    }
	}
    }
    rettv->vval.v_सङ्ख्या = hlID == (hlf_T)0 ? 0 : (पूर्णाङ्क)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    स्थिर पूर्णाङ्क
parse_diff_optarg(
    typval_T	    *opts,
    पूर्णाङ्क		    *diffopts,
    long	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    पूर्णाङ्क		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    जइ (algo != रिक्त)
    {
	जइ (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	अण्णहा जइ (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	अण्णहा जइ (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	अण्णहा जइ (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    जइ (output_fmt != रिक्त)
    {
	जइ (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	अण्णहा जइ (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	अण्णहा
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    प्रत्यागम FAIL;
	}
    }

    *diff_ctxlen = dict_get_सङ्ख्या_def(d, "context", 0);
    जइ (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    जइ (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    जइ (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    जइ (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    जइ (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    जइ (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    जइ (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    प्रत्यागम OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    स्थिर शून्य
list_to_diffin(list_T *l, diffin_T *din, पूर्णाङ्क icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_वस्तूनि(l, li)
    {
	str = tv_get_string(&li->li_tv);
	जइ (icase)
	{
	    str = strlow_save(str);
	    जइ (str == रिक्त)
		अनुवर्त;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	जइ (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (वर्ण *)ga.ga_data;
    din->din_mmfile.size = ga.ga_len;
}

/*
 * Get the start and end indices from the diff "hunk".
 */
    स्थिर dict_T *
get_diff_hunk_indices(diffhunk_T *hunk)
{
    dict_T	*hunk_dict;

    hunk_dict = dict_alloc();
    जइ (hunk_dict == रिक्त)
	प्रत्यागम रिक्त;

    dict_add_सङ्ख्या(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_सङ्ख्या(hunk_dict, "from_count", hunk->गणनम्_orig);
    dict_add_सङ्ख्या(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_सङ्ख्या(hunk_dict, "to_count", hunk->गणनम्_new);

    प्रत्यागम hunk_dict;
}
# endif

/*
 * "diff()" function
 */
    शून्य
f_diff(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    diffio_T dio;

    जइ (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	प्रत्यागम;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, परिमाण(वर्ण *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    पूर्णाङ्क		save_diff_flags = diff_flags;
    long	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    जइ (argvars[2].v_type != VAR_UNKNOWN)
	जइ (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    प्रत्यागम;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    पूर्णाङ्क	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    जइ (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    पूर्णाङ्क diff_status = diff_file(&dio);

    // restore 'diffexpr'
    जइ (restore_diffexpr)
	*p_dex = cc;

    जइ (diff_status == FAIL)
	गच्छ done;

    पूर्णाङ्क		hunk_idx = 0;
    dict_T	*hunk_dict;

    जइ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	जइ (rettv_list_alloc(rettv) != OK)
	    गच्छ done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = रिक्त;
	जाव (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    जइ (hunk_dict == रिक्त)
		गच्छ done;

	    जइ (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    अण्णहा
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    जइ (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    अण्णहा
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
