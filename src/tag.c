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

#અન્તર્ભાવ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
પ્રકારવ્યાખ્યા સંરચના tag_pointers
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
    પૂર્ણાંક		is_etag;	// TRUE for emacs tag
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
પ્રકારવ્યાખ્યા ગણના
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
પ્રકારવ્યાખ્યા ગણના
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_દ્વિઅંકી,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
પ્રકારવ્યાખ્યા સંરચના
{
    off_T	નીચલું_offset;	// offset for first char of first line that
				// could match
    off_T	ઉપલું_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    પૂર્ણાંક	નીચલું_char;		// first char at low_offset
    પૂર્ણાંક	ઉપલું_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
પ્રકારવ્યાખ્યા ગણના
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક	matchoff;		// tag match offset
    પૂર્ણાંક	match_re;		// TRUE if the tag matches a regexp
    પૂર્ણાંક	match_no_ic;		// TRUE if the tag matches with case
    પૂર્ણાંક	has_re;			// regular expression used
    પૂર્ણાંક	sortic;			// tags file sorted ignoring case (foldcase)
    પૂર્ણાંક	sort_error;		// tags file not sorted
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
#define MT_ગણતરી	16

સ્થિર અક્ષર	*mt_names[MT_ગણતરી/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
સ્થિર char_u	*nofile_fname = ખાલી;	// fname for NOTAGFILE error

સ્થિર શૂન્ય taglen_advance(પૂર્ણાંક l);

સ્થિર પૂર્ણાંક jumpto_tag(char_u *lbuf, પૂર્ણાંક forceit, પૂર્ણાંક keep_help);
#ifdef FEAT_EMACS_TAGS
સ્થિર પૂર્ણાંક parse_tag_લીટી(char_u *lbuf, પૂર્ણાંક is_etag, tagptrs_T *tagp);
#નહીંતર
સ્થિર પૂર્ણાંક parse_tag_લીટી(char_u *lbuf, tagptrs_T *tagp);
#endif
સ્થિર પૂર્ણાંક test_for_static(tagptrs_T *);
સ્થિર પૂર્ણાંક parse_match(char_u *lbuf, tagptrs_T *tagp);
સ્થિર char_u *tag_full_fname(tagptrs_T *tagp);
સ્થિર char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, પૂર્ણાંક expand);
#ifdef FEAT_EMACS_TAGS
સ્થિર પૂર્ણાંક test_for_current(પૂર્ણાંક, char_u *, char_u *, char_u *, char_u *);
#નહીંતર
સ્થિર પૂર્ણાંક test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
સ્થિર પૂર્ણાંક find_extra(char_u **pp);
સ્થિર શૂન્ય print_tag_list(પૂર્ણાંક new_tag, પૂર્ણાંક use_tagstack, પૂર્ણાંક અંક_matches, char_u **matches);
#જો defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
સ્થિર પૂર્ણાંક add_llist_tags(char_u *tag, પૂર્ણાંક અંક_matches, char_u **matches);
#endif

સ્થિર char_u	*tagmatchname = ખાલી;	// name of last used tag

#જો defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
સ્થિર taggy_T ptag_entry = {ખાલી, {{0, 0, 0}, 0}, 0, 0, ખાલી};
#endif

#ifdef FEAT_EVAL
સ્થિર પૂર્ણાંક  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
સ્થિર callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#જો defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    અક્ષર *
did_set_tagfunc(optset_T *args)
{
    પૂર્ણાંક	retval;

    જો (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    નહીંતર
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	જો (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    પાછા retval == FAIL ? e_invalid_argument : ખાલી;
}
#endif

#જો defined(EXITFREE)
    શૂન્ય
free_tagfunc_option(શૂન્ય)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    પૂર્ણાંક
set_ref_in_tagfunc(પૂર્ણાંક copyID UNUSED)
{
    પૂર્ણાંક	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    પાછા abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    શૂન્ય
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    જો (tfu_cb.cb_name != ખાલી && *tfu_cb.cb_name != NUL)
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
    પૂર્ણાંક
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    પૂર્ણાંક		type,
    પૂર્ણાંક		ગણતરી,
    પૂર્ણાંક		forceit,	// :ta with !
    પૂર્ણાંક		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    પૂર્ણાંક		tagstackidx = curwin->w_tagstackidx;
    પૂર્ણાંક		tagstacklen = curwin->w_tagstacklen;
    પૂર્ણાંક		cur_match = 0;
    પૂર્ણાંક		cur_fnum = curbuf->b_fnum;
    પૂર્ણાંક		oldtagstackidx = tagstackidx;
    પૂર્ણાંક		prevtagstackidx = tagstackidx;
    પૂર્ણાંક		prev_અંક_matches;
    પૂર્ણાંક		new_tag = FALSE;
    પૂર્ણાંક		i;
    પૂર્ણાંક		ic;
    પૂર્ણાંક		no_regexp = FALSE;
    પૂર્ણાંક		error_cur_match = 0;
    પૂર્ણાંક		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    પૂર્ણાંક		jumped_to_tag = FALSE;
#endif
    પૂર્ણાંક		new_અંક_matches;
    char_u	**new_matches;
    પૂર્ણાંક		use_tagstack;
    પૂર્ણાંક		skip_સંદેશ = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    પૂર્ણાંક		use_tfu = 1;
    char_u	*tofree = ખાલી;

    // remember the matches for the last used tag
    સ્થિર પૂર્ણાંક		અંક_matches = 0;
    સ્થિર પૂર્ણાંક		max_અંક_matches = 0;  // limit used for match search
    સ્થિર char_u	**matches = ખાલી;
    સ્થિર પૂર્ણાંક		flags;

#ifdef EXITFREE
    જો (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(અંક_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	અંક_matches = 0;
	પાછા FALSE;
    }
#endif

#ifdef FEAT_EVAL
    જો (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ઢગલો_within_tagfunc));
	પાછા FALSE;
    }
#endif

    જો (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	પાછા FALSE;

    જો (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_અંક_matches = અંક_matches;
    free_string_option(nofile_fname);
    nofile_fname = ખાલી;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    જો ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#જો defined(FEAT_QUICKFIX)
	જો (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    જો ((ptag_entry.tagname = vim_strsave(tag)) == ખાલી)
		જાઓ end_do_tag;
	}
#endif
    }
    નહીંતર
    {
#જો defined(FEAT_QUICKFIX)
	જો (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	નહીંતર
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	જો (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#જો defined(FEAT_QUICKFIX)
	    જો (g_do_tagpreview != 0)
	    {
		જો (ptag_entry.tagname != ખાલી
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		નહીંતર
		{
		    tagstack_clear_entry(&ptag_entry);
		    જો ((ptag_entry.tagname = vim_strsave(tag)) == ખાલી)
			જાઓ end_do_tag;
		}
	    }
	    નહીંતર
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		જ્યારે (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		જો (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    માટે (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = ખાલી;
		}

		/*
		 * put the tag name in the tag stack
		 */
		જો ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == ખાલી)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    જાઓ end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	નહીંતર
	{
	    જો (
#જો defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == ખાલી :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_ઢગલો_empty));
		જાઓ end_do_tag;
	    }

	    જો (type == DT_કાઢો)		// go to older position
	    {
#ifdef FEAT_FOLDING
		પૂર્ણાંક	old_ચાવી_Typed = ચાવી_Typed;
#endif
		જો ((tagstackidx -= ગણતરી) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_ઢગલો));
		    જો (tagstackidx + ગણતરી == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			જાઓ end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		નહીંતર જો (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_ટોચ_of_tag_ઢગલો));
		    જાઓ end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		જો (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    જો (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			જાઓ end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		નહીંતર
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		જો ((fdo_flags & FDO_TAG) && old_ચાવી_Typed)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(અંક_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		અંક_matches = 0;
		tag_freematch();
		જાઓ end_do_tag;
	    }

	    જો (type == DT_TAG
#જો defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#જો defined(FEAT_QUICKFIX)
		જો (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		નહીંતર
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    જો ((tagstackidx += ગણતરી - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_ટોચ_of_tag_ઢગલો));
			save_pos = FALSE;
		    }
		    નહીંતર જો (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_ઢગલો));
			tagstackidx = 0;
			જાઓ end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    નહીંતર				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#જો defined(FEAT_QUICKFIX)
		જો (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		નહીંતર
#endif
		{
		    જો (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		પસંદગી (type)
		{
		    સ્થિતિ DT_FIRST: cur_match = ગણતરી - 1; તોડો;
		    સ્થિતિ DT_SELECT:
		    સ્થિતિ DT_JUMP:
#ifdef FEAT_CSCOPE
		    સ્થિતિ DT_CSCOPE:
#endif
		    સ્થિતિ DT_LAST:  cur_match = MAXCOL - 1; તોડો;
		    સ્થિતિ DT_NEXT:  cur_match += ગણતરી; તોડો;
		    સ્થિતિ DT_PREV:  cur_match -= ગણતરી; તોડો;
		}
		જો (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		નહીંતર જો (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_સંદેશ = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#જો defined(FEAT_QUICKFIX)
	જો (g_do_tagpreview != 0)
	{
	    જો (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	નહીંતર
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    જો (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    જો (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    જો (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	જો (buf != ખાલી)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    માટે (;;)
    {
	પૂર્ણાંક	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	જો (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#જો defined(FEAT_QUICKFIX)
	નહીંતર જો (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	નહીંતર
	    name = tag;
	other_name = (tagmatchname == ખાલી || STRCMP(tagmatchname, name) != 0);
	જો (new_tag
		|| (cur_match >= અંક_matches && max_અંક_matches != MAXCOL)
		|| other_name)
	{
	    જો (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    જો (type == DT_SELECT || type == DT_JUMP
#જો defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    જો (type == DT_TAG)
		max_અંક_matches = MAXCOL;
	    નહીંતર
		max_અંક_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    જો (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    નહીંતર
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    જો (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    જો (verbose)
		flags |= TAG_VERBOSE;

	    જો (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    જો (find_tags(name, &new_અંક_matches, &new_matches, flags,
					    max_અંક_matches, buf_ffname) == OK
		    && new_અંક_matches < max_અંક_matches)
		max_અંક_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    જો (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_અંક_matches, new_matches);
		તોડો;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    જો (!new_tag && !other_name)
	    {
		પૂર્ણાંક	    j, k;
		પૂર્ણાંક	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		માટે (j = 0; j < અંક_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    માટે (i = idx; i < new_અંક_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			જો (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    માટે (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    તોડો;
			}
		    }
		}
	    }
	    FreeWild(અંક_matches, matches);
	    અંક_matches = new_અંક_matches;
	    matches = new_matches;
	}

	જો (અંક_matches <= 0)
	{
	    જો (verbose)
		semsg(_(e_tag_not_found_str), name);
#જો defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	નહીંતર
	{
	    પૂર્ણાંક ask_for_selection = FALSE;
#જો defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    જો (type == DT_CSCOPE && અંક_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    નહીંતર
#endif
	    જો (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = ગણતરી > 0 ? ગણતરી - 1 : 0;
	    નહીંતર જો (type == DT_SELECT || (type == DT_JUMP && અંક_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, અંક_matches, matches);
		ask_for_selection = TRUE;
	    }
#જો defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    નહીંતર જો (type == DT_LTAG)
	    {
		જો (add_llist_tags(tag, અંક_matches, matches) == FAIL)
		    જાઓ end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    જો (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_સંખ્યા(ખાલી);
		જો (i <= 0 || i > અંક_matches || got_int)
		{
		    // no valid choice: don't change anything
		    જો (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    તોડો;
		}
		cur_match = i - 1;
	    }

	    જો (cur_match >= અંક_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		જો ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == ખાલી)
		{
		    જો (અંક_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    નહીંતર
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_સંદેશ = TRUE;
		}
		cur_match = અંક_matches - 1;
	    }
	    જો (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		જો (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#જો defined(FEAT_QUICKFIX)
	    નહીંતર જો (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    જો (nofile_fname != ખાલી && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    જો (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (અંક_matches > 1 || ic)
		&& !skip_સંદેશ)
	    {
		// Give an indication of the number of matching tags
		sprintf((અક્ષર *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				અંક_matches,
				max_અંક_matches != MAXCOL ? _(" or more") : "");
		જો (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		જો ((અંક_matches > prev_અંક_matches || new_tag)
							   && અંક_matches > 1)
		{
		    જો (ic)
			સંદેશ_attr((અક્ષર *)IObuff, HL_ATTR(HLF_W));
		    નહીંતર
			સંદેશ((અક્ષર *)IObuff);
		    સંદેશ_scroll = TRUE;	// don't overwrite this message
		}
		નહીંતર
		    give_warning(IObuff, ic);
		જો (ic && !સંદેશ_scrolled && સંદેશ_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#જો defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (પૂર્ણાંક)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#જો defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, ખાલી, -1);
#endif

	    જો (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		જો ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_અંક_matches != MAXCOL
					     || cur_match < અંક_matches - 1)))
		{
		    error_cur_match = cur_match;
		    જો (use_tagstack)
			--tagstackidx;
		    જો (type == DT_PREV)
			--cur_match;
		    નહીંતર
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    ચાલુ;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    નહીંતર
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		જો (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	તોડો;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    જો (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    પાછા jumped_to_tag;
#નહીંતર
    પાછા FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    સ્થિર શૂન્ય
print_tag_list(
    પૂર્ણાંક		new_tag,
    પૂર્ણાંક		use_tagstack,
    પૂર્ણાંક		અંક_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    પૂર્ણાંક		tagstackidx = curwin->w_tagstackidx;
    પૂર્ણાંક		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    પૂર્ણાંક		taglen;
    પૂર્ણાંક		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (પૂર્ણાંક)(tagp.tagname_end - tagp.tagname + 2);
    જો (taglen < 18)
	taglen = 18;
    જો (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    જો (સંદેશ_col == 0)
	સંદેશ_didout = FALSE;	// overwrite previous message
    સંદેશ_start();
    સંદેશ_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    સંદેશ_clr_eos();
    taglen_advance(taglen);
    સંદેશ_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    માટે (i = 0; i < અંક_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	જો (!new_tag && (
#જો defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	નહીંતર
	    *IObuff = ' ';
	vim_snprintf((અક્ષર *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	સંદેશ_puts((અક્ષર *)IObuff);
	જો (tagp.tagkind != ખાલી)
	    સંદેશ_outtrans_len(tagp.tagkind,
			  (પૂર્ણાંક)(tagp.tagkind_end - tagp.tagkind));
	સંદેશ_advance(13);
	સંદેશ_outtrans_len_attr(tagp.tagname,
			   (પૂર્ણાંક)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	સંદેશ_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	જો (p != ખાલી)
	{
	    સંદેશ_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	જો (સંદેશ_col > 0)
	    સંદેશ_putchar('\n');
	જો (got_int)
	    તોડો;
	સંદેશ_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	જો (command_end != ખાલી)
	{
	    p = command_end + 3;
	    જ્યારે (*p && *p != '\r' && *p != '\n')
	    {
		જ્યારે (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		જો (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ચાલુ;
		}
		// skip "kind:<kind>" and "<kind>"
		જો (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    ચાલુ;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		જ્યારે (*p && *p != '\r' && *p != '\n')
		{
		    જો (સંદેશ_col + ptr2cells(p) >= cmdline_width)
		    {
			સંદેશ_putchar('\n');
			જો (got_int)
			    તોડો;
			સંદેશ_advance(15);
		    }
		    p = સંદેશ_outtrans_one(p, attr);
		    જો (*p == TAB)
		    {
			સંદેશ_puts_attr(" ", attr);
			તોડો;
		    }
		    જો (*p == ':')
			attr = 0;
		}
	    }
	    જો (સંદેશ_col > 15)
	    {
		સંદેશ_putchar('\n');
		જો (got_int)
		    તોડો;
		સંદેશ_advance(15);
	    }
	}
	નહીંતર
	{
	    માટે (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	જો (*p == '/' || *p == '?')
	{
	    ++p;
	    જો (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	જ્યારે (p != command_end && vim_isspace(*p))
	    ++p;

	જ્યારે (p != command_end)
	{
	    જો (સંદેશ_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		સંદેશ_putchar('\n');
	    જો (got_int)
		તોડો;
	    સંદેશ_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    જો (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    જો (*p == TAB)
	    {
		સંદેશ_putchar(' ');
		++p;
	    }
	    નહીંતર
		p = સંદેશ_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    જો (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		તોડો;
	    // don't display matching '/' or '?'
	    જો (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		તોડો;
	}
	જો (સંદેશ_col)
	    સંદેશ_putchar('\n');
	ui_breakcheck();
    }
    જો (got_int)
	got_int = FALSE;	// only stop the listing
}

#જો defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    સ્થિર પૂર્ણાંક
add_llist_tags(
    char_u	*tag,
    પૂર્ણાંક		અંક_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    પૂર્ણાંક		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    જો (list == ખાલી || fname == ખાલી || cmd == ખાલી)
    {
	vim_free(cmd);
	vim_free(fname);
	જો (list != ખાલી)
	    list_free(list);
	પાછા FAIL;
    }

    માટે (i = 0; i < અંક_matches; ++i)
    {
	પૂર્ણાંક	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (પૂર્ણાંક)(tagp.tagname_end - tagp.tagname);
	જો (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	જો (p == ખાલી)
	    ચાલુ;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	જો (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((અક્ષર *)tagp.command);
	નહીંતર
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    જો (cmd_end == ખાલી)
	    {
		માટે (p = tagp.command;
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
	    જો (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    જો (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    જો (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (પૂર્ણાંક)(cmd_end - cmd_start + 1);
	    જો (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    જો (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	જો ((dict = dict_alloc()) == ખાલી)
	    ચાલુ;
	જો (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ચાલુ;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_સંખ્યા(dict, "lnum", lnum);
	જો (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, ખાલી);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    પાછા OK;
}
#endif

/*
 * Free cached tags.
 */
    શૂન્ય
tag_freematch(શૂન્ય)
{
    VIM_CLEAR(tagmatchname);
}

    સ્થિર શૂન્ય
taglen_advance(પૂર્ણાંક l)
{
    જો (l == MAXCOL)
    {
	સંદેશ_putchar('\n');
	સંદેશ_advance(24);
    }
    નહીંતર
	સંદેશ_advance(13 + l);
}

/*
 * Print the tag stack
 */
    શૂન્ય
do_tags(exarg_T *eap UNUSED)
{
    પૂર્ણાંક		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    પૂર્ણાંક		tagstackidx = curwin->w_tagstackidx;
    પૂર્ણાંક		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    સંદેશ_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    માટે (i = 0; i < tagstacklen; ++i)
    {
	જો (tagstack[i].tagname != ખાલી)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    જો (name == ખાલી)	    // file name not available
		ચાલુ;

	    સંદેશ_putchar('\n');
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    સંદેશ_outtrans(IObuff);
	    સંદેશ_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    જો (tagstackidx == tagstacklen)	// idx at top of stack
	સંદેશ_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    સ્થિર પૂર્ણાંક
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    પૂર્ણાંક		i;

    જ્યારે (len > 0)
    {
	i = (પૂર્ણાંક)TOUPPER_ASC(*s1) - (પૂર્ણાંક)TOUPPER_ASC(*s2);
	જો (i != 0)
	    પાછા i;			// this character different
	જો (*s1 == NUL)
	    તોડો;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    પાછા 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*pat;		// the pattern
    પૂર્ણાંક		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    પૂર્ણાંક		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    સ્થિર શૂન્ય
prepare_pats(pat_T *pats, પૂર્ણાંક has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    જો (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	જો (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	નહીંતર જો (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	જો (pats->head == pats->pat)
	    pats->headlen = 0;
	નહીંતર
	    માટે (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		જો (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != ખાલી)
		    તોડો;
	જો (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    જો (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    નહીંતર
	pats->regmatch.regprog = ખાલી;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
પ્રકારવ્યાખ્યા ગણના {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    સ્થિર tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    જો (VIM_ISDIGIT(*cmd))
	પાછા *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    જો (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	પાછા (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    જો (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ખાલી)
	પાછા TAGCMD_INVALID;
    પાછા TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    પૂર્ણાંક		*match_ગણતરી,	// here the number of tags found will be placed
    પૂર્ણાંક		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    પૂર્ણાંક		ntags = 0;
    પૂર્ણાંક		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    જો (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ખાલી
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	પાછા FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    જો ((d = dict_alloc_lock(VAR_FIXED)) == ખાલી)
	પાછા FAIL;
    જો (!(flags & TAG_INS_COMP) && tag->user_data != ખાલી)
	dict_add_string(d, "user_data", tag->user_data);
    જો (buf_ffname != ખાલી)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((અક્ષર *)flagString, કદ(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    જો (result == FAIL)
	પાછા FAIL;
    જો (rettv.v_type == VAR_SPECIAL && rettv.vval.v_સંખ્યા == VVAL_NULL)
    {
	clear_tv(&rettv);
	પાછા NOTDONE;
    }
    જો (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_કિંમત_from_tagfunc));
	પાછા FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_વસ્તુઓ(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	પૂર્ણાંક		len;
	dict_iterator_T	iter;
	char_u		*dict_ચાવી;
	typval_T	*tv;
	પૂર્ણાંક		has_extra = 0;
	પૂર્ણાંક		name_only = flags & TAG_NAMES;

	જો (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_કિંમત_from_tagfunc));
	    તોડો;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# નહીંતર
	len = 2;
# endif
	res_name = ખાલી;
	res_fname = ખાલી;
	res_cmd = ખાલી;
	res_kind = ખાલી;

	dict_iterate_start(&item->li_tv, &iter);
	જ્યારે (ખાલી != (dict_ચાવી = dict_iterate_next(&iter, &tv)))
	{
	    જો (tv->v_type != VAR_STRING || tv->vval.v_string == ખાલી)
		ચાલુ;

	    len += (પૂર્ણાંક)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    જો (STRCMP(dict_ચાવી, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ચાલુ;
	    }
	    જો (STRCMP(dict_ચાવી, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ચાલુ;
	    }
	    જો (STRCMP(dict_ચાવી, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ચાલુ;
	    }
	    has_extra = 1;
	    જો (STRCMP(dict_ચાવી, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		ચાલુ;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (પૂર્ણાંક)STRLEN(dict_ચાવી) + 1;
	}

	જો (has_extra)
	    len += 2;	// need space for ;"

	જો (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_કિંમત_from_tagfunc));
	    તોડો;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	જો (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_કિંમત_from_tagfunc));
	    તોડો;
	}
	જો (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	જો (name_only)
	    mfp = vim_strsave(res_name);
	નહીંતર
	    mfp = alloc(કદ(char_u) + len + 1);

	જો (mfp == ખાલી)
	    ચાલુ;

	જો (!name_only)
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

	    જો (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    જો (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		જો (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		જ્યારે (ખાલી != (dict_ચાવી = dict_iterate_next(&iter, &tv)))
		{
		    જો (tv->v_type != VAR_STRING || tv->vval.v_string == ખાલી)
			ચાલુ;

		    જો (STRCMP(dict_ચાવી, "name") == 0)
			ચાલુ;
		    જો (STRCMP(dict_ચાવી, "filename") == 0)
			ચાલુ;
		    જો (STRCMP(dict_ચાવી, "cmd") == 0)
			ચાલુ;
		    જો (STRCMP(dict_ચાવી, "kind") == 0)
			ચાલુ;

		    *p++ = TAB;
		    STRCPY(p, dict_ચાવી);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	જો (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	નહીંતર
	{
	    vim_free(mfp);
	    તોડો;
	}
    }

    clear_tv(&rettv);

    *match_ગણતરી = ntags;
    પાછા result;
}
#endif

/*
 * State information used during a tag search
 */
પ્રકારવ્યાખ્યા સંરચના
{
    tagsearch_state_T	state;		// tag search state
    પૂર્ણાંક		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    પૂર્ણાંક		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    પૂર્ણાંક		flags;			// flags used for tag search
    પૂર્ણાંક		tag_file_sorted;	// !_TAG_FILE_SORTED value
    પૂર્ણાંક		get_searchpat;		// used for 'showfulltag'
    પૂર્ણાંક		help_only;		// only search for help tags
    પૂર્ણાંક		did_open;		// did open a tag file
    પૂર્ણાંક		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    પૂર્ણાંક		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    પૂર્ણાંક		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    પૂર્ણાંક		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    પૂર્ણાંક		is_txt;			// flag of file extension
#endif
    પૂર્ણાંક		match_ગણતરી;		// number of matches found
    garray_T	ga_match[MT_ગણતરી];	// stores matches in sequence
    hashtab_T	ht_match[MT_ગણતરી];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    સ્થિર પૂર્ણાંક
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    પૂર્ણાંક			flags,
    પૂર્ણાંક			mincount)
{
    પૂર્ણાંક		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = ખાલી;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (પૂર્ણાંક)STRLEN(pat);
    st->orgpat->regmatch.regprog = ખાલી;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = ખાલી;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_ગણતરી = 0;
    st->stop_searching = FALSE;

    માટે (mtt = 0; mtt < MT_ગણતરી; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], કદ(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    જો (st->tag_fname == ખાલી
	    || st->lbuf == ખાલી
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == ખાલી
#endif
       )
	પાછા FAIL;

    પાછા OK;
}

/*
 * Free the state used by find_tags()
 */
    સ્થિર શૂન્ય
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
    સ્થિર પૂર્ણાંક
findtags_in_help_init(findtags_state_T *st)
{
    પૂર્ણાંક		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    જો (st->is_txt)
	STRCPY(st->help_lang, "en");
    નહીંતર
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (પૂર્ણાંક)STRLEN(st->tag_fname);
	જો (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	નહીંતર
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    જો (st->help_lang_find != ખાલી
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	પાછા FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    જો ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == ખાલી
	    && curbuf->b_fname != ખાલી
	    && (i = (પૂર્ણાંક)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    નહીંતર
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	માટે (s = p_hlg; *s != NUL; ++s)
	{
	    જો (STRNICMP(s, st->help_lang, 2) == 0)
		તોડો;
	    ++st->help_pri;
	    જો ((s = vim_strchr(s, ',')) == ખાલી)
		તોડો;
	}
	જો (s == ખાલી || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    જો (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    પાછા TRUE;
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
    સ્થિર પૂર્ણાંક
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    પૂર્ણાંક		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    પૂર્ણાંક		retval;

    જો (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	પાછા NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_ગણતરી,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    પાછા retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
સ્થિર સંરચના
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
સ્થિર પૂર્ણાંક incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    સ્થિર શૂન્ય
emacs_tags_incstack_free(શૂન્ય)
{
    જ્યારે (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ખાલી;
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
    સ્થિર શૂન્ય
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    જો (vim_fgets(st->ebuf, LSIZE, st->fp))
	પાછા;

    માટે (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    જો (*p == NUL)
	પાછા;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    જો (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	પાછા;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    જો (incstack[incstack_idx].etag_fname == ખાલી)
	પાછા;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ખાલી;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    જો (fullpath_ebuf != ખાલી)
    {
	st->fp = mch_fopen((અક્ષર *)fullpath_ebuf, "r");
	જો (st->fp != ખાલી)
	{
	    જો (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    જો (st->fp == ખાલી)
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
    સ્થિર પૂર્ણાંક
emacs_tags_file_eof(findtags_state_T *st)
{
    જો (!incstack_idx)	// reached end of file. stop processing.
	પાછા FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    પાછા TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    સ્થિર પૂર્ણાંક
emacs_tags_parse_લીટી(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    જો (p_7f == ખાલી)
    {
etag_fail:
	જો (vim_strchr(lbuf, '\n') != ખાલી)
	    પાછા FAIL;

	// Truncated line.  Ignore it.
	જો (p_verbose >= 5)
	{
	    verbose_enter();
	    સંદેશ(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	પાછા OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    જો (p == ખાલી)
	p = p_7f + 1;
    નહીંતર
	++p;

    જો (!VIM_ISDIGIT(*p))	    // check for start of line number
	જાઓ etag_fail;
    tagp->command = p;

    જો (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    નહીંતર			    // second format: isolate tagname
    {
	જો (p_7f == lbuf)
	    જાઓ etag_fail;

	// find end of tagname
	માટે (p = p_7f - 1; !vim_iswordc(*p); --p)
	    જો (p == lbuf)
		જાઓ etag_fail;
	tagp->tagname_end = p + 1;
	જ્યારે (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    પાછા OK;
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
    સ્થિર tags_read_status_T
findtags_get_next_લીટી(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    પૂર્ણાંક		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    જો (st->state == TS_દ્વિઅંકી)
    {
	offset = sinfo_p->નીચલું_offset + ((sinfo_p->ઉપલું_offset
						- sinfo_p->નીચલું_offset) / 2);
	જો (offset == sinfo_p->curr_offset)
	    પાછા TAGS_READ_EOF; // End the binary search without a match.
	નહીંતર
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    નહીંતર જો (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	જો (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    જો (st->state == TS_દ્વિઅંકી || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	જો (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    જો (sinfo_p->curr_offset == sinfo_p->ઉપલું_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->નીચલું_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->નીચલું_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	જ્યારે (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	જો (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    પાછા TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    નહીંતર
    {
	// skip empty and blank lines
	કરો
	{
#ifdef FEAT_CSCOPE
	    જો (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    નહીંતર
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} જ્યારે (!eof && vim_isblankline(st->lbuf));

	જો (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    જો (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		પાછા TAGS_READ_IGNORE;
#endif
	    પાછા TAGS_READ_EOF;
	}
    }

    પાછા TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    સ્થિર પૂર્ણાંક
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    જો (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	પાછા TRUE;

    // Process the header line.
    જો (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    જો (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	માટે (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    પાછા FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    સ્થિર પૂર્ણાંક
findtags_start_state_handler(
    findtags_state_T	*st,
    પૂર્ણાંક			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    પૂર્ણાંક		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    પૂર્ણાંક		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    જો (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	પાછા findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    જો (st->linear || use_cscope)
#નહીંતર
    જો (st->linear)
#endif
	st->state = TS_LINEAR;
    નહીંતર જો (st->tag_file_sorted == NUL)
	st->state = TS_દ્વિઅંકી;
    નહીંતર જો (st->tag_file_sorted == '1')
	st->state = TS_દ્વિઅંકી;
    નહીંતર જો (st->tag_file_sorted == '2')
    {
	st->state = TS_દ્વિઅંકી;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    નહીંતર
	st->state = TS_LINEAR;

    જો (st->state == TS_દ્વિઅંકી && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    જો (st->state == TS_દ્વિઅંકી)
    {
	જો (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	નહીંતર
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->નીચલું_offset = 0;
	    sinfo_p->નીચલું_char = 0;
	    sinfo_p->ઉપલું_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->ઉપલું_char = 0xff;
	}
	પાછા FALSE;
    }

    પાછા TRUE;
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
    સ્થિર tagmatch_status_T
findtags_parse_લીટી(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    પૂર્ણાંક		status;
    પૂર્ણાંક		i;
    પૂર્ણાંક		cmplen;
    પૂર્ણાંક		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    જો (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	જો (tagpp->tagname_end == ખાલી)
	    // Corrupted tag line.
	    પાછા TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (પૂર્ણાંક)(tagpp->tagname_end - tagpp->tagname);
	જો (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	જો ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	નહીંતર જો (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    પાછા TAG_MATCH_NEXT;

	જો (st->state == TS_દ્વિઅંકી)
	{
	    // Simplistic check for unsorted tags file.
	    i = (પૂર્ણાંક)tagpp->tagname[0];
	    જો (margs->sortic)
		i = (પૂર્ણાંક)TOUPPER_ASC(tagpp->tagname[0]);
	    જો (i < sinfo_p->નીચલું_char || i > sinfo_p->ઉપલું_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    જો (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    નહીંતર
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    જો (tagcmp == 0)
	    {
		જો (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		નહીંતર જો (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    જો (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		પાછા TAG_MATCH_NEXT;
	    }
	    જો (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		જો (sinfo_p->curr_offset < sinfo_p->ઉપલું_offset)
		{
		    sinfo_p->નીચલું_offset = sinfo_p->curr_offset;
		    જો (margs->sortic)
			sinfo_p->નીચલું_char = TOUPPER_ASC(tagpp->tagname[0]);
		    નહીંતર
			sinfo_p->નીચલું_char = tagpp->tagname[0];
		    પાછા TAG_MATCH_NEXT;
		}
	    }
	    જો (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->ઉપલું_offset)
	    {
		sinfo_p->ઉપલું_offset = sinfo_p->curr_offset;
		જો (margs->sortic)
		    sinfo_p->ઉપલું_char = TOUPPER_ASC(tagpp->tagname[0]);
		નહીંતર
		    sinfo_p->ઉપલું_char = tagpp->tagname[0];
		પાછા TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    પાછા TAG_MATCH_STOP;
	}
	નહીંતર જો (st->state == TS_SKIP_BACK)
	{
	    જો (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    નહીંતર
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    પાછા TAG_MATCH_NEXT;
	}
	નહીંતર જો (st->state == TS_STEP_FORWARD)
	{
	    જો (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		જો ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    પાછા TAG_MATCH_STOP;	// past last match
		નહીંતર
		    પાછા TAG_MATCH_NEXT;	// before first match
	    }
	}
	નહીંતર
	    // skip this match if it can't match
	    જો (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		પાછા TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	જો (tagpp->fname_end == ખાલી)
	    status = FAIL;
	નહીંતર
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    નહીંતર
	status = parse_tag_લીટી(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    જો (status == FAIL)
	પાછા TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    જો (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    પાછા TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    સ્થિર શૂન્ય
findtags_matchargs_init(findtags_match_args_T *margs, પૂર્ણાંક flags)
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
    સ્થિર પૂર્ણાંક
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    પૂર્ણાંક		match = FALSE;
    પૂર્ણાંક		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (પૂર્ણાંક)(tagpp->tagname_end - tagpp->tagname);
    જો (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    જો (st->orgpat->len != cmplen)
	match = FALSE;
    નહીંતર
    {
	જો (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    જો (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	નહીંતર
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    જો (!match && st->orgpat->regmatch.regprog != ખાલી)
    {
	પૂર્ણાંક	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	જો (match)
	{
	    margs->matchoff = (પૂર્ણાંક)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    જો (st->orgpat->regmatch.rm_ic)
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

    પાછા match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    સ્થિર શૂન્ય
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_લીટી;
    પૂર્ણાંક		len;

    conv_લીટી = string_convert(&st->vimconv, st->lbuf, ખાલી);
    જો (conv_લીટી == ખાલી)
	પાછા;

    // Copy or swap lbuf and conv_line.
    len = (પૂર્ણાંક)STRLEN(conv_લીટી) + 1;
    જો (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_લીટી;
	st->lbuf_size = len;
    }
    નહીંતર
    {
	STRCPY(st->lbuf, conv_લીટી);
	vim_free(conv_લીટી);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    સ્થિર પૂર્ણાંક
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    પૂર્ણાંક		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    પૂર્ણાંક		name_only = (st->flags & TAG_NAMES);
    પૂર્ણાંક		mtt;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		is_current;		// file name matches
    પૂર્ણાંક		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    જો (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    નહીંતર
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
	જો (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	જો (is_static)
	{
	    જો (is_current)
		mtt = MT_ST_CUR;
	    નહીંતર
		mtt = MT_ST_OTH;
	}
	નહીંતર
	{
	    જો (is_current)
		mtt = MT_GL_CUR;
	    નહીંતર
		mtt = MT_GL_OTH;
	}
	જો (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	જો (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    જો (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#નહીંતર
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (પૂર્ણાંક)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(કદ(char_u) + len + 10 + ML_EXTRA + 1);
	જો (mfp != ખાલી)
	{
	    પૂર્ણાંક heuristic;

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
	    sprintf((અક્ષર *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    નહીંતર જો (name_only)
    {
	જો (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    જો (*temp_end == '/')
		જ્યારે (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    જો (tagpp->command + 2 < temp_end)
	    {
		len = (પૂર્ણાંક)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		જો (mfp != ખાલી)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    નહીંતર
		mfp = ખાલી;
	    st->get_searchpat = FALSE;
	}
	નહીંતર
	{
	    len = (પૂર્ણાંક)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(કદ(char_u) + len + 1);
	    જો (mfp != ખાલી)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    જો (State & MODE_ઉમેરો)
		st->get_searchpat = p_sft;
	}
    }
    નહીંતર
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
	len = (પૂર્ણાંક)tag_fname_len + (પૂર્ણાંક)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	જો (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (પૂર્ણાંક)ebuf_len + 1;
	}
	નહીંતર
	    ++len;
#endif
	mfp = alloc(કદ(char_u) + len + 1);
	જો (mfp != ખાલી)
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
	    જો (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    નહીંતર
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    જો (mfp != ખાલી)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	જો (use_cscope)
	    ++*hash;
	નહીંતર
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	જો (HASHITEM_EMPTY(hi))
	{
	    જો (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		પાછા FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_ગણતરી++;
	}
	નહીંતર
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    પાછા OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    સ્થિર શૂન્ય
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	શોધ_info;
    પૂર્ણાંક			retval;
#ifdef FEAT_CSCOPE
    પૂર્ણાંક			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(શોધ_info);

    // Read and parse the lines in the file one by one
    માટે (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	જો (st->state == TS_દ્વિઅંકી || st->state == TS_SKIP_BACK)
	    લીટી_breakcheck();
	નહીંતર
	    fast_breakcheck();
	જો ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ચાવીઓ(30, FALSE);
	જો (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    તોડો;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	જો (st->mincount == TAG_MANY && st->match_ગણતરી >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    તોડો;
	}
	જો (st->get_searchpat)
	    જાઓ લીટી_read_in;

	retval = findtags_get_next_લીટી(st, &શોધ_info);
	જો (retval == TAGS_READ_IGNORE)
	    ચાલુ;
	જો (retval == TAGS_READ_EOF)
	    તોડો;

લીટી_read_in:

	જો (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	જો (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    ચાલુ;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	જો (st->state == TS_START)
	{
	    જો (findtags_start_state_handler(st, &margs->sortic, &શોધ_info) == FALSE)
		ચાલુ;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	જો (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    જો (st->lbuf == ખાલી)
	    {
		જો (st->fp != ખાલી)
		    fclose(st->fp);
		st->fp = ખાલી;
		st->stop_searching = TRUE;
		પાછા;
	    }

	    જો (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, શોધ_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    શોધ_info.curr_offset = 0;
	    ચાલુ;
	}

	retval = findtags_parse_લીટી(st, &tagp, margs, &શોધ_info);
	જો (retval == TAG_MATCH_NEXT)
	    ચાલુ;
	જો (retval == TAG_MATCH_STOP)
	    તોડો;
	જો (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    જો (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    પાછા;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	જો (findtags_match_tag(st, &tagp, margs))
	{
	    જો (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		તોડો;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    સ્થિર શૂન્ય
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    પૂર્ણાંક		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = ખાલી;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    જો (use_cscope)
	st->fp = ખાલી;	    // avoid GCC warning
    નહીંતર
#endif
    {
#ifdef FEAT_MULTI_LANG
	જો (curbuf->b_help)
	{
	    જો (!findtags_in_help_init(st))
		પાછા;
	}
#endif

	st->fp = mch_fopen((અક્ષર *)st->tag_fname, "r");
	જો (st->fp == ખાલી)
	    પાછા;

	જો (p_verbose >= 5)
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

    જો (st->fp != ખાલી)
    {
	fclose(st->fp);
	st->fp = ખાલી;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    જો (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, ખાલી, ખાલી);

    જો (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    જો (st->match_ગણતરી >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    સ્થિર પૂર્ણાંક
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    પૂર્ણાંક		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    પૂર્ણાંક		mtt;
    પૂર્ણાંક		i;
    char_u	*mfp;
    char_u	*p;

    જો (st->match_ગણતરી > 0)
	matches = ALLOC_MULT(char_u *, st->match_ગણતરી);
    નહીંતર
	matches = ખાલી;
    st->match_ગણતરી = 0;
    માટે (mtt = 0; mtt < MT_ગણતરી; ++mtt)
    {
	માટે (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    જો (matches == ખાલી)
		vim_free(mfp);
	    નહીંતર
	    {
		જો (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    માટે (p = mfp + 1; *p != NUL; ++p)
			જો (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_ગણતરી++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    પાછા st->match_ગણતરી;
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
    પૂર્ણાંક
find_tags(
    char_u	*pat,			// pattern to search for
    પૂર્ણાંક		*અંક_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    પૂર્ણાંક		flags,
    પૂર્ણાંક		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    પૂર્ણાંક		first_file;		// trying first tag file
    પૂર્ણાંક		retval = FAIL;		// return value
    પૂર્ણાંક		round;

    પૂર્ણાંક		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    પૂર્ણાંક		i;
    char_u	*saved_pat = ખાલી;		// copy of pat[]
#endif

    પૂર્ણાંક		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    પૂર્ણાંક		has_re = (flags & TAG_REGEXP);	// regexp used
    પૂર્ણાંક		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    પૂર્ણાંક		use_cscope = (flags & TAG_CSCOPE);
#endif
    પૂર્ણાંક		verbose = (flags & TAG_VERBOSE);
    પૂર્ણાંક		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    પસંદગી (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	સ્થિતિ TC_FOLLOWIC:		 તોડો;
	સ્થિતિ TC_IGNORE:    p_ic = TRUE;  તોડો;
	સ્થિતિ TC_MATCH:     p_ic = FALSE; તોડો;
	સ્થિતિ TC_FOLLOWSCS: p_ic = ignorecase(pat); તોડો;
	સ્થિતિ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); તોડો;
    }

    help_save = curbuf->b_help;

    જો (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	જાઓ findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    જો (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    નહીંતર જો (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    જો (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	જો (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    જો (saved_pat != ખાલી)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    જો (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    જો (has_re && st.orgpat->regmatch.regprog == ખાલી)
	જાઓ findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    જો (retval != NOTDONE)
	જાઓ findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    જો ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == ખાલી
	    && curbuf->b_fname != ખાલી
	    && (i = (પૂર્ણાંક)STRLEN(curbuf->b_fname)) > 4
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
    માટે (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      માટે (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  જો (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      તોડો;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	જો (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	જો (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    તોડો;
#ifdef FEAT_CSCOPE
	જો (use_cscope)
	    તોડો;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    જો (!st.stop_searching)
    {
	જો (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    જો (retval == FAIL)
	st.match_ગણતરી = 0;

    *અંક_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    પાછા retval;
}

સ્થિર garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    સ્થિર શૂન્ય
found_tagfile_cb(char_u *fname, શૂન્ય *cookie UNUSED)
{
    જો (ga_grow(&tag_fnames, 1) == FAIL)
	પાછા;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#જો defined(EXITFREE)
    શૂન્ય
free_tag_stuff(શૂન્ય)
{
    ga_clear_strings(&tag_fnames);
    જો (curwin != ખાલી)
	do_tag(ખાલી, DT_FREE, 0, 0, 0);
    tag_freematch();

# જો defined(FEAT_QUICKFIX)
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
    પૂર્ણાંક
get_tagfname(
    tagname_T	*tnp,	// holds status info
    પૂર્ણાંક		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ખાલી;
    char_u		*r_ptr;
    પૂર્ણાંક			i;

    જો (first)
	CLEAR_POINTER(tnp);

    જો (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	જો (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, કદ(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# નહીંતર
		    "doc/tags doc/tags-??"
# endif
#નહીંતર
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, ખાલી);
	}

	જો (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    જો (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		પાછા FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    માટે (i = 0; i < tag_fnames.ga_len; ++i)
		જો (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    પાછા FAIL; // avoid duplicate file names
	}
	નહીંતર
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	પાછા OK;
    }

    જો (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	જો (tnp->tn_tags == ખાલી)
	    પાછા FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    માટે (;;)
    {
	જો (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_શોધ_ctx);
	    જો (fname != ખાલી)
		તોડો;

	    tnp->tn_did_filefind_init = FALSE;
	}
	નહીંતર
	{
	    char_u  *filename = ખાલી;

	    // Stop when used all parts of 'tags'.
	    જો (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_શોધ_ctx);
		tnp->tn_શોધ_ctx = ખાલી;
		પાછા FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (શૂન્ય)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    જો (r_ptr != ખાલી)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_શોધ_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_શોધ_ctx, TRUE, curbuf->b_ffname);
	    જો (tnp->tn_શોધ_ctx != ખાલી)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    પાછા OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    શૂન્ય
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_શોધ_ctx);
    tnp->tn_શોધ_ctx = ખાલી;
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
    સ્થિર પૂર્ણાંક
parse_tag_લીટી(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    પૂર્ણાંક		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    જો (is_etag)
	// emacs-style tag file
	પાછા emacs_tags_parse_લીટી(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    જો (p == ખાલી)
	પાછા FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    જો (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    જો (p == ખાલી)
	પાછા FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    જો (*p != NUL)
	++p;
    જો (*p == NUL)
	પાછા FAIL;
    tagp->command = p;

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    જ્યારે ((p = vim_strchr(p, '\t')) != ખાલી)
    {
	++p;
	જો (STRNCMP(p, "file:", 5) == 0)
	    પાછા TRUE;
    }

    પાછા FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    સ્થિર size_t
matching_લીટી_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    પાછા (p - lbuf) + STRLEN(p);
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
    સ્થિર પૂર્ણાંક
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    પૂર્ણાંક		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    જો (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    નહીંતર
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_લીટી(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = ખાલી;
    tagp->user_data = ખાલી;
    tagp->tagline = 0;
    tagp->command_end = ખાલી;

    જો (retval != OK)
	પાછા retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    જો (find_extra(&p) == OK)
    {
	જો (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	નહીંતર
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	જો (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    જ્યારે (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		જો (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		નહીંતર જો (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		નહીંતર જો (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((અક્ષર *)p + 5);
		જો (tagp->tagkind != ખાલી && tagp->user_data != ખાલી)
		    તોડો;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		જો (pc == ખાલી || (pt != ખાલી && pc > pt))
		    tagp->tagkind = p;
		જો (pt == ખાલી)
		    તોડો;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    જો (tagp->tagkind != ખાલી)
    {
	માટે (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    જો (tagp->user_data != ખાલી)
    {
	માટે (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    પાછા retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    સ્થિર char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    પૂર્ણાંક		c;

#ifdef FEAT_EMACS_TAGS
    જો (tagp->is_etag)
	c = 0;	    // to shut up GCC
    નહીંતર
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    જો (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    પાછા fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    પૂર્ણાંક		forceit,	// :ta with !
    પૂર્ણાંક		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    પૂર્ણાંક		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = ખાલી;
    char_u	*fname;
    tagptrs_T	tagp;
    પૂર્ણાંક		retval = FAIL;
    પૂર્ણાંક		getfile_result = GETFILE_UNUSED;
    પૂર્ણાંક		શોધ_options;
#ifdef FEAT_શોધ_EXTRA
    પૂર્ણાંક		save_no_hlsearch;
#endif
#જો defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ખાલી;
#endif
    char_u	*full_fname = ખાલી;
#ifdef FEAT_FOLDING
    પૂર્ણાંક		old_ચાવી_Typed = ચાવી_Typed;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    પૂર્ણાંક		isdigit = FALSE;

    જો (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	પાછા FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_લીટી_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    જો (lbuf != ખાલી)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    જો (pbuf == ખાલી || lbuf == ખાલી || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = ખાલી;
	જાઓ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    જો (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    માટે (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	જો (tagp.is_etag && *str == ',')// stop at ',' after line number
	    તોડો;
#endif
	*pbuf_end++ = *str++;
	જો (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    તોડો;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    જો (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	જો (isdigit)
	    str++;
	જો (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    જો (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    નહીંતર
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    જો (fname == ખાલી)
	જાઓ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    જો (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ખાલી))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	જો (nofile_fname == ખાલી)
	    nofile_fname = empty_option;
	જાઓ erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#જો defined(FEAT_QUICKFIX)
    જો (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	જો (!curwin->w_p_pvw)
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
    જો (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	જો (existing_buf != ખાલી)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    જો (swbuf_goto_win_with_buf(existing_buf) != ખાલી)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    જો (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	જો (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	જો ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	જો (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    જો (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    જાઓ erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    જો (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#જો defined(FEAT_QUICKFIX)
	જો (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	નહીંતર
#endif
	    keep_help_flag = curbuf->b_help;
    }

    જો (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, ખાલી, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    જો (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_શોધ_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	જો (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	જો (vim_strchr(p_cpo, CPO_TAGPAT) != ખાલી)
	    શોધ_options = 0;
	નહીંતર
	    શોધ_options = શોધ_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	જો (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	જો (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    જો (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    નહીંતર
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    જો (do_શોધ(ખાલી, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 શોધ_options, ખાલી))
		retval = OK;
	    નહીંતર
	    {
		પૂર્ણાંક	found = 1;
		પૂર્ણાંક	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		જો (!do_શોધ(ખાલી, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 શોધ_options, ખાલી))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (શૂન્ય)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((અક્ષર *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    જો (!do_શોધ(ખાલી, '/', '/', pbuf, pbuflen, (long)1,
							 શોધ_options, ખાલી))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((અક્ષર *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			જો (!do_શોધ(ખાલી, '/', '/', pbuf, pbuflen, (long)1,
							 શોધ_options, ખાલી))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		જો (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		નહીંતર
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    જો (found == 2 || !save_p_ic)
		    {
			સંદેશ(_(e_couldnt_find_tag_just_guessing));
			જો (!સંદેશ_scrolled && સંદેશ_silent == 0)
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
	નહીંતર
	{
	    પૂર્ણાંક		save_secure = secure;

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
	    જો (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_શોધ_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	જો (શોધ_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	જો (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	જો (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    જો (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    જો ((fdo_flags & FDO_TAG) && old_ચાવી_Typed)
		foldOpenCursor();
#endif
	}

#જો defined(FEAT_QUICKFIX)
	જો (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	જો (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    નહીંતર
    {
	જો (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	જો (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#જો defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	નહીંતર જો (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    જો (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#જો defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    જો (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#જો defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    પાછા retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    સ્થિર char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, પૂર્ણાંક expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ખાલી;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    જો (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       પાછા ખાલી;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    જો (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ખાલી)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ખાલી,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	જો (expanded_fname != ખાલી)
	    fname = expanded_fname;
    }

    જો ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	જો (retval != ખાલી)
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
    નહીંતર
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    પાછા retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    સ્થિર પૂર્ણાંક
test_for_current(
#ifdef FEAT_EMACS_TAGS
    પૂર્ણાંક	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    પૂર્ણાંક	    c;
    પૂર્ણાંક	    retval = FALSE;
    char_u  *fullname;

    જો (buf_ffname != ખાલી)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	જો (is_etag)
	    c = 0;	    // to shut up GCC
	નહીંતર
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	જો (fullname != ખાલી)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	જો (!is_etag)
#endif
	    *fname_end = c;
    }

    પાછા retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    માટે (;;)
    {
	જો (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	નહીંતર જો (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    જો (*str != first_char)
		str = ખાલી;
	    નહીંતર
		++str;
	}
	નહીંતર
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((અક્ષર *)str, "|;\"");
	    જો (str != ખાલી)
	    {
		++str;
		તોડો;
	    }

	}
	જો (str == ખાલી || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    તોડો;
	++str;	// skip ';'
	first_char = *str;
    }

    જો (str != ખાલી && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	પાછા OK;
    }
    પાછા FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    શૂન્ય
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    પૂર્ણાંક
expand_tags(
    પૂર્ણાંક		tagnames,	// expand tag names
    char_u	*pat,
    પૂર્ણાંક		*અંક_file,
    char_u	***file)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    પૂર્ણાંક		ret;

    name_buf = alloc(name_buf_size);
    જો (name_buf == ખાલી)
	પાછા FAIL;

    જો (tagnames)
	extra_flag = TAG_NAMES;
    નહીંતર
	extra_flag = 0;
    જો (pat[0] == '/')
	ret = find_tags(pat + 1, અંક_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    નહીંતર
	ret = find_tags(pat, અંક_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    જો (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 માટે (i = 0; i < *અંક_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     જો (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 જો (buf == ખાલી)
		 {
		     vim_free(name_buf);
		     પાછા FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ખાલી && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    પાછા ret;
}

#જો defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
add_tag_field(
    dict_T  *dict,
    અક્ષર    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		retval;

    // check that the field name doesn't exist yet
    જો (dict_has_ચાવી(dict, field_name))
    {
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	પાછા FAIL;
    }
    buf = alloc(MAXPATHL);
    જો (buf == ખાલી)
	પાછા FAIL;
    જો (start != ખાલી)
    {
	જો (end == ખાલી)
	{
	    end = start + STRLEN(start);
	    જ્યારે (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (પૂર્ણાંક)(end - start);
	જો (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    પાછા retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    પૂર્ણાંક
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    પૂર્ણાંક		અંક_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &અંક_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (પૂર્ણાંક)MAXCOL, buf_fname);
    જો (ret != OK || અંક_matches <= 0)
	પાછા ret;

    માટે (i = 0; i < અંક_matches; ++i)
    {
	જો (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ચાલુ;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	જો (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ચાલુ;
	}

	જો ((dict = dict_alloc()) == ખાલી)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    તોડો;
	}
	જો (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ચાલુ;
	}

	full_fname = tag_full_fname(&tp);
	જો (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    ખાલી) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_સંખ્યા(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	જો (tp.command_end != ખાલી)
	{
	    માટે (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		જો (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		નહીંતર જો (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		નહીંતર જો (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    પૂર્ણાંક	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    જ્યારે (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (પૂર્ણાંક)(p - n);
		    જો (*p == ':' && len > 0)
		    {
			s = ++p;
			જ્યારે (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			જો (add_tag_field(dict, (અક્ષર *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    નહીંતર
			// Skip field without colon.
			જ્યારે (*p != NUL && *p >= ' ')
			    ++p;
		    જો (*p == NUL)
			તોડો;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    પાછા ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    સ્થિર શૂન્ય
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_સંખ્યા(retdict, "matchnr", tag->cur_match + 1);
    dict_add_સંખ્યા(retdict, "bufnr", tag->cur_fnum);
    જો (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    જો ((pos = list_alloc_id(aid_tagstack_from)) == ખાલી)
	પાછા;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_સંખ્યા(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_સંખ્યા(pos, (varnumber_T)fmark->mark.lnum);
    list_append_સંખ્યા(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_સંખ્યા(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    શૂન્ય
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    પૂર્ણાંક		i;
    dict_T	*d;

    dict_add_સંખ્યા(retdict, "length", wp->w_tagstacklen);
    dict_add_સંખ્યા(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_વસ્તુઓ);
    જો (l == ખાલી)
	પાછા;
    dict_add_list(retdict, "items", l);

    માટે (i = 0; i < wp->w_tagstacklen; i++)
    {
	જો ((d = dict_alloc_id(aid_tagstack_details)) == ખાલી)
	    પાછા;
	જો (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    પાછા;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    સ્થિર શૂન્ય
tagstack_clear(win_T *wp)
{
    પૂર્ણાંક i;

    // Free the current tag stack
    માટે (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    સ્થિર શૂન્ય
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    પૂર્ણાંક		i;

    tagstack_clear_entry(&tagstack[0]);
    માટે (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    સ્થિર શૂન્ય
tagstack_ધકેલો_item(
	win_T	*wp,
	char_u	*tagname,
	પૂર્ણાંક	cur_fnum,
	પૂર્ણાંક	cur_match,
	pos_T	mark,
	પૂર્ણાંક	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    પૂર્ણાંક		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    જો (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    જો (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    સ્થિર શૂન્ય
tagstack_ધકેલો_વસ્તુઓ(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    પૂર્ણાંક		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	જો (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ખાલી)
	    ચાલુ;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	જો ((di = dict_find(itemdict, (char_u *)"from", -1)) == ખાલી)
	    ચાલુ;
	જો (list2fpos(&di->di_tv, &mark, &fnum, ખાલી, FALSE) != OK)
	    ચાલુ;
	જો ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ખાલી)
	    ચાલુ;

	જો (mark.col > 0)
	    mark.col--;
	tagstack_ધકેલો_item(wp, tagname,
		(પૂર્ણાંક)dict_get_સંખ્યા(itemdict, "bufnr"),
		(પૂર્ણાંક)dict_get_સંખ્યા(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    સ્થિર શૂન્ય
tagstack_set_curidx(win_T *wp, પૂર્ણાંક curidx)
{
    wp->w_tagstackidx = curidx;
    જો (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    જો (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    પૂર્ણાંક
set_tagstack(win_T *wp, dict_T *d, પૂર્ણાંક action)
{
    dictitem_T	*di;
    list_T	*l = ખાલી;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    જો (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_ઢગલો_within_tagfunc));
	પાછા FAIL;
    }
# endif

    જો ((di = dict_find(d, (char_u *)"items", -1)) != ખાલી)
    {
	જો (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    પાછા FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    જો ((di = dict_find(d, (char_u *)"curidx", -1)) != ખાલી)
	tagstack_set_curidx(wp, (પૂર્ણાંક)tv_get_સંખ્યા(&di->di_tv) - 1);

    જો (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	પૂર્ણાંક	tagstackidx = wp->w_tagstackidx;
	પૂર્ણાંક	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	જ્યારે (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    જો (l != ખાલી)
    {
	જો (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ધકેલો_વસ્તુઓ(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    પાછા OK;
}
#endif
