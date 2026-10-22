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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ tag_pointers
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
    ಪೂರ್ಣಾಙ್ಕ		is_etag;	// TRUE for emacs tag
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_ದ್ವಿಮಾನ,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    off_T	ಕೆಳಗಿನ_offset;	// offset for first char of first line that
				// could match
    off_T	ಮೇಲಿನ_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    ಪೂರ್ಣಾಙ್ಕ	ಕೆಳಗಿನ_char;		// first char at low_offset
    ಪೂರ್ಣಾಙ್ಕ	ಮೇಲಿನ_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ	matchoff;		// tag match offset
    ಪೂರ್ಣಾಙ್ಕ	match_re;		// TRUE if the tag matches a regexp
    ಪೂರ್ಣಾಙ್ಕ	match_no_ic;		// TRUE if the tag matches with case
    ಪೂರ್ಣಾಙ್ಕ	has_re;			// regular expression used
    ಪೂರ್ಣಾಙ್ಕ	sortic;			// tags file sorted ignoring case (foldcase)
    ಪೂರ್ಣಾಙ್ಕ	sort_error;		// tags file not sorted
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
#define MT_ಎಣಿಕೆ	16

ಸ್ಥಿರ ವರ್ಣ	*mt_names[MT_ಎಣಿಕೆ/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
ಸ್ಥಿರ char_u	*nofile_fname = ರಿಕ್ತ;	// fname for NOTAGFILE error

ಸ್ಥಿರ ಶೂನ್ಯ taglen_advance(ಪೂರ್ಣಾಙ್ಕ l);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ jumpto_tag(char_u *lbuf, ಪೂರ್ಣಾಙ್ಕ forceit, ಪೂರ್ಣಾಙ್ಕ keep_help);
#ifdef FEAT_EMACS_TAGS
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_tag_ಸಾಲು(char_u *lbuf, ಪೂರ್ಣಾಙ್ಕ is_etag, tagptrs_T *tagp);
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_tag_ಸಾಲು(char_u *lbuf, tagptrs_T *tagp);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ test_for_static(tagptrs_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_match(char_u *lbuf, tagptrs_T *tagp);
ಸ್ಥಿರ char_u *tag_full_fname(tagptrs_T *tagp);
ಸ್ಥಿರ char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, ಪೂರ್ಣಾಙ್ಕ expand);
#ifdef FEAT_EMACS_TAGS
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ test_for_current(ಪೂರ್ಣಾಙ್ಕ, char_u *, char_u *, char_u *, char_u *);
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_extra(char_u **pp);
ಸ್ಥಿರ ಶೂನ್ಯ print_tag_list(ಪೂರ್ಣಾಙ್ಕ new_tag, ಪೂರ್ಣಾಙ್ಕ use_tagstack, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_matches, char_u **matches);
#ಯದಿ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_llist_tags(char_u *tag, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_matches, char_u **matches);
#endif

ಸ್ಥಿರ char_u	*tagmatchname = ರಿಕ್ತ;	// name of last used tag

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ಸ್ಥಿರ taggy_T ptag_entry = {ರಿಕ್ತ, {{0, 0, 0}, 0}, 0, 0, ರಿಕ್ತ};
#endif

#ifdef FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
ಸ್ಥಿರ callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#ಯದಿ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    ವರ್ಣ *
did_set_tagfunc(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ	retval;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    ಅನ್ಯಥಾ
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	ಯದಿ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    ಪ್ರತ್ಯಾಗಮ retval == FAIL ? e_invalid_argument : ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_tagfunc_option(ಶೂನ್ಯ)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_tagfunc(ಪೂರ್ಣಾಙ್ಕ copyID UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    ಶೂನ್ಯ
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    ಯದಿ (tfu_cb.cb_name != ರಿಕ್ತ && *tfu_cb.cb_name != NUL)
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
    ಪೂರ್ಣಾಙ್ಕ
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    ಪೂರ್ಣಾಙ್ಕ		type,
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		forceit,	// :ta with !
    ಪೂರ್ಣಾಙ್ಕ		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಙ್ಕ		tagstacklen = curwin->w_tagstacklen;
    ಪೂರ್ಣಾಙ್ಕ		cur_match = 0;
    ಪೂರ್ಣಾಙ್ಕ		cur_fnum = curbuf->b_fnum;
    ಪೂರ್ಣಾಙ್ಕ		oldtagstackidx = tagstackidx;
    ಪೂರ್ಣಾಙ್ಕ		prevtagstackidx = tagstackidx;
    ಪೂರ್ಣಾಙ್ಕ		prev_ಅಂಕೆ_matches;
    ಪೂರ್ಣಾಙ್ಕ		new_tag = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ic;
    ಪೂರ್ಣಾಙ್ಕ		no_regexp = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error_cur_match = 0;
    ಪೂರ್ಣಾಙ್ಕ		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		jumped_to_tag = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		new_ಅಂಕೆ_matches;
    char_u	**new_matches;
    ಪೂರ್ಣಾಙ್ಕ		use_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		skip_ಸಂದೇಶ = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    ಪೂರ್ಣಾಙ್ಕ		use_tfu = 1;
    char_u	*tofree = ರಿಕ್ತ;

    // remember the matches for the last used tag
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		max_ಅಂಕೆ_matches = 0;  // limit used for match search
    ಸ್ಥಿರ char_u	**matches = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		flags;

#ifdef EXITFREE
    ಯದಿ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(ಅಂಕೆ_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	ಅಂಕೆ_matches = 0;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif

#ifdef FEAT_EVAL
    ಯದಿ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ರಾಶಿ_within_tagfunc));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif

    ಯದಿ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_ಅಂಕೆ_matches = ಅಂಕೆ_matches;
    free_string_option(nofile_fname);
    nofile_fname = ರಿಕ್ತ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ಯದಿ ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#ಯದಿ defined(FEAT_QUICKFIX)
	ಯದಿ (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    ಯದಿ ((ptag_entry.tagname = vim_strsave(tag)) == ರಿಕ್ತ)
		ಗಚ್ಛ end_do_tag;
	}
#endif
    }
    ಅನ್ಯಥಾ
    {
#ಯದಿ defined(FEAT_QUICKFIX)
	ಯದಿ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ಅನ್ಯಥಾ
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ಯದಿ (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#ಯದಿ defined(FEAT_QUICKFIX)
	    ಯದಿ (g_do_tagpreview != 0)
	    {
		ಯದಿ (ptag_entry.tagname != ರಿಕ್ತ
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ಅನ್ಯಥಾ
		{
		    tagstack_clear_entry(&ptag_entry);
		    ಯದಿ ((ptag_entry.tagname = vim_strsave(tag)) == ರಿಕ್ತ)
			ಗಚ್ಛ end_do_tag;
		}
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ಯಾವತ್ (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ಯದಿ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    ಪ್ರತಿ (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = ರಿಕ್ತ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ಯದಿ ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == ರಿಕ್ತ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    ಗಚ್ಛ end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (
#ಯದಿ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == ರಿಕ್ತ :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_ರಾಶಿ_empty));
		ಗಚ್ಛ end_do_tag;
	    }

	    ಯದಿ (type == DT_ತೆಗೆ)		// go to older position
	    {
#ifdef FEAT_FOLDING
		ಪೂರ್ಣಾಙ್ಕ	old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#endif
		ಯದಿ ((tagstackidx -= ಎಣಿಕೆ) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_ರಾಶಿ));
		    ಯದಿ (tagstackidx + ಎಣಿಕೆ == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			ಗಚ್ಛ end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ಅನ್ಯಥಾ ಯದಿ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_ತುದಿ_of_tag_ರಾಶಿ));
		    ಗಚ್ಛ end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ಯದಿ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ಯದಿ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			ಗಚ್ಛ end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ಅನ್ಯಥಾ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		ಯದಿ ((fdo_flags & FDO_TAG) && old_ಕೀಲಿ_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(ಅಂಕೆ_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		ಅಂಕೆ_matches = 0;
		tag_freematch();
		ಗಚ್ಛ end_do_tag;
	    }

	    ಯದಿ (type == DT_TAG
#ಯದಿ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ಅನ್ಯಥಾ
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ಯದಿ ((tagstackidx += ಎಣಿಕೆ - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_ತುದಿ_of_tag_ರಾಶಿ));
			save_pos = FALSE;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_ರಾಶಿ));
			tagstackidx = 0;
			ಗಚ್ಛ end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    ಅನ್ಯಥಾ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ಯದಿ defined(FEAT_QUICKFIX)
		ಯದಿ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ಅನ್ಯಥಾ
#endif
		{
		    ಯದಿ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		ವಿಕಲ್ಪ (type)
		{
		    ಪಕ್ಷ DT_FIRST: cur_match = ಎಣಿಕೆ - 1; ಭಙ್ಗ;
		    ಪಕ್ಷ DT_SELECT:
		    ಪಕ್ಷ DT_JUMP:
#ifdef FEAT_CSCOPE
		    ಪಕ್ಷ DT_CSCOPE:
#endif
		    ಪಕ್ಷ DT_LAST:  cur_match = MAXCOL - 1; ಭಙ್ಗ;
		    ಪಕ್ಷ DT_NEXT:  cur_match += ಎಣಿಕೆ; ಭಙ್ಗ;
		    ಪಕ್ಷ DT_PREV:  cur_match -= ಎಣಿಕೆ; ಭಙ್ಗ;
		}
		ಯದಿ (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		ಅನ್ಯಥಾ ಯದಿ (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_ಸಂದೇಶ = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ಯದಿ defined(FEAT_QUICKFIX)
	ಯದಿ (g_do_tagpreview != 0)
	{
	    ಯದಿ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	ಅನ್ಯಥಾ
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ಯದಿ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ಯದಿ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ಯದಿ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ಯದಿ (buf != ರಿಕ್ತ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ಯದಿ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ಯದಿ defined(FEAT_QUICKFIX)
	ಅನ್ಯಥಾ ಯದಿ (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	ಅನ್ಯಥಾ
	    name = tag;
	other_name = (tagmatchname == ರಿಕ್ತ || STRCMP(tagmatchname, name) != 0);
	ಯದಿ (new_tag
		|| (cur_match >= ಅಂಕೆ_matches && max_ಅಂಕೆ_matches != MAXCOL)
		|| other_name)
	{
	    ಯದಿ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ಯದಿ (type == DT_SELECT || type == DT_JUMP
#ಯದಿ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    ಯದಿ (type == DT_TAG)
		max_ಅಂಕೆ_matches = MAXCOL;
	    ಅನ್ಯಥಾ
		max_ಅಂಕೆ_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    ಯದಿ (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    ಅನ್ಯಥಾ
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    ಯದಿ (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    ಯದಿ (verbose)
		flags |= TAG_VERBOSE;

	    ಯದಿ (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    ಯದಿ (find_tags(name, &new_ಅಂಕೆ_matches, &new_matches, flags,
					    max_ಅಂಕೆ_matches, buf_ffname) == OK
		    && new_ಅಂಕೆ_matches < max_ಅಂಕೆ_matches)
		max_ಅಂಕೆ_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ಯದಿ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_ಅಂಕೆ_matches, new_matches);
		ಭಙ್ಗ;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ಯದಿ (!new_tag && !other_name)
	    {
		ಪೂರ್ಣಾಙ್ಕ	    j, k;
		ಪೂರ್ಣಾಙ್ಕ	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		ಪ್ರತಿ (j = 0; j < ಅಂಕೆ_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    ಪ್ರತಿ (i = idx; i < new_ಅಂಕೆ_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			ಯದಿ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    ಪ್ರತಿ (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ಭಙ್ಗ;
			}
		    }
		}
	    }
	    FreeWild(ಅಂಕೆ_matches, matches);
	    ಅಂಕೆ_matches = new_ಅಂಕೆ_matches;
	    matches = new_matches;
	}

	ಯದಿ (ಅಂಕೆ_matches <= 0)
	{
	    ಯದಿ (verbose)
		semsg(_(e_tag_not_found_str), name);
#ಯದಿ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ ask_for_selection = FALSE;
#ಯದಿ defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    ಯದಿ (type == DT_CSCOPE && ಅಂಕೆ_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    ಅನ್ಯಥಾ
#endif
	    ಯದಿ (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = ಎಣಿಕೆ > 0 ? ಎಣಿಕೆ - 1 : 0;
	    ಅನ್ಯಥಾ ಯದಿ (type == DT_SELECT || (type == DT_JUMP && ಅಂಕೆ_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, ಅಂಕೆ_matches, matches);
		ask_for_selection = TRUE;
	    }
#ಯದಿ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ಅನ್ಯಥಾ ಯದಿ (type == DT_LTAG)
	    {
		ಯದಿ (add_llist_tags(tag, ಅಂಕೆ_matches, matches) == FAIL)
		    ಗಚ್ಛ end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    ಯದಿ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_ಸಂಖ್ಯೆ(ರಿಕ್ತ);
		ಯದಿ (i <= 0 || i > ಅಂಕೆ_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ಯದಿ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    ಭಙ್ಗ;
		}
		cur_match = i - 1;
	    }

	    ಯದಿ (cur_match >= ಅಂಕೆ_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ಯದಿ ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == ರಿಕ್ತ)
		{
		    ಯದಿ (ಅಂಕೆ_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    ಅನ್ಯಥಾ
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_ಸಂದೇಶ = TRUE;
		}
		cur_match = ಅಂಕೆ_matches - 1;
	    }
	    ಯದಿ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ಯದಿ (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#ಯದಿ defined(FEAT_QUICKFIX)
	    ಅನ್ಯಥಾ ಯದಿ (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ಯದಿ (nofile_fname != ರಿಕ್ತ && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    ಯದಿ (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (ಅಂಕೆ_matches > 1 || ic)
		&& !skip_ಸಂದೇಶ)
	    {
		// Give an indication of the number of matching tags
		sprintf((ವರ್ಣ *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				ಅಂಕೆ_matches,
				max_ಅಂಕೆ_matches != MAXCOL ? _(" or more") : "");
		ಯದಿ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ಯದಿ ((ಅಂಕೆ_matches > prev_ಅಂಕೆ_matches || new_tag)
							   && ಅಂಕೆ_matches > 1)
		{
		    ಯದಿ (ic)
			ಸಂದೇಶ_attr((ವರ್ಣ *)IObuff, HL_ATTR(HLF_W));
		    ಅನ್ಯಥಾ
			ಸಂದೇಶ((ವರ್ಣ *)IObuff);
		    ಸಂದೇಶ_scroll = TRUE;	// don't overwrite this message
		}
		ಅನ್ಯಥಾ
		    give_warning(IObuff, ic);
		ಯದಿ (ic && !ಸಂದೇಶ_scrolled && ಸಂದೇಶ_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ಯದಿ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (ಪೂರ್ಣಾಙ್ಕ)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#ಯದಿ defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, ರಿಕ್ತ, -1);
#endif

	    ಯದಿ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ಯದಿ ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_ಅಂಕೆ_matches != MAXCOL
					     || cur_match < ಅಂಕೆ_matches - 1)))
		{
		    error_cur_match = cur_match;
		    ಯದಿ (use_tagstack)
			--tagstackidx;
		    ಯದಿ (type == DT_PREV)
			--cur_match;
		    ಅನ್ಯಥಾ
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    ಅನುವರ್ತ;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ಅನ್ಯಥಾ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ಯದಿ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	ಭಙ್ಗ;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    ಯದಿ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    ಪ್ರತ್ಯಾಗಮ jumped_to_tag;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_tag_list(
    ಪೂರ್ಣಾಙ್ಕ		new_tag,
    ಪೂರ್ಣಾಙ್ಕ		use_tagstack,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    ಪೂರ್ಣಾಙ್ಕ		taglen;
    ಪೂರ್ಣಾಙ್ಕ		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (ಪೂರ್ಣಾಙ್ಕ)(tagp.tagname_end - tagp.tagname + 2);
    ಯದಿ (taglen < 18)
	taglen = 18;
    ಯದಿ (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    ಯದಿ (ಸಂದೇಶ_col == 0)
	ಸಂದೇಶ_didout = FALSE;	// overwrite previous message
    ಸಂದೇಶ_start();
    ಸಂದೇಶ_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    ಸಂದೇಶ_clr_eos();
    taglen_advance(taglen);
    ಸಂದೇಶ_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	ಯದಿ (!new_tag && (
#ಯದಿ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	ಅನ್ಯಥಾ
	    *IObuff = ' ';
	vim_snprintf((ವರ್ಣ *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	ಸಂದೇಶ_puts((ವರ್ಣ *)IObuff);
	ಯದಿ (tagp.tagkind != ರಿಕ್ತ)
	    ಸಂದೇಶ_outtrans_len(tagp.tagkind,
			  (ಪೂರ್ಣಾಙ್ಕ)(tagp.tagkind_end - tagp.tagkind));
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outtrans_len_attr(tagp.tagname,
			   (ಪೂರ್ಣಾಙ್ಕ)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	ಸಂದೇಶ_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಸಂದೇಶ_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ಯದಿ (ಸಂದೇಶ_col > 0)
	    ಸಂದೇಶ_putchar('\n');
	ಯದಿ (got_int)
	    ಭಙ್ಗ;
	ಸಂದೇಶ_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	ಯದಿ (command_end != ರಿಕ್ತ)
	{
	    p = command_end + 3;
	    ಯಾವತ್ (*p && *p != '\r' && *p != '\n')
	    {
		ಯಾವತ್ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ಯದಿ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ಅನುವರ್ತ;
		}
		// skip "kind:<kind>" and "<kind>"
		ಯದಿ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    ಅನುವರ್ತ;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ಯಾವತ್ (*p && *p != '\r' && *p != '\n')
		{
		    ಯದಿ (ಸಂದೇಶ_col + ptr2cells(p) >= cmdline_width)
		    {
			ಸಂದೇಶ_putchar('\n');
			ಯದಿ (got_int)
			    ಭಙ್ಗ;
			ಸಂದೇಶ_advance(15);
		    }
		    p = ಸಂದೇಶ_outtrans_one(p, attr);
		    ಯದಿ (*p == TAB)
		    {
			ಸಂದೇಶ_puts_attr(" ", attr);
			ಭಙ್ಗ;
		    }
		    ಯದಿ (*p == ':')
			attr = 0;
		}
	    }
	    ಯದಿ (ಸಂದೇಶ_col > 15)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
		ಸಂದೇಶ_advance(15);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ಯದಿ (*p == '/' || *p == '?')
	{
	    ++p;
	    ಯದಿ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ಯಾವತ್ (p != command_end && vim_isspace(*p))
	    ++p;

	ಯಾವತ್ (p != command_end)
	{
	    ಯದಿ (ಸಂದೇಶ_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    ಸಂದೇಶ_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ಯದಿ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ಯದಿ (*p == TAB)
	    {
		ಸಂದೇಶ_putchar(' ');
		++p;
	    }
	    ಅನ್ಯಥಾ
		p = ಸಂದೇಶ_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ಯದಿ (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ಭಙ್ಗ;
	    // don't display matching '/' or '?'
	    ಯದಿ (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ಭಙ್ಗ;
	}
	ಯದಿ (ಸಂದೇಶ_col)
	    ಸಂದೇಶ_putchar('\n');
	ui_breakcheck();
    }
    ಯದಿ (got_int)
	got_int = FALSE;	// only stop the listing
}

#ಯದಿ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_llist_tags(
    char_u	*tag,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ಯದಿ (list == ರಿಕ್ತ || fname == ರಿಕ್ತ || cmd == ರಿಕ್ತ)
    {
	vim_free(cmd);
	vim_free(fname);
	ಯದಿ (list != ರಿಕ್ತ)
	    list_free(list);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (ಪೂರ್ಣಾಙ್ಕ)(tagp.tagname_end - tagp.tagname);
	ಯದಿ (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ಯದಿ (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((ವರ್ಣ *)tagp.command);
	ಅನ್ಯಥಾ
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    ಯದಿ (cmd_end == ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (p = tagp.command;
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
	    ಯದಿ (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    ಯದಿ (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ಯದಿ (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (ಪೂರ್ಣಾಙ್ಕ)(cmd_end - cmd_start + 1);
	    ಯದಿ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    ಯದಿ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ಯದಿ ((dict = dict_alloc()) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ಅನುವರ್ತ;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_ಸಂಖ್ಯೆ(dict, "lnum", lnum);
	ಯದಿ (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, ರಿಕ್ತ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Free cached tags.
 */
    ಶೂನ್ಯ
tag_freematch(ಶೂನ್ಯ)
{
    VIM_CLEAR(tagmatchname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
taglen_advance(ಪೂರ್ಣಾಙ್ಕ l)
{
    ಯದಿ (l == MAXCOL)
    {
	ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_advance(24);
    }
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_advance(13 + l);
}

/*
 * Print the tag stack
 */
    ಶೂನ್ಯ
do_tags(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಙ್ಕ		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    ಪ್ರತಿ (i = 0; i < tagstacklen; ++i)
    {
	ಯದಿ (tagstack[i].tagname != ರಿಕ್ತ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ಯದಿ (name == ರಿಕ್ತ)	    // file name not available
		ಅನುವರ್ತ;

	    ಸಂದೇಶ_putchar('\n');
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    ಸಂದೇಶ_outtrans(IObuff);
	    ಸಂದೇಶ_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    ಯದಿ (tagstackidx == tagstacklen)	// idx at top of stack
	ಸಂದೇಶ_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯಾವತ್ (len > 0)
    {
	i = (ಪೂರ್ಣಾಙ್ಕ)TOUPPER_ASC(*s1) - (ಪೂರ್ಣಾಙ್ಕ)TOUPPER_ASC(*s2);
	ಯದಿ (i != 0)
	    ಪ್ರತ್ಯಾಗಮ i;			// this character different
	ಯದಿ (*s1 == NUL)
	    ಭಙ್ಗ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಪ್ರತ್ಯಾಗಮ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*pat;		// the pattern
    ಪೂರ್ಣಾಙ್ಕ		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    ಪೂರ್ಣಾಙ್ಕ		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_pats(pat_T *pats, ಪೂರ್ಣಾಙ್ಕ has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    ಯದಿ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ಯದಿ (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	ಅನ್ಯಥಾ ಯದಿ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	ಯದಿ (pats->head == pats->pat)
	    pats->headlen = 0;
	ಅನ್ಯಥಾ
	    ಪ್ರತಿ (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		ಯದಿ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != ರಿಕ್ತ)
		    ಭಙ್ಗ;
	ಯದಿ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ಯದಿ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ಅನ್ಯಥಾ
	pats->regmatch.regprog = ರಿಕ್ತ;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ಸ್ಥಿರ tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    ಯದಿ (VIM_ISDIGIT(*cmd))
	ಪ್ರತ್ಯಾಗಮ *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    ಯದಿ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	ಪ್ರತ್ಯಾಗಮ (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    ಯದಿ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TAGCMD_INVALID;
    ಪ್ರತ್ಯಾಗಮ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    ಪೂರ್ಣಾಙ್ಕ		*match_ಎಣಿಕೆ,	// here the number of tags found will be placed
    ಪೂರ್ಣಾಙ್ಕ		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    ಪೂರ್ಣಾಙ್ಕ		ntags = 0;
    ಪೂರ್ಣಾಙ್ಕ		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    ಯದಿ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ರಿಕ್ತ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    ಯದಿ ((d = dict_alloc_lock(VAR_FIXED)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (!(flags & TAG_INS_COMP) && tag->user_data != ರಿಕ್ತ)
	dict_add_string(d, "user_data", tag->user_data);
    ಯದಿ (buf_ffname != ರಿಕ್ತ)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((ವರ್ಣ *)flagString, ಪರಿಮಾಣ(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ಯದಿ (result == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
    {
	clear_tv(&rettv);
	ಪ್ರತ್ಯಾಗಮ NOTDONE;
    }
    ಯದಿ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	ಪೂರ್ಣಾಙ್ಕ		len;
	dict_iterator_T	iter;
	char_u		*dict_ಕೀಲಿ;
	typval_T	*tv;
	ಪೂರ್ಣಾಙ್ಕ		has_extra = 0;
	ಪೂರ್ಣಾಙ್ಕ		name_only = flags & TAG_NAMES;

	ಯದಿ (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ಭಙ್ಗ;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# ಅನ್ಯಥಾ
	len = 2;
# endif
	res_name = ರಿಕ್ತ;
	res_fname = ರಿಕ್ತ;
	res_cmd = ರಿಕ್ತ;
	res_kind = ರಿಕ್ತ;

	dict_iterate_start(&item->li_tv, &iter);
	ಯಾವತ್ (ರಿಕ್ತ != (dict_ಕೀಲಿ = dict_iterate_next(&iter, &tv)))
	{
	    ಯದಿ (tv->v_type != VAR_STRING || tv->vval.v_string == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ಅನುವರ್ತ;
	    }
	    has_extra = 1;
	    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		ಅನುವರ್ತ;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(dict_ಕೀಲಿ) + 1;
	}

	ಯದಿ (has_extra)
	    len += 2;	// need space for ;"

	ಯದಿ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ಭಙ್ಗ;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	ಯದಿ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ಭಙ್ಗ;
	}
	ಯದಿ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ಯದಿ (name_only)
	    mfp = vim_strsave(res_name);
	ಅನ್ಯಥಾ
	    mfp = alloc(ಪರಿಮಾಣ(char_u) + len + 1);

	ಯದಿ (mfp == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ (!name_only)
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

	    ಯದಿ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ಯದಿ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ಯದಿ (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		ಯಾವತ್ (ರಿಕ್ತ != (dict_ಕೀಲಿ = dict_iterate_next(&iter, &tv)))
		{
		    ಯದಿ (tv->v_type != VAR_STRING || tv->vval.v_string == ರಿಕ್ತ)
			ಅನುವರ್ತ;

		    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "name") == 0)
			ಅನುವರ್ತ;
		    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "filename") == 0)
			ಅನುವರ್ತ;
		    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "cmd") == 0)
			ಅನುವರ್ತ;
		    ಯದಿ (STRCMP(dict_ಕೀಲಿ, "kind") == 0)
			ಅನುವರ್ತ;

		    *p++ = TAB;
		    STRCPY(p, dict_ಕೀಲಿ);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	ಯದಿ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	ಅನ್ಯಥಾ
	{
	    vim_free(mfp);
	    ಭಙ್ಗ;
	}
    }

    clear_tv(&rettv);

    *match_ಎಣಿಕೆ = ntags;
    ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/*
 * State information used during a tag search
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    tagsearch_state_T	state;		// tag search state
    ಪೂರ್ಣಾಙ್ಕ		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    ಪೂರ್ಣಾಙ್ಕ		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    ಪೂರ್ಣಾಙ್ಕ		flags;			// flags used for tag search
    ಪೂರ್ಣಾಙ್ಕ		tag_file_sorted;	// !_TAG_FILE_SORTED value
    ಪೂರ್ಣಾಙ್ಕ		get_searchpat;		// used for 'showfulltag'
    ಪೂರ್ಣಾಙ್ಕ		help_only;		// only search for help tags
    ಪೂರ್ಣಾಙ್ಕ		did_open;		// did open a tag file
    ಪೂರ್ಣಾಙ್ಕ		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    ಪೂರ್ಣಾಙ್ಕ		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    ಪೂರ್ಣಾಙ್ಕ		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    ಪೂರ್ಣಾಙ್ಕ		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    ಪೂರ್ಣಾಙ್ಕ		is_txt;			// flag of file extension
#endif
    ಪೂರ್ಣಾಙ್ಕ		match_ಎಣಿಕೆ;		// number of matches found
    garray_T	ga_match[MT_ಎಣಿಕೆ];	// stores matches in sequence
    hashtab_T	ht_match[MT_ಎಣಿಕೆ];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    ಪೂರ್ಣಾಙ್ಕ			flags,
    ಪೂರ್ಣಾಙ್ಕ			mincount)
{
    ಪೂರ್ಣಾಙ್ಕ		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = ರಿಕ್ತ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
    st->orgpat->regmatch.regprog = ರಿಕ್ತ;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = ರಿಕ್ತ;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_ಎಣಿಕೆ = 0;
    st->stop_searching = FALSE;

    ಪ್ರತಿ (mtt = 0; mtt < MT_ಎಣಿಕೆ; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], ಪರಿಮಾಣ(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    ಯದಿ (st->tag_fname == ರಿಕ್ತ
	    || st->lbuf == ರಿಕ್ತ
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == ರಿಕ್ತ
#endif
       )
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Free the state used by find_tags()
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_in_help_init(findtags_state_T *st)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ಯದಿ (st->is_txt)
	STRCPY(st->help_lang, "en");
    ಅನ್ಯಥಾ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(st->tag_fname);
	ಯದಿ (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	ಅನ್ಯಥಾ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ಯದಿ (st->help_lang_find != ರಿಕ್ತ
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ಯದಿ ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == ರಿಕ್ತ
	    && curbuf->b_fname != ರಿಕ್ತ
	    && (i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ಅನ್ಯಥಾ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	ಪ್ರತಿ (s = p_hlg; *s != NUL; ++s)
	{
	    ಯದಿ (STRNICMP(s, st->help_lang, 2) == 0)
		ಭಙ್ಗ;
	    ++st->help_pri;
	    ಯದಿ ((s = vim_strchr(s, ',')) == ರಿಕ್ತ)
		ಭಙ್ಗ;
	}
	ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ಯದಿ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    ಪೂರ್ಣಾಙ್ಕ		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    ಪೂರ್ಣಾಙ್ಕ		retval;

    ಯದಿ (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	ಪ್ರತ್ಯಾಗಮ NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_ಎಣಿಕೆ,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
ಸ್ಥಿರ ಸಂರಚನಾ
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emacs_tags_incstack_free(ಶೂನ್ಯ)
{
    ಯಾವತ್ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ರಿಕ್ತ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ಯದಿ (vim_fgets(st->ebuf, LSIZE, st->fp))
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ಯದಿ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	ಪ್ರತ್ಯಾಗಮ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    ಯದಿ (incstack[incstack_idx].etag_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ರಿಕ್ತ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    ಯದಿ (fullpath_ebuf != ರಿಕ್ತ)
    {
	st->fp = mch_fopen((ವರ್ಣ *)fullpath_ebuf, "r");
	ಯದಿ (st->fp != ರಿಕ್ತ)
	{
	    ಯದಿ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ಯದಿ (st->fp == ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
emacs_tags_file_eof(findtags_state_T *st)
{
    ಯದಿ (!incstack_idx)	// reached end of file. stop processing.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
emacs_tags_parse_ಸಾಲು(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ಯದಿ (p_7f == ರಿಕ್ತ)
    {
etag_fail:
	ಯದಿ (vim_strchr(lbuf, '\n') != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// Truncated line.  Ignore it.
	ಯದಿ (p_verbose >= 5)
	{
	    verbose_enter();
	    ಸಂದೇಶ(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ಯದಿ (p == ರಿಕ್ತ)
	p = p_7f + 1;
    ಅನ್ಯಥಾ
	++p;

    ಯದಿ (!VIM_ISDIGIT(*p))	    // check for start of line number
	ಗಚ್ಛ etag_fail;
    tagp->command = p;

    ಯದಿ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    ಅನ್ಯಥಾ			    // second format: isolate tagname
    {
	ಯದಿ (p_7f == lbuf)
	    ಗಚ್ಛ etag_fail;

	// find end of tagname
	ಪ್ರತಿ (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ಯದಿ (p == lbuf)
		ಗಚ್ಛ etag_fail;
	tagp->tagname_end = p + 1;
	ಯಾವತ್ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ tags_read_status_T
findtags_get_next_ಸಾಲು(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    ಪೂರ್ಣಾಙ್ಕ		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    ಯದಿ (st->state == TS_ದ್ವಿಮಾನ)
    {
	offset = sinfo_p->ಕೆಳಗಿನ_offset + ((sinfo_p->ಮೇಲಿನ_offset
						- sinfo_p->ಕೆಳಗಿನ_offset) / 2);
	ಯದಿ (offset == sinfo_p->curr_offset)
	    ಪ್ರತ್ಯಾಗಮ TAGS_READ_EOF; // End the binary search without a match.
	ಅನ್ಯಥಾ
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    ಅನ್ಯಥಾ ಯದಿ (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	ಯದಿ (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ಯದಿ (st->state == TS_ದ್ವಿಮಾನ || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	ಯದಿ (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    ಯದಿ (sinfo_p->curr_offset == sinfo_p->ಮೇಲಿನ_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->ಕೆಳಗಿನ_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->ಕೆಳಗಿನ_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	ಯಾವತ್ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	ಯದಿ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ಪ್ರತ್ಯಾಗಮ TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ಅನ್ಯಥಾ
    {
	// skip empty and blank lines
	ಕುರು
	{
#ifdef FEAT_CSCOPE
	    ಯದಿ (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    ಅನ್ಯಥಾ
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} ಯಾವತ್ (!eof && vim_isblankline(st->lbuf));

	ಯದಿ (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    ಯದಿ (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		ಪ್ರತ್ಯಾಗಮ TAGS_READ_IGNORE;
#endif
	    ಪ್ರತ್ಯಾಗಮ TAGS_READ_EOF;
	}
    }

    ಪ್ರತ್ಯಾಗಮ TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ಯದಿ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Process the header line.
    ಯದಿ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    ಯದಿ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	ಪ್ರತಿ (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_start_state_handler(
    findtags_state_T	*st,
    ಪೂರ್ಣಾಙ್ಕ			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ಪೂರ್ಣಾಙ್ಕ		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ಯದಿ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	ಪ್ರತ್ಯಾಗಮ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    ಯದಿ (st->linear || use_cscope)
#ಅನ್ಯಥಾ
    ಯದಿ (st->linear)
#endif
	st->state = TS_LINEAR;
    ಅನ್ಯಥಾ ಯದಿ (st->tag_file_sorted == NUL)
	st->state = TS_ದ್ವಿಮಾನ;
    ಅನ್ಯಥಾ ಯದಿ (st->tag_file_sorted == '1')
	st->state = TS_ದ್ವಿಮಾನ;
    ಅನ್ಯಥಾ ಯದಿ (st->tag_file_sorted == '2')
    {
	st->state = TS_ದ್ವಿಮಾನ;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ಅನ್ಯಥಾ
	st->state = TS_LINEAR;

    ಯದಿ (st->state == TS_ದ್ವಿಮಾನ && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ಯದಿ (st->state == TS_ದ್ವಿಮಾನ)
    {
	ಯದಿ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	ಅನ್ಯಥಾ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->ಕೆಳಗಿನ_offset = 0;
	    sinfo_p->ಕೆಳಗಿನ_char = 0;
	    sinfo_p->ಮೇಲಿನ_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->ಮೇಲಿನ_char = 0xff;
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ tagmatch_status_T
findtags_parse_ಸಾಲು(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    ಪೂರ್ಣಾಙ್ಕ		status;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		cmplen;
    ಪೂರ್ಣಾಙ್ಕ		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ಯದಿ (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	ಯದಿ (tagpp->tagname_end == ರಿಕ್ತ)
	    // Corrupted tag line.
	    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (ಪೂರ್ಣಾಙ್ಕ)(tagpp->tagname_end - tagpp->tagname);
	ಯದಿ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ಯದಿ ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ಅನ್ಯಥಾ ಯದಿ (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;

	ಯದಿ (st->state == TS_ದ್ವಿಮಾನ)
	{
	    // Simplistic check for unsorted tags file.
	    i = (ಪೂರ್ಣಾಙ್ಕ)tagpp->tagname[0];
	    ಯದಿ (margs->sortic)
		i = (ಪೂರ್ಣಾಙ್ಕ)TOUPPER_ASC(tagpp->tagname[0]);
	    ಯದಿ (i < sinfo_p->ಕೆಳಗಿನ_char || i > sinfo_p->ಮೇಲಿನ_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ಯದಿ (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    ಅನ್ಯಥಾ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ಯದಿ (tagcmp == 0)
	    {
		ಯದಿ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ಅನ್ಯಥಾ ಯದಿ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ಯದಿ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;
	    }
	    ಯದಿ (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		ಯದಿ (sinfo_p->curr_offset < sinfo_p->ಮೇಲಿನ_offset)
		{
		    sinfo_p->ಕೆಳಗಿನ_offset = sinfo_p->curr_offset;
		    ಯದಿ (margs->sortic)
			sinfo_p->ಕೆಳಗಿನ_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ಅನ್ಯಥಾ
			sinfo_p->ಕೆಳಗಿನ_char = tagpp->tagname[0];
		    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;
		}
	    }
	    ಯದಿ (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->ಮೇಲಿನ_offset)
	    {
		sinfo_p->ಮೇಲಿನ_offset = sinfo_p->curr_offset;
		ಯದಿ (margs->sortic)
		    sinfo_p->ಮೇಲಿನ_char = TOUPPER_ASC(tagpp->tagname[0]);
		ಅನ್ಯಥಾ
		    sinfo_p->ಮೇಲಿನ_char = tagpp->tagname[0];
		ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_STOP;
	}
	ಅನ್ಯಥಾ ಯದಿ (st->state == TS_SKIP_BACK)
	{
	    ಯದಿ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    ಅನ್ಯಥಾ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;
	}
	ಅನ್ಯಥಾ ಯದಿ (st->state == TS_STEP_FORWARD)
	{
	    ಯದಿ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		ಯದಿ ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_STOP;	// past last match
		ಅನ್ಯಥಾ
		    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;	// before first match
	    }
	}
	ಅನ್ಯಥಾ
	    // skip this match if it can't match
	    ಯದಿ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		ಪ್ರತ್ಯಾಗಮ TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	ಯದಿ (tagpp->fname_end == ರಿಕ್ತ)
	    status = FAIL;
	ಅನ್ಯಥಾ
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    ಅನ್ಯಥಾ
	status = parse_tag_ಸಾಲು(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    ಯದಿ (status == FAIL)
	ಪ್ರತ್ಯಾಗಮ TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    ಯದಿ (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    ಪ್ರತ್ಯಾಗಮ TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_matchargs_init(findtags_match_args_T *margs, ಪೂರ್ಣಾಙ್ಕ flags)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    ಪೂರ್ಣಾಙ್ಕ		match = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (ಪೂರ್ಣಾಙ್ಕ)(tagpp->tagname_end - tagpp->tagname);
    ಯದಿ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ಯದಿ (st->orgpat->len != cmplen)
	match = FALSE;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ಯದಿ (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ಅನ್ಯಥಾ
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    ಯದಿ (!match && st->orgpat->regmatch.regprog != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ಯದಿ (match)
	{
	    margs->matchoff = (ಪೂರ್ಣಾಙ್ಕ)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ಯದಿ (st->orgpat->regmatch.rm_ic)
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

    ಪ್ರತ್ಯಾಗಮ match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		len;

    conv_ಸಾಲು = string_convert(&st->vimconv, st->lbuf, ರಿಕ್ತ);
    ಯದಿ (conv_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Copy or swap lbuf and conv_line.
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(conv_ಸಾಲು) + 1;
    ಯದಿ (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_ಸಾಲು;
	st->lbuf_size = len;
    }
    ಅನ್ಯಥಾ
    {
	STRCPY(st->lbuf, conv_ಸಾಲು);
	vim_free(conv_ಸಾಲು);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ಪೂರ್ಣಾಙ್ಕ		name_only = (st->flags & TAG_NAMES);
    ಪೂರ್ಣಾಙ್ಕ		mtt;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_current;		// file name matches
    ಪೂರ್ಣಾಙ್ಕ		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    ಯದಿ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ಅನ್ಯಥಾ
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
	ಯದಿ (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	ಯದಿ (is_static)
	{
	    ಯದಿ (is_current)
		mtt = MT_ST_CUR;
	    ಅನ್ಯಥಾ
		mtt = MT_ST_OTH;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (is_current)
		mtt = MT_GL_CUR;
	    ಅನ್ಯಥಾ
		mtt = MT_GL_OTH;
	}
	ಯದಿ (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	ಯದಿ (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ಯದಿ (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#ಅನ್ಯಥಾ
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (ಪೂರ್ಣಾಙ್ಕ)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(ಪರಿಮಾಣ(char_u) + len + 10 + ML_EXTRA + 1);
	ಯದಿ (mfp != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ heuristic;

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
	    sprintf((ವರ್ಣ *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    ಅನ್ಯಥಾ ಯದಿ (name_only)
    {
	ಯದಿ (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    ಯದಿ (*temp_end == '/')
		ಯಾವತ್ (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    ಯದಿ (tagpp->command + 2 < temp_end)
	    {
		len = (ಪೂರ್ಣಾಙ್ಕ)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		ಯದಿ (mfp != ರಿಕ್ತ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ಅನ್ಯಥಾ
		mfp = ರಿಕ್ತ;
	    st->get_searchpat = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(ಪರಿಮಾಣ(char_u) + len + 1);
	    ಯದಿ (mfp != ರಿಕ್ತ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ಯದಿ (State & MODE_ಸೇರಿಸು)
		st->get_searchpat = p_sft;
	}
    }
    ಅನ್ಯಥಾ
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
	len = (ಪೂರ್ಣಾಙ್ಕ)tag_fname_len + (ಪೂರ್ಣಾಙ್ಕ)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	ಯದಿ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (ಪೂರ್ಣಾಙ್ಕ)ebuf_len + 1;
	}
	ಅನ್ಯಥಾ
	    ++len;
#endif
	mfp = alloc(ಪರಿಮಾಣ(char_u) + len + 1);
	ಯದಿ (mfp != ರಿಕ್ತ)
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
	    ಯದಿ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    ಅನ್ಯಥಾ
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    ಯದಿ (mfp != ರಿಕ್ತ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	ಯದಿ (use_cscope)
	    ++*hash;
	ಅನ್ಯಥಾ
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	ಯದಿ (HASHITEM_EMPTY(hi))
	{
	    ಯದಿ (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_ಎಣಿಕೆ++;
	}
	ಅನ್ಯಥಾ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	ಹುಡುಕು_info;
    ಪೂರ್ಣಾಙ್ಕ			retval;
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(ಹುಡುಕು_info);

    // Read and parse the lines in the file one by one
    ಪ್ರತಿ (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ಯದಿ (st->state == TS_ದ್ವಿಮಾನ || st->state == TS_SKIP_BACK)
	    ಸಾಲು_breakcheck();
	ಅನ್ಯಥಾ
	    fast_breakcheck();
	ಯದಿ ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ಕೀಲಿಗಳು(30, FALSE);
	ಯದಿ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ಭಙ್ಗ;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ಯದಿ (st->mincount == TAG_MANY && st->match_ಎಣಿಕೆ >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    ಭಙ್ಗ;
	}
	ಯದಿ (st->get_searchpat)
	    ಗಚ್ಛ ಸಾಲು_read_in;

	retval = findtags_get_next_ಸಾಲು(st, &ಹುಡುಕು_info);
	ಯದಿ (retval == TAGS_READ_IGNORE)
	    ಅನುವರ್ತ;
	ಯದಿ (retval == TAGS_READ_EOF)
	    ಭಙ್ಗ;

ಸಾಲು_read_in:

	ಯದಿ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ಯದಿ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    ಅನುವರ್ತ;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ಯದಿ (st->state == TS_START)
	{
	    ಯದಿ (findtags_start_state_handler(st, &margs->sortic, &ಹುಡುಕು_info) == FALSE)
		ಅನುವರ್ತ;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ಯದಿ (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    ಯದಿ (st->lbuf == ರಿಕ್ತ)
	    {
		ಯದಿ (st->fp != ರಿಕ್ತ)
		    fclose(st->fp);
		st->fp = ರಿಕ್ತ;
		st->stop_searching = TRUE;
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    ಯದಿ (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, ಹುಡುಕು_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    ಹುಡುಕು_info.curr_offset = 0;
	    ಅನುವರ್ತ;
	}

	retval = findtags_parse_ಸಾಲು(st, &tagp, margs, &ಹುಡುಕು_info);
	ಯದಿ (retval == TAG_MATCH_NEXT)
	    ಅನುವರ್ತ;
	ಯದಿ (retval == TAG_MATCH_STOP)
	    ಭಙ್ಗ;
	ಯದಿ (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    ಯದಿ (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ಯದಿ (findtags_match_tag(st, &tagp, margs))
	{
	    ಯದಿ (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ಭಙ್ಗ;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = ರಿಕ್ತ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    ಯದಿ (use_cscope)
	st->fp = ರಿಕ್ತ;	    // avoid GCC warning
    ಅನ್ಯಥಾ
#endif
    {
#ifdef FEAT_MULTI_LANG
	ಯದಿ (curbuf->b_help)
	{
	    ಯದಿ (!findtags_in_help_init(st))
		ಪ್ರತ್ಯಾಗಮ;
	}
#endif

	st->fp = mch_fopen((ವರ್ಣ *)st->tag_fname, "r");
	ಯದಿ (st->fp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (p_verbose >= 5)
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

    ಯದಿ (st->fp != ರಿಕ್ತ)
    {
	fclose(st->fp);
	st->fp = ರಿಕ್ತ;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    ಯದಿ (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, ರಿಕ್ತ, ರಿಕ್ತ);

    ಯದಿ (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    ಯದಿ (st->match_ಎಣಿಕೆ >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    ಪೂರ್ಣಾಙ್ಕ		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    ಪೂರ್ಣಾಙ್ಕ		mtt;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*mfp;
    char_u	*p;

    ಯದಿ (st->match_ಎಣಿಕೆ > 0)
	matches = ALLOC_MULT(char_u *, st->match_ಎಣಿಕೆ);
    ಅನ್ಯಥಾ
	matches = ರಿಕ್ತ;
    st->match_ಎಣಿಕೆ = 0;
    ಪ್ರತಿ (mtt = 0; mtt < MT_ಎಣಿಕೆ; ++mtt)
    {
	ಪ್ರತಿ (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    ಯದಿ (matches == ರಿಕ್ತ)
		vim_free(mfp);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    ಪ್ರತಿ (p = mfp + 1; *p != NUL; ++p)
			ಯದಿ (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_ಎಣಿಕೆ++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    ಪ್ರತ್ಯಾಗಮ st->match_ಎಣಿಕೆ;
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
    ಪೂರ್ಣಾಙ್ಕ
find_tags(
    char_u	*pat,			// pattern to search for
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    ಪೂರ್ಣಾಙ್ಕ		first_file;		// trying first tag file
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;		// return value
    ಪೂರ್ಣಾಙ್ಕ		round;

    ಪೂರ್ಣಾಙ್ಕ		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*saved_pat = ರಿಕ್ತ;		// copy of pat[]
#endif

    ಪೂರ್ಣಾಙ್ಕ		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    ಪೂರ್ಣಾಙ್ಕ		has_re = (flags & TAG_REGEXP);	// regexp used
    ಪೂರ್ಣಾಙ್ಕ		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    ಪೂರ್ಣಾಙ್ಕ		use_cscope = (flags & TAG_CSCOPE);
#endif
    ಪೂರ್ಣಾಙ್ಕ		verbose = (flags & TAG_VERBOSE);
    ಪೂರ್ಣಾಙ್ಕ		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    ವಿಕಲ್ಪ (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	ಪಕ್ಷ TC_FOLLOWIC:		 ಭಙ್ಗ;
	ಪಕ್ಷ TC_IGNORE:    p_ic = TRUE;  ಭಙ್ಗ;
	ಪಕ್ಷ TC_MATCH:     p_ic = FALSE; ಭಙ್ಗ;
	ಪಕ್ಷ TC_FOLLOWSCS: p_ic = ignorecase(pat); ಭಙ್ಗ;
	ಪಕ್ಷ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ಭಙ್ಗ;
    }

    help_save = curbuf->b_help;

    ಯದಿ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	ಗಚ್ಛ findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    ಯದಿ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    ಅನ್ಯಥಾ ಯದಿ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    ಯದಿ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ಯದಿ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ಯದಿ (saved_pat != ರಿಕ್ತ)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    ಯದಿ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ಯದಿ (has_re && st.orgpat->regmatch.regprog == ರಿಕ್ತ)
	ಗಚ್ಛ findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ಯದಿ (retval != NOTDONE)
	ಗಚ್ಛ findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ಯದಿ ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == ರಿಕ್ತ
	    && curbuf->b_fname != ರಿಕ್ತ
	    && (i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(curbuf->b_fname)) > 4
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
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      ಪ್ರತಿ (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  ಯದಿ (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      ಭಙ್ಗ;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	ಯದಿ (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ಯದಿ (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ಭಙ್ಗ;
#ifdef FEAT_CSCOPE
	ಯದಿ (use_cscope)
	    ಭಙ್ಗ;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ಯದಿ (!st.stop_searching)
    {
	ಯದಿ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ಯದಿ (retval == FAIL)
	st.match_ಎಣಿಕೆ = 0;

    *ಅಂಕೆ_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    ಪ್ರತ್ಯಾಗಮ retval;
}

ಸ್ಥಿರ garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
found_tagfile_cb(char_u *fname, ಶೂನ್ಯ *cookie UNUSED)
{
    ಯದಿ (ga_grow(&tag_fnames, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_tag_stuff(ಶೂನ್ಯ)
{
    ga_clear_strings(&tag_fnames);
    ಯದಿ (curwin != ರಿಕ್ತ)
	do_tag(ರಿಕ್ತ, DT_FREE, 0, 0, 0);
    tag_freematch();

# ಯದಿ defined(FEAT_QUICKFIX)
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
    ಪೂರ್ಣಾಙ್ಕ
get_tagfname(
    tagname_T	*tnp,	// holds status info
    ಪೂರ್ಣಾಙ್ಕ		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ರಿಕ್ತ;
    char_u		*r_ptr;
    ಪೂರ್ಣಾಙ್ಕ			i;

    ಯದಿ (first)
	CLEAR_POINTER(tnp);

    ಯದಿ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ಯದಿ (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, ಪರಿಮಾಣ(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ಅನ್ಯಥಾ
		    "doc/tags doc/tags-??"
# endif
#ಅನ್ಯಥಾ
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, ರಿಕ್ತ);
	}

	ಯದಿ (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ಯದಿ (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    ಪ್ರತಿ (i = 0; i < tag_fnames.ga_len; ++i)
		ಯದಿ (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    ಪ್ರತ್ಯಾಗಮ FAIL; // avoid duplicate file names
	}
	ಅನ್ಯಥಾ
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	ಯದಿ (tnp->tn_tags == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_ಹುಡುಕು_ctx);
	    ಯದಿ (fname != ರಿಕ್ತ)
		ಭಙ್ಗ;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    char_u  *filename = ರಿಕ್ತ;

	    // Stop when used all parts of 'tags'.
	    ಯದಿ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_ಹುಡುಕು_ctx);
		tnp->tn_ಹುಡುಕು_ctx = ರಿಕ್ತ;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (ಶೂನ್ಯ)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    ಯದಿ (r_ptr != ರಿಕ್ತ)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_ಹುಡುಕು_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_ಹುಡುಕು_ctx, TRUE, curbuf->b_ffname);
	    ಯದಿ (tnp->tn_ಹುಡುಕು_ctx != ರಿಕ್ತ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    ಶೂನ್ಯ
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_ಹುಡುಕು_ctx);
    tnp->tn_ಹುಡುಕು_ctx = ರಿಕ್ತ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_tag_ಸಾಲು(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    ಪೂರ್ಣಾಙ್ಕ		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    ಯದಿ (is_etag)
	// emacs-style tag file
	ಪ್ರತ್ಯಾಗಮ emacs_tags_parse_ಸಾಲು(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    ಯದಿ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    ಯದಿ (*p != NUL)
	++p;
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    tagp->command = p;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ಯಾವತ್ ((p = vim_strchr(p, '\t')) != ರಿಕ್ತ)
    {
	++p;
	ಯದಿ (STRNCMP(p, "file:", 5) == 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ಸ್ಥಿರ size_t
matching_ಸಾಲು_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    ಪ್ರತ್ಯಾಗಮ (p - lbuf) + STRLEN(p);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    ಪೂರ್ಣಾಙ್ಕ		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    ಯದಿ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    ಅನ್ಯಥಾ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_ಸಾಲು(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = ರಿಕ್ತ;
    tagp->user_data = ರಿಕ್ತ;
    tagp->tagline = 0;
    tagp->command_end = ರಿಕ್ತ;

    ಯದಿ (retval != OK)
	ಪ್ರತ್ಯಾಗಮ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ಯದಿ (find_extra(&p) == OK)
    {
	ಯದಿ (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	ಅನ್ಯಥಾ
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	ಯದಿ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ಯಾವತ್ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ಯದಿ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((ವರ್ಣ *)p + 5);
		ಯದಿ (tagp->tagkind != ರಿಕ್ತ && tagp->user_data != ರಿಕ್ತ)
		    ಭಙ್ಗ;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ಯದಿ (pc == ರಿಕ್ತ || (pt != ರಿಕ್ತ && pc > pt))
		    tagp->tagkind = p;
		ಯದಿ (pt == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ಯದಿ (tagp->tagkind != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    ಯದಿ (tagp->user_data != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ಸ್ಥಿರ char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    ಪೂರ್ಣಾಙ್ಕ		c;

#ifdef FEAT_EMACS_TAGS
    ಯದಿ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ಅನ್ಯಥಾ
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    ಯದಿ (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    ಪ್ರತ್ಯಾಗಮ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    ಪೂರ್ಣಾಙ್ಕ		forceit,	// :ta with !
    ಪೂರ್ಣಾಙ್ಕ		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    ಪೂರ್ಣಾಙ್ಕ		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = ರಿಕ್ತ;
    char_u	*fname;
    tagptrs_T	tagp;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		getfile_result = GETFILE_UNUSED;
    ಪೂರ್ಣಾಙ್ಕ		ಹುಡುಕು_options;
#ifdef FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಙ್ಕ		save_no_hlsearch;
#endif
#ಯದಿ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ರಿಕ್ತ;
#endif
    char_u	*full_fname = ರಿಕ್ತ;
#ifdef FEAT_FOLDING
    ಪೂರ್ಣಾಙ್ಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    ಪೂರ್ಣಾಙ್ಕ		isdigit = FALSE;

    ಯದಿ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ಸಾಲು_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ಯದಿ (lbuf != ರಿಕ್ತ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ಯದಿ (pbuf == ರಿಕ್ತ || lbuf == ರಿಕ್ತ || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = ರಿಕ್ತ;
	ಗಚ್ಛ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ಯದಿ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    ಪ್ರತಿ (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	ಯದಿ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ಭಙ್ಗ;
#endif
	*pbuf_end++ = *str++;
	ಯದಿ (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    ಭಙ್ಗ;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    ಯದಿ (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ಯದಿ (isdigit)
	    str++;
	ಯದಿ (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ಯದಿ (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    ಅನ್ಯಥಾ
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಗಚ್ಛ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ಯದಿ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ರಿಕ್ತ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ಯದಿ (nofile_fname == ರಿಕ್ತ)
	    nofile_fname = empty_option;
	ಗಚ್ಛ erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ಯದಿ defined(FEAT_QUICKFIX)
    ಯದಿ (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ಯದಿ (!curwin->w_p_pvw)
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
    ಯದಿ (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ಯದಿ (existing_buf != ರಿಕ್ತ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ಯದಿ (swbuf_goto_win_with_buf(existing_buf) != ರಿಕ್ತ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    ಯದಿ (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	ಯದಿ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	ಯದಿ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	ಯದಿ (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    ಯದಿ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    ಗಚ್ಛ erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    ಯದಿ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ಯದಿ defined(FEAT_QUICKFIX)
	ಯದಿ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	ಅನ್ಯಥಾ
#endif
	    keep_help_flag = curbuf->b_help;
    }

    ಯದಿ (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, ರಿಕ್ತ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ಯದಿ (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_ಹುಡುಕು_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ಯದಿ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ಯದಿ (vim_strchr(p_cpo, CPO_TAGPAT) != ರಿಕ್ತ)
	    ಹುಡುಕು_options = 0;
	ಅನ್ಯಥಾ
	    ಹುಡುಕು_options = ಹುಡುಕು_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	ಯದಿ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ಯದಿ (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ಯದಿ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ಅನ್ಯಥಾ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ಯದಿ (do_ಹುಡುಕು(ರಿಕ್ತ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 ಹುಡುಕು_options, ರಿಕ್ತ))
		retval = OK;
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ	found = 1;
		ಪೂರ್ಣಾಙ್ಕ	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ಯದಿ (!do_ಹುಡುಕು(ರಿಕ್ತ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 ಹುಡುಕು_options, ರಿಕ್ತ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (ಶೂನ್ಯ)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((ವರ್ಣ *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ಯದಿ (!do_ಹುಡುಕು(ರಿಕ್ತ, '/', '/', pbuf, pbuflen, (long)1,
							 ಹುಡುಕು_options, ರಿಕ್ತ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((ವರ್ಣ *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ಯದಿ (!do_ಹುಡುಕು(ರಿಕ್ತ, '/', '/', pbuf, pbuflen, (long)1,
							 ಹುಡುಕು_options, ರಿಕ್ತ))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		ಯದಿ (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ಅನ್ಯಥಾ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ಯದಿ (found == 2 || !save_p_ic)
		    {
			ಸಂದೇಶ(_(e_couldnt_find_tag_just_guessing));
			ಯದಿ (!ಸಂದೇಶ_scrolled && ಸಂದೇಶ_silent == 0)
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
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		save_secure = secure;

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
	    ಯದಿ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_ಹುಡುಕು_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ಯದಿ (ಹುಡುಕು_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	ಯದಿ (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	ಯದಿ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ಯದಿ (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    ಯದಿ ((fdo_flags & FDO_TAG) && old_ಕೀಲಿ_Typed)
		foldOpenCursor();
#endif
	}

#ಯದಿ defined(FEAT_QUICKFIX)
	ಯದಿ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	ಯದಿ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ಯದಿ (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#ಯದಿ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ಯದಿ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#ಯದಿ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ಯದಿ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#ಯದಿ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ಸ್ಥಿರ char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, ಪೂರ್ಣಾಙ್ಕ expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ರಿಕ್ತ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ಯದಿ (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ಯದಿ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ರಿಕ್ತ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ರಿಕ್ತ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ಯದಿ (expanded_fname != ರಿಕ್ತ)
	    fname = expanded_fname;
    }

    ಯದಿ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	ಯದಿ (retval != ರಿಕ್ತ)
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
    ಅನ್ಯಥಾ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_for_current(
#ifdef FEAT_EMACS_TAGS
    ಪೂರ್ಣಾಙ್ಕ	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    ಪೂರ್ಣಾಙ್ಕ	    c;
    ಪೂರ್ಣಾಙ್ಕ	    retval = FALSE;
    char_u  *fullname;

    ಯದಿ (buf_ffname != ರಿಕ್ತ)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	ಯದಿ (is_etag)
	    c = 0;	    // to shut up GCC
	ಅನ್ಯಥಾ
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	ಯದಿ (fullname != ರಿಕ್ತ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	ಯದಿ (!is_etag)
#endif
	    *fname_end = c;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ಅನ್ಯಥಾ ಯದಿ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ಯದಿ (*str != first_char)
		str = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		++str;
	}
	ಅನ್ಯಥಾ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((ವರ್ಣ *)str, "|;\"");
	    ಯದಿ (str != ರಿಕ್ತ)
	    {
		++str;
		ಭಙ್ಗ;
	    }

	}
	ಯದಿ (str == ರಿಕ್ತ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ಭಙ್ಗ;
	++str;	// skip ';'
	first_char = *str;
    }

    ಯದಿ (str != ರಿಕ್ತ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    ಶೂನ್ಯ
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_tags(
    ಪೂರ್ಣಾಙ್ಕ		tagnames,	// expand tag names
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_file,
    char_u	***file)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    name_buf = alloc(name_buf_size);
    ಯದಿ (name_buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (tagnames)
	extra_flag = TAG_NAMES;
    ಅನ್ಯಥಾ
	extra_flag = 0;
    ಯದಿ (pat[0] == '/')
	ret = find_tags(pat + 1, ಅಂಕೆ_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    ಅನ್ಯಥಾ
	ret = find_tags(pat, ಅಂಕೆ_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    ಯದಿ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 ಪ್ರತಿ (i = 0; i < *ಅಂಕೆ_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     ಯದಿ (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 ಯದಿ (buf == ರಿಕ್ತ)
		 {
		     vim_free(name_buf);
		     ಪ್ರತ್ಯಾಗಮ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ರಿಕ್ತ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    ಪ್ರತ್ಯಾಗಮ ret;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_tag_field(
    dict_T  *dict,
    ವರ್ಣ    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    // check that the field name doesn't exist yet
    ಯದಿ (dict_has_ಕೀಲಿ(dict, field_name))
    {
	ಯದಿ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    buf = alloc(MAXPATHL);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (start != ರಿಕ್ತ)
    {
	ಯದಿ (end == ರಿಕ್ತ)
	{
	    end = start + STRLEN(start);
	    ಯಾವತ್ (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (ಪೂರ್ಣಾಙ್ಕ)(end - start);
	ಯದಿ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &ಅಂಕೆ_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (ಪೂರ್ಣಾಙ್ಕ)MAXCOL, buf_fname);
    ಯದಿ (ret != OK || ಅಂಕೆ_matches <= 0)
	ಪ್ರತ್ಯಾಗಮ ret;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
    {
	ಯದಿ (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ಅನುವರ್ತ;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ಯದಿ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ಅನುವರ್ತ;
	}

	ಯದಿ ((dict = dict_alloc()) == ರಿಕ್ತ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ಭಙ್ಗ;
	}
	ಯದಿ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ಅನುವರ್ತ;
	}

	full_fname = tag_full_fname(&tp);
	ಯದಿ (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    ರಿಕ್ತ) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_ಸಂಖ್ಯೆ(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	ಯದಿ (tp.command_end != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ಯದಿ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ಅನ್ಯಥಾ ಯದಿ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    ಪೂರ್ಣಾಙ್ಕ	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ಯಾವತ್ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (ಪೂರ್ಣಾಙ್ಕ)(p - n);
		    ಯದಿ (*p == ':' && len > 0)
		    {
			s = ++p;
			ಯಾವತ್ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ಯದಿ (add_tag_field(dict, (ವರ್ಣ *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ಅನ್ಯಥಾ
			// Skip field without colon.
			ಯಾವತ್ (*p != NUL && *p >= ' ')
			    ++p;
		    ಯದಿ (*p == NUL)
			ಭಙ್ಗ;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_ಸಂಖ್ಯೆ(retdict, "matchnr", tag->cur_match + 1);
    dict_add_ಸಂಖ್ಯೆ(retdict, "bufnr", tag->cur_fnum);
    ಯದಿ (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    ಯದಿ ((pos = list_alloc_id(aid_tagstack_from)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_ಸಂಖ್ಯೆ(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_ಸಂಖ್ಯೆ(pos, (varnumber_T)fmark->mark.lnum);
    list_append_ಸಂಖ್ಯೆ(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_ಸಂಖ್ಯೆ(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    ಶೂನ್ಯ
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		i;
    dict_T	*d;

    dict_add_ಸಂಖ್ಯೆ(retdict, "length", wp->w_tagstacklen);
    dict_add_ಸಂಖ್ಯೆ(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_ವಸ್ತುಗಳು);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    dict_add_list(retdict, "items", l);

    ಪ್ರತಿ (i = 0; i < wp->w_tagstacklen; i++)
    {
	ಯದಿ ((d = dict_alloc_id(aid_tagstack_details)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_clear(win_T *wp)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    // Free the current tag stack
    ಪ್ರತಿ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		i;

    tagstack_clear_entry(&tagstack[0]);
    ಪ್ರತಿ (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_ತಳ್ಳು_item(
	win_T	*wp,
	char_u	*tagname,
	ಪೂರ್ಣಾಙ್ಕ	cur_fnum,
	ಪೂರ್ಣಾಙ್ಕ	cur_match,
	pos_T	mark,
	ಪೂರ್ಣಾಙ್ಕ	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ಪೂರ್ಣಾಙ್ಕ		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ಯದಿ (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    ಯದಿ (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_ತಳ್ಳು_ವಸ್ತುಗಳು(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    ಪೂರ್ಣಾಙ್ಕ		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ರಿಕ್ತ)
	    ಅನುವರ್ತ;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ಯದಿ ((di = dict_find(itemdict, (char_u *)"from", -1)) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ (list2fpos(&di->di_tv, &mark, &fnum, ರಿಕ್ತ, FALSE) != OK)
	    ಅನುವರ್ತ;
	ಯದಿ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ (mark.col > 0)
	    mark.col--;
	tagstack_ತಳ್ಳು_item(wp, tagname,
		(ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(itemdict, "bufnr"),
		(ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_set_curidx(win_T *wp, ಪೂರ್ಣಾಙ್ಕ curidx)
{
    wp->w_tagstackidx = curidx;
    ಯದಿ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ಯದಿ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    ಪೂರ್ಣಾಙ್ಕ
set_tagstack(win_T *wp, dict_T *d, ಪೂರ್ಣಾಙ್ಕ action)
{
    dictitem_T	*di;
    list_T	*l = ರಿಕ್ತ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ಯದಿ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ರಾಶಿ_within_tagfunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
# endif

    ಯದಿ ((di = dict_find(d, (char_u *)"items", -1)) != ರಿಕ್ತ)
    {
	ಯದಿ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ಯದಿ ((di = dict_find(d, (char_u *)"curidx", -1)) != ರಿಕ್ತ)
	tagstack_set_curidx(wp, (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&di->di_tv) - 1);

    ಯದಿ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	ಪೂರ್ಣಾಙ್ಕ	tagstackidx = wp->w_tagstackidx;
	ಪೂರ್ಣಾಙ್ಕ	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ಯಾವತ್ (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ಯದಿ (l != ರಿಕ್ತ)
    {
	ಯದಿ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ತಳ್ಳು_ವಸ್ತುಗಳು(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif
