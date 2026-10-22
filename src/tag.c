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

#অন্তর্ভাব "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
প্রকারনির্দেশ সংরচনা tag_pointers
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
    পূর্ণাঙ্ক		is_etag;	// TRUE for emacs tag
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
প্রকারনির্দেশ গণনা
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
প্রকারনির্দেশ গণনা
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_দ্বিমিক,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
প্রকারনির্দেশ সংরচনা
{
    off_T	নিচের_offset;	// offset for first char of first line that
				// could match
    off_T	উপরের_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    পূর্ণাঙ্ক	নিচের_char;		// first char at low_offset
    পূর্ণাঙ্ক	উপরের_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
প্রকারনির্দেশ গণনা
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক	matchoff;		// tag match offset
    পূর্ণাঙ্ক	match_re;		// TRUE if the tag matches a regexp
    পূর্ণাঙ্ক	match_no_ic;		// TRUE if the tag matches with case
    পূর্ণাঙ্ক	has_re;			// regular expression used
    পূর্ণাঙ্ক	sortic;			// tags file sorted ignoring case (foldcase)
    পূর্ণাঙ্ক	sort_error;		// tags file not sorted
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
#define MT_গুনতি	16

স্থির বর্ণ	*mt_names[MT_গুনতি/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
স্থির char_u	*nofile_fname = রিক্ত;	// fname for NOTAGFILE error

স্থির শূন্য taglen_advance(পূর্ণাঙ্ক l);

স্থির পূর্ণাঙ্ক jumpto_tag(char_u *lbuf, পূর্ণাঙ্ক forceit, পূর্ণাঙ্ক keep_help);
#ifdef FEAT_EMACS_TAGS
স্থির পূর্ণাঙ্ক parse_tag_সারি(char_u *lbuf, পূর্ণাঙ্ক is_etag, tagptrs_T *tagp);
#অন্যথা
স্থির পূর্ণাঙ্ক parse_tag_সারি(char_u *lbuf, tagptrs_T *tagp);
#endif
স্থির পূর্ণাঙ্ক test_for_static(tagptrs_T *);
স্থির পূর্ণাঙ্ক parse_match(char_u *lbuf, tagptrs_T *tagp);
স্থির char_u *tag_full_fname(tagptrs_T *tagp);
স্থির char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, পূর্ণাঙ্ক expand);
#ifdef FEAT_EMACS_TAGS
স্থির পূর্ণাঙ্ক test_for_current(পূর্ণাঙ্ক, char_u *, char_u *, char_u *, char_u *);
#অন্যথা
স্থির পূর্ণাঙ্ক test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
স্থির পূর্ণাঙ্ক find_extra(char_u **pp);
স্থির শূন্য print_tag_list(পূর্ণাঙ্ক new_tag, পূর্ণাঙ্ক use_tagstack, পূর্ণাঙ্ক অঙ্ক_matches, char_u **matches);
#যদি defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
স্থির পূর্ণাঙ্ক add_llist_tags(char_u *tag, পূর্ণাঙ্ক অঙ্ক_matches, char_u **matches);
#endif

স্থির char_u	*tagmatchname = রিক্ত;	// name of last used tag

#যদি defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
স্থির taggy_T ptag_entry = {রিক্ত, {{0, 0, 0}, 0}, 0, 0, রিক্ত};
#endif

#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
স্থির callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#যদি defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    বর্ণ *
did_set_tagfunc(optset_T *args)
{
    পূর্ণাঙ্ক	retval;

    যদি (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    অন্যথা
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	যদি (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    প্রত্যাগম retval == FAIL ? e_invalid_argument : রিক্ত;
}
#endif

#যদি defined(EXITFREE)
    শূন্য
free_tagfunc_option(শূন্য)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    পূর্ণাঙ্ক
set_ref_in_tagfunc(পূর্ণাঙ্ক copyID UNUSED)
{
    পূর্ণাঙ্ক	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    প্রত্যাগম abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    শূন্য
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    যদি (tfu_cb.cb_name != রিক্ত && *tfu_cb.cb_name != NUL)
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
    পূর্ণাঙ্ক
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    পূর্ণাঙ্ক		type,
    পূর্ণাঙ্ক		গুনতি,
    পূর্ণাঙ্ক		forceit,	// :ta with !
    পূর্ণাঙ্ক		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    পূর্ণাঙ্ক		tagstackidx = curwin->w_tagstackidx;
    পূর্ণাঙ্ক		tagstacklen = curwin->w_tagstacklen;
    পূর্ণাঙ্ক		cur_match = 0;
    পূর্ণাঙ্ক		cur_fnum = curbuf->b_fnum;
    পূর্ণাঙ্ক		oldtagstackidx = tagstackidx;
    পূর্ণাঙ্ক		prevtagstackidx = tagstackidx;
    পূর্ণাঙ্ক		prev_অঙ্ক_matches;
    পূর্ণাঙ্ক		new_tag = FALSE;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ic;
    পূর্ণাঙ্ক		no_regexp = FALSE;
    পূর্ণাঙ্ক		error_cur_match = 0;
    পূর্ণাঙ্ক		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক		jumped_to_tag = FALSE;
#endif
    পূর্ণাঙ্ক		new_অঙ্ক_matches;
    char_u	**new_matches;
    পূর্ণাঙ্ক		use_tagstack;
    পূর্ণাঙ্ক		skip_বার্তা = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    পূর্ণাঙ্ক		use_tfu = 1;
    char_u	*tofree = রিক্ত;

    // remember the matches for the last used tag
    স্থির পূর্ণাঙ্ক		অঙ্ক_matches = 0;
    স্থির পূর্ণাঙ্ক		max_অঙ্ক_matches = 0;  // limit used for match search
    স্থির char_u	**matches = রিক্ত;
    স্থির পূর্ণাঙ্ক		flags;

#ifdef EXITFREE
    যদি (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(অঙ্ক_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	অঙ্ক_matches = 0;
	প্রত্যাগম FALSE;
    }
#endif

#ifdef FEAT_EVAL
    যদি (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_স্তূপ_within_tagfunc));
	প্রত্যাগম FALSE;
    }
#endif

    যদি (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	প্রত্যাগম FALSE;

    যদি (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_অঙ্ক_matches = অঙ্ক_matches;
    free_string_option(nofile_fname);
    nofile_fname = রিক্ত;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    যদি ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#যদি defined(FEAT_QUICKFIX)
	যদি (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    যদি ((ptag_entry.tagname = vim_strsave(tag)) == রিক্ত)
		গচ্ছ end_do_tag;
	}
#endif
    }
    অন্যথা
    {
#যদি defined(FEAT_QUICKFIX)
	যদি (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	অন্যথা
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	যদি (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#যদি defined(FEAT_QUICKFIX)
	    যদি (g_do_tagpreview != 0)
	    {
		যদি (ptag_entry.tagname != রিক্ত
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		অন্যথা
		{
		    tagstack_clear_entry(&ptag_entry);
		    যদি ((ptag_entry.tagname = vim_strsave(tag)) == রিক্ত)
			গচ্ছ end_do_tag;
		}
	    }
	    অন্যথা
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		যাবত্ (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		যদি (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    প্রতি (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = রিক্ত;
		}

		/*
		 * put the tag name in the tag stack
		 */
		যদি ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == রিক্ত)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    গচ্ছ end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	অন্যথা
	{
	    যদি (
#যদি defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == রিক্ত :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_স্তূপ_empty));
		গচ্ছ end_do_tag;
	    }

	    যদি (type == DT_বের_করো)		// go to older position
	    {
#ifdef FEAT_FOLDING
		পূর্ণাঙ্ক	old_চাবি_Typed = চাবি_Typed;
#endif
		যদি ((tagstackidx -= গুনতি) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_স্তূপ));
		    যদি (tagstackidx + গুনতি == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			গচ্ছ end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		অন্যথা যদি (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_শীর্ষ_of_tag_স্তূপ));
		    গচ্ছ end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		যদি (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    যদি (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			গচ্ছ end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		অন্যথা
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		যদি ((fdo_flags & FDO_TAG) && old_চাবি_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(অঙ্ক_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		অঙ্ক_matches = 0;
		tag_freematch();
		গচ্ছ end_do_tag;
	    }

	    যদি (type == DT_TAG
#যদি defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#যদি defined(FEAT_QUICKFIX)
		যদি (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		অন্যথা
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    যদি ((tagstackidx += গুনতি - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_শীর্ষ_of_tag_স্তূপ));
			save_pos = FALSE;
		    }
		    অন্যথা যদি (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_স্তূপ));
			tagstackidx = 0;
			গচ্ছ end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    অন্যথা				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#যদি defined(FEAT_QUICKFIX)
		যদি (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		অন্যথা
#endif
		{
		    যদি (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		বিকল্প (type)
		{
		    পক্ষ DT_FIRST: cur_match = গুনতি - 1; ভঙ্গ;
		    পক্ষ DT_SELECT:
		    পক্ষ DT_JUMP:
#ifdef FEAT_CSCOPE
		    পক্ষ DT_CSCOPE:
#endif
		    পক্ষ DT_LAST:  cur_match = MAXCOL - 1; ভঙ্গ;
		    পক্ষ DT_NEXT:  cur_match += গুনতি; ভঙ্গ;
		    পক্ষ DT_PREV:  cur_match -= গুনতি; ভঙ্গ;
		}
		যদি (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		অন্যথা যদি (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_বার্তা = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#যদি defined(FEAT_QUICKFIX)
	যদি (g_do_tagpreview != 0)
	{
	    যদি (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	অন্যথা
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    যদি (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    যদি (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    যদি (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	যদি (buf != রিক্ত)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    প্রতি (;;)
    {
	পূর্ণাঙ্ক	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	যদি (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#যদি defined(FEAT_QUICKFIX)
	অন্যথা যদি (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	অন্যথা
	    name = tag;
	other_name = (tagmatchname == রিক্ত || STRCMP(tagmatchname, name) != 0);
	যদি (new_tag
		|| (cur_match >= অঙ্ক_matches && max_অঙ্ক_matches != MAXCOL)
		|| other_name)
	{
	    যদি (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    যদি (type == DT_SELECT || type == DT_JUMP
#যদি defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    যদি (type == DT_TAG)
		max_অঙ্ক_matches = MAXCOL;
	    অন্যথা
		max_অঙ্ক_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    যদি (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    অন্যথা
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    যদি (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    যদি (verbose)
		flags |= TAG_VERBOSE;

	    যদি (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    যদি (find_tags(name, &new_অঙ্ক_matches, &new_matches, flags,
					    max_অঙ্ক_matches, buf_ffname) == OK
		    && new_অঙ্ক_matches < max_অঙ্ক_matches)
		max_অঙ্ক_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    যদি (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_অঙ্ক_matches, new_matches);
		ভঙ্গ;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    যদি (!new_tag && !other_name)
	    {
		পূর্ণাঙ্ক	    j, k;
		পূর্ণাঙ্ক	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		প্রতি (j = 0; j < অঙ্ক_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    প্রতি (i = idx; i < new_অঙ্ক_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			যদি (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    প্রতি (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ভঙ্গ;
			}
		    }
		}
	    }
	    FreeWild(অঙ্ক_matches, matches);
	    অঙ্ক_matches = new_অঙ্ক_matches;
	    matches = new_matches;
	}

	যদি (অঙ্ক_matches <= 0)
	{
	    যদি (verbose)
		semsg(_(e_tag_not_found_str), name);
#যদি defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক ask_for_selection = FALSE;
#যদি defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    যদি (type == DT_CSCOPE && অঙ্ক_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    অন্যথা
#endif
	    যদি (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = গুনতি > 0 ? গুনতি - 1 : 0;
	    অন্যথা যদি (type == DT_SELECT || (type == DT_JUMP && অঙ্ক_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, অঙ্ক_matches, matches);
		ask_for_selection = TRUE;
	    }
#যদি defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    অন্যথা যদি (type == DT_LTAG)
	    {
		যদি (add_llist_tags(tag, অঙ্ক_matches, matches) == FAIL)
		    গচ্ছ end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    যদি (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_সংখ্যা(রিক্ত);
		যদি (i <= 0 || i > অঙ্ক_matches || got_int)
		{
		    // no valid choice: don't change anything
		    যদি (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    ভঙ্গ;
		}
		cur_match = i - 1;
	    }

	    যদি (cur_match >= অঙ্ক_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		যদি ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == রিক্ত)
		{
		    যদি (অঙ্ক_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    অন্যথা
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_বার্তা = TRUE;
		}
		cur_match = অঙ্ক_matches - 1;
	    }
	    যদি (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		যদি (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#যদি defined(FEAT_QUICKFIX)
	    অন্যথা যদি (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    যদি (nofile_fname != রিক্ত && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    যদি (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (অঙ্ক_matches > 1 || ic)
		&& !skip_বার্তা)
	    {
		// Give an indication of the number of matching tags
		sprintf((বর্ণ *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				অঙ্ক_matches,
				max_অঙ্ক_matches != MAXCOL ? _(" or more") : "");
		যদি (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		যদি ((অঙ্ক_matches > prev_অঙ্ক_matches || new_tag)
							   && অঙ্ক_matches > 1)
		{
		    যদি (ic)
			বার্তা_attr((বর্ণ *)IObuff, HL_ATTR(HLF_W));
		    অন্যথা
			বার্তা((বর্ণ *)IObuff);
		    বার্তা_scroll = TRUE;	// don't overwrite this message
		}
		অন্যথা
		    give_warning(IObuff, ic);
		যদি (ic && !বার্তা_scrolled && বার্তা_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#যদি defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((বর্ণ *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (পূর্ণাঙ্ক)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#যদি defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, রিক্ত, -1);
#endif

	    যদি (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		যদি ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_অঙ্ক_matches != MAXCOL
					     || cur_match < অঙ্ক_matches - 1)))
		{
		    error_cur_match = cur_match;
		    যদি (use_tagstack)
			--tagstackidx;
		    যদি (type == DT_PREV)
			--cur_match;
		    অন্যথা
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    অনুবর্ত;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    অন্যথা
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		যদি (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	ভঙ্গ;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    যদি (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    প্রত্যাগম jumped_to_tag;
#অন্যথা
    প্রত্যাগম FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    স্থির শূন্য
print_tag_list(
    পূর্ণাঙ্ক		new_tag,
    পূর্ণাঙ্ক		use_tagstack,
    পূর্ণাঙ্ক		অঙ্ক_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    পূর্ণাঙ্ক		tagstackidx = curwin->w_tagstackidx;
    পূর্ণাঙ্ক		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    পূর্ণাঙ্ক		taglen;
    পূর্ণাঙ্ক		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (পূর্ণাঙ্ক)(tagp.tagname_end - tagp.tagname + 2);
    যদি (taglen < 18)
	taglen = 18;
    যদি (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    যদি (বার্তা_col == 0)
	বার্তা_didout = FALSE;	// overwrite previous message
    বার্তা_start();
    বার্তা_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    বার্তা_clr_eos();
    taglen_advance(taglen);
    বার্তা_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    প্রতি (i = 0; i < অঙ্ক_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	যদি (!new_tag && (
#যদি defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	অন্যথা
	    *IObuff = ' ';
	vim_snprintf((বর্ণ *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	বার্তা_puts((বর্ণ *)IObuff);
	যদি (tagp.tagkind != রিক্ত)
	    বার্তা_outtrans_len(tagp.tagkind,
			  (পূর্ণাঙ্ক)(tagp.tagkind_end - tagp.tagkind));
	বার্তা_advance(13);
	বার্তা_outtrans_len_attr(tagp.tagname,
			   (পূর্ণাঙ্ক)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	বার্তা_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	যদি (p != রিক্ত)
	{
	    বার্তা_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	যদি (বার্তা_col > 0)
	    বার্তা_putchar('\n');
	যদি (got_int)
	    ভঙ্গ;
	বার্তা_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	যদি (command_end != রিক্ত)
	{
	    p = command_end + 3;
	    যাবত্ (*p && *p != '\r' && *p != '\n')
	    {
		যাবত্ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		যদি (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    অনুবর্ত;
		}
		// skip "kind:<kind>" and "<kind>"
		যদি (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    অনুবর্ত;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		যাবত্ (*p && *p != '\r' && *p != '\n')
		{
		    যদি (বার্তা_col + ptr2cells(p) >= cmdline_width)
		    {
			বার্তা_putchar('\n');
			যদি (got_int)
			    ভঙ্গ;
			বার্তা_advance(15);
		    }
		    p = বার্তা_outtrans_one(p, attr);
		    যদি (*p == TAB)
		    {
			বার্তা_puts_attr(" ", attr);
			ভঙ্গ;
		    }
		    যদি (*p == ':')
			attr = 0;
		}
	    }
	    যদি (বার্তা_col > 15)
	    {
		বার্তা_putchar('\n');
		যদি (got_int)
		    ভঙ্গ;
		বার্তা_advance(15);
	    }
	}
	অন্যথা
	{
	    প্রতি (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	যদি (*p == '/' || *p == '?')
	{
	    ++p;
	    যদি (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	যাবত্ (p != command_end && vim_isspace(*p))
	    ++p;

	যাবত্ (p != command_end)
	{
	    যদি (বার্তা_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		বার্তা_putchar('\n');
	    যদি (got_int)
		ভঙ্গ;
	    বার্তা_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    যদি (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    যদি (*p == TAB)
	    {
		বার্তা_putchar(' ');
		++p;
	    }
	    অন্যথা
		p = বার্তা_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    যদি (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ভঙ্গ;
	    // don't display matching '/' or '?'
	    যদি (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ভঙ্গ;
	}
	যদি (বার্তা_col)
	    বার্তা_putchar('\n');
	ui_breakcheck();
    }
    যদি (got_int)
	got_int = FALSE;	// only stop the listing
}

#যদি defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    স্থির পূর্ণাঙ্ক
add_llist_tags(
    char_u	*tag,
    পূর্ণাঙ্ক		অঙ্ক_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    পূর্ণাঙ্ক		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    যদি (list == রিক্ত || fname == রিক্ত || cmd == রিক্ত)
    {
	vim_free(cmd);
	vim_free(fname);
	যদি (list != রিক্ত)
	    list_free(list);
	প্রত্যাগম FAIL;
    }

    প্রতি (i = 0; i < অঙ্ক_matches; ++i)
    {
	পূর্ণাঙ্ক	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (পূর্ণাঙ্ক)(tagp.tagname_end - tagp.tagname);
	যদি (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	যদি (p == রিক্ত)
	    অনুবর্ত;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	যদি (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((বর্ণ *)tagp.command);
	অন্যথা
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    যদি (cmd_end == রিক্ত)
	    {
		প্রতি (p = tagp.command;
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
	    যদি (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    যদি (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    যদি (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (পূর্ণাঙ্ক)(cmd_end - cmd_start + 1);
	    যদি (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    যদি (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	যদি ((dict = dict_alloc()) == রিক্ত)
	    অনুবর্ত;
	যদি (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    অনুবর্ত;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_সংখ্যা(dict, "lnum", lnum);
	যদি (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, রিক্ত);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    প্রত্যাগম OK;
}
#endif

/*
 * Free cached tags.
 */
    শূন্য
tag_freematch(শূন্য)
{
    VIM_CLEAR(tagmatchname);
}

    স্থির শূন্য
taglen_advance(পূর্ণাঙ্ক l)
{
    যদি (l == MAXCOL)
    {
	বার্তা_putchar('\n');
	বার্তা_advance(24);
    }
    অন্যথা
	বার্তা_advance(13 + l);
}

/*
 * Print the tag stack
 */
    শূন্য
do_tags(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    পূর্ণাঙ্ক		tagstackidx = curwin->w_tagstackidx;
    পূর্ণাঙ্ক		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    বার্তা_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    প্রতি (i = 0; i < tagstacklen; ++i)
    {
	যদি (tagstack[i].tagname != রিক্ত)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    যদি (name == রিক্ত)	    // file name not available
		অনুবর্ত;

	    বার্তা_putchar('\n');
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    বার্তা_outtrans(IObuff);
	    বার্তা_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    যদি (tagstackidx == tagstacklen)	// idx at top of stack
	বার্তা_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    স্থির পূর্ণাঙ্ক
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    পূর্ণাঙ্ক		i;

    যাবত্ (len > 0)
    {
	i = (পূর্ণাঙ্ক)TOUPPER_ASC(*s1) - (পূর্ণাঙ্ক)TOUPPER_ASC(*s2);
	যদি (i != 0)
	    প্রত্যাগম i;			// this character different
	যদি (*s1 == NUL)
	    ভঙ্গ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    প্রত্যাগম 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
প্রকারনির্দেশ সংরচনা
{
    char_u	*pat;		// the pattern
    পূর্ণাঙ্ক		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    পূর্ণাঙ্ক		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    স্থির শূন্য
prepare_pats(pat_T *pats, পূর্ণাঙ্ক has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    যদি (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	যদি (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	অন্যথা যদি (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	যদি (pats->head == pats->pat)
	    pats->headlen = 0;
	অন্যথা
	    প্রতি (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		যদি (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != রিক্ত)
		    ভঙ্গ;
	যদি (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    যদি (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    অন্যথা
	pats->regmatch.regprog = রিক্ত;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
প্রকারনির্দেশ গণনা {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    স্থির tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    যদি (VIM_ISDIGIT(*cmd))
	প্রত্যাগম *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    যদি (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	প্রত্যাগম (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    যদি (vim_strpbrk(cmd, (char_u *)"\t\r\n") != রিক্ত)
	প্রত্যাগম TAGCMD_INVALID;
    প্রত্যাগম TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    পূর্ণাঙ্ক		*match_গুনতি,	// here the number of tags found will be placed
    পূর্ণাঙ্ক		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    পূর্ণাঙ্ক		ntags = 0;
    পূর্ণাঙ্ক		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    যদি (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == রিক্ত
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	প্রত্যাগম FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    যদি ((d = dict_alloc_lock(VAR_FIXED)) == রিক্ত)
	প্রত্যাগম FAIL;
    যদি (!(flags & TAG_INS_COMP) && tag->user_data != রিক্ত)
	dict_add_string(d, "user_data", tag->user_data);
    যদি (buf_ffname != রিক্ত)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((বর্ণ *)flagString, পরিমাণ(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    যদি (result == FAIL)
	প্রত্যাগম FAIL;
    যদি (rettv.v_type == VAR_SPECIAL && rettv.vval.v_সংখ্যা == VVAL_NULL)
    {
	clear_tv(&rettv);
	প্রত্যাগম NOTDONE;
    }
    যদি (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_মান_from_tagfunc));
	প্রত্যাগম FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_বস্তুসমূহ(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	পূর্ণাঙ্ক		len;
	dict_iterator_T	iter;
	char_u		*dict_চাবি;
	typval_T	*tv;
	পূর্ণাঙ্ক		has_extra = 0;
	পূর্ণাঙ্ক		name_only = flags & TAG_NAMES;

	যদি (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_মান_from_tagfunc));
	    ভঙ্গ;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# অন্যথা
	len = 2;
# endif
	res_name = রিক্ত;
	res_fname = রিক্ত;
	res_cmd = রিক্ত;
	res_kind = রিক্ত;

	dict_iterate_start(&item->li_tv, &iter);
	যাবত্ (রিক্ত != (dict_চাবি = dict_iterate_next(&iter, &tv)))
	{
	    যদি (tv->v_type != VAR_STRING || tv->vval.v_string == রিক্ত)
		অনুবর্ত;

	    len += (পূর্ণাঙ্ক)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    যদি (STRCMP(dict_চাবি, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		অনুবর্ত;
	    }
	    যদি (STRCMP(dict_চাবি, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		অনুবর্ত;
	    }
	    যদি (STRCMP(dict_চাবি, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		অনুবর্ত;
	    }
	    has_extra = 1;
	    যদি (STRCMP(dict_চাবি, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		অনুবর্ত;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (পূর্ণাঙ্ক)STRLEN(dict_চাবি) + 1;
	}

	যদি (has_extra)
	    len += 2;	// need space for ;"

	যদি (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_মান_from_tagfunc));
	    ভঙ্গ;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	যদি (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_মান_from_tagfunc));
	    ভঙ্গ;
	}
	যদি (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	যদি (name_only)
	    mfp = vim_strsave(res_name);
	অন্যথা
	    mfp = alloc(পরিমাণ(char_u) + len + 1);

	যদি (mfp == রিক্ত)
	    অনুবর্ত;

	যদি (!name_only)
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

	    যদি (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    যদি (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		যদি (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		যাবত্ (রিক্ত != (dict_চাবি = dict_iterate_next(&iter, &tv)))
		{
		    যদি (tv->v_type != VAR_STRING || tv->vval.v_string == রিক্ত)
			অনুবর্ত;

		    যদি (STRCMP(dict_চাবি, "name") == 0)
			অনুবর্ত;
		    যদি (STRCMP(dict_চাবি, "filename") == 0)
			অনুবর্ত;
		    যদি (STRCMP(dict_চাবি, "cmd") == 0)
			অনুবর্ত;
		    যদি (STRCMP(dict_চাবি, "kind") == 0)
			অনুবর্ত;

		    *p++ = TAB;
		    STRCPY(p, dict_চাবি);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	যদি (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	অন্যথা
	{
	    vim_free(mfp);
	    ভঙ্গ;
	}
    }

    clear_tv(&rettv);

    *match_গুনতি = ntags;
    প্রত্যাগম result;
}
#endif

/*
 * State information used during a tag search
 */
প্রকারনির্দেশ সংরচনা
{
    tagsearch_state_T	state;		// tag search state
    পূর্ণাঙ্ক		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    পূর্ণাঙ্ক		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    পূর্ণাঙ্ক		flags;			// flags used for tag search
    পূর্ণাঙ্ক		tag_file_sorted;	// !_TAG_FILE_SORTED value
    পূর্ণাঙ্ক		get_searchpat;		// used for 'showfulltag'
    পূর্ণাঙ্ক		help_only;		// only search for help tags
    পূর্ণাঙ্ক		did_open;		// did open a tag file
    পূর্ণাঙ্ক		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    পূর্ণাঙ্ক		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    পূর্ণাঙ্ক		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    পূর্ণাঙ্ক		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    পূর্ণাঙ্ক		is_txt;			// flag of file extension
#endif
    পূর্ণাঙ্ক		match_গুনতি;		// number of matches found
    garray_T	ga_match[MT_গুনতি];	// stores matches in sequence
    hashtab_T	ht_match[MT_গুনতি];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    স্থির পূর্ণাঙ্ক
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    পূর্ণাঙ্ক			flags,
    পূর্ণাঙ্ক			mincount)
{
    পূর্ণাঙ্ক		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = রিক্ত;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (পূর্ণাঙ্ক)STRLEN(pat);
    st->orgpat->regmatch.regprog = রিক্ত;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = রিক্ত;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_গুনতি = 0;
    st->stop_searching = FALSE;

    প্রতি (mtt = 0; mtt < MT_গুনতি; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], পরিমাণ(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    যদি (st->tag_fname == রিক্ত
	    || st->lbuf == রিক্ত
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == রিক্ত
#endif
       )
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Free the state used by find_tags()
 */
    স্থির শূন্য
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
    স্থির পূর্ণাঙ্ক
findtags_in_help_init(findtags_state_T *st)
{
    পূর্ণাঙ্ক		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    যদি (st->is_txt)
	STRCPY(st->help_lang, "en");
    অন্যথা
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (পূর্ণাঙ্ক)STRLEN(st->tag_fname);
	যদি (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	অন্যথা
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    যদি (st->help_lang_find != রিক্ত
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	প্রত্যাগম FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    যদি ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == রিক্ত
	    && curbuf->b_fname != রিক্ত
	    && (i = (পূর্ণাঙ্ক)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    অন্যথা
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	প্রতি (s = p_hlg; *s != NUL; ++s)
	{
	    যদি (STRNICMP(s, st->help_lang, 2) == 0)
		ভঙ্গ;
	    ++st->help_pri;
	    যদি ((s = vim_strchr(s, ',')) == রিক্ত)
		ভঙ্গ;
	}
	যদি (s == রিক্ত || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    যদি (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    প্রত্যাগম TRUE;
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
    স্থির পূর্ণাঙ্ক
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    পূর্ণাঙ্ক		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    পূর্ণাঙ্ক		retval;

    যদি (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	প্রত্যাগম NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_গুনতি,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    প্রত্যাগম retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
স্থির সংরচনা
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
স্থির পূর্ণাঙ্ক incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    স্থির শূন্য
emacs_tags_incstack_free(শূন্য)
{
    যাবত্ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = রিক্ত;
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
    স্থির শূন্য
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    যদি (vim_fgets(st->ebuf, LSIZE, st->fp))
	প্রত্যাগম;

    প্রতি (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    যদি (*p == NUL)
	প্রত্যাগম;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    যদি (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	প্রত্যাগম;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    যদি (incstack[incstack_idx].etag_fname == রিক্ত)
	প্রত্যাগম;

    incstack[incstack_idx].fp = st->fp;
    st->fp = রিক্ত;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    যদি (fullpath_ebuf != রিক্ত)
    {
	st->fp = mch_fopen((বর্ণ *)fullpath_ebuf, "r");
	যদি (st->fp != রিক্ত)
	{
	    যদি (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    যদি (st->fp == রিক্ত)
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
    স্থির পূর্ণাঙ্ক
emacs_tags_file_eof(findtags_state_T *st)
{
    যদি (!incstack_idx)	// reached end of file. stop processing.
	প্রত্যাগম FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    প্রত্যাগম TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    স্থির পূর্ণাঙ্ক
emacs_tags_parse_সারি(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    যদি (p_7f == রিক্ত)
    {
etag_fail:
	যদি (vim_strchr(lbuf, '\n') != রিক্ত)
	    প্রত্যাগম FAIL;

	// Truncated line.  Ignore it.
	যদি (p_verbose >= 5)
	{
	    verbose_enter();
	    বার্তা(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	প্রত্যাগম OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    যদি (p == রিক্ত)
	p = p_7f + 1;
    অন্যথা
	++p;

    যদি (!VIM_ISDIGIT(*p))	    // check for start of line number
	গচ্ছ etag_fail;
    tagp->command = p;

    যদি (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    অন্যথা			    // second format: isolate tagname
    {
	যদি (p_7f == lbuf)
	    গচ্ছ etag_fail;

	// find end of tagname
	প্রতি (p = p_7f - 1; !vim_iswordc(*p); --p)
	    যদি (p == lbuf)
		গচ্ছ etag_fail;
	tagp->tagname_end = p + 1;
	যাবত্ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    প্রত্যাগম OK;
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
    স্থির tags_read_status_T
findtags_get_next_সারি(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    পূর্ণাঙ্ক		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    যদি (st->state == TS_দ্বিমিক)
    {
	offset = sinfo_p->নিচের_offset + ((sinfo_p->উপরের_offset
						- sinfo_p->নিচের_offset) / 2);
	যদি (offset == sinfo_p->curr_offset)
	    প্রত্যাগম TAGS_READ_EOF; // End the binary search without a match.
	অন্যথা
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    অন্যথা যদি (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	যদি (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    যদি (st->state == TS_দ্বিমিক || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	যদি (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    যদি (sinfo_p->curr_offset == sinfo_p->উপরের_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->নিচের_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->নিচের_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	যাবত্ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	যদি (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    প্রত্যাগম TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    অন্যথা
    {
	// skip empty and blank lines
	কুরু
	{
#ifdef FEAT_CSCOPE
	    যদি (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    অন্যথা
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} যাবত্ (!eof && vim_isblankline(st->lbuf));

	যদি (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    যদি (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		প্রত্যাগম TAGS_READ_IGNORE;
#endif
	    প্রত্যাগম TAGS_READ_EOF;
	}
    }

    প্রত্যাগম TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    স্থির পূর্ণাঙ্ক
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    যদি (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	প্রত্যাগম TRUE;

    // Process the header line.
    যদি (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    যদি (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	প্রতি (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    প্রত্যাগম FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    স্থির পূর্ণাঙ্ক
findtags_start_state_handler(
    findtags_state_T	*st,
    পূর্ণাঙ্ক			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    পূর্ণাঙ্ক		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    যদি (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	প্রত্যাগম findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    যদি (st->linear || use_cscope)
#অন্যথা
    যদি (st->linear)
#endif
	st->state = TS_LINEAR;
    অন্যথা যদি (st->tag_file_sorted == NUL)
	st->state = TS_দ্বিমিক;
    অন্যথা যদি (st->tag_file_sorted == '1')
	st->state = TS_দ্বিমিক;
    অন্যথা যদি (st->tag_file_sorted == '2')
    {
	st->state = TS_দ্বিমিক;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    অন্যথা
	st->state = TS_LINEAR;

    যদি (st->state == TS_দ্বিমিক && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    যদি (st->state == TS_দ্বিমিক)
    {
	যদি (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	অন্যথা
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->নিচের_offset = 0;
	    sinfo_p->নিচের_char = 0;
	    sinfo_p->উপরের_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->উপরের_char = 0xff;
	}
	প্রত্যাগম FALSE;
    }

    প্রত্যাগম TRUE;
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
    স্থির tagmatch_status_T
findtags_parse_সারি(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    পূর্ণাঙ্ক		status;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		cmplen;
    পূর্ণাঙ্ক		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    যদি (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	যদি (tagpp->tagname_end == রিক্ত)
	    // Corrupted tag line.
	    প্রত্যাগম TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (পূর্ণাঙ্ক)(tagpp->tagname_end - tagpp->tagname);
	যদি (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	যদি ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	অন্যথা যদি (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    প্রত্যাগম TAG_MATCH_NEXT;

	যদি (st->state == TS_দ্বিমিক)
	{
	    // Simplistic check for unsorted tags file.
	    i = (পূর্ণাঙ্ক)tagpp->tagname[0];
	    যদি (margs->sortic)
		i = (পূর্ণাঙ্ক)TOUPPER_ASC(tagpp->tagname[0]);
	    যদি (i < sinfo_p->নিচের_char || i > sinfo_p->উপরের_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    যদি (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    অন্যথা
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    যদি (tagcmp == 0)
	    {
		যদি (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		অন্যথা যদি (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    যদি (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		প্রত্যাগম TAG_MATCH_NEXT;
	    }
	    যদি (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		যদি (sinfo_p->curr_offset < sinfo_p->উপরের_offset)
		{
		    sinfo_p->নিচের_offset = sinfo_p->curr_offset;
		    যদি (margs->sortic)
			sinfo_p->নিচের_char = TOUPPER_ASC(tagpp->tagname[0]);
		    অন্যথা
			sinfo_p->নিচের_char = tagpp->tagname[0];
		    প্রত্যাগম TAG_MATCH_NEXT;
		}
	    }
	    যদি (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->উপরের_offset)
	    {
		sinfo_p->উপরের_offset = sinfo_p->curr_offset;
		যদি (margs->sortic)
		    sinfo_p->উপরের_char = TOUPPER_ASC(tagpp->tagname[0]);
		অন্যথা
		    sinfo_p->উপরের_char = tagpp->tagname[0];
		প্রত্যাগম TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    প্রত্যাগম TAG_MATCH_STOP;
	}
	অন্যথা যদি (st->state == TS_SKIP_BACK)
	{
	    যদি (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    অন্যথা
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    প্রত্যাগম TAG_MATCH_NEXT;
	}
	অন্যথা যদি (st->state == TS_STEP_FORWARD)
	{
	    যদি (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		যদি ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    প্রত্যাগম TAG_MATCH_STOP;	// past last match
		অন্যথা
		    প্রত্যাগম TAG_MATCH_NEXT;	// before first match
	    }
	}
	অন্যথা
	    // skip this match if it can't match
	    যদি (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		প্রত্যাগম TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	যদি (tagpp->fname_end == রিক্ত)
	    status = FAIL;
	অন্যথা
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    অন্যথা
	status = parse_tag_সারি(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    যদি (status == FAIL)
	প্রত্যাগম TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    যদি (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    প্রত্যাগম TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    স্থির শূন্য
findtags_matchargs_init(findtags_match_args_T *margs, পূর্ণাঙ্ক flags)
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
    স্থির পূর্ণাঙ্ক
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    পূর্ণাঙ্ক		match = FALSE;
    পূর্ণাঙ্ক		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (পূর্ণাঙ্ক)(tagpp->tagname_end - tagpp->tagname);
    যদি (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    যদি (st->orgpat->len != cmplen)
	match = FALSE;
    অন্যথা
    {
	যদি (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    যদি (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	অন্যথা
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    যদি (!match && st->orgpat->regmatch.regprog != রিক্ত)
    {
	পূর্ণাঙ্ক	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	যদি (match)
	{
	    margs->matchoff = (পূর্ণাঙ্ক)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    যদি (st->orgpat->regmatch.rm_ic)
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

    প্রত্যাগম match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    স্থির শূন্য
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_সারি;
    পূর্ণাঙ্ক		len;

    conv_সারি = string_convert(&st->vimconv, st->lbuf, রিক্ত);
    যদি (conv_সারি == রিক্ত)
	প্রত্যাগম;

    // Copy or swap lbuf and conv_line.
    len = (পূর্ণাঙ্ক)STRLEN(conv_সারি) + 1;
    যদি (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_সারি;
	st->lbuf_size = len;
    }
    অন্যথা
    {
	STRCPY(st->lbuf, conv_সারি);
	vim_free(conv_সারি);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    স্থির পূর্ণাঙ্ক
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    পূর্ণাঙ্ক		name_only = (st->flags & TAG_NAMES);
    পূর্ণাঙ্ক		mtt;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		is_current;		// file name matches
    পূর্ণাঙ্ক		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    যদি (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    অন্যথা
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
	যদি (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	যদি (is_static)
	{
	    যদি (is_current)
		mtt = MT_ST_CUR;
	    অন্যথা
		mtt = MT_ST_OTH;
	}
	অন্যথা
	{
	    যদি (is_current)
		mtt = MT_GL_CUR;
	    অন্যথা
		mtt = MT_GL_OTH;
	}
	যদি (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	যদি (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    যদি (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#অন্যথা
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (পূর্ণাঙ্ক)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(পরিমাণ(char_u) + len + 10 + ML_EXTRA + 1);
	যদি (mfp != রিক্ত)
	{
	    পূর্ণাঙ্ক heuristic;

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
	    sprintf((বর্ণ *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    অন্যথা যদি (name_only)
    {
	যদি (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    যদি (*temp_end == '/')
		যাবত্ (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    যদি (tagpp->command + 2 < temp_end)
	    {
		len = (পূর্ণাঙ্ক)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		যদি (mfp != রিক্ত)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    অন্যথা
		mfp = রিক্ত;
	    st->get_searchpat = FALSE;
	}
	অন্যথা
	{
	    len = (পূর্ণাঙ্ক)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(পরিমাণ(char_u) + len + 1);
	    যদি (mfp != রিক্ত)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    যদি (State & MODE_ঢোকাও)
		st->get_searchpat = p_sft;
	}
    }
    অন্যথা
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
	len = (পূর্ণাঙ্ক)tag_fname_len + (পূর্ণাঙ্ক)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	যদি (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (পূর্ণাঙ্ক)ebuf_len + 1;
	}
	অন্যথা
	    ++len;
#endif
	mfp = alloc(পরিমাণ(char_u) + len + 1);
	যদি (mfp != রিক্ত)
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
	    যদি (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    অন্যথা
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    যদি (mfp != রিক্ত)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	যদি (use_cscope)
	    ++*hash;
	অন্যথা
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	যদি (HASHITEM_EMPTY(hi))
	{
	    যদি (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		প্রত্যাগম FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_গুনতি++;
	}
	অন্যথা
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    প্রত্যাগম OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    স্থির শূন্য
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	অনুসন্ধান_info;
    পূর্ণাঙ্ক			retval;
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(অনুসন্ধান_info);

    // Read and parse the lines in the file one by one
    প্রতি (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	যদি (st->state == TS_দ্বিমিক || st->state == TS_SKIP_BACK)
	    সারি_breakcheck();
	অন্যথা
	    fast_breakcheck();
	যদি ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_চাবিগুলি(30, FALSE);
	যদি (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ভঙ্গ;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	যদি (st->mincount == TAG_MANY && st->match_গুনতি >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    ভঙ্গ;
	}
	যদি (st->get_searchpat)
	    গচ্ছ সারি_read_in;

	retval = findtags_get_next_সারি(st, &অনুসন্ধান_info);
	যদি (retval == TAGS_READ_IGNORE)
	    অনুবর্ত;
	যদি (retval == TAGS_READ_EOF)
	    ভঙ্গ;

সারি_read_in:

	যদি (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	যদি (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    অনুবর্ত;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	যদি (st->state == TS_START)
	{
	    যদি (findtags_start_state_handler(st, &margs->sortic, &অনুসন্ধান_info) == FALSE)
		অনুবর্ত;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	যদি (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    যদি (st->lbuf == রিক্ত)
	    {
		যদি (st->fp != রিক্ত)
		    fclose(st->fp);
		st->fp = রিক্ত;
		st->stop_searching = TRUE;
		প্রত্যাগম;
	    }

	    যদি (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, অনুসন্ধান_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    অনুসন্ধান_info.curr_offset = 0;
	    অনুবর্ত;
	}

	retval = findtags_parse_সারি(st, &tagp, margs, &অনুসন্ধান_info);
	যদি (retval == TAG_MATCH_NEXT)
	    অনুবর্ত;
	যদি (retval == TAG_MATCH_STOP)
	    ভঙ্গ;
	যদি (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    যদি (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    প্রত্যাগম;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	যদি (findtags_match_tag(st, &tagp, margs))
	{
	    যদি (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ভঙ্গ;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    স্থির শূন্য
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = রিক্ত;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    যদি (use_cscope)
	st->fp = রিক্ত;	    // avoid GCC warning
    অন্যথা
#endif
    {
#ifdef FEAT_MULTI_LANG
	যদি (curbuf->b_help)
	{
	    যদি (!findtags_in_help_init(st))
		প্রত্যাগম;
	}
#endif

	st->fp = mch_fopen((বর্ণ *)st->tag_fname, "r");
	যদি (st->fp == রিক্ত)
	    প্রত্যাগম;

	যদি (p_verbose >= 5)
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

    যদি (st->fp != রিক্ত)
    {
	fclose(st->fp);
	st->fp = রিক্ত;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    যদি (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, রিক্ত, রিক্ত);

    যদি (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    যদি (st->match_গুনতি >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    স্থির পূর্ণাঙ্ক
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    পূর্ণাঙ্ক		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    পূর্ণাঙ্ক		mtt;
    পূর্ণাঙ্ক		i;
    char_u	*mfp;
    char_u	*p;

    যদি (st->match_গুনতি > 0)
	matches = ALLOC_MULT(char_u *, st->match_গুনতি);
    অন্যথা
	matches = রিক্ত;
    st->match_গুনতি = 0;
    প্রতি (mtt = 0; mtt < MT_গুনতি; ++mtt)
    {
	প্রতি (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    যদি (matches == রিক্ত)
		vim_free(mfp);
	    অন্যথা
	    {
		যদি (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    প্রতি (p = mfp + 1; *p != NUL; ++p)
			যদি (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_গুনতি++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    প্রত্যাগম st->match_গুনতি;
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
    পূর্ণাঙ্ক
find_tags(
    char_u	*pat,			// pattern to search for
    পূর্ণাঙ্ক		*অঙ্ক_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    পূর্ণাঙ্ক		first_file;		// trying first tag file
    পূর্ণাঙ্ক		retval = FAIL;		// return value
    পূর্ণাঙ্ক		round;

    পূর্ণাঙ্ক		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    পূর্ণাঙ্ক		i;
    char_u	*saved_pat = রিক্ত;		// copy of pat[]
#endif

    পূর্ণাঙ্ক		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    পূর্ণাঙ্ক		has_re = (flags & TAG_REGEXP);	// regexp used
    পূর্ণাঙ্ক		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    পূর্ণাঙ্ক		use_cscope = (flags & TAG_CSCOPE);
#endif
    পূর্ণাঙ্ক		verbose = (flags & TAG_VERBOSE);
    পূর্ণাঙ্ক		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    বিকল্প (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	পক্ষ TC_FOLLOWIC:		 ভঙ্গ;
	পক্ষ TC_IGNORE:    p_ic = TRUE;  ভঙ্গ;
	পক্ষ TC_MATCH:     p_ic = FALSE; ভঙ্গ;
	পক্ষ TC_FOLLOWSCS: p_ic = ignorecase(pat); ভঙ্গ;
	পক্ষ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ভঙ্গ;
    }

    help_save = curbuf->b_help;

    যদি (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	গচ্ছ findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    যদি (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    অন্যথা যদি (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    যদি (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	যদি (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    যদি (saved_pat != রিক্ত)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    যদি (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    যদি (has_re && st.orgpat->regmatch.regprog == রিক্ত)
	গচ্ছ findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    যদি (retval != NOTDONE)
	গচ্ছ findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    যদি ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == রিক্ত
	    && curbuf->b_fname != রিক্ত
	    && (i = (পূর্ণাঙ্ক)STRLEN(curbuf->b_fname)) > 4
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
    প্রতি (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      প্রতি (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  যদি (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      ভঙ্গ;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	যদি (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	যদি (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ভঙ্গ;
#ifdef FEAT_CSCOPE
	যদি (use_cscope)
	    ভঙ্গ;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    যদি (!st.stop_searching)
    {
	যদি (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    যদি (retval == FAIL)
	st.match_গুনতি = 0;

    *অঙ্ক_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    প্রত্যাগম retval;
}

স্থির garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    স্থির শূন্য
found_tagfile_cb(char_u *fname, শূন্য *cookie UNUSED)
{
    যদি (ga_grow(&tag_fnames, 1) == FAIL)
	প্রত্যাগম;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#যদি defined(EXITFREE)
    শূন্য
free_tag_stuff(শূন্য)
{
    ga_clear_strings(&tag_fnames);
    যদি (curwin != রিক্ত)
	do_tag(রিক্ত, DT_FREE, 0, 0, 0);
    tag_freematch();

# যদি defined(FEAT_QUICKFIX)
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
    পূর্ণাঙ্ক
get_tagfname(
    tagname_T	*tnp,	// holds status info
    পূর্ণাঙ্ক		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = রিক্ত;
    char_u		*r_ptr;
    পূর্ণাঙ্ক			i;

    যদি (first)
	CLEAR_POINTER(tnp);

    যদি (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	যদি (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, পরিমাণ(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# অন্যথা
		    "doc/tags doc/tags-??"
# endif
#অন্যথা
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, রিক্ত);
	}

	যদি (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    যদি (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		প্রত্যাগম FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    প্রতি (i = 0; i < tag_fnames.ga_len; ++i)
		যদি (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    প্রত্যাগম FAIL; // avoid duplicate file names
	}
	অন্যথা
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	প্রত্যাগম OK;
    }

    যদি (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	যদি (tnp->tn_tags == রিক্ত)
	    প্রত্যাগম FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    প্রতি (;;)
    {
	যদি (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_অনুসন্ধান_ctx);
	    যদি (fname != রিক্ত)
		ভঙ্গ;

	    tnp->tn_did_filefind_init = FALSE;
	}
	অন্যথা
	{
	    char_u  *filename = রিক্ত;

	    // Stop when used all parts of 'tags'.
	    যদি (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_অনুসন্ধান_ctx);
		tnp->tn_অনুসন্ধান_ctx = রিক্ত;
		প্রত্যাগম FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (শূন্য)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    যদি (r_ptr != রিক্ত)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_অনুসন্ধান_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_অনুসন্ধান_ctx, TRUE, curbuf->b_ffname);
	    যদি (tnp->tn_অনুসন্ধান_ctx != রিক্ত)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    প্রত্যাগম OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    শূন্য
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_অনুসন্ধান_ctx);
    tnp->tn_অনুসন্ধান_ctx = রিক্ত;
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
    স্থির পূর্ণাঙ্ক
parse_tag_সারি(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    পূর্ণাঙ্ক		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    যদি (is_etag)
	// emacs-style tag file
	প্রত্যাগম emacs_tags_parse_সারি(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    যদি (p == রিক্ত)
	প্রত্যাগম FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    যদি (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    যদি (p == রিক্ত)
	প্রত্যাগম FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    যদি (*p != NUL)
	++p;
    যদি (*p == NUL)
	প্রত্যাগম FAIL;
    tagp->command = p;

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    যাবত্ ((p = vim_strchr(p, '\t')) != রিক্ত)
    {
	++p;
	যদি (STRNCMP(p, "file:", 5) == 0)
	    প্রত্যাগম TRUE;
    }

    প্রত্যাগম FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    স্থির size_t
matching_সারি_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    প্রত্যাগম (p - lbuf) + STRLEN(p);
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
    স্থির পূর্ণাঙ্ক
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    পূর্ণাঙ্ক		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    যদি (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    অন্যথা
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_সারি(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = রিক্ত;
    tagp->user_data = রিক্ত;
    tagp->tagline = 0;
    tagp->command_end = রিক্ত;

    যদি (retval != OK)
	প্রত্যাগম retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    যদি (find_extra(&p) == OK)
    {
	যদি (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	অন্যথা
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	যদি (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    যাবত্ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		যদি (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		অন্যথা যদি (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		অন্যথা যদি (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((বর্ণ *)p + 5);
		যদি (tagp->tagkind != রিক্ত && tagp->user_data != রিক্ত)
		    ভঙ্গ;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		যদি (pc == রিক্ত || (pt != রিক্ত && pc > pt))
		    tagp->tagkind = p;
		যদি (pt == রিক্ত)
		    ভঙ্গ;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    যদি (tagp->tagkind != রিক্ত)
    {
	প্রতি (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    যদি (tagp->user_data != রিক্ত)
    {
	প্রতি (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    প্রত্যাগম retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    স্থির char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    পূর্ণাঙ্ক		c;

#ifdef FEAT_EMACS_TAGS
    যদি (tagp->is_etag)
	c = 0;	    // to shut up GCC
    অন্যথা
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    যদি (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    প্রত্যাগম fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    পূর্ণাঙ্ক		forceit,	// :ta with !
    পূর্ণাঙ্ক		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    পূর্ণাঙ্ক		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = রিক্ত;
    char_u	*fname;
    tagptrs_T	tagp;
    পূর্ণাঙ্ক		retval = FAIL;
    পূর্ণাঙ্ক		getfile_result = GETFILE_UNUSED;
    পূর্ণাঙ্ক		অনুসন্ধান_options;
#ifdef FEAT_অনুসন্ধান_EXTRA
    পূর্ণাঙ্ক		save_no_hlsearch;
#endif
#যদি defined(FEAT_QUICKFIX)
    win_T	*curwin_save = রিক্ত;
#endif
    char_u	*full_fname = রিক্ত;
#ifdef FEAT_FOLDING
    পূর্ণাঙ্ক		old_চাবি_Typed = চাবি_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    পূর্ণাঙ্ক		isdigit = FALSE;

    যদি (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	প্রত্যাগম FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_সারি_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    যদি (lbuf != রিক্ত)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    যদি (pbuf == রিক্ত || lbuf == রিক্ত || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = রিক্ত;
	গচ্ছ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    যদি (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    প্রতি (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	যদি (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ভঙ্গ;
#endif
	*pbuf_end++ = *str++;
	যদি (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    ভঙ্গ;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    যদি (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	যদি (isdigit)
	    str++;
	যদি (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    যদি (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    অন্যথা
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    যদি (fname == রিক্ত)
	গচ্ছ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    যদি (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, রিক্ত))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	যদি (nofile_fname == রিক্ত)
	    nofile_fname = empty_option;
	গচ্ছ erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#যদি defined(FEAT_QUICKFIX)
    যদি (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	যদি (!curwin->w_p_pvw)
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
    যদি (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	যদি (existing_buf != রিক্ত)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    যদি (swbuf_goto_win_with_buf(existing_buf) != রিক্ত)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    যদি (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	যদি (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	যদি ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	যদি (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    যদি (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    গচ্ছ erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    যদি (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#যদি defined(FEAT_QUICKFIX)
	যদি (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	অন্যথা
#endif
	    keep_help_flag = curbuf->b_help;
    }

    যদি (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, রিক্ত, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    যদি (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_অনুসন্ধান_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	যদি (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	যদি (vim_strchr(p_cpo, CPO_TAGPAT) != রিক্ত)
	    অনুসন্ধান_options = 0;
	অন্যথা
	    অনুসন্ধান_options = অনুসন্ধান_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	যদি (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	যদি (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    যদি (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    অন্যথা
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    যদি (do_অনুসন্ধান(রিক্ত, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 অনুসন্ধান_options, রিক্ত))
		retval = OK;
	    অন্যথা
	    {
		পূর্ণাঙ্ক	found = 1;
		পূর্ণাঙ্ক	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		যদি (!do_অনুসন্ধান(রিক্ত, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 অনুসন্ধান_options, রিক্ত))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (শূন্য)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((বর্ণ *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    যদি (!do_অনুসন্ধান(রিক্ত, '/', '/', pbuf, pbuflen, (long)1,
							 অনুসন্ধান_options, রিক্ত))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((বর্ণ *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			যদি (!do_অনুসন্ধান(রিক্ত, '/', '/', pbuf, pbuflen, (long)1,
							 অনুসন্ধান_options, রিক্ত))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		যদি (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		অন্যথা
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    যদি (found == 2 || !save_p_ic)
		    {
			বার্তা(_(e_couldnt_find_tag_just_guessing));
			যদি (!বার্তা_scrolled && বার্তা_silent == 0)
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
	অন্যথা
	{
	    পূর্ণাঙ্ক		save_secure = secure;

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
	    যদি (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_অনুসন্ধান_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	যদি (অনুসন্ধান_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	যদি (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	যদি (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    যদি (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    যদি ((fdo_flags & FDO_TAG) && old_চাবি_Typed)
		foldOpenCursor();
#endif
	}

#যদি defined(FEAT_QUICKFIX)
	যদি (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	যদি (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    অন্যথা
    {
	যদি (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	যদি (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#যদি defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	অন্যথা যদি (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    যদি (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#যদি defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    যদি (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#যদি defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    প্রত্যাগম retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    স্থির char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, পূর্ণাঙ্ক expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = রিক্ত;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    যদি (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       প্রত্যাগম রিক্ত;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    যদি (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == রিক্ত)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, রিক্ত,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	যদি (expanded_fname != রিক্ত)
	    fname = expanded_fname;
    }

    যদি ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	যদি (retval != রিক্ত)
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
    অন্যথা
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    প্রত্যাগম retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    স্থির পূর্ণাঙ্ক
test_for_current(
#ifdef FEAT_EMACS_TAGS
    পূর্ণাঙ্ক	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    পূর্ণাঙ্ক	    c;
    পূর্ণাঙ্ক	    retval = FALSE;
    char_u  *fullname;

    যদি (buf_ffname != রিক্ত)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	যদি (is_etag)
	    c = 0;	    // to shut up GCC
	অন্যথা
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	যদি (fullname != রিক্ত)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	যদি (!is_etag)
#endif
	    *fname_end = c;
    }

    প্রত্যাগম retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    প্রতি (;;)
    {
	যদি (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	অন্যথা যদি (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    যদি (*str != first_char)
		str = রিক্ত;
	    অন্যথা
		++str;
	}
	অন্যথা
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((বর্ণ *)str, "|;\"");
	    যদি (str != রিক্ত)
	    {
		++str;
		ভঙ্গ;
	    }

	}
	যদি (str == রিক্ত || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ভঙ্গ;
	++str;	// skip ';'
	first_char = *str;
    }

    যদি (str != রিক্ত && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	প্রত্যাগম OK;
    }
    প্রত্যাগম FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    শূন্য
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    পূর্ণাঙ্ক
expand_tags(
    পূর্ণাঙ্ক		tagnames,	// expand tag names
    char_u	*pat,
    পূর্ণাঙ্ক		*অঙ্ক_file,
    char_u	***file)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    পূর্ণাঙ্ক		ret;

    name_buf = alloc(name_buf_size);
    যদি (name_buf == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (tagnames)
	extra_flag = TAG_NAMES;
    অন্যথা
	extra_flag = 0;
    যদি (pat[0] == '/')
	ret = find_tags(pat + 1, অঙ্ক_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    অন্যথা
	ret = find_tags(pat, অঙ্ক_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    যদি (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 প্রতি (i = 0; i < *অঙ্ক_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     যদি (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 যদি (buf == রিক্ত)
		 {
		     vim_free(name_buf);
		     প্রত্যাগম FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != রিক্ত && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    প্রত্যাগম ret;
}

#যদি defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
add_tag_field(
    dict_T  *dict,
    বর্ণ    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		retval;

    // check that the field name doesn't exist yet
    যদি (dict_has_চাবি(dict, field_name))
    {
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	প্রত্যাগম FAIL;
    }
    buf = alloc(MAXPATHL);
    যদি (buf == রিক্ত)
	প্রত্যাগম FAIL;
    যদি (start != রিক্ত)
    {
	যদি (end == রিক্ত)
	{
	    end = start + STRLEN(start);
	    যাবত্ (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (পূর্ণাঙ্ক)(end - start);
	যদি (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    প্রত্যাগম retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    পূর্ণাঙ্ক
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    পূর্ণাঙ্ক		অঙ্ক_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &অঙ্ক_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (পূর্ণাঙ্ক)MAXCOL, buf_fname);
    যদি (ret != OK || অঙ্ক_matches <= 0)
	প্রত্যাগম ret;

    প্রতি (i = 0; i < অঙ্ক_matches; ++i)
    {
	যদি (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    অনুবর্ত;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	যদি (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    অনুবর্ত;
	}

	যদি ((dict = dict_alloc()) == রিক্ত)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ভঙ্গ;
	}
	যদি (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    অনুবর্ত;
	}

	full_fname = tag_full_fname(&tp);
	যদি (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    রিক্ত) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_সংখ্যা(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	যদি (tp.command_end != রিক্ত)
	{
	    প্রতি (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		যদি (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		অন্যথা যদি (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		অন্যথা যদি (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    পূর্ণাঙ্ক	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    যাবত্ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (পূর্ণাঙ্ক)(p - n);
		    যদি (*p == ':' && len > 0)
		    {
			s = ++p;
			যাবত্ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			যদি (add_tag_field(dict, (বর্ণ *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    অন্যথা
			// Skip field without colon.
			যাবত্ (*p != NUL && *p >= ' ')
			    ++p;
		    যদি (*p == NUL)
			ভঙ্গ;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    প্রত্যাগম ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    স্থির শূন্য
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_সংখ্যা(retdict, "matchnr", tag->cur_match + 1);
    dict_add_সংখ্যা(retdict, "bufnr", tag->cur_fnum);
    যদি (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    যদি ((pos = list_alloc_id(aid_tagstack_from)) == রিক্ত)
	প্রত্যাগম;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_সংখ্যা(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_সংখ্যা(pos, (varnumber_T)fmark->mark.lnum);
    list_append_সংখ্যা(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_সংখ্যা(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    শূন্য
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    পূর্ণাঙ্ক		i;
    dict_T	*d;

    dict_add_সংখ্যা(retdict, "length", wp->w_tagstacklen);
    dict_add_সংখ্যা(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_বস্তুসমূহ);
    যদি (l == রিক্ত)
	প্রত্যাগম;
    dict_add_list(retdict, "items", l);

    প্রতি (i = 0; i < wp->w_tagstacklen; i++)
    {
	যদি ((d = dict_alloc_id(aid_tagstack_details)) == রিক্ত)
	    প্রত্যাগম;
	যদি (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    প্রত্যাগম;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    স্থির শূন্য
tagstack_clear(win_T *wp)
{
    পূর্ণাঙ্ক i;

    // Free the current tag stack
    প্রতি (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    স্থির শূন্য
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    পূর্ণাঙ্ক		i;

    tagstack_clear_entry(&tagstack[0]);
    প্রতি (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    স্থির শূন্য
tagstack_ঠেলো_item(
	win_T	*wp,
	char_u	*tagname,
	পূর্ণাঙ্ক	cur_fnum,
	পূর্ণাঙ্ক	cur_match,
	pos_T	mark,
	পূর্ণাঙ্ক	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    পূর্ণাঙ্ক		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    যদি (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    যদি (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    স্থির শূন্য
tagstack_ঠেলো_বস্তুসমূহ(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    পূর্ণাঙ্ক		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	যদি (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == রিক্ত)
	    অনুবর্ত;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	যদি ((di = dict_find(itemdict, (char_u *)"from", -1)) == রিক্ত)
	    অনুবর্ত;
	যদি (list2fpos(&di->di_tv, &mark, &fnum, রিক্ত, FALSE) != OK)
	    অনুবর্ত;
	যদি ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == রিক্ত)
	    অনুবর্ত;

	যদি (mark.col > 0)
	    mark.col--;
	tagstack_ঠেলো_item(wp, tagname,
		(পূর্ণাঙ্ক)dict_get_সংখ্যা(itemdict, "bufnr"),
		(পূর্ণাঙ্ক)dict_get_সংখ্যা(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    স্থির শূন্য
tagstack_set_curidx(win_T *wp, পূর্ণাঙ্ক curidx)
{
    wp->w_tagstackidx = curidx;
    যদি (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    যদি (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    পূর্ণাঙ্ক
set_tagstack(win_T *wp, dict_T *d, পূর্ণাঙ্ক action)
{
    dictitem_T	*di;
    list_T	*l = রিক্ত;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    যদি (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_স্তূপ_within_tagfunc));
	প্রত্যাগম FAIL;
    }
# endif

    যদি ((di = dict_find(d, (char_u *)"items", -1)) != রিক্ত)
    {
	যদি (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    প্রত্যাগম FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    যদি ((di = dict_find(d, (char_u *)"curidx", -1)) != রিক্ত)
	tagstack_set_curidx(wp, (পূর্ণাঙ্ক)tv_get_সংখ্যা(&di->di_tv) - 1);

    যদি (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	পূর্ণাঙ্ক	tagstackidx = wp->w_tagstackidx;
	পূর্ণাঙ্ক	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	যাবত্ (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    যদি (l != রিক্ত)
    {
	যদি (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ঠেলো_বস্তুসমূহ(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    প্রত্যাগম OK;
}
#endif
