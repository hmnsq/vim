/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Code to handle tags and the tag stack
 */

#شامل "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
قسمتعریف ساخت لیبل_اشارے
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_انتہا;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_انتہا;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_انتہا;	// first char after command
    char_u	*لیبل_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    صحیح		is_etag;	// TRUE for emacs tag
#اگر_ختم
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_انتہا;	// end of tagkind
    char_u	*user_ڈیٹا;	// user_data string
    char_u	*user_ڈیٹا_انتہا;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
قسمتعریف شمار
{
    لیبل_فہرست_READ_SUCCESS = 1,
    لیبل_فہرست_READ_EOF,
    لیبل_فہرست_READ_IGNORE,
} لیبل_فہرست_read_حالت_T;

/*
 * States used during a tags search
 */
قسمتعریف شمار
{
    TS_آغاز,		// at start of file
    TS_خطی,		// linear searching forward, till EOF
    TS_ثنائی,		// binary searching
    TS_SKIP_عقب,	// skipping backwards
    TS_قدم_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
قسمتعریف ساخت
{
    off_T	کم_انحراف;	// offset for first char of first line that
				// could match
    off_T	بلند_انحراف;	// offset of char after last line that could
				//مطابقت
    off_T	curr_انحراف;	// Current file offset in search range
    off_T	curr_انحراف_مستعمل; // curr_offset used when skipping back
    off_T	مطابقت_انحراف;	// Where the binary search found a tag
    صحیح	کم_char;		// first char at low_offset
    صحیح	بلند_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
قسمتعریف شمار
{
    لیبل_مطابقت_SUCCESS = 1,
    لیبل_مطابقت_FAIL,
    لیبل_مطابقت_STOP,
    لیبل_مطابقت_اگلا
} tagmatch_حالت_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
قسمتعریف ساخت
{
    صحیح	matchoff;		// tag match offset
    صحیح	مطابقت_re;		// TRUE if the tag matches a regexp
    صحیح	مطابقت_no_ic;		// TRUE if the tag matches with case
    صحیح	has_re;			// regular expression used
    صحیح	sortic;			// tags file sorted ignoring case (foldcase)
    صحیح	ترتیب_دو_error;		// tags file not sorted
} findtags_مطابقت_دلائل_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#تعریف_کرو MT_ST_CUR	0		// static match in current file
#تعریف_کرو MT_GL_CUR	1		// global match in current file
#تعریف_کرو MT_GL_OTH	2		// global match in other file
#تعریف_کرو MT_ST_OTH	3		// static match in other file
#تعریف_کرو MT_IC_OFF	4		// add for icase match
#تعریف_کرو MT_RE_OFF	8		// add for regexp match
#تعریف_کرو MT_نقاب		7		// mask for printing priority
#تعریف_کرو MT_گنتی	16

ساکن حرف	*mt_نام[MT_گنتی/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#تعریف_کرو NOTAGFILE	99		// return value for jumpto_tag
ساکن char_u	*nofile_fname = تہی;	// fname for NOTAGFILE error

ساکن خالی taglen_advance(صحیح l);

ساکن صحیح jumpto_لیبل(char_u *lbuf, صحیح forceit, صحیح keep_help);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
ساکن صحیح parse_لیبل_سطر(char_u *lbuf, صحیح is_etag, tagptrs_T *tagp);
#ورنہ
ساکن صحیح parse_لیبل_سطر(char_u *lbuf, tagptrs_T *tagp);
#اگر_ختم
ساکن صحیح test_for_static(tagptrs_T *);
ساکن صحیح parse_مطابقت(char_u *lbuf, tagptrs_T *tagp);
ساکن char_u *لیبل_بھرا_fname(tagptrs_T *tagp);
ساکن char_u *expand_لیبل_fname(char_u *fname, char_u *لیبل_fname, صحیح expand);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
ساکن صحیح test_for_current(صحیح, char_u *, char_u *, char_u *, char_u *);
#ورنہ
ساکن صحیح test_for_current(char_u *, char_u *, char_u *, char_u *);
#اگر_ختم
ساکن صحیح ڈھونڈو_extra(char_u **pp);
ساکن خالی print_لیبل_list(صحیح new_لیبل, صحیح use_tagstack, صحیح عدد_matches, char_u **matches);
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ساکن صحیح جوڑو_llist_لیبل_فہرست(char_u *لیبل, صحیح عدد_matches, char_u **matches);
#اگر_ختم

ساکن char_u	*tagmatchname = تہی;	// name of last used tag

#اگر defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ساکن taggy_T ptag_اندراج = {تہی, {{0, 0, 0}, 0}, 0, 0, تہی};
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح  tfu_اندر_use = FALSE;	    // disallow recursive call of tagfunc
ساکن callback_T tfu_cb;	    // 'tagfunc' callback function
#اگر_ختم

// Used instead of NUL to separate tag fields in the growarrays.
#تعریف_کرو لیبل_SEP 0x02

#اگر defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    حرف *
did_سیٹ_tagfunc(optset_T *دلائل)
{
    صحیح	retval;

    اگر (دلائل->os_flags & OPT_LOCAL)
	retval = option_سیٹ_callback_func(دلائل->os_newval.string,
							    &curbuf->b_tfu_cb);
    ورنہ
    {
	retval = option_سیٹ_callback_func(دلائل->os_newval.string, &tfu_cb);
	اگر (retval == OK && !(دلائل->os_flags & OPT_GLOBAL))
	    سیٹ_buflocal_tfu_callback(curbuf);
    }

    واپس retval == FAIL ? e_invalid_دلیل : تہی;
}
#اگر_ختم

#اگر defined(EXITFREE)
    خالی
free_tagfunc_option(خالی)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    صحیح
سیٹ_ref_اندر_tagfunc(صحیح copyID غیر_مستعمل)
{
    صحیح	abort = FALSE;

    abort = سیٹ_ref_اندر_callback(&tfu_cb, copyID);

    واپس abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    خالی
سیٹ_buflocal_tfu_callback(buf_T *buf غیر_مستعمل)
{
    free_callback(&buf->b_tfu_cb);
    اگر (tfu_cb.cb_name != تہی && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#اگر_ختم

/*
 * Jump to tag; handling of tag commands and tag stack
 *
 * *tag != NUL: ":tag {tag}", jump to new tag, add to tag stack
 *
 * type == DT_TAG:	":tag [tag]", jump to newer position or same tag again
 * type == DT_HELP:	like DT_TAG, but don't use regexp.
 * type == DT_POP:	":pop" or CTRL-T, jump to old position
 * type == DT_NEXT:	jump to next match of same tag
 * type == DT_PREV:	jump to previous match of same tag
 * type == DT_FIRST:	jump to first match of same tag
 * type == DT_LAST:	jump to last match of same tag
 * type == DT_SELECT:	":tselect [tag]", select tag from a list of all matches
 * type == DT_JUMP:	":tjump [tag]", jump to tag or select tag from a list
 * type == DT_CSCOPE:	use cscope to find the tag
 * type == DT_LTAG:	use location list for displaying tag matches
 * type == DT_FREE:	free cached matches
 *
 * for cscope, returns TRUE if we jumped to tag or aborted, FALSE otherwise
 */
    صحیح
do_لیبل(
    char_u	*لیبل,		// tag (pattern) to jump to
    صحیح		type,
    صحیح		گنتی,
    صحیح		forceit,	// :ta with !
    صحیح		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		tagstacklen = curwin->w_tagstacklen;
    صحیح		cur_مطابقت = 0;
    صحیح		cur_fnum = curbuf->b_fnum;
    صحیح		oldtagstackidx = tagstackidx;
    صحیح		prevtagstackidx = tagstackidx;
    صحیح		پچھلا_عدد_matches;
    صحیح		new_لیبل = FALSE;
    صحیح		i;
    صحیح		ic;
    صحیح		no_regexp = FALSE;
    صحیح		error_cur_مطابقت = 0;
    صحیح		save_pos = FALSE;
    fmark_T	saved_fmark;
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح		jumped_مقصد_لیبل = FALSE;
#اگر_ختم
    صحیح		new_عدد_matches;
    char_u	**new_matches;
    صحیح		use_tagstack;
    صحیح		skip_پیغام = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    صحیح		use_tfu = 1;
    char_u	*tofree = تہی;

    // remember the matches for the last used tag
    ساکن صحیح		عدد_matches = 0;
    ساکن صحیح		max_عدد_matches = 0;  // limit used for match search
    ساکن char_u	**matches = تہی;
    ساکن صحیح		flags;

#اگر_تعریف_شدہ EXITFREE
    اگر (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(عدد_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_لیبل_فہرست();
# endif
	عدد_matches = 0;
	واپس FALSE;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (tfu_اندر_use)
    {
	emsg(_(e_cannot_modify_لیبل_ڈھیر_within_tagfunc));
	واپس FALSE;
    }
#اگر_ختم

    اگر (postponed_بانٹو == 0 && !check_can_سیٹ_curbuf_forceit(forceit))
	واپس FALSE;

    اگر (type == DT_HELP)
    {
	type = DT_لیبل;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    پچھلا_عدد_matches = عدد_matches;
    free_string_option(nofile_fname);
    nofile_fname = تہی;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    اگر ((!p_tgst && *لیبل != NUL))
    {
	use_tagstack = FALSE;
	new_لیبل = TRUE;
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	{
	    tagstack_clear_اندراج(&ptag_اندراج);
	    اگر ((ptag_اندراج.tagname = vim_strsave(لیبل)) == تہی)
		جاؤ انتہا_do_لیبل;
	}
#اگر_ختم
    }
    ورنہ
    {
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ورنہ
#اگر_ختم
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	اگر (*لیبل != NUL
		&& (type == DT_لیبل || type == DT_SELECT || type == DT_JUMP
#اگر_تعریف_شدہ FEAT_QUICKFIX
		    || type == DT_LTAG
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CSCOPE
		    || type == DT_CSCOPE
#اگر_ختم
		    ))
	{
#اگر defined(FEAT_QUICKFIX)
	    اگر (g_do_tagpreview != 0)
	    {
		اگر (ptag_اندراج.tagname != تہی
			&& STRCMP(ptag_اندراج.tagname, لیبل) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_مطابقت = ptag_اندراج.cur_مطابقت;
		    cur_fnum = ptag_اندراج.cur_fnum;
		}
		ورنہ
		{
		    tagstack_clear_اندراج(&ptag_اندراج);
		    اگر ((ptag_اندراج.tagname = vim_strsave(لیبل)) == تہی)
			جاؤ انتہا_do_لیبل;
		}
	    }
	    ورنہ
#اگر_ختم
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		جب (tagstackidx < tagstacklen)
		    tagstack_clear_اندراج(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		اگر (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_اندراج(&tagstack[0]);
		    برائے (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_ڈیٹا = تہی;
		}

		/*
		 * put the tag name in the tag stack
		 */
		اگر ((tagstack[tagstackidx].tagname = vim_strsave(لیبل)) == تہی)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    جاؤ انتہا_do_لیبل;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_لیبل = TRUE;
	}
	ورنہ
	{
	    اگر (
#اگر defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_اندراج.tagname == تہی :
#اگر_ختم
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_لیبل_ڈھیر_خالی_خانے));
		جاؤ انتہا_do_لیبل;
	    }

	    اگر (type == DT_نکالو)		// go to older position
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		صحیح	old_کلید_Typed = کلید_Typed;
#اگر_ختم
		اگر ((tagstackidx -= گنتی) < 0)
		{
		    emsg(_(e_at_bottom_کا_لیبل_ڈھیر));
		    اگر (tagstackidx + گنتی == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			جاؤ انتہا_do_لیبل;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ورنہ اگر (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_چوٹی_کا_لیبل_ڈھیر));
		    جاؤ انتہا_do_لیبل;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		اگر (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    اگر (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			جاؤ انتہا_do_لیبل;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ورنہ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.ستون = saved_fmark.mark.ستون;
		curwin->w_سیٹ_curswant = true;
		check_cursor();
#اگر_تعریف_شدہ FEAT_FOLDING
		اگر ((fdo_flags & FDO_لیبل) && old_کلید_Typed)
		    foldOpenCursor();
#اگر_ختم

		// remove the old list of matches
		FreeWild(عدد_matches, matches);
#اگر_تعریف_شدہ FEAT_CSCOPE
		cs_free_لیبل_فہرست();
#اگر_ختم
		عدد_matches = 0;
		لیبل_freematch();
		جاؤ انتہا_do_لیبل;
	    }

	    اگر (type == DT_لیبل
#اگر defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#اگر_ختم
	       )
	    {
#اگر defined(FEAT_QUICKFIX)
		اگر (g_do_tagpreview != 0)
		{
		    cur_مطابقت = ptag_اندراج.cur_مطابقت;
		    cur_fnum = ptag_اندراج.cur_fnum;
		}
		ورنہ
#اگر_ختم
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    اگر ((tagstackidx += گنتی - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_چوٹی_کا_لیبل_ڈھیر));
			save_pos = FALSE;
		    }
		    ورنہ اگر (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_کا_لیبل_ڈھیر));
			tagstackidx = 0;
			جاؤ انتہا_do_لیبل;
		    }
		    cur_مطابقت = tagstack[tagstackidx].cur_مطابقت;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_لیبل = TRUE;
	    }
	    ورنہ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#اگر defined(FEAT_QUICKFIX)
		اگر (g_do_tagpreview != 0)
		{
		    cur_مطابقت = ptag_اندراج.cur_مطابقت;
		    cur_fnum = ptag_اندراج.cur_fnum;
		}
		ورنہ
#اگر_ختم
		{
		    اگر (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_مطابقت = tagstack[tagstackidx].cur_مطابقت;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		انتخاب (type)
		{
		    حالت DT_پہلا: cur_مطابقت = گنتی - 1; توڑو;
		    حالت DT_SELECT:
		    حالت DT_JUMP:
#اگر_تعریف_شدہ FEAT_CSCOPE
		    حالت DT_CSCOPE:
#اگر_ختم
		    حالت DT_آخری:  cur_مطابقت = MAXCOL - 1; توڑو;
		    حالت DT_اگلا:  cur_مطابقت += گنتی; توڑو;
		    حالت DT_پچھلا:  cur_مطابقت -= گنتی; توڑو;
		}
		اگر (cur_مطابقت >= MAXCOL)
		    cur_مطابقت = MAXCOL - 1;
		ورنہ اگر (cur_مطابقت < 0)
		{
		    emsg(_(e_cannot_go_before_پہلا_matching_لیبل));
		    skip_پیغام = TRUE;
		    cur_مطابقت = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	{
	    اگر (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_اندراج.cur_مطابقت = cur_مطابقت;
		ptag_اندراج.cur_fnum = cur_fnum;
	    }
	}
	ورنہ
#اگر_ختم
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    اگر (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    اگر (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_مطابقت = cur_مطابقت;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    اگر (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	اگر (buf != تہی)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    برائے (;;)
    {
	صحیح	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	اگر (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#اگر defined(FEAT_QUICKFIX)
	ورنہ اگر (g_do_tagpreview != 0)
	    name = ptag_اندراج.tagname;
#اگر_ختم
	ورنہ
	    name = لیبل;
	other_name = (tagmatchname == تہی || STRCMP(tagmatchname, name) != 0);
	اگر (new_لیبل
		|| (cur_مطابقت >= عدد_matches && max_عدد_matches != MAXCOL)
		|| other_name)
	{
	    اگر (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    اگر (type == DT_SELECT || type == DT_JUMP
#اگر defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#اگر_ختم
		)
		cur_مطابقت = MAXCOL - 1;
	    اگر (type == DT_لیبل)
		max_عدد_matches = MAXCOL;
	    ورنہ
		max_عدد_matches = cur_مطابقت + 1;

	    // when the argument starts with '/', use it as a regexp
	    اگر (!no_regexp && *name == '/')
	    {
		flags = لیبل_REGEXP;
		++name;
	    }
	    ورنہ
		flags = لیبل_NOIC;

#اگر_تعریف_شدہ FEAT_CSCOPE
	    اگر (type == DT_CSCOPE)
		flags = لیبل_CSCOPE;
#اگر_ختم
	    اگر (verbose)
		flags |= لیبل_VERBOSE;

	    اگر (!use_tfu)
		flags |= لیبل_NO_TAGFUNC;

	    اگر (ڈھونڈو_لیبل_فہرست(name, &new_عدد_matches, &new_matches, flags,
					    max_عدد_matches, buf_ffname) == OK
		    && new_عدد_matches < max_عدد_matches)
		max_عدد_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    اگر (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_کھڑکی_unexpectedly_close_while_searching_for_لیبل_فہرست));
		FreeWild(new_عدد_matches, new_matches);
		توڑو;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    اگر (!new_لیبل && !other_name)
	    {
		صحیح	    j, k;
		صحیح	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		برائے (j = 0; j < عدد_matches; ++j)
		{
		    parse_مطابقت(matches[j], &tagp);
		    برائے (i = idx; i < new_عدد_matches; ++i)
		    {
			parse_مطابقت(new_matches[i], &tagp2);
			اگر (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    برائے (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    توڑو;
			}
		    }
		}
	    }
	    FreeWild(عدد_matches, matches);
	    عدد_matches = new_عدد_matches;
	    matches = new_matches;
	}

	اگر (عدد_matches <= 0)
	{
	    اگر (verbose)
		semsg(_(e_لیبل_not_ملا_str), name);
#اگر defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#اگر_ختم
	}
	ورنہ
	{
	    صحیح ask_for_selection = FALSE;
#اگر defined(FEAT_EVAL)
	    size_t  IObufflen;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CSCOPE
	    اگر (type == DT_CSCOPE && عدد_matches > 1)
	    {
		cs_print_لیبل_فہرست();
		ask_for_selection = TRUE;
	    }
	    ورنہ
#اگر_ختم
	    اگر (type == DT_لیبل && *لیبل != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_مطابقت = گنتی > 0 ? گنتی - 1 : 0;
	    ورنہ اگر (type == DT_SELECT || (type == DT_JUMP && عدد_matches > 1))
	    {
		print_لیبل_list(new_لیبل, use_tagstack, عدد_matches, matches);
		ask_for_selection = TRUE;
	    }
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ورنہ اگر (type == DT_LTAG)
	    {
		اگر (جوڑو_llist_لیبل_فہرست(لیبل, عدد_matches, matches) == FAIL)
		    جاؤ انتہا_do_لیبل;
		cur_مطابقت = 0;		// Jump to the first tag
	    }
#اگر_ختم

	    اگر (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_نمبر(تہی);
		اگر (i <= 0 || i > عدد_matches || got_int)
		{
		    // no valid choice: don't change anything
		    اگر (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#اگر_تعریف_شدہ FEAT_CSCOPE
		    cs_free_لیبل_فہرست();
		    jumped_مقصد_لیبل = TRUE;
#اگر_ختم
		    توڑو;
		}
		cur_مطابقت = i - 1;
	    }

	    اگر (cur_مطابقت >= عدد_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		اگر ((type == DT_اگلا || type == DT_پہلا)
						      && nofile_fname == تہی)
		{
		    اگر (عدد_matches == 1)
			emsg(_(e_there_is_only_one_matching_لیبل));
		    ورنہ
			emsg(_(e_cannot_go_beyond_آخری_matching_لیبل));
		    skip_پیغام = TRUE;
		}
		cur_مطابقت = عدد_matches - 1;
	    }
	    اگر (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_مطابقت = cur_مطابقت;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		اگر (use_tfu && parse_مطابقت(matches[cur_مطابقت], &tagp) == OK
			&& tagp.user_ڈیٹا)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_ڈیٹا);
		    tagstack[tagstackidx].user_ڈیٹا = vim_strnsave(
			  tagp.user_ڈیٹا, tagp.user_ڈیٹا_انتہا - tagp.user_ڈیٹا);
		}

		++tagstackidx;
	    }
#اگر defined(FEAT_QUICKFIX)
	    ورنہ اگر (g_do_tagpreview != 0)
	    {
		ptag_اندراج.cur_مطابقت = cur_مطابقت;
		ptag_اندراج.cur_fnum = cur_fnum;
	    }
#اگر_ختم

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    اگر (nofile_fname != تہی && error_cur_مطابقت != cur_مطابقت)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_مطابقت][0] & MT_IC_OFF);
	    اگر (type != DT_لیبل && type != DT_SELECT && type != DT_JUMP
#اگر_تعریف_شدہ FEAT_CSCOPE
		&& type != DT_CSCOPE
#اگر_ختم
		&& (عدد_matches > 1 || ic)
		&& !skip_پیغام)
	    {
		// Give an indication of the number of matching tags
		sprintf((حرف *)IObuff, _("tag %d of %d%s"),
				cur_مطابقت + 1,
				عدد_matches,
				max_عدد_matches != MAXCOL ? _(" or more") : "");
		اگر (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		اگر ((عدد_matches > پچھلا_عدد_matches || new_لیبل)
							   && عدد_matches > 1)
		{
		    اگر (ic)
			پیغام_attr((حرف *)IObuff, HL_ATTR(HLF_W));
		    ورنہ
			پیغام((حرف *)IObuff);
		    پیغام_scroll = TRUE;	// don't overwrite this message
		}
		ورنہ
		    give_warning(IObuff, ic);
		اگر (ic && !پیغام_scrolled && پیغام_silent == 0)
		{
		    برآمد_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#اگر defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE, ":ta %s\r", name);
	    سیٹ_vim_var_string(VV_SWAPCOMMAND, IObuff, (صحیح)IObufflen);
#اگر_ختم

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_لیبل(matches[cur_مطابقت], forceit, type != DT_CSCOPE);

#اگر defined(FEAT_EVAL)
	    سیٹ_vim_var_string(VV_SWAPCOMMAND, تہی, -1);
#اگر_ختم

	    اگر (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		اگر ((type == DT_پچھلا && cur_مطابقت > 0)
			|| ((type == DT_لیبل || type == DT_اگلا
							  || type == DT_پہلا)
			    && (max_عدد_matches != MAXCOL
					     || cur_مطابقت < عدد_matches - 1)))
		{
		    error_cur_مطابقت = cur_مطابقت;
		    اگر (use_tagstack)
			--tagstackidx;
		    اگر (type == DT_پچھلا)
			--cur_مطابقت;
		    ورنہ
		    {
			type = DT_اگلا;
			++cur_مطابقت;
		    }
		    جاری;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ورنہ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		اگر (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#اگر_تعریف_شدہ FEAT_CSCOPE
		jumped_مقصد_لیبل = TRUE;
#اگر_ختم
	    }
	}
	توڑو;
    }

انتہا_do_لیبل:
    // Only store the new index when using the tagstack and it's valid.
    اگر (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_بانٹو = 0;	// don't split next time
#اگر_تعریف_شدہ FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#اگر_ختم

    vim_free(tofree);
#اگر_تعریف_شدہ FEAT_CSCOPE
    واپس jumped_مقصد_لیبل;
#ورنہ
    واپس FALSE;
#اگر_ختم
}

/*
 * List all the matching tags.
 */
    ساکن خالی
print_لیبل_list(
    صحیح		new_لیبل,
    صحیح		use_tagstack,
    صحیح		عدد_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		i;
    char_u	*p;
    char_u	*command_انتہا;
    tagptrs_T	tagp;
    صحیح		taglen;
    صحیح		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_مطابقت(matches[0], &tagp);
    taglen = (صحیح)(tagp.tagname_انتہا - tagp.tagname + 2);
    اگر (taglen < 18)
	taglen = 18;
    اگر (taglen > cmdline_چوڑائی - 25)
	taglen = MAXCOL;
    اگر (پیغام_ستون == 0)
	پیغام_didout = FALSE;	// overwrite previous message
    پیغام_آغاز();
    پیغام_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    پیغام_clr_eos();
    taglen_advance(taglen);
    پیغام_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    برائے (i = 0; i < عدد_matches && !got_int; ++i)
    {
	parse_مطابقت(matches[i], &tagp);
	اگر (!new_لیبل && (
#اگر defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_اندراج.cur_مطابقت) ||
#اگر_ختم
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_مطابقت)))
	    *IObuff = '>';
	ورنہ
	    *IObuff = ' ';
	vim_snprintf((حرف *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_نام[matches[i][0] & MT_نقاب]);
	پیغام_puts((حرف *)IObuff);
	اگر (tagp.tagkind != تہی)
	    پیغام_outtrans_len(tagp.tagkind,
			  (صحیح)(tagp.tagkind_انتہا - tagp.tagkind));
	پیغام_advance(13);
	پیغام_outtrans_len_attr(tagp.tagname,
			   (صحیح)(tagp.tagname_انتہا - tagp.tagname),
						  HL_ATTR(HLF_T));
	پیغام_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = لیبل_بھرا_fname(&tagp);
	اگر (p != تہی)
	{
	    پیغام_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	اگر (پیغام_ستون > 0)
	    پیغام_putchar('\n');
	اگر (got_int)
	    توڑو;
	پیغام_advance(15);

	// print any extra fields
	command_انتہا = tagp.command_انتہا;
	اگر (command_انتہا != تہی)
	{
	    p = command_انتہا + 3;
	    جب (*p && *p != '\r' && *p != '\n')
	    {
		جب (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		اگر (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    جاری;
		}
		// skip "kind:<kind>" and "<kind>"
		اگر (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_انتہا;
		    جاری;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		جب (*p && *p != '\r' && *p != '\n')
		{
		    اگر (پیغام_ستون + ptr2cells(p) >= cmdline_چوڑائی)
		    {
			پیغام_putchar('\n');
			اگر (got_int)
			    توڑو;
			پیغام_advance(15);
		    }
		    p = پیغام_outtrans_one(p, attr);
		    اگر (*p == TAB)
		    {
			پیغام_puts_attr(" ", attr);
			توڑو;
		    }
		    اگر (*p == ':')
			attr = 0;
		}
	    }
	    اگر (پیغام_ستون > 15)
	    {
		پیغام_putchar('\n');
		اگر (got_int)
		    توڑو;
		پیغام_advance(15);
	    }
	}
	ورنہ
	{
	    برائے (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_انتہا = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	اگر (*p == '/' || *p == '?')
	{
	    ++p;
	    اگر (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	جب (p != command_انتہا && vim_isspace(*p))
	    ++p;

	جب (p != command_انتہا)
	{
	    اگر (پیغام_ستون + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_چوڑائی)
		پیغام_putchar('\n');
	    اگر (got_int)
		توڑو;
	    پیغام_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    اگر (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    اگر (*p == TAB)
	    {
		پیغام_putchar(' ');
		++p;
	    }
	    ورنہ
		p = پیغام_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    اگر (p == command_انتہا - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		توڑو;
	    // don't display matching '/' or '?'
	    اگر (p == command_انتہا - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		توڑو;
	}
	اگر (پیغام_ستون)
	    پیغام_putchar('\n');
	ui_breakcheck();
    }
    اگر (got_int)
	got_int = FALSE;	// only stop the listing
}

#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ساکن صحیح
جوڑو_llist_لیبل_فہرست(
    char_u	*لیبل,
    صحیح		عدد_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	لیبل_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    صحیح		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    اگر (list == تہی || fname == تہی || cmd == تہی)
    {
	vim_free(cmd);
	vim_free(fname);
	اگر (list != تہی)
	    list_free(list);
	واپس FAIL;
    }

    برائے (i = 0; i < عدد_matches; ++i)
    {
	صحیح	    len, cmd_len;
	طویل    lnum;
	dict_T  *dict;

	parse_مطابقت(matches[i], &tagp);

	// Save the tag name
	len = (صحیح)(tagp.tagname_انتہا - tagp.tagname);
	اگر (len > 128)
	    len = 128;
	vim_strncpy(لیبل_name, tagp.tagname, len);
	لیبل_name[len] = NUL;

	// Save the tag file name
	p = لیبل_بھرا_fname(&tagp);
	اگر (p == تہی)
	    جاری;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	اگر (محفوظ_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((حرف *)tagp.command);
	ورنہ
	{
	    char_u *cmd_آغاز, *cmd_انتہا;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_آغاز = tagp.command;
	    cmd_انتہا = tagp.command_انتہا;
	    اگر (cmd_انتہا == تہی)
	    {
		برائے (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_انتہا = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_انتہا--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    اگر (*cmd_آغاز == '/' || *cmd_آغاز == '?')
		cmd_آغاز++;

	    اگر (*cmd_انتہا == '/' || *cmd_انتہا == '?')
		cmd_انتہا--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    اگر (*cmd_آغاز == '^')
	    {
		STRCPY(cmd, "^");
		cmd_آغاز++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (صحیح)(cmd_انتہا - cmd_آغاز + 1);
	    اگر (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_آغاز, cmd_len);
	    len += cmd_len;

	    اگر (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	اگر ((dict = dict_alloc()) == تہی)
	    جاری;
	اگر (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    جاری;
	}

	dict_جوڑو_string(dict, "text", لیبل_name);
	dict_جوڑو_string(dict, "filename", fname);
	dict_جوڑو_نمبر(dict, "lnum", lnum);
	اگر (lnum == 0)
	    dict_جوڑو_string(dict, "pattern", cmd);
    }

    vim_snprintf((حرف *)IObuff, IOSIZE, "ltag %s", لیبل);
    سیٹ_errorlist(curwin, list, ' ', IObuff, تہی);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    واپس OK;
}
#اگر_ختم

/*
 * Free cached tags.
 */
    خالی
لیبل_freematch(خالی)
{
    VIM_CLEAR(tagmatchname);
}

    ساکن خالی
taglen_advance(صحیح l)
{
    اگر (l == MAXCOL)
    {
	پیغام_putchar('\n');
	پیغام_advance(24);
    }
    ورنہ
	پیغام_advance(13 + l);
}

/*
 * Print the tag stack
 */
    خالی
do_لیبل_فہرست(exarg_T *eap غیر_مستعمل)
{
    صحیح		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    پیغام_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    برائے (i = 0; i < tagstacklen; ++i)
    {
	اگر (tagstack[i].tagname != تہی)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    اگر (name == تہی)	    // file name not available
		جاری;

	    پیغام_putchar('\n');
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_مطابقت + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    پیغام_outtrans(IObuff);
	    پیغام_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	برآمد_flush();		    // show one line at a time
    }
    اگر (tagstackidx == tagstacklen)	// idx at top of stack
	پیغام_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ساکن صحیح
لیبل_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    صحیح		i;

    جب (len > 0)
    {
	i = (صحیح)TOUPPER_ASC(*s1) - (صحیح)TOUPPER_ASC(*s2);
	اگر (i != 0)
	    واپس i;			// this character different
	اگر (*s1 == NUL)
	    توڑو;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    واپس 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
قسمتعریف ساخت
{
    char_u	*pat;		// the pattern
    صحیح		len;		// length of pat[]
    char_u	*سر;		// start of pattern head
    صحیح		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ساکن خالی
prepare_pats(pat_T *pats, صحیح has_re)
{
    pats->سر = pats->pat;
    pats->headlen = pats->len;
    اگر (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	اگر (pats->pat[0] == '^')
	    pats->سر = pats->pat + 1;
	ورنہ اگر (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->سر = pats->pat + 2;
	اگر (pats->سر == pats->pat)
	    pats->headlen = 0;
	ورنہ
	    برائے (pats->headlen = 0; pats->سر[pats->headlen] != NUL;
							      ++pats->headlen)
		اگر (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->سر[pats->headlen]) != تہی)
		    توڑو;
	اگر (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    اگر (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ورنہ
	pats->regmatch.regprog = تہی;
}

#اگر_تعریف_شدہ FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
قسمتعریف شمار {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_پتہ,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ساکن tagcmd_T
tagfunc_cmd_قسم(char_u *cmd)
{
    اگر (VIM_ISDIGIT(*cmd))
	واپس *skipdigits(cmd) == NUL ? TAGCMD_پتہ : TAGCMD_INVALID;
    اگر (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*انتہا = skip_regexp(cmd + 1, *cmd, FALSE);

	واپس (*انتہا == *cmd && انتہا[1] == NUL)
					? TAGCMD_پتہ : TAGCMD_INVALID;
    }
    اگر (vim_strpbrk(cmd, (char_u *)"\t\r\n") != تہی)
	واپس TAGCMD_INVALID;
    واپس TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ساکن صحیح
ڈھونڈو_tagfunc_لیبل_فہرست(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    صحیح		*مطابقت_گنتی,	// here the number of tags found will be placed
    صحیح		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *شے;
    صحیح		ntags = 0;
    صحیح		نتیجہ = FAIL;
    typval_T	دلائل[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*لیبل = &curwin->w_tagstack[curwin->w_tagstackidx];

    اگر (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == تہی
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	واپس FAIL;

    دلائل[0].v_type = VAR_STRING;
    دلائل[0].vval.v_string = pat;
    دلائل[1].v_type = VAR_STRING;
    دلائل[1].vval.v_string = flagString;

    // create 'info' dict argument
    اگر ((d = dict_alloc_تالا(VAR_FIXED)) == تہی)
	واپس FAIL;
    اگر (!(flags & لیبل_INS_COMP) && لیبل->user_ڈیٹا != تہی)
	dict_جوڑو_string(d, "user_data", لیبل->user_ڈیٹا);
    اگر (buf_ffname != تہی)
	dict_جوڑو_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    دلائل[2].v_type = VAR_DICT;
    دلائل[2].vval.v_dict = d;

    دلائل[3].v_type = VAR_UNKNOWN;

    vim_snprintf((حرف *)flagString, حجم(flagString),
		 "%s%s%s",
		 g_لیبل_at_cursor      ? "c": "",
		 flags & لیبل_INS_COMP ? "i": "",
		 flags & لیبل_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    نتیجہ = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, دلائل);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    اگر (نتیجہ == FAIL)
	واپس FAIL;
    اگر (rettv.v_type == VAR_SPECIAL && rettv.vval.v_نمبر == VVAL_NULL)
    {
	clear_tv(&rettv);
	واپس NOTDONE;
    }
    اگر (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_قدر_from_tagfunc));
	واپس FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_اشیاء(taglist, شے)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_قسم;
	صحیح		len;
	dict_iterator_T	iter;
	char_u		*dict_کلید;
	typval_T	*tv;
	صحیح		has_extra = 0;
	صحیح		name_only = flags & لیبل_نام;

	اگر (شے->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    توڑو;
	}

# ifdef FEAT_EMACS_لیبل_فہرست
	len = 3;
# ورنہ
	len = 2;
# endif
	res_name = تہی;
	res_fname = تہی;
	res_cmd = تہی;
	res_قسم = تہی;

	dict_iterate_آغاز(&شے->li_tv, &iter);
	جب (تہی != (dict_کلید = dict_iterate_اگلا(&iter, &tv)))
	{
	    اگر (tv->v_type != VAR_STRING || tv->vval.v_string == تہی)
		جاری;

	    len += (صحیح)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    اگر (STRCMP(dict_کلید, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		جاری;
	    }
	    اگر (STRCMP(dict_کلید, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		جاری;
	    }
	    اگر (STRCMP(dict_کلید, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		جاری;
	    }
	    has_extra = 1;
	    اگر (STRCMP(dict_کلید, "kind") == 0)
	    {
		res_قسم = tv->vval.v_string;
		جاری;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (صحیح)STRLEN(dict_کلید) + 1;
	}

	اگر (has_extra)
	    len += 2;	// need space for ;"

	اگر (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    توڑو;
	}

	tagcmd_T cmdkind = tagfunc_cmd_قسم(res_cmd);
	اگر (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    توڑو;
	}
	اگر (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	اگر (name_only)
	    mfp = vim_strsave(res_name);
	ورنہ
	    mfp = alloc(حجم(char_u) + len + 1);

	اگر (mfp == تہی)
	    جاری;

	اگر (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = لیبل_SEP;	    // no tag file name
# ifdef FEAT_EMACS_لیبل_فہرست
	    *p++ = لیبل_SEP;
# endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    اگر (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    اگر (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		اگر (res_قسم)
		{
		    *p++ = TAB;
		    STRCPY(p, res_قسم);
		    p += STRLEN(p);
		}

		dict_iterate_آغاز(&شے->li_tv, &iter);
		جب (تہی != (dict_کلید = dict_iterate_اگلا(&iter, &tv)))
		{
		    اگر (tv->v_type != VAR_STRING || tv->vval.v_string == تہی)
			جاری;

		    اگر (STRCMP(dict_کلید, "name") == 0)
			جاری;
		    اگر (STRCMP(dict_کلید, "filename") == 0)
			جاری;
		    اگر (STRCMP(dict_کلید, "cmd") == 0)
			جاری;
		    اگر (STRCMP(dict_کلید, "kind") == 0)
			جاری;

		    *p++ = TAB;
		    STRCPY(p, dict_کلید);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	اگر (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_ڈیٹا))[ga->ga_len++] = mfp;
	    ++ntags;
	    نتیجہ = OK;
	}
	ورنہ
	{
	    vim_free(mfp);
	    توڑو;
	}
    }

    clear_tv(&rettv);

    *مطابقت_گنتی = ntags;
    واپس نتیجہ;
}
#اگر_ختم

/*
 * State information used during a tag search
 */
قسمتعریف ساخت
{
    tagsearch_state_T	state;		// tag search state
    صحیح		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    صحیح		lbuf_سائز;		// length of lbuf
    char_u	*لیبل_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    صحیح		flags;			// flags used for tag search
    صحیح		لیبل_file_sorted;	// !_TAG_FILE_SORTED value
    صحیح		get_searchpat;		// used for 'showfulltag'
    صحیح		help_only;		// only search for help tags
    صحیح		did_open;		// did open a tag file
    صحیح		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    صحیح		خطی;			// do a linear search
    vimconv_T	vimconv;
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    صحیح		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    صحیح		help_pri;		// help language priority
    char_u	*help_lang_ڈھونڈو;	// lang to be found
    صحیح		is_txt;			// flag of file extension
#اگر_ختم
    صحیح		مطابقت_گنتی;		// number of matches found
    garray_T	ga_مطابقت[MT_گنتی];	// stores matches in sequence
    hashtab_T	ht_مطابقت[MT_گنتی];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساکن صحیح
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    صحیح			flags,
    صحیح			mincount)
{
    صحیح		mtt;

    st->لیبل_fname = alloc(MAXPATHL + 1);
    st->fp = تہی;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (صحیح)STRLEN(pat);
    st->orgpat->regmatch.regprog = تہی;
    st->flags = flags;
    st->لیبل_file_sorted = NUL;
    st->help_only = (flags & لیبل_HELP);
    st->get_searchpat = FALSE;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_ڈھونڈو = تہی;
    st->is_txt = FALSE;
#اگر_ختم
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_سائز = LSIZE;
    st->lbuf = alloc(st->lbuf_سائز);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    st->ebuf = alloc(LSIZE);
#اگر_ختم
    st->مطابقت_گنتی = 0;
    st->stop_searching = FALSE;

    برائے (mtt = 0; mtt < MT_گنتی; ++mtt)
    {
	ga_init2(&st->ga_مطابقت[mtt], حجم(char_u *), 100);
	hash_init(&st->ht_مطابقت[mtt]);
    }

    // check for out of memory situation
    اگر (st->لیبل_fname == تہی
	    || st->lbuf == تہی
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	    || st->ebuf == تہی
#اگر_ختم
       )
	واپس FAIL;

    واپس OK;
}

/*
 * Free the state used by find_tags()
 */
    ساکن خالی
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->لیبل_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    vim_free(st->ebuf);
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    ساکن صحیح
findtags_اندر_help_init(findtags_state_T *st)
{
    صحیح		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    اگر (st->is_txt)
	STRCPY(st->help_lang, "en");
    ورنہ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (صحیح)STRLEN(st->لیبل_fname);
	اگر (i > 3 && st->لیبل_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->لیبل_fname + i - 2, 2);
	ورنہ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    اگر (st->help_lang_ڈھونڈو != تہی
	    && STRICMP(st->help_lang, st->help_lang_ڈھونڈو) != 0)
	واپس FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    اگر ((st->flags & لیبل_KEEP_LANG)
	    && st->help_lang_ڈھونڈو == تہی
	    && curbuf->b_fname != تہی
	    && (i = (صحیح)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ورنہ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	برائے (s = p_hlg; *s != NUL; ++s)
	{
	    اگر (STRNICMP(s, st->help_lang, 2) == 0)
		توڑو;
	    ++st->help_pri;
	    اگر ((s = vim_strchr(s, ',')) == تہی)
		توڑو;
	}
	اگر (s == تہی || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    اگر (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    واپس TRUE;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    ساکن صحیح
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    صحیح		use_tfu = ((st->flags & لیبل_NO_TAGFUNC) == 0);
    صحیح		retval;

    اگر (!use_tfu || tfu_اندر_use || *curbuf->b_p_tfu == NUL)
	واپس NOTDONE;

    tfu_اندر_use = TRUE;
    retval = ڈھونڈو_tagfunc_لیبل_فہرست(pat, st->ga_مطابقت, &st->مطابقت_گنتی,
						st->flags, buf_ffname);
    tfu_اندر_use = FALSE;

    واپس retval;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_سائز 42
ساکن ساخت
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_سائز];
ساکن صحیح incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ساکن خالی
emacs_لیبل_فہرست_incstack_free(خالی)
{
    جب (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = تہی;
	VIM_CLEAR(incstack[incstack_idx].etag_fname);
    }
}

/*
 * Emacs tags line with CTRL-L: New file name on next line.
 * The file name is followed by a ','.  Remember etag file name in ebuf.
 * The FILE pointer to the tags file is stored in "st->fp".  If another tags
 * file is included, then the FILE pointer to the new tags file is stored in
 * "st->fp". The old file pointer is saved in incstack.
 */
    ساکن خالی
emacs_لیبل_فہرست_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    اگر (vim_fgets(st->ebuf, LSIZE, st->fp))
	واپس;

    برائے (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    اگر (*p == NUL)
	واپس;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    اگر (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_سائز)
	واپس;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->لیبل_fname);
    اگر (incstack[incstack_idx].etag_fname == تہی)
	واپس;

    incstack[incstack_idx].fp = st->fp;
    st->fp = تہی;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_لیبل_fname(st->ebuf, st->لیبل_fname, FALSE);
    اگر (fullpath_ebuf != تہی)
    {
	st->fp = mch_fopen((حرف *)fullpath_ebuf, "r");
	اگر (st->fp != تہی)
	{
	    اگر (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_لیبل_file_راستہ_truncated_for_str), st->ebuf);
	    vim_strncpy(st->لیبل_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    اگر (st->fp == تہی)
    {
	// Can't open the included file, skip it and
	// restore old value of "fp".
	st->fp = incstack[incstack_idx].fp;
	vim_free(incstack[incstack_idx].etag_fname);
    }
}

/*
 * Reached the end of an emacs-style tags file. If this is an included tags
 * file, then pop it from the incstack and continue processing the parent tags
 * file. Otherwise, processed all the tags.
 * Returns TRUE if an included tags file is popped and processing should
 * continue with the parent tags file. Returns FALSE to stop processing tags.
 */
    ساکن صحیح
emacs_لیبل_فہرست_file_eof(findtags_state_T *st)
{
    اگر (!incstack_idx)	// reached end of file. stop processing.
	واپس FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->لیبل_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    واپس TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ساکن صحیح
emacs_لیبل_فہرست_parse_سطر(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    اگر (p_7f == تہی)
    {
etag_fail:
	اگر (vim_strchr(lbuf, '\n') != تہی)
	    واپس FAIL;

	// Truncated line.  Ignore it.
	اگر (p_verbose >= 5)
	{
	    verbose_enter();
	    پیغام(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_انتہا = lbuf;
	واپس OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    اگر (p == تہی)
	p = p_7f + 1;
    ورنہ
	++p;

    اگر (!VIM_ISDIGIT(*p))	    // check for start of line number
	جاؤ etag_fail;
    tagp->command = p;

    اگر (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_انتہا = p - 1;
    }
    ورنہ			    // second format: isolate tagname
    {
	اگر (p_7f == lbuf)
	    جاؤ etag_fail;

	// find end of tagname
	برائے (p = p_7f - 1; !vim_iswordc(*p); --p)
	    اگر (p == lbuf)
		جاؤ etag_fail;
	tagp->tagname_انتہا = p + 1;
	جب (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    واپس OK;
}
#اگر_ختم

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    ساکن لیبل_فہرست_read_حالت_T
findtags_get_اگلا_سطر(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    صحیح		eof;
    off_T	انحراف;

    // For binary search: compute the next offset to use.
    اگر (st->state == TS_ثنائی)
    {
	انحراف = sinfo_p->کم_انحراف + ((sinfo_p->بلند_انحراف
						- sinfo_p->کم_انحراف) / 2);
	اگر (انحراف == sinfo_p->curr_انحراف)
	    واپس لیبل_فہرست_READ_EOF; // End the binary search without a match.
	ورنہ
	    sinfo_p->curr_انحراف = انحراف;
    }

    // Skipping back (after a match during binary search).
    ورنہ اگر (st->state == TS_SKIP_عقب)
    {
	sinfo_p->curr_انحراف -= st->lbuf_سائز * 2;
	اگر (sinfo_p->curr_انحراف < 0)
	{
	    sinfo_p->curr_انحراف = 0;
	    rewind(st->fp);
	    st->state = TS_قدم_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    اگر (st->state == TS_ثنائی || st->state == TS_SKIP_عقب)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_انحراف_مستعمل = sinfo_p->curr_انحراف;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_انحراف, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_سائز, st->fp);
	اگر (!eof && sinfo_p->curr_انحراف != 0)
	{
	    sinfo_p->curr_انحراف = vim_ftell(st->fp);
	    اگر (sinfo_p->curr_انحراف == sinfo_p->بلند_انحراف)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->کم_انحراف, SEEK_SET);
		sinfo_p->curr_انحراف = sinfo_p->کم_انحراف;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_سائز, st->fp);
	}
	// skip empty and blank lines
	جب (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_انحراف = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_سائز, st->fp);
	}
	اگر (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_عقب;
	    sinfo_p->مطابقت_انحراف = vim_ftell(st->fp);
	    sinfo_p->curr_انحراف = sinfo_p->curr_انحراف_مستعمل;
	    واپس لیبل_فہرست_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ورنہ
    {
	// skip empty and blank lines
	کرو
	{
#اگر_تعریف_شدہ FEAT_CSCOPE
	    اگر (st->flags & لیبل_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_سائز);
	    ورنہ
#اگر_ختم
		eof = vim_fgets(st->lbuf, st->lbuf_سائز, st->fp);
	} جب (!eof && vim_isblankline(st->lbuf));

	اگر (eof)
	{
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	    اگر (emacs_لیبل_فہرست_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		واپس لیبل_فہرست_READ_IGNORE;
#اگر_ختم
	    واپس لیبل_فہرست_READ_EOF;
	}
    }

    واپس لیبل_فہرست_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ساکن صحیح
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    اگر (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	واپس TRUE;

    // Process the header line.
    اگر (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->لیبل_file_sorted = st->lbuf[18];
    اگر (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	برائے (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_تیاری(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    واپس FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ساکن صحیح
findtags_آغاز_state_handler(
    findtags_state_T	*st,
    صحیح			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & لیبل_CSCOPE);
#اگر_ختم
    صحیح		noic = (st->flags & لیبل_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    اگر (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	واپس findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#اگر_تعریف_شدہ FEAT_CSCOPE
    اگر (st->خطی || use_cscope)
#ورنہ
    اگر (st->خطی)
#اگر_ختم
	st->state = TS_خطی;
    ورنہ اگر (st->لیبل_file_sorted == NUL)
	st->state = TS_ثنائی;
    ورنہ اگر (st->لیبل_file_sorted == '1')
	st->state = TS_ثنائی;
    ورنہ اگر (st->لیبل_file_sorted == '2')
    {
	st->state = TS_ثنائی;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ورنہ
	st->state = TS_خطی;

    اگر (st->state == TS_ثنائی && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->خطی = TRUE;
	st->state = TS_خطی;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    اگر (st->state == TS_ثنائی)
    {
	اگر (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_خطی;
	ورنہ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->کم_انحراف = 0;
	    sinfo_p->کم_char = 0;
	    sinfo_p->بلند_انحراف = filesize;
	    sinfo_p->curr_انحراف = 0;
	    sinfo_p->بلند_char = 0xff;
	}
	واپس FALSE;
    }

    واپس TRUE;
}

/*
 * Parse a tag line read from a tags file.
 * Also compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->head" as a quick check if the tag may match.
 * Returns:
 * - TAG_MATCH_SUCCESS if the tag may match
 * - TAG_MATCH_FAIL if the tag doesn't match
 * - TAG_MATCH_NEXT to look for the next matching tag (used in a binary search)
 * - TAG_MATCH_STOP if all the tags are processed without a match. Uses the
 *   values in "margs" for doing the comparison.
 */
    ساکن tagmatch_حالت_T
findtags_parse_سطر(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_مطابقت_دلائل_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    صحیح		حالت_2;
    صحیح		i;
    صحیح		cmplen;
    صحیح		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    اگر (st->orgpat->headlen
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	    && !st->is_etag
#اگر_ختم
       )
    {
	CLEAR_میدان(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_انتہا = vim_strchr(st->lbuf, TAB);
	اگر (tagpp->tagname_انتہا == تہی)
	    // Corrupted tag line.
	    واپس لیبل_مطابقت_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (صحیح)(tagpp->tagname_انتہا - tagpp->tagname);
	اگر (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	اگر ((st->flags & لیبل_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ورنہ اگر (st->state == TS_خطی && st->orgpat->headlen != cmplen)
	    واپس لیبل_مطابقت_اگلا;

	اگر (st->state == TS_ثنائی)
	{
	    // Simplistic check for unsorted tags file.
	    i = (صحیح)tagpp->tagname[0];
	    اگر (margs->sortic)
		i = (صحیح)TOUPPER_ASC(tagpp->tagname[0]);
	    اگر (i < sinfo_p->کم_char || i > sinfo_p->بلند_char)
		margs->ترتیب_دو_error = TRUE;

	    // Compare the current tag with the searched tag.
	    اگر (margs->sortic)
		tagcmp = لیبل_strnicmp(tagpp->tagname, st->orgpat->سر,
							(size_t)cmplen);
	    ورنہ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->سر, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    اگر (tagcmp == 0)
	    {
		اگر (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ورنہ اگر (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    اگر (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_عقب;
		sinfo_p->مطابقت_انحراف = sinfo_p->curr_انحراف;
		واپس لیبل_مطابقت_اگلا;
	    }
	    اگر (tagcmp < 0)
	    {
		sinfo_p->curr_انحراف = vim_ftell(st->fp);
		اگر (sinfo_p->curr_انحراف < sinfo_p->بلند_انحراف)
		{
		    sinfo_p->کم_انحراف = sinfo_p->curr_انحراف;
		    اگر (margs->sortic)
			sinfo_p->کم_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ورنہ
			sinfo_p->کم_char = tagpp->tagname[0];
		    واپس لیبل_مطابقت_اگلا;
		}
	    }
	    اگر (tagcmp > 0 && sinfo_p->curr_انحراف != sinfo_p->بلند_انحراف)
	    {
		sinfo_p->بلند_انحراف = sinfo_p->curr_انحراف;
		اگر (margs->sortic)
		    sinfo_p->بلند_char = TOUPPER_ASC(tagpp->tagname[0]);
		ورنہ
		    sinfo_p->بلند_char = tagpp->tagname[0];
		واپس لیبل_مطابقت_اگلا;
	    }

	    // No match yet and are at the end of the binary search.
	    واپس لیبل_مطابقت_STOP;
	}
	ورنہ اگر (st->state == TS_SKIP_عقب)
	{
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
		st->state = TS_قدم_FORWARD;
	    ورنہ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_انحراف = sinfo_p->curr_انحراف_مستعمل;
	    واپس لیبل_مطابقت_اگلا;
	}
	ورنہ اگر (st->state == TS_قدم_FORWARD)
	{
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
	    {
		اگر ((off_T)vim_ftell(st->fp) > sinfo_p->مطابقت_انحراف)
		    واپس لیبل_مطابقت_STOP;	// past last match
		ورنہ
		    واپس لیبل_مطابقت_اگلا;	// before first match
	    }
	}
	ورنہ
	    // skip this match if it can't match
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
		واپس لیبل_مطابقت_اگلا;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_انتہا + 1;
	tagpp->fname_انتہا = vim_strchr(tagpp->fname, TAB);
	اگر (tagpp->fname_انتہا == تہی)
	    حالت_2 = FAIL;
	ورنہ
	{
	    tagpp->command = tagpp->fname_انتہا + 1;
	    حالت_2 = OK;
	}
    }
    ورنہ
	حالت_2 = parse_لیبل_سطر(st->lbuf,
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
		st->is_etag,
#اگر_ختم
		tagpp);

    اگر (حالت_2 == FAIL)
	واپس لیبل_مطابقت_FAIL;

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (st->is_etag)
	tagpp->fname = st->ebuf;
#اگر_ختم

    واپس لیبل_مطابقت_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ساکن خالی
findtags_matchargs_init(findtags_مطابقت_دلائل_T *margs, صحیح flags)
{
    margs->matchoff = 0;			// match offset
    margs->مطابقت_re = FALSE;			// match with regexp
    margs->مطابقت_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & لیبل_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->ترتیب_دو_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    ساکن صحیح
findtags_مطابقت_لیبل(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_مطابقت_دلائل_T *margs)
{
    صحیح		مطابقت = FALSE;
    صحیح		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (صحیح)(tagpp->tagname_انتہا - tagpp->tagname);
    اگر (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    اگر (st->orgpat->len != cmplen)
	مطابقت = FALSE;
    ورنہ
    {
	اگر (st->orgpat->regmatch.rm_ic)
	{
	    مطابقت =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    اگر (مطابقت)
		margs->مطابقت_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ورنہ
	    مطابقت = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->مطابقت_re = FALSE;
    اگر (!مطابقت && st->orgpat->regmatch.regprog != تہی)
    {
	صحیح	cc;

	cc = *tagpp->tagname_انتہا;
	*tagpp->tagname_انتہا = NUL;
	مطابقت = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	اگر (مطابقت)
	{
	    margs->matchoff = (صحیح)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    اگر (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->مطابقت_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_انتہا = cc;
	margs->مطابقت_re = TRUE;
    }

    واپس مطابقت;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ساکن خالی
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_سطر;
    صحیح		len;

    conv_سطر = string_convert(&st->vimconv, st->lbuf, تہی);
    اگر (conv_سطر == تہی)
	واپس;

    // Copy or swap lbuf and conv_line.
    len = (صحیح)STRLEN(conv_سطر) + 1;
    اگر (len > st->lbuf_سائز)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_سطر;
	st->lbuf_سائز = len;
    }
    ورنہ
    {
	STRCPY(st->lbuf, conv_سطر);
	vim_free(conv_سطر);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    ساکن صحیح
findtags_جوڑو_مطابقت(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_مطابقت_دلائل_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & لیبل_CSCOPE);
#اگر_ختم
    صحیح		name_only = (st->flags & لیبل_نام);
    صحیح		mtt;
    صحیح		len = 0;
    صحیح		is_current;		// file name matches
    صحیح		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#اگر_تعریف_شدہ FEAT_CSCOPE
    اگر (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ورنہ
#اگر_ختم
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
		st->is_etag,
#اگر_ختم
		tagpp->fname, tagpp->fname_انتہا, st->لیبل_fname, buf_ffname);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	is_static = FALSE;
	اگر (!st->is_etag)	// emacs tags are never static
#اگر_ختم
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	//مطابقت
	اگر (is_static)
	{
	    اگر (is_current)
		mtt = MT_ST_CUR;
	    ورنہ
		mtt = MT_ST_OTH;
	}
	ورنہ
	{
	    اگر (is_current)
		mtt = MT_GL_CUR;
	    ورنہ
		mtt = MT_GL_OTH;
	}
	اگر (st->orgpat->regmatch.rm_ic && !margs->مطابقت_no_ic)
	    mtt += MT_IC_OFF;
	اگر (margs->مطابقت_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    اگر (st->help_only)
    {
#اگر_تعریف_شدہ FEAT_MULTI_LANG
# define ML_EXTRA 3
#ورنہ
# define ML_EXTRA 0
#اگر_ختم
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_انتہا = NUL;
	len = (صحیح)(tagpp->tagname_انتہا - tagpp->tagname);
	mfp = alloc(حجم(char_u) + len + 10 + ML_EXTRA + 1);
	اگر (mfp != تہی)
	{
	    صحیح heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#اگر_ختم

	    heuristic = help_heuristic(tagpp->tagname,
				margs->مطابقت_re ? margs->matchoff : 0,
				!margs->مطابقت_no_ic);
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#اگر_ختم
	    sprintf((حرف *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_انتہا = TAB;
    }
    ورنہ اگر (name_only)
    {
	اگر (st->get_searchpat)
	{
	    char_u *عارضی_انتہا = tagpp->command;

	    اگر (*عارضی_انتہا == '/')
		جب (*عارضی_انتہا && *عارضی_انتہا != '\r'
			&& *عارضی_انتہا != '\n'
			&& *عارضی_انتہا != '$')
		    عارضی_انتہا++;

	    اگر (tagpp->command + 2 < عارضی_انتہا)
	    {
		len = (صحیح)(عارضی_انتہا - tagpp->command - 2);
		mfp = alloc(len + 2);
		اگر (mfp != تہی)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ورنہ
		mfp = تہی;
	    st->get_searchpat = FALSE;
	}
	ورنہ
	{
	    len = (صحیح)(tagpp->tagname_انتہا - tagpp->tagname);
	    mfp = alloc(حجم(char_u) + len + 1);
	    اگر (mfp != تہی)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    اگر (State & منوال_داخل_کرو)
		st->get_searchpat = p_sft;
	}
    }
    ورنہ
    {
	size_t لیبل_fname_len = STRLEN(st->لیبل_fname);
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	size_t ebuf_len = 0;
#اگر_ختم

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (صحیح)لیبل_fname_len + (صحیح)STRLEN(st->lbuf) + 3;
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	اگر (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (صحیح)ebuf_len + 1;
	}
	ورنہ
	    ++len;
#اگر_ختم
	mfp = alloc(حجم(char_u) + len + 1);
	اگر (mfp != تہی)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->لیبل_fname);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#اگر_ختم
	    p[لیبل_fname_len + 1] = لیبل_SEP;
	    s = p + 1 + لیبل_fname_len + 1;
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	    اگر (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = لیبل_SEP;
		s += ebuf_len + 1;
	    }
	    ورنہ
		*s++ = لیبل_SEP;
#اگر_ختم
	    STRCPY(s, st->lbuf);
	}
    }

    اگر (mfp != تہی)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#اگر_تعریف_شدہ FEAT_CSCOPE
	اگر (use_cscope)
	    ++*hash;
	ورنہ
#اگر_ختم
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_مطابقت[mtt], mfp, *hash);
	اگر (HASHITEM_خالی_خانے(hi))
	{
	    اگر (hash_جوڑو_شے(&st->ht_مطابقت[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_مطابقت[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		واپس FAIL;
	    }

	    ((char_u **)(st->ga_مطابقت[mtt].ga_ڈیٹا))
		[st->ga_مطابقت[mtt].ga_len++] = mfp;
	    st->مطابقت_گنتی++;
	}
	ورنہ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    واپس OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ساکن خالی
findtags_get_all_لیبل_فہرست(
    findtags_state_T		*st,
    findtags_مطابقت_دلائل_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	تلاش_info;
    صحیح			retval;
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح			use_cscope = (st->flags & لیبل_CSCOPE);
#اگر_ختم
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_میدان(تلاش_info);

    // Read and parse the lines in the file one by one
    برائے (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	اگر (st->state == TS_ثنائی || st->state == TS_SKIP_عقب)
	    سطر_breakcheck();
	ورنہ
	    fast_breakcheck();
	اگر ((st->flags & لیبل_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_کلیدیں(30, FALSE);
	اگر (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    توڑو;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	اگر (st->mincount == لیبل_MANY && st->مطابقت_گنتی >= لیبل_MANY)
	{
	    st->stop_searching = TRUE;
	    توڑو;
	}
	اگر (st->get_searchpat)
	    جاؤ سطر_read_اندر;

	retval = findtags_get_اگلا_سطر(st, &تلاش_info);
	اگر (retval == لیبل_فہرست_READ_IGNORE)
	    جاری;
	اگر (retval == لیبل_فہرست_READ_EOF)
	    توڑو;

سطر_read_اندر:

	اگر (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	اگر (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_خطی;
	    emacs_لیبل_فہرست_new_filename(st);
	    جاری;
	}
#اگر_ختم

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	اگر (st->state == TS_آغاز)
	{
	    اگر (findtags_آغاز_state_handler(st, &margs->sortic, &تلاش_info) == FALSE)
		جاری;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	اگر (st->lbuf[st->lbuf_سائز - 2] != NUL
#اگر_تعریف_شدہ FEAT_CSCOPE
		&& !use_cscope
#اگر_ختم
	   )
	{
	    st->lbuf_سائز *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_سائز);
	    اگر (st->lbuf == تہی)
	    {
		اگر (st->fp != تہی)
		    fclose(st->fp);
		st->fp = تہی;
		st->stop_searching = TRUE;
		واپس;
	    }

	    اگر (st->state == TS_قدم_FORWARD || st->state == TS_خطی)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, تلاش_info.curr_انحراف,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    تلاش_info.curr_انحراف = 0;
	    جاری;
	}

	retval = findtags_parse_سطر(st, &tagp, margs, &تلاش_info);
	اگر (retval == لیبل_مطابقت_اگلا)
	    جاری;
	اگر (retval == لیبل_مطابقت_STOP)
	    توڑو;
	اگر (retval == لیبل_مطابقت_FAIL)
	{
	    semsg(_(e_format_error_اندر_لیبل_فہرست_file_str), st->لیبل_fname);
#اگر_تعریف_شدہ FEAT_CSCOPE
	    اگر (!use_cscope)
#اگر_ختم
		semsg(_("Before byte %ld"), (طویل)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    واپس;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	اگر (findtags_مطابقت_لیبل(st, &tagp, margs))
	{
	    اگر (findtags_جوڑو_مطابقت(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		توڑو;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ساکن خالی
findtags_اندر_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_مطابقت_دلائل_T margs;
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & لیبل_CSCOPE);
#اگر_ختم

    st->vimconv.vc_type = CONV_NONE;
    st->لیبل_file_sorted = NUL;
    st->fp = تہی;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#اگر_تعریف_شدہ FEAT_CSCOPE
    اگر (use_cscope)
	st->fp = تہی;	    // avoid GCC warning
    ورنہ
#اگر_ختم
    {
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	اگر (curbuf->b_help)
	{
	    اگر (!findtags_اندر_help_init(st))
		واپس;
	}
#اگر_ختم

	st->fp = mch_fopen((حرف *)st->لیبل_fname, "r");
	اگر (st->fp == تہی)
	    واپس;

	اگر (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->لیبل_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_آغاز;	// we're at the start of the file
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    st->is_etag = FALSE;	// default is: not emacs style
#اگر_ختم

    // Read and parse the lines in the file one by one
    findtags_get_all_لیبل_فہرست(st, &margs, buf_ffname);

    اگر (st->fp != تہی)
    {
	fclose(st->fp);
	st->fp = تہی;
    }
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    emacs_لیبل_فہرست_incstack_free();
#اگر_ختم
    اگر (st->vimconv.vc_type != CONV_NONE)
	convert_تیاری(&st->vimconv, تہی, تہی);

    اگر (margs.ترتیب_دو_error)
	semsg(_(e_لیبل_فہرست_file_not_sorted_str), st->لیبل_fname);

    // Stop searching if sufficient tags have been found.
    اگر (st->مطابقت_گنتی >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ساکن صحیح
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    صحیح		name_only = (st->flags & لیبل_نام);
    char_u	**matches;
    صحیح		mtt;
    صحیح		i;
    char_u	*mfp;
    char_u	*p;

    اگر (st->مطابقت_گنتی > 0)
	matches = ALLOC_MULT(char_u *, st->مطابقت_گنتی);
    ورنہ
	matches = تہی;
    st->مطابقت_گنتی = 0;
    برائے (mtt = 0; mtt < MT_گنتی; ++mtt)
    {
	برائے (i = 0; i < st->ga_مطابقت[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_مطابقت[mtt].ga_ڈیٹا))[i];
	    اگر (matches == تہی)
		vim_free(mfp);
	    ورنہ
	    {
		اگر (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    برائے (p = mfp + 1; *p != NUL; ++p)
			اگر (*p == لیبل_SEP)
			    *p = NUL;
		}
		matches[st->مطابقت_گنتی++] = mfp;
	    }
	}

	ga_clear(&st->ga_مطابقت[mtt]);
	hash_clear(&st->ht_مطابقت[mtt]);
    }

    *matchesp = matches;
    واپس st->مطابقت_گنتی;
}

/*
 * find_tags() - search for tags in tags files
 *
 * Return FAIL if search completely failed (*num_matches will be 0, *matchesp
 * will be NULL), OK otherwise.
 *
 * Priority depending on which type of tag is recognized:
 *  6.	A static or global tag with a full matching tag for the current file.
 *  5.	A global tag with a full matching tag for another file.
 *  4.	A static tag with a full matching tag for another file.
 *  3.	A static or global tag with an ignore-case matching tag for the
 *	current file.
 *  2.	A global tag with an ignore-case matching tag for another file.
 *  1.	A static tag with an ignore-case matching tag for another file.
 *
 * Tags in an emacs-style tags file are always global.
 *
 * flags:
 * TAG_HELP	  only search for help tags
 * TAG_NAMES	  only return name of tag
 * TAG_REGEXP	  use "pat" as a regexp
 * TAG_NOIC	  don't always ignore case
 * TAG_KEEP_LANG  keep language
 * TAG_CSCOPE	  use cscope results for tags
 * TAG_NO_TAGFUNC do not call the 'tagfunc' function
 */
    صحیح
ڈھونڈو_لیبل_فہرست(
    char_u	*pat,			// pattern to search for
    صحیح		*عدد_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    صحیح		flags,
    صحیح		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    صحیح		پہلا_file;		// trying first tag file
    صحیح		retval = FAIL;		// return value
    صحیح		باری;

    صحیح		save_emsg_off;

    bool	help_save;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    صحیح		i;
    char_u	*saved_pat = تہی;		// copy of pat[]
#اگر_ختم

    صحیح		findall = (mincount == MAXCOL || mincount == لیبل_MANY);
						// find all matching tags
    صحیح		has_re = (flags & لیبل_REGEXP);	// regexp used
    صحیح		noic = (flags & لیبل_NOIC);
#اگر_تعریف_شدہ FEAT_CSCOPE
    صحیح		use_cscope = (flags & لیبل_CSCOPE);
#اگر_ختم
    صحیح		verbose = (flags & لیبل_VERBOSE);
    صحیح		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    انتخاب (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	حالت TC_FOLLOWIC:		 توڑو;
	حالت TC_IGNORE:    p_ic = TRUE;  توڑو;
	حالت TC_مطابقت:     p_ic = FALSE; توڑو;
	حالت TC_FOLLOWSCS: p_ic = ignorecase(pat); توڑو;
	حالت TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); توڑو;
    }

    help_save = curbuf->b_help;

    اگر (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	جاؤ findtag_انتہا;

#اگر_تعریف_شدہ FEAT_CSCOPE
    STRCPY(st.لیبل_fname, "from cscope");	// for error messages
#اگر_ختم

    /*
     * Initialize a few variables
     */
    اگر (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#اگر_تعریف_شدہ FEAT_CSCOPE
    ورنہ اگر (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    اگر (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	اگر (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    اگر (saved_pat != تہی)
	    {
		st.help_lang_ڈھونڈو = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#اگر_ختم
    اگر (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    اگر (has_re && st.orgpat->regmatch.regprog == تہی)
	جاؤ findtag_انتہا;

#اگر_تعریف_شدہ FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    اگر (retval != NOTDONE)
	جاؤ findtag_انتہا;

    // re-initialize the default return value
    retval = FAIL;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    اگر ((flags & لیبل_KEEP_LANG)
	    && st.help_lang_ڈھونڈو == تہی
	    && curbuf->b_fname != تہی
	    && (i = (صحیح)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#اگر_ختم

    /*
     * When finding a specified number of matches, first try with matching
     * case, so binary search can be used, and try ignore-case matches in a
     * second loop.
     * When finding all matches, 'tagbsearch' is off, or there is no fixed
     * string to look for, ignore case right away to avoid going though the
     * tags files twice.
     * When the tag file is case-fold sorted, it is either one or the other.
     * Only ignore case when TAG_NOIC not used or 'ignorecase' set.
     */
    st.orgpat->regmatch.rm_ic = ((p_ic || !noic)
			&& (findall || st.orgpat->headlen == 0 || !p_tbs));
    برائے (باری = 1; باری <= 2; ++باری)
    {
	st.خطی = (st.orgpat->headlen == 0 || !p_tbs || باری == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      برائے (پہلا_file = TRUE;
#اگر_تعریف_شدہ FEAT_CSCOPE
	    use_cscope ||
#اگر_ختم
		get_tagfname(&tn, پہلا_file, st.لیبل_fname) == OK;
							   پہلا_file = FALSE)
      {
	  findtags_اندر_file(&st, buf_ffname);
	  اگر (st.stop_searching
#اگر_تعریف_شدہ FEAT_CSCOPE
		  || use_cscope
#اگر_ختم
	     )
	  {
	      retval = OK;
	      توڑو;
	  }
      } // end of for-each-file loop

#اگر_تعریف_شدہ FEAT_CSCOPE
	اگر (!use_cscope)
#اگر_ختم
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	اگر (st.stop_searching || st.خطی || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    توڑو;
#اگر_تعریف_شدہ FEAT_CSCOPE
	اگر (use_cscope)
	    توڑو;
#اگر_ختم

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    اگر (!st.stop_searching)
    {
	اگر (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_لیبل_فہرست_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_انتہا:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    اگر (retval == FAIL)
	st.مطابقت_گنتی = 0;

    *عدد_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    vim_free(saved_pat);
#اگر_ختم

    p_ic = save_p_ic;

    واپس retval;
}

ساکن garray_T لیبل_fnames = GA_خالی_خانے;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ساکن خالی
ملا_tagfile_cb(char_u *fname, خالی *cookie غیر_مستعمل)
{
    اگر (ga_grow(&لیبل_fnames, 1) == FAIL)
	واپس;

    char_u	*لیبل_fname = vim_strsave(fname);

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    slash_adjust(لیبل_fname);
#اگر_ختم
    simplify_filename(لیبل_fname);
    ((char_u **)(لیبل_fnames.ga_ڈیٹا))[لیبل_fnames.ga_len++] = لیبل_fname;
}

#اگر defined(EXITFREE)
    خالی
free_لیبل_stuff(خالی)
{
    ga_clear_لڑیاں(&لیبل_fnames);
    اگر (curwin != تہی)
	do_لیبل(تہی, DT_FREE, 0, 0, 0);
    لیبل_freematch();

# اگر defined(FEAT_QUICKFIX)
    tagstack_clear_اندراج(&ptag_اندراج);
# endif
}
#اگر_ختم

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    صحیح
get_tagfname(
    tagname_T	*tnp,	// holds status info
    صحیح		پہلا,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = تہی;
    char_u		*r_ptr;
    صحیح			i;

    اگر (پہلا)
	CLEAR_اشارہ_گر(tnp);

    اگر (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	اگر (پہلا)
	{
	    ga_clear_لڑیاں(&لیبل_fnames);
	    ga_init2(&لیبل_fnames, حجم(char_u *), 10);
	    do_اندر_runtimepath((char_u *)
#اگر_تعریف_شدہ FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ورنہ
		    "doc/tags doc/tags-??"
# endif
#ورنہ
		    "doc/tags"
#اگر_ختم
					   , DIP_ALL, ملا_tagfile_cb, تہی);
	}

	اگر (tnp->tn_hf_idx >= لیبل_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    اگر (tnp->tn_hf_idx > لیبل_fnames.ga_len || *p_hf == NUL)
		واپس FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    slash_adjust(buf);
#اگر_ختم
	    simplify_filename(buf);

	    برائے (i = 0; i < لیبل_fnames.ga_len; ++i)
		اگر (STRCMP(buf, ((char_u **)(لیبل_fnames.ga_ڈیٹا))[i]) == 0)
		    واپس FAIL; // avoid duplicate file names
	}
	ورنہ
	    vim_strncpy(buf, ((char_u **)(لیبل_fnames.ga_ڈیٹا))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	واپس OK;
    }

    اگر (پہلا)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_لیبل_فہرست = vim_strsave((*curbuf->b_p_لیبل_فہرست != NUL)
						 ? curbuf->b_p_لیبل_فہرست : p_لیبل_فہرست);
	اگر (tnp->tn_لیبل_فہرست == تہی)
	    واپس FAIL;
	tnp->tn_np = tnp->tn_لیبل_فہرست;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    برائے (;;)
    {
	اگر (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_تلاش_ctx);
	    اگر (fname != تہی)
		توڑو;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ورنہ
	{
	    char_u  *filename = تہی;

	    // Stop when used all parts of 'tags'.
	    اگر (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_تلاش_ctx);
		tnp->tn_تلاش_ctx = تہی;
		واپس FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (خالی)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    اگر (r_ptr != تہی)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_تلاش_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_تلاش_ctx, TRUE, curbuf->b_ffname);
	    اگر (tnp->tn_تلاش_ctx != تہی)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    واپس OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    خالی
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_لیبل_فہرست);
    vim_findfile_cleanup(tnp->tn_تلاش_ctx);
    tnp->tn_تلاش_ctx = تہی;
    ga_clear_لڑیاں(&لیبل_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    ساکن صحیح
parse_لیبل_سطر(
    char_u	*lbuf,		// line to be parsed
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    صحیح		is_etag,
#اگر_ختم
    tagptrs_T	*tagp)
{
    char_u	*p;

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (is_etag)
	// emacs-style tag file
	واپس emacs_لیبل_فہرست_parse_سطر(lbuf, tagp);
#اگر_ختم

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    اگر (p == تہی)
	واپس FAIL;
    tagp->tagname_انتہا = p;

    // Isolate file name, from first to second white space
    اگر (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    اگر (p == تہی)
	واپس FAIL;
    tagp->fname_انتہا = p;

    // find start of search command, after second white space
    اگر (*p != NUL)
	++p;
    اگر (*p == NUL)
	واپس FAIL;
    tagp->command = p;

    واپس OK;
}

/*
 * Check if tagname is a static tag
 *
 * Static tags produced by the older ctags program have the format:
 *	'file:tag  file  /pattern'.
 * This is only recognized when both occurrence of 'file' are the same, to
 * avoid recognizing "string::string" or ":exit".
 *
 * Static tags produced by the new ctags program have the format:
 *	'tag  file  /pattern/;"<Tab>file:'	    "
 *
 * Return TRUE if it is a static tag and adjust *tagname to the real tag.
 * Return FALSE if it is not a static tag.
 */
    ساکن صحیح
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    جب ((p = vim_strchr(p, '\t')) != تہی)
    {
	++p;
	اگر (STRNCMP(p, "file:", 5) == 0)
	    واپس TRUE;
    }

    واپس FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ساکن size_t
matching_سطر_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    p += STRLEN(p) + 1;
#اگر_ختم
    واپس (p - lbuf) + STRLEN(p);
}

/*
 * Parse a line from a matching tag.  Does not change the line itself.
 *
 * The line that we get looks like this:
 * Emacs tag: <mtt><tag_fname><NUL><ebuf><NUL><lbuf>
 * other tag: <mtt><tag_fname><NUL><NUL><lbuf>
 * without Emacs tags: <mtt><tag_fname><NUL><lbuf>
 *
 * Return OK or FAIL.
 */
    ساکن صحیح
parse_مطابقت(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    صحیح		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->لیبل_fname = lbuf + 1;
    lbuf += STRLEN(tagp->لیبل_fname) + 2;
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_انتہا = lbuf++;
    }
    ورنہ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#اگر_ختم

    // Find search pattern and the file name for non-etags.
    retval = parse_لیبل_سطر(lbuf,
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
			tagp->is_etag,
#اگر_ختم
			tagp);

    tagp->tagkind = تہی;
    tagp->user_ڈیٹا = تہی;
    tagp->tagline = 0;
    tagp->command_انتہا = تہی;

    اگر (retval != OK)
	واپس retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    اگر (ڈھونڈو_extra(&p) == OK)
    {
	اگر (p > tagp->command && p[-1] == '|')
	    tagp->command_انتہا = p - 1;  // drop trailing bar
	ورنہ
	    tagp->command_انتہا = p;
	p += 2;	// skip ";\""
	اگر (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    جب (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		اگر (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ورنہ اگر (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_ڈیٹا = p + 10;
		ورنہ اگر (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((حرف *)p + 5);
		اگر (tagp->tagkind != تہی && tagp->user_ڈیٹا != تہی)
		    توڑو;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		اگر (pc == تہی || (pt != تہی && pc > pt))
		    tagp->tagkind = p;
		اگر (pt == تہی)
		    توڑو;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    اگر (tagp->tagkind != تہی)
    {
	برائے (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_انتہا = p;
    }
    اگر (tagp->user_ڈیٹا != تہی)
    {
	برائے (p = tagp->user_ڈیٹا;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_ڈیٹا_انتہا = p;
    }
    واپس retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ساکن char_u *
لیبل_بھرا_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    صحیح		c;

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ورنہ
#اگر_ختم
    {
	c = *tagp->fname_انتہا;
	*tagp->fname_انتہا = NUL;
    }
    fullname = expand_لیبل_fname(tagp->fname, tagp->لیبل_fname, FALSE);

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (!tagp->is_etag)
#اگر_ختم
	*tagp->fname_انتہا = c;

    واپس fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ساکن صحیح
jumpto_لیبل(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    صحیح		forceit,	// :ta with !
    صحیح		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    صحیح		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_انتہا;
    char_u	*tofree_fname = تہی;
    char_u	*fname;
    tagptrs_T	tagp;
    صحیح		retval = FAIL;
    صحیح		getfile_نتیجہ = GETFILE_غیر_مستعمل;
    صحیح		تلاش_options;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    صحیح		save_no_hlsearch;
#اگر_ختم
#اگر defined(FEAT_QUICKFIX)
    win_T	*curwin_save = تہی;
#اگر_ختم
    char_u	*بھرا_fname = تہی;
#اگر_تعریف_شدہ FEAT_FOLDING
    صحیح		old_کلید_Typed = کلید_Typed;    // getting the file may reset it
#اگر_ختم
    size_t	len;
    char_u	*lbuf;
    صحیح		ہندسہ_ہے = FALSE;

    اگر (postponed_بانٹو == 0 && !check_can_سیٹ_curbuf_forceit(forceit))
	واپس FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_سطر_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    اگر (lbuf != تہی)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    اگر (pbuf == تہی || lbuf == تہی || parse_مطابقت(lbuf, &tagp) == FAIL)
    {
	tagp.fname_انتہا = تہی;
	جاؤ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_انتہا = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    اگر (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	ہندسہ_ہے = TRUE;
	pbuf[0] = ':';
    }
    برائے (pbuf_انتہا = pbuf + ہندسہ_ہے;
	    *str && *str != '\n' && *str != '\r'; )
    {
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	اگر (tagp.is_etag && *str == ',')// stop at ',' after line number
	    توڑو;
#اگر_ختم
	*pbuf_انتہا++ = *str++;
	اگر (pbuf_انتہا - pbuf + 1 + ہندسہ_ہے >= LSIZE)
	    توڑو;
    }
    *pbuf_انتہا = NUL;

#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    اگر (!tagp.is_etag)
#اگر_ختم
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	اگر (ہندسہ_ہے)
	    str++;
	اگر (ڈھونڈو_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    اگر (str > pbuf && str[-1] == '|')
		pbuf_انتہا = str - 1;
	    ورنہ
		pbuf_انتہا = str;
	    *pbuf_انتہا = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_لیبل_fname(fname, tagp.لیبل_fname, TRUE);
    اگر (fname == تہی)
	جاؤ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    اگر (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, تہی))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	اگر (nofile_fname == تہی)
	    nofile_fname = خالی_خانے_option;
	جاؤ erret;
    }

    ++RedrawingDisabled;

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

#اگر defined(FEAT_QUICKFIX)
    اگر (g_do_tagpreview != 0)
    {
	postponed_بانٹو = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	اگر (!curwin->w_p_pvw)
	{
	    بھرا_fname = بھرا_Name_save(fname, FALSE);
	    fname = بھرا_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    اگر (postponed_بانٹو && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	اگر (existing_buf != تہی)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    اگر (swbuf_goto_win_with_buf(existing_buf) != تہی)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_نتیجہ = GETFILE_SAME_FILE;
	}
    }
    اگر (getfile_نتیجہ == GETFILE_غیر_مستعمل
				  && (postponed_بانٹو || cmdmod.cmod_tab != 0))
    {
	اگر (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_بانٹو |= WSP_VERT;

	اگر ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_اشاریہ(curtab) + 1;

	اگر (win_بانٹو(postponed_بانٹو > 0 ? postponed_بانٹو : 0,
						postponed_بانٹو_flags) == FAIL)
	{
	    اگر (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    جاؤ erret;
	}
	RESET_BINDING(curwin);
    }
#اگر_ختم

    اگر (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_بفر);
	ورنہ
#اگر_ختم
	    keep_help_flag = curbuf->b_help;
    }

    اگر (getfile_نتیجہ == GETFILE_غیر_مستعمل)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_نتیجہ = getfile(0, fname, تہی, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    اگر (GETFILE_SUCCESS(getfile_نتیجہ))	// got to the right file
    {
	curwin->w_سیٹ_curswant = true;
	postponed_بانٹو = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	اگر (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#اگر_ختم

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	اگر (vim_strchr(p_cpo, CPO_TAGPAT) != تہی)
	    تلاش_options = 0;
	ورنہ
	    تلاش_options = تلاش_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	اگر (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	اگر (str > pbuf_انتہا - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_انتہا - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    اگر (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ورنہ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    اگر (do_تلاش(تہی, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (طویل)1,
							 تلاش_options, تہی))
		retval = OK;
	    ورنہ
	    {
		صحیح	ملا = 1;
		صحیح	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		اگر (!do_تلاش(تہی, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (طویل)1,
							 تلاش_options, تہی))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    ملا = 2;
		    (خالی)test_for_static(&tagp);
		    cc = *tagp.tagname_انتہا;
		    *tagp.tagname_انتہا = NUL;
		    pbuflen = vim_snprintf((حرف *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    اگر (!do_تلاش(تہی, '/', '/', pbuf, pbuflen, (طویل)1,
							 تلاش_options, تہی))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((حرف *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			اگر (!do_تلاش(تہی, '/', '/', pbuf, pbuflen, (طویل)1,
							 تلاش_options, تہی))
			    ملا = 0;
		    }
		    *tagp.tagname_انتہا = cc;
		}
		اگر (ملا == 0)
		{
		    emsg(_(e_cannot_ڈھونڈو_لیبل_نمونہ));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ورنہ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    اگر (ملا == 2 || !save_p_ic)
		    {
			پیغام(_(e_couldnt_ڈھونڈو_لیبل_just_guessing));
			اگر (!پیغام_scrolled && پیغام_silent == 0)
			{
			    برآمد_flush();
			    ui_delay(1010L, TRUE);
			}
		    }
		    retval = OK;
		}
	    }
	    p_ws = save_p_ws;
	    p_ic = save_p_ic;
	    p_scs = save_p_scs;

	    // A search command may have positioned the cursor beyond the end
	    // of the line.  May need to correct that here.
	    check_cursor();
	}
	ورنہ
	{
	    صحیح		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#اگر_تعریف_شدہ HAVE_SANDBOX
	    ++sandbox;
#اگر_ختم
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.ستون = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    اگر (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#اگر_تعریف_شدہ HAVE_SANDBOX
	    --sandbox;
#اگر_ختم
	}

	magic_overruled = save_magic_overruled;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	اگر (تلاش_options)
	    سیٹ_no_hlsearch(save_no_hlsearch);
#اگر_ختم

	// Return OK if jumped to another file (at least we found the file!).
	اگر (getfile_نتیجہ == GETFILE_OPEN_OTHER)
	    retval = OK;

	اگر (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    اگر (curbuf->b_help)
		سیٹ_topline(curwin, curwin->w_cursor.lnum);
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر ((fdo_flags & FDO_لیبل) && old_کلید_Typed)
		foldOpenCursor();
#اگر_ختم
	}

#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#اگر_ختم

	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ورنہ
    {
	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	اگر (postponed_بانٹو)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_بانٹو = 0;
	}
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ورنہ اگر (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    اگر (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#اگر_ختم
    }
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    اگر (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#اگر_ختم

erret:
#اگر defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#اگر_ختم
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(بھرا_fname);

    واپس retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ساکن char_u *
expand_لیبل_fname(char_u *fname, char_u *لیبل_fname, صحیح expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = تہی;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    اگر (p_tagsecure && راستہ_with_url(fname))
    {
       emsg(_(e_لیبل_file_اندراج_must_not_be_url));
       واپس تہی;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    اگر (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == تہی)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, تہی,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	اگر (expanded_fname != تہی)
	    fname = expanded_fname;
    }

    اگر ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(لیبل_fname)) != لیبل_fname)
    {
	retval = alloc(MAXPATHL);
	اگر (retval != تہی)
	{
	    STRCPY(retval, لیبل_fname);
	    vim_strncpy(retval + (p - لیبل_fname), fname,
					      MAXPATHL - (p - لیبل_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    ورنہ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    واپس retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ساکن صحیح
test_for_current(
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
    صحیح	    is_etag,
#اگر_ختم
    char_u  *fname,
    char_u  *fname_انتہا,
    char_u  *لیبل_fname,
    char_u  *buf_ffname)
{
    صحیح	    c;
    صحیح	    retval = FALSE;
    char_u  *fullname;

    اگر (buf_ffname != تہی)	// if the buffer has a name
    {
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	اگر (is_etag)
	    c = 0;	    // to shut up GCC
	ورنہ
#اگر_ختم
	{
	    c = *fname_انتہا;
	    *fname_انتہا = NUL;
	}
	fullname = expand_لیبل_fname(fname, لیبل_fname, TRUE);
	اگر (fullname != تہی)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#اگر_تعریف_شدہ FEAT_EMACS_لیبل_فہرست
	اگر (!is_etag)
#اگر_ختم
	    *fname_انتہا = c;
    }

    واپس retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ساکن صحیح
ڈھونڈو_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	پہلا_char = **pp;

    // Repeat for addresses separated with ';'
    برائے (;;)
    {
	اگر (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ورنہ اگر (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    اگر (*str != پہلا_char)
		str = تہی;
	    ورنہ
		++str;
	}
	ورنہ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)لڑی_تلاش((حرف *)str, "|;\"");
	    اگر (str != تہی)
	    {
		++str;
		توڑو;
	    }

	}
	اگر (str == تہی || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    توڑو;
	++str;	// skip ';'
	پہلا_char = *str;
    }

    اگر (str != تہی && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	واپس OK;
    }
    واپس FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    خالی
tagstack_clear_اندراج(taggy_T *شے)
{
    VIM_CLEAR(شے->tagname);
    VIM_CLEAR(شے->user_ڈیٹا);
}

    صحیح
expand_لیبل_فہرست(
    صحیح		tagnames,	// expand tag names
    char_u	*pat,
    صحیح		*عدد_file,
    char_u	***file)
{
    صحیح		i;
    صحیح		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_سائز = 100;
    tagptrs_T	t_p;
    صحیح		ret;

    name_buf = alloc(name_buf_سائز);
    اگر (name_buf == تہی)
	واپس FAIL;

    اگر (tagnames)
	extra_flag = لیبل_نام;
    ورنہ
	extra_flag = 0;
    اگر (pat[0] == '/')
	ret = ڈھونڈو_لیبل_فہرست(pat + 1, عدد_file, file,
		لیبل_REGEXP | extra_flag | لیبل_VERBOSE,
		لیبل_MANY, curbuf->b_ffname);
    ورنہ
	ret = ڈھونڈو_لیبل_فہرست(pat, عدد_file, file,
	      لیبل_REGEXP | extra_flag | لیبل_VERBOSE | لیبل_NOIC,
		لیبل_MANY, curbuf->b_ffname);
    اگر (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 برائے (i = 0; i < *عدد_file; i++)
	 {
	     size_t	len;

	     parse_مطابقت((*file)[i], &t_p);
	     len = t_p.tagname_انتہا - t_p.tagname;
	     اگر (len > name_buf_سائز - 3)
	     {
		 char_u *buf;

		 name_buf_سائز = len + 3;
		 buf = vim_realloc(name_buf, name_buf_سائز);
		 اگر (buf == تہی)
		 {
		     vim_free(name_buf);
		     واپس FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != تہی && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_انتہا - t_p.fname);
	     (*file)[i][len + (t_p.fname_انتہا - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    واپس ret;
}

#اگر defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ساکن صحیح
جوڑو_لیبل_میدان(
    dict_T  *dict,
    حرف    *میدان_name,
    char_u  *آغاز,		// start of the value
    char_u  *انتہا)		// after the value; can be NULL
{
    char_u	*buf;
    صحیح		len = 0;
    صحیح		retval;

    // check that the field name doesn't exist yet
    اگر (dict_has_کلید(dict, میدان_name))
    {
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), میدان_name);
	    verbose_leave();
	}
	واپس FAIL;
    }
    buf = alloc(MAXPATHL);
    اگر (buf == تہی)
	واپس FAIL;
    اگر (آغاز != تہی)
    {
	اگر (انتہا == تہی)
	{
	    انتہا = آغاز + STRLEN(آغاز);
	    جب (انتہا > آغاز && (انتہا[-1] == '\r' || انتہا[-1] == '\n'))
		--انتہا;
	}
	len = (صحیح)(انتہا - آغاز);
	اگر (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, آغاز, len);
    }
    buf[len] = NUL;
    retval = dict_جوڑو_string(dict, میدان_name, buf);
    vim_free(buf);
    واپس retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    صحیح
get_لیبل_فہرست(list_T *list, char_u *pat, char_u *buf_fname)
{
    صحیح		عدد_matches, i, ret;
    char_u	**matches, *p;
    char_u	*بھرا_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    طویل	is_static;

    ret = ڈھونڈو_لیبل_فہرست(pat, &عدد_matches, &matches,
				لیبل_REGEXP | لیبل_NOIC, (صحیح)MAXCOL, buf_fname);
    اگر (ret != OK || عدد_matches <= 0)
	واپس ret;

    برائے (i = 0; i < عدد_matches; ++i)
    {
	اگر (parse_مطابقت(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    جاری;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	اگر (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    جاری;
	}

	اگر ((dict = dict_alloc()) == تہی)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    توڑو;
	}
	اگر (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    جاری;
	}

	بھرا_fname = لیبل_بھرا_fname(&tp);
	اگر (جوڑو_لیبل_میدان(dict, "name", tp.tagname, tp.tagname_انتہا) == FAIL
		|| جوڑو_لیبل_میدان(dict, "filename", بھرا_fname,
		    تہی) == FAIL
		|| جوڑو_لیبل_میدان(dict, "cmd", tp.command,
		    tp.command_انتہا) == FAIL
		|| جوڑو_لیبل_میدان(dict, "kind", tp.tagkind,
		    tp.tagkind_انتہا) == FAIL
		|| dict_جوڑو_نمبر(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(بھرا_fname);

	اگر (tp.command_انتہا != تہی)
	{
	    برائے (p = tp.command_انتہا + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		اگر (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_انتہا - 1;
		ورنہ اگر (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ورنہ اگر (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    صحیح	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    جب (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (صحیح)(p - n);
		    اگر (*p == ':' && len > 0)
		    {
			s = ++p;
			جب (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			اگر (جوڑو_لیبل_میدان(dict, (حرف *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ورنہ
			// Skip field without colon.
			جب (*p != NUL && *p >= ' ')
			    ++p;
		    اگر (*p == NUL)
			توڑو;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    واپس ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ساکن خالی
get_لیبل_details(taggy_T *لیبل, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_جوڑو_string(retdict, "tagname", لیبل->tagname);
    dict_جوڑو_نمبر(retdict, "matchnr", لیبل->cur_مطابقت + 1);
    dict_جوڑو_نمبر(retdict, "bufnr", لیبل->cur_fnum);
    اگر (لیبل->user_ڈیٹا)
	dict_جوڑو_string(retdict, "user_data", لیبل->user_ڈیٹا);

    اگر ((pos = list_alloc_id(aid_tagstack_from)) == تہی)
	واپس;
    dict_جوڑو_list(retdict, "from", pos);

    fmark = &لیبل->fmark;
    list_append_نمبر(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_نمبر(pos, (varnumber_T)fmark->mark.lnum);
    list_append_نمبر(pos, (varnumber_T)(fmark->mark.ستون == MAXCOL ?
					MAXCOL : fmark->mark.ستون + 1));
    list_append_نمبر(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    خالی
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    صحیح		i;
    dict_T	*d;

    dict_جوڑو_نمبر(retdict, "length", wp->w_tagstacklen);
    dict_جوڑو_نمبر(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_اشیاء);
    اگر (l == تہی)
	واپس;
    dict_جوڑو_list(retdict, "items", l);

    برائے (i = 0; i < wp->w_tagstacklen; i++)
    {
	اگر ((d = dict_alloc_id(aid_tagstack_details)) == تہی)
	    واپس;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    واپس;
	}

	get_لیبل_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ساکن خالی
tagstack_clear(win_T *wp)
{
    صحیح i;

    // Free the current tag stack
    برائے (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_اندراج(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ساکن خالی
tagstack_سرکاؤ(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحیح		i;

    tagstack_clear_اندراج(&tagstack[0]);
    برائے (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ساکن خالی
tagstack_ڈالو_شے(
	win_T	*wp,
	char_u	*tagname,
	صحیح	cur_fnum,
	صحیح	cur_مطابقت,
	pos_T	mark,
	صحیح	fnum,
	char_u  *user_ڈیٹا)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحیح		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    اگر (idx >= TAGSTACKSIZE)
    {
	tagstack_سرکاؤ(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_مطابقت = cur_مطابقت;
    اگر (tagstack[idx].cur_مطابقت < 0)
	tagstack[idx].cur_مطابقت = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_ڈیٹا = user_ڈیٹا;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ساکن خالی
tagstack_ڈالو_اشیاء(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    صحیح		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_اشیاء(l, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == تہی)
	    جاری;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	اگر ((di = dict_ڈھونڈو(itemdict, (char_u *)"from", -1)) == تہی)
	    جاری;
	اگر (list2fpos(&di->di_tv, &mark, &fnum, تہی, FALSE) != OK)
	    جاری;
	اگر ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == تہی)
	    جاری;

	اگر (mark.ستون > 0)
	    mark.ستون--;
	tagstack_ڈالو_شے(wp, tagname,
		(صحیح)dict_get_نمبر(itemdict, "bufnr"),
		(صحیح)dict_get_نمبر(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ساکن خالی
tagstack_سیٹ_curidx(win_T *wp, صحیح curidx)
{
    wp->w_tagstackidx = curidx;
    اگر (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    اگر (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    صحیح
سیٹ_tagstack(win_T *wp, dict_T *d, صحیح action)
{
    dictitem_T	*di;
    list_T	*l = تہی;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    اگر (tfu_اندر_use)
    {
	emsg(_(e_cannot_modify_لیبل_ڈھیر_within_tagfunc));
	واپس FAIL;
    }
# endif

    اگر ((di = dict_ڈھونڈو(d, (char_u *)"items", -1)) != تہی)
    {
	اگر (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    واپس FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    اگر ((di = dict_ڈھونڈو(d, (char_u *)"curidx", -1)) != تہی)
	tagstack_سیٹ_curidx(wp, (صحیح)tv_get_نمبر(&di->di_tv) - 1);

    اگر (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	صحیح	tagstackidx = wp->w_tagstackidx;
	صحیح	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	جب (tagstackidx < tagstacklen)
	    tagstack_clear_اندراج(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    اگر (l != تہی)
    {
	اگر (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ڈالو_اشیاء(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    واپس OK;
}
#اگر_ختم
