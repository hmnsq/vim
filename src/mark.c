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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ସ୍ଥିର xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

ସ୍ଥିର ଶୂନ୍ୟ fname2fnum(xfmark_T *fm);
ସ୍ଥିର ଶୂନ୍ୟ fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
ସ୍ଥିର char_u *mark_ଧାଡ଼ି(pos_T *mp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead_len);
ସ୍ଥିର ଶୂନ୍ୟ show_one_mark(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *, pos_T *, char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା current);
ସ୍ଥିର ଶୂନ୍ୟ mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setmark(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setmark_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ଯଦି (c < 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (c == '\'' || c == '`')
    {
	ଯଦି (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	ଅନ୍ୟଥା
	    curwin->w_pcmark = *pos;
	ଫେରନ୍ତୁ OK;
    }

    buf = buflist_findnr(fnum);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (c == '"')
    {
	buf->b_last_cursor = *pos;
	ଫେରନ୍ତୁ OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ଯଦି (c == '[')
    {
	buf->b_op_start = *pos;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (c == ']')
    {
	buf->b_op_end = *pos;
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (c == '<' || c == '>')
    {
	ଯଦି (c == '<')
	    buf->b_visual.vi_start = *pos;
	ଅନ୍ୟଥା
	    buf->b_visual.vi_end = *pos;
	ଯଦି (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ଯଦି (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	ଅନ୍ୟଥା
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    ଶୂନ୍ୟ
mark_forget_file(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ଯଦି (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ଯଦି (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * ଆକାର(wp->w_jumplist[i]));
	}

    ପାଇଁ (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ଯଦି (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    ଯଦି (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * ଆକାର(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    ଶୂନ୍ୟ
setpcmark(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ଯଦି (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	ଫେରନ୍ତୁ;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ଯଦି (jop_flags & JOP_ଗଦା)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ଯଦି (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ଯଦି (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	ପାଇଁ (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = ଖାଲି;
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
    ଶୂନ୍ୟ
checkpcmark(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ଯଦି (curwin->w_jumplistlen == 0)	    // nothing to jump to
	ଫେରନ୍ତୁ (pos_T *)ଖାଲି;

    ପାଇଁ (;;)
    {
	ଯଦି (curwin->w_jumplistidx + ଗଣନା_2 < 0
		|| curwin->w_jumplistidx + ଗଣନା_2 >= curwin->w_jumplistlen)
	    ଫେରନ୍ତୁ (pos_T *)ଖାଲି;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ଯଦି (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ଯଦି (curwin->w_jumplistidx + ଗଣନା_2 < 0)
		ଫେରନ୍ତୁ (pos_T *)ଖାଲି;
	}

	curwin->w_jumplistidx += ଗଣନା_2;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ଯଦି (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ଯଦି (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ଯଦି (buflist_findnr(fmark.fnum) == ଖାଲି)
	    {					     // Skip this one ..
		ଗଣନା_2 += ଗଣନା_2 < 0 ? -1 : 1;
		ଜାରି;
	    }
	    ଯଦି (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		ଫେରନ୍ତୁ (pos_T *)ଖାଲି;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	ଅନ୍ୟଥା
	    pos = &(jmp->fmark.mark);
	ଫେରନ୍ତୁ pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (curbuf->b_changelistlen == 0)	    // nothing to jump to
	ଫେରନ୍ତୁ (pos_T *)ଖାଲି;

    n = curwin->w_changelistidx;
    ଯଦି (n + ଗଣନା_2 < 0)
    {
	ଯଦି (n == 0)
	    ଫେରନ୍ତୁ (pos_T *)ଖାଲି;
	n = 0;
    }
    ଅନ୍ୟଥା ଯଦି (n + ଗଣନା_2 >= curbuf->b_changelistlen)
    {
	ଯଦି (n == curbuf->b_changelistlen - 1)
	    ଫେରନ୍ତୁ (pos_T *)ଖାଲି;
	n = curbuf->b_changelistlen - 1;
    }
    ଅନ୍ୟଥା
	n += ଗଣନା_2;
    curwin->w_changelistidx = n;
    ଫେରନ୍ତୁ curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା changefile)
{
    ଫେରନ୍ତୁ getmark_buf_fnum(buf, c, changefile, ଖାଲି);
}

    pos_T *
getmark(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା changefile)
{
    ଫେରନ୍ତୁ getmark_buf_fnum(curbuf, c, changefile, ଖାଲି);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changefile,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    ସ୍ଥିର pos_T	pos_copy;

    posp = ଖାଲି;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ଯଦି (c < 0)
	ଫେରନ୍ତୁ posp;
    ଯଦି (c > '~')			// check for islower()/isupper()
	;
    ଅନ୍ୟଥା ଯଦି (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    ଅନ୍ୟଥା ଯଦି (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    ଅନ୍ୟଥା ଯଦି (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_ଭର୍ତ୍ତି);
    ଅନ୍ୟଥା ଯଦି (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    ଅନ୍ୟଥା ଯଦି (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    ଅନ୍ୟଥା ଯଦି (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    ଅନ୍ୟଥା ଯଦି (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ଯଦି (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ଅନ୍ୟଥା ଯଦି (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ଯଦି (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    ଅନ୍ୟଥା ଯଦି (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	ଯଦି (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	ଅନ୍ୟଥା
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ଯଦି (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ଯଦି (c == '<')
		pos_copy.col = 0;
	    ଅନ୍ୟଥା
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    ଅନ୍ୟଥା ଯଦି (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ଯଦି (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	ଅନ୍ୟଥା
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ଯଦି (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ଯଦି (fnum != ଖାଲି)
	    *fnum = namedfm[c].fmark.fnum;
	ଅନ୍ୟଥା ଯଦି (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ଯଦି (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ଯଦି (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    ଫେରନ୍ତୁ (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    ଅନ୍ୟଥା
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    ଫେରନ୍ତୁ posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,	// direction for search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		begin_ଧାଡ଼ି)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    pos_T	*result = ଖାଲି;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ଯଦି (dir == BACKWARD && begin_ଧାଡ଼ି)
	pos.col = 0;
    ଅନ୍ୟଥା ଯଦି (dir == FORWARD && begin_ଧାଡ଼ି)
	pos.col = MAXCOL;

    ପାଇଁ (i = 0; i < NMARKS; i++)
    {
	ଯଦି (curbuf->b_namedm[i].lnum > 0)
	{
	    ଯଦି (dir == FORWARD)
	    {
		ଯଦି ((result == ଖାଲି || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି ((result == ଖାଲି || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    ଫେରନ୍ତୁ result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    ଯଦି (fm->fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ଯଦି (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    ଅନ୍ୟଥା
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (ଶୂନ୍ୟ)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    ଶୂନ୍ୟ
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*wp;

    ଯଦି (buf->b_ffname == ଖାଲି)
	ଫେରନ୍ତୁ;

    name = home_replace_save(buf, buf->b_ffname);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ପାଇଁ (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    ସ୍ଥିର ଶୂନ୍ୟ
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ଯଦି (fm->fmark.fnum == 0
	    && fm->fname != ଖାଲି
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_mark(pos_T *pos)
{
    ଯଦି (pos == ଖାଲି)
    {
	emsg(_(e_unknown_mark));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ଯଦି (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (pos->lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	emsg(_(e_mark_has_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    ଶୂନ୍ୟ
clrallmarks(buf_T *buf)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = -1;

    ଯଦି (i == -1)	// first call ever: initialize
	ପାଇଁ (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = ଖାଲି;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    ପାଇଁ (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_ଭର୍ତ୍ତି.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead_len)
{
    ଯଦି (fmark->fnum == curbuf->b_fnum)		    // current buffer
	ଫେରନ୍ତୁ mark_ଧାଡ଼ି(&(fmark->mark), lead_len);
    ଫେରନ୍ତୁ buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    ସ୍ଥିର char_u *
mark_ଧାଡ଼ି(pos_T *mp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead_len)
{
    char_u	*s, *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    // Truncate the line to fit it in the window.
    len = 0;
    ପାଇଁ (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ଯଦି (len >= Columns - lead_len)
	    ଭାଙ୍ଗ;
    }
    *p = NUL;
    ଫେରନ୍ତୁ s;
}

/*
 * print the marks
 */
    ଶୂନ୍ୟ
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ଯଦି (arg != ଖାଲି && *arg == NUL)
	arg = ଖାଲି;

    show_one_mark('\'', arg, &curwin->w_pcmark, ଖାଲି, TRUE);
    ପାଇଁ (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], ଖାଲି, TRUE);
    ପାଇଁ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ଯଦି (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	ଅନ୍ୟଥା
	    name = namedfm[i].fname;
	ଯଦି (name != ଖାଲି)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ଯଦି (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, ଖାଲି, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, ଖାଲି, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, ଖାଲି, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_ଭର୍ତ୍ତି, ଖାଲି, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, ଖାଲି, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    ଯଦି ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    ଅନ୍ୟଥା
	posp = endp;
    show_one_mark('<', arg, posp, ଖାଲି, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, ଖାଲି, TRUE);

    show_one_mark(-1, arg, ଖାଲି, ଖାଲି, FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
show_one_mark(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current)	// in current file
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_title = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree = FALSE;
    char_u	*name = name_arg;

    ଯଦି (c == -1)			    // finish up
    {
	ଯଦି (did_title)
	    did_title = FALSE;
	ଅନ୍ୟଥା
	{
	    ଯଦି (arg == ଖାଲି)
		ସନ୍ଦେଶ(_("No marks set"));
	    ଅନ୍ୟଥା
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    ଅନ୍ୟଥା ଯଦି (!got_int
	    && (arg == ଖାଲି || vim_strchr(arg, c) != ଖାଲି)
	    && p->lnum != 0)
    {
	ଯଦି (name == ଖାଲି && current)
	{
	    name = mark_ଧାଡ଼ି(p, 15);
	    ଯଦି (name == ଖାଲି)
	    {
		emsg(_(e_out_of_memory));
		ଫେରନ୍ତୁ;
	    }
	    mustfree = TRUE;
	}
	ଯଦି (!message_filtered(name))
	{
	    ଯଦି (!did_title)
	    {
		// Highlight title
		ସନ୍ଦେଶ_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (!got_int)
	    {
		sprintf((ଅକ୍ଷର *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		ସନ୍ଦେଶ_outtrans(IObuff);
		ଯଦି (name != ଖାଲି)
		{
		    ସନ୍ଦେଶ_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	ଯଦି (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    ଶୂନ୍ୟ
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		from, to;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lower;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		digit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    ଅନ୍ୟଥା ଯଦି (eap->forceit)
	emsg(_(e_invalid_argument));
    ଅନ୍ୟଥା ଯଦି (*eap->arg == NUL)
	emsg(_(e_argument_required));
    ଅନ୍ୟଥା
    {
	// clear specified marks only
	ପାଇଁ (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ଯଦି (lower || digit || ASCII_ISUPPER(*p))
	    {
		ଯଦି (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    ଯଦି (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			ଫେରନ୍ତୁ;
		    }
		    p += 2;
		}
		ଅନ୍ୟଥା
		    // clear one lower case mark
		    from = to = *p;

		ପାଇଁ (i = from; i <= to; ++i)
		{
		    ଯଦି (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (digit)
			    n = i - '0' + NMARKS;
			ଅନ୍ୟଥା
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
	    ଅନ୍ୟଥା
		ବାଛ (*p)
		{
		    ସ୍ଥିତି '"': curbuf->b_last_cursor.lnum = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '^': curbuf->b_last_ଭର୍ତ୍ତି.lnum = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '.': curbuf->b_last_change.lnum = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '[': curbuf->b_op_start.lnum    = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି ']': curbuf->b_op_end.lnum      = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '<': curbuf->b_visual.vi_start.lnum = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି '>': curbuf->b_visual.vi_end.lnum   = 0; ଭାଙ୍ଗ;
		    ସ୍ଥିତି ' ': ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  semsg(_(e_invalid_argument_str), p);
			      ଫେରନ୍ତୁ;
		}
	}
    }
}

/*
 * print the jumplist
 */
    ଶୂନ୍ୟ
ex_jumps(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    ସନ୍ଦେଶ_puts_title(_("\n jump line  col file/text"));
    ପାଇଁ (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ଯଦି (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ଯଦି (name == ଖାଲି && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ଯଦି (name == ଖାଲି || message_filtered(name))
	    {
		vim_free(name);
		ଜାରି;
	    }

	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
	    {
		vim_free(name);
		ଭାଙ୍ଗ;
	    }
	    sprintf((ଅକ୍ଷର *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    ସନ୍ଦେଶ_outtrans(IObuff);
	    ସନ୍ଦେଶ_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ଯଦି (curwin->w_jumplistidx == curwin->w_jumplistlen)
	ସନ୍ଦେଶ_puts("\n>");
}

    ଶୂନ୍ୟ
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    ଶୂନ୍ୟ
ex_changes(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;

    // Highlight title
    ସନ୍ଦେଶ_puts_title(_("\nchange line  col text"));

    ପାଇଁ (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ଯଦି (curbuf->b_changelist[i].lnum != 0)
	{
	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    sprintf((ଅକ୍ଷର *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    ସନ୍ଦେଶ_outtrans(IObuff);
	    name = mark_ଧାଡ଼ି(&curbuf->b_changelist[i], 17);
	    ଯଦି (name == ଖାଲି)
		ଭାଙ୍ଗ;
	    ସନ୍ଦେଶ_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ଯଦି (curwin->w_changelistidx == curbuf->b_changelistlen)
	ସନ୍ଦେଶ_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	ଯଦି (*lp >= line1 && *lp <= line2) \
	{ \
	    ଯଦି (amount == MAXLNUM) \
		*lp = 0; \
	    ଅନ୍ୟଥା \
		*lp += amount; \
	} \
	ଅନ୍ୟଥା ଯଦି (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	ଯଦି (*lp >= line1 && *lp <= line2) \
	{ \
	    ଯଦି (amount == MAXLNUM) \
		*lp = line1; \
	    ଅନ୍ୟଥା \
		*lp += amount; \
	} \
	ଅନ୍ୟଥା ଯଦି (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ଯଦି (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ଯଦି (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    ଅନ୍ୟଥା /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	ଅନ୍ୟଥା ଯଦି (amount_after && posp->lnum > line2) \
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
    ଶୂନ୍ୟ
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    ଶୂନ୍ୟ
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		adjust_folds UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    ସ୍ଥିର pos_T initpos = {1, 0, 0};

    ଯଦି (line2 < line1 && amount_after == 0L)	    // nothing to do
	ଫେରନ୍ତୁ;

    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	ପାଇଁ (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ଯଦି (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	ପାଇଁ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ଯଦି (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_ଭର୍ତ୍ତି.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	ଯଦି (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	ପାଇଁ (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(ଖାଲି, line1, line2, amount, amount_after);
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
    ଯଦି (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    ପାଇଁ (i = 0; i < win->w_jumplistlen; ++i)
		ଯଦି (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ଯଦି (win->w_buffer == curbuf)
	{
	    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		ପାଇଁ (i = 0; i < win->w_tagstacklen; i++)
		    ଯଦି (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ଯଦି (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ଯଦି (win != curwin)
	    {
		ଯଦି (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ଯଦି (amount == MAXLNUM)	    // topline is deleted
		    {
			ଯଦି (line1 <= 1)
			    win->w_topline = 1;
			ଅନ୍ୟଥା
			    win->w_topline = line1 - 1;
		    }
		    ଅନ୍ୟଥା ଯଦି (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		ଅନ୍ୟଥା ଯଦି (amount_after && win->w_topline > line2)
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
	    ଯଦି (adjust_folds)
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
	ଯଦି (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ଯଦି (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    ଅନ୍ୟଥା ଯଦି (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    ଅନ୍ୟଥା \
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
    ଶୂନ୍ୟ
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		spaces_removed)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ଯଦି ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	ଫେରନ୍ତୁ; // nothing to do

    // named marks, lower case and upper case
    ପାଇଁ (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	ଯଦି (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    ପାଇଁ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ଯଦି (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_ଭର୍ତ୍ତି));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    ପାଇଁ (i = 0; i < curbuf->b_changelistlen; ++i)
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
	ପାଇଁ (i = 0; i < win->w_jumplistlen; ++i)
	    ଯଦି (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	ଯଦି (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    ପାଇଁ (i = 0; i < win->w_tagstacklen; i++)
		ଯଦି (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ଯଦି (win != curwin)
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
    ଶୂନ୍ୟ
cleanup_jumplist(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା loadfiles)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    from, to;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mustfree;

    ଯଦି (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	ପାଇଁ (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ଯଦି ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    ପାଇଁ (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ଯଦି (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	ପାଇଁ (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ଯଦି (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		ଭାଙ୍ଗ;
	ଯଦି (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	ଅନ୍ୟଥା ଯଦି (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_ଗଦା);
	ଅନ୍ୟଥା				// adjacent duplicate
	    mustfree = TRUE;

	ଯଦି (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	ଅନ୍ୟଥା
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    ଯଦି (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    ଶୂନ୍ୟ
copy_jumplist(win_T *from, win_T *to)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	ଯଦି (from->w_jumplist[i].fname != ଖାଲି)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    ଶୂନ୍ୟ
free_jumplist(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    ଶୂନ୍ୟ
set_last_cursor(win_T *win)
{
    ଯଦି (win->w_buffer != ଖାଲି)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_all_marks(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ଯଦି (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#ଯଦି defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ namedfm;
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ଯଦି (pos->lnum <= 0)
	ଫେରନ୍ତୁ OK;

    d = dict_alloc();
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	ଫେରନ୍ତୁ FAIL;
    }

    lpos = list_alloc();
    ଯଦି (lpos == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    list_append_ସଂଖ୍ୟା(lpos, bufnr);
    list_append_ସଂଖ୍ୟା(lpos, pos->lnum);
    list_append_ସଂଖ୍ୟା(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_ସଂଖ୍ୟା(lpos, pos->coladd);

    ଯଦି (dict_add_string_len(d, "mark", mname, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != ଖାଲି && dict_add_string(d, "file", fname) == FAIL))
    {
	ଯଦି (lpos->lv_refcount == 0)
	    list_free(lpos);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Get information about marks local to a buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Marks 'a' to 'z'
    ପାଇଁ (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, ଖାଲି);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, ଖାଲି);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_ଭର୍ତ୍ତି, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, ଖାଲି);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, ଖାଲି);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    ପାଇଁ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ଯଦି (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	ଅନ୍ୟଥା
	    name = namedfm[i].fname;
	ଯଦି (name != ଖାଲି)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ଯଦି (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    ଶୂନ୍ୟ
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ଖାଲି;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	ଫେରନ୍ତୁ;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
