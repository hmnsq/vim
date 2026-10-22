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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਟੈਗ_ਸੂਚਕ_ਸਾਰੇ
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_ਸਿਰਾ;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_ਸਿਰਾ;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_ਸਿਰਾ;	// first char after command
    char_u	*ਟੈਗ_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਪੂਰਨ		is_etag;	// TRUE for emacs tag
#ਜੇ_ਅੰਤ
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_ਸਿਰਾ;	// end of tagkind
    char_u	*user_ਅੰਕੜੇ;	// user_data string
    char_u	*user_ਅੰਕੜੇ_ਸਿਰਾ;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਟੈਗ_ਸੂਚੀ_READ_SUCCESS = 1,
    ਟੈਗ_ਸੂਚੀ_READ_EOF,
    ਟੈਗ_ਸੂਚੀ_READ_IGNORE,
} ਟੈਗ_ਸੂਚੀ_read_ਸਥਿਤੀ_T;

/*
 * States used during a tags search
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    TS_ਸ਼ੁਰੂ,		// at start of file
    TS_ਰੇਖਿਕ,		// linear searching forward, till EOF
    TS_ਦੁਆਧਾਰੀ,		// binary searching
    TS_SKIP_ਪਿੱਠ,	// skipping backwards
    TS_ਕਦਮ_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    off_T	ਨੀਵਾਂ_ਵਿੱਥ;	// offset for first char of first line that
				// could match
    off_T	ਉੱਚਾ_ਵਿੱਥ;	// offset of char after last line that could
				//ਮੇਲ
    off_T	curr_ਵਿੱਥ;	// Current file offset in search range
    off_T	curr_ਵਿੱਥ_ਵਰਤਿਆ; // curr_offset used when skipping back
    off_T	ਮੇਲ_ਵਿੱਥ;	// Where the binary search found a tag
    ਪੂਰਨ	ਨੀਵਾਂ_char;		// first char at low_offset
    ਪੂਰਨ	ਉੱਚਾ_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਟੈਗ_ਮੇਲ_SUCCESS = 1,
    ਟੈਗ_ਮੇਲ_FAIL,
    ਟੈਗ_ਮੇਲ_STOP,
    ਟੈਗ_ਮੇਲ_ਅਗਲਾ
} tagmatch_ਸਥਿਤੀ_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	matchoff;		// tag match offset
    ਪੂਰਨ	ਮੇਲ_re;		// TRUE if the tag matches a regexp
    ਪੂਰਨ	ਮੇਲ_no_ic;		// TRUE if the tag matches with case
    ਪੂਰਨ	has_re;			// regular expression used
    ਪੂਰਨ	sortic;			// tags file sorted ignoring case (foldcase)
    ਪੂਰਨ	ਛਾਂਟੋ_error;		// tags file not sorted
} findtags_ਮੇਲ_ਦਲੀਲਾਂ_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_ST_CUR	0		// static match in current file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_GL_CUR	1		// global match in current file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_GL_OTH	2		// global match in other file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_ST_OTH	3		// static match in other file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_IC_OFF	4		// add for icase match
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_RE_OFF	8		// add for regexp match
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_ਨਕਾਬ		7		// mask for printing priority
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MT_ਗਿਣਤੀ	16

ਸਥਿਰ ਅੱਖਰ	*mt_ਨਾਂ[MT_ਗਿਣਤੀ/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOTAGFILE	99		// return value for jumpto_tag
ਸਥਿਰ char_u	*nofile_fname = ਨਿਰਾ;	// fname for NOTAGFILE error

ਸਥਿਰ ਖਾਲੀ taglen_advance(ਪੂਰਨ l);

ਸਥਿਰ ਪੂਰਨ jumpto_ਟੈਗ(char_u *lbuf, ਪੂਰਨ forceit, ਪੂਰਨ keep_help);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
ਸਥਿਰ ਪੂਰਨ parse_ਟੈਗ_ਸਤਰ(char_u *lbuf, ਪੂਰਨ is_etag, tagptrs_T *tagp);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ parse_ਟੈਗ_ਸਤਰ(char_u *lbuf, tagptrs_T *tagp);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ test_for_static(tagptrs_T *);
ਸਥਿਰ ਪੂਰਨ parse_ਮੇਲ(char_u *lbuf, tagptrs_T *tagp);
ਸਥਿਰ char_u *ਟੈਗ_ਭਰਿਆ_fname(tagptrs_T *tagp);
ਸਥਿਰ char_u *expand_ਟੈਗ_fname(char_u *fname, char_u *ਟੈਗ_fname, ਪੂਰਨ expand);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
ਸਥਿਰ ਪੂਰਨ test_for_current(ਪੂਰਨ, char_u *, char_u *, char_u *, char_u *);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ test_for_current(char_u *, char_u *, char_u *, char_u *);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_extra(char_u **pp);
ਸਥਿਰ ਖਾਲੀ print_ਟੈਗ_list(ਪੂਰਨ new_ਟੈਗ, ਪੂਰਨ use_tagstack, ਪੂਰਨ ਅੰਕ_matches, char_u **matches);
#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_llist_ਟੈਗ_ਸੂਚੀ(char_u *ਟੈਗ, ਪੂਰਨ ਅੰਕ_matches, char_u **matches);
#ਜੇ_ਅੰਤ

ਸਥਿਰ char_u	*tagmatchname = ਨਿਰਾ;	// name of last used tag

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ਸਥਿਰ taggy_T ptag_ਇੰਦਰਾਜ = {ਨਿਰਾ, {{0, 0, 0}, 0}, 0, 0, ਨਿਰਾ};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ  tfu_ਵਿੱਚ_use = FALSE;	    // disallow recursive call of tagfunc
ਸਥਿਰ callback_T tfu_cb;	    // 'tagfunc' callback function
#ਜੇ_ਅੰਤ

// Used instead of NUL to separate tag fields in the growarrays.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਟੈਗ_SEP 0x02

#ਜੇ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_tagfunc(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ	retval;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string,
							    &curbuf->b_tfu_cb);
    ਨਹੀਂਤਾਂ
    {
	retval = option_ਸੈੱਟ_callback_func(ਦਲੀਲਾਂ->os_newval.string, &tfu_cb);
	ਜੇ (retval == OK && !(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	    ਸੈੱਟ_buflocal_tfu_callback(curbuf);
    }

    ਵਾਪਸ retval == FAIL ? e_invalid_ਦਲੀਲ : ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_tagfunc_option(ਖਾਲੀ)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_tagfunc(ਪੂਰਨ copyID ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	abort = FALSE;

    abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&tfu_cb, copyID);

    ਵਾਪਸ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    ਖਾਲੀ
ਸੈੱਟ_buflocal_tfu_callback(buf_T *buf ਅਣਵਰਤਿਆ)
{
    free_callback(&buf->b_tfu_cb);
    ਜੇ (tfu_cb.cb_name != ਨਿਰਾ && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
do_ਟੈਗ(
    char_u	*ਟੈਗ,		// tag (pattern) to jump to
    ਪੂਰਨ		type,
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    ਪੂਰਨ		forceit,	// :ta with !
    ਪੂਰਨ		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ਪੂਰਨ		tagstackidx = curwin->w_tagstackidx;
    ਪੂਰਨ		tagstacklen = curwin->w_tagstacklen;
    ਪੂਰਨ		cur_ਮੇਲ = 0;
    ਪੂਰਨ		cur_fnum = curbuf->b_fnum;
    ਪੂਰਨ		oldtagstackidx = tagstackidx;
    ਪੂਰਨ		prevtagstackidx = tagstackidx;
    ਪੂਰਨ		prev_ਅੰਕ_matches;
    ਪੂਰਨ		new_ਟੈਗ = FALSE;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ic;
    ਪੂਰਨ		no_regexp = FALSE;
    ਪੂਰਨ		error_cur_ਮੇਲ = 0;
    ਪੂਰਨ		save_pos = FALSE;
    fmark_T	saved_fmark;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ		jumped_ਮੰਜ਼ਿਲ_ਟੈਗ = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		new_ਅੰਕ_matches;
    char_u	**new_matches;
    ਪੂਰਨ		use_tagstack;
    ਪੂਰਨ		skip_ਸੁਨੇਹਾ = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    ਪੂਰਨ		use_tfu = 1;
    char_u	*tofree = ਨਿਰਾ;

    // remember the matches for the last used tag
    ਸਥਿਰ ਪੂਰਨ		ਅੰਕ_matches = 0;
    ਸਥਿਰ ਪੂਰਨ		max_ਅੰਕ_matches = 0;  // limit used for match search
    ਸਥਿਰ char_u	**matches = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ		flags;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EXITFREE
    ਜੇ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(ਅੰਕ_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_ਟੈਗ_ਸੂਚੀ();
# endif
	ਅੰਕ_matches = 0;
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (tfu_ਵਿੱਚ_use)
    {
	emsg(_(e_cannot_modify_ਟੈਗ_ਢੇਰ_within_tagfunc));
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (postponed_ਵੰਡੋ == 0 && !check_can_ਸੈੱਟ_curbuf_forceit(forceit))
	ਵਾਪਸ FALSE;

    ਜੇ (type == DT_HELP)
    {
	type = DT_ਟੈਗ;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_ਅੰਕ_matches = ਅੰਕ_matches;
    free_string_option(nofile_fname);
    nofile_fname = ਨਿਰਾ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ਜੇ ((!p_tgst && *ਟੈਗ != NUL))
    {
	use_tagstack = FALSE;
	new_ਟੈਗ = TRUE;
#ਜੇ defined(FEAT_QUICKFIX)
	ਜੇ (g_do_tagpreview != 0)
	{
	    tagstack_clear_ਇੰਦਰਾਜ(&ptag_ਇੰਦਰਾਜ);
	    ਜੇ ((ptag_ਇੰਦਰਾਜ.tagname = vim_strsave(ਟੈਗ)) == ਨਿਰਾ)
		ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(FEAT_QUICKFIX)
	ਜੇ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ਜੇ (*ਟੈਗ != NUL
		&& (type == DT_ਟੈਗ || type == DT_SELECT || type == DT_JUMP
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		    || type == DT_LTAG
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		    || type == DT_CSCOPE
#ਜੇ_ਅੰਤ
		    ))
	{
#ਜੇ defined(FEAT_QUICKFIX)
	    ਜੇ (g_do_tagpreview != 0)
	    {
		ਜੇ (ptag_ਇੰਦਰਾਜ.tagname != ਨਿਰਾ
			&& STRCMP(ptag_ਇੰਦਰਾਜ.tagname, ਟੈਗ) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_ਮੇਲ = ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ;
		    cur_fnum = ptag_ਇੰਦਰਾਜ.cur_fnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    tagstack_clear_ਇੰਦਰਾਜ(&ptag_ਇੰਦਰਾਜ);
		    ਜੇ ((ptag_ਇੰਦਰਾਜ.tagname = vim_strsave(ਟੈਗ)) == ਨਿਰਾ)
			ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		}
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ਜਦੋਂ (tagstackidx < tagstacklen)
		    tagstack_clear_ਇੰਦਰਾਜ(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ਜੇ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_ਇੰਦਰਾਜ(&tagstack[0]);
		    ਲਈ (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_ਅੰਕੜੇ = ਨਿਰਾ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ਜੇ ((tagstack[tagstackidx].tagname = vim_strsave(ਟੈਗ)) == ਨਿਰਾ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_ਟੈਗ = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (
#ਜੇ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_ਇੰਦਰਾਜ.tagname == ਨਿਰਾ :
#ਜੇ_ਅੰਤ
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_ਟੈਗ_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ));
		ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
	    }

	    ਜੇ (type == DT_ਕੱਢੋ)		// go to older position
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਪੂਰਨ	old_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ
		ਜੇ ((tagstackidx -= ਗਿਣਤੀ_2) < 0)
		{
		    emsg(_(e_at_bottom_ਦਾ_ਟੈਗ_ਢੇਰ));
		    ਜੇ (tagstackidx + ਗਿਣਤੀ_2 == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ਨਹੀਂਤਾਂ ਜੇ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_ਸਿਖਰ_ਦਾ_ਟੈਗ_ਢੇਰ));
		    ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ਜੇ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ਜੇ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.ਖਾਨਾ = saved_fmark.mark.ਖਾਨਾ;
		curwin->w_ਸੈੱਟ_curswant = true;
		check_cursor();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ ((fdo_flags & FDO_ਟੈਗ) && old_ਕੁੰਜੀ_Typed)
		    foldOpenCursor();
#ਜੇ_ਅੰਤ

		// remove the old list of matches
		FreeWild(ਅੰਕ_matches, matches);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		cs_free_ਟੈਗ_ਸੂਚੀ();
#ਜੇ_ਅੰਤ
		ਅੰਕ_matches = 0;
		ਟੈਗ_freematch();
		ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
	    }

	    ਜੇ (type == DT_ਟੈਗ
#ਜੇ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#ਜੇ_ਅੰਤ
	       )
	    {
#ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (g_do_tagpreview != 0)
		{
		    cur_ਮੇਲ = ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ;
		    cur_fnum = ptag_ਇੰਦਰਾਜ.cur_fnum;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ਜੇ ((tagstackidx += ਗਿਣਤੀ_2 - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_ਸਿਖਰ_ਦਾ_ਟੈਗ_ਢੇਰ));
			save_pos = FALSE;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_ਦਾ_ਟੈਗ_ਢੇਰ));
			tagstackidx = 0;
			ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		    }
		    cur_ਮੇਲ = tagstack[tagstackidx].cur_ਮੇਲ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_ਟੈਗ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ਜੇ defined(FEAT_QUICKFIX)
		ਜੇ (g_do_tagpreview != 0)
		{
		    cur_ਮੇਲ = ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ;
		    cur_fnum = ptag_ਇੰਦਰਾਜ.cur_fnum;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    ਜੇ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_ਮੇਲ = tagstack[tagstackidx].cur_ਮੇਲ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		ਚੋਣ (type)
		{
		    ਹਾਲਤ DT_ਪਹਿਲਾ: cur_ਮੇਲ = ਗਿਣਤੀ_2 - 1; ਰੋਕੋ;
		    ਹਾਲਤ DT_SELECT:
		    ਹਾਲਤ DT_JUMP:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		    ਹਾਲਤ DT_CSCOPE:
#ਜੇ_ਅੰਤ
		    ਹਾਲਤ DT_ਆਖਰੀ:  cur_ਮੇਲ = MAXCOL - 1; ਰੋਕੋ;
		    ਹਾਲਤ DT_ਅਗਲਾ:  cur_ਮੇਲ += ਗਿਣਤੀ_2; ਰੋਕੋ;
		    ਹਾਲਤ DT_PREV:  cur_ਮੇਲ -= ਗਿਣਤੀ_2; ਰੋਕੋ;
		}
		ਜੇ (cur_ਮੇਲ >= MAXCOL)
		    cur_ਮੇਲ = MAXCOL - 1;
		ਨਹੀਂਤਾਂ ਜੇ (cur_ਮੇਲ < 0)
		{
		    emsg(_(e_cannot_go_before_ਪਹਿਲਾ_matching_ਟੈਗ));
		    skip_ਸੁਨੇਹਾ = TRUE;
		    cur_ਮੇਲ = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ਜੇ defined(FEAT_QUICKFIX)
	ਜੇ (g_do_tagpreview != 0)
	{
	    ਜੇ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ = cur_ਮੇਲ;
		ptag_ਇੰਦਰਾਜ.cur_fnum = cur_fnum;
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ਜੇ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ਜੇ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_ਮੇਲ = cur_ਮੇਲ;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ਜੇ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ਜੇ (buf != ਨਿਰਾ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ਜੇ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ਜੇ defined(FEAT_QUICKFIX)
	ਨਹੀਂਤਾਂ ਜੇ (g_do_tagpreview != 0)
	    name = ptag_ਇੰਦਰਾਜ.tagname;
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    name = ਟੈਗ;
	other_name = (tagmatchname == ਨਿਰਾ || STRCMP(tagmatchname, name) != 0);
	ਜੇ (new_ਟੈਗ
		|| (cur_ਮੇਲ >= ਅੰਕ_matches && max_ਅੰਕ_matches != MAXCOL)
		|| other_name)
	{
	    ਜੇ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ਜੇ (type == DT_SELECT || type == DT_JUMP
#ਜੇ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#ਜੇ_ਅੰਤ
		)
		cur_ਮੇਲ = MAXCOL - 1;
	    ਜੇ (type == DT_ਟੈਗ)
		max_ਅੰਕ_matches = MAXCOL;
	    ਨਹੀਂਤਾਂ
		max_ਅੰਕ_matches = cur_ਮੇਲ + 1;

	    // when the argument starts with '/', use it as a regexp
	    ਜੇ (!no_regexp && *name == '/')
	    {
		flags = ਟੈਗ_REGEXP;
		++name;
	    }
	    ਨਹੀਂਤਾਂ
		flags = ਟੈਗ_NOIC;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	    ਜੇ (type == DT_CSCOPE)
		flags = ਟੈਗ_CSCOPE;
#ਜੇ_ਅੰਤ
	    ਜੇ (verbose)
		flags |= ਟੈਗ_VERBOSE;

	    ਜੇ (!use_tfu)
		flags |= ਟੈਗ_NO_TAGFUNC;

	    ਜੇ (ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(name, &new_ਅੰਕ_matches, &new_matches, flags,
					    max_ਅੰਕ_matches, buf_ffname) == OK
		    && new_ਅੰਕ_matches < max_ਅੰਕ_matches)
		max_ਅੰਕ_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ਜੇ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_ਖਿੜਕੀ_unexpectedly_close_while_searching_for_ਟੈਗ_ਸੂਚੀ));
		FreeWild(new_ਅੰਕ_matches, new_matches);
		ਰੋਕੋ;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ਜੇ (!new_ਟੈਗ && !other_name)
	    {
		ਪੂਰਨ	    j, k;
		ਪੂਰਨ	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		ਲਈ (j = 0; j < ਅੰਕ_matches; ++j)
		{
		    parse_ਮੇਲ(matches[j], &tagp);
		    ਲਈ (i = idx; i < new_ਅੰਕ_matches; ++i)
		    {
			parse_ਮੇਲ(new_matches[i], &tagp2);
			ਜੇ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    ਲਈ (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ਰੋਕੋ;
			}
		    }
		}
	    }
	    FreeWild(ਅੰਕ_matches, matches);
	    ਅੰਕ_matches = new_ਅੰਕ_matches;
	    matches = new_matches;
	}

	ਜੇ (ਅੰਕ_matches <= 0)
	{
	    ਜੇ (verbose)
		semsg(_(e_ਟੈਗ_not_ਮਿਲਿਆ_str), name);
#ਜੇ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ ask_for_selection = FALSE;
#ਜੇ defined(FEAT_EVAL)
	    size_t  IObufflen;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	    ਜੇ (type == DT_CSCOPE && ਅੰਕ_matches > 1)
	    {
		cs_print_ਟੈਗ_ਸੂਚੀ();
		ask_for_selection = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜੇ (type == DT_ਟੈਗ && *ਟੈਗ != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_ਮੇਲ = ਗਿਣਤੀ_2 > 0 ? ਗਿਣਤੀ_2 - 1 : 0;
	    ਨਹੀਂਤਾਂ ਜੇ (type == DT_SELECT || (type == DT_JUMP && ਅੰਕ_matches > 1))
	    {
		print_ਟੈਗ_list(new_ਟੈਗ, use_tagstack, ਅੰਕ_matches, matches);
		ask_for_selection = TRUE;
	    }
#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ਨਹੀਂਤਾਂ ਜੇ (type == DT_LTAG)
	    {
		ਜੇ (ਜੋੜੋ_llist_ਟੈਗ_ਸੂਚੀ(ਟੈਗ, ਅੰਕ_matches, matches) == FAIL)
		    ਜਾਓ ਸਿਰਾ_do_ਟੈਗ;
		cur_ਮੇਲ = 0;		// Jump to the first tag
	    }
#ਜੇ_ਅੰਤ

	    ਜੇ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_ਸੰਖਿਆ(ਨਿਰਾ);
		ਜੇ (i <= 0 || i > ਅੰਕ_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ਜੇ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		    cs_free_ਟੈਗ_ਸੂਚੀ();
		    jumped_ਮੰਜ਼ਿਲ_ਟੈਗ = TRUE;
#ਜੇ_ਅੰਤ
		    ਰੋਕੋ;
		}
		cur_ਮੇਲ = i - 1;
	    }

	    ਜੇ (cur_ਮੇਲ >= ਅੰਕ_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ਜੇ ((type == DT_ਅਗਲਾ || type == DT_ਪਹਿਲਾ)
						      && nofile_fname == ਨਿਰਾ)
		{
		    ਜੇ (ਅੰਕ_matches == 1)
			emsg(_(e_there_is_only_one_matching_ਟੈਗ));
		    ਨਹੀਂਤਾਂ
			emsg(_(e_cannot_go_beyond_ਆਖਰੀ_matching_ਟੈਗ));
		    skip_ਸੁਨੇਹਾ = TRUE;
		}
		cur_ਮੇਲ = ਅੰਕ_matches - 1;
	    }
	    ਜੇ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_ਮੇਲ = cur_ਮੇਲ;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ਜੇ (use_tfu && parse_ਮੇਲ(matches[cur_ਮੇਲ], &tagp) == OK
			&& tagp.user_ਅੰਕੜੇ)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_ਅੰਕੜੇ);
		    tagstack[tagstackidx].user_ਅੰਕੜੇ = vim_strnsave(
			  tagp.user_ਅੰਕੜੇ, tagp.user_ਅੰਕੜੇ_ਸਿਰਾ - tagp.user_ਅੰਕੜੇ);
		}

		++tagstackidx;
	    }
#ਜੇ defined(FEAT_QUICKFIX)
	    ਨਹੀਂਤਾਂ ਜੇ (g_do_tagpreview != 0)
	    {
		ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ = cur_ਮੇਲ;
		ptag_ਇੰਦਰਾਜ.cur_fnum = cur_fnum;
	    }
#ਜੇ_ਅੰਤ

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ਜੇ (nofile_fname != ਨਿਰਾ && error_cur_ਮੇਲ != cur_ਮੇਲ)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_ਮੇਲ][0] & MT_IC_OFF);
	    ਜੇ (type != DT_ਟੈਗ && type != DT_SELECT && type != DT_JUMP
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		&& type != DT_CSCOPE
#ਜੇ_ਅੰਤ
		&& (ਅੰਕ_matches > 1 || ic)
		&& !skip_ਸੁਨੇਹਾ)
	    {
		// Give an indication of the number of matching tags
		sprintf((ਅੱਖਰ *)IObuff, _("tag %d of %d%s"),
				cur_ਮੇਲ + 1,
				ਅੰਕ_matches,
				max_ਅੰਕ_matches != MAXCOL ? _(" or more") : "");
		ਜੇ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ਜੇ ((ਅੰਕ_matches > prev_ਅੰਕ_matches || new_ਟੈਗ)
							   && ਅੰਕ_matches > 1)
		{
		    ਜੇ (ic)
			ਸੁਨੇਹਾ_attr((ਅੱਖਰ *)IObuff, HL_ATTR(HLF_W));
		    ਨਹੀਂਤਾਂ
			ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
		    ਸੁਨੇਹਾ_scroll = TRUE;	// don't overwrite this message
		}
		ਨਹੀਂਤਾਂ
		    give_warning(IObuff, ic);
		ਜੇ (ic && !ਸੁਨੇਹਾ_scrolled && ਸੁਨੇਹਾ_silent == 0)
		{
		    ਬਾਹਰਲਾ_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ਜੇ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE, ":ta %s\r", name);
	    ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, IObuff, (ਪੂਰਨ)IObufflen);
#ਜੇ_ਅੰਤ

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_ਟੈਗ(matches[cur_ਮੇਲ], forceit, type != DT_CSCOPE);

#ਜੇ defined(FEAT_EVAL)
	    ਸੈੱਟ_vim_var_string(VV_SWAPCOMMAND, ਨਿਰਾ, -1);
#ਜੇ_ਅੰਤ

	    ਜੇ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ਜੇ ((type == DT_PREV && cur_ਮੇਲ > 0)
			|| ((type == DT_ਟੈਗ || type == DT_ਅਗਲਾ
							  || type == DT_ਪਹਿਲਾ)
			    && (max_ਅੰਕ_matches != MAXCOL
					     || cur_ਮੇਲ < ਅੰਕ_matches - 1)))
		{
		    error_cur_ਮੇਲ = cur_ਮੇਲ;
		    ਜੇ (use_tagstack)
			--tagstackidx;
		    ਜੇ (type == DT_PREV)
			--cur_ਮੇਲ;
		    ਨਹੀਂਤਾਂ
		    {
			type = DT_ਅਗਲਾ;
			++cur_ਮੇਲ;
		    }
		    ਜਾਰੀ;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ਜੇ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		jumped_ਮੰਜ਼ਿਲ_ਟੈਗ = TRUE;
#ਜੇ_ਅੰਤ
	    }
	}
	ਰੋਕੋ;
    }

ਸਿਰਾ_do_ਟੈਗ:
    // Only store the new index when using the tagstack and it's valid.
    ਜੇ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_ਵੰਡੋ = 0;	// don't split next time
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#ਜੇ_ਅੰਤ

    vim_free(tofree);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਵਾਪਸ jumped_ਮੰਜ਼ਿਲ_ਟੈਗ;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * List all the matching tags.
 */
    ਸਥਿਰ ਖਾਲੀ
print_ਟੈਗ_list(
    ਪੂਰਨ		new_ਟੈਗ,
    ਪੂਰਨ		use_tagstack,
    ਪੂਰਨ		ਅੰਕ_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ਪੂਰਨ		tagstackidx = curwin->w_tagstackidx;
    ਪੂਰਨ		i;
    char_u	*p;
    char_u	*command_ਸਿਰਾ;
    tagptrs_T	tagp;
    ਪੂਰਨ		taglen;
    ਪੂਰਨ		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_ਮੇਲ(matches[0], &tagp);
    taglen = (ਪੂਰਨ)(tagp.tagname_ਸਿਰਾ - tagp.tagname + 2);
    ਜੇ (taglen < 18)
	taglen = 18;
    ਜੇ (taglen > cmdline_ਚੌੜਾਈ - 25)
	taglen = MAXCOL;
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
	ਸੁਨੇਹਾ_didout = FALSE;	// overwrite previous message
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    ਸੁਨੇਹਾ_clr_eos();
    taglen_advance(taglen);
    ਸੁਨੇਹਾ_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    ਲਈ (i = 0; i < ਅੰਕ_matches && !got_int; ++i)
    {
	parse_ਮੇਲ(matches[i], &tagp);
	ਜੇ (!new_ਟੈਗ && (
#ਜੇ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_ਇੰਦਰਾਜ.cur_ਮੇਲ) ||
#ਜੇ_ਅੰਤ
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_ਮੇਲ)))
	    *IObuff = '>';
	ਨਹੀਂਤਾਂ
	    *IObuff = ' ';
	vim_snprintf((ਅੱਖਰ *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_ਨਾਂ[matches[i][0] & MT_ਨਕਾਬ]);
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	ਜੇ (tagp.tagkind != ਨਿਰਾ)
	    ਸੁਨੇਹਾ_outtrans_len(tagp.tagkind,
			  (ਪੂਰਨ)(tagp.tagkind_ਸਿਰਾ - tagp.tagkind));
	ਸੁਨੇਹਾ_advance(13);
	ਸੁਨੇਹਾ_outtrans_len_attr(tagp.tagname,
			   (ਪੂਰਨ)(tagp.tagname_ਸਿਰਾ - tagp.tagname),
						  HL_ATTR(HLF_T));
	ਸੁਨੇਹਾ_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = ਟੈਗ_ਭਰਿਆ_fname(&tagp);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 0)
	    ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)
	    ਰੋਕੋ;
	ਸੁਨੇਹਾ_advance(15);

	// print any extra fields
	command_ਸਿਰਾ = tagp.command_ਸਿਰਾ;
	ਜੇ (command_ਸਿਰਾ != ਨਿਰਾ)
	{
	    p = command_ਸਿਰਾ + 3;
	    ਜਦੋਂ (*p && *p != '\r' && *p != '\n')
	    {
		ਜਦੋਂ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ਜੇ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ਜਾਰੀ;
		}
		// skip "kind:<kind>" and "<kind>"
		ਜੇ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_ਸਿਰਾ;
		    ਜਾਰੀ;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ਜਦੋਂ (*p && *p != '\r' && *p != '\n')
		{
		    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ + ptr2cells(p) >= cmdline_ਚੌੜਾਈ)
		    {
			ਸੁਨੇਹਾ_putchar('\n');
			ਜੇ (got_int)
			    ਰੋਕੋ;
			ਸੁਨੇਹਾ_advance(15);
		    }
		    p = ਸੁਨੇਹਾ_outtrans_one(p, attr);
		    ਜੇ (*p == TAB)
		    {
			ਸੁਨੇਹਾ_puts_attr(" ", attr);
			ਰੋਕੋ;
		    }
		    ਜੇ (*p == ':')
			attr = 0;
		}
	    }
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ > 15)
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਜੇ (got_int)
		    ਰੋਕੋ;
		ਸੁਨੇਹਾ_advance(15);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_ਸਿਰਾ = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ਜੇ (*p == '/' || *p == '?')
	{
	    ++p;
	    ਜੇ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ਜਦੋਂ (p != command_ਸਿਰਾ && vim_isspace(*p))
	    ++p;

	ਜਦੋਂ (p != command_ਸਿਰਾ)
	{
	    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_ਚੌੜਾਈ)
		ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    ਸੁਨੇਹਾ_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ਜੇ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ਜੇ (*p == TAB)
	    {
		ਸੁਨੇਹਾ_putchar(' ');
		++p;
	    }
	    ਨਹੀਂਤਾਂ
		p = ਸੁਨੇਹਾ_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ਜੇ (p == command_ਸਿਰਾ - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ਰੋਕੋ;
	    // don't display matching '/' or '?'
	    ਜੇ (p == command_ਸਿਰਾ - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ਰੋਕੋ;
	}
	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ)
	    ਸੁਨੇਹਾ_putchar('\n');
	ui_breakcheck();
    }
    ਜੇ (got_int)
	got_int = FALSE;	// only stop the listing
}

#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_llist_ਟੈਗ_ਸੂਚੀ(
    char_u	*ਟੈਗ,
    ਪੂਰਨ		ਅੰਕ_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	ਟੈਗ_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    ਪੂਰਨ		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ਜੇ (list == ਨਿਰਾ || fname == ਨਿਰਾ || cmd == ਨਿਰਾ)
    {
	vim_free(cmd);
	vim_free(fname);
	ਜੇ (list != ਨਿਰਾ)
	    list_free(list);
	ਵਾਪਸ FAIL;
    }

    ਲਈ (i = 0; i < ਅੰਕ_matches; ++i)
    {
	ਪੂਰਨ	    len, cmd_len;
	ਲੰਮਾ    lnum;
	dict_T  *dict;

	parse_ਮੇਲ(matches[i], &tagp);

	// Save the tag name
	len = (ਪੂਰਨ)(tagp.tagname_ਸਿਰਾ - tagp.tagname);
	ਜੇ (len > 128)
	    len = 128;
	vim_strncpy(ਟੈਗ_name, tagp.tagname, len);
	ਟੈਗ_name[len] = NUL;

	// Save the tag file name
	p = ਟੈਗ_ਭਰਿਆ_fname(&tagp);
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਰੀ;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((ਅੱਖਰ *)tagp.command);
	ਨਹੀਂਤਾਂ
	{
	    char_u *cmd_ਸ਼ੁਰੂ, *cmd_ਸਿਰਾ;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_ਸ਼ੁਰੂ = tagp.command;
	    cmd_ਸਿਰਾ = tagp.command_ਸਿਰਾ;
	    ਜੇ (cmd_ਸਿਰਾ == ਨਿਰਾ)
	    {
		ਲਈ (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_ਸਿਰਾ = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_ਸਿਰਾ--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    ਜੇ (*cmd_ਸ਼ੁਰੂ == '/' || *cmd_ਸ਼ੁਰੂ == '?')
		cmd_ਸ਼ੁਰੂ++;

	    ਜੇ (*cmd_ਸਿਰਾ == '/' || *cmd_ਸਿਰਾ == '?')
		cmd_ਸਿਰਾ--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ਜੇ (*cmd_ਸ਼ੁਰੂ == '^')
	    {
		STRCPY(cmd, "^");
		cmd_ਸ਼ੁਰੂ++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (ਪੂਰਨ)(cmd_ਸਿਰਾ - cmd_ਸ਼ੁਰੂ + 1);
	    ਜੇ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_ਸ਼ੁਰੂ, cmd_len);
	    len += cmd_len;

	    ਜੇ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ਜੇ ((dict = dict_alloc()) == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ਜਾਰੀ;
	}

	dict_ਜੋੜੋ_string(dict, "text", ਟੈਗ_name);
	dict_ਜੋੜੋ_string(dict, "filename", fname);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", lnum);
	ਜੇ (lnum == 0)
	    dict_ਜੋੜੋ_string(dict, "pattern", cmd);
    }

    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "ltag %s", ਟੈਗ);
    ਸੈੱਟ_errorlist(curwin, list, ' ', IObuff, ਨਿਰਾ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Free cached tags.
 */
    ਖਾਲੀ
ਟੈਗ_freematch(ਖਾਲੀ)
{
    VIM_CLEAR(tagmatchname);
}

    ਸਥਿਰ ਖਾਲੀ
taglen_advance(ਪੂਰਨ l)
{
    ਜੇ (l == MAXCOL)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਸੁਨੇਹਾ_advance(24);
    }
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_advance(13 + l);
}

/*
 * Print the tag stack
 */
    ਖਾਲੀ
do_ਟੈਗ_ਸੂਚੀ(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    ਪੂਰਨ		tagstackidx = curwin->w_tagstackidx;
    ਪੂਰਨ		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    ਸੁਨੇਹਾ_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    ਲਈ (i = 0; i < tagstacklen; ++i)
    {
	ਜੇ (tagstack[i].tagname != ਨਿਰਾ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ਜੇ (name == ਨਿਰਾ)	    // file name not available
		ਜਾਰੀ;

	    ਸੁਨੇਹਾ_putchar('\n');
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_ਮੇਲ + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    ਸੁਨੇਹਾ_outtrans(IObuff);
	    ਸੁਨੇਹਾ_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	ਬਾਹਰਲਾ_flush();		    // show one line at a time
    }
    ਜੇ (tagstackidx == tagstacklen)	// idx at top of stack
	ਸੁਨੇਹਾ_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ਸਥਿਰ ਪੂਰਨ
ਟੈਗ_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    ਪੂਰਨ		i;

    ਜਦੋਂ (len > 0)
    {
	i = (ਪੂਰਨ)TOUPPER_ASC(*s1) - (ਪੂਰਨ)TOUPPER_ASC(*s2);
	ਜੇ (i != 0)
	    ਵਾਪਸ i;			// this character different
	ਜੇ (*s1 == NUL)
	    ਰੋਕੋ;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ਵਾਪਸ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*pat;		// the pattern
    ਪੂਰਨ		len;		// length of pat[]
    char_u	*ਸਿਰ;		// start of pattern head
    ਪੂਰਨ		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_pats(pat_T *pats, ਪੂਰਨ has_re)
{
    pats->ਸਿਰ = pats->pat;
    pats->headlen = pats->len;
    ਜੇ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ਜੇ (pats->pat[0] == '^')
	    pats->ਸਿਰ = pats->pat + 1;
	ਨਹੀਂਤਾਂ ਜੇ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->ਸਿਰ = pats->pat + 2;
	ਜੇ (pats->ਸਿਰ == pats->pat)
	    pats->headlen = 0;
	ਨਹੀਂਤਾਂ
	    ਲਈ (pats->headlen = 0; pats->ਸਿਰ[pats->headlen] != NUL;
							      ++pats->headlen)
		ਜੇ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->ਸਿਰ[pats->headlen]) != ਨਿਰਾ)
		    ਰੋਕੋ;
	ਜੇ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ਜੇ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ਨਹੀਂਤਾਂ
	pats->regmatch.regprog = ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ਪਤਾ,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ਸਥਿਰ tagcmd_T
tagfunc_cmd_ਕਿਸਮ(char_u *cmd)
{
    ਜੇ (VIM_ISDIGIT(*cmd))
	ਵਾਪਸ *skipdigits(cmd) == NUL ? TAGCMD_ਪਤਾ : TAGCMD_INVALID;
    ਜੇ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*ਸਿਰਾ = skip_regexp(cmd + 1, *cmd, FALSE);

	ਵਾਪਸ (*ਸਿਰਾ == *cmd && ਸਿਰਾ[1] == NUL)
					? TAGCMD_ਪਤਾ : TAGCMD_INVALID;
    }
    ਜੇ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ਨਿਰਾ)
	ਵਾਪਸ TAGCMD_INVALID;
    ਵਾਪਸ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_tagfunc_ਟੈਗ_ਸੂਚੀ(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    ਪੂਰਨ		*ਮੇਲ_ਗਿਣਤੀ,	// here the number of tags found will be placed
    ਪੂਰਨ		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *ਵਸਤੂ;
    ਪੂਰਨ		ntags = 0;
    ਪੂਰਨ		ਨਤੀਜਾ = FAIL;
    typval_T	ਦਲੀਲਾਂ[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*ਟੈਗ = &curwin->w_tagstack[curwin->w_tagstackidx];

    ਜੇ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ਨਿਰਾ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	ਵਾਪਸ FAIL;

    ਦਲੀਲਾਂ[0].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[0].vval.v_string = pat;
    ਦਲੀਲਾਂ[1].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[1].vval.v_string = flagString;

    // create 'info' dict argument
    ਜੇ ((d = dict_alloc_ਤਾਲਾ(VAR_FIXED)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (!(flags & ਟੈਗ_INS_COMP) && ਟੈਗ->user_ਅੰਕੜੇ != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(d, "user_data", ਟੈਗ->user_ਅੰਕੜੇ);
    ਜੇ (buf_ffname != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    ਦਲੀਲਾਂ[2].v_type = VAR_DICT;
    ਦਲੀਲਾਂ[2].vval.v_dict = d;

    ਦਲੀਲਾਂ[3].v_type = VAR_UNKNOWN;

    vim_snprintf((ਅੱਖਰ *)flagString, ਆਕਾਰ(flagString),
		 "%s%s%s",
		 g_ਟੈਗ_at_cursor      ? "c": "",
		 flags & ਟੈਗ_INS_COMP ? "i": "",
		 flags & ਟੈਗ_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    ਨਤੀਜਾ = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, ਦਲੀਲਾਂ);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ਜੇ (ਨਤੀਜਾ == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_ਸੰਖਿਆ == VVAL_NULL)
    {
	clear_tv(&rettv);
	ਵਾਪਸ NOTDONE;
    }
    ਜੇ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_ਮੁੱਲ_from_tagfunc));
	ਵਾਪਸ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(taglist, ਵਸਤੂ)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_ਕਿਸਮ;
	ਪੂਰਨ		len;
	dict_iterator_T	iter;
	char_u		*dict_ਕੁੰਜੀ;
	typval_T	*tv;
	ਪੂਰਨ		has_extra = 0;
	ਪੂਰਨ		name_only = flags & ਟੈਗ_ਨਾਂ;

	ਜੇ (ਵਸਤੂ->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_ਮੁੱਲ_from_tagfunc));
	    ਰੋਕੋ;
	}

# ifdef FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	len = 3;
# ਨਹੀਂਤਾਂ
	len = 2;
# endif
	res_name = ਨਿਰਾ;
	res_fname = ਨਿਰਾ;
	res_cmd = ਨਿਰਾ;
	res_ਕਿਸਮ = ਨਿਰਾ;

	dict_iterate_ਸ਼ੁਰੂ(&ਵਸਤੂ->li_tv, &iter);
	ਜਦੋਂ (ਨਿਰਾ != (dict_ਕੁੰਜੀ = dict_iterate_ਅਗਲਾ(&iter, &tv)))
	{
	    ਜੇ (tv->v_type != VAR_STRING || tv->vval.v_string == ਨਿਰਾ)
		ਜਾਰੀ;

	    len += (ਪੂਰਨ)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ਜਾਰੀ;
	    }
	    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ਜਾਰੀ;
	    }
	    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ਜਾਰੀ;
	    }
	    has_extra = 1;
	    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "kind") == 0)
	    {
		res_ਕਿਸਮ = tv->vval.v_string;
		ਜਾਰੀ;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (ਪੂਰਨ)STRLEN(dict_ਕੁੰਜੀ) + 1;
	}

	ਜੇ (has_extra)
	    len += 2;	// need space for ;"

	ਜੇ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_ਮੁੱਲ_from_tagfunc));
	    ਰੋਕੋ;
	}

	tagcmd_T cmdkind = tagfunc_cmd_ਕਿਸਮ(res_cmd);
	ਜੇ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_ਮੁੱਲ_from_tagfunc));
	    ਰੋਕੋ;
	}
	ਜੇ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ਜੇ (name_only)
	    mfp = vim_strsave(res_name);
	ਨਹੀਂਤਾਂ
	    mfp = alloc(ਆਕਾਰ(char_u) + len + 1);

	ਜੇ (mfp == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = ਟੈਗ_SEP;	    // no tag file name
# ifdef FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	    *p++ = ਟੈਗ_SEP;
# endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    ਜੇ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ਜੇ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ਜੇ (res_ਕਿਸਮ)
		{
		    *p++ = TAB;
		    STRCPY(p, res_ਕਿਸਮ);
		    p += STRLEN(p);
		}

		dict_iterate_ਸ਼ੁਰੂ(&ਵਸਤੂ->li_tv, &iter);
		ਜਦੋਂ (ਨਿਰਾ != (dict_ਕੁੰਜੀ = dict_iterate_ਅਗਲਾ(&iter, &tv)))
		{
		    ਜੇ (tv->v_type != VAR_STRING || tv->vval.v_string == ਨਿਰਾ)
			ਜਾਰੀ;

		    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "name") == 0)
			ਜਾਰੀ;
		    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "filename") == 0)
			ਜਾਰੀ;
		    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "cmd") == 0)
			ਜਾਰੀ;
		    ਜੇ (STRCMP(dict_ਕੁੰਜੀ, "kind") == 0)
			ਜਾਰੀ;

		    *p++ = TAB;
		    STRCPY(p, dict_ਕੁੰਜੀ);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	ਜੇ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_ਅੰਕੜੇ))[ga->ga_len++] = mfp;
	    ++ntags;
	    ਨਤੀਜਾ = OK;
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_free(mfp);
	    ਰੋਕੋ;
	}
    }

    clear_tv(&rettv);

    *ਮੇਲ_ਗਿਣਤੀ = ntags;
    ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * State information used during a tag search
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    tagsearch_state_T	state;		// tag search state
    ਪੂਰਨ		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    ਪੂਰਨ		lbuf_ਮਾਪ;		// length of lbuf
    char_u	*ਟੈਗ_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    ਪੂਰਨ		flags;			// flags used for tag search
    ਪੂਰਨ		ਟੈਗ_file_sorted;	// !_TAG_FILE_SORTED value
    ਪੂਰਨ		get_searchpat;		// used for 'showfulltag'
    ਪੂਰਨ		help_only;		// only search for help tags
    ਪੂਰਨ		did_open;		// did open a tag file
    ਪੂਰਨ		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    ਪੂਰਨ		ਰੇਖਿਕ;			// do a linear search
    vimconv_T	vimconv;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਪੂਰਨ		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    ਪੂਰਨ		help_pri;		// help language priority
    char_u	*help_lang_ਲੱਭੋ;	// lang to be found
    ਪੂਰਨ		is_txt;			// flag of file extension
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਮੇਲ_ਗਿਣਤੀ;		// number of matches found
    garray_T	ga_ਮੇਲ[MT_ਗਿਣਤੀ];	// stores matches in sequence
    hashtab_T	ht_ਮੇਲ[MT_ਗਿਣਤੀ];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    ਪੂਰਨ			flags,
    ਪੂਰਨ			mincount)
{
    ਪੂਰਨ		mtt;

    st->ਟੈਗ_fname = alloc(MAXPATHL + 1);
    st->fp = ਨਿਰਾ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (ਪੂਰਨ)STRLEN(pat);
    st->orgpat->regmatch.regprog = ਨਿਰਾ;
    st->flags = flags;
    st->ਟੈਗ_file_sorted = NUL;
    st->help_only = (flags & ਟੈਗ_HELP);
    st->get_searchpat = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_ਲੱਭੋ = ਨਿਰਾ;
    st->is_txt = FALSE;
#ਜੇ_ਅੰਤ
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_ਮਾਪ = LSIZE;
    st->lbuf = alloc(st->lbuf_ਮਾਪ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    st->ebuf = alloc(LSIZE);
#ਜੇ_ਅੰਤ
    st->ਮੇਲ_ਗਿਣਤੀ = 0;
    st->stop_searching = FALSE;

    ਲਈ (mtt = 0; mtt < MT_ਗਿਣਤੀ; ++mtt)
    {
	ga_init2(&st->ga_ਮੇਲ[mtt], ਆਕਾਰ(char_u *), 100);
	hash_init(&st->ht_ਮੇਲ[mtt]);
    }

    // check for out of memory situation
    ਜੇ (st->ਟੈਗ_fname == ਨਿਰਾ
	    || st->lbuf == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	    || st->ebuf == ਨਿਰਾ
#ਜੇ_ਅੰਤ
       )
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Free the state used by find_tags()
 */
    ਸਥਿਰ ਖਾਲੀ
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->ਟੈਗ_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    vim_free(st->ebuf);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_ਵਿੱਚ_help_init(findtags_state_T *st)
{
    ਪੂਰਨ		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ਜੇ (st->is_txt)
	STRCPY(st->help_lang, "en");
    ਨਹੀਂਤਾਂ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (ਪੂਰਨ)STRLEN(st->ਟੈਗ_fname);
	ਜੇ (i > 3 && st->ਟੈਗ_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->ਟੈਗ_fname + i - 2, 2);
	ਨਹੀਂਤਾਂ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ਜੇ (st->help_lang_ਲੱਭੋ != ਨਿਰਾ
	    && STRICMP(st->help_lang, st->help_lang_ਲੱਭੋ) != 0)
	ਵਾਪਸ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ਜੇ ((st->flags & ਟੈਗ_KEEP_LANG)
	    && st->help_lang_ਲੱਭੋ == ਨਿਰਾ
	    && curbuf->b_fname != ਨਿਰਾ
	    && (i = (ਪੂਰਨ)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ਨਹੀਂਤਾਂ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	ਲਈ (s = p_hlg; *s != NUL; ++s)
	{
	    ਜੇ (STRNICMP(s, st->help_lang, 2) == 0)
		ਰੋਕੋ;
	    ++st->help_pri;
	    ਜੇ ((s = vim_strchr(s, ',')) == ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਜੇ (s == ਨਿਰਾ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ਜੇ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    ਪੂਰਨ		use_tfu = ((st->flags & ਟੈਗ_NO_TAGFUNC) == 0);
    ਪੂਰਨ		retval;

    ਜੇ (!use_tfu || tfu_ਵਿੱਚ_use || *curbuf->b_p_tfu == NUL)
	ਵਾਪਸ NOTDONE;

    tfu_ਵਿੱਚ_use = TRUE;
    retval = ਲੱਭੋ_tagfunc_ਟੈਗ_ਸੂਚੀ(pat, st->ga_ਮੇਲ, &st->ਮੇਲ_ਗਿਣਤੀ,
						st->flags, buf_ffname);
    tfu_ਵਿੱਚ_use = FALSE;

    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_ਮਾਪ 42
ਸਥਿਰ ਬਣਤਰ
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_ਮਾਪ];
ਸਥਿਰ ਪੂਰਨ incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ਸਥਿਰ ਖਾਲੀ
emacs_ਟੈਗ_ਸੂਚੀ_incstack_free(ਖਾਲੀ)
{
    ਜਦੋਂ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ਨਿਰਾ;
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
    ਸਥਿਰ ਖਾਲੀ
emacs_ਟੈਗ_ਸੂਚੀ_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ਜੇ (vim_fgets(st->ebuf, LSIZE, st->fp))
	ਵਾਪਸ;

    ਲਈ (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ਜੇ (*p == NUL)
	ਵਾਪਸ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ਜੇ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_ਮਾਪ)
	ਵਾਪਸ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->ਟੈਗ_fname);
    ਜੇ (incstack[incstack_idx].etag_fname == ਨਿਰਾ)
	ਵਾਪਸ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ਨਿਰਾ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_ਟੈਗ_fname(st->ebuf, st->ਟੈਗ_fname, FALSE);
    ਜੇ (fullpath_ebuf != ਨਿਰਾ)
    {
	st->fp = mch_fopen((ਅੱਖਰ *)fullpath_ebuf, "r");
	ਜੇ (st->fp != ਨਿਰਾ)
	{
	    ਜੇ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_ਟੈਗ_file_ਰਸਤਾ_truncated_for_str), st->ebuf);
	    vim_strncpy(st->ਟੈਗ_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ਜੇ (st->fp == ਨਿਰਾ)
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
    ਸਥਿਰ ਪੂਰਨ
emacs_ਟੈਗ_ਸੂਚੀ_file_eof(findtags_state_T *st)
{
    ਜੇ (!incstack_idx)	// reached end of file. stop processing.
	ਵਾਪਸ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->ਟੈਗ_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    ਵਾਪਸ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ਸਥਿਰ ਪੂਰਨ
emacs_ਟੈਗ_ਸੂਚੀ_parse_ਸਤਰ(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ਜੇ (p_7f == ਨਿਰਾ)
    {
etag_fail:
	ਜੇ (vim_strchr(lbuf, '\n') != ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	// Truncated line.  Ignore it.
	ਜੇ (p_verbose >= 5)
	{
	    verbose_enter();
	    ਸੁਨੇਹਾ(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_ਸਿਰਾ = lbuf;
	ਵਾਪਸ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ਜੇ (p == ਨਿਰਾ)
	p = p_7f + 1;
    ਨਹੀਂਤਾਂ
	++p;

    ਜੇ (!VIM_ISDIGIT(*p))	    // check for start of line number
	ਜਾਓ etag_fail;
    tagp->command = p;

    ਜੇ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_ਸਿਰਾ = p - 1;
    }
    ਨਹੀਂਤਾਂ			    // second format: isolate tagname
    {
	ਜੇ (p_7f == lbuf)
	    ਜਾਓ etag_fail;

	// find end of tagname
	ਲਈ (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ਜੇ (p == lbuf)
		ਜਾਓ etag_fail;
	tagp->tagname_ਸਿਰਾ = p + 1;
	ਜਦੋਂ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    ਸਥਿਰ ਟੈਗ_ਸੂਚੀ_read_ਸਥਿਤੀ_T
findtags_get_ਅਗਲਾ_ਸਤਰ(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    ਪੂਰਨ		eof;
    off_T	ਵਿੱਥ;

    // For binary search: compute the next offset to use.
    ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ)
    {
	ਵਿੱਥ = sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ + ((sinfo_p->ਉੱਚਾ_ਵਿੱਥ
						- sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ) / 2);
	ਜੇ (ਵਿੱਥ == sinfo_p->curr_ਵਿੱਥ)
	    ਵਾਪਸ ਟੈਗ_ਸੂਚੀ_READ_EOF; // End the binary search without a match.
	ਨਹੀਂਤਾਂ
	    sinfo_p->curr_ਵਿੱਥ = ਵਿੱਥ;
    }

    // Skipping back (after a match during binary search).
    ਨਹੀਂਤਾਂ ਜੇ (st->state == TS_SKIP_ਪਿੱਠ)
    {
	sinfo_p->curr_ਵਿੱਥ -= st->lbuf_ਮਾਪ * 2;
	ਜੇ (sinfo_p->curr_ਵਿੱਥ < 0)
	{
	    sinfo_p->curr_ਵਿੱਥ = 0;
	    rewind(st->fp);
	    st->state = TS_ਕਦਮ_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ || st->state == TS_SKIP_ਪਿੱਠ)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_ਵਿੱਥ_ਵਰਤਿਆ = sinfo_p->curr_ਵਿੱਥ;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_ਵਿੱਥ, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_ਮਾਪ, st->fp);
	ਜੇ (!eof && sinfo_p->curr_ਵਿੱਥ != 0)
	{
	    sinfo_p->curr_ਵਿੱਥ = vim_ftell(st->fp);
	    ਜੇ (sinfo_p->curr_ਵਿੱਥ == sinfo_p->ਉੱਚਾ_ਵਿੱਥ)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ, SEEK_SET);
		sinfo_p->curr_ਵਿੱਥ = sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_ਮਾਪ, st->fp);
	}
	// skip empty and blank lines
	ਜਦੋਂ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_ਵਿੱਥ = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_ਮਾਪ, st->fp);
	}
	ਜੇ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_ਪਿੱਠ;
	    sinfo_p->ਮੇਲ_ਵਿੱਥ = vim_ftell(st->fp);
	    sinfo_p->curr_ਵਿੱਥ = sinfo_p->curr_ਵਿੱਥ_ਵਰਤਿਆ;
	    ਵਾਪਸ ਟੈਗ_ਸੂਚੀ_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ਨਹੀਂਤਾਂ
    {
	// skip empty and blank lines
	ਕਰੋ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	    ਜੇ (st->flags & ਟੈਗ_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_ਮਾਪ);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		eof = vim_fgets(st->lbuf, st->lbuf_ਮਾਪ, st->fp);
	} ਜਦੋਂ (!eof && vim_isblankline(st->lbuf));

	ਜੇ (eof)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	    ਜੇ (emacs_ਟੈਗ_ਸੂਚੀ_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		ਵਾਪਸ ਟੈਗ_ਸੂਚੀ_READ_IGNORE;
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ ਟੈਗ_ਸੂਚੀ_READ_EOF;
	}
    }

    ਵਾਪਸ ਟੈਗ_ਸੂਚੀ_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ਜੇ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	ਵਾਪਸ TRUE;

    // Process the header line.
    ਜੇ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->ਟੈਗ_file_sorted = st->lbuf[18];
    ਜੇ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	ਲਈ (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_ਤਿਆਰੀ(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    ਵਾਪਸ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_ਸ਼ੁਰੂ_state_handler(
    findtags_state_T	*st,
    ਪੂਰਨ			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ		use_cscope = (st->flags & ਟੈਗ_CSCOPE);
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		noic = (st->flags & ਟੈਗ_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ਜੇ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	ਵਾਪਸ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਜੇ (st->ਰੇਖਿਕ || use_cscope)
#ਨਹੀਂਤਾਂ
    ਜੇ (st->ਰੇਖਿਕ)
#ਜੇ_ਅੰਤ
	st->state = TS_ਰੇਖਿਕ;
    ਨਹੀਂਤਾਂ ਜੇ (st->ਟੈਗ_file_sorted == NUL)
	st->state = TS_ਦੁਆਧਾਰੀ;
    ਨਹੀਂਤਾਂ ਜੇ (st->ਟੈਗ_file_sorted == '1')
	st->state = TS_ਦੁਆਧਾਰੀ;
    ਨਹੀਂਤਾਂ ਜੇ (st->ਟੈਗ_file_sorted == '2')
    {
	st->state = TS_ਦੁਆਧਾਰੀ;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ਨਹੀਂਤਾਂ
	st->state = TS_ਰੇਖਿਕ;

    ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->ਰੇਖਿਕ = TRUE;
	st->state = TS_ਰੇਖਿਕ;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ)
    {
	ਜੇ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_ਰੇਖਿਕ;
	ਨਹੀਂਤਾਂ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ = 0;
	    sinfo_p->ਨੀਵਾਂ_char = 0;
	    sinfo_p->ਉੱਚਾ_ਵਿੱਥ = filesize;
	    sinfo_p->curr_ਵਿੱਥ = 0;
	    sinfo_p->ਉੱਚਾ_char = 0xff;
	}
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ tagmatch_ਸਥਿਤੀ_T
findtags_parse_ਸਤਰ(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_ਮੇਲ_ਦਲੀਲਾਂ_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    ਪੂਰਨ		ਸਥਿਤੀ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		cmplen;
    ਪੂਰਨ		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ਜੇ (st->orgpat->headlen
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	    && !st->is_etag
#ਜੇ_ਅੰਤ
       )
    {
	CLEAR_ਖੇਤਰ(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_ਸਿਰਾ = vim_strchr(st->lbuf, TAB);
	ਜੇ (tagpp->tagname_ਸਿਰਾ == ਨਿਰਾ)
	    // Corrupted tag line.
	    ਵਾਪਸ ਟੈਗ_ਮੇਲ_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (ਪੂਰਨ)(tagpp->tagname_ਸਿਰਾ - tagpp->tagname);
	ਜੇ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ਜੇ ((st->flags & ਟੈਗ_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ਨਹੀਂਤਾਂ ਜੇ (st->state == TS_ਰੇਖਿਕ && st->orgpat->headlen != cmplen)
	    ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;

	ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ)
	{
	    // Simplistic check for unsorted tags file.
	    i = (ਪੂਰਨ)tagpp->tagname[0];
	    ਜੇ (margs->sortic)
		i = (ਪੂਰਨ)TOUPPER_ASC(tagpp->tagname[0]);
	    ਜੇ (i < sinfo_p->ਨੀਵਾਂ_char || i > sinfo_p->ਉੱਚਾ_char)
		margs->ਛਾਂਟੋ_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ਜੇ (margs->sortic)
		tagcmp = ਟੈਗ_strnicmp(tagpp->tagname, st->orgpat->ਸਿਰ,
							(size_t)cmplen);
	    ਨਹੀਂਤਾਂ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->ਸਿਰ, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ਜੇ (tagcmp == 0)
	    {
		ਜੇ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ਨਹੀਂਤਾਂ ਜੇ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ਜੇ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_ਪਿੱਠ;
		sinfo_p->ਮੇਲ_ਵਿੱਥ = sinfo_p->curr_ਵਿੱਥ;
		ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;
	    }
	    ਜੇ (tagcmp < 0)
	    {
		sinfo_p->curr_ਵਿੱਥ = vim_ftell(st->fp);
		ਜੇ (sinfo_p->curr_ਵਿੱਥ < sinfo_p->ਉੱਚਾ_ਵਿੱਥ)
		{
		    sinfo_p->ਨੀਵਾਂ_ਵਿੱਥ = sinfo_p->curr_ਵਿੱਥ;
		    ਜੇ (margs->sortic)
			sinfo_p->ਨੀਵਾਂ_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ਨਹੀਂਤਾਂ
			sinfo_p->ਨੀਵਾਂ_char = tagpp->tagname[0];
		    ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;
		}
	    }
	    ਜੇ (tagcmp > 0 && sinfo_p->curr_ਵਿੱਥ != sinfo_p->ਉੱਚਾ_ਵਿੱਥ)
	    {
		sinfo_p->ਉੱਚਾ_ਵਿੱਥ = sinfo_p->curr_ਵਿੱਥ;
		ਜੇ (margs->sortic)
		    sinfo_p->ਉੱਚਾ_char = TOUPPER_ASC(tagpp->tagname[0]);
		ਨਹੀਂਤਾਂ
		    sinfo_p->ਉੱਚਾ_char = tagpp->tagname[0];
		ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;
	    }

	    // No match yet and are at the end of the binary search.
	    ਵਾਪਸ ਟੈਗ_ਮੇਲ_STOP;
	}
	ਨਹੀਂਤਾਂ ਜੇ (st->state == TS_SKIP_ਪਿੱਠ)
	{
	    ਜੇ (MB_STRNICMP(tagpp->tagname, st->orgpat->ਸਿਰ, cmplen) != 0)
		st->state = TS_ਕਦਮ_FORWARD;
	    ਨਹੀਂਤਾਂ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_ਵਿੱਥ = sinfo_p->curr_ਵਿੱਥ_ਵਰਤਿਆ;
	    ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (st->state == TS_ਕਦਮ_FORWARD)
	{
	    ਜੇ (MB_STRNICMP(tagpp->tagname, st->orgpat->ਸਿਰ, cmplen) != 0)
	    {
		ਜੇ ((off_T)vim_ftell(st->fp) > sinfo_p->ਮੇਲ_ਵਿੱਥ)
		    ਵਾਪਸ ਟੈਗ_ਮੇਲ_STOP;	// past last match
		ਨਹੀਂਤਾਂ
		    ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;	// before first match
	    }
	}
	ਨਹੀਂਤਾਂ
	    // skip this match if it can't match
	    ਜੇ (MB_STRNICMP(tagpp->tagname, st->orgpat->ਸਿਰ, cmplen) != 0)
		ਵਾਪਸ ਟੈਗ_ਮੇਲ_ਅਗਲਾ;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_ਸਿਰਾ + 1;
	tagpp->fname_ਸਿਰਾ = vim_strchr(tagpp->fname, TAB);
	ਜੇ (tagpp->fname_ਸਿਰਾ == ਨਿਰਾ)
	    ਸਥਿਤੀ = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    tagpp->command = tagpp->fname_ਸਿਰਾ + 1;
	    ਸਥਿਤੀ = OK;
	}
    }
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = parse_ਟੈਗ_ਸਤਰ(st->lbuf,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
		st->is_etag,
#ਜੇ_ਅੰਤ
		tagpp);

    ਜੇ (ਸਥਿਤੀ == FAIL)
	ਵਾਪਸ ਟੈਗ_ਮੇਲ_FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (st->is_etag)
	tagpp->fname = st->ebuf;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਟੈਗ_ਮੇਲ_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ਸਥਿਰ ਖਾਲੀ
findtags_matchargs_init(findtags_ਮੇਲ_ਦਲੀਲਾਂ_T *margs, ਪੂਰਨ flags)
{
    margs->matchoff = 0;			// match offset
    margs->ਮੇਲ_re = FALSE;			// match with regexp
    margs->ਮੇਲ_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & ਟੈਗ_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->ਛਾਂਟੋ_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_ਮੇਲ_ਟੈਗ(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ਮੇਲ_ਦਲੀਲਾਂ_T *margs)
{
    ਪੂਰਨ		ਮੇਲ = FALSE;
    ਪੂਰਨ		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (ਪੂਰਨ)(tagpp->tagname_ਸਿਰਾ - tagpp->tagname);
    ਜੇ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ਜੇ (st->orgpat->len != cmplen)
	ਮੇਲ = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (st->orgpat->regmatch.rm_ic)
	{
	    ਮੇਲ =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ਜੇ (ਮੇਲ)
		margs->ਮੇਲ_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ਨਹੀਂਤਾਂ
	    ਮੇਲ = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->ਮੇਲ_re = FALSE;
    ਜੇ (!ਮੇਲ && st->orgpat->regmatch.regprog != ਨਿਰਾ)
    {
	ਪੂਰਨ	cc;

	cc = *tagpp->tagname_ਸਿਰਾ;
	*tagpp->tagname_ਸਿਰਾ = NUL;
	ਮੇਲ = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ਜੇ (ਮੇਲ)
	{
	    margs->matchoff = (ਪੂਰਨ)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ਜੇ (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->ਮੇਲ_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_ਸਿਰਾ = cc;
	margs->ਮੇਲ_re = TRUE;
    }

    ਵਾਪਸ ਮੇਲ;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    ਸਥਿਰ ਖਾਲੀ
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_ਸਤਰ;
    ਪੂਰਨ		len;

    conv_ਸਤਰ = string_convert(&st->vimconv, st->lbuf, ਨਿਰਾ);
    ਜੇ (conv_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Copy or swap lbuf and conv_line.
    len = (ਪੂਰਨ)STRLEN(conv_ਸਤਰ) + 1;
    ਜੇ (len > st->lbuf_ਮਾਪ)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_ਸਤਰ;
	st->lbuf_ਮਾਪ = len;
    }
    ਨਹੀਂਤਾਂ
    {
	STRCPY(st->lbuf, conv_ਸਤਰ);
	vim_free(conv_ਸਤਰ);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_ਜੋੜੋ_ਮੇਲ(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ਮੇਲ_ਦਲੀਲਾਂ_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ		use_cscope = (st->flags & ਟੈਗ_CSCOPE);
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		name_only = (st->flags & ਟੈਗ_ਨਾਂ);
    ਪੂਰਨ		mtt;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		is_current;		// file name matches
    ਪੂਰਨ		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਜੇ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
		st->is_etag,
#ਜੇ_ਅੰਤ
		tagpp->fname, tagpp->fname_ਸਿਰਾ, st->ਟੈਗ_fname, buf_ffname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	is_static = FALSE;
	ਜੇ (!st->is_etag)	// emacs tags are never static
#ਜੇ_ਅੰਤ
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	//ਮੇਲ
	ਜੇ (is_static)
	{
	    ਜੇ (is_current)
		mtt = MT_ST_CUR;
	    ਨਹੀਂਤਾਂ
		mtt = MT_ST_OTH;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (is_current)
		mtt = MT_GL_CUR;
	    ਨਹੀਂਤਾਂ
		mtt = MT_GL_OTH;
	}
	ਜੇ (st->orgpat->regmatch.rm_ic && !margs->ਮੇਲ_no_ic)
	    mtt += MT_IC_OFF;
	ਜੇ (margs->ਮੇਲ_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ਜੇ (st->help_only)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
# define ML_EXTRA 3
#ਨਹੀਂਤਾਂ
# define ML_EXTRA 0
#ਜੇ_ਅੰਤ
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_ਸਿਰਾ = NUL;
	len = (ਪੂਰਨ)(tagpp->tagname_ਸਿਰਾ - tagpp->tagname);
	mfp = alloc(ਆਕਾਰ(char_u) + len + 10 + ML_EXTRA + 1);
	ਜੇ (mfp != ਨਿਰਾ)
	{
	    ਪੂਰਨ heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#ਜੇ_ਅੰਤ

	    heuristic = help_heuristic(tagpp->tagname,
				margs->ਮੇਲ_re ? margs->matchoff : 0,
				!margs->ਮੇਲ_no_ic);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#ਜੇ_ਅੰਤ
	    sprintf((ਅੱਖਰ *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_ਸਿਰਾ = TAB;
    }
    ਨਹੀਂਤਾਂ ਜੇ (name_only)
    {
	ਜੇ (st->get_searchpat)
	{
	    char_u *ਆਰਜ਼ੀ_ਸਿਰਾ = tagpp->command;

	    ਜੇ (*ਆਰਜ਼ੀ_ਸਿਰਾ == '/')
		ਜਦੋਂ (*ਆਰਜ਼ੀ_ਸਿਰਾ && *ਆਰਜ਼ੀ_ਸਿਰਾ != '\r'
			&& *ਆਰਜ਼ੀ_ਸਿਰਾ != '\n'
			&& *ਆਰਜ਼ੀ_ਸਿਰਾ != '$')
		    ਆਰਜ਼ੀ_ਸਿਰਾ++;

	    ਜੇ (tagpp->command + 2 < ਆਰਜ਼ੀ_ਸਿਰਾ)
	    {
		len = (ਪੂਰਨ)(ਆਰਜ਼ੀ_ਸਿਰਾ - tagpp->command - 2);
		mfp = alloc(len + 2);
		ਜੇ (mfp != ਨਿਰਾ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ਨਹੀਂਤਾਂ
		mfp = ਨਿਰਾ;
	    st->get_searchpat = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    len = (ਪੂਰਨ)(tagpp->tagname_ਸਿਰਾ - tagpp->tagname);
	    mfp = alloc(ਆਕਾਰ(char_u) + len + 1);
	    ਜੇ (mfp != ਨਿਰਾ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
		st->get_searchpat = p_sft;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	size_t ਟੈਗ_fname_len = STRLEN(st->ਟੈਗ_fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	size_t ebuf_len = 0;
#ਜੇ_ਅੰਤ

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (ਪੂਰਨ)ਟੈਗ_fname_len + (ਪੂਰਨ)STRLEN(st->lbuf) + 3;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	ਜੇ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (ਪੂਰਨ)ebuf_len + 1;
	}
	ਨਹੀਂਤਾਂ
	    ++len;
#ਜੇ_ਅੰਤ
	mfp = alloc(ਆਕਾਰ(char_u) + len + 1);
	ਜੇ (mfp != ਨਿਰਾ)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->ਟੈਗ_fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#ਜੇ_ਅੰਤ
	    p[ਟੈਗ_fname_len + 1] = ਟੈਗ_SEP;
	    s = p + 1 + ਟੈਗ_fname_len + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	    ਜੇ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = ਟੈਗ_SEP;
		s += ebuf_len + 1;
	    }
	    ਨਹੀਂਤਾਂ
		*s++ = ਟੈਗ_SEP;
#ਜੇ_ਅੰਤ
	    STRCPY(s, st->lbuf);
	}
    }

    ਜੇ (mfp != ਨਿਰਾ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	ਜੇ (use_cscope)
	    ++*hash;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_ਮੇਲ[mtt], mfp, *hash);
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਜੇ (hash_ਜੋੜੋ_ਵਸਤੂ(&st->ht_ਮੇਲ[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_ਮੇਲ[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		ਵਾਪਸ FAIL;
	    }

	    ((char_u **)(st->ga_ਮੇਲ[mtt].ga_ਅੰਕੜੇ))
		[st->ga_ਮੇਲ[mtt].ga_len++] = mfp;
	    st->ਮੇਲ_ਗਿਣਤੀ++;
	}
	ਨਹੀਂਤਾਂ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    ਵਾਪਸ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ਸਥਿਰ ਖਾਲੀ
findtags_get_all_ਟੈਗ_ਸੂਚੀ(
    findtags_state_T		*st,
    findtags_ਮੇਲ_ਦਲੀਲਾਂ_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	ਖੋਜ_info;
    ਪੂਰਨ			retval;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ			use_cscope = (st->flags & ਟੈਗ_CSCOPE);
#ਜੇ_ਅੰਤ
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_ਖੇਤਰ(ਖੋਜ_info);

    // Read and parse the lines in the file one by one
    ਲਈ (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ਜੇ (st->state == TS_ਦੁਆਧਾਰੀ || st->state == TS_SKIP_ਪਿੱਠ)
	    ਸਤਰ_breakcheck();
	ਨਹੀਂਤਾਂ
	    fast_breakcheck();
	ਜੇ ((st->flags & ਟੈਗ_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ਕੁੰਜੀਆਂ(30, FALSE);
	ਜੇ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ਰੋਕੋ;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ਜੇ (st->mincount == ਟੈਗ_MANY && st->ਮੇਲ_ਗਿਣਤੀ >= ਟੈਗ_MANY)
	{
	    st->stop_searching = TRUE;
	    ਰੋਕੋ;
	}
	ਜੇ (st->get_searchpat)
	    ਜਾਓ ਸਤਰ_read_ਵਿੱਚ;

	retval = findtags_get_ਅਗਲਾ_ਸਤਰ(st, &ਖੋਜ_info);
	ਜੇ (retval == ਟੈਗ_ਸੂਚੀ_READ_IGNORE)
	    ਜਾਰੀ;
	ਜੇ (retval == ਟੈਗ_ਸੂਚੀ_READ_EOF)
	    ਰੋਕੋ;

ਸਤਰ_read_ਵਿੱਚ:

	ਜੇ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ਜੇ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_ਰੇਖਿਕ;
	    emacs_ਟੈਗ_ਸੂਚੀ_new_filename(st);
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ਜੇ (st->state == TS_ਸ਼ੁਰੂ)
	{
	    ਜੇ (findtags_ਸ਼ੁਰੂ_state_handler(st, &margs->sortic, &ਖੋਜ_info) == FALSE)
		ਜਾਰੀ;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ਜੇ (st->lbuf[st->lbuf_ਮਾਪ - 2] != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		&& !use_cscope
#ਜੇ_ਅੰਤ
	   )
	{
	    st->lbuf_ਮਾਪ *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_ਮਾਪ);
	    ਜੇ (st->lbuf == ਨਿਰਾ)
	    {
		ਜੇ (st->fp != ਨਿਰਾ)
		    fclose(st->fp);
		st->fp = ਨਿਰਾ;
		st->stop_searching = TRUE;
		ਵਾਪਸ;
	    }

	    ਜੇ (st->state == TS_ਕਦਮ_FORWARD || st->state == TS_ਰੇਖਿਕ)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, ਖੋਜ_info.curr_ਵਿੱਥ,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    ਖੋਜ_info.curr_ਵਿੱਥ = 0;
	    ਜਾਰੀ;
	}

	retval = findtags_parse_ਸਤਰ(st, &tagp, margs, &ਖੋਜ_info);
	ਜੇ (retval == ਟੈਗ_ਮੇਲ_ਅਗਲਾ)
	    ਜਾਰੀ;
	ਜੇ (retval == ਟੈਗ_ਮੇਲ_STOP)
	    ਰੋਕੋ;
	ਜੇ (retval == ਟੈਗ_ਮੇਲ_FAIL)
	{
	    semsg(_(e_format_error_ਵਿੱਚ_ਟੈਗ_ਸੂਚੀ_file_str), st->ਟੈਗ_fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	    ਜੇ (!use_cscope)
#ਜੇ_ਅੰਤ
		semsg(_("Before byte %ld"), (ਲੰਮਾ)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    ਵਾਪਸ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ਜੇ (findtags_ਮੇਲ_ਟੈਗ(st, &tagp, margs))
	{
	    ਜੇ (findtags_ਜੋੜੋ_ਮੇਲ(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ਰੋਕੋ;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    ਸਥਿਰ ਖਾਲੀ
findtags_ਵਿੱਚ_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_ਮੇਲ_ਦਲੀਲਾਂ_T margs;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ		use_cscope = (st->flags & ਟੈਗ_CSCOPE);
#ਜੇ_ਅੰਤ

    st->vimconv.vc_type = CONV_NONE;
    st->ਟੈਗ_file_sorted = NUL;
    st->fp = ਨਿਰਾ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਜੇ (use_cscope)
	st->fp = ਨਿਰਾ;	    // avoid GCC warning
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	ਜੇ (curbuf->b_help)
	{
	    ਜੇ (!findtags_ਵਿੱਚ_help_init(st))
		ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ

	st->fp = mch_fopen((ਅੱਖਰ *)st->ਟੈਗ_fname, "r");
	ਜੇ (st->fp == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਜੇ (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->ਟੈਗ_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_ਸ਼ੁਰੂ;	// we're at the start of the file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    st->is_etag = FALSE;	// default is: not emacs style
#ਜੇ_ਅੰਤ

    // Read and parse the lines in the file one by one
    findtags_get_all_ਟੈਗ_ਸੂਚੀ(st, &margs, buf_ffname);

    ਜੇ (st->fp != ਨਿਰਾ)
    {
	fclose(st->fp);
	st->fp = ਨਿਰਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    emacs_ਟੈਗ_ਸੂਚੀ_incstack_free();
#ਜੇ_ਅੰਤ
    ਜੇ (st->vimconv.vc_type != CONV_NONE)
	convert_ਤਿਆਰੀ(&st->vimconv, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (margs.ਛਾਂਟੋ_error)
	semsg(_(e_ਟੈਗ_ਸੂਚੀ_file_not_sorted_str), st->ਟੈਗ_fname);

    // Stop searching if sufficient tags have been found.
    ਜੇ (st->ਮੇਲ_ਗਿਣਤੀ >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ਸਥਿਰ ਪੂਰਨ
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    ਪੂਰਨ		name_only = (st->flags & ਟੈਗ_ਨਾਂ);
    char_u	**matches;
    ਪੂਰਨ		mtt;
    ਪੂਰਨ		i;
    char_u	*mfp;
    char_u	*p;

    ਜੇ (st->ਮੇਲ_ਗਿਣਤੀ > 0)
	matches = ALLOC_MULT(char_u *, st->ਮੇਲ_ਗਿਣਤੀ);
    ਨਹੀਂਤਾਂ
	matches = ਨਿਰਾ;
    st->ਮੇਲ_ਗਿਣਤੀ = 0;
    ਲਈ (mtt = 0; mtt < MT_ਗਿਣਤੀ; ++mtt)
    {
	ਲਈ (i = 0; i < st->ga_ਮੇਲ[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_ਮੇਲ[mtt].ga_ਅੰਕੜੇ))[i];
	    ਜੇ (matches == ਨਿਰਾ)
		vim_free(mfp);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    ਲਈ (p = mfp + 1; *p != NUL; ++p)
			ਜੇ (*p == ਟੈਗ_SEP)
			    *p = NUL;
		}
		matches[st->ਮੇਲ_ਗਿਣਤੀ++] = mfp;
	    }
	}

	ga_clear(&st->ga_ਮੇਲ[mtt]);
	hash_clear(&st->ht_ਮੇਲ[mtt]);
    }

    *matchesp = matches;
    ਵਾਪਸ st->ਮੇਲ_ਗਿਣਤੀ;
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
    ਪੂਰਨ
ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(
    char_u	*pat,			// pattern to search for
    ਪੂਰਨ		*ਅੰਕ_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    ਪੂਰਨ		flags,
    ਪੂਰਨ		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    ਪੂਰਨ		ਪਹਿਲਾ_file;		// trying first tag file
    ਪੂਰਨ		retval = FAIL;		// return value
    ਪੂਰਨ		ਗੇੜਾ;

    ਪੂਰਨ		save_emsg_off;

    bool	help_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਪੂਰਨ		i;
    char_u	*saved_pat = ਨਿਰਾ;		// copy of pat[]
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		findall = (mincount == MAXCOL || mincount == ਟੈਗ_MANY);
						// find all matching tags
    ਪੂਰਨ		has_re = (flags & ਟੈਗ_REGEXP);	// regexp used
    ਪੂਰਨ		noic = (flags & ਟੈਗ_NOIC);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਪੂਰਨ		use_cscope = (flags & ਟੈਗ_CSCOPE);
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		verbose = (flags & ਟੈਗ_VERBOSE);
    ਪੂਰਨ		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    ਚੋਣ (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	ਹਾਲਤ TC_FOLLOWIC:		 ਰੋਕੋ;
	ਹਾਲਤ TC_IGNORE:    p_ic = TRUE;  ਰੋਕੋ;
	ਹਾਲਤ TC_ਮੇਲ:     p_ic = FALSE; ਰੋਕੋ;
	ਹਾਲਤ TC_FOLLOWSCS: p_ic = ignorecase(pat); ਰੋਕੋ;
	ਹਾਲਤ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ਰੋਕੋ;
    }

    help_save = curbuf->b_help;

    ਜੇ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	ਜਾਓ findtag_ਸਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    STRCPY(st.ਟੈਗ_fname, "from cscope");	// for error messages
#ਜੇ_ਅੰਤ

    /*
     * Initialize a few variables
     */
    ਜੇ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
    ਨਹੀਂਤਾਂ ਜੇ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਜੇ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ਜੇ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ਜੇ (saved_pat != ਨਿਰਾ)
	    {
		st.help_lang_ਲੱਭੋ = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#ਜੇ_ਅੰਤ
    ਜੇ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ਜੇ (has_re && st.orgpat->regmatch.regprog == ਨਿਰਾ)
	ਜਾਓ findtag_ਸਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ਜੇ (retval != NOTDONE)
	ਜਾਓ findtag_ਸਿਰਾ;

    // re-initialize the default return value
    retval = FAIL;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ਜੇ ((flags & ਟੈਗ_KEEP_LANG)
	    && st.help_lang_ਲੱਭੋ == ਨਿਰਾ
	    && curbuf->b_fname != ਨਿਰਾ
	    && (i = (ਪੂਰਨ)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#ਜੇ_ਅੰਤ

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
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	st.ਰੇਖਿਕ = (st.orgpat->headlen == 0 || !p_tbs || ਗੇੜਾ == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      ਲਈ (ਪਹਿਲਾ_file = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	    use_cscope ||
#ਜੇ_ਅੰਤ
		get_tagfname(&tn, ਪਹਿਲਾ_file, st.ਟੈਗ_fname) == OK;
							   ਪਹਿਲਾ_file = FALSE)
      {
	  findtags_ਵਿੱਚ_file(&st, buf_ffname);
	  ਜੇ (st.stop_searching
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		  || use_cscope
#ਜੇ_ਅੰਤ
	     )
	  {
	      retval = OK;
	      ਰੋਕੋ;
	  }
      } // end of for-each-file loop

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	ਜੇ (!use_cscope)
#ਜੇ_ਅੰਤ
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ਜੇ (st.stop_searching || st.ਰੇਖਿਕ || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
	ਜੇ (use_cscope)
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ਜੇ (!st.stop_searching)
    {
	ਜੇ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_ਟੈਗ_ਸੂਚੀ_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_ਸਿਰਾ:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ਜੇ (retval == FAIL)
	st.ਮੇਲ_ਗਿਣਤੀ = 0;

    *ਅੰਕ_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    vim_free(saved_pat);
#ਜੇ_ਅੰਤ

    p_ic = save_p_ic;

    ਵਾਪਸ retval;
}

ਸਥਿਰ garray_T ਟੈਗ_fnames = GA_ਖਾਲੀ_ਥਾਵਾਂ;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ਸਥਿਰ ਖਾਲੀ
ਮਿਲਿਆ_tagfile_cb(char_u *fname, ਖਾਲੀ *cookie ਅਣਵਰਤਿਆ)
{
    ਜੇ (ga_grow(&ਟੈਗ_fnames, 1) == FAIL)
	ਵਾਪਸ;

    char_u	*ਟੈਗ_fname = vim_strsave(fname);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    slash_adjust(ਟੈਗ_fname);
#ਜੇ_ਅੰਤ
    simplify_filename(ਟੈਗ_fname);
    ((char_u **)(ਟੈਗ_fnames.ga_ਅੰਕੜੇ))[ਟੈਗ_fnames.ga_len++] = ਟੈਗ_fname;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_ਟੈਗ_stuff(ਖਾਲੀ)
{
    ga_clear_ਲੜੀਆਂ(&ਟੈਗ_fnames);
    ਜੇ (curwin != ਨਿਰਾ)
	do_ਟੈਗ(ਨਿਰਾ, DT_FREE, 0, 0, 0);
    ਟੈਗ_freematch();

# ਜੇ defined(FEAT_QUICKFIX)
    tagstack_clear_ਇੰਦਰਾਜ(&ptag_ਇੰਦਰਾਜ);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    ਪੂਰਨ
get_tagfname(
    tagname_T	*tnp,	// holds status info
    ਪੂਰਨ		ਪਹਿਲਾ,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ਨਿਰਾ;
    char_u		*r_ptr;
    ਪੂਰਨ			i;

    ਜੇ (ਪਹਿਲਾ)
	CLEAR_ਸੂਚਕ(tnp);

    ਜੇ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ਜੇ (ਪਹਿਲਾ)
	{
	    ga_clear_ਲੜੀਆਂ(&ਟੈਗ_fnames);
	    ga_init2(&ਟੈਗ_fnames, ਆਕਾਰ(char_u *), 10);
	    do_ਵਿੱਚ_runtimepath((char_u *)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ਨਹੀਂਤਾਂ
		    "doc/tags doc/tags-??"
# endif
#ਨਹੀਂਤਾਂ
		    "doc/tags"
#ਜੇ_ਅੰਤ
					   , DIP_ALL, ਮਿਲਿਆ_tagfile_cb, ਨਿਰਾ);
	}

	ਜੇ (tnp->tn_hf_idx >= ਟੈਗ_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ਜੇ (tnp->tn_hf_idx > ਟੈਗ_fnames.ga_len || *p_hf == NUL)
		ਵਾਪਸ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    slash_adjust(buf);
#ਜੇ_ਅੰਤ
	    simplify_filename(buf);

	    ਲਈ (i = 0; i < ਟੈਗ_fnames.ga_len; ++i)
		ਜੇ (STRCMP(buf, ((char_u **)(ਟੈਗ_fnames.ga_ਅੰਕੜੇ))[i]) == 0)
		    ਵਾਪਸ FAIL; // avoid duplicate file names
	}
	ਨਹੀਂਤਾਂ
	    vim_strncpy(buf, ((char_u **)(ਟੈਗ_fnames.ga_ਅੰਕੜੇ))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	ਵਾਪਸ OK;
    }

    ਜੇ (ਪਹਿਲਾ)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_ਟੈਗ_ਸੂਚੀ = vim_strsave((*curbuf->b_p_ਟੈਗ_ਸੂਚੀ != NUL)
						 ? curbuf->b_p_ਟੈਗ_ਸੂਚੀ : p_ਟੈਗ_ਸੂਚੀ);
	ਜੇ (tnp->tn_ਟੈਗ_ਸੂਚੀ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	tnp->tn_np = tnp->tn_ਟੈਗ_ਸੂਚੀ;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    ਲਈ (;;)
    {
	ਜੇ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_ਖੋਜ_ctx);
	    ਜੇ (fname != ਨਿਰਾ)
		ਰੋਕੋ;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  *filename = ਨਿਰਾ;

	    // Stop when used all parts of 'tags'.
	    ਜੇ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_ਖੋਜ_ctx);
		tnp->tn_ਖੋਜ_ctx = ਨਿਰਾ;
		ਵਾਪਸ FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (ਖਾਲੀ)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    ਜੇ (r_ptr != ਨਿਰਾ)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_ਖੋਜ_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_ਖੋਜ_ctx, TRUE, curbuf->b_ffname);
	    ਜੇ (tnp->tn_ਖੋਜ_ctx != ਨਿਰਾ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    ਵਾਪਸ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    ਖਾਲੀ
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_ਟੈਗ_ਸੂਚੀ);
    vim_findfile_cleanup(tnp->tn_ਖੋਜ_ctx);
    tnp->tn_ਖੋਜ_ctx = ਨਿਰਾ;
    ga_clear_ਲੜੀਆਂ(&ਟੈਗ_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_ਟੈਗ_ਸਤਰ(
    char_u	*lbuf,		// line to be parsed
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਪੂਰਨ		is_etag,
#ਜੇ_ਅੰਤ
    tagptrs_T	*tagp)
{
    char_u	*p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (is_etag)
	// emacs-style tag file
	ਵਾਪਸ emacs_ਟੈਗ_ਸੂਚੀ_parse_ਸਤਰ(lbuf, tagp);
#ਜੇ_ਅੰਤ

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    tagp->tagname_ਸਿਰਾ = p;

    // Isolate file name, from first to second white space
    ਜੇ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    tagp->fname_ਸਿਰਾ = p;

    // find start of search command, after second white space
    ਜੇ (*p != NUL)
	++p;
    ਜੇ (*p == NUL)
	ਵਾਪਸ FAIL;
    tagp->command = p;

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ਜਦੋਂ ((p = vim_strchr(p, '\t')) != ਨਿਰਾ)
    {
	++p;
	ਜੇ (STRNCMP(p, "file:", 5) == 0)
	    ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    ਸਥਿਰ size_t
matching_ਸਤਰ_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    p += STRLEN(p) + 1;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (p - lbuf) + STRLEN(p);
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
    ਸਥਿਰ ਪੂਰਨ
parse_ਮੇਲ(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    ਪੂਰਨ		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->ਟੈਗ_fname = lbuf + 1;
    lbuf += STRLEN(tagp->ਟੈਗ_fname) + 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_ਸਿਰਾ = lbuf++;
    }
    ਨਹੀਂਤਾਂ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#ਜੇ_ਅੰਤ

    // Find search pattern and the file name for non-etags.
    retval = parse_ਟੈਗ_ਸਤਰ(lbuf,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
			tagp->is_etag,
#ਜੇ_ਅੰਤ
			tagp);

    tagp->tagkind = ਨਿਰਾ;
    tagp->user_ਅੰਕੜੇ = ਨਿਰਾ;
    tagp->tagline = 0;
    tagp->command_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ (retval != OK)
	ਵਾਪਸ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ਜੇ (ਲੱਭੋ_extra(&p) == OK)
    {
	ਜੇ (p > tagp->command && p[-1] == '|')
	    tagp->command_ਸਿਰਾ = p - 1;  // drop trailing bar
	ਨਹੀਂਤਾਂ
	    tagp->command_ਸਿਰਾ = p;
	p += 2;	// skip ";\""
	ਜੇ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ਜਦੋਂ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ਜੇ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_ਅੰਕੜੇ = p + 10;
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((ਅੱਖਰ *)p + 5);
		ਜੇ (tagp->tagkind != ਨਿਰਾ && tagp->user_ਅੰਕੜੇ != ਨਿਰਾ)
		    ਰੋਕੋ;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ਜੇ (pc == ਨਿਰਾ || (pt != ਨਿਰਾ && pc > pt))
		    tagp->tagkind = p;
		ਜੇ (pt == ਨਿਰਾ)
		    ਰੋਕੋ;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ਜੇ (tagp->tagkind != ਨਿਰਾ)
    {
	ਲਈ (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_ਸਿਰਾ = p;
    }
    ਜੇ (tagp->user_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	ਲਈ (p = tagp->user_ਅੰਕੜੇ;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_ਅੰਕੜੇ_ਸਿਰਾ = p;
    }
    ਵਾਪਸ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ਸਥਿਰ char_u *
ਟੈਗ_ਭਰਿਆ_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    ਪੂਰਨ		c;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	c = *tagp->fname_ਸਿਰਾ;
	*tagp->fname_ਸਿਰਾ = NUL;
    }
    fullname = expand_ਟੈਗ_fname(tagp->fname, tagp->ਟੈਗ_fname, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (!tagp->is_etag)
#ਜੇ_ਅੰਤ
	*tagp->fname_ਸਿਰਾ = c;

    ਵਾਪਸ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
jumpto_ਟੈਗ(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    ਪੂਰਨ		forceit,	// :ta with !
    ਪੂਰਨ		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    ਪੂਰਨ		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_ਸਿਰਾ;
    char_u	*tofree_fname = ਨਿਰਾ;
    char_u	*fname;
    tagptrs_T	tagp;
    ਪੂਰਨ		retval = FAIL;
    ਪੂਰਨ		getfile_ਨਤੀਜਾ = GETFILE_ਅਣਵਰਤਿਆ;
    ਪੂਰਨ		ਖੋਜ_options;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    ਪੂਰਨ		save_no_hlsearch;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    char_u	*ਭਰਿਆ_fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਪੂਰਨ		old_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;    // getting the file may reset it
#ਜੇ_ਅੰਤ
    size_t	len;
    char_u	*lbuf;
    ਪੂਰਨ		ਅੰਕ_ਹੈ = FALSE;

    ਜੇ (postponed_ਵੰਡੋ == 0 && !check_can_ਸੈੱਟ_curbuf_forceit(forceit))
	ਵਾਪਸ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ਸਤਰ_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ਜੇ (lbuf != ਨਿਰਾ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ਜੇ (pbuf == ਨਿਰਾ || lbuf == ਨਿਰਾ || parse_ਮੇਲ(lbuf, &tagp) == FAIL)
    {
	tagp.fname_ਸਿਰਾ = ਨਿਰਾ;
	ਜਾਓ erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_ਸਿਰਾ = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ਜੇ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	ਅੰਕ_ਹੈ = TRUE;
	pbuf[0] = ':';
    }
    ਲਈ (pbuf_ਸਿਰਾ = pbuf + ਅੰਕ_ਹੈ;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	ਜੇ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	*pbuf_ਸਿਰਾ++ = *str++;
	ਜੇ (pbuf_ਸਿਰਾ - pbuf + 1 + ਅੰਕ_ਹੈ >= LSIZE)
	    ਰੋਕੋ;
    }
    *pbuf_ਸਿਰਾ = NUL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਜੇ (!tagp.is_etag)
#ਜੇ_ਅੰਤ
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ਜੇ (ਅੰਕ_ਹੈ)
	    str++;
	ਜੇ (ਲੱਭੋ_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ਜੇ (str > pbuf && str[-1] == '|')
		pbuf_ਸਿਰਾ = str - 1;
	    ਨਹੀਂਤਾਂ
		pbuf_ਸਿਰਾ = str;
	    *pbuf_ਸਿਰਾ = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_ਟੈਗ_fname(fname, tagp.ਟੈਗ_fname, TRUE);
    ਜੇ (fname == ਨਿਰਾ)
	ਜਾਓ erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ਜੇ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ਨਿਰਾ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ਜੇ (nofile_fname == ਨਿਰਾ)
	    nofile_fname = ਖਾਲੀ_ਥਾਵਾਂ_option;
	ਜਾਓ erret;
    }

    ++RedrawingDisabled;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_QUICKFIX)
    ਜੇ (g_do_tagpreview != 0)
    {
	postponed_ਵੰਡੋ = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ਜੇ (!curwin->w_p_pvw)
	{
	    ਭਰਿਆ_fname = ਭਰਿਆ_Name_save(fname, FALSE);
	    fname = ਭਰਿਆ_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    ਜੇ (postponed_ਵੰਡੋ && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ਜੇ (existing_buf != ਨਿਰਾ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ਜੇ (swbuf_goto_win_with_buf(existing_buf) != ਨਿਰਾ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_ਨਤੀਜਾ = GETFILE_SAME_FILE;
	}
    }
    ਜੇ (getfile_ਨਤੀਜਾ == GETFILE_ਅਣਵਰਤਿਆ
				  && (postponed_ਵੰਡੋ || cmdmod.cmod_tab != 0))
    {
	ਜੇ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_ਵੰਡੋ |= WSP_VERT;

	ਜੇ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_ਸੂਚਕਾਂਕ(curtab) + 1;

	ਜੇ (win_ਵੰਡੋ(postponed_ਵੰਡੋ > 0 ? postponed_ਵੰਡੋ : 0,
						postponed_ਵੰਡੋ_flags) == FAIL)
	{
	    ਜੇ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    ਜਾਓ erret;
	}
	RESET_BINDING(curwin);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ਜੇ defined(FEAT_QUICKFIX)
	ਜੇ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_ਬਫਰ);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    keep_help_flag = curbuf->b_help;
    }

    ਜੇ (getfile_ਨਤੀਜਾ == GETFILE_ਅਣਵਰਤਿਆ)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_ਨਤੀਜਾ = getfile(0, fname, ਨਿਰਾ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ਜੇ (GETFILE_SUCCESS(getfile_ਨਤੀਜਾ))	// got to the right file
    {
	curwin->w_ਸੈੱਟ_curswant = true;
	postponed_ਵੰਡੋ = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ਜੇ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#ਜੇ_ਅੰਤ

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ਜੇ (vim_strchr(p_cpo, CPO_TAGPAT) != ਨਿਰਾ)
	    ਖੋਜ_options = 0;
	ਨਹੀਂਤਾਂ
	    ਖੋਜ_options = ਖੋਜ_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	ਜੇ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ਜੇ (str > pbuf_ਸਿਰਾ - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_ਸਿਰਾ - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ਜੇ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ਨਹੀਂਤਾਂ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ਜੇ (do_ਖੋਜ(ਨਿਰਾ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ਲੰਮਾ)1,
							 ਖੋਜ_options, ਨਿਰਾ))
		retval = OK;
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ	ਮਿਲਿਆ = 1;
		ਪੂਰਨ	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ਜੇ (!do_ਖੋਜ(ਨਿਰਾ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ਲੰਮਾ)1,
							 ਖੋਜ_options, ਨਿਰਾ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    ਮਿਲਿਆ = 2;
		    (ਖਾਲੀ)test_for_static(&tagp);
		    cc = *tagp.tagname_ਸਿਰਾ;
		    *tagp.tagname_ਸਿਰਾ = NUL;
		    pbuflen = vim_snprintf((ਅੱਖਰ *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ਜੇ (!do_ਖੋਜ(ਨਿਰਾ, '/', '/', pbuf, pbuflen, (ਲੰਮਾ)1,
							 ਖੋਜ_options, ਨਿਰਾ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((ਅੱਖਰ *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ਜੇ (!do_ਖੋਜ(ਨਿਰਾ, '/', '/', pbuf, pbuflen, (ਲੰਮਾ)1,
							 ਖੋਜ_options, ਨਿਰਾ))
			    ਮਿਲਿਆ = 0;
		    }
		    *tagp.tagname_ਸਿਰਾ = cc;
		}
		ਜੇ (ਮਿਲਿਆ == 0)
		{
		    emsg(_(e_cannot_ਲੱਭੋ_ਟੈਗ_ਨਮੂਨਾ));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ਜੇ (ਮਿਲਿਆ == 2 || !save_p_ic)
		    {
			ਸੁਨੇਹਾ(_(e_couldnt_ਲੱਭੋ_ਟੈਗ_just_guessing));
			ਜੇ (!ਸੁਨੇਹਾ_scrolled && ਸੁਨੇਹਾ_silent == 0)
			{
			    ਬਾਹਰਲਾ_flush();
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
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	    ++sandbox;
#ਜੇ_ਅੰਤ
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    ਜੇ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	    --sandbox;
#ਜੇ_ਅੰਤ
	}

	magic_overruled = save_magic_overruled;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ਜੇ (ਖੋਜ_options)
	    ਸੈੱਟ_no_hlsearch(save_no_hlsearch);
#ਜੇ_ਅੰਤ

	// Return OK if jumped to another file (at least we found the file!).
	ਜੇ (getfile_ਨਤੀਜਾ == GETFILE_OPEN_OTHER)
	    retval = OK;

	ਜੇ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ਜੇ (curbuf->b_help)
		ਸੈੱਟ_topline(curwin, curwin->w_cursor.lnum);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ ((fdo_flags & FDO_ਟੈਗ) && old_ਕੁੰਜੀ_Typed)
		foldOpenCursor();
#ਜੇ_ਅੰਤ
	}

#ਜੇ defined(FEAT_QUICKFIX)
	ਜੇ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#ਜੇ_ਅੰਤ

	ਜੇ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ਜੇ (postponed_ਵੰਡੋ)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_ਵੰਡੋ = 0;
	}
#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ਜੇ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#ਜੇ_ਅੰਤ
    }
#ਜੇ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ਜੇ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#ਜੇ_ਅੰਤ

erret:
#ਜੇ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#ਜੇ_ਅੰਤ
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(ਭਰਿਆ_fname);

    ਵਾਪਸ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ਸਥਿਰ char_u *
expand_ਟੈਗ_fname(char_u *fname, char_u *ਟੈਗ_fname, ਪੂਰਨ expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ਨਿਰਾ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ਜੇ (p_tagsecure && ਰਸਤਾ_with_url(fname))
    {
       emsg(_(e_ਟੈਗ_file_ਇੰਦਰਾਜ_must_not_be_url));
       ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ਜੇ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ਨਿਰਾ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ਨਿਰਾ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ਜੇ (expanded_fname != ਨਿਰਾ)
	    fname = expanded_fname;
    }

    ਜੇ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(ਟੈਗ_fname)) != ਟੈਗ_fname)
    {
	retval = alloc(MAXPATHL);
	ਜੇ (retval != ਨਿਰਾ)
	{
	    STRCPY(retval, ਟੈਗ_fname);
	    vim_strncpy(retval + (p - ਟੈਗ_fname), fname,
					      MAXPATHL - (p - ਟੈਗ_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    ਨਹੀਂਤਾਂ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    ਵਾਪਸ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ਸਥਿਰ ਪੂਰਨ
test_for_current(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
    ਪੂਰਨ	    is_etag,
#ਜੇ_ਅੰਤ
    char_u  *fname,
    char_u  *fname_ਸਿਰਾ,
    char_u  *ਟੈਗ_fname,
    char_u  *buf_ffname)
{
    ਪੂਰਨ	    c;
    ਪੂਰਨ	    retval = FALSE;
    char_u  *fullname;

    ਜੇ (buf_ffname != ਨਿਰਾ)	// if the buffer has a name
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	ਜੇ (is_etag)
	    c = 0;	    // to shut up GCC
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    c = *fname_ਸਿਰਾ;
	    *fname_ਸਿਰਾ = NUL;
	}
	fullname = expand_ਟੈਗ_fname(fname, ਟੈਗ_fname, TRUE);
	ਜੇ (fullname != ਨਿਰਾ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EMACS_ਟੈਗ_ਸੂਚੀ
	ਜੇ (!is_etag)
#ਜੇ_ਅੰਤ
	    *fname_ਸਿਰਾ = c;
    }

    ਵਾਪਸ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	ਪਹਿਲਾ_char = **pp;

    // Repeat for addresses separated with ';'
    ਲਈ (;;)
    {
	ਜੇ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ਨਹੀਂਤਾਂ ਜੇ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ਜੇ (*str != ਪਹਿਲਾ_char)
		str = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		++str;
	}
	ਨਹੀਂਤਾਂ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)str, "|;\"");
	    ਜੇ (str != ਨਿਰਾ)
	    {
		++str;
		ਰੋਕੋ;
	    }

	}
	ਜੇ (str == ਨਿਰਾ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ਰੋਕੋ;
	++str;	// skip ';'
	ਪਹਿਲਾ_char = *str;
    }

    ਜੇ (str != ਨਿਰਾ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    ਖਾਲੀ
tagstack_clear_ਇੰਦਰਾਜ(taggy_T *ਵਸਤੂ)
{
    VIM_CLEAR(ਵਸਤੂ->tagname);
    VIM_CLEAR(ਵਸਤੂ->user_ਅੰਕੜੇ);
}

    ਪੂਰਨ
expand_ਟੈਗ_ਸੂਚੀ(
    ਪੂਰਨ		tagnames,	// expand tag names
    char_u	*pat,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_ਮਾਪ = 100;
    tagptrs_T	t_p;
    ਪੂਰਨ		ret;

    name_buf = alloc(name_buf_ਮਾਪ);
    ਜੇ (name_buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (tagnames)
	extra_flag = ਟੈਗ_ਨਾਂ;
    ਨਹੀਂਤਾਂ
	extra_flag = 0;
    ਜੇ (pat[0] == '/')
	ret = ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(pat + 1, ਅੰਕ_file, file,
		ਟੈਗ_REGEXP | extra_flag | ਟੈਗ_VERBOSE,
		ਟੈਗ_MANY, curbuf->b_ffname);
    ਨਹੀਂਤਾਂ
	ret = ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(pat, ਅੰਕ_file, file,
	      ਟੈਗ_REGEXP | extra_flag | ਟੈਗ_VERBOSE | ਟੈਗ_NOIC,
		ਟੈਗ_MANY, curbuf->b_ffname);
    ਜੇ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 ਲਈ (i = 0; i < *ਅੰਕ_file; i++)
	 {
	     size_t	len;

	     parse_ਮੇਲ((*file)[i], &t_p);
	     len = t_p.tagname_ਸਿਰਾ - t_p.tagname;
	     ਜੇ (len > name_buf_ਮਾਪ - 3)
	     {
		 char_u *buf;

		 name_buf_ਮਾਪ = len + 3;
		 buf = vim_realloc(name_buf, name_buf_ਮਾਪ);
		 ਜੇ (buf == ਨਿਰਾ)
		 {
		     vim_free(name_buf);
		     ਵਾਪਸ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ਨਿਰਾ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_ਸਿਰਾ - t_p.fname);
	     (*file)[i][len + (t_p.fname_ਸਿਰਾ - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    ਵਾਪਸ ret;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(
    dict_T  *dict,
    ਅੱਖਰ    *ਖੇਤਰ_name,
    char_u  *ਸ਼ੁਰੂ,		// start of the value
    char_u  *ਸਿਰਾ)		// after the value; can be NULL
{
    char_u	*buf;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		retval;

    // check that the field name doesn't exist yet
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, ਖੇਤਰ_name))
    {
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), ਖੇਤਰ_name);
	    verbose_leave();
	}
	ਵਾਪਸ FAIL;
    }
    buf = alloc(MAXPATHL);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
	{
	    ਸਿਰਾ = ਸ਼ੁਰੂ + STRLEN(ਸ਼ੁਰੂ);
	    ਜਦੋਂ (ਸਿਰਾ > ਸ਼ੁਰੂ && (ਸਿਰਾ[-1] == '\r' || ਸਿਰਾ[-1] == '\n'))
		--ਸਿਰਾ;
	}
	len = (ਪੂਰਨ)(ਸਿਰਾ - ਸ਼ੁਰੂ);
	ਜੇ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, ਸ਼ੁਰੂ, len);
    }
    buf[len] = NUL;
    retval = dict_ਜੋੜੋ_string(dict, ਖੇਤਰ_name, buf);
    vim_free(buf);
    ਵਾਪਸ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    ਪੂਰਨ
get_ਟੈਗ_ਸੂਚੀ(list_T *list, char_u *pat, char_u *buf_fname)
{
    ਪੂਰਨ		ਅੰਕ_matches, i, ret;
    char_u	**matches, *p;
    char_u	*ਭਰਿਆ_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    ਲੰਮਾ	is_static;

    ret = ਲੱਭੋ_ਟੈਗ_ਸੂਚੀ(pat, &ਅੰਕ_matches, &matches,
				ਟੈਗ_REGEXP | ਟੈਗ_NOIC, (ਪੂਰਨ)MAXCOL, buf_fname);
    ਜੇ (ret != OK || ਅੰਕ_matches <= 0)
	ਵਾਪਸ ret;

    ਲਈ (i = 0; i < ਅੰਕ_matches; ++i)
    {
	ਜੇ (parse_ਮੇਲ(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ਜਾਰੀ;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ਜੇ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ਜਾਰੀ;
	}

	ਜੇ ((dict = dict_alloc()) == ਨਿਰਾ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ਰੋਕੋ;
	}
	ਜੇ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ਜਾਰੀ;
	}

	ਭਰਿਆ_fname = ਟੈਗ_ਭਰਿਆ_fname(&tp);
	ਜੇ (ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(dict, "name", tp.tagname, tp.tagname_ਸਿਰਾ) == FAIL
		|| ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(dict, "filename", ਭਰਿਆ_fname,
		    ਨਿਰਾ) == FAIL
		|| ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(dict, "cmd", tp.command,
		    tp.command_ਸਿਰਾ) == FAIL
		|| ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(dict, "kind", tp.tagkind,
		    tp.tagkind_ਸਿਰਾ) == FAIL
		|| dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(ਭਰਿਆ_fname);

	ਜੇ (tp.command_ਸਿਰਾ != ਨਿਰਾ)
	{
	    ਲਈ (p = tp.command_ਸਿਰਾ + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ਜੇ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_ਸਿਰਾ - 1;
		ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ਨਹੀਂਤਾਂ ਜੇ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    ਪੂਰਨ	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ਜਦੋਂ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (ਪੂਰਨ)(p - n);
		    ਜੇ (*p == ':' && len > 0)
		    {
			s = ++p;
			ਜਦੋਂ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ਜੇ (ਜੋੜੋ_ਟੈਗ_ਖੇਤਰ(dict, (ਅੱਖਰ *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ਨਹੀਂਤਾਂ
			// Skip field without colon.
			ਜਦੋਂ (*p != NUL && *p >= ' ')
			    ++p;
		    ਜੇ (*p == NUL)
			ਰੋਕੋ;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    ਵਾਪਸ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਟੈਗ_details(taggy_T *ਟੈਗ, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_ਜੋੜੋ_string(retdict, "tagname", ਟੈਗ->tagname);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "matchnr", ਟੈਗ->cur_ਮੇਲ + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "bufnr", ਟੈਗ->cur_fnum);
    ਜੇ (ਟੈਗ->user_ਅੰਕੜੇ)
	dict_ਜੋੜੋ_string(retdict, "user_data", ਟੈਗ->user_ਅੰਕੜੇ);

    ਜੇ ((pos = list_alloc_id(aid_tagstack_from)) == ਨਿਰਾ)
	ਵਾਪਸ;
    dict_ਜੋੜੋ_list(retdict, "from", pos);

    fmark = &ਟੈਗ->fmark;
    list_append_ਸੰਖਿਆ(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_ਸੰਖਿਆ(pos, (varnumber_T)fmark->mark.lnum);
    list_append_ਸੰਖਿਆ(pos, (varnumber_T)(fmark->mark.ਖਾਨਾ == MAXCOL ?
					MAXCOL : fmark->mark.ਖਾਨਾ + 1));
    list_append_ਸੰਖਿਆ(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    ਖਾਲੀ
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    ਪੂਰਨ		i;
    dict_T	*d;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "length", wp->w_tagstacklen);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_ਚੀਜ਼ਾਂ);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;
    dict_ਜੋੜੋ_list(retdict, "items", l);

    ਲਈ (i = 0; i < wp->w_tagstacklen; i++)
    {
	ਜੇ ((d = dict_alloc_id(aid_tagstack_details)) == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ਵਾਪਸ;
	}

	get_ਟੈਗ_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ਸਥਿਰ ਖਾਲੀ
tagstack_clear(win_T *wp)
{
    ਪੂਰਨ i;

    // Free the current tag stack
    ਲਈ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_ਇੰਦਰਾਜ(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ਸਥਿਰ ਖਾਲੀ
tagstack_ਖਿਸਕਾਓ(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ਪੂਰਨ		i;

    tagstack_clear_ਇੰਦਰਾਜ(&tagstack[0]);
    ਲਈ (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ਸਥਿਰ ਖਾਲੀ
tagstack_ਧੱਕੋ_ਵਸਤੂ(
	win_T	*wp,
	char_u	*tagname,
	ਪੂਰਨ	cur_fnum,
	ਪੂਰਨ	cur_ਮੇਲ,
	pos_T	mark,
	ਪੂਰਨ	fnum,
	char_u  *user_ਅੰਕੜੇ)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ਪੂਰਨ		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ਜੇ (idx >= TAGSTACKSIZE)
    {
	tagstack_ਖਿਸਕਾਓ(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_ਮੇਲ = cur_ਮੇਲ;
    ਜੇ (tagstack[idx].cur_ਮੇਲ < 0)
	tagstack[idx].cur_ਮੇਲ = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_ਅੰਕੜੇ = user_ਅੰਕੜੇ;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    ਸਥਿਰ ਖਾਲੀ
tagstack_ਧੱਕੋ_ਚੀਜ਼ਾਂ(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    ਪੂਰਨ		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ਨਿਰਾ)
	    ਜਾਰੀ;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ਜੇ ((di = dict_ਲੱਭੋ(itemdict, (char_u *)"from", -1)) == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ (list2fpos(&di->di_tv, &mark, &fnum, ਨਿਰਾ, FALSE) != OK)
	    ਜਾਰੀ;
	ਜੇ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (mark.ਖਾਨਾ > 0)
	    mark.ਖਾਨਾ--;
	tagstack_ਧੱਕੋ_ਵਸਤੂ(wp, tagname,
		(ਪੂਰਨ)dict_get_ਸੰਖਿਆ(itemdict, "bufnr"),
		(ਪੂਰਨ)dict_get_ਸੰਖਿਆ(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ਸਥਿਰ ਖਾਲੀ
tagstack_ਸੈੱਟ_curidx(win_T *wp, ਪੂਰਨ curidx)
{
    wp->w_tagstackidx = curidx;
    ਜੇ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ਜੇ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    ਪੂਰਨ
ਸੈੱਟ_tagstack(win_T *wp, dict_T *d, ਪੂਰਨ action)
{
    dictitem_T	*di;
    list_T	*l = ਨਿਰਾ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ਜੇ (tfu_ਵਿੱਚ_use)
    {
	emsg(_(e_cannot_modify_ਟੈਗ_ਢੇਰ_within_tagfunc));
	ਵਾਪਸ FAIL;
    }
# endif

    ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"items", -1)) != ਨਿਰਾ)
    {
	ਜੇ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ਵਾਪਸ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"curidx", -1)) != ਨਿਰਾ)
	tagstack_ਸੈੱਟ_curidx(wp, (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&di->di_tv) - 1);

    ਜੇ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	ਪੂਰਨ	tagstackidx = wp->w_tagstackidx;
	ਪੂਰਨ	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ਜਦੋਂ (tagstackidx < tagstacklen)
	    tagstack_clear_ਇੰਦਰਾਜ(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ਜੇ (l != ਨਿਰਾ)
    {
	ਜੇ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ਧੱਕੋ_ਚੀਜ਼ਾਂ(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ
