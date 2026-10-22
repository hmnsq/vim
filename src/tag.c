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

#അന്തര്ഭാവ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
തരനിർവചനം ഘടന tag_pointers
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
    പൂർണ്ണസംഖ്യ		is_etag;	// TRUE for emacs tag
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
തരനിർവചനം എണ്ണൽ
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
തരനിർവചനം എണ്ണൽ
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_ദ്വിമാന,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
തരനിർവചനം ഘടന
{
    off_T	താഴെയുള്ള_offset;	// offset for first char of first line that
				// could match
    off_T	മുകളിലെ_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    പൂർണ്ണസംഖ്യ	താഴെയുള്ള_char;		// first char at low_offset
    പൂർണ്ണസംഖ്യ	മുകളിലെ_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
തരനിർവചനം എണ്ണൽ
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ	matchoff;		// tag match offset
    പൂർണ്ണസംഖ്യ	match_re;		// TRUE if the tag matches a regexp
    പൂർണ്ണസംഖ്യ	match_no_ic;		// TRUE if the tag matches with case
    പൂർണ്ണസംഖ്യ	has_re;			// regular expression used
    പൂർണ്ണസംഖ്യ	sortic;			// tags file sorted ignoring case (foldcase)
    പൂർണ്ണസംഖ്യ	sort_error;		// tags file not sorted
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
#define MT_എണ്ണം	16

സ്ഥിതം അക്ഷരം	*mt_names[MT_എണ്ണം/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
സ്ഥിതം char_u	*nofile_fname = ശൂന്യ;	// fname for NOTAGFILE error

സ്ഥിതം ശൂന്യം taglen_advance(പൂർണ്ണസംഖ്യ l);

സ്ഥിതം പൂർണ്ണസംഖ്യ jumpto_tag(char_u *lbuf, പൂർണ്ണസംഖ്യ forceit, പൂർണ്ണസംഖ്യ keep_help);
#ifdef FEAT_EMACS_TAGS
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_tag_വരി(char_u *lbuf, പൂർണ്ണസംഖ്യ is_etag, tagptrs_T *tagp);
#അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_tag_വരി(char_u *lbuf, tagptrs_T *tagp);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ test_for_static(tagptrs_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ parse_match(char_u *lbuf, tagptrs_T *tagp);
സ്ഥിതം char_u *tag_full_fname(tagptrs_T *tagp);
സ്ഥിതം char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, പൂർണ്ണസംഖ്യ expand);
#ifdef FEAT_EMACS_TAGS
സ്ഥിതം പൂർണ്ണസംഖ്യ test_for_current(പൂർണ്ണസംഖ്യ, char_u *, char_u *, char_u *, char_u *);
#അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ find_extra(char_u **pp);
സ്ഥിതം ശൂന്യം print_tag_list(പൂർണ്ണസംഖ്യ new_tag, പൂർണ്ണസംഖ്യ use_tagstack, പൂർണ്ണസംഖ്യ അക്കം_matches, char_u **matches);
#എങ്കിൽ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
സ്ഥിതം പൂർണ്ണസംഖ്യ add_llist_tags(char_u *tag, പൂർണ്ണസംഖ്യ അക്കം_matches, char_u **matches);
#endif

സ്ഥിതം char_u	*tagmatchname = ശൂന്യ;	// name of last used tag

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
സ്ഥിതം taggy_T ptag_entry = {ശൂന്യ, {{0, 0, 0}, 0}, 0, 0, ശൂന്യ};
#endif

#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
സ്ഥിതം callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    അക്ഷരം *
did_set_tagfunc(optset_T *args)
{
    പൂർണ്ണസംഖ്യ	retval;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    അല്ലെങ്കിൽ
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	എങ്കിൽ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    തിരികെ retval == FAIL ? e_invalid_argument : ശൂന്യ;
}
#endif

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_tagfunc_option(ശൂന്യം)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_tagfunc(പൂർണ്ണസംഖ്യ copyID UNUSED)
{
    പൂർണ്ണസംഖ്യ	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    തിരികെ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    ശൂന്യം
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    എങ്കിൽ (tfu_cb.cb_name != ശൂന്യ && *tfu_cb.cb_name != NUL)
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
    പൂർണ്ണസംഖ്യ
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    പൂർണ്ണസംഖ്യ		type,
    പൂർണ്ണസംഖ്യ		എണ്ണം,
    പൂർണ്ണസംഖ്യ		forceit,	// :ta with !
    പൂർണ്ണസംഖ്യ		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    പൂർണ്ണസംഖ്യ		tagstackidx = curwin->w_tagstackidx;
    പൂർണ്ണസംഖ്യ		tagstacklen = curwin->w_tagstacklen;
    പൂർണ്ണസംഖ്യ		cur_match = 0;
    പൂർണ്ണസംഖ്യ		cur_fnum = curbuf->b_fnum;
    പൂർണ്ണസംഖ്യ		oldtagstackidx = tagstackidx;
    പൂർണ്ണസംഖ്യ		prevtagstackidx = tagstackidx;
    പൂർണ്ണസംഖ്യ		prev_അക്കം_matches;
    പൂർണ്ണസംഖ്യ		new_tag = FALSE;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		ic;
    പൂർണ്ണസംഖ്യ		no_regexp = FALSE;
    പൂർണ്ണസംഖ്യ		error_cur_match = 0;
    പൂർണ്ണസംഖ്യ		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		jumped_to_tag = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		new_അക്കം_matches;
    char_u	**new_matches;
    പൂർണ്ണസംഖ്യ		use_tagstack;
    പൂർണ്ണസംഖ്യ		skip_സന്ദേശം = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    പൂർണ്ണസംഖ്യ		use_tfu = 1;
    char_u	*tofree = ശൂന്യ;

    // remember the matches for the last used tag
    സ്ഥിതം പൂർണ്ണസംഖ്യ		അക്കം_matches = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		max_അക്കം_matches = 0;  // limit used for match search
    സ്ഥിതം char_u	**matches = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		flags;

#ifdef EXITFREE
    എങ്കിൽ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(അക്കം_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	അക്കം_matches = 0;
	തിരികെ FALSE;
    }
#endif

#ifdef FEAT_EVAL
    എങ്കിൽ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_അട്ടി_within_tagfunc));
	തിരികെ FALSE;
    }
#endif

    എങ്കിൽ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	തിരികെ FALSE;

    എങ്കിൽ (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_അക്കം_matches = അക്കം_matches;
    free_string_option(nofile_fname);
    nofile_fname = ശൂന്യ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    എങ്കിൽ ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#എങ്കിൽ defined(FEAT_QUICKFIX)
	എങ്കിൽ (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    എങ്കിൽ ((ptag_entry.tagname = vim_strsave(tag)) == ശൂന്യ)
		പോകുക end_do_tag;
	}
#endif
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ defined(FEAT_QUICKFIX)
	എങ്കിൽ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	അല്ലെങ്കിൽ
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	എങ്കിൽ (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#എങ്കിൽ defined(FEAT_QUICKFIX)
	    എങ്കിൽ (g_do_tagpreview != 0)
	    {
		എങ്കിൽ (ptag_entry.tagname != ശൂന്യ
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		അല്ലെങ്കിൽ
		{
		    tagstack_clear_entry(&ptag_entry);
		    എങ്കിൽ ((ptag_entry.tagname = vim_strsave(tag)) == ശൂന്യ)
			പോകുക end_do_tag;
		}
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ആകുമ്പോൾ (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		എങ്കിൽ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    വേണ്ടി (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = ശൂന്യ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		എങ്കിൽ ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == ശൂന്യ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    പോകുക end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (
#എങ്കിൽ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == ശൂന്യ :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_അട്ടി_empty));
		പോകുക end_do_tag;
	    }

	    എങ്കിൽ (type == DT_എടുക്കുക)		// go to older position
	    {
#ifdef FEAT_FOLDING
		പൂർണ്ണസംഖ്യ	old_താക്കോൽ_Typed = താക്കോൽ_Typed;
#endif
		എങ്കിൽ ((tagstackidx -= എണ്ണം) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_അട്ടി));
		    എങ്കിൽ (tagstackidx + എണ്ണം == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			പോകുക end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_മുകൾ_of_tag_അട്ടി));
		    പോകുക end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		എങ്കിൽ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    എങ്കിൽ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			പോകുക end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		അല്ലെങ്കിൽ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		എങ്കിൽ ((fdo_flags & FDO_TAG) && old_താക്കോൽ_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(അക്കം_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		അക്കം_matches = 0;
		tag_freematch();
		പോകുക end_do_tag;
	    }

	    എങ്കിൽ (type == DT_TAG
#എങ്കിൽ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		അല്ലെങ്കിൽ
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    എങ്കിൽ ((tagstackidx += എണ്ണം - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_മുകൾ_of_tag_അട്ടി));
			save_pos = FALSE;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_അട്ടി));
			tagstackidx = 0;
			പോകുക end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    അല്ലെങ്കിൽ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#എങ്കിൽ defined(FEAT_QUICKFIX)
		എങ്കിൽ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		അല്ലെങ്കിൽ
#endif
		{
		    എങ്കിൽ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		തിരഞ്ഞെടുപ്പ് (type)
		{
		    അവസ്ഥ DT_FIRST: cur_match = എണ്ണം - 1; നിർത്തുക;
		    അവസ്ഥ DT_SELECT:
		    അവസ്ഥ DT_JUMP:
#ifdef FEAT_CSCOPE
		    അവസ്ഥ DT_CSCOPE:
#endif
		    അവസ്ഥ DT_LAST:  cur_match = MAXCOL - 1; നിർത്തുക;
		    അവസ്ഥ DT_NEXT:  cur_match += എണ്ണം; നിർത്തുക;
		    അവസ്ഥ DT_PREV:  cur_match -= എണ്ണം; നിർത്തുക;
		}
		എങ്കിൽ (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		അല്ലെങ്കിൽ എങ്കിൽ (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_സന്ദേശം = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#എങ്കിൽ defined(FEAT_QUICKFIX)
	എങ്കിൽ (g_do_tagpreview != 0)
	{
	    എങ്കിൽ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	അല്ലെങ്കിൽ
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    എങ്കിൽ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    എങ്കിൽ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    എങ്കിൽ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	എങ്കിൽ (buf != ശൂന്യ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	എങ്കിൽ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#എങ്കിൽ defined(FEAT_QUICKFIX)
	അല്ലെങ്കിൽ എങ്കിൽ (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	അല്ലെങ്കിൽ
	    name = tag;
	other_name = (tagmatchname == ശൂന്യ || STRCMP(tagmatchname, name) != 0);
	എങ്കിൽ (new_tag
		|| (cur_match >= അക്കം_matches && max_അക്കം_matches != MAXCOL)
		|| other_name)
	{
	    എങ്കിൽ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    എങ്കിൽ (type == DT_SELECT || type == DT_JUMP
#എങ്കിൽ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    എങ്കിൽ (type == DT_TAG)
		max_അക്കം_matches = MAXCOL;
	    അല്ലെങ്കിൽ
		max_അക്കം_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    എങ്കിൽ (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    അല്ലെങ്കിൽ
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    എങ്കിൽ (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    എങ്കിൽ (verbose)
		flags |= TAG_VERBOSE;

	    എങ്കിൽ (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    എങ്കിൽ (find_tags(name, &new_അക്കം_matches, &new_matches, flags,
					    max_അക്കം_matches, buf_ffname) == OK
		    && new_അക്കം_matches < max_അക്കം_matches)
		max_അക്കം_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    എങ്കിൽ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_അക്കം_matches, new_matches);
		നിർത്തുക;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    എങ്കിൽ (!new_tag && !other_name)
	    {
		പൂർണ്ണസംഖ്യ	    j, k;
		പൂർണ്ണസംഖ്യ	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		വേണ്ടി (j = 0; j < അക്കം_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    വേണ്ടി (i = idx; i < new_അക്കം_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			എങ്കിൽ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    വേണ്ടി (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    നിർത്തുക;
			}
		    }
		}
	    }
	    FreeWild(അക്കം_matches, matches);
	    അക്കം_matches = new_അക്കം_matches;
	    matches = new_matches;
	}

	എങ്കിൽ (അക്കം_matches <= 0)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_tag_not_found_str), name);
#എങ്കിൽ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ ask_for_selection = FALSE;
#എങ്കിൽ defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    എങ്കിൽ (type == DT_CSCOPE && അക്കം_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    അല്ലെങ്കിൽ
#endif
	    എങ്കിൽ (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = എണ്ണം > 0 ? എണ്ണം - 1 : 0;
	    അല്ലെങ്കിൽ എങ്കിൽ (type == DT_SELECT || (type == DT_JUMP && അക്കം_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, അക്കം_matches, matches);
		ask_for_selection = TRUE;
	    }
#എങ്കിൽ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    അല്ലെങ്കിൽ എങ്കിൽ (type == DT_LTAG)
	    {
		എങ്കിൽ (add_llist_tags(tag, അക്കം_matches, matches) == FAIL)
		    പോകുക end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    എങ്കിൽ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_സംഖ്യ(ശൂന്യ);
		എങ്കിൽ (i <= 0 || i > അക്കം_matches || got_int)
		{
		    // no valid choice: don't change anything
		    എങ്കിൽ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    നിർത്തുക;
		}
		cur_match = i - 1;
	    }

	    എങ്കിൽ (cur_match >= അക്കം_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		എങ്കിൽ ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == ശൂന്യ)
		{
		    എങ്കിൽ (അക്കം_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    അല്ലെങ്കിൽ
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_സന്ദേശം = TRUE;
		}
		cur_match = അക്കം_matches - 1;
	    }
	    എങ്കിൽ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		എങ്കിൽ (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#എങ്കിൽ defined(FEAT_QUICKFIX)
	    അല്ലെങ്കിൽ എങ്കിൽ (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    എങ്കിൽ (nofile_fname != ശൂന്യ && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    എങ്കിൽ (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (അക്കം_matches > 1 || ic)
		&& !skip_സന്ദേശം)
	    {
		// Give an indication of the number of matching tags
		sprintf((അക്ഷരം *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				അക്കം_matches,
				max_അക്കം_matches != MAXCOL ? _(" or more") : "");
		എങ്കിൽ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		എങ്കിൽ ((അക്കം_matches > prev_അക്കം_matches || new_tag)
							   && അക്കം_matches > 1)
		{
		    എങ്കിൽ (ic)
			സന്ദേശം_attr((അക്ഷരം *)IObuff, HL_ATTR(HLF_W));
		    അല്ലെങ്കിൽ
			സന്ദേശം((അക്ഷരം *)IObuff);
		    സന്ദേശം_scroll = TRUE;	// don't overwrite this message
		}
		അല്ലെങ്കിൽ
		    give_warning(IObuff, ic);
		എങ്കിൽ (ic && !സന്ദേശം_scrolled && സന്ദേശം_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#എങ്കിൽ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (പൂർണ്ണസംഖ്യ)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#എങ്കിൽ defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, ശൂന്യ, -1);
#endif

	    എങ്കിൽ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		എങ്കിൽ ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_അക്കം_matches != MAXCOL
					     || cur_match < അക്കം_matches - 1)))
		{
		    error_cur_match = cur_match;
		    എങ്കിൽ (use_tagstack)
			--tagstackidx;
		    എങ്കിൽ (type == DT_PREV)
			--cur_match;
		    അല്ലെങ്കിൽ
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    തുടരുക;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    അല്ലെങ്കിൽ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		എങ്കിൽ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	നിർത്തുക;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    എങ്കിൽ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    തിരികെ jumped_to_tag;
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    സ്ഥിതം ശൂന്യം
print_tag_list(
    പൂർണ്ണസംഖ്യ		new_tag,
    പൂർണ്ണസംഖ്യ		use_tagstack,
    പൂർണ്ണസംഖ്യ		അക്കം_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    പൂർണ്ണസംഖ്യ		tagstackidx = curwin->w_tagstackidx;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    പൂർണ്ണസംഖ്യ		taglen;
    പൂർണ്ണസംഖ്യ		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (പൂർണ്ണസംഖ്യ)(tagp.tagname_end - tagp.tagname + 2);
    എങ്കിൽ (taglen < 18)
	taglen = 18;
    എങ്കിൽ (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    എങ്കിൽ (സന്ദേശം_col == 0)
	സന്ദേശം_didout = FALSE;	// overwrite previous message
    സന്ദേശം_start();
    സന്ദേശം_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    സന്ദേശം_clr_eos();
    taglen_advance(taglen);
    സന്ദേശം_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    വേണ്ടി (i = 0; i < അക്കം_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	എങ്കിൽ (!new_tag && (
#എങ്കിൽ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	അല്ലെങ്കിൽ
	    *IObuff = ' ';
	vim_snprintf((അക്ഷരം *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	സന്ദേശം_puts((അക്ഷരം *)IObuff);
	എങ്കിൽ (tagp.tagkind != ശൂന്യ)
	    സന്ദേശം_outtrans_len(tagp.tagkind,
			  (പൂർണ്ണസംഖ്യ)(tagp.tagkind_end - tagp.tagkind));
	സന്ദേശം_advance(13);
	സന്ദേശം_outtrans_len_attr(tagp.tagname,
			   (പൂർണ്ണസംഖ്യ)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	സന്ദേശം_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    സന്ദേശം_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	എങ്കിൽ (സന്ദേശം_col > 0)
	    സന്ദേശം_putchar('\n');
	എങ്കിൽ (got_int)
	    നിർത്തുക;
	സന്ദേശം_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	എങ്കിൽ (command_end != ശൂന്യ)
	{
	    p = command_end + 3;
	    ആകുമ്പോൾ (*p && *p != '\r' && *p != '\n')
	    {
		ആകുമ്പോൾ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		എങ്കിൽ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    തുടരുക;
		}
		// skip "kind:<kind>" and "<kind>"
		എങ്കിൽ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    തുടരുക;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ആകുമ്പോൾ (*p && *p != '\r' && *p != '\n')
		{
		    എങ്കിൽ (സന്ദേശം_col + ptr2cells(p) >= cmdline_width)
		    {
			സന്ദേശം_putchar('\n');
			എങ്കിൽ (got_int)
			    നിർത്തുക;
			സന്ദേശം_advance(15);
		    }
		    p = സന്ദേശം_outtrans_one(p, attr);
		    എങ്കിൽ (*p == TAB)
		    {
			സന്ദേശം_puts_attr(" ", attr);
			നിർത്തുക;
		    }
		    എങ്കിൽ (*p == ':')
			attr = 0;
		}
	    }
	    എങ്കിൽ (സന്ദേശം_col > 15)
	    {
		സന്ദേശം_putchar('\n');
		എങ്കിൽ (got_int)
		    നിർത്തുക;
		സന്ദേശം_advance(15);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	എങ്കിൽ (*p == '/' || *p == '?')
	{
	    ++p;
	    എങ്കിൽ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ആകുമ്പോൾ (p != command_end && vim_isspace(*p))
	    ++p;

	ആകുമ്പോൾ (p != command_end)
	{
	    എങ്കിൽ (സന്ദേശം_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		സന്ദേശം_putchar('\n');
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    സന്ദേശം_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    എങ്കിൽ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    എങ്കിൽ (*p == TAB)
	    {
		സന്ദേശം_putchar(' ');
		++p;
	    }
	    അല്ലെങ്കിൽ
		p = സന്ദേശം_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    എങ്കിൽ (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		നിർത്തുക;
	    // don't display matching '/' or '?'
	    എങ്കിൽ (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		നിർത്തുക;
	}
	എങ്കിൽ (സന്ദേശം_col)
	    സന്ദേശം_putchar('\n');
	ui_breakcheck();
    }
    എങ്കിൽ (got_int)
	got_int = FALSE;	// only stop the listing
}

#എങ്കിൽ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_llist_tags(
    char_u	*tag,
    പൂർണ്ണസംഖ്യ		അക്കം_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    എങ്കിൽ (list == ശൂന്യ || fname == ശൂന്യ || cmd == ശൂന്യ)
    {
	vim_free(cmd);
	vim_free(fname);
	എങ്കിൽ (list != ശൂന്യ)
	    list_free(list);
	തിരികെ FAIL;
    }

    വേണ്ടി (i = 0; i < അക്കം_matches; ++i)
    {
	പൂർണ്ണസംഖ്യ	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (പൂർണ്ണസംഖ്യ)(tagp.tagname_end - tagp.tagname);
	എങ്കിൽ (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	എങ്കിൽ (p == ശൂന്യ)
	    തുടരുക;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	എങ്കിൽ (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((അക്ഷരം *)tagp.command);
	അല്ലെങ്കിൽ
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    എങ്കിൽ (cmd_end == ശൂന്യ)
	    {
		വേണ്ടി (p = tagp.command;
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
	    എങ്കിൽ (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    എങ്കിൽ (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    എങ്കിൽ (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (പൂർണ്ണസംഖ്യ)(cmd_end - cmd_start + 1);
	    എങ്കിൽ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    എങ്കിൽ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	എങ്കിൽ ((dict = dict_alloc()) == ശൂന്യ)
	    തുടരുക;
	എങ്കിൽ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    തുടരുക;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_സംഖ്യ(dict, "lnum", lnum);
	എങ്കിൽ (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, ശൂന്യ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    തിരികെ OK;
}
#endif

/*
 * Free cached tags.
 */
    ശൂന്യം
tag_freematch(ശൂന്യം)
{
    VIM_CLEAR(tagmatchname);
}

    സ്ഥിതം ശൂന്യം
taglen_advance(പൂർണ്ണസംഖ്യ l)
{
    എങ്കിൽ (l == MAXCOL)
    {
	സന്ദേശം_putchar('\n');
	സന്ദേശം_advance(24);
    }
    അല്ലെങ്കിൽ
	സന്ദേശം_advance(13 + l);
}

/*
 * Print the tag stack
 */
    ശൂന്യം
do_tags(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    പൂർണ്ണസംഖ്യ		tagstackidx = curwin->w_tagstackidx;
    പൂർണ്ണസംഖ്യ		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    സന്ദേശം_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    വേണ്ടി (i = 0; i < tagstacklen; ++i)
    {
	എങ്കിൽ (tagstack[i].tagname != ശൂന്യ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    എങ്കിൽ (name == ശൂന്യ)	    // file name not available
		തുടരുക;

	    സന്ദേശം_putchar('\n');
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    സന്ദേശം_outtrans(IObuff);
	    സന്ദേശം_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    എങ്കിൽ (tagstackidx == tagstacklen)	// idx at top of stack
	സന്ദേശം_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    പൂർണ്ണസംഖ്യ		i;

    ആകുമ്പോൾ (len > 0)
    {
	i = (പൂർണ്ണസംഖ്യ)TOUPPER_ASC(*s1) - (പൂർണ്ണസംഖ്യ)TOUPPER_ASC(*s2);
	എങ്കിൽ (i != 0)
	    തിരികെ i;			// this character different
	എങ്കിൽ (*s1 == NUL)
	    നിർത്തുക;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    തിരികെ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
തരനിർവചനം ഘടന
{
    char_u	*pat;		// the pattern
    പൂർണ്ണസംഖ്യ		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    പൂർണ്ണസംഖ്യ		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    സ്ഥിതം ശൂന്യം
prepare_pats(pat_T *pats, പൂർണ്ണസംഖ്യ has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    എങ്കിൽ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	എങ്കിൽ (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	എങ്കിൽ (pats->head == pats->pat)
	    pats->headlen = 0;
	അല്ലെങ്കിൽ
	    വേണ്ടി (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		എങ്കിൽ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != ശൂന്യ)
		    നിർത്തുക;
	എങ്കിൽ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    എങ്കിൽ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    അല്ലെങ്കിൽ
	pats->regmatch.regprog = ശൂന്യ;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
തരനിർവചനം എണ്ണൽ {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    സ്ഥിതം tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    എങ്കിൽ (VIM_ISDIGIT(*cmd))
	തിരികെ *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    എങ്കിൽ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	തിരികെ (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    എങ്കിൽ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ശൂന്യ)
	തിരികെ TAGCMD_INVALID;
    തിരികെ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    പൂർണ്ണസംഖ്യ		*match_എണ്ണം,	// here the number of tags found will be placed
    പൂർണ്ണസംഖ്യ		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    പൂർണ്ണസംഖ്യ		ntags = 0;
    പൂർണ്ണസംഖ്യ		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    എങ്കിൽ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ശൂന്യ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	തിരികെ FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    എങ്കിൽ ((d = dict_alloc_lock(VAR_FIXED)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (!(flags & TAG_INS_COMP) && tag->user_data != ശൂന്യ)
	dict_add_string(d, "user_data", tag->user_data);
    എങ്കിൽ (buf_ffname != ശൂന്യ)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((അക്ഷരം *)flagString, വലുപ്പം(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    എങ്കിൽ (result == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_സംഖ്യ == VVAL_NULL)
    {
	clear_tv(&rettv);
	തിരികെ NOTDONE;
    }
    എങ്കിൽ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_മൂല്യം_from_tagfunc));
	തിരികെ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ഇനങ്ങൾ(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	പൂർണ്ണസംഖ്യ		len;
	dict_iterator_T	iter;
	char_u		*dict_താക്കോൽ;
	typval_T	*tv;
	പൂർണ്ണസംഖ്യ		has_extra = 0;
	പൂർണ്ണസംഖ്യ		name_only = flags & TAG_NAMES;

	എങ്കിൽ (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_മൂല്യം_from_tagfunc));
	    നിർത്തുക;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# അല്ലെങ്കിൽ
	len = 2;
# endif
	res_name = ശൂന്യ;
	res_fname = ശൂന്യ;
	res_cmd = ശൂന്യ;
	res_kind = ശൂന്യ;

	dict_iterate_start(&item->li_tv, &iter);
	ആകുമ്പോൾ (ശൂന്യ != (dict_താക്കോൽ = dict_iterate_next(&iter, &tv)))
	{
	    എങ്കിൽ (tv->v_type != VAR_STRING || tv->vval.v_string == ശൂന്യ)
		തുടരുക;

	    len += (പൂർണ്ണസംഖ്യ)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		തുടരുക;
	    }
	    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		തുടരുക;
	    }
	    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		തുടരുക;
	    }
	    has_extra = 1;
	    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		തുടരുക;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (പൂർണ്ണസംഖ്യ)STRLEN(dict_താക്കോൽ) + 1;
	}

	എങ്കിൽ (has_extra)
	    len += 2;	// need space for ;"

	എങ്കിൽ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_മൂല്യം_from_tagfunc));
	    നിർത്തുക;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	എങ്കിൽ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_മൂല്യം_from_tagfunc));
	    നിർത്തുക;
	}
	എങ്കിൽ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	എങ്കിൽ (name_only)
	    mfp = vim_strsave(res_name);
	അല്ലെങ്കിൽ
	    mfp = alloc(വലുപ്പം(char_u) + len + 1);

	എങ്കിൽ (mfp == ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (!name_only)
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

	    എങ്കിൽ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    എങ്കിൽ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		എങ്കിൽ (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		ആകുമ്പോൾ (ശൂന്യ != (dict_താക്കോൽ = dict_iterate_next(&iter, &tv)))
		{
		    എങ്കിൽ (tv->v_type != VAR_STRING || tv->vval.v_string == ശൂന്യ)
			തുടരുക;

		    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "name") == 0)
			തുടരുക;
		    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "filename") == 0)
			തുടരുക;
		    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "cmd") == 0)
			തുടരുക;
		    എങ്കിൽ (STRCMP(dict_താക്കോൽ, "kind") == 0)
			തുടരുക;

		    *p++ = TAB;
		    STRCPY(p, dict_താക്കോൽ);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	എങ്കിൽ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	അല്ലെങ്കിൽ
	{
	    vim_free(mfp);
	    നിർത്തുക;
	}
    }

    clear_tv(&rettv);

    *match_എണ്ണം = ntags;
    തിരികെ result;
}
#endif

/*
 * State information used during a tag search
 */
തരനിർവചനം ഘടന
{
    tagsearch_state_T	state;		// tag search state
    പൂർണ്ണസംഖ്യ		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    പൂർണ്ണസംഖ്യ		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    പൂർണ്ണസംഖ്യ		flags;			// flags used for tag search
    പൂർണ്ണസംഖ്യ		tag_file_sorted;	// !_TAG_FILE_SORTED value
    പൂർണ്ണസംഖ്യ		get_searchpat;		// used for 'showfulltag'
    പൂർണ്ണസംഖ്യ		help_only;		// only search for help tags
    പൂർണ്ണസംഖ്യ		did_open;		// did open a tag file
    പൂർണ്ണസംഖ്യ		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    പൂർണ്ണസംഖ്യ		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    പൂർണ്ണസംഖ്യ		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    പൂർണ്ണസംഖ്യ		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    പൂർണ്ണസംഖ്യ		is_txt;			// flag of file extension
#endif
    പൂർണ്ണസംഖ്യ		match_എണ്ണം;		// number of matches found
    garray_T	ga_match[MT_എണ്ണം];	// stores matches in sequence
    hashtab_T	ht_match[MT_എണ്ണം];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    പൂർണ്ണസംഖ്യ			flags,
    പൂർണ്ണസംഖ്യ			mincount)
{
    പൂർണ്ണസംഖ്യ		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = ശൂന്യ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
    st->orgpat->regmatch.regprog = ശൂന്യ;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = ശൂന്യ;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_എണ്ണം = 0;
    st->stop_searching = FALSE;

    വേണ്ടി (mtt = 0; mtt < MT_എണ്ണം; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], വലുപ്പം(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    എങ്കിൽ (st->tag_fname == ശൂന്യ
	    || st->lbuf == ശൂന്യ
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == ശൂന്യ
#endif
       )
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Free the state used by find_tags()
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_in_help_init(findtags_state_T *st)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    എങ്കിൽ (st->is_txt)
	STRCPY(st->help_lang, "en");
    അല്ലെങ്കിൽ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (പൂർണ്ണസംഖ്യ)STRLEN(st->tag_fname);
	എങ്കിൽ (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	അല്ലെങ്കിൽ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    എങ്കിൽ (st->help_lang_find != ശൂന്യ
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	തിരികെ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    എങ്കിൽ ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == ശൂന്യ
	    && curbuf->b_fname != ശൂന്യ
	    && (i = (പൂർണ്ണസംഖ്യ)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    അല്ലെങ്കിൽ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	വേണ്ടി (s = p_hlg; *s != NUL; ++s)
	{
	    എങ്കിൽ (STRNICMP(s, st->help_lang, 2) == 0)
		നിർത്തുക;
	    ++st->help_pri;
	    എങ്കിൽ ((s = vim_strchr(s, ',')) == ശൂന്യ)
		നിർത്തുക;
	}
	എങ്കിൽ (s == ശൂന്യ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    എങ്കിൽ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    തിരികെ TRUE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    പൂർണ്ണസംഖ്യ		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    പൂർണ്ണസംഖ്യ		retval;

    എങ്കിൽ (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	തിരികെ NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_എണ്ണം,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    തിരികെ retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
സ്ഥിതം ഘടന
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
സ്ഥിതം പൂർണ്ണസംഖ്യ incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    സ്ഥിതം ശൂന്യം
emacs_tags_incstack_free(ശൂന്യം)
{
    ആകുമ്പോൾ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ശൂന്യ;
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
    സ്ഥിതം ശൂന്യം
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    എങ്കിൽ (vim_fgets(st->ebuf, LSIZE, st->fp))
	തിരികെ;

    വേണ്ടി (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    എങ്കിൽ (*p == NUL)
	തിരികെ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    എങ്കിൽ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	തിരികെ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    എങ്കിൽ (incstack[incstack_idx].etag_fname == ശൂന്യ)
	തിരികെ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ശൂന്യ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    എങ്കിൽ (fullpath_ebuf != ശൂന്യ)
    {
	st->fp = mch_fopen((അക്ഷരം *)fullpath_ebuf, "r");
	എങ്കിൽ (st->fp != ശൂന്യ)
	{
	    എങ്കിൽ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    എങ്കിൽ (st->fp == ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
emacs_tags_file_eof(findtags_state_T *st)
{
    എങ്കിൽ (!incstack_idx)	// reached end of file. stop processing.
	തിരികെ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    തിരികെ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
emacs_tags_parse_വരി(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    എങ്കിൽ (p_7f == ശൂന്യ)
    {
etag_fail:
	എങ്കിൽ (vim_strchr(lbuf, '\n') != ശൂന്യ)
	    തിരികെ FAIL;

	// Truncated line.  Ignore it.
	എങ്കിൽ (p_verbose >= 5)
	{
	    verbose_enter();
	    സന്ദേശം(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	തിരികെ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    എങ്കിൽ (p == ശൂന്യ)
	p = p_7f + 1;
    അല്ലെങ്കിൽ
	++p;

    എങ്കിൽ (!VIM_ISDIGIT(*p))	    // check for start of line number
	പോകുക etag_fail;
    tagp->command = p;

    എങ്കിൽ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    അല്ലെങ്കിൽ			    // second format: isolate tagname
    {
	എങ്കിൽ (p_7f == lbuf)
	    പോകുക etag_fail;

	// find end of tagname
	വേണ്ടി (p = p_7f - 1; !vim_iswordc(*p); --p)
	    എങ്കിൽ (p == lbuf)
		പോകുക etag_fail;
	tagp->tagname_end = p + 1;
	ആകുമ്പോൾ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    തിരികെ OK;
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
    സ്ഥിതം tags_read_status_T
findtags_get_next_വരി(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    പൂർണ്ണസംഖ്യ		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    എങ്കിൽ (st->state == TS_ദ്വിമാന)
    {
	offset = sinfo_p->താഴെയുള്ള_offset + ((sinfo_p->മുകളിലെ_offset
						- sinfo_p->താഴെയുള്ള_offset) / 2);
	എങ്കിൽ (offset == sinfo_p->curr_offset)
	    തിരികെ TAGS_READ_EOF; // End the binary search without a match.
	അല്ലെങ്കിൽ
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    അല്ലെങ്കിൽ എങ്കിൽ (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	എങ്കിൽ (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    എങ്കിൽ (st->state == TS_ദ്വിമാന || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	എങ്കിൽ (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    എങ്കിൽ (sinfo_p->curr_offset == sinfo_p->മുകളിലെ_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->താഴെയുള്ള_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->താഴെയുള്ള_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	ആകുമ്പോൾ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	എങ്കിൽ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    തിരികെ TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    അല്ലെങ്കിൽ
    {
	// skip empty and blank lines
	ചെയ്യുക
	{
#ifdef FEAT_CSCOPE
	    എങ്കിൽ (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    അല്ലെങ്കിൽ
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} ആകുമ്പോൾ (!eof && vim_isblankline(st->lbuf));

	എങ്കിൽ (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    എങ്കിൽ (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		തിരികെ TAGS_READ_IGNORE;
#endif
	    തിരികെ TAGS_READ_EOF;
	}
    }

    തിരികെ TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    എങ്കിൽ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	തിരികെ TRUE;

    // Process the header line.
    എങ്കിൽ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    എങ്കിൽ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	വേണ്ടി (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    തിരികെ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_start_state_handler(
    findtags_state_T	*st,
    പൂർണ്ണസംഖ്യ			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    പൂർണ്ണസംഖ്യ		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    എങ്കിൽ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	തിരികെ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    എങ്കിൽ (st->linear || use_cscope)
#അല്ലെങ്കിൽ
    എങ്കിൽ (st->linear)
#endif
	st->state = TS_LINEAR;
    അല്ലെങ്കിൽ എങ്കിൽ (st->tag_file_sorted == NUL)
	st->state = TS_ദ്വിമാന;
    അല്ലെങ്കിൽ എങ്കിൽ (st->tag_file_sorted == '1')
	st->state = TS_ദ്വിമാന;
    അല്ലെങ്കിൽ എങ്കിൽ (st->tag_file_sorted == '2')
    {
	st->state = TS_ദ്വിമാന;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    അല്ലെങ്കിൽ
	st->state = TS_LINEAR;

    എങ്കിൽ (st->state == TS_ദ്വിമാന && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    എങ്കിൽ (st->state == TS_ദ്വിമാന)
    {
	എങ്കിൽ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	അല്ലെങ്കിൽ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->താഴെയുള്ള_offset = 0;
	    sinfo_p->താഴെയുള്ള_char = 0;
	    sinfo_p->മുകളിലെ_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->മുകളിലെ_char = 0xff;
	}
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
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
    സ്ഥിതം tagmatch_status_T
findtags_parse_വരി(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    പൂർണ്ണസംഖ്യ		status;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		cmplen;
    പൂർണ്ണസംഖ്യ		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    എങ്കിൽ (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	എങ്കിൽ (tagpp->tagname_end == ശൂന്യ)
	    // Corrupted tag line.
	    തിരികെ TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (പൂർണ്ണസംഖ്യ)(tagpp->tagname_end - tagpp->tagname);
	എങ്കിൽ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	എങ്കിൽ ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	അല്ലെങ്കിൽ എങ്കിൽ (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    തിരികെ TAG_MATCH_NEXT;

	എങ്കിൽ (st->state == TS_ദ്വിമാന)
	{
	    // Simplistic check for unsorted tags file.
	    i = (പൂർണ്ണസംഖ്യ)tagpp->tagname[0];
	    എങ്കിൽ (margs->sortic)
		i = (പൂർണ്ണസംഖ്യ)TOUPPER_ASC(tagpp->tagname[0]);
	    എങ്കിൽ (i < sinfo_p->താഴെയുള്ള_char || i > sinfo_p->മുകളിലെ_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    എങ്കിൽ (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    അല്ലെങ്കിൽ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    എങ്കിൽ (tagcmp == 0)
	    {
		എങ്കിൽ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		അല്ലെങ്കിൽ എങ്കിൽ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    എങ്കിൽ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		തിരികെ TAG_MATCH_NEXT;
	    }
	    എങ്കിൽ (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		എങ്കിൽ (sinfo_p->curr_offset < sinfo_p->മുകളിലെ_offset)
		{
		    sinfo_p->താഴെയുള്ള_offset = sinfo_p->curr_offset;
		    എങ്കിൽ (margs->sortic)
			sinfo_p->താഴെയുള്ള_char = TOUPPER_ASC(tagpp->tagname[0]);
		    അല്ലെങ്കിൽ
			sinfo_p->താഴെയുള്ള_char = tagpp->tagname[0];
		    തിരികെ TAG_MATCH_NEXT;
		}
	    }
	    എങ്കിൽ (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->മുകളിലെ_offset)
	    {
		sinfo_p->മുകളിലെ_offset = sinfo_p->curr_offset;
		എങ്കിൽ (margs->sortic)
		    sinfo_p->മുകളിലെ_char = TOUPPER_ASC(tagpp->tagname[0]);
		അല്ലെങ്കിൽ
		    sinfo_p->മുകളിലെ_char = tagpp->tagname[0];
		തിരികെ TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    തിരികെ TAG_MATCH_STOP;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (st->state == TS_SKIP_BACK)
	{
	    എങ്കിൽ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    അല്ലെങ്കിൽ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    തിരികെ TAG_MATCH_NEXT;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (st->state == TS_STEP_FORWARD)
	{
	    എങ്കിൽ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		എങ്കിൽ ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    തിരികെ TAG_MATCH_STOP;	// past last match
		അല്ലെങ്കിൽ
		    തിരികെ TAG_MATCH_NEXT;	// before first match
	    }
	}
	അല്ലെങ്കിൽ
	    // skip this match if it can't match
	    എങ്കിൽ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		തിരികെ TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	എങ്കിൽ (tagpp->fname_end == ശൂന്യ)
	    status = FAIL;
	അല്ലെങ്കിൽ
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    അല്ലെങ്കിൽ
	status = parse_tag_വരി(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    എങ്കിൽ (status == FAIL)
	തിരികെ TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    തിരികെ TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    സ്ഥിതം ശൂന്യം
findtags_matchargs_init(findtags_match_args_T *margs, പൂർണ്ണസംഖ്യ flags)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    പൂർണ്ണസംഖ്യ		match = FALSE;
    പൂർണ്ണസംഖ്യ		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (പൂർണ്ണസംഖ്യ)(tagpp->tagname_end - tagpp->tagname);
    എങ്കിൽ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    എങ്കിൽ (st->orgpat->len != cmplen)
	match = FALSE;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    എങ്കിൽ (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	അല്ലെങ്കിൽ
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    എങ്കിൽ (!match && st->orgpat->regmatch.regprog != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	എങ്കിൽ (match)
	{
	    margs->matchoff = (പൂർണ്ണസംഖ്യ)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    എങ്കിൽ (st->orgpat->regmatch.rm_ic)
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

    തിരികെ match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    സ്ഥിതം ശൂന്യം
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_വരി;
    പൂർണ്ണസംഖ്യ		len;

    conv_വരി = string_convert(&st->vimconv, st->lbuf, ശൂന്യ);
    എങ്കിൽ (conv_വരി == ശൂന്യ)
	തിരികെ;

    // Copy or swap lbuf and conv_line.
    len = (പൂർണ്ണസംഖ്യ)STRLEN(conv_വരി) + 1;
    എങ്കിൽ (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_വരി;
	st->lbuf_size = len;
    }
    അല്ലെങ്കിൽ
    {
	STRCPY(st->lbuf, conv_വരി);
	vim_free(conv_വരി);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    പൂർണ്ണസംഖ്യ		name_only = (st->flags & TAG_NAMES);
    പൂർണ്ണസംഖ്യ		mtt;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		is_current;		// file name matches
    പൂർണ്ണസംഖ്യ		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    എങ്കിൽ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    അല്ലെങ്കിൽ
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
	എങ്കിൽ (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	എങ്കിൽ (is_static)
	{
	    എങ്കിൽ (is_current)
		mtt = MT_ST_CUR;
	    അല്ലെങ്കിൽ
		mtt = MT_ST_OTH;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (is_current)
		mtt = MT_GL_CUR;
	    അല്ലെങ്കിൽ
		mtt = MT_GL_OTH;
	}
	എങ്കിൽ (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	എങ്കിൽ (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    എങ്കിൽ (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#അല്ലെങ്കിൽ
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (പൂർണ്ണസംഖ്യ)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(വലുപ്പം(char_u) + len + 10 + ML_EXTRA + 1);
	എങ്കിൽ (mfp != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ heuristic;

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
	    sprintf((അക്ഷരം *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (name_only)
    {
	എങ്കിൽ (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    എങ്കിൽ (*temp_end == '/')
		ആകുമ്പോൾ (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    എങ്കിൽ (tagpp->command + 2 < temp_end)
	    {
		len = (പൂർണ്ണസംഖ്യ)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		എങ്കിൽ (mfp != ശൂന്യ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    അല്ലെങ്കിൽ
		mfp = ശൂന്യ;
	    st->get_searchpat = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    len = (പൂർണ്ണസംഖ്യ)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(വലുപ്പം(char_u) + len + 1);
	    എങ്കിൽ (mfp != ശൂന്യ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    എങ്കിൽ (State & MODE_തിരുകുക)
		st->get_searchpat = p_sft;
	}
    }
    അല്ലെങ്കിൽ
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
	len = (പൂർണ്ണസംഖ്യ)tag_fname_len + (പൂർണ്ണസംഖ്യ)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	എങ്കിൽ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (പൂർണ്ണസംഖ്യ)ebuf_len + 1;
	}
	അല്ലെങ്കിൽ
	    ++len;
#endif
	mfp = alloc(വലുപ്പം(char_u) + len + 1);
	എങ്കിൽ (mfp != ശൂന്യ)
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
	    എങ്കിൽ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    അല്ലെങ്കിൽ
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    എങ്കിൽ (mfp != ശൂന്യ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	എങ്കിൽ (use_cscope)
	    ++*hash;
	അല്ലെങ്കിൽ
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	എങ്കിൽ (HASHITEM_EMPTY(hi))
	{
	    എങ്കിൽ (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		തിരികെ FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_എണ്ണം++;
	}
	അല്ലെങ്കിൽ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    തിരികെ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    സ്ഥിതം ശൂന്യം
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	തിരയൽ_info;
    പൂർണ്ണസംഖ്യ			retval;
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(തിരയൽ_info);

    // Read and parse the lines in the file one by one
    വേണ്ടി (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	എങ്കിൽ (st->state == TS_ദ്വിമാന || st->state == TS_SKIP_BACK)
	    വരി_breakcheck();
	അല്ലെങ്കിൽ
	    fast_breakcheck();
	എങ്കിൽ ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_താക്കോലുകൾ(30, FALSE);
	എങ്കിൽ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    നിർത്തുക;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	എങ്കിൽ (st->mincount == TAG_MANY && st->match_എണ്ണം >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    നിർത്തുക;
	}
	എങ്കിൽ (st->get_searchpat)
	    പോകുക വരി_read_in;

	retval = findtags_get_next_വരി(st, &തിരയൽ_info);
	എങ്കിൽ (retval == TAGS_READ_IGNORE)
	    തുടരുക;
	എങ്കിൽ (retval == TAGS_READ_EOF)
	    നിർത്തുക;

വരി_read_in:

	എങ്കിൽ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	എങ്കിൽ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    തുടരുക;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	എങ്കിൽ (st->state == TS_START)
	{
	    എങ്കിൽ (findtags_start_state_handler(st, &margs->sortic, &തിരയൽ_info) == FALSE)
		തുടരുക;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	എങ്കിൽ (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    എങ്കിൽ (st->lbuf == ശൂന്യ)
	    {
		എങ്കിൽ (st->fp != ശൂന്യ)
		    fclose(st->fp);
		st->fp = ശൂന്യ;
		st->stop_searching = TRUE;
		തിരികെ;
	    }

	    എങ്കിൽ (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, തിരയൽ_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    തിരയൽ_info.curr_offset = 0;
	    തുടരുക;
	}

	retval = findtags_parse_വരി(st, &tagp, margs, &തിരയൽ_info);
	എങ്കിൽ (retval == TAG_MATCH_NEXT)
	    തുടരുക;
	എങ്കിൽ (retval == TAG_MATCH_STOP)
	    നിർത്തുക;
	എങ്കിൽ (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    എങ്കിൽ (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    തിരികെ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	എങ്കിൽ (findtags_match_tag(st, &tagp, margs))
	{
	    എങ്കിൽ (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		നിർത്തുക;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    സ്ഥിതം ശൂന്യം
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = ശൂന്യ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    എങ്കിൽ (use_cscope)
	st->fp = ശൂന്യ;	    // avoid GCC warning
    അല്ലെങ്കിൽ
#endif
    {
#ifdef FEAT_MULTI_LANG
	എങ്കിൽ (curbuf->b_help)
	{
	    എങ്കിൽ (!findtags_in_help_init(st))
		തിരികെ;
	}
#endif

	st->fp = mch_fopen((അക്ഷരം *)st->tag_fname, "r");
	എങ്കിൽ (st->fp == ശൂന്യ)
	    തിരികെ;

	എങ്കിൽ (p_verbose >= 5)
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

    എങ്കിൽ (st->fp != ശൂന്യ)
    {
	fclose(st->fp);
	st->fp = ശൂന്യ;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    എങ്കിൽ (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, ശൂന്യ, ശൂന്യ);

    എങ്കിൽ (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    എങ്കിൽ (st->match_എണ്ണം >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    പൂർണ്ണസംഖ്യ		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    പൂർണ്ണസംഖ്യ		mtt;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*mfp;
    char_u	*p;

    എങ്കിൽ (st->match_എണ്ണം > 0)
	matches = ALLOC_MULT(char_u *, st->match_എണ്ണം);
    അല്ലെങ്കിൽ
	matches = ശൂന്യ;
    st->match_എണ്ണം = 0;
    വേണ്ടി (mtt = 0; mtt < MT_എണ്ണം; ++mtt)
    {
	വേണ്ടി (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    എങ്കിൽ (matches == ശൂന്യ)
		vim_free(mfp);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    വേണ്ടി (p = mfp + 1; *p != NUL; ++p)
			എങ്കിൽ (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_എണ്ണം++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    തിരികെ st->match_എണ്ണം;
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
    പൂർണ്ണസംഖ്യ
find_tags(
    char_u	*pat,			// pattern to search for
    പൂർണ്ണസംഖ്യ		*അക്കം_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    പൂർണ്ണസംഖ്യ		first_file;		// trying first tag file
    പൂർണ്ണസംഖ്യ		retval = FAIL;		// return value
    പൂർണ്ണസംഖ്യ		round;

    പൂർണ്ണസംഖ്യ		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    പൂർണ്ണസംഖ്യ		i;
    char_u	*saved_pat = ശൂന്യ;		// copy of pat[]
#endif

    പൂർണ്ണസംഖ്യ		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    പൂർണ്ണസംഖ്യ		has_re = (flags & TAG_REGEXP);	// regexp used
    പൂർണ്ണസംഖ്യ		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    പൂർണ്ണസംഖ്യ		use_cscope = (flags & TAG_CSCOPE);
#endif
    പൂർണ്ണസംഖ്യ		verbose = (flags & TAG_VERBOSE);
    പൂർണ്ണസംഖ്യ		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    തിരഞ്ഞെടുപ്പ് (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	അവസ്ഥ TC_FOLLOWIC:		 നിർത്തുക;
	അവസ്ഥ TC_IGNORE:    p_ic = TRUE;  നിർത്തുക;
	അവസ്ഥ TC_MATCH:     p_ic = FALSE; നിർത്തുക;
	അവസ്ഥ TC_FOLLOWSCS: p_ic = ignorecase(pat); നിർത്തുക;
	അവസ്ഥ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); നിർത്തുക;
    }

    help_save = curbuf->b_help;

    എങ്കിൽ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	പോകുക findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    എങ്കിൽ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    അല്ലെങ്കിൽ എങ്കിൽ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    എങ്കിൽ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	എങ്കിൽ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    എങ്കിൽ (saved_pat != ശൂന്യ)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    എങ്കിൽ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    എങ്കിൽ (has_re && st.orgpat->regmatch.regprog == ശൂന്യ)
	പോകുക findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    എങ്കിൽ (retval != NOTDONE)
	പോകുക findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    എങ്കിൽ ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == ശൂന്യ
	    && curbuf->b_fname != ശൂന്യ
	    && (i = (പൂർണ്ണസംഖ്യ)STRLEN(curbuf->b_fname)) > 4
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
    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      വേണ്ടി (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  എങ്കിൽ (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      നിർത്തുക;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	എങ്കിൽ (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	എങ്കിൽ (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    നിർത്തുക;
#ifdef FEAT_CSCOPE
	എങ്കിൽ (use_cscope)
	    നിർത്തുക;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    എങ്കിൽ (!st.stop_searching)
    {
	എങ്കിൽ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    എങ്കിൽ (retval == FAIL)
	st.match_എണ്ണം = 0;

    *അക്കം_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    തിരികെ retval;
}

സ്ഥിതം garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    സ്ഥിതം ശൂന്യം
found_tagfile_cb(char_u *fname, ശൂന്യം *cookie UNUSED)
{
    എങ്കിൽ (ga_grow(&tag_fnames, 1) == FAIL)
	തിരികെ;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_tag_stuff(ശൂന്യം)
{
    ga_clear_strings(&tag_fnames);
    എങ്കിൽ (curwin != ശൂന്യ)
	do_tag(ശൂന്യ, DT_FREE, 0, 0, 0);
    tag_freematch();

# എങ്കിൽ defined(FEAT_QUICKFIX)
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
    പൂർണ്ണസംഖ്യ
get_tagfname(
    tagname_T	*tnp,	// holds status info
    പൂർണ്ണസംഖ്യ		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ശൂന്യ;
    char_u		*r_ptr;
    പൂർണ്ണസംഖ്യ			i;

    എങ്കിൽ (first)
	CLEAR_POINTER(tnp);

    എങ്കിൽ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	എങ്കിൽ (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, വലുപ്പം(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# അല്ലെങ്കിൽ
		    "doc/tags doc/tags-??"
# endif
#അല്ലെങ്കിൽ
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, ശൂന്യ);
	}

	എങ്കിൽ (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    എങ്കിൽ (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		തിരികെ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    വേണ്ടി (i = 0; i < tag_fnames.ga_len; ++i)
		എങ്കിൽ (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    തിരികെ FAIL; // avoid duplicate file names
	}
	അല്ലെങ്കിൽ
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	തിരികെ OK;
    }

    എങ്കിൽ (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	എങ്കിൽ (tnp->tn_tags == ശൂന്യ)
	    തിരികെ FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    വേണ്ടി (;;)
    {
	എങ്കിൽ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_തിരയൽ_ctx);
	    എങ്കിൽ (fname != ശൂന്യ)
		നിർത്തുക;

	    tnp->tn_did_filefind_init = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    char_u  *filename = ശൂന്യ;

	    // Stop when used all parts of 'tags'.
	    എങ്കിൽ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_തിരയൽ_ctx);
		tnp->tn_തിരയൽ_ctx = ശൂന്യ;
		തിരികെ FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (ശൂന്യം)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    എങ്കിൽ (r_ptr != ശൂന്യ)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_തിരയൽ_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_തിരയൽ_ctx, TRUE, curbuf->b_ffname);
	    എങ്കിൽ (tnp->tn_തിരയൽ_ctx != ശൂന്യ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    തിരികെ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    ശൂന്യം
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_തിരയൽ_ctx);
    tnp->tn_തിരയൽ_ctx = ശൂന്യ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_tag_വരി(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    പൂർണ്ണസംഖ്യ		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (is_etag)
	// emacs-style tag file
	തിരികെ emacs_tags_parse_വരി(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    എങ്കിൽ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    എങ്കിൽ (*p != NUL)
	++p;
    എങ്കിൽ (*p == NUL)
	തിരികെ FAIL;
    tagp->command = p;

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ആകുമ്പോൾ ((p = vim_strchr(p, '\t')) != ശൂന്യ)
    {
	++p;
	എങ്കിൽ (STRNCMP(p, "file:", 5) == 0)
	    തിരികെ TRUE;
    }

    തിരികെ FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    സ്ഥിതം size_t
matching_വരി_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    തിരികെ (p - lbuf) + STRLEN(p);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    പൂർണ്ണസംഖ്യ		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    അല്ലെങ്കിൽ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_വരി(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = ശൂന്യ;
    tagp->user_data = ശൂന്യ;
    tagp->tagline = 0;
    tagp->command_end = ശൂന്യ;

    എങ്കിൽ (retval != OK)
	തിരികെ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    എങ്കിൽ (find_extra(&p) == OK)
    {
	എങ്കിൽ (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	അല്ലെങ്കിൽ
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	എങ്കിൽ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ആകുമ്പോൾ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		എങ്കിൽ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((അക്ഷരം *)p + 5);
		എങ്കിൽ (tagp->tagkind != ശൂന്യ && tagp->user_data != ശൂന്യ)
		    നിർത്തുക;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		എങ്കിൽ (pc == ശൂന്യ || (pt != ശൂന്യ && pc > pt))
		    tagp->tagkind = p;
		എങ്കിൽ (pt == ശൂന്യ)
		    നിർത്തുക;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    എങ്കിൽ (tagp->tagkind != ശൂന്യ)
    {
	വേണ്ടി (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    എങ്കിൽ (tagp->user_data != ശൂന്യ)
    {
	വേണ്ടി (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    തിരികെ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    സ്ഥിതം char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    പൂർണ്ണസംഖ്യ		c;

#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    അല്ലെങ്കിൽ
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    തിരികെ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    പൂർണ്ണസംഖ്യ		forceit,	// :ta with !
    പൂർണ്ണസംഖ്യ		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    പൂർണ്ണസംഖ്യ		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = ശൂന്യ;
    char_u	*fname;
    tagptrs_T	tagp;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    പൂർണ്ണസംഖ്യ		getfile_result = GETFILE_UNUSED;
    പൂർണ്ണസംഖ്യ		തിരയൽ_options;
#ifdef FEAT_തിരയൽ_EXTRA
    പൂർണ്ണസംഖ്യ		save_no_hlsearch;
#endif
#എങ്കിൽ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ശൂന്യ;
#endif
    char_u	*full_fname = ശൂന്യ;
#ifdef FEAT_FOLDING
    പൂർണ്ണസംഖ്യ		old_താക്കോൽ_Typed = താക്കോൽ_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    പൂർണ്ണസംഖ്യ		isdigit = FALSE;

    എങ്കിൽ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	തിരികെ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_വരി_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    എങ്കിൽ (lbuf != ശൂന്യ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    എങ്കിൽ (pbuf == ശൂന്യ || lbuf == ശൂന്യ || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = ശൂന്യ;
	പോകുക erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    എങ്കിൽ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    വേണ്ടി (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	എങ്കിൽ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    നിർത്തുക;
#endif
	*pbuf_end++ = *str++;
	എങ്കിൽ (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    നിർത്തുക;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    എങ്കിൽ (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	എങ്കിൽ (isdigit)
	    str++;
	എങ്കിൽ (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    എങ്കിൽ (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    അല്ലെങ്കിൽ
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    എങ്കിൽ (fname == ശൂന്യ)
	പോകുക erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    എങ്കിൽ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ശൂന്യ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	എങ്കിൽ (nofile_fname == ശൂന്യ)
	    nofile_fname = empty_option;
	പോകുക erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#എങ്കിൽ defined(FEAT_QUICKFIX)
    എങ്കിൽ (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	എങ്കിൽ (!curwin->w_p_pvw)
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
    എങ്കിൽ (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	എങ്കിൽ (existing_buf != ശൂന്യ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    എങ്കിൽ (swbuf_goto_win_with_buf(existing_buf) != ശൂന്യ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    എങ്കിൽ (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	എങ്കിൽ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	എങ്കിൽ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	എങ്കിൽ (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    എങ്കിൽ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    പോകുക erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    എങ്കിൽ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#എങ്കിൽ defined(FEAT_QUICKFIX)
	എങ്കിൽ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	അല്ലെങ്കിൽ
#endif
	    keep_help_flag = curbuf->b_help;
    }

    എങ്കിൽ (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, ശൂന്യ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    എങ്കിൽ (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_തിരയൽ_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	എങ്കിൽ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	എങ്കിൽ (vim_strchr(p_cpo, CPO_TAGPAT) != ശൂന്യ)
	    തിരയൽ_options = 0;
	അല്ലെങ്കിൽ
	    തിരയൽ_options = തിരയൽ_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	എങ്കിൽ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	എങ്കിൽ (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    എങ്കിൽ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    അല്ലെങ്കിൽ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    എങ്കിൽ (do_തിരയൽ(ശൂന്യ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 തിരയൽ_options, ശൂന്യ))
		retval = OK;
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ	found = 1;
		പൂർണ്ണസംഖ്യ	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		എങ്കിൽ (!do_തിരയൽ(ശൂന്യ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 തിരയൽ_options, ശൂന്യ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (ശൂന്യം)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((അക്ഷരം *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    എങ്കിൽ (!do_തിരയൽ(ശൂന്യ, '/', '/', pbuf, pbuflen, (long)1,
							 തിരയൽ_options, ശൂന്യ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((അക്ഷരം *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			എങ്കിൽ (!do_തിരയൽ(ശൂന്യ, '/', '/', pbuf, pbuflen, (long)1,
							 തിരയൽ_options, ശൂന്യ))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		എങ്കിൽ (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		അല്ലെങ്കിൽ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    എങ്കിൽ (found == 2 || !save_p_ic)
		    {
			സന്ദേശം(_(e_couldnt_find_tag_just_guessing));
			എങ്കിൽ (!സന്ദേശം_scrolled && സന്ദേശം_silent == 0)
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
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		save_secure = secure;

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
	    എങ്കിൽ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_തിരയൽ_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	എങ്കിൽ (തിരയൽ_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	എങ്കിൽ (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	എങ്കിൽ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    എങ്കിൽ (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    എങ്കിൽ ((fdo_flags & FDO_TAG) && old_താക്കോൽ_Typed)
		foldOpenCursor();
#endif
	}

#എങ്കിൽ defined(FEAT_QUICKFIX)
	എങ്കിൽ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	എങ്കിൽ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	എങ്കിൽ (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#എങ്കിൽ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	അല്ലെങ്കിൽ എങ്കിൽ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    എങ്കിൽ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#എങ്കിൽ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    എങ്കിൽ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#എങ്കിൽ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    തിരികെ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    സ്ഥിതം char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, പൂർണ്ണസംഖ്യ expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ശൂന്യ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    എങ്കിൽ (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       തിരികെ ശൂന്യ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    എങ്കിൽ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ശൂന്യ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ശൂന്യ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	എങ്കിൽ (expanded_fname != ശൂന്യ)
	    fname = expanded_fname;
    }

    എങ്കിൽ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	എങ്കിൽ (retval != ശൂന്യ)
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
    അല്ലെങ്കിൽ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    തിരികെ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_for_current(
#ifdef FEAT_EMACS_TAGS
    പൂർണ്ണസംഖ്യ	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    പൂർണ്ണസംഖ്യ	    c;
    പൂർണ്ണസംഖ്യ	    retval = FALSE;
    char_u  *fullname;

    എങ്കിൽ (buf_ffname != ശൂന്യ)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	എങ്കിൽ (is_etag)
	    c = 0;	    // to shut up GCC
	അല്ലെങ്കിൽ
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	എങ്കിൽ (fullname != ശൂന്യ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	എങ്കിൽ (!is_etag)
#endif
	    *fname_end = c;
    }

    തിരികെ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    വേണ്ടി (;;)
    {
	എങ്കിൽ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	അല്ലെങ്കിൽ എങ്കിൽ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    എങ്കിൽ (*str != first_char)
		str = ശൂന്യ;
	    അല്ലെങ്കിൽ
		++str;
	}
	അല്ലെങ്കിൽ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((അക്ഷരം *)str, "|;\"");
	    എങ്കിൽ (str != ശൂന്യ)
	    {
		++str;
		നിർത്തുക;
	    }

	}
	എങ്കിൽ (str == ശൂന്യ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    നിർത്തുക;
	++str;	// skip ';'
	first_char = *str;
    }

    എങ്കിൽ (str != ശൂന്യ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	തിരികെ OK;
    }
    തിരികെ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    ശൂന്യം
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    പൂർണ്ണസംഖ്യ
expand_tags(
    പൂർണ്ണസംഖ്യ		tagnames,	// expand tag names
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		*അക്കം_file,
    char_u	***file)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    പൂർണ്ണസംഖ്യ		ret;

    name_buf = alloc(name_buf_size);
    എങ്കിൽ (name_buf == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (tagnames)
	extra_flag = TAG_NAMES;
    അല്ലെങ്കിൽ
	extra_flag = 0;
    എങ്കിൽ (pat[0] == '/')
	ret = find_tags(pat + 1, അക്കം_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    അല്ലെങ്കിൽ
	ret = find_tags(pat, അക്കം_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    എങ്കിൽ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 വേണ്ടി (i = 0; i < *അക്കം_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     എങ്കിൽ (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 എങ്കിൽ (buf == ശൂന്യ)
		 {
		     vim_free(name_buf);
		     തിരികെ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ശൂന്യ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    തിരികെ ret;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_tag_field(
    dict_T  *dict,
    അക്ഷരം    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		retval;

    // check that the field name doesn't exist yet
    എങ്കിൽ (dict_has_താക്കോൽ(dict, field_name))
    {
	എങ്കിൽ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	തിരികെ FAIL;
    }
    buf = alloc(MAXPATHL);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (start != ശൂന്യ)
    {
	എങ്കിൽ (end == ശൂന്യ)
	{
	    end = start + STRLEN(start);
	    ആകുമ്പോൾ (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (പൂർണ്ണസംഖ്യ)(end - start);
	എങ്കിൽ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    തിരികെ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    പൂർണ്ണസംഖ്യ
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    പൂർണ്ണസംഖ്യ		അക്കം_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &അക്കം_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (പൂർണ്ണസംഖ്യ)MAXCOL, buf_fname);
    എങ്കിൽ (ret != OK || അക്കം_matches <= 0)
	തിരികെ ret;

    വേണ്ടി (i = 0; i < അക്കം_matches; ++i)
    {
	എങ്കിൽ (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    തുടരുക;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	എങ്കിൽ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    തുടരുക;
	}

	എങ്കിൽ ((dict = dict_alloc()) == ശൂന്യ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    നിർത്തുക;
	}
	എങ്കിൽ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    തുടരുക;
	}

	full_fname = tag_full_fname(&tp);
	എങ്കിൽ (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    ശൂന്യ) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_സംഖ്യ(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	എങ്കിൽ (tp.command_end != ശൂന്യ)
	{
	    വേണ്ടി (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		എങ്കിൽ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		അല്ലെങ്കിൽ എങ്കിൽ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    പൂർണ്ണസംഖ്യ	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ആകുമ്പോൾ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (പൂർണ്ണസംഖ്യ)(p - n);
		    എങ്കിൽ (*p == ':' && len > 0)
		    {
			s = ++p;
			ആകുമ്പോൾ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			എങ്കിൽ (add_tag_field(dict, (അക്ഷരം *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    അല്ലെങ്കിൽ
			// Skip field without colon.
			ആകുമ്പോൾ (*p != NUL && *p >= ' ')
			    ++p;
		    എങ്കിൽ (*p == NUL)
			നിർത്തുക;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    തിരികെ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    സ്ഥിതം ശൂന്യം
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_സംഖ്യ(retdict, "matchnr", tag->cur_match + 1);
    dict_add_സംഖ്യ(retdict, "bufnr", tag->cur_fnum);
    എങ്കിൽ (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    എങ്കിൽ ((pos = list_alloc_id(aid_tagstack_from)) == ശൂന്യ)
	തിരികെ;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_സംഖ്യ(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_സംഖ്യ(pos, (varnumber_T)fmark->mark.lnum);
    list_append_സംഖ്യ(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_സംഖ്യ(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    ശൂന്യം
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    പൂർണ്ണസംഖ്യ		i;
    dict_T	*d;

    dict_add_സംഖ്യ(retdict, "length", wp->w_tagstacklen);
    dict_add_സംഖ്യ(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_ഇനങ്ങൾ);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;
    dict_add_list(retdict, "items", l);

    വേണ്ടി (i = 0; i < wp->w_tagstacklen; i++)
    {
	എങ്കിൽ ((d = dict_alloc_id(aid_tagstack_details)) == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    തിരികെ;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    സ്ഥിതം ശൂന്യം
tagstack_clear(win_T *wp)
{
    പൂർണ്ണസംഖ്യ i;

    // Free the current tag stack
    വേണ്ടി (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    സ്ഥിതം ശൂന്യം
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    പൂർണ്ണസംഖ്യ		i;

    tagstack_clear_entry(&tagstack[0]);
    വേണ്ടി (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    സ്ഥിതം ശൂന്യം
tagstack_തള്ളുക_item(
	win_T	*wp,
	char_u	*tagname,
	പൂർണ്ണസംഖ്യ	cur_fnum,
	പൂർണ്ണസംഖ്യ	cur_match,
	pos_T	mark,
	പൂർണ്ണസംഖ്യ	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    പൂർണ്ണസംഖ്യ		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    എങ്കിൽ (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    എങ്കിൽ (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    സ്ഥിതം ശൂന്യം
tagstack_തള്ളുക_ഇനങ്ങൾ(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    പൂർണ്ണസംഖ്യ		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ശൂന്യ)
	    തുടരുക;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	എങ്കിൽ ((di = dict_find(itemdict, (char_u *)"from", -1)) == ശൂന്യ)
	    തുടരുക;
	എങ്കിൽ (list2fpos(&di->di_tv, &mark, &fnum, ശൂന്യ, FALSE) != OK)
	    തുടരുക;
	എങ്കിൽ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (mark.col > 0)
	    mark.col--;
	tagstack_തള്ളുക_item(wp, tagname,
		(പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(itemdict, "bufnr"),
		(പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    സ്ഥിതം ശൂന്യം
tagstack_set_curidx(win_T *wp, പൂർണ്ണസംഖ്യ curidx)
{
    wp->w_tagstackidx = curidx;
    എങ്കിൽ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    എങ്കിൽ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    പൂർണ്ണസംഖ്യ
set_tagstack(win_T *wp, dict_T *d, പൂർണ്ണസംഖ്യ action)
{
    dictitem_T	*di;
    list_T	*l = ശൂന്യ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    എങ്കിൽ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_അട്ടി_within_tagfunc));
	തിരികെ FAIL;
    }
# endif

    എങ്കിൽ ((di = dict_find(d, (char_u *)"items", -1)) != ശൂന്യ)
    {
	എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    തിരികെ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    എങ്കിൽ ((di = dict_find(d, (char_u *)"curidx", -1)) != ശൂന്യ)
	tagstack_set_curidx(wp, (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&di->di_tv) - 1);

    എങ്കിൽ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	പൂർണ്ണസംഖ്യ	tagstackidx = wp->w_tagstackidx;
	പൂർണ്ണസംഖ്യ	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ആകുമ്പോൾ (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    എങ്കിൽ (l != ശൂന്യ)
    {
	എങ്കിൽ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_തള്ളുക_ഇനങ്ങൾ(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    തിരികെ OK;
}
#endif
