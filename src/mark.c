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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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
ਸਥਿਰ xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

ਸਥਿਰ ਖਾਲੀ fname2fnum(xfmark_T *fm);
ਸਥਿਰ ਖਾਲੀ fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
ਸਥਿਰ char_u *mark_ਸਤਰ(pos_T *mp, ਪੂਰਨ lead_len);
ਸਥਿਰ ਖਾਲੀ show_one_mark(ਪੂਰਨ, char_u *, pos_T *, char_u *, ਪੂਰਨ current);
ਸਥਿਰ ਖਾਲੀ mark_adjust_ਅੰਦਰੂਨੀ(linenr_T line1, linenr_T line2, ਲੰਮਾ amount,
    ਲੰਮਾ amount_after, ਪੂਰਨ adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ਪੂਰਨ
setmark(ਪੂਰਨ c)
{
    ਵਾਪਸ setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ਪੂਰਨ
setmark_pos(ਪੂਰਨ c, pos_T *pos, ਪੂਰਨ fnum)
{
    ਪੂਰਨ		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ਜੇ (c < 0)
	ਵਾਪਸ FAIL;

    ਜੇ (c == '\'' || c == '`')
    {
	ਜੇ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	ਨਹੀਂਤਾਂ
	    curwin->w_pcmark = *pos;
	ਵਾਪਸ OK;
    }

    buf = buflist_findnr(fnum);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (c == '"')
    {
	buf->b_ਆਖਰੀ_cursor = *pos;
	ਵਾਪਸ OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ਜੇ (c == '[')
    {
	buf->b_op_ਸ਼ੁਰੂ = *pos;
	ਵਾਪਸ OK;
    }
    ਜੇ (c == ']')
    {
	buf->b_op_ਸਿਰਾ = *pos;
	ਵਾਪਸ OK;
    }

    ਜੇ (c == '<' || c == '>')
    {
	ਜੇ (c == '<')
	    buf->b_visual.vi_ਸ਼ੁਰੂ = *pos;
	ਨਹੀਂਤਾਂ
	    buf->b_visual.vi_ਸਿਰਾ = *pos;
	ਜੇ (buf->b_visual.vi_ਬਹੁਲਕ == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_ਬਹੁਲਕ = 'v';
	ਵਾਪਸ OK;
    }

    ਜੇ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	ਵਾਪਸ OK;
    }
    ਜੇ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ਜੇ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	ਨਹੀਂਤਾਂ
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	namedfm[i].ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    ਖਾਲੀ
mark_forget_file(win_T *wp, ਪੂਰਨ fnum)
{
    ਪੂਰਨ		i;

    ਲਈ (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ਜੇ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ਜੇ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * ਆਕਾਰ(wp->w_jumplist[i]));
	}

    ਲਈ (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ਜੇ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_ਇੰਦਰਾਜ(&wp->w_tagstack[i]);
	    ਜੇ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * ਆਕਾਰ(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    ਖਾਲੀ
setpcmark(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ਜੇ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	ਵਾਪਸ;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ਜੇ (jop_flags & JOP_ਢੇਰ)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ਜੇ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ਜੇ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	ਲਈ (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    fm->ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
}

/*
 * To change context, call setpcmark(), then move the current position to
 * where ever, then call checkpcmark().  This ensures that the previous
 * context will only be changed if the cursor moved to a different line.
 * If pcmark was deleted (with "dG") the previous mark is restored.
 */
    ਖਾਲੀ
checkpcmark(ਖਾਲੀ)
{
    ਜੇ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ਜੇ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	ਵਾਪਸ (pos_T *)ਨਿਰਾ;

    ਲਈ (;;)
    {
	ਜੇ (curwin->w_jumplistidx + ਗਿਣਤੀ_2 < 0
		|| curwin->w_jumplistidx + ਗਿਣਤੀ_2 >= curwin->w_jumplistlen)
	    ਵਾਪਸ (pos_T *)ਨਿਰਾ;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ਜੇ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ਜੇ (curwin->w_jumplistidx + ਗਿਣਤੀ_2 < 0)
		ਵਾਪਸ (pos_T *)ਨਿਰਾ;
	}

	curwin->w_jumplistidx += ਗਿਣਤੀ_2;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ਜੇ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ਜੇ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ਜੇ (buflist_findnr(fmark.fnum) == ਨਿਰਾ)
	    {					     // Skip this one ..
		ਗਿਣਤੀ_2 += ਗਿਣਤੀ_2 < 0 ? -1 : 1;
		ਜਾਰੀ;
	    }
	    ਜੇ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		ਵਾਪਸ (pos_T *)ਨਿਰਾ;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	ਨਹੀਂਤਾਂ
	    pos = &(jmp->fmark.mark);
	ਵਾਪਸ pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		n;

    ਜੇ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	ਵਾਪਸ (pos_T *)ਨਿਰਾ;

    n = curwin->w_changelistidx;
    ਜੇ (n + ਗਿਣਤੀ_2 < 0)
    {
	ਜੇ (n == 0)
	    ਵਾਪਸ (pos_T *)ਨਿਰਾ;
	n = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n + ਗਿਣਤੀ_2 >= curbuf->b_changelistlen)
    {
	ਜੇ (n == curbuf->b_changelistlen - 1)
	    ਵਾਪਸ (pos_T *)ਨਿਰਾ;
	n = curbuf->b_changelistlen - 1;
    }
    ਨਹੀਂਤਾਂ
	n += ਗਿਣਤੀ_2;
    curwin->w_changelistidx = n;
    ਵਾਪਸ curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, ਪੂਰਨ c, ਪੂਰਨ changefile)
{
    ਵਾਪਸ getmark_buf_fnum(buf, c, changefile, ਨਿਰਾ);
}

    pos_T *
getmark(ਪੂਰਨ c, ਪੂਰਨ changefile)
{
    ਵਾਪਸ getmark_buf_fnum(curbuf, c, changefile, ਨਿਰਾ);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ਪੂਰਨ		c,
    ਪੂਰਨ		changefile,
    ਪੂਰਨ		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ਸਥਿਰ pos_T	pos_copy;

    posp = ਨਿਰਾ;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ਜੇ (c < 0)
	ਵਾਪਸ posp;
    ਜੇ (c > '~')			// check for islower()/isupper()
	;
    ਨਹੀਂਤਾਂ ਜੇ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_ਆਖਰੀ_cursor);
    ਨਹੀਂਤਾਂ ਜੇ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_ਆਖਰੀ_ਪਾਓ);
    ਨਹੀਂਤਾਂ ਜੇ (c == '.')			// to where last change was made
	posp = &(buf->b_ਆਖਰੀ_change);
    ਨਹੀਂਤਾਂ ਜੇ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_ਸ਼ੁਰੂ);
    ਨਹੀਂਤਾਂ ਜੇ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_ਸਿਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ਪੂਰਨ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ਜੇ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ਪੂਰਨ	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ਜੇ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_ਸ਼ੁਰੂ;
	endp = &buf->b_visual.vi_ਸਿਰਾ;
	ਜੇ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	ਨਹੀਂਤਾਂ
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ਜੇ (buf->b_visual.vi_ਬਹੁਲਕ == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ਜੇ (c == '<')
		pos_copy.ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ
		pos_copy.ਖਾਨਾ = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ਜੇ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	ਨਹੀਂਤਾਂ
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ਜੇ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ਜੇ (fnum != ਨਿਰਾ)
	    *fnum = namedfm[c].fmark.fnum;
	ਨਹੀਂਤਾਂ ਜੇ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ਜੇ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ਜੇ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    ਵਾਪਸ (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    ਨਹੀਂਤਾਂ
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    ਵਾਪਸ posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ਪੂਰਨ		dir,	// direction for search
    ਪੂਰਨ		begin_ਸਤਰ)
{
    ਪੂਰਨ		i;
    pos_T	*ਨਤੀਜਾ = ਨਿਰਾ;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ਜੇ (dir == BACKWARD && begin_ਸਤਰ)
	pos.ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (dir == FORWARD && begin_ਸਤਰ)
	pos.ਖਾਨਾ = MAXCOL;

    ਲਈ (i = 0; i < NMARKS; i++)
    {
	ਜੇ (curbuf->b_namedm[i].lnum > 0)
	{
	    ਜੇ (dir == FORWARD)
	    {
		ਜੇ ((ਨਤੀਜਾ == ਨਿਰਾ || LT_POS(curbuf->b_namedm[i], *ਨਤੀਜਾ))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    ਨਤੀਜਾ = &curbuf->b_namedm[i];
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ ((ਨਤੀਜਾ == ਨਿਰਾ || LT_POS(*ਨਤੀਜਾ, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    ਨਤੀਜਾ = &curbuf->b_namedm[i];
	    }
	}
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    ਸਥਿਰ ਖਾਲੀ
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    ਜੇ (fm->fname == ਨਿਰਾ)
	ਵਾਪਸ;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ਜੇ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		|| fm->fname[1] == '\\'
#ਜੇ_ਅੰਤ
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    ਨਹੀਂਤਾਂ
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (ਖਾਲੀ)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    ਖਾਲੀ
fmarks_check_ਨਾਂ(buf_T *buf)
{
    char_u	*name;
    ਪੂਰਨ		i;
    win_T	*wp;

    ਜੇ (buf->b_ffname == ਨਿਰਾ)
	ਵਾਪਸ;

    name = home_replace_save(buf, buf->b_ffname);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ਲਈ (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ਸਥਿਰ ਖਾਲੀ
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ਜੇ (fm->fmark.fnum == 0
	    && fm->fname != ਨਿਰਾ
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
    ਪੂਰਨ
check_mark(pos_T *pos)
{
    ਜੇ (pos == ਨਿਰਾ)
    {
	emsg(_(e_unknown_mark));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ਜੇ (pos->lnum == 0)
	    emsg(_(e_mark_not_ਸੈੱਟ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (pos->lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	emsg(_(e_mark_has_invalid_ਸਤਰ_ਸੰਖਿਆ));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    ਖਾਲੀ
clrallmarks(buf_T *buf)
{
    ਸਥਿਰ ਪੂਰਨ		i = -1;

    ਜੇ (i == -1)	// first call ever: initialize
	ਲਈ (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	    namedfm[i].ਸਮਾਂ_ਸੈੱਟ = 0;
#ਜੇ_ਅੰਤ
	}

    ਲਈ (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_ਸ਼ੁਰੂ.lnum = 0;		// start/end op mark cleared
    buf->b_op_ਸਿਰਾ.lnum = 0;
    buf->b_ਆਖਰੀ_cursor.lnum = 1;	// '" mark cleared
    buf->b_ਆਖਰੀ_cursor.ਖਾਨਾ = 0;
    buf->b_ਆਖਰੀ_cursor.coladd = 0;
    buf->b_ਆਖਰੀ_ਪਾਓ.lnum = 0;	// '^ mark cleared
    buf->b_ਆਖਰੀ_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, ਪੂਰਨ lead_len)
{
    ਜੇ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	ਵਾਪਸ mark_ਸਤਰ(&(fmark->mark), lead_len);
    ਵਾਪਸ buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ਸਥਿਰ char_u *
mark_ਸਤਰ(pos_T *mp, ਪੂਰਨ lead_len)
{
    char_u	*s, *p;
    ਪੂਰਨ		len;

    ਜੇ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    // Truncate the line to fit it in the window.
    len = 0;
    ਲਈ (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ਜੇ (len >= Columns - lead_len)
	    ਰੋਕੋ;
    }
    *p = NUL;
    ਵਾਪਸ s;
}

/*
 * print the marks
 */
    ਖਾਲੀ
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ਜੇ (arg != ਨਿਰਾ && *arg == NUL)
	arg = ਨਿਰਾ;

    show_one_mark('\'', arg, &curwin->w_pcmark, ਨਿਰਾ, TRUE);
    ਲਈ (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ਨਿਰਾ, TRUE);
    ਲਈ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ਜੇ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	ਨਹੀਂਤਾਂ
	    name = namedfm[i].fname;
	ਜੇ (name != ਨਿਰਾ)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ਜੇ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_ਆਖਰੀ_cursor, ਨਿਰਾ, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_ਸ਼ੁਰੂ, ਨਿਰਾ, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_ਸਿਰਾ, ਨਿਰਾ, TRUE);
    show_one_mark('^', arg, &curbuf->b_ਆਖਰੀ_ਪਾਓ, ਨਿਰਾ, TRUE);
    show_one_mark('.', arg, &curbuf->b_ਆਖਰੀ_change, ਨਿਰਾ, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_ਸ਼ੁਰੂ;
    endp = &curbuf->b_visual.vi_ਸਿਰਾ;
    ਜੇ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    ਨਹੀਂਤਾਂ
	posp = endp;
    show_one_mark('<', arg, posp, ਨਿਰਾ, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ਨਿਰਾ, TRUE);

    show_one_mark(-1, arg, ਨਿਰਾ, ਨਿਰਾ, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
show_one_mark(
    ਪੂਰਨ		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ਪੂਰਨ		current)	// in current file
{
    ਸਥਿਰ ਪੂਰਨ	did_title = FALSE;
    ਪੂਰਨ		mustfree = FALSE;
    char_u	*name = name_arg;

    ਜੇ (c == -1)			    // finish up
    {
	ਜੇ (did_title)
	    did_title = FALSE;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (arg == ਨਿਰਾ)
		ਸੁਨੇਹਾ(_("No marks set"));
	    ਨਹੀਂਤਾਂ
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    ਨਹੀਂਤਾਂ ਜੇ (!got_int
	    && (arg == ਨਿਰਾ || vim_strchr(arg, c) != ਨਿਰਾ)
	    && p->lnum != 0)
    {
	ਜੇ (name == ਨਿਰਾ && current)
	{
	    name = mark_ਸਤਰ(p, 15);
	    ਜੇ (name == ਨਿਰਾ)
	    {
		emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		ਵਾਪਸ;
	    }
	    mustfree = TRUE;
	}
	ਜੇ (!ਸੁਨੇਹਾ_filtered(name))
	{
	    ਜੇ (!did_title)
	    {
		// Highlight title
		ਸੁਨੇਹਾ_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (!got_int)
	    {
		sprintf((ਅੱਖਰ *)IObuff, " %c %6ld %4d ", c, p->lnum, p->ਖਾਨਾ);
		ਸੁਨੇਹਾ_outtrans(IObuff);
		ਜੇ (name != ਨਿਰਾ)
		{
		    ਸੁਨੇਹਾ_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    ਬਾਹਰਲਾ_flush();		    // show one line at a time
	}
	ਜੇ (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    ਖਾਲੀ
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    ਪੂਰਨ		from, ਮੰਜ਼ਿਲ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਹੇਠਲਾ;
    ਪੂਰਨ		digit;
    ਪੂਰਨ		n;

    ਜੇ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    ਨਹੀਂਤਾਂ ਜੇ (eap->forceit)
	emsg(_(e_invalid_ਦਲੀਲ));
    ਨਹੀਂਤਾਂ ਜੇ (*eap->arg == NUL)
	emsg(_(e_ਦਲੀਲ_required));
    ਨਹੀਂਤਾਂ
    {
	// clear specified marks only
	ਲਈ (p = eap->arg; *p != NUL; ++p)
	{
	    ਹੇਠਲਾ = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ਜੇ (ਹੇਠਲਾ || digit || ASCII_ISUPPER(*p))
	    {
		ਜੇ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    ਮੰਜ਼ਿਲ = p[2];
		    ਜੇ (!(ਹੇਠਲਾ ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || ਮੰਜ਼ਿਲ < from)
		    {
			semsg(_(e_invalid_ਦਲੀਲ_str), p);
			ਵਾਪਸ;
		    }
		    p += 2;
		}
		ਨਹੀਂਤਾਂ
		    // clear one lower case mark
		    from = ਮੰਜ਼ਿਲ = *p;

		ਲਈ (i = from; i <= ਮੰਜ਼ਿਲ; ++i)
		{
		    ਜੇ (ਹੇਠਲਾ)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (digit)
			    n = i - '0' + NMARKS;
			ਨਹੀਂਤਾਂ
			    n = i - 'A';
			namedfm[n].fmark.mark.lnum = 0;
			namedfm[n].fmark.fnum = 0;
			VIM_CLEAR(namedfm[n].fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
			namedfm[n].ਸਮਾਂ_ਸੈੱਟ = digit ? 0 : vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਚੋਣ (*p)
		{
		    ਹਾਲਤ '"': curbuf->b_ਆਖਰੀ_cursor.lnum = 0; ਰੋਕੋ;
		    ਹਾਲਤ '^': curbuf->b_ਆਖਰੀ_ਪਾਓ.lnum = 0; ਰੋਕੋ;
		    ਹਾਲਤ '.': curbuf->b_ਆਖਰੀ_change.lnum = 0; ਰੋਕੋ;
		    ਹਾਲਤ '[': curbuf->b_op_ਸ਼ੁਰੂ.lnum    = 0; ਰੋਕੋ;
		    ਹਾਲਤ ']': curbuf->b_op_ਸਿਰਾ.lnum      = 0; ਰੋਕੋ;
		    ਹਾਲਤ '<': curbuf->b_visual.vi_ਸ਼ੁਰੂ.lnum = 0; ਰੋਕੋ;
		    ਹਾਲਤ '>': curbuf->b_visual.vi_ਸਿਰਾ.lnum   = 0; ਰੋਕੋ;
		    ਹਾਲਤ ' ': ਰੋਕੋ;
		    ਮੂਲ:  semsg(_(e_invalid_ਦਲੀਲ_str), p);
			      ਵਾਪਸ;
		}
	}
    }
}

/*
 * print the jumplist
 */
    ਖਾਲੀ
ex_jumps(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    ਸੁਨੇਹਾ_puts_title(_("\n jump line  col file/text"));
    ਲਈ (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ਜੇ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ਜੇ (name == ਨਿਰਾ && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ਜੇ (name == ਨਿਰਾ || ਸੁਨੇਹਾ_filtered(name))
	    {
		vim_free(name);
		ਜਾਰੀ;
	    }

	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
	    {
		vim_free(name);
		ਰੋਕੋ;
	    }
	    sprintf((ਅੱਖਰ *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.ਖਾਨਾ);
	    ਸੁਨੇਹਾ_outtrans(IObuff);
	    ਸੁਨੇਹਾ_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	ਬਾਹਰਲਾ_flush();
    }
    ਜੇ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	ਸੁਨੇਹਾ_puts("\n>");
}

    ਖਾਲੀ
ex_clearjumps(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    ਖਾਲੀ
ex_changes(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    char_u	*name;

    // Highlight title
    ਸੁਨੇਹਾ_puts_title(_("\nchange line  col text"));

    ਲਈ (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ਜੇ (curbuf->b_changelist[i].lnum != 0)
	{
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    sprintf((ਅੱਖਰ *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (ਲੰਮਾ)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].ਖਾਨਾ);
	    ਸੁਨੇਹਾ_outtrans(IObuff);
	    name = mark_ਸਤਰ(&curbuf->b_changelist[i], 17);
	    ਜੇ (name == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਸੁਨੇਹਾ_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	ਬਾਹਰਲਾ_flush();
    }
    ਜੇ (curwin->w_changelistidx == curbuf->b_changelistlen)
	ਸੁਨੇਹਾ_puts("\n>");
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ one_adjust(ਜੋੜੋ) \
    { \
	lp = ਜੋੜੋ; \
	ਜੇ (*lp >= line1 && *lp <= line2) \
	{ \
	    ਜੇ (amount == MAXLNUM) \
		*lp = 0; \
	    ਨਹੀਂਤਾਂ \
		*lp += amount; \
	} \
	ਨਹੀਂਤਾਂ ਜੇ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ one_adjust_nodel(ਜੋੜੋ) \
    { \
	lp = ਜੋੜੋ; \
	ਜੇ (*lp >= line1 && *lp <= line2) \
	{ \
	    ਜੇ (amount == MAXLNUM) \
		*lp = line1; \
	    ਨਹੀਂਤਾਂ \
		*lp += amount; \
	} \
	ਨਹੀਂਤਾਂ ਜੇ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ਜੇ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ਜੇ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->ਖਾਨਾ = 0; \
	    } \
	    ਨਹੀਂਤਾਂ /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	ਨਹੀਂਤਾਂ ਜੇ (amount_after && posp->lnum > line2) \
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
    ਖਾਲੀ
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    mark_adjust_ਅੰਦਰੂਨੀ(line1, line2, amount, amount_after, TRUE);
}

    ਖਾਲੀ
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after)
{
    mark_adjust_ਅੰਦਰੂਨੀ(line1, line2, amount, amount_after, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
mark_adjust_ਅੰਦਰੂਨੀ(
    linenr_T	line1,
    linenr_T	line2,
    ਲੰਮਾ	amount,
    ਲੰਮਾ	amount_after,
    ਪੂਰਨ		adjust_folds ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ਸਥਿਰ pos_T initpos = {1, 0, 0};

    ਜੇ (line2 < line1 && amount_after == 0L)	    // nothing to do
	ਵਾਪਸ;

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	ਲਈ (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ਜੇ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	ਲਈ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ਜੇ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_ਆਖਰੀ_ਪਾਓ.lnum));

	// last change position
	one_adjust(&(curbuf->b_ਆਖਰੀ_change.lnum));

	// last cursor position, if it was set
	ਜੇ (!EQUAL_POS(curbuf->b_ਆਖਰੀ_cursor, initpos))
	    one_adjust(&(curbuf->b_ਆਖਰੀ_cursor.lnum));


	// list of change positions
	ਲਈ (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_ਸ਼ੁਰੂ.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_ਸਿਰਾ.lnum));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ਨਿਰਾ, line1, line2, amount, amount_after);
	// location lists
	FOR_ALL_TAB_WINDOWS(tab, win)
	    qf_mark_adjust(win, line1, line2, amount, amount_after);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	sign_mark_adjust(line1, line2, amount, amount_after);
#ਜੇ_ਅੰਤ
    }

    // previous context mark
    one_adjust(&(curwin->w_pcmark.lnum));

    // previous pcmark
    one_adjust(&(curwin->w_prev_pcmark.lnum));

    // saved cursor for formatting
    ਜੇ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    ਲਈ (i = 0; i < win->w_jumplistlen; ++i)
		ਜੇ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ਜੇ (win->w_ਬਫਰ == curbuf)
	{
	    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		ਲਈ (i = 0; i < win->w_tagstacklen; i++)
		    ਜੇ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ਜੇ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ਜੇ (win != curwin)
	    {
		ਜੇ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ਜੇ (amount == MAXLNUM)	    // topline is deleted
		    {
			ਜੇ (line1 <= 1)
			    win->w_topline = 1;
			ਨਹੀਂਤਾਂ
			    win->w_topline = line1 - 1;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    win->w_topfill = 0;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ ਜੇ (amount_after && win->w_topline > line2)
		{
		    win->w_topline += amount_after;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    win->w_topfill = 0;
#ਜੇ_ਅੰਤ
		}
		one_adjust_cursor(&(win->w_cursor));
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // adjust folds
	    ਜੇ (adjust_folds)
		foldMarkAdjust(win, line1, line2, amount, amount_after);
#ਜੇ_ਅੰਤ
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // adjust diffs
    diff_mark_adjust(line1, line2, amount, amount_after);
#ਜੇ_ਅੰਤ

    // adjust per-window "last cursor" positions
    FOR_ALL_BUF_WININFO(curbuf, wip)
	one_adjust_cursor(&(wip->wi_fpos));
}

// This code is used often, needs to be fast.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਨਾ_adjust(pp) \
    { \
	posp = pp; \
	ਜੇ (posp->lnum == lnum && posp->ਖਾਨਾ >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ਜੇ (ਖਾਨਾ_amount < 0 && posp->ਖਾਨਾ <= (colnr_T)-ਖਾਨਾ_amount) \
		posp->ਖਾਨਾ = 0; \
	    ਨਹੀਂਤਾਂ ਜੇ (posp->ਖਾਨਾ < spaces_removed) \
		posp->ਖਾਨਾ = ਖਾਨਾ_amount + spaces_removed; \
	    ਨਹੀਂਤਾਂ \
		posp->ਖਾਨਾ += ਖਾਨਾ_amount; \
	} \
    }

/*
 * Adjust marks in line "lnum" at column "mincol" and further: add
 * "lnum_amount" to the line number and add "col_amount" to the column
 * position.
 * "spaces_removed" is the number of spaces that were removed, matters when the
 * cursor is inside them.
 */
    ਖਾਲੀ
mark_ਖਾਨਾ_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    ਲੰਮਾ	lnum_amount,
    ਲੰਮਾ	ਖਾਨਾ_amount,
    ਪੂਰਨ		spaces_removed)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ਜੇ ((ਖਾਨਾ_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	ਵਾਪਸ; // nothing to do

    // named marks, lower case and upper case
    ਲਈ (i = 0; i < NMARKS; i++)
    {
	ਖਾਨਾ_adjust(&(curbuf->b_namedm[i]));
	ਜੇ (namedfm[i].fmark.fnum == fnum)
	    ਖਾਨਾ_adjust(&(namedfm[i].fmark.mark));
    }
    ਲਈ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ਜੇ (namedfm[i].fmark.fnum == fnum)
	    ਖਾਨਾ_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    ਖਾਨਾ_adjust(&(curbuf->b_ਆਖਰੀ_ਪਾਓ));

    // last change position
    ਖਾਨਾ_adjust(&(curbuf->b_ਆਖਰੀ_change));

    // list of change positions
    ਲਈ (i = 0; i < curbuf->b_changelistlen; ++i)
	ਖਾਨਾ_adjust(&(curbuf->b_changelist[i]));

    // Visual area
    ਖਾਨਾ_adjust(&(curbuf->b_visual.vi_ਸ਼ੁਰੂ));
    ਖਾਨਾ_adjust(&(curbuf->b_visual.vi_ਸਿਰਾ));

    // previous context mark
    ਖਾਨਾ_adjust(&(curwin->w_pcmark));

    // previous pcmark
    ਖਾਨਾ_adjust(&(curwin->w_prev_pcmark));

    // saved cursor for formatting
    ਖਾਨਾ_adjust(&saved_cursor);

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_WINDOWS(win)
    {
	// marks in the jumplist
	ਲਈ (i = 0; i < win->w_jumplistlen; ++i)
	    ਜੇ (win->w_jumplist[i].fmark.fnum == fnum)
		ਖਾਨਾ_adjust(&(win->w_jumplist[i].fmark.mark));

	ਜੇ (win->w_ਬਫਰ == curbuf)
	{
	    // marks in the tag stack
	    ਲਈ (i = 0; i < win->w_tagstacklen; i++)
		ਜੇ (win->w_tagstack[i].fmark.fnum == fnum)
		    ਖਾਨਾ_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ਜੇ (win != curwin)
		ਖਾਨਾ_adjust(&win->w_cursor);
	}
    }
}

/*
 * When deleting lines, this may create duplicate marks in the
 * jumplist. They will be removed here for the specified window.
 * When "loadfiles" is TRUE first ensure entries have the "fnum" field set
 * (this may be a bit slow).
 */
    ਖਾਲੀ
cleanup_jumplist(win_T *wp, ਪੂਰਨ loadfiles)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    from, ਮੰਜ਼ਿਲ;
    ਪੂਰਨ	    mustfree;

    ਜੇ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	ਲਈ (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ਜੇ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    ਮੰਜ਼ਿਲ = 0;
    ਲਈ (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ਜੇ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = ਮੰਜ਼ਿਲ;
	ਲਈ (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ਜੇ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		ਰੋਕੋ;
	ਜੇ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ਢੇਰ);
	ਨਹੀਂਤਾਂ				// adjacent duplicate
	    mustfree = TRUE;

	ਜੇ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	ਨਹੀਂਤਾਂ
	    wp->w_jumplist[ਮੰਜ਼ਿਲ++] = wp->w_jumplist[from];
    }
    ਜੇ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = ਮੰਜ਼ਿਲ;
    wp->w_jumplistlen = ਮੰਜ਼ਿਲ;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    ਖਾਲੀ
copy_jumplist(win_T *from, win_T *ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < from->w_jumplistlen; ++i)
    {
	ਮੰਜ਼ਿਲ->w_jumplist[i] = from->w_jumplist[i];
	ਜੇ (from->w_jumplist[i].fname != ਨਿਰਾ)
	    ਮੰਜ਼ਿਲ->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    ਮੰਜ਼ਿਲ->w_jumplistlen = from->w_jumplistlen;
    ਮੰਜ਼ਿਲ->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    ਖਾਲੀ
free_jumplist(win_T *wp)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    ਖਾਲੀ
ਸੈੱਟ_ਆਖਰੀ_cursor(win_T *win)
{
    ਜੇ (win->w_ਬਫਰ != ਨਿਰਾ)
	win->w_ਬਫਰ->b_ਆਖਰੀ_cursor = win->w_cursor;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_all_marks(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ਜੇ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(ਖਾਲੀ)
{
    ਵਾਪਸ namedfm;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ਪੂਰਨ bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ਜੇ (pos->lnum <= 0)
	ਵਾਪਸ OK;

    d = dict_alloc();
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	ਵਾਪਸ FAIL;
    }

    lpos = list_alloc();
    ਜੇ (lpos == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    list_append_ਸੰਖਿਆ(lpos, bufnr);
    list_append_ਸੰਖਿਆ(lpos, pos->lnum);
    list_append_ਸੰਖਿਆ(lpos, pos->ਖਾਨਾ < MAXCOL ? pos->ਖਾਨਾ + 1 : MAXCOL);
    list_append_ਸੰਖਿਆ(lpos, pos->coladd);

    ਜੇ (dict_ਜੋੜੋ_string_len(d, "mark", mname, (ਪੂਰਨ)mnamelen) == FAIL
	    || dict_ਜੋੜੋ_list(d, "pos", lpos) == FAIL
	    || (fname != ਨਿਰਾ && dict_ਜੋੜੋ_string(d, "file", fname) == FAIL))
    {
	ਜੇ (lpos->lv_refcount == 0)
	    list_free(lpos);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ਪੂਰਨ		i;

    // Marks 'a' to 'z'
    ਲਈ (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	ਜੋੜੋ_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ਨਿਰਾ);
    }

    // Mark '' is a window local mark and not a buffer local mark
    ਜੋੜੋ_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ਨਿਰਾ);

    ਜੋੜੋ_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_ਆਖਰੀ_cursor, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_ਸ਼ੁਰੂ, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_ਸਿਰਾ, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_ਆਖਰੀ_ਪਾਓ, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_ਆਖਰੀ_change, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_ਸ਼ੁਰੂ, buf->b_fnum, ਨਿਰਾ);
    ਜੋੜੋ_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_ਸਿਰਾ, buf->b_fnum, ਨਿਰਾ);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ਸਥਿਰ ਖਾਲੀ
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ਪੂਰਨ		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    ਲਈ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ਜੇ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	ਨਹੀਂਤਾਂ
	    name = namedfm[i].fname;
	ਜੇ (name != ਨਿਰਾ)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    ਜੋੜੋ_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ਜੇ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    ਖਾਲੀ
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ਨਿਰਾ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	ਵਾਪਸ;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#ਜੇ_ਅੰਤ
