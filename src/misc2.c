/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * misc2.c: Various functions.
 */
#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ char_u	*username = ಖಾಲಿ; // cached result of mch_get_user_name()

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ coladvance2(pos_T *pos, ಪೂರ್ಣಾಂಕ addspaces, ಪೂರ್ಣಾಂಕ finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    ಪೂರ್ಣಾಂಕ
virtual_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_ve_flags = get_ve_flags();

    ಆದರೆ (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_ಸೇರಿಸು) && (State & ಬಹುಲಕ_ಸೇರಿಸು)))
	ಹಿಂತಿರುಗಿ TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    ಆದರೆ (virtual_op != MAYBE)
	ಹಿಂತಿರುಗಿ virtual_op;
    ಹಿಂತಿರುಗಿ (cur_ve_flags & VE_ಖಂಡ) && VIsual_ಸಕ್ರಿಯ && VIsual_ಬಹುಲಕ == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    ಪೂರ್ಣಾಂಕ
getviscol(ಶೂನ್ಯ)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, ಖಾಲಿ, ಖಾಲಿ, GETVCOL_NO_ABOVE);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    ಪೂರ್ಣಾಂಕ
coladvance_force(colnr_T wcol)
{
    ಪೂರ್ಣಾಂಕ rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    ಆದರೆ (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ಇಲ್ಲದಿದ್ದರೆ
	// Virtcol is valid
	ಗಣ_valid_virtcol(curwin, wcol);
    ಹಿಂತಿರುಗಿ rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    ಪೂರ್ಣಾಂಕ
getviscol2(colnr_T ಲಂಬಸಾಲು, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, ಖಾಲಿ, ಖಾಲಿ, GETVCOL_NO_ABOVE);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)x;
}

/*
 * Try to advance the Cursor to the specified screen column "wantcol".
 * If virtual editing: fine tune the cursor position.
 * Note that all virtual positions off the end of a line should share
 * a curwin->w_cursor.col value (n.b. this is equal to STRLEN(line)),
 * beginning at coladd 0.
 *
 * return OK if desired column is reached, FAIL if not
 */
    ಪೂರ್ಣಾಂಕ
coladvance(colnr_T wantcol)
{
    ಪೂರ್ಣಾಂಕ rc = getvpos(&curwin->w_cursor, wantcol);

    ಆದರೆ (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	ಗಣ_valid_virtcol(curwin, wantcol);
    ಹಿಂತಿರುಗಿ rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    ಪೂರ್ಣಾಂಕ
getvpos(pos_T *pos, colnr_T wantcol)
{
    ಹಿಂತಿರುಗಿ coladvance2(pos, FALSE, virtual_ಸಕ್ರಿಯ(), wantcol);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
coladvance2(
    pos_T	*pos,
    ಪೂರ್ಣಾಂಕ		addspaces,	// change the text to achieve our goal?
    ಪೂರ್ಣಾಂಕ		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		linelen;
    colnr_T	ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		csize = 0;
    ಪೂರ್ಣಾಂಕ		one_ಇನ್ನಷ್ಟು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		ತಲೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

    one_ಇನ್ನಷ್ಟು = (State & ಬಹುಲಕ_ಸೇರಿಸು)
		    || restart_edit != NUL
		    || (VIsual_ಸಕ್ರಿಯ && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    ಸಾಲು = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    ಆದರೆ (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_ಇನ್ನಷ್ಟು;
	    ಲಂಬಸಾಲು = wcol;

	    ಆದರೆ ((addspaces || finetune) && !VIsual_ಸಕ್ರಿಯ)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_ಇನ್ನಷ್ಟು;
		ಆದರೆ (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ		ಅಗಲ = curwin->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(curwin);
	chartabsize_T	cts;

	ಆದರೆ (finetune
		&& curwin->w_p_wrap
		&& curwin->w_ಅಗಲ != 0
		&& wcol >= (colnr_T)ಅಗಲ
		&& ಅಗಲ > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    ಆದರೆ (csize > 0)
		csize--;

	    ಆದರೆ (wcol / ಅಗಲ > (colnr_T)csize / ಅಗಲ
		    && ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0 || (ಪೂರ್ಣಾಂಕ)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / ಅಗಲ + 1) * ಅಗಲ - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ಸಾಲು, ಸಾಲು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	cts.cts_no_above = true;
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &ತಲೆ, ಖಾಲಿ);
	    MB_PTR_ADV(cts.cts_ptr);
#ಇಲ್ಲದಿದ್ದರೆ
	    csize = lbr_chartabsize_adv(&cts);
#ಆದರೆ_ಅಂತ್ಯ
	    cts.cts_vcol += csize;
	}
	ಲಂಬಸಾಲು = cts.cts_vcol;
	idx = (ಪೂರ್ಣಾಂಕ)(cts.cts_ptr - ಸಾಲು);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	ಆದರೆ (ಲಂಬಸಾಲು > wcol || (!virtual_ಸಕ್ರಿಯ() && one_ಇನ್ನಷ್ಟು == 0))
	{
	    idx -= 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= ತಲೆ;
#ಆದರೆ_ಅಂತ್ಯ
	    ಲಂಬಸಾಲು -= csize;
	}

	ಆದರೆ (virtual_ಸಕ್ರಿಯ()
		&& addspaces
		&& wcol >= 0
		&& ((ಲಂಬಸಾಲು != wcol && ಲಂಬಸಾಲು != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    ಆದರೆ (ಸಾಲು[idx] == NUL)
	    {
		// Append spaces
		ಪೂರ್ಣಾಂಕ	correct = wcol - ಲಂಬಸಾಲು;
		char_u	*newline = alloc(idx + correct + 1);
		ಪೂರ್ಣಾಂಕ	t;

		ಆದರೆ (newline == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		ಗಾಗಿ (t = 0; t < idx; ++t)
		    newline[t] = ಸಾಲು[t];

		ಗಾಗಿ (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		ಬದಲಾಯಿತು_ಬೈಟುಗಳು(pos->lnum, (colnr_T)idx);
		idx += correct;
		ಲಂಬಸಾಲು = wcol;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Break a tab
		ಪೂರ್ಣಾಂಕ	correct = wcol - ಲಂಬಸಾಲು - csize + 1; // negative!!
		char_u	*newline;
		ಪೂರ್ಣಾಂಕ	t, s = 0;
		ಪೂರ್ಣಾಂಕ	v;

		ಆದರೆ (-correct > csize)
		    ಹಿಂತಿರುಗಿ FAIL;

		newline = alloc(linelen + csize);
		ಆದರೆ (newline == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		ಗಾಗಿ (t = 0; t < linelen; t++)
		{
		    ಆದರೆ (t != idx)
			newline[s++] = ಸಾಲು[t];
		    ಇಲ್ಲದಿದ್ದರೆ
			ಗಾಗಿ (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		ಬದಲಾಯಿತು_ಬೈಟುಗಳು(pos->lnum, idx);
		idx += (csize - 1 + correct);
		ಲಂಬಸಾಲು += correct;
	    }
	}
    }

    ಆದರೆ (idx < 0)
	pos->ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	pos->ಲಂಬಸಾಲು = idx;

    pos->coladd = 0;

    ಆದರೆ (finetune)
    {
	ಆದರೆ (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    ಆದರೆ (!one_ಇನ್ನಷ್ಟು)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, ಖಾಲಿ, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ b = (ಪೂರ್ಣಾಂಕ)wcol - (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು;

	    // The difference between wcol and col is used to set coladd.
	    ಆದರೆ (b > 0 && b < (MAXCOL - 2 * curwin->w_ಅಗಲ))
		pos->coladd = b;

	    ಲಂಬಸಾಲು += b;
	}
    }

    // prevent from moving onto a trail byte
    ಆದರೆ (has_mbyte)
	mb_adjustpos(curbuf, pos);

    ಆದರೆ (wcol < 0 || ಲಂಬಸಾಲು < wcol)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    ಪೂರ್ಣಾಂಕ
inc_cursor(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    ಪೂರ್ಣಾಂಕ
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    ಆದರೆ (lp->ಲಂಬಸಾಲು != MAXCOL)
    {
	p = ml_get_pos(lp);
	ಆದರೆ (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(p);

		lp->ಲಂಬಸಾಲು += l;
		ಹಿಂತಿರುಗಿ ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->ಲಂಬಸಾಲು++;
	    lp->coladd = 0;
	    ಹಿಂತಿರುಗಿ ((p[1] != NUL) ? 0 : 2);
	}
    }
    ಆದರೆ (lp->lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)     // there is a next line
    {
	lp->ಲಂಬಸಾಲು = 0;
	lp->lnum++;
	lp->coladd = 0;
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    ಪೂರ್ಣಾಂಕ
incl(pos_T *lp)
{
    ಪೂರ್ಣಾಂಕ	    r;

    ಆದರೆ ((r = inc(lp)) >= 1 && lp->ಲಂಬಸಾಲು)
	r = inc(lp);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    ಪೂರ್ಣಾಂಕ
dec_cursor(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ dec(&curwin->w_cursor);
}

    ಪೂರ್ಣಾಂಕ
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    ಆದರೆ (lp->ಲಂಬಸಾಲು == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->ಲಂಬಸಾಲು = ml_get_len(lp->lnum);
	ಆದರೆ (has_mbyte)
	    lp->ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + lp->ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (lp->ಲಂಬಸಾಲು > 0)
    {
	// still within line
	lp->ಲಂಬಸಾಲು--;
	ಆದರೆ (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + lp->ಲಂಬಸಾಲು);
	}
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->ಲಂಬಸಾಲು = ml_get_len(lp->lnum);
	ಆದರೆ (has_mbyte)
	    lp->ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + lp->ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ 1;
    }

    // at start of file
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    ಪೂರ್ಣಾಂಕ
decl(pos_T *lp)
{
    ಪೂರ್ಣಾಂಕ	    r;

    ಆದರೆ ((r = dec(lp)) == 1 && lp->ಲಂಬಸಾಲು)
	r = dec(lp);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Get the line number relative to the current cursor position, i.e. the
 * difference between line number and cursor position. Only look for lines that
 * can be visible, folded lines don't count.
 */
    linenr_T
get_cursor_rel_lnum(
    win_T	*wp,
    linenr_T	lnum)		    // line number to get the result for
{
    linenr_T	cursor = wp->w_cursor.lnum;
    linenr_T	retval = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (hasAnyFolding(wp))
    {
	ಆದರೆ (lnum > cursor)
	{
	    ಆಗ (lnum > cursor)
	    {
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		ಆದರೆ (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum < cursor)
	{
	    ಆಗ (lnum < cursor)
	    {
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, ಖಾಲಿ, &lnum, TRUE, ಖಾಲಿ);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		ಆದರೆ (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	retval = lnum - cursor;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    ಶೂನ್ಯ
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    ಆದರೆ (pos->lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	pos->lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (pos->ಲಂಬಸಾಲು > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	ಆದರೆ (pos->ಲಂಬಸಾಲು > len)
	    pos->ಲಂಬಸಾಲು = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    ಶೂನ್ಯ
check_cursor_lnum(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	ಆದರೆ (!hasFolding(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
						&curwin->w_cursor.lnum, ಖಾಲಿ))
#ಆದರೆ_ಅಂತ್ಯ
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }
    ಆದರೆ (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    ಶೂನ್ಯ
check_cursor_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    check_cursor_ಲಂಬಸಾಲು_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    ಶೂನ್ಯ
check_cursor_ಲಂಬಸಾಲು_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.ಲಂಬಸಾಲು;
    colnr_T      oldcoladd = win->w_cursor.ಲಂಬಸಾಲು + win->w_cursor.coladd;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_ಬಫರ್, win->w_cursor.lnum);
    ಆದರೆ (len == 0)
	win->w_cursor.ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win->w_cursor.ಲಂಬಸಾಲು >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) || restart_edit
		|| (VIsual_ಸಕ್ರಿಯ && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_ಸಕ್ರಿಯ())
	    win->w_cursor.ಲಂಬಸಾಲು = len;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    win->w_cursor.ಲಂಬಸಾಲು = len - 1;
	    // Move the cursor to the head byte.
	    ಆದರೆ (has_mbyte)
		mb_adjustpos(win->w_ಬಫರ್, &win->w_cursor);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win->w_cursor.ಲಂಬಸಾಲು < 0)
	win->w_cursor.ಲಂಬಸಾಲು = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    ಆದರೆ (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_ve_flags == VE_ALL)
    {
	ಆದರೆ (oldcoladd > win->w_cursor.ಲಂಬಸಾಲು)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.ಲಂಬಸಾಲು;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    ಆದರೆ (win->w_cursor.ಲಂಬಸಾಲು + 1 < len)
	    {
		ಪೂರ್ಣಾಂಕ cs, ce;

		getvcol(win, &win->w_cursor, &cs, ಖಾಲಿ, &ce, 0);
		ಆದರೆ (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    ಶೂನ್ಯ
check_cursor(ಶೂನ್ಯ)
{
    check_cursor_lnum();
    check_cursor_ಲಂಬಸಾಲು();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    ಶೂನ್ಯ
check_visual_pos(ಶೂನ್ಯ)
{
    ಆದರೆ (VIsual.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	VIsual.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	VIsual.ಲಂಬಸಾಲು = 0;
	VIsual.coladd = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ len = ml_get_len(VIsual.lnum);

	ಆದರೆ (VIsual.ಲಂಬಸಾಲು > len)
	{
	    VIsual.ಲಂಬಸಾಲು = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    ಶೂನ್ಯ
adjust_cursor_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > 0
	    && (!VIsual_ಸಕ್ರಿಯ || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.ಲಂಬಸಾಲು;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_leftcol(colnr_T leftcol)
{
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    // Return quickly when there is no change.
    ಆದರೆ (curwin->w_leftcol == leftcol)
	ಹಿಂತಿರುಗಿ FALSE;
    curwin->w_leftcol = leftcol;

    ಬದಲಾಯಿತು_cline_bef_curs();
    ದೀರ್ಘ lastcol = curwin->w_leftcol + curwin->w_ಅಗಲ - curwin_ಲಂಬಸಾಲು_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    ದೀರ್ಘ siso = get_sidescrolloff_ಮೌಲ್ಯ();
    ಆದರೆ (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(ಶೂನ್ಯ)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, ಖಾಲಿ, &e, 0);
    ಆದರೆ (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s < curwin->w_leftcol)
    {
	retval = TRUE;
	ಆದರೆ (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    ಬದಲಾಯಿತು_cline_bef_curs();
	}
    }

    ಆದರೆ (retval)
	curwin->w_ಗಣ_curswant = true;
    redraw_later(UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    ಪೂರ್ಣಾಂಕ
copy_option_part(
    char_u	**option,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ಅಕ್ಷರ	*sep_chars)
{
    ಪೂರ್ಣಾಂಕ	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    ಆದರೆ (*p == '.')
	buf[len++] = *p++;
    ಆಗ (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == ಖಾಲಿ)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	ಆದರೆ (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != ಖಾಲಿ)
	    ++p;
	ಆದರೆ (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    ಆದರೆ (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_ಗಮ್ಯ_option_part(p);	// p points to next file name

    *option = p;
    ಹಿಂತಿರುಗಿ len;
}

#ಆದರೆ !defined(HAVE_MEMSET) && !defined(PROTO)
    ಶೂನ್ಯ *
vim_memset(ಶೂನ್ಯ *ptr, ಪೂರ್ಣಾಂಕ c, size_t ಗಾತ್ರ_2)
{
    ಅಕ್ಷರ *p = ptr;

    ಆಗ (ಗಾತ್ರ_2-- > 0)
	*p++ = c;
    ಹಿಂತಿರುಗಿ ptr;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    ಪೂರ್ಣಾಂಕ
vim_isspace(ಪೂರ್ಣಾಂಕ x)
{
    ಹಿಂತಿರುಗಿ ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

ಸ್ಥಿರ ರಚನೆ modmasktable
{
    ಗಿಡ್ಡ	mod_ಮುಖವಾಡ;	// Bit-mask for particular key modifier
    ಗಿಡ್ಡ	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[] =
{
    {MOD_ಮುಖವಾಡ_ALT,		MOD_ಮುಖವಾಡ_ALT,		(char_u)'M'},
    {MOD_ಮುಖವಾಡ_META,		MOD_ಮುಖವಾಡ_META,		(char_u)'T'},
    {MOD_ಮುಖವಾಡ_CTRL,		MOD_ಮುಖವಾಡ_CTRL,		(char_u)'C'},
    {MOD_ಮುಖವಾಡ_ಸರಿಸು,		MOD_ಮುಖವಾಡ_ಸರಿಸು,		(char_u)'S'},
    {MOD_ಮುಖವಾಡ_MULTI_CLICK,	MOD_ಮುಖವಾಡ_2_CLICK,	(char_u)'2'},
    {MOD_ಮುಖವಾಡ_MULTI_CLICK,	MOD_ಮುಖವಾಡ_3_CLICK,	(char_u)'3'},
    {MOD_ಮುಖವಾಡ_MULTI_CLICK,	MOD_ಮುಖವಾಡ_4_CLICK,	(char_u)'4'},
#ಆದರೆ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_ಮುಖವಾಡ_CMD,		MOD_ಮುಖವಾಡ_CMD,		(char_u)'D'},
#ಆದರೆ_ಅಂತ್ಯ
    // 'A' must be the last one
    {MOD_ಮುಖವಾಡ_ALT,		MOD_ಮುಖವಾಡ_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಕೀಲಿಗಳು_ನಮೂದು_ಗಾತ್ರ 5

ಸ್ಥಿರ char_u modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[] =
{
//  mod mask	    with modifier		without modifier
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '&', '9',			'@', '1',	// begin
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '&', '0',			'@', '2',	// cancel
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '1',			'@', '4',	// command
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '2',			'@', '5',	// copy
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '3',			'@', '6',	// create
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '4',			'k', 'D',	// delete char
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '5',			'k', 'L',	// delete line
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '7',			'@', '7',	// end
    MOD_ಮುಖವಾಡ_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_C_ಕೊನೆ,	'@', '7',	// end
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '9',			'@', '9',	// exit
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '*', '0',			'@', '0',	//ಕಂಡುಹಿಡಿ
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '#', '1',			'%', '1',	// help
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '#', '2',			'k', 'h',	// home
    MOD_ಮುಖವಾಡ_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_C_HOME,	'k', 'h',	// home
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '#', '3',			'k', 'I',	// insert
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '#', '4',			'k', 'l',	// left arrow
    MOD_ಮುಖವಾಡ_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_C_ಎಡ,	'k', 'l',	// left arrow
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'a',			'%', '3',	//ಸಂದೇಶ
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'b',			'%', '4',	// move
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'c',			'%', '5',	//ಮುಂದಿನ
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'd',			'%', '7',	// options
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'e',			'%', '8',	//ಹಿಂದಿನ
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'f',			'%', '9',	// print
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'g',			'%', '0',	// redo
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'h',			'&', '3',	// replace
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'i',			'k', 'r',	// right arr.
    MOD_ಮುಖವಾಡ_CTRL,  KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_C_ಬಲ,	'k', 'r',	// right arr.
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '%', 'j',			'&', '5',	// resume
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '!', '1',			'&', '6',	// save
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '!', '2',			'&', '7',	// suspend
    MOD_ಮುಖವಾಡ_ಸರಿಸು, '!', '3',			'&', '8',	// undo
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_ಮೇಲೆ,	'k', 'u',	// up arrow
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_ಕೆಳಕ್ಕೆ,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_XF1,	KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_XF1,
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_XF2,	KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_XF2,
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_XF3,	KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_XF3,
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_XF4,	KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_XF4,

    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F1,	'k', '1',	// F1
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F2,	'k', '2',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F3,	'k', '3',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F4,	'k', '4',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F5,	'k', '5',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F6,	'k', '6',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F7,	'k', '7',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F8,	'k', '8',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F9,	'k', '9',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F10,	'k', ';',	// F10

    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F11,	'F', '1',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F12,	'F', '2',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F13,	'F', '3',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F14,	'F', '4',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F15,	'F', '5',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F16,	'F', '6',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F17,	'F', '7',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F18,	'F', '8',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F19,	'F', '9',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F20,	'F', 'A',

    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F21,	'F', 'B',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F22,	'F', 'C',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F23,	'F', 'D',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F24,	'F', 'E',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F25,	'F', 'F',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F26,	'F', 'G',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F27,	'F', 'H',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F28,	'F', 'I',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F29,	'F', 'J',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F30,	'F', 'K',

    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F31,	'F', 'L',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F32,	'F', 'M',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F33,	'F', 'N',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F34,	'F', 'O',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F35,	'F', 'P',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F36,	'F', 'Q',
    MOD_ಮುಖವಾಡ_ಸರಿಸು, KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_ಮುಖವಾಡ_ಸರಿಸು, 'k', 'B',			KS_EXTRA, (ಪೂರ್ಣಾಂಕ)KE_TAB,

    NUL
};

ಸ್ಥಿರ ರಚನೆ ಕೀಲಿ_name_ನಮೂದು
{
    ಪೂರ್ಣಾಂಕ		enabled;	    // is this entry available (TRUE/FALSE)?
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;		    // special key code or ascii value
    string_T	name;		    // name of key
    ಪೂರ್ಣಾಂಕ		is_alt;		    // is an alternative name
} ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[] =
// Must be sorted by the 'name.string' field in ascending order because it is used by bsearch()!
{
    {TRUE, K_BS, STR_LITERAL_INIT("BackSpace"), TRUE},
    {TRUE, '|', STR_LITERAL_INIT("Bar"), FALSE},
    {TRUE, K_BS, STR_LITERAL_INIT("BS"), FALSE},
    {TRUE, '\\', STR_LITERAL_INIT("Bslash"), FALSE},
    {TRUE, K_COMMAND, STR_LITERAL_INIT("Cmd"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("CR"), FALSE},
    {TRUE, CSI, STR_LITERAL_INIT("CSI"), FALSE},
    {TRUE, K_CURSORHOLD, STR_LITERAL_INIT("CursorHold"), FALSE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_ಕೆಳಕ್ಕೆ, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_ಕೊನೆ, STR_LITERAL_INIT("End"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Enter"), TRUE},
    {TRUE, ESC, STR_LITERAL_INIT("Esc"), FALSE},

    {TRUE, K_F1, STR_LITERAL_INIT("F1"), FALSE},
    {TRUE, K_F10, STR_LITERAL_INIT("F10"), FALSE},
    {TRUE, K_F11, STR_LITERAL_INIT("F11"), FALSE},
    {TRUE, K_F12, STR_LITERAL_INIT("F12"), FALSE},
    {TRUE, K_F13, STR_LITERAL_INIT("F13"), FALSE},
    {TRUE, K_F14, STR_LITERAL_INIT("F14"), FALSE},
    {TRUE, K_F15, STR_LITERAL_INIT("F15"), FALSE},
    {TRUE, K_F16, STR_LITERAL_INIT("F16"), FALSE},
    {TRUE, K_F17, STR_LITERAL_INIT("F17"), FALSE},
    {TRUE, K_F18, STR_LITERAL_INIT("F18"), FALSE},
    {TRUE, K_F19, STR_LITERAL_INIT("F19"), FALSE},

    {TRUE, K_F2, STR_LITERAL_INIT("F2"), FALSE},
    {TRUE, K_F20, STR_LITERAL_INIT("F20"), FALSE},
    {TRUE, K_F21, STR_LITERAL_INIT("F21"), FALSE},
    {TRUE, K_F22, STR_LITERAL_INIT("F22"), FALSE},
    {TRUE, K_F23, STR_LITERAL_INIT("F23"), FALSE},
    {TRUE, K_F24, STR_LITERAL_INIT("F24"), FALSE},
    {TRUE, K_F25, STR_LITERAL_INIT("F25"), FALSE},
    {TRUE, K_F26, STR_LITERAL_INIT("F26"), FALSE},
    {TRUE, K_F27, STR_LITERAL_INIT("F27"), FALSE},
    {TRUE, K_F28, STR_LITERAL_INIT("F28"), FALSE},
    {TRUE, K_F29, STR_LITERAL_INIT("F29"), FALSE},

    {TRUE, K_F3, STR_LITERAL_INIT("F3"), FALSE},
    {TRUE, K_F30, STR_LITERAL_INIT("F30"), FALSE},
    {TRUE, K_F31, STR_LITERAL_INIT("F31"), FALSE},
    {TRUE, K_F32, STR_LITERAL_INIT("F32"), FALSE},
    {TRUE, K_F33, STR_LITERAL_INIT("F33"), FALSE},
    {TRUE, K_F34, STR_LITERAL_INIT("F34"), FALSE},
    {TRUE, K_F35, STR_LITERAL_INIT("F35"), FALSE},
    {TRUE, K_F36, STR_LITERAL_INIT("F36"), FALSE},
    {TRUE, K_F37, STR_LITERAL_INIT("F37"), FALSE},

    {TRUE, K_F4, STR_LITERAL_INIT("F4"), FALSE},
    {TRUE, K_F5, STR_LITERAL_INIT("F5"), FALSE},
    {TRUE, K_F6, STR_LITERAL_INIT("F6"), FALSE},
    {TRUE, K_F7, STR_LITERAL_INIT("F7"), FALSE},
    {TRUE, K_F8, STR_LITERAL_INIT("F8"), FALSE},
    {TRUE, K_F9, STR_LITERAL_INIT("F9"), FALSE},

    {TRUE, K_FOCUSGAINED, STR_LITERAL_INIT("FocusGained"), FALSE},
    {TRUE, K_FOCUSLOST, STR_LITERAL_INIT("FocusLost"), FALSE},
    {TRUE, K_HELP, STR_LITERAL_INIT("Help"), FALSE},
    {TRUE, K_HOME, STR_LITERAL_INIT("Home"), FALSE},
    {TRUE, K_IGNORE, STR_LITERAL_INIT("Ignore"), FALSE},
    {TRUE, K_INS, STR_LITERAL_INIT("Ins"), TRUE},
    {TRUE, K_INS, STR_LITERAL_INIT("Insert"), FALSE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_JSBTERM_MOUSE, STR_LITERAL_INIT("JsbMouse"), FALSE},
    {TRUE, K_K0, STR_LITERAL_INIT("k0"), FALSE},
    {TRUE, K_K1, STR_LITERAL_INIT("k1"), FALSE},
    {TRUE, K_K2, STR_LITERAL_INIT("k2"), FALSE},
    {TRUE, K_K3, STR_LITERAL_INIT("k3"), FALSE},
    {TRUE, K_K4, STR_LITERAL_INIT("k4"), FALSE},
    {TRUE, K_K5, STR_LITERAL_INIT("k5"), FALSE},
    {TRUE, K_K6, STR_LITERAL_INIT("k6"), FALSE},
    {TRUE, K_K7, STR_LITERAL_INIT("k7"), FALSE},
    {TRUE, K_K8, STR_LITERAL_INIT("k8"), FALSE},
    {TRUE, K_K9, STR_LITERAL_INIT("k9"), FALSE},

    {TRUE, K_KDEL, STR_LITERAL_INIT("kDel"), FALSE},
    {TRUE, K_KDIVIDE, STR_LITERAL_INIT("kDivide"), FALSE},
    {TRUE, K_KEND, STR_LITERAL_INIT("kEnd"), FALSE},
    {TRUE, K_KENTER, STR_LITERAL_INIT("kEnter"), FALSE},
    {TRUE, K_KHOME, STR_LITERAL_INIT("kHome"), FALSE},
    {TRUE, K_KINS, STR_LITERAL_INIT("kInsert"), FALSE},
    {TRUE, K_KMINUS, STR_LITERAL_INIT("kMinus"), FALSE},
    {TRUE, K_KMULTIPLY, STR_LITERAL_INIT("kMultiply"), FALSE},
    {TRUE, K_KPAGEDOWN, STR_LITERAL_INIT("kPageDown"), FALSE},
    {TRUE, K_KPAGEUP, STR_LITERAL_INIT("kPageUp"), FALSE},
    {TRUE, K_KPLUS, STR_LITERAL_INIT("kPlus"), FALSE},
    {TRUE, K_KPOINT, STR_LITERAL_INIT("kPoint"), FALSE},
    {TRUE, K_ಎಡ, STR_LITERAL_INIT("Left"), FALSE},
    {TRUE, K_LEFTDRAG, STR_LITERAL_INIT("LeftDrag"), FALSE},
    {TRUE, K_LEFTMOUSE, STR_LITERAL_INIT("LeftMouse"), FALSE},
    {TRUE, K_LEFTMOUSE_NM, STR_LITERAL_INIT("LeftMouseNM"), FALSE},
    {TRUE, K_LEFTRELEASE, STR_LITERAL_INIT("LeftRelease"), FALSE},
    {TRUE, K_LEFTRELEASE_NM, STR_LITERAL_INIT("LeftReleaseNM"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("LF"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("LineFeed"), TRUE},
    {TRUE, '<', STR_LITERAL_INIT("lt"), FALSE},
    {TRUE, K_MIDDLEDRAG, STR_LITERAL_INIT("MiddleDrag"), FALSE},
    {TRUE, K_MIDDLEMOUSE, STR_LITERAL_INIT("MiddleMouse"), FALSE},
    {TRUE, K_MIDDLERELEASE, STR_LITERAL_INIT("MiddleRelease"), FALSE},
    {TRUE, K_MOUSE, STR_LITERAL_INIT("Mouse"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("MouseDown"), TRUE},
    {TRUE, K_MOUSEMOVE, STR_LITERAL_INIT("MouseMove"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("MouseUp"), TRUE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_NET
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_NETTERM_MOUSE, STR_LITERAL_INIT("NetMouse"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("NewLine"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("NL"), FALSE},
    {TRUE, K_ZERO, STR_LITERAL_INIT("Nul"), FALSE},
    {TRUE, OSC, STR_LITERAL_INIT("OSC"), FALSE},
    {TRUE, K_PAGEDOWN, STR_LITERAL_INIT("PageDown"), FALSE},
    {TRUE, K_PAGEUP, STR_LITERAL_INIT("PageUp"), FALSE},
    {TRUE, K_PE, STR_LITERAL_INIT("PasteEnd"), FALSE},
    {TRUE, K_PS, STR_LITERAL_INIT("PasteStart"), FALSE},
    {TRUE, K_PLUG, STR_LITERAL_INIT("Plug"), FALSE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_ಬಲ, STR_LITERAL_INIT("Right"), FALSE},
    {TRUE, K_RIGHTDRAG, STR_LITERAL_INIT("RightDrag"), FALSE},
    {TRUE, K_RIGHTMOUSE, STR_LITERAL_INIT("RightMouse"), FALSE},
    {TRUE, K_RIGHTRELEASE, STR_LITERAL_INIT("RightRelease"), FALSE},
    {TRUE, K_SCRIPT_COMMAND, STR_LITERAL_INIT("ScriptCmd"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("ScrollWheelDown"), FALSE},
    {TRUE, K_MOUSERIGHT, STR_LITERAL_INIT("ScrollWheelLeft"), FALSE},
    {TRUE, K_MOUSELEFT, STR_LITERAL_INIT("ScrollWheelRight"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("ScrollWheelUp"), FALSE},
    {TRUE, K_SGR_MOUSE, STR_LITERAL_INIT("SgrMouse"), FALSE},
    {TRUE, K_SGR_MOUSERELEASE, STR_LITERAL_INIT("SgrMouseRelease"), FALSE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_ಮೇಲೆ, STR_LITERAL_INIT("Up"), FALSE},
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_URXVT
    TRUE,
#ಇಲ್ಲದಿದ್ದರೆ
    FALSE,
#ಆದರೆ_ಅಂತ್ಯ
	K_URXVT_MOUSE, STR_LITERAL_INIT("UrxvtMouse"), FALSE},
    {TRUE, K_X1DRAG, STR_LITERAL_INIT("X1Drag"), FALSE},
    {TRUE, K_X1MOUSE, STR_LITERAL_INIT("X1Mouse"), FALSE},
    {TRUE, K_X1RELEASE, STR_LITERAL_INIT("X1Release"), FALSE},
    {TRUE, K_X2DRAG, STR_LITERAL_INIT("X2Drag"), FALSE},
    {TRUE, K_X2MOUSE, STR_LITERAL_INIT("X2Mouse"), FALSE},
    {TRUE, K_X2RELEASE, STR_LITERAL_INIT("X2Release"), FALSE},
    {TRUE, K_CSI, STR_LITERAL_INIT("xCSI"), FALSE},
    {TRUE, K_XDOWN, STR_LITERAL_INIT("xDown"), FALSE},
    {TRUE, K_XEND, STR_LITERAL_INIT("xEnd"), FALSE},
    {TRUE, K_XF1, STR_LITERAL_INIT("xF1"), FALSE},
    {TRUE, K_XF2, STR_LITERAL_INIT("xF2"), FALSE},
    {TRUE, K_XF3, STR_LITERAL_INIT("xF3"), FALSE},
    {TRUE, K_XF4, STR_LITERAL_INIT("xF4"), FALSE},
    {TRUE, K_XHOME, STR_LITERAL_INIT("xHome"), FALSE},
    {TRUE, K_XLEFT, STR_LITERAL_INIT("xLeft"), FALSE},
    {TRUE, K_OSC, STR_LITERAL_INIT("xOSC"), FALSE},
    {TRUE, K_XRIGHT, STR_LITERAL_INIT("xRight"), FALSE},
    {TRUE, K_XUP, STR_LITERAL_INIT("xUp"), FALSE},
    {TRUE, K_ZEND, STR_LITERAL_INIT("zEnd"), FALSE},
    {TRUE, K_ZHOME, STR_LITERAL_INIT("zHome"), FALSE}
    // NOTE: When adding a long name update MAX_KEY_NAME_LEN.
};

/*
 * Return the modifier mask bit (MOD_MASK_*) which corresponds to the given
 * modifier name ('S' for Shift, 'C' for Ctrl etc).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
name_ಗಮ್ಯ_mod_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    i;

    c = TOUPPER_ASC(c);
    ಗಾಗಿ (i = 0; mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].mod_ಮುಖವಾಡ != 0; i++)
	ಆದರೆ (c == mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].name)
	    ಹಿಂತಿರುಗಿ mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].mod_flag;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    ಪೂರ್ಣಾಂಕ
simplify_ಕೀಲಿ(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, ಪೂರ್ಣಾಂಕ *modifiers)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    key0;
    ಪೂರ್ಣಾಂಕ	    key1;

    ಆದರೆ (!(*modifiers & (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL)))
	ಹಿಂತಿರುಗಿ ಕೀಲಿ;

    // TAB is a special case
    ಆದರೆ (ಕೀಲಿ == TAB && (*modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು))
    {
	*modifiers &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
	ಹಿಂತಿರುಗಿ K_S_TAB;
    }
    key0 = KEY2TERMCAP0(ಕೀಲಿ);
    key1 = KEY2TERMCAP1(ಕೀಲಿ);
    ಗಾಗಿ (i = 0; modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i] != NUL; i += MOD_ಕೀಲಿಗಳು_ನಮೂದು_ಗಾತ್ರ)
    {
	ಆದರೆ (key0 == modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 3]
		&& key1 == modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 4]
		&& (*modifiers & modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i]))
	{
	    *modifiers &= ~modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i];
	    ಹಿಂತಿರುಗಿ TERMCAP2_ಕೀಲಿ(modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 1],
		    modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 2]);
	}
    }
    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    ಪೂರ್ಣಾಂಕ
handle_x_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
    ಆಯ್ಕೆ (ಕೀಲಿ)
    {
	ಸ್ಥಿತಿ K_XUP:	ಹಿಂತಿರುಗಿ K_ಮೇಲೆ;
	ಸ್ಥಿತಿ K_XDOWN:	ಹಿಂತಿರುಗಿ K_ಕೆಳಕ್ಕೆ;
	ಸ್ಥಿತಿ K_XLEFT:	ಹಿಂತಿರುಗಿ K_ಎಡ;
	ಸ್ಥಿತಿ K_XRIGHT:	ಹಿಂತಿರುಗಿ K_ಬಲ;
	ಸ್ಥಿತಿ K_XHOME:	ಹಿಂತಿರುಗಿ K_HOME;
	ಸ್ಥಿತಿ K_ZHOME:	ಹಿಂತಿರುಗಿ K_HOME;
	ಸ್ಥಿತಿ K_XEND:	ಹಿಂತಿರುಗಿ K_ಕೊನೆ;
	ಸ್ಥಿತಿ K_ZEND:	ಹಿಂತಿರುಗಿ K_ಕೊನೆ;
	ಸ್ಥಿತಿ K_XF1:	ಹಿಂತಿರುಗಿ K_F1;
	ಸ್ಥಿತಿ K_XF2:	ಹಿಂತಿರುಗಿ K_F2;
	ಸ್ಥಿತಿ K_XF3:	ಹಿಂತಿರುಗಿ K_F3;
	ಸ್ಥಿತಿ K_XF4:	ಹಿಂತಿರುಗಿ K_F4;
	ಸ್ಥಿತಿ K_S_XF1:	ಹಿಂತಿರುಗಿ K_S_F1;
	ಸ್ಥಿತಿ K_S_XF2:	ಹಿಂತಿರುಗಿ K_S_F2;
	ಸ್ಥಿತಿ K_S_XF3:	ಹಿಂತಿರುಗಿ K_S_F3;
	ಸ್ಥಿತಿ K_S_XF4:	ಹಿಂತಿರುಗಿ K_S_F4;
    }
    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_ಕೀಲಿ_name(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ modifiers)
{
    ಸ್ಥಿರ char_u string[MAX_ಕೀಲಿ_NAME_LEN + 1];
    ಪೂರ್ಣಾಂಕ	    i, idx, len;
    ಪೂರ್ಣಾಂಕ	    ಕೋಷ್ಟಕ_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    ಆದರೆ (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_ಕೀಲಿ)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    ಆದರೆ (IS_SPECIAL(c))
    {
	ಗಾಗಿ (i = 0; modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i] != 0; i += MOD_ಕೀಲಿಗಳು_ನಮೂದು_ಗಾತ್ರ)
	    ಆದರೆ (       KEY2TERMCAP0(c) == (ಪೂರ್ಣಾಂಕ)modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 1]
		    && (ಪೂರ್ಣಾಂಕ)KEY2TERMCAP1(c) == (ಪೂರ್ಣಾಂಕ)modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 2])
	    {
		modifiers |= modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i];
		c = TERMCAP2_ಕೀಲಿ(modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 3],
						   modifier_ಕೀಲಿಗಳು_ಕೋಷ್ಟಕ[i + 4]);
		ನಿಲ್ಲಿಸು;
	    }
    }

    // try to find the key in the special key table
    ಕೋಷ್ಟಕ_idx = ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ_ಒಳ_ಕೋಷ್ಟಕ(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    ಆದರೆ (c > 0 && (*mb_char2len)(c) == 1)
    {
	ಆದರೆ (ಕೋಷ್ಟಕ_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_ಮುಖವಾಡ_ALT;
	    // try again, to find the un-alted key in the special key table
	    ಕೋಷ್ಟಕ_idx = ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ_ಒಳ_ಕೋಷ್ಟಕ(c);
	}
	ಆದರೆ (ಕೋಷ್ಟಕ_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_ಮುಖವಾಡ_CTRL;
	}
    }

    // translate the modifier into a string
    ಗಾಗಿ (i = 0; mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].name != 'A'; i++)
	ಆದರೆ ((modifiers & mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].mod_ಮುಖವಾಡ)
						== mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].mod_flag)
	{
	    string[idx++] = mod_ಮುಖವಾಡ_ಕೋಷ್ಟಕ[i].name;
	    string[idx++] = (char_u)'-';
	}

    ಆದರೆ (ಕೋಷ್ಟಕ_idx < 0)		// unknown special key, may output t_xx
    {
	ಆದರೆ (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = (*mb_char2len)(c);
	    ಆದರೆ (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	*s = transchar(c);
		ಆಗ (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ		// use name of special key
    {
	string_T    *s;

	s = &ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[ಕೋಷ್ಟಕ_idx].name;

	ಆದರೆ (s->ಉದ್ದಳತೆ + idx + 2 <= MAX_ಕೀಲಿ_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (ಪೂರ್ಣಾಂಕ)s->ಉದ್ದಳತೆ;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    ಹಿಂತಿರುಗಿ string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    ಪೂರ್ಣಾಂಕ
trans_special(
    char_u	**srcp,
    char_u	*dst,
    ಪೂರ್ಣಾಂಕ		flags,		// FSK_ values
    ಪೂರ್ಣಾಂಕ		escape_ks,	// escape K_SPECIAL bytes in the character
    ಪೂರ್ಣಾಂಕ		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    ಪೂರ್ಣಾಂಕ		modifiers = 0;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;

    ಕೀಲಿ = ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ(srcp, &modifiers, flags, did_simplify);
    ಆದರೆ (ಕೀಲಿ == 0)
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ special_ಗಮ್ಯ_buf(ಕೀಲಿ, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    ಪೂರ್ಣಾಂಕ
special_ಗಮ್ಯ_buf(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, ಪೂರ್ಣಾಂಕ modifiers, ಪೂರ್ಣಾಂಕ escape_ks, char_u *dst)
{
    ಪೂರ್ಣಾಂಕ		dlen = 0;

    // Put the appropriate modifier in a string
    ಆದರೆ (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(ಕೀಲಿ);
	dst[dlen++] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (escape_ks)
	dlen = (ಪೂರ್ಣಾಂಕ)(ಕೂಡಿಸು_char2buf(ಕೀಲಿ, dst + dlen) - dst);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	dlen += (*mb_char2bytes)(ಕೀಲಿ, dst + dlen);
    ಇಲ್ಲದಿದ್ದರೆ
	dst[dlen++] = ಕೀಲಿ;

    ಹಿಂತಿರುಗಿ dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ(
    char_u	**srcp,
    ಪೂರ್ಣಾಂಕ		*modp,
    ಪೂರ್ಣಾಂಕ		flags,		// FSK_ values
    ಪೂರ್ಣಾಂಕ		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*ಕೊನೆಯ_dash;
    char_u	*ಕೊನೆ_ದ_name;
    char_u	*src;
    char_u	*bp;
    ಪೂರ್ಣಾಂಕ		ಒಳ_string = flags & FSK_ಒಳ_STRING;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ಬಿಟ್;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    uvarnumber_T	n;
    ಪೂರ್ಣಾಂಕ		l;

    src = *srcp;
    ಆದರೆ (src[0] != '<')
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    ಕೊನೆಯ_dash = src;
    ಗಾಗಿ (bp = src + 1; *bp == '-' || vim_is_ಸಾಮಾನ್ಯ_IDc(*bp); bp++)
    {
	ಆದರೆ (*bp == '-')
	{
	    ಕೊನೆಯ_dash = bp;
	    ಆದರೆ (bp[1] != NUL)
	    {
		ಆದರೆ (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		ಇಲ್ಲದಿದ್ದರೆ
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		ಆದರೆ (!(ಒಳ_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	ಆದರೆ (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, ಖಾಲಿ, &l, STR2NR_ALL, ಖಾಲಿ, ಖಾಲಿ, 0, TRUE, ಖಾಲಿ);
	    ಆದರೆ (l == 0)
	    {
		emsg(_(e_invalid_ವಾದ));
		ಹಿಂತಿರುಗಿ 0;
	    }
	    bp += l + 5;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (*bp == '>')	// found matching '>'
    {
	ಕೊನೆ_ದ_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	ಗಾಗಿ (bp = src + 1; bp < ಕೊನೆಯ_dash; bp++)
	{
	    ಆದರೆ (*bp != '-')
	    {
		ಬಿಟ್ = name_ಗಮ್ಯ_mod_ಮುಖವಾಡ(*bp);
		ಆದರೆ (ಬಿಟ್ == 0x0)
		    ನಿಲ್ಲಿಸು;	// Illegal modifier name
		modifiers |= ಬಿಟ್;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	ಆದರೆ (bp >= ಕೊನೆಯ_dash)
	{
	    ಆದರೆ (STRNICMP(ಕೊನೆಯ_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(ಕೊನೆಯ_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(ಕೊನೆಯ_dash + 6, ಖಾಲಿ, &l, STR2NR_ALL, ಖಾಲಿ,
							    &n, 0, TRUE, ಖಾಲಿ);
		ಆದರೆ (l == 0)
		{
		    emsg(_(e_invalid_ವಾದ));
		    ಹಿಂತಿರುಗಿ 0;
		}
		ಕೀಲಿ = (ಪೂರ್ಣಾಂಕ)n;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ off = 1;

		// Modifier with single letter, or special key name.
		ಆದರೆ (ಒಳ_string && ಕೊನೆಯ_dash[1] == '\\' && ಕೊನೆಯ_dash[2] == '"')
		    off = 2;
		ಆದರೆ (has_mbyte)
		    l = mb_ptr2len(ಕೊನೆಯ_dash + off);
		ಇಲ್ಲದಿದ್ದರೆ
		    l = 1;
		ಆದರೆ (modifiers != 0 && ಕೊನೆಯ_dash[l + off] == '>')
		    ಕೀಲಿ = PTR2CHAR(ಕೊನೆಯ_dash + off);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಕೀಲಿ = get_special_ಕೀಲಿ_ಸಂಕೇತ(ಕೊನೆಯ_dash + off);
		    ಆದರೆ (!(flags & FSK_KEEP_X_ಕೀಲಿ))
			ಕೀಲಿ = handle_x_ಕೀಲಿಗಳು(ಕೀಲಿ);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    ಆದರೆ (ಕೀಲಿ != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);

		ಆದರೆ ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    ಆದರೆ (ಕೀಲಿ == K_BS)
			ಕೀಲಿ = BS;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ == K_DEL || ಕೀಲಿ == K_KDEL)
			ಕೀಲಿ = DEL;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    ಆದರೆ ((flags & FSK_SIMPLIFY) != 0)
		    {
			ಆದರೆ (did_simplify != ಖಾಲಿ)
			    *did_simplify = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಕೀಲಿ = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		ಆದರೆ (!IS_SPECIAL(ಕೀಲಿ))
		    ಕೀಲಿ = ಹೊರತೆಗೆ_modifiers(ಕೀಲಿ, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = ಕೊನೆ_ದ_name;
		ಹಿಂತಿರುಗಿ ಕೀಲಿ;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}


/*
 * Some keys are used with Ctrl without Shift and are still expected to be
 * mapped as if Shift was pressed:
 * CTRL-2 is CTRL-@
 * CTRL-6 is CTRL-^
 * CTRL-- is CTRL-_
 * Also, unless no_reduce_keys is set then <C-H> and <C-h> mean the same thing,
 * use "H".
 * Returns the possibly adjusted key.
 */
    ಪೂರ್ಣಾಂಕ
may_adjust_ಕೀಲಿ_for_ctrl(ಪೂರ್ಣಾಂಕ modifiers, ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
    ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_CTRL) == 0)
	ಹಿಂತಿರುಗಿ ಕೀಲಿ;

    ಆದರೆ (ASCII_ISALPHA(ಕೀಲಿ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	check_no_reduce_ಕೀಲಿಗಳು();  // may update the no_reduce_keys flag
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ no_reduce_ಕೀಲಿಗಳು == 0 ? TOUPPER_ASC(ಕೀಲಿ) : ಕೀಲಿ;
    }
    ಆದರೆ (ಕೀಲಿ == '2')
	ಹಿಂತಿರುಗಿ '@';
    ಆದರೆ (ಕೀಲಿ == '6')
	ಹಿಂತಿರುಗಿ '^';
    ಆದರೆ (ಕೀಲಿ == '-')
	ಹಿಂತಿರುಗಿ '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    ಆದರೆ (ಕೀಲಿ == '$' && (modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) == 0)
	ಹಿಂತಿರುಗಿ ']';

    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    ಪೂರ್ಣಾಂಕ
may_remove_ಸರಿಸು_modifier(ಪೂರ್ಣಾಂಕ modifiers, ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
    ಆದರೆ ((modifiers == MOD_ಮುಖವಾಡ_ಸರಿಸು
		|| modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_ALT)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
		|| modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CMD)
#ಆದರೆ_ಅಂತ್ಯ
		|| modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_META))
	    && ((ಕೀಲಿ >= '!' && ಕೀಲಿ <= '/')
		|| (ಕೀಲಿ >= ':' && ಕೀಲಿ <= 'Z')
		|| vim_isdigit(ಕೀಲಿ)
		|| (ಕೀಲಿ >= '[' && ಕೀಲಿ <= '`')
		|| (ಕೀಲಿ >= '{' && ಕೀಲಿ <= '~')))
	ಹಿಂತಿರುಗಿ modifiers & ~MOD_ಮುಖವಾಡ_ಸರಿಸು;

    ಆದರೆ (modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL)
		&& (ಕೀಲಿ == '{' || ಕೀಲಿ == '}' || ಕೀಲಿ == '|'))
	ಹಿಂತಿರುಗಿ modifiers & ~MOD_ಮುಖವಾಡ_ಸರಿಸು;

    ಹಿಂತಿರುಗಿ modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊರತೆಗೆ_modifiers(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, ಪೂರ್ಣಾಂಕ *modp, ಪೂರ್ಣಾಂಕ simplify, ಪೂರ್ಣಾಂಕ *did_simplify)
{
    ಪೂರ್ಣಾಂಕ	modifiers = *modp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
    // Command-key really special, no fancynest
    ಆದರೆ (!(modifiers & MOD_ಮುಖವಾಡ_CMD))
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) && ASCII_ISALPHA(ಕೀಲಿ))
    {
	ಕೀಲಿ = TOUPPER_ASC(ಕೀಲಿ);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	ಆದರೆ (simplify || modifiers == MOD_ಮುಖವಾಡ_ಸರಿಸು
		|| modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_ALT)
		|| modifiers == (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_META))
	    modifiers &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_CTRL) && ASCII_ISALPHA(ಕೀಲಿ))
	ಕೀಲಿ = TOUPPER_ASC(ಕೀಲಿ);

    ಆದರೆ (simplify && (modifiers & MOD_ಮುಖವಾಡ_CTRL)
	    && ((ಕೀಲಿ >= '?' && ಕೀಲಿ <= '_') || ASCII_ISALPHA(ಕೀಲಿ)))
    {
	ಕೀಲಿ = Ctrl_chr(ಕೀಲಿ);
	modifiers &= ~MOD_ಮುಖವಾಡ_CTRL;
	// <C-@> is <Nul>
	ಆದರೆ (ಕೀಲಿ == NUL)
	    ಕೀಲಿ = K_ZERO;
	ಆದರೆ (did_simplify != ಖಾಲಿ)
	    *did_simplify = TRUE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
    // Command-key really special, no fancynest
    ಆದರೆ (!(modifiers & MOD_ಮುಖವಾಡ_CMD))
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (simplify && (modifiers & MOD_ಮುಖವಾಡ_ALT) && ಕೀಲಿ < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	ಕೀಲಿ |= 0x80;
	modifiers &= ~MOD_ಮುಖವಾಡ_ALT;	// remove the META modifier
	ಆದರೆ (did_simplify != ಖಾಲಿ)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_special_ಕೀಲಿ_ಒಳ_ಕೋಷ್ಟಕ(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ); i++)
	ಆದರೆ (c == ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[i].ಕೀಲಿ && !ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[i].is_alt)
	    ಹಿಂತಿರುಗಿ ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[i].enabled ? i : -1;

    ಹಿಂತಿರುಗಿ -1;
}


/*
 * Compare two 'struct key_name_entry' structures.
 * Note that the target string (p1) may contain additional trailing characters
 * that should not factor into the comparison. Example:
 * 'LeftMouse>", "<LeftMouse>"] ...'
 * should match with
 * 'LeftMouse'.
 * These characters are identified by vim_isNormalIDc().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cmp_ಕೀಲಿ_name_ನಮೂದು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    char_u  *p1 = ((ರಚನೆ ಕೀಲಿ_name_ನಮೂದು *)a)->name.string;
    char_u  *p2 = ((ರಚನೆ ಕೀಲಿ_name_ನಮೂದು *)b)->name.string;
    ಪೂರ್ಣಾಂಕ	    ಫಲಿತಾಂಶ = 0;

    ಆದರೆ (p1 == p2)
	ಹಿಂತಿರುಗಿ 0;

    ಆಗ (vim_is_ಸಾಮಾನ್ಯ_IDc(*p1) && *p2 != NUL)
    {
	ಆದರೆ ((ಫಲಿತಾಂಶ = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    ನಿಲ್ಲಿಸು;
	++p1;
	++p2;
    }

    ಆದರೆ (ಫಲಿತಾಂಶ == 0)
    {
	ಆದರೆ (*p2 == NUL)
	{
	    ಆದರೆ (vim_is_ಸಾಮಾನ್ಯ_IDc(*p1))
		ಫಲಿತಾಂಶ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಫಲಿತಾಂಶ = -1;
	}
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    ಪೂರ್ಣಾಂಕ
get_special_ಕೀಲಿ_ಸಂಕೇತ(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    ಆದರೆ (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	ಆದರೆ (ಕೂಡಿಸು_termcap_ನಮೂದು(string, FALSE) == OK)
	    ಹಿಂತಿರುಗಿ TERMCAP2_ಕೀಲಿ(name[2], name[3]);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ರಚನೆ ಕೀಲಿ_name_ನಮೂದು	ಗುರಿ;
	ರಚನೆ ಕೀಲಿ_name_ನಮೂದು	*ನಮೂದು;

	ಗುರಿ.enabled = TRUE;
	ಗುರಿ.ಕೀಲಿ = 0;
	ಗುರಿ.name.string = name;
	ಗುರಿ.name.ಉದ್ದಳತೆ = 0;

	ನಮೂದು = (ರಚನೆ ಕೀಲಿ_name_ನಮೂದು *)bsearch(
	    &ಗುರಿ,
	    &ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ,
	    ARRAY_ಉದ್ದಳತೆ(ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ),
	    ಗಾತ್ರ(ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[0]),
	    cmp_ಕೀಲಿ_name_ನಮೂದು);
	ಆದರೆ (ನಮೂದು != ಖಾಲಿ && ನಮೂದು->enabled)
	{
	    ಪೂರ್ಣಾಂಕ ಕೀಲಿ = ನಮೂದು->ಕೀಲಿ;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    ಹಿಂತಿರುಗಿ ಕೀಲಿ == K_TAB ? TAB : ಕೀಲಿ;
	}
    }

    ಹಿಂತಿರುಗಿ 0;
}

    char_u *
get_ಕೀಲಿ_name(ಪೂರ್ಣಾಂಕ i)
{
    ಆದರೆ (i < 0 || i >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ ಕೀಲಿ_ಹೆಸರುಗಳು_ಕೋಷ್ಟಕ[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    ಪೂರ್ಣಾಂಕ
get_fileformat(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		c = *buf->b_p_ff;

    ಆದರೆ (buf->b_p_bin || c == 'u')
	ಹಿಂತಿರುಗಿ EOL_UNIX;
    ಆದರೆ (c == 'm')
	ಹಿಂತಿರುಗಿ EOL_MAC;
    ಹಿಂತಿರುಗಿ EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    ಪೂರ್ಣಾಂಕ
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (eap != ಖಾಲಿ && eap->force_ff != 0)
	c = eap->force_ff;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ ((eap != ಖಾಲಿ && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    ಹಿಂತಿರುಗಿ EOL_UNIX;
	c = *buf->b_p_ff;
    }
    ಆದರೆ (c == 'u')
	ಹಿಂತಿರುಗಿ EOL_UNIX;
    ಆದರೆ (c == 'm')
	ಹಿಂತಿರುಗಿ EOL_MAC;
    ಹಿಂತಿರುಗಿ EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    ಶೂನ್ಯ
ಗಣ_fileformat(
    ಪೂರ್ಣಾಂಕ		t,
    ಪೂರ್ಣಾಂಕ		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    ಅಕ್ಷರ	*p = ಖಾಲಿ;

    ಆಯ್ಕೆ (t)
    {
    ಸ್ಥಿತಿ EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (p != ಖಾಲಿ)
	ಗಣ_string_option_ನೇರ((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_ಸ್ಥಿತಿ(curbuf);
    redraw_tabline = TRUE;
#ಆದರೆ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    ಪೂರ್ಣಾಂಕ
default_fileformat(ಶೂನ್ಯ)
{
    ಆಯ್ಕೆ (*p_ffs)
    {
	ಸ್ಥಿತಿ 'm':   ಹಿಂತಿರುಗಿ EOL_MAC;
	ಸ್ಥಿತಿ 'd':   ಹಿಂತಿರುಗಿ EOL_DOS;
    }
    ಹಿಂತಿರುಗಿ EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    ಪೂರ್ಣಾಂಕ
call_shell(char_u *cmd, ಪೂರ್ಣಾಂಕ opt)
{
    ಪೂರ್ಣಾಂಕ		retval;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    proftime_T	wait_ಸಮಯ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == ಖಾಲಿ ? p_sh : cmd);
	ಸಂದೇಶ_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_enter(&wait_ಸಮಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_ಖಾಲಿಗಳು));
	retval = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	++hold_gui_events;
#ಆದರೆ_ಅಂತ್ಯ
	// The external command may update a tags file, clear cached tags.
	ಟ್ಯಾಗ್_freematch();

	ಆದರೆ (cmd == ಖಾಲಿ || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    ಆದರೆ (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		ಆದರೆ (ecmd == ಖಾಲಿ)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    ಆದರೆ (ncmd != ಖಾಲಿ)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((ಅಕ್ಷರ *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		retval = -1;
	    ಆದರೆ (ecmd != cmd)
		vim_free(ecmd);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	--hold_gui_events;
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_nr(VV_SHELL_ERROR, (ದೀರ್ಘ)retval);
# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_exit(&wait_ಸಮಯ);
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    ಪೂರ್ಣಾಂಕ
get_real_state(ಶೂನ್ಯ)
{
    ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
    {
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (VIsual_select)
		ಹಿಂತಿರುಗಿ ಬಹುಲಕ_SELECT;
	    ಹಿಂತಿರುಗಿ ಬಹುಲಕ_VISUAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಕ್ತಾಯ_op)
	    ಹಿಂತಿರುಗಿ ಬಹುಲಕ_OP_PENDING;
    }
    ಹಿಂತಿರುಗಿ State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    ಪೂರ್ಣಾಂಕ
after_pathsep(char_u *b, char_u *p)
{
    ಹಿಂತಿರುಗಿ p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_ತಲೆ_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    ಪೂರ್ಣಾಂಕ
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    ಆದರೆ (f1 == ಖಾಲಿ || f2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    (ಶೂನ್ಯ)vim_ತುಂಬಿದ_Name(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    ಹಿಂತಿರುಗಿ (t1 - ffname == t2 - f2
	     && pathcmp((ಅಕ್ಷರ *)ffname, (ಅಕ್ಷರ *)f2, (ಪೂರ್ಣಾಂಕ)(t1 - ffname)) == 0);
}

#ಆದರೆ defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
vim_chdirfile(char_u *fname, ಅಕ್ಷರ *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    ಆದರೆ (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    ಆದರೆ (pathcmp((ಅಕ್ಷರ *)old_dir, (ಅಕ್ಷರ *)new_dir, -1) == 0)
	// nothing to do
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (trigger_autocmd != ಖಾಲಿ)
	trigger_Dir_ಬದಲಾಯಿತು_Pre((char_u *)trigger_autocmd, new_dir);

    ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)new_dir) != 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (trigger_autocmd != ಖಾಲಿ)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
illegal_slash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    ಆದರೆ (name[0] == NUL)
	ಹಿಂತಿರುಗಿ FALSE;	    // no file name is not illegal
    ಆದರೆ (name[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) - 1] != '/')
	ಹಿಂತಿರುಗಿ FALSE;	    // no trailing slash
    ಆದರೆ (mch_isdir((char_u *)name))
	ಹಿಂತಿರುಗಿ FALSE;	    // trailing slash for a directory
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    ಪೂರ್ಣಾಂಕ
vim_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    ಹಿಂತಿರುಗಿ illegal_slash(name) ? -1 : stat(name, stp);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_ಕೋಷ್ಟಕ[SHAPE_IDX_ಎಣಿಕೆ] =
{
    // The values will be filled in from the 'guicursor' and 'mouseshape'
    // defaults when Vim starts.
    // Adjust the SHAPE_IDX_ defines when making changes!
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "n", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "v", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "i", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "r", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "c", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ci", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "cr", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "o", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ve", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "e", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "s", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "sd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vs", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "m", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "ml", SHAPE_MOUSE},
    {0,	0, 0, 100L, 100L, 100L, 0, 0, "sm", SHAPE_CURSOR},
};

# ifdef FEAT_MOUSESHAPE
/*
 * Table with names for mouse shapes.  Keep in sync with all the tables for
 * mch_set_mouse_shape()!.
 */
ಸ್ಥಿರ string_T mshape_ಹೆಸರುಗಳು[] =
{
    STR_LITERAL_INIT("arrow"),	// default, must be the first one
    STR_LITERAL_INIT("blank"),	// hidden
    STR_LITERAL_INIT("beam"),
    STR_LITERAL_INIT("updown"),
    STR_LITERAL_INIT("udsizing"),
    STR_LITERAL_INIT("leftright"),
    STR_LITERAL_INIT("lrsizing"),
    STR_LITERAL_INIT("busy"),
    STR_LITERAL_INIT("no"),
    STR_LITERAL_INIT("crosshair"),
    STR_LITERAL_INIT("hand1"),
    STR_LITERAL_INIT("hand2"),
    STR_LITERAL_INIT("pencil"),
    STR_LITERAL_INIT("question"),
    STR_LITERAL_INIT("rightup-arrow"),
    STR_LITERAL_INIT("up-arrow"),
    {ಖಾಲಿ, 0}
};

#  define MSHAPE_ಹೆಸರುಗಳು_ಎಣಿಕೆ  (ARRAY_ಉದ್ದಳತೆ(mshape_ಹೆಸರುಗಳು) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    ಅಕ್ಷರ *
parse_shape_opt(ಪೂರ್ಣಾಂಕ what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    ಪೂರ್ಣಾಂಕ		idx = 0;		// init for GCC
    ಪೂರ್ಣಾಂಕ		all_idx;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	n;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ve = FALSE;	// found "ve" flag
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    /*
     * First round: check for errors; second round: do it for real.
     */
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	ಆದರೆ (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    modep = p_guicursor;
	ಆಗ (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    ಆದರೆ (colonp == ಖಾಲಿ || (commap != ಖಾಲಿ && commap < colonp))
		ಹಿಂತಿರುಗಿ e_missing_colon_2;
	    ಆದರೆ (colonp == modep)
		ಹಿಂತಿರುಗಿ e_illegal_ಬಹುಲಕ;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    ಆಗ (modep < colonp || all_idx >= 0)
	    {
		ಆದರೆ (all_idx < 0)
		{
		    // Find the mode.
		    ಆದರೆ (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			len = 2;
		    ಆದರೆ (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_ಎಣಿಕೆ - 1;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಗಾಗಿ (idx = 0; idx < SHAPE_IDX_ಎಣಿಕೆ; ++idx)
			    ಆದರೆ (STRNICMP(modep, shape_ಕೋಷ್ಟಕ[idx].name, len)
									 == 0)
				ನಿಲ್ಲಿಸು;
			ಆದರೆ (idx == SHAPE_IDX_ಎಣಿಕೆ
				   || (shape_ಕೋಷ್ಟಕ[idx].ಬಳಸಿದ_for & what) == 0)
			    ಹಿಂತಿರುಗಿ e_illegal_ಬಹುಲಕ;
			ಆದರೆ (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    ಸಿಕ್ಕಿತು_ve = TRUE;
		    }
		    modep += len + 1;
		}

		ಆದರೆ (all_idx >= 0)
		    idx = all_idx--;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುತ್ತು == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    ಆದರೆ (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_ಕೋಷ್ಟಕ[idx].mshape = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
# endif
		    {
			// Set the defaults, for the missing parts
			shape_ಕೋಷ್ಟಕ[idx].shape = SHAPE_ಖಂಡ;
			shape_ಕೋಷ್ಟಕ[idx].blinkwait = 700L;
			shape_ಕೋಷ್ಟಕ[idx].blinkon = 400L;
			shape_ಕೋಷ್ಟಕ[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		ಗಾಗಿ (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    ಆದರೆ (what == SHAPE_MOUSE)
		    {
			ಗಾಗಿ (i = 0; ; ++i)
			{
			    ಆದರೆ (mshape_ಹೆಸರುಗಳು[i].string == ಖಾಲಿ)
			    {
				ಆದರೆ (!VIM_ISDIGIT(*p))
				    ಹಿಂತಿರುಗಿ e_illegal_mouseshape;
				ಆದರೆ (ಸುತ್ತು == 2)
				    shape_ಕೋಷ್ಟಕ[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				ಇಲ್ಲದಿದ್ದರೆ
				    (ಶೂನ್ಯ)getdigits(&p);
				ನಿಲ್ಲಿಸು;
			    }
			    ಆದರೆ (STRNICMP(p, mshape_ಹೆಸರುಗಳು[i].string, mshape_ಹೆಸರುಗಳು[i].ಉದ್ದಳತೆ) == 0)
			    {
				ಆದರೆ (ಸುತ್ತು == 2)
				    shape_ಕೋಷ್ಟಕ[idx].mshape = i;
				p += mshape_ಹೆಸರುಗಳು[i].ಉದ್ದಳತೆ;
				ನಿಲ್ಲಿಸು;
			    }
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			ಆದರೆ (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			ಆದರೆ (len != 0)
			{
			    p += len;
			    ಆದರೆ (!VIM_ISDIGIT(*p))
				ಹಿಂತಿರುಗಿ e_digit_expected;
			    n = getdigits(&p);
			    ಆದರೆ (len == 3)   // "ver" or "hor"
			    {
				ಆದರೆ (n == 0)
				    ಹಿಂತಿರುಗಿ e_illegal_percentage;
				ಆದರೆ (ಸುತ್ತು == 2)
				{
				    ಆದರೆ (TOLOWER_ASC(i) == 'v')
					shape_ಕೋಷ್ಟಕ[idx].shape = SHAPE_VER;
				    ಇಲ್ಲದಿದ್ದರೆ
					shape_ಕೋಷ್ಟಕ[idx].shape = SHAPE_HOR;
				    shape_ಕೋಷ್ಟಕ[idx].percentage = n;
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುತ್ತು == 2)
			    {
				ಆದರೆ (len == 9)
				    shape_ಕೋಷ್ಟಕ[idx].blinkwait = n;
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 7)
				    shape_ಕೋಷ್ಟಕ[idx].blinkon = n;
				ಇಲ್ಲದಿದ್ದರೆ
				    shape_ಕೋಷ್ಟಕ[idx].blinkoff = n;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "block", 5) == 0)
			{
			    ಆದರೆ (ಸುತ್ತು == 2)
				shape_ಕೋಷ್ಟಕ[idx].shape = SHAPE_ಖಂಡ;
			    p += 5;
			}
			ಇಲ್ಲದಿದ್ದರೆ	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    ಆದರೆ (commap == ಖಾಲಿ)		    // last part
			    {
				ಆದರೆ (endp == ಖಾಲಿ)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (endp > commap || endp == ಖಾಲಿ)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    ಆದರೆ (slashp != ಖಾಲಿ && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_ಗುಂಪು(p, (ಪೂರ್ಣಾಂಕ)(slashp - p));
				p = slashp + 1;
			    }
			    ಆದರೆ (ಸುತ್ತು == 2)
			    {
				shape_ಕೋಷ್ಟಕ[idx].id = syn_check_ಗುಂಪು(p,
							     (ಪೂರ್ಣಾಂಕ)(endp - p));
				shape_ಕೋಷ್ಟಕ[idx].id_lm = shape_ಕೋಷ್ಟಕ[idx].id;
				ಆದರೆ (slashp != ಖಾಲಿ && slashp < endp)
				    shape_ಕೋಷ್ಟಕ[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    ಆದರೆ (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    ಆದರೆ (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    ಆದರೆ (!ಸಿಕ್ಕಿತು_ve)
    {
# ifdef FEAT_MOUSESHAPE
	ಆದರೆ (what == SHAPE_MOUSE)
	{
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].mshape = shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].mshape;
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].shape = shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].shape;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].percentage =
					 shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].percentage;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].blinkwait =
					  shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].blinkwait;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].blinkon =
					    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].blinkon;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].blinkoff =
					   shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].blinkoff;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].id = shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].id;
	    shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VE].id_lm = shape_ಕೋಷ್ಟಕ[SHAPE_IDX_V].id_lm;
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

# ಆದರೆ defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    ಪೂರ್ಣಾಂಕ
get_shape_idx(ಪೂರ್ಣಾಂಕ mouse)
{
#  ifdef FEAT_MOUSESHAPE
    ಆದರೆ (mouse && (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE))
    {
#   ifdef FEAT_GUI
	ಪೂರ್ಣಾಂಕ x, y;
	gui_mch_getmouse(&x, &y);
	ಆದರೆ (Y_2_ಅಡ್ಡಸಾಲು(y) == ಅಡ್ಡಸಾಲುಗಳು - 1)
	    ಹಿಂತಿರುಗಿ SHAPE_IDX_MOREL;
#   endif
	ಹಿಂತಿರುಗಿ SHAPE_IDX_ಇನ್ನಷ್ಟು;
    }
    ಆದರೆ (mouse && drag_ಸ್ಥಿತಿ_ಸಾಲು)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_SDRAG;
    ಆದರೆ (mouse && drag_sep_ಸಾಲು)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_VDRAG;
#  endif
    ಆದರೆ (!mouse && State == ಬಹುಲಕ_SHOWMATCH)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_SM;
    ಆದರೆ (State & VREPLACE_FLAG)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_R;
    ಆದರೆ (State & REPLACE_FLAG)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_R;
    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_I;
    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
    {
	ಆದರೆ (cmdline_at_ಕೊನೆ())
	    ಹಿಂತಿರುಗಿ SHAPE_IDX_C;
	ಆದರೆ (cmdline_overstrike())
	    ಹಿಂತಿರುಗಿ SHAPE_IDX_CR;
	ಹಿಂತಿರುಗಿ SHAPE_IDX_CI;
    }
    ಆದರೆ (ಮುಕ್ತಾಯ_op)
	ಹಿಂತಿರುಗಿ SHAPE_IDX_O;
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (*p_sel == 'e')
	    ಹಿಂತಿರುಗಿ SHAPE_IDX_VE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ SHAPE_IDX_V;
    }
    ಹಿಂತಿರುಗಿ SHAPE_IDX_N;
}
# endif

# ಆದರೆ defined(FEAT_MOUSESHAPE)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    ಶೂನ್ಯ
update_mouseshape(ಪೂರ್ಣಾಂಕ shape_idx)
{
    ಪೂರ್ಣಾಂಕ new_mouse_shape;

    // Only works in GUI mode.
    ಆದರೆ (!gui.ಒಳ_use || gui.starting)
	ಹಿಂತಿರುಗಿ;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    ಆದರೆ (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	ಹಿಂತಿರುಗಿ;
    }

    // When ignoring the mouse don't change shape on the statusline.
    ಆದರೆ (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_ಸ್ಥಿತಿ
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    ಆದರೆ (shape_idx == -2
	    && current_mouse_shape != shape_ಕೋಷ್ಟಕ[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_ಕೋಷ್ಟಕ[SHAPE_IDX_ಸ್ಥಿತಿ].mshape
	    && current_mouse_shape != shape_ಕೋಷ್ಟಕ[SHAPE_IDX_VSEP].mshape)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (shape_idx < 0)
	new_mouse_shape = shape_ಕೋಷ್ಟಕ[get_shape_idx(TRUE)].mshape;
    ಇಲ್ಲದಿದ್ದರೆ
	new_mouse_shape = shape_ಕೋಷ್ಟಕ[shape_idx].mshape;
    ಆದರೆ (new_mouse_shape != current_mouse_shape)
    {
	mch_ಗಣ_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#ಆದರೆ_ಅಂತ್ಯ // CURSOR_SHAPE

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    ಶೂನ್ಯ
f_getmouseshape(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
# ಆದರೆ defined(FEAT_MOUSESHAPE)
    ಆದರೆ (current_mouse_shape >= 0
			      && current_mouse_shape < (ಪೂರ್ಣಾಂಕ)MSHAPE_ಹೆಸರುಗಳು_ಎಣಿಕೆ)
	rettv->vval.v_string = vim_strnsave(
				  mshape_ಹೆಸರುಗಳು[current_mouse_shape].string,
				  mshape_ಹೆಸರುಗಳು[current_mouse_shape].ಉದ್ದಳತೆ);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    ಪೂರ್ಣಾಂಕ
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    ಪೂರ್ಣಾಂಕ		r;
    char_u	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
    ಅಕ್ಷರ	*ಹುಡುಕು_ctx = ಖಾಲಿ;

    dir_name = ಕಂಡುಹಿಡಿ_directory_ಒಳ_ಮಾರ್ಗ(new_dir, (ಪೂರ್ಣಾಂಕ)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
    vim_free(file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
    vim_findfile_cleanup(ಹುಡುಕು_ctx);
    ಆದರೆ (dir_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    r = mch_chdir((ಅಕ್ಷರ *)dir_name);
    vim_free(dir_name);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_user_name(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ಆದರೆ (username == ಖಾಲಿ)
    {
	ಆದರೆ (mch_get_user_name(buf, len) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	username = vim_strsave(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_strncpy(buf, username, len - 1);
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    ಶೂನ್ಯ
free_username(ಶೂನ್ಯ)
{
    vim_free(username);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    ಶೂನ್ಯ
qsort(
    ಶೂನ್ಯ	*ತಳ,
    size_t	elm_ಎಣಿಕೆ,
    size_t	elm_ಗಾತ್ರ,
    ಪೂರ್ಣಾಂಕ (*cmp)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		gap;

    buf = alloc(elm_ಗಾತ್ರ);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (gap = elm_ಎಣಿಕೆ / 2; gap > 0; gap /= 2)
	ಗಾಗಿ (i = gap; i < elm_ಎಣಿಕೆ; ++i)
	    ಗಾಗಿ (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)ತಳ + j * elm_ಗಾತ್ರ;
		p2 = (char_u *)ತಳ + (j + gap) * elm_ಗಾತ್ರ;
		ಆದರೆ ((*cmp)((ಶೂನ್ಯ *)p1, (ಶೂನ್ಯ *)p2) <= 0)
		    ನಿಲ್ಲಿಸು;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_ಗಾತ್ರ);
		mch_memmove(p1, p2, elm_ಗಾತ್ರ);
		mch_memmove(p2, buf, elm_ಗಾತ್ರ);
	    }

    vim_free(buf);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The putenv() implementation below comes from the "screen" program.
 * Included with permission from Juergen Weigert.
 * See pty.c for the copyright notice.
 */

/*
 *  putenv  --	put value into environment
 *
 *  Usage:  i = putenv (string)
 *    int i;
 *    char  *string;
 *
 *  where string is of the form <name>=<value>.
 *  Putenv returns 0 normally, -1 on error (not enough core for malloc).
 *
 *  Putenv may need to add a new name into the environment, or to
 *  associate a value longer than the current value with a particular
 *  name.  So, to make life simpler, putenv() copies your entire
 *  environment into the heap (i.e. malloc()) from the stack
 *  (i.e. where it resides when your process is initiated) the first
 *  time you call it.
 *
 *  (history removed, not very interesting.  See the "screen" sources.)
 */

#ಆದರೆ !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  envsize = -1;	// current size of environment
ಬಾಹ್ಯ ಅಕ್ಷರ **environ;		// the global which is your env.

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  findenv(ಅಕ್ಷರ *name); // look for a name in the env.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  newenv(ಶೂನ್ಯ);	// copy env. from stack to heap
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  moreenv(ಶೂನ್ಯ);	// incr. size of env.

    ಪೂರ್ಣಾಂಕ
putenv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಅಕ್ಷರ    *p;

    ಆದರೆ (envsize < 0)
    {				// first time putenv called
	ಆದರೆ (newenv() < 0)	// copy env. to heap
	    ಹಿಂತಿರುಗಿ -1;
    }

    i = findenv((ಅಕ್ಷರ *)string); // look for name in environment

    ಆದರೆ (i < 0)
    {				// name must be added
	ಗಾಗಿ (i = 0; environ[i]; i++);
	ಆದರೆ (i >= (envsize - 1))
	{			// need new slot
	    ಆದರೆ (moreenv() < 0)
		ಹಿಂತಿರುಗಿ -1;
	}
	p = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string) + 1);
	ಆದರೆ (p == ಖಾಲಿ)		// not enough core
	    ಹಿಂತಿರುಗಿ -1;
	environ[i + 1] = 0;	// new end of env.
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {				// name already in env.
	p = vim_realloc(environ[i], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string) + 1);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findenv(ಅಕ್ಷರ *name)
{
    ಅಕ್ಷರ    *namechar, *envchar;
    ಪೂರ್ಣಾಂಕ	    i, ಸಿಕ್ಕಿತು;

    ಸಿಕ್ಕಿತು = 0;
    ಗಾಗಿ (i = 0; environ[i] && !ಸಿಕ್ಕಿತು; i++)
    {
	envchar = environ[i];
	namechar = name;
	ಆಗ (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	ಸಿಕ್ಕಿತು = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು ? i - 1 : -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
newenv(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ    **env, *elem;
    ಪೂರ್ಣಾಂಕ	    i, esize;

    ಗಾಗಿ (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(ಅಕ್ಷರ *, esize);
    ಆದರೆ (env == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಗಾಗಿ (i = 0; environ[i]; i++)
    {
	elem = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(environ[i]) + 1);
	ಆದರೆ (elem == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
moreenv(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    esize;
    ಅಕ್ಷರ    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((ಅಕ್ಷರ *)environ, esize * ಗಾತ್ರ (*env));
    ಆದರೆ (env == 0)
	ಹಿಂತಿರುಗಿ -1;
    environ = env;
    envsize = esize;
    ಹಿಂತಿರುಗಿ 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(ಸ್ಥಿರಾಂಕ char_u *string)
{
    ಪೂರ್ಣಾಂಕ i;
    char_u *p;

    ಆದರೆ (envsize < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    i = findenv((ಅಕ್ಷರ *)string);

    ಆದರೆ (i < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = vim_strchr((char_u *)environ[i], '=');
    ಹಿಂತಿರುಗಿ (p + 1);
}
# endif

#ಆದರೆ_ಅಂತ್ಯ // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    ಪೂರ್ಣಾಂಕ
filewritable(char_u *fname)
{
    ಪೂರ್ಣಾಂಕ		retval = 0;
# ಆದರೆ defined(UNIX) || defined(VMS)
    ಪೂರ್ಣಾಂಕ		perm = 0;
# endif

# ಆದರೆ defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    ಆದರೆ (
# ifdef MSWIN
	    mch_writable(fname) &&
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((ಅಕ್ಷರ *)fname, W_OK) == 0
       )
    {
	++retval;
	ಆದರೆ (mch_isdir(fname))
	    ++retval;
    }
    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಂಕ
get2c(FILE *fd)
{
    ಪೂರ್ಣಾಂಕ		c, n;

    n = getc(fd);
    ಆದರೆ (n == EOF) ಹಿಂತಿರುಗಿ -1;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಂಕ
get3c(FILE *fd)
{
    ಪೂರ್ಣಾಂಕ		c, n;

    n = getc(fd);
    ಆದರೆ (n == EOF) ಹಿಂತಿರುಗಿ -1;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    n = (n << 8) + c;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    ಪೂರ್ಣಾಂಕ
get4c(FILE *fd)
{
    ಪೂರ್ಣಾಂಕ		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    ಚಿಹ್ನೆರಹಿತ	n;

    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    n = (ಚಿಹ್ನೆರಹಿತ)c;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    n = (n << 8) + (ಚಿಹ್ನೆರಹಿತ)c;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    n = (n << 8) + (ಚಿಹ್ನೆರಹಿತ)c;
    c = getc(fd);
    ಆದರೆ (c == EOF) ಹಿಂತಿರುಗಿ -1;
    n = (n << 8) + (ಚಿಹ್ನೆರಹಿತ)c;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, ಪೂರ್ಣಾಂಕ cnt)
{
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    // allocate memory
    str = alloc(cnt + 1);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Read the string.  Quit when running into the EOF.
    ಗಾಗಿ (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	ಆದರೆ (c == EOF)
	{
	    vim_free(str);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	str[i] = c;
    }
    str[i] = NUL;
    ಹಿಂತಿರುಗಿ str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    ಪೂರ್ಣಾಂಕ
put_ಬೈಟುಗಳು(FILE *fd, long_u nr, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = len - 1; i >= 0; --i)
	ಆದರೆ (putc((ಪೂರ್ಣಾಂಕ)(nr >> (i * 8)), fd) == EOF)
	    ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    ದೀರ್ಘ
elapsed(ರಚನೆ timeval *ಆರಂಭ_tv)
{
    ರಚನೆ timeval  now_tv;

    gettimeofday(&now_tv, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ (now_tv.tv_sec - ಆರಂಭ_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - ಆರಂಭ_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    ದೀರ್ಘ
elapsed(DWORD ಆರಂಭ_tick)
{
    DWORD	now = Get_Tick_ಎಣಿಕೆ();

    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)now - (ದೀರ್ಘ)ಆರಂಭ_tick;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
mch_parse_cmd(char_u *cmd, ಪೂರ್ಣಾಂಕ use_shcf, ಅಕ್ಷರ ***argv, ಪೂರ್ಣಾಂಕ *argc)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p, *d;
    ಪೂರ್ಣಾಂಕ		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    ಗಾಗಿ (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (i == 2)
		(*argv)[*argc] = (ಅಕ್ಷರ *)p;
	    ++*argc;
	    d = p;
	    ಆಗ (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		ಆದರೆ (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    ಆದರೆ (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    ಆದರೆ (*p == NUL)
	    {
		ಆದರೆ (i == 2)
		    *d++ = NUL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	ಆದರೆ (*argv == ಖಾಲಿ)
	{
	    ಆದರೆ (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		ಗಾಗಿ (;;)
		{
		    p = skiptowhite(p);
		    ಆದರೆ (*p == NUL)
			ನಿಲ್ಲಿಸು;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(ಅಕ್ಷರ *, *argc + 4);
	    ಆದರೆ (*argv == ಖಾಲಿ)	    // out of memory
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
build_argv_from_string(char_u *cmd, ಅಕ್ಷರ ***argv, ಪೂರ್ಣಾಂಕ *argc)
{
    char_u	*cmd_copy;
    ಪೂರ್ಣಾಂಕ		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    ಆದರೆ (cmd_copy == ಖಾಲಿ
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (i = 0; i < *argc; i++)
	(*argv)[i] = (ಅಕ್ಷರ *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = ಖಾಲಿ;
    vim_free(cmd_copy);
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
build_argv_from_list(list_T *l, ಅಕ್ಷರ ***argv, ಪೂರ್ಣಾಂಕ *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(ಅಕ್ಷರ *, l->lv_len + 1);
    ಆದರೆ (*argv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    *argc = 0;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	ಆದರೆ (s == ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*argv)[*argc] = (ಅಕ್ಷರ *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    ಪೂರ್ಣಾಂಕ
get_special_pty_type(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಹಿಂತಿರುಗಿ get_conpty_type();
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

// compare two keyvalue_T structs by case sensitive value
    ಪೂರ್ಣಾಂಕ
cmp_keyvalue_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಹಿಂತಿರುಗಿ STRCMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string);
}

// compare two keyvalue_T structs by value with length
    ಪೂರ್ಣಾಂಕ
cmp_keyvalue_ಮೌಲ್ಯ_n(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಹಿಂತಿರುಗಿ STRNCMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string, MAX(kv1->ಮೌಲ್ಯ.ಉದ್ದಳತೆ,
		kv2->ಮೌಲ್ಯ.ಉದ್ದಳತೆ));
}

// compare two keyvalue_T structs by case insensitive value
    ಪೂರ್ಣಾಂಕ
cmp_keyvalue_ಮೌಲ್ಯ_i(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಹಿಂತಿರುಗಿ STRICMP(kv1->ಮೌಲ್ಯ.string, kv2->ಮೌಲ್ಯ.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    ಪೂರ್ಣಾಂಕ
cmp_keyvalue_ಮೌಲ್ಯ_ni(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    ಹಿಂತಿರುಗಿ vim_strnicmp_asc((ಅಕ್ಷರ *)kv1->ಮೌಲ್ಯ.string,
	    (ಅಕ್ಷರ *)kv2->ಮೌಲ್ಯ.string, MAX(kv1->ಮೌಲ್ಯ.ಉದ್ದಳತೆ,
		    kv2->ಮೌಲ್ಯ.ಉದ್ದಳತೆ));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    ಶೂನ್ಯ *
mergesort_list(
    ಶೂನ್ಯ	*ತಲೆ,
    ಶೂನ್ಯ	*(*get_ಮುಂದಿನ)(ಶೂನ್ಯ *),
    ಶೂನ್ಯ	(*ಗಣ_ಮುಂದಿನ)(ಶೂನ್ಯ *, ಶೂನ್ಯ *),
    ಶೂನ್ಯ	*(*get_prev)(ಶೂನ್ಯ *),
    ಶೂನ್ಯ	(*ಗಣ_prev)(ಶೂನ್ಯ *, ಶೂನ್ಯ *),
    ಪೂರ್ಣಾಂಕ		(*compare)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *))
{
    ಆದರೆ (!ತಲೆ || !get_ಮುಂದಿನ(ತಲೆ))
	ಹಿಂತಿರುಗಿ ತಲೆ;

    // Count length
    ಪೂರ್ಣಾಂಕ	    n = 0;
    ಶೂನ್ಯ*   curr = ತಲೆ;
    ಆಗ (curr)
    {
	n++;
	curr = get_ಮುಂದಿನ(curr);
    }

    ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_2;
    ಗಾಗಿ (ಗಾತ್ರ_2 = 1; ಗಾತ್ರ_2 < n; ಗಾತ್ರ_2 *= 2)
    {
	ಶೂನ್ಯ*	new_ತಲೆ = ಖಾಲಿ;
	ಶೂನ್ಯ*	ಬಾಲ = ಖಾಲಿ;
	curr = ತಲೆ;

	ಆಗ (curr)
	{
	    // Split two runs
	    ಶೂನ್ಯ    *ಎಡ_2 = curr;
	    ಶೂನ್ಯ    *ಬಲ_2 = ಎಡ_2;
	    ಪೂರ್ಣಾಂಕ	    i;
	    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2 && ಬಲ_2; ++i)
		ಬಲ_2 = get_ಮುಂದಿನ(ಬಲ_2);

	    ಶೂನ್ಯ    *ಮುಂದಿನ = ಬಲ_2;
	    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2 && ಮುಂದಿನ; ++i)
		ಮುಂದಿನ = get_ಮುಂದಿನ(ಮುಂದಿನ);

	    // Break links
	    ಶೂನ್ಯ    *l_ಕೊನೆ = ಬಲ_2 ? get_prev(ಬಲ_2) : ಖಾಲಿ;
	    ಆದರೆ (l_ಕೊನೆ)
		ಗಣ_ಮುಂದಿನ(l_ಕೊನೆ, ಖಾಲಿ);
	    ಆದರೆ (ಬಲ_2)
		ಗಣ_prev(ಬಲ_2, ಖಾಲಿ);

	    ಶೂನ್ಯ    *r_ಕೊನೆ = ಮುಂದಿನ ? get_prev(ಮುಂದಿನ) : ಖಾಲಿ;
	    ಆದರೆ (r_ಕೊನೆ)
		ಗಣ_ಮುಂದಿನ(r_ಕೊನೆ, ಖಾಲಿ);
	    ಆದರೆ (ಮುಂದಿನ)
		ಗಣ_prev(ಮುಂದಿನ, ಖಾಲಿ);

	    //ವಿಲೀನ
	    ಶೂನ್ಯ    *merged = ಖಾಲಿ;
	    ಶೂನ್ಯ    *merged_ಬಾಲ = ಖಾಲಿ;

	    ಆಗ (ಎಡ_2 || ಬಲ_2)
	    {
		ಶೂನ್ಯ	*chosen = ಖಾಲಿ;
		ಆದರೆ (!ಎಡ_2)
		{
		    chosen = ಬಲ_2;
		    ಬಲ_2 = get_ಮುಂದಿನ(ಬಲ_2);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಬಲ_2)
		{
		    chosen = ಎಡ_2;
		    ಎಡ_2 = get_ಮುಂದಿನ(ಎಡ_2);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compare(ಎಡ_2, ಬಲ_2) <= 0)
		{
		    chosen = ಎಡ_2;
		    ಎಡ_2 = get_ಮುಂದಿನ(ಎಡ_2);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    chosen = ಬಲ_2;
		    ಬಲ_2 = get_ಮುಂದಿನ(ಬಲ_2);
		}

		ಆದರೆ (merged_ಬಾಲ)
		{
		    ಗಣ_ಮುಂದಿನ(merged_ಬಾಲ, chosen);
		    ಗಣ_prev(chosen, merged_ಬಾಲ);
		    merged_ಬಾಲ = chosen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    merged = merged_ಬಾಲ = chosen;
		    ಗಣ_prev(chosen, ಖಾಲಿ);
		}
	    }

	    // Connect to full list
	    ಆದರೆ (!new_ತಲೆ)
		new_ತಲೆ = merged;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಣ_ಮುಂದಿನ(ಬಾಲ, merged);
		ಗಣ_prev(merged, ಬಾಲ);
	    }

	    // Move tail to end
	    ಆಗ (get_ಮುಂದಿನ(merged_ಬಾಲ))
		merged_ಬಾಲ = get_ಮುಂದಿನ(merged_ಬಾಲ);
	    ಬಾಲ = merged_ಬಾಲ;

	    curr = ಮುಂದಿನ;
	}

	ತಲೆ = new_ತಲೆ;
    }

    ಹಿಂತಿರುಗಿ ತಲೆ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char_u base64_ಕೋಷ್ಟಕ[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

ಸ್ಥಿರ char_u base64_dec_ಕೋಷ್ಟಕ[256];

#ವ್ಯಾಖ್ಯಾನಿಸು BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_base64_dec_ಕೋಷ್ಟಕ(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ base64_dec_tbl_initialized = FALSE;

    ಆದರೆ (base64_dec_tbl_initialized)
	ಹಿಂತಿರುಗಿ;

    vim_memset(base64_dec_ಕೋಷ್ಟಕ, 0xFF, ಗಾತ್ರ(base64_dec_ಕೋಷ್ಟಕ));
    ಗಾಗಿ (size_t i = 0; i < ಗಾತ್ರ(base64_ಕೋಷ್ಟಕ) - 1; i++)
	base64_dec_ಕೋಷ್ಟಕ[(char_u)base64_ಕೋಷ್ಟಕ[i]] = (char_u)i;
    base64_dec_ಕೋಷ್ಟಕ['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    ದೀರ್ಘ
base64_encode_buf(char_u *dst, ಸ್ಥಿರಾಂಕ char_u *src, size_t len)
{
    size_t i, j;

    ಗಾಗಿ (i = 0, j = 0; i < len;)
    {
	size_t ಉಳಿದ = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = ಉಳಿದ > 1 ? src[i++] : 0;
	int_u octet_c = ಉಳಿದ > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_ಕೋಷ್ಟಕ[(triple >> 18) & 0x3F];
	dst[j++] = base64_ಕೋಷ್ಟಕ[(triple >> 12) & 0x3F];
	dst[j++] = ಉಳಿದ > 1
	    ? base64_ಕೋಷ್ಟಕ[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = ಉಳಿದ > 2
	    ? base64_ಕೋಷ್ಟಕ[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(ಸ್ಥಿರಾಂಕ char_u *ದತ್ತಾಂಶ, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    ಆದರೆ (encoded == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    base64_encode_buf(encoded, ದತ್ತಾಂಶ, len);
    ಹಿಂತಿರುಗಿ encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
base64_decode(ಸ್ಥಿರಾಂಕ char_u *ದತ್ತಾಂಶ, size_t len, garray_T *ಹೊರಹೋಗು)
{
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (len % 4 != 0)
    {
	semsg(_(e_invalid_ವಾದ_str), ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    init_base64_dec_ಕೋಷ್ಟಕ();

    size_t decoded_len = (len / 4) * 3;
    ಆದರೆ (ದತ್ತಾಂಶ[len - 1] == '=')
	decoded_len--;
    ಆದರೆ (ದತ್ತಾಂಶ[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    ಗಾಗಿ (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_ಕೋಷ್ಟಕ[(char_u)ದತ್ತಾಂಶ[i++]];
	int_u sextet_b = base64_dec_ಕೋಷ್ಟಕ[(char_u)ದತ್ತಾಂಶ[i++]];
	int_u sextet_c = base64_dec_ಕೋಷ್ಟಕ[(char_u)ದತ್ತಾಂಶ[i++]];
	int_u sextet_d = base64_dec_ಕೋಷ್ಟಕ[(char_u)ದತ್ತಾಂಶ[i++]];

	ಆದರೆ (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_ವಾದ_str), ದತ್ತಾಂಶ);
	    ga_clear(ಹೊರಹೋಗು);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	ಆದರೆ (j < decoded_len)
	{
	    ga_append(ಹೊರಹೋಗು, (triple >> 16) & 0xFF);
	    j++;
	}
	ಆದರೆ (j < decoded_len)
	{
	    ga_append(ಹೊರಹೋಗು, (triple >> 8) & 0xFF);
	    j++;
	}
	ಆದರೆ (j < decoded_len)
	{
	    ga_append(ಹೊರಹೋಗು, triple & 0xFF);
	    j++;
	}

	ಆದರೆ (j == decoded_len)
	{
	    ಆದರೆ ((ದತ್ತಾಂಶ[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((ದತ್ತಾಂಶ[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_ವಾದ_str), ದತ್ತಾಂಶ);
		ga_clear(ಹೊರಹೋಗು);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}
