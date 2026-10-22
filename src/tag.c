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

#அந்தர்பாவ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
வகைவரை கட்டமைப்பு tag_pointers
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
    முழு		is_etag;	// TRUE for emacs tag
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
வகைவரை எண்ணிக்கை
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
வகைவரை எண்ணிக்கை
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_இருமம்,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
வகைவரை கட்டமைப்பு
{
    off_T	கீழ்_offset;	// offset for first char of first line that
				// could match
    off_T	உயர்வு_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    முழு	கீழ்_char;		// first char at low_offset
    முழு	உயர்வு_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
வகைவரை எண்ணிக்கை
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
வகைவரை கட்டமைப்பு
{
    முழு	matchoff;		// tag match offset
    முழு	match_re;		// TRUE if the tag matches a regexp
    முழு	match_no_ic;		// TRUE if the tag matches with case
    முழு	has_re;			// regular expression used
    முழு	sortic;			// tags file sorted ignoring case (foldcase)
    முழு	sort_error;		// tags file not sorted
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
#define MT_மொத்தம்	16

நிலையான எழுத்து	*mt_names[MT_மொத்தம்/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
நிலையான char_u	*nofile_fname = வெற்று;	// fname for NOTAGFILE error

நிலையான வெறுமை taglen_advance(முழு l);

நிலையான முழு jumpto_tag(char_u *lbuf, முழு forceit, முழு keep_help);
#ifdef FEAT_EMACS_TAGS
நிலையான முழு parse_tag_வரி(char_u *lbuf, முழு is_etag, tagptrs_T *tagp);
#இல்லையெனில்
நிலையான முழு parse_tag_வரி(char_u *lbuf, tagptrs_T *tagp);
#endif
நிலையான முழு test_for_static(tagptrs_T *);
நிலையான முழு parse_match(char_u *lbuf, tagptrs_T *tagp);
நிலையான char_u *tag_full_fname(tagptrs_T *tagp);
நிலையான char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, முழு expand);
#ifdef FEAT_EMACS_TAGS
நிலையான முழு test_for_current(முழு, char_u *, char_u *, char_u *, char_u *);
#இல்லையெனில்
நிலையான முழு test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
நிலையான முழு find_extra(char_u **pp);
நிலையான வெறுமை print_tag_list(முழு new_tag, முழு use_tagstack, முழு இலக்கம்_matches, char_u **matches);
#எனில் defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
நிலையான முழு add_llist_tags(char_u *tag, முழு இலக்கம்_matches, char_u **matches);
#endif

நிலையான char_u	*tagmatchname = வெற்று;	// name of last used tag

#எனில் defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
நிலையான taggy_T ptag_entry = {வெற்று, {{0, 0, 0}, 0}, 0, 0, வெற்று};
#endif

#ifdef FEAT_EVAL
நிலையான முழு  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
நிலையான callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#எனில் defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    எழுத்து *
did_set_tagfunc(optset_T *args)
{
    முழு	retval;

    எனில் (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    இல்லையெனில்
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	எனில் (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    திரும்பு retval == FAIL ? e_invalid_argument : வெற்று;
}
#endif

#எனில் defined(EXITFREE)
    வெறுமை
free_tagfunc_option(வெறுமை)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    முழு
set_ref_in_tagfunc(முழு copyID UNUSED)
{
    முழு	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    திரும்பு abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    வெறுமை
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    எனில் (tfu_cb.cb_name != வெற்று && *tfu_cb.cb_name != NUL)
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
    முழு
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    முழு		type,
    முழு		மொத்தம்,
    முழு		forceit,	// :ta with !
    முழு		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    முழு		tagstackidx = curwin->w_tagstackidx;
    முழு		tagstacklen = curwin->w_tagstacklen;
    முழு		cur_match = 0;
    முழு		cur_fnum = curbuf->b_fnum;
    முழு		oldtagstackidx = tagstackidx;
    முழு		prevtagstackidx = tagstackidx;
    முழு		prev_இலக்கம்_matches;
    முழு		new_tag = FALSE;
    முழு		i;
    முழு		ic;
    முழு		no_regexp = FALSE;
    முழு		error_cur_match = 0;
    முழு		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    முழு		jumped_to_tag = FALSE;
#endif
    முழு		new_இலக்கம்_matches;
    char_u	**new_matches;
    முழு		use_tagstack;
    முழு		skip_செய்தி = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    முழு		use_tfu = 1;
    char_u	*tofree = வெற்று;

    // remember the matches for the last used tag
    நிலையான முழு		இலக்கம்_matches = 0;
    நிலையான முழு		max_இலக்கம்_matches = 0;  // limit used for match search
    நிலையான char_u	**matches = வெற்று;
    நிலையான முழு		flags;

#ifdef EXITFREE
    எனில் (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(இலக்கம்_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	இலக்கம்_matches = 0;
	திரும்பு FALSE;
    }
#endif

#ifdef FEAT_EVAL
    எனில் (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_அடுக்கு_within_tagfunc));
	திரும்பு FALSE;
    }
#endif

    எனில் (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	திரும்பு FALSE;

    எனில் (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_இலக்கம்_matches = இலக்கம்_matches;
    free_string_option(nofile_fname);
    nofile_fname = வெற்று;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    எனில் ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#எனில் defined(FEAT_QUICKFIX)
	எனில் (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    எனில் ((ptag_entry.tagname = vim_strsave(tag)) == வெற்று)
		செல் end_do_tag;
	}
#endif
    }
    இல்லையெனில்
    {
#எனில் defined(FEAT_QUICKFIX)
	எனில் (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	இல்லையெனில்
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	எனில் (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#எனில் defined(FEAT_QUICKFIX)
	    எனில் (g_do_tagpreview != 0)
	    {
		எனில் (ptag_entry.tagname != வெற்று
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		இல்லையெனில்
		{
		    tagstack_clear_entry(&ptag_entry);
		    எனில் ((ptag_entry.tagname = vim_strsave(tag)) == வெற்று)
			செல் end_do_tag;
		}
	    }
	    இல்லையெனில்
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		போது (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		எனில் (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    க்கு (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = வெற்று;
		}

		/*
		 * put the tag name in the tag stack
		 */
		எனில் ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == வெற்று)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    செல் end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	இல்லையெனில்
	{
	    எனில் (
#எனில் defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == வெற்று :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_அடுக்கு_empty));
		செல் end_do_tag;
	    }

	    எனில் (type == DT_எடு)		// go to older position
	    {
#ifdef FEAT_FOLDING
		முழு	old_சாவி_Typed = சாவி_Typed;
#endif
		எனில் ((tagstackidx -= மொத்தம்) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_அடுக்கு));
		    எனில் (tagstackidx + மொத்தம் == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			செல் end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		இல்லையெனில் எனில் (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_உச்சி_of_tag_அடுக்கு));
		    செல் end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		எனில் (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    எனில் (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			செல் end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		இல்லையெனில்
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		எனில் ((fdo_flags & FDO_TAG) && old_சாவி_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(இலக்கம்_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		இலக்கம்_matches = 0;
		tag_freematch();
		செல் end_do_tag;
	    }

	    எனில் (type == DT_TAG
#எனில் defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#எனில் defined(FEAT_QUICKFIX)
		எனில் (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		இல்லையெனில்
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    எனில் ((tagstackidx += மொத்தம் - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_உச்சி_of_tag_அடுக்கு));
			save_pos = FALSE;
		    }
		    இல்லையெனில் எனில் (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_அடுக்கு));
			tagstackidx = 0;
			செல் end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    இல்லையெனில்				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#எனில் defined(FEAT_QUICKFIX)
		எனில் (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		இல்லையெனில்
#endif
		{
		    எனில் (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		தேர்வு (type)
		{
		    நிலை DT_FIRST: cur_match = மொத்தம் - 1; நிறுத்து;
		    நிலை DT_SELECT:
		    நிலை DT_JUMP:
#ifdef FEAT_CSCOPE
		    நிலை DT_CSCOPE:
#endif
		    நிலை DT_LAST:  cur_match = MAXCOL - 1; நிறுத்து;
		    நிலை DT_NEXT:  cur_match += மொத்தம்; நிறுத்து;
		    நிலை DT_PREV:  cur_match -= மொத்தம்; நிறுத்து;
		}
		எனில் (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		இல்லையெனில் எனில் (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_செய்தி = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#எனில் defined(FEAT_QUICKFIX)
	எனில் (g_do_tagpreview != 0)
	{
	    எனில் (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	இல்லையெனில்
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    எனில் (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    எனில் (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    எனில் (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	எனில் (buf != வெற்று)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    க்கு (;;)
    {
	முழு	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	எனில் (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#எனில் defined(FEAT_QUICKFIX)
	இல்லையெனில் எனில் (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	இல்லையெனில்
	    name = tag;
	other_name = (tagmatchname == வெற்று || STRCMP(tagmatchname, name) != 0);
	எனில் (new_tag
		|| (cur_match >= இலக்கம்_matches && max_இலக்கம்_matches != MAXCOL)
		|| other_name)
	{
	    எனில் (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    எனில் (type == DT_SELECT || type == DT_JUMP
#எனில் defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    எனில் (type == DT_TAG)
		max_இலக்கம்_matches = MAXCOL;
	    இல்லையெனில்
		max_இலக்கம்_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    எனில் (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    இல்லையெனில்
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    எனில் (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    எனில் (verbose)
		flags |= TAG_VERBOSE;

	    எனில் (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    எனில் (find_tags(name, &new_இலக்கம்_matches, &new_matches, flags,
					    max_இலக்கம்_matches, buf_ffname) == OK
		    && new_இலக்கம்_matches < max_இலக்கம்_matches)
		max_இலக்கம்_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    எனில் (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_இலக்கம்_matches, new_matches);
		நிறுத்து;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    எனில் (!new_tag && !other_name)
	    {
		முழு	    j, k;
		முழு	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		க்கு (j = 0; j < இலக்கம்_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    க்கு (i = idx; i < new_இலக்கம்_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			எனில் (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    க்கு (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    நிறுத்து;
			}
		    }
		}
	    }
	    FreeWild(இலக்கம்_matches, matches);
	    இலக்கம்_matches = new_இலக்கம்_matches;
	    matches = new_matches;
	}

	எனில் (இலக்கம்_matches <= 0)
	{
	    எனில் (verbose)
		semsg(_(e_tag_not_found_str), name);
#எனில் defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	இல்லையெனில்
	{
	    முழு ask_for_selection = FALSE;
#எனில் defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    எனில் (type == DT_CSCOPE && இலக்கம்_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    இல்லையெனில்
#endif
	    எனில் (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = மொத்தம் > 0 ? மொத்தம் - 1 : 0;
	    இல்லையெனில் எனில் (type == DT_SELECT || (type == DT_JUMP && இலக்கம்_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, இலக்கம்_matches, matches);
		ask_for_selection = TRUE;
	    }
#எனில் defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    இல்லையெனில் எனில் (type == DT_LTAG)
	    {
		எனில் (add_llist_tags(tag, இலக்கம்_matches, matches) == FAIL)
		    செல் end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    எனில் (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_எண்(வெற்று);
		எனில் (i <= 0 || i > இலக்கம்_matches || got_int)
		{
		    // no valid choice: don't change anything
		    எனில் (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    நிறுத்து;
		}
		cur_match = i - 1;
	    }

	    எனில் (cur_match >= இலக்கம்_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		எனில் ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == வெற்று)
		{
		    எனில் (இலக்கம்_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    இல்லையெனில்
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_செய்தி = TRUE;
		}
		cur_match = இலக்கம்_matches - 1;
	    }
	    எனில் (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		எனில் (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#எனில் defined(FEAT_QUICKFIX)
	    இல்லையெனில் எனில் (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    எனில் (nofile_fname != வெற்று && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    எனில் (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (இலக்கம்_matches > 1 || ic)
		&& !skip_செய்தி)
	    {
		// Give an indication of the number of matching tags
		sprintf((எழுத்து *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				இலக்கம்_matches,
				max_இலக்கம்_matches != MAXCOL ? _(" or more") : "");
		எனில் (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		எனில் ((இலக்கம்_matches > prev_இலக்கம்_matches || new_tag)
							   && இலக்கம்_matches > 1)
		{
		    எனில் (ic)
			செய்தி_attr((எழுத்து *)IObuff, HL_ATTR(HLF_W));
		    இல்லையெனில்
			செய்தி((எழுத்து *)IObuff);
		    செய்தி_scroll = TRUE;	// don't overwrite this message
		}
		இல்லையெனில்
		    give_warning(IObuff, ic);
		எனில் (ic && !செய்தி_scrolled && செய்தி_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#எனில் defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (முழு)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#எனில் defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, வெற்று, -1);
#endif

	    எனில் (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		எனில் ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_இலக்கம்_matches != MAXCOL
					     || cur_match < இலக்கம்_matches - 1)))
		{
		    error_cur_match = cur_match;
		    எனில் (use_tagstack)
			--tagstackidx;
		    எனில் (type == DT_PREV)
			--cur_match;
		    இல்லையெனில்
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    தொடர்;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    இல்லையெனில்
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		எனில் (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	நிறுத்து;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    எனில் (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    திரும்பு jumped_to_tag;
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    நிலையான வெறுமை
print_tag_list(
    முழு		new_tag,
    முழு		use_tagstack,
    முழு		இலக்கம்_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    முழு		tagstackidx = curwin->w_tagstackidx;
    முழு		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    முழு		taglen;
    முழு		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (முழு)(tagp.tagname_end - tagp.tagname + 2);
    எனில் (taglen < 18)
	taglen = 18;
    எனில் (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    எனில் (செய்தி_col == 0)
	செய்தி_didout = FALSE;	// overwrite previous message
    செய்தி_start();
    செய்தி_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    செய்தி_clr_eos();
    taglen_advance(taglen);
    செய்தி_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    க்கு (i = 0; i < இலக்கம்_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	எனில் (!new_tag && (
#எனில் defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	இல்லையெனில்
	    *IObuff = ' ';
	vim_snprintf((எழுத்து *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	செய்தி_puts((எழுத்து *)IObuff);
	எனில் (tagp.tagkind != வெற்று)
	    செய்தி_outtrans_len(tagp.tagkind,
			  (முழு)(tagp.tagkind_end - tagp.tagkind));
	செய்தி_advance(13);
	செய்தி_outtrans_len_attr(tagp.tagname,
			   (முழு)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	செய்தி_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	எனில் (p != வெற்று)
	{
	    செய்தி_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	எனில் (செய்தி_col > 0)
	    செய்தி_putchar('\n');
	எனில் (got_int)
	    நிறுத்து;
	செய்தி_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	எனில் (command_end != வெற்று)
	{
	    p = command_end + 3;
	    போது (*p && *p != '\r' && *p != '\n')
	    {
		போது (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		எனில் (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    தொடர்;
		}
		// skip "kind:<kind>" and "<kind>"
		எனில் (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    தொடர்;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		போது (*p && *p != '\r' && *p != '\n')
		{
		    எனில் (செய்தி_col + ptr2cells(p) >= cmdline_width)
		    {
			செய்தி_putchar('\n');
			எனில் (got_int)
			    நிறுத்து;
			செய்தி_advance(15);
		    }
		    p = செய்தி_outtrans_one(p, attr);
		    எனில் (*p == TAB)
		    {
			செய்தி_puts_attr(" ", attr);
			நிறுத்து;
		    }
		    எனில் (*p == ':')
			attr = 0;
		}
	    }
	    எனில் (செய்தி_col > 15)
	    {
		செய்தி_putchar('\n');
		எனில் (got_int)
		    நிறுத்து;
		செய்தி_advance(15);
	    }
	}
	இல்லையெனில்
	{
	    க்கு (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	எனில் (*p == '/' || *p == '?')
	{
	    ++p;
	    எனில் (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	போது (p != command_end && vim_isspace(*p))
	    ++p;

	போது (p != command_end)
	{
	    எனில் (செய்தி_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		செய்தி_putchar('\n');
	    எனில் (got_int)
		நிறுத்து;
	    செய்தி_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    எனில் (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    எனில் (*p == TAB)
	    {
		செய்தி_putchar(' ');
		++p;
	    }
	    இல்லையெனில்
		p = செய்தி_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    எனில் (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		நிறுத்து;
	    // don't display matching '/' or '?'
	    எனில் (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		நிறுத்து;
	}
	எனில் (செய்தி_col)
	    செய்தி_putchar('\n');
	ui_breakcheck();
    }
    எனில் (got_int)
	got_int = FALSE;	// only stop the listing
}

#எனில் defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    நிலையான முழு
add_llist_tags(
    char_u	*tag,
    முழு		இலக்கம்_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    முழு		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    எனில் (list == வெற்று || fname == வெற்று || cmd == வெற்று)
    {
	vim_free(cmd);
	vim_free(fname);
	எனில் (list != வெற்று)
	    list_free(list);
	திரும்பு FAIL;
    }

    க்கு (i = 0; i < இலக்கம்_matches; ++i)
    {
	முழு	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (முழு)(tagp.tagname_end - tagp.tagname);
	எனில் (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	எனில் (p == வெற்று)
	    தொடர்;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	எனில் (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((எழுத்து *)tagp.command);
	இல்லையெனில்
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    எனில் (cmd_end == வெற்று)
	    {
		க்கு (p = tagp.command;
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
	    எனில் (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    எனில் (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    எனில் (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (முழு)(cmd_end - cmd_start + 1);
	    எனில் (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    எனில் (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	எனில் ((dict = dict_alloc()) == வெற்று)
	    தொடர்;
	எனில் (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    தொடர்;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_எண்(dict, "lnum", lnum);
	எனில் (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, வெற்று);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    திரும்பு OK;
}
#endif

/*
 * Free cached tags.
 */
    வெறுமை
tag_freematch(வெறுமை)
{
    VIM_CLEAR(tagmatchname);
}

    நிலையான வெறுமை
taglen_advance(முழு l)
{
    எனில் (l == MAXCOL)
    {
	செய்தி_putchar('\n');
	செய்தி_advance(24);
    }
    இல்லையெனில்
	செய்தி_advance(13 + l);
}

/*
 * Print the tag stack
 */
    வெறுமை
do_tags(exarg_T *eap UNUSED)
{
    முழு		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    முழு		tagstackidx = curwin->w_tagstackidx;
    முழு		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    செய்தி_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    க்கு (i = 0; i < tagstacklen; ++i)
    {
	எனில் (tagstack[i].tagname != வெற்று)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    எனில் (name == வெற்று)	    // file name not available
		தொடர்;

	    செய்தி_putchar('\n');
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    செய்தி_outtrans(IObuff);
	    செய்தி_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    எனில் (tagstackidx == tagstacklen)	// idx at top of stack
	செய்தி_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    நிலையான முழு
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    முழு		i;

    போது (len > 0)
    {
	i = (முழு)TOUPPER_ASC(*s1) - (முழு)TOUPPER_ASC(*s2);
	எனில் (i != 0)
	    திரும்பு i;			// this character different
	எனில் (*s1 == NUL)
	    நிறுத்து;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    திரும்பு 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
வகைவரை கட்டமைப்பு
{
    char_u	*pat;		// the pattern
    முழு		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    முழு		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    நிலையான வெறுமை
prepare_pats(pat_T *pats, முழு has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    எனில் (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	எனில் (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	இல்லையெனில் எனில் (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	எனில் (pats->head == pats->pat)
	    pats->headlen = 0;
	இல்லையெனில்
	    க்கு (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		எனில் (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != வெற்று)
		    நிறுத்து;
	எனில் (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    எனில் (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    இல்லையெனில்
	pats->regmatch.regprog = வெற்று;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
வகைவரை எண்ணிக்கை {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    நிலையான tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    எனில் (VIM_ISDIGIT(*cmd))
	திரும்பு *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    எனில் (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	திரும்பு (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    எனில் (vim_strpbrk(cmd, (char_u *)"\t\r\n") != வெற்று)
	திரும்பு TAGCMD_INVALID;
    திரும்பு TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    நிலையான முழு
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    முழு		*match_மொத்தம்,	// here the number of tags found will be placed
    முழு		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    முழு		ntags = 0;
    முழு		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    எனில் (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == வெற்று
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	திரும்பு FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    எனில் ((d = dict_alloc_lock(VAR_FIXED)) == வெற்று)
	திரும்பு FAIL;
    எனில் (!(flags & TAG_INS_COMP) && tag->user_data != வெற்று)
	dict_add_string(d, "user_data", tag->user_data);
    எனில் (buf_ffname != வெற்று)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((எழுத்து *)flagString, அளவு(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    எனில் (result == FAIL)
	திரும்பு FAIL;
    எனில் (rettv.v_type == VAR_SPECIAL && rettv.vval.v_எண் == VVAL_NULL)
    {
	clear_tv(&rettv);
	திரும்பு NOTDONE;
    }
    எனில் (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_மதிப்பு_from_tagfunc));
	திரும்பு FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_பொருட்கள்(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	முழு		len;
	dict_iterator_T	iter;
	char_u		*dict_சாவி;
	typval_T	*tv;
	முழு		has_extra = 0;
	முழு		name_only = flags & TAG_NAMES;

	எனில் (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_மதிப்பு_from_tagfunc));
	    நிறுத்து;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# இல்லையெனில்
	len = 2;
# endif
	res_name = வெற்று;
	res_fname = வெற்று;
	res_cmd = வெற்று;
	res_kind = வெற்று;

	dict_iterate_start(&item->li_tv, &iter);
	போது (வெற்று != (dict_சாவி = dict_iterate_next(&iter, &tv)))
	{
	    எனில் (tv->v_type != VAR_STRING || tv->vval.v_string == வெற்று)
		தொடர்;

	    len += (முழு)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    எனில் (STRCMP(dict_சாவி, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		தொடர்;
	    }
	    எனில் (STRCMP(dict_சாவி, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		தொடர்;
	    }
	    எனில் (STRCMP(dict_சாவி, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		தொடர்;
	    }
	    has_extra = 1;
	    எனில் (STRCMP(dict_சாவி, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		தொடர்;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (முழு)STRLEN(dict_சாவி) + 1;
	}

	எனில் (has_extra)
	    len += 2;	// need space for ;"

	எனில் (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_மதிப்பு_from_tagfunc));
	    நிறுத்து;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	எனில் (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_மதிப்பு_from_tagfunc));
	    நிறுத்து;
	}
	எனில் (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	எனில் (name_only)
	    mfp = vim_strsave(res_name);
	இல்லையெனில்
	    mfp = alloc(அளவு(char_u) + len + 1);

	எனில் (mfp == வெற்று)
	    தொடர்;

	எனில் (!name_only)
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

	    எனில் (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    எனில் (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		எனில் (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		போது (வெற்று != (dict_சாவி = dict_iterate_next(&iter, &tv)))
		{
		    எனில் (tv->v_type != VAR_STRING || tv->vval.v_string == வெற்று)
			தொடர்;

		    எனில் (STRCMP(dict_சாவி, "name") == 0)
			தொடர்;
		    எனில் (STRCMP(dict_சாவி, "filename") == 0)
			தொடர்;
		    எனில் (STRCMP(dict_சாவி, "cmd") == 0)
			தொடர்;
		    எனில் (STRCMP(dict_சாவி, "kind") == 0)
			தொடர்;

		    *p++ = TAB;
		    STRCPY(p, dict_சாவி);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	எனில் (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	இல்லையெனில்
	{
	    vim_free(mfp);
	    நிறுத்து;
	}
    }

    clear_tv(&rettv);

    *match_மொத்தம் = ntags;
    திரும்பு result;
}
#endif

/*
 * State information used during a tag search
 */
வகைவரை கட்டமைப்பு
{
    tagsearch_state_T	state;		// tag search state
    முழு		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    முழு		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    முழு		flags;			// flags used for tag search
    முழு		tag_file_sorted;	// !_TAG_FILE_SORTED value
    முழு		get_searchpat;		// used for 'showfulltag'
    முழு		help_only;		// only search for help tags
    முழு		did_open;		// did open a tag file
    முழு		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    முழு		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    முழு		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    முழு		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    முழு		is_txt;			// flag of file extension
#endif
    முழு		match_மொத்தம்;		// number of matches found
    garray_T	ga_match[MT_மொத்தம்];	// stores matches in sequence
    hashtab_T	ht_match[MT_மொத்தம்];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    நிலையான முழு
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    முழு			flags,
    முழு			mincount)
{
    முழு		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = வெற்று;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (முழு)STRLEN(pat);
    st->orgpat->regmatch.regprog = வெற்று;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = வெற்று;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_மொத்தம் = 0;
    st->stop_searching = FALSE;

    க்கு (mtt = 0; mtt < MT_மொத்தம்; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], அளவு(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    எனில் (st->tag_fname == வெற்று
	    || st->lbuf == வெற்று
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == வெற்று
#endif
       )
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Free the state used by find_tags()
 */
    நிலையான வெறுமை
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
    நிலையான முழு
findtags_in_help_init(findtags_state_T *st)
{
    முழு		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    எனில் (st->is_txt)
	STRCPY(st->help_lang, "en");
    இல்லையெனில்
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (முழு)STRLEN(st->tag_fname);
	எனில் (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	இல்லையெனில்
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    எனில் (st->help_lang_find != வெற்று
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	திரும்பு FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    எனில் ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == வெற்று
	    && curbuf->b_fname != வெற்று
	    && (i = (முழு)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    இல்லையெனில்
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	க்கு (s = p_hlg; *s != NUL; ++s)
	{
	    எனில் (STRNICMP(s, st->help_lang, 2) == 0)
		நிறுத்து;
	    ++st->help_pri;
	    எனில் ((s = vim_strchr(s, ',')) == வெற்று)
		நிறுத்து;
	}
	எனில் (s == வெற்று || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    எனில் (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    திரும்பு TRUE;
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
    நிலையான முழு
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    முழு		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    முழு		retval;

    எனில் (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	திரும்பு NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_மொத்தம்,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    திரும்பு retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
நிலையான கட்டமைப்பு
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
நிலையான முழு incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    நிலையான வெறுமை
emacs_tags_incstack_free(வெறுமை)
{
    போது (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = வெற்று;
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
    நிலையான வெறுமை
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    எனில் (vim_fgets(st->ebuf, LSIZE, st->fp))
	திரும்பு;

    க்கு (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    எனில் (*p == NUL)
	திரும்பு;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    எனில் (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	திரும்பு;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    எனில் (incstack[incstack_idx].etag_fname == வெற்று)
	திரும்பு;

    incstack[incstack_idx].fp = st->fp;
    st->fp = வெற்று;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    எனில் (fullpath_ebuf != வெற்று)
    {
	st->fp = mch_fopen((எழுத்து *)fullpath_ebuf, "r");
	எனில் (st->fp != வெற்று)
	{
	    எனில் (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    எனில் (st->fp == வெற்று)
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
    நிலையான முழு
emacs_tags_file_eof(findtags_state_T *st)
{
    எனில் (!incstack_idx)	// reached end of file. stop processing.
	திரும்பு FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    திரும்பு TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    நிலையான முழு
emacs_tags_parse_வரி(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    எனில் (p_7f == வெற்று)
    {
etag_fail:
	எனில் (vim_strchr(lbuf, '\n') != வெற்று)
	    திரும்பு FAIL;

	// Truncated line.  Ignore it.
	எனில் (p_verbose >= 5)
	{
	    verbose_enter();
	    செய்தி(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	திரும்பு OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    எனில் (p == வெற்று)
	p = p_7f + 1;
    இல்லையெனில்
	++p;

    எனில் (!VIM_ISDIGIT(*p))	    // check for start of line number
	செல் etag_fail;
    tagp->command = p;

    எனில் (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    இல்லையெனில்			    // second format: isolate tagname
    {
	எனில் (p_7f == lbuf)
	    செல் etag_fail;

	// find end of tagname
	க்கு (p = p_7f - 1; !vim_iswordc(*p); --p)
	    எனில் (p == lbuf)
		செல் etag_fail;
	tagp->tagname_end = p + 1;
	போது (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    திரும்பு OK;
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
    நிலையான tags_read_status_T
findtags_get_next_வரி(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    முழு		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    எனில் (st->state == TS_இருமம்)
    {
	offset = sinfo_p->கீழ்_offset + ((sinfo_p->உயர்வு_offset
						- sinfo_p->கீழ்_offset) / 2);
	எனில் (offset == sinfo_p->curr_offset)
	    திரும்பு TAGS_READ_EOF; // End the binary search without a match.
	இல்லையெனில்
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    இல்லையெனில் எனில் (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	எனில் (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    எனில் (st->state == TS_இருமம் || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	எனில் (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    எனில் (sinfo_p->curr_offset == sinfo_p->உயர்வு_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->கீழ்_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->கீழ்_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	போது (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	எனில் (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    திரும்பு TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    இல்லையெனில்
    {
	// skip empty and blank lines
	செய்
	{
#ifdef FEAT_CSCOPE
	    எனில் (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    இல்லையெனில்
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} போது (!eof && vim_isblankline(st->lbuf));

	எனில் (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    எனில் (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		திரும்பு TAGS_READ_IGNORE;
#endif
	    திரும்பு TAGS_READ_EOF;
	}
    }

    திரும்பு TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    நிலையான முழு
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    எனில் (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	திரும்பு TRUE;

    // Process the header line.
    எனில் (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    எனில் (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	க்கு (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    திரும்பு FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    நிலையான முழு
findtags_start_state_handler(
    findtags_state_T	*st,
    முழு			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    முழு		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    முழு		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    எனில் (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	திரும்பு findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    எனில் (st->linear || use_cscope)
#இல்லையெனில்
    எனில் (st->linear)
#endif
	st->state = TS_LINEAR;
    இல்லையெனில் எனில் (st->tag_file_sorted == NUL)
	st->state = TS_இருமம்;
    இல்லையெனில் எனில் (st->tag_file_sorted == '1')
	st->state = TS_இருமம்;
    இல்லையெனில் எனில் (st->tag_file_sorted == '2')
    {
	st->state = TS_இருமம்;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    இல்லையெனில்
	st->state = TS_LINEAR;

    எனில் (st->state == TS_இருமம் && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    எனில் (st->state == TS_இருமம்)
    {
	எனில் (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	இல்லையெனில்
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->கீழ்_offset = 0;
	    sinfo_p->கீழ்_char = 0;
	    sinfo_p->உயர்வு_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->உயர்வு_char = 0xff;
	}
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
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
    நிலையான tagmatch_status_T
findtags_parse_வரி(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    முழு		status;
    முழு		i;
    முழு		cmplen;
    முழு		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    எனில் (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	எனில் (tagpp->tagname_end == வெற்று)
	    // Corrupted tag line.
	    திரும்பு TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (முழு)(tagpp->tagname_end - tagpp->tagname);
	எனில் (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	எனில் ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	இல்லையெனில் எனில் (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    திரும்பு TAG_MATCH_NEXT;

	எனில் (st->state == TS_இருமம்)
	{
	    // Simplistic check for unsorted tags file.
	    i = (முழு)tagpp->tagname[0];
	    எனில் (margs->sortic)
		i = (முழு)TOUPPER_ASC(tagpp->tagname[0]);
	    எனில் (i < sinfo_p->கீழ்_char || i > sinfo_p->உயர்வு_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    எனில் (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    இல்லையெனில்
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    எனில் (tagcmp == 0)
	    {
		எனில் (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		இல்லையெனில் எனில் (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    எனில் (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		திரும்பு TAG_MATCH_NEXT;
	    }
	    எனில் (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		எனில் (sinfo_p->curr_offset < sinfo_p->உயர்வு_offset)
		{
		    sinfo_p->கீழ்_offset = sinfo_p->curr_offset;
		    எனில் (margs->sortic)
			sinfo_p->கீழ்_char = TOUPPER_ASC(tagpp->tagname[0]);
		    இல்லையெனில்
			sinfo_p->கீழ்_char = tagpp->tagname[0];
		    திரும்பு TAG_MATCH_NEXT;
		}
	    }
	    எனில் (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->உயர்வு_offset)
	    {
		sinfo_p->உயர்வு_offset = sinfo_p->curr_offset;
		எனில் (margs->sortic)
		    sinfo_p->உயர்வு_char = TOUPPER_ASC(tagpp->tagname[0]);
		இல்லையெனில்
		    sinfo_p->உயர்வு_char = tagpp->tagname[0];
		திரும்பு TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    திரும்பு TAG_MATCH_STOP;
	}
	இல்லையெனில் எனில் (st->state == TS_SKIP_BACK)
	{
	    எனில் (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    இல்லையெனில்
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    திரும்பு TAG_MATCH_NEXT;
	}
	இல்லையெனில் எனில் (st->state == TS_STEP_FORWARD)
	{
	    எனில் (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		எனில் ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    திரும்பு TAG_MATCH_STOP;	// past last match
		இல்லையெனில்
		    திரும்பு TAG_MATCH_NEXT;	// before first match
	    }
	}
	இல்லையெனில்
	    // skip this match if it can't match
	    எனில் (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		திரும்பு TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	எனில் (tagpp->fname_end == வெற்று)
	    status = FAIL;
	இல்லையெனில்
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    இல்லையெனில்
	status = parse_tag_வரி(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    எனில் (status == FAIL)
	திரும்பு TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    எனில் (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    திரும்பு TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    நிலையான வெறுமை
findtags_matchargs_init(findtags_match_args_T *margs, முழு flags)
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
    நிலையான முழு
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    முழு		match = FALSE;
    முழு		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (முழு)(tagpp->tagname_end - tagpp->tagname);
    எனில் (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    எனில் (st->orgpat->len != cmplen)
	match = FALSE;
    இல்லையெனில்
    {
	எனில் (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    எனில் (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	இல்லையெனில்
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    எனில் (!match && st->orgpat->regmatch.regprog != வெற்று)
    {
	முழு	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	எனில் (match)
	{
	    margs->matchoff = (முழு)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    எனில் (st->orgpat->regmatch.rm_ic)
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

    திரும்பு match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    நிலையான வெறுமை
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_வரி;
    முழு		len;

    conv_வரி = string_convert(&st->vimconv, st->lbuf, வெற்று);
    எனில் (conv_வரி == வெற்று)
	திரும்பு;

    // Copy or swap lbuf and conv_line.
    len = (முழு)STRLEN(conv_வரி) + 1;
    எனில் (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_வரி;
	st->lbuf_size = len;
    }
    இல்லையெனில்
    {
	STRCPY(st->lbuf, conv_வரி);
	vim_free(conv_வரி);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    நிலையான முழு
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    முழு		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    முழு		name_only = (st->flags & TAG_NAMES);
    முழு		mtt;
    முழு		len = 0;
    முழு		is_current;		// file name matches
    முழு		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    எனில் (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    இல்லையெனில்
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
	எனில் (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	எனில் (is_static)
	{
	    எனில் (is_current)
		mtt = MT_ST_CUR;
	    இல்லையெனில்
		mtt = MT_ST_OTH;
	}
	இல்லையெனில்
	{
	    எனில் (is_current)
		mtt = MT_GL_CUR;
	    இல்லையெனில்
		mtt = MT_GL_OTH;
	}
	எனில் (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	எனில் (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    எனில் (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#இல்லையெனில்
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (முழு)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(அளவு(char_u) + len + 10 + ML_EXTRA + 1);
	எனில் (mfp != வெற்று)
	{
	    முழு heuristic;

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
	    sprintf((எழுத்து *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    இல்லையெனில் எனில் (name_only)
    {
	எனில் (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    எனில் (*temp_end == '/')
		போது (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    எனில் (tagpp->command + 2 < temp_end)
	    {
		len = (முழு)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		எனில் (mfp != வெற்று)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    இல்லையெனில்
		mfp = வெற்று;
	    st->get_searchpat = FALSE;
	}
	இல்லையெனில்
	{
	    len = (முழு)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(அளவு(char_u) + len + 1);
	    எனில் (mfp != வெற்று)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    எனில் (State & MODE_செருகு)
		st->get_searchpat = p_sft;
	}
    }
    இல்லையெனில்
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
	len = (முழு)tag_fname_len + (முழு)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	எனில் (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (முழு)ebuf_len + 1;
	}
	இல்லையெனில்
	    ++len;
#endif
	mfp = alloc(அளவு(char_u) + len + 1);
	எனில் (mfp != வெற்று)
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
	    எனில் (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    இல்லையெனில்
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    எனில் (mfp != வெற்று)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	எனில் (use_cscope)
	    ++*hash;
	இல்லையெனில்
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	எனில் (HASHITEM_EMPTY(hi))
	{
	    எனில் (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		திரும்பு FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_மொத்தம்++;
	}
	இல்லையெனில்
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    திரும்பு OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    நிலையான வெறுமை
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	தேடல்_info;
    முழு			retval;
#ifdef FEAT_CSCOPE
    முழு			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(தேடல்_info);

    // Read and parse the lines in the file one by one
    க்கு (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	எனில் (st->state == TS_இருமம் || st->state == TS_SKIP_BACK)
	    வரி_breakcheck();
	இல்லையெனில்
	    fast_breakcheck();
	எனில் ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_சாவிகள்(30, FALSE);
	எனில் (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    நிறுத்து;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	எனில் (st->mincount == TAG_MANY && st->match_மொத்தம் >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    நிறுத்து;
	}
	எனில் (st->get_searchpat)
	    செல் வரி_read_in;

	retval = findtags_get_next_வரி(st, &தேடல்_info);
	எனில் (retval == TAGS_READ_IGNORE)
	    தொடர்;
	எனில் (retval == TAGS_READ_EOF)
	    நிறுத்து;

வரி_read_in:

	எனில் (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	எனில் (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    தொடர்;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	எனில் (st->state == TS_START)
	{
	    எனில் (findtags_start_state_handler(st, &margs->sortic, &தேடல்_info) == FALSE)
		தொடர்;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	எனில் (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    எனில் (st->lbuf == வெற்று)
	    {
		எனில் (st->fp != வெற்று)
		    fclose(st->fp);
		st->fp = வெற்று;
		st->stop_searching = TRUE;
		திரும்பு;
	    }

	    எனில் (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, தேடல்_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    தேடல்_info.curr_offset = 0;
	    தொடர்;
	}

	retval = findtags_parse_வரி(st, &tagp, margs, &தேடல்_info);
	எனில் (retval == TAG_MATCH_NEXT)
	    தொடர்;
	எனில் (retval == TAG_MATCH_STOP)
	    நிறுத்து;
	எனில் (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    எனில் (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    திரும்பு;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	எனில் (findtags_match_tag(st, &tagp, margs))
	{
	    எனில் (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		நிறுத்து;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    நிலையான வெறுமை
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    முழு		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = வெற்று;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    எனில் (use_cscope)
	st->fp = வெற்று;	    // avoid GCC warning
    இல்லையெனில்
#endif
    {
#ifdef FEAT_MULTI_LANG
	எனில் (curbuf->b_help)
	{
	    எனில் (!findtags_in_help_init(st))
		திரும்பு;
	}
#endif

	st->fp = mch_fopen((எழுத்து *)st->tag_fname, "r");
	எனில் (st->fp == வெற்று)
	    திரும்பு;

	எனில் (p_verbose >= 5)
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

    எனில் (st->fp != வெற்று)
    {
	fclose(st->fp);
	st->fp = வெற்று;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    எனில் (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, வெற்று, வெற்று);

    எனில் (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    எனில் (st->match_மொத்தம் >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    நிலையான முழு
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    முழு		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    முழு		mtt;
    முழு		i;
    char_u	*mfp;
    char_u	*p;

    எனில் (st->match_மொத்தம் > 0)
	matches = ALLOC_MULT(char_u *, st->match_மொத்தம்);
    இல்லையெனில்
	matches = வெற்று;
    st->match_மொத்தம் = 0;
    க்கு (mtt = 0; mtt < MT_மொத்தம்; ++mtt)
    {
	க்கு (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    எனில் (matches == வெற்று)
		vim_free(mfp);
	    இல்லையெனில்
	    {
		எனில் (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    க்கு (p = mfp + 1; *p != NUL; ++p)
			எனில் (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_மொத்தம்++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    திரும்பு st->match_மொத்தம்;
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
    முழு
find_tags(
    char_u	*pat,			// pattern to search for
    முழு		*இலக்கம்_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    முழு		flags,
    முழு		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    முழு		first_file;		// trying first tag file
    முழு		retval = FAIL;		// return value
    முழு		round;

    முழு		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    முழு		i;
    char_u	*saved_pat = வெற்று;		// copy of pat[]
#endif

    முழு		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    முழு		has_re = (flags & TAG_REGEXP);	// regexp used
    முழு		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    முழு		use_cscope = (flags & TAG_CSCOPE);
#endif
    முழு		verbose = (flags & TAG_VERBOSE);
    முழு		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    தேர்வு (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	நிலை TC_FOLLOWIC:		 நிறுத்து;
	நிலை TC_IGNORE:    p_ic = TRUE;  நிறுத்து;
	நிலை TC_MATCH:     p_ic = FALSE; நிறுத்து;
	நிலை TC_FOLLOWSCS: p_ic = ignorecase(pat); நிறுத்து;
	நிலை TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); நிறுத்து;
    }

    help_save = curbuf->b_help;

    எனில் (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	செல் findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    எனில் (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    இல்லையெனில் எனில் (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    எனில் (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	எனில் (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    எனில் (saved_pat != வெற்று)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    எனில் (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    எனில் (has_re && st.orgpat->regmatch.regprog == வெற்று)
	செல் findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    எனில் (retval != NOTDONE)
	செல் findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    எனில் ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == வெற்று
	    && curbuf->b_fname != வெற்று
	    && (i = (முழு)STRLEN(curbuf->b_fname)) > 4
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
    க்கு (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      க்கு (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  எனில் (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      நிறுத்து;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	எனில் (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	எனில் (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    நிறுத்து;
#ifdef FEAT_CSCOPE
	எனில் (use_cscope)
	    நிறுத்து;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    எனில் (!st.stop_searching)
    {
	எனில் (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    எனில் (retval == FAIL)
	st.match_மொத்தம் = 0;

    *இலக்கம்_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    திரும்பு retval;
}

நிலையான garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    நிலையான வெறுமை
found_tagfile_cb(char_u *fname, வெறுமை *cookie UNUSED)
{
    எனில் (ga_grow(&tag_fnames, 1) == FAIL)
	திரும்பு;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_tag_stuff(வெறுமை)
{
    ga_clear_strings(&tag_fnames);
    எனில் (curwin != வெற்று)
	do_tag(வெற்று, DT_FREE, 0, 0, 0);
    tag_freematch();

# எனில் defined(FEAT_QUICKFIX)
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
    முழு
get_tagfname(
    tagname_T	*tnp,	// holds status info
    முழு		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = வெற்று;
    char_u		*r_ptr;
    முழு			i;

    எனில் (first)
	CLEAR_POINTER(tnp);

    எனில் (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	எனில் (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, அளவு(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# இல்லையெனில்
		    "doc/tags doc/tags-??"
# endif
#இல்லையெனில்
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, வெற்று);
	}

	எனில் (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    எனில் (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		திரும்பு FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    க்கு (i = 0; i < tag_fnames.ga_len; ++i)
		எனில் (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    திரும்பு FAIL; // avoid duplicate file names
	}
	இல்லையெனில்
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	திரும்பு OK;
    }

    எனில் (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	எனில் (tnp->tn_tags == வெற்று)
	    திரும்பு FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    க்கு (;;)
    {
	எனில் (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_தேடல்_ctx);
	    எனில் (fname != வெற்று)
		நிறுத்து;

	    tnp->tn_did_filefind_init = FALSE;
	}
	இல்லையெனில்
	{
	    char_u  *filename = வெற்று;

	    // Stop when used all parts of 'tags'.
	    எனில் (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_தேடல்_ctx);
		tnp->tn_தேடல்_ctx = வெற்று;
		திரும்பு FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (வெறுமை)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    எனில் (r_ptr != வெற்று)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_தேடல்_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_தேடல்_ctx, TRUE, curbuf->b_ffname);
	    எனில் (tnp->tn_தேடல்_ctx != வெற்று)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    திரும்பு OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    வெறுமை
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_தேடல்_ctx);
    tnp->tn_தேடல்_ctx = வெற்று;
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
    நிலையான முழு
parse_tag_வரி(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    முழு		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    எனில் (is_etag)
	// emacs-style tag file
	திரும்பு emacs_tags_parse_வரி(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    எனில் (p == வெற்று)
	திரும்பு FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    எனில் (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    எனில் (p == வெற்று)
	திரும்பு FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    எனில் (*p != NUL)
	++p;
    எனில் (*p == NUL)
	திரும்பு FAIL;
    tagp->command = p;

    திரும்பு OK;
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
    நிலையான முழு
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    போது ((p = vim_strchr(p, '\t')) != வெற்று)
    {
	++p;
	எனில் (STRNCMP(p, "file:", 5) == 0)
	    திரும்பு TRUE;
    }

    திரும்பு FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    நிலையான size_t
matching_வரி_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    திரும்பு (p - lbuf) + STRLEN(p);
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
    நிலையான முழு
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    முழு		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    எனில் (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    இல்லையெனில்
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_வரி(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = வெற்று;
    tagp->user_data = வெற்று;
    tagp->tagline = 0;
    tagp->command_end = வெற்று;

    எனில் (retval != OK)
	திரும்பு retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    எனில் (find_extra(&p) == OK)
    {
	எனில் (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	இல்லையெனில்
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	எனில் (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    போது (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		எனில் (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		இல்லையெனில் எனில் (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		இல்லையெனில் எனில் (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((எழுத்து *)p + 5);
		எனில் (tagp->tagkind != வெற்று && tagp->user_data != வெற்று)
		    நிறுத்து;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		எனில் (pc == வெற்று || (pt != வெற்று && pc > pt))
		    tagp->tagkind = p;
		எனில் (pt == வெற்று)
		    நிறுத்து;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    எனில் (tagp->tagkind != வெற்று)
    {
	க்கு (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    எனில் (tagp->user_data != வெற்று)
    {
	க்கு (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    திரும்பு retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    நிலையான char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    முழு		c;

#ifdef FEAT_EMACS_TAGS
    எனில் (tagp->is_etag)
	c = 0;	    // to shut up GCC
    இல்லையெனில்
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    எனில் (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    திரும்பு fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    நிலையான முழு
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    முழு		forceit,	// :ta with !
    முழு		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    முழு		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = வெற்று;
    char_u	*fname;
    tagptrs_T	tagp;
    முழு		retval = FAIL;
    முழு		getfile_result = GETFILE_UNUSED;
    முழு		தேடல்_options;
#ifdef FEAT_தேடல்_EXTRA
    முழு		save_no_hlsearch;
#endif
#எனில் defined(FEAT_QUICKFIX)
    win_T	*curwin_save = வெற்று;
#endif
    char_u	*full_fname = வெற்று;
#ifdef FEAT_FOLDING
    முழு		old_சாவி_Typed = சாவி_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    முழு		isdigit = FALSE;

    எனில் (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	திரும்பு FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_வரி_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    எனில் (lbuf != வெற்று)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    எனில் (pbuf == வெற்று || lbuf == வெற்று || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = வெற்று;
	செல் erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    எனில் (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    க்கு (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	எனில் (tagp.is_etag && *str == ',')// stop at ',' after line number
	    நிறுத்து;
#endif
	*pbuf_end++ = *str++;
	எனில் (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    நிறுத்து;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    எனில் (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	எனில் (isdigit)
	    str++;
	எனில் (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    எனில் (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    இல்லையெனில்
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    எனில் (fname == வெற்று)
	செல் erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    எனில் (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, வெற்று))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	எனில் (nofile_fname == வெற்று)
	    nofile_fname = empty_option;
	செல் erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#எனில் defined(FEAT_QUICKFIX)
    எனில் (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	எனில் (!curwin->w_p_pvw)
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
    எனில் (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	எனில் (existing_buf != வெற்று)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    எனில் (swbuf_goto_win_with_buf(existing_buf) != வெற்று)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    எனில் (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	எனில் (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	எனில் ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	எனில் (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    எனில் (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    செல் erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    எனில் (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#எனில் defined(FEAT_QUICKFIX)
	எனில் (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	இல்லையெனில்
#endif
	    keep_help_flag = curbuf->b_help;
    }

    எனில் (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, வெற்று, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    எனில் (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_தேடல்_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	எனில் (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	எனில் (vim_strchr(p_cpo, CPO_TAGPAT) != வெற்று)
	    தேடல்_options = 0;
	இல்லையெனில்
	    தேடல்_options = தேடல்_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	எனில் (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	எனில் (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    எனில் (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    இல்லையெனில்
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    எனில் (do_தேடல்(வெற்று, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 தேடல்_options, வெற்று))
		retval = OK;
	    இல்லையெனில்
	    {
		முழு	found = 1;
		முழு	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		எனில் (!do_தேடல்(வெற்று, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 தேடல்_options, வெற்று))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (வெறுமை)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((எழுத்து *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    எனில் (!do_தேடல்(வெற்று, '/', '/', pbuf, pbuflen, (long)1,
							 தேடல்_options, வெற்று))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((எழுத்து *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			எனில் (!do_தேடல்(வெற்று, '/', '/', pbuf, pbuflen, (long)1,
							 தேடல்_options, வெற்று))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		எனில் (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		இல்லையெனில்
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    எனில் (found == 2 || !save_p_ic)
		    {
			செய்தி(_(e_couldnt_find_tag_just_guessing));
			எனில் (!செய்தி_scrolled && செய்தி_silent == 0)
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
	இல்லையெனில்
	{
	    முழு		save_secure = secure;

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
	    எனில் (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_தேடல்_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	எனில் (தேடல்_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	எனில் (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	எனில் (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    எனில் (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    எனில் ((fdo_flags & FDO_TAG) && old_சாவி_Typed)
		foldOpenCursor();
#endif
	}

#எனில் defined(FEAT_QUICKFIX)
	எனில் (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	எனில் (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    இல்லையெனில்
    {
	எனில் (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	எனில் (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#எனில் defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	இல்லையெனில் எனில் (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    எனில் (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#எனில் defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    எனில் (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#எனில் defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    திரும்பு retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    நிலையான char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, முழு expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = வெற்று;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    எனில் (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       திரும்பு வெற்று;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    எனில் (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == வெற்று)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, வெற்று,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	எனில் (expanded_fname != வெற்று)
	    fname = expanded_fname;
    }

    எனில் ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	எனில் (retval != வெற்று)
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
    இல்லையெனில்
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    திரும்பு retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    நிலையான முழு
test_for_current(
#ifdef FEAT_EMACS_TAGS
    முழு	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    முழு	    c;
    முழு	    retval = FALSE;
    char_u  *fullname;

    எனில் (buf_ffname != வெற்று)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	எனில் (is_etag)
	    c = 0;	    // to shut up GCC
	இல்லையெனில்
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	எனில் (fullname != வெற்று)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	எனில் (!is_etag)
#endif
	    *fname_end = c;
    }

    திரும்பு retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    நிலையான முழு
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    க்கு (;;)
    {
	எனில் (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	இல்லையெனில் எனில் (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    எனில் (*str != first_char)
		str = வெற்று;
	    இல்லையெனில்
		++str;
	}
	இல்லையெனில்
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((எழுத்து *)str, "|;\"");
	    எனில் (str != வெற்று)
	    {
		++str;
		நிறுத்து;
	    }

	}
	எனில் (str == வெற்று || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    நிறுத்து;
	++str;	// skip ';'
	first_char = *str;
    }

    எனில் (str != வெற்று && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	திரும்பு OK;
    }
    திரும்பு FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    வெறுமை
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    முழு
expand_tags(
    முழு		tagnames,	// expand tag names
    char_u	*pat,
    முழு		*இலக்கம்_file,
    char_u	***file)
{
    முழு		i;
    முழு		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    முழு		ret;

    name_buf = alloc(name_buf_size);
    எனில் (name_buf == வெற்று)
	திரும்பு FAIL;

    எனில் (tagnames)
	extra_flag = TAG_NAMES;
    இல்லையெனில்
	extra_flag = 0;
    எனில் (pat[0] == '/')
	ret = find_tags(pat + 1, இலக்கம்_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    இல்லையெனில்
	ret = find_tags(pat, இலக்கம்_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    எனில் (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 க்கு (i = 0; i < *இலக்கம்_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     எனில் (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 எனில் (buf == வெற்று)
		 {
		     vim_free(name_buf);
		     திரும்பு FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != வெற்று && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    திரும்பு ret;
}

#எனில் defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    நிலையான முழு
add_tag_field(
    dict_T  *dict,
    எழுத்து    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    முழு		len = 0;
    முழு		retval;

    // check that the field name doesn't exist yet
    எனில் (dict_has_சாவி(dict, field_name))
    {
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	திரும்பு FAIL;
    }
    buf = alloc(MAXPATHL);
    எனில் (buf == வெற்று)
	திரும்பு FAIL;
    எனில் (start != வெற்று)
    {
	எனில் (end == வெற்று)
	{
	    end = start + STRLEN(start);
	    போது (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (முழு)(end - start);
	எனில் (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    திரும்பு retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    முழு
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    முழு		இலக்கம்_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &இலக்கம்_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (முழு)MAXCOL, buf_fname);
    எனில் (ret != OK || இலக்கம்_matches <= 0)
	திரும்பு ret;

    க்கு (i = 0; i < இலக்கம்_matches; ++i)
    {
	எனில் (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    தொடர்;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	எனில் (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    தொடர்;
	}

	எனில் ((dict = dict_alloc()) == வெற்று)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    நிறுத்து;
	}
	எனில் (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    தொடர்;
	}

	full_fname = tag_full_fname(&tp);
	எனில் (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    வெற்று) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_எண்(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	எனில் (tp.command_end != வெற்று)
	{
	    க்கு (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		எனில் (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		இல்லையெனில் எனில் (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		இல்லையெனில் எனில் (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    முழு	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    போது (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (முழு)(p - n);
		    எனில் (*p == ':' && len > 0)
		    {
			s = ++p;
			போது (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			எனில் (add_tag_field(dict, (எழுத்து *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    இல்லையெனில்
			// Skip field without colon.
			போது (*p != NUL && *p >= ' ')
			    ++p;
		    எனில் (*p == NUL)
			நிறுத்து;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    திரும்பு ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    நிலையான வெறுமை
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_எண்(retdict, "matchnr", tag->cur_match + 1);
    dict_add_எண்(retdict, "bufnr", tag->cur_fnum);
    எனில் (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    எனில் ((pos = list_alloc_id(aid_tagstack_from)) == வெற்று)
	திரும்பு;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_எண்(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_எண்(pos, (varnumber_T)fmark->mark.lnum);
    list_append_எண்(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_எண்(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    வெறுமை
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    முழு		i;
    dict_T	*d;

    dict_add_எண்(retdict, "length", wp->w_tagstacklen);
    dict_add_எண்(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_பொருட்கள்);
    எனில் (l == வெற்று)
	திரும்பு;
    dict_add_list(retdict, "items", l);

    க்கு (i = 0; i < wp->w_tagstacklen; i++)
    {
	எனில் ((d = dict_alloc_id(aid_tagstack_details)) == வெற்று)
	    திரும்பு;
	எனில் (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    திரும்பு;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    நிலையான வெறுமை
tagstack_clear(win_T *wp)
{
    முழு i;

    // Free the current tag stack
    க்கு (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    நிலையான வெறுமை
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    முழு		i;

    tagstack_clear_entry(&tagstack[0]);
    க்கு (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    நிலையான வெறுமை
tagstack_தள்ளு_item(
	win_T	*wp,
	char_u	*tagname,
	முழு	cur_fnum,
	முழு	cur_match,
	pos_T	mark,
	முழு	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    முழு		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    எனில் (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    எனில் (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    நிலையான வெறுமை
tagstack_தள்ளு_பொருட்கள்(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    முழு		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	எனில் (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == வெற்று)
	    தொடர்;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	எனில் ((di = dict_find(itemdict, (char_u *)"from", -1)) == வெற்று)
	    தொடர்;
	எனில் (list2fpos(&di->di_tv, &mark, &fnum, வெற்று, FALSE) != OK)
	    தொடர்;
	எனில் ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == வெற்று)
	    தொடர்;

	எனில் (mark.col > 0)
	    mark.col--;
	tagstack_தள்ளு_item(wp, tagname,
		(முழு)dict_get_எண்(itemdict, "bufnr"),
		(முழு)dict_get_எண்(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    நிலையான வெறுமை
tagstack_set_curidx(win_T *wp, முழு curidx)
{
    wp->w_tagstackidx = curidx;
    எனில் (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    எனில் (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    முழு
set_tagstack(win_T *wp, dict_T *d, முழு action)
{
    dictitem_T	*di;
    list_T	*l = வெற்று;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    எனில் (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_அடுக்கு_within_tagfunc));
	திரும்பு FAIL;
    }
# endif

    எனில் ((di = dict_find(d, (char_u *)"items", -1)) != வெற்று)
    {
	எனில் (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    திரும்பு FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    எனில் ((di = dict_find(d, (char_u *)"curidx", -1)) != வெற்று)
	tagstack_set_curidx(wp, (முழு)tv_get_எண்(&di->di_tv) - 1);

    எனில் (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	முழு	tagstackidx = wp->w_tagstackidx;
	முழு	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	போது (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    எனில் (l != வெற்று)
    {
	எனில் (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_தள்ளு_பொருட்கள்(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    திரும்பு OK;
}
#endif
