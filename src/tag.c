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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା tag_pointers
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_etag;	// TRUE for emacs tag
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_ଦ୍ୱିମିକ,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    off_T	ତଳ_offset;	// offset for first char of first line that
				// could match
    off_T	ଉପର_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ତଳ_char;		// first char at low_offset
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଉପର_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	matchoff;		// tag match offset
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	match_re;		// TRUE if the tag matches a regexp
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	match_no_ic;		// TRUE if the tag matches with case
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_re;			// regular expression used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sortic;			// tags file sorted ignoring case (foldcase)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sort_error;		// tags file not sorted
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
#define MT_ଗଣନା	16

ସ୍ଥିର ଅକ୍ଷର	*mt_names[MT_ଗଣନା/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
ସ୍ଥିର char_u	*nofile_fname = ଖାଲି;	// fname for NOTAGFILE error

ସ୍ଥିର ଶୂନ୍ୟ taglen_advance(ପୂର୍ଣ୍ଣସଂଖ୍ୟା l);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା jumpto_tag(char_u *lbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep_help);
#ifdef FEAT_EMACS_TAGS
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_tag_ଧାଡ଼ି(char_u *lbuf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_etag, tagptrs_T *tagp);
#ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_tag_ଧାଡ଼ି(char_u *lbuf, tagptrs_T *tagp);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_for_static(tagptrs_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_match(char_u *lbuf, tagptrs_T *tagp);
ସ୍ଥିର char_u *tag_full_fname(tagptrs_T *tagp);
ସ୍ଥିର char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand);
#ifdef FEAT_EMACS_TAGS
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_for_current(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *, char_u *, char_u *, char_u *);
#ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_extra(char_u **pp);
ସ୍ଥିର ଶୂନ୍ୟ print_tag_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_tag, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_tagstack, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_matches, char_u **matches);
#ଯଦି defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_llist_tags(char_u *tag, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_matches, char_u **matches);
#endif

ସ୍ଥିର char_u	*tagmatchname = ଖାଲି;	// name of last used tag

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ସ୍ଥିର taggy_T ptag_entry = {ଖାଲି, {{0, 0, 0}, 0}, 0, 0, ଖାଲି};
#endif

#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
ସ୍ଥିର callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#ଯଦି defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    ଅକ୍ଷର *
did_set_tagfunc(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval;

    ଯଦି (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    ଅନ୍ୟଥା
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	ଯଦି (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    ଫେରନ୍ତୁ retval == FAIL ? e_invalid_argument : ଖାଲି;
}
#endif

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_tagfunc_option(ଶୂନ୍ୟ)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_tagfunc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    ଫେରନ୍ତୁ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    ଶୂନ୍ୟ
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    ଯଦି (tfu_cb.cb_name != ଖାଲି && *tfu_cb.cb_name != NUL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,	// :ta with !
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagstackidx = curwin->w_tagstackidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagstacklen = curwin->w_tagstacklen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_match = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_fnum = curbuf->b_fnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldtagstackidx = tagstackidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevtagstackidx = tagstackidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ଅଙ୍କ_matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_tag = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_regexp = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error_cur_match = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jumped_to_tag = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_ଅଙ୍କ_matches;
    char_u	**new_matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_ସନ୍ଦେଶ = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_tfu = 1;
    char_u	*tofree = ଖାଲି;

    // remember the matches for the last used tag
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_ଅଙ୍କ_matches = 0;  // limit used for match search
    ସ୍ଥିର char_u	**matches = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

#ifdef EXITFREE
    ଯଦି (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(ଅଙ୍କ_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	ଅଙ୍କ_matches = 0;
	ଫେରନ୍ତୁ FALSE;
    }
#endif

#ifdef FEAT_EVAL
    ଯଦି (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ଗଦା_within_tagfunc));
	ଫେରନ୍ତୁ FALSE;
    }
#endif

    ଯଦି (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_ଅଙ୍କ_matches = ଅଙ୍କ_matches;
    free_string_option(nofile_fname);
    nofile_fname = ଖାଲି;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ଯଦି ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#ଯଦି defined(FEAT_QUICKFIX)
	ଯଦି (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    ଯଦି ((ptag_entry.tagname = vim_strsave(tag)) == ଖାଲି)
		ଯାଅ end_do_tag;
	}
#endif
    }
    ଅନ୍ୟଥା
    {
#ଯଦି defined(FEAT_QUICKFIX)
	ଯଦି (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ଅନ୍ୟଥା
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ଯଦି (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#ଯଦି defined(FEAT_QUICKFIX)
	    ଯଦି (g_do_tagpreview != 0)
	    {
		ଯଦି (ptag_entry.tagname != ଖାଲି
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ଅନ୍ୟଥା
		{
		    tagstack_clear_entry(&ptag_entry);
		    ଯଦି ((ptag_entry.tagname = vim_strsave(tag)) == ଖାଲି)
			ଯାଅ end_do_tag;
		}
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ଯେତେବେଳେ (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ଯଦି (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    ପାଇଁ (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = ଖାଲି;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ଯଦି ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == ଖାଲି)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    ଯାଅ end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (
#ଯଦି defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == ଖାଲି :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_ଗଦା_empty));
		ଯାଅ end_do_tag;
	    }

	    ଯଦି (type == DT_କାଢ଼)		// go to older position
	    {
#ifdef FEAT_FOLDING
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_ଚାବି_Typed = ଚାବି_Typed;
#endif
		ଯଦି ((tagstackidx -= ଗଣନା_2) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_ଗଦା));
		    ଯଦି (tagstackidx + ଗଣନା_2 == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			ଯାଅ end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ଅନ୍ୟଥା ଯଦି (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_ଶୀର୍ଷ_of_tag_ଗଦା));
		    ଯାଅ end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ଯଦି (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ଯଦି (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			ଯାଅ end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ଅନ୍ୟଥା
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		ଯଦି ((fdo_flags & FDO_TAG) && old_ଚାବି_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(ଅଙ୍କ_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		ଅଙ୍କ_matches = 0;
		tag_freematch();
		ଯାଅ end_do_tag;
	    }

	    ଯଦି (type == DT_TAG
#ଯଦି defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ଅନ୍ୟଥା
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ଯଦି ((tagstackidx += ଗଣନା_2 - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_ଶୀର୍ଷ_of_tag_ଗଦା));
			save_pos = FALSE;
		    }
		    ଅନ୍ୟଥା ଯଦି (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_ଗଦା));
			tagstackidx = 0;
			ଯାଅ end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    ଅନ୍ୟଥା				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ଯଦି defined(FEAT_QUICKFIX)
		ଯଦି (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ଅନ୍ୟଥା
#endif
		{
		    ଯଦି (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		ବାଛ (type)
		{
		    ସ୍ଥିତି DT_FIRST: cur_match = ଗଣନା_2 - 1; ଭାଙ୍ଗ;
		    ସ୍ଥିତି DT_SELECT:
		    ସ୍ଥିତି DT_JUMP:
#ifdef FEAT_CSCOPE
		    ସ୍ଥିତି DT_CSCOPE:
#endif
		    ସ୍ଥିତି DT_LAST:  cur_match = MAXCOL - 1; ଭାଙ୍ଗ;
		    ସ୍ଥିତି DT_NEXT:  cur_match += ଗଣନା_2; ଭାଙ୍ଗ;
		    ସ୍ଥିତି DT_PREV:  cur_match -= ଗଣନା_2; ଭାଙ୍ଗ;
		}
		ଯଦି (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		ଅନ୍ୟଥା ଯଦି (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_ସନ୍ଦେଶ = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ଯଦି defined(FEAT_QUICKFIX)
	ଯଦି (g_do_tagpreview != 0)
	{
	    ଯଦି (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	ଅନ୍ୟଥା
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ଯଦି (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ଯଦି (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ଯଦି (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ଯଦି (buf != ଖାଲି)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ଯଦି (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ଯଦି defined(FEAT_QUICKFIX)
	ଅନ୍ୟଥା ଯଦି (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	ଅନ୍ୟଥା
	    name = tag;
	other_name = (tagmatchname == ଖାଲି || STRCMP(tagmatchname, name) != 0);
	ଯଦି (new_tag
		|| (cur_match >= ଅଙ୍କ_matches && max_ଅଙ୍କ_matches != MAXCOL)
		|| other_name)
	{
	    ଯଦି (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ଯଦି (type == DT_SELECT || type == DT_JUMP
#ଯଦି defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    ଯଦି (type == DT_TAG)
		max_ଅଙ୍କ_matches = MAXCOL;
	    ଅନ୍ୟଥା
		max_ଅଙ୍କ_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    ଯଦି (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    ଅନ୍ୟଥା
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    ଯଦି (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    ଯଦି (verbose)
		flags |= TAG_VERBOSE;

	    ଯଦି (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    ଯଦି (find_tags(name, &new_ଅଙ୍କ_matches, &new_matches, flags,
					    max_ଅଙ୍କ_matches, buf_ffname) == OK
		    && new_ଅଙ୍କ_matches < max_ଅଙ୍କ_matches)
		max_ଅଙ୍କ_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ଯଦି (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_ଅଙ୍କ_matches, new_matches);
		ଭାଙ୍ଗ;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ଯଦି (!new_tag && !other_name)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j, k;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		ପାଇଁ (j = 0; j < ଅଙ୍କ_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    ପାଇଁ (i = idx; i < new_ଅଙ୍କ_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			ଯଦି (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    ପାଇଁ (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ଭାଙ୍ଗ;
			}
		    }
		}
	    }
	    FreeWild(ଅଙ୍କ_matches, matches);
	    ଅଙ୍କ_matches = new_ଅଙ୍କ_matches;
	    matches = new_matches;
	}

	ଯଦି (ଅଙ୍କ_matches <= 0)
	{
	    ଯଦି (verbose)
		semsg(_(e_tag_not_found_str), name);
#ଯଦି defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ask_for_selection = FALSE;
#ଯଦି defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    ଯଦି (type == DT_CSCOPE && ଅଙ୍କ_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    ଅନ୍ୟଥା
#endif
	    ଯଦି (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = ଗଣନା_2 > 0 ? ଗଣନା_2 - 1 : 0;
	    ଅନ୍ୟଥା ଯଦି (type == DT_SELECT || (type == DT_JUMP && ଅଙ୍କ_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, ଅଙ୍କ_matches, matches);
		ask_for_selection = TRUE;
	    }
#ଯଦି defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ଅନ୍ୟଥା ଯଦି (type == DT_LTAG)
	    {
		ଯଦି (add_llist_tags(tag, ଅଙ୍କ_matches, matches) == FAIL)
		    ଯାଅ end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    ଯଦି (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_ସଂଖ୍ୟା(ଖାଲି);
		ଯଦି (i <= 0 || i > ଅଙ୍କ_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ଯଦି (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    ଭାଙ୍ଗ;
		}
		cur_match = i - 1;
	    }

	    ଯଦି (cur_match >= ଅଙ୍କ_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ଯଦି ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == ଖାଲି)
		{
		    ଯଦି (ଅଙ୍କ_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    ଅନ୍ୟଥା
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_ସନ୍ଦେଶ = TRUE;
		}
		cur_match = ଅଙ୍କ_matches - 1;
	    }
	    ଯଦି (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ଯଦି (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#ଯଦି defined(FEAT_QUICKFIX)
	    ଅନ୍ୟଥା ଯଦି (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ଯଦି (nofile_fname != ଖାଲି && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    ଯଦି (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (ଅଙ୍କ_matches > 1 || ic)
		&& !skip_ସନ୍ଦେଶ)
	    {
		// Give an indication of the number of matching tags
		sprintf((ଅକ୍ଷର *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				ଅଙ୍କ_matches,
				max_ଅଙ୍କ_matches != MAXCOL ? _(" or more") : "");
		ଯଦି (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ଯଦି ((ଅଙ୍କ_matches > prev_ଅଙ୍କ_matches || new_tag)
							   && ଅଙ୍କ_matches > 1)
		{
		    ଯଦି (ic)
			ସନ୍ଦେଶ_attr((ଅକ୍ଷର *)IObuff, HL_ATTR(HLF_W));
		    ଅନ୍ୟଥା
			ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
		    ସନ୍ଦେଶ_scroll = TRUE;	// don't overwrite this message
		}
		ଅନ୍ୟଥା
		    give_warning(IObuff, ic);
		ଯଦି (ic && !ସନ୍ଦେଶ_scrolled && ସନ୍ଦେଶ_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ଯଦି defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#ଯଦି defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, ଖାଲି, -1);
#endif

	    ଯଦି (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ଯଦି ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_ଅଙ୍କ_matches != MAXCOL
					     || cur_match < ଅଙ୍କ_matches - 1)))
		{
		    error_cur_match = cur_match;
		    ଯଦି (use_tagstack)
			--tagstackidx;
		    ଯଦି (type == DT_PREV)
			--cur_match;
		    ଅନ୍ୟଥା
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    ଜାରି;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ଅନ୍ୟଥା
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ଯଦି (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	ଭାଙ୍ଗ;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    ଯଦି (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    ଫେରନ୍ତୁ jumped_to_tag;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
print_tag_list(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_tag,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_tagstack,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagstackidx = curwin->w_tagstackidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		taglen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagp.tagname_end - tagp.tagname + 2);
    ଯଦି (taglen < 18)
	taglen = 18;
    ଯଦି (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    ଯଦି (ସନ୍ଦେଶ_col == 0)
	ସନ୍ଦେଶ_didout = FALSE;	// overwrite previous message
    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    ସନ୍ଦେଶ_clr_eos();
    taglen_advance(taglen);
    ସନ୍ଦେଶ_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    ପାଇଁ (i = 0; i < ଅଙ୍କ_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	ଯଦି (!new_tag && (
#ଯଦି defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	ଅନ୍ୟଥା
	    *IObuff = ' ';
	vim_snprintf((ଅକ୍ଷର *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)IObuff);
	ଯଦି (tagp.tagkind != ଖାଲି)
	    ସନ୍ଦେଶ_outtrans_len(tagp.tagkind,
			  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagp.tagkind_end - tagp.tagkind));
	ସନ୍ଦେଶ_advance(13);
	ସନ୍ଦେଶ_outtrans_len_attr(tagp.tagname,
			   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	ସନ୍ଦେଶ_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	ଯଦି (p != ଖାଲି)
	{
	    ସନ୍ଦେଶ_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ଯଦି (ସନ୍ଦେଶ_col > 0)
	    ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)
	    ଭାଙ୍ଗ;
	ସନ୍ଦେଶ_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	ଯଦି (command_end != ଖାଲି)
	{
	    p = command_end + 3;
	    ଯେତେବେଳେ (*p && *p != '\r' && *p != '\n')
	    {
		ଯେତେବେଳେ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ଯଦି (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ଜାରି;
		}
		// skip "kind:<kind>" and "<kind>"
		ଯଦି (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    ଜାରି;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ଯେତେବେଳେ (*p && *p != '\r' && *p != '\n')
		{
		    ଯଦି (ସନ୍ଦେଶ_col + ptr2cells(p) >= cmdline_width)
		    {
			ସନ୍ଦେଶ_putchar('\n');
			ଯଦି (got_int)
			    ଭାଙ୍ଗ;
			ସନ୍ଦେଶ_advance(15);
		    }
		    p = ସନ୍ଦେଶ_outtrans_one(p, attr);
		    ଯଦି (*p == TAB)
		    {
			ସନ୍ଦେଶ_puts_attr(" ", attr);
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (*p == ':')
			attr = 0;
		}
	    }
	    ଯଦି (ସନ୍ଦେଶ_col > 15)
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
		ସନ୍ଦେଶ_advance(15);
	    }
	}
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ଯଦି (*p == '/' || *p == '?')
	{
	    ++p;
	    ଯଦି (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ଯେତେବେଳେ (p != command_end && vim_isspace(*p))
	    ++p;

	ଯେତେବେଳେ (p != command_end)
	{
	    ଯଦି (ସନ୍ଦେଶ_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    ସନ୍ଦେଶ_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ଯଦି (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ଯଦି (*p == TAB)
	    {
		ସନ୍ଦେଶ_putchar(' ');
		++p;
	    }
	    ଅନ୍ୟଥା
		p = ସନ୍ଦେଶ_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ଯଦି (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ଭାଙ୍ଗ;
	    // don't display matching '/' or '?'
	    ଯଦି (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ଭାଙ୍ଗ;
	}
	ଯଦି (ସନ୍ଦେଶ_col)
	    ସନ୍ଦେଶ_putchar('\n');
	ui_breakcheck();
    }
    ଯଦି (got_int)
	got_int = FALSE;	// only stop the listing
}

#ଯଦି defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_llist_tags(
    char_u	*tag,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ଯଦି (list == ଖାଲି || fname == ଖାଲି || cmd == ଖାଲି)
    {
	vim_free(cmd);
	vim_free(fname);
	ଯଦି (list != ଖାଲି)
	    list_free(list);
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; ++i)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagp.tagname_end - tagp.tagname);
	ଯଦି (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	ଯଦି (p == ଖାଲି)
	    ଜାରି;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ଯଦି (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((ଅକ୍ଷର *)tagp.command);
	ଅନ୍ୟଥା
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    ଯଦି (cmd_end == ଖାଲି)
	    {
		ପାଇଁ (p = tagp.command;
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
	    ଯଦି (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    ଯଦି (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ଯଦି (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cmd_end - cmd_start + 1);
	    ଯଦି (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    ଯଦି (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ଯଦି ((dict = dict_alloc()) == ଖାଲି)
	    ଜାରି;
	ଯଦି (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ଜାରି;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_ସଂଖ୍ୟା(dict, "lnum", lnum);
	ଯଦି (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, ଖାଲି);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Free cached tags.
 */
    ଶୂନ୍ୟ
tag_freematch(ଶୂନ୍ୟ)
{
    VIM_CLEAR(tagmatchname);
}

    ସ୍ଥିର ଶୂନ୍ୟ
taglen_advance(ପୂର୍ଣ୍ଣସଂଖ୍ୟା l)
{
    ଯଦି (l == MAXCOL)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ସନ୍ଦେଶ_advance(24);
    }
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_advance(13 + l);
}

/*
 * Print the tag stack
 */
    ଶୂନ୍ୟ
do_tags(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagstackidx = curwin->w_tagstackidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    ସନ୍ଦେଶ_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    ପାଇଁ (i = 0; i < tagstacklen; ++i)
    {
	ଯଦି (tagstack[i].tagname != ଖାଲି)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ଯଦି (name == ଖାଲି)	    // file name not available
		ଜାରି;

	    ସନ୍ଦେଶ_putchar('\n');
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    ସନ୍ଦେଶ_outtrans(IObuff);
	    ସନ୍ଦେଶ_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    ଯଦି (tagstackidx == tagstacklen)	// idx at top of stack
	ସନ୍ଦେଶ_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯେତେବେଳେ (len > 0)
    {
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOUPPER_ASC(*s1) - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOUPPER_ASC(*s2);
	ଯଦି (i != 0)
	    ଫେରନ୍ତୁ i;			// this character different
	ଯଦି (*s1 == NUL)
	    ଭାଙ୍ଗ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ଫେରନ୍ତୁ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*pat;		// the pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prepare_pats(pat_T *pats, ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    ଯଦି (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ଯଦି (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	ଅନ୍ୟଥା ଯଦି (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	ଯଦି (pats->head == pats->pat)
	    pats->headlen = 0;
	ଅନ୍ୟଥା
	    ପାଇଁ (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		ଯଦି (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != ଖାଲି)
		    ଭାଙ୍ଗ;
	ଯଦି (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ଯଦି (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ଅନ୍ୟଥା
	pats->regmatch.regprog = ଖାଲି;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ସ୍ଥିର tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    ଯଦି (VIM_ISDIGIT(*cmd))
	ଫେରନ୍ତୁ *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    ଯଦି (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	ଫେରନ୍ତୁ (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    ଯଦି (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ଖାଲି)
	ଫେରନ୍ତୁ TAGCMD_INVALID;
    ଫେରନ୍ତୁ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*match_ଗଣନା,	// here the number of tags found will be placed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ntags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    ଯଦି (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ଖାଲି
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	ଫେରନ୍ତୁ FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    ଯଦି ((d = dict_alloc_lock(VAR_FIXED)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (!(flags & TAG_INS_COMP) && tag->user_data != ଖାଲି)
	dict_add_string(d, "user_data", tag->user_data);
    ଯଦି (buf_ffname != ଖାଲି)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((ଅକ୍ଷର *)flagString, ଆକାର(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ଯଦି (result == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (rettv.v_type == VAR_SPECIAL && rettv.vval.v_ସଂଖ୍ୟା == VVAL_NULL)
    {
	clear_tv(&rettv);
	ଫେରନ୍ତୁ NOTDONE;
    }
    ଯଦି (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_ମୂଲ୍ୟ_from_tagfunc));
	ଫେରନ୍ତୁ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	dict_iterator_T	iter;
	char_u		*dict_ଚାବି;
	typval_T	*tv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_extra = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_only = flags & TAG_NAMES;

	ଯଦି (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_ମୂଲ୍ୟ_from_tagfunc));
	    ଭାଙ୍ଗ;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# ଅନ୍ୟଥା
	len = 2;
# endif
	res_name = ଖାଲି;
	res_fname = ଖାଲି;
	res_cmd = ଖାଲି;
	res_kind = ଖାଲି;

	dict_iterate_start(&item->li_tv, &iter);
	ଯେତେବେଳେ (ଖାଲି != (dict_ଚାବି = dict_iterate_next(&iter, &tv)))
	{
	    ଯଦି (tv->v_type != VAR_STRING || tv->vval.v_string == ଖାଲି)
		ଜାରି;

	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ଯଦି (STRCMP(dict_ଚାବି, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ଜାରି;
	    }
	    ଯଦି (STRCMP(dict_ଚାବି, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ଜାରି;
	    }
	    ଯଦି (STRCMP(dict_ଚାବି, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ଜାରି;
	    }
	    has_extra = 1;
	    ଯଦି (STRCMP(dict_ଚାବି, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		ଜାରି;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(dict_ଚାବି) + 1;
	}

	ଯଦି (has_extra)
	    len += 2;	// need space for ;"

	ଯଦି (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_ମୂଲ୍ୟ_from_tagfunc));
	    ଭାଙ୍ଗ;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	ଯଦି (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_ମୂଲ୍ୟ_from_tagfunc));
	    ଭାଙ୍ଗ;
	}
	ଯଦି (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ଯଦି (name_only)
	    mfp = vim_strsave(res_name);
	ଅନ୍ୟଥା
	    mfp = alloc(ଆକାର(char_u) + len + 1);

	ଯଦି (mfp == ଖାଲି)
	    ଜାରି;

	ଯଦି (!name_only)
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

	    ଯଦି (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ଯଦି (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ଯଦି (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		ଯେତେବେଳେ (ଖାଲି != (dict_ଚାବି = dict_iterate_next(&iter, &tv)))
		{
		    ଯଦି (tv->v_type != VAR_STRING || tv->vval.v_string == ଖାଲି)
			ଜାରି;

		    ଯଦି (STRCMP(dict_ଚାବି, "name") == 0)
			ଜାରି;
		    ଯଦି (STRCMP(dict_ଚାବି, "filename") == 0)
			ଜାରି;
		    ଯଦି (STRCMP(dict_ଚାବି, "cmd") == 0)
			ଜାରି;
		    ଯଦି (STRCMP(dict_ଚାବି, "kind") == 0)
			ଜାରି;

		    *p++ = TAB;
		    STRCPY(p, dict_ଚାବି);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	ଯଦି (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	ଅନ୍ୟଥା
	{
	    vim_free(mfp);
	    ଭାଙ୍ଗ;
	}
    }

    clear_tv(&rettv);

    *match_ଗଣନା = ntags;
    ଫେରନ୍ତୁ result;
}
#endif

/*
 * State information used during a tag search
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    tagsearch_state_T	state;		// tag search state
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;			// flags used for tag search
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tag_file_sorted;	// !_TAG_FILE_SORTED value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		get_searchpat;		// used for 'showfulltag'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		help_only;		// only search for help tags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_open;		// did open a tag file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_txt;			// flag of file extension
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match_ଗଣନା;		// number of matches found
    garray_T	ga_match[MT_ଗଣନା];	// stores matches in sequence
    hashtab_T	ht_match[MT_ଗଣନା];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			mincount)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = ଖାଲି;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
    st->orgpat->regmatch.regprog = ଖାଲି;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = ଖାଲି;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_ଗଣନା = 0;
    st->stop_searching = FALSE;

    ପାଇଁ (mtt = 0; mtt < MT_ଗଣନା; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], ଆକାର(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    ଯଦି (st->tag_fname == ଖାଲି
	    || st->lbuf == ଖାଲି
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == ଖାଲି
#endif
       )
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Free the state used by find_tags()
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_in_help_init(findtags_state_T *st)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ଯଦି (st->is_txt)
	STRCPY(st->help_lang, "en");
    ଅନ୍ୟଥା
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(st->tag_fname);
	ଯଦି (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	ଅନ୍ୟଥା
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ଯଦି (st->help_lang_find != ଖାଲି
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	ଫେରନ୍ତୁ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ଯଦି ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == ଖାଲି
	    && curbuf->b_fname != ଖାଲି
	    && (i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ଅନ୍ୟଥା
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	ପାଇଁ (s = p_hlg; *s != NUL; ++s)
	{
	    ଯଦି (STRNICMP(s, st->help_lang, 2) == 0)
		ଭାଙ୍ଗ;
	    ++st->help_pri;
	    ଯଦି ((s = vim_strchr(s, ',')) == ଖାଲି)
		ଭାଙ୍ଗ;
	}
	ଯଦି (s == ଖାଲି || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ଯଦି (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    ଯଦି (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	ଫେରନ୍ତୁ NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_ଗଣନା,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    ଫେରନ୍ତୁ retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
ସ୍ଥିର ସଂରଚନା
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
emacs_tags_incstack_free(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ଖାଲି;
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
    ସ୍ଥିର ଶୂନ୍ୟ
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ଯଦି (vim_fgets(st->ebuf, LSIZE, st->fp))
	ଫେରନ୍ତୁ;

    ପାଇଁ (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ଯଦି (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	ଫେରନ୍ତୁ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    ଯଦି (incstack[incstack_idx].etag_fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ଖାଲି;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    ଯଦି (fullpath_ebuf != ଖାଲି)
    {
	st->fp = mch_fopen((ଅକ୍ଷର *)fullpath_ebuf, "r");
	ଯଦି (st->fp != ଖାଲି)
	{
	    ଯଦି (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ଯଦି (st->fp == ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
emacs_tags_file_eof(findtags_state_T *st)
{
    ଯଦି (!incstack_idx)	// reached end of file. stop processing.
	ଫେରନ୍ତୁ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
emacs_tags_parse_ଧାଡ଼ି(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ଯଦି (p_7f == ଖାଲି)
    {
etag_fail:
	ଯଦି (vim_strchr(lbuf, '\n') != ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	// Truncated line.  Ignore it.
	ଯଦି (p_verbose >= 5)
	{
	    verbose_enter();
	    ସନ୍ଦେଶ(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	ଫେରନ୍ତୁ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ଯଦି (p == ଖାଲି)
	p = p_7f + 1;
    ଅନ୍ୟଥା
	++p;

    ଯଦି (!VIM_ISDIGIT(*p))	    // check for start of line number
	ଯାଅ etag_fail;
    tagp->command = p;

    ଯଦି (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    ଅନ୍ୟଥା			    // second format: isolate tagname
    {
	ଯଦି (p_7f == lbuf)
	    ଯାଅ etag_fail;

	// find end of tagname
	ପାଇଁ (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ଯଦି (p == lbuf)
		ଯାଅ etag_fail;
	tagp->tagname_end = p + 1;
	ଯେତେବେଳେ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର tags_read_status_T
findtags_get_next_ଧାଡ଼ି(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    ଯଦି (st->state == TS_ଦ୍ୱିମିକ)
    {
	offset = sinfo_p->ତଳ_offset + ((sinfo_p->ଉପର_offset
						- sinfo_p->ତଳ_offset) / 2);
	ଯଦି (offset == sinfo_p->curr_offset)
	    ଫେରନ୍ତୁ TAGS_READ_EOF; // End the binary search without a match.
	ଅନ୍ୟଥା
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    ଅନ୍ୟଥା ଯଦି (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	ଯଦି (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ଯଦି (st->state == TS_ଦ୍ୱିମିକ || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	ଯଦି (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    ଯଦି (sinfo_p->curr_offset == sinfo_p->ଉପର_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->ତଳ_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->ତଳ_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	ଯେତେବେଳେ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	ଯଦି (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ଫେରନ୍ତୁ TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ଅନ୍ୟଥା
    {
	// skip empty and blank lines
	କର
	{
#ifdef FEAT_CSCOPE
	    ଯଦି (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    ଅନ୍ୟଥା
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} ଯେତେବେଳେ (!eof && vim_isblankline(st->lbuf));

	ଯଦି (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    ଯଦି (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		ଫେରନ୍ତୁ TAGS_READ_IGNORE;
#endif
	    ଫେରନ୍ତୁ TAGS_READ_EOF;
	}
    }

    ଫେରନ୍ତୁ TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ଯଦି (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	ଫେରନ୍ତୁ TRUE;

    // Process the header line.
    ଯଦି (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    ଯଦି (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	ପାଇଁ (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_start_state_handler(
    findtags_state_T	*st,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ଯଦି (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	ଫେରନ୍ତୁ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    ଯଦି (st->linear || use_cscope)
#ଅନ୍ୟଥା
    ଯଦି (st->linear)
#endif
	st->state = TS_LINEAR;
    ଅନ୍ୟଥା ଯଦି (st->tag_file_sorted == NUL)
	st->state = TS_ଦ୍ୱିମିକ;
    ଅନ୍ୟଥା ଯଦି (st->tag_file_sorted == '1')
	st->state = TS_ଦ୍ୱିମିକ;
    ଅନ୍ୟଥା ଯଦି (st->tag_file_sorted == '2')
    {
	st->state = TS_ଦ୍ୱିମିକ;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ଅନ୍ୟଥା
	st->state = TS_LINEAR;

    ଯଦି (st->state == TS_ଦ୍ୱିମିକ && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ଯଦି (st->state == TS_ଦ୍ୱିମିକ)
    {
	ଯଦି (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	ଅନ୍ୟଥା
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->ତଳ_offset = 0;
	    sinfo_p->ତଳ_char = 0;
	    sinfo_p->ଉପର_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->ଉପର_char = 0xff;
	}
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର tagmatch_status_T
findtags_parse_ଧାଡ଼ି(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmplen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ଯଦି (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	ଯଦି (tagpp->tagname_end == ଖାଲି)
	    // Corrupted tag line.
	    ଫେରନ୍ତୁ TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagpp->tagname_end - tagpp->tagname);
	ଯଦି (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ଯଦି ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ଅନ୍ୟଥା ଯଦି (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    ଫେରନ୍ତୁ TAG_MATCH_NEXT;

	ଯଦି (st->state == TS_ଦ୍ୱିମିକ)
	{
	    // Simplistic check for unsorted tags file.
	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tagpp->tagname[0];
	    ଯଦି (margs->sortic)
		i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOUPPER_ASC(tagpp->tagname[0]);
	    ଯଦି (i < sinfo_p->ତଳ_char || i > sinfo_p->ଉପର_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ଯଦି (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    ଅନ୍ୟଥା
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ଯଦି (tagcmp == 0)
	    {
		ଯଦି (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ଅନ୍ୟଥା ଯଦି (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ଯଦି (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		ଫେରନ୍ତୁ TAG_MATCH_NEXT;
	    }
	    ଯଦି (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		ଯଦି (sinfo_p->curr_offset < sinfo_p->ଉପର_offset)
		{
		    sinfo_p->ତଳ_offset = sinfo_p->curr_offset;
		    ଯଦି (margs->sortic)
			sinfo_p->ତଳ_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ଅନ୍ୟଥା
			sinfo_p->ତଳ_char = tagpp->tagname[0];
		    ଫେରନ୍ତୁ TAG_MATCH_NEXT;
		}
	    }
	    ଯଦି (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->ଉପର_offset)
	    {
		sinfo_p->ଉପର_offset = sinfo_p->curr_offset;
		ଯଦି (margs->sortic)
		    sinfo_p->ଉପର_char = TOUPPER_ASC(tagpp->tagname[0]);
		ଅନ୍ୟଥା
		    sinfo_p->ଉପର_char = tagpp->tagname[0];
		ଫେରନ୍ତୁ TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    ଫେରନ୍ତୁ TAG_MATCH_STOP;
	}
	ଅନ୍ୟଥା ଯଦି (st->state == TS_SKIP_BACK)
	{
	    ଯଦି (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    ଅନ୍ୟଥା
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ଫେରନ୍ତୁ TAG_MATCH_NEXT;
	}
	ଅନ୍ୟଥା ଯଦି (st->state == TS_STEP_FORWARD)
	{
	    ଯଦି (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		ଯଦି ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    ଫେରନ୍ତୁ TAG_MATCH_STOP;	// past last match
		ଅନ୍ୟଥା
		    ଫେରନ୍ତୁ TAG_MATCH_NEXT;	// before first match
	    }
	}
	ଅନ୍ୟଥା
	    // skip this match if it can't match
	    ଯଦି (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		ଫେରନ୍ତୁ TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	ଯଦି (tagpp->fname_end == ଖାଲି)
	    status = FAIL;
	ଅନ୍ୟଥା
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    ଅନ୍ୟଥା
	status = parse_tag_ଧାଡ଼ି(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    ଯଦି (status == FAIL)
	ଫେରନ୍ତୁ TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    ଯଦି (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    ଫେରନ୍ତୁ TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
findtags_matchargs_init(findtags_match_args_T *margs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagpp->tagname_end - tagpp->tagname);
    ଯଦି (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ଯଦି (st->orgpat->len != cmplen)
	match = FALSE;
    ଅନ୍ୟଥା
    {
	ଯଦି (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ଯଦି (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ଅନ୍ୟଥା
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    ଯଦି (!match && st->orgpat->regmatch.regprog != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ଯଦି (match)
	{
	    margs->matchoff = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ଯଦି (st->orgpat->regmatch.rm_ic)
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

    ଫେରନ୍ତୁ match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    conv_ଧାଡ଼ି = string_convert(&st->vimconv, st->lbuf, ଖାଲି);
    ଯଦି (conv_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Copy or swap lbuf and conv_line.
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(conv_ଧାଡ଼ି) + 1;
    ଯଦି (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_ଧାଡ଼ି;
	st->lbuf_size = len;
    }
    ଅନ୍ୟଥା
    {
	STRCPY(st->lbuf, conv_ଧାଡ଼ି);
	vim_free(conv_ଧାଡ଼ି);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_only = (st->flags & TAG_NAMES);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mtt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_current;		// file name matches
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    ଯଦି (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ଅନ୍ୟଥା
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
	ଯଦି (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	ଯଦି (is_static)
	{
	    ଯଦି (is_current)
		mtt = MT_ST_CUR;
	    ଅନ୍ୟଥା
		mtt = MT_ST_OTH;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (is_current)
		mtt = MT_GL_CUR;
	    ଅନ୍ୟଥା
		mtt = MT_GL_OTH;
	}
	ଯଦି (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	ଯଦି (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ଯଦି (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#ଅନ୍ୟଥା
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(ଆକାର(char_u) + len + 10 + ML_EXTRA + 1);
	ଯଦି (mfp != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା heuristic;

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
	    sprintf((ଅକ୍ଷର *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    ଅନ୍ୟଥା ଯଦି (name_only)
    {
	ଯଦି (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    ଯଦି (*temp_end == '/')
		ଯେତେବେଳେ (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    ଯଦି (tagpp->command + 2 < temp_end)
	    {
		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		ଯଦି (mfp != ଖାଲି)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ଅନ୍ୟଥା
		mfp = ଖାଲି;
	    st->get_searchpat = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(ଆକାର(char_u) + len + 1);
	    ଯଦି (mfp != ଖାଲି)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ଯଦି (State & MODE_ଭର୍ତ୍ତି)
		st->get_searchpat = p_sft;
	}
    }
    ଅନ୍ୟଥା
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
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tag_fname_len + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	ଯଦି (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ebuf_len + 1;
	}
	ଅନ୍ୟଥା
	    ++len;
#endif
	mfp = alloc(ଆକାର(char_u) + len + 1);
	ଯଦି (mfp != ଖାଲି)
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
	    ଯଦି (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    ଅନ୍ୟଥା
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    ଯଦି (mfp != ଖାଲି)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	ଯଦି (use_cscope)
	    ++*hash;
	ଅନ୍ୟଥା
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	ଯଦି (HASHITEM_EMPTY(hi))
	{
	    ଯଦି (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		ଫେରନ୍ତୁ FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_ଗଣନା++;
	}
	ଅନ୍ୟଥା
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	ସନ୍ଧାନ_info;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			retval;
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(ସନ୍ଧାନ_info);

    // Read and parse the lines in the file one by one
    ପାଇଁ (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ଯଦି (st->state == TS_ଦ୍ୱିମିକ || st->state == TS_SKIP_BACK)
	    ଧାଡ଼ି_breakcheck();
	ଅନ୍ୟଥା
	    fast_breakcheck();
	ଯଦି ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ଚାବିଗୁଡ଼ିକ(30, FALSE);
	ଯଦି (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ଭାଙ୍ଗ;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ଯଦି (st->mincount == TAG_MANY && st->match_ଗଣନା >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (st->get_searchpat)
	    ଯାଅ ଧାଡ଼ି_read_in;

	retval = findtags_get_next_ଧାଡ଼ି(st, &ସନ୍ଧାନ_info);
	ଯଦି (retval == TAGS_READ_IGNORE)
	    ଜାରି;
	ଯଦି (retval == TAGS_READ_EOF)
	    ଭାଙ୍ଗ;

ଧାଡ଼ି_read_in:

	ଯଦି (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ଯଦି (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    ଜାରି;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ଯଦି (st->state == TS_START)
	{
	    ଯଦି (findtags_start_state_handler(st, &margs->sortic, &ସନ୍ଧାନ_info) == FALSE)
		ଜାରି;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ଯଦି (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    ଯଦି (st->lbuf == ଖାଲି)
	    {
		ଯଦି (st->fp != ଖାଲି)
		    fclose(st->fp);
		st->fp = ଖାଲି;
		st->stop_searching = TRUE;
		ଫେରନ୍ତୁ;
	    }

	    ଯଦି (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, ସନ୍ଧାନ_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    ସନ୍ଧାନ_info.curr_offset = 0;
	    ଜାରି;
	}

	retval = findtags_parse_ଧାଡ଼ି(st, &tagp, margs, &ସନ୍ଧାନ_info);
	ଯଦି (retval == TAG_MATCH_NEXT)
	    ଜାରି;
	ଯଦି (retval == TAG_MATCH_STOP)
	    ଭାଙ୍ଗ;
	ଯଦି (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    ଯଦି (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    ଫେରନ୍ତୁ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ଯଦି (findtags_match_tag(st, &tagp, margs))
	{
	    ଯଦି (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ଭାଙ୍ଗ;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = ଖାଲି;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    ଯଦି (use_cscope)
	st->fp = ଖାଲି;	    // avoid GCC warning
    ଅନ୍ୟଥା
#endif
    {
#ifdef FEAT_MULTI_LANG
	ଯଦି (curbuf->b_help)
	{
	    ଯଦି (!findtags_in_help_init(st))
		ଫେରନ୍ତୁ;
	}
#endif

	st->fp = mch_fopen((ଅକ୍ଷର *)st->tag_fname, "r");
	ଯଦି (st->fp == ଖାଲି)
	    ଫେରନ୍ତୁ;

	ଯଦି (p_verbose >= 5)
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

    ଯଦି (st->fp != ଖାଲି)
    {
	fclose(st->fp);
	st->fp = ଖାଲି;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    ଯଦି (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, ଖାଲି, ଖାଲି);

    ଯଦି (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    ଯଦି (st->match_ଗଣନା >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mtt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*mfp;
    char_u	*p;

    ଯଦି (st->match_ଗଣନା > 0)
	matches = ALLOC_MULT(char_u *, st->match_ଗଣନା);
    ଅନ୍ୟଥା
	matches = ଖାଲି;
    st->match_ଗଣନା = 0;
    ପାଇଁ (mtt = 0; mtt < MT_ଗଣନା; ++mtt)
    {
	ପାଇଁ (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    ଯଦି (matches == ଖାଲି)
		vim_free(mfp);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    ପାଇଁ (p = mfp + 1; *p != NUL; ++p)
			ଯଦି (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_ଗଣନା++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    ଫେରନ୍ତୁ st->match_ଗଣନା;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_tags(
    char_u	*pat,			// pattern to search for
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_file;		// trying first tag file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;		// return value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*saved_pat = ଖାଲି;		// copy of pat[]
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_re = (flags & TAG_REGEXP);	// regexp used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_cscope = (flags & TAG_CSCOPE);
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose = (flags & TAG_VERBOSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    ବାଛ (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	ସ୍ଥିତି TC_FOLLOWIC:		 ଭାଙ୍ଗ;
	ସ୍ଥିତି TC_IGNORE:    p_ic = TRUE;  ଭାଙ୍ଗ;
	ସ୍ଥିତି TC_MATCH:     p_ic = FALSE; ଭାଙ୍ଗ;
	ସ୍ଥିତି TC_FOLLOWSCS: p_ic = ignorecase(pat); ଭାଙ୍ଗ;
	ସ୍ଥିତି TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ଭାଙ୍ଗ;
    }

    help_save = curbuf->b_help;

    ଯଦି (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	ଯାଅ findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    ଯଦି (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    ଅନ୍ୟଥା ଯଦି (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    ଯଦି (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ଯଦି (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ଯଦି (saved_pat != ଖାଲି)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    ଯଦି (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ଯଦି (has_re && st.orgpat->regmatch.regprog == ଖାଲି)
	ଯାଅ findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ଯଦି (retval != NOTDONE)
	ଯାଅ findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ଯଦି ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == ଖାଲି
	    && curbuf->b_fname != ଖାଲି
	    && (i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(curbuf->b_fname)) > 4
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
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      ପାଇଁ (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  ଯଦି (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      ଭାଙ୍ଗ;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	ଯଦି (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ଯଦି (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ଭାଙ୍ଗ;
#ifdef FEAT_CSCOPE
	ଯଦି (use_cscope)
	    ଭାଙ୍ଗ;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ଯଦି (!st.stop_searching)
    {
	ଯଦି (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ଯଦି (retval == FAIL)
	st.match_ଗଣନା = 0;

    *ଅଙ୍କ_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    ଫେରନ୍ତୁ retval;
}

ସ୍ଥିର garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
found_tagfile_cb(char_u *fname, ଶୂନ୍ୟ *cookie UNUSED)
{
    ଯଦି (ga_grow(&tag_fnames, 1) == FAIL)
	ଫେରନ୍ତୁ;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_tag_stuff(ଶୂନ୍ୟ)
{
    ga_clear_strings(&tag_fnames);
    ଯଦି (curwin != ଖାଲି)
	do_tag(ଖାଲି, DT_FREE, 0, 0, 0);
    tag_freematch();

# ଯଦି defined(FEAT_QUICKFIX)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tagfname(
    tagname_T	*tnp,	// holds status info
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ଖାଲି;
    char_u		*r_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    ଯଦି (first)
	CLEAR_POINTER(tnp);

    ଯଦି (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ଯଦି (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, ଆକାର(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ଅନ୍ୟଥା
		    "doc/tags doc/tags-??"
# endif
#ଅନ୍ୟଥା
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, ଖାଲି);
	}

	ଯଦି (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ଯଦି (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		ଫେରନ୍ତୁ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    ପାଇଁ (i = 0; i < tag_fnames.ga_len; ++i)
		ଯଦି (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    ଫେରନ୍ତୁ FAIL; // avoid duplicate file names
	}
	ଅନ୍ୟଥା
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	ଯଦି (tnp->tn_tags == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    ପାଇଁ (;;)
    {
	ଯଦି (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_ସନ୍ଧାନ_ctx);
	    ଯଦି (fname != ଖାଲି)
		ଭାଙ୍ଗ;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    char_u  *filename = ଖାଲି;

	    // Stop when used all parts of 'tags'.
	    ଯଦି (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_ସନ୍ଧାନ_ctx);
		tnp->tn_ସନ୍ଧାନ_ctx = ଖାଲି;
		ଫେରନ୍ତୁ FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (ଶୂନ୍ୟ)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    ଯଦି (r_ptr != ଖାଲି)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_ସନ୍ଧାନ_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_ସନ୍ଧାନ_ctx, TRUE, curbuf->b_ffname);
	    ଯଦି (tnp->tn_ସନ୍ଧାନ_ctx != ଖାଲି)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    ଫେରନ୍ତୁ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    ଶୂନ୍ୟ
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_ସନ୍ଧାନ_ctx);
    tnp->tn_ସନ୍ଧାନ_ctx = ଖାଲି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_tag_ଧାଡ଼ି(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    ଯଦି (is_etag)
	// emacs-style tag file
	ଫେରନ୍ତୁ emacs_tags_parse_ଧାଡ଼ି(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    ଯଦି (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    ଯଦି (*p != NUL)
	++p;
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ FAIL;
    tagp->command = p;

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ଯେତେବେଳେ ((p = vim_strchr(p, '\t')) != ଖାଲି)
    {
	++p;
	ଯଦି (STRNCMP(p, "file:", 5) == 0)
	    ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ସ୍ଥିର size_t
matching_ଧାଡ଼ି_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    ଫେରନ୍ତୁ (p - lbuf) + STRLEN(p);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    ଯଦି (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    ଅନ୍ୟଥା
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_ଧାଡ଼ି(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = ଖାଲି;
    tagp->user_data = ଖାଲି;
    tagp->tagline = 0;
    tagp->command_end = ଖାଲି;

    ଯଦି (retval != OK)
	ଫେରନ୍ତୁ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ଯଦି (find_extra(&p) == OK)
    {
	ଯଦି (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	ଅନ୍ୟଥା
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	ଯଦି (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ଯେତେବେଳେ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ଯଦି (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((ଅକ୍ଷର *)p + 5);
		ଯଦି (tagp->tagkind != ଖାଲି && tagp->user_data != ଖାଲି)
		    ଭାଙ୍ଗ;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ଯଦି (pc == ଖାଲି || (pt != ଖାଲି && pc > pt))
		    tagp->tagkind = p;
		ଯଦି (pt == ଖାଲି)
		    ଭାଙ୍ଗ;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ଯଦି (tagp->tagkind != ଖାଲି)
    {
	ପାଇଁ (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    ଯଦି (tagp->user_data != ଖାଲି)
    {
	ପାଇଁ (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ସ୍ଥିର char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

#ifdef FEAT_EMACS_TAGS
    ଯଦି (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ଅନ୍ୟଥା
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    ଯଦି (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    ଫେରନ୍ତୁ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,	// :ta with !
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = ଖାଲି;
    char_u	*fname;
    tagptrs_T	tagp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getfile_result = GETFILE_UNUSED;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଧାନ_options;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_no_hlsearch;
#endif
#ଯଦି defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ଖାଲି;
#endif
    char_u	*full_fname = ଖାଲି;
#ifdef FEAT_FOLDING
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_ଚାବି_Typed = ଚାବି_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isdigit = FALSE;

    ଯଦି (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ଫେରନ୍ତୁ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ଧାଡ଼ି_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ଯଦି (lbuf != ଖାଲି)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ଯଦି (pbuf == ଖାଲି || lbuf == ଖାଲି || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = ଖାଲି;
	ଯାଅ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ଯଦି (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    ପାଇଁ (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	ଯଦି (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ଭାଙ୍ଗ;
#endif
	*pbuf_end++ = *str++;
	ଯଦି (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    ଭାଙ୍ଗ;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    ଯଦି (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ଯଦି (isdigit)
	    str++;
	ଯଦି (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ଯଦି (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    ଅନ୍ୟଥା
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    ଯଦି (fname == ଖାଲି)
	ଯାଅ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ଯଦି (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ଖାଲି))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ଯଦି (nofile_fname == ଖାଲି)
	    nofile_fname = empty_option;
	ଯାଅ erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ଯଦି defined(FEAT_QUICKFIX)
    ଯଦି (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ଯଦି (!curwin->w_p_pvw)
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
    ଯଦି (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ଯଦି (existing_buf != ଖାଲି)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ଯଦି (swbuf_goto_win_with_buf(existing_buf) != ଖାଲି)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    ଯଦି (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	ଯଦି (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	ଯଦି ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	ଯଦି (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    ଯଦି (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    ଯାଅ erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    ଯଦି (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ଯଦି defined(FEAT_QUICKFIX)
	ଯଦି (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	ଅନ୍ୟଥା
#endif
	    keep_help_flag = curbuf->b_help;
    }

    ଯଦି (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, ଖାଲି, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ଯଦି (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ଯଦି (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ଯଦି (vim_strchr(p_cpo, CPO_TAGPAT) != ଖାଲି)
	    ସନ୍ଧାନ_options = 0;
	ଅନ୍ୟଥା
	    ସନ୍ଧାନ_options = ସନ୍ଧାନ_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	ଯଦି (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ଯଦି (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ଯଦି (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ଅନ୍ୟଥା
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ଯଦି (do_ସନ୍ଧାନ(ଖାଲି, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 ସନ୍ଧାନ_options, ଖାଲି))
		retval = OK;
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	found = 1;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ଯଦି (!do_ସନ୍ଧାନ(ଖାଲି, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 ସନ୍ଧାନ_options, ଖାଲି))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (ଶୂନ୍ୟ)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((ଅକ୍ଷର *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ଯଦି (!do_ସନ୍ଧାନ(ଖାଲି, '/', '/', pbuf, pbuflen, (long)1,
							 ସନ୍ଧାନ_options, ଖାଲି))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((ଅକ୍ଷର *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ଯଦି (!do_ସନ୍ଧାନ(ଖାଲି, '/', '/', pbuf, pbuflen, (long)1,
							 ସନ୍ଧାନ_options, ଖାଲି))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		ଯଦି (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ଅନ୍ୟଥା
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ଯଦି (found == 2 || !save_p_ic)
		    {
			ସନ୍ଦେଶ(_(e_couldnt_find_tag_just_guessing));
			ଯଦି (!ସନ୍ଦେଶ_scrolled && ସନ୍ଦେଶ_silent == 0)
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
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_secure = secure;

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
	    ଯଦି (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ଯଦି (ସନ୍ଧାନ_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	ଯଦି (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	ଯଦି (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ଯଦି (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    ଯଦି ((fdo_flags & FDO_TAG) && old_ଚାବି_Typed)
		foldOpenCursor();
#endif
	}

#ଯଦି defined(FEAT_QUICKFIX)
	ଯଦି (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	ଯଦି (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ଯଦି (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#ଯଦି defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ଅନ୍ୟଥା ଯଦି (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ଯଦି (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#ଯଦି defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ଯଦି (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#ଯଦି defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    ଫେରନ୍ତୁ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ସ୍ଥିର char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ଖାଲି;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ଯଦି (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       ଫେରନ୍ତୁ ଖାଲି;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ଯଦି (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ଖାଲି)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ଖାଲି,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ଯଦି (expanded_fname != ଖାଲି)
	    fname = expanded_fname;
    }

    ଯଦି ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	ଯଦି (retval != ଖାଲି)
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
    ଅନ୍ୟଥା
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    ଫେରନ୍ତୁ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_for_current(
#ifdef FEAT_EMACS_TAGS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FALSE;
    char_u  *fullname;

    ଯଦି (buf_ffname != ଖାଲି)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	ଯଦି (is_etag)
	    c = 0;	    // to shut up GCC
	ଅନ୍ୟଥା
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	ଯଦି (fullname != ଖାଲି)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	ଯଦି (!is_etag)
#endif
	    *fname_end = c;
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    ପାଇଁ (;;)
    {
	ଯଦି (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ଅନ୍ୟଥା ଯଦି (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ଯଦି (*str != first_char)
		str = ଖାଲି;
	    ଅନ୍ୟଥା
		++str;
	}
	ଅନ୍ୟଥା
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((ଅକ୍ଷର *)str, "|;\"");
	    ଯଦି (str != ଖାଲି)
	    {
		++str;
		ଭାଙ୍ଗ;
	    }

	}
	ଯଦି (str == ଖାଲି || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ଭାଙ୍ଗ;
	++str;	// skip ';'
	first_char = *str;
    }

    ଯଦି (str != ଖାଲି && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    ଶୂନ୍ୟ
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_tags(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tagnames,	// expand tag names
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    name_buf = alloc(name_buf_size);
    ଯଦି (name_buf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (tagnames)
	extra_flag = TAG_NAMES;
    ଅନ୍ୟଥା
	extra_flag = 0;
    ଯଦି (pat[0] == '/')
	ret = find_tags(pat + 1, ଅଙ୍କ_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    ଅନ୍ୟଥା
	ret = find_tags(pat, ଅଙ୍କ_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    ଯଦି (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 ପାଇଁ (i = 0; i < *ଅଙ୍କ_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     ଯଦି (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 ଯଦି (buf == ଖାଲି)
		 {
		     vim_free(name_buf);
		     ଫେରନ୍ତୁ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ଖାଲି && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    ଫେରନ୍ତୁ ret;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_tag_field(
    dict_T  *dict,
    ଅକ୍ଷର    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    // check that the field name doesn't exist yet
    ଯଦି (dict_has_ଚାବି(dict, field_name))
    {
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	ଫେରନ୍ତୁ FAIL;
    }
    buf = alloc(MAXPATHL);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (start != ଖାଲି)
    {
	ଯଦି (end == ଖାଲି)
	{
	    end = start + STRLEN(start);
	    ଯେତେବେଳେ (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - start);
	ଯଦି (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    ଫେରନ୍ତୁ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &ଅଙ୍କ_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MAXCOL, buf_fname);
    ଯଦି (ret != OK || ଅଙ୍କ_matches <= 0)
	ଫେରନ୍ତୁ ret;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_matches; ++i)
    {
	ଯଦି (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ଜାରି;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ଯଦି (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ଜାରି;
	}

	ଯଦି ((dict = dict_alloc()) == ଖାଲି)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ଜାରି;
	}

	full_fname = tag_full_fname(&tp);
	ଯଦି (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    ଖାଲି) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_ସଂଖ୍ୟା(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	ଯଦି (tp.command_end != ଖାଲି)
	{
	    ପାଇଁ (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ଯଦି (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ଅନ୍ୟଥା ଯଦି (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ଯେତେବେଳେ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - n);
		    ଯଦି (*p == ':' && len > 0)
		    {
			s = ++p;
			ଯେତେବେଳେ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ଯଦି (add_tag_field(dict, (ଅକ୍ଷର *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ଅନ୍ୟଥା
			// Skip field without colon.
			ଯେତେବେଳେ (*p != NUL && *p >= ' ')
			    ++p;
		    ଯଦି (*p == NUL)
			ଭାଙ୍ଗ;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    ଫେରନ୍ତୁ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_ସଂଖ୍ୟା(retdict, "matchnr", tag->cur_match + 1);
    dict_add_ସଂଖ୍ୟା(retdict, "bufnr", tag->cur_fnum);
    ଯଦି (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    ଯଦି ((pos = list_alloc_id(aid_tagstack_from)) == ଖାଲି)
	ଫେରନ୍ତୁ;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_ସଂଖ୍ୟା(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_ସଂଖ୍ୟା(pos, (varnumber_T)fmark->mark.lnum);
    list_append_ସଂଖ୍ୟା(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_ସଂଖ୍ୟା(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    ଶୂନ୍ୟ
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    dict_T	*d;

    dict_add_ସଂଖ୍ୟା(retdict, "length", wp->w_tagstacklen);
    dict_add_ସଂଖ୍ୟା(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_ବସ୍ତୁଗୁଡ଼ିକ);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;
    dict_add_list(retdict, "items", l);

    ପାଇଁ (i = 0; i < wp->w_tagstacklen; i++)
    {
	ଯଦି ((d = dict_alloc_id(aid_tagstack_details)) == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ଫେରନ୍ତୁ;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tagstack_clear(win_T *wp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // Free the current tag stack
    ପାଇଁ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    tagstack_clear_entry(&tagstack[0]);
    ପାଇଁ (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tagstack_ଠେଲ_item(
	win_T	*wp,
	char_u	*tagname,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur_fnum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur_match,
	pos_T	mark,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ଯଦି (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    ଯଦି (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tagstack_ଠେଲ_ବସ୍ତୁଗୁଡ଼ିକ(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ଯଦି (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ଖାଲି)
	    ଜାରି;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ଯଦି ((di = dict_find(itemdict, (char_u *)"from", -1)) == ଖାଲି)
	    ଜାରି;
	ଯଦି (list2fpos(&di->di_tv, &mark, &fnum, ଖାଲି, FALSE) != OK)
	    ଜାରି;
	ଯଦି ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ଖାଲି)
	    ଜାରି;

	ଯଦି (mark.col > 0)
	    mark.col--;
	tagstack_ଠେଲ_item(wp, tagname,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(itemdict, "bufnr"),
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tagstack_set_curidx(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା curidx)
{
    wp->w_tagstackidx = curidx;
    ଯଦି (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ଯଦି (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_tagstack(win_T *wp, dict_T *d, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action)
{
    dictitem_T	*di;
    list_T	*l = ଖାଲି;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ଯଦି (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ଗଦା_within_tagfunc));
	ଫେରନ୍ତୁ FAIL;
    }
# endif

    ଯଦି ((di = dict_find(d, (char_u *)"items", -1)) != ଖାଲି)
    {
	ଯଦି (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ଫେରନ୍ତୁ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ଯଦି ((di = dict_find(d, (char_u *)"curidx", -1)) != ଖାଲି)
	tagstack_set_curidx(wp, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&di->di_tv) - 1);

    ଯଦି (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tagstackidx = wp->w_tagstackidx;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ଯେତେବେଳେ (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ଯଦି (l != ଖାଲି)
    {
	ଯଦି (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ଠେଲ_ବସ୍ତୁଗୁଡ଼ିକ(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    ଫେରନ୍ତୁ OK;
}
#endif
