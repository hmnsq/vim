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

#अन्तर्भाव "vim.h"

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
स्थिर xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

स्थिर शून्य fname2fnum(xfmark_T *fm);
स्थिर शून्य fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
स्थिर char_u *mark_पङ्क्तिः(pos_T *mp, पूर्णाङ्क lead_len);
स्थिर शून्य show_one_mark(पूर्णाङ्क, char_u *, pos_T *, char_u *, पूर्णाङ्क current);
स्थिर शून्य mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, पूर्णाङ्क adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    पूर्णाङ्क
setmark(पूर्णाङ्क c)
{
    प्रत्यागम setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    पूर्णाङ्क
setmark_pos(पूर्णाङ्क c, pos_T *pos, पूर्णाङ्क fnum)
{
    पूर्णाङ्क		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    जइ (c < 0)
	प्रत्यागम FAIL;

    जइ (c == '\'' || c == '`')
    {
	जइ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	अण्णहा
	    curwin->w_pcmark = *pos;
	प्रत्यागम OK;
    }

    buf = buflist_findnr(fnum);
    जइ (buf == रिक्त)
	प्रत्यागम FAIL;

    जइ (c == '"')
    {
	buf->b_last_cursor = *pos;
	प्रत्यागम OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    जइ (c == '[')
    {
	buf->b_op_start = *pos;
	प्रत्यागम OK;
    }
    जइ (c == ']')
    {
	buf->b_op_end = *pos;
	प्रत्यागम OK;
    }

    जइ (c == '<' || c == '>')
    {
	जइ (c == '<')
	    buf->b_visual.vi_start = *pos;
	अण्णहा
	    buf->b_visual.vi_end = *pos;
	जइ (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	प्रत्यागम OK;
    }

    जइ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	प्रत्यागम OK;
    }
    जइ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	जइ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	अण्णहा
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	प्रत्यागम OK;
    }
    प्रत्यागम FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    शून्य
mark_forget_file(win_T *wp, पूर्णाङ्क fnum)
{
    पूर्णाङ्क		i;

    प्रति (i = wp->w_jumplistlen - 1; i >= 0; --i)
	जइ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    जइ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * परिमाण(wp->w_jumplist[i]));
	}

    प्रति (i = wp->w_tagstacklen - 1; i >= 0; --i)
	जइ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    जइ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * परिमाण(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    शून्य
setpcmark(शून्य)
{
    पूर्णाङ्क		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    जइ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	प्रत्यागम;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    जइ (jop_flags & JOP_राशिः)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	जइ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    जइ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	प्रति (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = रिक्त;
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
    शून्य
checkpcmark(शून्य)
{
    जइ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(पूर्णाङ्क गणनम्)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    जइ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	प्रत्यागम (pos_T *)रिक्त;

    प्रति (;;)
    {
	जइ (curwin->w_jumplistidx + गणनम् < 0
		|| curwin->w_jumplistidx + गणनम् >= curwin->w_jumplistlen)
	    प्रत्यागम (pos_T *)रिक्त;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	जइ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    जइ (curwin->w_jumplistidx + गणनम् < 0)
		प्रत्यागम (pos_T *)रिक्त;
	}

	curwin->w_jumplistidx += गणनम्;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	जइ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	जइ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    जइ (buflist_findnr(fmark.fnum) == रिक्त)
	    {					     // Skip this one ..
		गणनम् += गणनम् < 0 ? -1 : 1;
		अनुवर्त;
	    }
	    जइ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		प्रत्यागम (pos_T *)रिक्त;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	अण्णहा
	    pos = &(jmp->fmark.mark);
	प्रत्यागम pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(पूर्णाङ्क गणनम्)
{
    पूर्णाङ्क		n;

    जइ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	प्रत्यागम (pos_T *)रिक्त;

    n = curwin->w_changelistidx;
    जइ (n + गणनम् < 0)
    {
	जइ (n == 0)
	    प्रत्यागम (pos_T *)रिक्त;
	n = 0;
    }
    अण्णहा जइ (n + गणनम् >= curbuf->b_changelistlen)
    {
	जइ (n == curbuf->b_changelistlen - 1)
	    प्रत्यागम (pos_T *)रिक्त;
	n = curbuf->b_changelistlen - 1;
    }
    अण्णहा
	n += गणनम्;
    curwin->w_changelistidx = n;
    प्रत्यागम curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, पूर्णाङ्क c, पूर्णाङ्क changefile)
{
    प्रत्यागम getmark_buf_fnum(buf, c, changefile, रिक्त);
}

    pos_T *
getmark(पूर्णाङ्क c, पूर्णाङ्क changefile)
{
    प्रत्यागम getmark_buf_fnum(curbuf, c, changefile, रिक्त);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    पूर्णाङ्क		c,
    पूर्णाङ्क		changefile,
    पूर्णाङ्क		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    स्थिर pos_T	pos_copy;

    posp = रिक्त;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    जइ (c < 0)
	प्रत्यागम posp;
    जइ (c > '~')			// check for islower()/isupper()
	;
    अण्णहा जइ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    अण्णहा जइ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    अण्णहा जइ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_निवेशय);
    अण्णहा जइ (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    अण्णहा जइ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    अण्णहा जइ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    अण्णहा जइ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	पूर्णाङ्क	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	जइ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    अण्णहा जइ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	पूर्णाङ्क	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	जइ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    अण्णहा जइ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	जइ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	अण्णहा
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	जइ (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    जइ (c == '<')
		pos_copy.col = 0;
	    अण्णहा
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    अण्णहा जइ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    अण्णहा जइ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	जइ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	अण्णहा
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	जइ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	जइ (fnum != रिक्त)
	    *fnum = namedfm[c].fmark.fnum;
	अण्णहा जइ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    जइ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		जइ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    प्रत्यागम (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    अण्णहा
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    प्रत्यागम posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    पूर्णाङ्क		dir,	// direction for search
    पूर्णाङ्क		begin_पङ्क्तिः)
{
    पूर्णाङ्क		i;
    pos_T	*result = रिक्त;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    जइ (dir == BACKWARD && begin_पङ्क्तिः)
	pos.col = 0;
    अण्णहा जइ (dir == FORWARD && begin_पङ्क्तिः)
	pos.col = MAXCOL;

    प्रति (i = 0; i < NMARKS; i++)
    {
	जइ (curbuf->b_namedm[i].lnum > 0)
	{
	    जइ (dir == FORWARD)
	    {
		जइ ((result == रिक्त || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    अण्णहा
	    {
		जइ ((result == रिक्त || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    प्रत्यागम result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    स्थिर शून्य
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    जइ (fm->fname == रिक्त)
	प्रत्यागम;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    जइ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    अण्णहा
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (शून्य)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    शून्य
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    पूर्णाङ्क		i;
    win_T	*wp;

    जइ (buf->b_ffname == रिक्त)
	प्रत्यागम;

    name = home_replace_save(buf, buf->b_ffname);
    जइ (name == रिक्त)
	प्रत्यागम;

    प्रति (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	प्रति (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    स्थिर शून्य
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    जइ (fm->fmark.fnum == 0
	    && fm->fname != रिक्त
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
    पूर्णाङ्क
check_mark(pos_T *pos)
{
    जइ (pos == रिक्त)
    {
	emsg(_(e_unknown_mark));
	प्रत्यागम FAIL;
    }
    जइ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	जइ (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	प्रत्यागम FAIL;
    }
    जइ (pos->lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	emsg(_(e_mark_has_invalid_पङ्क्तिः_सङ्ख्या));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    शून्य
clrallmarks(buf_T *buf)
{
    स्थिर पूर्णाङ्क		i = -1;

    जइ (i == -1)	// first call ever: initialize
	प्रति (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = रिक्त;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    प्रति (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_निवेशय.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, पूर्णाङ्क lead_len)
{
    जइ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	प्रत्यागम mark_पङ्क्तिः(&(fmark->mark), lead_len);
    प्रत्यागम buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    स्थिर char_u *
mark_पङ्क्तिः(pos_T *mp, पूर्णाङ्क lead_len)
{
    char_u	*s, *p;
    पूर्णाङ्क		len;

    जइ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	प्रत्यागम vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    जइ (s == रिक्त)
	प्रत्यागम रिक्त;
    // Truncate the line to fit it in the window.
    len = 0;
    प्रति (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	जइ (len >= Columns - lead_len)
	    भङ्ग;
    }
    *p = NUL;
    प्रत्यागम s;
}

/*
 * print the marks
 */
    शून्य
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णाङ्क		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    जइ (arg != रिक्त && *arg == NUL)
	arg = रिक्त;

    show_one_mark('\'', arg, &curwin->w_pcmark, रिक्त, TRUE);
    प्रति (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], रिक्त, TRUE);
    प्रति (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	जइ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	अण्णहा
	    name = namedfm[i].fname;
	जइ (name != रिक्त)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    जइ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, रिक्त, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, रिक्त, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, रिक्त, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_निवेशय, रिक्त, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, रिक्त, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    जइ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    अण्णहा
	posp = endp;
    show_one_mark('<', arg, posp, रिक्त, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, रिक्त, TRUE);

    show_one_mark(-1, arg, रिक्त, रिक्त, FALSE);
}

    स्थिर शून्य
show_one_mark(
    पूर्णाङ्क		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    पूर्णाङ्क		current)	// in current file
{
    स्थिर पूर्णाङ्क	did_title = FALSE;
    पूर्णाङ्क		mustfree = FALSE;
    char_u	*name = name_arg;

    जइ (c == -1)			    // finish up
    {
	जइ (did_title)
	    did_title = FALSE;
	अण्णहा
	{
	    जइ (arg == रिक्त)
		सन्देशः(_("No marks set"));
	    अण्णहा
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    अण्णहा जइ (!got_int
	    && (arg == रिक्त || vim_strchr(arg, c) != रिक्त)
	    && p->lnum != 0)
    {
	जइ (name == रिक्त && current)
	{
	    name = mark_पङ्क्तिः(p, 15);
	    जइ (name == रिक्त)
	    {
		emsg(_(e_out_of_memory));
		प्रत्यागम;
	    }
	    mustfree = TRUE;
	}
	जइ (!message_filtered(name))
	{
	    जइ (!did_title)
	    {
		// Highlight title
		सन्देशः_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    सन्देशः_putchar('\n');
	    जइ (!got_int)
	    {
		sprintf((वर्ण *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		सन्देशः_outtrans(IObuff);
		जइ (name != रिक्त)
		{
		    सन्देशः_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	जइ (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    शून्य
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    पूर्णाङ्क		from, to;
    पूर्णाङ्क		i;
    पूर्णाङ्क		lower;
    पूर्णाङ्क		digit;
    पूर्णाङ्क		n;

    जइ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    अण्णहा जइ (eap->forceit)
	emsg(_(e_invalid_argument));
    अण्णहा जइ (*eap->arg == NUL)
	emsg(_(e_argument_required));
    अण्णहा
    {
	// clear specified marks only
	प्रति (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    जइ (lower || digit || ASCII_ISUPPER(*p))
	    {
		जइ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    जइ (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			प्रत्यागम;
		    }
		    p += 2;
		}
		अण्णहा
		    // clear one lower case mark
		    from = to = *p;

		प्रति (i = from; i <= to; ++i)
		{
		    जइ (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    अण्णहा
		    {
			जइ (digit)
			    n = i - '0' + NMARKS;
			अण्णहा
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
	    अण्णहा
		विकल्प (*p)
		{
		    पक्ष '"': curbuf->b_last_cursor.lnum = 0; भङ्ग;
		    पक्ष '^': curbuf->b_last_निवेशय.lnum = 0; भङ्ग;
		    पक्ष '.': curbuf->b_last_change.lnum = 0; भङ्ग;
		    पक्ष '[': curbuf->b_op_start.lnum    = 0; भङ्ग;
		    पक्ष ']': curbuf->b_op_end.lnum      = 0; भङ्ग;
		    पक्ष '<': curbuf->b_visual.vi_start.lnum = 0; भङ्ग;
		    पक्ष '>': curbuf->b_visual.vi_end.lnum   = 0; भङ्ग;
		    पक्ष ' ': भङ्ग;
		    स्वभाव:  semsg(_(e_invalid_argument_str), p);
			      प्रत्यागम;
		}
	}
    }
}

/*
 * print the jumplist
 */
    शून्य
ex_jumps(exarg_T *eap UNUSED)
{
    पूर्णाङ्क		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    सन्देशः_puts_title(_("\n jump line  col file/text"));
    प्रति (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	जइ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    जइ (name == रिक्त && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    जइ (name == रिक्त || message_filtered(name))
	    {
		vim_free(name);
		अनुवर्त;
	    }

	    सन्देशः_putchar('\n');
	    जइ (got_int)
	    {
		vim_free(name);
		भङ्ग;
	    }
	    sprintf((वर्ण *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    सन्देशः_outtrans(IObuff);
	    सन्देशः_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    जइ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	सन्देशः_puts("\n>");
}

    शून्य
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    शून्य
ex_changes(exarg_T *eap UNUSED)
{
    पूर्णाङ्क		i;
    char_u	*name;

    // Highlight title
    सन्देशः_puts_title(_("\nchange line  col text"));

    प्रति (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	जइ (curbuf->b_changelist[i].lnum != 0)
	{
	    सन्देशः_putchar('\n');
	    जइ (got_int)
		भङ्ग;
	    sprintf((वर्ण *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    सन्देशः_outtrans(IObuff);
	    name = mark_पङ्क्तिः(&curbuf->b_changelist[i], 17);
	    जइ (name == रिक्त)
		भङ्ग;
	    सन्देशः_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    जइ (curwin->w_changelistidx == curbuf->b_changelistlen)
	सन्देशः_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	जइ (*lp >= line1 && *lp <= line2) \
	{ \
	    जइ (amount == MAXLNUM) \
		*lp = 0; \
	    अण्णहा \
		*lp += amount; \
	} \
	अण्णहा जइ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	जइ (*lp >= line1 && *lp <= line2) \
	{ \
	    जइ (amount == MAXLNUM) \
		*lp = line1; \
	    अण्णहा \
		*lp += amount; \
	} \
	अण्णहा जइ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	जइ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    जइ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    अण्णहा /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	अण्णहा जइ (amount_after && posp->lnum > line2) \
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
    शून्य
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    शून्य
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    स्थिर शून्य
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    पूर्णाङ्क		adjust_folds UNUSED)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    स्थिर pos_T initpos = {1, 0, 0};

    जइ (line2 < line1 && amount_after == 0L)	    // nothing to do
	प्रत्यागम;

    जइ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	प्रति (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    जइ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	प्रति (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    जइ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_निवेशय.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	जइ (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	प्रति (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(रिक्त, line1, line2, amount, amount_after);
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
    जइ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	जइ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    प्रति (i = 0; i < win->w_jumplistlen; ++i)
		जइ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	जइ (win->w_buffer == curbuf)
	{
	    जइ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		प्रति (i = 0; i < win->w_tagstacklen; i++)
		    जइ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    जइ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    जइ (win != curwin)
	    {
		जइ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    जइ (amount == MAXLNUM)	    // topline is deleted
		    {
			जइ (line1 <= 1)
			    win->w_topline = 1;
			अण्णहा
			    win->w_topline = line1 - 1;
		    }
		    अण्णहा जइ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		अण्णहा जइ (amount_after && win->w_topline > line2)
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
	    जइ (adjust_folds)
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
	जइ (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    जइ (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    अण्णहा जइ (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    अण्णहा \
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
    शून्य
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    पूर्णाङ्क		spaces_removed)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    जइ ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	प्रत्यागम; // nothing to do

    // named marks, lower case and upper case
    प्रति (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	जइ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    प्रति (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	जइ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_निवेशय));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    प्रति (i = 0; i < curbuf->b_changelistlen; ++i)
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
	प्रति (i = 0; i < win->w_jumplistlen; ++i)
	    जइ (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	जइ (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    प्रति (i = 0; i < win->w_tagstacklen; i++)
		जइ (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    जइ (win != curwin)
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
    शून्य
cleanup_jumplist(win_T *wp, पूर्णाङ्क loadfiles)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    from, to;
    पूर्णाङ्क	    mustfree;

    जइ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	प्रति (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    जइ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    प्रति (from = 0; from < wp->w_jumplistlen; ++from)
    {
	जइ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	प्रति (i = from + 1; i < wp->w_jumplistlen; ++i)
	    जइ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		भङ्ग;
	जइ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	अण्णहा जइ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_राशिः);
	अण्णहा				// adjacent duplicate
	    mustfree = TRUE;

	जइ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	अण्णहा
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    जइ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    शून्य
copy_jumplist(win_T *from, win_T *to)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	जइ (from->w_jumplist[i].fname != रिक्त)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    शून्य
free_jumplist(win_T *wp)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    शून्य
set_last_cursor(win_T *win)
{
    जइ (win->w_buffer != रिक्त)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#जइ defined(EXITFREE)
    शून्य
free_all_marks(शून्य)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	जइ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#जइ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(शून्य)
{
    प्रत्यागम namedfm;
}
#endif

#जइ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    स्थिर पूर्णाङ्क
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, पूर्णाङ्क bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    जइ (pos->lnum <= 0)
	प्रत्यागम OK;

    d = dict_alloc();
    जइ (d == रिक्त)
	प्रत्यागम FAIL;

    जइ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	प्रत्यागम FAIL;
    }

    lpos = list_alloc();
    जइ (lpos == रिक्त)
	प्रत्यागम FAIL;

    list_append_सङ्ख्या(lpos, bufnr);
    list_append_सङ्ख्या(lpos, pos->lnum);
    list_append_सङ्ख्या(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_सङ्ख्या(lpos, pos->coladd);

    जइ (dict_add_string_len(d, "mark", mname, (पूर्णाङ्क)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != रिक्त && dict_add_string(d, "file", fname) == FAIL))
    {
	जइ (lpos->lv_refcount == 0)
	    list_free(lpos);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Get information about marks local to a buffer.
 */
    स्थिर शून्य
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    पूर्णाङ्क		i;

    // Marks 'a' to 'z'
    प्रति (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, रिक्त);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, रिक्त);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_निवेशय, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, रिक्त);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    स्थिर शून्य
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    पूर्णाङ्क		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    प्रति (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	जइ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	अण्णहा
	    name = namedfm[i].fname;
	जइ (name != रिक्त)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    जइ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    शून्य
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = रिक्त;

    जइ (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    जइ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	प्रत्यागम;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    जइ (buf == रिक्त)
	प्रत्यागम;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
