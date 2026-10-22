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

#अन्तर्भाव "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
प्रकारव्याख्या संरचना tag_pointers
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
    पूर्णांक		is_etag;	// TRUE for emacs tag
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
प्रकारव्याख्या गणना
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
प्रकारव्याख्या गणना
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_द्विमान,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
प्रकारव्याख्या संरचना
{
    off_T	खालचा_offset;	// offset for first char of first line that
				// could match
    off_T	वरचा_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    पूर्णांक	खालचा_char;		// first char at low_offset
    पूर्णांक	वरचा_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
प्रकारव्याख्या गणना
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
प्रकारव्याख्या संरचना
{
    पूर्णांक	matchoff;		// tag match offset
    पूर्णांक	match_re;		// TRUE if the tag matches a regexp
    पूर्णांक	match_no_ic;		// TRUE if the tag matches with case
    पूर्णांक	has_re;			// regular expression used
    पूर्णांक	sortic;			// tags file sorted ignoring case (foldcase)
    पूर्णांक	sort_error;		// tags file not sorted
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
#define MT_मोजणी	16

स्थिर अक्षर	*mt_names[MT_मोजणी/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
स्थिर char_u	*nofile_fname = रिक्त;	// fname for NOTAGFILE error

स्थिर शून्य taglen_advance(पूर्णांक l);

स्थिर पूर्णांक jumpto_tag(char_u *lbuf, पूर्णांक forceit, पूर्णांक keep_help);
#ifdef FEAT_EMACS_TAGS
स्थिर पूर्णांक parse_tag_ओळ(char_u *lbuf, पूर्णांक is_etag, tagptrs_T *tagp);
#अन्यथा
स्थिर पूर्णांक parse_tag_ओळ(char_u *lbuf, tagptrs_T *tagp);
#endif
स्थिर पूर्णांक test_for_static(tagptrs_T *);
स्थिर पूर्णांक parse_match(char_u *lbuf, tagptrs_T *tagp);
स्थिर char_u *tag_full_fname(tagptrs_T *tagp);
स्थिर char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, पूर्णांक expand);
#ifdef FEAT_EMACS_TAGS
स्थिर पूर्णांक test_for_current(पूर्णांक, char_u *, char_u *, char_u *, char_u *);
#अन्यथा
स्थिर पूर्णांक test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
स्थिर पूर्णांक find_extra(char_u **pp);
स्थिर शून्य print_tag_list(पूर्णांक new_tag, पूर्णांक use_tagstack, पूर्णांक अंक_matches, char_u **matches);
#जर defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
स्थिर पूर्णांक add_llist_tags(char_u *tag, पूर्णांक अंक_matches, char_u **matches);
#endif

स्थिर char_u	*tagmatchname = रिक्त;	// name of last used tag

#जर defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
स्थिर taggy_T ptag_entry = {रिक्त, {{0, 0, 0}, 0}, 0, 0, रिक्त};
#endif

#ifdef FEAT_EVAL
स्थिर पूर्णांक  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
स्थिर callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#जर defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    अक्षर *
did_set_tagfunc(optset_T *args)
{
    पूर्णांक	retval;

    जर (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    अन्यथा
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	जर (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    परत retval == FAIL ? e_invalid_argument : रिक्त;
}
#endif

#जर defined(EXITFREE)
    शून्य
free_tagfunc_option(शून्य)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    पूर्णांक
set_ref_in_tagfunc(पूर्णांक copyID UNUSED)
{
    पूर्णांक	पात = FALSE;

    पात = set_ref_in_callback(&tfu_cb, copyID);

    परत पात;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    शून्य
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    जर (tfu_cb.cb_name != रिक्त && *tfu_cb.cb_name != NUL)
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
    पूर्णांक
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    पूर्णांक		type,
    पूर्णांक		मोजणी,
    पूर्णांक		forceit,	// :ta with !
    पूर्णांक		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णांक		tagstackidx = curwin->w_tagstackidx;
    पूर्णांक		tagstacklen = curwin->w_tagstacklen;
    पूर्णांक		cur_match = 0;
    पूर्णांक		cur_fnum = curbuf->b_fnum;
    पूर्णांक		oldtagstackidx = tagstackidx;
    पूर्णांक		prevtagstackidx = tagstackidx;
    पूर्णांक		prev_अंक_matches;
    पूर्णांक		new_tag = FALSE;
    पूर्णांक		i;
    पूर्णांक		ic;
    पूर्णांक		no_regexp = FALSE;
    पूर्णांक		error_cur_match = 0;
    पूर्णांक		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    पूर्णांक		jumped_to_tag = FALSE;
#endif
    पूर्णांक		new_अंक_matches;
    char_u	**new_matches;
    पूर्णांक		use_tagstack;
    पूर्णांक		skip_संदेश = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    पूर्णांक		use_tfu = 1;
    char_u	*tofree = रिक्त;

    // remember the matches for the last used tag
    स्थिर पूर्णांक		अंक_matches = 0;
    स्थिर पूर्णांक		max_अंक_matches = 0;  // limit used for match search
    स्थिर char_u	**matches = रिक्त;
    स्थिर पूर्णांक		flags;

#ifdef EXITFREE
    जर (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(अंक_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	अंक_matches = 0;
	परत FALSE;
    }
#endif

#ifdef FEAT_EVAL
    जर (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_चळत_within_tagfunc));
	परत FALSE;
    }
#endif

    जर (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	परत FALSE;

    जर (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_अंक_matches = अंक_matches;
    free_string_option(nofile_fname);
    nofile_fname = रिक्त;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    जर ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#जर defined(FEAT_QUICKFIX)
	जर (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    जर ((ptag_entry.tagname = vim_strsave(tag)) == रिक्त)
		जा end_do_tag;
	}
#endif
    }
    अन्यथा
    {
#जर defined(FEAT_QUICKFIX)
	जर (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	अन्यथा
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	जर (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#जर defined(FEAT_QUICKFIX)
	    जर (g_do_tagpreview != 0)
	    {
		जर (ptag_entry.tagname != रिक्त
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		अन्यथा
		{
		    tagstack_clear_entry(&ptag_entry);
		    जर ((ptag_entry.tagname = vim_strsave(tag)) == रिक्त)
			जा end_do_tag;
		}
	    }
	    अन्यथा
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		पर्यंत (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		जर (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    साठी (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = रिक्त;
		}

		/*
		 * put the tag name in the tag stack
		 */
		जर ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == रिक्त)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    जा end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	अन्यथा
	{
	    जर (
#जर defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == रिक्त :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_चळत_empty));
		जा end_do_tag;
	    }

	    जर (type == DT_काढा)		// go to older position
	    {
#ifdef FEAT_FOLDING
		पूर्णांक	old_किल्ली_Typed = किल्ली_Typed;
#endif
		जर ((tagstackidx -= मोजणी) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_चळत));
		    जर (tagstackidx + मोजणी == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			जा end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		अन्यथा जर (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_शीर्ष_of_tag_चळत));
		    जा end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		जर (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    जर (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			जा end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		अन्यथा
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		जर ((fdo_flags & FDO_TAG) && old_किल्ली_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(अंक_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		अंक_matches = 0;
		tag_freematch();
		जा end_do_tag;
	    }

	    जर (type == DT_TAG
#जर defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#जर defined(FEAT_QUICKFIX)
		जर (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		अन्यथा
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    जर ((tagstackidx += मोजणी - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_शीर्ष_of_tag_चळत));
			save_pos = FALSE;
		    }
		    अन्यथा जर (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_चळत));
			tagstackidx = 0;
			जा end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    अन्यथा				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#जर defined(FEAT_QUICKFIX)
		जर (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		अन्यथा
#endif
		{
		    जर (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		निवड (type)
		{
		    स्थिती DT_FIRST: cur_match = मोजणी - 1; थांबा;
		    स्थिती DT_SELECT:
		    स्थिती DT_JUMP:
#ifdef FEAT_CSCOPE
		    स्थिती DT_CSCOPE:
#endif
		    स्थिती DT_LAST:  cur_match = MAXCOL - 1; थांबा;
		    स्थिती DT_NEXT:  cur_match += मोजणी; थांबा;
		    स्थिती DT_PREV:  cur_match -= मोजणी; थांबा;
		}
		जर (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		अन्यथा जर (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_संदेश = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#जर defined(FEAT_QUICKFIX)
	जर (g_do_tagpreview != 0)
	{
	    जर (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	अन्यथा
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    जर (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    जर (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    जर (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	जर (buf != रिक्त)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    साठी (;;)
    {
	पूर्णांक	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	जर (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#जर defined(FEAT_QUICKFIX)
	अन्यथा जर (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	अन्यथा
	    name = tag;
	other_name = (tagmatchname == रिक्त || STRCMP(tagmatchname, name) != 0);
	जर (new_tag
		|| (cur_match >= अंक_matches && max_अंक_matches != MAXCOL)
		|| other_name)
	{
	    जर (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    जर (type == DT_SELECT || type == DT_JUMP
#जर defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    जर (type == DT_TAG)
		max_अंक_matches = MAXCOL;
	    अन्यथा
		max_अंक_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    जर (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    अन्यथा
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    जर (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    जर (verbose)
		flags |= TAG_VERBOSE;

	    जर (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    जर (find_tags(name, &new_अंक_matches, &new_matches, flags,
					    max_अंक_matches, buf_ffname) == OK
		    && new_अंक_matches < max_अंक_matches)
		max_अंक_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    जर (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_अंक_matches, new_matches);
		थांबा;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    जर (!new_tag && !other_name)
	    {
		पूर्णांक	    j, k;
		पूर्णांक	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		साठी (j = 0; j < अंक_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    साठी (i = idx; i < new_अंक_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			जर (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    साठी (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    थांबा;
			}
		    }
		}
	    }
	    FreeWild(अंक_matches, matches);
	    अंक_matches = new_अंक_matches;
	    matches = new_matches;
	}

	जर (अंक_matches <= 0)
	{
	    जर (verbose)
		semsg(_(e_tag_not_found_str), name);
#जर defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	अन्यथा
	{
	    पूर्णांक ask_for_selection = FALSE;
#जर defined(FEAT_EVAL)
	    माप_प्रकार  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    जर (type == DT_CSCOPE && अंक_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    अन्यथा
#endif
	    जर (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = मोजणी > 0 ? मोजणी - 1 : 0;
	    अन्यथा जर (type == DT_SELECT || (type == DT_JUMP && अंक_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, अंक_matches, matches);
		ask_for_selection = TRUE;
	    }
#जर defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    अन्यथा जर (type == DT_LTAG)
	    {
		जर (add_llist_tags(tag, अंक_matches, matches) == FAIL)
		    जा end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    जर (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_संख्या(रिक्त);
		जर (i <= 0 || i > अंक_matches || got_int)
		{
		    // no valid choice: don't change anything
		    जर (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    थांबा;
		}
		cur_match = i - 1;
	    }

	    जर (cur_match >= अंक_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		जर ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == रिक्त)
		{
		    जर (अंक_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    अन्यथा
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_संदेश = TRUE;
		}
		cur_match = अंक_matches - 1;
	    }
	    जर (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		जर (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#जर defined(FEAT_QUICKFIX)
	    अन्यथा जर (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    जर (nofile_fname != रिक्त && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    जर (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (अंक_matches > 1 || ic)
		&& !skip_संदेश)
	    {
		// Give an indication of the number of matching tags
		प्र_लिखो((अक्षर *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				अंक_matches,
				max_अंक_matches != MAXCOL ? _(" or more") : "");
		जर (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		जर ((अंक_matches > prev_अंक_matches || new_tag)
							   && अंक_matches > 1)
		{
		    जर (ic)
			संदेश_attr((अक्षर *)IObuff, HL_ATTR(HLF_W));
		    अन्यथा
			संदेश((अक्षर *)IObuff);
		    संदेश_scroll = TRUE;	// don't overwrite this message
		}
		अन्यथा
		    give_warning(IObuff, ic);
		जर (ic && !संदेश_scrolled && संदेश_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#जर defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (पूर्णांक)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#जर defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, रिक्त, -1);
#endif

	    जर (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		जर ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_अंक_matches != MAXCOL
					     || cur_match < अंक_matches - 1)))
		{
		    error_cur_match = cur_match;
		    जर (use_tagstack)
			--tagstackidx;
		    जर (type == DT_PREV)
			--cur_match;
		    अन्यथा
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    सुरूठेवा;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    अन्यथा
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		जर (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	थांबा;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    जर (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    परत jumped_to_tag;
#अन्यथा
    परत FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    स्थिर शून्य
print_tag_list(
    पूर्णांक		new_tag,
    पूर्णांक		use_tagstack,
    पूर्णांक		अंक_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णांक		tagstackidx = curwin->w_tagstackidx;
    पूर्णांक		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    पूर्णांक		taglen;
    पूर्णांक		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (पूर्णांक)(tagp.tagname_end - tagp.tagname + 2);
    जर (taglen < 18)
	taglen = 18;
    जर (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    जर (संदेश_col == 0)
	संदेश_didout = FALSE;	// overwrite previous message
    संदेश_start();
    संदेश_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    संदेश_clr_eos();
    taglen_advance(taglen);
    संदेश_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    साठी (i = 0; i < अंक_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	जर (!new_tag && (
#जर defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	अन्यथा
	    *IObuff = ' ';
	vim_snprintf((अक्षर *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	संदेश_puts((अक्षर *)IObuff);
	जर (tagp.tagkind != रिक्त)
	    संदेश_outtrans_len(tagp.tagkind,
			  (पूर्णांक)(tagp.tagkind_end - tagp.tagkind));
	संदेश_advance(13);
	संदेश_outtrans_len_attr(tagp.tagname,
			   (पूर्णांक)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	संदेश_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	जर (p != रिक्त)
	{
	    संदेश_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	जर (संदेश_col > 0)
	    संदेश_putchar('\n');
	जर (got_int)
	    थांबा;
	संदेश_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	जर (command_end != रिक्त)
	{
	    p = command_end + 3;
	    पर्यंत (*p && *p != '\r' && *p != '\n')
	    {
		पर्यंत (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		जर (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    सुरूठेवा;
		}
		// skip "kind:<kind>" and "<kind>"
		जर (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    सुरूठेवा;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		पर्यंत (*p && *p != '\r' && *p != '\n')
		{
		    जर (संदेश_col + ptr2cells(p) >= cmdline_width)
		    {
			संदेश_putchar('\n');
			जर (got_int)
			    थांबा;
			संदेश_advance(15);
		    }
		    p = संदेश_outtrans_one(p, attr);
		    जर (*p == TAB)
		    {
			संदेश_puts_attr(" ", attr);
			थांबा;
		    }
		    जर (*p == ':')
			attr = 0;
		}
	    }
	    जर (संदेश_col > 15)
	    {
		संदेश_putchar('\n');
		जर (got_int)
		    थांबा;
		संदेश_advance(15);
	    }
	}
	अन्यथा
	{
	    साठी (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	जर (*p == '/' || *p == '?')
	{
	    ++p;
	    जर (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	पर्यंत (p != command_end && vim_isspace(*p))
	    ++p;

	पर्यंत (p != command_end)
	{
	    जर (संदेश_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		संदेश_putchar('\n');
	    जर (got_int)
		थांबा;
	    संदेश_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    जर (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    जर (*p == TAB)
	    {
		संदेश_putchar(' ');
		++p;
	    }
	    अन्यथा
		p = संदेश_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    जर (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		थांबा;
	    // don't display matching '/' or '?'
	    जर (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		थांबा;
	}
	जर (संदेश_col)
	    संदेश_putchar('\n');
	ui_breakcheck();
    }
    जर (got_int)
	got_int = FALSE;	// only stop the listing
}

#जर defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    स्थिर पूर्णांक
add_llist_tags(
    char_u	*tag,
    पूर्णांक		अंक_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    पूर्णांक		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    जर (list == रिक्त || fname == रिक्त || cmd == रिक्त)
    {
	vim_free(cmd);
	vim_free(fname);
	जर (list != रिक्त)
	    list_free(list);
	परत FAIL;
    }

    साठी (i = 0; i < अंक_matches; ++i)
    {
	पूर्णांक	    len, cmd_len;
	दीर्घ    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (पूर्णांक)(tagp.tagname_end - tagp.tagname);
	जर (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	जर (p == रिक्त)
	    सुरूठेवा;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	जर (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = म_से_द((अक्षर *)tagp.command);
	अन्यथा
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    जर (cmd_end == रिक्त)
	    {
		साठी (p = tagp.command;
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
	    जर (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    जर (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    जर (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (पूर्णांक)(cmd_end - cmd_start + 1);
	    जर (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    जर (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	जर ((dict = dict_alloc()) == रिक्त)
	    सुरूठेवा;
	जर (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    सुरूठेवा;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_संख्या(dict, "lnum", lnum);
	जर (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((अक्षर *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, रिक्त);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    परत OK;
}
#endif

/*
 * Free cached tags.
 */
    शून्य
tag_freematch(शून्य)
{
    VIM_CLEAR(tagmatchname);
}

    स्थिर शून्य
taglen_advance(पूर्णांक l)
{
    जर (l == MAXCOL)
    {
	संदेश_putchar('\n');
	संदेश_advance(24);
    }
    अन्यथा
	संदेश_advance(13 + l);
}

/*
 * Print the tag stack
 */
    शून्य
do_tags(exarg_T *eap UNUSED)
{
    पूर्णांक		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णांक		tagstackidx = curwin->w_tagstackidx;
    पूर्णांक		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    संदेश_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    साठी (i = 0; i < tagstacklen; ++i)
    {
	जर (tagstack[i].tagname != रिक्त)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    जर (name == रिक्त)	    // file name not available
		सुरूठेवा;

	    संदेश_putchar('\n');
	    vim_snprintf((अक्षर *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    संदेश_outtrans(IObuff);
	    संदेश_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    जर (tagstackidx == tagstacklen)	// idx at top of stack
	संदेश_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    स्थिर पूर्णांक
tag_strnicmp(char_u *s1, char_u *s2, माप_प्रकार len)
{
    पूर्णांक		i;

    पर्यंत (len > 0)
    {
	i = (पूर्णांक)TOUPPER_ASC(*s1) - (पूर्णांक)TOUPPER_ASC(*s2);
	जर (i != 0)
	    परत i;			// this character different
	जर (*s1 == NUL)
	    थांबा;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    परत 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
प्रकारव्याख्या संरचना
{
    char_u	*pat;		// the pattern
    पूर्णांक		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    पूर्णांक		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    स्थिर शून्य
prepare_pats(pat_T *pats, पूर्णांक has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    जर (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	जर (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	अन्यथा जर (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	जर (pats->head == pats->pat)
	    pats->headlen = 0;
	अन्यथा
	    साठी (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		जर (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != रिक्त)
		    थांबा;
	जर (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    जर (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    अन्यथा
	pats->regmatch.regprog = रिक्त;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
प्रकारव्याख्या गणना {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    स्थिर tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    जर (VIM_ISDIGIT(*cmd))
	परत *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    जर (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	परत (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    जर (vim_strpbrk(cmd, (char_u *)"\t\r\n") != रिक्त)
	परत TAGCMD_INVALID;
    परत TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    स्थिर पूर्णांक
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    पूर्णांक		*match_मोजणी,	// here the number of tags found will be placed
    पूर्णांक		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    पूर्णांक		ntags = 0;
    पूर्णांक		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    जर (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == रिक्त
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	परत FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    जर ((d = dict_alloc_lock(VAR_FIXED)) == रिक्त)
	परत FAIL;
    जर (!(flags & TAG_INS_COMP) && tag->user_data != रिक्त)
	dict_add_string(d, "user_data", tag->user_data);
    जर (buf_ffname != रिक्त)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((अक्षर *)flagString, आकार(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    जर (result == FAIL)
	परत FAIL;
    जर (rettv.v_type == VAR_SPECIAL && rettv.vval.v_संख्या == VVAL_NULL)
    {
	clear_tv(&rettv);
	परत NOTDONE;
    }
    जर (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	परत FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_वस्तुसंच(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	पूर्णांक		len;
	dict_iterator_T	iter;
	char_u		*dict_किल्ली;
	typval_T	*tv;
	पूर्णांक		has_extra = 0;
	पूर्णांक		name_only = flags & TAG_NAMES;

	जर (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    थांबा;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# अन्यथा
	len = 2;
# endif
	res_name = रिक्त;
	res_fname = रिक्त;
	res_cmd = रिक्त;
	res_kind = रिक्त;

	dict_iterate_start(&item->li_tv, &iter);
	पर्यंत (रिक्त != (dict_किल्ली = dict_iterate_next(&iter, &tv)))
	{
	    जर (tv->v_type != VAR_STRING || tv->vval.v_string == रिक्त)
		सुरूठेवा;

	    len += (पूर्णांक)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    जर (STRCMP(dict_किल्ली, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		सुरूठेवा;
	    }
	    जर (STRCMP(dict_किल्ली, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		सुरूठेवा;
	    }
	    जर (STRCMP(dict_किल्ली, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		सुरूठेवा;
	    }
	    has_extra = 1;
	    जर (STRCMP(dict_किल्ली, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		सुरूठेवा;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (पूर्णांक)STRLEN(dict_किल्ली) + 1;
	}

	जर (has_extra)
	    len += 2;	// need space for ;"

	जर (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    थांबा;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	जर (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    थांबा;
	}
	जर (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	जर (name_only)
	    mfp = vim_strsave(res_name);
	अन्यथा
	    mfp = alloc(आकार(char_u) + len + 1);

	जर (mfp == रिक्त)
	    सुरूठेवा;

	जर (!name_only)
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

	    जर (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    जर (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		जर (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		पर्यंत (रिक्त != (dict_किल्ली = dict_iterate_next(&iter, &tv)))
		{
		    जर (tv->v_type != VAR_STRING || tv->vval.v_string == रिक्त)
			सुरूठेवा;

		    जर (STRCMP(dict_किल्ली, "name") == 0)
			सुरूठेवा;
		    जर (STRCMP(dict_किल्ली, "filename") == 0)
			सुरूठेवा;
		    जर (STRCMP(dict_किल्ली, "cmd") == 0)
			सुरूठेवा;
		    जर (STRCMP(dict_किल्ली, "kind") == 0)
			सुरूठेवा;

		    *p++ = TAB;
		    STRCPY(p, dict_किल्ली);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	जर (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	अन्यथा
	{
	    vim_free(mfp);
	    थांबा;
	}
    }

    clear_tv(&rettv);

    *match_मोजणी = ntags;
    परत result;
}
#endif

/*
 * State information used during a tag search
 */
प्रकारव्याख्या संरचना
{
    tagsearch_state_T	state;		// tag search state
    पूर्णांक		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    पूर्णांक		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    खाता	*fp;			// current tags file pointer
    पूर्णांक		flags;			// flags used for tag search
    पूर्णांक		tag_file_sorted;	// !_TAG_FILE_SORTED value
    पूर्णांक		get_searchpat;		// used for 'showfulltag'
    पूर्णांक		help_only;		// only search for help tags
    पूर्णांक		did_open;		// did open a tag file
    पूर्णांक		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    पूर्णांक		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    पूर्णांक		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    पूर्णांक		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    पूर्णांक		is_txt;			// flag of file extension
#endif
    पूर्णांक		match_मोजणी;		// number of matches found
    garray_T	ga_match[MT_मोजणी];	// stores matches in sequence
    hashtab_T	ht_match[MT_मोजणी];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    स्थिर पूर्णांक
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    पूर्णांक			flags,
    पूर्णांक			mincount)
{
    पूर्णांक		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = रिक्त;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (पूर्णांक)STRLEN(pat);
    st->orgpat->regmatch.regprog = रिक्त;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = रिक्त;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_मोजणी = 0;
    st->stop_searching = FALSE;

    साठी (mtt = 0; mtt < MT_मोजणी; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], आकार(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    जर (st->tag_fname == रिक्त
	    || st->lbuf == रिक्त
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == रिक्त
#endif
       )
	परत FAIL;

    परत OK;
}

/*
 * Free the state used by find_tags()
 */
    स्थिर शून्य
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
    स्थिर पूर्णांक
findtags_in_help_init(findtags_state_T *st)
{
    पूर्णांक		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    जर (st->is_txt)
	STRCPY(st->help_lang, "en");
    अन्यथा
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (पूर्णांक)STRLEN(st->tag_fname);
	जर (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	अन्यथा
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    जर (st->help_lang_find != रिक्त
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	परत FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    जर ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == रिक्त
	    && curbuf->b_fname != रिक्त
	    && (i = (पूर्णांक)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    अन्यथा
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	साठी (s = p_hlg; *s != NUL; ++s)
	{
	    जर (STRNICMP(s, st->help_lang, 2) == 0)
		थांबा;
	    ++st->help_pri;
	    जर ((s = vim_strchr(s, ',')) == रिक्त)
		थांबा;
	}
	जर (s == रिक्त || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    जर (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    परत TRUE;
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
    स्थिर पूर्णांक
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    पूर्णांक		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    पूर्णांक		retval;

    जर (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	परत NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_मोजणी,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    परत retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
स्थिर संरचना
{
    खाता	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
स्थिर पूर्णांक incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    स्थिर शून्य
emacs_tags_incstack_free(शून्य)
{
    पर्यंत (incstack_idx)
    {
	--incstack_idx;
	ख_बंद(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = रिक्त;
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
    स्थिर शून्य
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    जर (vim_fgets(st->ebuf, LSIZE, st->fp))
	परत;

    साठी (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    जर (*p == NUL)
	परत;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    जर (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	परत;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    जर (incstack[incstack_idx].etag_fname == रिक्त)
	परत;

    incstack[incstack_idx].fp = st->fp;
    st->fp = रिक्त;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    जर (fullpath_ebuf != रिक्त)
    {
	st->fp = mch_fopen((अक्षर *)fullpath_ebuf, "r");
	जर (st->fp != रिक्त)
	{
	    जर (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    जर (st->fp == रिक्त)
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
    स्थिर पूर्णांक
emacs_tags_file_eof(findtags_state_T *st)
{
    जर (!incstack_idx)	// reached end of file. stop processing.
	परत FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    ख_बंद(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    परत TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    स्थिर पूर्णांक
emacs_tags_parse_ओळ(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    जर (p_7f == रिक्त)
    {
etag_fail:
	जर (vim_strchr(lbuf, '\n') != रिक्त)
	    परत FAIL;

	// Truncated line.  Ignore it.
	जर (p_verbose >= 5)
	{
	    verbose_enter();
	    संदेश(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	परत OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    जर (p == रिक्त)
	p = p_7f + 1;
    अन्यथा
	++p;

    जर (!VIM_ISDIGIT(*p))	    // check for start of line number
	जा etag_fail;
    tagp->command = p;

    जर (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    अन्यथा			    // second format: isolate tagname
    {
	जर (p_7f == lbuf)
	    जा etag_fail;

	// find end of tagname
	साठी (p = p_7f - 1; !vim_iswordc(*p); --p)
	    जर (p == lbuf)
		जा etag_fail;
	tagp->tagname_end = p + 1;
	पर्यंत (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    परत OK;
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
    स्थिर tags_read_status_T
findtags_get_next_ओळ(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    पूर्णांक		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    जर (st->state == TS_द्विमान)
    {
	offset = sinfo_p->खालचा_offset + ((sinfo_p->वरचा_offset
						- sinfo_p->खालचा_offset) / 2);
	जर (offset == sinfo_p->curr_offset)
	    परत TAGS_READ_EOF; // End the binary search without a match.
	अन्यथा
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    अन्यथा जर (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	जर (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    शुरुआत(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    जर (st->state == TS_द्विमान || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, शुरू_से);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	जर (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    जर (sinfo_p->curr_offset == sinfo_p->वरचा_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->खालचा_offset, शुरू_से);
		sinfo_p->curr_offset = sinfo_p->खालचा_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	पर्यंत (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	जर (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    परत TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    अन्यथा
    {
	// skip empty and blank lines
	करा
	{
#ifdef FEAT_CSCOPE
	    जर (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    अन्यथा
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} पर्यंत (!eof && vim_isblankline(st->lbuf));

	जर (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    जर (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		परत TAGS_READ_IGNORE;
#endif
	    परत TAGS_READ_EOF;
	}
    }

    परत TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    स्थिर पूर्णांक
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    जर (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	परत TRUE;

    // Process the header line.
    जर (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    जर (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	साठी (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    परत FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    स्थिर पूर्णांक
findtags_start_state_handler(
    findtags_state_T	*st,
    पूर्णांक			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    पूर्णांक		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    पूर्णांक		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    जर (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	परत findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    जर (st->linear || use_cscope)
#अन्यथा
    जर (st->linear)
#endif
	st->state = TS_LINEAR;
    अन्यथा जर (st->tag_file_sorted == NUL)
	st->state = TS_द्विमान;
    अन्यथा जर (st->tag_file_sorted == '1')
	st->state = TS_द्विमान;
    अन्यथा जर (st->tag_file_sorted == '2')
    {
	st->state = TS_द्विमान;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    अन्यथा
	st->state = TS_LINEAR;

    जर (st->state == TS_द्विमान && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    जर (st->state == TS_द्विमान)
    {
	जर (vim_fseek(st->fp, 0L, अंत_से) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	अन्यथा
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, शुरू_से);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->खालचा_offset = 0;
	    sinfo_p->खालचा_char = 0;
	    sinfo_p->वरचा_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->वरचा_char = 0xff;
	}
	परत FALSE;
    }

    परत TRUE;
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
    स्थिर tagmatch_status_T
findtags_parse_ओळ(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    पूर्णांक		status;
    पूर्णांक		i;
    पूर्णांक		cmplen;
    पूर्णांक		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    जर (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	जर (tagpp->tagname_end == रिक्त)
	    // Corrupted tag line.
	    परत TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (पूर्णांक)(tagpp->tagname_end - tagpp->tagname);
	जर (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	जर ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	अन्यथा जर (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    परत TAG_MATCH_NEXT;

	जर (st->state == TS_द्विमान)
	{
	    // Simplistic check for unsorted tags file.
	    i = (पूर्णांक)tagpp->tagname[0];
	    जर (margs->sortic)
		i = (पूर्णांक)TOUPPER_ASC(tagpp->tagname[0]);
	    जर (i < sinfo_p->खालचा_char || i > sinfo_p->वरचा_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    जर (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(माप_प्रकार)cmplen);
	    अन्यथा
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    जर (tagcmp == 0)
	    {
		जर (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		अन्यथा जर (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    जर (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		परत TAG_MATCH_NEXT;
	    }
	    जर (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		जर (sinfo_p->curr_offset < sinfo_p->वरचा_offset)
		{
		    sinfo_p->खालचा_offset = sinfo_p->curr_offset;
		    जर (margs->sortic)
			sinfo_p->खालचा_char = TOUPPER_ASC(tagpp->tagname[0]);
		    अन्यथा
			sinfo_p->खालचा_char = tagpp->tagname[0];
		    परत TAG_MATCH_NEXT;
		}
	    }
	    जर (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->वरचा_offset)
	    {
		sinfo_p->वरचा_offset = sinfo_p->curr_offset;
		जर (margs->sortic)
		    sinfo_p->वरचा_char = TOUPPER_ASC(tagpp->tagname[0]);
		अन्यथा
		    sinfo_p->वरचा_char = tagpp->tagname[0];
		परत TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    परत TAG_MATCH_STOP;
	}
	अन्यथा जर (st->state == TS_SKIP_BACK)
	{
	    जर (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    अन्यथा
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    परत TAG_MATCH_NEXT;
	}
	अन्यथा जर (st->state == TS_STEP_FORWARD)
	{
	    जर (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		जर ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    परत TAG_MATCH_STOP;	// past last match
		अन्यथा
		    परत TAG_MATCH_NEXT;	// before first match
	    }
	}
	अन्यथा
	    // skip this match if it can't match
	    जर (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		परत TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	जर (tagpp->fname_end == रिक्त)
	    status = FAIL;
	अन्यथा
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    अन्यथा
	status = parse_tag_ओळ(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    जर (status == FAIL)
	परत TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    जर (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    परत TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    स्थिर शून्य
findtags_matchargs_init(findtags_match_args_T *margs, पूर्णांक flags)
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
    स्थिर पूर्णांक
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    पूर्णांक		match = FALSE;
    पूर्णांक		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (पूर्णांक)(tagpp->tagname_end - tagpp->tagname);
    जर (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    जर (st->orgpat->len != cmplen)
	match = FALSE;
    अन्यथा
    {
	जर (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    जर (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	अन्यथा
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    जर (!match && st->orgpat->regmatch.regprog != रिक्त)
    {
	पूर्णांक	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	जर (match)
	{
	    margs->matchoff = (पूर्णांक)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    जर (st->orgpat->regmatch.rm_ic)
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

    परत match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    स्थिर शून्य
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_ओळ;
    पूर्णांक		len;

    conv_ओळ = string_convert(&st->vimconv, st->lbuf, रिक्त);
    जर (conv_ओळ == रिक्त)
	परत;

    // Copy or swap lbuf and conv_line.
    len = (पूर्णांक)STRLEN(conv_ओळ) + 1;
    जर (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_ओळ;
	st->lbuf_size = len;
    }
    अन्यथा
    {
	STRCPY(st->lbuf, conv_ओळ);
	vim_free(conv_ओळ);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    स्थिर पूर्णांक
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    पूर्णांक		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    पूर्णांक		name_only = (st->flags & TAG_NAMES);
    पूर्णांक		mtt;
    पूर्णांक		len = 0;
    पूर्णांक		is_current;		// file name matches
    पूर्णांक		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    जर (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    अन्यथा
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
	जर (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	जर (is_static)
	{
	    जर (is_current)
		mtt = MT_ST_CUR;
	    अन्यथा
		mtt = MT_ST_OTH;
	}
	अन्यथा
	{
	    जर (is_current)
		mtt = MT_GL_CUR;
	    अन्यथा
		mtt = MT_GL_OTH;
	}
	जर (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	जर (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    जर (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#अन्यथा
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (पूर्णांक)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(आकार(char_u) + len + 10 + ML_EXTRA + 1);
	जर (mfp != रिक्त)
	{
	    पूर्णांक heuristic;

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
	    प्र_लिखो((अक्षर *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    अन्यथा जर (name_only)
    {
	जर (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    जर (*temp_end == '/')
		पर्यंत (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    जर (tagpp->command + 2 < temp_end)
	    {
		len = (पूर्णांक)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		जर (mfp != रिक्त)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    अन्यथा
		mfp = रिक्त;
	    st->get_searchpat = FALSE;
	}
	अन्यथा
	{
	    len = (पूर्णांक)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(आकार(char_u) + len + 1);
	    जर (mfp != रिक्त)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    जर (State & MODE_घाला)
		st->get_searchpat = p_sft;
	}
    }
    अन्यथा
    {
	माप_प्रकार tag_fname_len = STRLEN(st->tag_fname);
#ifdef FEAT_EMACS_TAGS
	माप_प्रकार ebuf_len = 0;
#endif

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (पूर्णांक)tag_fname_len + (पूर्णांक)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	जर (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (पूर्णांक)ebuf_len + 1;
	}
	अन्यथा
	    ++len;
#endif
	mfp = alloc(आकार(char_u) + len + 1);
	जर (mfp != रिक्त)
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
	    जर (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    अन्यथा
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    जर (mfp != रिक्त)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	जर (use_cscope)
	    ++*hash;
	अन्यथा
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	जर (HASHITEM_EMPTY(hi))
	{
	    जर (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		परत FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_मोजणी++;
	}
	अन्यथा
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    परत OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    स्थिर शून्य
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	शोध_info;
    पूर्णांक			retval;
#ifdef FEAT_CSCOPE
    पूर्णांक			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(शोध_info);

    // Read and parse the lines in the file one by one
    साठी (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	जर (st->state == TS_द्विमान || st->state == TS_SKIP_BACK)
	    ओळ_breakcheck();
	अन्यथा
	    fast_breakcheck();
	जर ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_किल्ल्या(30, FALSE);
	जर (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    थांबा;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	जर (st->mincount == TAG_MANY && st->match_मोजणी >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    थांबा;
	}
	जर (st->get_searchpat)
	    जा ओळ_read_in;

	retval = findtags_get_next_ओळ(st, &शोध_info);
	जर (retval == TAGS_READ_IGNORE)
	    सुरूठेवा;
	जर (retval == TAGS_READ_EOF)
	    थांबा;

ओळ_read_in:

	जर (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	जर (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    सुरूठेवा;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	जर (st->state == TS_START)
	{
	    जर (findtags_start_state_handler(st, &margs->sortic, &शोध_info) == FALSE)
		सुरूठेवा;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	जर (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    जर (st->lbuf == रिक्त)
	    {
		जर (st->fp != रिक्त)
		    ख_बंद(st->fp);
		st->fp = रिक्त;
		st->stop_searching = TRUE;
		परत;
	    }

	    जर (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, शोध_info.curr_offset,
								     शुरू_से);
	    // this will try the same thing again, make sure the offset is
	    // different
	    शोध_info.curr_offset = 0;
	    सुरूठेवा;
	}

	retval = findtags_parse_ओळ(st, &tagp, margs, &शोध_info);
	जर (retval == TAG_MATCH_NEXT)
	    सुरूठेवा;
	जर (retval == TAG_MATCH_STOP)
	    थांबा;
	जर (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    जर (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (दीर्घ)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    परत;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	जर (findtags_match_tag(st, &tagp, margs))
	{
	    जर (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		थांबा;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    स्थिर शून्य
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    पूर्णांक		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = रिक्त;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    जर (use_cscope)
	st->fp = रिक्त;	    // avoid GCC warning
    अन्यथा
#endif
    {
#ifdef FEAT_MULTI_LANG
	जर (curbuf->b_help)
	{
	    जर (!findtags_in_help_init(st))
		परत;
	}
#endif

	st->fp = mch_fopen((अक्षर *)st->tag_fname, "r");
	जर (st->fp == रिक्त)
	    परत;

	जर (p_verbose >= 5)
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

    जर (st->fp != रिक्त)
    {
	ख_बंद(st->fp);
	st->fp = रिक्त;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    जर (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, रिक्त, रिक्त);

    जर (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    जर (st->match_मोजणी >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    स्थिर पूर्णांक
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    पूर्णांक		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    पूर्णांक		mtt;
    पूर्णांक		i;
    char_u	*mfp;
    char_u	*p;

    जर (st->match_मोजणी > 0)
	matches = ALLOC_MULT(char_u *, st->match_मोजणी);
    अन्यथा
	matches = रिक्त;
    st->match_मोजणी = 0;
    साठी (mtt = 0; mtt < MT_मोजणी; ++mtt)
    {
	साठी (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    जर (matches == रिक्त)
		vim_free(mfp);
	    अन्यथा
	    {
		जर (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    साठी (p = mfp + 1; *p != NUL; ++p)
			जर (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_मोजणी++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    परत st->match_मोजणी;
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
    पूर्णांक
find_tags(
    char_u	*pat,			// pattern to search for
    पूर्णांक		*अंक_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    पूर्णांक		flags,
    पूर्णांक		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    पूर्णांक		first_file;		// trying first tag file
    पूर्णांक		retval = FAIL;		// return value
    पूर्णांक		round;

    पूर्णांक		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    पूर्णांक		i;
    char_u	*saved_pat = रिक्त;		// copy of pat[]
#endif

    पूर्णांक		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    पूर्णांक		has_re = (flags & TAG_REGEXP);	// regexp used
    पूर्णांक		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    पूर्णांक		use_cscope = (flags & TAG_CSCOPE);
#endif
    पूर्णांक		verbose = (flags & TAG_VERBOSE);
    पूर्णांक		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    निवड (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	स्थिती TC_FOLLOWIC:		 थांबा;
	स्थिती TC_IGNORE:    p_ic = TRUE;  थांबा;
	स्थिती TC_MATCH:     p_ic = FALSE; थांबा;
	स्थिती TC_FOLLOWSCS: p_ic = ignorecase(pat); थांबा;
	स्थिती TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); थांबा;
    }

    help_save = curbuf->b_help;

    जर (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	जा findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    जर (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    अन्यथा जर (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    जर (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	जर (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    जर (saved_pat != रिक्त)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    जर (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    जर (has_re && st.orgpat->regmatch.regprog == रिक्त)
	जा findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    जर (retval != NOTDONE)
	जा findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    जर ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == रिक्त
	    && curbuf->b_fname != रिक्त
	    && (i = (पूर्णांक)STRLEN(curbuf->b_fname)) > 4
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
    साठी (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      साठी (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  जर (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      थांबा;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	जर (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	जर (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    थांबा;
#ifdef FEAT_CSCOPE
	जर (use_cscope)
	    थांबा;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    जर (!st.stop_searching)
    {
	जर (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    जर (retval == FAIL)
	st.match_मोजणी = 0;

    *अंक_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    परत retval;
}

स्थिर garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    स्थिर शून्य
found_tagfile_cb(char_u *fname, शून्य *cookie UNUSED)
{
    जर (ga_grow(&tag_fnames, 1) == FAIL)
	परत;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#जर defined(EXITFREE)
    शून्य
free_tag_stuff(शून्य)
{
    ga_clear_strings(&tag_fnames);
    जर (curwin != रिक्त)
	do_tag(रिक्त, DT_FREE, 0, 0, 0);
    tag_freematch();

# जर defined(FEAT_QUICKFIX)
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
    पूर्णांक
get_tagfname(
    tagname_T	*tnp,	// holds status info
    पूर्णांक		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = रिक्त;
    char_u		*r_ptr;
    पूर्णांक			i;

    जर (first)
	CLEAR_POINTER(tnp);

    जर (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	जर (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, आकार(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# अन्यथा
		    "doc/tags doc/tags-??"
# endif
#अन्यथा
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, रिक्त);
	}

	जर (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    जर (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		परत FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    साठी (i = 0; i < tag_fnames.ga_len; ++i)
		जर (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    परत FAIL; // avoid duplicate file names
	}
	अन्यथा
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	परत OK;
    }

    जर (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	जर (tnp->tn_tags == रिक्त)
	    परत FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    साठी (;;)
    {
	जर (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_शोध_ctx);
	    जर (fname != रिक्त)
		थांबा;

	    tnp->tn_did_filefind_init = FALSE;
	}
	अन्यथा
	{
	    char_u  *filename = रिक्त;

	    // Stop when used all parts of 'tags'.
	    जर (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_शोध_ctx);
		tnp->tn_शोध_ctx = रिक्त;
		परत FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (शून्य)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    जर (r_ptr != रिक्त)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_शोध_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_शोध_ctx, TRUE, curbuf->b_ffname);
	    जर (tnp->tn_शोध_ctx != रिक्त)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    परत OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    शून्य
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_शोध_ctx);
    tnp->tn_शोध_ctx = रिक्त;
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
    स्थिर पूर्णांक
parse_tag_ओळ(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    पूर्णांक		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    जर (is_etag)
	// emacs-style tag file
	परत emacs_tags_parse_ओळ(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    जर (p == रिक्त)
	परत FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    जर (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    जर (p == रिक्त)
	परत FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    जर (*p != NUL)
	++p;
    जर (*p == NUL)
	परत FAIL;
    tagp->command = p;

    परत OK;
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
    स्थिर पूर्णांक
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    पर्यंत ((p = vim_strchr(p, '\t')) != रिक्त)
    {
	++p;
	जर (STRNCMP(p, "file:", 5) == 0)
	    परत TRUE;
    }

    परत FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    स्थिर माप_प्रकार
matching_ओळ_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    परत (p - lbuf) + STRLEN(p);
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
    स्थिर पूर्णांक
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    पूर्णांक		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    जर (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    अन्यथा
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_ओळ(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = रिक्त;
    tagp->user_data = रिक्त;
    tagp->tagline = 0;
    tagp->command_end = रिक्त;

    जर (retval != OK)
	परत retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    जर (find_extra(&p) == OK)
    {
	जर (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	अन्यथा
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	जर (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    पर्यंत (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		जर (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		अन्यथा जर (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		अन्यथा जर (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = म_से_प((अक्षर *)p + 5);
		जर (tagp->tagkind != रिक्त && tagp->user_data != रिक्त)
		    थांबा;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		जर (pc == रिक्त || (pt != रिक्त && pc > pt))
		    tagp->tagkind = p;
		जर (pt == रिक्त)
		    थांबा;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    जर (tagp->tagkind != रिक्त)
    {
	साठी (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    जर (tagp->user_data != रिक्त)
    {
	साठी (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    परत retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    स्थिर char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    पूर्णांक		c;

#ifdef FEAT_EMACS_TAGS
    जर (tagp->is_etag)
	c = 0;	    // to shut up GCC
    अन्यथा
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    जर (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    परत fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    स्थिर पूर्णांक
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    पूर्णांक		forceit,	// :ta with !
    पूर्णांक		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    पूर्णांक		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = रिक्त;
    char_u	*fname;
    tagptrs_T	tagp;
    पूर्णांक		retval = FAIL;
    पूर्णांक		getfile_result = GETFILE_UNUSED;
    पूर्णांक		शोध_options;
#ifdef FEAT_शोध_EXTRA
    पूर्णांक		save_no_hlsearch;
#endif
#जर defined(FEAT_QUICKFIX)
    win_T	*curwin_save = रिक्त;
#endif
    char_u	*full_fname = रिक्त;
#ifdef FEAT_FOLDING
    पूर्णांक		old_किल्ली_Typed = किल्ली_Typed;    // getting the file may reset it
#endif
    माप_प्रकार	len;
    char_u	*lbuf;
    पूर्णांक		है_अंक = FALSE;

    जर (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	परत FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ओळ_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    जर (lbuf != रिक्त)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    जर (pbuf == रिक्त || lbuf == रिक्त || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = रिक्त;
	जा erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    जर (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	है_अंक = TRUE;
	pbuf[0] = ':';
    }
    साठी (pbuf_end = pbuf + है_अंक;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	जर (tagp.is_etag && *str == ',')// stop at ',' after line number
	    थांबा;
#endif
	*pbuf_end++ = *str++;
	जर (pbuf_end - pbuf + 1 + है_अंक >= LSIZE)
	    थांबा;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    जर (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	जर (है_अंक)
	    str++;
	जर (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    जर (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    अन्यथा
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    जर (fname == रिक्त)
	जा erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    जर (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, रिक्त))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	जर (nofile_fname == रिक्त)
	    nofile_fname = empty_option;
	जा erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#जर defined(FEAT_QUICKFIX)
    जर (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	जर (!curwin->w_p_pvw)
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
    जर (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	जर (existing_buf != रिक्त)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    जर (swbuf_goto_win_with_buf(existing_buf) != रिक्त)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    जर (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	जर (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	जर ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	जर (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    जर (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    जा erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    जर (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#जर defined(FEAT_QUICKFIX)
	जर (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	अन्यथा
#endif
	    keep_help_flag = curbuf->b_help;
    }

    जर (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, रिक्त, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    जर (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_शोध_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	जर (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	जर (vim_strchr(p_cpo, CPO_TAGPAT) != रिक्त)
	    शोध_options = 0;
	अन्यथा
	    शोध_options = शोध_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	जर (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	जर (str > pbuf_end - 1)	// search command with nothing following
	{
	    माप_प्रकार pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    जर (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    अन्यथा
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    जर (do_शोध(रिक्त, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (दीर्घ)1,
							 शोध_options, रिक्त))
		retval = OK;
	    अन्यथा
	    {
		पूर्णांक	found = 1;
		पूर्णांक	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		जर (!do_शोध(रिक्त, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (दीर्घ)1,
							 शोध_options, रिक्त))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (शून्य)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((अक्षर *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    जर (!do_शोध(रिक्त, '/', '/', pbuf, pbuflen, (दीर्घ)1,
							 शोध_options, रिक्त))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((अक्षर *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			जर (!do_शोध(रिक्त, '/', '/', pbuf, pbuflen, (दीर्घ)1,
							 शोध_options, रिक्त))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		जर (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		अन्यथा
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    जर (found == 2 || !save_p_ic)
		    {
			संदेश(_(e_couldnt_find_tag_just_guessing));
			जर (!संदेश_scrolled && संदेश_silent == 0)
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
	अन्यथा
	{
	    पूर्णांक		save_secure = secure;

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
	    जर (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_शोध_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	जर (शोध_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	जर (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	जर (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    जर (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    जर ((fdo_flags & FDO_TAG) && old_किल्ली_Typed)
		foldOpenCursor();
#endif
	}

#जर defined(FEAT_QUICKFIX)
	जर (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	जर (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    अन्यथा
    {
	जर (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	जर (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#जर defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	अन्यथा जर (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    जर (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#जर defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    जर (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#जर defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    परत retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    स्थिर char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, पूर्णांक expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = रिक्त;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    जर (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       परत रिक्त;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    जर (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == रिक्त)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, रिक्त,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	जर (expanded_fname != रिक्त)
	    fname = expanded_fname;
    }

    जर ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	जर (retval != रिक्त)
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
    अन्यथा
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    परत retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    स्थिर पूर्णांक
test_for_current(
#ifdef FEAT_EMACS_TAGS
    पूर्णांक	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    पूर्णांक	    c;
    पूर्णांक	    retval = FALSE;
    char_u  *fullname;

    जर (buf_ffname != रिक्त)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	जर (is_etag)
	    c = 0;	    // to shut up GCC
	अन्यथा
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	जर (fullname != रिक्त)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	जर (!is_etag)
#endif
	    *fname_end = c;
    }

    परत retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    स्थिर पूर्णांक
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    साठी (;;)
    {
	जर (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	अन्यथा जर (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    जर (*str != first_char)
		str = रिक्त;
	    अन्यथा
		++str;
	}
	अन्यथा
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)म_माला((अक्षर *)str, "|;\"");
	    जर (str != रिक्त)
	    {
		++str;
		थांबा;
	    }

	}
	जर (str == रिक्त || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    थांबा;
	++str;	// skip ';'
	first_char = *str;
    }

    जर (str != रिक्त && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	परत OK;
    }
    परत FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    शून्य
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    पूर्णांक
expand_tags(
    पूर्णांक		tagnames,	// expand tag names
    char_u	*pat,
    पूर्णांक		*अंक_file,
    char_u	***file)
{
    पूर्णांक		i;
    पूर्णांक		extra_flag;
    char_u	*name_buf;
    माप_प्रकार	name_buf_size = 100;
    tagptrs_T	t_p;
    पूर्णांक		ret;

    name_buf = alloc(name_buf_size);
    जर (name_buf == रिक्त)
	परत FAIL;

    जर (tagnames)
	extra_flag = TAG_NAMES;
    अन्यथा
	extra_flag = 0;
    जर (pat[0] == '/')
	ret = find_tags(pat + 1, अंक_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    अन्यथा
	ret = find_tags(pat, अंक_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    जर (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 साठी (i = 0; i < *अंक_file; i++)
	 {
	     माप_प्रकार	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     जर (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 जर (buf == रिक्त)
		 {
		     vim_free(name_buf);
		     परत FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != रिक्त && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    परत ret;
}

#जर defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
add_tag_field(
    dict_T  *dict,
    अक्षर    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    पूर्णांक		len = 0;
    पूर्णांक		retval;

    // check that the field name doesn't exist yet
    जर (dict_has_किल्ली(dict, field_name))
    {
	जर (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	परत FAIL;
    }
    buf = alloc(MAXPATHL);
    जर (buf == रिक्त)
	परत FAIL;
    जर (start != रिक्त)
    {
	जर (end == रिक्त)
	{
	    end = start + STRLEN(start);
	    पर्यंत (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (पूर्णांक)(end - start);
	जर (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    परत retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    पूर्णांक
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    पूर्णांक		अंक_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    दीर्घ	is_static;

    ret = find_tags(pat, &अंक_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (पूर्णांक)MAXCOL, buf_fname);
    जर (ret != OK || अंक_matches <= 0)
	परत ret;

    साठी (i = 0; i < अंक_matches; ++i)
    {
	जर (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    सुरूठेवा;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	जर (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    सुरूठेवा;
	}

	जर ((dict = dict_alloc()) == रिक्त)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    थांबा;
	}
	जर (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    सुरूठेवा;
	}

	full_fname = tag_full_fname(&tp);
	जर (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    रिक्त) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_संख्या(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	जर (tp.command_end != रिक्त)
	{
	    साठी (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		जर (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		अन्यथा जर (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		अन्यथा जर (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    पूर्णांक	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    पर्यंत (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (पूर्णांक)(p - n);
		    जर (*p == ':' && len > 0)
		    {
			s = ++p;
			पर्यंत (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			जर (add_tag_field(dict, (अक्षर *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    अन्यथा
			// Skip field without colon.
			पर्यंत (*p != NUL && *p >= ' ')
			    ++p;
		    जर (*p == NUL)
			थांबा;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    परत ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    स्थिर शून्य
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_संख्या(retdict, "matchnr", tag->cur_match + 1);
    dict_add_संख्या(retdict, "bufnr", tag->cur_fnum);
    जर (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    जर ((pos = list_alloc_id(aid_tagstack_from)) == रिक्त)
	परत;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_संख्या(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_संख्या(pos, (varnumber_T)fmark->mark.lnum);
    list_append_संख्या(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_संख्या(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    शून्य
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    पूर्णांक		i;
    dict_T	*d;

    dict_add_संख्या(retdict, "length", wp->w_tagstacklen);
    dict_add_संख्या(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_वस्तुसंच);
    जर (l == रिक्त)
	परत;
    dict_add_list(retdict, "items", l);

    साठी (i = 0; i < wp->w_tagstacklen; i++)
    {
	जर ((d = dict_alloc_id(aid_tagstack_details)) == रिक्त)
	    परत;
	जर (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    परत;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    स्थिर शून्य
tagstack_clear(win_T *wp)
{
    पूर्णांक i;

    // Free the current tag stack
    साठी (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    स्थिर शून्य
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    पूर्णांक		i;

    tagstack_clear_entry(&tagstack[0]);
    साठी (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    स्थिर शून्य
tagstack_ढकला_item(
	win_T	*wp,
	char_u	*tagname,
	पूर्णांक	cur_fnum,
	पूर्णांक	cur_match,
	pos_T	mark,
	पूर्णांक	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    पूर्णांक		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    जर (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    जर (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    स्थिर शून्य
tagstack_ढकला_वस्तुसंच(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    पूर्णांक		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	जर (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == रिक्त)
	    सुरूठेवा;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	जर ((di = dict_find(itemdict, (char_u *)"from", -1)) == रिक्त)
	    सुरूठेवा;
	जर (list2fpos(&di->di_tv, &mark, &fnum, रिक्त, FALSE) != OK)
	    सुरूठेवा;
	जर ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == रिक्त)
	    सुरूठेवा;

	जर (mark.col > 0)
	    mark.col--;
	tagstack_ढकला_item(wp, tagname,
		(पूर्णांक)dict_get_संख्या(itemdict, "bufnr"),
		(पूर्णांक)dict_get_संख्या(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    स्थिर शून्य
tagstack_set_curidx(win_T *wp, पूर्णांक curidx)
{
    wp->w_tagstackidx = curidx;
    जर (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    जर (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    पूर्णांक
set_tagstack(win_T *wp, dict_T *d, पूर्णांक action)
{
    dictitem_T	*di;
    list_T	*l = रिक्त;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    जर (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_चळत_within_tagfunc));
	परत FAIL;
    }
# endif

    जर ((di = dict_find(d, (char_u *)"items", -1)) != रिक्त)
    {
	जर (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    परत FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    जर ((di = dict_find(d, (char_u *)"curidx", -1)) != रिक्त)
	tagstack_set_curidx(wp, (पूर्णांक)tv_get_संख्या(&di->di_tv) - 1);

    जर (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	पूर्णांक	tagstackidx = wp->w_tagstackidx;
	पूर्णांक	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	पर्यंत (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    जर (l != रिक्त)
    {
	जर (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ढकला_वस्तुसंच(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    परत OK;
}
#endif
