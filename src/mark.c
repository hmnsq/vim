/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * mark.c: functions for setting marks and jumping to them
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * This file contains routines to maintain and manipulate marks.
 */

/*
 * If a named file mark's lnum is non-zero, it is valid.
 * If a named file mark's fnum is non-zero, it is for an existing buffer,
 * otherwise it is from .viminfo and namedfm[n].fname is the file name.
 * There are marks 'A - 'Z (set by user) and '0 to '9 (set when writing
 * viminfo).
 */
ಸ್ಥಿರ xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

ಸ್ಥಿರ ಶೂನ್ಯ fname2fnum(xfmark_T *fm);
ಸ್ಥಿರ ಶೂನ್ಯ fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
ಸ್ಥಿರ char_u *mark_ಸಾಲು(pos_T *mp, ಪೂರ್ಣಾಂಕ lead_len);
ಸ್ಥಿರ ಶೂನ್ಯ show_one_mark(ಪೂರ್ಣಾಂಕ, char_u *, pos_T *, char_u *, ಪೂರ್ಣಾಂಕ current);
ಸ್ಥಿರ ಶೂನ್ಯ mark_adjust_ಆಂತರಿಕ(linenr_T line1, linenr_T line2, ದೀರ್ಘ amount,
    ದೀರ್ಘ amount_after, ಪೂರ್ಣಾಂಕ adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ಪೂರ್ಣಾಂಕ
setmark(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ಪೂರ್ಣಾಂಕ
setmark_pos(ಪೂರ್ಣಾಂಕ c, pos_T *pos, ಪೂರ್ಣಾಂಕ fnum)
{
    ಪೂರ್ಣಾಂಕ		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ಆದರೆ (c < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (c == '\'' || c == '`')
    {
	ಆದರೆ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curwin->w_pcmark = *pos;
	ಹಿಂತಿರುಗಿ OK;
    }

    buf = buflist_findnr(fnum);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (c == '"')
    {
	buf->b_ಕೊನೆಯ_cursor = *pos;
	ಹಿಂತಿರುಗಿ OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ಆದರೆ (c == '[')
    {
	buf->b_op_ಆರಂಭ = *pos;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (c == ']')
    {
	buf->b_op_ಕೊನೆ = *pos;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (c == '<' || c == '>')
    {
	ಆದರೆ (c == '<')
	    buf->b_visual.vi_ಆರಂಭ = *pos;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_visual.vi_ಕೊನೆ = *pos;
	ಆದರೆ (buf->b_visual.vi_ಬಹುಲಕ == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_ಬಹುಲಕ = 'v';
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ಆದರೆ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	ಇಲ್ಲದಿದ್ದರೆ
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	namedfm[i].ಸಮಯ_ಗಣ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    ಶೂನ್ಯ
mark_forget_file(win_T *wp, ಪೂರ್ಣಾಂಕ fnum)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ಆದರೆ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ಆದರೆ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * ಗಾತ್ರ(wp->w_jumplist[i]));
	}

    ಗಾಗಿ (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ಆದರೆ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_ನಮೂದು(&wp->w_tagstack[i]);
	    ಆದರೆ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * ಗಾತ್ರ(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    ಶೂನ್ಯ
setpcmark(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ಆದರೆ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	ಹಿಂತಿರುಗಿ;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ಆದರೆ (jop_flags & JOP_ರಾಶಿ)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ಆದರೆ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ಆದರೆ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	ಗಾಗಿ (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    fm->ಸಮಯ_ಗಣ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * To change context, call setpcmark(), then move the current position to
 * where ever, then call checkpcmark().  This ensures that the previous
 * context will only be changed if the cursor moved to a different line.
 * If pcmark was deleted (with "dG") the previous mark is restored.
 */
    ಶೂನ್ಯ
checkpcmark(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ಆದರೆ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (curwin->w_jumplistidx + ಎಣಿಕೆ_2 < 0
		|| curwin->w_jumplistidx + ಎಣಿಕೆ_2 >= curwin->w_jumplistlen)
	    ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ಆದರೆ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ಆದರೆ (curwin->w_jumplistidx + ಎಣಿಕೆ_2 < 0)
		ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;
	}

	curwin->w_jumplistidx += ಎಣಿಕೆ_2;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ಆದರೆ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ಆದರೆ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ಆದರೆ (buflist_findnr(fmark.fnum) == ಖಾಲಿ)
	    {					     // Skip this one ..
		ಎಣಿಕೆ_2 += ಎಣಿಕೆ_2 < 0 ? -1 : 1;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pos = &(jmp->fmark.mark);
	ಹಿಂತಿರುಗಿ pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;

    n = curwin->w_changelistidx;
    ಆದರೆ (n + ಎಣಿಕೆ_2 < 0)
    {
	ಆದರೆ (n == 0)
	    ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;
	n = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n + ಎಣಿಕೆ_2 >= curbuf->b_changelistlen)
    {
	ಆದರೆ (n == curbuf->b_changelistlen - 1)
	    ಹಿಂತಿರುಗಿ (pos_T *)ಖಾಲಿ;
	n = curbuf->b_changelistlen - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	n += ಎಣಿಕೆ_2;
    curwin->w_changelistidx = n;
    ಹಿಂತಿರುಗಿ curbuf->b_changelist + n;
}

/*
 * Find mark "c" in buffer pointed to by "buf".
 * If "changefile" is TRUE it's allowed to edit another file for '0, 'A, etc.
 * If "fnum" is not NULL store the fnum there for '0, 'A etc., don't edit
 * another file.
 * Returns:
 * - pointer to pos_T if found.  lnum is 0 when mark not set, -1 when mark is
 *   in another file which can't be gotten. (caller needs to check lnum!)
 * - NULL if there is no mark called 'c'.
 * - -1 if mark is in other file and jumped there (only if changefile is TRUE)
 */
    pos_T *
getmark_buf(buf_T *buf, ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ changefile)
{
    ಹಿಂತಿರುಗಿ getmark_buf_fnum(buf, c, changefile, ಖಾಲಿ);
}

    pos_T *
getmark(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ changefile)
{
    ಹಿಂತಿರುಗಿ getmark_buf_fnum(curbuf, c, changefile, ಖಾಲಿ);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		c,
    ಪೂರ್ಣಾಂಕ		changefile,
    ಪೂರ್ಣಾಂಕ		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ಸ್ಥಿರ pos_T	pos_copy;

    posp = ಖಾಲಿ;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ಆದರೆ (c < 0)
	ಹಿಂತಿರುಗಿ posp;
    ಆದರೆ (c > '~')			// check for islower()/isupper()
	;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_ಕೊನೆಯ_cursor);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_ಕೊನೆಯ_ಸೇರಿಸು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '.')			// to where last change was made
	posp = &(buf->b_ಕೊನೆಯ_change);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_ಆರಂಭ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_ಕೊನೆ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ಪೂರ್ಣಾಂಕ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ಆದರೆ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ಪೂರ್ಣಾಂಕ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ಆದರೆ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_ಆರಂಭ;
	endp = &buf->b_visual.vi_ಕೊನೆ;
	ಆದರೆ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	ಇಲ್ಲದಿದ್ದರೆ
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ಆದರೆ (buf->b_visual.vi_ಬಹುಲಕ == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ಆದರೆ (c == '<')
		pos_copy.ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		pos_copy.ಲಂಬಸಾಲು = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ಆದರೆ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	ಇಲ್ಲದಿದ್ದರೆ
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ಆದರೆ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ಆದರೆ (fnum != ಖಾಲಿ)
	    *fnum = namedfm[c].fmark.fnum;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ಆದರೆ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ಆದರೆ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    ಹಿಂತಿರುಗಿ (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    ಹಿಂತಿರುಗಿ posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ಪೂರ್ಣಾಂಕ		dir,	// direction for search
    ಪೂರ್ಣಾಂಕ		begin_ಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		i;
    pos_T	*ಫಲಿತಾಂಶ = ಖಾಲಿ;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ಆದರೆ (dir == BACKWARD && begin_ಸಾಲು)
	pos.ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == FORWARD && begin_ಸಾಲು)
	pos.ಲಂಬಸಾಲು = MAXCOL;

    ಗಾಗಿ (i = 0; i < NMARKS; i++)
    {
	ಆದರೆ (curbuf->b_namedm[i].lnum > 0)
	{
	    ಆದರೆ (dir == FORWARD)
	    {
		ಆದರೆ ((ಫಲಿತಾಂಶ == ಖಾಲಿ || LT_POS(curbuf->b_namedm[i], *ಫಲಿತಾಂಶ))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    ಫಲಿತಾಂಶ = &curbuf->b_namedm[i];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ ((ಫಲಿತಾಂಶ == ಖಾಲಿ || LT_POS(*ಫಲಿತಾಂಶ, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    ಫಲಿತಾಂಶ = &curbuf->b_namedm[i];
	    }
	}
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    ಆದರೆ (fm->fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ಆದರೆ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		|| fm->fname[1] == '\\'
#ಆದರೆ_ಅಂತ್ಯ
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (ಶೂನ್ಯ)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    ಶೂನ್ಯ
fmarks_check_ಹೆಸರುಗಳು(buf_T *buf)
{
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;

    ಆದರೆ (buf->b_ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    name = home_replace_save(buf, buf->b_ffname);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ಗಾಗಿ (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ಆದರೆ (fm->fmark.fnum == 0
	    && fm->fname != ಖಾಲಿ
	    && fnamecmp(name, fm->fname) == 0)
    {
	fm->fmark.fnum = buf->b_fnum;
	VIM_CLEAR(fm->fname);
    }
}

/*
 * Check a if a position from a mark is valid.
 * Give and error message and return FAIL if not.
 */
    ಪೂರ್ಣಾಂಕ
check_mark(pos_T *pos)
{
    ಆದರೆ (pos == ಖಾಲಿ)
    {
	emsg(_(e_unknown_mark));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ಆದರೆ (pos->lnum == 0)
	    emsg(_(e_mark_not_ಗಣ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (pos->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_mark_has_invalid_ಸಾಲು_ಸಂಖ್ಯೆ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    ಶೂನ್ಯ
clrallmarks(buf_T *buf)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		i = -1;

    ಆದರೆ (i == -1)	// first call ever: initialize
	ಗಾಗಿ (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	    namedfm[i].ಸಮಯ_ಗಣ = 0;
#ಆದರೆ_ಅಂತ್ಯ
	}

    ಗಾಗಿ (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_ಆರಂಭ.lnum = 0;		// start/end op mark cleared
    buf->b_op_ಕೊನೆ.lnum = 0;
    buf->b_ಕೊನೆಯ_cursor.lnum = 1;	// '" mark cleared
    buf->b_ಕೊನೆಯ_cursor.ಲಂಬಸಾಲು = 0;
    buf->b_ಕೊನೆಯ_cursor.coladd = 0;
    buf->b_ಕೊನೆಯ_ಸೇರಿಸು.lnum = 0;	// '^ mark cleared
    buf->b_ಕೊನೆಯ_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, ಪೂರ್ಣಾಂಕ lead_len)
{
    ಆದರೆ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	ಹಿಂತಿರುಗಿ mark_ಸಾಲು(&(fmark->mark), lead_len);
    ಹಿಂತಿರುಗಿ buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ಸ್ಥಿರ char_u *
mark_ಸಾಲು(pos_T *mp, ಪೂರ್ಣಾಂಕ lead_len)
{
    char_u	*s, *p;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    // Truncate the line to fit it in the window.
    len = 0;
    ಗಾಗಿ (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ಆದರೆ (len >= Columns - lead_len)
	    ನಿಲ್ಲಿಸು;
    }
    *p = NUL;
    ಹಿಂತಿರುಗಿ s;
}

/*
 * print the marks
 */
    ಶೂನ್ಯ
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ಆದರೆ (arg != ಖಾಲಿ && *arg == NUL)
	arg = ಖಾಲಿ;

    show_one_mark('\'', arg, &curwin->w_pcmark, ಖಾಲಿ, TRUE);
    ಗಾಗಿ (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ಖಾಲಿ, TRUE);
    ಗಾಗಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ಆದರೆ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	ಇಲ್ಲದಿದ್ದರೆ
	    name = namedfm[i].fname;
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ಆದರೆ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_ಕೊನೆಯ_cursor, ಖಾಲಿ, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_ಆರಂಭ, ಖಾಲಿ, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_ಕೊನೆ, ಖಾಲಿ, TRUE);
    show_one_mark('^', arg, &curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು, ಖಾಲಿ, TRUE);
    show_one_mark('.', arg, &curbuf->b_ಕೊನೆಯ_change, ಖಾಲಿ, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_ಆರಂಭ;
    endp = &curbuf->b_visual.vi_ಕೊನೆ;
    ಆದರೆ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    ಇಲ್ಲದಿದ್ದರೆ
	posp = endp;
    show_one_mark('<', arg, posp, ಖಾಲಿ, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ಖಾಲಿ, TRUE);

    show_one_mark(-1, arg, ಖಾಲಿ, ಖಾಲಿ, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_one_mark(
    ಪೂರ್ಣಾಂಕ		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ಪೂರ್ಣಾಂಕ		current)	// in current file
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_title = FALSE;
    ಪೂರ್ಣಾಂಕ		mustfree = FALSE;
    char_u	*name = name_arg;

    ಆದರೆ (c == -1)			    // finish up
    {
	ಆದರೆ (did_title)
	    did_title = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (arg == ಖಾಲಿ)
		ಸಂದೇಶ(_("No marks set"));
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!got_int
	    && (arg == ಖಾಲಿ || vim_strchr(arg, c) != ಖಾಲಿ)
	    && p->lnum != 0)
    {
	ಆದರೆ (name == ಖಾಲಿ && current)
	{
	    name = mark_ಸಾಲು(p, 15);
	    ಆದರೆ (name == ಖಾಲಿ)
	    {
		emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		ಹಿಂತಿರುಗಿ;
	    }
	    mustfree = TRUE;
	}
	ಆದರೆ (!ಸಂದೇಶ_filtered(name))
	{
	    ಆದರೆ (!did_title)
	    {
		// Highlight title
		ಸಂದೇಶ_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (!got_int)
	    {
		sprintf((ಅಕ್ಷರ *)IObuff, " %c %6ld %4d ", c, p->lnum, p->ಲಂಬಸಾಲು);
		ಸಂದೇಶ_outtrans(IObuff);
		ಆದರೆ (name != ಖಾಲಿ)
		{
		    ಸಂದೇಶ_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    ಹೊರಹೋಗು_flush();		    // show one line at a time
	}
	ಆದರೆ (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    ಶೂನ್ಯ
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		from, ಗಮ್ಯ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಅಧೋ;
    ಪೂರ್ಣಾಂಕ		digit;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->forceit)
	emsg(_(e_invalid_ವಾದ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->arg == NUL)
	emsg(_(e_ವಾದ_required));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// clear specified marks only
	ಗಾಗಿ (p = eap->arg; *p != NUL; ++p)
	{
	    ಅಧೋ = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ಆದರೆ (ಅಧೋ || digit || ASCII_ISUPPER(*p))
	    {
		ಆದರೆ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    ಗಮ್ಯ = p[2];
		    ಆದರೆ (!(ಅಧೋ ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || ಗಮ್ಯ < from)
		    {
			semsg(_(e_invalid_ವಾದ_str), p);
			ಹಿಂತಿರುಗಿ;
		    }
		    p += 2;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    // clear one lower case mark
		    from = ಗಮ್ಯ = *p;

		ಗಾಗಿ (i = from; i <= ಗಮ್ಯ; ++i)
		{
		    ಆದರೆ (ಅಧೋ)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (digit)
			    n = i - '0' + NMARKS;
			ಇಲ್ಲದಿದ್ದರೆ
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
			namedfm[n].ಸಮಯ_ಗಣ = digit ? 0 : vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆಯ್ಕೆ (*p)
		{
		    ಸ್ಥಿತಿ '"': curbuf->b_ಕೊನೆಯ_cursor.lnum = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '^': curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು.lnum = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '.': curbuf->b_ಕೊನೆಯ_change.lnum = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '[': curbuf->b_op_ಆರಂಭ.lnum    = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ']': curbuf->b_op_ಕೊನೆ.lnum      = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '<': curbuf->b_visual.vi_ಆರಂಭ.lnum = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ '>': curbuf->b_visual.vi_ಕೊನೆ.lnum   = 0; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ' ': ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:  semsg(_(e_invalid_ವಾದ_str), p);
			      ಹಿಂತಿರುಗಿ;
		}
	}
    }
}

/*
 * print the jumplist
 */
    ಶೂನ್ಯ
ex_jumps(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n jump line  col file/text"));
    ಗಾಗಿ (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ಆದರೆ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ಆದರೆ (name == ಖಾಲಿ && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ಆದರೆ (name == ಖಾಲಿ || ಸಂದೇಶ_filtered(name))
	    {
		vim_free(name);
		ಮುಂದುವರಿ;
	    }

	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
	    {
		vim_free(name);
		ನಿಲ್ಲಿಸು;
	    }
	    sprintf((ಅಕ್ಷರ *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.ಲಂಬಸಾಲು);
	    ಸಂದೇಶ_outtrans(IObuff);
	    ಸಂದೇಶ_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	ಹೊರಹೋಗು_flush();
    }
    ಆದರೆ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	ಸಂದೇಶ_puts("\n>");
}

    ಶೂನ್ಯ
ex_clearjumps(exarg_T *eap ಬಳಸದ)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    ಶೂನ್ಯ
ex_changes(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\nchange line  col text"));

    ಗಾಗಿ (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ಆದರೆ (curbuf->b_changelist[i].lnum != 0)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    sprintf((ಅಕ್ಷರ *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (ದೀರ್ಘ)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].ಲಂಬಸಾಲು);
	    ಸಂದೇಶ_outtrans(IObuff);
	    name = mark_ಸಾಲು(&curbuf->b_changelist[i], 17);
	    ಆದರೆ (name == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಸಂದೇಶ_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	ಹೊರಹೋಗು_flush();
    }
    ಆದರೆ (curwin->w_changelistidx == curbuf->b_changelistlen)
	ಸಂದೇಶ_puts("\n>");
}

#ವ್ಯಾಖ್ಯಾನಿಸು one_adjust(ಕೂಡಿಸು) \
    { \
	lp = ಕೂಡಿಸು; \
	ಆದರೆ (*lp >= line1 && *lp <= line2) \
	{ \
	    ಆದರೆ (amount == MAXLNUM) \
		*lp = 0; \
	    ಇಲ್ಲದಿದ್ದರೆ \
		*lp += amount; \
	} \
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#ವ್ಯಾಖ್ಯಾನಿಸು one_adjust_nodel(ಕೂಡಿಸು) \
    { \
	lp = ಕೂಡಿಸು; \
	ಆದರೆ (*lp >= line1 && *lp <= line2) \
	{ \
	    ಆದರೆ (amount == MAXLNUM) \
		*lp = line1; \
	    ಇಲ್ಲದಿದ್ದರೆ \
		*lp += amount; \
	} \
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#ವ್ಯಾಖ್ಯಾನಿಸು one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ಆದರೆ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ಆದರೆ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->ಲಂಬಸಾಲು = 0; \
	    } \
	    ಇಲ್ಲದಿದ್ದರೆ /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after && posp->lnum > line2) \
	    posp->lnum += amount_after; \
    }

/*
 * Adjust marks between "line1" and "line2" (inclusive) to move "amount" lines.
 * Must be called before changed_*(), appended_lines() or deleted_lines().
 * May be called before or after changing the text.
 * When deleting lines "line1" to "line2", use an "amount" of MAXLNUM: The
 * marks within this range are made invalid.
 * If "amount_after" is non-zero adjust marks after "line2".
 * Example: Delete lines 34 and 35: mark_adjust(34, 35, MAXLNUM, -2);
 * Example: Insert two lines below 55: mark_adjust(56, MAXLNUM, 2, 0);
 *				   or: mark_adjust(56, 55, MAXLNUM, 2);
 */
    ಶೂನ್ಯ
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    mark_adjust_ಆಂತರಿಕ(line1, line2, amount, amount_after, TRUE);
}

    ಶೂನ್ಯ
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after)
{
    mark_adjust_ಆಂತರಿಕ(line1, line2, amount, amount_after, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mark_adjust_ಆಂತರಿಕ(
    linenr_T	line1,
    linenr_T	line2,
    ದೀರ್ಘ	amount,
    ದೀರ್ಘ	amount_after,
    ಪೂರ್ಣಾಂಕ		adjust_folds ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ಸ್ಥಿರ pos_T initpos = {1, 0, 0};

    ಆದರೆ (line2 < line1 && amount_after == 0L)	    // nothing to do
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	ಗಾಗಿ (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ಆದರೆ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	ಗಾಗಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ಆದರೆ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು.lnum));

	// last change position
	one_adjust(&(curbuf->b_ಕೊನೆಯ_change.lnum));

	// last cursor position, if it was set
	ಆದರೆ (!EQUAL_POS(curbuf->b_ಕೊನೆಯ_cursor, initpos))
	    one_adjust(&(curbuf->b_ಕೊನೆಯ_cursor.lnum));


	// list of change positions
	ಗಾಗಿ (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_ಆರಂಭ.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_ಕೊನೆ.lnum));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ಖಾಲಿ, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#ಆದರೆ_ಅಂತ್ಯ
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_prev_pcmark.lnum));

    // saved cursor for formatting
    ಆದರೆ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    ಗಾಗಿ (i = 0; i < win->w_jumplistlen; ++i)
		ಆದರೆ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ಆದರೆ (win->w_ಬಫರ್ == curbuf)
	{
	    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		ಗಾಗಿ (i = 0; i < win->w_tagstacklen; i++)
		    ಆದರೆ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ಆದರೆ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ಆದರೆ (win != curwin)
	    {
		ಆದರೆ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ಆದರೆ (amount == MAXLNUM)	    // topline is deleted
		    {
			ಆದರೆ (line1 <= 1)
			    win->w_topline = 1;
			ಇಲ್ಲದಿದ್ದರೆ
			    win->w_topline = line1 - 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    win->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    win->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // adjust folds
	    ಆದರೆ (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#ಆದರೆ_ಅಂತ್ಯ

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#ವ್ಯಾಖ್ಯಾನಿಸು ಲಂಬಸಾಲು_adjust(pp) \
    { \
	posp = pp; \
	ಆದರೆ (posp->lnum == lnum && posp->ಲಂಬಸಾಲು >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ಆದರೆ (ಲಂಬಸಾಲು_amount < 0 && posp->ಲಂಬಸಾಲು <= (colnr_T)-ಲಂಬಸಾಲು_amount) \
		posp->ಲಂಬಸಾಲು = 0; \
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (posp->ಲಂಬಸಾಲು < spaces_removed) \
		posp->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_amount + spaces_removed; \
	    ಇಲ್ಲದಿದ್ದರೆ \
		posp->ಲಂಬಸಾಲು += ಲಂಬಸಾಲು_amount; \
	} \
    }

/*
 * Adjust marks in line "lnum" at column "mincol" and further: add
 * "lnum_amount" to the line number and add "col_amount" to the column
 * position.
 * "spaces_removed" is the number of spaces that were removed, matters when the
 * cursor is inside them.
 */
    ಶೂನ್ಯ
mark_ಲಂಬಸಾಲು_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    ದೀರ್ಘ	lnum_amount,
    ದೀರ್ಘ	ಲಂಬಸಾಲು_amount,
    ಪೂರ್ಣಾಂಕ		spaces_removed)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ಆದರೆ ((ಲಂಬಸಾಲು_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	ಹಿಂತಿರುಗಿ; // nothing to do

    // named marks, lower case and upper case
    ಗಾಗಿ (i = 0; i < NMARKS; i++)
    {
	ಲಂಬಸಾಲು_adjust(&(curbuf->b_namedm[i]));
	ಆದರೆ (namedfm[i].fmark.fnum == fnum)
	    ಲಂಬಸಾಲು_adjust(&(namedfm[i].fmark.mark));
    }
    ಗಾಗಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ಆದರೆ (namedfm[i].fmark.fnum == fnum)
	    ಲಂಬಸಾಲು_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    ಲಂಬಸಾಲು_adjust(&(curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು));

    // last change position
    ಲಂಬಸಾಲು_adjust(&(curbuf->b_ಕೊನೆಯ_change));

    // list of change positions
    ಗಾಗಿ (i = 0; i < curbuf->b_changelistlen; ++i)
	ಲಂಬಸಾಲು_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    ಲಂಬಸಾಲು_adjust(&(curbuf->b_visual.vi_ಆರಂಭ));
    ಲಂಬಸಾಲು_adjust(&(curbuf->b_visual.vi_ಕೊನೆ));

    // previous context mark
    ಲಂಬಸಾಲು_adjust(&(curwin->w_pcmark));

    // previous pcmark
    ಲಂಬಸಾಲು_adjust(&(curwin->w_prev_pcmark));

    // saved cursor for formatting
    ಲಂಬಸಾಲು_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	ಗಾಗಿ (i = 0; i < win->w_jumplistlen; ++i)
	    ಆದರೆ (win->w_jumplist[i].fmark.fnum == fnum)
		ಲಂಬಸಾಲು_adjust(&(win->w_jumplist[i].fmark.mark));

	ಆದರೆ (win->w_ಬಫರ್ == curbuf)
	{
	    // marks in the tag stack
	    ಗಾಗಿ (i = 0; i < win->w_tagstacklen; i++)
		ಆದರೆ (win->w_tagstack[i].fmark.fnum == fnum)
		    ಲಂಬಸಾಲು_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ಆದರೆ (win != curwin)
		ಲಂಬಸಾಲು_adjust(&win->w_cursor);
	}
    }
}

/*
 * When deleting lines, this may create duplicate marks in the
 * jumplist. They will be removed here for the specified window.
 * When "loadfiles" is TRUE first ensure entries have the "fnum" field set
 * (this may be a bit slow).
 */
    ಶೂನ್ಯ
cleanup_jumplist(win_T *wp, ಪೂರ್ಣಾಂಕ loadfiles)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    from, ಗಮ್ಯ;
    ಪೂರ್ಣಾಂಕ	    mustfree;

    ಆದರೆ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	ಗಾಗಿ (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ಆದರೆ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    ಗಮ್ಯ = 0;
    ಗಾಗಿ (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ಆದರೆ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = ಗಮ್ಯ;
	ಗಾಗಿ (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ಆದರೆ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ರಾಶಿ);
	ಇಲ್ಲದಿದ್ದರೆ				// adjacent duplicate
	    mustfree = TRUE;

	ಆದರೆ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	ಇಲ್ಲದಿದ್ದರೆ
	    wp->w_jumplist[ಗಮ್ಯ++] = wp->w_jumplist[from];
    }
    ಆದರೆ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = ಗಮ್ಯ;
    wp->w_jumplistlen = ಗಮ್ಯ;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    ಶೂನ್ಯ
copy_jumplist(win_T *from, win_T *ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < from->w_jumplistlen; ++i)
    {
	ಗಮ್ಯ->w_jumplist[i] = from->w_jumplist[i];
	ಆದರೆ (from->w_jumplist[i].fname != ಖಾಲಿ)
	    ಗಮ್ಯ->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    ಗಮ್ಯ->w_jumplistlen = from->w_jumplistlen;
    ಗಮ್ಯ->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    ಶೂನ್ಯ
free_jumplist(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    ಶೂನ್ಯ
ಗಣ_ಕೊನೆಯ_cursor(win_T *win)
{
    ಆದರೆ (win->w_ಬಫರ್ != ಖಾಲಿ)
	win->w_ಬಫರ್->b_ಕೊನೆಯ_cursor = win->w_cursor;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_marks(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ಆದರೆ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ namedfm;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ಪೂರ್ಣಾಂಕ bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ಆದರೆ (pos->lnum <= 0)
	ಹಿಂತಿರುಗಿ OK;

    d = dict_alloc();
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    lpos = list_alloc();
    ಆದರೆ (lpos == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    list_append_ಸಂಖ್ಯೆ(lpos, bufnr);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->lnum);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->ಲಂಬಸಾಲು < MAXCOL ? pos->ಲಂಬಸಾಲು + 1 : MAXCOL);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->coladd);

    ಆದರೆ (dict_ಕೂಡಿಸು_string_len(d, "mark", mname, (ಪೂರ್ಣಾಂಕ)mnamelen) == FAIL
	    || dict_ಕೂಡಿಸು_list(d, "pos", lpos) == FAIL
	    || (fname != ಖಾಲಿ && dict_ಕೂಡಿಸು_string(d, "file", fname) == FAIL))
    {
	ಆದರೆ (lpos->lv_refcount == 0)
	    list_free(lpos);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ಪೂರ್ಣಾಂಕ		i;

    // Marks 'a' to 'z'
    ಗಾಗಿ (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	ಕೂಡಿಸು_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ಖಾಲಿ);
    }

    // Mark '' is a window local mark and not a buffer local mark
    ಕೂಡಿಸು_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ಖಾಲಿ);

    ಕೂಡಿಸು_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_ಕೊನೆಯ_cursor, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_ಆರಂಭ, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_ಕೊನೆ, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_ಕೊನೆಯ_ಸೇರಿಸು, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_ಕೊನೆಯ_change, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_ಆರಂಭ, buf->b_fnum, ಖಾಲಿ);
    ಕೂಡಿಸು_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_ಕೊನೆ, buf->b_fnum, ಖಾಲಿ);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    ಗಾಗಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ಆದರೆ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    name = namedfm[i].fname;
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    ಕೂಡಿಸು_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ಆದರೆ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    ಶೂನ್ಯ
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ಖಾಲಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	ಹಿಂತಿರುಗಿ;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#ಆದರೆ_ಅಂತ್ಯ
