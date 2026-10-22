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
ಸ್ಥಿರ char_u *mark_ಸಾಲು(pos_T *mp, ಪೂರ್ಣಾಙ್ಕ lead_len);
ಸ್ಥಿರ ಶೂನ್ಯ show_one_mark(ಪೂರ್ಣಾಙ್ಕ, char_u *, pos_T *, char_u *, ಪೂರ್ಣಾಙ್ಕ current);
ಸ್ಥಿರ ಶೂನ್ಯ mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, ಪೂರ್ಣಾಙ್ಕ adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ಪೂರ್ಣಾಙ್ಕ
setmark(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ಪೂರ್ಣಾಙ್ಕ
setmark_pos(ಪೂರ್ಣಾಙ್ಕ c, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ fnum)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ಯದಿ (c < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (c == '\'' || c == '`')
    {
	ಯದಿ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	ಅನ್ಯಥಾ
	    curwin->w_pcmark = *pos;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    buf = buflist_findnr(fnum);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (c == '"')
    {
	buf->b_last_cursor = *pos;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ಯದಿ (c == '[')
    {
	buf->b_op_start = *pos;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (c == ']')
    {
	buf->b_op_end = *pos;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (c == '<' || c == '>')
    {
	ಯದಿ (c == '<')
	    buf->b_visual.vi_start = *pos;
	ಅನ್ಯಥಾ
	    buf->b_visual.vi_end = *pos;
	ಯದಿ (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ಯದಿ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	ಅನ್ಯಥಾ
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    ಶೂನ್ಯ
mark_forget_file(win_T *wp, ಪೂರ್ಣಾಙ್ಕ fnum)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ಯದಿ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ಯದಿ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * ಪರಿಮಾಣ(wp->w_jumplist[i]));
	}

    ಪ್ರತಿ (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ಯದಿ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    ಯದಿ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * ಪರಿಮಾಣ(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    ಶೂನ್ಯ
setpcmark(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ಯದಿ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	ಪ್ರತ್ಯಾಗಮ;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ಯದಿ (jop_flags & JOP_ರಾಶಿ)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ಯದಿ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ಯದಿ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	ಪ್ರತಿ (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ರಿಕ್ತ;
#ifdef FEAT_VIMINFO
    fm->time_set = vim_time();
#endif
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
    ಯದಿ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ಯದಿ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (curwin->w_jumplistidx + ಎಣಿಕೆ < 0
		|| curwin->w_jumplistidx + ಎಣಿಕೆ >= curwin->w_jumplistlen)
	    ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ಯದಿ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ಯದಿ (curwin->w_jumplistidx + ಎಣಿಕೆ < 0)
		ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;
	}

	curwin->w_jumplistidx += ಎಣಿಕೆ;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ಯದಿ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ಯದಿ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ಯದಿ (buflist_findnr(fmark.fnum) == ರಿಕ್ತ)
	    {					     // Skip this one ..
		ಎಣಿಕೆ += ಎಣಿಕೆ < 0 ? -1 : 1;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	ಅನ್ಯಥಾ
	    pos = &(jmp->fmark.mark);
	ಪ್ರತ್ಯಾಗಮ pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;

    n = curwin->w_changelistidx;
    ಯದಿ (n + ಎಣಿಕೆ < 0)
    {
	ಯದಿ (n == 0)
	    ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;
	n = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (n + ಎಣಿಕೆ >= curbuf->b_changelistlen)
    {
	ಯದಿ (n == curbuf->b_changelistlen - 1)
	    ಪ್ರತ್ಯಾಗಮ (pos_T *)ರಿಕ್ತ;
	n = curbuf->b_changelistlen - 1;
    }
    ಅನ್ಯಥಾ
	n += ಎಣಿಕೆ;
    curwin->w_changelistidx = n;
    ಪ್ರತ್ಯಾಗಮ curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ changefile)
{
    ಪ್ರತ್ಯಾಗಮ getmark_buf_fnum(buf, c, changefile, ರಿಕ್ತ);
}

    pos_T *
getmark(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ changefile)
{
    ಪ್ರತ್ಯಾಗಮ getmark_buf_fnum(curbuf, c, changefile, ರಿಕ್ತ);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		c,
    ಪೂರ್ಣಾಙ್ಕ		changefile,
    ಪೂರ್ಣಾಙ್ಕ		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ಸ್ಥಿರ pos_T	pos_copy;

    posp = ರಿಕ್ತ;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ಯದಿ (c < 0)
	ಪ್ರತ್ಯಾಗಮ posp;
    ಯದಿ (c > '~')			// check for islower()/isupper()
	;
    ಅನ್ಯಥಾ ಯದಿ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    ಅನ್ಯಥಾ ಯದಿ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    ಅನ್ಯಥಾ ಯದಿ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_ಸೇರಿಸು);
    ಅನ್ಯಥಾ ಯದಿ (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    ಅನ್ಯಥಾ ಯದಿ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    ಅನ್ಯಥಾ ಯದಿ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    ಅನ್ಯಥಾ ಯದಿ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ಪೂರ್ಣಾಙ್ಕ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ಯದಿ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ಅನ್ಯಥಾ ಯದಿ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ಪೂರ್ಣಾಙ್ಕ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ಯದಿ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ಅನ್ಯಥಾ ಯದಿ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	ಯದಿ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	ಅನ್ಯಥಾ
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ಯದಿ (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ಯದಿ (c == '<')
		pos_copy.col = 0;
	    ಅನ್ಯಥಾ
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    ಅನ್ಯಥಾ ಯದಿ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ಯದಿ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	ಅನ್ಯಥಾ
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ಯದಿ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ಯದಿ (fnum != ರಿಕ್ತ)
	    *fnum = namedfm[c].fmark.fnum;
	ಅನ್ಯಥಾ ಯದಿ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ಯದಿ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ಯದಿ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    ಪ್ರತ್ಯಾಗಮ (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    ಅನ್ಯಥಾ
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    ಪ್ರತ್ಯಾಗಮ posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ಪೂರ್ಣಾಙ್ಕ		dir,	// direction for search
    ಪೂರ್ಣಾಙ್ಕ		begin_ಸಾಲು)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    pos_T	*result = ರಿಕ್ತ;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ಯದಿ (dir == BACKWARD && begin_ಸಾಲು)
	pos.col = 0;
    ಅನ್ಯಥಾ ಯದಿ (dir == FORWARD && begin_ಸಾಲು)
	pos.col = MAXCOL;

    ಪ್ರತಿ (i = 0; i < NMARKS; i++)
    {
	ಯದಿ (curbuf->b_namedm[i].lnum > 0)
	{
	    ಯದಿ (dir == FORWARD)
	    {
		ಯದಿ ((result == ರಿಕ್ತ || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ ((result == ರಿಕ್ತ || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ result;
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

    ಯದಿ (fm->fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ಯದಿ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    ಅನ್ಯಥಾ
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
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;

    ಯದಿ (buf->b_ffname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    name = home_replace_save(buf, buf->b_ffname);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ಪ್ರತಿ (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ಯದಿ (fm->fmark.fnum == 0
	    && fm->fname != ರಿಕ್ತ
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
    ಪೂರ್ಣಾಙ್ಕ
check_mark(pos_T *pos)
{
    ಯದಿ (pos == ರಿಕ್ತ)
    {
	emsg(_(e_unknown_mark));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ಯದಿ (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (pos->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_mark_has_invalid_ಸಾಲು_ಸಂಖ್ಯೆ));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    ಶೂನ್ಯ
clrallmarks(buf_T *buf)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		i = -1;

    ಯದಿ (i == -1)	// first call ever: initialize
	ಪ್ರತಿ (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ರಿಕ್ತ;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    ಪ್ರತಿ (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_ಸೇರಿಸು.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, ಪೂರ್ಣಾಙ್ಕ lead_len)
{
    ಯದಿ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	ಪ್ರತ್ಯಾಗಮ mark_ಸಾಲು(&(fmark->mark), lead_len);
    ಪ್ರತ್ಯಾಗಮ buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ಸ್ಥಿರ char_u *
mark_ಸಾಲು(pos_T *mp, ಪೂರ್ಣಾಙ್ಕ lead_len)
{
    char_u	*s, *p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    // Truncate the line to fit it in the window.
    len = 0;
    ಪ್ರತಿ (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ಯದಿ (len >= Columns - lead_len)
	    ಭಙ್ಗ;
    }
    *p = NUL;
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * print the marks
 */
    ಶೂನ್ಯ
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ಯದಿ (arg != ರಿಕ್ತ && *arg == NUL)
	arg = ರಿಕ್ತ;

    show_one_mark('\'', arg, &curwin->w_pcmark, ರಿಕ್ತ, TRUE);
    ಪ್ರತಿ (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ರಿಕ್ತ, TRUE);
    ಪ್ರತಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ಯದಿ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	ಅನ್ಯಥಾ
	    name = namedfm[i].fname;
	ಯದಿ (name != ರಿಕ್ತ)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ಯದಿ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, ರಿಕ್ತ, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, ರಿಕ್ತ, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, ರಿಕ್ತ, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_ಸೇರಿಸು, ರಿಕ್ತ, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, ರಿಕ್ತ, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    ಯದಿ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    ಅನ್ಯಥಾ
	posp = endp;
    show_one_mark('<', arg, posp, ರಿಕ್ತ, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ರಿಕ್ತ, TRUE);

    show_one_mark(-1, arg, ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_one_mark(
    ಪೂರ್ಣಾಙ್ಕ		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ಪೂರ್ಣಾಙ್ಕ		current)	// in current file
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_title = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		mustfree = FALSE;
    char_u	*name = name_arg;

    ಯದಿ (c == -1)			    // finish up
    {
	ಯದಿ (did_title)
	    did_title = FALSE;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (arg == ರಿಕ್ತ)
		ಸಂದೇಶ(_("No marks set"));
	    ಅನ್ಯಥಾ
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    ಅನ್ಯಥಾ ಯದಿ (!got_int
	    && (arg == ರಿಕ್ತ || vim_strchr(arg, c) != ರಿಕ್ತ)
	    && p->lnum != 0)
    {
	ಯದಿ (name == ರಿಕ್ತ && current)
	{
	    name = mark_ಸಾಲು(p, 15);
	    ಯದಿ (name == ರಿಕ್ತ)
	    {
		emsg(_(e_out_of_memory));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    mustfree = TRUE;
	}
	ಯದಿ (!message_filtered(name))
	{
	    ಯದಿ (!did_title)
	    {
		// Highlight title
		ಸಂದೇಶ_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (!got_int)
	    {
		sprintf((ವರ್ಣ *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		ಸಂದೇಶ_outtrans(IObuff);
		ಯದಿ (name != ರಿಕ್ತ)
		{
		    ಸಂದೇಶ_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	ಯದಿ (mustfree)
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
    ಪೂರ್ಣಾಙ್ಕ		from, to;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		lower;
    ಪೂರ್ಣಾಙ್ಕ		digit;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    ಅನ್ಯಥಾ ಯದಿ (eap->forceit)
	emsg(_(e_invalid_argument));
    ಅನ್ಯಥಾ ಯದಿ (*eap->arg == NUL)
	emsg(_(e_argument_required));
    ಅನ್ಯಥಾ
    {
	// clear specified marks only
	ಪ್ರತಿ (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ಯದಿ (lower || digit || ASCII_ISUPPER(*p))
	    {
		ಯದಿ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    ಯದಿ (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			ಪ್ರತ್ಯಾಗಮ;
		    }
		    p += 2;
		}
		ಅನ್ಯಥಾ
		    // clear one lower case mark
		    from = to = *p;

		ಪ್ರತಿ (i = from; i <= to; ++i)
		{
		    ಯದಿ (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (digit)
			    n = i - '0' + NMARKS;
			ಅನ್ಯಥಾ
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#ifdef FEAT_VIMINFO
			namedfm[n].time_set = digit ? 0 : vim_time();
#endif
		    }
		}
	    }
	    ಅನ್ಯಥಾ
		ವಿಕಲ್ಪ (*p)
		{
		    ಪಕ್ಷ '"': curbuf->b_last_cursor.lnum = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ '^': curbuf->b_last_ಸೇರಿಸು.lnum = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ '.': curbuf->b_last_change.lnum = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ '[': curbuf->b_op_start.lnum    = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ ']': curbuf->b_op_end.lnum      = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ '<': curbuf->b_visual.vi_start.lnum = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ '>': curbuf->b_visual.vi_end.lnum   = 0; ಭಙ್ಗ;
		    ಪಕ್ಷ ' ': ಭಙ್ಗ;
		    ಸ್ವಭಾವ:  semsg(_(e_invalid_argument_str), p);
			      ಪ್ರತ್ಯಾಗಮ;
		}
	}
    }
}

/*
 * print the jumplist
 */
    ಶೂನ್ಯ
ex_jumps(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n jump line  col file/text"));
    ಪ್ರತಿ (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ಯದಿ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ಯದಿ (name == ರಿಕ್ತ && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ಯದಿ (name == ರಿಕ್ತ || message_filtered(name))
	    {
		vim_free(name);
		ಅನುವರ್ತ;
	    }

	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
	    {
		vim_free(name);
		ಭಙ್ಗ;
	    }
	    sprintf((ವರ್ಣ *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    ಸಂದೇಶ_outtrans(IObuff);
	    ಸಂದೇಶ_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ಯದಿ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	ಸಂದೇಶ_puts("\n>");
}

    ಶೂನ್ಯ
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    ಶೂನ್ಯ
ex_changes(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\nchange line  col text"));

    ಪ್ರತಿ (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ಯದಿ (curbuf->b_changelist[i].lnum != 0)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    sprintf((ವರ್ಣ *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    ಸಂದೇಶ_outtrans(IObuff);
	    name = mark_ಸಾಲು(&curbuf->b_changelist[i], 17);
	    ಯದಿ (name == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಸಂದೇಶ_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ಯದಿ (curwin->w_changelistidx == curbuf->b_changelistlen)
	ಸಂದೇಶ_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	ಯದಿ (*lp >= line1 && *lp <= line2) \
	{ \
	    ಯದಿ (amount == MAXLNUM) \
		*lp = 0; \
	    ಅನ್ಯಥಾ \
		*lp += amount; \
	} \
	ಅನ್ಯಥಾ ಯದಿ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	ಯದಿ (*lp >= line1 && *lp <= line2) \
	{ \
	    ಯದಿ (amount == MAXLNUM) \
		*lp = line1; \
	    ಅನ್ಯಥಾ \
		*lp += amount; \
	} \
	ಅನ್ಯಥಾ ಯದಿ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ಯದಿ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ಯದಿ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    ಅನ್ಯಥಾ /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	ಅನ್ಯಥಾ ಯದಿ (amount_after && posp->lnum > line2) \
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
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    ಶೂನ್ಯ
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    ಪೂರ್ಣಾಙ್ಕ		adjust_folds UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ಸ್ಥಿರ pos_T initpos = {1, 0, 0};

    ಯದಿ (line2 < line1 && amount_after == 0L)	    // nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	ಪ್ರತಿ (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ಯದಿ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	ಪ್ರತಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ಯದಿ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_ಸೇರಿಸು.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	ಯದಿ (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	ಪ್ರತಿ (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ರಿಕ್ತ, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#endif

#ifdef FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#endif
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_prev_pcmark.lnum));

    // saved cursor for formatting
    ಯದಿ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    ಪ್ರತಿ (i = 0; i < win->w_jumplistlen; ++i)
		ಯದಿ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ಯದಿ (win->w_buffer == curbuf)
	{
	    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		ಪ್ರತಿ (i = 0; i < win->w_tagstacklen; i++)
		    ಯದಿ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ಯದಿ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ಯದಿ (win != curwin)
	    {
		ಯದಿ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ಯದಿ (amount == MAXLNUM)	    // topline is deleted
		    {
			ಯದಿ (line1 <= 1)
			    win->w_topline = 1;
			ಅನ್ಯಥಾ
			    win->w_topline = line1 - 1;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		ಅನ್ಯಥಾ ಯದಿ (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#ifdef FEAT_FOLDING
	    // adjust folds
	    ಯದಿ (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#endif
	}
    }

#ifdef FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#endif

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#define col_adjust(pp) \
    { \
	posp = pp; \
	ಯದಿ (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ಯದಿ (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    ಅನ್ಯಥಾ ಯದಿ (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    ಅನ್ಯಥಾ \
		posp->col += col_amount; \
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
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    ಪೂರ್ಣಾಙ್ಕ		spaces_removed)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ಯದಿ ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	ಪ್ರತ್ಯಾಗಮ; // nothing to do

    // named marks, lower case and upper case
    ಪ್ರತಿ (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	ಯದಿ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    ಪ್ರತಿ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ಯದಿ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_ಸೇರಿಸು));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    ಪ್ರತಿ (i = 0; i < curbuf->b_changelistlen; ++i)
	col_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    col_adjust(&(curbuf->b_visual.vi_start));
    col_adjust(&(curbuf->b_visual.vi_end));

    // previous context mark
    col_adjust(&(curwin->w_pcmark));

    // previous pcmark
    col_adjust(&(curwin->w_prev_pcmark));

    // saved cursor for formatting
    col_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	ಪ್ರತಿ (i = 0; i < win->w_jumplistlen; ++i)
	    ಯದಿ (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	ಯದಿ (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    ಪ್ರತಿ (i = 0; i < win->w_tagstacklen; i++)
		ಯದಿ (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ಯದಿ (win != curwin)
		col_adjust(&win->w_cursor);
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
cleanup_jumplist(win_T *wp, ಪೂರ್ಣಾಙ್ಕ loadfiles)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    from, to;
    ಪೂರ್ಣಾಙ್ಕ	    mustfree;

    ಯದಿ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	ಪ್ರತಿ (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ಯದಿ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    ಪ್ರತಿ (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ಯದಿ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	ಪ್ರತಿ (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ಯದಿ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		ಭಙ್ಗ;
	ಯದಿ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	ಅನ್ಯಥಾ ಯದಿ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ರಾಶಿ);
	ಅನ್ಯಥಾ				// adjacent duplicate
	    mustfree = TRUE;

	ಯದಿ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	ಅನ್ಯಥಾ
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    ಯದಿ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    ಶೂನ್ಯ
copy_jumplist(win_T *from, win_T *to)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	ಯದಿ (from->w_jumplist[i].fname != ರಿಕ್ತ)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    ಶೂನ್ಯ
free_jumplist(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    ಶೂನ್ಯ
set_last_cursor(win_T *win)
{
    ಯದಿ (win->w_buffer != ರಿಕ್ತ)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_marks(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ಯದಿ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#ಯದಿ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ namedfm;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ಯದಿ (pos->lnum <= 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    d = dict_alloc();
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    lpos = list_alloc();
    ಯದಿ (lpos == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    list_append_ಸಂಖ್ಯೆ(lpos, bufnr);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->lnum);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_ಸಂಖ್ಯೆ(lpos, pos->coladd);

    ಯದಿ (dict_add_string_len(d, "mark", mname, (ಪೂರ್ಣಾಙ್ಕ)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != ರಿಕ್ತ && dict_add_string(d, "file", fname) == FAIL))
    {
	ಯದಿ (lpos->lv_refcount == 0)
	    list_free(lpos);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Marks 'a' to 'z'
    ಪ್ರತಿ (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ರಿಕ್ತ);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ರಿಕ್ತ);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_ಸೇರಿಸು, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, ರಿಕ್ತ);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, ರಿಕ್ತ);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    ಪ್ರತಿ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ಯದಿ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	ಅನ್ಯಥಾ
	    name = namedfm[i].fname;
	ಯದಿ (name != ರಿಕ್ತ)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ಯದಿ (namedfm[i].fmark.fnum != 0)
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
    buf_T	*buf = ರಿಕ್ತ;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	ಪ್ರತ್ಯಾಗಮ;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
