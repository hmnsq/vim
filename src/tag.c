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
تعریفنوع ساختار tag_pointers
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_end;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_end;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_end;	// first char after command
    char_u	*tag_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#ifdef FEAT_EMACS_TAGS
    صحیح		is_etag;	// TRUE for emacs tag
#endif
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_end;	// end of tagkind
    char_u	*user_data;	// user_data string
    char_u	*user_data_end;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
تعریفنوع شمارش
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
تعریفنوع شمارش
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_دودویی,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
تعریفنوع ساختار
{
    off_T	کم_offset;	// offset for first char of first line that
				// could match
    off_T	بالا_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    صحیح	کم_char;		// first char at low_offset
    صحیح	بالا_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
تعریفنوع شمارش
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
تعریفنوع ساختار
{
    صحیح	matchoff;		// tag match offset
    صحیح	match_re;		// TRUE if the tag matches a regexp
    صحیح	match_no_ic;		// TRUE if the tag matches with case
    صحیح	has_re;			// regular expression used
    صحیح	sortic;			// tags file sorted ignoring case (foldcase)
    صحیح	sort_error;		// tags file not sorted
} findtags_match_args_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#define MT_ST_CUR	0		// static match in current file
#define MT_GL_CUR	1		// global match in current file
#define MT_GL_OTH	2		// global match in other file
#define MT_ST_OTH	3		// static match in other file
#define MT_IC_OFF	4		// add for icase match
#define MT_RE_OFF	8		// add for regexp match
#define MT_MASK		7		// mask for printing priority
#define MT_شمار	16

ایستا نویسه	*mt_names[MT_شمار/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
ایستا char_u	*nofile_fname = پوچ;	// fname for NOTAGFILE error

ایستا تهی taglen_advance(صحیح l);

ایستا صحیح jumpto_tag(char_u *lbuf, صحیح forceit, صحیح keep_help);
#ifdef FEAT_EMACS_TAGS
ایستا صحیح parse_tag_سطر(char_u *lbuf, صحیح is_etag, tagptrs_T *tagp);
#وگرنه
ایستا صحیح parse_tag_سطر(char_u *lbuf, tagptrs_T *tagp);
#endif
ایستا صحیح test_for_static(tagptrs_T *);
ایستا صحیح parse_match(char_u *lbuf, tagptrs_T *tagp);
ایستا char_u *tag_full_fname(tagptrs_T *tagp);
ایستا char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, صحیح expand);
#ifdef FEAT_EMACS_TAGS
ایستا صحیح test_for_current(صحیح, char_u *, char_u *, char_u *, char_u *);
#وگرنه
ایستا صحیح test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
ایستا صحیح find_extra(char_u **pp);
ایستا تهی print_tag_list(صحیح new_tag, صحیح use_tagstack, صحیح رقم_matches, char_u **matches);
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ایستا صحیح add_llist_tags(char_u *tag, صحیح رقم_matches, char_u **matches);
#endif

ایستا char_u	*tagmatchname = پوچ;	// name of last used tag

#اگر defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ایستا taggy_T ptag_entry = {پوچ, {{0, 0, 0}, 0}, 0, 0, پوچ};
#endif

#ifdef FEAT_EVAL
ایستا صحیح  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
ایستا callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#اگر defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    نویسه *
did_set_tagfunc(optset_T *args)
{
    صحیح	retval;

    اگر (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    وگرنه
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	اگر (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    بازگشت retval == FAIL ? e_invalid_argument : پوچ;
}
#endif

#اگر defined(EXITFREE)
    تهی
free_tagfunc_option(تهی)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    صحیح
set_ref_in_tagfunc(صحیح copyID UNUSED)
{
    صحیح	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    بازگشت abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    تهی
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    اگر (tfu_cb.cb_name != پوچ && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#endif

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
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    صحیح		type,
    صحیح		شمار,
    صحیح		forceit,	// :ta with !
    صحیح		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		tagstacklen = curwin->w_tagstacklen;
    صحیح		cur_match = 0;
    صحیح		cur_fnum = curbuf->b_fnum;
    صحیح		oldtagstackidx = tagstackidx;
    صحیح		prevtagstackidx = tagstackidx;
    صحیح		prev_رقم_matches;
    صحیح		new_tag = FALSE;
    صحیح		i;
    صحیح		ic;
    صحیح		no_regexp = FALSE;
    صحیح		error_cur_match = 0;
    صحیح		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    صحیح		jumped_to_tag = FALSE;
#endif
    صحیح		new_رقم_matches;
    char_u	**new_matches;
    صحیح		use_tagstack;
    صحیح		skip_پیام = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    صحیح		use_tfu = 1;
    char_u	*tofree = پوچ;

    // remember the matches for the last used tag
    ایستا صحیح		رقم_matches = 0;
    ایستا صحیح		max_رقم_matches = 0;  // limit used for match search
    ایستا char_u	**matches = پوچ;
    ایستا صحیح		flags;

#ifdef EXITFREE
    اگر (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(رقم_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	رقم_matches = 0;
	بازگشت FALSE;
    }
#endif

#ifdef FEAT_EVAL
    اگر (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_پشته_within_tagfunc));
	بازگشت FALSE;
    }
#endif

    اگر (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	بازگشت FALSE;

    اگر (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_رقم_matches = رقم_matches;
    free_string_option(nofile_fname);
    nofile_fname = پوچ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    اگر ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    اگر ((ptag_entry.tagname = vim_strsave(tag)) == پوچ)
		برو end_do_tag;
	}
#endif
    }
    وگرنه
    {
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	وگرنه
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	اگر (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#اگر defined(FEAT_QUICKFIX)
	    اگر (g_do_tagpreview != 0)
	    {
		اگر (ptag_entry.tagname != پوچ
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		وگرنه
		{
		    tagstack_clear_entry(&ptag_entry);
		    اگر ((ptag_entry.tagname = vim_strsave(tag)) == پوچ)
			برو end_do_tag;
		}
	    }
	    وگرنه
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		وقتی (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		اگر (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    برای (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = پوچ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		اگر ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == پوچ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    برو end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	وگرنه
	{
	    اگر (
#اگر defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == پوچ :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_پشته_empty));
		برو end_do_tag;
	    }

	    اگر (type == DT_برداشت)		// go to older position
	    {
#ifdef FEAT_FOLDING
		صحیح	old_کلید_Typed = کلید_Typed;
#endif
		اگر ((tagstackidx -= شمار) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_پشته));
		    اگر (tagstackidx + شمار == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			برو end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		وگرنه اگر (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_بالاترین_of_tag_پشته));
		    برو end_do_tag;
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
			برو end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		وگرنه
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		اگر ((fdo_flags & FDO_TAG) && old_کلید_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(رقم_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		رقم_matches = 0;
		tag_freematch();
		برو end_do_tag;
	    }

	    اگر (type == DT_TAG
#اگر defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#اگر defined(FEAT_QUICKFIX)
		اگر (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		وگرنه
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    اگر ((tagstackidx += شمار - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_بالاترین_of_tag_پشته));
			save_pos = FALSE;
		    }
		    وگرنه اگر (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_پشته));
			tagstackidx = 0;
			برو end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    وگرنه				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#اگر defined(FEAT_QUICKFIX)
		اگر (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		وگرنه
#endif
		{
		    اگر (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		انتخاب (type)
		{
		    حالت DT_FIRST: cur_match = شمار - 1; بشکن;
		    حالت DT_SELECT:
		    حالت DT_JUMP:
#ifdef FEAT_CSCOPE
		    حالت DT_CSCOPE:
#endif
		    حالت DT_LAST:  cur_match = MAXCOL - 1; بشکن;
		    حالت DT_NEXT:  cur_match += شمار; بشکن;
		    حالت DT_PREV:  cur_match -= شمار; بشکن;
		}
		اگر (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		وگرنه اگر (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_پیام = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	{
	    اگر (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	وگرنه
#endif
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
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
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

	اگر (buf != پوچ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    برای (;;)
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
	وگرنه اگر (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	وگرنه
	    name = tag;
	other_name = (tagmatchname == پوچ || STRCMP(tagmatchname, name) != 0);
	اگر (new_tag
		|| (cur_match >= رقم_matches && max_رقم_matches != MAXCOL)
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
#endif
		)
		cur_match = MAXCOL - 1;
	    اگر (type == DT_TAG)
		max_رقم_matches = MAXCOL;
	    وگرنه
		max_رقم_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    اگر (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    وگرنه
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    اگر (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    اگر (verbose)
		flags |= TAG_VERBOSE;

	    اگر (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    اگر (find_tags(name, &new_رقم_matches, &new_matches, flags,
					    max_رقم_matches, buf_ffname) == OK
		    && new_رقم_matches < max_رقم_matches)
		max_رقم_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    اگر (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_رقم_matches, new_matches);
		بشکن;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    اگر (!new_tag && !other_name)
	    {
		صحیح	    j, k;
		صحیح	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		برای (j = 0; j < رقم_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    برای (i = idx; i < new_رقم_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			اگر (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    برای (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    بشکن;
			}
		    }
		}
	    }
	    FreeWild(رقم_matches, matches);
	    رقم_matches = new_رقم_matches;
	    matches = new_matches;
	}

	اگر (رقم_matches <= 0)
	{
	    اگر (verbose)
		semsg(_(e_tag_not_found_str), name);
#اگر defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	وگرنه
	{
	    صحیح ask_for_selection = FALSE;
#اگر defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    اگر (type == DT_CSCOPE && رقم_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    وگرنه
#endif
	    اگر (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = شمار > 0 ? شمار - 1 : 0;
	    وگرنه اگر (type == DT_SELECT || (type == DT_JUMP && رقم_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, رقم_matches, matches);
		ask_for_selection = TRUE;
	    }
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    وگرنه اگر (type == DT_LTAG)
	    {
		اگر (add_llist_tags(tag, رقم_matches, matches) == FAIL)
		    برو end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    اگر (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_عدد(پوچ);
		اگر (i <= 0 || i > رقم_matches || got_int)
		{
		    // no valid choice: don't change anything
		    اگر (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    بشکن;
		}
		cur_match = i - 1;
	    }

	    اگر (cur_match >= رقم_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		اگر ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == پوچ)
		{
		    اگر (رقم_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    وگرنه
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_پیام = TRUE;
		}
		cur_match = رقم_matches - 1;
	    }
	    اگر (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		اگر (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#اگر defined(FEAT_QUICKFIX)
	    وگرنه اگر (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    اگر (nofile_fname != پوچ && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    اگر (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (رقم_matches > 1 || ic)
		&& !skip_پیام)
	    {
		// Give an indication of the number of matching tags
		sprintf((نویسه *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				رقم_matches,
				max_رقم_matches != MAXCOL ? _(" or more") : "");
		اگر (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		اگر ((رقم_matches > prev_رقم_matches || new_tag)
							   && رقم_matches > 1)
		{
		    اگر (ic)
			پیام_attr((نویسه *)IObuff, HL_ATTR(HLF_W));
		    وگرنه
			پیام((نویسه *)IObuff);
		    پیام_scroll = TRUE;	// don't overwrite this message
		}
		وگرنه
		    give_warning(IObuff, ic);
		اگر (ic && !پیام_scrolled && پیام_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#اگر defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (صحیح)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#اگر defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, پوچ, -1);
#endif

	    اگر (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		اگر ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_رقم_matches != MAXCOL
					     || cur_match < رقم_matches - 1)))
		{
		    error_cur_match = cur_match;
		    اگر (use_tagstack)
			--tagstackidx;
		    اگر (type == DT_PREV)
			--cur_match;
		    وگرنه
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    ادامه;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    وگرنه
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		اگر (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	بشکن;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    اگر (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    بازگشت jumped_to_tag;
#وگرنه
    بازگشت FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    ایستا تهی
print_tag_list(
    صحیح		new_tag,
    صحیح		use_tagstack,
    صحیح		رقم_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    صحیح		taglen;
    صحیح		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (صحیح)(tagp.tagname_end - tagp.tagname + 2);
    اگر (taglen < 18)
	taglen = 18;
    اگر (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    اگر (پیام_col == 0)
	پیام_didout = FALSE;	// overwrite previous message
    پیام_start();
    پیام_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    پیام_clr_eos();
    taglen_advance(taglen);
    پیام_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    برای (i = 0; i < رقم_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	اگر (!new_tag && (
#اگر defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	وگرنه
	    *IObuff = ' ';
	vim_snprintf((نویسه *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	پیام_puts((نویسه *)IObuff);
	اگر (tagp.tagkind != پوچ)
	    پیام_outtrans_len(tagp.tagkind,
			  (صحیح)(tagp.tagkind_end - tagp.tagkind));
	پیام_advance(13);
	پیام_outtrans_len_attr(tagp.tagname,
			   (صحیح)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	پیام_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	اگر (p != پوچ)
	{
	    پیام_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	اگر (پیام_col > 0)
	    پیام_putchar('\n');
	اگر (got_int)
	    بشکن;
	پیام_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	اگر (command_end != پوچ)
	{
	    p = command_end + 3;
	    وقتی (*p && *p != '\r' && *p != '\n')
	    {
		وقتی (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		اگر (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ادامه;
		}
		// skip "kind:<kind>" and "<kind>"
		اگر (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    ادامه;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		وقتی (*p && *p != '\r' && *p != '\n')
		{
		    اگر (پیام_col + ptr2cells(p) >= cmdline_width)
		    {
			پیام_putchar('\n');
			اگر (got_int)
			    بشکن;
			پیام_advance(15);
		    }
		    p = پیام_outtrans_one(p, attr);
		    اگر (*p == TAB)
		    {
			پیام_puts_attr(" ", attr);
			بشکن;
		    }
		    اگر (*p == ':')
			attr = 0;
		}
	    }
	    اگر (پیام_col > 15)
	    {
		پیام_putchar('\n');
		اگر (got_int)
		    بشکن;
		پیام_advance(15);
	    }
	}
	وگرنه
	{
	    برای (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
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
	وقتی (p != command_end && vim_isspace(*p))
	    ++p;

	وقتی (p != command_end)
	{
	    اگر (پیام_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		پیام_putchar('\n');
	    اگر (got_int)
		بشکن;
	    پیام_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    اگر (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    اگر (*p == TAB)
	    {
		پیام_putchar(' ');
		++p;
	    }
	    وگرنه
		p = پیام_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    اگر (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		بشکن;
	    // don't display matching '/' or '?'
	    اگر (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		بشکن;
	}
	اگر (پیام_col)
	    پیام_putchar('\n');
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
    ایستا صحیح
add_llist_tags(
    char_u	*tag,
    صحیح		رقم_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    صحیح		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    اگر (list == پوچ || fname == پوچ || cmd == پوچ)
    {
	vim_free(cmd);
	vim_free(fname);
	اگر (list != پوچ)
	    list_free(list);
	بازگشت FAIL;
    }

    برای (i = 0; i < رقم_matches; ++i)
    {
	صحیح	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (صحیح)(tagp.tagname_end - tagp.tagname);
	اگر (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	اگر (p == پوچ)
	    ادامه;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	اگر (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((نویسه *)tagp.command);
	وگرنه
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    اگر (cmd_end == پوچ)
	    {
		برای (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_end = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_end--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    اگر (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    اگر (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    اگر (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (صحیح)(cmd_end - cmd_start + 1);
	    اگر (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
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

	اگر ((dict = dict_alloc()) == پوچ)
	    ادامه;
	اگر (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ادامه;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_عدد(dict, "lnum", lnum);
	اگر (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((نویسه *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, پوچ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    بازگشت OK;
}
#endif

/*
 * Free cached tags.
 */
    تهی
tag_freematch(تهی)
{
    VIM_CLEAR(tagmatchname);
}

    ایستا تهی
taglen_advance(صحیح l)
{
    اگر (l == MAXCOL)
    {
	پیام_putchar('\n');
	پیام_advance(24);
    }
    وگرنه
	پیام_advance(13 + l);
}

/*
 * Print the tag stack
 */
    تهی
do_tags(exarg_T *eap UNUSED)
{
    صحیح		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    صحیح		tagstackidx = curwin->w_tagstackidx;
    صحیح		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    پیام_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    برای (i = 0; i < tagstacklen; ++i)
    {
	اگر (tagstack[i].tagname != پوچ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    اگر (name == پوچ)	    // file name not available
		ادامه;

	    پیام_putchar('\n');
	    vim_snprintf((نویسه *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    پیام_outtrans(IObuff);
	    پیام_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    اگر (tagstackidx == tagstacklen)	// idx at top of stack
	پیام_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ایستا صحیح
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    صحیح		i;

    وقتی (len > 0)
    {
	i = (صحیح)TOUPPER_ASC(*s1) - (صحیح)TOUPPER_ASC(*s2);
	اگر (i != 0)
	    بازگشت i;			// this character different
	اگر (*s1 == NUL)
	    بشکن;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    بازگشت 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
تعریفنوع ساختار
{
    char_u	*pat;		// the pattern
    صحیح		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    صحیح		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ایستا تهی
prepare_pats(pat_T *pats, صحیح has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    اگر (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	اگر (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	وگرنه اگر (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	اگر (pats->head == pats->pat)
	    pats->headlen = 0;
	وگرنه
	    برای (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		اگر (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != پوچ)
		    بشکن;
	اگر (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    اگر (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    وگرنه
	pats->regmatch.regprog = پوچ;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
تعریفنوع شمارش {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ایستا tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    اگر (VIM_ISDIGIT(*cmd))
	بازگشت *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    اگر (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	بازگشت (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    اگر (vim_strpbrk(cmd, (char_u *)"\t\r\n") != پوچ)
	بازگشت TAGCMD_INVALID;
    بازگشت TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ایستا صحیح
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    صحیح		*match_شمار,	// here the number of tags found will be placed
    صحیح		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    صحیح		ntags = 0;
    صحیح		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    اگر (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == پوچ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	بازگشت FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    اگر ((d = dict_alloc_lock(VAR_FIXED)) == پوچ)
	بازگشت FAIL;
    اگر (!(flags & TAG_INS_COMP) && tag->user_data != پوچ)
	dict_add_string(d, "user_data", tag->user_data);
    اگر (buf_ffname != پوچ)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((نویسه *)flagString, اندازه(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    اگر (result == FAIL)
	بازگشت FAIL;
    اگر (rettv.v_type == VAR_SPECIAL && rettv.vval.v_عدد == VVAL_NULL)
    {
	clear_tv(&rettv);
	بازگشت NOTDONE;
    }
    اگر (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_مقدار_from_tagfunc));
	بازگشت FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_اقلام(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	صحیح		len;
	dict_iterator_T	iter;
	char_u		*dict_کلید;
	typval_T	*tv;
	صحیح		has_extra = 0;
	صحیح		name_only = flags & TAG_NAMES;

	اگر (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_مقدار_from_tagfunc));
	    بشکن;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# وگرنه
	len = 2;
# endif
	res_name = پوچ;
	res_fname = پوچ;
	res_cmd = پوچ;
	res_kind = پوچ;

	dict_iterate_start(&item->li_tv, &iter);
	وقتی (پوچ != (dict_کلید = dict_iterate_next(&iter, &tv)))
	{
	    اگر (tv->v_type != VAR_STRING || tv->vval.v_string == پوچ)
		ادامه;

	    len += (صحیح)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    اگر (STRCMP(dict_کلید, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ادامه;
	    }
	    اگر (STRCMP(dict_کلید, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ادامه;
	    }
	    اگر (STRCMP(dict_کلید, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ادامه;
	    }
	    has_extra = 1;
	    اگر (STRCMP(dict_کلید, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		ادامه;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (صحیح)STRLEN(dict_کلید) + 1;
	}

	اگر (has_extra)
	    len += 2;	// need space for ;"

	اگر (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_مقدار_from_tagfunc));
	    بشکن;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	اگر (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_مقدار_from_tagfunc));
	    بشکن;
	}
	اگر (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	اگر (name_only)
	    mfp = vim_strsave(res_name);
	وگرنه
	    mfp = alloc(اندازه(char_u) + len + 1);

	اگر (mfp == پوچ)
	    ادامه;

	اگر (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = TAG_SEP;	    // no tag file name
# ifdef FEAT_EMACS_TAGS
	    *p++ = TAG_SEP;
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

		اگر (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		وقتی (پوچ != (dict_کلید = dict_iterate_next(&iter, &tv)))
		{
		    اگر (tv->v_type != VAR_STRING || tv->vval.v_string == پوچ)
			ادامه;

		    اگر (STRCMP(dict_کلید, "name") == 0)
			ادامه;
		    اگر (STRCMP(dict_کلید, "filename") == 0)
			ادامه;
		    اگر (STRCMP(dict_کلید, "cmd") == 0)
			ادامه;
		    اگر (STRCMP(dict_کلید, "kind") == 0)
			ادامه;

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
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	وگرنه
	{
	    vim_free(mfp);
	    بشکن;
	}
    }

    clear_tv(&rettv);

    *match_شمار = ntags;
    بازگشت result;
}
#endif

/*
 * State information used during a tag search
 */
تعریفنوع ساختار
{
    tagsearch_state_T	state;		// tag search state
    صحیح		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    صحیح		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    صحیح		flags;			// flags used for tag search
    صحیح		tag_file_sorted;	// !_TAG_FILE_SORTED value
    صحیح		get_searchpat;		// used for 'showfulltag'
    صحیح		help_only;		// only search for help tags
    صحیح		did_open;		// did open a tag file
    صحیح		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    صحیح		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    صحیح		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    صحیح		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    صحیح		is_txt;			// flag of file extension
#endif
    صحیح		match_شمار;		// number of matches found
    garray_T	ga_match[MT_شمار];	// stores matches in sequence
    hashtab_T	ht_match[MT_شمار];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ایستا صحیح
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    صحیح			flags,
    صحیح			mincount)
{
    صحیح		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = پوچ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (صحیح)STRLEN(pat);
    st->orgpat->regmatch.regprog = پوچ;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = پوچ;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_شمار = 0;
    st->stop_searching = FALSE;

    برای (mtt = 0; mtt < MT_شمار; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], اندازه(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    اگر (st->tag_fname == پوچ
	    || st->lbuf == پوچ
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == پوچ
#endif
       )
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Free the state used by find_tags()
 */
    ایستا تهی
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->tag_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#ifdef FEAT_EMACS_TAGS
    vim_free(st->ebuf);
#endif
}

#ifdef FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    ایستا صحیح
findtags_in_help_init(findtags_state_T *st)
{
    صحیح		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    اگر (st->is_txt)
	STRCPY(st->help_lang, "en");
    وگرنه
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (صحیح)STRLEN(st->tag_fname);
	اگر (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	وگرنه
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    اگر (st->help_lang_find != پوچ
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	بازگشت FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    اگر ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == پوچ
	    && curbuf->b_fname != پوچ
	    && (i = (صحیح)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    وگرنه
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	برای (s = p_hlg; *s != NUL; ++s)
	{
	    اگر (STRNICMP(s, st->help_lang, 2) == 0)
		بشکن;
	    ++st->help_pri;
	    اگر ((s = vim_strchr(s, ',')) == پوچ)
		بشکن;
	}
	اگر (s == پوچ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    اگر (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    بازگشت TRUE;
}
#endif

#ifdef FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    ایستا صحیح
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    صحیح		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    صحیح		retval;

    اگر (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	بازگشت NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_شمار,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    بازگشت retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
ایستا ساختار
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
ایستا صحیح incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ایستا تهی
emacs_tags_incstack_free(تهی)
{
    وقتی (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = پوچ;
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
    ایستا تهی
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    اگر (vim_fgets(st->ebuf, LSIZE, st->fp))
	بازگشت;

    برای (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    اگر (*p == NUL)
	بازگشت;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    اگر (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	بازگشت;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    اگر (incstack[incstack_idx].etag_fname == پوچ)
	بازگشت;

    incstack[incstack_idx].fp = st->fp;
    st->fp = پوچ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    اگر (fullpath_ebuf != پوچ)
    {
	st->fp = mch_fopen((نویسه *)fullpath_ebuf, "r");
	اگر (st->fp != پوچ)
	{
	    اگر (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    اگر (st->fp == پوچ)
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
    ایستا صحیح
emacs_tags_file_eof(findtags_state_T *st)
{
    اگر (!incstack_idx)	// reached end of file. stop processing.
	بازگشت FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    بازگشت TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ایستا صحیح
emacs_tags_parse_سطر(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    اگر (p_7f == پوچ)
    {
etag_fail:
	اگر (vim_strchr(lbuf, '\n') != پوچ)
	    بازگشت FAIL;

	// Truncated line.  Ignore it.
	اگر (p_verbose >= 5)
	{
	    verbose_enter();
	    پیام(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	بازگشت OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    اگر (p == پوچ)
	p = p_7f + 1;
    وگرنه
	++p;

    اگر (!VIM_ISDIGIT(*p))	    // check for start of line number
	برو etag_fail;
    tagp->command = p;

    اگر (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    وگرنه			    // second format: isolate tagname
    {
	اگر (p_7f == lbuf)
	    برو etag_fail;

	// find end of tagname
	برای (p = p_7f - 1; !vim_iswordc(*p); --p)
	    اگر (p == lbuf)
		برو etag_fail;
	tagp->tagname_end = p + 1;
	وقتی (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    بازگشت OK;
}
#endif

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    ایستا tags_read_status_T
findtags_get_next_سطر(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    صحیح		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    اگر (st->state == TS_دودویی)
    {
	offset = sinfo_p->کم_offset + ((sinfo_p->بالا_offset
						- sinfo_p->کم_offset) / 2);
	اگر (offset == sinfo_p->curr_offset)
	    بازگشت TAGS_READ_EOF; // End the binary search without a match.
	وگرنه
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    وگرنه اگر (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	اگر (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    اگر (st->state == TS_دودویی || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	اگر (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    اگر (sinfo_p->curr_offset == sinfo_p->بالا_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->کم_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->کم_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	وقتی (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	اگر (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    بازگشت TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    وگرنه
    {
	// skip empty and blank lines
	انجامبده
	{
#ifdef FEAT_CSCOPE
	    اگر (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    وگرنه
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} وقتی (!eof && vim_isblankline(st->lbuf));

	اگر (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    اگر (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		بازگشت TAGS_READ_IGNORE;
#endif
	    بازگشت TAGS_READ_EOF;
	}
    }

    بازگشت TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ایستا صحیح
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    اگر (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	بازگشت TRUE;

    // Process the header line.
    اگر (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    اگر (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	برای (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    بازگشت FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ایستا صحیح
findtags_start_state_handler(
    findtags_state_T	*st,
    صحیح			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    صحیح		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    اگر (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	بازگشت findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    اگر (st->linear || use_cscope)
#وگرنه
    اگر (st->linear)
#endif
	st->state = TS_LINEAR;
    وگرنه اگر (st->tag_file_sorted == NUL)
	st->state = TS_دودویی;
    وگرنه اگر (st->tag_file_sorted == '1')
	st->state = TS_دودویی;
    وگرنه اگر (st->tag_file_sorted == '2')
    {
	st->state = TS_دودویی;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    وگرنه
	st->state = TS_LINEAR;

    اگر (st->state == TS_دودویی && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    اگر (st->state == TS_دودویی)
    {
	اگر (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	وگرنه
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->کم_offset = 0;
	    sinfo_p->کم_char = 0;
	    sinfo_p->بالا_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->بالا_char = 0xff;
	}
	بازگشت FALSE;
    }

    بازگشت TRUE;
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
    ایستا tagmatch_status_T
findtags_parse_سطر(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    صحیح		status;
    صحیح		i;
    صحیح		cmplen;
    صحیح		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    اگر (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	اگر (tagpp->tagname_end == پوچ)
	    // Corrupted tag line.
	    بازگشت TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (صحیح)(tagpp->tagname_end - tagpp->tagname);
	اگر (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	اگر ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	وگرنه اگر (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    بازگشت TAG_MATCH_NEXT;

	اگر (st->state == TS_دودویی)
	{
	    // Simplistic check for unsorted tags file.
	    i = (صحیح)tagpp->tagname[0];
	    اگر (margs->sortic)
		i = (صحیح)TOUPPER_ASC(tagpp->tagname[0]);
	    اگر (i < sinfo_p->کم_char || i > sinfo_p->بالا_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    اگر (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    وگرنه
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    اگر (tagcmp == 0)
	    {
		اگر (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		وگرنه اگر (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    اگر (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		بازگشت TAG_MATCH_NEXT;
	    }
	    اگر (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		اگر (sinfo_p->curr_offset < sinfo_p->بالا_offset)
		{
		    sinfo_p->کم_offset = sinfo_p->curr_offset;
		    اگر (margs->sortic)
			sinfo_p->کم_char = TOUPPER_ASC(tagpp->tagname[0]);
		    وگرنه
			sinfo_p->کم_char = tagpp->tagname[0];
		    بازگشت TAG_MATCH_NEXT;
		}
	    }
	    اگر (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->بالا_offset)
	    {
		sinfo_p->بالا_offset = sinfo_p->curr_offset;
		اگر (margs->sortic)
		    sinfo_p->بالا_char = TOUPPER_ASC(tagpp->tagname[0]);
		وگرنه
		    sinfo_p->بالا_char = tagpp->tagname[0];
		بازگشت TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    بازگشت TAG_MATCH_STOP;
	}
	وگرنه اگر (st->state == TS_SKIP_BACK)
	{
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    وگرنه
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    بازگشت TAG_MATCH_NEXT;
	}
	وگرنه اگر (st->state == TS_STEP_FORWARD)
	{
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		اگر ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    بازگشت TAG_MATCH_STOP;	// past last match
		وگرنه
		    بازگشت TAG_MATCH_NEXT;	// before first match
	    }
	}
	وگرنه
	    // skip this match if it can't match
	    اگر (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		بازگشت TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	اگر (tagpp->fname_end == پوچ)
	    status = FAIL;
	وگرنه
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    وگرنه
	status = parse_tag_سطر(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    اگر (status == FAIL)
	بازگشت TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    اگر (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    بازگشت TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ایستا تهی
findtags_matchargs_init(findtags_match_args_T *margs, صحیح flags)
{
    margs->matchoff = 0;			// match offset
    margs->match_re = FALSE;			// match with regexp
    margs->match_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & TAG_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->sort_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    ایستا صحیح
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    صحیح		match = FALSE;
    صحیح		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (صحیح)(tagpp->tagname_end - tagpp->tagname);
    اگر (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    اگر (st->orgpat->len != cmplen)
	match = FALSE;
    وگرنه
    {
	اگر (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    اگر (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	وگرنه
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    اگر (!match && st->orgpat->regmatch.regprog != پوچ)
    {
	صحیح	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	اگر (match)
	{
	    margs->matchoff = (صحیح)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    اگر (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->match_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_end = cc;
	margs->match_re = TRUE;
    }

    بازگشت match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ایستا تهی
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_سطر;
    صحیح		len;

    conv_سطر = string_convert(&st->vimconv, st->lbuf, پوچ);
    اگر (conv_سطر == پوچ)
	بازگشت;

    // Copy or swap lbuf and conv_line.
    len = (صحیح)STRLEN(conv_سطر) + 1;
    اگر (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_سطر;
	st->lbuf_size = len;
    }
    وگرنه
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
    ایستا صحیح
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    صحیح		name_only = (st->flags & TAG_NAMES);
    صحیح		mtt;
    صحیح		len = 0;
    صحیح		is_current;		// file name matches
    صحیح		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    اگر (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    وگرنه
#endif
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp->fname, tagpp->fname_end, st->tag_fname, buf_ffname);
#ifdef FEAT_EMACS_TAGS
	is_static = FALSE;
	اگر (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	اگر (is_static)
	{
	    اگر (is_current)
		mtt = MT_ST_CUR;
	    وگرنه
		mtt = MT_ST_OTH;
	}
	وگرنه
	{
	    اگر (is_current)
		mtt = MT_GL_CUR;
	    وگرنه
		mtt = MT_GL_OTH;
	}
	اگر (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	اگر (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    اگر (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#وگرنه
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (صحیح)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(اندازه(char_u) + len + 10 + ML_EXTRA + 1);
	اگر (mfp != پوچ)
	{
	    صحیح heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#ifdef FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#endif

	    heuristic = help_heuristic(tagpp->tagname,
				margs->match_re ? margs->matchoff : 0,
				!margs->match_no_ic);
#ifdef FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#endif
	    sprintf((نویسه *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    وگرنه اگر (name_only)
    {
	اگر (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    اگر (*temp_end == '/')
		وقتی (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    اگر (tagpp->command + 2 < temp_end)
	    {
		len = (صحیح)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		اگر (mfp != پوچ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    وگرنه
		mfp = پوچ;
	    st->get_searchpat = FALSE;
	}
	وگرنه
	{
	    len = (صحیح)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(اندازه(char_u) + len + 1);
	    اگر (mfp != پوچ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    اگر (State & MODE_درج)
		st->get_searchpat = p_sft;
	}
    }
    وگرنه
    {
	size_t tag_fname_len = STRLEN(st->tag_fname);
#ifdef FEAT_EMACS_TAGS
	size_t ebuf_len = 0;
#endif

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (صحیح)tag_fname_len + (صحیح)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	اگر (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (صحیح)ebuf_len + 1;
	}
	وگرنه
	    ++len;
#endif
	mfp = alloc(اندازه(char_u) + len + 1);
	اگر (mfp != پوچ)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->tag_fname);
#ifdef BACKSLASH_IN_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#endif
	    p[tag_fname_len + 1] = TAG_SEP;
	    s = p + 1 + tag_fname_len + 1;
#ifdef FEAT_EMACS_TAGS
	    اگر (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    وگرنه
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    اگر (mfp != پوچ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	اگر (use_cscope)
	    ++*hash;
	وگرنه
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	اگر (HASHITEM_EMPTY(hi))
	{
	    اگر (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		بازگشت FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_شمار++;
	}
	وگرنه
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    بازگشت OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ایستا تهی
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	جستجو_info;
    صحیح			retval;
#ifdef FEAT_CSCOPE
    صحیح			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(جستجو_info);

    // Read and parse the lines in the file one by one
    برای (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	اگر (st->state == TS_دودویی || st->state == TS_SKIP_BACK)
	    سطر_breakcheck();
	وگرنه
	    fast_breakcheck();
	اگر ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_کلیدها(30, FALSE);
	اگر (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    بشکن;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	اگر (st->mincount == TAG_MANY && st->match_شمار >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    بشکن;
	}
	اگر (st->get_searchpat)
	    برو سطر_read_in;

	retval = findtags_get_next_سطر(st, &جستجو_info);
	اگر (retval == TAGS_READ_IGNORE)
	    ادامه;
	اگر (retval == TAGS_READ_EOF)
	    بشکن;

سطر_read_in:

	اگر (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
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
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    ادامه;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	اگر (st->state == TS_START)
	{
	    اگر (findtags_start_state_handler(st, &margs->sortic, &جستجو_info) == FALSE)
		ادامه;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	اگر (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    اگر (st->lbuf == پوچ)
	    {
		اگر (st->fp != پوچ)
		    fclose(st->fp);
		st->fp = پوچ;
		st->stop_searching = TRUE;
		بازگشت;
	    }

	    اگر (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, جستجو_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    جستجو_info.curr_offset = 0;
	    ادامه;
	}

	retval = findtags_parse_سطر(st, &tagp, margs, &جستجو_info);
	اگر (retval == TAG_MATCH_NEXT)
	    ادامه;
	اگر (retval == TAG_MATCH_STOP)
	    بشکن;
	اگر (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    اگر (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    بازگشت;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	اگر (findtags_match_tag(st, &tagp, margs))
	{
	    اگر (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		بشکن;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ایستا تهی
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    صحیح		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = پوچ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    اگر (use_cscope)
	st->fp = پوچ;	    // avoid GCC warning
    وگرنه
#endif
    {
#ifdef FEAT_MULTI_LANG
	اگر (curbuf->b_help)
	{
	    اگر (!findtags_in_help_init(st))
		بازگشت;
	}
#endif

	st->fp = mch_fopen((نویسه *)st->tag_fname, "r");
	اگر (st->fp == پوچ)
	    بازگشت;

	اگر (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->tag_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_START;	// we're at the start of the file
#ifdef FEAT_EMACS_TAGS
    st->is_etag = FALSE;	// default is: not emacs style
#endif

    // Read and parse the lines in the file one by one
    findtags_get_all_tags(st, &margs, buf_ffname);

    اگر (st->fp != پوچ)
    {
	fclose(st->fp);
	st->fp = پوچ;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    اگر (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, پوچ, پوچ);

    اگر (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    اگر (st->match_شمار >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ایستا صحیح
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    صحیح		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    صحیح		mtt;
    صحیح		i;
    char_u	*mfp;
    char_u	*p;

    اگر (st->match_شمار > 0)
	matches = ALLOC_MULT(char_u *, st->match_شمار);
    وگرنه
	matches = پوچ;
    st->match_شمار = 0;
    برای (mtt = 0; mtt < MT_شمار; ++mtt)
    {
	برای (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    اگر (matches == پوچ)
		vim_free(mfp);
	    وگرنه
	    {
		اگر (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    برای (p = mfp + 1; *p != NUL; ++p)
			اگر (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_شمار++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    بازگشت st->match_شمار;
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
find_tags(
    char_u	*pat,			// pattern to search for
    صحیح		*رقم_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    صحیح		flags,
    صحیح		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    صحیح		first_file;		// trying first tag file
    صحیح		retval = FAIL;		// return value
    صحیح		round;

    صحیح		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    صحیح		i;
    char_u	*saved_pat = پوچ;		// copy of pat[]
#endif

    صحیح		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    صحیح		has_re = (flags & TAG_REGEXP);	// regexp used
    صحیح		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    صحیح		use_cscope = (flags & TAG_CSCOPE);
#endif
    صحیح		verbose = (flags & TAG_VERBOSE);
    صحیح		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    انتخاب (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	حالت TC_FOLLOWIC:		 بشکن;
	حالت TC_IGNORE:    p_ic = TRUE;  بشکن;
	حالت TC_MATCH:     p_ic = FALSE; بشکن;
	حالت TC_FOLLOWSCS: p_ic = ignorecase(pat); بشکن;
	حالت TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); بشکن;
    }

    help_save = curbuf->b_help;

    اگر (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	برو findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    اگر (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    وگرنه اگر (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    اگر (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	اگر (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    اگر (saved_pat != پوچ)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    اگر (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    اگر (has_re && st.orgpat->regmatch.regprog == پوچ)
	برو findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    اگر (retval != NOTDONE)
	برو findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    اگر ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == پوچ
	    && curbuf->b_fname != پوچ
	    && (i = (صحیح)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#endif

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
    برای (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      برای (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  اگر (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      بشکن;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	اگر (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	اگر (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    بشکن;
#ifdef FEAT_CSCOPE
	اگر (use_cscope)
	    بشکن;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    اگر (!st.stop_searching)
    {
	اگر (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    اگر (retval == FAIL)
	st.match_شمار = 0;

    *رقم_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    بازگشت retval;
}

ایستا garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ایستا تهی
found_tagfile_cb(char_u *fname, تهی *cookie UNUSED)
{
    اگر (ga_grow(&tag_fnames, 1) == FAIL)
	بازگشت;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#اگر defined(EXITFREE)
    تهی
free_tag_stuff(تهی)
{
    ga_clear_strings(&tag_fnames);
    اگر (curwin != پوچ)
	do_tag(پوچ, DT_FREE, 0, 0, 0);
    tag_freematch();

# اگر defined(FEAT_QUICKFIX)
    tagstack_clear_entry(&ptag_entry);
# endif
}
#endif

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    صحیح
get_tagfname(
    tagname_T	*tnp,	// holds status info
    صحیح		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = پوچ;
    char_u		*r_ptr;
    صحیح			i;

    اگر (first)
	CLEAR_POINTER(tnp);

    اگر (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	اگر (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, اندازه(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# وگرنه
		    "doc/tags doc/tags-??"
# endif
#وگرنه
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, پوچ);
	}

	اگر (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    اگر (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		بازگشت FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    برای (i = 0; i < tag_fnames.ga_len; ++i)
		اگر (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    بازگشت FAIL; // avoid duplicate file names
	}
	وگرنه
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	بازگشت OK;
    }

    اگر (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	اگر (tnp->tn_tags == پوچ)
	    بازگشت FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    برای (;;)
    {
	اگر (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_جستجو_ctx);
	    اگر (fname != پوچ)
		بشکن;

	    tnp->tn_did_filefind_init = FALSE;
	}
	وگرنه
	{
	    char_u  *filename = پوچ;

	    // Stop when used all parts of 'tags'.
	    اگر (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_جستجو_ctx);
		tnp->tn_جستجو_ctx = پوچ;
		بازگشت FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (تهی)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    اگر (r_ptr != پوچ)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_جستجو_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_جستجو_ctx, TRUE, curbuf->b_ffname);
	    اگر (tnp->tn_جستجو_ctx != پوچ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    بازگشت OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    تهی
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_جستجو_ctx);
    tnp->tn_جستجو_ctx = پوچ;
    ga_clear_strings(&tag_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    ایستا صحیح
parse_tag_سطر(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    صحیح		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    اگر (is_etag)
	// emacs-style tag file
	بازگشت emacs_tags_parse_سطر(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    اگر (p == پوچ)
	بازگشت FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    اگر (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    اگر (p == پوچ)
	بازگشت FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    اگر (*p != NUL)
	++p;
    اگر (*p == NUL)
	بازگشت FAIL;
    tagp->command = p;

    بازگشت OK;
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
    ایستا صحیح
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    وقتی ((p = vim_strchr(p, '\t')) != پوچ)
    {
	++p;
	اگر (STRNCMP(p, "file:", 5) == 0)
	    بازگشت TRUE;
    }

    بازگشت FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ایستا size_t
matching_سطر_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    بازگشت (p - lbuf) + STRLEN(p);
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
    ایستا صحیح
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    صحیح		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    اگر (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    وگرنه
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_سطر(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = پوچ;
    tagp->user_data = پوچ;
    tagp->tagline = 0;
    tagp->command_end = پوچ;

    اگر (retval != OK)
	بازگشت retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    اگر (find_extra(&p) == OK)
    {
	اگر (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	وگرنه
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	اگر (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    وقتی (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		اگر (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		وگرنه اگر (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		وگرنه اگر (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((نویسه *)p + 5);
		اگر (tagp->tagkind != پوچ && tagp->user_data != پوچ)
		    بشکن;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		اگر (pc == پوچ || (pt != پوچ && pc > pt))
		    tagp->tagkind = p;
		اگر (pt == پوچ)
		    بشکن;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    اگر (tagp->tagkind != پوچ)
    {
	برای (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    اگر (tagp->user_data != پوچ)
    {
	برای (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    بازگشت retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ایستا char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    صحیح		c;

#ifdef FEAT_EMACS_TAGS
    اگر (tagp->is_etag)
	c = 0;	    // to shut up GCC
    وگرنه
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    اگر (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    بازگشت fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ایستا صحیح
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    صحیح		forceit,	// :ta with !
    صحیح		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    صحیح		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = پوچ;
    char_u	*fname;
    tagptrs_T	tagp;
    صحیح		retval = FAIL;
    صحیح		getfile_result = GETFILE_UNUSED;
    صحیح		جستجو_options;
#ifdef FEAT_جستجو_EXTRA
    صحیح		save_no_hlsearch;
#endif
#اگر defined(FEAT_QUICKFIX)
    win_T	*curwin_save = پوچ;
#endif
    char_u	*full_fname = پوچ;
#ifdef FEAT_FOLDING
    صحیح		old_کلید_Typed = کلید_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    صحیح		isdigit = FALSE;

    اگر (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	بازگشت FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_سطر_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    اگر (lbuf != پوچ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    اگر (pbuf == پوچ || lbuf == پوچ || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = پوچ;
	برو erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    اگر (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    برای (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	اگر (tagp.is_etag && *str == ',')// stop at ',' after line number
	    بشکن;
#endif
	*pbuf_end++ = *str++;
	اگر (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    بشکن;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    اگر (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	اگر (isdigit)
	    str++;
	اگر (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    اگر (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    وگرنه
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    اگر (fname == پوچ)
	برو erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    اگر (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, پوچ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	اگر (nofile_fname == پوچ)
	    nofile_fname = empty_option;
	برو erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#اگر defined(FEAT_QUICKFIX)
    اگر (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	اگر (!curwin->w_p_pvw)
	{
	    full_fname = FullName_save(fname, FALSE);
	    fname = full_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    اگر (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	اگر (existing_buf != پوچ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    اگر (swbuf_goto_win_with_buf(existing_buf) != پوچ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    اگر (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	اگر (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	اگر ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	اگر (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    اگر (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    برو erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    اگر (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#اگر defined(FEAT_QUICKFIX)
	اگر (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	وگرنه
#endif
	    keep_help_flag = curbuf->b_help;
    }

    اگر (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, پوچ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    اگر (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_جستجو_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	اگر (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	اگر (vim_strchr(p_cpo, CPO_TAGPAT) != پوچ)
	    جستجو_options = 0;
	وگرنه
	    جستجو_options = جستجو_KEEP;

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
	اگر (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

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
	    وگرنه
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    اگر (do_جستجو(پوچ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 جستجو_options, پوچ))
		retval = OK;
	    وگرنه
	    {
		صحیح	found = 1;
		صحیح	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		اگر (!do_جستجو(پوچ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 جستجو_options, پوچ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (تهی)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((نویسه *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    اگر (!do_جستجو(پوچ, '/', '/', pbuf, pbuflen, (long)1,
							 جستجو_options, پوچ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((نویسه *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			اگر (!do_جستجو(پوچ, '/', '/', pbuf, pbuflen, (long)1,
							 جستجو_options, پوچ))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		اگر (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		وگرنه
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    اگر (found == 2 || !save_p_ic)
		    {
			پیام(_(e_couldnt_find_tag_just_guessing));
			اگر (!پیام_scrolled && پیام_silent == 0)
			{
			    out_flush();
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
	وگرنه
	{
	    صحیح		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#ifdef HAVE_SANDBOX
	    ++sandbox;
#endif
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    اگر (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_جستجو_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	اگر (جستجو_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	اگر (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	اگر (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    اگر (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    اگر ((fdo_flags & FDO_TAG) && old_کلید_Typed)
		foldOpenCursor();
#endif
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
#endif

	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    وگرنه
    {
	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	اگر (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	وگرنه اگر (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    اگر (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#اگر defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    اگر (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#اگر defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    بازگشت retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ایستا char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, صحیح expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = پوچ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    اگر (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       بازگشت پوچ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    اگر (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == پوچ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, پوچ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	اگر (expanded_fname != پوچ)
	    fname = expanded_fname;
    }

    اگر ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	اگر (retval != پوچ)
	{
	    STRCPY(retval, tag_fname);
	    vim_strncpy(retval + (p - tag_fname), fname,
					      MAXPATHL - (p - tag_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    وگرنه
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    بازگشت retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ایستا صحیح
test_for_current(
#ifdef FEAT_EMACS_TAGS
    صحیح	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    صحیح	    c;
    صحیح	    retval = FALSE;
    char_u  *fullname;

    اگر (buf_ffname != پوچ)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	اگر (is_etag)
	    c = 0;	    // to shut up GCC
	وگرنه
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	اگر (fullname != پوچ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	اگر (!is_etag)
#endif
	    *fname_end = c;
    }

    بازگشت retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ایستا صحیح
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    برای (;;)
    {
	اگر (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	وگرنه اگر (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    اگر (*str != first_char)
		str = پوچ;
	    وگرنه
		++str;
	}
	وگرنه
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((نویسه *)str, "|;\"");
	    اگر (str != پوچ)
	    {
		++str;
		بشکن;
	    }

	}
	اگر (str == پوچ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    بشکن;
	++str;	// skip ';'
	first_char = *str;
    }

    اگر (str != پوچ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	بازگشت OK;
    }
    بازگشت FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    تهی
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    صحیح
expand_tags(
    صحیح		tagnames,	// expand tag names
    char_u	*pat,
    صحیح		*رقم_file,
    char_u	***file)
{
    صحیح		i;
    صحیح		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    صحیح		ret;

    name_buf = alloc(name_buf_size);
    اگر (name_buf == پوچ)
	بازگشت FAIL;

    اگر (tagnames)
	extra_flag = TAG_NAMES;
    وگرنه
	extra_flag = 0;
    اگر (pat[0] == '/')
	ret = find_tags(pat + 1, رقم_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    وگرنه
	ret = find_tags(pat, رقم_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    اگر (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 برای (i = 0; i < *رقم_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     اگر (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 اگر (buf == پوچ)
		 {
		     vim_free(name_buf);
		     بازگشت FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != پوچ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    بازگشت ret;
}

#اگر defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ایستا صحیح
add_tag_field(
    dict_T  *dict,
    نویسه    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    صحیح		len = 0;
    صحیح		retval;

    // check that the field name doesn't exist yet
    اگر (dict_has_کلید(dict, field_name))
    {
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	بازگشت FAIL;
    }
    buf = alloc(MAXPATHL);
    اگر (buf == پوچ)
	بازگشت FAIL;
    اگر (start != پوچ)
    {
	اگر (end == پوچ)
	{
	    end = start + STRLEN(start);
	    وقتی (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (صحیح)(end - start);
	اگر (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    بازگشت retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    صحیح
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    صحیح		رقم_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &رقم_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (صحیح)MAXCOL, buf_fname);
    اگر (ret != OK || رقم_matches <= 0)
	بازگشت ret;

    برای (i = 0; i < رقم_matches; ++i)
    {
	اگر (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ادامه;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	اگر (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ادامه;
	}

	اگر ((dict = dict_alloc()) == پوچ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    بشکن;
	}
	اگر (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ادامه;
	}

	full_fname = tag_full_fname(&tp);
	اگر (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    پوچ) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_عدد(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	اگر (tp.command_end != پوچ)
	{
	    برای (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		اگر (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		وگرنه اگر (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		وگرنه اگر (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    صحیح	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    وقتی (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (صحیح)(p - n);
		    اگر (*p == ':' && len > 0)
		    {
			s = ++p;
			وقتی (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			اگر (add_tag_field(dict, (نویسه *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    وگرنه
			// Skip field without colon.
			وقتی (*p != NUL && *p >= ' ')
			    ++p;
		    اگر (*p == NUL)
			بشکن;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    بازگشت ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ایستا تهی
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_عدد(retdict, "matchnr", tag->cur_match + 1);
    dict_add_عدد(retdict, "bufnr", tag->cur_fnum);
    اگر (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    اگر ((pos = list_alloc_id(aid_tagstack_from)) == پوچ)
	بازگشت;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_عدد(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_عدد(pos, (varnumber_T)fmark->mark.lnum);
    list_append_عدد(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_عدد(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    تهی
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    صحیح		i;
    dict_T	*d;

    dict_add_عدد(retdict, "length", wp->w_tagstacklen);
    dict_add_عدد(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_اقلام);
    اگر (l == پوچ)
	بازگشت;
    dict_add_list(retdict, "items", l);

    برای (i = 0; i < wp->w_tagstacklen; i++)
    {
	اگر ((d = dict_alloc_id(aid_tagstack_details)) == پوچ)
	    بازگشت;
	اگر (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    بازگشت;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ایستا تهی
tagstack_clear(win_T *wp)
{
    صحیح i;

    // Free the current tag stack
    برای (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ایستا تهی
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحیح		i;

    tagstack_clear_entry(&tagstack[0]);
    برای (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ایستا تهی
tagstack_افزودن_item(
	win_T	*wp,
	char_u	*tagname,
	صحیح	cur_fnum,
	صحیح	cur_match,
	pos_T	mark,
	صحیح	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    صحیح		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    اگر (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    اگر (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ایستا تهی
tagstack_افزودن_اقلام(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    صحیح		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_اقلام(l, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == پوچ)
	    ادامه;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	اگر ((di = dict_find(itemdict, (char_u *)"from", -1)) == پوچ)
	    ادامه;
	اگر (list2fpos(&di->di_tv, &mark, &fnum, پوچ, FALSE) != OK)
	    ادامه;
	اگر ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == پوچ)
	    ادامه;

	اگر (mark.col > 0)
	    mark.col--;
	tagstack_افزودن_item(wp, tagname,
		(صحیح)dict_get_عدد(itemdict, "bufnr"),
		(صحیح)dict_get_عدد(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ایستا تهی
tagstack_set_curidx(win_T *wp, صحیح curidx)
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
set_tagstack(win_T *wp, dict_T *d, صحیح action)
{
    dictitem_T	*di;
    list_T	*l = پوچ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    اگر (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_پشته_within_tagfunc));
	بازگشت FAIL;
    }
# endif

    اگر ((di = dict_find(d, (char_u *)"items", -1)) != پوچ)
    {
	اگر (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    بازگشت FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    اگر ((di = dict_find(d, (char_u *)"curidx", -1)) != پوچ)
	tagstack_set_curidx(wp, (صحیح)tv_get_عدد(&di->di_tv) - 1);

    اگر (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	صحیح	tagstackidx = wp->w_tagstackidx;
	صحیح	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	وقتی (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    اگر (l != پوچ)
    {
	اگر (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_افزودن_اقلام(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    بازگشت OK;
}
#endif
