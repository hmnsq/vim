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

#include "vim.h"

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
static xfmark_T namedfm[NMARKS + EXTRA_MARKS];		// marks with file nr

static void fname2fnum(xfmark_T *fm);
static void fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf);
static char_u *mark_line(pos_T *mp, ចំនួនគត់ lead_len);
static void show_one_mark(ចំនួនគត់, char_u *, pos_T *, char_u *, ចំនួនគត់ current);
static void mark_adjust_internal(linenr_T line1, linenr_T line2, long amount,
    long amount_after, ចំនួនគត់ adjust_folds);

/*
 * Set named mark "c" at current cursor position.
 * Returns OK on success, FAIL if bad name given.
 */
    ចំនួនគត់
setmark(ចំនួនគត់ c)
{
    ត្រឡប់ setmark_pos(c, &curwin->w_cursor, curbuf->b_fnum);
}

/*
 * Set named mark "c" to position "pos".
 * When "c" is upper case use file "fnum".
 * Returns OK on success, FAIL if bad name given.
 */
    ចំនួនគត់
setmark_pos(ចំនួនគត់ c, pos_T *pos, ចំនួនគត់ fnum)
{
    ចំនួនគត់		i;
    buf_T	*buf;

    // Check for a special key (may cause islower() to crash).
    ប្រសិនបើ (c < 0)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (c == '\'' || c == '`')
    {
	ប្រសិនបើ (pos == &curwin->w_cursor)
	{
	    setpcmark();
	    // keep it even when the cursor doesn't move
	    curwin->w_prev_pcmark = curwin->w_pcmark;
	}
	បើមិនដូច្នេះ
	    curwin->w_pcmark = *pos;
	ត្រឡប់ OK;
    }

    buf = buflist_findnr(fnum);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (c == '"')
    {
	buf->b_last_cursor = *pos;
	ត្រឡប់ OK;
    }

    // Allow setting '[ and '] for an autocommand that simulates reading a
    // file.
    ប្រសិនបើ (c == '[')
    {
	buf->b_op_start = *pos;
	ត្រឡប់ OK;
    }
    ប្រសិនបើ (c == ']')
    {
	buf->b_op_end = *pos;
	ត្រឡប់ OK;
    }

    ប្រសិនបើ (c == '<' || c == '>')
    {
	ប្រសិនបើ (c == '<')
	    buf->b_visual.vi_start = *pos;
	បើមិនដូច្នេះ
	    buf->b_visual.vi_end = *pos;
	ប្រសិនបើ (buf->b_visual.vi_mode == NUL)
	    // Visual_mode has not yet been set, use a sane default.
	    buf->b_visual.vi_mode = 'v';
	ត្រឡប់ OK;
    }

    ប្រសិនបើ (ASCII_ISLOWER(c))
    {
	i = c - 'a';
	buf->b_namedm[i] = *pos;
	ត្រឡប់ OK;
    }
    ប្រសិនបើ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))
    {
	ប្រសិនបើ (VIM_ISDIGIT(c))
	    i = c - '0' + NMARKS;
	បើមិនដូច្នេះ
	    i = c - 'A';
	namedfm[i].fmark.mark = *pos;
	namedfm[i].fmark.fnum = fnum;
	VIM_CLEAR(namedfm[i].fname);
#ifdef FEAT_VIMINFO
	namedfm[i].time_set = vim_time();
#endif
	ត្រឡប់ OK;
    }
    ត្រឡប់ FAIL;
}

/*
 * Delete every entry referring to file 'fnum' from both the jumplist and the
 * tag stack.
 */
    void
mark_forget_file(win_T *wp, ចំនួនគត់ fnum)
{
    ចំនួនគត់		i;

    សម្រាប់ (i = wp->w_jumplistlen - 1; i >= 0; --i)
	ប្រសិនបើ (wp->w_jumplist[i].fmark.fnum == fnum)
	{
	    vim_free(wp->w_jumplist[i].fname);
	    ប្រសិនបើ (wp->w_jumplistidx > i)
		--wp->w_jumplistidx;
	    --wp->w_jumplistlen;
	    mch_memmove(&wp->w_jumplist[i], &wp->w_jumplist[i + 1],
			(wp->w_jumplistlen - i) * parimaa_na(wp->w_jumplist[i]));
	}

    សម្រាប់ (i = wp->w_tagstacklen - 1; i >= 0; --i)
	ប្រសិនបើ (wp->w_tagstack[i].fmark.fnum == fnum)
	{
	    tagstack_clear_entry(&wp->w_tagstack[i]);
	    ប្រសិនបើ (wp->w_tagstackidx > i)
		--wp->w_tagstackidx;
	    --wp->w_tagstacklen;
	    mch_memmove(&wp->w_tagstack[i], &wp->w_tagstack[i + 1],
			(wp->w_tagstacklen - i) * parimaa_na(wp->w_tagstack[i]));
	}
}

/*
 * Set the previous context mark to the current position and add it to the
 * jump list.
 */
    void
setpcmark(void)
{
    ចំនួនគត់		i;
    xfmark_T	*fm;

    // for :global the mark is set only once
    ប្រសិនបើ (global_busy || listcmd_busy || (cmdmod.cmod_flags & CMOD_KEEPJUMPS))
	ត្រឡប់;

    curwin->w_prev_pcmark = curwin->w_pcmark;
    curwin->w_pcmark = curwin->w_cursor;

    ប្រសិនបើ (jop_flags & JOP_STACK)
    {
	// jumpoptions=stack: if we're somewhere in the middle of the jumplist
	// discard everything after the current index.
	ប្រសិនបើ (curwin->w_jumplistidx < curwin->w_jumplistlen - 1)
	    // Discard the rest of the jumplist by cutting the length down to
	    // contain nothing beyond the current index.
	    curwin->w_jumplistlen = curwin->w_jumplistidx + 1;
    }

    // If jumplist is full: remove oldest entry
    ប្រសិនបើ (++curwin->w_jumplistlen > JUMPLISTSIZE)
    {
	curwin->w_jumplistlen = JUMPLISTSIZE;
	vim_free(curwin->w_jumplist[0].fname);
	សម្រាប់ (i = 1; i < JUMPLISTSIZE; ++i)
	    curwin->w_jumplist[i - 1] = curwin->w_jumplist[i];
    }
    curwin->w_jumplistidx = curwin->w_jumplistlen;
    fm = &curwin->w_jumplist[curwin->w_jumplistlen - 1];

    fm->fmark.mark = curwin->w_pcmark;
    fm->fmark.fnum = curbuf->b_fnum;
    fm->fname = NULL;
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
    void
checkpcmark(void)
{
    ប្រសិនបើ (curwin->w_prev_pcmark.lnum != 0
	    && (EQUAL_POS(curwin->w_pcmark, curwin->w_cursor)
		|| curwin->w_pcmark.lnum == 0))
	curwin->w_pcmark = curwin->w_prev_pcmark;
    curwin->w_prev_pcmark.lnum = 0;		// it has been checked
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
    pos_T *
movemark(ចំនួនគត់ count)
{
    pos_T	*pos;
    xfmark_T	*jmp;

    cleanup_jumplist(curwin, TRUE);

    ប្រសិនបើ (curwin->w_jumplistlen == 0)	    // nothing to jump to
	ត្រឡប់ (pos_T *)NULL;

    សម្រាប់ (;;)
    {
	ប្រសិនបើ (curwin->w_jumplistidx + count < 0
		|| curwin->w_jumplistidx + count >= curwin->w_jumplistlen)
	    ត្រឡប់ (pos_T *)NULL;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position
	 * to list.  Careful: If there are duplicates (CTRL-O immediately after
	 * starting Vim on a file), another entry may have been removed.
	 */
	ប្រសិនបើ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	{
	    setpcmark();
	    --curwin->w_jumplistidx;	// skip the new entry
	    ប្រសិនបើ (curwin->w_jumplistidx + count < 0)
		ត្រឡប់ (pos_T *)NULL;
	}

	curwin->w_jumplistidx += count;

	jmp = curwin->w_jumplist + curwin->w_jumplistidx;
	ប្រសិនបើ (jmp->fmark.fnum == 0)
	    fname2fnum(jmp);
	ប្រសិនបើ (jmp->fmark.fnum != curbuf->b_fnum)
	{
	    // Make a copy, an autocommand may make "jmp" invalid.
	    fmark_T fmark = jmp->fmark;

	    // jump to the file with the mark
	    ប្រសិនបើ (buflist_findnr(fmark.fnum) == NULL)
	    {					     // Skip this one ..
		count += count < 0 ? -1 : 1;
		continue;
	    }
	    ប្រសិនបើ (buflist_getfile(fmark.fnum, fmark.mark.lnum, 0, FALSE) == FAIL)
		ត្រឡប់ (pos_T *)NULL;
	    // Set lnum again, autocommands my have changed it
	    curwin->w_cursor = fmark.mark;
	    pos = (pos_T *)-1;
	}
	បើមិនដូច្នេះ
	    pos = &(jmp->fmark.mark);
	ត្រឡប់ pos;
    }
}

/*
 * Move "count" positions in the changelist (count may be negative).
 */
    pos_T *
movechangelist(ចំនួនគត់ count)
{
    ចំនួនគត់		n;

    ប្រសិនបើ (curbuf->b_changelistlen == 0)	    // nothing to jump to
	ត្រឡប់ (pos_T *)NULL;

    n = curwin->w_changelistidx;
    ប្រសិនបើ (n + count < 0)
    {
	ប្រសិនបើ (n == 0)
	    ត្រឡប់ (pos_T *)NULL;
	n = 0;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (n + count >= curbuf->b_changelistlen)
    {
	ប្រសិនបើ (n == curbuf->b_changelistlen - 1)
	    ត្រឡប់ (pos_T *)NULL;
	n = curbuf->b_changelistlen - 1;
    }
    បើមិនដូច្នេះ
	n += count;
    curwin->w_changelistidx = n;
    ត្រឡប់ curbuf->b_changelist + n;
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
getmark_buf(buf_T *buf, ចំនួនគត់ c, ចំនួនគត់ changefile)
{
    ត្រឡប់ getmark_buf_fnum(buf, c, changefile, NULL);
}

    pos_T *
getmark(ចំនួនគត់ c, ចំនួនគត់ changefile)
{
    ត្រឡប់ getmark_buf_fnum(curbuf, c, changefile, NULL);
}

    pos_T *
getmark_buf_fnum(
    buf_T	*buf,
    ចំនួនគត់		c,
    ចំនួនគត់		changefile,
    ចំនួនគត់		*fnum)
{
    pos_T		*posp;
    pos_T		*startp, *endp;
    static pos_T	pos_copy;

    posp = NULL;

    // Check for special key, can't be a mark name and might cause islower()
    // to crash.
    ប្រសិនបើ (c < 0)
	ត្រឡប់ posp;
    ប្រសិនបើ (c > '~')			// check for islower()/isupper()
	;
    បើមិនដូច្នេះ ប្រសិនបើ (c == '\'' || c == '`')	// previous context mark
    {
	pos_copy = curwin->w_pcmark;	// need to make a copy because
	posp = &pos_copy;		//   w_pcmark may be changed soon
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == '"')			// to pos when leaving buffer
	posp = &(buf->b_last_cursor);
    បើមិនដូច្នេះ ប្រសិនបើ (c == '^')			// to where Insert mode stopped
	posp = &(buf->b_last_insert);
    បើមិនដូច្នេះ ប្រសិនបើ (c == '.')			// to where last change was made
	posp = &(buf->b_last_change);
    បើមិនដូច្នេះ ប្រសិនបើ (c == '[')			// to start of previous operator
	posp = &(buf->b_op_start);
    បើមិនដូច្នេះ ប្រសិនបើ (c == ']')			// to end of previous operator
	posp = &(buf->b_op_end);
    បើមិនដូច្នេះ ប្រសិនបើ (c == '{' || c == '}')	// to previous/next paragraph
    {
	pos_T	pos;
	oparg_T	oa;
	ចំនួនគត់	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ប្រសិនបើ (findpar(&oa.inclusive,
			       c == '}' ? FORWARD : BACKWARD, 1L, NUL, FALSE))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == '(' || c == ')')	// to previous/next sentence
    {
	pos_T	pos;
	ចំនួនគត់	slcb = listcmd_busy;

	pos = curwin->w_cursor;
	listcmd_busy = TRUE;	    // avoid that '' is changed
	ប្រសិនបើ (findsent(c == ')' ? FORWARD : BACKWARD, 1L))
	{
	    pos_copy = curwin->w_cursor;
	    posp = &pos_copy;
	}
	curwin->w_cursor = pos;
	listcmd_busy = slcb;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (c == '<' || c == '>')	// start/end of visual area
    {
	startp = &buf->b_visual.vi_start;
	endp = &buf->b_visual.vi_end;
	ប្រសិនបើ (((c == '<') == LT_POS(*startp, *endp) || endp->lnum == 0)
							  && startp->lnum != 0)
	    posp = startp;
	បើមិនដូច្នេះ
	    posp = endp;
	/*
	 * For Visual line mode, set mark at begin or end of line
	 */
	ប្រសិនបើ (buf->b_visual.vi_mode == 'V')
	{
	    pos_copy = *posp;
	    posp = &pos_copy;
	    ប្រសិនបើ (c == '<')
		pos_copy.col = 0;
	    បើមិនដូច្នេះ
		pos_copy.col = MAXCOL;
	    pos_copy.coladd = 0;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (ASCII_ISLOWER(c))		// normal named mark
    {
	posp = &(buf->b_namedm[c - 'a']);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (ASCII_ISUPPER(c) || VIM_ISDIGIT(c))	// named file mark
    {
	ប្រសិនបើ (VIM_ISDIGIT(c))
	    c = c - '0' + NMARKS;
	បើមិនដូច្នេះ
	    c -= 'A';
	posp = &(namedfm[c].fmark.mark);

	ប្រសិនបើ (namedfm[c].fmark.fnum == 0)
	    fname2fnum(&namedfm[c]);

	ប្រសិនបើ (fnum != NULL)
	    *fnum = namedfm[c].fmark.fnum;
	បើមិនដូច្នេះ ប្រសិនបើ (namedfm[c].fmark.fnum != buf->b_fnum)
	{
	    // mark is in another file
	    posp = &pos_copy;

	    ប្រសិនបើ (namedfm[c].fmark.mark.lnum != 0
				       && changefile && namedfm[c].fmark.fnum)
	    {
		ប្រសិនបើ (buflist_getfile(namedfm[c].fmark.fnum,
				      (linenr_T)1, GETF_SETMARK, FALSE) == OK)
		{
		    // Set the lnum now, autocommands could have changed it
		    curwin->w_cursor = namedfm[c].fmark.mark;
		    ត្រឡប់ (pos_T *)-1;
		}
		pos_copy.lnum = -1;	// can't get file
	    }
	    បើមិនដូច្នេះ
		pos_copy.lnum = 0;	// mark exists, but is not valid in
					// current buffer
	}
    }

    ត្រឡប់ posp;
}

/*
 * Search for the next named mark in the current file.
 *
 * Returns pointer to pos_T of the next mark or NULL if no mark is found.
 */
    pos_T *
getnextmark(
    pos_T	*startpos,	// where to start
    ចំនួនគត់		dir,	// direction for search
    ចំនួនគត់		begin_line)
{
    ចំនួនគត់		i;
    pos_T	*result = NULL;
    pos_T	pos;

    pos = *startpos;

    // When searching backward and leaving the cursor on the first non-blank,
    // position must be in a previous line.
    // When searching forward and leaving the cursor on the first non-blank,
    // position must be in a next line.
    ប្រសិនបើ (dir == BACKWARD && begin_line)
	pos.col = 0;
    បើមិនដូច្នេះ ប្រសិនបើ (dir == FORWARD && begin_line)
	pos.col = MAXCOL;

    សម្រាប់ (i = 0; i < NMARKS; i++)
    {
	ប្រសិនបើ (curbuf->b_namedm[i].lnum > 0)
	{
	    ប្រសិនបើ (dir == FORWARD)
	    {
		ប្រសិនបើ ((result == NULL || LT_POS(curbuf->b_namedm[i], *result))
			&& LT_POS(pos, curbuf->b_namedm[i]))
		    result = &curbuf->b_namedm[i];
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ ((result == NULL || LT_POS(*result, curbuf->b_namedm[i]))
			&& LT_POS(curbuf->b_namedm[i], pos))
		    result = &curbuf->b_namedm[i];
	    }
	}
    }

    ត្រឡប់ result;
}

/*
 * For an xtended filemark: set the fnum from the fname.
 * This is used for marks obtained from the .viminfo file.  It's postponed
 * until the mark is used to avoid a long startup delay.
 */
    static void
fname2fnum(xfmark_T *fm)
{
    char_u	*p;

    ប្រសិនបើ (fm->fname == NULL)
	ត្រឡប់;

    /*
     * First expand "~/" in the file name to the home directory.
     * Don't expand the whole name, it may contain other '~' chars.
     */
    ប្រសិនបើ (fm->fname[0] == '~' && (fm->fname[1] == '/'
#ifdef BACKSLASH_IN_FILENAME
		|| fm->fname[1] == '\\'
#endif
		))
    {
	size_t len;

	len = expand_env((char_u *)"~/", NameBuff, MAXPATHL);
	vim_strncpy(NameBuff + len, fm->fname + 2, MAXPATHL - len - 1);
    }
    បើមិនដូច្នេះ
	vim_strncpy(NameBuff, fm->fname, MAXPATHL - 1);

    // Try to shorten the file name.
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(NameBuff, IObuff);

    // buflist_new() will call fmarks_check_names()
    (void)buflist_new(NameBuff, p, (linenr_T)1, 0);
}

/*
 * Check all file marks for a name that matches the file name in buf.
 * May replace the name with an fnum.
 * Used for marks that come from the .viminfo file.
 */
    void
fmarks_check_names(buf_T *buf)
{
    char_u	*name;
    ចំនួនគត់		i;
    win_T	*wp;

    ប្រសិនបើ (buf->b_ffname == NULL)
	ត្រឡប់;

    name = home_replace_save(buf, buf->b_ffname);
    ប្រសិនបើ (name == NULL)
	ត្រឡប់;

    សម្រាប់ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
	fmarks_check_one(&namedfm[i], name, buf);

    FOR_ALL_WINDOWS(wp)
    {
	សម្រាប់ (i = 0; i < wp->w_jumplistlen; ++i)
	    fmarks_check_one(&wp->w_jumplist[i], name, buf);
    }

    vim_free(name);
}

    static void
fmarks_check_one(xfmark_T *fm, char_u *name, buf_T *buf)
{
    ប្រសិនបើ (fm->fmark.fnum == 0
	    && fm->fname != NULL
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
    ចំនួនគត់
check_mark(pos_T *pos)
{
    ប្រសិនបើ (pos == NULL)
    {
	emsg(_(e_unknown_mark));
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (pos->lnum <= 0)
    {
	// lnum is negative if mark is in another file can can't get that
	// file, error message already give then.
	ប្រសិនបើ (pos->lnum == 0)
	    emsg(_(e_mark_not_set));
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (pos->lnum > curbuf->b_ml.ml_line_count)
    {
	emsg(_(e_mark_has_invalid_line_number));
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * clrallmarks() - clear all marks in the buffer 'buf'
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
    void
clrallmarks(buf_T *buf)
{
    static ចំនួនគត់		i = -1;

    ប្រសិនបើ (i == -1)	// first call ever: initialize
	សម្រាប់ (i = 0; i < NMARKS + 1; i++)
	{
	    namedfm[i].fmark.mark.lnum = 0;
	    namedfm[i].fname = NULL;
#ifdef FEAT_VIMINFO
	    namedfm[i].time_set = 0;
#endif
	}

    សម្រាប់ (i = 0; i < NMARKS; i++)
	buf->b_namedm[i].lnum = 0;
    buf->b_op_start.lnum = 0;		// start/end op mark cleared
    buf->b_op_end.lnum = 0;
    buf->b_last_cursor.lnum = 1;	// '" mark cleared
    buf->b_last_cursor.col = 0;
    buf->b_last_cursor.coladd = 0;
    buf->b_last_insert.lnum = 0;	// '^ mark cleared
    buf->b_last_change.lnum = 0;	// '. mark cleared
    buf->b_changelistlen = 0;
}

/*
 * Get name of file from a filemark.
 * When it's in the current buffer, return the text at the mark.
 * Returns an allocated string.
 */
    char_u *
fm_getname(fmark_T *fmark, ចំនួនគត់ lead_len)
{
    ប្រសិនបើ (fmark->fnum == curbuf->b_fnum)		    // current buffer
	ត្រឡប់ mark_line(&(fmark->mark), lead_len);
    ត្រឡប់ buflist_nr2name(fmark->fnum, FALSE, TRUE);
}

/*
 * Return the line at mark "mp".  Truncate to fit in window.
 * The returned string has been allocated.
 */
    static char_u *
mark_line(pos_T *mp, ចំនួនគត់ lead_len)
{
    char_u	*s, *p;
    ចំនួនគត់		len;

    ប្រសិនបើ (mp->lnum == 0 || mp->lnum > curbuf->b_ml.ml_line_count)
	ត្រឡប់ vim_strsave((char_u *)"-invalid-");
    // Allow for up to 5 bytes per character.
    s = vim_strnsave(skipwhite(ml_get(mp->lnum)), Columns * 5);
    ប្រសិនបើ (s == NULL)
	ត្រឡប់ NULL;
    // Truncate the line to fit it in the window.
    len = 0;
    សម្រាប់ (p = s; *p != NUL; MB_PTR_ADV(p))
    {
	len += ptr2cells(p);
	ប្រសិនបើ (len >= Columns - lead_len)
	    bha_nga;
    }
    *p = NUL;
    ត្រឡប់ s;
}

/*
 * print the marks
 */
    void
ex_marks(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ចំនួនគត់		i;
    char_u	*name;
    pos_T	*posp, *startp, *endp;

    ប្រសិនបើ (arg != NULL && *arg == NUL)
	arg = NULL;

    show_one_mark('\'', arg, &curwin->w_pcmark, NULL, TRUE);
    សម្រាប់ (i = 0; i < NMARKS; ++i)
	show_one_mark(i + 'a', arg, &curbuf->b_namedm[i], NULL, TRUE);
    សម្រាប់ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ប្រសិនបើ (namedfm[i].fmark.fnum != 0)
	    name = fm_getname(&namedfm[i].fmark, 15);
	បើមិនដូច្នេះ
	    name = namedfm[i].fname;
	ប្រសិនបើ (name != NULL)
	{
	    show_one_mark(i >= NMARKS ? i - NMARKS + '0' : i + 'A',
		    arg, &namedfm[i].fmark.mark, name,
		    namedfm[i].fmark.fnum == curbuf->b_fnum);
	    ប្រសិនបើ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
    show_one_mark('"', arg, &curbuf->b_last_cursor, NULL, TRUE);
    show_one_mark('[', arg, &curbuf->b_op_start, NULL, TRUE);
    show_one_mark(']', arg, &curbuf->b_op_end, NULL, TRUE);
    show_one_mark('^', arg, &curbuf->b_last_insert, NULL, TRUE);
    show_one_mark('.', arg, &curbuf->b_last_change, NULL, TRUE);

    // Show the marks as where they will jump to.
    startp = &curbuf->b_visual.vi_start;
    endp = &curbuf->b_visual.vi_end;
    ប្រសិនបើ ((LT_POS(*startp, *endp) || endp->lnum == 0) && startp->lnum != 0)
	posp = startp;
    បើមិនដូច្នេះ
	posp = endp;
    show_one_mark('<', arg, posp, NULL, TRUE);
    show_one_mark('>', arg, posp == startp ? endp : startp, NULL, TRUE);

    show_one_mark(-1, arg, NULL, NULL, FALSE);
}

    static void
show_one_mark(
    ចំនួនគត់		c,
    char_u	*arg,
    pos_T	*p,
    char_u	*name_arg,
    ចំនួនគត់		current)	// in current file
{
    static ចំនួនគត់	did_title = FALSE;
    ចំនួនគត់		mustfree = FALSE;
    char_u	*name = name_arg;

    ប្រសិនបើ (c == -1)			    // finish up
    {
	ប្រសិនបើ (did_title)
	    did_title = FALSE;
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (arg == NULL)
		msg(_("No marks set"));
	    បើមិនដូច្នេះ
		semsg(_(e_no_marks_matching_str), arg);
	}
    }
    // don't output anything if 'q' typed at --more-- prompt
    បើមិនដូច្នេះ ប្រសិនបើ (!got_int
	    && (arg == NULL || vim_strchr(arg, c) != NULL)
	    && p->lnum != 0)
    {
	ប្រសិនបើ (name == NULL && current)
	{
	    name = mark_line(p, 15);
	    ប្រសិនបើ (name == NULL)
	    {
		emsg(_(e_out_of_memory));
		ត្រឡប់;
	    }
	    mustfree = TRUE;
	}
	ប្រសិនបើ (!message_filtered(name))
	{
	    ប្រសិនបើ (!did_title)
	    {
		// Highlight title
		msg_puts_title(_("\nmark line  col file/text"));
		did_title = TRUE;
	    }
	    msg_putchar('\n');
	    ប្រសិនបើ (!got_int)
	    {
		sprintf((var_na *)IObuff, " %c %6ld %4d ", c, p->lnum, p->col);
		msg_outtrans(IObuff);
		ប្រសិនបើ (name != NULL)
		{
		    msg_outtrans_attr(name, current ? HL_ATTR(HLF_D) : 0);
		}
	    }
	    out_flush();		    // show one line at a time
	}
	ប្រសិនបើ (mustfree)
	    vim_free(name);
    }
}

/*
 * ":delmarks[!] [marks]"
 */
    void
ex_delmarks(exarg_T *eap)
{
    char_u	*p;
    ចំនួនគត់		from, to;
    ចំនួនគត់		i;
    ចំនួនគត់		lower;
    ចំនួនគត់		digit;
    ចំនួនគត់		n;

    ប្រសិនបើ (*eap->arg == NUL && eap->forceit)
	// clear all marks
	clrallmarks(curbuf);
    បើមិនដូច្នេះ ប្រសិនបើ (eap->forceit)
	emsg(_(e_invalid_argument));
    បើមិនដូច្នេះ ប្រសិនបើ (*eap->arg == NUL)
	emsg(_(e_argument_required));
    បើមិនដូច្នេះ
    {
	// clear specified marks only
	សម្រាប់ (p = eap->arg; *p != NUL; ++p)
	{
	    lower = ASCII_ISLOWER(*p);
	    digit = VIM_ISDIGIT(*p);
	    ប្រសិនបើ (lower || digit || ASCII_ISUPPER(*p))
	    {
		ប្រសិនបើ (p[1] == '-')
		{
		    // clear range of marks
		    from = *p;
		    to = p[2];
		    ប្រសិនបើ (!(lower ? ASCII_ISLOWER(p[2])
				: (digit ? VIM_ISDIGIT(p[2])
				    : ASCII_ISUPPER(p[2])))
			    || to < from)
		    {
			semsg(_(e_invalid_argument_str), p);
			ត្រឡប់;
		    }
		    p += 2;
		}
		បើមិនដូច្នេះ
		    // clear one lower case mark
		    from = to = *p;

		សម្រាប់ (i = from; i <= to; ++i)
		{
		    ប្រសិនបើ (lower)
			curbuf->b_namedm[i - 'a'].lnum = 0;
		    បើមិនដូច្នេះ
		    {
			ប្រសិនបើ (digit)
			    n = i - '0' + NMARKS;
			បើមិនដូច្នេះ
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
	    បើមិនដូច្នេះ
		switch (*p)
		{
		    pak_sa '"': curbuf->b_last_cursor.lnum = 0; bha_nga;
		    pak_sa '^': curbuf->b_last_insert.lnum = 0; bha_nga;
		    pak_sa '.': curbuf->b_last_change.lnum = 0; bha_nga;
		    pak_sa '[': curbuf->b_op_start.lnum    = 0; bha_nga;
		    pak_sa ']': curbuf->b_op_end.lnum      = 0; bha_nga;
		    pak_sa '<': curbuf->b_visual.vi_start.lnum = 0; bha_nga;
		    pak_sa '>': curbuf->b_visual.vi_end.lnum   = 0; bha_nga;
		    pak_sa ' ': bha_nga;
		    default:  semsg(_(e_invalid_argument_str), p);
			      ត្រឡប់;
		}
	}
    }
}

/*
 * print the jumplist
 */
    void
ex_jumps(exarg_T *eap UNUSED)
{
    ចំនួនគត់		i;
    char_u	*name;

    cleanup_jumplist(curwin, TRUE);

    // Highlight title
    msg_puts_title(_("\n jump line  col file/text"));
    សម្រាប់ (i = 0; i < curwin->w_jumplistlen && !got_int; ++i)
    {
	ប្រសិនបើ (curwin->w_jumplist[i].fmark.mark.lnum != 0)
	{
	    name = fm_getname(&curwin->w_jumplist[i].fmark, 16);

	    // Make sure to output the current indicator, even when on an wiped
	    // out buffer.  ":filter" may still skip it.
	    ប្រសិនបើ (name == NULL && i == curwin->w_jumplistidx)
		name = vim_strsave((char_u *)"-invalid-");
	    // apply :filter /pat/ or file name not available
	    ប្រសិនបើ (name == NULL || message_filtered(name))
	    {
		vim_free(name);
		continue;
	    }

	    msg_putchar('\n');
	    ប្រសិនបើ (got_int)
	    {
		vim_free(name);
		bha_nga;
	    }
	    sprintf((var_na *)IObuff, "%c %2d %5ld %4d ",
		i == curwin->w_jumplistidx ? '>' : ' ',
		i > curwin->w_jumplistidx ? i - curwin->w_jumplistidx
					  : curwin->w_jumplistidx - i,
		curwin->w_jumplist[i].fmark.mark.lnum,
		curwin->w_jumplist[i].fmark.mark.col);
	    msg_outtrans(IObuff);
	    msg_outtrans_attr(name,
			    curwin->w_jumplist[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ប្រសិនបើ (curwin->w_jumplistidx == curwin->w_jumplistlen)
	msg_puts("\n>");
}

    void
ex_clearjumps(exarg_T *eap UNUSED)
{
    free_jumplist(curwin);
    curwin->w_jumplistlen = 0;
    curwin->w_jumplistidx = 0;
}

/*
 * print the changelist
 */
    void
ex_changes(exarg_T *eap UNUSED)
{
    ចំនួនគត់		i;
    char_u	*name;

    // Highlight title
    msg_puts_title(_("\nchange line  col text"));

    សម្រាប់ (i = 0; i < curbuf->b_changelistlen && !got_int; ++i)
    {
	ប្រសិនបើ (curbuf->b_changelist[i].lnum != 0)
	{
	    msg_putchar('\n');
	    ប្រសិនបើ (got_int)
		bha_nga;
	    sprintf((var_na *)IObuff, "%c %3d %5ld %4d ",
		    i == curwin->w_changelistidx ? '>' : ' ',
		    i > curwin->w_changelistidx ? i - curwin->w_changelistidx
						: curwin->w_changelistidx - i,
		    (long)curbuf->b_changelist[i].lnum,
		    curbuf->b_changelist[i].col);
	    msg_outtrans(IObuff);
	    name = mark_line(&curbuf->b_changelist[i], 17);
	    ប្រសិនបើ (name == NULL)
		bha_nga;
	    msg_outtrans_attr(name, HL_ATTR(HLF_D));
	    vim_free(name);
	    ui_breakcheck();
	}
	out_flush();
    }
    ប្រសិនបើ (curwin->w_changelistidx == curbuf->b_changelistlen)
	msg_puts("\n>");
}

#define one_adjust(add) \
    { \
	lp = add; \
	ប្រសិនបើ (*lp >= line1 && *lp <= line2) \
	{ \
	    ប្រសិនបើ (amount == MAXLNUM) \
		*lp = 0; \
	    បើមិនដូច្នេះ \
		*lp += amount; \
	} \
	បើមិនដូច្នេះ ប្រសិនបើ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// don't delete the line, just put at first deleted line
#define one_adjust_nodel(add) \
    { \
	lp = add; \
	ប្រសិនបើ (*lp >= line1 && *lp <= line2) \
	{ \
	    ប្រសិនបើ (amount == MAXLNUM) \
		*lp = line1; \
	    បើមិនដូច្នេះ \
		*lp += amount; \
	} \
	បើមិនដូច្នេះ ប្រសិនបើ (amount_after && *lp > line2) \
	    *lp += amount_after; \
    }

// Like one_adjust_nodel(), but if the position is within the deleted range,
// move it to the start of the line before the range.
#define one_adjust_cursor(pp) \
    { \
	pos_T *posp = pp; \
	ប្រសិនបើ (posp->lnum >= line1 && posp->lnum <= line2) \
	{ \
	    ប្រសិនបើ (amount == MAXLNUM) /* line with cursor is deleted */ \
	    { \
		posp->lnum = MAX(line1 - 1, 1); \
		posp->col = 0; \
	    } \
	    បើមិនដូច្នេះ /* keep cursor on the same line */ \
		posp->lnum += amount; \
	} \
	បើមិនដូច្នេះ ប្រសិនបើ (amount_after && posp->lnum > line2) \
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
    void
mark_adjust(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, TRUE);
}

    void
mark_adjust_nofold(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after)
{
    mark_adjust_internal(line1, line2, amount, amount_after, FALSE);
}

    static void
mark_adjust_internal(
    linenr_T	line1,
    linenr_T	line2,
    long	amount,
    long	amount_after,
    ចំនួនគត់		adjust_folds UNUSED)
{
    ចំនួនគត់		i;
    ចំនួនគត់		fnum = curbuf->b_fnum;
    linenr_T	*lp;
    win_T	*win;
    tabpage_T	*tab;
    wininfo_T	*wip;
    static pos_T initpos = {1, 0, 0};

    ប្រសិនបើ (line2 < line1 && amount_after == 0L)	    // nothing to do
	ត្រឡប់;

    ប្រសិនបើ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// named marks, lower case and upper case
	សម្រាប់ (i = 0; i < NMARKS; i++)
	{
	    one_adjust(&(curbuf->b_namedm[i].lnum));
	    ប្រសិនបើ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}
	សម្រាប់ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
	{
	    ប្រសិនបើ (namedfm[i].fmark.fnum == fnum)
		one_adjust_nodel(&(namedfm[i].fmark.mark.lnum));
	}

	// last Insert position
	one_adjust(&(curbuf->b_last_insert.lnum));

	// last change position
	one_adjust(&(curbuf->b_last_change.lnum));

	// last cursor position, if it was set
	ប្រសិនបើ (!EQUAL_POS(curbuf->b_last_cursor, initpos))
	    one_adjust(&(curbuf->b_last_cursor.lnum));


	// list of change positions
	សម្រាប់ (i = 0; i < curbuf->b_changelistlen; ++i)
	    one_adjust_nodel(&(curbuf->b_changelist[i].lnum));

	// Visual area
	one_adjust_nodel(&(curbuf->b_visual.vi_start.lnum));
	one_adjust_nodel(&(curbuf->b_visual.vi_end.lnum));

#ifdef FEAT_QUICKFIX
	// quickfix marks
	qf_mark_adjust(NULL, line1, line2, amount, amount_after);
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
    ប្រសិនបើ (saved_cursor.lnum != 0)
	one_adjust_nodel(&(saved_cursor.lnum));

    /*
     * Adjust items in all windows related to the current buffer.
     */
    FOR_ALL_TAB_WINDOWS(tab, win)
    {
	ប្រសិនបើ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    // Marks in the jumplist.  When deleting lines, this may create
	    // duplicate marks in the jumplist, they will be removed later.
	    សម្រាប់ (i = 0; i < win->w_jumplistlen; ++i)
		ប្រសិនបើ (win->w_jumplist[i].fmark.fnum == fnum)
		    one_adjust_nodel(&(win->w_jumplist[i].fmark.mark.lnum));

	ប្រសិនបើ (win->w_buffer == curbuf)
	{
	    ប្រសិនបើ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
		// marks in the tag stack
		សម្រាប់ (i = 0; i < win->w_tagstacklen; i++)
		    ប្រសិនបើ (win->w_tagstack[i].fmark.fnum == fnum)
			one_adjust_nodel(&(win->w_tagstack[i].fmark.mark.lnum));

	    // the displayed Visual area
	    ប្រសិនបើ (win->w_old_cursor_lnum != 0)
	    {
		one_adjust_nodel(&(win->w_old_cursor_lnum));
		one_adjust_nodel(&(win->w_old_visual_lnum));
	    }

	    // topline and cursor position for windows with the same buffer
	    // other than the current window
	    ប្រសិនបើ (win != curwin)
	    {
		ប្រសិនបើ (win->w_topline >= line1 && win->w_topline <= line2)
		{
		    ប្រសិនបើ (amount == MAXLNUM)	    // topline is deleted
		    {
			ប្រសិនបើ (line1 <= 1)
			    win->w_topline = 1;
			បើមិនដូច្នេះ
			    win->w_topline = line1 - 1;
		    }
		    បើមិនដូច្នេះ ប្រសិនបើ (win->w_topline > line1)
			// keep topline on the same line, unless inserting just
			// above it (we probably want to see that line then)
			win->w_topline += amount;
#ifdef FEAT_DIFF
		    win->w_topfill = 0;
#endif
		}
		បើមិនដូច្នេះ ប្រសិនបើ (amount_after && win->w_topline > line2)
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
	    ប្រសិនបើ (adjust_folds)
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
	ប្រសិនបើ (posp->lnum == lnum && posp->col >= mincol) \
	{ \
	    posp->lnum += lnum_amount; \
	    ប្រសិនបើ (col_amount < 0 && posp->col <= (colnr_T)-col_amount) \
		posp->col = 0; \
	    បើមិនដូច្នេះ ប្រសិនបើ (posp->col < spaces_removed) \
		posp->col = col_amount + spaces_removed; \
	    បើមិនដូច្នេះ \
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
    void
mark_col_adjust(
    linenr_T	lnum,
    colnr_T	mincol,
    long	lnum_amount,
    long	col_amount,
    ចំនួនគត់		spaces_removed)
{
    ចំនួនគត់		i;
    ចំនួនគត់		fnum = curbuf->b_fnum;
    win_T	*win;
    pos_T	*posp;

    ប្រសិនបើ ((col_amount == 0L && lnum_amount == 0L)
				       || (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	ត្រឡប់; // nothing to do

    // named marks, lower case and upper case
    សម្រាប់ (i = 0; i < NMARKS; i++)
    {
	col_adjust(&(curbuf->b_namedm[i]));
	ប្រសិនបើ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }
    សម្រាប់ (i = NMARKS; i < NMARKS + EXTRA_MARKS; i++)
    {
	ប្រសិនបើ (namedfm[i].fmark.fnum == fnum)
	    col_adjust(&(namedfm[i].fmark.mark));
    }

    // last Insert position
    col_adjust(&(curbuf->b_last_insert));

    // last change position
    col_adjust(&(curbuf->b_last_change));

    // list of change positions
    សម្រាប់ (i = 0; i < curbuf->b_changelistlen; ++i)
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
	សម្រាប់ (i = 0; i < win->w_jumplistlen; ++i)
	    ប្រសិនបើ (win->w_jumplist[i].fmark.fnum == fnum)
		col_adjust(&(win->w_jumplist[i].fmark.mark));

	ប្រសិនបើ (win->w_buffer == curbuf)
	{
	    // marks in the tag stack
	    សម្រាប់ (i = 0; i < win->w_tagstacklen; i++)
		ប្រសិនបើ (win->w_tagstack[i].fmark.fnum == fnum)
		    col_adjust(&(win->w_tagstack[i].fmark.mark));

	    // cursor position for other windows with the same buffer
	    ប្រសិនបើ (win != curwin)
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
    void
cleanup_jumplist(win_T *wp, ចំនួនគត់ loadfiles)
{
    ចំនួនគត់	    i;
    ចំនួនគត់	    from, to;
    ចំនួនគត់	    mustfree;

    ប្រសិនបើ (loadfiles)
    {
	// If specified, load all the files from the jump list. This is
	// needed to properly clean up duplicate entries, but will take some
	// time.
	សម្រាប់ (i = 0; i < wp->w_jumplistlen; ++i)
	{
	    ប្រសិនបើ ((wp->w_jumplist[i].fmark.fnum == 0) &&
		    (wp->w_jumplist[i].fmark.mark.lnum != 0))
		fname2fnum(&wp->w_jumplist[i]);
	}
    }

    to = 0;
    សម្រាប់ (from = 0; from < wp->w_jumplistlen; ++from)
    {
	ប្រសិនបើ (wp->w_jumplistidx == from)
	    wp->w_jumplistidx = to;
	សម្រាប់ (i = from + 1; i < wp->w_jumplistlen; ++i)
	    ប្រសិនបើ (wp->w_jumplist[i].fmark.fnum
					== wp->w_jumplist[from].fmark.fnum
		    && wp->w_jumplist[from].fmark.fnum != 0
		    && wp->w_jumplist[i].fmark.mark.lnum
				  == wp->w_jumplist[from].fmark.mark.lnum)
		bha_nga;
	ប្រសិនបើ (i >= wp->w_jumplistlen)	// not duplicate
	    mustfree = FALSE;
	បើមិនដូច្នេះ ប្រសិនបើ (i > from + 1)		// non-adjacent duplicate
	    // jumpoptions=stack: remove duplicates only when adjacent.
	    mustfree = !(jop_flags & JOP_STACK);
	បើមិនដូច្នេះ				// adjacent duplicate
	    mustfree = TRUE;

	ប្រសិនបើ (mustfree)
	    vim_free(wp->w_jumplist[from].fname);
	បើមិនដូច្នេះ
	    wp->w_jumplist[to++] = wp->w_jumplist[from];
    }
    ប្រសិនបើ (wp->w_jumplistidx == wp->w_jumplistlen)
	wp->w_jumplistidx = to;
    wp->w_jumplistlen = to;
}

/*
 * Copy the jumplist from window "from" to window "to".
 */
    void
copy_jumplist(win_T *from, win_T *to)
{
    ចំនួនគត់		i;

    សម្រាប់ (i = 0; i < from->w_jumplistlen; ++i)
    {
	to->w_jumplist[i] = from->w_jumplist[i];
	ប្រសិនបើ (from->w_jumplist[i].fname != NULL)
	    to->w_jumplist[i].fname = vim_strsave(from->w_jumplist[i].fname);
    }
    to->w_jumplistlen = from->w_jumplistlen;
    to->w_jumplistidx = from->w_jumplistidx;
}

/*
 * Free items in the jumplist of window "wp".
 */
    void
free_jumplist(win_T *wp)
{
    ចំនួនគត់		i;

    សម្រាប់ (i = 0; i < wp->w_jumplistlen; ++i)
	vim_free(wp->w_jumplist[i].fname);
}

    void
set_last_cursor(win_T *win)
{
    ប្រសិនបើ (win->w_buffer != NULL)
	win->w_buffer->b_last_cursor = win->w_cursor;
}

#ប្រសិនបើ defined(EXITFREE)
    void
free_all_marks(void)
{
    ចំនួនគត់		i;

    សម្រាប់ (i = 0; i < NMARKS + EXTRA_MARKS; i++)
	ប្រសិនបើ (namedfm[i].fmark.mark.lnum != 0)
	    vim_free(namedfm[i].fname);
}
#endif

#ប្រសិនបើ defined(FEAT_VIMINFO)
/*
 * Return a pointer to the named file marks.
 */
    xfmark_T *
get_namedfm(void)
{
    ត្រឡប់ namedfm;
}
#endif

#ប្រសិនបើ defined(FEAT_EVAL)
/*
 * Add information about mark 'mname' to list 'l'
 */
    static ចំនួនគត់
add_mark(list_T *l, char_u *mname, size_t mnamelen, pos_T *pos, ចំនួនគត់ bufnr, char_u *fname)
{
    dict_T	*d;
    list_T	*lpos;

    ប្រសិនបើ (pos->lnum <= 0)
	ត្រឡប់ OK;

    d = dict_alloc();
    ប្រសិនបើ (d == NULL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (list_append_dict(l, d) == FAIL)
    {
	dict_unref(d);
	ត្រឡប់ FAIL;
    }

    lpos = list_alloc();
    ប្រសិនបើ (lpos == NULL)
	ត្រឡប់ FAIL;

    list_append_number(lpos, bufnr);
    list_append_number(lpos, pos->lnum);
    list_append_number(lpos, pos->col < MAXCOL ? pos->col + 1 : MAXCOL);
    list_append_number(lpos, pos->coladd);

    ប្រសិនបើ (dict_add_string_len(d, "mark", mname, (ចំនួនគត់)mnamelen) == FAIL
	    || dict_add_list(d, "pos", lpos) == FAIL
	    || (fname != NULL && dict_add_string(d, "file", fname) == FAIL))
    {
	ប្រសិនបើ (lpos->lv_refcount == 0)
	    list_free(lpos);
	ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

/*
 * Get information about marks local to a buffer.
 */
    static void
get_buf_local_marks(buf_T *buf, list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ចំនួនគត់		i;

    // Marks 'a' to 'z'
    សម្រាប់ (i = 0; i < NMARKS; ++i)
    {
	mname[1] = 'a' + i;
	add_mark(l, mname, mnamelen, &buf->b_namedm[i], buf->b_fnum, NULL);
    }

    // Mark '' is a window local mark and not a buffer local mark
    add_mark(l, (char_u *)"''", STRLEN_LITERAL("''"), &curwin->w_pcmark, curbuf->b_fnum, NULL);

    add_mark(l, (char_u *)"'\"", STRLEN_LITERAL("'\""), &buf->b_last_cursor, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'[", STRLEN_LITERAL("'["), &buf->b_op_start, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"']", STRLEN_LITERAL("']"), &buf->b_op_end, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'^", STRLEN_LITERAL("'^"), &buf->b_last_insert, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'.", STRLEN_LITERAL("'."), &buf->b_last_change, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'<", STRLEN_LITERAL("'<"), &buf->b_visual.vi_start, buf->b_fnum, NULL);
    add_mark(l, (char_u *)"'>", STRLEN_LITERAL("'>"), &buf->b_visual.vi_end, buf->b_fnum, NULL);
}

/*
 * Get information about global marks ('A' to 'Z' and '0' to '9')
 */
    static void
get_global_marks(list_T *l)
{
    char_u	mname[3] = "' ";
    size_t	mnamelen = STRLEN_LITERAL("' ");
    ចំនួនគត់		i;
    char_u	*name;

    // Marks 'A' to 'Z' and '0' to '9'
    សម្រាប់ (i = 0; i < NMARKS + EXTRA_MARKS; ++i)
    {
	ប្រសិនបើ (namedfm[i].fmark.fnum != 0)
	    name = buflist_nr2name(namedfm[i].fmark.fnum, TRUE, TRUE);
	បើមិនដូច្នេះ
	    name = namedfm[i].fname;
	ប្រសិនបើ (name != NULL)
	{
	    mname[1] = i >= NMARKS ? i - NMARKS + '0' : i + 'A';
	    add_mark(l, mname, mnamelen, &namedfm[i].fmark.mark,
		    namedfm[i].fmark.fnum, name);
	    ប្រសិនបើ (namedfm[i].fmark.fnum != 0)
		vim_free(name);
	}
    }
}

/*
 * getmarklist() function
 */
    void
f_getmarklist(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = NULL;

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_global_marks(rettv->vval.v_list);
	ត្រឡប់;
    }

    buf = tv_get_buf(&argvars[0], FALSE);
    ប្រសិនបើ (buf == NULL)
	ត្រឡប់;

    get_buf_local_marks(buf, rettv->vval.v_list);
}
#endif
