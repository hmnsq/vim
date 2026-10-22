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

#ضمن "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
تعريفنوع بنية ٽيگ_اشارا
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_پڇاڙي;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_پڇاڙي;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_پڇاڙي;	// first char after command
    char_u	*ٽيگ_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    صحيح		is_etag;	// TRUE for emacs tag
#جيڪڏهن_پورو
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_پڇاڙي;	// end of tagkind
    char_u	*user_معلومات;	// user_data string
    char_u	*user_معلومات_پڇاڙي;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
تعريفنوع تعداد
{
    ٽيگ_فهرست_READ_SUCCESS = 1,
    ٽيگ_فهرست_READ_EOF,
    ٽيگ_فهرست_READ_IGNORE,
} ٽيگ_فهرست_read_حالت_T;

/*
 * States used during a tags search
 */
تعريفنوع تعداد
{
    TS_شروعات,		// at start of file
    TS_لڪيري,		// linear searching forward, till EOF
    TS_ٻٽو,		// binary searching
    TS_SKIP_پٺ,	// skipping backwards
    TS_قدم_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
تعريفنوع بنية
{
    off_T	هيٺيون_وٿي;	// offset for first char of first line that
				// could match
    off_T	مٿاهون_وٿي;	// offset of char after last line that could
				//ميلاپ
    off_T	curr_وٿي;	// Current file offset in search range
    off_T	curr_وٿي_استعمال_ٿيل; // curr_offset used when skipping back
    off_T	ميلاپ_وٿي;	// Where the binary search found a tag
    صحيح	هيٺيون_char;		// first char at low_offset
    صحيح	مٿاهون_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
تعريفنوع تعداد
{
    ٽيگ_ميلاپ_SUCCESS = 1,
    ٽيگ_ميلاپ_FAIL,
    ٽيگ_ميلاپ_STOP,
    ٽيگ_ميلاپ_ايندڙ
} tagmatch_حالت_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
تعريفنوع بنية
{
    صحيح	matchoff;		// tag match offset
    صحيح	ميلاپ_re;		// TRUE if the tag matches a regexp
    صحيح	ميلاپ_no_ic;		// TRUE if the tag matches with case
    صحيح	has_re;			// regular expression used
    صحيح	sortic;			// tags file sorted ignoring case (foldcase)
    صحيح	ترتيب_ڏيو_error;		// tags file not sorted
} findtags_ميلاپ_دليلون_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#وصف_ڪريو MT_ST_CUR	0		// static match in current file
#وصف_ڪريو MT_GL_CUR	1		// global match in current file
#وصف_ڪريو MT_GL_OTH	2		// global match in other file
#وصف_ڪريو MT_ST_OTH	3		// static match in other file
#وصف_ڪريو MT_IC_OFF	4		// add for icase match
#وصف_ڪريو MT_RE_OFF	8		// add for regexp match
#وصف_ڪريو MT_نقاب		7		// mask for printing priority
#وصف_ڪريو MT_ڳڻپ	16

ساكن حرف	*mt_نالا[MT_ڳڻپ/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#وصف_ڪريو NOTAGFILE	99		// return value for jumpto_tag
ساكن char_u	*nofile_fname = عدم;	// fname for NOTAGFILE error

ساكن فراغ taglen_advance(صحيح l);

ساكن صحيح jumpto_ٽيگ(char_u *lbuf, صحيح forceit, صحيح keep_help);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
ساكن صحيح parse_ٽيگ_سٽ(char_u *lbuf, صحيح is_etag, tagptrs_T *tagp);
#وإلا
ساكن صحيح parse_ٽيگ_سٽ(char_u *lbuf, tagptrs_T *tagp);
#جيڪڏهن_پورو
ساكن صحيح test_for_static(tagptrs_T *);
ساكن صحيح parse_ميلاپ(char_u *lbuf, tagptrs_T *tagp);
ساكن char_u *ٽيگ_ڀريل_fname(tagptrs_T *tagp);
ساكن char_u *expand_ٽيگ_fname(char_u *fname, char_u *ٽيگ_fname, صحيح expand);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
ساكن صحيح test_for_current(صحيح, char_u *, char_u *, char_u *, char_u *);
#وإلا
ساكن صحيح test_for_current(char_u *, char_u *, char_u *, char_u *);
#جيڪڏهن_پورو
ساكن صحيح ڳوليو_extra(char_u **pp);
ساكن فراغ print_ٽيگ_list(صحيح new_ٽيگ, صحيح use_tagstack, صحيح انگ_matches, char_u **matches);
#إذا defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ساكن صحيح جوڙيو_llist_ٽيگ_فهرست(char_u *ٽيگ, صحيح انگ_matches, char_u **matches);
#جيڪڏهن_پورو

ساكن char_u	*tagmatchname = عدم;	// name of last used tag

#إذا defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ساكن taggy_T ptag_داخلا = {عدم, {{0, 0, 0}, 0}, 0, 0, عدم};
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن صحيح  tfu_اندريون_use = FALSE;	    // disallow recursive call of tagfunc
ساكن callback_T tfu_cb;	    // 'tagfunc' callback function
#جيڪڏهن_پورو

// Used instead of NUL to separate tag fields in the growarrays.
#وصف_ڪريو ٽيگ_SEP 0x02

#إذا defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    حرف *
did_سيٽ_tagfunc(optset_T *دليلون)
{
    صحيح	retval;

    إذا (دليلون->os_flags & OPT_LOCAL)
	retval = option_سيٽ_callback_func(دليلون->os_newval.string,
							    &curbuf->b_tfu_cb);
    وإلا
    {
	retval = option_سيٽ_callback_func(دليلون->os_newval.string, &tfu_cb);
	إذا (retval == OK && !(دليلون->os_flags & OPT_GLOBAL))
	    سيٽ_buflocal_tfu_callback(curbuf);
    }

    أرجع retval == FAIL ? e_invalid_دليل : عدم;
}
#جيڪڏهن_پورو

#إذا defined(EXITFREE)
    فراغ
free_tagfunc_option(فراغ)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    صحيح
سيٽ_ref_اندريون_tagfunc(صحيح copyID اڻ_استعمال_ٿيل)
{
    صحيح	abort = FALSE;

    abort = سيٽ_ref_اندريون_callback(&tfu_cb, copyID);

    أرجع abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    فراغ
سيٽ_buflocal_tfu_callback(buf_T *buf اڻ_استعمال_ٿيل)
{
    free_callback(&buf->b_tfu_cb);
    إذا (tfu_cb.cb_name != عدم && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#جيڪڏهن_پورو

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
    صحيح
do_ٽيگ(
    char_u	*ٽيگ,		// tag (pattern) to jump to
    صحيح		type,
    صحيح		ڳڻپ,
    صحيح		forceit,	// :ta with !
    صحيح		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحيح		tagstackidx = curwin->w_tagstackidx;
    صحيح		tagstacklen = curwin->w_tagstacklen;
    صحيح		cur_ميلاپ = 0;
    صحيح		cur_fnum = curbuf->b_fnum;
    صحيح		oldtagstackidx = tagstackidx;
    صحيح		prevtagstackidx = tagstackidx;
    صحيح		prev_انگ_matches;
    صحيح		new_ٽيگ = FALSE;
    صحيح		i;
    صحيح		ic;
    صحيح		no_regexp = FALSE;
    صحيح		error_cur_ميلاپ = 0;
    صحيح		save_pos = FALSE;
    fmark_T	saved_fmark;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح		jumped_منزل_ٽيگ = FALSE;
#جيڪڏهن_پورو
    صحيح		new_انگ_matches;
    char_u	**new_matches;
    صحيح		use_tagstack;
    صحيح		skip_نياپو = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    صحيح		use_tfu = 1;
    char_u	*tofree = عدم;

    // remember the matches for the last used tag
    ساكن صحيح		انگ_matches = 0;
    ساكن صحيح		max_انگ_matches = 0;  // limit used for match search
    ساكن char_u	**matches = عدم;
    ساكن صحيح		flags;

#جيڪڏهن_وصف_ٿيل EXITFREE
    إذا (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(انگ_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_ٽيگ_فهرست();
# endif
	انگ_matches = 0;
	أرجع FALSE;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (tfu_اندريون_use)
    {
	emsg(_(e_cannot_modify_ٽيگ_ڍير_within_tagfunc));
	أرجع FALSE;
    }
#جيڪڏهن_پورو

    إذا (postponed_ورهايو == 0 && !check_can_سيٽ_curbuf_forceit(forceit))
	أرجع FALSE;

    إذا (type == DT_HELP)
    {
	type = DT_ٽيگ;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_انگ_matches = انگ_matches;
    free_string_option(nofile_fname);
    nofile_fname = عدم;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    إذا ((!p_tgst && *ٽيگ != NUL))
    {
	use_tagstack = FALSE;
	new_ٽيگ = TRUE;
#إذا defined(FEAT_QUICKFIX)
	إذا (g_do_tagpreview != 0)
	{
	    tagstack_clear_داخلا(&ptag_داخلا);
	    إذا ((ptag_داخلا.tagname = vim_strsave(ٽيگ)) == عدم)
		اذهب پڇاڙي_do_ٽيگ;
	}
#جيڪڏهن_پورو
    }
    وإلا
    {
#إذا defined(FEAT_QUICKFIX)
	إذا (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	وإلا
#جيڪڏهن_پورو
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	إذا (*ٽيگ != NUL
		&& (type == DT_ٽيگ || type == DT_SELECT || type == DT_JUMP
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
		    || type == DT_LTAG
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		    || type == DT_CSCOPE
#جيڪڏهن_پورو
		    ))
	{
#إذا defined(FEAT_QUICKFIX)
	    إذا (g_do_tagpreview != 0)
	    {
		إذا (ptag_داخلا.tagname != عدم
			&& STRCMP(ptag_داخلا.tagname, ٽيگ) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_ميلاپ = ptag_داخلا.cur_ميلاپ;
		    cur_fnum = ptag_داخلا.cur_fnum;
		}
		وإلا
		{
		    tagstack_clear_داخلا(&ptag_داخلا);
		    إذا ((ptag_داخلا.tagname = vim_strsave(ٽيگ)) == عدم)
			اذهب پڇاڙي_do_ٽيگ;
		}
	    }
	    وإلا
#جيڪڏهن_پورو
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		طالما (tagstackidx < tagstacklen)
		    tagstack_clear_داخلا(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		إذا (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_داخلا(&tagstack[0]);
		    لكل (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_معلومات = عدم;
		}

		/*
		 * put the tag name in the tag stack
		 */
		إذا ((tagstack[tagstackidx].tagname = vim_strsave(ٽيگ)) == عدم)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    اذهب پڇاڙي_do_ٽيگ;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_ٽيگ = TRUE;
	}
	وإلا
	{
	    إذا (
#إذا defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_داخلا.tagname == عدم :
#جيڪڏهن_پورو
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_ٽيگ_ڍير_خالي_جايون));
		اذهب پڇاڙي_do_ٽيگ;
	    }

	    إذا (type == DT_ڪڍو)		// go to older position
	    {
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		صحيح	old_ڪنجي_Typed = ڪنجي_Typed;
#جيڪڏهن_پورو
		إذا ((tagstackidx -= ڳڻپ) < 0)
		{
		    emsg(_(e_at_bottom_جو_ٽيگ_ڍير));
		    إذا (tagstackidx + ڳڻپ == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			اذهب پڇاڙي_do_ٽيگ;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		وإلا إذا (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_چوٽي_جو_ٽيگ_ڍير));
		    اذهب پڇاڙي_do_ٽيگ;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		إذا (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    إذا (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			اذهب پڇاڙي_do_ٽيگ;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		وإلا
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.کانو = saved_fmark.mark.کانو;
		curwin->w_سيٽ_curswant = true;
		check_cursor();
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		إذا ((fdo_flags & FDO_ٽيگ) && old_ڪنجي_Typed)
		    foldOpenCursor();
#جيڪڏهن_پورو

		// remove the old list of matches
		FreeWild(انگ_matches, matches);
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		cs_free_ٽيگ_فهرست();
#جيڪڏهن_پورو
		انگ_matches = 0;
		ٽيگ_freematch();
		اذهب پڇاڙي_do_ٽيگ;
	    }

	    إذا (type == DT_ٽيگ
#إذا defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#جيڪڏهن_پورو
	       )
	    {
#إذا defined(FEAT_QUICKFIX)
		إذا (g_do_tagpreview != 0)
		{
		    cur_ميلاپ = ptag_داخلا.cur_ميلاپ;
		    cur_fnum = ptag_داخلا.cur_fnum;
		}
		وإلا
#جيڪڏهن_پورو
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    إذا ((tagstackidx += ڳڻپ - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_چوٽي_جو_ٽيگ_ڍير));
			save_pos = FALSE;
		    }
		    وإلا إذا (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_جو_ٽيگ_ڍير));
			tagstackidx = 0;
			اذهب پڇاڙي_do_ٽيگ;
		    }
		    cur_ميلاپ = tagstack[tagstackidx].cur_ميلاپ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_ٽيگ = TRUE;
	    }
	    وإلا				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#إذا defined(FEAT_QUICKFIX)
		إذا (g_do_tagpreview != 0)
		{
		    cur_ميلاپ = ptag_داخلا.cur_ميلاپ;
		    cur_fnum = ptag_داخلا.cur_fnum;
		}
		وإلا
#جيڪڏهن_پورو
		{
		    إذا (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_ميلاپ = tagstack[tagstackidx].cur_ميلاپ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		بدل (type)
		{
		    حالة DT_پهريون: cur_ميلاپ = ڳڻپ - 1; اكسر;
		    حالة DT_SELECT:
		    حالة DT_JUMP:
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		    حالة DT_CSCOPE:
#جيڪڏهن_پورو
		    حالة DT_آخري:  cur_ميلاپ = MAXCOL - 1; اكسر;
		    حالة DT_ايندڙ:  cur_ميلاپ += ڳڻپ; اكسر;
		    حالة DT_PREV:  cur_ميلاپ -= ڳڻپ; اكسر;
		}
		إذا (cur_ميلاپ >= MAXCOL)
		    cur_ميلاپ = MAXCOL - 1;
		وإلا إذا (cur_ميلاپ < 0)
		{
		    emsg(_(e_cannot_go_before_پهريون_matching_ٽيگ));
		    skip_نياپو = TRUE;
		    cur_ميلاپ = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#إذا defined(FEAT_QUICKFIX)
	إذا (g_do_tagpreview != 0)
	{
	    إذا (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_داخلا.cur_ميلاپ = cur_ميلاپ;
		ptag_داخلا.cur_fnum = cur_fnum;
	    }
	}
	وإلا
#جيڪڏهن_پورو
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    إذا (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    إذا (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_ميلاپ = cur_ميلاپ;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    إذا (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	إذا (buf != عدم)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    لكل (;;)
    {
	صحيح	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	إذا (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#إذا defined(FEAT_QUICKFIX)
	وإلا إذا (g_do_tagpreview != 0)
	    name = ptag_داخلا.tagname;
#جيڪڏهن_پورو
	وإلا
	    name = ٽيگ;
	other_name = (tagmatchname == عدم || STRCMP(tagmatchname, name) != 0);
	إذا (new_ٽيگ
		|| (cur_ميلاپ >= انگ_matches && max_انگ_matches != MAXCOL)
		|| other_name)
	{
	    إذا (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    إذا (type == DT_SELECT || type == DT_JUMP
#إذا defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#جيڪڏهن_پورو
		)
		cur_ميلاپ = MAXCOL - 1;
	    إذا (type == DT_ٽيگ)
		max_انگ_matches = MAXCOL;
	    وإلا
		max_انگ_matches = cur_ميلاپ + 1;

	    // when the argument starts with '/', use it as a regexp
	    إذا (!no_regexp && *name == '/')
	    {
		flags = ٽيگ_REGEXP;
		++name;
	    }
	    وإلا
		flags = ٽيگ_NOIC;

#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	    إذا (type == DT_CSCOPE)
		flags = ٽيگ_CSCOPE;
#جيڪڏهن_پورو
	    إذا (verbose)
		flags |= ٽيگ_VERBOSE;

	    إذا (!use_tfu)
		flags |= ٽيگ_NO_TAGFUNC;

	    إذا (ڳوليو_ٽيگ_فهرست(name, &new_انگ_matches, &new_matches, flags,
					    max_انگ_matches, buf_ffname) == OK
		    && new_انگ_matches < max_انگ_matches)
		max_انگ_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    إذا (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_دري_unexpectedly_close_while_searching_for_ٽيگ_فهرست));
		FreeWild(new_انگ_matches, new_matches);
		اكسر;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    إذا (!new_ٽيگ && !other_name)
	    {
		صحيح	    j, k;
		صحيح	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		لكل (j = 0; j < انگ_matches; ++j)
		{
		    parse_ميلاپ(matches[j], &tagp);
		    لكل (i = idx; i < new_انگ_matches; ++i)
		    {
			parse_ميلاپ(new_matches[i], &tagp2);
			إذا (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    لكل (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    اكسر;
			}
		    }
		}
	    }
	    FreeWild(انگ_matches, matches);
	    انگ_matches = new_انگ_matches;
	    matches = new_matches;
	}

	إذا (انگ_matches <= 0)
	{
	    إذا (verbose)
		semsg(_(e_ٽيگ_not_مليو_str), name);
#إذا defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#جيڪڏهن_پورو
	}
	وإلا
	{
	    صحيح ask_for_selection = FALSE;
#إذا defined(FEAT_EVAL)
	    size_t  IObufflen;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	    إذا (type == DT_CSCOPE && انگ_matches > 1)
	    {
		cs_print_ٽيگ_فهرست();
		ask_for_selection = TRUE;
	    }
	    وإلا
#جيڪڏهن_پورو
	    إذا (type == DT_ٽيگ && *ٽيگ != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_ميلاپ = ڳڻپ > 0 ? ڳڻپ - 1 : 0;
	    وإلا إذا (type == DT_SELECT || (type == DT_JUMP && انگ_matches > 1))
	    {
		print_ٽيگ_list(new_ٽيگ, use_tagstack, انگ_matches, matches);
		ask_for_selection = TRUE;
	    }
#إذا defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    وإلا إذا (type == DT_LTAG)
	    {
		إذا (جوڙيو_llist_ٽيگ_فهرست(ٽيگ, انگ_matches, matches) == FAIL)
		    اذهب پڇاڙي_do_ٽيگ;
		cur_ميلاپ = 0;		// Jump to the first tag
	    }
#جيڪڏهن_پورو

	    إذا (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_عدد(عدم);
		إذا (i <= 0 || i > انگ_matches || got_int)
		{
		    // no valid choice: don't change anything
		    إذا (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		    cs_free_ٽيگ_فهرست();
		    jumped_منزل_ٽيگ = TRUE;
#جيڪڏهن_پورو
		    اكسر;
		}
		cur_ميلاپ = i - 1;
	    }

	    إذا (cur_ميلاپ >= انگ_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		إذا ((type == DT_ايندڙ || type == DT_پهريون)
						      && nofile_fname == عدم)
		{
		    إذا (انگ_matches == 1)
			emsg(_(e_there_is_only_one_matching_ٽيگ));
		    وإلا
			emsg(_(e_cannot_go_beyond_آخري_matching_ٽيگ));
		    skip_نياپو = TRUE;
		}
		cur_ميلاپ = انگ_matches - 1;
	    }
	    إذا (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_ميلاپ = cur_ميلاپ;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		إذا (use_tfu && parse_ميلاپ(matches[cur_ميلاپ], &tagp) == OK
			&& tagp.user_معلومات)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_معلومات);
		    tagstack[tagstackidx].user_معلومات = vim_strnsave(
			  tagp.user_معلومات, tagp.user_معلومات_پڇاڙي - tagp.user_معلومات);
		}

		++tagstackidx;
	    }
#إذا defined(FEAT_QUICKFIX)
	    وإلا إذا (g_do_tagpreview != 0)
	    {
		ptag_داخلا.cur_ميلاپ = cur_ميلاپ;
		ptag_داخلا.cur_fnum = cur_fnum;
	    }
#جيڪڏهن_پورو

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    إذا (nofile_fname != عدم && error_cur_ميلاپ != cur_ميلاپ)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_ميلاپ][0] & MT_IC_OFF);
	    إذا (type != DT_ٽيگ && type != DT_SELECT && type != DT_JUMP
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		&& type != DT_CSCOPE
#جيڪڏهن_پورو
		&& (انگ_matches > 1 || ic)
		&& !skip_نياپو)
	    {
		// Give an indication of the number of matching tags
		sprintf((حرف *)IObuff, _("tag %d of %d%s"),
				cur_ميلاپ + 1,
				انگ_matches,
				max_انگ_matches != MAXCOL ? _(" or more") : "");
		إذا (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		إذا ((انگ_matches > prev_انگ_matches || new_ٽيگ)
							   && انگ_matches > 1)
		{
		    إذا (ic)
			نياپو_attr((حرف *)IObuff, HL_ATTR(HLF_W));
		    وإلا
			نياپو((حرف *)IObuff);
		    نياپو_scroll = TRUE;	// don't overwrite this message
		}
		وإلا
		    give_warning(IObuff, ic);
		إذا (ic && !نياپو_scrolled && نياپو_silent == 0)
		{
		    ٻاهريون_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#إذا defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE, ":ta %s\r", name);
	    سيٽ_vim_var_string(VV_SWAPCOMMAND, IObuff, (صحيح)IObufflen);
#جيڪڏهن_پورو

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_ٽيگ(matches[cur_ميلاپ], forceit, type != DT_CSCOPE);

#إذا defined(FEAT_EVAL)
	    سيٽ_vim_var_string(VV_SWAPCOMMAND, عدم, -1);
#جيڪڏهن_پورو

	    إذا (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		إذا ((type == DT_PREV && cur_ميلاپ > 0)
			|| ((type == DT_ٽيگ || type == DT_ايندڙ
							  || type == DT_پهريون)
			    && (max_انگ_matches != MAXCOL
					     || cur_ميلاپ < انگ_matches - 1)))
		{
		    error_cur_ميلاپ = cur_ميلاپ;
		    إذا (use_tagstack)
			--tagstackidx;
		    إذا (type == DT_PREV)
			--cur_ميلاپ;
		    وإلا
		    {
			type = DT_ايندڙ;
			++cur_ميلاپ;
		    }
		    استمر;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    وإلا
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		إذا (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		jumped_منزل_ٽيگ = TRUE;
#جيڪڏهن_پورو
	    }
	}
	اكسر;
    }

پڇاڙي_do_ٽيگ:
    // Only store the new index when using the tagstack and it's valid.
    إذا (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_ورهايو = 0;	// don't split next time
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#جيڪڏهن_پورو

    vim_free(tofree);
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    أرجع jumped_منزل_ٽيگ;
#وإلا
    أرجع FALSE;
#جيڪڏهن_پورو
}

/*
 * List all the matching tags.
 */
    ساكن فراغ
print_ٽيگ_list(
    صحيح		new_ٽيگ,
    صحيح		use_tagstack,
    صحيح		انگ_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحيح		tagstackidx = curwin->w_tagstackidx;
    صحيح		i;
    char_u	*p;
    char_u	*command_پڇاڙي;
    tagptrs_T	tagp;
    صحيح		taglen;
    صحيح		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_ميلاپ(matches[0], &tagp);
    taglen = (صحيح)(tagp.tagname_پڇاڙي - tagp.tagname + 2);
    إذا (taglen < 18)
	taglen = 18;
    إذا (taglen > cmdline_ويڪر - 25)
	taglen = MAXCOL;
    إذا (نياپو_کانو == 0)
	نياپو_didout = FALSE;	// overwrite previous message
    نياپو_شروعات();
    نياپو_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    نياپو_clr_eos();
    taglen_advance(taglen);
    نياپو_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    لكل (i = 0; i < انگ_matches && !got_int; ++i)
    {
	parse_ميلاپ(matches[i], &tagp);
	إذا (!new_ٽيگ && (
#إذا defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_داخلا.cur_ميلاپ) ||
#جيڪڏهن_پورو
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_ميلاپ)))
	    *IObuff = '>';
	وإلا
	    *IObuff = ' ';
	vim_snprintf((حرف *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_نالا[matches[i][0] & MT_نقاب]);
	نياپو_puts((حرف *)IObuff);
	إذا (tagp.tagkind != عدم)
	    نياپو_outtrans_len(tagp.tagkind,
			  (صحيح)(tagp.tagkind_پڇاڙي - tagp.tagkind));
	نياپو_advance(13);
	نياپو_outtrans_len_attr(tagp.tagname,
			   (صحيح)(tagp.tagname_پڇاڙي - tagp.tagname),
						  HL_ATTR(HLF_T));
	نياپو_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = ٽيگ_ڀريل_fname(&tagp);
	إذا (p != عدم)
	{
	    نياپو_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	إذا (نياپو_کانو > 0)
	    نياپو_putchar('\n');
	إذا (got_int)
	    اكسر;
	نياپو_advance(15);

	// print any extra fields
	command_پڇاڙي = tagp.command_پڇاڙي;
	إذا (command_پڇاڙي != عدم)
	{
	    p = command_پڇاڙي + 3;
	    طالما (*p && *p != '\r' && *p != '\n')
	    {
		طالما (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		إذا (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    استمر;
		}
		// skip "kind:<kind>" and "<kind>"
		إذا (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_پڇاڙي;
		    استمر;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		طالما (*p && *p != '\r' && *p != '\n')
		{
		    إذا (نياپو_کانو + ptr2cells(p) >= cmdline_ويڪر)
		    {
			نياپو_putchar('\n');
			إذا (got_int)
			    اكسر;
			نياپو_advance(15);
		    }
		    p = نياپو_outtrans_one(p, attr);
		    إذا (*p == TAB)
		    {
			نياپو_puts_attr(" ", attr);
			اكسر;
		    }
		    إذا (*p == ':')
			attr = 0;
		}
	    }
	    إذا (نياپو_کانو > 15)
	    {
		نياپو_putchar('\n');
		إذا (got_int)
		    اكسر;
		نياپو_advance(15);
	    }
	}
	وإلا
	{
	    لكل (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_پڇاڙي = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	إذا (*p == '/' || *p == '?')
	{
	    ++p;
	    إذا (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	طالما (p != command_پڇاڙي && vim_isspace(*p))
	    ++p;

	طالما (p != command_پڇاڙي)
	{
	    إذا (نياپو_کانو + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_ويڪر)
		نياپو_putchar('\n');
	    إذا (got_int)
		اكسر;
	    نياپو_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    إذا (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    إذا (*p == TAB)
	    {
		نياپو_putchar(' ');
		++p;
	    }
	    وإلا
		p = نياپو_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    إذا (p == command_پڇاڙي - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		اكسر;
	    // don't display matching '/' or '?'
	    إذا (p == command_پڇاڙي - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		اكسر;
	}
	إذا (نياپو_کانو)
	    نياپو_putchar('\n');
	ui_breakcheck();
    }
    إذا (got_int)
	got_int = FALSE;	// only stop the listing
}

#إذا defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ساكن صحيح
جوڙيو_llist_ٽيگ_فهرست(
    char_u	*ٽيگ,
    صحيح		انگ_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	ٽيگ_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    صحيح		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    إذا (list == عدم || fname == عدم || cmd == عدم)
    {
	vim_free(cmd);
	vim_free(fname);
	إذا (list != عدم)
	    list_free(list);
	أرجع FAIL;
    }

    لكل (i = 0; i < انگ_matches; ++i)
    {
	صحيح	    len, cmd_len;
	ڊگهو    lnum;
	dict_T  *dict;

	parse_ميلاپ(matches[i], &tagp);

	// Save the tag name
	len = (صحيح)(tagp.tagname_پڇاڙي - tagp.tagname);
	إذا (len > 128)
	    len = 128;
	vim_strncpy(ٽيگ_name, tagp.tagname, len);
	ٽيگ_name[len] = NUL;

	// Save the tag file name
	p = ٽيگ_ڀريل_fname(&tagp);
	إذا (p == عدم)
	    استمر;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	إذا (محفوظ_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((حرف *)tagp.command);
	وإلا
	{
	    char_u *cmd_شروعات, *cmd_پڇاڙي;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_شروعات = tagp.command;
	    cmd_پڇاڙي = tagp.command_پڇاڙي;
	    إذا (cmd_پڇاڙي == عدم)
	    {
		لكل (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_پڇاڙي = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_پڇاڙي--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    إذا (*cmd_شروعات == '/' || *cmd_شروعات == '?')
		cmd_شروعات++;

	    إذا (*cmd_پڇاڙي == '/' || *cmd_پڇاڙي == '?')
		cmd_پڇاڙي--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    إذا (*cmd_شروعات == '^')
	    {
		STRCPY(cmd, "^");
		cmd_شروعات++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (صحيح)(cmd_پڇاڙي - cmd_شروعات + 1);
	    إذا (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_شروعات, cmd_len);
	    len += cmd_len;

	    إذا (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	إذا ((dict = dict_alloc()) == عدم)
	    استمر;
	إذا (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    استمر;
	}

	dict_جوڙيو_string(dict, "text", ٽيگ_name);
	dict_جوڙيو_string(dict, "filename", fname);
	dict_جوڙيو_عدد(dict, "lnum", lnum);
	إذا (lnum == 0)
	    dict_جوڙيو_string(dict, "pattern", cmd);
    }

    vim_snprintf((حرف *)IObuff, IOSIZE, "ltag %s", ٽيگ);
    سيٽ_errorlist(curwin, list, ' ', IObuff, عدم);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Free cached tags.
 */
    فراغ
ٽيگ_freematch(فراغ)
{
    VIM_CLEAR(tagmatchname);
}

    ساكن فراغ
taglen_advance(صحيح l)
{
    إذا (l == MAXCOL)
    {
	نياپو_putchar('\n');
	نياپو_advance(24);
    }
    وإلا
	نياپو_advance(13 + l);
}

/*
 * Print the tag stack
 */
    فراغ
do_ٽيگ_فهرست(exarg_T *eap اڻ_استعمال_ٿيل)
{
    صحيح		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    صحيح		tagstackidx = curwin->w_tagstackidx;
    صحيح		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    نياپو_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    لكل (i = 0; i < tagstacklen; ++i)
    {
	إذا (tagstack[i].tagname != عدم)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    إذا (name == عدم)	    // file name not available
		استمر;

	    نياپو_putchar('\n');
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_ميلاپ + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    نياپو_outtrans(IObuff);
	    نياپو_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	ٻاهريون_flush();		    // show one line at a time
    }
    إذا (tagstackidx == tagstacklen)	// idx at top of stack
	نياپو_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ساكن صحيح
ٽيگ_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    صحيح		i;

    طالما (len > 0)
    {
	i = (صحيح)TOUPPER_ASC(*s1) - (صحيح)TOUPPER_ASC(*s2);
	إذا (i != 0)
	    أرجع i;			// this character different
	إذا (*s1 == NUL)
	    اكسر;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    أرجع 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
تعريفنوع بنية
{
    char_u	*pat;		// the pattern
    صحيح		len;		// length of pat[]
    char_u	*سر;		// start of pattern head
    صحيح		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ساكن فراغ
prepare_pats(pat_T *pats, صحيح has_re)
{
    pats->سر = pats->pat;
    pats->headlen = pats->len;
    إذا (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	إذا (pats->pat[0] == '^')
	    pats->سر = pats->pat + 1;
	وإلا إذا (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->سر = pats->pat + 2;
	إذا (pats->سر == pats->pat)
	    pats->headlen = 0;
	وإلا
	    لكل (pats->headlen = 0; pats->سر[pats->headlen] != NUL;
							      ++pats->headlen)
		إذا (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->سر[pats->headlen]) != عدم)
		    اكسر;
	إذا (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    إذا (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    وإلا
	pats->regmatch.regprog = عدم;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
تعريفنوع تعداد {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_پتو,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ساكن tagcmd_T
tagfunc_cmd_قسم(char_u *cmd)
{
    إذا (VIM_ISDIGIT(*cmd))
	أرجع *skipdigits(cmd) == NUL ? TAGCMD_پتو : TAGCMD_INVALID;
    إذا (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*پڇاڙي = skip_regexp(cmd + 1, *cmd, FALSE);

	أرجع (*پڇاڙي == *cmd && پڇاڙي[1] == NUL)
					? TAGCMD_پتو : TAGCMD_INVALID;
    }
    إذا (vim_strpbrk(cmd, (char_u *)"\t\r\n") != عدم)
	أرجع TAGCMD_INVALID;
    أرجع TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ساكن صحيح
ڳوليو_tagfunc_ٽيگ_فهرست(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    صحيح		*ميلاپ_ڳڻپ,	// here the number of tags found will be placed
    صحيح		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *شي;
    صحيح		ntags = 0;
    صحيح		نتيجو = FAIL;
    typval_T	دليلون[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*ٽيگ = &curwin->w_tagstack[curwin->w_tagstackidx];

    إذا (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == عدم
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	أرجع FAIL;

    دليلون[0].v_type = VAR_STRING;
    دليلون[0].vval.v_string = pat;
    دليلون[1].v_type = VAR_STRING;
    دليلون[1].vval.v_string = flagString;

    // create 'info' dict argument
    إذا ((d = dict_alloc_تالو(VAR_FIXED)) == عدم)
	أرجع FAIL;
    إذا (!(flags & ٽيگ_INS_COMP) && ٽيگ->user_معلومات != عدم)
	dict_جوڙيو_string(d, "user_data", ٽيگ->user_معلومات);
    إذا (buf_ffname != عدم)
	dict_جوڙيو_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    دليلون[2].v_type = VAR_DICT;
    دليلون[2].vval.v_dict = d;

    دليلون[3].v_type = VAR_UNKNOWN;

    vim_snprintf((حرف *)flagString, حجم(flagString),
		 "%s%s%s",
		 g_ٽيگ_at_cursor      ? "c": "",
		 flags & ٽيگ_INS_COMP ? "i": "",
		 flags & ٽيگ_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    نتيجو = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, دليلون);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    إذا (نتيجو == FAIL)
	أرجع FAIL;
    إذا (rettv.v_type == VAR_SPECIAL && rettv.vval.v_عدد == VVAL_NULL)
    {
	clear_tv(&rettv);
	أرجع NOTDONE;
    }
    إذا (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_قدر_from_tagfunc));
	أرجع FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_شيون(taglist, شي)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_قسم;
	صحيح		len;
	dict_iterator_T	iter;
	char_u		*dict_ڪنجي;
	typval_T	*tv;
	صحيح		has_extra = 0;
	صحيح		name_only = flags & ٽيگ_نالا;

	إذا (شي->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    اكسر;
	}

# ifdef FEAT_EMACS_ٽيگ_فهرست
	len = 3;
# وإلا
	len = 2;
# endif
	res_name = عدم;
	res_fname = عدم;
	res_cmd = عدم;
	res_قسم = عدم;

	dict_iterate_شروعات(&شي->li_tv, &iter);
	طالما (عدم != (dict_ڪنجي = dict_iterate_ايندڙ(&iter, &tv)))
	{
	    إذا (tv->v_type != VAR_STRING || tv->vval.v_string == عدم)
		استمر;

	    len += (صحيح)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    إذا (STRCMP(dict_ڪنجي, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		استمر;
	    }
	    إذا (STRCMP(dict_ڪنجي, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		استمر;
	    }
	    إذا (STRCMP(dict_ڪنجي, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		استمر;
	    }
	    has_extra = 1;
	    إذا (STRCMP(dict_ڪنجي, "kind") == 0)
	    {
		res_قسم = tv->vval.v_string;
		استمر;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (صحيح)STRLEN(dict_ڪنجي) + 1;
	}

	إذا (has_extra)
	    len += 2;	// need space for ;"

	إذا (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    اكسر;
	}

	tagcmd_T cmdkind = tagfunc_cmd_قسم(res_cmd);
	إذا (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_قدر_from_tagfunc));
	    اكسر;
	}
	إذا (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	إذا (name_only)
	    mfp = vim_strsave(res_name);
	وإلا
	    mfp = alloc(حجم(char_u) + len + 1);

	إذا (mfp == عدم)
	    استمر;

	إذا (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = ٽيگ_SEP;	    // no tag file name
# ifdef FEAT_EMACS_ٽيگ_فهرست
	    *p++ = ٽيگ_SEP;
# endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    إذا (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    إذا (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		إذا (res_قسم)
		{
		    *p++ = TAB;
		    STRCPY(p, res_قسم);
		    p += STRLEN(p);
		}

		dict_iterate_شروعات(&شي->li_tv, &iter);
		طالما (عدم != (dict_ڪنجي = dict_iterate_ايندڙ(&iter, &tv)))
		{
		    إذا (tv->v_type != VAR_STRING || tv->vval.v_string == عدم)
			استمر;

		    إذا (STRCMP(dict_ڪنجي, "name") == 0)
			استمر;
		    إذا (STRCMP(dict_ڪنجي, "filename") == 0)
			استمر;
		    إذا (STRCMP(dict_ڪنجي, "cmd") == 0)
			استمر;
		    إذا (STRCMP(dict_ڪنجي, "kind") == 0)
			استمر;

		    *p++ = TAB;
		    STRCPY(p, dict_ڪنجي);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	إذا (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_معلومات))[ga->ga_len++] = mfp;
	    ++ntags;
	    نتيجو = OK;
	}
	وإلا
	{
	    vim_free(mfp);
	    اكسر;
	}
    }

    clear_tv(&rettv);

    *ميلاپ_ڳڻپ = ntags;
    أرجع نتيجو;
}
#جيڪڏهن_پورو

/*
 * State information used during a tag search
 */
تعريفنوع بنية
{
    tagsearch_state_T	state;		// tag search state
    صحيح		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    صحيح		lbuf_سائيز;		// length of lbuf
    char_u	*ٽيگ_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    صحيح		flags;			// flags used for tag search
    صحيح		ٽيگ_file_sorted;	// !_TAG_FILE_SORTED value
    صحيح		get_searchpat;		// used for 'showfulltag'
    صحيح		help_only;		// only search for help tags
    صحيح		did_open;		// did open a tag file
    صحيح		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    صحيح		لڪيري;			// do a linear search
    vimconv_T	vimconv;
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    صحيح		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    صحيح		help_pri;		// help language priority
    char_u	*help_lang_ڳوليو;	// lang to be found
    صحيح		is_txt;			// flag of file extension
#جيڪڏهن_پورو
    صحيح		ميلاپ_ڳڻپ;		// number of matches found
    garray_T	ga_ميلاپ[MT_ڳڻپ];	// stores matches in sequence
    hashtab_T	ht_ميلاپ[MT_ڳڻپ];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساكن صحيح
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    صحيح			flags,
    صحيح			mincount)
{
    صحيح		mtt;

    st->ٽيگ_fname = alloc(MAXPATHL + 1);
    st->fp = عدم;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (صحيح)STRLEN(pat);
    st->orgpat->regmatch.regprog = عدم;
    st->flags = flags;
    st->ٽيگ_file_sorted = NUL;
    st->help_only = (flags & ٽيگ_HELP);
    st->get_searchpat = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_ڳوليو = عدم;
    st->is_txt = FALSE;
#جيڪڏهن_پورو
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_سائيز = LSIZE;
    st->lbuf = alloc(st->lbuf_سائيز);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    st->ebuf = alloc(LSIZE);
#جيڪڏهن_پورو
    st->ميلاپ_ڳڻپ = 0;
    st->stop_searching = FALSE;

    لكل (mtt = 0; mtt < MT_ڳڻپ; ++mtt)
    {
	ga_init2(&st->ga_ميلاپ[mtt], حجم(char_u *), 100);
	hash_init(&st->ht_ميلاپ[mtt]);
    }

    // check for out of memory situation
    إذا (st->ٽيگ_fname == عدم
	    || st->lbuf == عدم
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	    || st->ebuf == عدم
#جيڪڏهن_پورو
       )
	أرجع FAIL;

    أرجع OK;
}

/*
 * Free the state used by find_tags()
 */
    ساكن فراغ
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->ٽيگ_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    vim_free(st->ebuf);
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    ساكن صحيح
findtags_اندريون_help_init(findtags_state_T *st)
{
    صحيح		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    إذا (st->is_txt)
	STRCPY(st->help_lang, "en");
    وإلا
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (صحيح)STRLEN(st->ٽيگ_fname);
	إذا (i > 3 && st->ٽيگ_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->ٽيگ_fname + i - 2, 2);
	وإلا
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    إذا (st->help_lang_ڳوليو != عدم
	    && STRICMP(st->help_lang, st->help_lang_ڳوليو) != 0)
	أرجع FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    إذا ((st->flags & ٽيگ_KEEP_LANG)
	    && st->help_lang_ڳوليو == عدم
	    && curbuf->b_fname != عدم
	    && (i = (صحيح)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    وإلا
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	لكل (s = p_hlg; *s != NUL; ++s)
	{
	    إذا (STRNICMP(s, st->help_lang, 2) == 0)
		اكسر;
	    ++st->help_pri;
	    إذا ((s = vim_strchr(s, ',')) == عدم)
		اكسر;
	}
	إذا (s == عدم || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    إذا (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    أرجع TRUE;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    ساكن صحيح
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    صحيح		use_tfu = ((st->flags & ٽيگ_NO_TAGFUNC) == 0);
    صحيح		retval;

    إذا (!use_tfu || tfu_اندريون_use || *curbuf->b_p_tfu == NUL)
	أرجع NOTDONE;

    tfu_اندريون_use = TRUE;
    retval = ڳوليو_tagfunc_ٽيگ_فهرست(pat, st->ga_ميلاپ, &st->ميلاپ_ڳڻپ,
						st->flags, buf_ffname);
    tfu_اندريون_use = FALSE;

    أرجع retval;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_سائيز 42
ساكن بنية
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_سائيز];
ساكن صحيح incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ساكن فراغ
emacs_ٽيگ_فهرست_incstack_free(فراغ)
{
    طالما (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = عدم;
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
    ساكن فراغ
emacs_ٽيگ_فهرست_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    إذا (vim_fgets(st->ebuf, LSIZE, st->fp))
	أرجع;

    لكل (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    إذا (*p == NUL)
	أرجع;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    إذا (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_سائيز)
	أرجع;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->ٽيگ_fname);
    إذا (incstack[incstack_idx].etag_fname == عدم)
	أرجع;

    incstack[incstack_idx].fp = st->fp;
    st->fp = عدم;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_ٽيگ_fname(st->ebuf, st->ٽيگ_fname, FALSE);
    إذا (fullpath_ebuf != عدم)
    {
	st->fp = mch_fopen((حرف *)fullpath_ebuf, "r");
	إذا (st->fp != عدم)
	{
	    إذا (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_ٽيگ_file_رستو_truncated_for_str), st->ebuf);
	    vim_strncpy(st->ٽيگ_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    إذا (st->fp == عدم)
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
    ساكن صحيح
emacs_ٽيگ_فهرست_file_eof(findtags_state_T *st)
{
    إذا (!incstack_idx)	// reached end of file. stop processing.
	أرجع FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->ٽيگ_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    أرجع TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ساكن صحيح
emacs_ٽيگ_فهرست_parse_سٽ(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    إذا (p_7f == عدم)
    {
etag_fail:
	إذا (vim_strchr(lbuf, '\n') != عدم)
	    أرجع FAIL;

	// Truncated line.  Ignore it.
	إذا (p_verbose >= 5)
	{
	    verbose_enter();
	    نياپو(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_پڇاڙي = lbuf;
	أرجع OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    إذا (p == عدم)
	p = p_7f + 1;
    وإلا
	++p;

    إذا (!VIM_ISDIGIT(*p))	    // check for start of line number
	اذهب etag_fail;
    tagp->command = p;

    إذا (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_پڇاڙي = p - 1;
    }
    وإلا			    // second format: isolate tagname
    {
	إذا (p_7f == lbuf)
	    اذهب etag_fail;

	// find end of tagname
	لكل (p = p_7f - 1; !vim_iswordc(*p); --p)
	    إذا (p == lbuf)
		اذهب etag_fail;
	tagp->tagname_پڇاڙي = p + 1;
	طالما (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    ساكن ٽيگ_فهرست_read_حالت_T
findtags_get_ايندڙ_سٽ(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    صحيح		eof;
    off_T	وٿي;

    // For binary search: compute the next offset to use.
    إذا (st->state == TS_ٻٽو)
    {
	وٿي = sinfo_p->هيٺيون_وٿي + ((sinfo_p->مٿاهون_وٿي
						- sinfo_p->هيٺيون_وٿي) / 2);
	إذا (وٿي == sinfo_p->curr_وٿي)
	    أرجع ٽيگ_فهرست_READ_EOF; // End the binary search without a match.
	وإلا
	    sinfo_p->curr_وٿي = وٿي;
    }

    // Skipping back (after a match during binary search).
    وإلا إذا (st->state == TS_SKIP_پٺ)
    {
	sinfo_p->curr_وٿي -= st->lbuf_سائيز * 2;
	إذا (sinfo_p->curr_وٿي < 0)
	{
	    sinfo_p->curr_وٿي = 0;
	    rewind(st->fp);
	    st->state = TS_قدم_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    إذا (st->state == TS_ٻٽو || st->state == TS_SKIP_پٺ)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_وٿي_استعمال_ٿيل = sinfo_p->curr_وٿي;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_وٿي, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_سائيز, st->fp);
	إذا (!eof && sinfo_p->curr_وٿي != 0)
	{
	    sinfo_p->curr_وٿي = vim_ftell(st->fp);
	    إذا (sinfo_p->curr_وٿي == sinfo_p->مٿاهون_وٿي)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->هيٺيون_وٿي, SEEK_SET);
		sinfo_p->curr_وٿي = sinfo_p->هيٺيون_وٿي;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_سائيز, st->fp);
	}
	// skip empty and blank lines
	طالما (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_وٿي = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_سائيز, st->fp);
	}
	إذا (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_پٺ;
	    sinfo_p->ميلاپ_وٿي = vim_ftell(st->fp);
	    sinfo_p->curr_وٿي = sinfo_p->curr_وٿي_استعمال_ٿيل;
	    أرجع ٽيگ_فهرست_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    وإلا
    {
	// skip empty and blank lines
	افعل
	{
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	    إذا (st->flags & ٽيگ_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_سائيز);
	    وإلا
#جيڪڏهن_پورو
		eof = vim_fgets(st->lbuf, st->lbuf_سائيز, st->fp);
	} طالما (!eof && vim_isblankline(st->lbuf));

	إذا (eof)
	{
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	    إذا (emacs_ٽيگ_فهرست_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		أرجع ٽيگ_فهرست_READ_IGNORE;
#جيڪڏهن_پورو
	    أرجع ٽيگ_فهرست_READ_EOF;
	}
    }

    أرجع ٽيگ_فهرست_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ساكن صحيح
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    إذا (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	أرجع TRUE;

    // Process the header line.
    إذا (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->ٽيگ_file_sorted = st->lbuf[18];
    إذا (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	لكل (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_تياري(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    أرجع FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ساكن صحيح
findtags_شروعات_state_handler(
    findtags_state_T	*st,
    صحيح			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح		use_cscope = (st->flags & ٽيگ_CSCOPE);
#جيڪڏهن_پورو
    صحيح		noic = (st->flags & ٽيگ_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    إذا (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	أرجع findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    إذا (st->لڪيري || use_cscope)
#وإلا
    إذا (st->لڪيري)
#جيڪڏهن_پورو
	st->state = TS_لڪيري;
    وإلا إذا (st->ٽيگ_file_sorted == NUL)
	st->state = TS_ٻٽو;
    وإلا إذا (st->ٽيگ_file_sorted == '1')
	st->state = TS_ٻٽو;
    وإلا إذا (st->ٽيگ_file_sorted == '2')
    {
	st->state = TS_ٻٽو;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    وإلا
	st->state = TS_لڪيري;

    إذا (st->state == TS_ٻٽو && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->لڪيري = TRUE;
	st->state = TS_لڪيري;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    إذا (st->state == TS_ٻٽو)
    {
	إذا (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_لڪيري;
	وإلا
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->هيٺيون_وٿي = 0;
	    sinfo_p->هيٺيون_char = 0;
	    sinfo_p->مٿاهون_وٿي = filesize;
	    sinfo_p->curr_وٿي = 0;
	    sinfo_p->مٿاهون_char = 0xff;
	}
	أرجع FALSE;
    }

    أرجع TRUE;
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
    ساكن tagmatch_حالت_T
findtags_parse_سٽ(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_ميلاپ_دليلون_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    صحيح		حالت;
    صحيح		i;
    صحيح		cmplen;
    صحيح		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    إذا (st->orgpat->headlen
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	    && !st->is_etag
#جيڪڏهن_پورو
       )
    {
	CLEAR_ميدان(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_پڇاڙي = vim_strchr(st->lbuf, TAB);
	إذا (tagpp->tagname_پڇاڙي == عدم)
	    // Corrupted tag line.
	    أرجع ٽيگ_ميلاپ_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (صحيح)(tagpp->tagname_پڇاڙي - tagpp->tagname);
	إذا (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	إذا ((st->flags & ٽيگ_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	وإلا إذا (st->state == TS_لڪيري && st->orgpat->headlen != cmplen)
	    أرجع ٽيگ_ميلاپ_ايندڙ;

	إذا (st->state == TS_ٻٽو)
	{
	    // Simplistic check for unsorted tags file.
	    i = (صحيح)tagpp->tagname[0];
	    إذا (margs->sortic)
		i = (صحيح)TOUPPER_ASC(tagpp->tagname[0]);
	    إذا (i < sinfo_p->هيٺيون_char || i > sinfo_p->مٿاهون_char)
		margs->ترتيب_ڏيو_error = TRUE;

	    // Compare the current tag with the searched tag.
	    إذا (margs->sortic)
		tagcmp = ٽيگ_strnicmp(tagpp->tagname, st->orgpat->سر,
							(size_t)cmplen);
	    وإلا
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->سر, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    إذا (tagcmp == 0)
	    {
		إذا (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		وإلا إذا (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    إذا (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_پٺ;
		sinfo_p->ميلاپ_وٿي = sinfo_p->curr_وٿي;
		أرجع ٽيگ_ميلاپ_ايندڙ;
	    }
	    إذا (tagcmp < 0)
	    {
		sinfo_p->curr_وٿي = vim_ftell(st->fp);
		إذا (sinfo_p->curr_وٿي < sinfo_p->مٿاهون_وٿي)
		{
		    sinfo_p->هيٺيون_وٿي = sinfo_p->curr_وٿي;
		    إذا (margs->sortic)
			sinfo_p->هيٺيون_char = TOUPPER_ASC(tagpp->tagname[0]);
		    وإلا
			sinfo_p->هيٺيون_char = tagpp->tagname[0];
		    أرجع ٽيگ_ميلاپ_ايندڙ;
		}
	    }
	    إذا (tagcmp > 0 && sinfo_p->curr_وٿي != sinfo_p->مٿاهون_وٿي)
	    {
		sinfo_p->مٿاهون_وٿي = sinfo_p->curr_وٿي;
		إذا (margs->sortic)
		    sinfo_p->مٿاهون_char = TOUPPER_ASC(tagpp->tagname[0]);
		وإلا
		    sinfo_p->مٿاهون_char = tagpp->tagname[0];
		أرجع ٽيگ_ميلاپ_ايندڙ;
	    }

	    // No match yet and are at the end of the binary search.
	    أرجع ٽيگ_ميلاپ_STOP;
	}
	وإلا إذا (st->state == TS_SKIP_پٺ)
	{
	    إذا (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
		st->state = TS_قدم_FORWARD;
	    وإلا
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_وٿي = sinfo_p->curr_وٿي_استعمال_ٿيل;
	    أرجع ٽيگ_ميلاپ_ايندڙ;
	}
	وإلا إذا (st->state == TS_قدم_FORWARD)
	{
	    إذا (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
	    {
		إذا ((off_T)vim_ftell(st->fp) > sinfo_p->ميلاپ_وٿي)
		    أرجع ٽيگ_ميلاپ_STOP;	// past last match
		وإلا
		    أرجع ٽيگ_ميلاپ_ايندڙ;	// before first match
	    }
	}
	وإلا
	    // skip this match if it can't match
	    إذا (MB_STRNICMP(tagpp->tagname, st->orgpat->سر, cmplen) != 0)
		أرجع ٽيگ_ميلاپ_ايندڙ;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_پڇاڙي + 1;
	tagpp->fname_پڇاڙي = vim_strchr(tagpp->fname, TAB);
	إذا (tagpp->fname_پڇاڙي == عدم)
	    حالت = FAIL;
	وإلا
	{
	    tagpp->command = tagpp->fname_پڇاڙي + 1;
	    حالت = OK;
	}
    }
    وإلا
	حالت = parse_ٽيگ_سٽ(st->lbuf,
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
		st->is_etag,
#جيڪڏهن_پورو
		tagpp);

    إذا (حالت == FAIL)
	أرجع ٽيگ_ميلاپ_FAIL;

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (st->is_etag)
	tagpp->fname = st->ebuf;
#جيڪڏهن_پورو

    أرجع ٽيگ_ميلاپ_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ساكن فراغ
findtags_matchargs_init(findtags_ميلاپ_دليلون_T *margs, صحيح flags)
{
    margs->matchoff = 0;			// match offset
    margs->ميلاپ_re = FALSE;			// match with regexp
    margs->ميلاپ_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & ٽيگ_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->ترتيب_ڏيو_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    ساكن صحيح
findtags_ميلاپ_ٽيگ(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ميلاپ_دليلون_T *margs)
{
    صحيح		ميلاپ = FALSE;
    صحيح		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (صحيح)(tagpp->tagname_پڇاڙي - tagpp->tagname);
    إذا (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    إذا (st->orgpat->len != cmplen)
	ميلاپ = FALSE;
    وإلا
    {
	إذا (st->orgpat->regmatch.rm_ic)
	{
	    ميلاپ =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    إذا (ميلاپ)
		margs->ميلاپ_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	وإلا
	    ميلاپ = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->ميلاپ_re = FALSE;
    إذا (!ميلاپ && st->orgpat->regmatch.regprog != عدم)
    {
	صحيح	cc;

	cc = *tagpp->tagname_پڇاڙي;
	*tagpp->tagname_پڇاڙي = NUL;
	ميلاپ = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	إذا (ميلاپ)
	{
	    margs->matchoff = (صحيح)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    إذا (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->ميلاپ_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_پڇاڙي = cc;
	margs->ميلاپ_re = TRUE;
    }

    أرجع ميلاپ;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ساكن فراغ
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_سٽ;
    صحيح		len;

    conv_سٽ = string_convert(&st->vimconv, st->lbuf, عدم);
    إذا (conv_سٽ == عدم)
	أرجع;

    // Copy or swap lbuf and conv_line.
    len = (صحيح)STRLEN(conv_سٽ) + 1;
    إذا (len > st->lbuf_سائيز)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_سٽ;
	st->lbuf_سائيز = len;
    }
    وإلا
    {
	STRCPY(st->lbuf, conv_سٽ);
	vim_free(conv_سٽ);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    ساكن صحيح
findtags_جوڙيو_ميلاپ(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ميلاپ_دليلون_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح		use_cscope = (st->flags & ٽيگ_CSCOPE);
#جيڪڏهن_پورو
    صحيح		name_only = (st->flags & ٽيگ_نالا);
    صحيح		mtt;
    صحيح		len = 0;
    صحيح		is_current;		// file name matches
    صحيح		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    إذا (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    وإلا
#جيڪڏهن_پورو
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
		st->is_etag,
#جيڪڏهن_پورو
		tagpp->fname, tagpp->fname_پڇاڙي, st->ٽيگ_fname, buf_ffname);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	is_static = FALSE;
	إذا (!st->is_etag)	// emacs tags are never static
#جيڪڏهن_پورو
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	//ميلاپ
	إذا (is_static)
	{
	    إذا (is_current)
		mtt = MT_ST_CUR;
	    وإلا
		mtt = MT_ST_OTH;
	}
	وإلا
	{
	    إذا (is_current)
		mtt = MT_GL_CUR;
	    وإلا
		mtt = MT_GL_OTH;
	}
	إذا (st->orgpat->regmatch.rm_ic && !margs->ميلاپ_no_ic)
	    mtt += MT_IC_OFF;
	إذا (margs->ميلاپ_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    إذا (st->help_only)
    {
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
# define ML_EXTRA 3
#وإلا
# define ML_EXTRA 0
#جيڪڏهن_پورو
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_پڇاڙي = NUL;
	len = (صحيح)(tagpp->tagname_پڇاڙي - tagpp->tagname);
	mfp = alloc(حجم(char_u) + len + 10 + ML_EXTRA + 1);
	إذا (mfp != عدم)
	{
	    صحيح heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#جيڪڏهن_پورو

	    heuristic = help_heuristic(tagpp->tagname,
				margs->ميلاپ_re ? margs->matchoff : 0,
				!margs->ميلاپ_no_ic);
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#جيڪڏهن_پورو
	    sprintf((حرف *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_پڇاڙي = TAB;
    }
    وإلا إذا (name_only)
    {
	إذا (st->get_searchpat)
	{
	    char_u *عارضي_پڇاڙي = tagpp->command;

	    إذا (*عارضي_پڇاڙي == '/')
		طالما (*عارضي_پڇاڙي && *عارضي_پڇاڙي != '\r'
			&& *عارضي_پڇاڙي != '\n'
			&& *عارضي_پڇاڙي != '$')
		    عارضي_پڇاڙي++;

	    إذا (tagpp->command + 2 < عارضي_پڇاڙي)
	    {
		len = (صحيح)(عارضي_پڇاڙي - tagpp->command - 2);
		mfp = alloc(len + 2);
		إذا (mfp != عدم)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    وإلا
		mfp = عدم;
	    st->get_searchpat = FALSE;
	}
	وإلا
	{
	    len = (صحيح)(tagpp->tagname_پڇاڙي - tagpp->tagname);
	    mfp = alloc(حجم(char_u) + len + 1);
	    إذا (mfp != عدم)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    إذا (State & گهڻائي_وجهو)
		st->get_searchpat = p_sft;
	}
    }
    وإلا
    {
	size_t ٽيگ_fname_len = STRLEN(st->ٽيگ_fname);
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	size_t ebuf_len = 0;
#جيڪڏهن_پورو

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (صحيح)ٽيگ_fname_len + (صحيح)STRLEN(st->lbuf) + 3;
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	إذا (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (صحيح)ebuf_len + 1;
	}
	وإلا
	    ++len;
#جيڪڏهن_پورو
	mfp = alloc(حجم(char_u) + len + 1);
	إذا (mfp != عدم)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->ٽيگ_fname);
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#جيڪڏهن_پورو
	    p[ٽيگ_fname_len + 1] = ٽيگ_SEP;
	    s = p + 1 + ٽيگ_fname_len + 1;
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	    إذا (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = ٽيگ_SEP;
		s += ebuf_len + 1;
	    }
	    وإلا
		*s++ = ٽيگ_SEP;
#جيڪڏهن_پورو
	    STRCPY(s, st->lbuf);
	}
    }

    إذا (mfp != عدم)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	إذا (use_cscope)
	    ++*hash;
	وإلا
#جيڪڏهن_پورو
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_ميلاپ[mtt], mfp, *hash);
	إذا (HASHITEM_خالي_جايون(hi))
	{
	    إذا (hash_جوڙيو_شي(&st->ht_ميلاپ[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_ميلاپ[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		أرجع FAIL;
	    }

	    ((char_u **)(st->ga_ميلاپ[mtt].ga_معلومات))
		[st->ga_ميلاپ[mtt].ga_len++] = mfp;
	    st->ميلاپ_ڳڻپ++;
	}
	وإلا
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    أرجع OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ساكن فراغ
findtags_get_all_ٽيگ_فهرست(
    findtags_state_T		*st,
    findtags_ميلاپ_دليلون_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	ڳولا_info;
    صحيح			retval;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح			use_cscope = (st->flags & ٽيگ_CSCOPE);
#جيڪڏهن_پورو
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_ميدان(ڳولا_info);

    // Read and parse the lines in the file one by one
    لكل (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	إذا (st->state == TS_ٻٽو || st->state == TS_SKIP_پٺ)
	    سٽ_breakcheck();
	وإلا
	    fast_breakcheck();
	إذا ((st->flags & ٽيگ_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ڪنجيون(30, FALSE);
	إذا (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    اكسر;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	إذا (st->mincount == ٽيگ_MANY && st->ميلاپ_ڳڻپ >= ٽيگ_MANY)
	{
	    st->stop_searching = TRUE;
	    اكسر;
	}
	إذا (st->get_searchpat)
	    اذهب سٽ_read_اندريون;

	retval = findtags_get_ايندڙ_سٽ(st, &ڳولا_info);
	إذا (retval == ٽيگ_فهرست_READ_IGNORE)
	    استمر;
	إذا (retval == ٽيگ_فهرست_READ_EOF)
	    اكسر;

سٽ_read_اندريون:

	إذا (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	إذا (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_لڪيري;
	    emacs_ٽيگ_فهرست_new_filename(st);
	    استمر;
	}
#جيڪڏهن_پورو

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	إذا (st->state == TS_شروعات)
	{
	    إذا (findtags_شروعات_state_handler(st, &margs->sortic, &ڳولا_info) == FALSE)
		استمر;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	إذا (st->lbuf[st->lbuf_سائيز - 2] != NUL
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		&& !use_cscope
#جيڪڏهن_پورو
	   )
	{
	    st->lbuf_سائيز *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_سائيز);
	    إذا (st->lbuf == عدم)
	    {
		إذا (st->fp != عدم)
		    fclose(st->fp);
		st->fp = عدم;
		st->stop_searching = TRUE;
		أرجع;
	    }

	    إذا (st->state == TS_قدم_FORWARD || st->state == TS_لڪيري)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, ڳولا_info.curr_وٿي,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    ڳولا_info.curr_وٿي = 0;
	    استمر;
	}

	retval = findtags_parse_سٽ(st, &tagp, margs, &ڳولا_info);
	إذا (retval == ٽيگ_ميلاپ_ايندڙ)
	    استمر;
	إذا (retval == ٽيگ_ميلاپ_STOP)
	    اكسر;
	إذا (retval == ٽيگ_ميلاپ_FAIL)
	{
	    semsg(_(e_format_error_اندريون_ٽيگ_فهرست_file_str), st->ٽيگ_fname);
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	    إذا (!use_cscope)
#جيڪڏهن_پورو
		semsg(_("Before byte %ld"), (ڊگهو)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    أرجع;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	إذا (findtags_ميلاپ_ٽيگ(st, &tagp, margs))
	{
	    إذا (findtags_جوڙيو_ميلاپ(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		اكسر;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ساكن فراغ
findtags_اندريون_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_ميلاپ_دليلون_T margs;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح		use_cscope = (st->flags & ٽيگ_CSCOPE);
#جيڪڏهن_پورو

    st->vimconv.vc_type = CONV_NONE;
    st->ٽيگ_file_sorted = NUL;
    st->fp = عدم;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    إذا (use_cscope)
	st->fp = عدم;	    // avoid GCC warning
    وإلا
#جيڪڏهن_پورو
    {
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	إذا (curbuf->b_help)
	{
	    إذا (!findtags_اندريون_help_init(st))
		أرجع;
	}
#جيڪڏهن_پورو

	st->fp = mch_fopen((حرف *)st->ٽيگ_fname, "r");
	إذا (st->fp == عدم)
	    أرجع;

	إذا (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->ٽيگ_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_شروعات;	// we're at the start of the file
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    st->is_etag = FALSE;	// default is: not emacs style
#جيڪڏهن_پورو

    // Read and parse the lines in the file one by one
    findtags_get_all_ٽيگ_فهرست(st, &margs, buf_ffname);

    إذا (st->fp != عدم)
    {
	fclose(st->fp);
	st->fp = عدم;
    }
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    emacs_ٽيگ_فهرست_incstack_free();
#جيڪڏهن_پورو
    إذا (st->vimconv.vc_type != CONV_NONE)
	convert_تياري(&st->vimconv, عدم, عدم);

    إذا (margs.ترتيب_ڏيو_error)
	semsg(_(e_ٽيگ_فهرست_file_not_sorted_str), st->ٽيگ_fname);

    // Stop searching if sufficient tags have been found.
    إذا (st->ميلاپ_ڳڻپ >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ساكن صحيح
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    صحيح		name_only = (st->flags & ٽيگ_نالا);
    char_u	**matches;
    صحيح		mtt;
    صحيح		i;
    char_u	*mfp;
    char_u	*p;

    إذا (st->ميلاپ_ڳڻپ > 0)
	matches = ALLOC_MULT(char_u *, st->ميلاپ_ڳڻپ);
    وإلا
	matches = عدم;
    st->ميلاپ_ڳڻپ = 0;
    لكل (mtt = 0; mtt < MT_ڳڻپ; ++mtt)
    {
	لكل (i = 0; i < st->ga_ميلاپ[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_ميلاپ[mtt].ga_معلومات))[i];
	    إذا (matches == عدم)
		vim_free(mfp);
	    وإلا
	    {
		إذا (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    لكل (p = mfp + 1; *p != NUL; ++p)
			إذا (*p == ٽيگ_SEP)
			    *p = NUL;
		}
		matches[st->ميلاپ_ڳڻپ++] = mfp;
	    }
	}

	ga_clear(&st->ga_ميلاپ[mtt]);
	hash_clear(&st->ht_ميلاپ[mtt]);
    }

    *matchesp = matches;
    أرجع st->ميلاپ_ڳڻپ;
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
    صحيح
ڳوليو_ٽيگ_فهرست(
    char_u	*pat,			// pattern to search for
    صحيح		*انگ_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    صحيح		flags,
    صحيح		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    صحيح		پهريون_file;		// trying first tag file
    صحيح		retval = FAIL;		// return value
    صحيح		ڦيرو;

    صحيح		save_emsg_off;

    bool	help_save;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    صحيح		i;
    char_u	*saved_pat = عدم;		// copy of pat[]
#جيڪڏهن_پورو

    صحيح		findall = (mincount == MAXCOL || mincount == ٽيگ_MANY);
						// find all matching tags
    صحيح		has_re = (flags & ٽيگ_REGEXP);	// regexp used
    صحيح		noic = (flags & ٽيگ_NOIC);
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    صحيح		use_cscope = (flags & ٽيگ_CSCOPE);
#جيڪڏهن_پورو
    صحيح		verbose = (flags & ٽيگ_VERBOSE);
    صحيح		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    بدل (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	حالة TC_FOLLOWIC:		 اكسر;
	حالة TC_IGNORE:    p_ic = TRUE;  اكسر;
	حالة TC_ميلاپ:     p_ic = FALSE; اكسر;
	حالة TC_FOLLOWSCS: p_ic = ignorecase(pat); اكسر;
	حالة TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); اكسر;
    }

    help_save = curbuf->b_help;

    إذا (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	اذهب findtag_پڇاڙي;

#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    STRCPY(st.ٽيگ_fname, "from cscope");	// for error messages
#جيڪڏهن_پورو

    /*
     * Initialize a few variables
     */
    إذا (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
    وإلا إذا (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    إذا (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	إذا (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    إذا (saved_pat != عدم)
	    {
		st.help_lang_ڳوليو = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#جيڪڏهن_پورو
    إذا (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    إذا (has_re && st.orgpat->regmatch.regprog == عدم)
	اذهب findtag_پڇاڙي;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    إذا (retval != NOTDONE)
	اذهب findtag_پڇاڙي;

    // re-initialize the default return value
    retval = FAIL;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    إذا ((flags & ٽيگ_KEEP_LANG)
	    && st.help_lang_ڳوليو == عدم
	    && curbuf->b_fname != عدم
	    && (i = (صحيح)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#جيڪڏهن_پورو

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
    لكل (ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
    {
	st.لڪيري = (st.orgpat->headlen == 0 || !p_tbs || ڦيرو == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      لكل (پهريون_file = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	    use_cscope ||
#جيڪڏهن_پورو
		get_tagfname(&tn, پهريون_file, st.ٽيگ_fname) == OK;
							   پهريون_file = FALSE)
      {
	  findtags_اندريون_file(&st, buf_ffname);
	  إذا (st.stop_searching
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
		  || use_cscope
#جيڪڏهن_پورو
	     )
	  {
	      retval = OK;
	      اكسر;
	  }
      } // end of for-each-file loop

#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	إذا (!use_cscope)
#جيڪڏهن_پورو
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	إذا (st.stop_searching || st.لڪيري || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
	إذا (use_cscope)
	    اكسر;
#جيڪڏهن_پورو

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    إذا (!st.stop_searching)
    {
	إذا (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_ٽيگ_فهرست_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_پڇاڙي:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    إذا (retval == FAIL)
	st.ميلاپ_ڳڻپ = 0;

    *انگ_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    vim_free(saved_pat);
#جيڪڏهن_پورو

    p_ic = save_p_ic;

    أرجع retval;
}

ساكن garray_T ٽيگ_fnames = GA_خالي_جايون;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ساكن فراغ
مليو_tagfile_cb(char_u *fname, فراغ *cookie اڻ_استعمال_ٿيل)
{
    إذا (ga_grow(&ٽيگ_fnames, 1) == FAIL)
	أرجع;

    char_u	*ٽيگ_fname = vim_strsave(fname);

#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    slash_adjust(ٽيگ_fname);
#جيڪڏهن_پورو
    simplify_filename(ٽيگ_fname);
    ((char_u **)(ٽيگ_fnames.ga_معلومات))[ٽيگ_fnames.ga_len++] = ٽيگ_fname;
}

#إذا defined(EXITFREE)
    فراغ
free_ٽيگ_stuff(فراغ)
{
    ga_clear_لڙيون(&ٽيگ_fnames);
    إذا (curwin != عدم)
	do_ٽيگ(عدم, DT_FREE, 0, 0, 0);
    ٽيگ_freematch();

# إذا defined(FEAT_QUICKFIX)
    tagstack_clear_داخلا(&ptag_داخلا);
# endif
}
#جيڪڏهن_پورو

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    صحيح
get_tagfname(
    tagname_T	*tnp,	// holds status info
    صحيح		پهريون,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = عدم;
    char_u		*r_ptr;
    صحيح			i;

    إذا (پهريون)
	CLEAR_اشارو(tnp);

    إذا (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	إذا (پهريون)
	{
	    ga_clear_لڙيون(&ٽيگ_fnames);
	    ga_init2(&ٽيگ_fnames, حجم(char_u *), 10);
	    do_اندريون_runtimepath((char_u *)
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# وإلا
		    "doc/tags doc/tags-??"
# endif
#وإلا
		    "doc/tags"
#جيڪڏهن_پورو
					   , DIP_ALL, مليو_tagfile_cb, عدم);
	}

	إذا (tnp->tn_hf_idx >= ٽيگ_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    إذا (tnp->tn_hf_idx > ٽيگ_fnames.ga_len || *p_hf == NUL)
		أرجع FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	    slash_adjust(buf);
#جيڪڏهن_پورو
	    simplify_filename(buf);

	    لكل (i = 0; i < ٽيگ_fnames.ga_len; ++i)
		إذا (STRCMP(buf, ((char_u **)(ٽيگ_fnames.ga_معلومات))[i]) == 0)
		    أرجع FAIL; // avoid duplicate file names
	}
	وإلا
	    vim_strncpy(buf, ((char_u **)(ٽيگ_fnames.ga_معلومات))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	أرجع OK;
    }

    إذا (پهريون)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_ٽيگ_فهرست = vim_strsave((*curbuf->b_p_ٽيگ_فهرست != NUL)
						 ? curbuf->b_p_ٽيگ_فهرست : p_ٽيگ_فهرست);
	إذا (tnp->tn_ٽيگ_فهرست == عدم)
	    أرجع FAIL;
	tnp->tn_np = tnp->tn_ٽيگ_فهرست;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    لكل (;;)
    {
	إذا (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_ڳولا_ctx);
	    إذا (fname != عدم)
		اكسر;

	    tnp->tn_did_filefind_init = FALSE;
	}
	وإلا
	{
	    char_u  *filename = عدم;

	    // Stop when used all parts of 'tags'.
	    إذا (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_ڳولا_ctx);
		tnp->tn_ڳولا_ctx = عدم;
		أرجع FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (فراغ)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    إذا (r_ptr != عدم)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_ڳولا_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_ڳولا_ctx, TRUE, curbuf->b_ffname);
	    إذا (tnp->tn_ڳولا_ctx != عدم)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    أرجع OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    فراغ
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_ٽيگ_فهرست);
    vim_findfile_cleanup(tnp->tn_ڳولا_ctx);
    tnp->tn_ڳولا_ctx = عدم;
    ga_clear_لڙيون(&ٽيگ_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    ساكن صحيح
parse_ٽيگ_سٽ(
    char_u	*lbuf,		// line to be parsed
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    صحيح		is_etag,
#جيڪڏهن_پورو
    tagptrs_T	*tagp)
{
    char_u	*p;

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (is_etag)
	// emacs-style tag file
	أرجع emacs_ٽيگ_فهرست_parse_سٽ(lbuf, tagp);
#جيڪڏهن_پورو

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    إذا (p == عدم)
	أرجع FAIL;
    tagp->tagname_پڇاڙي = p;

    // Isolate file name, from first to second white space
    إذا (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    إذا (p == عدم)
	أرجع FAIL;
    tagp->fname_پڇاڙي = p;

    // find start of search command, after second white space
    إذا (*p != NUL)
	++p;
    إذا (*p == NUL)
	أرجع FAIL;
    tagp->command = p;

    أرجع OK;
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
    ساكن صحيح
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    طالما ((p = vim_strchr(p, '\t')) != عدم)
    {
	++p;
	إذا (STRNCMP(p, "file:", 5) == 0)
	    أرجع TRUE;
    }

    أرجع FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ساكن size_t
matching_سٽ_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    p += STRLEN(p) + 1;
#جيڪڏهن_پورو
    أرجع (p - lbuf) + STRLEN(p);
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
    ساكن صحيح
parse_ميلاپ(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    صحيح		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->ٽيگ_fname = lbuf + 1;
    lbuf += STRLEN(tagp->ٽيگ_fname) + 2;
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_پڇاڙي = lbuf++;
    }
    وإلا
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#جيڪڏهن_پورو

    // Find search pattern and the file name for non-etags.
    retval = parse_ٽيگ_سٽ(lbuf,
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
			tagp->is_etag,
#جيڪڏهن_پورو
			tagp);

    tagp->tagkind = عدم;
    tagp->user_معلومات = عدم;
    tagp->tagline = 0;
    tagp->command_پڇاڙي = عدم;

    إذا (retval != OK)
	أرجع retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    إذا (ڳوليو_extra(&p) == OK)
    {
	إذا (p > tagp->command && p[-1] == '|')
	    tagp->command_پڇاڙي = p - 1;  // drop trailing bar
	وإلا
	    tagp->command_پڇاڙي = p;
	p += 2;	// skip ";\""
	إذا (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    طالما (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		إذا (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		وإلا إذا (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_معلومات = p + 10;
		وإلا إذا (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((حرف *)p + 5);
		إذا (tagp->tagkind != عدم && tagp->user_معلومات != عدم)
		    اكسر;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		إذا (pc == عدم || (pt != عدم && pc > pt))
		    tagp->tagkind = p;
		إذا (pt == عدم)
		    اكسر;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    إذا (tagp->tagkind != عدم)
    {
	لكل (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_پڇاڙي = p;
    }
    إذا (tagp->user_معلومات != عدم)
    {
	لكل (p = tagp->user_معلومات;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_معلومات_پڇاڙي = p;
    }
    أرجع retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ساكن char_u *
ٽيگ_ڀريل_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    صحيح		c;

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (tagp->is_etag)
	c = 0;	    // to shut up GCC
    وإلا
#جيڪڏهن_پورو
    {
	c = *tagp->fname_پڇاڙي;
	*tagp->fname_پڇاڙي = NUL;
    }
    fullname = expand_ٽيگ_fname(tagp->fname, tagp->ٽيگ_fname, FALSE);

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (!tagp->is_etag)
#جيڪڏهن_پورو
	*tagp->fname_پڇاڙي = c;

    أرجع fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ساكن صحيح
jumpto_ٽيگ(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    صحيح		forceit,	// :ta with !
    صحيح		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    صحيح		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_پڇاڙي;
    char_u	*tofree_fname = عدم;
    char_u	*fname;
    tagptrs_T	tagp;
    صحيح		retval = FAIL;
    صحيح		getfile_نتيجو = GETFILE_اڻ_استعمال_ٿيل;
    صحيح		ڳولا_options;
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
    صحيح		save_no_hlsearch;
#جيڪڏهن_پورو
#إذا defined(FEAT_QUICKFIX)
    win_T	*curwin_save = عدم;
#جيڪڏهن_پورو
    char_u	*ڀريل_fname = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    صحيح		old_ڪنجي_Typed = ڪنجي_Typed;    // getting the file may reset it
#جيڪڏهن_پورو
    size_t	len;
    char_u	*lbuf;
    صحيح		انگ_آهي = FALSE;

    إذا (postponed_ورهايو == 0 && !check_can_سيٽ_curbuf_forceit(forceit))
	أرجع FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_سٽ_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    إذا (lbuf != عدم)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    إذا (pbuf == عدم || lbuf == عدم || parse_ميلاپ(lbuf, &tagp) == FAIL)
    {
	tagp.fname_پڇاڙي = عدم;
	اذهب erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_پڇاڙي = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    إذا (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	انگ_آهي = TRUE;
	pbuf[0] = ':';
    }
    لكل (pbuf_پڇاڙي = pbuf + انگ_آهي;
	    *str && *str != '\n' && *str != '\r'; )
    {
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	إذا (tagp.is_etag && *str == ',')// stop at ',' after line number
	    اكسر;
#جيڪڏهن_پورو
	*pbuf_پڇاڙي++ = *str++;
	إذا (pbuf_پڇاڙي - pbuf + 1 + انگ_آهي >= LSIZE)
	    اكسر;
    }
    *pbuf_پڇاڙي = NUL;

#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    إذا (!tagp.is_etag)
#جيڪڏهن_پورو
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	إذا (انگ_آهي)
	    str++;
	إذا (ڳوليو_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    إذا (str > pbuf && str[-1] == '|')
		pbuf_پڇاڙي = str - 1;
	    وإلا
		pbuf_پڇاڙي = str;
	    *pbuf_پڇاڙي = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_ٽيگ_fname(fname, tagp.ٽيگ_fname, TRUE);
    إذا (fname == عدم)
	اذهب erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    إذا (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, عدم))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	إذا (nofile_fname == عدم)
	    nofile_fname = خالي_جايون_option;
	اذهب erret;
    }

    ++RedrawingDisabled;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    need_mouse_correct = TRUE;
#جيڪڏهن_پورو

#إذا defined(FEAT_QUICKFIX)
    إذا (g_do_tagpreview != 0)
    {
	postponed_ورهايو = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	إذا (!curwin->w_p_pvw)
	{
	    ڀريل_fname = ڀريل_Name_save(fname, FALSE);
	    fname = ڀريل_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    إذا (postponed_ورهايو && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	إذا (existing_buf != عدم)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    إذا (swbuf_goto_win_with_buf(existing_buf) != عدم)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_نتيجو = GETFILE_SAME_FILE;
	}
    }
    إذا (getfile_نتيجو == GETFILE_اڻ_استعمال_ٿيل
				  && (postponed_ورهايو || cmdmod.cmod_tab != 0))
    {
	إذا (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_ورهايو |= WSP_VERT;

	إذا ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_اشاريو(curtab) + 1;

	إذا (win_ورهايو(postponed_ورهايو > 0 ? postponed_ورهايو : 0,
						postponed_ورهايو_flags) == FAIL)
	{
	    إذا (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    اذهب erret;
	}
	RESET_BINDING(curwin);
    }
#جيڪڏهن_پورو

    إذا (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#إذا defined(FEAT_QUICKFIX)
	إذا (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_بفر);
	وإلا
#جيڪڏهن_پورو
	    keep_help_flag = curbuf->b_help;
    }

    إذا (getfile_نتيجو == GETFILE_اڻ_استعمال_ٿيل)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_نتيجو = getfile(0, fname, عدم, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    إذا (GETFILE_SUCCESS(getfile_نتيجو))	// got to the right file
    {
	curwin->w_سيٽ_curswant = true;
	postponed_ورهايو = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#جيڪڏهن_پورو
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	إذا (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#جيڪڏهن_پورو

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	إذا (vim_strchr(p_cpo, CPO_TAGPAT) != عدم)
	    ڳولا_options = 0;
	وإلا
	    ڳولا_options = ڳولا_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	إذا (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	إذا (str > pbuf_پڇاڙي - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_پڇاڙي - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    إذا (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    وإلا
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    إذا (do_ڳولا(عدم, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ڊگهو)1,
							 ڳولا_options, عدم))
		retval = OK;
	    وإلا
	    {
		صحيح	مليو = 1;
		صحيح	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		إذا (!do_ڳولا(عدم, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ڊگهو)1,
							 ڳولا_options, عدم))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    مليو = 2;
		    (فراغ)test_for_static(&tagp);
		    cc = *tagp.tagname_پڇاڙي;
		    *tagp.tagname_پڇاڙي = NUL;
		    pbuflen = vim_snprintf((حرف *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    إذا (!do_ڳولا(عدم, '/', '/', pbuf, pbuflen, (ڊگهو)1,
							 ڳولا_options, عدم))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((حرف *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			إذا (!do_ڳولا(عدم, '/', '/', pbuf, pbuflen, (ڊگهو)1,
							 ڳولا_options, عدم))
			    مليو = 0;
		    }
		    *tagp.tagname_پڇاڙي = cc;
		}
		إذا (مليو == 0)
		{
		    emsg(_(e_cannot_ڳوليو_ٽيگ_نمونو));
		    curwin->w_cursor.lnum = save_lnum;
		}
		وإلا
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    إذا (مليو == 2 || !save_p_ic)
		    {
			نياپو(_(e_couldnt_ڳوليو_ٽيگ_just_guessing));
			إذا (!نياپو_scrolled && نياپو_silent == 0)
			{
			    ٻاهريون_flush();
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
	وإلا
	{
	    صحيح		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
	    ++sandbox;
#جيڪڏهن_پورو
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.کانو = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    إذا (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
	    --sandbox;
#جيڪڏهن_پورو
	}

	magic_overruled = save_magic_overruled;
#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	إذا (ڳولا_options)
	    سيٽ_no_hlsearch(save_no_hlsearch);
#جيڪڏهن_پورو

	// Return OK if jumped to another file (at least we found the file!).
	إذا (getfile_نتيجو == GETFILE_OPEN_OTHER)
	    retval = OK;

	إذا (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    إذا (curbuf->b_help)
		سيٽ_topline(curwin, curwin->w_cursor.lnum);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا ((fdo_flags & FDO_ٽيگ) && old_ڪنجي_Typed)
		foldOpenCursor();
#جيڪڏهن_پورو
	}

#إذا defined(FEAT_QUICKFIX)
	إذا (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#جيڪڏهن_پورو

	إذا (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    وإلا
    {
	إذا (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	إذا (postponed_ورهايو)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_ورهايو = 0;
	}
#إذا defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	وإلا إذا (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    إذا (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#جيڪڏهن_پورو
    }
#إذا defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    إذا (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#جيڪڏهن_پورو

erret:
#إذا defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#جيڪڏهن_پورو
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(ڀريل_fname);

    أرجع retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ساكن char_u *
expand_ٽيگ_fname(char_u *fname, char_u *ٽيگ_fname, صحيح expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = عدم;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    إذا (p_tagsecure && رستو_with_url(fname))
    {
       emsg(_(e_ٽيگ_file_داخلا_must_not_be_url));
       أرجع عدم;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    إذا (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == عدم)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, عدم,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	إذا (expanded_fname != عدم)
	    fname = expanded_fname;
    }

    إذا ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(ٽيگ_fname)) != ٽيگ_fname)
    {
	retval = alloc(MAXPATHL);
	إذا (retval != عدم)
	{
	    STRCPY(retval, ٽيگ_fname);
	    vim_strncpy(retval + (p - ٽيگ_fname), fname,
					      MAXPATHL - (p - ٽيگ_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    وإلا
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    أرجع retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ساكن صحيح
test_for_current(
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
    صحيح	    is_etag,
#جيڪڏهن_پورو
    char_u  *fname,
    char_u  *fname_پڇاڙي,
    char_u  *ٽيگ_fname,
    char_u  *buf_ffname)
{
    صحيح	    c;
    صحيح	    retval = FALSE;
    char_u  *fullname;

    إذا (buf_ffname != عدم)	// if the buffer has a name
    {
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	إذا (is_etag)
	    c = 0;	    // to shut up GCC
	وإلا
#جيڪڏهن_پورو
	{
	    c = *fname_پڇاڙي;
	    *fname_پڇاڙي = NUL;
	}
	fullname = expand_ٽيگ_fname(fname, ٽيگ_fname, TRUE);
	إذا (fullname != عدم)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#جيڪڏهن_وصف_ٿيل FEAT_EMACS_ٽيگ_فهرست
	إذا (!is_etag)
#جيڪڏهن_پورو
	    *fname_پڇاڙي = c;
    }

    أرجع retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ساكن صحيح
ڳوليو_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	پهريون_char = **pp;

    // Repeat for addresses separated with ';'
    لكل (;;)
    {
	إذا (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	وإلا إذا (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    إذا (*str != پهريون_char)
		str = عدم;
	    وإلا
		++str;
	}
	وإلا
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)لڙي_ڳولا((حرف *)str, "|;\"");
	    إذا (str != عدم)
	    {
		++str;
		اكسر;
	    }

	}
	إذا (str == عدم || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    اكسر;
	++str;	// skip ';'
	پهريون_char = *str;
    }

    إذا (str != عدم && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    فراغ
tagstack_clear_داخلا(taggy_T *شي)
{
    VIM_CLEAR(شي->tagname);
    VIM_CLEAR(شي->user_معلومات);
}

    صحيح
expand_ٽيگ_فهرست(
    صحيح		tagnames,	// expand tag names
    char_u	*pat,
    صحيح		*انگ_file,
    char_u	***file)
{
    صحيح		i;
    صحيح		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_سائيز = 100;
    tagptrs_T	t_p;
    صحيح		ret;

    name_buf = alloc(name_buf_سائيز);
    إذا (name_buf == عدم)
	أرجع FAIL;

    إذا (tagnames)
	extra_flag = ٽيگ_نالا;
    وإلا
	extra_flag = 0;
    إذا (pat[0] == '/')
	ret = ڳوليو_ٽيگ_فهرست(pat + 1, انگ_file, file,
		ٽيگ_REGEXP | extra_flag | ٽيگ_VERBOSE,
		ٽيگ_MANY, curbuf->b_ffname);
    وإلا
	ret = ڳوليو_ٽيگ_فهرست(pat, انگ_file, file,
	      ٽيگ_REGEXP | extra_flag | ٽيگ_VERBOSE | ٽيگ_NOIC,
		ٽيگ_MANY, curbuf->b_ffname);
    إذا (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 لكل (i = 0; i < *انگ_file; i++)
	 {
	     size_t	len;

	     parse_ميلاپ((*file)[i], &t_p);
	     len = t_p.tagname_پڇاڙي - t_p.tagname;
	     إذا (len > name_buf_سائيز - 3)
	     {
		 char_u *buf;

		 name_buf_سائيز = len + 3;
		 buf = vim_realloc(name_buf, name_buf_سائيز);
		 إذا (buf == عدم)
		 {
		     vim_free(name_buf);
		     أرجع FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != عدم && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_پڇاڙي - t_p.fname);
	     (*file)[i][len + (t_p.fname_پڇاڙي - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    أرجع ret;
}

#إذا defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ساكن صحيح
جوڙيو_ٽيگ_ميدان(
    dict_T  *dict,
    حرف    *ميدان_name,
    char_u  *شروعات,		// start of the value
    char_u  *پڇاڙي)		// after the value; can be NULL
{
    char_u	*buf;
    صحيح		len = 0;
    صحيح		retval;

    // check that the field name doesn't exist yet
    إذا (dict_has_ڪنجي(dict, ميدان_name))
    {
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), ميدان_name);
	    verbose_leave();
	}
	أرجع FAIL;
    }
    buf = alloc(MAXPATHL);
    إذا (buf == عدم)
	أرجع FAIL;
    إذا (شروعات != عدم)
    {
	إذا (پڇاڙي == عدم)
	{
	    پڇاڙي = شروعات + STRLEN(شروعات);
	    طالما (پڇاڙي > شروعات && (پڇاڙي[-1] == '\r' || پڇاڙي[-1] == '\n'))
		--پڇاڙي;
	}
	len = (صحيح)(پڇاڙي - شروعات);
	إذا (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, شروعات, len);
    }
    buf[len] = NUL;
    retval = dict_جوڙيو_string(dict, ميدان_name, buf);
    vim_free(buf);
    أرجع retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    صحيح
get_ٽيگ_فهرست(list_T *list, char_u *pat, char_u *buf_fname)
{
    صحيح		انگ_matches, i, ret;
    char_u	**matches, *p;
    char_u	*ڀريل_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    ڊگهو	is_static;

    ret = ڳوليو_ٽيگ_فهرست(pat, &انگ_matches, &matches,
				ٽيگ_REGEXP | ٽيگ_NOIC, (صحيح)MAXCOL, buf_fname);
    إذا (ret != OK || انگ_matches <= 0)
	أرجع ret;

    لكل (i = 0; i < انگ_matches; ++i)
    {
	إذا (parse_ميلاپ(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    استمر;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	إذا (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    استمر;
	}

	إذا ((dict = dict_alloc()) == عدم)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    اكسر;
	}
	إذا (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    استمر;
	}

	ڀريل_fname = ٽيگ_ڀريل_fname(&tp);
	إذا (جوڙيو_ٽيگ_ميدان(dict, "name", tp.tagname, tp.tagname_پڇاڙي) == FAIL
		|| جوڙيو_ٽيگ_ميدان(dict, "filename", ڀريل_fname,
		    عدم) == FAIL
		|| جوڙيو_ٽيگ_ميدان(dict, "cmd", tp.command,
		    tp.command_پڇاڙي) == FAIL
		|| جوڙيو_ٽيگ_ميدان(dict, "kind", tp.tagkind,
		    tp.tagkind_پڇاڙي) == FAIL
		|| dict_جوڙيو_عدد(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(ڀريل_fname);

	إذا (tp.command_پڇاڙي != عدم)
	{
	    لكل (p = tp.command_پڇاڙي + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		إذا (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_پڇاڙي - 1;
		وإلا إذا (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		وإلا إذا (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    صحيح	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    طالما (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (صحيح)(p - n);
		    إذا (*p == ':' && len > 0)
		    {
			s = ++p;
			طالما (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			إذا (جوڙيو_ٽيگ_ميدان(dict, (حرف *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    وإلا
			// Skip field without colon.
			طالما (*p != NUL && *p >= ' ')
			    ++p;
		    إذا (*p == NUL)
			اكسر;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    أرجع ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ساكن فراغ
get_ٽيگ_details(taggy_T *ٽيگ, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_جوڙيو_string(retdict, "tagname", ٽيگ->tagname);
    dict_جوڙيو_عدد(retdict, "matchnr", ٽيگ->cur_ميلاپ + 1);
    dict_جوڙيو_عدد(retdict, "bufnr", ٽيگ->cur_fnum);
    إذا (ٽيگ->user_معلومات)
	dict_جوڙيو_string(retdict, "user_data", ٽيگ->user_معلومات);

    إذا ((pos = list_alloc_id(aid_tagstack_from)) == عدم)
	أرجع;
    dict_جوڙيو_list(retdict, "from", pos);

    fmark = &ٽيگ->fmark;
    list_append_عدد(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_عدد(pos, (varnumber_T)fmark->mark.lnum);
    list_append_عدد(pos, (varnumber_T)(fmark->mark.کانو == MAXCOL ?
					MAXCOL : fmark->mark.کانو + 1));
    list_append_عدد(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    فراغ
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    صحيح		i;
    dict_T	*d;

    dict_جوڙيو_عدد(retdict, "length", wp->w_tagstacklen);
    dict_جوڙيو_عدد(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_شيون);
    إذا (l == عدم)
	أرجع;
    dict_جوڙيو_list(retdict, "items", l);

    لكل (i = 0; i < wp->w_tagstacklen; i++)
    {
	إذا ((d = dict_alloc_id(aid_tagstack_details)) == عدم)
	    أرجع;
	إذا (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    أرجع;
	}

	get_ٽيگ_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ساكن فراغ
tagstack_clear(win_T *wp)
{
    صحيح i;

    // Free the current tag stack
    لكل (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_داخلا(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ساكن فراغ
tagstack_سرڪايو(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحيح		i;

    tagstack_clear_داخلا(&tagstack[0]);
    لكل (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ساكن فراغ
tagstack_ڌڪو_شي(
	win_T	*wp,
	char_u	*tagname,
	صحيح	cur_fnum,
	صحيح	cur_ميلاپ,
	pos_T	mark,
	صحيح	fnum,
	char_u  *user_معلومات)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحيح		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    إذا (idx >= TAGSTACKSIZE)
    {
	tagstack_سرڪايو(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_ميلاپ = cur_ميلاپ;
    إذا (tagstack[idx].cur_ميلاپ < 0)
	tagstack[idx].cur_ميلاپ = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_معلومات = user_معلومات;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ساكن فراغ
tagstack_ڌڪو_شيون(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    صحيح		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_شيون(l, li)
    {
	إذا (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == عدم)
	    استمر;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	إذا ((di = dict_ڳوليو(itemdict, (char_u *)"from", -1)) == عدم)
	    استمر;
	إذا (list2fpos(&di->di_tv, &mark, &fnum, عدم, FALSE) != OK)
	    استمر;
	إذا ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == عدم)
	    استمر;

	إذا (mark.کانو > 0)
	    mark.کانو--;
	tagstack_ڌڪو_شي(wp, tagname,
		(صحيح)dict_get_عدد(itemdict, "bufnr"),
		(صحيح)dict_get_عدد(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ساكن فراغ
tagstack_سيٽ_curidx(win_T *wp, صحيح curidx)
{
    wp->w_tagstackidx = curidx;
    إذا (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    إذا (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    صحيح
سيٽ_tagstack(win_T *wp, dict_T *d, صحيح action)
{
    dictitem_T	*di;
    list_T	*l = عدم;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    إذا (tfu_اندريون_use)
    {
	emsg(_(e_cannot_modify_ٽيگ_ڍير_within_tagfunc));
	أرجع FAIL;
    }
# endif

    إذا ((di = dict_ڳوليو(d, (char_u *)"items", -1)) != عدم)
    {
	إذا (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    أرجع FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    إذا ((di = dict_ڳوليو(d, (char_u *)"curidx", -1)) != عدم)
	tagstack_سيٽ_curidx(wp, (صحيح)tv_get_عدد(&di->di_tv) - 1);

    إذا (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	صحيح	tagstackidx = wp->w_tagstackidx;
	صحيح	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	طالما (tagstackidx < tagstacklen)
	    tagstack_clear_داخلا(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    إذا (l != عدم)
    {
	إذا (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ڌڪو_شيون(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    أرجع OK;
}
#جيڪڏهن_پورو
