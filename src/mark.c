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
स्थिर char_u *mark_ओळ(pos_T *mp, पूर्णांक lead_len);
स्थिर शून्य show_one_mark(पूर्णांक, char_u *, pos_T *, char_u *, पूर्णांक current);
स्थिर शून्य mark_adjust_internal(linenr_T line1, linenr_T line2, दीर्घ amount,
    दीर्घ amount_after, पूर्णांक adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    पूर्णांक
setmark(पूर्णांक c)
{
    परत setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    पूर्णांक
setmark_pos(पूर्णांक c, pos_T *pos, पूर्णांक fnum)
{
    पूर्णांक		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    जर (c < 0)
	परत FAIL;

    जर (c == '\'' || c == '`')
    {
	जर (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	अन्यथा
	    curwin->w_pcmark = *pos;
	परत OK;
    }

    buf = buflist_findnr(fnum);
    जर (buf == रिक्त)
	परत FAIL;

    जर (c == '"')
    {
	buf->b_last_cursor = *pos;
	परत OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    जर (c == '[')
    {
	buf->b_op_start = *pos;
	परत OK;
    }
    जर (c == ']')
    {
	buf->b_op_end = *pos;
	परत OK;
    }

    जर (c == '<' || c == '>')
    {
	जर (c == '<')
	    buf->b_visual.vi_start = *pos;
	अन्यथा
	    buf->b_visual.vi_end = *pos;
	जर (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	परत OK;
    }

    जर (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	परत OK;
    }
    जर (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	जर (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	अन्यथा
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	परत OK;
    }
    परत FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    शून्य
mark_forget_file(win_T *wp, पूर्णांक fnum)
{
    पूर्णांक		i;

    साठी (i = wp->w_jumplistlen - 1; i >= 0; --i)
	जर (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    जर (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * आकार(wp->w_jumplist[i]));
	}

    साठी (i = wp->w_tagstacklen - 1; i >= 0; --i)
	जर (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    जर (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * आकार(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    शून्य
setpcmark(शून्य)
{
    पूर्णांक		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    जर (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	परत;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    जर (jop_flags & JOP_चळत)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	जर (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    जर (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	साठी (i = 1; i < JUMPLISTSIZE; ++i)
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
    जर (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(पूर्णांक मोजणी)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    जर (curwin->w_jumplistlen == 0)	    // nothing to jump to
	परत (pos_T *)रिक्त;

    साठी (;;)
    {
	जर (curwin->w_jumplistidx + मोजणी < 0
		|| curwin->w_jumplistidx + मोजणी >= curwin->w_jumplistlen)
	    परत (pos_T *)रिक्त;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	जर (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    जर (curwin->w_jumplistidx + मोजणी < 0)
		परत (pos_T *)रिक्त;
	}

	curwin->w_jumplistidx += मोजणी;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	जर (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	जर (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    जर (buflist_findnr(fmark.fnum) == रिक्त)
	    {					     // Skip this one ..
		मोजणी += मोजणी < 0 ? -1 : 1;
		सुरूठेवा;
	    }
	    जर (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		परत (pos_T *)रिक्त;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	अन्यथा
	    pos = &(jmp->fmark.mark);
	परत pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(पूर्णांक मोजणी)
{
    पूर्णांक		n;

    जर (curbuf->b_changelistlen == 0)	    // nothing to jump to
	परत (pos_T *)रिक्त;

    n = curwin->w_changelistidx;
    जर (n + मोजणी < 0)
    {
	जर (n == 0)
	    परत (pos_T *)रिक्त;
	n = 0;
    }
    अन्यथा जर (n + मोजणी >= curbuf->b_changelistlen)
    {
	जर (n == curbuf->b_changelistlen - 1)
	    परत (pos_T *)रिक्त;
	n = curbuf->b_changelistlen - 1;
    }
    अन्यथा
	n += मोजणी;
    curwin->w_changelistidx = n;
    परत curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, पूर्णांक c, पूर्णांक changefile)
{
    परत getmark_buf_fnum(buf, c, changefile, रिक्त);
}

    pos_T *
getmark(पूर्णांक c, पूर्णांक changefile)
{
    परत getmark_buf_fnum(curbuf, c, changefile, रिक्त);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    पूर्णांक		c,
    पूर्णांक		changefile,
    पूर्णांक		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    स्थिर pos_T	pos_copy;

    posp = रिक्त;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    जर (c < 0)
	परत posp;
    जर (c > '~')			// check for islower()/isupper()
	;
    अन्यथा जर (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    अन्यथा जर (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    अन्यथा जर (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_घाला);
    अन्यथा जर (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    अन्यथा जर (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    अन्यथा जर (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    अन्यथा जर (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	पूर्णांक	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	जर (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    अन्यथा जर (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	पूर्णांक	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	जर (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    अन्यथा जर (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	जर (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	अन्यथा
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	जर (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    जर (c == '<')
		pos_copy.col = 0;
	    अन्यथा
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    अन्यथा जर (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    अन्यथा जर (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	जर (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	अन्यथा
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	जर (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	जर (fnum != रिक्त)
	    *fnum = namedfm[c].fmark.fnum;
	अन्यथा जर (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    जर (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		जर (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    परत (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    अन्यथा
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    परत posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    पूर्णांक		dir,	// direction for search
    पूर्णांक		begin_ओळ)
{
    पूर्णांक		i;
    pos_T	*result = रिक्त;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    जर (dir == BACKWARD && begin_ओळ)
	pos.col = 0;
    अन्यथा जर (dir == FORWARD && begin_ओळ)
	pos.col = MAXCOL;

    साठी (i = 0; i < NMARKS; i++)
    {
	जर (curbuf->b_namedm[i].lnum > 0)
	{
	    जर (dir == FORWARD)
	    {
		जर ((result == रिक्त || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    अन्यथा
	    {
		जर ((result == रिक्त || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    परत result;
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

    जर (fm->fname == रिक्त)
	परत;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    जर (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	माप_प्रकार len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    अन्यथा
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
    पूर्णांक		i;
    win_T	*wp;

    जर (buf->b_ffname == रिक्त)
	परत;

    name = home_replace_save(buf, buf->b_ffname);
    जर (name == रिक्त)
	परत;

    साठी (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	साठी (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    स्थिर शून्य
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    जर (fm->fmark.fnum == 0
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
    पूर्णांक
check_mark(pos_T *pos)
{
    जर (pos == रिक्त)
    {
	emsg(_(e_unknown_mark));
	परत FAIL;
    }
    जर (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	जर (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	परत FAIL;
    }
    जर (pos->lnum > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	emsg(_(e_mark_has_invalid_ओळ_संख्या));
	परत FAIL;
    }
    परत OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    शून्य
clrallmarks(buf_T *buf)
{
    स्थिर पूर्णांक		i = -1;

    जर (i == -1)	// first call ever: initialize
	साठी (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = रिक्त;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    साठी (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_घाला.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, पूर्णांक lead_len)
{
    जर (fmark->fnum == curbuf->b_fnum)		    // current buffer
	परत mark_ओळ(&(fmark->mark), lead_len);
    परत buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    स्थिर char_u *
mark_ओळ(pos_T *mp, पूर्णांक lead_len)
{
    char_u	*s, *p;
    पूर्णांक		len;

    जर (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	परत vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    जर (s == रिक्त)
	परत रिक्त;
    // Truncate the line to fit it in the window.
    len = 0;
    साठी (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	जर (len >= Columns - lead_len)
	    थांबा;
    }
    *p = NUL;
    परत s;
}

/*
 * print the marks
 */
    शून्य
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णांक		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    जर (arg != रिक्त && *arg == NUL)
	arg = रिक्त;

    show_one_mark('\'', arg, &curwin->w_pcmark, रिक्त, TRUE);
    साठी (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], रिक्त, TRUE);
    साठी (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	जर (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	अन्यथा
	    name = namedfm[i].fname;
	जर (name != रिक्त)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    जर (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, रिक्त, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, रिक्त, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, रिक्त, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_घाला, रिक्त, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, रिक्त, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    जर ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    अन्यथा
	posp = endp;
    show_one_mark('<', arg, posp, रिक्त, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, रिक्त, TRUE);

    show_one_mark(-1, arg, रिक्त, रिक्त, FALSE);
}

    स्थिर शून्य
show_one_mark(
    पूर्णांक		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    पूर्णांक		current)	// in current file
{
    स्थिर पूर्णांक	did_title = FALSE;
    पूर्णांक		mustfree = FALSE;
    char_u	*name = name_arg;

    जर (c == -1)			    // finish up
    {
	जर (did_title)
	    did_title = FALSE;
	अन्यथा
	{
	    जर (arg == रिक्त)
		संदेश(_("No marks set"));
	    अन्यथा
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    अन्यथा जर (!got_int
	    && (arg == रिक्त || vim_strchr(arg, c) != रिक्त)
	    && p->lnum != 0)
    {
	जर (name == रिक्त && current)
	{
	    name = mark_ओळ(p, 15);
	    जर (name == रिक्त)
	    {
		emsg(_(e_out_of_memory));
		परत;
	    }
	    mustfree = TRUE;
	}
	जर (!message_filtered(name))
	{
	    जर (!did_title)
	    {
		// Highlight title
		संदेश_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    संदेश_putchar('\n');
	    जर (!got_int)
	    {
		प्र_लिखो((अक्षर *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		संदेश_outtrans(IObuff);
		जर (name != रिक्त)
		{
		    संदेश_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	जर (mustfree)
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
    पूर्णांक		from, to;
    पूर्णांक		i;
    पूर्णांक		lower;
    पूर्णांक		digit;
    पूर्णांक		n;

    जर (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    अन्यथा जर (eap->forceit)
	emsg(_(e_invalid_argument));
    अन्यथा जर (*eap->arg == NUL)
	emsg(_(e_argument_required));
    अन्यथा
    {
	// clear specified marks only
	साठी (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    जर (lower || digit || ASCII_ISUPPER(*p))
	    {
		जर (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    जर (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			परत;
		    }
		    p += 2;
		}
		अन्यथा
		    // clear one lower case mark
		    from = to = *p;

		साठी (i = from; i <= to; ++i)
		{
		    जर (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    अन्यथा
		    {
			जर (digit)
			    n = i - '0' + NMARKS;
			अन्यथा
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
	    अन्यथा
		निवड (*p)
		{
		    स्थिती '"': curbuf->b_last_cursor.lnum = 0; थांबा;
		    स्थिती '^': curbuf->b_last_घाला.lnum = 0; थांबा;
		    स्थिती '.': curbuf->b_last_change.lnum = 0; थांबा;
		    स्थिती '[': curbuf->b_op_start.lnum    = 0; थांबा;
		    स्थिती ']': curbuf->b_op_end.lnum      = 0; थांबा;
		    स्थिती '<': curbuf->b_visual.vi_start.lnum = 0; थांबा;
		    स्थिती '>': curbuf->b_visual.vi_end.lnum   = 0; थांबा;
		    स्थिती ' ': थांबा;
		    पूर्वनिर्धारित:  semsg(_(e_invalid_argument_str), p);
			      परत;
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
    पूर्णांक		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    संदेश_puts_title(_("\n jump line  col file/text"));
    साठी (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	जर (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    जर (name == रिक्त && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    जर (name == रिक्त || message_filtered(name))
	    {
		vim_free(name);
		सुरूठेवा;
	    }

	    संदेश_putchar('\n');
	    जर (got_int)
	    {
		vim_free(name);
		थांबा;
	    }
	    प्र_लिखो((अक्षर *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    संदेश_outtrans(IObuff);
	    संदेश_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    जर (curwin->w_jumplistidx == curwin->w_jumplistlen)
	संदेश_puts("\n>");
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
    पूर्णांक		i;
    char_u	*name;

    // Highlight title
    संदेश_puts_title(_("\nchange line  col text"));

    साठी (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	जर (curbuf->b_changelist[i].lnum != 0)
	{
	    संदेश_putchar('\n');
	    जर (got_int)
		थांबा;
	    प्र_लिखो((अक्षर *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (दीर्घ)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    संदेश_outtrans(IObuff);
	    name = mark_ओळ(&curbuf->b_changelist[i], 17);
	    जर (name == रिक्त)
		थांबा;
	    संदेश_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    जर (curwin->w_changelistidx == curbuf->b_changelistlen)
	संदेश_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	जर (*lp >= line1 && *lp <= line2) \
	{ \
	    जर (amount == MAXLNUM) \
		*lp = 0; \
	    अन्यथा \
		*lp += amount; \
	} \
	अन्यथा जर (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	जर (*lp >= line1 && *lp <= line2) \
	{ \
	    जर (amount == MAXLNUM) \
		*lp = line1; \
	    अन्यथा \
		*lp += amount; \
	} \
	अन्यथा जर (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	जर (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    जर (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    अन्यथा /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	अन्यथा जर (amount_after && posp->lnum > line2) \
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
    दीर्घ	amount,
    दीर्घ	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    शून्य
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    दीर्घ	amount,
    दीर्घ	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    स्थिर शून्य
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    दीर्घ	amount,
    दीर्घ	amount_after,
    पूर्णांक		adjust_folds UNUSED)
{
    पूर्णांक		i;
    पूर्णांक		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    स्थिर pos_T initpos = {1, 0, 0};

    जर (line2 < line1 && amount_after == 0L)	    // nothing to do
	परत;

    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	साठी (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    जर (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	साठी (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    जर (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_घाला.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	जर (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	साठी (i = 0; i < curbuf->b_changelistlen; ++i)
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
    जर (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    साठी (i = 0; i < win->w_jumplistlen; ++i)
		जर (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	जर (win->w_buffer == curbuf)
	{
	    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		साठी (i = 0; i < win->w_tagstacklen; i++)
		    जर (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    जर (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    जर (win != curwin)
	    {
		जर (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    जर (amount == MAXLNUM)	    // topline is deleted
		    {
			जर (line1 <= 1)
			    win->w_topline = 1;
			अन्यथा
			    win->w_topline = line1 - 1;
		    }
		    अन्यथा जर (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		अन्यथा जर (amount_after && win->w_topline > line2)
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
	    जर (adjust_folds)
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
	जर (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    जर (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    अन्यथा जर (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    अन्यथा \
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
    दीर्घ	lnum_amount,
    दीर्घ	col_amount,
    पूर्णांक		spaces_removed)
{
    पूर्णांक		i;
    पूर्णांक		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    जर ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	परत; // nothing to do

    // named marks, lower case and upper case
    साठी (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	जर (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    साठी (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	जर (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_घाला));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    साठी (i = 0; i < curbuf->b_changelistlen; ++i)
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
	साठी (i = 0; i < win->w_jumplistlen; ++i)
	    जर (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	जर (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    साठी (i = 0; i < win->w_tagstacklen; i++)
		जर (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    जर (win != curwin)
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
cleanup_jumplist(win_T *wp, पूर्णांक loadfiles)
{
    पूर्णांक	    i;
    पूर्णांक	    from, to;
    पूर्णांक	    mustfree;

    जर (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	साठी (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    जर ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    साठी (from = 0; from < wp->w_jumplistlen; ++from)
    {
	जर (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	साठी (i = from + 1; i < wp->w_jumplistlen; ++i)
	    जर (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		थांबा;
	जर (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	अन्यथा जर (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_चळत);
	अन्यथा				// adjacent duplicate
	    mustfree = TRUE;

	जर (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	अन्यथा
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    जर (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    शून्य
copy_jumplist(win_T *from, win_T *to)
{
    पूर्णांक		i;

    साठी (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	जर (from->w_jumplist[i].fname != रिक्त)
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
    पूर्णांक		i;

    साठी (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    शून्य
set_last_cursor(win_T *win)
{
    जर (win->w_buffer != रिक्त)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#जर defined(EXITFREE)
    शून्य
free_all_marks(शून्य)
{
    पूर्णांक		i;

    साठी (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	जर (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#जर defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(शून्य)
{
    परत namedfm;
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    स्थिर पूर्णांक
add_mark(list_T *l, char_u *mname, माप_प्रकार mnamelen, pos_T *pos, पूर्णांक bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    जर (pos->lnum <= 0)
	परत OK;

    d = dict_alloc();
    जर (d == रिक्त)
	परत FAIL;

    जर (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	परत FAIL;
    }

    lpos = list_alloc();
    जर (lpos == रिक्त)
	परत FAIL;

    list_append_संख्या(lpos, bufnr);
    list_append_संख्या(lpos, pos->lnum);
    list_append_संख्या(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_संख्या(lpos, pos->coladd);

    जर (dict_add_string_len(d, "mark", mname, (पूर्णांक)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != रिक्त && dict_add_string(d, "file", fname) == FAIL))
    {
	जर (lpos->lv_refcount == 0)
	    list_free(lpos);
	परत FAIL;
    }

    परत OK;
}

/*
 * Get information about marks local to a buffer.
 */
    स्थिर शून्य
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    माप_प्रकार	mnamelen = STRLEN_LITERAL("' ");
    पूर्णांक		i;

    // Marks 'a' to 'z'
    साठी (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, रिक्त);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, रिक्त);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, रिक्त);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_घाला, buf->b_fnum, रिक्त);
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
    माप_प्रकार	mnamelen = STRLEN_LITERAL("' ");
    पूर्णांक		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    साठी (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	जर (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	अन्यथा
	    name = namedfm[i].fname;
	जर (name != रिक्त)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    जर (namedfm[i].fmark.fnum != 0)
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

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	परत;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	परत;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
