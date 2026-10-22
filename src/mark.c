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

#અન્તર્ભાવ "vim.h"

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
સ્થિર xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

સ્થિર શૂન્ય fname2fnum(xfmark_T *fm);
સ્થિર શૂન્ય fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
સ્થિર char_u *mark_લીટી(pos_T *mp, પૂર્ણાંક lead_len);
સ્થિર શૂન્ય show_one_mark(પૂર્ણાંક, char_u *, pos_T *, char_u *, પૂર્ણાંક current);
સ્થિર શૂન્ય mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, પૂર્ણાંક adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    પૂર્ણાંક
setmark(પૂર્ણાંક c)
{
    પાછા setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    પૂર્ણાંક
setmark_pos(પૂર્ણાંક c, pos_T *pos, પૂર્ણાંક fnum)
{
    પૂર્ણાંક		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    જો (c < 0)
	પાછા FAIL;

    જો (c == '\'' || c == '`')
    {
	જો (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	નહીંતર
	    curwin->w_pcmark = *pos;
	પાછા OK;
    }

    buf = buflist_findnr(fnum);
    જો (buf == ખાલી)
	પાછા FAIL;

    જો (c == '"')
    {
	buf->b_last_cursor = *pos;
	પાછા OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    જો (c == '[')
    {
	buf->b_op_start = *pos;
	પાછા OK;
    }
    જો (c == ']')
    {
	buf->b_op_end = *pos;
	પાછા OK;
    }

    જો (c == '<' || c == '>')
    {
	જો (c == '<')
	    buf->b_visual.vi_start = *pos;
	નહીંતર
	    buf->b_visual.vi_end = *pos;
	જો (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	પાછા OK;
    }

    જો (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	પાછા OK;
    }
    જો (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	જો (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	નહીંતર
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	પાછા OK;
    }
    પાછા FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    શૂન્ય
mark_forget_file(win_T *wp, પૂર્ણાંક fnum)
{
    પૂર્ણાંક		i;

    માટે (i = wp->w_jumplistlen - 1; i >= 0; --i)
	જો (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    જો (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * કદ(wp->w_jumplist[i]));
	}

    માટે (i = wp->w_tagstacklen - 1; i >= 0; --i)
	જો (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    જો (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * કદ(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    શૂન્ય
setpcmark(શૂન્ય)
{
    પૂર્ણાંક		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    જો (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	પાછા;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    જો (jop_flags & JOP_ઢગલો)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	જો (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    જો (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	માટે (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ખાલી;
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
    શૂન્ય
checkpcmark(શૂન્ય)
{
    જો (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(પૂર્ણાંક ગણતરી)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    જો (curwin->w_jumplistlen == 0)	    // nothing to jump to
	પાછા (pos_T *)ખાલી;

    માટે (;;)
    {
	જો (curwin->w_jumplistidx + ગણતરી < 0
		|| curwin->w_jumplistidx + ગણતરી >= curwin->w_jumplistlen)
	    પાછા (pos_T *)ખાલી;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	જો (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    જો (curwin->w_jumplistidx + ગણતરી < 0)
		પાછા (pos_T *)ખાલી;
	}

	curwin->w_jumplistidx += ગણતરી;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	જો (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	જો (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    જો (buflist_findnr(fmark.fnum) == ખાલી)
	    {					     // Skip this one ..
		ગણતરી += ગણતરી < 0 ? -1 : 1;
		ચાલુ;
	    }
	    જો (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		પાછા (pos_T *)ખાલી;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	નહીંતર
	    pos = &(jmp->fmark.mark);
	પાછા pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(પૂર્ણાંક ગણતરી)
{
    પૂર્ણાંક		n;

    જો (curbuf->b_changelistlen == 0)	    // nothing to jump to
	પાછા (pos_T *)ખાલી;

    n = curwin->w_changelistidx;
    જો (n + ગણતરી < 0)
    {
	જો (n == 0)
	    પાછા (pos_T *)ખાલી;
	n = 0;
    }
    નહીંતર જો (n + ગણતરી >= curbuf->b_changelistlen)
    {
	જો (n == curbuf->b_changelistlen - 1)
	    પાછા (pos_T *)ખાલી;
	n = curbuf->b_changelistlen - 1;
    }
    નહીંતર
	n += ગણતરી;
    curwin->w_changelistidx = n;
    પાછા curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, પૂર્ણાંક c, પૂર્ણાંક changefile)
{
    પાછા getmark_buf_fnum(buf, c, changefile, ખાલી);
}

    pos_T *
getmark(પૂર્ણાંક c, પૂર્ણાંક changefile)
{
    પાછા getmark_buf_fnum(curbuf, c, changefile, ખાલી);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    પૂર્ણાંક		c,
    પૂર્ણાંક		changefile,
    પૂર્ણાંક		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    સ્થિર pos_T	pos_copy;

    posp = ખાલી;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    જો (c < 0)
	પાછા posp;
    જો (c > '~')			// check for islower()/isupper()
	;
    નહીંતર જો (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    નહીંતર જો (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    નહીંતર જો (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_ઉમેરો);
    નહીંતર જો (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    નહીંતર જો (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    નહીંતર જો (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    નહીંતર જો (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	પૂર્ણાંક	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	જો (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    નહીંતર જો (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	પૂર્ણાંક	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	જો (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    નહીંતર જો (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	જો (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	નહીંતર
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	જો (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    જો (c == '<')
		pos_copy.col = 0;
	    નહીંતર
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    નહીંતર જો (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    નહીંતર જો (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	જો (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	નહીંતર
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	જો (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	જો (fnum != ખાલી)
	    *fnum = namedfm[c].fmark.fnum;
	નહીંતર જો (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    જો (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		જો (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    પાછા (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    નહીંતર
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    પાછા posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    પૂર્ણાંક		dir,	// direction for search
    પૂર્ણાંક		begin_લીટી)
{
    પૂર્ણાંક		i;
    pos_T	*result = ખાલી;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    જો (dir == BACKWARD && begin_લીટી)
	pos.col = 0;
    નહીંતર જો (dir == FORWARD && begin_લીટી)
	pos.col = MAXCOL;

    માટે (i = 0; i < NMARKS; i++)
    {
	જો (curbuf->b_namedm[i].lnum > 0)
	{
	    જો (dir == FORWARD)
	    {
		જો ((result == ખાલી || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    નહીંતર
	    {
		જો ((result == ખાલી || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    પાછા result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    સ્થિર શૂન્ય
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    જો (fm->fname == ખાલી)
	પાછા;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    જો (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    નહીંતર
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (શૂન્ય)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    શૂન્ય
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    પૂર્ણાંક		i;
    win_T	*wp;

    જો (buf->b_ffname == ખાલી)
	પાછા;

    name = home_replace_save(buf, buf->b_ffname);
    જો (name == ખાલી)
	પાછા;

    માટે (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	માટે (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    સ્થિર શૂન્ય
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    જો (fm->fmark.fnum == 0
	    && fm->fname != ખાલી
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
    પૂર્ણાંક
check_mark(pos_T *pos)
{
    જો (pos == ખાલી)
    {
	emsg(_(e_unknown_mark));
	પાછા FAIL;
    }
    જો (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	જો (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	પાછા FAIL;
    }
    જો (pos->lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	emsg(_(e_mark_has_invalid_લીટી_સંખ્યા));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    શૂન્ય
clrallmarks(buf_T *buf)
{
    સ્થિર પૂર્ણાંક		i = -1;

    જો (i == -1)	// first call ever: initialize
	માટે (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ખાલી;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    માટે (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_ઉમેરો.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, પૂર્ણાંક lead_len)
{
    જો (fmark->fnum == curbuf->b_fnum)		    // current buffer
	પાછા mark_લીટી(&(fmark->mark), lead_len);
    પાછા buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    સ્થિર char_u *
mark_લીટી(pos_T *mp, પૂર્ણાંક lead_len)
{
    char_u	*s, *p;
    પૂર્ણાંક		len;

    જો (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	પાછા vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    જો (s == ખાલી)
	પાછા ખાલી;
    // Truncate the line to fit it in the window.
    len = 0;
    માટે (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	જો (len >= Columns - lead_len)
	    તોડો;
    }
    *p = NUL;
    પાછા s;
}

/*
 * print the marks
 */
    શૂન્ય
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    જો (arg != ખાલી && *arg == NUL)
	arg = ખાલી;

    show_one_mark('\'', arg, &curwin->w_pcmark, ખાલી, TRUE);
    માટે (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ખાલી, TRUE);
    માટે (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	જો (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	નહીંતર
	    name = namedfm[i].fname;
	જો (name != ખાલી)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    જો (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, ખાલી, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, ખાલી, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, ખાલી, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_ઉમેરો, ખાલી, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, ખાલી, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    જો ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    નહીંતર
	posp = endp;
    show_one_mark('<', arg, posp, ખાલી, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ખાલી, TRUE);

    show_one_mark(-1, arg, ખાલી, ખાલી, FALSE);
}

    સ્થિર શૂન્ય
show_one_mark(
    પૂર્ણાંક		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    પૂર્ણાંક		current)	// in current file
{
    સ્થિર પૂર્ણાંક	did_title = FALSE;
    પૂર્ણાંક		mustfree = FALSE;
    char_u	*name = name_arg;

    જો (c == -1)			    // finish up
    {
	જો (did_title)
	    did_title = FALSE;
	નહીંતર
	{
	    જો (arg == ખાલી)
		સંદેશ(_("No marks set"));
	    નહીંતર
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    નહીંતર જો (!got_int
	    && (arg == ખાલી || vim_strchr(arg, c) != ખાલી)
	    && p->lnum != 0)
    {
	જો (name == ખાલી && current)
	{
	    name = mark_લીટી(p, 15);
	    જો (name == ખાલી)
	    {
		emsg(_(e_out_of_memory));
		પાછા;
	    }
	    mustfree = TRUE;
	}
	જો (!message_filtered(name))
	{
	    જો (!did_title)
	    {
		// Highlight title
		સંદેશ_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    સંદેશ_putchar('\n');
	    જો (!got_int)
	    {
		sprintf((અક્ષર *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		સંદેશ_outtrans(IObuff);
		જો (name != ખાલી)
		{
		    સંદેશ_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	જો (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    શૂન્ય
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    પૂર્ણાંક		from, to;
    પૂર્ણાંક		i;
    પૂર્ણાંક		lower;
    પૂર્ણાંક		digit;
    પૂર્ણાંક		n;

    જો (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    નહીંતર જો (eap->forceit)
	emsg(_(e_invalid_argument));
    નહીંતર જો (*eap->arg == NUL)
	emsg(_(e_argument_required));
    નહીંતર
    {
	// clear specified marks only
	માટે (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    જો (lower || digit || ASCII_ISUPPER(*p))
	    {
		જો (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    જો (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			પાછા;
		    }
		    p += 2;
		}
		નહીંતર
		    // clear one lower case mark
		    from = to = *p;

		માટે (i = from; i <= to; ++i)
		{
		    જો (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    નહીંતર
		    {
			જો (digit)
			    n = i - '0' + NMARKS;
			નહીંતર
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
	    નહીંતર
		પસંદગી (*p)
		{
		    સ્થિતિ '"': curbuf->b_last_cursor.lnum = 0; તોડો;
		    સ્થિતિ '^': curbuf->b_last_ઉમેરો.lnum = 0; તોડો;
		    સ્થિતિ '.': curbuf->b_last_change.lnum = 0; તોડો;
		    સ્થિતિ '[': curbuf->b_op_start.lnum    = 0; તોડો;
		    સ્થિતિ ']': curbuf->b_op_end.lnum      = 0; તોડો;
		    સ્થિતિ '<': curbuf->b_visual.vi_start.lnum = 0; તોડો;
		    સ્થિતિ '>': curbuf->b_visual.vi_end.lnum   = 0; તોડો;
		    સ્થિતિ ' ': તોડો;
		    મૂળભૂત:  semsg(_(e_invalid_argument_str), p);
			      પાછા;
		}
	}
    }
}

/*
 * print the jumplist
 */
    શૂન્ય
ex_jumps(exarg_T *eap UNUSED)
{
    પૂર્ણાંક		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    સંદેશ_puts_title(_("\n jump line  col file/text"));
    માટે (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	જો (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    જો (name == ખાલી && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    જો (name == ખાલી || message_filtered(name))
	    {
		vim_free(name);
		ચાલુ;
	    }

	    સંદેશ_putchar('\n');
	    જો (got_int)
	    {
		vim_free(name);
		તોડો;
	    }
	    sprintf((અક્ષર *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    સંદેશ_outtrans(IObuff);
	    સંદેશ_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    જો (curwin->w_jumplistidx == curwin->w_jumplistlen)
	સંદેશ_puts("\n>");
}

    શૂન્ય
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    શૂન્ય
ex_changes(exarg_T *eap UNUSED)
{
    પૂર્ણાંક		i;
    char_u	*name;

    // Highlight title
    સંદેશ_puts_title(_("\nchange line  col text"));

    માટે (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	જો (curbuf->b_changelist[i].lnum != 0)
	{
	    સંદેશ_putchar('\n');
	    જો (got_int)
		તોડો;
	    sprintf((અક્ષર *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    સંદેશ_outtrans(IObuff);
	    name = mark_લીટી(&curbuf->b_changelist[i], 17);
	    જો (name == ખાલી)
		તોડો;
	    સંદેશ_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    જો (curwin->w_changelistidx == curbuf->b_changelistlen)
	સંદેશ_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	જો (*lp >= line1 && *lp <= line2) \
	{ \
	    જો (amount == MAXLNUM) \
		*lp = 0; \
	    નહીંતર \
		*lp += amount; \
	} \
	નહીંતર જો (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	જો (*lp >= line1 && *lp <= line2) \
	{ \
	    જો (amount == MAXLNUM) \
		*lp = line1; \
	    નહીંતર \
		*lp += amount; \
	} \
	નહીંતર જો (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	જો (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    જો (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    નહીંતર /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	નહીંતર જો (amount_after && posp->lnum > line2) \
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
    શૂન્ય
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    શૂન્ય
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    સ્થિર શૂન્ય
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    પૂર્ણાંક		adjust_folds UNUSED)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    સ્થિર pos_T initpos = {1, 0, 0};

    જો (line2 < line1 && amount_after == 0L)	    // nothing to do
	પાછા;

    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	માટે (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    જો (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	માટે (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    જો (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_ઉમેરો.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	જો (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	માટે (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ખાલી, line1, line2, amount, amount_after);
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
    જો (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    માટે (i = 0; i < win->w_jumplistlen; ++i)
		જો (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	જો (win->w_buffer == curbuf)
	{
	    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		માટે (i = 0; i < win->w_tagstacklen; i++)
		    જો (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    જો (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    જો (win != curwin)
	    {
		જો (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    જો (amount == MAXLNUM)	    // topline is deleted
		    {
			જો (line1 <= 1)
			    win->w_topline = 1;
			નહીંતર
			    win->w_topline = line1 - 1;
		    }
		    નહીંતર જો (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		નહીંતર જો (amount_after && win->w_topline > line2)
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
	    જો (adjust_folds)
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
	જો (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    જો (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    નહીંતર જો (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    નહીંતર \
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
    શૂન્ય
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    પૂર્ણાંક		spaces_removed)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    જો ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	પાછા; // nothing to do

    // named marks, lower case and upper case
    માટે (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	જો (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    માટે (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	જો (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_ઉમેરો));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    માટે (i = 0; i < curbuf->b_changelistlen; ++i)
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
	માટે (i = 0; i < win->w_jumplistlen; ++i)
	    જો (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	જો (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    માટે (i = 0; i < win->w_tagstacklen; i++)
		જો (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    જો (win != curwin)
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
    શૂન્ય
cleanup_jumplist(win_T *wp, પૂર્ણાંક loadfiles)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    from, to;
    પૂર્ણાંક	    mustfree;

    જો (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	માટે (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    જો ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    માટે (from = 0; from < wp->w_jumplistlen; ++from)
    {
	જો (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	માટે (i = from + 1; i < wp->w_jumplistlen; ++i)
	    જો (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		તોડો;
	જો (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	નહીંતર જો (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ઢગલો);
	નહીંતર				// adjacent duplicate
	    mustfree = TRUE;

	જો (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	નહીંતર
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    જો (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    શૂન્ય
copy_jumplist(win_T *from, win_T *to)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	જો (from->w_jumplist[i].fname != ખાલી)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    શૂન્ય
free_jumplist(win_T *wp)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    શૂન્ય
set_last_cursor(win_T *win)
{
    જો (win->w_buffer != ખાલી)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#જો defined(EXITFREE)
    શૂન્ય
free_all_marks(શૂન્ય)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	જો (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#જો defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(શૂન્ય)
{
    પાછા namedfm;
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    સ્થિર પૂર્ણાંક
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, પૂર્ણાંક bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    જો (pos->lnum <= 0)
	પાછા OK;

    d = dict_alloc();
    જો (d == ખાલી)
	પાછા FAIL;

    જો (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	પાછા FAIL;
    }

    lpos = list_alloc();
    જો (lpos == ખાલી)
	પાછા FAIL;

    list_append_સંખ્યા(lpos, bufnr);
    list_append_સંખ્યા(lpos, pos->lnum);
    list_append_સંખ્યા(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_સંખ્યા(lpos, pos->coladd);

    જો (dict_add_string_len(d, "mark", mname, (પૂર્ણાંક)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != ખાલી && dict_add_string(d, "file", fname) == FAIL))
    {
	જો (lpos->lv_refcount == 0)
	    list_free(lpos);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Get information about marks local to a buffer.
 */
    સ્થિર શૂન્ય
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    પૂર્ણાંક		i;

    // Marks 'a' to 'z'
    માટે (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ખાલી);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ખાલી);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_ઉમેરો, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, ખાલી);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, ખાલી);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    સ્થિર શૂન્ય
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    પૂર્ણાંક		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    માટે (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	જો (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	નહીંતર
	    name = namedfm[i].fname;
	જો (name != ખાલી)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    જો (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    શૂન્ય
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ખાલી;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	પાછા;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	પાછા;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
