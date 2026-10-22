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

#जर defined(FEAT_DIFF)

स्थिर पूर्णांक diff_busy = FALSE;	    // using diff structs, don't change them
स्थिर पूर्णांक diff_need_update = FALSE; // ex_diffupdate needs to be called

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
स्थिर पूर्णांक	diff_flags = DIFF_INTERNAL | DIFF_FILLER | DIFF_CLOSE_OFF | DIFF_INLINE_CHAR;

स्थिर दीर्घ diff_algorithm = XDF_INDENT_HEURISTIC;
स्थिर पूर्णांक diff_word_gap = 5;  // gap threshold for inline:word

# define LBUFLEN 50		// length of line in diff file

// Max file size xdiff is equipped to deal with. The value (1GB - 1MB) comes
// from Git's implementation.
# define MAX_XDIFF_SIZE (1024L * 1024 * 1023)

स्थिर पूर्णांक diff_a_works = MAYBE; // TRUE when "diff -a" works, FALSE when it
				 // doesn't work, MAYBE when not checked yet
# जर defined(MSWIN)
स्थिर पूर्णांक diff_bin_works = MAYBE; // TRUE when "diff --binary" works, FALSE
				   // when it doesn't work, MAYBE when not
				   // checked yet
# endif

# define MAX_DIFF_ANCHORS 20

// used for diff input
प्रकारव्याख्या संरचना {
    char_u	*din_fname;  // used for external diff
    mmfile_t	din_mmfile;  // used for internal diff
} diffin_T;

// used for diff result
प्रकारव्याख्या संरचना {
    char_u	*dout_fname;  // used for external diff
    garray_T	dout_ga;      // used for internal diff
} diffout_T;

// used for recording hunks from xdiff
प्रकारव्याख्या संरचना {
    linenr_T lnum_orig;
    दीर्घ     मोजणी_orig;
    linenr_T lnum_new;
    दीर्घ     मोजणी_new;
} diffhunk_T;

प्रकारव्याख्या गणना {
    DIO_OUTPUT_INDICES = 0,	// default
    DIO_OUTPUT_UNIFIED = 1	// unified diff format
} dio_outfmt_T;

// two diff inputs and one result
प्रकारव्याख्या संरचना {
    diffin_T	    dio_orig;     // original file input
    diffin_T	    dio_new;      // new file input
    diffout_T	    dio_diff;     // diff result
    पूर्णांक		    dio_internal; // using internal diff
    dio_outfmt_T    dio_outfmt;   // internal diff output format
    पूर्णांक		    dio_ctxlen;   // unified diff context length
} diffio_T;

स्थिर पूर्णांक diff_buf_idx(buf_T *buf);
स्थिर पूर्णांक diff_buf_idx_tp(buf_T *buf, tabpage_T *tp);
स्थिर शून्य diff_mark_adjust_tp(tabpage_T *tp, पूर्णांक idx, linenr_T line1, linenr_T line2, दीर्घ amount, दीर्घ amount_after);
स्थिर शून्य diff_check_unchanged(tabpage_T *tp, diff_T *dp);
स्थिर पूर्णांक diff_check_sanity(tabpage_T *tp, diff_T *dp);
स्थिर पूर्णांक check_external_diff(diffio_T *diffio);
स्थिर पूर्णांक diff_file(diffio_T *diffio);
स्थिर पूर्णांक diff_equal_entry(diff_T *dp, पूर्णांक idx1, पूर्णांक idx2);
स्थिर पूर्णांक diff_cmp(char_u *s1, char_u *s2);
# ifdef FEAT_FOLDING
स्थिर शून्य diff_fold_update(diff_T *dp, पूर्णांक skip_idx);
# endif
स्थिर शून्य diff_read(पूर्णांक idx_orig, पूर्णांक idx_new, diffio_T *dio);
स्थिर शून्य diff_copy_entry(diff_T *dprev, diff_T *dp, पूर्णांक idx_orig, पूर्णांक idx_new);
स्थिर diff_T *diff_alloc_new(tabpage_T *tp, diff_T *dprev, diff_T *dp);
स्थिर पूर्णांक parse_diff_ed(char_u *ओळ, diffhunk_T *hunk);
स्थिर पूर्णांक parse_diff_unified(char_u *ओळ, diffhunk_T *hunk);
स्थिर पूर्णांक xdiff_out_indices(दीर्घ start_a, दीर्घ मोजणी_a, दीर्घ start_b, दीर्घ मोजणी_b, शून्य *priv);
स्थिर पूर्णांक xdiff_out_unified(शून्य *priv, mmbuffer_t *mb, पूर्णांक nbuf);
स्थिर पूर्णांक parse_diffanchors(पूर्णांक check_only, buf_T *buf, linenr_T *anchors, पूर्णांक *अंक_anchors);

# define FOR_ALL_DIFFBLOCKS_IN_TAB(tp, dp) \
    साठी ((dp) = (tp)->tp_first_diff; (dp) != रिक्त; (dp) = (dp)->df_next)

// Cursor into the diff block list for diff_infold() and
// diff_check_with_linestatus(): both scan blocks in ascending df_lnum order,
// usually with an increasing "lnum" during redraw.  Remembering the last block
// at or before "lnum" lets the next call resume there instead of from the head.
// Cleared when a block is freed, allocated or line-shifted (see below).
स्थिर diff_T	    *diff_finger_dp = रिक्त;
स्थिर tabpage_T    *diff_finger_tp = रिक्त;
स्थिर पूर्णांक	    diff_finger_idx = -1;
स्थिर linenr_T	    diff_finger_lnum = 0;

    स्थिर शून्य
clear_diffblock(diff_T *dp)
{
    जर (dp == diff_finger_dp)
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
diff_scan_start(tabpage_T *tp, पूर्णांक idx, linenr_T lnum)
{
    जर (diff_finger_dp != रिक्त
	    && diff_finger_tp == tp
	    && diff_finger_idx == idx
	    && lnum >= diff_finger_lnum)
	परत diff_finger_dp;
    परत tp->tp_first_diff;
}

/*
 * Remember "dp" as the finger for the next scan: the last block with
 * df_lnum[idx] <= "lnum" seen by the scan, or NULL when there is none.
 */
    स्थिर शून्य
diff_scan_remember(tabpage_T *tp, पूर्णांक idx, linenr_T lnum, diff_T *dp)
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
    पूर्णांक		i;
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	जर (i != DB_मोजणी)
	{
	    tp->tp_diffbuf[i] = रिक्त;
	    tp->tp_diff_invalid = TRUE;
	    जर (tp == curtab)
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
    पूर्णांक		i;

    जर (!win->w_p_diff)
    {
	// When there is no window showing a diff for this buffer, remove
	// it from the diffs.
	FOR_ALL_WINDOWS(wp)
	    जर (wp->w_buffer == win->w_buffer && wp->w_p_diff)
		थांबा;
	जर (wp == रिक्त)
	{
	    i = diff_buf_idx(win->w_buffer);
	    जर (i != DB_मोजणी)
	    {
		curtab->tp_diffbuf[i] = रिक्त;
		curtab->tp_diff_invalid = TRUE;
		diff_redraw(TRUE);
	    }
	}
    }
    अन्यथा
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
    पूर्णांक		i;

    जर (diff_buf_idx(buf) != DB_मोजणी)
	परत;		// It's already there.

    साठी (i = 0; i < DB_मोजणी; ++i)
	जर (curtab->tp_diffbuf[i] == रिक्त)
	{
	    curtab->tp_diffbuf[i] = buf;
	    curtab->tp_diff_invalid = TRUE;
	    diff_redraw(TRUE);
	    परत;
	}

    semsg(_(e_cannot_diff_more_than_nr_buffers), DB_मोजणी);
}

/*
 * Remove all buffers to make diffs for.
 */
    स्थिर शून्य
diff_buf_clear(शून्य)
{
    पूर्णांक		i;

    साठी (i = 0; i < DB_मोजणी; ++i)
	जर (curtab->tp_diffbuf[i] != रिक्त)
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
    स्थिर पूर्णांक
diff_buf_idx(buf_T *buf)
{
    पूर्णांक		idx;

    साठी (idx = 0; idx < DB_मोजणी; ++idx)
	जर (curtab->tp_diffbuf[idx] == buf)
	    थांबा;
    परत idx;
}

/*
 * Find buffer "buf" in the list of diff buffers for tab page "tp".
 * Return its index or DB_COUNT if not found.
 */
    स्थिर पूर्णांक
diff_buf_idx_tp(buf_T *buf, tabpage_T *tp)
{
    पूर्णांक		idx;

    साठी (idx = 0; idx < DB_मोजणी; ++idx)
	जर (tp->tp_diffbuf[idx] == buf)
	    थांबा;
    परत idx;
}

/*
 * Mark the diff info involving buffer "buf" as invalid, it will be updated
 * when info is requested.
 */
    शून्य
diff_invalidate(buf_T *buf)
{
    tabpage_T	*tp;
    पूर्णांक		i;

    FOR_ALL_TABPAGES(tp)
    {
	i = diff_buf_idx_tp(buf, tp);
	जर (i != DB_मोजणी)
	{
	    tp->tp_diff_invalid = TRUE;
	    जर (tp == curtab)
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
    दीर्घ	amount,
    दीर्घ	amount_after)
{
    पूर्णांक		idx;
    tabpage_T	*tp;

    // Handle all tab pages that use the current buffer in a diff.
    FOR_ALL_TABPAGES(tp)
    {
	idx = diff_buf_idx_tp(curbuf, tp);
	जर (idx != DB_मोजणी)
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
    पूर्णांक		idx,
    linenr_T	line1,
    linenr_T	line2,
    दीर्घ	amount,
    दीर्घ	amount_after)
{
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dnext;
    पूर्णांक		i;
    पूर्णांक		inserted, deleted;
    पूर्णांक		n, off;
    linenr_T	last;
    linenr_T	lnum_deleted = line1;	// lnum of remaining deletion
    पूर्णांक		check_unchanged;

    diff_finger_dp = रिक्त;		// may shift df_lnum, invalidate the finger

    जर (diff_internal())
    {
	// Will update diffs before redrawing.  Set _invalid to update the
	// diffs themselves, set _update to also update folds properly just
	// before redrawing.
	// Do update marks here, it is needed for :%diffput.
	tp->tp_diff_invalid = TRUE;
	tp->tp_diff_update = TRUE;
    }

    जर (line2 == MAXLNUM)
    {
	// mark_adjust(99, MAXLNUM, 9, 0): insert lines
	inserted = amount;
	deleted = 0;
    }
    अन्यथा जर (amount_after > 0)
    {
	// mark_adjust(99, 98, MAXLNUM, 9): a change that inserts lines
	inserted = amount_after;
	deleted = 0;
    }
    अन्यथा
    {
	// mark_adjust(98, 99, MAXLNUM, -2): delete lines
	inserted = 0;
	deleted = -amount_after;
    }

    dprev = रिक्त;
    dp = tp->tp_first_diff;
    साठी (;;)
    {
	// If the change is after the previous diff block and before the next
	// diff block, thus not touching an existing change, create a new diff
	// block.  Don't do this when ex_diffgetput() is busy.
	जर ((dp == रिक्त || dp->df_lnum[idx] - 1 > line2
		    || (line2 == MAXLNUM && dp->df_lnum[idx] > line1))
		&& (dprev == रिक्त
		    || dprev->df_lnum[idx] + dprev->df_मोजणी[idx] < line1)
		&& !diff_busy)
	{
	    dnext = diff_alloc_new(tp, dprev, dp);
	    जर (dnext == रिक्त)
		परत;

	    dnext->df_lnum[idx] = line1;
	    dnext->df_मोजणी[idx] = inserted;
	    साठी (i = 0; i < DB_मोजणी; ++i)
		जर (tp->tp_diffbuf[i] != रिक्त && i != idx)
		{
		    जर (dprev == रिक्त)
			dnext->df_lnum[i] = line1;
		    अन्यथा
			dnext->df_lnum[i] = line1
			    + (dprev->df_lnum[i] + dprev->df_मोजणी[i])
			    - (dprev->df_lnum[idx] + dprev->df_मोजणी[idx]);
		    dnext->df_मोजणी[i] = deleted;
		}
	}

	// if at end of the list, quit
	जर (dp == रिक्त)
	    थांबा;

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
	last = dp->df_lnum[idx] + dp->df_मोजणी[idx] - 1;

	// 1. change completely above line1: nothing to do
	जर (last >= line1 - 1)
	{
	    जर (diff_busy)
	    {
		// Currently in the middle of updating diff blocks. All we want
		// is to adjust the line numbers and nothing else.
		जर (dp->df_lnum[idx] > line2)
		    dp->df_lnum[idx] += amount_after;

		// Advance to next entry.
		dprev = dp;
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    // 6. change below line2: only adjust for amount_after; also when
	    // "deleted" became zero when deleted all lines between two diffs
	    जर (dp->df_lnum[idx] - (deleted + inserted != 0) > line2)
	    {
		जर (amount_after == 0)
		    थांबा;	// nothing left to change
		dp->df_lnum[idx] += amount_after;
	    }
	    अन्यथा
	    {
		check_unchanged = FALSE;

		// 2. 3. 4. 5.: inserted/deleted lines touching this diff.
		जर (deleted > 0)
		{
		    off = 0;
		    जर (dp->df_lnum[idx] >= line1)
		    {
			जर (last <= line2)
			{
			    // 4. delete all lines of diff
			    जर (dp->df_next != रिक्त
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - lnum_deleted;
				deleted -= n;
				n -= dp->df_मोजणी[idx];
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    अन्यथा
				n = deleted - dp->df_मोजणी[idx];
			    dp->df_मोजणी[idx] = 0;
			}
			अन्यथा
			{
			    // 5. delete lines at or just before top of diff
			    off = dp->df_lnum[idx] - lnum_deleted;
			    n = off;
			    dp->df_मोजणी[idx] -= line2 - dp->df_lnum[idx] + 1;
			    check_unchanged = TRUE;
			}
			dp->df_lnum[idx] = line1;
		    }
		    अन्यथा
		    {
			जर (last < line2)
			{
			    // 2. delete at end of diff
			    dp->df_मोजणी[idx] -= last - lnum_deleted + 1;
			    जर (dp->df_next != रिक्त
				    && dp->df_next->df_lnum[idx] - 1 <= line2)
			    {
				// delete continues in next diff, only do
				// lines until that one
				n = dp->df_next->df_lnum[idx] - 1 - last;
				deleted -= dp->df_next->df_lnum[idx]
							       - lnum_deleted;
				lnum_deleted = dp->df_next->df_lnum[idx];
			    }
			    अन्यथा
				n = line2 - last;
			    check_unchanged = TRUE;
			}
			अन्यथा
			{
			    // 3. delete lines inside the diff
			    n = 0;
			    dp->df_मोजणी[idx] -= deleted;
			}
		    }

		    साठी (i = 0; i < DB_मोजणी; ++i)
			जर (tp->tp_diffbuf[i] != रिक्त && i != idx)
			{
			    जर (dp->df_lnum[i] > off)
				dp->df_lnum[i] -= off;
			    अन्यथा
				dp->df_lnum[i] = 1;
			    dp->df_मोजणी[i] += n;
			}
		}
		अन्यथा
		{
		    जर (dp->df_lnum[idx] <= line1)
		    {
			// inserted lines somewhere in this diff
			dp->df_मोजणी[idx] += inserted;
			check_unchanged = TRUE;
		    }
		    अन्यथा
			// inserted lines somewhere above this diff
			dp->df_lnum[idx] += inserted;
		}

		जर (check_unchanged)
		    // Check if inserted lines are equal, may reduce the
		    // size of the diff.  TODO: also check for equal lines
		    // in the middle and perhaps split the block.
		    diff_check_unchanged(tp, dp);
	    }
	}

	// check if this block touches the previous one, may merge them.
	जर (dprev != रिक्त && !dp->is_linematched && !diff_busy
				&& dprev->df_lnum[idx] + dprev->df_मोजणी[idx]
							== dp->df_lnum[idx])
	{
	    साठी (i = 0; i < DB_मोजणी; ++i)
		जर (tp->tp_diffbuf[i] != रिक्त)
		    dprev->df_मोजणी[i] += dp->df_मोजणी[i];
	    dprev->df_next = dp->df_next;
	    clear_diffblock(dp);
	    dp = dprev->df_next;
	}
	अन्यथा
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    dprev = रिक्त;
    dp = tp->tp_first_diff;
    पर्यंत (dp != रिक्त)
    {
	// All counts are zero, remove this entry.
	साठी (i = 0; i < DB_मोजणी; ++i)
	    जर (tp->tp_diffbuf[i] != रिक्त && dp->df_मोजणी[i] != 0)
		थांबा;
	जर (i == DB_मोजणी)
	{
	    dnext = dp->df_next;
	    clear_diffblock(dp);
	    dp = dnext;
	    जर (dprev == रिक्त)
		tp->tp_first_diff = dnext;
	    अन्यथा
		dprev->df_next = dnext;
	}
	अन्यथा
	{
	    // Advance to next entry.
	    dprev = dp;
	    dp = dp->df_next;
	}

    }

    जर (tp == curtab)
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
    जर (dnew == रिक्त)
	परत रिक्त;
    diff_finger_dp = रिक्त;		// list changed, invalidate the finger

    dnew->is_linematched = FALSE;
    dnew->df_next = dp;
    जर (dprev == रिक्त)
	tp->tp_first_diff = dnew;
    अन्यथा
	dprev->df_next = dnew;

    dnew->has_changes = FALSE;
    ga_init2(&dnew->df_changes, आकार(diffline_change_T), 20);
    परत dnew;
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
    पूर्णांक		i_org;
    पूर्णांक		i_new;
    पूर्णांक		off_org, off_new;
    char_u	*ओळ_org;
    पूर्णांक		dir = FORWARD;

    // Find the first buffers, use it as the original, compare the other
    // buffer lines against this one.
    साठी (i_org = 0; i_org < DB_मोजणी; ++i_org)
	जर (tp->tp_diffbuf[i_org] != रिक्त)
	    थांबा;
    जर (i_org == DB_मोजणी)	// safety check
	परत;

    जर (diff_check_sanity(tp, dp) == FAIL)
	परत;

    // First check lines at the top, then at the bottom.
    off_org = 0;
    off_new = 0;
    साठी (;;)
    {
	// Repeat until a line is found which is different or the number of
	// lines has become zero.
	पर्यंत (dp->df_मोजणी[i_org] > 0)
	{
	    // Copy the line, the next ml_get() will invalidate it.
	    जर (dir == BACKWARD)
		off_org = dp->df_मोजणी[i_org] - 1;
	    ओळ_org = vim_strsave(ml_get_buf(tp->tp_diffbuf[i_org],
					dp->df_lnum[i_org] + off_org, FALSE));
	    जर (ओळ_org == रिक्त)
		परत;
	    साठी (i_new = i_org + 1; i_new < DB_मोजणी; ++i_new)
	    {
		जर (tp->tp_diffbuf[i_new] == रिक्त)
		    सुरूठेवा;
		जर (dir == BACKWARD)
		    off_new = dp->df_मोजणी[i_new] - 1;
		// if other buffer doesn't have this line, it was inserted
		जर (off_new < 0 || off_new >= dp->df_मोजणी[i_new])
		    थांबा;
		जर (diff_cmp(ओळ_org, ml_get_buf(tp->tp_diffbuf[i_new],
				   dp->df_lnum[i_new] + off_new, FALSE)) != 0)
		    थांबा;
	    }
	    vim_free(ओळ_org);

	    // Stop when a line isn't equal in all diff buffers.
	    जर (i_new != DB_मोजणी)
		थांबा;

	    // Line matched in all buffers, remove it from the diff.
	    साठी (i_new = i_org; i_new < DB_मोजणी; ++i_new)
		जर (tp->tp_diffbuf[i_new] != रिक्त)
		{
		    जर (dir == FORWARD)
			++dp->df_lnum[i_new];
		    --dp->df_मोजणी[i_new];
		}
	}
	जर (dir == BACKWARD)
	    थांबा;
	dir = BACKWARD;
    }
}

/*
 * Check if a diff block doesn't contain invalid line numbers.
 * This can happen when the diff program returns invalid results.
 */
    स्थिर पूर्णांक
diff_check_sanity(tabpage_T *tp, diff_T *dp)
{
    पूर्णांक		i;

    साठी (i = 0; i < DB_मोजणी; ++i)
	जर (tp->tp_diffbuf[i] != रिक्त)
	    जर (dp->df_lnum[i] + dp->df_मोजणी[i] - 1
				      > tp->tp_diffbuf[i]->b_ml.ml_ओळ_मोजणी)
		परत FAIL;
    परत OK;
}

/*
 * Mark all diff buffers in the current tab page for redraw.
 */
    शून्य
diff_redraw(
    पूर्णांक		dofold)	    // also recompute the folds
{
    win_T	*wp;
    win_T	*wp_other = रिक्त;
    पूर्णांक		used_max_fill_other = FALSE;
    पूर्णांक		used_max_fill_curwin = FALSE;
    पूर्णांक		n;

    need_diff_redraw = FALSE;
    FOR_ALL_WINDOWS(wp)
    {
	// when closing windows or wiping buffers skip invalid window
	जर (!wp->w_p_diff || !buf_valid(wp->w_buffer))
	    सुरूठेवा;

	redraw_win_later(wp, UPD_SOME_VALID);
	जर (wp != curwin)
	    wp_other = wp;
# ifdef FEAT_FOLDING
	जर (dofold && foldmethodIsDiff(wp))
	    foldUpdateAll(wp);
# endif
	// A change may have made filler lines invalid, need to take care of
	// that for other windows.
	n = diff_check_fill(wp, wp->w_topline);
	जर ((wp != curwin && wp->w_topfill > 0) || n > 0)
	{
	    जर (wp->w_topfill > n)
		wp->w_topfill = (n < 0 ? 0 : n);
	    अन्यथा जर (n > 0 && n > wp->w_topfill)
	    {
		wp->w_topfill = n;
		जर (wp == curwin)
		    used_max_fill_curwin = TRUE;
		अन्यथा जर (wp_other != रिक्त)
		    used_max_fill_other = TRUE;
	    }
	    check_topfill(wp, FALSE);
	}
    }

    जर (wp_other != रिक्त && curwin->w_p_scb)
    {
	जर (used_max_fill_curwin)
	    // The current window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(wp_other, curwin);
	अन्यथा जर (used_max_fill_other)
	    // The other window was set to use the maximum number of filler
	    // lines, may need to reduce them.
	    diff_set_topline(curwin, wp_other);
    }
}

    स्थिर शून्य
clear_diffin(diffin_T *din)
{
    जर (din->din_fname == रिक्त)
	VIM_CLEAR(din->din_mmfile.ptr);
    अन्यथा
	mch_remove(din->din_fname);
}

    स्थिर शून्य
clear_diffout(diffout_T *dout)
{
    जर (dout->dout_fname == रिक्त)
	ga_clear_strings(&dout->dout_ga);
    अन्यथा
	mch_remove(dout->dout_fname);
}

/*
 * Write buffer "buf" to a memory buffer.
 * Return FAIL for failure.
 */
    स्थिर पूर्णांक
diff_write_buffer(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    linenr_T	lnum;
    char_u	*s;
    दीर्घ	len = 0;
    char_u	*ptr;

    जर (end < 0)
	end = buf->b_ml.ml_ओळ_मोजणी;

    जर (buf->b_ml.ml_flags & ML_EMPTY || end < start)
    {
	din->din_mmfile.ptr = रिक्त;
	din->din_mmfile.size = 0;
	परत OK;
    }

    // xdiff requires one big block of memory with all the text.
    साठी (lnum = start; lnum <= end; ++lnum)
	len += ml_get_buf_len(buf, lnum) + 1;
    ptr = alloc(len);
    जर (ptr == रिक्त)
    {
	// Allocating memory failed.  This can happen, because we try to read
	// the whole buffer text into memory.  Set the failed flag, the diff
	// will be retried with external diff.  The flag is never reset.
	buf->b_diff_failed = true;
	जर (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Not enough memory to use internal diff for buffer \"%s\""),
								 buf->b_fname);
	    verbose_leave();
	}
	परत FAIL;
    }
    din->din_mmfile.ptr = (अक्षर *)ptr;
    din->din_mmfile.size = len;

    len = 0;
    साठी (lnum = start; lnum <= end; ++lnum)
    {
	साठी (s = ml_get_buf(buf, lnum, FALSE); *s != NUL; )
	{
	    जर (diff_flags & DIFF_ICASE)
	    {
		पूर्णांक c;
		पूर्णांक	orig_len;
		पूर्णांक	c_len = 1;
		char_u	cbuf[MB_MAXBYTES + 1];

		जर (*s == NL)
		    c = NUL;
		अन्यथा
		{
		    // xdiff doesn't support ignoring case, fold-case the text.
		    c = PTR2CHAR(s);
		    c_len = MB_CHAR2LEN(c);
		    c = MB_CASEFOLD(c);
		}
		orig_len = mb_ptr2len(s);
		जर (mb_char2bytes(c, cbuf) != c_len)
		    // TODO: handle byte length difference.
		    // One example is Å (3 bytes) and å (2 bytes).
		    mch_memmove(ptr + len, s, orig_len);
		अन्यथा
		{
		    mch_memmove(ptr + len, cbuf, c_len);
		    जर (orig_len > c_len)
		    {
			// Copy remaining composing characters
			mch_memmove(ptr + len + c_len, s + c_len,
				orig_len - c_len);
		    }
		}

		s += orig_len;
		len += orig_len;
	    }
	    अन्यथा
	    {
		ptr[len++] = *s == NL ? NUL : *s;
		s++;
	    }
	}
	ptr[len++] = NL;
    }
    परत OK;
}

/*
 * Write buffer "buf" to file or memory buffer.
 * Return FAIL for failure.
 */
    स्थिर पूर्णांक
diff_write(buf_T *buf, diffin_T *din, linenr_T start, linenr_T end)
{
    पूर्णांक		r;
    पूर्णांक		save_ml_flags;
    char_u	*save_ff;
    पूर्णांक		save_cmod_flags;

    जर (din->din_fname == रिक्त)
	परत diff_write_buffer(buf, din, start, end);

    // Writing the diff buffers may trigger changes in the window structure
    // via aucmd_prepbuf()/aucmd_restbuf() commands.
    // This may cause recursively calling winframe_remove() which is not safe and causes
    // use after free, so let's stop it here.
    जर (frames_locked())
	परत FAIL;

    जर (end < 0)
	end = buf->b_ml.ml_ओळ_मोजणी;

    // Always use 'fileformat' set to "unix".
    save_ml_flags = buf->b_ml.ml_flags;
    save_ff = buf->b_p_ff;
    buf->b_p_ff = vim_strsave((char_u *)FF_UNIX);
    save_cmod_flags = cmdmod.cmod_flags;
    // Writing the buffer is an implementation detail of performing the diff,
    // so it shouldn't update the '[ and '] marks.
    cmdmod.cmod_flags |= CMOD_LOCKMARKS;
    जर (end < start)
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
    परत r;
}

    स्थिर पूर्णांक
lnum_compare(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    linenr_T	lnum1 = *(linenr_T*)s1;
    linenr_T	lnum2 = *(linenr_T*)s2;
    जर (lnum1 < lnum2)
	परत -1;
    जर (lnum1 > lnum2)
	परत 1;
    परत 0;
}

/*
 * Update the diffs for all buffers involved.
 */
    स्थिर शून्य
diff_try_update(
	diffio_T    *dio,
	पूर्णांक	    idx_orig,
	exarg_T	    *eap)	// "eap" can be NULL
{
    buf_T	*buf;
    पूर्णांक		idx_new;

    जर (dio->dio_internal)
    {
	ga_init2(&dio->dio_diff.dout_ga, आकार(अक्षर *), 1000);
    }
    अन्यथा
    {
	// We need three temp file names.
	dio->dio_orig.din_fname = vim_tempname('o', TRUE);
	dio->dio_new.din_fname = vim_tempname('n', TRUE);
	dio->dio_diff.dout_fname = vim_tempname('d', TRUE);
	जर (dio->dio_orig.din_fname == रिक्त
		|| dio->dio_new.din_fname == रिक्त
		|| dio->dio_diff.dout_fname == रिक्त)
	    जा theend;
    }

    // Check external diff is actually working.
    जर (!dio->dio_internal && check_external_diff(dio) == FAIL)
	जा theend;

    // :diffupdate!
    जर (eap != रिक्त && eap->forceit)
	साठी (idx_new = idx_orig; idx_new < DB_मोजणी; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    जर (buf_valid(buf))
		buf_check_timestamp(buf, FALSE);
	}

    // Parse and sort diff anchors if enabled
    पूर्णांक अंक_anchors = पूर्णांक_उच्च;
    linenr_T anchors[DB_मोजणी][MAX_DIFF_ANCHORS];
    CLEAR_FIELD(anchors);
    जर (diff_flags & DIFF_ANCHOR)
    {
	साठी (पूर्णांक idx = 0; idx < DB_मोजणी; idx++)
	{
	    जर (curtab->tp_diffbuf[idx] == रिक्त)
		सुरूठेवा;
	    पूर्णांक buf_अंक_anchors = 0;
	    जर (parse_diffanchors(FALSE,
			curtab->tp_diffbuf[idx],
			anchors[idx],
			&buf_अंक_anchors) != OK)
	    {
		emsg(_(e_failed_to_find_all_diff_anchors));
		अंक_anchors = 0;
		CLEAR_FIELD(anchors);
		थांबा;
	    }
	    जर (buf_अंक_anchors < अंक_anchors)
		अंक_anchors = buf_अंक_anchors;

	    जर (buf_अंक_anchors > 0)
		क्विक((शून्य *)anchors[idx],
			(माप_प्रकार)buf_अंक_anchors,
			आकार(linenr_T),
			lnum_compare);
	}
    }
    जर (अंक_anchors == पूर्णांक_उच्च)
	अंक_anchors = 0;

    // Split the files into multiple sections by anchors. Each section starts
    // from one anchor (inclusive) and ends at the next anchor (exclusive).
    // Diff each section separately before combining the results. If we don't
    // have any anchors, we will have one big section of the entire file.
    साठी (पूर्णांक anchor_i = 0; anchor_i <= अंक_anchors; anchor_i++)
    {
	diff_T	*orig_diff = रिक्त;
	जर (anchor_i != 0)
	{
	    orig_diff = curtab->tp_first_diff;
	    curtab->tp_first_diff = रिक्त;
	}
	linenr_T lnum_start = (anchor_i == 0) ? 1 : anchors[idx_orig][anchor_i - 1];
	linenr_T lnum_end = (anchor_i == अंक_anchors) ? -1 : anchors[idx_orig][anchor_i] - 1;

	// Write the first buffer to a tempfile or mmfile_t.
	buf = curtab->tp_diffbuf[idx_orig];
	जर (diff_write(buf, &dio->dio_orig, lnum_start, lnum_end) == FAIL)
	{
	    जर (orig_diff != रिक्त)
	    {
		// Clean up in-progress diff blocks
		curtab->tp_first_diff = orig_diff;
		diff_clear(curtab);
	    }
	    जा theend;
	}

	// Make a difference between the first buffer and every other.
	साठी (idx_new = idx_orig + 1; idx_new < DB_मोजणी; ++idx_new)
	{
	    buf = curtab->tp_diffbuf[idx_new];
	    जर (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
		सुरूठेवा; // skip buffer that isn't loaded

	    lnum_start = anchor_i == 0 ? 1 : anchors[idx_new][anchor_i - 1];
	    lnum_end = anchor_i == अंक_anchors ? -1 : anchors[idx_new][anchor_i] - 1;

	    // Write the other buffer and diff with the first one.
	    जर (diff_write(buf, &dio->dio_new, lnum_start, lnum_end) == FAIL)
		सुरूठेवा;
	    जर (diff_file(dio) == FAIL)
		सुरूठेवा;

	    // Read the diff output and add each entry to the diff list.
	    diff_read(idx_orig, idx_new, dio);

	    clear_diffin(&dio->dio_new);
	    clear_diffout(&dio->dio_diff);
	}
	clear_diffin(&dio->dio_orig);

	जर (anchor_i != 0)
	{
	    diff_finger_dp = रिक्त;	// df_lnum shifted, invalidate the finger

	    // Combine the new diff blocks with the existing ones
	    साठी (diff_T *dp = curtab->tp_first_diff; dp != रिक्त; dp = dp->df_next)
	    {
		साठी (पूर्णांक idx = 0; idx < DB_मोजणी; idx++)
		{
		    जर (anchors[idx][anchor_i - 1] > 0)
			dp->df_lnum[idx] += anchors[idx][anchor_i - 1] - 1;
		}
	    }
	    जर (orig_diff != रिक्त)
	    {
		diff_T *last_diff = orig_diff;
		पर्यंत (last_diff->df_next != रिक्त)
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
    पूर्णांक
diff_internal(शून्य)
{
    परत (diff_flags & DIFF_INTERNAL) != 0
# ifdef FEAT_EVAL
	&& *p_dex == NUL
# endif
	;
}

/*
 * Return TRUE if the internal diff failed for one of the diff buffers.
 */
    स्थिर पूर्णांक
diff_internal_failed(शून्य)
{
    पूर्णांक idx;

    // Only need to do something when there is another buffer.
    साठी (idx = 0; idx < DB_मोजणी; ++idx)
	जर (curtab->tp_diffbuf[idx] != रिक्त
		&& curtab->tp_diffbuf[idx]->b_diff_failed)
	    परत TRUE;
    परत FALSE;
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
    पूर्णांक		idx_orig;
    पूर्णांक		idx_new;
    diffio_T	diffio;
    पूर्णांक		had_diffs = curtab->tp_first_diff != रिक्त;

    जर (diff_busy)
    {
	diff_need_update = TRUE;
	परत;
    }

    // Delete all diffblocks.
    diff_clear(curtab);
    curtab->tp_diff_invalid = FALSE;

    // Use the first buffer as the original text.
    साठी (idx_orig = 0; idx_orig < DB_मोजणी; ++idx_orig)
	जर (curtab->tp_diffbuf[idx_orig] != रिक्त)
	    थांबा;
    जर (idx_orig == DB_मोजणी)
	जा theend;

    // Only need to do something when there is another buffer.
    साठी (idx_new = idx_orig + 1; idx_new < DB_मोजणी; ++idx_new)
	जर (curtab->tp_diffbuf[idx_new] != रिक्त)
	    थांबा;
    जर (idx_new == DB_मोजणी)
	जा theend;

    // Only use the internal method if it did not fail for one of the buffers.
    CLEAR_FIELD(diffio);
    diffio.dio_internal = diff_internal() && !diff_internal_failed();

    diff_try_update(&diffio, idx_orig, eap);
    जर (diffio.dio_internal && diff_internal_failed())
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
    जर (had_diffs || curtab->tp_first_diff != रिक्त)
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
    स्थिर पूर्णांक
check_external_diff(diffio_T *diffio)
{
    खाता	*fd;
    पूर्णांक		ok;
    पूर्णांक		io_error = FALSE;

    // May try twice, first with "-a" and then without.
    साठी (;;)
    {
	ok = FALSE;
	fd = mch_fopen((अक्षर *)diffio->dio_orig.din_fname, "w");
	जर (fd == रिक्त)
	    io_error = TRUE;
	अन्यथा
	{
	    जर (ख_डालो("line1\n", (माप_प्रकार)6, (माप_प्रकार)1, fd) != 1)
		io_error = TRUE;
	    ख_बंद(fd);
	    fd = mch_fopen((अक्षर *)diffio->dio_new.din_fname, "w");
	    जर (fd == रिक्त)
		io_error = TRUE;
	    अन्यथा
	    {
		जर (ख_डालो("line2\n", (माप_प्रकार)6, (माप_प्रकार)1, fd) != 1)
		    io_error = TRUE;
		ख_बंद(fd);
		fd = रिक्त;
		जर (diff_file(diffio) == OK)
		    fd = mch_fopen((अक्षर *)diffio->dio_diff.dout_fname, "r");
		जर (fd == रिक्त)
		    io_error = TRUE;
		अन्यथा
		{
		    char_u	linebuf[LBUFLEN];

		    साठी (;;)
		    {
			// For normal diff there must be a line that contains
			// "1c1".  For unified diff "@@ -1 +1 @@".
			जर (vim_fgets(linebuf, LBUFLEN, fd))
			    थांबा;
			जर (STRNCMP(linebuf, "1c1", 3) == 0
				|| STRNCMP(linebuf, "@@ -1 +1 @@", 11) == 0)
			    ok = TRUE;
		    }
		    ख_बंद(fd);
		}
		mch_remove(diffio->dio_diff.dout_fname);
		mch_remove(diffio->dio_new.din_fname);
	    }
	    mch_remove(diffio->dio_orig.din_fname);
	}

# ifdef FEAT_EVAL
	// When using 'diffexpr' break here.
	जर (*p_dex != NUL)
	    थांबा;
# endif

# जर defined(MSWIN)
	// If the "-a" argument works, also check if "--binary" works.
	जर (ok && diff_a_works == MAYBE && diff_bin_works == MAYBE)
	{
	    diff_a_works = TRUE;
	    diff_bin_works = TRUE;
	    सुरूठेवा;
	}
	जर (!ok && diff_a_works == TRUE && diff_bin_works == TRUE)
	{
	    // Tried --binary, but it failed. "-a" works though.
	    diff_bin_works = FALSE;
	    ok = TRUE;
	}
# endif

	// If we checked if "-a" works already, break here.
	जर (diff_a_works != MAYBE)
	    थांबा;
	diff_a_works = ok;

	// If "-a" works break here, otherwise retry without "-a".
	जर (ok)
	    थांबा;
    }
    जर (!ok)
    {
	जर (io_error)
	    emsg(_(e_cannot_read_or_write_temp_files));
	emsg(_(e_cannot_create_diffs));
	diff_a_works = MAYBE;
# जर defined(MSWIN)
	diff_bin_works = MAYBE;
# endif
	परत FAIL;
    }
    परत OK;
}

/*
 * Invoke the xdiff function.
 */
    स्थिर पूर्णांक
diff_file_internal(diffio_T *diffio)
{
    xpparam_t	    param;
    xdemitconf_t    emit_cfg;
    xdemitcb_t	    emit_cb;

    CLEAR_FIELD(param);
    CLEAR_FIELD(emit_cfg);
    CLEAR_FIELD(emit_cb);

    param.flags = diff_algorithm;

    जर (diff_flags & DIFF_IWHITE)
	param.flags |= XDF_IGNORE_WHITESPACE_CHANGE;
    जर (diff_flags & DIFF_IWHITEALL)
	param.flags |= XDF_IGNORE_WHITESPACE;
    जर (diff_flags & DIFF_IWHITEEOL)
	param.flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
    जर (diff_flags & DIFF_IBLANK)
	param.flags |= XDF_IGNORE_BLANK_ओळी;

    emit_cfg.ctxlen = diffio->dio_ctxlen;
    emit_cb.priv = &diffio->dio_diff;
    जर (diffio->dio_outfmt == DIO_OUTPUT_INDICES)
	emit_cfg.hunk_func = xdiff_out_indices;
    अन्यथा
	emit_cb.out_ओळ = xdiff_out_unified;
    जर (diffio->dio_orig.din_mmfile.size > MAX_XDIFF_SIZE ||
	diffio->dio_new.din_mmfile.size > MAX_XDIFF_SIZE)
    {
	emsg(_(e_problem_creating_internal_diff));
	परत FAIL;
    }
    जर (xdl_diff(&diffio->dio_orig.din_mmfile,
		&diffio->dio_new.din_mmfile,
		&param, &emit_cfg, &emit_cb) < 0)
    {
	emsg(_(e_problem_creating_internal_diff));
	परत FAIL;
    }
    परत OK;
}

/*
 * Make a diff between files "tmp_orig" and "tmp_new", results in "tmp_diff".
 * return OK or FAIL;
 */
    स्थिर पूर्णांक
diff_file(diffio_T *dio)
{
    char_u	*cmd;
    माप_प्रकार	len;
    char_u	*tmp_orig = dio->dio_orig.din_fname;
    char_u	*tmp_new = dio->dio_new.din_fname;
    char_u	*tmp_diff = dio->dio_diff.dout_fname;

# ifdef FEAT_EVAL
    जर (*p_dex != NUL)
    {
	// Use 'diffexpr' to generate the diff file.
	eval_diff(tmp_orig, tmp_new, tmp_diff);
	परत OK;
    }
    अन्यथा
# endif
    // Use xdiff for generating the diff.
    जर (dio->dio_internal)
	परत diff_file_internal(dio);

    जर (check_restricted())
	परत FAIL;

    len = STRLEN(tmp_orig) + STRLEN(tmp_new)
				+ STRLEN(tmp_diff) + STRLEN(p_srr) + 27;
    cmd = alloc(len);
    जर (cmd == रिक्त)
	परत FAIL;

    // We don't want $DIFF_OPTIONS to get in the way.
    जर (दो_पर्या("DIFF_OPTIONS"))
	vim_setenv((char_u *)"DIFF_OPTIONS", (char_u *)"");

    // Build the diff command and execute it.  Always use -a, binary
    // differences are of no use.  Ignore errors, diff returns
    // non-zero when differences have been found.
    vim_snprintf((अक्षर *)cmd, len, "diff %s%s%s%s%s%s%s%s %s",
	    diff_a_works == FALSE ? "" : "-a ",
# जर defined(MSWIN)
	    diff_bin_works == TRUE ? "--binary " : "",
# अन्यथा
	    "",
# endif
	    (diff_flags & DIFF_IWHITE) ? "-b " : "",
	    (diff_flags & DIFF_IWHITEALL) ? "-w " : "",
	    (diff_flags & DIFF_IWHITEEOL) ? "-Z " : "",
	    (diff_flags & DIFF_IBLANK) ? "-B " : "",
	    (diff_flags & DIFF_ICASE) ? "-i " : "",
	    tmp_orig, tmp_new);
    append_redir(cmd, (पूर्णांक)len, p_srr, tmp_diff);
    block_autocmds();	// avoid ShellCmdPost stuff
    (शून्य)call_shell(cmd, SHELL_FILTER|SHELL_SILENT|SHELL_DOOUT);
    unblock_autocmds();
    vim_free(cmd);
    परत OK;
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
    माप_प्रकार	buflen;
    win_T	*old_curwin = curwin;
    char_u	*newname = रिक्त;	// name of patched file buffer
# ifdef UNIX
    char_u	dirbuf[MAXPATHL];
    char_u	*fullname = रिक्त;
# endif
# ifdef FEAT_BROWSE
    char_u	*browseFile = रिक्त;
    पूर्णांक		save_cmod_flags = cmdmod.cmod_flags;
# endif
    stat_T	st;
    char_u	*esc_name = रिक्त;

# ifdef FEAT_BROWSE
    जर (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	browseFile = do_browse(0, (char_u *)_("Patch file"),
			 eap->arg, रिक्त, रिक्त,
			 (char_u *)_(BROWSE_FILTER_ALL_FILES), रिक्त);
	जर (browseFile == रिक्त)
	    परत;		// operation cancelled
	eap->arg = browseFile;
	cmdmod.cmod_flags &= ~CMOD_BROWSE; // don't let do_ecmd() browse again
    }
# endif

    // We need two temp file names.
    tmp_orig = vim_tempname('o', FALSE);
    tmp_new = vim_tempname('n', FALSE);
    जर (tmp_orig == रिक्त || tmp_new == रिक्त)
	जा theend;

    // Write the current buffer to "tmp_orig".
    जर (buf_write(curbuf, tmp_orig, रिक्त,
		(linenr_T)1, curbuf->b_ml.ml_ओळ_मोजणी,
				     रिक्त, FALSE, FALSE, FALSE, TRUE) == FAIL)
	जा theend;

# ifdef UNIX
    // Get the absolute path of the patchfile, changing directory below.
    fullname = FullName_save(eap->arg, FALSE);
# endif
    esc_name = vim_strsave_shellescape(
# ifdef UNIX
		    fullname != रिक्त ? fullname :
# endif
		    eap->arg, TRUE, TRUE);
    जर (esc_name == रिक्त)
	जा theend;
    buflen = STRLEN(tmp_orig) + STRLEN(esc_name) + STRLEN(tmp_new) + 16;
    buf = alloc(buflen);
    जर (buf == रिक्त)
	जा theend;

# ifdef UNIX
    // Temporarily chdir to /tmp, to avoid patching files in the current
    // directory when the patch file contains more than one patch.  When we
    // have our own temp dir use that instead, it will be cleaned up when we
    // exit (any .rej files created).  Don't change directory if we can't
    // return to the current.
    जर (mch_dirname(dirbuf, MAXPATHL) != OK || mch_chdir((अक्षर *)dirbuf) != 0)
	dirbuf[0] = NUL;
    अन्यथा
    {
#  ifdef TEMPDIRNAMES
	जर (vim_tempdir != रिक्त)
	    vim_ignored = mch_chdir((अक्षर *)vim_tempdir);
	अन्यथा
#  endif
	    vim_ignored = mch_chdir("/tmp");
	shorten_fnames(TRUE);
    }
# endif

# ifdef FEAT_EVAL
    जर (*p_pex != NUL)
	// Use 'patchexpr' to generate the new file.
	eval_patch(tmp_orig,
#  ifdef UNIX
		fullname != रिक्त ? fullname :
#  endif
		eap->arg, tmp_new);
    अन्यथा
# endif
    {
	जर (check_restricted())
	    जा theend;

	// Build the patch command and execute it.  Ignore errors.  Switch to
	// cooked mode to allow the user to respond to prompts.
	vim_snprintf((अक्षर *)buf, buflen, "patch -o %s %s < %s",
						  tmp_new, tmp_orig, esc_name);
	block_autocmds();	// Avoid ShellCmdPost stuff
	(शून्य)call_shell(buf, SHELL_FILTER | SHELL_COOKED);
	unblock_autocmds();
    }

# ifdef UNIX
    जर (dirbuf[0] != NUL)
    {
	जर (mch_chdir((अक्षर *)dirbuf) != 0)
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
    जर (mch_stat((अक्षर *)tmp_new, &st) < 0 || st.st_size == 0)
	emsg(_(e_cannot_read_patch_output));
    अन्यथा
    {
	जर (curbuf->b_fname != रिक्त)
	{
	    newname = vim_strnsave(curbuf->b_fname,
						  STRLEN(curbuf->b_fname) + 4);
	    जर (newname != रिक्त)
		STRCAT(newname, ".new");
	}

# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	// don't use a new tab page, each tab page has its own diffs
	cmdmod.cmod_tab = 0;

	जर (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) != FAIL)
	{
	    // Pretend it was a ":split fname" command
	    eap->cmdidx = CMD_split;
	    eap->arg = tmp_new;
	    do_exedit(eap, old_curwin);

	    // check that split worked and editing tmp_new
	    जर (curwin != old_curwin && win_valid(old_curwin))
	    {
		// Set 'diff', 'scrollbind' on and 'wrap' off.
		diff_win_options(curwin, TRUE);
		diff_win_options(old_curwin, TRUE);

		जर (newname != रिक्त)
		{
		    // do a ":file filename.new" on the patched buffer
		    eap->arg = newname;
		    ex_file(eap);

		    // Do filetype detection with the new name.
		    जर (au_has_group((char_u *)"filetypedetect"))
			do_cmdline_cmd(
				     (char_u *)":doau filetypedetect BufRead");
		}
	    }
	}
    }

theend:
    जर (tmp_orig != रिक्त)
	mch_remove(tmp_orig);
    vim_free(tmp_orig);
    जर (tmp_new != रिक्त)
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

    जर (win_split(0, (diff_flags & DIFF_VERTICAL) ? WSP_VERT : 0) == FAIL)
	परत;

    // Pretend it was a ":split fname" command
    eap->cmdidx = CMD_split;
    curwin->w_p_diff = TRUE;
    do_exedit(eap, old_curwin);

    जर (curwin == old_curwin)		// split didn't work
	परत;

    // Set 'diff', 'scrollbind' on and 'wrap' off.
    diff_win_options(curwin, TRUE);
    जर (win_valid(old_curwin))
    {
	diff_win_options(old_curwin, TRUE);

	जर (bufref_valid(&old_curbuf))
	    // Move the cursor position to that of the old window.
	    curwin->w_cursor.lnum = diff_get_corresponding_ओळ(
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
set_diff_option(win_T *wp, पूर्णांक मूल्य)
{
    win_T *old_curwin = curwin;

    curwin = wp;
    curbuf = curwin->w_buffer;
    ++curbuf_lock;
    set_option_मूल्य_give_err((char_u *)"diff", (दीर्घ)मूल्य, रिक्त, OPT_LOCAL);
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
    पूर्णांक		addbuf)		// Add buffer to diff.
{
# ifdef FEAT_FOLDING
    win_T *old_curwin = curwin;

    // close the manually opened folds
    curwin = wp;
    newFoldLevel();
    curwin = old_curwin;
# endif

    // Use 'scrollbind' and 'cursorbind' when available
    जर (!wp->w_p_diff)
	wp->w_p_scb_save = wp->w_p_scb;
    wp->w_p_scb = TRUE;
    जर (!wp->w_p_diff)
	wp->w_p_crb_save = wp->w_p_crb;
    wp->w_p_crb = TRUE;
    जर (!(diff_flags & DIFF_FOLLOWWRAP))
    {
	जर (!wp->w_p_diff)
	    wp->w_p_wrap_save = wp->w_p_wrap;
	wp->w_p_wrap = FALSE;
	wp->w_skipcol = 0;
    }
# ifdef FEAT_FOLDING
    जर (!wp->w_p_diff)
    {
	जर (wp->w_p_diff_saved)
	    free_string_option(wp->w_p_fdm_save);
	wp->w_p_fdm_save = vim_strsave(wp->w_p_fdm);
    }
    set_string_option_direct_in_win(wp, (char_u *)"fdm", -1, (char_u *)"diff",
						       OPT_LOCAL|OPT_FREE, 0);
    जर (!wp->w_p_diff)
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
    जर (vim_strchr(p_sbo, 'h') == रिक्त)
	do_cmdline_cmd((char_u *)"set sbo+=hor");
    // Save the current values, to be restored in ex_diffoff().
    wp->w_p_diff_saved = TRUE;

    set_diff_option(wp, TRUE);

    जर (addbuf)
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
    पूर्णांक		diffwin = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	जर (eap->forceit ? wp->w_p_diff : wp == curwin)
	{
	    // Set 'diff' off. If option values were saved in
	    // diff_win_options(), restore the ones whose settings seem to have
	    // been left over from diff mode.
	    set_diff_option(wp, FALSE);

	    जर (wp->w_p_diff_saved)
	    {

		जर (wp->w_p_scb)
		    wp->w_p_scb = wp->w_p_scb_save;
		जर (wp->w_p_crb)
		    wp->w_p_crb = wp->w_p_crb_save;
		जर (!(diff_flags & DIFF_FOLLOWWRAP))
		{
		    जर (!wp->w_p_wrap && wp->w_p_wrap_save)
		    {
			wp->w_p_wrap = TRUE;
			wp->w_leftcol = 0;
		    }
		}
# ifdef FEAT_FOLDING
		free_string_option(wp->w_p_fdm);
		wp->w_p_fdm = vim_strsave(
		    *wp->w_p_fdm_save ? wp->w_p_fdm_save : (char_u*)"manual");

		जर (wp->w_p_fdc == diff_foldcolumn)
		    wp->w_p_fdc = wp->w_p_fdc_save;
		जर (wp->w_p_fdl == 0)
		    wp->w_p_fdl = wp->w_p_fdl_save;

		// Only restore 'foldenable' when 'foldmethod' is not
		// "manual", otherwise we continue to show the diff folds.
		जर (wp->w_p_fen)
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
    जर (eap->forceit)
	diff_buf_clear();

    जर (!diffwin)
    {
	diff_need_update = FALSE;
	curtab->tp_diff_invalid = FALSE;
	curtab->tp_diff_update = FALSE;
	diff_clear(curtab);
    }

    // Remove "hor" from 'scrollopt' if there are no diff windows left.
    जर (!diffwin && vim_strchr(p_sbo, 'h') != रिक्त)
	do_cmdline_cmd((char_u *)"set sbo-=hor");
}

/*
 * Read the diff output and add each entry to the diff list.
 */
    स्थिर शून्य
diff_read(
    पूर्णांक		idx_orig,	// idx of original file
    पूर्णांक		idx_new,	// idx of new file
    diffio_T   *dio)		// diff output
{
    खाता	*fd = रिक्त;
    पूर्णांक		ओळ_hunk_idx = 0;  // line or hunk index
    diff_T	*dprev = रिक्त;
    diff_T	*dp = curtab->tp_first_diff;
    diff_T	*dn, *dpl;
    diffout_T   *dout = &dio->dio_diff;
    char_u	linebuf[LBUFLEN];   // only need to hold the diff line
    char_u	*ओळ;
    दीर्घ	off;
    पूर्णांक		i;
    पूर्णांक		notset = TRUE;	    // block "*dp" not set yet
    diffhunk_T	*hunk = रिक्त;	    // init to avoid gcc warning

    गणना {
	DIFF_ED,
	DIFF_UNIFIED,
	DIFF_NONE
    } diffstyle = DIFF_NONE;

    जर (dout->dout_fname == रिक्त)
    {
	diffstyle = DIFF_UNIFIED;
    }
    अन्यथा
    {
	fd = mch_fopen((अक्षर *)dout->dout_fname, "r");
	जर (fd == रिक्त)
	{
	    emsg(_(e_cannot_read_diff_output));
	    परत;
	}
    }

    जर (!dio->dio_internal)
    {
	hunk = ALLOC_ONE(diffhunk_T);
	जर (hunk == रिक्त)
	{
	    जर (fd != रिक्त)
		ख_बंद(fd);
	    परत;
	}
    }

    साठी (;;)
    {
	जर (dio->dio_internal)
	{
	    जर (ओळ_hunk_idx >= dout->dout_ga.ga_len)
		थांबा;      // did last hunk
	    hunk = ((diffhunk_T **)dout->dout_ga.ga_data)[ओळ_hunk_idx++];
	}
	अन्यथा
	{
	    जर (fd == रिक्त)
	    {
		जर (ओळ_hunk_idx >= dout->dout_ga.ga_len)
		    थांबा;	    // did last line
		ओळ = ((char_u **)dout->dout_ga.ga_data)[ओळ_hunk_idx++];
	    }
	    अन्यथा
	    {
		जर (vim_fgets(linebuf, LBUFLEN, fd))
		    थांबा;		// end of file
		ओळ = linebuf;
	    }

	    जर (diffstyle == DIFF_NONE)
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
		जर (SAFE_isdigit(*ओळ))
		    diffstyle = DIFF_ED;
		अन्यथा जर ((STRNCMP(ओळ, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		अन्यथा जर ((STRNCMP(ओळ, "--- ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ओळ, "+++ ", 4) == 0)
			&& (vim_fgets(linebuf, LBUFLEN, fd) == 0)
			&& (STRNCMP(ओळ, "@@ ", 3) == 0))
		    diffstyle = DIFF_UNIFIED;
		अन्यथा
		    // Format not recognized yet, skip over this line.  Cygwin
		    // diff may put a warning at the start of the file.
		    सुरूठेवा;
	    }

	    जर (diffstyle == DIFF_ED)
	    {
		जर (!SAFE_isdigit(*ओळ))
		    सुरूठेवा;	// not the start of a diff block
		जर (parse_diff_ed(ओळ, hunk) == FAIL)
		    सुरूठेवा;
	    }
	    अन्यथा जर (diffstyle == DIFF_UNIFIED)
	    {
		जर (STRNCMP(ओळ, "@@ ", 3)  != 0)
		    सुरूठेवा;	// not the start of a diff block
		जर (parse_diff_unified(ओळ, hunk) == FAIL)
		    सुरूठेवा;
	    }
	    अन्यथा
	    {
		emsg(_(e_invalid_diff_format));
		थांबा;
	    }
	}

	// Go over blocks before the change, for which orig and new are equal.
	// Copy blocks from orig to new.
	पर्यंत (dp != रिक्त
		&& hunk->lnum_orig > dp->df_lnum[idx_orig]
						      + dp->df_मोजणी[idx_orig])
	{
	    जर (notset)
		diff_copy_entry(dprev, dp, idx_orig, idx_new);
	    dprev = dp;
	    dp = dp->df_next;
	    notset = TRUE;
	}

	जर (dp != रिक्त
		&& hunk->lnum_orig <= dp->df_lnum[idx_orig]
						       + dp->df_मोजणी[idx_orig]
		&& hunk->lnum_orig + hunk->मोजणी_orig >= dp->df_lnum[idx_orig])
	{
	    // New block overlaps with existing block(s).
	    // First find last block that overlaps.
	    साठी (dpl = dp; dpl->df_next != रिक्त; dpl = dpl->df_next)
		जर (hunk->lnum_orig + hunk->मोजणी_orig
					     < dpl->df_next->df_lnum[idx_orig])
		    थांबा;

	    // If the newly found block starts before the old one, set the
	    // start back a number of lines.
	    off = dp->df_lnum[idx_orig] - hunk->lnum_orig;
	    जर (off > 0)
	    {
		साठी (i = idx_orig; i < idx_new; ++i)
		    जर (curtab->tp_diffbuf[i] != रिक्त)
		    {
			dp->df_lnum[i] -= off;
			dp->df_मोजणी[i] += off;
		    }
		dp->df_lnum[idx_new] = hunk->lnum_new;
		dp->df_मोजणी[idx_new] = hunk->मोजणी_new;
	    }
	    अन्यथा जर (notset)
	    {
		// new block inside existing one, adjust new block
		dp->df_lnum[idx_new] = hunk->lnum_new + off;
		dp->df_मोजणी[idx_new] = hunk->मोजणी_new - off;
	    }
	    अन्यथा
	    {
		// second overlap of new block with existing block

		// if this hunk has different orig/new counts, adjust
		// the diff block size first. When we handled the first hunk we
		// would have expanded it to fit, without knowing that this
		// hunk exists
		पूर्णांक orig_size_in_dp = MIN(hunk->मोजणी_orig,
			dp->df_lnum[idx_orig] +
			dp->df_मोजणी[idx_orig] - hunk->lnum_orig);
		पूर्णांक size_diff = hunk->मोजणी_new - orig_size_in_dp;
		dp->df_मोजणी[idx_new] += size_diff;

		// grow existing block to include the overlap completely
		off = hunk->lnum_new + hunk->मोजणी_new
		    - (dp->df_lnum[idx_new] + dp->df_मोजणी[idx_new]);
		जर (off > 0)
		    dp->df_मोजणी[idx_new] += off;
	    }

	    // Adjust the size of the block to include all the lines to the
	    // end of the existing block or the new diff, whatever ends last.
	    off = (hunk->lnum_orig + hunk->मोजणी_orig)
			 - (dpl->df_lnum[idx_orig] + dpl->df_मोजणी[idx_orig]);
	    जर (off < 0)
	    {
		// new change ends in existing block, adjust the end. We only
		// need to do this once per block or we will over-adjust.
		जर (notset || dp != dpl)
		{
		    // adjusting by 'off' here is only correct if
		    // there is not another hunk in this block. we
		    // adjust for this when we encounter a second
		    // overlap later.
		    dp->df_मोजणी[idx_new] += -off;
		}
		off = 0;
	    }
	    साठी (i = idx_orig; i < idx_new; ++i)
		जर (curtab->tp_diffbuf[i] != रिक्त)
		    dp->df_मोजणी[i] = dpl->df_lnum[i] + dpl->df_मोजणी[i]
						       - dp->df_lnum[i] + off;

	    // Delete the diff blocks that have been merged into one.
	    dn = dp->df_next;
	    dp->df_next = dpl->df_next;
	    पर्यंत (dn != dp->df_next)
	    {
		dpl = dn->df_next;
		clear_diffblock(dn);
		dn = dpl;
	    }
	}
	अन्यथा
	{
	    // Allocate a new diffblock.
	    dp = diff_alloc_new(curtab, dprev, dp);
	    जर (dp == रिक्त)
		जा done;

	    dp->df_lnum[idx_orig] = hunk->lnum_orig;
	    dp->df_मोजणी[idx_orig] = hunk->मोजणी_orig;
	    dp->df_lnum[idx_new] = hunk->lnum_new;
	    dp->df_मोजणी[idx_new] = hunk->मोजणी_new;

	    // Set values for other buffers, these must be equal to the
	    // original buffer, otherwise there would have been a change
	    // already.
	    साठी (i = idx_orig + 1; i < idx_new; ++i)
		जर (curtab->tp_diffbuf[i] != रिक्त)
		    diff_copy_entry(dprev, dp, idx_orig, i);
	}
	notset = FALSE;		// "*dp" has been set
    }

    // for remaining diff blocks orig and new are equal
    पर्यंत (dp != रिक्त)
    {
	जर (notset)
	    diff_copy_entry(dprev, dp, idx_orig, idx_new);
	dprev = dp;
	dp = dp->df_next;
	notset = TRUE;
    }

done:
    जर (!dio->dio_internal)
	vim_free(hunk);

    जर (fd != रिक्त)
	ख_बंद(fd);
}

/*
 * Copy an entry at "dp" from "idx_orig" to "idx_new".
 */
    स्थिर शून्य
diff_copy_entry(
    diff_T	*dprev,
    diff_T	*dp,
    पूर्णांक		idx_orig,
    पूर्णांक		idx_new)
{
    दीर्घ	off;

    जर (dprev == रिक्त)
	off = 0;
    अन्यथा
	off = (dprev->df_lnum[idx_orig] + dprev->df_मोजणी[idx_orig])
	    - (dprev->df_lnum[idx_new] + dprev->df_मोजणी[idx_new]);
    dp->df_lnum[idx_new] = dp->df_lnum[idx_orig] - off;
    dp->df_मोजणी[idx_new] = dp->df_मोजणी[idx_orig];
}

/*
 * Clear the list of diffblocks for tab page "tp".
 */
    शून्य
diff_clear(tabpage_T *tp)
{
    diff_T	*p, *next_p;

    साठी (p = tp->tp_first_diff; p != रिक्त; p = next_p)
    {
	next_p = p->df_next;
	clear_diffblock(p);
    }
    tp->tp_first_diff = रिक्त;
}

/*
 *  return true if the options are set to use diff linematch
 */
    स्थिर पूर्णांक
diff_linematch(diff_T *dp)
{
    जर (!(diff_flags & DIFF_LINEMATCH))
	परत 0;

    // are there more than three diff buffers?
    पूर्णांक tsize = 0;
    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
    {
	जर (curtab->tp_diffbuf[i] != रिक्त)
	{
	    // for the rare case (bug?) that the count of a diff block is
	    // negative, do not run the algorithm because this will try to
	    // allocate a negative amount of space and crash
	    जर (dp->df_मोजणी[i] < 0)
		परत FALSE;
	    tsize += dp->df_मोजणी[i];
	}
    }

    // avoid allocating a huge array because it will lag
    परत tsize <= linematch_ओळी;
}

    स्थिर पूर्णांक
get_max_diff_length(स्थिरांक diff_T *dp)
{
    पूर्णांक maxlength = 0;

    साठी (पूर्णांक k = 0; k < DB_मोजणी; k++)
    {
	जर (curtab->tp_diffbuf[k] != रिक्त)
	{
	    जर (dp->df_मोजणी[k] > maxlength)
		maxlength = dp->df_मोजणी[k];
	}
    }
    परत maxlength;
}

/*
 * Find the first diff block that includes the specified line. Also find the
 * next diff block that's not in the current chain of adjacent blocks that are
 * all touching each other directly.
 */
    स्थिर शून्य
find_शीर्ष_diff_block(
    diff_T	**thistopdiff,
    diff_T	**next_adjacent_blocks,
    पूर्णांक		fromidx,
    पूर्णांक		topline)
{
    diff_T	*topdiff = रिक्त;
    diff_T	*localtopdiff = रिक्त;
    पूर्णांक		topdiffchange = 0;

    साठी (topdiff = curtab->tp_first_diff; topdiff != रिक्त;
						topdiff = topdiff->df_next)
    {
	// set the top of the current overlapping diff block set as we
	// iterate through all of the sets of overlapping diff blocks
	जर (!localtopdiff || topdiffchange)
	{
	    localtopdiff = topdiff;
	    topdiffchange = 0;
	}

	// check if the fromwin topline is matched by the current diff. if so,
	// set it to the top of the diff block
	जर (topline >= topdiff->df_lnum[fromidx] && topline <=
		(topdiff->df_lnum[fromidx] + topdiff->df_मोजणी[fromidx]))
	{
	    // this line is inside the current diff block, so we will save the
	    // top block of the set of blocks to refer to later
	    जर ((*thistopdiff) == रिक्त)
		(*thistopdiff) = localtopdiff;
	}

	// check if the next set of overlapping diff blocks is next
	जर (!(topdiff->df_next && (topdiff->df_next->df_lnum[fromidx] ==
			(topdiff->df_lnum[fromidx] +
						topdiff->df_मोजणी[fromidx]))))
	{
	    // mark that the next diff block is belongs to a different set of
	    // overlapping diff blocks
	    topdiffchange = 1;

	    // if we already have found that the line number is inside a diff
	    // block, set the marker of the next block and finish the iteration
	    जर (*thistopdiff)
	    {
		(*next_adjacent_blocks) = topdiff->df_next;
		थांबा;
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
    स्थिरांक पूर्णांक	fromidx,
    स्थिरांक पूर्णांक	toidx,
    स्थिरांक पूर्णांक	from_topline,
    स्थिरांक पूर्णांक	from_topfill,
    पूर्णांक		*topfill,
    linenr_T	*topline)
{
    // find the position from the top of the diff block, and the next diff
    // block that's no longer adjacent to the current block. "Adjacency" means
    // a chain of diff blocks that are directly touching each other, allowed by
    // linematch and diff anchors.
    diff_T	*thistopdiff = रिक्त;
    diff_T	*next_adjacent_blocks = रिक्त;
    पूर्णांक		virtual_ओळी_passed = 0;
    पूर्णांक		curlinenum_to = 1;

    find_शीर्ष_diff_block(&thistopdiff, &next_adjacent_blocks, fromidx, from_topline);

    // count the virtual lines (either filler or concrete line) that have been
    // passed in the source buffer. There could be multiple diff blocks if
    // there are adjacent empty blocks (count == 0 at fromidx).
    diff_T *curdif = thistopdiff;
    पर्यंत (curdif && (curdif->df_lnum[fromidx] + curdif->df_मोजणी[fromidx])
							<= from_topline)
    {
	virtual_ओळी_passed += get_max_diff_length(curdif);

	curdif = curdif->df_next;
    }

    जर (curdif != next_adjacent_blocks)
	virtual_ओळी_passed += from_topline - curdif->df_lnum[fromidx];
    virtual_ओळी_passed -= from_topfill;

    // clamp negative values in case from_topfill hasn't been updated yet and
    // is larger than total virtual lines, which could happen when setting
    // diffopt multiple times
    जर (virtual_ओळी_passed < 0)
	virtual_ओळी_passed = 0;

    // move the same amount of virtual lines in the target buffer to find the
    // cursor's line number
    जर (thistopdiff != रिक्त) // this should not be null, but just for safety
	curlinenum_to = thistopdiff->df_lnum[toidx];

    पूर्णांक virt_ओळी_डावा = virtual_ओळी_passed;
    curdif = thistopdiff;
    पर्यंत (virt_ओळी_डावा > 0 && curdif != रिक्त && curdif != next_adjacent_blocks)
    {
	curlinenum_to += MIN(virt_ओळी_डावा, curdif->df_मोजणी[toidx]);
	virt_ओळी_डावा -= MIN(virt_ओळी_डावा, get_max_diff_length(curdif));
	curdif = curdif->df_next;
    }

    // count the total number of virtual lines between the top diff block and
    // the found line in the target buffer
    पूर्णांक max_virt_ओळी = 0;
    साठी (diff_T *dp = thistopdiff; dp != रिक्त; dp = dp->df_next)
    {
	जर (dp->df_lnum[toidx] + dp->df_मोजणी[toidx] <= curlinenum_to)
	    max_virt_ओळी += get_max_diff_length(dp);
	अन्यथा
	{
	    जर (dp->df_lnum[toidx] <= curlinenum_to)
		max_virt_ओळी += curlinenum_to - dp->df_lnum[toidx];
	    थांबा;
	}
    }

    जर (diff_flags & DIFF_FILLER)
	// should always be non-negative as max_virt_lines is larger
	(*topfill) = max_virt_ओळी - virtual_ओळी_passed;
    (*topline) = curlinenum_to;
}

// Apply results from the linematch algorithm and apply to 'dp' by splitting it
// into multiple adjacent diff blocks.
    स्थिर शून्य
apply_linematch_results(
    diff_T	*dp,
    माप_प्रकार	decisions_length,
    स्थिरांक पूर्णांक	*decisions)
{
    // get the start line number here in each diff buffer, and then increment
    पूर्णांक		ओळ_numbers[DB_मोजणी];
    पूर्णांक		outputmap[DB_मोजणी];
    माप_प्रकार	ndiffs = 0;

    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
    {
	जर (curtab->tp_diffbuf[i] != रिक्त)
	{
	    ओळ_numbers[i] = dp->df_lnum[i];
	    dp->df_मोजणी[i] = 0;

	    // Keep track of the index of the diff buffer we are using here.
	    // We will use this to write the output of the algorithm to
	    // diff_T structs at the correct indexes
	    outputmap[ndiffs] = i;
	    ndiffs++;
	}
    }

    // write the diffs starting with the current diff block
    diff_T *dp_s = dp;
    साठी (माप_प्रकार i = 0; i < decisions_length; i++)
    {
	// Don't allocate on first iter since we can reuse the initial
	// diffblock
	जर (i != 0 && (decisions[i - 1] != decisions[i]))
	{
	    // create new sub diff blocks to segment the original diff block
	    // which we further divided by running the linematch algorithm
	    dp_s = diff_alloc_new(curtab, dp_s, dp_s->df_next);
	    dp_s->is_linematched = TRUE;
	    साठी (पूर्णांक j = 0; j < DB_मोजणी; j++)
	    {
		जर (curtab->tp_diffbuf[j] != रिक्त)
		{
		    dp_s->df_lnum[j] = ओळ_numbers[j];
		    dp_s->df_मोजणी[j] = 0;
		}
	    }
	}
	साठी (माप_प्रकार j = 0; j < ndiffs; j++)
	{
	    जर (decisions[i] & (1 << j))
	    {
		// will need to use the map here
		dp_s->df_मोजणी[outputmap[j]]++;
		ओळ_numbers[outputmap[j]]++;
	    }
	}
    }
    dp->is_linematched = TRUE;
}

    स्थिर शून्य
run_linematch_algorithm(diff_T *dp)
{
    // define buffers for diff algorithm
    diffin_T		diffbufs_mm[DB_मोजणी];
    स्थिरांक mmfile_t	*diffbufs[DB_मोजणी];
    पूर्णांक			diff_length[DB_मोजणी];
    माप_प्रकार		ndiffs = 0;

    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
    {
	जर (curtab->tp_diffbuf[i] != रिक्त)
	{
	    // write the contents of the entire buffer to
	    // diffbufs_mm[diffbuffers_count]
	    जर (dp->df_मोजणी[i] > 0)
	    {
		diff_write_buffer(curtab->tp_diffbuf[i], &diffbufs_mm[ndiffs],
			dp->df_lnum[i], dp->df_lnum[i] + dp->df_मोजणी[i] - 1);
	    }
	    अन्यथा
	    {
		diffbufs_mm[ndiffs].din_mmfile.size = 0;
		diffbufs_mm[ndiffs].din_mmfile.ptr = रिक्त;
	    }

	    diffbufs[ndiffs] = &diffbufs_mm[ndiffs].din_mmfile;

	    // keep track of the length of this diff block to pass it to the
	    // linematch algorithm
	    diff_length[ndiffs] = dp->df_मोजणी[i];

	    // increment the amount of diff buffers we are passing to the
	    // algorithm
	    ndiffs++;
	}
    }

    // we will get the output of the linematch algorithm in the format of an
    // array of integers (*decisions) and the length of that array
    // (decisions_length)
    पूर्णांक *decisions = रिक्त;
    स्थिरांक पूर्णांक iwhite = (diff_flags & (DIFF_IWHITEALL | DIFF_IWHITE)) > 0 ? 1 : 0;
    माप_प्रकार decisions_length =
	linematch_nbuffers(diffbufs, diff_length, ndiffs, &decisions, iwhite);

    साठी (माप_प्रकार i = 0; i < ndiffs; i++)
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
    पूर्णांक
diff_check_with_linestatus(win_T *wp, linenr_T lnum, पूर्णांक *linestatus)
{
    पूर्णांक		idx;		// index in tp_diffbuf[] for this buffer
    diff_T	*dp;
    पूर्णांक		maxcount;
    पूर्णांक		i;
    buf_T	*buf = wp->w_buffer;
    पूर्णांक		cmp;

    जर (linestatus != रिक्त)
	*linestatus = 0;

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जर (curtab->tp_first_diff == रिक्त || !wp->w_p_diff)	// no diffs at all
	परत 0;

    // safety check: "lnum" must be a buffer line
    जर (lnum < 1 || lnum > buf->b_ml.ml_ओळ_मोजणी + 1)
	परत 0;

    idx = diff_buf_idx(buf);
    जर (idx == DB_मोजणी)
	परत 0;		// no diffs for buffer "buf"

# ifdef FEAT_FOLDING
    // A closed fold never has filler lines.
    जर (hasFoldingWin(wp, lnum, रिक्त, रिक्त, TRUE, रिक्त))
	परत 0;
# endif

    // Search for a change that includes "lnum" in the list of diffblocks.
    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = रिक्त;		// last block with df_lnum[idx] <= lnum
    साठी (dp = diff_scan_start(curtab, idx, lnum); dp != रिक्त; dp = dp->df_next)
    {
	जर (dp->df_lnum[idx] <= lnum)
	    le = dp;
	जर (lnum <= dp->df_lnum[idx] + dp->df_मोजणी[idx])
	    थांबा;
    }
    diff_scan_remember(curtab, idx, lnum, le);
    जर (dp == रिक्त || lnum < dp->df_lnum[idx])
	परत 0;

    // Don't run linematch when lnum is offscreen.  Useful for scrollbind
    // calculations which need to count all the filler lines above the screen.
    जर (lnum >= wp->w_topline && lnum < wp->w_botline
				&& !dp->is_linematched && diff_linematch(dp)
				&& diff_check_sanity(curtab, dp))
	run_linematch_algorithm(dp);

    // Insert filler lines above the line just below the change.  Will return 0
    // when this buf had the max count.
    पूर्णांक अंक_fill = 0;
    पर्यंत (lnum == dp->df_lnum[idx] + dp->df_मोजणी[idx])
    {
	// Only calculate fill lines if 'diffopt' contains "filler". Otherwise
	// returns 0 filler lines.
	जर (diff_flags & DIFF_FILLER)
	{
	    maxcount = get_max_diff_length(dp);
	    अंक_fill += maxcount - dp->df_मोजणी[idx];
	}

	// If there are adjacent blocks (e.g. linematch or anchor), loop
	// through them. It's possible for multiple adjacent blocks to
	// contribute to filler lines.
	// This also helps us find the last diff block in the list of adjacent
	// blocks which is necessary when it is a change/inserted line right
	// after added lines.
	जर (dp->df_next != रिक्त
		&& lnum >= dp->df_next->df_lnum[idx]
		&& lnum <= dp->df_next->df_lnum[idx] + dp->df_next->df_मोजणी[idx])
	    dp = dp->df_next;
	अन्यथा
	    थांबा;
    }

    जर (lnum < dp->df_lnum[idx] + dp->df_मोजणी[idx])
    {
	पूर्णांक	zero = FALSE;

	// Changed or inserted line.  If the other buffers have a count of
	// zero, the lines were inserted.  If the other buffers have the same
	// count, check if the lines are identical.
	cmp = FALSE;
	साठी (i = 0; i < DB_मोजणी; ++i)
	    जर (i != idx && curtab->tp_diffbuf[i] != रिक्त)
	    {
		जर (dp->df_मोजणी[i] == 0)
		    zero = TRUE;
		अन्यथा
		{
		    जर (dp->df_मोजणी[i] != dp->df_मोजणी[idx])
		    {
			जर (linestatus)
			    *linestatus = -1;	// nr of lines changed.
			परत अंक_fill;
		    }
		    cmp = TRUE;
		}
	    }
	जर (cmp)
	{
	    // Compare all lines.  If they are equal the lines were inserted
	    // in some buffers, deleted in others, but not changed.
	    साठी (i = 0; i < DB_मोजणी; ++i)
		जर (i != idx && curtab->tp_diffbuf[i] != रिक्त
						      && dp->df_मोजणी[i] != 0)
		    जर (!diff_equal_entry(dp, idx, i))
		    {
			जर (linestatus)
			    *linestatus = -1;
			परत अंक_fill;
		    }
	}
	// If there is no buffer with zero lines then there is no difference
	// any longer.  Happens when making a change (or undo) that removes
	// the difference.  Can't remove the entry here, we might be halfway
	// updating the window.  Just report the text as unchanged.  Other
	// windows might still show the change though.
	जर (zero == FALSE)
	    परत अंक_fill;
	जर (linestatus)
	    *linestatus = -2;
	परत अंक_fill;
    }
    परत अंक_fill;
}

/*
 * Compare two entries in diff "*dp" and return TRUE if they are equal.
 */
    स्थिर पूर्णांक
diff_equal_entry(diff_T *dp, पूर्णांक idx1, पूर्णांक idx2)
{
    पूर्णांक		i;
    char_u	*ओळ;
    पूर्णांक		cmp;

    जर (dp->df_मोजणी[idx1] != dp->df_मोजणी[idx2])
	परत FALSE;
    जर (diff_check_sanity(curtab, dp) == FAIL)
	परत FALSE;
    साठी (i = 0; i < dp->df_मोजणी[idx1]; ++i)
    {
	ओळ = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx1],
					       dp->df_lnum[idx1] + i, FALSE));
	जर (ओळ == रिक्त)
	    परत FALSE;
	cmp = diff_cmp(ओळ, ml_get_buf(curtab->tp_diffbuf[idx2],
					       dp->df_lnum[idx2] + i, FALSE));
	vim_free(ओळ);
	जर (cmp != 0)
	    परत FALSE;
    }
    परत TRUE;
}

/*
 * Compare the characters at "p1" and "p2".  If they are equal (possibly
 * ignoring case) return TRUE and set "len" to the number of bytes.
 */
    स्थिर पूर्णांक
diff_equal_char(char_u *p1, char_u *p2, पूर्णांक *len)
{
    पूर्णांक l  = (*mb_ptr2len)(p1);

    जर (l != (*mb_ptr2len)(p2))
	परत FALSE;
    जर (l > 1)
    {
	जर (STRNCMP(p1, p2, l) != 0
		&& (!enc_utf8
		    || !(diff_flags & DIFF_ICASE)
		    || utf_fold(utf_ptr2char(p1))
						!= utf_fold(utf_ptr2char(p2))))
	    परत FALSE;
	*len = l;
    }
    अन्यथा
    {
	जर ((*p1 != *p2)
		&& (!(diff_flags & DIFF_ICASE)
		    || TOLOWER_LOC(*p1) != TOLOWER_LOC(*p2)))
	    परत FALSE;
	*len = 1;
    }
    परत TRUE;
}

/*
 * Compare strings "s1" and "s2" according to 'diffopt'.
 * Return non-zero when they are different.
 */
    स्थिर पूर्णांक
diff_cmp(char_u *s1, char_u *s2)
{
    char_u	*p1, *p2;
    पूर्णांक		l;

    जर ((diff_flags & DIFF_IBLANK)
	    && (*skipwhite(s1) == NUL || *skipwhite(s2) == NUL))
	परत 0;

    जर ((diff_flags & (DIFF_ICASE | ALL_WHITE_DIFF)) == 0)
	परत STRCMP(s1, s2);
    जर ((diff_flags & DIFF_ICASE) && !(diff_flags & ALL_WHITE_DIFF))
	परत MB_STRICMP(s1, s2);

    p1 = s1;
    p2 = s2;

    // Ignore white space changes and possibly ignore case.
    पर्यंत (*p1 != NUL && *p2 != NUL)
    {
	जर (((diff_flags & DIFF_IWHITE)
		    && VIM_ISWHITE(*p1) && VIM_ISWHITE(*p2))
		|| ((diff_flags & DIFF_IWHITEALL)
		    && (VIM_ISWHITE(*p1) || VIM_ISWHITE(*p2))))
	{
	    p1 = skipwhite(p1);
	    p2 = skipwhite(p2);
	}
	अन्यथा
	{
	    जर (!diff_equal_char(p1, p2, &l))
		थांबा;
	    p1 += l;
	    p2 += l;
	}
    }

    // Ignore trailing white space.
    p1 = skipwhite(p1);
    p2 = skipwhite(p2);
    जर (*p1 != NUL || *p2 != NUL)
	परत 1;
    परत 0;
}

/*
 * Return the number of filler lines above "lnum".
 */
    पूर्णांक
diff_check_fill(win_T *wp, linenr_T lnum)
{
    पूर्णांक		n;

    // be quick when there are no filler lines
    जर (!(diff_flags & DIFF_FILLER))
	परत 0;
    n = diff_check_with_linestatus(wp, lnum, रिक्त);
    जर (n <= 0)
	परत 0;
    परत n;
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
    पूर्णांक		fromidx;
    पूर्णांक		toidx;
    diff_T	*dp;

    fromidx = diff_buf_idx(frombuf);
    जर (fromidx == DB_मोजणी)
	परत;		// safety check

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    lnum = fromwin->w_topline;
    towin->w_topfill = 0;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जर (lnum <= dp->df_lnum[fromidx] + dp->df_मोजणी[fromidx])
	    थांबा;
    जर (dp == रिक्त)
    {
	// After last change, compute topline relative to end of file; no
	// filler lines.
	towin->w_topline = towin->w_buffer->b_ml.ml_ओळ_मोजणी
				       - (frombuf->b_ml.ml_ओळ_मोजणी - lnum);
    }
    अन्यथा
    {
	// Find index for "towin".
	toidx = diff_buf_idx(towin->w_buffer);
	जर (toidx == DB_मोजणी)
	    परत;		// safety check

	towin->w_topline = lnum + (dp->df_lnum[toidx] - dp->df_lnum[fromidx]);
	जर (lnum >= dp->df_lnum[fromidx])
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
    जर (towin->w_topline > towin->w_buffer->b_ml.ml_ओळ_मोजणी)
    {
	towin->w_topline = towin->w_buffer->b_ml.ml_ओळ_मोजणी;
	towin->w_botfill = true;
    }
    जर (towin->w_topline < 1)
    {
	towin->w_topline = 1;
	towin->w_topfill = 0;
    }

    // When w_topline changes need to recompute w_botline and cursor position
    invalidate_botline_win(towin);
    changed_ओळ_abv_curs_win(towin);

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
    स्थिर पूर्णांक
parse_diffanchors(
    पूर्णांक		check_only,
    buf_T	*buf,
    linenr_T	*anchors,
    पूर्णांक		*अंक_anchors)
{
    पूर्णांक i;
    char_u *dia = (*buf->b_p_dia == NUL) ? p_dia : buf->b_p_dia;

    buf_T *orig_curbuf = curbuf;
    win_T *orig_curwin = curwin;

    win_T *bufwin = रिक्त;
    जर (check_only)
	bufwin = curwin;
    अन्यथा
    {
	// Find the first window tied to this buffer and ignore the rest. Will
	// only matter for window-specific addresses like `.` or `''`.
	FOR_ALL_WINDOWS(bufwin)
	    जर (bufwin->w_buffer == buf && bufwin->w_p_diff)
		थांबा;
	जर (bufwin == रिक्त && *dia != NUL)
	{
	    // The buffer is hidden. Currently this is not supported due to the
	    // edge cases of needing to decide if an address is window-specific
	    // or not. We could add more checks in the future so we can detect
	    // whether an address relies on curwin to make this more fleixble.
	    emsg(_(e_diff_anchors_with_hidden_windows));
	    परत FAIL;
	}
    }

    साठी (i = 0; i < MAX_DIFF_ANCHORS && *dia != NUL; i++)
    {
	जर (*dia == ',') // don't allow empty values
	    परत FAIL;

	curbuf = buf;
	curwin = bufwin;
	linenr_T lnum = get_address(रिक्त, &dia, ADDR_ओळी, check_only, TRUE, FALSE, 1);
	curbuf = orig_curbuf;
	curwin = orig_curwin;

	जर (dia == रिक्त) // error detected
	    परत FAIL;
	जर (*dia != ',' && *dia != NUL)
	    परत FAIL;

	जर (!check_only
		&& (lnum == MAXLNUM || lnum <= 0 || lnum > buf->b_ml.ml_ओळ_मोजणी + 1))
	{
	    emsg(_(e_invalid_range));
	    परत FAIL;
	}

	जर (anchors != रिक्त)
	    anchors[i] = lnum;

	जर (*dia == ',')
	    dia++;
    }
    जर (i == MAX_DIFF_ANCHORS && *dia != NUL)
    {
	semsg(_(e_cannot_have_more_than_nr_diff_anchors), MAX_DIFF_ANCHORS);
	परत FAIL;
    }
    जर (अंक_anchors != रिक्त)
	*अंक_anchors = i;
    परत OK;
}

/*
 * This is called when 'diffanchors' is changed.
 */
    पूर्णांक
diffanchors_changed(पूर्णांक buflocal)
{
    पूर्णांक result = parse_diffanchors(TRUE, curbuf, रिक्त, रिक्त);
    जर (result == OK && (diff_flags & DIFF_ANCHOR))
    {
	tabpage_T	*tp;
	FOR_ALL_TABPAGES(tp)
	{
	    जर (!buflocal)
		tp->tp_diff_invalid = TRUE;
	    अन्यथा
	    {
		साठी (पूर्णांक idx = 0; idx < DB_मोजणी; ++idx)
		    जर (tp->tp_diffbuf[idx] == curbuf)
		    {
			tp->tp_diff_invalid = TRUE;
			थांबा;
		    }
	    }
	}
    }
    परत result;
}

/*
 * This is called when 'diffopt' is changed.
 */
    पूर्णांक
diffopt_changed(शून्य)
{
    char_u	*p;
    पूर्णांक		diff_context_new = 6;
    पूर्णांक		linematch_ओळी_new = 0;
    पूर्णांक		diff_flags_new = 0;
    पूर्णांक		diff_foldcolumn_new = 2;
    दीर्घ	diff_algorithm_new = 0;
    दीर्घ	diff_indent_heuristic = 0;
    tabpage_T	*tp;

    p = p_dip;
    पर्यंत (*p != NUL)
    {
	// Note: Keep this in sync with p_dip_values
	जर (STRNCMP(p, "filler", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_FILLER;
	}
	अन्यथा जर (STRNCMP(p, "anchor", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_ANCHOR;
	}
	अन्यथा जर (STRNCMP(p, "context:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    diff_context_new = getdigits(&p);
	}
	अन्यथा जर (STRNCMP(p, "iblank", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IBLANK;
	}
	अन्यथा जर (STRNCMP(p, "icase", 5) == 0)
	{
	    p += 5;
	    diff_flags_new |= DIFF_ICASE;
	}
	अन्यथा जर (STRNCMP(p, "iwhiteall", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEALL;
	}
	अन्यथा जर (STRNCMP(p, "iwhiteeol", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_IWHITEEOL;
	}
	अन्यथा जर (STRNCMP(p, "iwhite", 6) == 0)
	{
	    p += 6;
	    diff_flags_new |= DIFF_IWHITE;
	}
	अन्यथा जर (STRNCMP(p, "horizontal", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_HORIZONTAL;
	}
	अन्यथा जर (STRNCMP(p, "vertical", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_VERTICAL;
	}
	अन्यथा जर (STRNCMP(p, "foldcolumn:", 11) == 0 && VIM_ISDIGIT(p[11]))
	{
	    p += 11;
	    diff_foldcolumn_new = getdigits(&p);
	}
	अन्यथा जर (STRNCMP(p, "hiddenoff", 9) == 0)
	{
	    p += 9;
	    diff_flags_new |= DIFF_HIDDEN_OFF;
	}
	अन्यथा जर (STRNCMP(p, "closeoff", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_CLOSE_OFF;
	}
	अन्यथा जर (STRNCMP(p, "followwrap", 10) == 0)
	{
	    p += 10;
	    diff_flags_new |= DIFF_FOLLOWWRAP;
	}
	अन्यथा जर (STRNCMP(p, "indent-heuristic", 16) == 0)
	{
	    p += 16;
	    diff_indent_heuristic = XDF_INDENT_HEURISTIC;
	}
	अन्यथा जर (STRNCMP(p, "internal", 8) == 0)
	{
	    p += 8;
	    diff_flags_new |= DIFF_INTERNAL;
	}
	अन्यथा जर (STRNCMP(p, "algorithm:", 10) == 0)
	{
	    // Note: Keep this in sync with p_dip_algorithm_values.
	    p += 10;
	    जर (STRNCMP(p, "myers", 5) == 0)
	    {
		p += 5;
		diff_algorithm_new = 0;
	    }
	    अन्यथा जर (STRNCMP(p, "minimal", 7) == 0)
	    {
		p += 7;
		diff_algorithm_new = XDF_NEED_MINIMAL;
	    }
	    अन्यथा जर (STRNCMP(p, "patience", 8) == 0)
	    {
		p += 8;
		diff_algorithm_new = XDF_PATIENCE_DIFF;
	    }
	    अन्यथा जर (STRNCMP(p, "histogram", 9) == 0)
	    {
		p += 9;
		diff_algorithm_new = XDF_HISTOGRAM_DIFF;
	    }
	    अन्यथा
		परत FAIL;
	}
	अन्यथा जर (STRNCMP(p, "inline:", 7) == 0)
	{
	    // Note: Keep this in sync with p_dip_inline_values.
	    p += 7;
	    जर (STRNCMP(p, "none", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_NONE;
	    }
	    अन्यथा जर (STRNCMP(p, "simple", 6) == 0)
	    {
		p += 6;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_SIMPLE;
	    }
	    अन्यथा जर (STRNCMP(p, "char", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_CHAR;
	    }
	    अन्यथा जर (STRNCMP(p, "word", 4) == 0)
	    {
		p += 4;
		diff_flags_new &= ~(ALL_INLINE);
		diff_flags_new |= DIFF_INLINE_WORD;
	    }
	    अन्यथा
		परत FAIL;
	}
	अन्यथा जर (STRNCMP(p, "linematch:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    linematch_ओळी_new = getdigits(&p);
	    diff_flags_new |= DIFF_LINEMATCH;

	    // linematch does not make sense without filler set
	    diff_flags_new |= DIFF_FILLER;
	}

	जर (*p != ',' && *p != NUL)
	    परत FAIL;
	जर (*p == ',')
	    ++p;
    }

    diff_algorithm_new |= diff_indent_heuristic;

    // Can't have both "horizontal" and "vertical".
    जर ((diff_flags_new & DIFF_HORIZONTAL) && (diff_flags_new & DIFF_VERTICAL))
	परत FAIL;

    // If flags were added or removed, or the algorithm was changed, need to
    // update the diff.
    जर (diff_flags != diff_flags_new || diff_algorithm != diff_algorithm_new)
	FOR_ALL_TABPAGES(tp)
	    tp->tp_diff_invalid = TRUE;

    diff_flags = diff_flags_new;
    diff_context = diff_context_new == 0 ? 1 : diff_context_new;
    linematch_ओळी = linematch_ओळी_new;
    diff_foldcolumn = diff_foldcolumn_new;
    diff_algorithm = diff_algorithm_new;

    diff_redraw(TRUE);

    // recompute the scroll binding with the new option value, may
    // remove or add filler lines
    check_scrollbind((linenr_T)0, 0L);

    परत OK;
}

/*
 * Return TRUE if 'diffopt' contains "horizontal".
 */
    पूर्णांक
diffopt_horizontal(शून्य)
{
    परत (diff_flags & DIFF_HORIZONTAL) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "hiddenoff".
 */
    पूर्णांक
diffopt_hiddenoff(शून्य)
{
    परत (diff_flags & DIFF_HIDDEN_OFF) != 0;
}

/*
 * Return TRUE if 'diffopt' contains "closeoff".
 */
    पूर्णांक
diffopt_closeoff(शून्य)
{
    परत (diff_flags & DIFF_CLOSE_OFF) != 0;
}

/*
 * Called when a line has been updated. Used for updating inline diff in Insert
 * mode without waiting for global diff update later.
 */
    शून्य
diff_update_ओळ(linenr_T lnum)
{
    पूर्णांक		idx;
    diff_T	*dp;

    जर (!(diff_flags & ALL_INLINE_DIFF))
	// We only care if we are doing inline-diff where we cache the diff results
	परत;

    idx = diff_buf_idx(curbuf);
    जर (idx == DB_मोजणी)
	परत;
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जर (lnum <= dp->df_lnum[idx] + dp->df_मोजणी[idx])
	    थांबा;

    // clear the inline change cache as it's invalid
    जर (dp != रिक्त)
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
    पूर्णांक
diff_change_parse(
    diffline_T *diffline,
    diffline_change_T *change,
    पूर्णांक *change_start,
    पूर्णांक *change_end)
{
    जर (change->dc_start_lnum_off[diffline->bufidx] < diffline->lineoff)
	*change_start = 0;
    अन्यथा
	*change_start = change->dc_start[diffline->bufidx];
    जर (change->dc_end_lnum_off[diffline->bufidx] > diffline->lineoff)
	*change_end = पूर्णांक_उच्च;
    अन्यथा
	*change_end = change->dc_end[diffline->bufidx];

    जर (change == &simple_diffline_change)
    {
	// This is what we returned from simple inline diff. We always consider
	// the range to be changed, rather than added for now.
	परत FALSE;
    }

    // Find out whether this is an addition. Note that for multi buffer diff,
    // to tell whether lines are additions we check whether all the other diff
    // lines are identical (in diff_check_with_linestatus). If so, we mark them
    // as add. We don't do that for inline diff here for simplicity.
    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
    {
	जर (i == diffline->bufidx)
	    सुरूठेवा;
	जर (change->dc_start[i] != change->dc_end[i]
		|| change->dc_end_lnum_off[i] != change->dc_start_lnum_off[i])
	{
	    परत FALSE;
	}
    }
    परत TRUE;
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
    स्थिर पूर्णांक
diff_find_change_simple(
    win_T	*wp,
    linenr_T	lnum,
    diff_T	*dp,
    पूर्णांक		idx,
    पूर्णांक		*startp,	// first char of the change
    पूर्णांक		*endp)		// last char of the change
{
    char_u	*ओळ_org;
    char_u	*ओळ_new;
    पूर्णांक		i;
    पूर्णांक		si_org, si_new;
    पूर्णांक		ei_org, ei_new;
    पूर्णांक		off;
    पूर्णांक		added = TRUE;
    char_u	*p1, *p2;
    पूर्णांक		l;

    जर (diff_flags & DIFF_INLINE_NONE)
    {
	// We only care about the return value, not the actual string comparisons.
	ओळ_org = रिक्त;
    }
    अन्यथा
    {
	// Make a copy of the line, the next ml_get() will invalidate it.
	ओळ_org = vim_strsave(ml_get_buf(wp->w_buffer, lnum, FALSE));
	जर (ओळ_org == रिक्त)
	    परत FALSE;
    }

    off = lnum - dp->df_lnum[idx];

    साठी (i = 0; i < DB_मोजणी; ++i)
	जर (curtab->tp_diffbuf[i] != रिक्त && i != idx)
	{
	    // Skip lines that are not in the other change (filler lines).
	    जर (off >= dp->df_मोजणी[i])
		सुरूठेवा;
	    added = FALSE;
	    जर (diff_flags & DIFF_INLINE_NONE)
		थांबा; // early terminate as we only care about the return value

	    ओळ_new = ml_get_buf(curtab->tp_diffbuf[i],
						 dp->df_lnum[i] + off, FALSE);

	    // Search for start of difference
	    si_org = si_new = 0;
	    पर्यंत (ओळ_org[si_org] != NUL)
	    {
		जर (((diff_flags & DIFF_IWHITE)
			    && VIM_ISWHITE(ओळ_org[si_org])
					      && VIM_ISWHITE(ओळ_new[si_new]))
			|| ((diff_flags & DIFF_IWHITEALL)
			    && (VIM_ISWHITE(ओळ_org[si_org])
					    || VIM_ISWHITE(ओळ_new[si_new]))))
		{
		    si_org = (पूर्णांक)(skipwhite(ओळ_org + si_org) - ओळ_org);
		    si_new = (पूर्णांक)(skipwhite(ओळ_new + si_new) - ओळ_new);
		}
		अन्यथा
		{
		    जर (!diff_equal_char(ओळ_org + si_org, ओळ_new + si_new,
									   &l))
			थांबा;
		    si_org += l;
		    si_new += l;
		}
	    }
	    जर (has_mbyte)
	    {
		// Move back to first byte of character in both lines (may
		// have "nn^" in line_org and "n^ in line_new).
		si_org -= (*mb_head_off)(ओळ_org, ओळ_org + si_org);
		si_new -= (*mb_head_off)(ओळ_new, ओळ_new + si_new);
	    }
	    जर (*startp > si_org)
		*startp = si_org;

	    // Search for end of difference, if any.
	    जर (ओळ_org[si_org] != NUL || ओळ_new[si_new] != NUL)
	    {
		ei_org = (पूर्णांक)STRLEN(ओळ_org);
		ei_new = (पूर्णांक)STRLEN(ओळ_new);
		पर्यंत (ei_org >= *startp && ei_new >= si_new
						&& ei_org >= 0 && ei_new >= 0)
		{
		    जर (((diff_flags & DIFF_IWHITE)
				&& VIM_ISWHITE(ओळ_org[ei_org])
					      && VIM_ISWHITE(ओळ_new[ei_new]))
			    || ((diff_flags & DIFF_IWHITEALL)
				&& (VIM_ISWHITE(ओळ_org[ei_org])
					    || VIM_ISWHITE(ओळ_new[ei_new]))))
		    {
			पर्यंत (ei_org >= *startp
					     && VIM_ISWHITE(ओळ_org[ei_org]))
			    --ei_org;
			पर्यंत (ei_new >= si_new
					     && VIM_ISWHITE(ओळ_new[ei_new]))
			    --ei_new;
		    }
		    अन्यथा
		    {
			p1 = ओळ_org + ei_org;
			p2 = ओळ_new + ei_new;
			p1 -= (*mb_head_off)(ओळ_org, p1);
			p2 -= (*mb_head_off)(ओळ_new, p2);
			जर (!diff_equal_char(p1, p2, &l))
			    थांबा;
			ei_org -= l;
			ei_new -= l;
		    }
		}
		जर (*endp < ei_org)
		    *endp = ei_org;
	    }
	}

    vim_free(ओळ_org);
    परत added;
}

/*
 * Mapping used for mapping from temporary mmfile created for inline diff back
 * to original buffer's line/col.
 */
प्रकारव्याख्या संरचना
{
    दीर्घ byte_start;
    दीर्घ अंक_bytes;
    पूर्णांक lineoff;
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
diff_refine_inline_char_highlight(diff_T *dp_orig, garray_T *linemap, पूर्णांक idx1)
{
    // Perform multiple passes so that newly merged blocks will now be long
    // enough which may cause other previously unmerged gaps to be merged as
    // well.
    पूर्णांक pass = 1;
    करा
    {
	पूर्णांक has_unmerged_gaps = FALSE;
	पूर्णांक has_merged_gaps = FALSE;
	diff_T *dp = dp_orig;
	पर्यंत (dp!= रिक्त && dp->df_next != रिक्त)
	{
	    // Only use first buffer to calculate the gap because the gap is
	    // unchanged text, which would be the same in all buffers.
	    जर (dp->df_lnum[idx1] + dp->df_मोजणी[idx1] - 1 >= linemap[idx1].ga_len
		    || dp->df_next->df_lnum[idx1] - 1 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    // If the gap occurs over different lines, don't consider it
	    linemap_entry_T *entry1 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_lnum[idx1] + dp->df_मोजणी[idx1] - 1];
	    linemap_entry_T *entry2 = &((linemap_entry_T*)linemap[idx1].ga_data)[dp->df_next->df_lnum[idx1] - 1];
	    जर (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    linenr_T gap = dp->df_next->df_lnum[idx1] - (dp->df_lnum[idx1] + dp->df_मोजणी[idx1]);
	    जर (gap <= 3)
	    {
		linenr_T max_df_मोजणी = 0;
		साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
		    max_df_मोजणी = MAX(max_df_मोजणी, dp->df_मोजणी[i] + dp->df_next->df_मोजणी[i]);

		जर (max_df_मोजणी >= gap * 4)
		{
		    // Merge current block with the next one. Don't advance the
		    // pointer so we try the same merged block against the next
		    // one.
		    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
		    {
			dp->df_मोजणी[i] = dp->df_next->df_lnum[i]
			    + dp->df_next->df_मोजणी[i] - dp->df_lnum[i];
		    }
		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);
		    has_merged_gaps = TRUE;
		    सुरूठेवा;
		}
		अन्यथा
		    has_unmerged_gaps = TRUE;
	    }
	    dp = dp->df_next;
	}
	जर (!has_unmerged_gaps || !has_merged_gaps)
	    थांबा;
    } पर्यंत (pass++ < 4); // use limited number of passes to avoid excessive looping
}

/*
 * Refine inline word diff blocks by merging blocks that are only separated
 * by whitespace or punctuation. This creates more coherent highlighting.
 */
    स्थिर शून्य
diff_refine_inline_word_highlight(diff_T *dp_orig, garray_T *linemap, पूर्णांक idx1,
				  linenr_T start_lnum)
{
    पूर्णांक pass = 1;
    करा
    {
	diff_T *dp = dp_orig;

	पर्यंत (dp != रिक्त && dp->df_next != रिक्त)
	{
	    // Index of the last line of this block and of the first line of the
	    // next one.  When this block is empty, which happens for inserted
	    // lines, it has no last line and "idx_entry1" is negative.
	    linenr_T	idx_entry1 = dp->df_lnum[idx1] + dp->df_मोजणी[idx1] - 2;
	    linenr_T	idx_entry2 = dp->df_next->df_lnum[idx1] - 1;

	    // Only merge blocks on the same line
	    जर (idx_entry1 < 0 || idx_entry1 >= linemap[idx1].ga_len
		    || idx_entry2 < 0 || idx_entry2 >= linemap[idx1].ga_len)
	    {
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    linemap_entry_T *entry1 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry1];
	    linemap_entry_T *entry2 =
		&((linemap_entry_T *)linemap[idx1].ga_data)[idx_entry2];

	    // Skip if blocks are on different lines
	    जर (entry1->lineoff != entry2->lineoff)
	    {
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    // Calculate the gap between blocks
	    पूर्णांक gap_start = entry1->byte_start + entry1->अंक_bytes;
	    पूर्णांक gap_end = entry2->byte_start;
	    पूर्णांक gap_size = gap_end - gap_start;

	    // Merge adjacent diff blocks separated by small gaps to reduce visual
	    // fragmentation. Gap threshold is set to 5 bytes which handles most
	    // common separators (spaces, punctuation, short variable names) while
	    // still preserving visually distinct changes.
	    जर (gap_size <= 0 || gap_size > diff_word_gap)
	    {
		dp = dp->df_next;
		सुरूठेवा;
	    }

	    // Get the text between the two blocks
	    char_u *ओळ = ml_get_buf(curtab->tp_diffbuf[idx1],
		    start_lnum + entry1->lineoff, FALSE);
	    char_u *gap_text = ओळ + gap_start;

	    // Check if gap contains only whitespace and/or punctuation
	    bool only_non_word = true;
	    bool has_content = false;

	    साठी (पूर्णांक i = 0; i < gap_size && gap_text[i] != NUL; i++)
	    {
		has_content = true;
		पूर्णांक char_class = mb_get_class_buf(gap_text + i,
				curtab->tp_diffbuf[idx1]);
		// class 2 is word characters, if we find any, don't merge
		जर (char_class == 2)
		{
		    only_non_word = false;
		    थांबा;
		}
	    }

	    // Merge if the gap is small and contains only non-word characters
	    जर (has_content && only_non_word)
	    {
		दीर्घ total_change_bytes = 0;
		साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
		{
		    जर (curtab->tp_diffbuf[i] != रिक्त)
		    {
			// count bytes in the first block
			साठी (पूर्णांक k = 0; k < dp->df_मोजणी[i]; k++)
			{
			    पूर्णांक idx = dp->df_lnum[i] + k - 1;
			    जर (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].अंक_bytes;
			}
			// count bytes in the next block
			साठी (पूर्णांक k = 0; k < dp->df_next->df_मोजणी[i]; k++)
			{
			    पूर्णांक idx = dp->df_next->df_lnum[i] + k - 1;
			    जर (idx < linemap[i].ga_len)
				total_change_bytes +=
				    ((linemap_entry_T *)linemap[i].ga_data)[idx].अंक_bytes;
			}
		    }
		}

		जर (total_change_bytes >= gap_size * 2)
		{
		    // Merge the blocks by extending the first block to include the next
		    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
		    {
			जर (curtab->tp_diffbuf[i] != रिक्त)
			{
			    dp->df_मोजणी[i] = dp->df_next->df_lnum[i]
				+ dp->df_next->df_मोजणी[i] - dp->df_lnum[i];
			}
		    }

		    diff_T *dp_next = dp->df_next;
		    dp->df_next = dp_next->df_next;
		    clear_diffblock(dp_next);

		    // Don't advance dp, check if can merge with the next block too
		    सुरूठेवा;
		}
	    }

	    dp = dp->df_next;
	}
    } पर्यंत (pass++ < 4);  // use limited number of passes to avoid excessive looping
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
    garray_T	linemap[DB_मोजणी];
    garray_T	file1_str;
    garray_T	file2_str;
    पूर्णांक		file1_idx = -1;

    दीर्घ	save_diff_algorithm = diff_algorithm;

    CLEAR_FIELD(dio);
    ga_init2(&dio.dio_diff.dout_ga, आकार(अक्षर *), 1000);

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
    buf_T	*(orig_diffbuf[DB_मोजणी]);
    स_नकल(orig_diffbuf, curtab->tp_diffbuf, आकार(orig_diffbuf));

    // Buffers to populate mmfile 1/2 that would be passed to xdiff as memory
    // files. Use a grow array as it is not obvious how much exact space we
    // need.
    ga_init2(&file1_str, 1, 1024);
    ga_init2(&file2_str, 1, 1024);

    // Line map to map from generated mmfiles' line numbers back to original
    // diff blocks' locations. Need this even for char diff because not all
    // characters are 1-byte long / ASCII.
    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
	ga_init2(&linemap[i], आकार(linemap_entry_T), 128);

    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
    {
	dio.dio_diff.dout_ga.ga_len = 0;

	buf_T *buf = curtab->tp_diffbuf[i];
	जर (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	    सुरूठेवा; // skip buffer that isn't loaded

	जर (dp->df_मोजणी[i] == 0)
	{
	    // skip buffers that don't have any texts in this block so we don't
	    // end up marking the entire block as modified in multi-buffer diff
	    curtab->tp_diffbuf[i] = रिक्त;
	    सुरूठेवा;
	}

	जर (file1_idx == -1)
	    file1_idx = i;

	garray_T	*curstr = (file1_idx != i) ? &file2_str : &file1_str;

	linenr_T numlines = 0;
	curstr->ga_len = 0;

	// Split each line into chars/words and populate fake file buffer as
	// newline-delimited tokens as that's what xdiff requires.
	साठी (पूर्णांक off = 0; off < dp->df_मोजणी[i]; off++)
	{
	    char_u *curline = ml_get_buf(curtab->tp_diffbuf[i],
		    dp->df_lnum[i] + off, FALSE);

	    पूर्णांक in_keyword = FALSE;

	    // iwhiteeol support vars
	    पूर्णांक last_white = FALSE;
	    पूर्णांक eol_ga_len = -1;
	    पूर्णांक eol_linemap_len = -1;
	    पूर्णांक eol_numlines = -1;

	    char_u *s;
	    साठी (s = curline; *s != NUL;)
	    {
		पूर्णांक new_in_keyword = FALSE;
		जर (diff_flags & DIFF_INLINE_WORD)
		{
		    // Always use the first buffer's 'iskeyword' to have a
		    // consistent diff.
		    // For multibyte chars, only treat alphanumeric chars
		    // (class 2) as "word", as other classes such as emojis and
		    // CJK ideographs do not usually benefit from word diff as
		    // Vim doesn't have a good way to segment them.
		    new_in_keyword = (mb_get_class_buf(s, curtab->tp_diffbuf[file1_idx]) == 2);
		}
		जर (in_keyword && !new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		जर (VIM_ISWHITE(*s))
		{
		    जर (diff_flags & DIFF_IWHITEALL)
		    {
			in_keyword = FALSE;
			s = skipwhite(s);
			सुरूठेवा;
		    }
		    अन्यथा जर ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			जर (!last_white)
			{
			    eol_ga_len = curstr->ga_len;
			    eol_linemap_len = linemap[i].ga_len;
			    eol_numlines = numlines;
			    last_white = TRUE;
			}
		    }
		}
		अन्यथा
		{
		    जर ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
		    {
			last_white = FALSE;
			eol_ga_len = -1;
			eol_linemap_len = -1;
			eol_numlines = -1;
		    }
		}

		पूर्णांक char_len = 1;
		जर (*s == NL)
		    // NL is internal substitute for NUL
		    ga_append(curstr, NUL);
		अन्यथा
		{
		    char_len = mb_ptr2len(s);

		    जर (VIM_ISWHITE(*s) && (diff_flags & DIFF_IWHITE))
			// Treat the entire white space span as a single char.
			char_len = skipwhite(s) - s;

		    जर (diff_flags & DIFF_ICASE)
		    {
			पूर्णांक c;
			char_u cbuf[MB_MAXBYTES + 1];
			// xdiff doesn't support ignoring case, fold-case the text manually.
			c = PTR2CHAR(s);
			पूर्णांक c_len = MB_CHAR2LEN(c);
			c = MB_CASEFOLD(c);
			पूर्णांक c_fold_len = mb_char2bytes(c, cbuf);
			ga_concat_len(curstr, cbuf, c_fold_len);
			जर (char_len > c_len)
			{
			    // There may be remaining composing characters. Write those back in.
			    // Composing characters don't need case folding.
			    ga_concat_len(curstr, s + c_len, char_len - c_len);
			}
		    }
		    अन्यथा
			ga_concat_len(curstr, s, char_len);
		}

		जर (!new_in_keyword)
		{
		    ga_append(curstr, NL);
		    numlines++;
		}

		जर (!new_in_keyword || (new_in_keyword && !in_keyword))
		{
		    // create a new mapping entry from the xdiff mmfile back to
		    // original line/col.
		    linemap_entry_T linemap_entry;
		    linemap_entry.lineoff = off;
		    linemap_entry.byte_start = s - curline;
		    linemap_entry.अंक_bytes = char_len;
		    जर (ga_grow(&linemap[i], 1) != OK)
			जा done;
		    ((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
			= linemap_entry;
		    linemap[i].ga_len += 1;
		}
		अन्यथा
		{
		    // Still inside a keyword. Just increment byte count but
		    // don't make a new entry.
		    // linemap always has at least one entry here
		    ((linemap_entry_T*)linemap[i].ga_data)[linemap[i].ga_len-1].अंक_bytes
			+= char_len;
		}

		in_keyword = new_in_keyword;
		s += char_len;
	    }
	    जर (in_keyword)
	    {
		ga_append(curstr, NL);
		numlines++;
	    }

	    जर ((diff_flags & DIFF_IWHITEEOL) || (diff_flags & DIFF_IWHITE))
	    {
		// Need to trim trailing whitespace. Do this simply by
		// resetting arrays back to before we encountered them.
		जर (eol_ga_len != -1)
		{
		    curstr->ga_len = eol_ga_len;
		    linemap[i].ga_len = eol_linemap_len;
		    numlines = eol_numlines;
		}
	    }

	    जर (!(diff_flags & DIFF_IWHITEALL))
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
		linemap_entry.अंक_bytes = आकार(NL);
		जर (ga_grow(&linemap[i], 1) != OK)
		    जा done;
		((linemap_entry_T*)(linemap[i].ga_data))[linemap[i].ga_len]
		    = linemap_entry;
		linemap[i].ga_len += 1;
	    }
	}

	जर (file1_idx != i)
	{
	    dio.dio_new.din_mmfile.ptr = (अक्षर *)curstr->ga_data;
	    dio.dio_new.din_mmfile.size = curstr->ga_len;
	}
	अन्यथा
	{
	    dio.dio_orig.din_mmfile.ptr = (अक्षर *)curstr->ga_data;
	    dio.dio_orig.din_mmfile.size = curstr->ga_len;
	}
	जर (file1_idx != i)
	{
	    // Perform diff with first file and read the results
	    पूर्णांक diff_status = diff_file_internal(&dio);
	    जर (diff_status == FAIL)
		जा done;

	    diff_read(0, i, &dio);
	    clear_diffout(&dio.dio_diff);
	}
    }
    diff_T *new_diff = curtab->tp_first_diff;

    जर (diff_flags & DIFF_INLINE_WORD && file1_idx != -1)
	diff_refine_inline_word_highlight(new_diff, linemap, file1_idx, dp->df_lnum[file1_idx]);
    अन्यथा जर (diff_flags & DIFF_INLINE_CHAR && file1_idx != -1)
	diff_refine_inline_char_highlight(new_diff, linemap, file1_idx);

    // After the diff, use the linemap to obtain the original line/col of the
    // changes and cache them in dp.
    dp->df_changes.ga_len = 0; // this should already be zero
    साठी (; new_diff != रिक्त; new_diff = new_diff->df_next)
    {
	diffline_change_T change;
	CLEAR_FIELD(change);
	साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
	{
	    जर (new_diff->df_lnum[i] <= 0) // should never be < 0. Checking just for safety.
		सुरूठेवा;
	    linenr_T diff_lnum = new_diff->df_lnum[i] - 1; // use zero-index
	    linenr_T diff_lnum_end = diff_lnum + new_diff->df_मोजणी[i];

	    जर (diff_lnum >= linemap[i].ga_len)
	    {
		change.dc_start[i] = MAXCOL;
		change.dc_start_lnum_off[i] = पूर्णांक_उच्च;
	    }
	    अन्यथा
	    {
		change.dc_start[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].byte_start;
		change.dc_start_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum].lineoff;
	    }

	    जर (diff_lnum == diff_lnum_end)
	    {
		change.dc_end[i] = change.dc_start[i];
		change.dc_end_lnum_off[i] = change.dc_start_lnum_off[i];
	    }
	    अन्यथा जर (diff_lnum_end - 1 >= linemap[i].ga_len)
	    {
		change.dc_end[i] = MAXCOL;
		change.dc_end_lnum_off[i] = पूर्णांक_उच्च;
	    }
	    अन्यथा
	    {
		change.dc_end[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].byte_start +
		    ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].अंक_bytes;
		change.dc_end_lnum_off[i] = ((linemap_entry_T*)linemap[i].ga_data)[diff_lnum_end-1].lineoff;
	    }
	}
	जर (ga_grow(&dp->df_changes, 1) != OK)
	{
	    dp->df_changes.ga_len = 0;
	    जा done;
	}
	((diffline_change_T*)(dp->df_changes.ga_data))[dp->df_changes.ga_len] = change;
	dp->df_changes.ga_len += 1;
    }

done:
    diff_algorithm = save_diff_algorithm;

    dp->has_changes = TRUE;

    diff_clear(curtab);
    curtab->tp_first_diff = orig_diff;
    स_नकल(curtab->tp_diffbuf, orig_diffbuf, आकार(orig_diffbuf));

    ga_clear(&file1_str);
    ga_clear(&file2_str);
    // No need to clear dio.dio_orig/dio_new because they were referencing
    // strings that are now cleared.
    clear_diffout(&dio.dio_diff);
    साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
	ga_clear(&linemap[i]);
}

/*
 * Find the difference within a changed line.
 * Returns TRUE if the line was added, no other buffer has it.
 */
    पूर्णांक
diff_find_change(
    win_T	*wp,
    linenr_T	lnum,
    diffline_T	*diffline)
{
    diff_T	*dp;
    पूर्णांक		idx;
    पूर्णांक		off;

    idx = diff_buf_idx(wp->w_buffer);
    जर (idx == DB_मोजणी)	// cannot happen
	परत FALSE;

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जर (lnum < dp->df_lnum[idx] + dp->df_मोजणी[idx])
	    थांबा;
    जर (dp == रिक्त || diff_check_sanity(curtab, dp) == FAIL)
	परत FALSE;

    जर (lnum - dp->df_lnum[idx] > पूर्णांक_उच्च)
	// Integer overflow protection
	परत FALSE;
    off = lnum - dp->df_lnum[idx];

    जर (!(diff_flags & ALL_INLINE_DIFF) || diff_internal_failed())
    {
	// Use simple algorithm
	पूर्णांक	change_start = MAXCOL;	// first col of changed area
	पूर्णांक	change_end = -1;	// last col of changed area
	पूर्णांक	ret;

	ret = diff_find_change_simple(wp, lnum, dp, idx, &change_start, &change_end);

	// convert from inclusive end to exclusive end per diffline's contract
	change_end += 1;

	// Create a mock diffline struct. We always only have one so no need to
	// allocate memory.
	CLEAR_FIELD(simple_diffline_change);
	diffline->changes = &simple_diffline_change;
	diffline->अंक_changes = 1;
	diffline->bufidx = idx;
	diffline->lineoff = lnum - dp->df_lnum[idx];

	simple_diffline_change.dc_start[idx] = change_start;
	simple_diffline_change.dc_end[idx] = change_end;
	simple_diffline_change.dc_start_lnum_off[idx] = off;
	simple_diffline_change.dc_end_lnum_off[idx] = off;
	परत ret;
    }

    // Use inline diff algorithm.
    // The diff changes are usually cached so we check that first.
    जर (!dp->has_changes)
	diff_find_change_inline_diff(dp);

    garray_T *changes = &dp->df_changes;

    // Use linear search to find the first change for this line. We could
    // optimize this to use binary search, but there should usually be a
    // limited number of inline changes per diff block, and limited number of
    // diff blocks shown on screen, so it is not necessary.
    पूर्णांक अंक_changes = 0;
    पूर्णांक change_idx = 0;
    diffline->changes = रिक्त;
    साठी (change_idx = 0; change_idx < changes->ga_len; change_idx++)
    {
	diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[change_idx];
	जर (change->dc_end_lnum_off[idx] < off)
	    सुरूठेवा;
	जर (change->dc_start_lnum_off[idx] > off)
	    थांबा;
	जर (diffline->changes == रिक्त)
	    diffline->changes = change;
	अंक_changes++;
    }
    diffline->अंक_changes = अंक_changes;
    diffline->bufidx = idx;
    diffline->lineoff = off;

    // Detect simple cases of added lines in the end within a diff block. This
    // has to be the last change of this diff block, and all other buffers are
    // considering this to be an addition past their last line. Other scenarios
    // will be considered a changed line instead.
    पूर्णांक added = FALSE;
    जर (अंक_changes == 1 && change_idx == dp->df_changes.ga_len)
    {
	added = TRUE;
	साठी (पूर्णांक i = 0; i < DB_मोजणी; i++)
	{
	    जर (idx == i)
		सुरूठेवा;
	    जर (curtab->tp_diffbuf[i] == रिक्त)
		सुरूठेवा;
	    diffline_change_T *change = &((diffline_change_T*)dp->df_changes.ga_data)[dp->df_changes.ga_len-1];
	    जर (change->dc_start_lnum_off[i] != पूर्णांक_उच्च)
	    {
		added = FALSE;
		थांबा;
	    }
	}
    }
    परत added;
}

# जर defined(FEAT_FOLDING)
/*
 * Return TRUE if line "lnum" is not close to a diff block, this line should
 * be in a fold.
 * Return FALSE if there are no diff blocks at all in this window.
 */
    पूर्णांक
diff_infold(win_T *wp, linenr_T lnum)
{
    पूर्णांक		i;
    पूर्णांक		idx = -1;
    पूर्णांक		other = FALSE;
    diff_T	*dp;

    // Return if 'diff' isn't set.
    जर (!wp->w_p_diff)
	परत FALSE;

    साठी (i = 0; i < DB_मोजणी; ++i)
    {
	जर (curtab->tp_diffbuf[i] == wp->w_buffer)
	    idx = i;
	अन्यथा जर (curtab->tp_diffbuf[i] != रिक्त)
	    other = TRUE;
    }

    // return here if there are no diffs in the window
    जर (idx == -1 || !other)
	परत FALSE;

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    // Return if there are no diff blocks.  All lines will be folded.
    जर (curtab->tp_first_diff == रिक्त)
	परत TRUE;

    // Resume from the scan finger to avoid restarting at the first block.
    diff_T	*le = रिक्त;		// last block with df_lnum[idx] <= lnum
    पूर्णांक		result = TRUE;
    साठी (dp = diff_scan_start(curtab, idx, lnum); dp != रिक्त; dp = dp->df_next)
    {
	जर (dp->df_lnum[idx] <= lnum)
	    le = dp;
	// If this change is below the line there can't be any further match.
	जर (dp->df_lnum[idx] - diff_context > lnum)
	    थांबा;
	// If this change ends before the line we have a match.
	जर (dp->df_lnum[idx] + dp->df_मोजणी[idx] + diff_context > lnum)
	{
	    result = FALSE;
	    थांबा;
	}
    }
    diff_scan_remember(curtab, idx, lnum, le);
    परत result;
}
# endif

/*
 * "dp" and "do" commands.
 */
    शून्य
nv_diffgetput(पूर्णांक put, दीर्घ मोजणी)
{
    exarg_T	ea;
    char_u	buf[30];

# ifdef FEAT_JOB_CHANNEL
    जर (bt_prompt(curbuf))
    {
	vim_beep(BO_OPER);
	परत;
    }
# endif
    जर (मोजणी == 0)
	ea.arg = (char_u *)"";
    अन्यथा
    {
	vim_snprintf((अक्षर *)buf, 30, "%ld", मोजणी);
	ea.arg = buf;
    }
    जर (put)
	ea.cmdidx = CMD_diffput;
    अन्यथा
	ea.cmdidx = CMD_diffget;
    ea.addr_मोजणी = 0;
    ea.line1 = curwin->w_cursor.lnum;
    ea.line2 = curwin->w_cursor.lnum;
    ex_diffgetput(&ea);
}

/*
 * Return TRUE if "diff" appears in the list of diff blocks of the current tab.
 */
    स्थिर पूर्णांक
valid_diff(diff_T *diff)
{
    diff_T	*dp;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जर (dp == diff)
	    परत TRUE;
    परत FALSE;
}

/*
 * ":diffget"
 * ":diffput"
 */
    शून्य
ex_diffgetput(exarg_T *eap)
{
    linenr_T	lnum;
    पूर्णांक		मोजणी;
    linenr_T	off = 0;
    diff_T	*dp;
    diff_T	*dprev;
    diff_T	*dfree;
    पूर्णांक		idx_cur;
    पूर्णांक		idx_other;
    पूर्णांक		idx_from;
    पूर्णांक		idx_to;
    पूर्णांक		i;
    पूर्णांक		added;
    char_u	*p;
    aco_save_T	aco;
    buf_T	*buf;
    पूर्णांक		start_skip, end_skip;
    पूर्णांक		new_मोजणी;
    पूर्णांक		buf_empty;
    पूर्णांक		found_not_ma = FALSE;

    // Find the current buffer in the list of diff buffers.
    idx_cur = diff_buf_idx(curbuf);
    जर (idx_cur == DB_मोजणी)
    {
	emsg(_(e_current_buffer_is_not_in_diff_mode));
	परत;
    }

    जर (*eap->arg == NUL)
    {
	// No argument: Find the other buffer in the list of diff buffers.
	साठी (idx_other = 0; idx_other < DB_मोजणी; ++idx_other)
	    जर (curtab->tp_diffbuf[idx_other] != curbuf
		    && curtab->tp_diffbuf[idx_other] != रिक्त)
	    {
		जर (eap->cmdidx != CMD_diffput
				     || curtab->tp_diffbuf[idx_other]->b_p_ma)
		    थांबा;
		found_not_ma = TRUE;
	    }
	जर (idx_other == DB_मोजणी)
	{
	    जर (found_not_ma)
		emsg(_(e_no_other_buffer_in_diff_mode_is_modifiable));
	    अन्यथा
		emsg(_(e_no_other_buffer_in_diff_mode));
	    परत;
	}

	// Check that there isn't a third buffer in the list
	साठी (i = idx_other + 1; i < DB_मोजणी; ++i)
	    जर (curtab->tp_diffbuf[i] != curbuf
		    && curtab->tp_diffbuf[i] != रिक्त
		    && (eap->cmdidx != CMD_diffput || curtab->tp_diffbuf[i]->b_p_ma))
	    {
		emsg(_(e_more_than_two_buffers_in_diff_mode_dont_know_which_one_to_use));
		परत;
	    }
    }
    अन्यथा
    {
	// Buffer number or pattern given.  Ignore trailing white space.
	p = eap->arg + STRLEN(eap->arg);
	पर्यंत (p > eap->arg && VIM_ISWHITE(p[-1]))
	    --p;
	साठी (i = 0; vim_isdigit(eap->arg[i]) && eap->arg + i < p; ++i)
	    ;
	जर (eap->arg + i == p)	    // digits only
	    i = म_से_द((अक्षर *)eap->arg);
	अन्यथा
	{
	    i = buflist_findpat(eap->arg, p, FALSE, TRUE, FALSE);
	    जर (i < 0)
		परत;		// error message already given
	}
	buf = buflist_findnr(i);
	जर (buf == रिक्त)
	{
	    semsg(_(e_cant_find_buffer_str), eap->arg);
	    परत;
	}
	जर (buf == curbuf)
	    परत;		// nothing to do
	idx_other = diff_buf_idx(buf);
	जर (idx_other == DB_मोजणी)
	{
	    semsg(_(e_buffer_str_is_not_in_diff_mode), eap->arg);
	    परत;
	}
    }

    diff_busy = TRUE;

    // When no range given include the line above or below the cursor.
    जर (eap->addr_मोजणी == 0)
    {
	// Make it possible that ":diffget" on the last line gets line below
	// the cursor line when there is no difference above the cursor.
	पूर्णांक linestatus = 0;
	जर (eap->line1 == curbuf->b_ml.ml_ओळ_मोजणी
		&& (diff_check_with_linestatus(curwin, eap->line1, &linestatus) == 0
		    && linestatus == 0)
		&& (eap->line1 == 1 ||
		    (diff_check_with_linestatus(curwin, eap->line1 - 1, &linestatus) >= 0
		     && linestatus == 0)))
	    ++eap->line2;
	अन्यथा जर (eap->line1 > 0)
	    --eap->line1;
    }

    जर (eap->cmdidx == CMD_diffget)
    {
	idx_from = idx_other;
	idx_to = idx_cur;
    }
    अन्यथा
    {
	idx_from = idx_cur;
	idx_to = idx_other;
	// Need to make the other buffer the current buffer to be able to make
	// changes in it.
	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, curtab->tp_diffbuf[idx_other]);
	जर (curbuf != curtab->tp_diffbuf[idx_other])
	    // Could not find a window for this buffer, the rest is likely to
	    // fail.
	    जा theend;
    }

    // May give the warning for a changed buffer here, which can trigger the
    // FileChangedRO autocommand, which may do nasty things and mess
    // everything up.
    जर (!curbuf->b_changed)
    {
	change_warning(0);
	जर (diff_buf_idx(curbuf) != idx_to)
	{
	    emsg(_(e_buffer_changed_unexpectedly));
	    जा theend;
	}
    }

    dprev = रिक्त;
    साठी (dp = curtab->tp_first_diff; dp != रिक्त; )
    {
	जर (!eap->addr_मोजणी)
	{
	    // Handle the case with adjacent diff blocks (e.g. using linematch
	    // or anchors) at/above the cursor. Since a range wasn't specified,
	    // we just want to grab one diff block rather than all of them in
	    // the vicinity.
	    पर्यंत (dp->df_next
		    && dp->df_next->df_lnum[idx_cur] == dp->df_lnum[idx_cur] +
							dp->df_मोजणी[idx_cur]
		    && dp->df_next->df_lnum[idx_cur] == eap->line1 + off + 1)
	    {
		dprev = dp;
		dp = dp->df_next;
	    }
	}
	जर (dp->df_lnum[idx_cur] > eap->line2 + off)
	    थांबा;	// past the range that was specified

	dfree = रिक्त;
	lnum = dp->df_lnum[idx_to];
	मोजणी = dp->df_मोजणी[idx_to];
	// The empty line of an empty buffer is deleted below, include it in
	// the undo information, otherwise undo leaves a line behind.
	linenr_T undo_bot = lnum + मोजणी + (मोजणी == 0 && BUFEMPTY() ? 1 : 0);

	जर (dp->df_lnum[idx_cur] + dp->df_मोजणी[idx_cur] > eap->line1 + off
		&& u_save(lnum - 1, undo_bot) != FAIL)
	{
	    // Inside the specified range and saving for undo worked.
	    start_skip = 0;
	    end_skip = 0;
	    जर (eap->addr_मोजणी > 0)
	    {
		// A range was specified: check if lines need to be skipped.
		start_skip = eap->line1 + off - dp->df_lnum[idx_cur];
		जर (start_skip > 0)
		{
		    // range starts below start of current diff block
		    जर (start_skip > मोजणी)
		    {
			lnum += मोजणी;
			मोजणी = 0;
		    }
		    अन्यथा
		    {
			मोजणी -= start_skip;
			lnum += start_skip;
		    }
		}
		अन्यथा
		    start_skip = 0;

		end_skip = dp->df_lnum[idx_cur] + dp->df_मोजणी[idx_cur] - 1
							 - (eap->line2 + off);
		जर (end_skip > 0)
		{
		    // range ends above end of current/from diff block
		    जर (idx_cur == idx_from)	// :diffput
		    {
			i = dp->df_मोजणी[idx_cur] - start_skip - end_skip;
			जर (मोजणी > i)
			    मोजणी = i;
		    }
		    अन्यथा			// :diffget
		    {
			मोजणी -= end_skip;
			end_skip = dp->df_मोजणी[idx_from] - start_skip - मोजणी;
			जर (end_skip < 0)
			    end_skip = 0;
		    }
		}
		अन्यथा
		    end_skip = 0;
	    }

	    buf_empty = BUFEMPTY();
	    added = 0;
	    साठी (i = 0; i < मोजणी; ++i)
	    {
		// remember deleting the last line of the buffer
		buf_empty = curbuf->b_ml.ml_ओळ_मोजणी == 1;
		जर (ml_delete(lnum) == OK)
		    --added;
	    }
	    साठी (i = 0; i < dp->df_मोजणी[idx_from] - start_skip - end_skip; ++i)
	    {
		linenr_T nr;

		nr = dp->df_lnum[idx_from] + start_skip + i;
		जर (nr > curtab->tp_diffbuf[idx_from]->b_ml.ml_ओळ_मोजणी)
		    थांबा;
		p = vim_strsave(ml_get_buf(curtab->tp_diffbuf[idx_from],
								  nr, FALSE));
		जर (p != रिक्त)
		{
		    ml_append(lnum + i - 1, p, 0, FALSE);
		    vim_free(p);
		    ++added;
		    जर (buf_empty && curbuf->b_ml.ml_ओळ_मोजणी == 2)
		    {
			// Added the first line into an empty buffer, need to
			// delete the dummy empty line.
			buf_empty = FALSE;
			ml_delete((linenr_T)2);
		    }
		}
	    }
	    new_मोजणी = dp->df_मोजणी[idx_to] + added;
	    dp->df_मोजणी[idx_to] = new_मोजणी;

	    जर (start_skip == 0 && end_skip == 0)
	    {
		// Check if there are any other buffers and if the diff is
		// equal in them.
		साठी (i = 0; i < DB_मोजणी; ++i)
		    जर (curtab->tp_diffbuf[i] != रिक्त && i != idx_from
								&& i != idx_to
			    && !diff_equal_entry(dp, idx_from, i))
			थांबा;
		जर (i == DB_मोजणी)
		{
		    // delete the diff entry, the buffers are now equal here
		    dfree = dp;
		    dp = dp->df_next;
		    जर (dprev == रिक्त)
			curtab->tp_first_diff = dp;
		    अन्यथा
			dprev->df_next = dp;
		}
	    }

	    जर (added != 0)
	    {
		// Adjust marks.  This will change the following entries!
		mark_adjust(lnum, lnum + मोजणी - 1, (दीर्घ)MAXLNUM, (दीर्घ)added);
		जर (curwin->w_cursor.lnum >= lnum)
		{
		    // Adjust the cursor position if it's in/after the changed
		    // lines.
		    जर (curwin->w_cursor.lnum >= lnum + मोजणी)
		    {
			curwin->w_cursor.lnum += added;
			// When the buffer was previously empty, the cursor may
			// now be beyond the last line, so clamp cursor lnum.
			curwin->w_cursor.lnum = MIN(curwin->w_cursor.lnum,
						curbuf->b_ml.ml_ओळ_मोजणी);
		    }
		    अन्यथा जर (added < 0)
			curwin->w_cursor.lnum = lnum;
		}
	    }
	    changed_ओळी(lnum, 0, lnum + मोजणी, (दीर्घ)added);

	    जर (dfree != रिक्त)
	    {
		// Diff is deleted, update folds in other windows.
# ifdef FEAT_FOLDING
		diff_fold_update(dfree, idx_to);
# endif
		clear_diffblock(dfree);
	    }

	    // mark_adjust() may have made "dp" invalid.  We don't know where
	    // to continue then, bail out.
	    जर (added != 0 && !valid_diff(dp))
		थांबा;

	    जर (dfree == रिक्त)
		// mark_adjust() may have changed the count in a wrong way
		dp->df_मोजणी[idx_to] = new_मोजणी;

	    // When changing the current buffer, keep track of line numbers
	    जर (idx_cur == idx_to)
		off += added;
	}

	// If before the range or not deleted, go to next diff.
	जर (dfree == रिक्त)
	{
	    dprev = dp;
	    dp = dp->df_next;
	}
    }

    // restore curwin/curbuf and a few other things
    जर (eap->cmdidx != CMD_diffget)
    {
	// Syncing undo only works for the current buffer, but we change
	// another buffer.  Sync undo if the command was typed.  This isn't
	// 100% right when ":diffput" is used in a function or mapping.
	जर (किल्ली_Typed)
	    u_sync(FALSE);
	aucmd_restbuf(&aco);
    }

theend:
    diff_busy = FALSE;
    जर (diff_need_update)
	ex_diffupdate(रिक्त);

    // Check that the cursor is on a valid character and update its
    // position.  When there were filler lines the topline has become
    // invalid.
    check_cursor();
    changed_ओळ_abv_curs();

# ifdef FEAT_FOLDING
    // If all diffs are gone, update folds in all diff windows.
    जर (curtab->tp_first_diff == रिक्त)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS_IN_TAB(curtab, wp)
	    जर (wp->w_p_diff && wp->w_p_fdm[0] == 'd' && wp->w_p_fen)
		foldUpdateAll(wp);
    }
# endif

    जर (diff_need_update)
	// redraw already done by ex_diffupdate()
	diff_need_update = FALSE;
    अन्यथा
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
diff_fold_update(diff_T *dp, पूर्णांक skip_idx)
{
    पूर्णांक		i;
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	साठी (i = 0; i < DB_मोजणी; ++i)
	    जर (curtab->tp_diffbuf[i] == wp->w_buffer && i != skip_idx)
		foldUpdate(wp, dp->df_lnum[i],
					    dp->df_lnum[i] + dp->df_मोजणी[i]);
}
# endif

/*
 * Return TRUE if buffer "buf" is in diff-mode.
 */
    पूर्णांक
diff_mode_buf(buf_T *buf)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	जर (diff_buf_idx_tp(buf, tp) != DB_मोजणी)
	    परत TRUE;
    परत FALSE;
}

/*
 * Move "count" times in direction "dir" to the next diff block.
 * Return FAIL if there isn't such a diff block.
 */
    पूर्णांक
diff_move_to(पूर्णांक dir, दीर्घ मोजणी)
{
    पूर्णांक		idx;
    linenr_T	lnum = curwin->w_cursor.lnum;
    diff_T	*dp;

    idx = diff_buf_idx(curbuf);
    जर (idx == DB_मोजणी || curtab->tp_first_diff == रिक्त)
	परत FAIL;

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जर (curtab->tp_first_diff == रिक्त)		// no diffs today
	परत FAIL;

    पर्यंत (--मोजणी >= 0)
    {
	// Check if already before first diff.
	जर (dir == BACKWARD && lnum <= curtab->tp_first_diff->df_lnum[idx])
	    थांबा;

	साठी (dp = curtab->tp_first_diff; ; dp = dp->df_next)
	{
	    जर (dp == रिक्त)
		थांबा;
	    जर ((dir == FORWARD && lnum < dp->df_lnum[idx])
		    || (dir == BACKWARD
			&& (dp->df_next == रिक्त
			    || lnum <= dp->df_next->df_lnum[idx])))
	    {
		lnum = dp->df_lnum[idx];
		थांबा;
	    }
	}
    }

    // don't end up past the end of the file
    जर (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	lnum = curbuf->b_ml.ml_ओळ_मोजणी;

    // When the cursor didn't move at all we fail.
    जर (lnum == curwin->w_cursor.lnum)
	परत FAIL;

    setpcmark();
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;

    परत OK;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.
 */
    स्थिर linenr_T
diff_get_corresponding_ओळ_int(
    buf_T	*buf1,
    linenr_T	lnum1)
{
    पूर्णांक		idx1;
    पूर्णांक		idx2;
    diff_T	*dp;
    पूर्णांक		baseline = 0;

    idx1 = diff_buf_idx(buf1);
    idx2 = diff_buf_idx(curbuf);
    जर (idx1 == DB_मोजणी || idx2 == DB_मोजणी || curtab->tp_first_diff == रिक्त)
	परत lnum1;

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    जर (curtab->tp_first_diff == रिक्त)		// no diffs today
	परत lnum1;

    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
    {
	जर (dp->df_lnum[idx1] > lnum1)
	    परत lnum1 - baseline;
	जर ((dp->df_lnum[idx1] + dp->df_मोजणी[idx1]) > lnum1)
	{
	    // Inside the diffblock
	    baseline = lnum1 - dp->df_lnum[idx1];
	    जर (baseline > dp->df_मोजणी[idx2])
		baseline = dp->df_मोजणी[idx2];

	    परत dp->df_lnum[idx2] + baseline;
	}
	जर (    (dp->df_lnum[idx1] == lnum1)
	     && (dp->df_मोजणी[idx1] == 0)
	     && (dp->df_lnum[idx2] <= curwin->w_cursor.lnum)
	     && ((dp->df_lnum[idx2] + dp->df_मोजणी[idx2])
						      > curwin->w_cursor.lnum))
	    /*
	     * Special case: if the cursor is just after a zero-count
	     * block (i.e. all filler) and the target cursor is already
	     * inside the corresponding block, leave the target cursor
	     * unmoved. This makes repeated CTRL-W W operations work
	     * as expected.
	     */
	    परत curwin->w_cursor.lnum;
	baseline = (dp->df_lnum[idx1] + dp->df_मोजणी[idx1])
				   - (dp->df_lnum[idx2] + dp->df_मोजणी[idx2]);
    }

    // If we get here then the cursor is after the last diff
    परत lnum1 - baseline;
}

/*
 * Return the line number in the current window that is closest to "lnum1" in
 * "buf1" in diff mode.  Checks the line number to be valid.
 */
    linenr_T
diff_get_corresponding_ओळ(buf_T *buf1, linenr_T lnum1)
{
    linenr_T lnum = diff_get_corresponding_ओळ_int(buf1, lnum1);

    // don't end up past the end of the file
    जर (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	परत curbuf->b_ml.ml_ओळ_मोजणी;
    परत lnum;
}

/*
 * For line "lnum" in the current window find the equivalent lnum in window
 * "wp", compensating for inserted/deleted lines.
 */
    linenr_T
diff_lnum_win(linenr_T lnum, win_T *wp)
{
    diff_T	*dp;
    पूर्णांक		idx;
    पूर्णांक		i;
    linenr_T	n;

    idx = diff_buf_idx(curbuf);
    जर (idx == DB_मोजणी)		// safety check
	परत (linenr_T)0;

    जर (curtab->tp_diff_invalid)
	ex_diffupdate(रिक्त);		// update after a big change

    // search for a change that includes "lnum" in the list of diffblocks.
    FOR_ALL_DIFFBLOCKS_IN_TAB(curtab, dp)
	जर (lnum <= dp->df_lnum[idx] + dp->df_मोजणी[idx])
	    थांबा;

    // When after the last change, compute relative to the last line number.
    जर (dp == रिक्त)
	परत wp->w_buffer->b_ml.ml_ओळ_मोजणी
					- (curbuf->b_ml.ml_ओळ_मोजणी - lnum);

    // Find index for "wp".
    i = diff_buf_idx(wp->w_buffer);
    जर (i == DB_मोजणी)			// safety check
	परत (linenr_T)0;

    n = lnum + (dp->df_lnum[i] - dp->df_lnum[idx]);
    जर (n > dp->df_lnum[i] + dp->df_मोजणी[i])
	n = dp->df_lnum[i] + dp->df_मोजणी[i];
    परत n;
}

/*
 * Handle an ED style diff line.
 * Return FAIL if the line does not contain diff info.
 */
    स्थिर पूर्णांक
parse_diff_ed(
	char_u	    *ओळ,
	diffhunk_T  *hunk)
{
    char_u *p;
    दीर्घ    f1, l1, f2, l2;
    पूर्णांक	    difftype;

    // The line must be one of three formats:
    // change: {first}[,{last}]c{first}[,{last}]
    // append: {first}a{first}[,{last}]
    // delete: {first}[,{last}]d{first}
    p = ओळ;
    f1 = getdigits(&p);
    जर (*p == ',')
    {
	++p;
	l1 = getdigits(&p);
    }
    अन्यथा
	l1 = f1;
    जर (*p != 'a' && *p != 'c' && *p != 'd')
	परत FAIL;		// invalid diff format
    difftype = *p++;
    f2 = getdigits(&p);
    जर (*p == ',')
    {
	++p;
	l2 = getdigits(&p);
    }
    अन्यथा
	l2 = f2;
    जर (l1 < f1 || l2 < f2)
	परत FAIL;

    जर (difftype == 'a')
    {
	hunk->lnum_orig = f1 + 1;
	hunk->मोजणी_orig = 0;
    }
    अन्यथा
    {
	hunk->lnum_orig = f1;
	hunk->मोजणी_orig = l1 - f1 + 1;
    }
    जर (difftype == 'd')
    {
	hunk->lnum_new = f2 + 1;
	hunk->मोजणी_new = 0;
    }
    अन्यथा
    {
	hunk->lnum_new = f2;
	hunk->मोजणी_new = l2 - f2 + 1;
    }
    परत OK;
}

/*
 * Parses unified diff with zero(!) context lines.
 * Return FAIL if there is no diff information in "line".
 */
    स्थिर पूर्णांक
parse_diff_unified(
	char_u	    *ओळ,
	diffhunk_T  *hunk)
{
    char_u *p;
    दीर्घ    oldline, oldcount, newline, newcount;

    // Parse unified diff hunk header:
    // @@ -oldline,oldcount +newline,newcount @@
    p = ओळ;
    जर (*p++ == '@' && *p++ == '@' && *p++ == ' ' && *p++ == '-')
    {
	oldline = getdigits(&p);
	जर (*p == ',')
	{
	    ++p;
	    oldcount = getdigits(&p);
	}
	अन्यथा
	    oldcount = 1;
	जर (*p++ == ' ' && *p++ == '+')
	{
	    newline = getdigits(&p);
	    जर (*p == ',')
	    {
		++p;
		newcount = getdigits(&p);
	    }
	    अन्यथा
		newcount = 1;
	}
	अन्यथा
	    परत FAIL;	// invalid diff format

	जर (oldcount == 0)
	    oldline += 1;
	जर (newcount == 0)
	    newline += 1;
	जर (newline == 0)
	    newline = 1;

	hunk->lnum_orig = oldline;
	hunk->मोजणी_orig = oldcount;
	hunk->lnum_new = newline;
	hunk->मोजणी_new = newcount;

	परत OK;
    }

    परत FAIL;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the diff output (indices) in a grow array.
 */
    स्थिर पूर्णांक
xdiff_out_indices(
	दीर्घ start_a,
	दीर्घ मोजणी_a,
	दीर्घ start_b,
	दीर्घ मोजणी_b,
	शून्य *priv)
{
    diffout_T	*dout = (diffout_T *)priv;
    diffhunk_T *p = ALLOC_ONE(diffhunk_T);

    जर (p == रिक्त)
	परत -1;

    जर (ga_grow(&dout->dout_ga, 1) == FAIL)
    {
	vim_free(p);
	परत -1;
    }

    p->lnum_orig  = start_a + 1;
    p->मोजणी_orig = मोजणी_a;
    p->lnum_new   = start_b + 1;
    p->मोजणी_new  = मोजणी_b;
    ((diffhunk_T **)dout->dout_ga.ga_data)[dout->dout_ga.ga_len++] = p;
    परत 0;
}

/*
 * Callback function for the xdl_diff() function.
 * Stores the unified diff output in a grow array.
 */
    स्थिर पूर्णांक
xdiff_out_unified(
	शून्य *priv,
	mmbuffer_t *mb,
	पूर्णांक nbuf)
{
    diffout_T	*dout = (diffout_T *)priv;
    पूर्णांक		i;

    साठी (i = 0; i < nbuf; i++)
	ga_concat_len(&dout->dout_ga, (char_u *)mb[i].ptr, mb[i].size);

    परत 0;
}

#endif	// FEAT_DIFF

#जर defined(FEAT_EVAL)

/*
 * "diff_filler()" function
 */
    शून्य
f_diff_filler(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_DIFF
    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = diff_check_fill(curwin, tv_get_lnum(argvars));
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
    स्थिर पूर्णांक		fnum = 0;
    स्थिर पूर्णांक		prev_diff_flags = 0;
    स्थिर पूर्णांक		change_start = 0;
    स्थिर पूर्णांक		change_end = 0;
    स्थिर hlf_T	hlID = (hlf_T)0;
    पूर्णांक			cache_results = TRUE;
    पूर्णांक			col;
    diffline_T		diffline;

    CLEAR_FIELD(diffline);

    जर (in_vim9script()
	    && (check_for_lnum_arg(argvars,0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL))
	परत;

    जर (diff_flags & ALL_INLINE_DIFF)
    {
	// Remember the results if using simple since it's recalculated per
	// call. Otherwise just call diff_find_change() every time since
	// internally the result is cached internally.
	cache_results = FALSE;
    }

    lnum = tv_get_lnum(argvars);
    जर (lnum < 0)	// ignore type error in {lnum} arg
	lnum = 0;
    जर (!cache_results
	    || lnum != prev_lnum
	    || changedtick != CHANGEDTICK(curbuf)
	    || fnum != curbuf->b_fnum
	    || diff_flags != prev_diff_flags)
    {
	// New line, buffer, change: need to get the values.
	पूर्णांक linestatus = 0;
	diff_check_with_linestatus(curwin, lnum, &linestatus);
	जर (linestatus < 0)
	{
	    जर (linestatus == -1)
	    {
		change_start = MAXCOL;
		change_end = -1;
		जर (diff_find_change(curwin, lnum, &diffline))
		    hlID = HLF_ADD;	// added line
		अन्यथा
		{
		    hlID = HLF_CHD;	// changed line
		    जर (diffline.अंक_changes > 0 && cache_results)
		    {
			change_start = diffline.changes[0].dc_start[diffline.bufidx];
			change_end = diffline.changes[0].dc_end[diffline.bufidx];
		    }
		}
	    }
	    अन्यथा
		hlID = HLF_ADD;	// added line
	}
	अन्यथा
	    hlID = (hlf_T)0;

	जर (cache_results)
	{
	    prev_lnum = lnum;
	    changedtick = CHANGEDTICK(curbuf);
	    fnum = curbuf->b_fnum;
	    prev_diff_flags = diff_flags;
	}
    }

    जर (hlID == HLF_CHD || hlID == HLF_TXD)
    {
	col = tv_get_संख्या(&argvars[1]) - 1; // ignore type error in {col}
	जर (cache_results)
	{
	    जर (col >= change_start && col < change_end)
		hlID = HLF_TXD;			// changed text
	    अन्यथा
		hlID = HLF_CHD;			// changed line
	}
	अन्यथा
	{
	    hlID = HLF_CHD;
	    साठी (पूर्णांक i = 0; i < diffline.अंक_changes; i++)
	    {
		पूर्णांक added = diff_change_parse(&diffline, &diffline.changes[i],
			&change_start, &change_end);
		जर (col >= change_start && col < change_end)
		{
		    hlID = added ? HLF_TXA : HLF_TXD;
		    थांबा;
		}
		जर (col < change_start)
		    // the remaining changes are past this column and not relevant
		    थांबा;
	    }
	}
    }
    rettv->vval.v_संख्या = hlID == (hlf_T)0 ? 0 : (पूर्णांक)hlID;
# endif
}

# ifdef FEAT_DIFF
/*
 * Parse the diff options passed in "optarg" to the diff() function and return
 * the options in "diffopts" and the diff algorithm in "diffalgo".
 */
    स्थिर पूर्णांक
parse_diff_optarg(
    typval_T	    *opts,
    पूर्णांक		    *diffopts,
    दीर्घ	    *diffalgo,
    dio_outfmt_T    *diff_output_fmt,
    पूर्णांक		    *diff_ctxlen)
{
    dict_T *d = opts->vval.v_dict;

    char_u  *algo = dict_get_string(d, "algorithm", FALSE);
    जर (algo != रिक्त)
    {
	जर (STRNCMP(algo, "myers", 5) == 0)
	    *diffalgo = 0;
	अन्यथा जर (STRNCMP(algo, "minimal", 7) == 0)
	    *diffalgo = XDF_NEED_MINIMAL;
	अन्यथा जर (STRNCMP(algo, "patience", 8) == 0)
	    *diffalgo = XDF_PATIENCE_DIFF;
	अन्यथा जर (STRNCMP(algo, "histogram", 9) == 0)
	    *diffalgo = XDF_HISTOGRAM_DIFF;
    }

    char_u  *output_fmt = dict_get_string(d, "output", FALSE);
    जर (output_fmt != रिक्त)
    {
	जर (STRNCMP(output_fmt, "unified", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_UNIFIED;
	अन्यथा जर (STRNCMP(output_fmt, "indices", 7) == 0)
	    *diff_output_fmt = DIO_OUTPUT_INDICES;
	अन्यथा
	{
	    semsg(_(e_unsupported_diff_output_format_str), output_fmt);
	    परत FAIL;
	}
    }

    *diff_ctxlen = dict_get_संख्या_def(d, "context", 0);
    जर (*diff_ctxlen < 0)
	*diff_ctxlen = 0;

    जर (dict_get_bool(d, "iblank", FALSE))
	*diffopts |= DIFF_IBLANK;
    जर (dict_get_bool(d, "icase", FALSE))
	*diffopts |= DIFF_ICASE;
    जर (dict_get_bool(d, "iwhite", FALSE))
	*diffopts |= DIFF_IWHITE;
    जर (dict_get_bool(d, "iwhiteall", FALSE))
	*diffopts |= DIFF_IWHITEALL;
    जर (dict_get_bool(d, "iwhiteeol", FALSE))
	*diffopts |= DIFF_IWHITEEOL;
    जर (dict_get_bool(d, "indent-heuristic", FALSE))
	*diffalgo |= XDF_INDENT_HEURISTIC;

    परत OK;
}

/*
 * Concatenate the List of strings in "l" and store the result in
 * "din->din_mmfile.ptr" and the length in "din->din_mmfile.size".
 */
    स्थिर शून्य
list_to_diffin(list_T *l, diffin_T *din, पूर्णांक icase)
{
    garray_T	ga;
    listitem_T	*li;
    char_u	*str;

    ga_init2(&ga, 1, 2048);

    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	str = tv_get_string(&li->li_tv);
	जर (icase)
	{
	    str = strlow_save(str);
	    जर (str == रिक्त)
		सुरूठेवा;
	}
	ga_concat(&ga, str);
	ga_append(&ga, NL);
	जर (icase)
	    vim_free(str);
    }

    din->din_mmfile.ptr = (अक्षर *)ga.ga_data;
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
    जर (hunk_dict == रिक्त)
	परत रिक्त;

    dict_add_संख्या(hunk_dict, "from_idx", hunk->lnum_orig - 1);
    dict_add_संख्या(hunk_dict, "from_count", hunk->मोजणी_orig);
    dict_add_संख्या(hunk_dict, "to_idx", hunk->lnum_new  - 1);
    dict_add_संख्या(hunk_dict, "to_count", hunk->मोजणी_new);

    परत hunk_dict;
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

    जर (check_for_nonnull_list_arg(argvars, 0) == FAIL
	    || check_for_nonnull_list_arg(argvars, 1) == FAIL
	    || check_for_opt_nonnull_dict_arg(argvars, 2) == FAIL)
	परत;

    CLEAR_FIELD(dio);
    dio.dio_internal = TRUE;
    ga_init2(&dio.dio_diff.dout_ga, आकार(अक्षर *), 1000);

    list_T *orig_list = argvars[0].vval.v_list;
    list_T *new_list = argvars[1].vval.v_list;

    // Save the 'diffopt' option value and restore it after getting the diff.
    पूर्णांक		save_diff_flags = diff_flags;
    दीर्घ	save_diff_algorithm = diff_algorithm;
    diff_flags = DIFF_INTERNAL;
    diff_algorithm = 0;
    dio.dio_outfmt = DIO_OUTPUT_UNIFIED;
    जर (argvars[2].v_type != VAR_UNKNOWN)
	जर (parse_diff_optarg(&argvars[2], &diff_flags, &diff_algorithm,
			&dio.dio_outfmt, &dio.dio_ctxlen) == FAIL)
	    परत;

    // Concatenate the List of strings into a single string using newline
    // separator.  Internal diff library expects a single string.
    list_to_diffin(orig_list, &dio.dio_orig, diff_flags & DIFF_ICASE);
    list_to_diffin(new_list, &dio.dio_new, diff_flags & DIFF_ICASE);

    // If 'diffexpr' is set, then the internal diff is not used.  Set
    // 'diffexpr' to an empty string temporarily.
    पूर्णांक	    restore_diffexpr = FALSE;
    char_u  cc = *p_dex;
    जर (*p_dex != NUL)
    {
	restore_diffexpr = TRUE;
	*p_dex = NUL;
    }

    // Compute the diff
    पूर्णांक diff_status = diff_file(&dio);

    // restore 'diffexpr'
    जर (restore_diffexpr)
	*p_dex = cc;

    जर (diff_status == FAIL)
	जा done;

    पूर्णांक		hunk_idx = 0;
    dict_T	*hunk_dict;

    जर (dio.dio_outfmt == DIO_OUTPUT_INDICES)
    {
	जर (rettv_list_alloc(rettv) != OK)
	    जा done;
	list_T	*l = rettv->vval.v_list;

	// Process each diff hunk
	diffhunk_T	*hunk = रिक्त;
	पर्यंत (hunk_idx < dio.dio_diff.dout_ga.ga_len)
	{
	    hunk = ((diffhunk_T **)dio.dio_diff.dout_ga.ga_data)[hunk_idx++];

	    hunk_dict = get_diff_hunk_indices(hunk);
	    जर (hunk_dict == रिक्त)
		जा done;

	    जर (list_append_dict(l, hunk_dict) == FAIL)
		dict_unref(hunk_dict);
	}
    }
    अन्यथा
    {
	ga_append(&dio.dio_diff.dout_ga, NUL);
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string =
			vim_strsave((char_u *)dio.dio_diff.dout_ga.ga_data);
    }

done:
    clear_diffin(&dio.dio_new);
    जर (dio.dio_outfmt == DIO_OUTPUT_INDICES)
	clear_diffout(&dio.dio_diff);
    अन्यथा
	ga_clear(&dio.dio_diff.dout_ga);
    clear_diffin(&dio.dio_orig);
    // Restore the 'diffopt' option value.
    diff_flags = save_diff_flags;
    diff_algorithm = save_diff_algorithm;
# endif
}

#endif
