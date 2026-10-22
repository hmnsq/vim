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
प्रकारनिर्देश संरचना tag_pointers
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
    पूर्णाङ्क		is_etag;	// TRUE for emacs tag
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
प्रकारनिर्देश गणना
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
प्रकारनिर्देश गणना
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
प्रकारनिर्देश संरचना
{
    off_T	खालचा_offset;	// offset for first char of first line that
				// could match
    off_T	वरचा_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    पूर्णाङ्क	खालचा_char;		// first char at low_offset
    पूर्णाङ्क	वरचा_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
प्रकारनिर्देश गणना
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क	matchoff;		// tag match offset
    पूर्णाङ्क	match_re;		// TRUE if the tag matches a regexp
    पूर्णाङ्क	match_no_ic;		// TRUE if the tag matches with case
    पूर्णाङ्क	has_re;			// regular expression used
    पूर्णाङ्क	sortic;			// tags file sorted ignoring case (foldcase)
    पूर्णाङ्क	sort_error;		// tags file not sorted
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

स्थिर वर्ण	*mt_names[MT_मोजणी/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
स्थिर char_u	*nofile_fname = रिक्त;	// fname for NOTAGFILE error

स्थिर शून्य taglen_advance(पूर्णाङ्क l);

स्थिर पूर्णाङ्क jumpto_tag(char_u *lbuf, पूर्णाङ्क forceit, पूर्णाङ्क keep_help);
#ifdef FEAT_EMACS_TAGS
स्थिर पूर्णाङ्क parse_tag_ओळ(char_u *lbuf, पूर्णाङ्क is_etag, tagptrs_T *tagp);
#अन्यथा
स्थिर पूर्णाङ्क parse_tag_ओळ(char_u *lbuf, tagptrs_T *tagp);
#endif
स्थिर पूर्णाङ्क test_for_static(tagptrs_T *);
स्थिर पूर्णाङ्क parse_match(char_u *lbuf, tagptrs_T *tagp);
स्थिर char_u *tag_full_fname(tagptrs_T *tagp);
स्थिर char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, पूर्णाङ्क expand);
#ifdef FEAT_EMACS_TAGS
स्थिर पूर्णाङ्क test_for_current(पूर्णाङ्क, char_u *, char_u *, char_u *, char_u *);
#अन्यथा
स्थिर पूर्णाङ्क test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
स्थिर पूर्णाङ्क find_extra(char_u **pp);
स्थिर शून्य print_tag_list(पूर्णाङ्क new_tag, पूर्णाङ्क use_tagstack, पूर्णाङ्क अंक_matches, char_u **matches);
#यदि defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
स्थिर पूर्णाङ्क add_llist_tags(char_u *tag, पूर्णाङ्क अंक_matches, char_u **matches);
#endif

स्थिर char_u	*tagmatchname = रिक्त;	// name of last used tag

#यदि defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
स्थिर taggy_T ptag_entry = {रिक्त, {{0, 0, 0}, 0}, 0, 0, रिक्त};
#endif

#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
स्थिर callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#यदि defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    वर्ण *
did_set_tagfunc(optset_T *args)
{
    पूर्णाङ्क	retval;

    यदि (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    अन्यथा
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	यदि (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    प्रत्यागम retval == FAIL ? e_invalid_argument : रिक्त;
}
#endif

#यदि defined(EXITFREE)
    शून्य
free_tagfunc_option(शून्य)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    पूर्णाङ्क
set_ref_in_tagfunc(पूर्णाङ्क copyID UNUSED)
{
    पूर्णाङ्क	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    प्रत्यागम abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    शून्य
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    यदि (tfu_cb.cb_name != रिक्त && *tfu_cb.cb_name != NUL)
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
    पूर्णाङ्क
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    पूर्णाङ्क		type,
    पूर्णाङ्क		मोजणी,
    पूर्णाङ्क		forceit,	// :ta with !
    पूर्णाङ्क		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णाङ्क		tagstackidx = curwin->w_tagstackidx;
    पूर्णाङ्क		tagstacklen = curwin->w_tagstacklen;
    पूर्णाङ्क		cur_match = 0;
    पूर्णाङ्क		cur_fnum = curbuf->b_fnum;
    पूर्णाङ्क		oldtagstackidx = tagstackidx;
    पूर्णाङ्क		prevtagstackidx = tagstackidx;
    पूर्णाङ्क		prev_अंक_matches;
    पूर्णाङ्क		new_tag = FALSE;
    पूर्णाङ्क		i;
    पूर्णाङ्क		ic;
    पूर्णाङ्क		no_regexp = FALSE;
    पूर्णाङ्क		error_cur_match = 0;
    पूर्णाङ्क		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    पूर्णाङ्क		jumped_to_tag = FALSE;
#endif
    पूर्णाङ्क		new_अंक_matches;
    char_u	**new_matches;
    पूर्णाङ्क		use_tagstack;
    पूर्णाङ्क		skip_संदेश = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    पूर्णाङ्क		use_tfu = 1;
    char_u	*tofree = रिक्त;

    // remember the matches for the last used tag
    स्थिर पूर्णाङ्क		अंक_matches = 0;
    स्थिर पूर्णाङ्क		max_अंक_matches = 0;  // limit used for match search
    स्थिर char_u	**matches = रिक्त;
    स्थिर पूर्णाङ्क		flags;

#ifdef EXITFREE
    यदि (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(अंक_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	अंक_matches = 0;
	प्रत्यागम FALSE;
    }
#endif

#ifdef FEAT_EVAL
    यदि (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_चळत_within_tagfunc));
	प्रत्यागम FALSE;
    }
#endif

    यदि (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	प्रत्यागम FALSE;

    यदि (type == DT_HELP)
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
    यदि ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#यदि defined(FEAT_QUICKFIX)
	यदि (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    यदि ((ptag_entry.tagname = vim_strsave(tag)) == रिक्त)
		गच्छ end_do_tag;
	}
#endif
    }
    अन्यथा
    {
#यदि defined(FEAT_QUICKFIX)
	यदि (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	अन्यथा
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	यदि (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#यदि defined(FEAT_QUICKFIX)
	    यदि (g_do_tagpreview != 0)
	    {
		यदि (ptag_entry.tagname != रिक्त
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
		    यदि ((ptag_entry.tagname = vim_strsave(tag)) == रिक्त)
			गच्छ end_do_tag;
		}
	    }
	    अन्यथा
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		यावत् (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		यदि (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    प्रति (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = रिक्त;
		}

		/*
		 * put the tag name in the tag stack
		 */
		यदि ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == रिक्त)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    गच्छ end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	अन्यथा
	{
	    यदि (
#यदि defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == रिक्त :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_चळत_empty));
		गच्छ end_do_tag;
	    }

	    यदि (type == DT_काढा)		// go to older position
	    {
#ifdef FEAT_FOLDING
		पूर्णाङ्क	old_किल्ली_Typed = किल्ली_Typed;
#endif
		यदि ((tagstackidx -= मोजणी) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_चळत));
		    यदि (tagstackidx + मोजणी == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			गच्छ end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		अन्यथा यदि (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_शीर्ष_of_tag_चळत));
		    गच्छ end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		यदि (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    यदि (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			गच्छ end_do_tag;
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
		यदि ((fdo_flags & FDO_TAG) && old_किल्ली_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(अंक_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		अंक_matches = 0;
		tag_freematch();
		गच्छ end_do_tag;
	    }

	    यदि (type == DT_TAG
#यदि defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#यदि defined(FEAT_QUICKFIX)
		यदि (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		अन्यथा
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    यदि ((tagstackidx += मोजणी - 1) >= tagstacklen)
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
		    अन्यथा यदि (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_चळत));
			tagstackidx = 0;
			गच्छ end_do_tag;
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

#यदि defined(FEAT_QUICKFIX)
		यदि (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		अन्यथा
#endif
		{
		    यदि (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		विकल्प (type)
		{
		    पक्ष DT_FIRST: cur_match = मोजणी - 1; भङ्ग;
		    पक्ष DT_SELECT:
		    पक्ष DT_JUMP:
#ifdef FEAT_CSCOPE
		    पक्ष DT_CSCOPE:
#endif
		    पक्ष DT_LAST:  cur_match = MAXCOL - 1; भङ्ग;
		    पक्ष DT_NEXT:  cur_match += मोजणी; भङ्ग;
		    पक्ष DT_PREV:  cur_match -= मोजणी; भङ्ग;
		}
		यदि (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		अन्यथा यदि (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_संदेश = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#यदि defined(FEAT_QUICKFIX)
	यदि (g_do_tagpreview != 0)
	{
	    यदि (type != DT_SELECT && type != DT_JUMP)
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
	    यदि (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    यदि (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    यदि (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	यदि (buf != रिक्त)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    प्रति (;;)
    {
	पूर्णाङ्क	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	यदि (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#यदि defined(FEAT_QUICKFIX)
	अन्यथा यदि (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	अन्यथा
	    name = tag;
	other_name = (tagmatchname == रिक्त || STRCMP(tagmatchname, name) != 0);
	यदि (new_tag
		|| (cur_match >= अंक_matches && max_अंक_matches != MAXCOL)
		|| other_name)
	{
	    यदि (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    यदि (type == DT_SELECT || type == DT_JUMP
#यदि defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    यदि (type == DT_TAG)
		max_अंक_matches = MAXCOL;
	    अन्यथा
		max_अंक_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    यदि (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    अन्यथा
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    यदि (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    यदि (verbose)
		flags |= TAG_VERBOSE;

	    यदि (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    यदि (find_tags(name, &new_अंक_matches, &new_matches, flags,
					    max_अंक_matches, buf_ffname) == OK
		    && new_अंक_matches < max_अंक_matches)
		max_अंक_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    यदि (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_अंक_matches, new_matches);
		भङ्ग;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    यदि (!new_tag && !other_name)
	    {
		पूर्णाङ्क	    j, k;
		पूर्णाङ्क	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		प्रति (j = 0; j < अंक_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    प्रति (i = idx; i < new_अंक_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			यदि (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    प्रति (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    भङ्ग;
			}
		    }
		}
	    }
	    FreeWild(अंक_matches, matches);
	    अंक_matches = new_अंक_matches;
	    matches = new_matches;
	}

	यदि (अंक_matches <= 0)
	{
	    यदि (verbose)
		semsg(_(e_tag_not_found_str), name);
#यदि defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	अन्यथा
	{
	    पूर्णाङ्क ask_for_selection = FALSE;
#यदि defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    यदि (type == DT_CSCOPE && अंक_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    अन्यथा
#endif
	    यदि (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = मोजणी > 0 ? मोजणी - 1 : 0;
	    अन्यथा यदि (type == DT_SELECT || (type == DT_JUMP && अंक_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, अंक_matches, matches);
		ask_for_selection = TRUE;
	    }
#यदि defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    अन्यथा यदि (type == DT_LTAG)
	    {
		यदि (add_llist_tags(tag, अंक_matches, matches) == FAIL)
		    गच्छ end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    यदि (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_संख्या(रिक्त);
		यदि (i <= 0 || i > अंक_matches || got_int)
		{
		    // no valid choice: don't change anything
		    यदि (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    भङ्ग;
		}
		cur_match = i - 1;
	    }

	    यदि (cur_match >= अंक_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		यदि ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == रिक्त)
		{
		    यदि (अंक_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    अन्यथा
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_संदेश = TRUE;
		}
		cur_match = अंक_matches - 1;
	    }
	    यदि (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		यदि (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#यदि defined(FEAT_QUICKFIX)
	    अन्यथा यदि (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    यदि (nofile_fname != रिक्त && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    यदि (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (अंक_matches > 1 || ic)
		&& !skip_संदेश)
	    {
		// Give an indication of the number of matching tags
		sprintf((वर्ण *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				अंक_matches,
				max_अंक_matches != MAXCOL ? _(" or more") : "");
		यदि (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		यदि ((अंक_matches > prev_अंक_matches || new_tag)
							   && अंक_matches > 1)
		{
		    यदि (ic)
			संदेश_attr((वर्ण *)IObuff, HL_ATTR(HLF_W));
		    अन्यथा
			संदेश((वर्ण *)IObuff);
		    संदेश_scroll = TRUE;	// don't overwrite this message
		}
		अन्यथा
		    give_warning(IObuff, ic);
		यदि (ic && !संदेश_scrolled && संदेश_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#यदि defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (पूर्णाङ्क)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#यदि defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, रिक्त, -1);
#endif

	    यदि (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		यदि ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_अंक_matches != MAXCOL
					     || cur_match < अंक_matches - 1)))
		{
		    error_cur_match = cur_match;
		    यदि (use_tagstack)
			--tagstackidx;
		    यदि (type == DT_PREV)
			--cur_match;
		    अन्यथा
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    अनुवर्त;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    अन्यथा
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		यदि (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	भङ्ग;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    यदि (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    प्रत्यागम jumped_to_tag;
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    स्थिर शून्य
print_tag_list(
    पूर्णाङ्क		new_tag,
    पूर्णाङ्क		use_tagstack,
    पूर्णाङ्क		अंक_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णाङ्क		tagstackidx = curwin->w_tagstackidx;
    पूर्णाङ्क		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    पूर्णाङ्क		taglen;
    पूर्णाङ्क		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (पूर्णाङ्क)(tagp.tagname_end - tagp.tagname + 2);
    यदि (taglen < 18)
	taglen = 18;
    यदि (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    यदि (संदेश_col == 0)
	संदेश_didout = FALSE;	// overwrite previous message
    संदेश_start();
    संदेश_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    संदेश_clr_eos();
    taglen_advance(taglen);
    संदेश_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    प्रति (i = 0; i < अंक_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	यदि (!new_tag && (
#यदि defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	अन्यथा
	    *IObuff = ' ';
	vim_snprintf((वर्ण *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	संदेश_puts((वर्ण *)IObuff);
	यदि (tagp.tagkind != रिक्त)
	    संदेश_outtrans_len(tagp.tagkind,
			  (पूर्णाङ्क)(tagp.tagkind_end - tagp.tagkind));
	संदेश_advance(13);
	संदेश_outtrans_len_attr(tagp.tagname,
			   (पूर्णाङ्क)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	संदेश_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	यदि (p != रिक्त)
	{
	    संदेश_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	यदि (संदेश_col > 0)
	    संदेश_putchar('\n');
	यदि (got_int)
	    भङ्ग;
	संदेश_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	यदि (command_end != रिक्त)
	{
	    p = command_end + 3;
	    यावत् (*p && *p != '\r' && *p != '\n')
	    {
		यावत् (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		यदि (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    अनुवर्त;
		}
		// skip "kind:<kind>" and "<kind>"
		यदि (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    अनुवर्त;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		यावत् (*p && *p != '\r' && *p != '\n')
		{
		    यदि (संदेश_col + ptr2cells(p) >= cmdline_width)
		    {
			संदेश_putchar('\n');
			यदि (got_int)
			    भङ्ग;
			संदेश_advance(15);
		    }
		    p = संदेश_outtrans_one(p, attr);
		    यदि (*p == TAB)
		    {
			संदेश_puts_attr(" ", attr);
			भङ्ग;
		    }
		    यदि (*p == ':')
			attr = 0;
		}
	    }
	    यदि (संदेश_col > 15)
	    {
		संदेश_putchar('\n');
		यदि (got_int)
		    भङ्ग;
		संदेश_advance(15);
	    }
	}
	अन्यथा
	{
	    प्रति (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	यदि (*p == '/' || *p == '?')
	{
	    ++p;
	    यदि (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	यावत् (p != command_end && vim_isspace(*p))
	    ++p;

	यावत् (p != command_end)
	{
	    यदि (संदेश_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		संदेश_putchar('\n');
	    यदि (got_int)
		भङ्ग;
	    संदेश_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    यदि (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    यदि (*p == TAB)
	    {
		संदेश_putchar(' ');
		++p;
	    }
	    अन्यथा
		p = संदेश_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    यदि (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		भङ्ग;
	    // don't display matching '/' or '?'
	    यदि (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		भङ्ग;
	}
	यदि (संदेश_col)
	    संदेश_putchar('\n');
	ui_breakcheck();
    }
    यदि (got_int)
	got_int = FALSE;	// only stop the listing
}

#यदि defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    स्थिर पूर्णाङ्क
add_llist_tags(
    char_u	*tag,
    पूर्णाङ्क		अंक_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    पूर्णाङ्क		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    यदि (list == रिक्त || fname == रिक्त || cmd == रिक्त)
    {
	vim_free(cmd);
	vim_free(fname);
	यदि (list != रिक्त)
	    list_free(list);
	प्रत्यागम FAIL;
    }

    प्रति (i = 0; i < अंक_matches; ++i)
    {
	पूर्णाङ्क	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (पूर्णाङ्क)(tagp.tagname_end - tagp.tagname);
	यदि (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	यदि (p == रिक्त)
	    अनुवर्त;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	यदि (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((वर्ण *)tagp.command);
	अन्यथा
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    यदि (cmd_end == रिक्त)
	    {
		प्रति (p = tagp.command;
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
	    यदि (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    यदि (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    यदि (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (पूर्णाङ्क)(cmd_end - cmd_start + 1);
	    यदि (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    यदि (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	यदि ((dict = dict_alloc()) == रिक्त)
	    अनुवर्त;
	यदि (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    अनुवर्त;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_संख्या(dict, "lnum", lnum);
	यदि (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((वर्ण *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, रिक्त);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    प्रत्यागम OK;
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
taglen_advance(पूर्णाङ्क l)
{
    यदि (l == MAXCOL)
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
    पूर्णाङ्क		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    पूर्णाङ्क		tagstackidx = curwin->w_tagstackidx;
    पूर्णाङ्क		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    संदेश_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    प्रति (i = 0; i < tagstacklen; ++i)
    {
	यदि (tagstack[i].tagname != रिक्त)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    यदि (name == रिक्त)	    // file name not available
		अनुवर्त;

	    संदेश_putchar('\n');
	    vim_snprintf((वर्ण *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
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
    यदि (tagstackidx == tagstacklen)	// idx at top of stack
	संदेश_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    स्थिर पूर्णाङ्क
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    पूर्णाङ्क		i;

    यावत् (len > 0)
    {
	i = (पूर्णाङ्क)TOUPPER_ASC(*s1) - (पूर्णाङ्क)TOUPPER_ASC(*s2);
	यदि (i != 0)
	    प्रत्यागम i;			// this character different
	यदि (*s1 == NUL)
	    भङ्ग;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    प्रत्यागम 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
प्रकारनिर्देश संरचना
{
    char_u	*pat;		// the pattern
    पूर्णाङ्क		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    पूर्णाङ्क		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    स्थिर शून्य
prepare_pats(pat_T *pats, पूर्णाङ्क has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    यदि (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	यदि (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	अन्यथा यदि (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	यदि (pats->head == pats->pat)
	    pats->headlen = 0;
	अन्यथा
	    प्रति (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		यदि (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != रिक्त)
		    भङ्ग;
	यदि (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    यदि (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    अन्यथा
	pats->regmatch.regprog = रिक्त;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
प्रकारनिर्देश गणना {
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
    यदि (VIM_ISDIGIT(*cmd))
	प्रत्यागम *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    यदि (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	प्रत्यागम (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    यदि (vim_strpbrk(cmd, (char_u *)"\t\r\n") != रिक्त)
	प्रत्यागम TAGCMD_INVALID;
    प्रत्यागम TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    पूर्णाङ्क		*match_मोजणी,	// here the number of tags found will be placed
    पूर्णाङ्क		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    पूर्णाङ्क		ntags = 0;
    पूर्णाङ्क		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    यदि (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == रिक्त
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	प्रत्यागम FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    यदि ((d = dict_alloc_lock(VAR_FIXED)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (!(flags & TAG_INS_COMP) && tag->user_data != रिक्त)
	dict_add_string(d, "user_data", tag->user_data);
    यदि (buf_ffname != रिक्त)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((वर्ण *)flagString, परिमाण(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    यदि (result == FAIL)
	प्रत्यागम FAIL;
    यदि (rettv.v_type == VAR_SPECIAL && rettv.vval.v_संख्या == VVAL_NULL)
    {
	clear_tv(&rettv);
	प्रत्यागम NOTDONE;
    }
    यदि (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	प्रत्यागम FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_वस्तुसंच(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	पूर्णाङ्क		len;
	dict_iterator_T	iter;
	char_u		*dict_किल्ली;
	typval_T	*tv;
	पूर्णाङ्क		has_extra = 0;
	पूर्णाङ्क		name_only = flags & TAG_NAMES;

	यदि (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    भङ्ग;
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
	यावत् (रिक्त != (dict_किल्ली = dict_iterate_next(&iter, &tv)))
	{
	    यदि (tv->v_type != VAR_STRING || tv->vval.v_string == रिक्त)
		अनुवर्त;

	    len += (पूर्णाङ्क)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    यदि (STRCMP(dict_किल्ली, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		अनुवर्त;
	    }
	    यदि (STRCMP(dict_किल्ली, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		अनुवर्त;
	    }
	    यदि (STRCMP(dict_किल्ली, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		अनुवर्त;
	    }
	    has_extra = 1;
	    यदि (STRCMP(dict_किल्ली, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		अनुवर्त;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (पूर्णाङ्क)STRLEN(dict_किल्ली) + 1;
	}

	यदि (has_extra)
	    len += 2;	// need space for ;"

	यदि (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    भङ्ग;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	यदि (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_मूल्य_from_tagfunc));
	    भङ्ग;
	}
	यदि (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	यदि (name_only)
	    mfp = vim_strsave(res_name);
	अन्यथा
	    mfp = alloc(परिमाण(char_u) + len + 1);

	यदि (mfp == रिक्त)
	    अनुवर्त;

	यदि (!name_only)
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

	    यदि (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    यदि (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		यदि (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		यावत् (रिक्त != (dict_किल्ली = dict_iterate_next(&iter, &tv)))
		{
		    यदि (tv->v_type != VAR_STRING || tv->vval.v_string == रिक्त)
			अनुवर्त;

		    यदि (STRCMP(dict_किल्ली, "name") == 0)
			अनुवर्त;
		    यदि (STRCMP(dict_किल्ली, "filename") == 0)
			अनुवर्त;
		    यदि (STRCMP(dict_किल्ली, "cmd") == 0)
			अनुवर्त;
		    यदि (STRCMP(dict_किल्ली, "kind") == 0)
			अनुवर्त;

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
	यदि (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	अन्यथा
	{
	    vim_free(mfp);
	    भङ्ग;
	}
    }

    clear_tv(&rettv);

    *match_मोजणी = ntags;
    प्रत्यागम result;
}
#endif

/*
 * State information used during a tag search
 */
प्रकारनिर्देश संरचना
{
    tagsearch_state_T	state;		// tag search state
    पूर्णाङ्क		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    पूर्णाङ्क		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    पूर्णाङ्क		flags;			// flags used for tag search
    पूर्णाङ्क		tag_file_sorted;	// !_TAG_FILE_SORTED value
    पूर्णाङ्क		get_searchpat;		// used for 'showfulltag'
    पूर्णाङ्क		help_only;		// only search for help tags
    पूर्णाङ्क		did_open;		// did open a tag file
    पूर्णाङ्क		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    पूर्णाङ्क		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    पूर्णाङ्क		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    पूर्णाङ्क		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    पूर्णाङ्क		is_txt;			// flag of file extension
#endif
    पूर्णाङ्क		match_मोजणी;		// number of matches found
    garray_T	ga_match[MT_मोजणी];	// stores matches in sequence
    hashtab_T	ht_match[MT_मोजणी];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    स्थिर पूर्णाङ्क
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    पूर्णाङ्क			flags,
    पूर्णाङ्क			mincount)
{
    पूर्णाङ्क		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = रिक्त;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (पूर्णाङ्क)STRLEN(pat);
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

    प्रति (mtt = 0; mtt < MT_मोजणी; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], परिमाण(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    यदि (st->tag_fname == रिक्त
	    || st->lbuf == रिक्त
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == रिक्त
#endif
       )
	प्रत्यागम FAIL;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
findtags_in_help_init(findtags_state_T *st)
{
    पूर्णाङ्क		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    यदि (st->is_txt)
	STRCPY(st->help_lang, "en");
    अन्यथा
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (पूर्णाङ्क)STRLEN(st->tag_fname);
	यदि (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	अन्यथा
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    यदि (st->help_lang_find != रिक्त
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	प्रत्यागम FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    यदि ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == रिक्त
	    && curbuf->b_fname != रिक्त
	    && (i = (पूर्णाङ्क)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    अन्यथा
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	प्रति (s = p_hlg; *s != NUL; ++s)
	{
	    यदि (STRNICMP(s, st->help_lang, 2) == 0)
		भङ्ग;
	    ++st->help_pri;
	    यदि ((s = vim_strchr(s, ',')) == रिक्त)
		भङ्ग;
	}
	यदि (s == रिक्त || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    यदि (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    प्रत्यागम TRUE;
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
    स्थिर पूर्णाङ्क
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    पूर्णाङ्क		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    पूर्णाङ्क		retval;

    यदि (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	प्रत्यागम NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_मोजणी,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    प्रत्यागम retval;
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
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
स्थिर पूर्णाङ्क incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    स्थिर शून्य
emacs_tags_incstack_free(शून्य)
{
    यावत् (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
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

    यदि (vim_fgets(st->ebuf, LSIZE, st->fp))
	प्रत्यागम;

    प्रति (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    यदि (*p == NUL)
	प्रत्यागम;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    यदि (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	प्रत्यागम;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    यदि (incstack[incstack_idx].etag_fname == रिक्त)
	प्रत्यागम;

    incstack[incstack_idx].fp = st->fp;
    st->fp = रिक्त;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    यदि (fullpath_ebuf != रिक्त)
    {
	st->fp = mch_fopen((वर्ण *)fullpath_ebuf, "r");
	यदि (st->fp != रिक्त)
	{
	    यदि (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    यदि (st->fp == रिक्त)
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
    स्थिर पूर्णाङ्क
emacs_tags_file_eof(findtags_state_T *st)
{
    यदि (!incstack_idx)	// reached end of file. stop processing.
	प्रत्यागम FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    प्रत्यागम TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    स्थिर पूर्णाङ्क
emacs_tags_parse_ओळ(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    यदि (p_7f == रिक्त)
    {
etag_fail:
	यदि (vim_strchr(lbuf, '\n') != रिक्त)
	    प्रत्यागम FAIL;

	// Truncated line.  Ignore it.
	यदि (p_verbose >= 5)
	{
	    verbose_enter();
	    संदेश(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	प्रत्यागम OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    यदि (p == रिक्त)
	p = p_7f + 1;
    अन्यथा
	++p;

    यदि (!VIM_ISDIGIT(*p))	    // check for start of line number
	गच्छ etag_fail;
    tagp->command = p;

    यदि (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    अन्यथा			    // second format: isolate tagname
    {
	यदि (p_7f == lbuf)
	    गच्छ etag_fail;

	// find end of tagname
	प्रति (p = p_7f - 1; !vim_iswordc(*p); --p)
	    यदि (p == lbuf)
		गच्छ etag_fail;
	tagp->tagname_end = p + 1;
	यावत् (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    प्रत्यागम OK;
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
    पूर्णाङ्क		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    यदि (st->state == TS_द्विमान)
    {
	offset = sinfo_p->खालचा_offset + ((sinfo_p->वरचा_offset
						- sinfo_p->खालचा_offset) / 2);
	यदि (offset == sinfo_p->curr_offset)
	    प्रत्यागम TAGS_READ_EOF; // End the binary search without a match.
	अन्यथा
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    अन्यथा यदि (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	यदि (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    यदि (st->state == TS_द्विमान || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	यदि (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    यदि (sinfo_p->curr_offset == sinfo_p->वरचा_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->खालचा_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->खालचा_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	यावत् (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	यदि (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    प्रत्यागम TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    अन्यथा
    {
	// skip empty and blank lines
	कुरु
	{
#ifdef FEAT_CSCOPE
	    यदि (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    अन्यथा
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} यावत् (!eof && vim_isblankline(st->lbuf));

	यदि (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    यदि (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		प्रत्यागम TAGS_READ_IGNORE;
#endif
	    प्रत्यागम TAGS_READ_EOF;
	}
    }

    प्रत्यागम TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    स्थिर पूर्णाङ्क
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    यदि (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	प्रत्यागम TRUE;

    // Process the header line.
    यदि (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    यदि (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	प्रति (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    प्रत्यागम FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    स्थिर पूर्णाङ्क
findtags_start_state_handler(
    findtags_state_T	*st,
    पूर्णाङ्क			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    पूर्णाङ्क		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    पूर्णाङ्क		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    यदि (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	प्रत्यागम findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    यदि (st->linear || use_cscope)
#अन्यथा
    यदि (st->linear)
#endif
	st->state = TS_LINEAR;
    अन्यथा यदि (st->tag_file_sorted == NUL)
	st->state = TS_द्विमान;
    अन्यथा यदि (st->tag_file_sorted == '1')
	st->state = TS_द्विमान;
    अन्यथा यदि (st->tag_file_sorted == '2')
    {
	st->state = TS_द्विमान;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    अन्यथा
	st->state = TS_LINEAR;

    यदि (st->state == TS_द्विमान && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    यदि (st->state == TS_द्विमान)
    {
	यदि (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	अन्यथा
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->खालचा_offset = 0;
	    sinfo_p->खालचा_char = 0;
	    sinfo_p->वरचा_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->वरचा_char = 0xff;
	}
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
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
    पूर्णाङ्क		status;
    पूर्णाङ्क		i;
    पूर्णाङ्क		cmplen;
    पूर्णाङ्क		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    यदि (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	यदि (tagpp->tagname_end == रिक्त)
	    // Corrupted tag line.
	    प्रत्यागम TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (पूर्णाङ्क)(tagpp->tagname_end - tagpp->tagname);
	यदि (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	यदि ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	अन्यथा यदि (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    प्रत्यागम TAG_MATCH_NEXT;

	यदि (st->state == TS_द्विमान)
	{
	    // Simplistic check for unsorted tags file.
	    i = (पूर्णाङ्क)tagpp->tagname[0];
	    यदि (margs->sortic)
		i = (पूर्णाङ्क)TOUPPER_ASC(tagpp->tagname[0]);
	    यदि (i < sinfo_p->खालचा_char || i > sinfo_p->वरचा_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    यदि (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    अन्यथा
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    यदि (tagcmp == 0)
	    {
		यदि (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		अन्यथा यदि (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    यदि (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		प्रत्यागम TAG_MATCH_NEXT;
	    }
	    यदि (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		यदि (sinfo_p->curr_offset < sinfo_p->वरचा_offset)
		{
		    sinfo_p->खालचा_offset = sinfo_p->curr_offset;
		    यदि (margs->sortic)
			sinfo_p->खालचा_char = TOUPPER_ASC(tagpp->tagname[0]);
		    अन्यथा
			sinfo_p->खालचा_char = tagpp->tagname[0];
		    प्रत्यागम TAG_MATCH_NEXT;
		}
	    }
	    यदि (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->वरचा_offset)
	    {
		sinfo_p->वरचा_offset = sinfo_p->curr_offset;
		यदि (margs->sortic)
		    sinfo_p->वरचा_char = TOUPPER_ASC(tagpp->tagname[0]);
		अन्यथा
		    sinfo_p->वरचा_char = tagpp->tagname[0];
		प्रत्यागम TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    प्रत्यागम TAG_MATCH_STOP;
	}
	अन्यथा यदि (st->state == TS_SKIP_BACK)
	{
	    यदि (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    अन्यथा
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    प्रत्यागम TAG_MATCH_NEXT;
	}
	अन्यथा यदि (st->state == TS_STEP_FORWARD)
	{
	    यदि (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		यदि ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    प्रत्यागम TAG_MATCH_STOP;	// past last match
		अन्यथा
		    प्रत्यागम TAG_MATCH_NEXT;	// before first match
	    }
	}
	अन्यथा
	    // skip this match if it can't match
	    यदि (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		प्रत्यागम TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	यदि (tagpp->fname_end == रिक्त)
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

    यदि (status == FAIL)
	प्रत्यागम TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    यदि (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    प्रत्यागम TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    स्थिर शून्य
findtags_matchargs_init(findtags_match_args_T *margs, पूर्णाङ्क flags)
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
    स्थिर पूर्णाङ्क
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    पूर्णाङ्क		match = FALSE;
    पूर्णाङ्क		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (पूर्णाङ्क)(tagpp->tagname_end - tagpp->tagname);
    यदि (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    यदि (st->orgpat->len != cmplen)
	match = FALSE;
    अन्यथा
    {
	यदि (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    यदि (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	अन्यथा
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    यदि (!match && st->orgpat->regmatch.regprog != रिक्त)
    {
	पूर्णाङ्क	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	यदि (match)
	{
	    margs->matchoff = (पूर्णाङ्क)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    यदि (st->orgpat->regmatch.rm_ic)
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

    प्रत्यागम match;
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
    पूर्णाङ्क		len;

    conv_ओळ = string_convert(&st->vimconv, st->lbuf, रिक्त);
    यदि (conv_ओळ == रिक्त)
	प्रत्यागम;

    // Copy or swap lbuf and conv_line.
    len = (पूर्णाङ्क)STRLEN(conv_ओळ) + 1;
    यदि (len > st->lbuf_size)
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
    स्थिर पूर्णाङ्क
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    पूर्णाङ्क		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    पूर्णाङ्क		name_only = (st->flags & TAG_NAMES);
    पूर्णाङ्क		mtt;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		is_current;		// file name matches
    पूर्णाङ्क		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    यदि (use_cscope)
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
	यदि (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	यदि (is_static)
	{
	    यदि (is_current)
		mtt = MT_ST_CUR;
	    अन्यथा
		mtt = MT_ST_OTH;
	}
	अन्यथा
	{
	    यदि (is_current)
		mtt = MT_GL_CUR;
	    अन्यथा
		mtt = MT_GL_OTH;
	}
	यदि (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	यदि (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    यदि (st->help_only)
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
	len = (पूर्णाङ्क)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(परिमाण(char_u) + len + 10 + ML_EXTRA + 1);
	यदि (mfp != रिक्त)
	{
	    पूर्णाङ्क heuristic;

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
	    sprintf((वर्ण *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    अन्यथा यदि (name_only)
    {
	यदि (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    यदि (*temp_end == '/')
		यावत् (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    यदि (tagpp->command + 2 < temp_end)
	    {
		len = (पूर्णाङ्क)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		यदि (mfp != रिक्त)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    अन्यथा
		mfp = रिक्त;
	    st->get_searchpat = FALSE;
	}
	अन्यथा
	{
	    len = (पूर्णाङ्क)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(परिमाण(char_u) + len + 1);
	    यदि (mfp != रिक्त)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    यदि (State & MODE_घाला)
		st->get_searchpat = p_sft;
	}
    }
    अन्यथा
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
	len = (पूर्णाङ्क)tag_fname_len + (पूर्णाङ्क)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	यदि (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (पूर्णाङ्क)ebuf_len + 1;
	}
	अन्यथा
	    ++len;
#endif
	mfp = alloc(परिमाण(char_u) + len + 1);
	यदि (mfp != रिक्त)
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
	    यदि (st->is_etag)
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

    यदि (mfp != रिक्त)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	यदि (use_cscope)
	    ++*hash;
	अन्यथा
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	यदि (HASHITEM_EMPTY(hi))
	{
	    यदि (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		प्रत्यागम FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_मोजणी++;
	}
	अन्यथा
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    प्रत्यागम OK;
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
    पूर्णाङ्क			retval;
#ifdef FEAT_CSCOPE
    पूर्णाङ्क			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(शोध_info);

    // Read and parse the lines in the file one by one
    प्रति (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	यदि (st->state == TS_द्विमान || st->state == TS_SKIP_BACK)
	    ओळ_breakcheck();
	अन्यथा
	    fast_breakcheck();
	यदि ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_किल्ल्या(30, FALSE);
	यदि (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    भङ्ग;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	यदि (st->mincount == TAG_MANY && st->match_मोजणी >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    भङ्ग;
	}
	यदि (st->get_searchpat)
	    गच्छ ओळ_read_in;

	retval = findtags_get_next_ओळ(st, &शोध_info);
	यदि (retval == TAGS_READ_IGNORE)
	    अनुवर्त;
	यदि (retval == TAGS_READ_EOF)
	    भङ्ग;

ओळ_read_in:

	यदि (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	यदि (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    अनुवर्त;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	यदि (st->state == TS_START)
	{
	    यदि (findtags_start_state_handler(st, &margs->sortic, &शोध_info) == FALSE)
		अनुवर्त;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	यदि (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    यदि (st->lbuf == रिक्त)
	    {
		यदि (st->fp != रिक्त)
		    fclose(st->fp);
		st->fp = रिक्त;
		st->stop_searching = TRUE;
		प्रत्यागम;
	    }

	    यदि (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, शोध_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    शोध_info.curr_offset = 0;
	    अनुवर्त;
	}

	retval = findtags_parse_ओळ(st, &tagp, margs, &शोध_info);
	यदि (retval == TAG_MATCH_NEXT)
	    अनुवर्त;
	यदि (retval == TAG_MATCH_STOP)
	    भङ्ग;
	यदि (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    यदि (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    प्रत्यागम;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	यदि (findtags_match_tag(st, &tagp, margs))
	{
	    यदि (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		भङ्ग;
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
    पूर्णाङ्क		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = रिक्त;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    यदि (use_cscope)
	st->fp = रिक्त;	    // avoid GCC warning
    अन्यथा
#endif
    {
#ifdef FEAT_MULTI_LANG
	यदि (curbuf->b_help)
	{
	    यदि (!findtags_in_help_init(st))
		प्रत्यागम;
	}
#endif

	st->fp = mch_fopen((वर्ण *)st->tag_fname, "r");
	यदि (st->fp == रिक्त)
	    प्रत्यागम;

	यदि (p_verbose >= 5)
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

    यदि (st->fp != रिक्त)
    {
	fclose(st->fp);
	st->fp = रिक्त;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    यदि (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, रिक्त, रिक्त);

    यदि (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    यदि (st->match_मोजणी >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    स्थिर पूर्णाङ्क
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    पूर्णाङ्क		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    पूर्णाङ्क		mtt;
    पूर्णाङ्क		i;
    char_u	*mfp;
    char_u	*p;

    यदि (st->match_मोजणी > 0)
	matches = ALLOC_MULT(char_u *, st->match_मोजणी);
    अन्यथा
	matches = रिक्त;
    st->match_मोजणी = 0;
    प्रति (mtt = 0; mtt < MT_मोजणी; ++mtt)
    {
	प्रति (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    यदि (matches == रिक्त)
		vim_free(mfp);
	    अन्यथा
	    {
		यदि (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    प्रति (p = mfp + 1; *p != NUL; ++p)
			यदि (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_मोजणी++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    प्रत्यागम st->match_मोजणी;
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
    पूर्णाङ्क
find_tags(
    char_u	*pat,			// pattern to search for
    पूर्णाङ्क		*अंक_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    पूर्णाङ्क		flags,
    पूर्णाङ्क		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    पूर्णाङ्क		first_file;		// trying first tag file
    पूर्णाङ्क		retval = FAIL;		// return value
    पूर्णाङ्क		round;

    पूर्णाङ्क		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    पूर्णाङ्क		i;
    char_u	*saved_pat = रिक्त;		// copy of pat[]
#endif

    पूर्णाङ्क		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    पूर्णाङ्क		has_re = (flags & TAG_REGEXP);	// regexp used
    पूर्णाङ्क		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    पूर्णाङ्क		use_cscope = (flags & TAG_CSCOPE);
#endif
    पूर्णाङ्क		verbose = (flags & TAG_VERBOSE);
    पूर्णाङ्क		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    विकल्प (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	पक्ष TC_FOLLOWIC:		 भङ्ग;
	पक्ष TC_IGNORE:    p_ic = TRUE;  भङ्ग;
	पक्ष TC_MATCH:     p_ic = FALSE; भङ्ग;
	पक्ष TC_FOLLOWSCS: p_ic = ignorecase(pat); भङ्ग;
	पक्ष TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); भङ्ग;
    }

    help_save = curbuf->b_help;

    यदि (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	गच्छ findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    यदि (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    अन्यथा यदि (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    यदि (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	यदि (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    यदि (saved_pat != रिक्त)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    यदि (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    यदि (has_re && st.orgpat->regmatch.regprog == रिक्त)
	गच्छ findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    यदि (retval != NOTDONE)
	गच्छ findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    यदि ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == रिक्त
	    && curbuf->b_fname != रिक्त
	    && (i = (पूर्णाङ्क)STRLEN(curbuf->b_fname)) > 4
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
    प्रति (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      प्रति (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  यदि (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      भङ्ग;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	यदि (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	यदि (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    भङ्ग;
#ifdef FEAT_CSCOPE
	यदि (use_cscope)
	    भङ्ग;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    यदि (!st.stop_searching)
    {
	यदि (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    यदि (retval == FAIL)
	st.match_मोजणी = 0;

    *अंक_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    प्रत्यागम retval;
}

स्थिर garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    स्थिर शून्य
found_tagfile_cb(char_u *fname, शून्य *cookie UNUSED)
{
    यदि (ga_grow(&tag_fnames, 1) == FAIL)
	प्रत्यागम;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#यदि defined(EXITFREE)
    शून्य
free_tag_stuff(शून्य)
{
    ga_clear_strings(&tag_fnames);
    यदि (curwin != रिक्त)
	do_tag(रिक्त, DT_FREE, 0, 0, 0);
    tag_freematch();

# यदि defined(FEAT_QUICKFIX)
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
    पूर्णाङ्क
get_tagfname(
    tagname_T	*tnp,	// holds status info
    पूर्णाङ्क		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = रिक्त;
    char_u		*r_ptr;
    पूर्णाङ्क			i;

    यदि (first)
	CLEAR_POINTER(tnp);

    यदि (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	यदि (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, परिमाण(char_u *), 10);
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

	यदि (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    यदि (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		प्रत्यागम FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    प्रति (i = 0; i < tag_fnames.ga_len; ++i)
		यदि (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    प्रत्यागम FAIL; // avoid duplicate file names
	}
	अन्यथा
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	प्रत्यागम OK;
    }

    यदि (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	यदि (tnp->tn_tags == रिक्त)
	    प्रत्यागम FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    प्रति (;;)
    {
	यदि (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_शोध_ctx);
	    यदि (fname != रिक्त)
		भङ्ग;

	    tnp->tn_did_filefind_init = FALSE;
	}
	अन्यथा
	{
	    char_u  *filename = रिक्त;

	    // Stop when used all parts of 'tags'.
	    यदि (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_शोध_ctx);
		tnp->tn_शोध_ctx = रिक्त;
		प्रत्यागम FAIL;
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
	    यदि (r_ptr != रिक्त)
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
	    यदि (tnp->tn_शोध_ctx != रिक्त)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
parse_tag_ओळ(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    पूर्णाङ्क		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    यदि (is_etag)
	// emacs-style tag file
	प्रत्यागम emacs_tags_parse_ओळ(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    यदि (p == रिक्त)
	प्रत्यागम FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    यदि (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    यदि (p == रिक्त)
	प्रत्यागम FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    यदि (*p != NUL)
	++p;
    यदि (*p == NUL)
	प्रत्यागम FAIL;
    tagp->command = p;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    यावत् ((p = vim_strchr(p, '\t')) != रिक्त)
    {
	++p;
	यदि (STRNCMP(p, "file:", 5) == 0)
	    प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    स्थिर size_t
matching_ओळ_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    प्रत्यागम (p - lbuf) + STRLEN(p);
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
    स्थिर पूर्णाङ्क
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    पूर्णाङ्क		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    यदि (*lbuf)
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

    यदि (retval != OK)
	प्रत्यागम retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    यदि (find_extra(&p) == OK)
    {
	यदि (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	अन्यथा
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	यदि (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    यावत् (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		यदि (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		अन्यथा यदि (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		अन्यथा यदि (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((वर्ण *)p + 5);
		यदि (tagp->tagkind != रिक्त && tagp->user_data != रिक्त)
		    भङ्ग;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		यदि (pc == रिक्त || (pt != रिक्त && pc > pt))
		    tagp->tagkind = p;
		यदि (pt == रिक्त)
		    भङ्ग;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    यदि (tagp->tagkind != रिक्त)
    {
	प्रति (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    यदि (tagp->user_data != रिक्त)
    {
	प्रति (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    प्रत्यागम retval;
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
    पूर्णाङ्क		c;

#ifdef FEAT_EMACS_TAGS
    यदि (tagp->is_etag)
	c = 0;	    // to shut up GCC
    अन्यथा
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    यदि (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    प्रत्यागम fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    पूर्णाङ्क		forceit,	// :ta with !
    पूर्णाङ्क		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    पूर्णाङ्क		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = रिक्त;
    char_u	*fname;
    tagptrs_T	tagp;
    पूर्णाङ्क		retval = FAIL;
    पूर्णाङ्क		getfile_result = GETFILE_UNUSED;
    पूर्णाङ्क		शोध_options;
#ifdef FEAT_शोध_EXTRA
    पूर्णाङ्क		save_no_hlsearch;
#endif
#यदि defined(FEAT_QUICKFIX)
    win_T	*curwin_save = रिक्त;
#endif
    char_u	*full_fname = रिक्त;
#ifdef FEAT_FOLDING
    पूर्णाङ्क		old_किल्ली_Typed = किल्ली_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    पूर्णाङ्क		isdigit = FALSE;

    यदि (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	प्रत्यागम FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ओळ_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    यदि (lbuf != रिक्त)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    यदि (pbuf == रिक्त || lbuf == रिक्त || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = रिक्त;
	गच्छ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    यदि (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    प्रति (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	यदि (tagp.is_etag && *str == ',')// stop at ',' after line number
	    भङ्ग;
#endif
	*pbuf_end++ = *str++;
	यदि (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    भङ्ग;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    यदि (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	यदि (isdigit)
	    str++;
	यदि (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    यदि (str > pbuf && str[-1] == '|')
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
    यदि (fname == रिक्त)
	गच्छ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    यदि (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, रिक्त))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	यदि (nofile_fname == रिक्त)
	    nofile_fname = empty_option;
	गच्छ erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#यदि defined(FEAT_QUICKFIX)
    यदि (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	यदि (!curwin->w_p_pvw)
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
    यदि (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	यदि (existing_buf != रिक्त)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    यदि (swbuf_goto_win_with_buf(existing_buf) != रिक्त)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    यदि (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	यदि (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	यदि ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	यदि (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    यदि (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    गच्छ erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    यदि (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#यदि defined(FEAT_QUICKFIX)
	यदि (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	अन्यथा
#endif
	    keep_help_flag = curbuf->b_help;
    }

    यदि (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, रिक्त, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    यदि (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_शोध_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#यदि defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	यदि (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	यदि (vim_strchr(p_cpo, CPO_TAGPAT) != रिक्त)
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
	यदि (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	यदि (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    यदि (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    अन्यथा
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    यदि (do_शोध(रिक्त, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 शोध_options, रिक्त))
		retval = OK;
	    अन्यथा
	    {
		पूर्णाङ्क	found = 1;
		पूर्णाङ्क	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		यदि (!do_शोध(रिक्त, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 शोध_options, रिक्त))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (शून्य)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((वर्ण *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    यदि (!do_शोध(रिक्त, '/', '/', pbuf, pbuflen, (long)1,
							 शोध_options, रिक्त))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((वर्ण *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			यदि (!do_शोध(रिक्त, '/', '/', pbuf, pbuflen, (long)1,
							 शोध_options, रिक्त))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		यदि (found == 0)
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
		    यदि (found == 2 || !save_p_ic)
		    {
			संदेश(_(e_couldnt_find_tag_just_guessing));
			यदि (!संदेश_scrolled && संदेश_silent == 0)
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
	    पूर्णाङ्क		save_secure = secure;

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
	    यदि (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_शोध_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	यदि (शोध_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	यदि (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	यदि (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    यदि (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    यदि ((fdo_flags & FDO_TAG) && old_किल्ली_Typed)
		foldOpenCursor();
#endif
	}

#यदि defined(FEAT_QUICKFIX)
	यदि (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	यदि (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    अन्यथा
    {
	यदि (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	यदि (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#यदि defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	अन्यथा यदि (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    यदि (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#यदि defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    यदि (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#यदि defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    प्रत्यागम retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    स्थिर char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, पूर्णाङ्क expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = रिक्त;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    यदि (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       प्रत्यागम रिक्त;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    यदि (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == रिक्त)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, रिक्त,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	यदि (expanded_fname != रिक्त)
	    fname = expanded_fname;
    }

    यदि ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	यदि (retval != रिक्त)
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

    प्रत्यागम retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    स्थिर पूर्णाङ्क
test_for_current(
#ifdef FEAT_EMACS_TAGS
    पूर्णाङ्क	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    पूर्णाङ्क	    c;
    पूर्णाङ्क	    retval = FALSE;
    char_u  *fullname;

    यदि (buf_ffname != रिक्त)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	यदि (is_etag)
	    c = 0;	    // to shut up GCC
	अन्यथा
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	यदि (fullname != रिक्त)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	यदि (!is_etag)
#endif
	    *fname_end = c;
    }

    प्रत्यागम retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    प्रति (;;)
    {
	यदि (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	अन्यथा यदि (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    यदि (*str != first_char)
		str = रिक्त;
	    अन्यथा
		++str;
	}
	अन्यथा
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((वर्ण *)str, "|;\"");
	    यदि (str != रिक्त)
	    {
		++str;
		भङ्ग;
	    }

	}
	यदि (str == रिक्त || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    भङ्ग;
	++str;	// skip ';'
	first_char = *str;
    }

    यदि (str != रिक्त && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	प्रत्यागम OK;
    }
    प्रत्यागम FAIL;
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

    पूर्णाङ्क
expand_tags(
    पूर्णाङ्क		tagnames,	// expand tag names
    char_u	*pat,
    पूर्णाङ्क		*अंक_file,
    char_u	***file)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    पूर्णाङ्क		ret;

    name_buf = alloc(name_buf_size);
    यदि (name_buf == रिक्त)
	प्रत्यागम FAIL;

    यदि (tagnames)
	extra_flag = TAG_NAMES;
    अन्यथा
	extra_flag = 0;
    यदि (pat[0] == '/')
	ret = find_tags(pat + 1, अंक_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    अन्यथा
	ret = find_tags(pat, अंक_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    यदि (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 प्रति (i = 0; i < *अंक_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     यदि (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 यदि (buf == रिक्त)
		 {
		     vim_free(name_buf);
		     प्रत्यागम FAIL;
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
    प्रत्यागम ret;
}

#यदि defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
add_tag_field(
    dict_T  *dict,
    वर्ण    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		retval;

    // check that the field name doesn't exist yet
    यदि (dict_has_किल्ली(dict, field_name))
    {
	यदि (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	प्रत्यागम FAIL;
    }
    buf = alloc(MAXPATHL);
    यदि (buf == रिक्त)
	प्रत्यागम FAIL;
    यदि (start != रिक्त)
    {
	यदि (end == रिक्त)
	{
	    end = start + STRLEN(start);
	    यावत् (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (पूर्णाङ्क)(end - start);
	यदि (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    प्रत्यागम retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    पूर्णाङ्क
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    पूर्णाङ्क		अंक_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &अंक_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (पूर्णाङ्क)MAXCOL, buf_fname);
    यदि (ret != OK || अंक_matches <= 0)
	प्रत्यागम ret;

    प्रति (i = 0; i < अंक_matches; ++i)
    {
	यदि (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    अनुवर्त;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	यदि (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    अनुवर्त;
	}

	यदि ((dict = dict_alloc()) == रिक्त)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    भङ्ग;
	}
	यदि (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    अनुवर्त;
	}

	full_fname = tag_full_fname(&tp);
	यदि (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    रिक्त) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_संख्या(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	यदि (tp.command_end != रिक्त)
	{
	    प्रति (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		यदि (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		अन्यथा यदि (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		अन्यथा यदि (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    पूर्णाङ्क	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    यावत् (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (पूर्णाङ्क)(p - n);
		    यदि (*p == ':' && len > 0)
		    {
			s = ++p;
			यावत् (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			यदि (add_tag_field(dict, (वर्ण *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    अन्यथा
			// Skip field without colon.
			यावत् (*p != NUL && *p >= ' ')
			    ++p;
		    यदि (*p == NUL)
			भङ्ग;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    प्रत्यागम ret;
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
    यदि (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    यदि ((pos = list_alloc_id(aid_tagstack_from)) == रिक्त)
	प्रत्यागम;
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
    पूर्णाङ्क		i;
    dict_T	*d;

    dict_add_संख्या(retdict, "length", wp->w_tagstacklen);
    dict_add_संख्या(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_वस्तुसंच);
    यदि (l == रिक्त)
	प्रत्यागम;
    dict_add_list(retdict, "items", l);

    प्रति (i = 0; i < wp->w_tagstacklen; i++)
    {
	यदि ((d = dict_alloc_id(aid_tagstack_details)) == रिक्त)
	    प्रत्यागम;
	यदि (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    प्रत्यागम;
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
    पूर्णाङ्क i;

    // Free the current tag stack
    प्रति (i = 0; i < wp->w_tagstacklen; ++i)
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
    पूर्णाङ्क		i;

    tagstack_clear_entry(&tagstack[0]);
    प्रति (i = 1; i < wp->w_tagstacklen; ++i)
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
	पूर्णाङ्क	cur_fnum,
	पूर्णाङ्क	cur_match,
	pos_T	mark,
	पूर्णाङ्क	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    पूर्णाङ्क		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    यदि (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    यदि (tagstack[idx].cur_match < 0)
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
    पूर्णाङ्क		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	यदि (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == रिक्त)
	    अनुवर्त;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	यदि ((di = dict_find(itemdict, (char_u *)"from", -1)) == रिक्त)
	    अनुवर्त;
	यदि (list2fpos(&di->di_tv, &mark, &fnum, रिक्त, FALSE) != OK)
	    अनुवर्त;
	यदि ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == रिक्त)
	    अनुवर्त;

	यदि (mark.col > 0)
	    mark.col--;
	tagstack_ढकला_item(wp, tagname,
		(पूर्णाङ्क)dict_get_संख्या(itemdict, "bufnr"),
		(पूर्णाङ्क)dict_get_संख्या(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    स्थिर शून्य
tagstack_set_curidx(win_T *wp, पूर्णाङ्क curidx)
{
    wp->w_tagstackidx = curidx;
    यदि (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    यदि (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    पूर्णाङ्क
set_tagstack(win_T *wp, dict_T *d, पूर्णाङ्क action)
{
    dictitem_T	*di;
    list_T	*l = रिक्त;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    यदि (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_चळत_within_tagfunc));
	प्रत्यागम FAIL;
    }
# endif

    यदि ((di = dict_find(d, (char_u *)"items", -1)) != रिक्त)
    {
	यदि (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    प्रत्यागम FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    यदि ((di = dict_find(d, (char_u *)"curidx", -1)) != रिक्त)
	tagstack_set_curidx(wp, (पूर्णाङ्क)tv_get_संख्या(&di->di_tv) - 1);

    यदि (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	पूर्णाङ्क	tagstackidx = wp->w_tagstackidx;
	पूर्णाङ्क	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	यावत् (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    यदि (l != रिक्त)
    {
	यदि (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ढकला_वस्तुसंच(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    प्रत्यागम OK;
}
#endif
