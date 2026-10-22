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

#அந்தர்பாவ "vim.h"

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
நிலையான xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

நிலையான வெறுமை fname2fnum(xfmark_T *fm);
நிலையான வெறுமை fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
நிலையான char_u *mark_வரி(pos_T *mp, முழு lead_len);
நிலையான வெறுமை show_one_mark(முழு, char_u *, pos_T *, char_u *, முழு current);
நிலையான வெறுமை mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, முழு adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    முழு
setmark(முழு c)
{
    திரும்பு setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    முழு
setmark_pos(முழு c, pos_T *pos, முழு fnum)
{
    முழு		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    எனில் (c < 0)
	திரும்பு FAIL;

    எனில் (c == '\'' || c == '`')
    {
	எனில் (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	இல்லையெனில்
	    curwin->w_pcmark = *pos;
	திரும்பு OK;
    }

    buf = buflist_findnr(fnum);
    எனில் (buf == வெற்று)
	திரும்பு FAIL;

    எனில் (c == '"')
    {
	buf->b_last_cursor = *pos;
	திரும்பு OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    எனில் (c == '[')
    {
	buf->b_op_start = *pos;
	திரும்பு OK;
    }
    எனில் (c == ']')
    {
	buf->b_op_end = *pos;
	திரும்பு OK;
    }

    எனில் (c == '<' || c == '>')
    {
	எனில் (c == '<')
	    buf->b_visual.vi_start = *pos;
	இல்லையெனில்
	    buf->b_visual.vi_end = *pos;
	எனில் (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	திரும்பு OK;
    }

    எனில் (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	திரும்பு OK;
    }
    எனில் (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	எனில் (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	இல்லையெனில்
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	திரும்பு OK;
    }
    திரும்பு FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    வெறுமை
mark_forget_file(win_T *wp, முழு fnum)
{
    முழு		i;

    க்கு (i = wp->w_jumplistlen - 1; i >= 0; --i)
	எனில் (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    எனில் (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * அளவு(wp->w_jumplist[i]));
	}

    க்கு (i = wp->w_tagstacklen - 1; i >= 0; --i)
	எனில் (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    எனில் (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * அளவு(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    வெறுமை
setpcmark(வெறுமை)
{
    முழு		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    எனில் (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	திரும்பு;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    எனில் (jop_flags & JOP_அடுக்கு)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	எனில் (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    எனில் (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	க்கு (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = வெற்று;
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
    வெறுமை
checkpcmark(வெறுமை)
{
    எனில் (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(முழு மொத்தம்)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    எனில் (curwin->w_jumplistlen == 0)	    // nothing to jump to
	திரும்பு (pos_T *)வெற்று;

    க்கு (;;)
    {
	எனில் (curwin->w_jumplistidx + மொத்தம் < 0
		|| curwin->w_jumplistidx + மொத்தம் >= curwin->w_jumplistlen)
	    திரும்பு (pos_T *)வெற்று;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	எனில் (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    எனில் (curwin->w_jumplistidx + மொத்தம் < 0)
		திரும்பு (pos_T *)வெற்று;
	}

	curwin->w_jumplistidx += மொத்தம்;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	எனில் (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	எனில் (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    எனில் (buflist_findnr(fmark.fnum) == வெற்று)
	    {					     // Skip this one ..
		மொத்தம் += மொத்தம் < 0 ? -1 : 1;
		தொடர்;
	    }
	    எனில் (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		திரும்பு (pos_T *)வெற்று;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	இல்லையெனில்
	    pos = &(jmp->fmark.mark);
	திரும்பு pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(முழு மொத்தம்)
{
    முழு		n;

    எனில் (curbuf->b_changelistlen == 0)	    // nothing to jump to
	திரும்பு (pos_T *)வெற்று;

    n = curwin->w_changelistidx;
    எனில் (n + மொத்தம் < 0)
    {
	எனில் (n == 0)
	    திரும்பு (pos_T *)வெற்று;
	n = 0;
    }
    இல்லையெனில் எனில் (n + மொத்தம் >= curbuf->b_changelistlen)
    {
	எனில் (n == curbuf->b_changelistlen - 1)
	    திரும்பு (pos_T *)வெற்று;
	n = curbuf->b_changelistlen - 1;
    }
    இல்லையெனில்
	n += மொத்தம்;
    curwin->w_changelistidx = n;
    திரும்பு curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, முழு c, முழு changefile)
{
    திரும்பு getmark_buf_fnum(buf, c, changefile, வெற்று);
}

    pos_T *
getmark(முழு c, முழு changefile)
{
    திரும்பு getmark_buf_fnum(curbuf, c, changefile, வெற்று);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    முழு		c,
    முழு		changefile,
    முழு		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    நிலையான pos_T	pos_copy;

    posp = வெற்று;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    எனில் (c < 0)
	திரும்பு posp;
    எனில் (c > '~')			// check for islower()/isupper()
	;
    இல்லையெனில் எனில் (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    இல்லையெனில் எனில் (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    இல்லையெனில் எனில் (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_செருகு);
    இல்லையெனில் எனில் (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    இல்லையெனில் எனில் (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    இல்லையெனில் எனில் (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    இல்லையெனில் எனில் (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	முழு	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	எனில் (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    இல்லையெனில் எனில் (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	முழு	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	எனில் (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    இல்லையெனில் எனில் (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	எனில் (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	இல்லையெனில்
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	எனில் (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    எனில் (c == '<')
		pos_copy.col = 0;
	    இல்லையெனில்
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    இல்லையெனில் எனில் (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    இல்லையெனில் எனில் (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	எனில் (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	இல்லையெனில்
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	எனில் (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	எனில் (fnum != வெற்று)
	    *fnum = namedfm[c].fmark.fnum;
	இல்லையெனில் எனில் (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    எனில் (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		எனில் (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    திரும்பு (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    இல்லையெனில்
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    திரும்பு posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    முழு		dir,	// direction for search
    முழு		begin_வரி)
{
    முழு		i;
    pos_T	*result = வெற்று;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    எனில் (dir == BACKWARD && begin_வரி)
	pos.col = 0;
    இல்லையெனில் எனில் (dir == FORWARD && begin_வரி)
	pos.col = MAXCOL;

    க்கு (i = 0; i < NMARKS; i++)
    {
	எனில் (curbuf->b_namedm[i].lnum > 0)
	{
	    எனில் (dir == FORWARD)
	    {
		எனில் ((result == வெற்று || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    இல்லையெனில்
	    {
		எனில் ((result == வெற்று || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    திரும்பு result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    நிலையான வெறுமை
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    எனில் (fm->fname == வெற்று)
	திரும்பு;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    எனில் (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    இல்லையெனில்
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (வெறுமை)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    வெறுமை
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    முழு		i;
    win_T	*wp;

    எனில் (buf->b_ffname == வெற்று)
	திரும்பு;

    name = home_replace_save(buf, buf->b_ffname);
    எனில் (name == வெற்று)
	திரும்பு;

    க்கு (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	க்கு (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    நிலையான வெறுமை
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    எனில் (fm->fmark.fnum == 0
	    && fm->fname != வெற்று
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
    முழு
check_mark(pos_T *pos)
{
    எனில் (pos == வெற்று)
    {
	emsg(_(e_unknown_mark));
	திரும்பு FAIL;
    }
    எனில் (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	எனில் (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	திரும்பு FAIL;
    }
    எனில் (pos->lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	emsg(_(e_mark_has_invalid_வரி_எண்));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    வெறுமை
clrallmarks(buf_T *buf)
{
    நிலையான முழு		i = -1;

    எனில் (i == -1)	// first call ever: initialize
	க்கு (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = வெற்று;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    க்கு (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_செருகு.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, முழு lead_len)
{
    எனில் (fmark->fnum == curbuf->b_fnum)		    // current buffer
	திரும்பு mark_வரி(&(fmark->mark), lead_len);
    திரும்பு buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    நிலையான char_u *
mark_வரி(pos_T *mp, முழு lead_len)
{
    char_u	*s, *p;
    முழு		len;

    எனில் (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    எனில் (s == வெற்று)
	திரும்பு வெற்று;
    // Truncate the line to fit it in the window.
    len = 0;
    க்கு (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	எனில் (len >= Columns - lead_len)
	    நிறுத்து;
    }
    *p = NUL;
    திரும்பு s;
}

/*
 * print the marks
 */
    வெறுமை
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    முழு		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    எனில் (arg != வெற்று && *arg == NUL)
	arg = வெற்று;

    show_one_mark('\'', arg, &curwin->w_pcmark, வெற்று, TRUE);
    க்கு (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], வெற்று, TRUE);
    க்கு (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	எனில் (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	இல்லையெனில்
	    name = namedfm[i].fname;
	எனில் (name != வெற்று)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    எனில் (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, வெற்று, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, வெற்று, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, வெற்று, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_செருகு, வெற்று, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, வெற்று, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    எனில் ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    இல்லையெனில்
	posp = endp;
    show_one_mark('<', arg, posp, வெற்று, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, வெற்று, TRUE);

    show_one_mark(-1, arg, வெற்று, வெற்று, FALSE);
}

    நிலையான வெறுமை
show_one_mark(
    முழு		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    முழு		current)	// in current file
{
    நிலையான முழு	did_title = FALSE;
    முழு		mustfree = FALSE;
    char_u	*name = name_arg;

    எனில் (c == -1)			    // finish up
    {
	எனில் (did_title)
	    did_title = FALSE;
	இல்லையெனில்
	{
	    எனில் (arg == வெற்று)
		செய்தி(_("No marks set"));
	    இல்லையெனில்
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    இல்லையெனில் எனில் (!got_int
	    && (arg == வெற்று || vim_strchr(arg, c) != வெற்று)
	    && p->lnum != 0)
    {
	எனில் (name == வெற்று && current)
	{
	    name = mark_வரி(p, 15);
	    எனில் (name == வெற்று)
	    {
		emsg(_(e_out_of_memory));
		திரும்பு;
	    }
	    mustfree = TRUE;
	}
	எனில் (!message_filtered(name))
	{
	    எனில் (!did_title)
	    {
		// Highlight title
		செய்தி_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    செய்தி_putchar('\n');
	    எனில் (!got_int)
	    {
		sprintf((எழுத்து *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		செய்தி_outtrans(IObuff);
		எனில் (name != வெற்று)
		{
		    செய்தி_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	எனில் (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    வெறுமை
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    முழு		from, to;
    முழு		i;
    முழு		lower;
    முழு		digit;
    முழு		n;

    எனில் (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    இல்லையெனில் எனில் (eap->forceit)
	emsg(_(e_invalid_argument));
    இல்லையெனில் எனில் (*eap->arg == NUL)
	emsg(_(e_argument_required));
    இல்லையெனில்
    {
	// clear specified marks only
	க்கு (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    எனில் (lower || digit || ASCII_ISUPPER(*p))
	    {
		எனில் (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    எனில் (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			திரும்பு;
		    }
		    p += 2;
		}
		இல்லையெனில்
		    // clear one lower case mark
		    from = to = *p;

		க்கு (i = from; i <= to; ++i)
		{
		    எனில் (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    இல்லையெனில்
		    {
			எனில் (digit)
			    n = i - '0' + NMARKS;
			இல்லையெனில்
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
	    இல்லையெனில்
		தேர்வு (*p)
		{
		    நிலை '"': curbuf->b_last_cursor.lnum = 0; நிறுத்து;
		    நிலை '^': curbuf->b_last_செருகு.lnum = 0; நிறுத்து;
		    நிலை '.': curbuf->b_last_change.lnum = 0; நிறுத்து;
		    நிலை '[': curbuf->b_op_start.lnum    = 0; நிறுத்து;
		    நிலை ']': curbuf->b_op_end.lnum      = 0; நிறுத்து;
		    நிலை '<': curbuf->b_visual.vi_start.lnum = 0; நிறுத்து;
		    நிலை '>': curbuf->b_visual.vi_end.lnum   = 0; நிறுத்து;
		    நிலை ' ': நிறுத்து;
		    இயல்பு:  semsg(_(e_invalid_argument_str), p);
			      திரும்பு;
		}
	}
    }
}

/*
 * print the jumplist
 */
    வெறுமை
ex_jumps(exarg_T *eap UNUSED)
{
    முழு		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    செய்தி_puts_title(_("\n jump line  col file/text"));
    க்கு (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	எனில் (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    எனில் (name == வெற்று && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    எனில் (name == வெற்று || message_filtered(name))
	    {
		vim_free(name);
		தொடர்;
	    }

	    செய்தி_putchar('\n');
	    எனில் (got_int)
	    {
		vim_free(name);
		நிறுத்து;
	    }
	    sprintf((எழுத்து *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    செய்தி_outtrans(IObuff);
	    செய்தி_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    எனில் (curwin->w_jumplistidx == curwin->w_jumplistlen)
	செய்தி_puts("\n>");
}

    வெறுமை
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    வெறுமை
ex_changes(exarg_T *eap UNUSED)
{
    முழு		i;
    char_u	*name;

    // Highlight title
    செய்தி_puts_title(_("\nchange line  col text"));

    க்கு (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	எனில் (curbuf->b_changelist[i].lnum != 0)
	{
	    செய்தி_putchar('\n');
	    எனில் (got_int)
		நிறுத்து;
	    sprintf((எழுத்து *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    செய்தி_outtrans(IObuff);
	    name = mark_வரி(&curbuf->b_changelist[i], 17);
	    எனில் (name == வெற்று)
		நிறுத்து;
	    செய்தி_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    எனில் (curwin->w_changelistidx == curbuf->b_changelistlen)
	செய்தி_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	எனில் (*lp >= line1 && *lp <= line2) \
	{ \
	    எனில் (amount == MAXLNUM) \
		*lp = 0; \
	    இல்லையெனில் \
		*lp += amount; \
	} \
	இல்லையெனில் எனில் (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	எனில் (*lp >= line1 && *lp <= line2) \
	{ \
	    எனில் (amount == MAXLNUM) \
		*lp = line1; \
	    இல்லையெனில் \
		*lp += amount; \
	} \
	இல்லையெனில் எனில் (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	எனில் (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    எனில் (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    இல்லையெனில் /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	இல்லையெனில் எனில் (amount_after && posp->lnum > line2) \
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
    வெறுமை
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    வெறுமை
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    நிலையான வெறுமை
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    முழு		adjust_folds UNUSED)
{
    முழு		i;
    முழு		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    நிலையான pos_T initpos = {1, 0, 0};

    எனில் (line2 < line1 && amount_after == 0L)	    // nothing to do
	திரும்பு;

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	க்கு (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    எனில் (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	க்கு (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    எனில் (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_செருகு.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	எனில் (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	க்கு (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(வெற்று, line1, line2, amount, amount_after);
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
    எனில் (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    க்கு (i = 0; i < win->w_jumplistlen; ++i)
		எனில் (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	எனில் (win->w_buffer == curbuf)
	{
	    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		க்கு (i = 0; i < win->w_tagstacklen; i++)
		    எனில் (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    எனில் (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    எனில் (win != curwin)
	    {
		எனில் (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    எனில் (amount == MAXLNUM)	    // topline is deleted
		    {
			எனில் (line1 <= 1)
			    win->w_topline = 1;
			இல்லையெனில்
			    win->w_topline = line1 - 1;
		    }
		    இல்லையெனில் எனில் (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		இல்லையெனில் எனில் (amount_after && win->w_topline > line2)
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
	    எனில் (adjust_folds)
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
	எனில் (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    எனில் (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    இல்லையெனில் எனில் (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    இல்லையெனில் \
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
    வெறுமை
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    முழு		spaces_removed)
{
    முழு		i;
    முழு		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    எனில் ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	திரும்பு; // nothing to do

    // named marks, lower case and upper case
    க்கு (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	எனில் (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    க்கு (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	எனில் (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_செருகு));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    க்கு (i = 0; i < curbuf->b_changelistlen; ++i)
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
	க்கு (i = 0; i < win->w_jumplistlen; ++i)
	    எனில் (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	எனில் (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    க்கு (i = 0; i < win->w_tagstacklen; i++)
		எனில் (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    எனில் (win != curwin)
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
    வெறுமை
cleanup_jumplist(win_T *wp, முழு loadfiles)
{
    முழு	    i;
    முழு	    from, to;
    முழு	    mustfree;

    எனில் (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	க்கு (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    எனில் ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    க்கு (from = 0; from < wp->w_jumplistlen; ++from)
    {
	எனில் (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	க்கு (i = from + 1; i < wp->w_jumplistlen; ++i)
	    எனில் (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		நிறுத்து;
	எனில் (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	இல்லையெனில் எனில் (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_அடுக்கு);
	இல்லையெனில்				// adjacent duplicate
	    mustfree = TRUE;

	எனில் (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	இல்லையெனில்
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    எனில் (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    வெறுமை
copy_jumplist(win_T *from, win_T *to)
{
    முழு		i;

    க்கு (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	எனில் (from->w_jumplist[i].fname != வெற்று)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    வெறுமை
free_jumplist(win_T *wp)
{
    முழு		i;

    க்கு (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    வெறுமை
set_last_cursor(win_T *win)
{
    எனில் (win->w_buffer != வெற்று)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_all_marks(வெறுமை)
{
    முழு		i;

    க்கு (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	எனில் (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#எனில் defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(வெறுமை)
{
    திரும்பு namedfm;
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    நிலையான முழு
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, முழு bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    எனில் (pos->lnum <= 0)
	திரும்பு OK;

    d = dict_alloc();
    எனில் (d == வெற்று)
	திரும்பு FAIL;

    எனில் (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	திரும்பு FAIL;
    }

    lpos = list_alloc();
    எனில் (lpos == வெற்று)
	திரும்பு FAIL;

    list_append_எண்(lpos, bufnr);
    list_append_எண்(lpos, pos->lnum);
    list_append_எண்(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_எண்(lpos, pos->coladd);

    எனில் (dict_add_string_len(d, "mark", mname, (முழு)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != வெற்று && dict_add_string(d, "file", fname) == FAIL))
    {
	எனில் (lpos->lv_refcount == 0)
	    list_free(lpos);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Get information about marks local to a buffer.
 */
    நிலையான வெறுமை
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    முழு		i;

    // Marks 'a' to 'z'
    க்கு (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, வெற்று);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, வெற்று);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_செருகு, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, வெற்று);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, வெற்று);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    நிலையான வெறுமை
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    முழு		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    க்கு (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	எனில் (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	இல்லையெனில்
	    name = namedfm[i].fname;
	எனில் (name != வெற்று)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    எனில் (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    வெறுமை
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = வெற்று;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	திரும்பு;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	திரும்பு;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
