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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಟ್ಯಾಗ್_ಸೂಚಕಗಳು
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_ಕೊನೆ;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_ಕೊನೆ;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_ಕೊನೆ;	// first char after command
    char_u	*ಟ್ಯಾಗ್_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಪೂರ್ಣಾಂಕ		is_etag;	// TRUE for emacs tag
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_ಕೊನೆ;	// end of tagkind
    char_u	*user_ದತ್ತಾಂಶ;	// user_data string
    char_u	*user_ದತ್ತಾಂಶ_ಕೊನೆ;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಟ್ಯಾಗುಗಳು_READ_SUCCESS = 1,
    ಟ್ಯಾಗುಗಳು_READ_EOF,
    ಟ್ಯಾಗುಗಳು_READ_IGNORE,
} ಟ್ಯಾಗುಗಳು_read_ಸ್ಥಿತಿ_T;

/*
 * States used during a tags search
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    TS_ಆರಂಭ,		// at start of file
    TS_ರೇಖೀಯ,		// linear searching forward, till EOF
    TS_ದ್ವಿಮಾನ,		// binary searching
    TS_SKIP_ಹಿಂಭಾಗ,	// skipping backwards
    TS_ಹೆಜ್ಜೆ_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    off_T	ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ;	// offset for first char of first line that
				// could match
    off_T	ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ;	// offset of char after last line that could
				//ಹೊಂದಿಕೆ
    off_T	curr_ಸ್ಥಾನಾಂತರ;	// Current file offset in search range
    off_T	curr_ಸ್ಥಾನಾಂತರ_ಬಳಸಿದ; // curr_offset used when skipping back
    off_T	ಹೊಂದಿಕೆ_ಸ್ಥಾನಾಂತರ;	// Where the binary search found a tag
    ಪೂರ್ಣಾಂಕ	ಕೆಳಗಿನ_char;		// first char at low_offset
    ಪೂರ್ಣಾಂಕ	ಮೇಲಿನ_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_SUCCESS = 1,
    ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_FAIL,
    ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_STOP,
    ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ
} tagmatch_ಸ್ಥಿತಿ_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	matchoff;		// tag match offset
    ಪೂರ್ಣಾಂಕ	ಹೊಂದಿಕೆ_re;		// TRUE if the tag matches a regexp
    ಪೂರ್ಣಾಂಕ	ಹೊಂದಿಕೆ_no_ic;		// TRUE if the tag matches with case
    ಪೂರ್ಣಾಂಕ	has_re;			// regular expression used
    ಪೂರ್ಣಾಂಕ	sortic;			// tags file sorted ignoring case (foldcase)
    ಪೂರ್ಣಾಂಕ	ವಿಂಗಡಿಸು_error;		// tags file not sorted
} findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MT_ST_CUR	0		// static match in current file
#ವ್ಯಾಖ್ಯಾನಿಸು MT_GL_CUR	1		// global match in current file
#ವ್ಯಾಖ್ಯಾನಿಸು MT_GL_OTH	2		// global match in other file
#ವ್ಯಾಖ್ಯಾನಿಸು MT_ST_OTH	3		// static match in other file
#ವ್ಯಾಖ್ಯಾನಿಸು MT_IC_OFF	4		// add for icase match
#ವ್ಯಾಖ್ಯಾನಿಸು MT_RE_OFF	8		// add for regexp match
#ವ್ಯಾಖ್ಯಾನಿಸು MT_ಮುಖವಾಡ		7		// mask for printing priority
#ವ್ಯಾಖ್ಯಾನಿಸು MT_ಎಣಿಕೆ	16

ಸ್ಥಿರ ಅಕ್ಷರ	*mt_ಹೆಸರುಗಳು[MT_ಎಣಿಕೆ/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#ವ್ಯಾಖ್ಯಾನಿಸು NOTAGFILE	99		// return value for jumpto_tag
ಸ್ಥಿರ char_u	*nofile_fname = ಖಾಲಿ;	// fname for NOTAGFILE error

ಸ್ಥಿರ ಶೂನ್ಯ taglen_advance(ಪೂರ್ಣಾಂಕ l);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ jumpto_ಟ್ಯಾಗ್(char_u *lbuf, ಪೂರ್ಣಾಂಕ forceit, ಪೂರ್ಣಾಂಕ keep_help);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_ಟ್ಯಾಗ್_ಸಾಲು(char_u *lbuf, ಪೂರ್ಣಾಂಕ is_etag, tagptrs_T *tagp);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_ಟ್ಯಾಗ್_ಸಾಲು(char_u *lbuf, tagptrs_T *tagp);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ test_for_static(tagptrs_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_ಹೊಂದಿಕೆ(char_u *lbuf, tagptrs_T *tagp);
ಸ್ಥಿರ char_u *ಟ್ಯಾಗ್_ತುಂಬಿದ_fname(tagptrs_T *tagp);
ಸ್ಥಿರ char_u *expand_ಟ್ಯಾಗ್_fname(char_u *fname, char_u *ಟ್ಯಾಗ್_fname, ಪೂರ್ಣಾಂಕ expand);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ test_for_current(ಪೂರ್ಣಾಂಕ, char_u *, char_u *, char_u *, char_u *);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ test_for_current(char_u *, char_u *, char_u *, char_u *);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_extra(char_u **pp);
ಸ್ಥಿರ ಶೂನ್ಯ print_ಟ್ಯಾಗ್_list(ಪೂರ್ಣಾಂಕ new_ಟ್ಯಾಗ್, ಪೂರ್ಣಾಂಕ use_tagstack, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_matches, char_u **matches);
#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_llist_ಟ್ಯಾಗುಗಳು(char_u *ಟ್ಯಾಗ್, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_matches, char_u **matches);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ char_u	*tagmatchname = ಖಾಲಿ;	// name of last used tag

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
ಸ್ಥಿರ taggy_T ptag_ನಮೂದು = {ಖಾಲಿ, {{0, 0, 0}, 0}, 0, 0, ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  tfu_ಒಳ_use = FALSE;	    // disallow recursive call of tagfunc
ಸ್ಥಿರ callback_T tfu_cb;	    // 'tagfunc' callback function
#ಆದರೆ_ಅಂತ್ಯ

// Used instead of NUL to separate tag fields in the growarrays.
#ವ್ಯಾಖ್ಯಾನಿಸು ಟ್ಯಾಗ್_SEP 0x02

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    ಅಕ್ಷರ *
did_ಗಣ_tagfunc(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ	retval;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string,
							    &curbuf->b_tfu_cb);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = option_ಗಣ_callback_func(ವಾದಗಳು->os_newval.string, &tfu_cb);
	ಆದರೆ (retval == OK && !(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	    ಗಣ_buflocal_tfu_callback(curbuf);
    }

    ಹಿಂತಿರುಗಿ retval == FAIL ? e_invalid_ವಾದ : ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_tagfunc_option(ಶೂನ್ಯ)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_tagfunc(ಪೂರ್ಣಾಂಕ copyID ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	abort = FALSE;

    abort = ಗಣ_ref_ಒಳ_callback(&tfu_cb, copyID);

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    ಶೂನ್ಯ
ಗಣ_buflocal_tfu_callback(buf_T *buf ಬಳಸದ)
{
    free_callback(&buf->b_tfu_cb);
    ಆದರೆ (tfu_cb.cb_name != ಖಾಲಿ && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
do_ಟ್ಯಾಗ್(
    char_u	*ಟ್ಯಾಗ್,		// tag (pattern) to jump to
    ಪೂರ್ಣಾಂಕ		type,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		forceit,	// :ta with !
    ಪೂರ್ಣಾಂಕ		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಂಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಂಕ		tagstacklen = curwin->w_tagstacklen;
    ಪೂರ್ಣಾಂಕ		cur_ಹೊಂದಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		cur_fnum = curbuf->b_fnum;
    ಪೂರ್ಣಾಂಕ		oldtagstackidx = tagstackidx;
    ಪೂರ್ಣಾಂಕ		prevtagstackidx = tagstackidx;
    ಪೂರ್ಣಾಂಕ		prev_ಅಂಕೆ_matches;
    ಪೂರ್ಣಾಂಕ		new_ಟ್ಯಾಗ್ = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ic;
    ಪೂರ್ಣಾಂಕ		no_regexp = FALSE;
    ಪೂರ್ಣಾಂಕ		error_cur_ಹೊಂದಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		save_pos = FALSE;
    fmark_T	saved_fmark;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		jumped_ಗಮ್ಯ_ಟ್ಯಾಗ್ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		new_ಅಂಕೆ_matches;
    char_u	**new_matches;
    ಪೂರ್ಣಾಂಕ		use_tagstack;
    ಪೂರ್ಣಾಂಕ		skip_ಸಂದೇಶ = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    ಪೂರ್ಣಾಂಕ		use_tfu = 1;
    char_u	*tofree = ಖಾಲಿ;

    // remember the matches for the last used tag
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		max_ಅಂಕೆ_matches = 0;  // limit used for match search
    ಸ್ಥಿರ char_u	**matches = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		flags;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EXITFREE
    ಆದರೆ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(ಅಂಕೆ_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_ಟ್ಯಾಗುಗಳು();
# endif
	ಅಂಕೆ_matches = 0;
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (tfu_ಒಳ_use)
    {
	emsg(_(e_cannot_modify_ಟ್ಯಾಗ್_ರಾಶಿ_within_tagfunc));
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (postponed_ವಿಭಜಿಸು == 0 && !check_can_ಗಣ_curbuf_forceit(forceit))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (type == DT_HELP)
    {
	type = DT_ಟ್ಯಾಗ್;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_ಅಂಕೆ_matches = ಅಂಕೆ_matches;
    free_string_option(nofile_fname);
    nofile_fname = ಖಾಲಿ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ಆದರೆ ((!p_tgst && *ಟ್ಯಾಗ್ != NUL))
    {
	use_tagstack = FALSE;
	new_ಟ್ಯಾಗ್ = TRUE;
#ಆದರೆ defined(FEAT_QUICKFIX)
	ಆದರೆ (g_do_tagpreview != 0)
	{
	    tagstack_clear_ನಮೂದು(&ptag_ನಮೂದು);
	    ಆದರೆ ((ptag_ನಮೂದು.tagname = vim_strsave(ಟ್ಯಾಗ್)) == ಖಾಲಿ)
		ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(FEAT_QUICKFIX)
	ಆದರೆ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ಆದರೆ (*ಟ್ಯಾಗ್ != NUL
		&& (type == DT_ಟ್ಯಾಗ್ || type == DT_SELECT || type == DT_JUMP
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		    || type == DT_LTAG
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		    || type == DT_CSCOPE
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	{
#ಆದರೆ defined(FEAT_QUICKFIX)
	    ಆದರೆ (g_do_tagpreview != 0)
	    {
		ಆದರೆ (ptag_ನಮೂದು.tagname != ಖಾಲಿ
			&& STRCMP(ptag_ನಮೂದು.tagname, ಟ್ಯಾಗ್) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_ಹೊಂದಿಕೆ = ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ;
		    cur_fnum = ptag_ನಮೂದು.cur_fnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tagstack_clear_ನಮೂದು(&ptag_ನಮೂದು);
		    ಆದರೆ ((ptag_ನಮೂದು.tagname = vim_strsave(ಟ್ಯಾಗ್)) == ಖಾಲಿ)
			ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ಆಗ (tagstackidx < tagstacklen)
		    tagstack_clear_ನಮೂದು(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ಆದರೆ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_ನಮೂದು(&tagstack[0]);
		    ಗಾಗಿ (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_ದತ್ತಾಂಶ = ಖಾಲಿ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ಆದರೆ ((tagstack[tagstackidx].tagname = vim_strsave(ಟ್ಯಾಗ್)) == ಖಾಲಿ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_ಟ್ಯಾಗ್ = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (
#ಆದರೆ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_ನಮೂದು.tagname == ಖಾಲಿ :
#ಆದರೆ_ಅಂತ್ಯ
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_ಟ್ಯಾಗ್_ರಾಶಿ_ಖಾಲಿಗಳು));
		ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
	    }

	    ಆದರೆ (type == DT_ತೆಗೆ)		// go to older position
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಪೂರ್ಣಾಂಕ	old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ ((tagstackidx -= ಎಣಿಕೆ_2) < 0)
		{
		    emsg(_(e_at_bottom_ದ_ಟ್ಯಾಗ್_ರಾಶಿ));
		    ಆದರೆ (tagstackidx + ಎಣಿಕೆ_2 == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_ತುದಿ_ದ_ಟ್ಯಾಗ್_ರಾಶಿ));
		    ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ಆದರೆ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ಆದರೆ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.ಲಂಬಸಾಲು = saved_fmark.mark.ಲಂಬಸಾಲು;
		curwin->w_ಗಣ_curswant = true;
		check_cursor();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ ((fdo_flags & FDO_ಟ್ಯಾಗ್) && old_ಕೀಲಿ_Typed)
		    foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

		// remove the old list of matches
		FreeWild(ಅಂಕೆ_matches, matches);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		cs_free_ಟ್ಯಾಗುಗಳು();
#ಆದರೆ_ಅಂತ್ಯ
		ಅಂಕೆ_matches = 0;
		ಟ್ಯಾಗ್_freematch();
		ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
	    }

	    ಆದರೆ (type == DT_ಟ್ಯಾಗ್
#ಆದರೆ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
#ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (g_do_tagpreview != 0)
		{
		    cur_ಹೊಂದಿಕೆ = ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ;
		    cur_fnum = ptag_ನಮೂದು.cur_fnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ಆದರೆ ((tagstackidx += ಎಣಿಕೆ_2 - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_ತುದಿ_ದ_ಟ್ಯಾಗ್_ರಾಶಿ));
			save_pos = FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_ದ_ಟ್ಯಾಗ್_ರಾಶಿ));
			tagstackidx = 0;
			ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		    }
		    cur_ಹೊಂದಿಕೆ = tagstack[tagstackidx].cur_ಹೊಂದಿಕೆ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_ಟ್ಯಾಗ್ = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ಆದರೆ defined(FEAT_QUICKFIX)
		ಆದರೆ (g_do_tagpreview != 0)
		{
		    cur_ಹೊಂದಿಕೆ = ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ;
		    cur_fnum = ptag_ನಮೂದು.cur_fnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ಆದರೆ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_ಹೊಂದಿಕೆ = tagstack[tagstackidx].cur_ಹೊಂದಿಕೆ;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		ಆಯ್ಕೆ (type)
		{
		    ಸ್ಥಿತಿ DT_ಮೊದಲ: cur_ಹೊಂದಿಕೆ = ಎಣಿಕೆ_2 - 1; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ DT_SELECT:
		    ಸ್ಥಿತಿ DT_JUMP:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		    ಸ್ಥಿತಿ DT_CSCOPE:
#ಆದರೆ_ಅಂತ್ಯ
		    ಸ್ಥಿತಿ DT_ಕೊನೆಯ:  cur_ಹೊಂದಿಕೆ = MAXCOL - 1; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ DT_ಮುಂದಿನ:  cur_ಹೊಂದಿಕೆ += ಎಣಿಕೆ_2; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ DT_PREV:  cur_ಹೊಂದಿಕೆ -= ಎಣಿಕೆ_2; ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (cur_ಹೊಂದಿಕೆ >= MAXCOL)
		    cur_ಹೊಂದಿಕೆ = MAXCOL - 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_ಹೊಂದಿಕೆ < 0)
		{
		    emsg(_(e_cannot_go_before_ಮೊದಲ_matching_ಟ್ಯಾಗ್));
		    skip_ಸಂದೇಶ = TRUE;
		    cur_ಹೊಂದಿಕೆ = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ಆದರೆ defined(FEAT_QUICKFIX)
	ಆದರೆ (g_do_tagpreview != 0)
	{
	    ಆದರೆ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
		ptag_ನಮೂದು.cur_fnum = cur_fnum;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ಆದರೆ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ಆದರೆ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ಆದರೆ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ಆದರೆ (buf != ಖಾಲಿ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ಆದರೆ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ಆದರೆ defined(FEAT_QUICKFIX)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g_do_tagpreview != 0)
	    name = ptag_ನಮೂದು.tagname;
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    name = ಟ್ಯಾಗ್;
	other_name = (tagmatchname == ಖಾಲಿ || STRCMP(tagmatchname, name) != 0);
	ಆದರೆ (new_ಟ್ಯಾಗ್
		|| (cur_ಹೊಂದಿಕೆ >= ಅಂಕೆ_matches && max_ಅಂಕೆ_matches != MAXCOL)
		|| other_name)
	{
	    ಆದರೆ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ಆದರೆ (type == DT_SELECT || type == DT_JUMP
#ಆದರೆ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#ಆದರೆ_ಅಂತ್ಯ
		)
		cur_ಹೊಂದಿಕೆ = MAXCOL - 1;
	    ಆದರೆ (type == DT_ಟ್ಯಾಗ್)
		max_ಅಂಕೆ_matches = MAXCOL;
	    ಇಲ್ಲದಿದ್ದರೆ
		max_ಅಂಕೆ_matches = cur_ಹೊಂದಿಕೆ + 1;

	    // when the argument starts with '/', use it as a regexp
	    ಆದರೆ (!no_regexp && *name == '/')
	    {
		flags = ಟ್ಯಾಗ್_REGEXP;
		++name;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		flags = ಟ್ಯಾಗ್_NOIC;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	    ಆದರೆ (type == DT_CSCOPE)
		flags = ಟ್ಯಾಗ್_CSCOPE;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (verbose)
		flags |= ಟ್ಯಾಗ್_VERBOSE;

	    ಆದರೆ (!use_tfu)
		flags |= ಟ್ಯಾಗ್_NO_TAGFUNC;

	    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(name, &new_ಅಂಕೆ_matches, &new_matches, flags,
					    max_ಅಂಕೆ_matches, buf_ffname) == OK
		    && new_ಅಂಕೆ_matches < max_ಅಂಕೆ_matches)
		max_ಅಂಕೆ_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ಆದರೆ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_ಕಿಟಕಿ_unexpectedly_close_while_searching_for_ಟ್ಯಾಗುಗಳು));
		FreeWild(new_ಅಂಕೆ_matches, new_matches);
		ನಿಲ್ಲಿಸು;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ಆದರೆ (!new_ಟ್ಯಾಗ್ && !other_name)
	    {
		ಪೂರ್ಣಾಂಕ	    j, k;
		ಪೂರ್ಣಾಂಕ	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		ಗಾಗಿ (j = 0; j < ಅಂಕೆ_matches; ++j)
		{
		    parse_ಹೊಂದಿಕೆ(matches[j], &tagp);
		    ಗಾಗಿ (i = idx; i < new_ಅಂಕೆ_matches; ++i)
		    {
			parse_ಹೊಂದಿಕೆ(new_matches[i], &tagp2);
			ಆದರೆ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    ಗಾಗಿ (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
	    }
	    FreeWild(ಅಂಕೆ_matches, matches);
	    ಅಂಕೆ_matches = new_ಅಂಕೆ_matches;
	    matches = new_matches;
	}

	ಆದರೆ (ಅಂಕೆ_matches <= 0)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_ಟ್ಯಾಗ್_not_ಸಿಕ್ಕಿತು_str), name);
#ಆದರೆ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ ask_for_selection = FALSE;
#ಆದರೆ defined(FEAT_EVAL)
	    size_t  IObufflen;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	    ಆದರೆ (type == DT_CSCOPE && ಅಂಕೆ_matches > 1)
	    {
		cs_print_ಟ್ಯಾಗುಗಳು();
		ask_for_selection = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (type == DT_ಟ್ಯಾಗ್ && *ಟ್ಯಾಗ್ != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_ಹೊಂದಿಕೆ = ಎಣಿಕೆ_2 > 0 ? ಎಣಿಕೆ_2 - 1 : 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == DT_SELECT || (type == DT_JUMP && ಅಂಕೆ_matches > 1))
	    {
		print_ಟ್ಯಾಗ್_list(new_ಟ್ಯಾಗ್, use_tagstack, ಅಂಕೆ_matches, matches);
		ask_for_selection = TRUE;
	    }
#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == DT_LTAG)
	    {
		ಆದರೆ (ಕೂಡಿಸು_llist_ಟ್ಯಾಗುಗಳು(ಟ್ಯಾಗ್, ಅಂಕೆ_matches, matches) == FAIL)
		    ಹೋಗು ಕೊನೆ_do_ಟ್ಯಾಗ್;
		cur_ಹೊಂದಿಕೆ = 0;		// Jump to the first tag
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_ಸಂಖ್ಯೆ(ಖಾಲಿ);
		ಆದರೆ (i <= 0 || i > ಅಂಕೆ_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ಆದರೆ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		    cs_free_ಟ್ಯಾಗುಗಳು();
		    jumped_ಗಮ್ಯ_ಟ್ಯಾಗ್ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		    ನಿಲ್ಲಿಸು;
		}
		cur_ಹೊಂದಿಕೆ = i - 1;
	    }

	    ಆದರೆ (cur_ಹೊಂದಿಕೆ >= ಅಂಕೆ_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ಆದರೆ ((type == DT_ಮುಂದಿನ || type == DT_ಮೊದಲ)
						      && nofile_fname == ಖಾಲಿ)
		{
		    ಆದರೆ (ಅಂಕೆ_matches == 1)
			emsg(_(e_there_is_only_one_matching_ಟ್ಯಾಗ್));
		    ಇಲ್ಲದಿದ್ದರೆ
			emsg(_(e_cannot_go_beyond_ಕೊನೆಯ_matching_ಟ್ಯಾಗ್));
		    skip_ಸಂದೇಶ = TRUE;
		}
		cur_ಹೊಂದಿಕೆ = ಅಂಕೆ_matches - 1;
	    }
	    ಆದರೆ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ಆದರೆ (use_tfu && parse_ಹೊಂದಿಕೆ(matches[cur_ಹೊಂದಿಕೆ], &tagp) == OK
			&& tagp.user_ದತ್ತಾಂಶ)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_ದತ್ತಾಂಶ);
		    tagstack[tagstackidx].user_ದತ್ತಾಂಶ = vim_strnsave(
			  tagp.user_ದತ್ತಾಂಶ, tagp.user_ದತ್ತಾಂಶ_ಕೊನೆ - tagp.user_ದತ್ತಾಂಶ);
		}

		++tagstackidx;
	    }
#ಆದರೆ defined(FEAT_QUICKFIX)
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g_do_tagpreview != 0)
	    {
		ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
		ptag_ನಮೂದು.cur_fnum = cur_fnum;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ಆದರೆ (nofile_fname != ಖಾಲಿ && error_cur_ಹೊಂದಿಕೆ != cur_ಹೊಂದಿಕೆ)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_ಹೊಂದಿಕೆ][0] & MT_IC_OFF);
	    ಆದರೆ (type != DT_ಟ್ಯಾಗ್ && type != DT_SELECT && type != DT_JUMP
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		&& type != DT_CSCOPE
#ಆದರೆ_ಅಂತ್ಯ
		&& (ಅಂಕೆ_matches > 1 || ic)
		&& !skip_ಸಂದೇಶ)
	    {
		// Give an indication of the number of matching tags
		sprintf((ಅಕ್ಷರ *)IObuff, _("tag %d of %d%s"),
				cur_ಹೊಂದಿಕೆ + 1,
				ಅಂಕೆ_matches,
				max_ಅಂಕೆ_matches != MAXCOL ? _(" or more") : "");
		ಆದರೆ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ಆದರೆ ((ಅಂಕೆ_matches > prev_ಅಂಕೆ_matches || new_ಟ್ಯಾಗ್)
							   && ಅಂಕೆ_matches > 1)
		{
		    ಆದರೆ (ic)
			ಸಂದೇಶ_attr((ಅಕ್ಷರ *)IObuff, HL_ATTR(HLF_W));
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
		    ಸಂದೇಶ_scroll = TRUE;	// don't overwrite this message
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    give_warning(IObuff, ic);
		ಆದರೆ (ic && !ಸಂದೇಶ_scrolled && ಸಂದೇಶ_silent == 0)
		{
		    ಹೊರಹೋಗು_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ಆದರೆ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE, ":ta %s\r", name);
	    ಗಣ_vim_var_string(VV_SWAPCOMMAND, IObuff, (ಪೂರ್ಣಾಂಕ)IObufflen);
#ಆದರೆ_ಅಂತ್ಯ

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_ಟ್ಯಾಗ್(matches[cur_ಹೊಂದಿಕೆ], forceit, type != DT_CSCOPE);

#ಆದರೆ defined(FEAT_EVAL)
	    ಗಣ_vim_var_string(VV_SWAPCOMMAND, ಖಾಲಿ, -1);
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ಆದರೆ ((type == DT_PREV && cur_ಹೊಂದಿಕೆ > 0)
			|| ((type == DT_ಟ್ಯಾಗ್ || type == DT_ಮುಂದಿನ
							  || type == DT_ಮೊದಲ)
			    && (max_ಅಂಕೆ_matches != MAXCOL
					     || cur_ಹೊಂದಿಕೆ < ಅಂಕೆ_matches - 1)))
		{
		    error_cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
		    ಆದರೆ (use_tagstack)
			--tagstackidx;
		    ಆದರೆ (type == DT_PREV)
			--cur_ಹೊಂದಿಕೆ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			type = DT_ಮುಂದಿನ;
			++cur_ಹೊಂದಿಕೆ;
		    }
		    ಮುಂದುವರಿ;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ಆದರೆ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		jumped_ಗಮ್ಯ_ಟ್ಯಾಗ್ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ನಿಲ್ಲಿಸು;
    }

ಕೊನೆ_do_ಟ್ಯಾಗ್:
    // Only store the new index when using the tagstack and it's valid.
    ಆದರೆ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_ವಿಭಜಿಸು = 0;	// don't split next time
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(tofree);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಹಿಂತಿರುಗಿ jumped_ಗಮ್ಯ_ಟ್ಯಾಗ್;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * List all the matching tags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_ಟ್ಯಾಗ್_list(
    ಪೂರ್ಣಾಂಕ		new_ಟ್ಯಾಗ್,
    ಪೂರ್ಣಾಂಕ		use_tagstack,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಂಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    char_u	*command_ಕೊನೆ;
    tagptrs_T	tagp;
    ಪೂರ್ಣಾಂಕ		taglen;
    ಪೂರ್ಣಾಂಕ		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_ಹೊಂದಿಕೆ(matches[0], &tagp);
    taglen = (ಪೂರ್ಣಾಂಕ)(tagp.tagname_ಕೊನೆ - tagp.tagname + 2);
    ಆದರೆ (taglen < 18)
	taglen = 18;
    ಆದರೆ (taglen > cmdline_ಅಗಲ - 25)
	taglen = MAXCOL;
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
	ಸಂದೇಶ_didout = FALSE;	// overwrite previous message
    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    ಸಂದೇಶ_clr_eos();
    taglen_advance(taglen);
    ಸಂದೇಶ_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches && !got_int; ++i)
    {
	parse_ಹೊಂದಿಕೆ(matches[i], &tagp);
	ಆದರೆ (!new_ಟ್ಯಾಗ್ && (
#ಆದರೆ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_ನಮೂದು.cur_ಹೊಂದಿಕೆ) ||
#ಆದರೆ_ಅಂತ್ಯ
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_ಹೊಂದಿಕೆ)))
	    *IObuff = '>';
	ಇಲ್ಲದಿದ್ದರೆ
	    *IObuff = ' ';
	vim_snprintf((ಅಕ್ಷರ *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_ಹೆಸರುಗಳು[matches[i][0] & MT_ಮುಖವಾಡ]);
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	ಆದರೆ (tagp.tagkind != ಖಾಲಿ)
	    ಸಂದೇಶ_outtrans_len(tagp.tagkind,
			  (ಪೂರ್ಣಾಂಕ)(tagp.tagkind_ಕೊನೆ - tagp.tagkind));
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outtrans_len_attr(tagp.tagname,
			   (ಪೂರ್ಣಾಂಕ)(tagp.tagname_ಕೊನೆ - tagp.tagname),
						  HL_ATTR(HLF_T));
	ಸಂದೇಶ_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = ಟ್ಯಾಗ್_ತುಂಬಿದ_fname(&tagp);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಸಂದೇಶ_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 0)
	    ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)
	    ನಿಲ್ಲಿಸು;
	ಸಂದೇಶ_advance(15);

	// print any extra fields
	command_ಕೊನೆ = tagp.command_ಕೊನೆ;
	ಆದರೆ (command_ಕೊನೆ != ಖಾಲಿ)
	{
	    p = command_ಕೊನೆ + 3;
	    ಆಗ (*p && *p != '\r' && *p != '\n')
	    {
		ಆಗ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ಆದರೆ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    ಮುಂದುವರಿ;
		}
		// skip "kind:<kind>" and "<kind>"
		ಆದರೆ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_ಕೊನೆ;
		    ಮುಂದುವರಿ;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ಆಗ (*p && *p != '\r' && *p != '\n')
		{
		    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು + ptr2cells(p) >= cmdline_ಅಗಲ)
		    {
			ಸಂದೇಶ_putchar('\n');
			ಆದರೆ (got_int)
			    ನಿಲ್ಲಿಸು;
			ಸಂದೇಶ_advance(15);
		    }
		    p = ಸಂದೇಶ_outtrans_one(p, attr);
		    ಆದರೆ (*p == TAB)
		    {
			ಸಂದೇಶ_puts_attr(" ", attr);
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (*p == ':')
			attr = 0;
		}
	    }
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು > 15)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
		ಸಂದೇಶ_advance(15);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_ಕೊನೆ = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ಆದರೆ (*p == '/' || *p == '?')
	{
	    ++p;
	    ಆದರೆ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ಆಗ (p != command_ಕೊನೆ && vim_isspace(*p))
	    ++p;

	ಆಗ (p != command_ಕೊನೆ)
	{
	    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_ಅಗಲ)
		ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    ಸಂದೇಶ_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ಆದರೆ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ಆದರೆ (*p == TAB)
	    {
		ಸಂದೇಶ_putchar(' ');
		++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		p = ಸಂದೇಶ_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ಆದರೆ (p == command_ಕೊನೆ - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ನಿಲ್ಲಿಸು;
	    // don't display matching '/' or '?'
	    ಆದರೆ (p == command_ಕೊನೆ - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು)
	    ಸಂದೇಶ_putchar('\n');
	ui_breakcheck();
    }
    ಆದರೆ (got_int)
	got_int = FALSE;	// only stop the listing
}

#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_llist_ಟ್ಯಾಗುಗಳು(
    char_u	*ಟ್ಯಾಗ್,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	ಟ್ಯಾಗ್_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ಆದರೆ (list == ಖಾಲಿ || fname == ಖಾಲಿ || cmd == ಖಾಲಿ)
    {
	vim_free(cmd);
	vim_free(fname);
	ಆದರೆ (list != ಖಾಲಿ)
	    list_free(list);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
    {
	ಪೂರ್ಣಾಂಕ	    len, cmd_len;
	ದೀರ್ಘ    lnum;
	dict_T  *dict;

	parse_ಹೊಂದಿಕೆ(matches[i], &tagp);

	// Save the tag name
	len = (ಪೂರ್ಣಾಂಕ)(tagp.tagname_ಕೊನೆ - tagp.tagname);
	ಆದರೆ (len > 128)
	    len = 128;
	vim_strncpy(ಟ್ಯಾಗ್_name, tagp.tagname, len);
	ಟ್ಯಾಗ್_name[len] = NUL;

	// Save the tag file name
	p = ಟ್ಯಾಗ್_ತುಂಬಿದ_fname(&tagp);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((ಅಕ್ಷರ *)tagp.command);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *cmd_ಆರಂಭ, *cmd_ಕೊನೆ;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_ಆರಂಭ = tagp.command;
	    cmd_ಕೊನೆ = tagp.command_ಕೊನೆ;
	    ಆದರೆ (cmd_ಕೊನೆ == ಖಾಲಿ)
	    {
		ಗಾಗಿ (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_ಕೊನೆ = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_ಕೊನೆ--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    ಆದರೆ (*cmd_ಆರಂಭ == '/' || *cmd_ಆರಂಭ == '?')
		cmd_ಆರಂಭ++;

	    ಆದರೆ (*cmd_ಕೊನೆ == '/' || *cmd_ಕೊನೆ == '?')
		cmd_ಕೊನೆ--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ಆದರೆ (*cmd_ಆರಂಭ == '^')
	    {
		STRCPY(cmd, "^");
		cmd_ಆರಂಭ++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (ಪೂರ್ಣಾಂಕ)(cmd_ಕೊನೆ - cmd_ಆರಂಭ + 1);
	    ಆದರೆ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_ಆರಂಭ, cmd_len);
	    len += cmd_len;

	    ಆದರೆ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ಆದರೆ ((dict = dict_alloc()) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    ಮುಂದುವರಿ;
	}

	dict_ಕೂಡಿಸು_string(dict, "text", ಟ್ಯಾಗ್_name);
	dict_ಕೂಡಿಸು_string(dict, "filename", fname);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", lnum);
	ಆದರೆ (lnum == 0)
	    dict_ಕೂಡಿಸು_string(dict, "pattern", cmd);
    }

    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "ltag %s", ಟ್ಯಾಗ್);
    ಗಣ_errorlist(curwin, list, ' ', IObuff, ಖಾಲಿ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Free cached tags.
 */
    ಶೂನ್ಯ
ಟ್ಯಾಗ್_freematch(ಶೂನ್ಯ)
{
    VIM_CLEAR(tagmatchname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
taglen_advance(ಪೂರ್ಣಾಂಕ l)
{
    ಆದರೆ (l == MAXCOL)
    {
	ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_advance(24);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_advance(13 + l);
}

/*
 * Print the tag stack
 */
    ಶೂನ್ಯ
do_ಟ್ಯಾಗುಗಳು(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    ಪೂರ್ಣಾಂಕ		tagstackidx = curwin->w_tagstackidx;
    ಪೂರ್ಣಾಂಕ		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    ಗಾಗಿ (i = 0; i < tagstacklen; ++i)
    {
	ಆದರೆ (tagstack[i].tagname != ಖಾಲಿ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ಆದರೆ (name == ಖಾಲಿ)	    // file name not available
		ಮುಂದುವರಿ;

	    ಸಂದೇಶ_putchar('\n');
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_ಹೊಂದಿಕೆ + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    ಸಂದೇಶ_outtrans(IObuff);
	    ಸಂದೇಶ_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	ಹೊರಹೋಗು_flush();		    // show one line at a time
    }
    ಆದರೆ (tagstackidx == tagstacklen)	// idx at top of stack
	ಸಂದೇಶ_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಟ್ಯಾಗ್_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆಗ (len > 0)
    {
	i = (ಪೂರ್ಣಾಂಕ)TOUPPER_ASC(*s1) - (ಪೂರ್ಣಾಂಕ)TOUPPER_ASC(*s2);
	ಆದರೆ (i != 0)
	    ಹಿಂತಿರುಗಿ i;			// this character different
	ಆದರೆ (*s1 == NUL)
	    ನಿಲ್ಲಿಸು;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ಹಿಂತಿರುಗಿ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*pat;		// the pattern
    ಪೂರ್ಣಾಂಕ		len;		// length of pat[]
    char_u	*ತಲೆ;		// start of pattern head
    ಪೂರ್ಣಾಂಕ		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_pats(pat_T *pats, ಪೂರ್ಣಾಂಕ has_re)
{
    pats->ತಲೆ = pats->pat;
    pats->headlen = pats->len;
    ಆದರೆ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ಆದರೆ (pats->pat[0] == '^')
	    pats->ತಲೆ = pats->pat + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->ತಲೆ = pats->pat + 2;
	ಆದರೆ (pats->ತಲೆ == pats->pat)
	    pats->headlen = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾಗಿ (pats->headlen = 0; pats->ತಲೆ[pats->headlen] != NUL;
							      ++pats->headlen)
		ಆದರೆ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->ತಲೆ[pats->headlen]) != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	ಆದರೆ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ಆದರೆ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ಇಲ್ಲದಿದ್ದರೆ
	pats->regmatch.regprog = ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ವಿಳಾಸ,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    ಸ್ಥಿರ tagcmd_T
tagfunc_cmd_ಬಗೆ(char_u *cmd)
{
    ಆದರೆ (VIM_ISDIGIT(*cmd))
	ಹಿಂತಿರುಗಿ *skipdigits(cmd) == NUL ? TAGCMD_ವಿಳಾಸ : TAGCMD_INVALID;
    ಆದರೆ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*ಕೊನೆ = skip_regexp(cmd + 1, *cmd, FALSE);

	ಹಿಂತಿರುಗಿ (*ಕೊನೆ == *cmd && ಕೊನೆ[1] == NUL)
					? TAGCMD_ವಿಳಾಸ : TAGCMD_INVALID;
    }
    ಆದರೆ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TAGCMD_INVALID;
    ಹಿಂತಿರುಗಿ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_tagfunc_ಟ್ಯಾಗುಗಳು(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    ಪೂರ್ಣಾಂಕ		*ಹೊಂದಿಕೆ_ಎಣಿಕೆ,	// here the number of tags found will be placed
    ಪೂರ್ಣಾಂಕ		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		ntags = 0;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = FAIL;
    typval_T	ವಾದಗಳು[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*ಟ್ಯಾಗ್ = &curwin->w_tagstack[curwin->w_tagstackidx];

    ಆದರೆ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ಖಾಲಿ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    ವಾದಗಳು[0].v_type = VAR_STRING;
    ವಾದಗಳು[0].vval.v_string = pat;
    ವಾದಗಳು[1].v_type = VAR_STRING;
    ವಾದಗಳು[1].vval.v_string = flagString;

    // create 'info' dict argument
    ಆದರೆ ((d = dict_alloc_ಬೀಗ(VAR_FIXED)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (!(flags & ಟ್ಯಾಗ್_INS_COMP) && ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(d, "user_data", ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ);
    ಆದರೆ (buf_ffname != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    ವಾದಗಳು[2].v_type = VAR_DICT;
    ವಾದಗಳು[2].vval.v_dict = d;

    ವಾದಗಳು[3].v_type = VAR_UNKNOWN;

    vim_snprintf((ಅಕ್ಷರ *)flagString, ಗಾತ್ರ(flagString),
		 "%s%s%s",
		 g_ಟ್ಯಾಗ್_at_cursor      ? "c": "",
		 flags & ಟ್ಯಾಗ್_INS_COMP ? "i": "",
		 flags & ಟ್ಯಾಗ್_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    ಫಲಿತಾಂಶ = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, ವಾದಗಳು);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ಆದರೆ (ಫಲಿತಾಂಶ == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_ಸಂಖ್ಯೆ == VVAL_NULL)
    {
	clear_tv(&rettv);
	ಹಿಂತಿರುಗಿ NOTDONE;
    }
    ಆದರೆ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(taglist, ಅಂಶ)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_ಬಗೆ;
	ಪೂರ್ಣಾಂಕ		len;
	dict_iterator_T	iter;
	char_u		*dict_ಕೀಲಿ;
	typval_T	*tv;
	ಪೂರ್ಣಾಂಕ		has_extra = 0;
	ಪೂರ್ಣಾಂಕ		name_only = flags & ಟ್ಯಾಗ್_ಹೆಸರುಗಳು;

	ಆದರೆ (ಅಂಶ->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ನಿಲ್ಲಿಸು;
	}

# ifdef FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	len = 3;
# ಇಲ್ಲದಿದ್ದರೆ
	len = 2;
# endif
	res_name = ಖಾಲಿ;
	res_fname = ಖಾಲಿ;
	res_cmd = ಖಾಲಿ;
	res_ಬಗೆ = ಖಾಲಿ;

	dict_iterate_ಆರಂಭ(&ಅಂಶ->li_tv, &iter);
	ಆಗ (ಖಾಲಿ != (dict_ಕೀಲಿ = dict_iterate_ಮುಂದಿನ(&iter, &tv)))
	{
	    ಆದರೆ (tv->v_type != VAR_STRING || tv->vval.v_string == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    len += (ಪೂರ್ಣಾಂಕ)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		ಮುಂದುವರಿ;
	    }
	    has_extra = 1;
	    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "kind") == 0)
	    {
		res_ಬಗೆ = tv->vval.v_string;
		ಮುಂದುವರಿ;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (ಪೂರ್ಣಾಂಕ)STRLEN(dict_ಕೀಲಿ) + 1;
	}

	ಆದರೆ (has_extra)
	    len += 2;	// need space for ;"

	ಆದರೆ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ನಿಲ್ಲಿಸು;
	}

	tagcmd_T cmdkind = tagfunc_cmd_ಬಗೆ(res_cmd);
	ಆದರೆ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_ಮೌಲ್ಯ_from_tagfunc));
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ಆದರೆ (name_only)
	    mfp = vim_strsave(res_name);
	ಇಲ್ಲದಿದ್ದರೆ
	    mfp = alloc(ಗಾತ್ರ(char_u) + len + 1);

	ಆದರೆ (mfp == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = ಟ್ಯಾಗ್_SEP;	    // no tag file name
# ifdef FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	    *p++ = ಟ್ಯಾಗ್_SEP;
# endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    ಆದರೆ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ಆದರೆ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ಆದರೆ (res_ಬಗೆ)
		{
		    *p++ = TAB;
		    STRCPY(p, res_ಬಗೆ);
		    p += STRLEN(p);
		}

		dict_iterate_ಆರಂಭ(&ಅಂಶ->li_tv, &iter);
		ಆಗ (ಖಾಲಿ != (dict_ಕೀಲಿ = dict_iterate_ಮುಂದಿನ(&iter, &tv)))
		{
		    ಆದರೆ (tv->v_type != VAR_STRING || tv->vval.v_string == ಖಾಲಿ)
			ಮುಂದುವರಿ;

		    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "name") == 0)
			ಮುಂದುವರಿ;
		    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "filename") == 0)
			ಮುಂದುವರಿ;
		    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "cmd") == 0)
			ಮುಂದುವರಿ;
		    ಆದರೆ (STRCMP(dict_ಕೀಲಿ, "kind") == 0)
			ಮುಂದುವರಿ;

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
	ಆದರೆ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_ದತ್ತಾಂಶ))[ga->ga_len++] = mfp;
	    ++ntags;
	    ಫಲಿತಾಂಶ = OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_free(mfp);
	    ನಿಲ್ಲಿಸು;
	}
    }

    clear_tv(&rettv);

    *ಹೊಂದಿಕೆ_ಎಣಿಕೆ = ntags;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * State information used during a tag search
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    tagsearch_state_T	state;		// tag search state
    ಪೂರ್ಣಾಂಕ		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    ಪೂರ್ಣಾಂಕ		lbuf_ಗಾತ್ರ;		// length of lbuf
    char_u	*ಟ್ಯಾಗ್_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    ಪೂರ್ಣಾಂಕ		flags;			// flags used for tag search
    ಪೂರ್ಣಾಂಕ		ಟ್ಯಾಗ್_file_sorted;	// !_TAG_FILE_SORTED value
    ಪೂರ್ಣಾಂಕ		get_searchpat;		// used for 'showfulltag'
    ಪೂರ್ಣಾಂಕ		help_only;		// only search for help tags
    ಪೂರ್ಣಾಂಕ		did_open;		// did open a tag file
    ಪೂರ್ಣಾಂಕ		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    ಪೂರ್ಣಾಂಕ		ರೇಖೀಯ;			// do a linear search
    vimconv_T	vimconv;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಪೂರ್ಣಾಂಕ		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    ಪೂರ್ಣಾಂಕ		help_pri;		// help language priority
    char_u	*help_lang_ಕಂಡುಹಿಡಿ;	// lang to be found
    ಪೂರ್ಣಾಂಕ		is_txt;			// flag of file extension
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ_ಎಣಿಕೆ;		// number of matches found
    garray_T	ga_ಹೊಂದಿಕೆ[MT_ಎಣಿಕೆ];	// stores matches in sequence
    hashtab_T	ht_ಹೊಂದಿಕೆ[MT_ಎಣಿಕೆ];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    ಪೂರ್ಣಾಂಕ			flags,
    ಪೂರ್ಣಾಂಕ			mincount)
{
    ಪೂರ್ಣಾಂಕ		mtt;

    st->ಟ್ಯಾಗ್_fname = alloc(MAXPATHL + 1);
    st->fp = ಖಾಲಿ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
    st->orgpat->regmatch.regprog = ಖಾಲಿ;
    st->flags = flags;
    st->ಟ್ಯಾಗ್_file_sorted = NUL;
    st->help_only = (flags & ಟ್ಯಾಗ್_HELP);
    st->get_searchpat = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
    st->is_txt = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_ಗಾತ್ರ = LSIZE;
    st->lbuf = alloc(st->lbuf_ಗಾತ್ರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    st->ebuf = alloc(LSIZE);
#ಆದರೆ_ಅಂತ್ಯ
    st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ = 0;
    st->stop_searching = FALSE;

    ಗಾಗಿ (mtt = 0; mtt < MT_ಎಣಿಕೆ; ++mtt)
    {
	ga_init2(&st->ga_ಹೊಂದಿಕೆ[mtt], ಗಾತ್ರ(char_u *), 100);
	hash_init(&st->ht_ಹೊಂದಿಕೆ[mtt]);
    }

    // check for out of memory situation
    ಆದರೆ (st->ಟ್ಯಾಗ್_fname == ಖಾಲಿ
	    || st->lbuf == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	    || st->ebuf == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
       )
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Free the state used by find_tags()
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->ಟ್ಯಾಗ್_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    vim_free(st->ebuf);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_ಒಳ_help_init(findtags_state_T *st)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ಆದರೆ (st->is_txt)
	STRCPY(st->help_lang, "en");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (ಪೂರ್ಣಾಂಕ)STRLEN(st->ಟ್ಯಾಗ್_fname);
	ಆದರೆ (i > 3 && st->ಟ್ಯಾಗ್_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->ಟ್ಯಾಗ್_fname + i - 2, 2);
	ಇಲ್ಲದಿದ್ದರೆ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ಆದರೆ (st->help_lang_ಕಂಡುಹಿಡಿ != ಖಾಲಿ
	    && STRICMP(st->help_lang, st->help_lang_ಕಂಡುಹಿಡಿ) != 0)
	ಹಿಂತಿರುಗಿ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ಆದರೆ ((st->flags & ಟ್ಯಾಗ್_KEEP_LANG)
	    && st->help_lang_ಕಂಡುಹಿಡಿ == ಖಾಲಿ
	    && curbuf->b_fname != ಖಾಲಿ
	    && (i = (ಪೂರ್ಣಾಂಕ)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	ಗಾಗಿ (s = p_hlg; *s != NUL; ++s)
	{
	    ಆದರೆ (STRNICMP(s, st->help_lang, 2) == 0)
		ನಿಲ್ಲಿಸು;
	    ++st->help_pri;
	    ಆದರೆ ((s = vim_strchr(s, ',')) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ಆದರೆ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    ಪೂರ್ಣಾಂಕ		use_tfu = ((st->flags & ಟ್ಯಾಗ್_NO_TAGFUNC) == 0);
    ಪೂರ್ಣಾಂಕ		retval;

    ಆದರೆ (!use_tfu || tfu_ಒಳ_use || *curbuf->b_p_tfu == NUL)
	ಹಿಂತಿರುಗಿ NOTDONE;

    tfu_ಒಳ_use = TRUE;
    retval = ಕಂಡುಹಿಡಿ_tagfunc_ಟ್ಯಾಗುಗಳು(pat, st->ga_ಹೊಂದಿಕೆ, &st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ,
						st->flags, buf_ffname);
    tfu_ಒಳ_use = FALSE;

    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_ಗಾತ್ರ 42
ಸ್ಥಿರ ರಚನೆ
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_ಗಾತ್ರ];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emacs_ಟ್ಯಾಗುಗಳು_incstack_free(ಶೂನ್ಯ)
{
    ಆಗ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ಖಾಲಿ;
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
emacs_ಟ್ಯಾಗುಗಳು_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ಆದರೆ (vim_fgets(st->ebuf, LSIZE, st->fp))
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ಆದರೆ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->ಟ್ಯಾಗ್_fname);
    ಆದರೆ (incstack[incstack_idx].etag_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ಖಾಲಿ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_ಟ್ಯಾಗ್_fname(st->ebuf, st->ಟ್ಯಾಗ್_fname, FALSE);
    ಆದರೆ (fullpath_ebuf != ಖಾಲಿ)
    {
	st->fp = mch_fopen((ಅಕ್ಷರ *)fullpath_ebuf, "r");
	ಆದರೆ (st->fp != ಖಾಲಿ)
	{
	    ಆದರೆ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_ಟ್ಯಾಗ್_file_ಮಾರ್ಗ_truncated_for_str), st->ebuf);
	    vim_strncpy(st->ಟ್ಯಾಗ್_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ಆದರೆ (st->fp == ಖಾಲಿ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
emacs_ಟ್ಯಾಗುಗಳು_file_eof(findtags_state_T *st)
{
    ಆದರೆ (!incstack_idx)	// reached end of file. stop processing.
	ಹಿಂತಿರುಗಿ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->ಟ್ಯಾಗ್_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
emacs_ಟ್ಯಾಗುಗಳು_parse_ಸಾಲು(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ಆದರೆ (p_7f == ಖಾಲಿ)
    {
etag_fail:
	ಆದರೆ (vim_strchr(lbuf, '\n') != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Truncated line.  Ignore it.
	ಆದರೆ (p_verbose >= 5)
	{
	    verbose_enter();
	    ಸಂದೇಶ(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_ಕೊನೆ = lbuf;
	ಹಿಂತಿರುಗಿ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ಆದರೆ (p == ಖಾಲಿ)
	p = p_7f + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	++p;

    ಆದರೆ (!VIM_ISDIGIT(*p))	    // check for start of line number
	ಹೋಗು etag_fail;
    tagp->command = p;

    ಆದರೆ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_ಕೊನೆ = p - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ			    // second format: isolate tagname
    {
	ಆದರೆ (p_7f == lbuf)
	    ಹೋಗು etag_fail;

	// find end of tagname
	ಗಾಗಿ (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ಆದರೆ (p == lbuf)
		ಹೋಗು etag_fail;
	tagp->tagname_ಕೊನೆ = p + 1;
	ಆಗ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    ಸ್ಥಿರ ಟ್ಯಾಗುಗಳು_read_ಸ್ಥಿತಿ_T
findtags_get_ಮುಂದಿನ_ಸಾಲು(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    ಪೂರ್ಣಾಂಕ		eof;
    off_T	ಸ್ಥಾನಾಂತರ;

    // For binary search: compute the next offset to use.
    ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ)
    {
	ಸ್ಥಾನಾಂತರ = sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ + ((sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ
						- sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ) / 2);
	ಆದರೆ (ಸ್ಥಾನಾಂತರ == sinfo_p->curr_ಸ್ಥಾನಾಂತರ)
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗುಗಳು_READ_EOF; // End the binary search without a match.
	ಇಲ್ಲದಿದ್ದರೆ
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;
    }

    // Skipping back (after a match during binary search).
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->state == TS_SKIP_ಹಿಂಭಾಗ)
    {
	sinfo_p->curr_ಸ್ಥಾನಾಂತರ -= st->lbuf_ಗಾತ್ರ * 2;
	ಆದರೆ (sinfo_p->curr_ಸ್ಥಾನಾಂತರ < 0)
	{
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = 0;
	    rewind(st->fp);
	    st->state = TS_ಹೆಜ್ಜೆ_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ || st->state == TS_SKIP_ಹಿಂಭಾಗ)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_ಸ್ಥಾನಾಂತರ_ಬಳಸಿದ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_ಸ್ಥಾನಾಂತರ, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_ಗಾತ್ರ, st->fp);
	ಆದರೆ (!eof && sinfo_p->curr_ಸ್ಥಾನಾಂತರ != 0)
	{
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = vim_ftell(st->fp);
	    ಆದರೆ (sinfo_p->curr_ಸ್ಥಾನಾಂತರ == sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ, SEEK_SET);
		sinfo_p->curr_ಸ್ಥಾನಾಂತರ = sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_ಗಾತ್ರ, st->fp);
	}
	// skip empty and blank lines
	ಆಗ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_ಗಾತ್ರ, st->fp);
	}
	ಆದರೆ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_ಹಿಂಭಾಗ;
	    sinfo_p->ಹೊಂದಿಕೆ_ಸ್ಥಾನಾಂತರ = vim_ftell(st->fp);
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ_ಬಳಸಿದ;
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗುಗಳು_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// skip empty and blank lines
	ಮಾಡು
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	    ಆದರೆ (st->flags & ಟ್ಯಾಗ್_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_ಗಾತ್ರ);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		eof = vim_fgets(st->lbuf, st->lbuf_ಗಾತ್ರ, st->fp);
	} ಆಗ (!eof && vim_isblankline(st->lbuf));

	ಆದರೆ (eof)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	    ಆದರೆ (emacs_ಟ್ಯಾಗುಗಳು_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		ಹಿಂತಿರುಗಿ ಟ್ಯಾಗುಗಳು_READ_IGNORE;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗುಗಳು_READ_EOF;
	}
    }

    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗುಗಳು_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ಆದರೆ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	ಹಿಂತಿರುಗಿ TRUE;

    // Process the header line.
    ಆದರೆ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->ಟ್ಯಾಗ್_file_sorted = st->lbuf[18];
    ಆದರೆ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	ಗಾಗಿ (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_ಸಿದ್ಧತೆ(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_ಆರಂಭ_state_handler(
    findtags_state_T	*st,
    ಪೂರ್ಣಾಂಕ			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		use_cscope = (st->flags & ಟ್ಯಾಗ್_CSCOPE);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		noic = (st->flags & ಟ್ಯಾಗ್_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ಆದರೆ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	ಹಿಂತಿರುಗಿ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಆದರೆ (st->ರೇಖೀಯ || use_cscope)
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (st->ರೇಖೀಯ)
#ಆದರೆ_ಅಂತ್ಯ
	st->state = TS_ರೇಖೀಯ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->ಟ್ಯಾಗ್_file_sorted == NUL)
	st->state = TS_ದ್ವಿಮಾನ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->ಟ್ಯಾಗ್_file_sorted == '1')
	st->state = TS_ದ್ವಿಮಾನ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->ಟ್ಯಾಗ್_file_sorted == '2')
    {
	st->state = TS_ದ್ವಿಮಾನ;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	st->state = TS_ರೇಖೀಯ;

    ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->ರೇಖೀಯ = TRUE;
	st->state = TS_ರೇಖೀಯ;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ)
    {
	ಆದರೆ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_ರೇಖೀಯ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ = 0;
	    sinfo_p->ಕೆಳಗಿನ_char = 0;
	    sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ = filesize;
	    sinfo_p->curr_ಸ್ಥಾನಾಂತರ = 0;
	    sinfo_p->ಮೇಲಿನ_char = 0xff;
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
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
    ಸ್ಥಿರ tagmatch_ಸ್ಥಿತಿ_T
findtags_parse_ಸಾಲು(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		cmplen;
    ಪೂರ್ಣಾಂಕ		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ಆದರೆ (st->orgpat->headlen
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	    && !st->is_etag
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	CLEAR_ಕ್ಷೇತ್ರ(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_ಕೊನೆ = vim_strchr(st->lbuf, TAB);
	ಆದರೆ (tagpp->tagname_ಕೊನೆ == ಖಾಲಿ)
	    // Corrupted tag line.
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (ಪೂರ್ಣಾಂಕ)(tagpp->tagname_ಕೊನೆ - tagpp->tagname);
	ಆದರೆ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ಆದರೆ ((st->flags & ಟ್ಯಾಗ್_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->state == TS_ರೇಖೀಯ && st->orgpat->headlen != cmplen)
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;

	ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ)
	{
	    // Simplistic check for unsorted tags file.
	    i = (ಪೂರ್ಣಾಂಕ)tagpp->tagname[0];
	    ಆದರೆ (margs->sortic)
		i = (ಪೂರ್ಣಾಂಕ)TOUPPER_ASC(tagpp->tagname[0]);
	    ಆದರೆ (i < sinfo_p->ಕೆಳಗಿನ_char || i > sinfo_p->ಮೇಲಿನ_char)
		margs->ವಿಂಗಡಿಸು_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ಆದರೆ (margs->sortic)
		tagcmp = ಟ್ಯಾಗ್_strnicmp(tagpp->tagname, st->orgpat->ತಲೆ,
							(size_t)cmplen);
	    ಇಲ್ಲದಿದ್ದರೆ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->ತಲೆ, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ಆದರೆ (tagcmp == 0)
	    {
		ಆದರೆ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ಆದರೆ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_ಹಿಂಭಾಗ;
		sinfo_p->ಹೊಂದಿಕೆ_ಸ್ಥಾನಾಂತರ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ;
		ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;
	    }
	    ಆದರೆ (tagcmp < 0)
	    {
		sinfo_p->curr_ಸ್ಥಾನಾಂತರ = vim_ftell(st->fp);
		ಆದರೆ (sinfo_p->curr_ಸ್ಥಾನಾಂತರ < sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ)
		{
		    sinfo_p->ಕೆಳಗಿನ_ಸ್ಥಾನಾಂತರ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ;
		    ಆದರೆ (margs->sortic)
			sinfo_p->ಕೆಳಗಿನ_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ಇಲ್ಲದಿದ್ದರೆ
			sinfo_p->ಕೆಳಗಿನ_char = tagpp->tagname[0];
		    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;
		}
	    }
	    ಆದರೆ (tagcmp > 0 && sinfo_p->curr_ಸ್ಥಾನಾಂತರ != sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ)
	    {
		sinfo_p->ಮೇಲಿನ_ಸ್ಥಾನಾಂತರ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ;
		ಆದರೆ (margs->sortic)
		    sinfo_p->ಮೇಲಿನ_char = TOUPPER_ASC(tagpp->tagname[0]);
		ಇಲ್ಲದಿದ್ದರೆ
		    sinfo_p->ಮೇಲಿನ_char = tagpp->tagname[0];
		ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;
	    }

	    // No match yet and are at the end of the binary search.
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_STOP;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->state == TS_SKIP_ಹಿಂಭಾಗ)
	{
	    ಆದರೆ (MB_STRNICMP(tagpp->tagname, st->orgpat->ತಲೆ, cmplen) != 0)
		st->state = TS_ಹೆಜ್ಜೆ_FORWARD;
	    ಇಲ್ಲದಿದ್ದರೆ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_ಸ್ಥಾನಾಂತರ = sinfo_p->curr_ಸ್ಥಾನಾಂತರ_ಬಳಸಿದ;
	    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (st->state == TS_ಹೆಜ್ಜೆ_FORWARD)
	{
	    ಆದರೆ (MB_STRNICMP(tagpp->tagname, st->orgpat->ತಲೆ, cmplen) != 0)
	    {
		ಆದರೆ ((off_T)vim_ftell(st->fp) > sinfo_p->ಹೊಂದಿಕೆ_ಸ್ಥಾನಾಂತರ)
		    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_STOP;	// past last match
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;	// before first match
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // skip this match if it can't match
	    ಆದರೆ (MB_STRNICMP(tagpp->tagname, st->orgpat->ತಲೆ, cmplen) != 0)
		ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_ಕೊನೆ + 1;
	tagpp->fname_ಕೊನೆ = vim_strchr(tagpp->fname, TAB);
	ಆದರೆ (tagpp->fname_ಕೊನೆ == ಖಾಲಿ)
	    ಸ್ಥಿತಿ_2 = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tagpp->command = tagpp->fname_ಕೊನೆ + 1;
	    ಸ್ಥಿತಿ_2 = OK;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = parse_ಟ್ಯಾಗ್_ಸಾಲು(st->lbuf,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
		st->is_etag,
#ಆದರೆ_ಅಂತ್ಯ
		tagpp);

    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
	ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (st->is_etag)
	tagpp->fname = st->ebuf;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_matchargs_init(findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T *margs, ಪೂರ್ಣಾಂಕ flags)
{
    margs->matchoff = 0;			// match offset
    margs->ಹೊಂದಿಕೆ_re = FALSE;			// match with regexp
    margs->ಹೊಂದಿಕೆ_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & ಟ್ಯಾಗ್_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->ವಿಂಗಡಿಸು_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_ಹೊಂದಿಕೆ_ಟ್ಯಾಗ್(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T *margs)
{
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ = FALSE;
    ಪೂರ್ಣಾಂಕ		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (ಪೂರ್ಣಾಂಕ)(tagpp->tagname_ಕೊನೆ - tagpp->tagname);
    ಆದರೆ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ಆದರೆ (st->orgpat->len != cmplen)
	ಹೊಂದಿಕೆ = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (st->orgpat->regmatch.rm_ic)
	{
	    ಹೊಂದಿಕೆ =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ಆದರೆ (ಹೊಂದಿಕೆ)
		margs->ಹೊಂದಿಕೆ_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊಂದಿಕೆ = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->ಹೊಂದಿಕೆ_re = FALSE;
    ಆದರೆ (!ಹೊಂದಿಕೆ && st->orgpat->regmatch.regprog != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	cc;

	cc = *tagpp->tagname_ಕೊನೆ;
	*tagpp->tagname_ಕೊನೆ = NUL;
	ಹೊಂದಿಕೆ = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ಆದರೆ (ಹೊಂದಿಕೆ)
	{
	    margs->matchoff = (ಪೂರ್ಣಾಂಕ)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ಆದರೆ (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->ಹೊಂದಿಕೆ_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_ಕೊನೆ = cc;
	margs->ಹೊಂದಿಕೆ_re = TRUE;
    }

    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
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
    ಪೂರ್ಣಾಂಕ		len;

    conv_ಸಾಲು = string_convert(&st->vimconv, st->lbuf, ಖಾಲಿ);
    ಆದರೆ (conv_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Copy or swap lbuf and conv_line.
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(conv_ಸಾಲು) + 1;
    ಆದರೆ (len > st->lbuf_ಗಾತ್ರ)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_ಸಾಲು;
	st->lbuf_ಗಾತ್ರ = len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_ಕೂಡಿಸು_ಹೊಂದಿಕೆ(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		use_cscope = (st->flags & ಟ್ಯಾಗ್_CSCOPE);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		name_only = (st->flags & ಟ್ಯಾಗ್_ಹೆಸರುಗಳು);
    ಪೂರ್ಣಾಂಕ		mtt;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		is_current;		// file name matches
    ಪೂರ್ಣಾಂಕ		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಆದರೆ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
		st->is_etag,
#ಆದರೆ_ಅಂತ್ಯ
		tagpp->fname, tagpp->fname_ಕೊನೆ, st->ಟ್ಯಾಗ್_fname, buf_ffname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	is_static = FALSE;
	ಆದರೆ (!st->is_etag)	// emacs tags are never static
#ಆದರೆ_ಅಂತ್ಯ
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	//ಹೊಂದಿಕೆ
	ಆದರೆ (is_static)
	{
	    ಆದರೆ (is_current)
		mtt = MT_ST_CUR;
	    ಇಲ್ಲದಿದ್ದರೆ
		mtt = MT_ST_OTH;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (is_current)
		mtt = MT_GL_CUR;
	    ಇಲ್ಲದಿದ್ದರೆ
		mtt = MT_GL_OTH;
	}
	ಆದರೆ (st->orgpat->regmatch.rm_ic && !margs->ಹೊಂದಿಕೆ_no_ic)
	    mtt += MT_IC_OFF;
	ಆದರೆ (margs->ಹೊಂದಿಕೆ_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ಆದರೆ (st->help_only)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
# define ML_EXTRA 3
#ಇಲ್ಲದಿದ್ದರೆ
# define ML_EXTRA 0
#ಆದರೆ_ಅಂತ್ಯ
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_ಕೊನೆ = NUL;
	len = (ಪೂರ್ಣಾಂಕ)(tagpp->tagname_ಕೊನೆ - tagpp->tagname);
	mfp = alloc(ಗಾತ್ರ(char_u) + len + 10 + ML_EXTRA + 1);
	ಆದರೆ (mfp != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#ಆದರೆ_ಅಂತ್ಯ

	    heuristic = help_heuristic(tagpp->tagname,
				margs->ಹೊಂದಿಕೆ_re ? margs->matchoff : 0,
				!margs->ಹೊಂದಿಕೆ_no_ic);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#ಆದರೆ_ಅಂತ್ಯ
	    sprintf((ಅಕ್ಷರ *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_ಕೊನೆ = TAB;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_only)
    {
	ಆದರೆ (st->get_searchpat)
	{
	    char_u *ತಾತ್ಕಾಲಿಕ_ಕೊನೆ = tagpp->command;

	    ಆದರೆ (*ತಾತ್ಕಾಲಿಕ_ಕೊನೆ == '/')
		ಆಗ (*ತಾತ್ಕಾಲಿಕ_ಕೊನೆ && *ತಾತ್ಕಾಲಿಕ_ಕೊನೆ != '\r'
			&& *ತಾತ್ಕಾಲಿಕ_ಕೊನೆ != '\n'
			&& *ತಾತ್ಕಾಲಿಕ_ಕೊನೆ != '$')
		    ತಾತ್ಕಾಲಿಕ_ಕೊನೆ++;

	    ಆದರೆ (tagpp->command + 2 < ತಾತ್ಕಾಲಿಕ_ಕೊನೆ)
	    {
		len = (ಪೂರ್ಣಾಂಕ)(ತಾತ್ಕಾಲಿಕ_ಕೊನೆ - tagpp->command - 2);
		mfp = alloc(len + 2);
		ಆದರೆ (mfp != ಖಾಲಿ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mfp = ಖಾಲಿ;
	    st->get_searchpat = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len = (ಪೂರ್ಣಾಂಕ)(tagpp->tagname_ಕೊನೆ - tagpp->tagname);
	    mfp = alloc(ಗಾತ್ರ(char_u) + len + 1);
	    ಆದರೆ (mfp != ಖಾಲಿ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
		st->get_searchpat = p_sft;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t ಟ್ಯಾಗ್_fname_len = STRLEN(st->ಟ್ಯಾಗ್_fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	size_t ebuf_len = 0;
#ಆದರೆ_ಅಂತ್ಯ

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (ಪೂರ್ಣಾಂಕ)ಟ್ಯಾಗ್_fname_len + (ಪೂರ್ಣಾಂಕ)STRLEN(st->lbuf) + 3;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	ಆದರೆ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (ಪೂರ್ಣಾಂಕ)ebuf_len + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ++len;
#ಆದರೆ_ಅಂತ್ಯ
	mfp = alloc(ಗಾತ್ರ(char_u) + len + 1);
	ಆದರೆ (mfp != ಖಾಲಿ)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->ಟ್ಯಾಗ್_fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#ಆದರೆ_ಅಂತ್ಯ
	    p[ಟ್ಯಾಗ್_fname_len + 1] = ಟ್ಯಾಗ್_SEP;
	    s = p + 1 + ಟ್ಯಾಗ್_fname_len + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	    ಆದರೆ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = ಟ್ಯಾಗ್_SEP;
		s += ebuf_len + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*s++ = ಟ್ಯಾಗ್_SEP;
#ಆದರೆ_ಅಂತ್ಯ
	    STRCPY(s, st->lbuf);
	}
    }

    ಆದರೆ (mfp != ಖಾಲಿ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	ಆದರೆ (use_cscope)
	    ++*hash;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_ಹೊಂದಿಕೆ[mtt], mfp, *hash);
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ಆದರೆ (hash_ಕೂಡಿಸು_ಅಂಶ(&st->ht_ಹೊಂದಿಕೆ[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_ಹೊಂದಿಕೆ[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ((char_u **)(st->ga_ಹೊಂದಿಕೆ[mtt].ga_ದತ್ತಾಂಶ))
		[st->ga_ಹೊಂದಿಕೆ[mtt].ga_len++] = mfp;
	    st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findtags_get_all_ಟ್ಯಾಗುಗಳು(
    findtags_state_T		*st,
    findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	ಹುಡುಕು_info;
    ಪೂರ್ಣಾಂಕ			retval;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ			use_cscope = (st->flags & ಟ್ಯಾಗ್_CSCOPE);
#ಆದರೆ_ಅಂತ್ಯ
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_ಕ್ಷೇತ್ರ(ಹುಡುಕು_info);

    // Read and parse the lines in the file one by one
    ಗಾಗಿ (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ಆದರೆ (st->state == TS_ದ್ವಿಮಾನ || st->state == TS_SKIP_ಹಿಂಭಾಗ)
	    ಸಾಲು_breakcheck();
	ಇಲ್ಲದಿದ್ದರೆ
	    fast_breakcheck();
	ಆದರೆ ((st->flags & ಟ್ಯಾಗ್_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_ಕೀಲಿಗಳು(30, FALSE);
	ಆದರೆ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ಆದರೆ (st->mincount == ಟ್ಯಾಗ್_MANY && st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ >= ಟ್ಯಾಗ್_MANY)
	{
	    st->stop_searching = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (st->get_searchpat)
	    ಹೋಗು ಸಾಲು_read_ಒಳ;

	retval = findtags_get_ಮುಂದಿನ_ಸಾಲು(st, &ಹುಡುಕು_info);
	ಆದರೆ (retval == ಟ್ಯಾಗುಗಳು_READ_IGNORE)
	    ಮುಂದುವರಿ;
	ಆದರೆ (retval == ಟ್ಯಾಗುಗಳು_READ_EOF)
	    ನಿಲ್ಲಿಸು;

ಸಾಲು_read_ಒಳ:

	ಆದರೆ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ಆದರೆ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_ರೇಖೀಯ;
	    emacs_ಟ್ಯಾಗುಗಳು_new_filename(st);
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ಆದರೆ (st->state == TS_ಆರಂಭ)
	{
	    ಆದರೆ (findtags_ಆರಂಭ_state_handler(st, &margs->sortic, &ಹುಡುಕು_info) == FALSE)
		ಮುಂದುವರಿ;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ಆದರೆ (st->lbuf[st->lbuf_ಗಾತ್ರ - 2] != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		&& !use_cscope
#ಆದರೆ_ಅಂತ್ಯ
	   )
	{
	    st->lbuf_ಗಾತ್ರ *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_ಗಾತ್ರ);
	    ಆದರೆ (st->lbuf == ಖಾಲಿ)
	    {
		ಆದರೆ (st->fp != ಖಾಲಿ)
		    fclose(st->fp);
		st->fp = ಖಾಲಿ;
		st->stop_searching = TRUE;
		ಹಿಂತಿರುಗಿ;
	    }

	    ಆದರೆ (st->state == TS_ಹೆಜ್ಜೆ_FORWARD || st->state == TS_ರೇಖೀಯ)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, ಹುಡುಕು_info.curr_ಸ್ಥಾನಾಂತರ,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    ಹುಡುಕು_info.curr_ಸ್ಥಾನಾಂತರ = 0;
	    ಮುಂದುವರಿ;
	}

	retval = findtags_parse_ಸಾಲು(st, &tagp, margs, &ಹುಡುಕು_info);
	ಆದರೆ (retval == ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_ಮುಂದಿನ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (retval == ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_STOP)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (retval == ಟ್ಯಾಗ್_ಹೊಂದಿಕೆ_FAIL)
	{
	    semsg(_(e_format_error_ಒಳ_ಟ್ಯಾಗುಗಳು_file_str), st->ಟ್ಯಾಗ್_fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	    ಆದರೆ (!use_cscope)
#ಆದರೆ_ಅಂತ್ಯ
		semsg(_("Before byte %ld"), (ದೀರ್ಘ)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    ಹಿಂತಿರುಗಿ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ಆದರೆ (findtags_ಹೊಂದಿಕೆ_ಟ್ಯಾಗ್(st, &tagp, margs))
	{
	    ಆದರೆ (findtags_ಕೂಡಿಸು_ಹೊಂದಿಕೆ(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ನಿಲ್ಲಿಸು;
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
findtags_ಒಳ_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_ಹೊಂದಿಕೆ_ವಾದಗಳು_T margs;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		use_cscope = (st->flags & ಟ್ಯಾಗ್_CSCOPE);
#ಆದರೆ_ಅಂತ್ಯ

    st->vimconv.vc_type = CONV_NONE;
    st->ಟ್ಯಾಗ್_file_sorted = NUL;
    st->fp = ಖಾಲಿ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಆದರೆ (use_cscope)
	st->fp = ಖಾಲಿ;	    // avoid GCC warning
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	ಆದರೆ (curbuf->b_help)
	{
	    ಆದರೆ (!findtags_ಒಳ_help_init(st))
		ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	st->fp = mch_fopen((ಅಕ್ಷರ *)st->ಟ್ಯಾಗ್_fname, "r");
	ಆದರೆ (st->fp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->ಟ್ಯಾಗ್_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_ಆರಂಭ;	// we're at the start of the file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    st->is_etag = FALSE;	// default is: not emacs style
#ಆದರೆ_ಅಂತ್ಯ

    // Read and parse the lines in the file one by one
    findtags_get_all_ಟ್ಯಾಗುಗಳು(st, &margs, buf_ffname);

    ಆದರೆ (st->fp != ಖಾಲಿ)
    {
	fclose(st->fp);
	st->fp = ಖಾಲಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    emacs_ಟ್ಯಾಗುಗಳು_incstack_free();
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (st->vimconv.vc_type != CONV_NONE)
	convert_ಸಿದ್ಧತೆ(&st->vimconv, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (margs.ವಿಂಗಡಿಸು_error)
	semsg(_(e_ಟ್ಯಾಗುಗಳು_file_not_sorted_str), st->ಟ್ಯಾಗ್_fname);

    // Stop searching if sufficient tags have been found.
    ಆದರೆ (st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    ಪೂರ್ಣಾಂಕ		name_only = (st->flags & ಟ್ಯಾಗ್_ಹೆಸರುಗಳು);
    char_u	**matches;
    ಪೂರ್ಣಾಂಕ		mtt;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*mfp;
    char_u	*p;

    ಆದರೆ (st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ > 0)
	matches = ALLOC_MULT(char_u *, st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ);
    ಇಲ್ಲದಿದ್ದರೆ
	matches = ಖಾಲಿ;
    st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ = 0;
    ಗಾಗಿ (mtt = 0; mtt < MT_ಎಣಿಕೆ; ++mtt)
    {
	ಗಾಗಿ (i = 0; i < st->ga_ಹೊಂದಿಕೆ[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_ಹೊಂದಿಕೆ[mtt].ga_ದತ್ತಾಂಶ))[i];
	    ಆದರೆ (matches == ಖಾಲಿ)
		vim_free(mfp);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    ಗಾಗಿ (p = mfp + 1; *p != NUL; ++p)
			ಆದರೆ (*p == ಟ್ಯಾಗ್_SEP)
			    *p = NUL;
		}
		matches[st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ++] = mfp;
	    }
	}

	ga_clear(&st->ga_ಹೊಂದಿಕೆ[mtt]);
	hash_clear(&st->ht_ಹೊಂದಿಕೆ[mtt]);
    }

    *matchesp = matches;
    ಹಿಂತಿರುಗಿ st->ಹೊಂದಿಕೆ_ಎಣಿಕೆ;
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
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(
    char_u	*pat,			// pattern to search for
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_file;		// trying first tag file
    ಪೂರ್ಣಾಂಕ		retval = FAIL;		// return value
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    ಪೂರ್ಣಾಂಕ		save_emsg_off;

    bool	help_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*saved_pat = ಖಾಲಿ;		// copy of pat[]
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		findall = (mincount == MAXCOL || mincount == ಟ್ಯಾಗ್_MANY);
						// find all matching tags
    ಪೂರ್ಣಾಂಕ		has_re = (flags & ಟ್ಯಾಗ್_REGEXP);	// regexp used
    ಪೂರ್ಣಾಂಕ		noic = (flags & ಟ್ಯಾಗ್_NOIC);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಪೂರ್ಣಾಂಕ		use_cscope = (flags & ಟ್ಯಾಗ್_CSCOPE);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		verbose = (flags & ಟ್ಯಾಗ್_VERBOSE);
    ಪೂರ್ಣಾಂಕ		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    ಆಯ್ಕೆ (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	ಸ್ಥಿತಿ TC_FOLLOWIC:		 ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TC_IGNORE:    p_ic = TRUE;  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TC_ಹೊಂದಿಕೆ:     p_ic = FALSE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TC_FOLLOWSCS: p_ic = ignorecase(pat); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ನಿಲ್ಲಿಸು;
    }

    help_save = curbuf->b_help;

    ಆದರೆ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	ಹೋಗು findtag_ಕೊನೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    STRCPY(st.ಟ್ಯಾಗ್_fname, "from cscope");	// for error messages
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Initialize a few variables
     */
    ಆದರೆ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಆದರೆ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ಆದರೆ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ಆದರೆ (saved_pat != ಖಾಲಿ)
	    {
		st.help_lang_ಕಂಡುಹಿಡಿ = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ಆದರೆ (has_re && st.orgpat->regmatch.regprog == ಖಾಲಿ)
	ಹೋಗು findtag_ಕೊನೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ಆದರೆ (retval != NOTDONE)
	ಹೋಗು findtag_ಕೊನೆ;

    // re-initialize the default return value
    retval = FAIL;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ಆದರೆ ((flags & ಟ್ಯಾಗ್_KEEP_LANG)
	    && st.help_lang_ಕಂಡುಹಿಡಿ == ಖಾಲಿ
	    && curbuf->b_fname != ಖಾಲಿ
	    && (i = (ಪೂರ್ಣಾಂಕ)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

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
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	st.ರೇಖೀಯ = (st.orgpat->headlen == 0 || !p_tbs || ಸುತ್ತು == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      ಗಾಗಿ (ಮೊದಲ_file = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	    use_cscope ||
#ಆದರೆ_ಅಂತ್ಯ
		get_tagfname(&tn, ಮೊದಲ_file, st.ಟ್ಯಾಗ್_fname) == OK;
							   ಮೊದಲ_file = FALSE)
      {
	  findtags_ಒಳ_file(&st, buf_ffname);
	  ಆದರೆ (st.stop_searching
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		  || use_cscope
#ಆದರೆ_ಅಂತ್ಯ
	     )
	  {
	      retval = OK;
	      ನಿಲ್ಲಿಸು;
	  }
      } // end of for-each-file loop

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	ಆದರೆ (!use_cscope)
#ಆದರೆ_ಅಂತ್ಯ
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ಆದರೆ (st.stop_searching || st.ರೇಖೀಯ || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
	ಆದರೆ (use_cscope)
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ಆದರೆ (!st.stop_searching)
    {
	ಆದರೆ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_ಟ್ಯಾಗುಗಳು_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_ಕೊನೆ:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ಆದರೆ (retval == FAIL)
	st.ಹೊಂದಿಕೆ_ಎಣಿಕೆ = 0;

    *ಅಂಕೆ_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    vim_free(saved_pat);
#ಆದರೆ_ಅಂತ್ಯ

    p_ic = save_p_ic;

    ಹಿಂತಿರುಗಿ retval;
}

ಸ್ಥಿರ garray_T ಟ್ಯಾಗ್_fnames = GA_ಖಾಲಿಗಳು;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿಕ್ಕಿತು_tagfile_cb(char_u *fname, ಶೂನ್ಯ *cookie ಬಳಸದ)
{
    ಆದರೆ (ga_grow(&ಟ್ಯಾಗ್_fnames, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    char_u	*ಟ್ಯಾಗ್_fname = vim_strsave(fname);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    slash_adjust(ಟ್ಯಾಗ್_fname);
#ಆದರೆ_ಅಂತ್ಯ
    simplify_filename(ಟ್ಯಾಗ್_fname);
    ((char_u **)(ಟ್ಯಾಗ್_fnames.ga_ದತ್ತಾಂಶ))[ಟ್ಯಾಗ್_fnames.ga_len++] = ಟ್ಯಾಗ್_fname;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_ಟ್ಯಾಗ್_stuff(ಶೂನ್ಯ)
{
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಟ್ಯಾಗ್_fnames);
    ಆದರೆ (curwin != ಖಾಲಿ)
	do_ಟ್ಯಾಗ್(ಖಾಲಿ, DT_FREE, 0, 0, 0);
    ಟ್ಯಾಗ್_freematch();

# ಆದರೆ defined(FEAT_QUICKFIX)
    tagstack_clear_ನಮೂದು(&ptag_ನಮೂದು);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
get_tagfname(
    tagname_T	*tnp,	// holds status info
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ಖಾಲಿ;
    char_u		*r_ptr;
    ಪೂರ್ಣಾಂಕ			i;

    ಆದರೆ (ಮೊದಲ)
	CLEAR_ಸೂಚಕ(tnp);

    ಆದರೆ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ಆದರೆ (ಮೊದಲ)
	{
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಟ್ಯಾಗ್_fnames);
	    ga_init2(&ಟ್ಯಾಗ್_fnames, ಗಾತ್ರ(char_u *), 10);
	    do_ಒಳ_runtimepath((char_u *)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ಇಲ್ಲದಿದ್ದರೆ
		    "doc/tags doc/tags-??"
# endif
#ಇಲ್ಲದಿದ್ದರೆ
		    "doc/tags"
#ಆದರೆ_ಅಂತ್ಯ
					   , DIP_ALL, ಸಿಕ್ಕಿತು_tagfile_cb, ಖಾಲಿ);
	}

	ಆದರೆ (tnp->tn_hf_idx >= ಟ್ಯಾಗ್_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ಆದರೆ (tnp->tn_hf_idx > ಟ್ಯಾಗ್_fnames.ga_len || *p_hf == NUL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    slash_adjust(buf);
#ಆದರೆ_ಅಂತ್ಯ
	    simplify_filename(buf);

	    ಗಾಗಿ (i = 0; i < ಟ್ಯಾಗ್_fnames.ga_len; ++i)
		ಆದರೆ (STRCMP(buf, ((char_u **)(ಟ್ಯಾಗ್_fnames.ga_ದತ್ತಾಂಶ))[i]) == 0)
		    ಹಿಂತಿರುಗಿ FAIL; // avoid duplicate file names
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_strncpy(buf, ((char_u **)(ಟ್ಯಾಗ್_fnames.ga_ದತ್ತಾಂಶ))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (ಮೊದಲ)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_ಟ್ಯಾಗುಗಳು = vim_strsave((*curbuf->b_p_ಟ್ಯಾಗುಗಳು != NUL)
						 ? curbuf->b_p_ಟ್ಯಾಗುಗಳು : p_ಟ್ಯಾಗುಗಳು);
	ಆದರೆ (tnp->tn_ಟ್ಯಾಗುಗಳು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	tnp->tn_np = tnp->tn_ಟ್ಯಾಗುಗಳು;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_ಹುಡುಕು_ctx);
	    ಆದರೆ (fname != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *filename = ಖಾಲಿ;

	    // Stop when used all parts of 'tags'.
	    ಆದರೆ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_ಹುಡುಕು_ctx);
		tnp->tn_ಹುಡುಕು_ctx = ಖಾಲಿ;
		ಹಿಂತಿರುಗಿ FAIL;
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
	    ಆದರೆ (r_ptr != ಖಾಲಿ)
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
	    ಆದರೆ (tnp->tn_ಹುಡುಕು_ctx != ಖಾಲಿ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    ಶೂನ್ಯ
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_ಟ್ಯಾಗುಗಳು);
    vim_findfile_cleanup(tnp->tn_ಹುಡುಕು_ctx);
    tnp->tn_ಹುಡುಕು_ctx = ಖಾಲಿ;
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಟ್ಯಾಗ್_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_ಟ್ಯಾಗ್_ಸಾಲು(
    char_u	*lbuf,		// line to be parsed
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಪೂರ್ಣಾಂಕ		is_etag,
#ಆದರೆ_ಅಂತ್ಯ
    tagptrs_T	*tagp)
{
    char_u	*p;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (is_etag)
	// emacs-style tag file
	ಹಿಂತಿರುಗಿ emacs_ಟ್ಯಾಗುಗಳು_parse_ಸಾಲು(lbuf, tagp);
#ಆದರೆ_ಅಂತ್ಯ

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    tagp->tagname_ಕೊನೆ = p;

    // Isolate file name, from first to second white space
    ಆದರೆ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    tagp->fname_ಕೊನೆ = p;

    // find start of search command, after second white space
    ಆದರೆ (*p != NUL)
	++p;
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ FAIL;
    tagp->command = p;

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ಆಗ ((p = vim_strchr(p, '\t')) != ಖಾಲಿ)
    {
	++p;
	ಆದರೆ (STRNCMP(p, "file:", 5) == 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    p += STRLEN(p) + 1;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (p - lbuf) + STRLEN(p);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_ಹೊಂದಿಕೆ(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    ಪೂರ್ಣಾಂಕ		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->ಟ್ಯಾಗ್_fname = lbuf + 1;
    lbuf += STRLEN(tagp->ಟ್ಯಾಗ್_fname) + 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_ಕೊನೆ = lbuf++;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Find search pattern and the file name for non-etags.
    retval = parse_ಟ್ಯಾಗ್_ಸಾಲು(lbuf,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
			tagp->is_etag,
#ಆದರೆ_ಅಂತ್ಯ
			tagp);

    tagp->tagkind = ಖಾಲಿ;
    tagp->user_ದತ್ತಾಂಶ = ಖಾಲಿ;
    tagp->tagline = 0;
    tagp->command_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ (retval != OK)
	ಹಿಂತಿರುಗಿ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_extra(&p) == OK)
    {
	ಆದರೆ (p > tagp->command && p[-1] == '|')
	    tagp->command_ಕೊನೆ = p - 1;  // drop trailing bar
	ಇಲ್ಲದಿದ್ದರೆ
	    tagp->command_ಕೊನೆ = p;
	p += 2;	// skip ";\""
	ಆದರೆ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ಆಗ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ಆದರೆ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_ದತ್ತಾಂಶ = p + 10;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((ಅಕ್ಷರ *)p + 5);
		ಆದರೆ (tagp->tagkind != ಖಾಲಿ && tagp->user_ದತ್ತಾಂಶ != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ಆದರೆ (pc == ಖಾಲಿ || (pt != ಖಾಲಿ && pc > pt))
		    tagp->tagkind = p;
		ಆದರೆ (pt == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ಆದರೆ (tagp->tagkind != ಖಾಲಿ)
    {
	ಗಾಗಿ (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_ಕೊನೆ = p;
    }
    ಆದರೆ (tagp->user_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	ಗಾಗಿ (p = tagp->user_ದತ್ತಾಂಶ;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_ದತ್ತಾಂಶ_ಕೊನೆ = p;
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    ಸ್ಥಿರ char_u *
ಟ್ಯಾಗ್_ತುಂಬಿದ_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    ಪೂರ್ಣಾಂಕ		c;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	c = *tagp->fname_ಕೊನೆ;
	*tagp->fname_ಕೊನೆ = NUL;
    }
    fullname = expand_ಟ್ಯಾಗ್_fname(tagp->fname, tagp->ಟ್ಯಾಗ್_fname, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (!tagp->is_etag)
#ಆದರೆ_ಅಂತ್ಯ
	*tagp->fname_ಕೊನೆ = c;

    ಹಿಂತಿರುಗಿ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
jumpto_ಟ್ಯಾಗ್(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    ಪೂರ್ಣಾಂಕ		forceit,	// :ta with !
    ಪೂರ್ಣಾಂಕ		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    ಪೂರ್ಣಾಂಕ		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_ಕೊನೆ;
    char_u	*tofree_fname = ಖಾಲಿ;
    char_u	*fname;
    tagptrs_T	tagp;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ಪೂರ್ಣಾಂಕ		getfile_ಫಲಿತಾಂಶ = GETFILE_ಬಳಸದ;
    ಪೂರ್ಣಾಂಕ		ಹುಡುಕು_options;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    ಪೂರ್ಣಾಂಕ		save_no_hlsearch;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*ತುಂಬಿದ_fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;    // getting the file may reset it
#ಆದರೆ_ಅಂತ್ಯ
    size_t	len;
    char_u	*lbuf;
    ಪೂರ್ಣಾಂಕ		ಅಂಕಿಯೇ = FALSE;

    ಆದರೆ (postponed_ವಿಭಜಿಸು == 0 && !check_can_ಗಣ_curbuf_forceit(forceit))
	ಹಿಂತಿರುಗಿ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_ಸಾಲು_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ಆದರೆ (lbuf != ಖಾಲಿ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ಆದರೆ (pbuf == ಖಾಲಿ || lbuf == ಖಾಲಿ || parse_ಹೊಂದಿಕೆ(lbuf, &tagp) == FAIL)
    {
	tagp.fname_ಕೊನೆ = ಖಾಲಿ;
	ಹೋಗು erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_ಕೊನೆ = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ಆದರೆ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	ಅಂಕಿಯೇ = TRUE;
	pbuf[0] = ':';
    }
    ಗಾಗಿ (pbuf_ಕೊನೆ = pbuf + ಅಂಕಿಯೇ;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	ಆದರೆ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	*pbuf_ಕೊನೆ++ = *str++;
	ಆದರೆ (pbuf_ಕೊನೆ - pbuf + 1 + ಅಂಕಿಯೇ >= LSIZE)
	    ನಿಲ್ಲಿಸು;
    }
    *pbuf_ಕೊನೆ = NUL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಆದರೆ (!tagp.is_etag)
#ಆದರೆ_ಅಂತ್ಯ
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ಆದರೆ (ಅಂಕಿಯೇ)
	    str++;
	ಆದರೆ (ಕಂಡುಹಿಡಿ_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ಆದರೆ (str > pbuf && str[-1] == '|')
		pbuf_ಕೊನೆ = str - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		pbuf_ಕೊನೆ = str;
	    *pbuf_ಕೊನೆ = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_ಟ್ಯಾಗ್_fname(fname, tagp.ಟ್ಯಾಗ್_fname, TRUE);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹೋಗು erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ಆದರೆ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ಖಾಲಿ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ಆದರೆ (nofile_fname == ಖಾಲಿ)
	    nofile_fname = ಖಾಲಿಗಳು_option;
	ಹೋಗು erret;
    }

    ++RedrawingDisabled;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_QUICKFIX)
    ಆದರೆ (g_do_tagpreview != 0)
    {
	postponed_ವಿಭಜಿಸು = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ಆದರೆ (!curwin->w_p_pvw)
	{
	    ತುಂಬಿದ_fname = ತುಂಬಿದ_Name_save(fname, FALSE);
	    fname = ತುಂಬಿದ_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    ಆದರೆ (postponed_ವಿಭಜಿಸು && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ಆದರೆ (existing_buf != ಖಾಲಿ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ಆದರೆ (swbuf_goto_win_with_buf(existing_buf) != ಖಾಲಿ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_ಫಲಿತಾಂಶ = GETFILE_SAME_FILE;
	}
    }
    ಆದರೆ (getfile_ಫಲಿತಾಂಶ == GETFILE_ಬಳಸದ
				  && (postponed_ವಿಭಜಿಸು || cmdmod.cmod_tab != 0))
    {
	ಆದರೆ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_ವಿಭಜಿಸು |= WSP_VERT;

	ಆದರೆ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_ಸೂಚ್ಯಂಕ(curtab) + 1;

	ಆದರೆ (win_ವಿಭಜಿಸು(postponed_ವಿಭಜಿಸು > 0 ? postponed_ವಿಭಜಿಸು : 0,
						postponed_ವಿಭಜಿಸು_flags) == FAIL)
	{
	    ಆದರೆ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    ಹೋಗು erret;
	}
	RESET_BINDING(curwin);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ಆದರೆ defined(FEAT_QUICKFIX)
	ಆದರೆ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_ಬಫರ್);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    keep_help_flag = curbuf->b_help;
    }

    ಆದರೆ (getfile_ಫಲಿತಾಂಶ == GETFILE_ಬಳಸದ)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_ಫಲಿತಾಂಶ = getfile(0, fname, ಖಾಲಿ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ಆದರೆ (GETFILE_SUCCESS(getfile_ಫಲಿತಾಂಶ))	// got to the right file
    {
	curwin->w_ಗಣ_curswant = true;
	postponed_ವಿಭಜಿಸು = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ಆದರೆ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ಆದರೆ (vim_strchr(p_cpo, CPO_TAGPAT) != ಖಾಲಿ)
	    ಹುಡುಕು_options = 0;
	ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ಆದರೆ (str > pbuf_ಕೊನೆ - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_ಕೊನೆ - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ಆದರೆ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ಆದರೆ (do_ಹುಡುಕು(ಖಾಲಿ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ದೀರ್ಘ)1,
							 ಹುಡುಕು_options, ಖಾಲಿ))
		retval = OK;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು = 1;
		ಪೂರ್ಣಾಂಕ	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ಆದರೆ (!do_ಹುಡುಕು(ಖಾಲಿ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (ದೀರ್ಘ)1,
							 ಹುಡುಕು_options, ಖಾಲಿ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    ಸಿಕ್ಕಿತು = 2;
		    (ಶೂನ್ಯ)test_for_static(&tagp);
		    cc = *tagp.tagname_ಕೊನೆ;
		    *tagp.tagname_ಕೊನೆ = NUL;
		    pbuflen = vim_snprintf((ಅಕ್ಷರ *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ಆದರೆ (!do_ಹುಡುಕು(ಖಾಲಿ, '/', '/', pbuf, pbuflen, (ದೀರ್ಘ)1,
							 ಹುಡುಕು_options, ಖಾಲಿ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((ಅಕ್ಷರ *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ಆದರೆ (!do_ಹುಡುಕು(ಖಾಲಿ, '/', '/', pbuf, pbuflen, (ದೀರ್ಘ)1,
							 ಹುಡುಕು_options, ಖಾಲಿ))
			    ಸಿಕ್ಕಿತು = 0;
		    }
		    *tagp.tagname_ಕೊನೆ = cc;
		}
		ಆದರೆ (ಸಿಕ್ಕಿತು == 0)
		{
		    emsg(_(e_cannot_ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_ನಮೂನೆ));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ಆದರೆ (ಸಿಕ್ಕಿತು == 2 || !save_p_ic)
		    {
			ಸಂದೇಶ(_(e_couldnt_ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_just_guessing));
			ಆದರೆ (!ಸಂದೇಶ_scrolled && ಸಂದೇಶ_silent == 0)
			{
			    ಹೊರಹೋಗು_flush();
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
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	    ++sandbox;
#ಆದರೆ_ಅಂತ್ಯ
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    ಆದರೆ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	    --sandbox;
#ಆದರೆ_ಅಂತ್ಯ
	}

	magic_overruled = save_magic_overruled;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ಆದರೆ (ಹುಡುಕು_options)
	    ಗಣ_no_hlsearch(save_no_hlsearch);
#ಆದರೆ_ಅಂತ್ಯ

	// Return OK if jumped to another file (at least we found the file!).
	ಆದರೆ (getfile_ಫಲಿತಾಂಶ == GETFILE_OPEN_OTHER)
	    retval = OK;

	ಆದರೆ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ಆದರೆ (curbuf->b_help)
		ಗಣ_topline(curwin, curwin->w_cursor.lnum);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ ((fdo_flags & FDO_ಟ್ಯಾಗ್) && old_ಕೀಲಿ_Typed)
		foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
	}

#ಆದರೆ defined(FEAT_QUICKFIX)
	ಆದರೆ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ಆದರೆ (postponed_ವಿಭಜಿಸು)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_ವಿಭಜಿಸು = 0;
	}
#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ಆದರೆ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
#ಆದರೆ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ಆದರೆ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#ಆದರೆ_ಅಂತ್ಯ

erret:
#ಆದರೆ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(ತುಂಬಿದ_fname);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    ಸ್ಥಿರ char_u *
expand_ಟ್ಯಾಗ್_fname(char_u *fname, char_u *ಟ್ಯಾಗ್_fname, ಪೂರ್ಣಾಂಕ expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ಖಾಲಿ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ಆದರೆ (p_tagsecure && ಮಾರ್ಗ_with_url(fname))
    {
       emsg(_(e_ಟ್ಯಾಗ್_file_ನಮೂದು_must_not_be_url));
       ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ಆದರೆ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ಖಾಲಿ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ಖಾಲಿ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ಆದರೆ (expanded_fname != ಖಾಲಿ)
	    fname = expanded_fname;
    }

    ಆದರೆ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(ಟ್ಯಾಗ್_fname)) != ಟ್ಯಾಗ್_fname)
    {
	retval = alloc(MAXPATHL);
	ಆದರೆ (retval != ಖಾಲಿ)
	{
	    STRCPY(retval, ಟ್ಯಾಗ್_fname);
	    vim_strncpy(retval + (p - ಟ್ಯಾಗ್_fname), fname,
					      MAXPATHL - (p - ಟ್ಯಾಗ್_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_for_current(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
    ಪೂರ್ಣಾಂಕ	    is_etag,
#ಆದರೆ_ಅಂತ್ಯ
    char_u  *fname,
    char_u  *fname_ಕೊನೆ,
    char_u  *ಟ್ಯಾಗ್_fname,
    char_u  *buf_ffname)
{
    ಪೂರ್ಣಾಂಕ	    c;
    ಪೂರ್ಣಾಂಕ	    retval = FALSE;
    char_u  *fullname;

    ಆದರೆ (buf_ffname != ಖಾಲಿ)	// if the buffer has a name
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	ಆದರೆ (is_etag)
	    c = 0;	    // to shut up GCC
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    c = *fname_ಕೊನೆ;
	    *fname_ಕೊನೆ = NUL;
	}
	fullname = expand_ಟ್ಯಾಗ್_fname(fname, ಟ್ಯಾಗ್_fname, TRUE);
	ಆದರೆ (fullname != ಖಾಲಿ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
	ಆದರೆ (!is_etag)
#ಆದರೆ_ಅಂತ್ಯ
	    *fname_ಕೊನೆ = c;
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	ಮೊದಲ_char = **pp;

    // Repeat for addresses separated with ';'
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ಆದರೆ (*str != ಮೊದಲ_char)
		str = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		++str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)str, "|;\"");
	    ಆದರೆ (str != ಖಾಲಿ)
	    {
		++str;
		ನಿಲ್ಲಿಸು;
	    }

	}
	ಆದರೆ (str == ಖಾಲಿ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ನಿಲ್ಲಿಸು;
	++str;	// skip ';'
	ಮೊದಲ_char = *str;
    }

    ಆದರೆ (str != ಖಾಲಿ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    ಶೂನ್ಯ
tagstack_clear_ನಮೂದು(taggy_T *ಅಂಶ)
{
    VIM_CLEAR(ಅಂಶ->tagname);
    VIM_CLEAR(ಅಂಶ->user_ದತ್ತಾಂಶ);
}

    ಪೂರ್ಣಾಂಕ
expand_ಟ್ಯಾಗುಗಳು(
    ಪೂರ್ಣಾಂಕ		tagnames,	// expand tag names
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_ಗಾತ್ರ = 100;
    tagptrs_T	t_p;
    ಪೂರ್ಣಾಂಕ		ret;

    name_buf = alloc(name_buf_ಗಾತ್ರ);
    ಆದರೆ (name_buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (tagnames)
	extra_flag = ಟ್ಯಾಗ್_ಹೆಸರುಗಳು;
    ಇಲ್ಲದಿದ್ದರೆ
	extra_flag = 0;
    ಆದರೆ (pat[0] == '/')
	ret = ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(pat + 1, ಅಂಕೆ_file, file,
		ಟ್ಯಾಗ್_REGEXP | extra_flag | ಟ್ಯಾಗ್_VERBOSE,
		ಟ್ಯಾಗ್_MANY, curbuf->b_ffname);
    ಇಲ್ಲದಿದ್ದರೆ
	ret = ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(pat, ಅಂಕೆ_file, file,
	      ಟ್ಯಾಗ್_REGEXP | extra_flag | ಟ್ಯಾಗ್_VERBOSE | ಟ್ಯಾಗ್_NOIC,
		ಟ್ಯಾಗ್_MANY, curbuf->b_ffname);
    ಆದರೆ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 ಗಾಗಿ (i = 0; i < *ಅಂಕೆ_file; i++)
	 {
	     size_t	len;

	     parse_ಹೊಂದಿಕೆ((*file)[i], &t_p);
	     len = t_p.tagname_ಕೊನೆ - t_p.tagname;
	     ಆದರೆ (len > name_buf_ಗಾತ್ರ - 3)
	     {
		 char_u *buf;

		 name_buf_ಗಾತ್ರ = len + 3;
		 buf = vim_realloc(name_buf, name_buf_ಗಾತ್ರ);
		 ಆದರೆ (buf == ಖಾಲಿ)
		 {
		     vim_free(name_buf);
		     ಹಿಂತಿರುಗಿ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ಖಾಲಿ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_ಕೊನೆ - t_p.fname);
	     (*file)[i][len + (t_p.fname_ಕೊನೆ - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(
    dict_T  *dict,
    ಅಕ್ಷರ    *ಕ್ಷೇತ್ರ_name,
    char_u  *ಆರಂಭ,		// start of the value
    char_u  *ಕೊನೆ)		// after the value; can be NULL
{
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		retval;

    // check that the field name doesn't exist yet
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, ಕ್ಷೇತ್ರ_name))
    {
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), ಕ್ಷೇತ್ರ_name);
	    verbose_leave();
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }
    buf = alloc(MAXPATHL);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
    {
	ಆದರೆ (ಕೊನೆ == ಖಾಲಿ)
	{
	    ಕೊನೆ = ಆರಂಭ + STRLEN(ಆರಂಭ);
	    ಆಗ (ಕೊನೆ > ಆರಂಭ && (ಕೊನೆ[-1] == '\r' || ಕೊನೆ[-1] == '\n'))
		--ಕೊನೆ;
	}
	len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ಆರಂಭ);
	ಆದರೆ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, ಆರಂಭ, len);
    }
    buf[len] = NUL;
    retval = dict_ಕೂಡಿಸು_string(dict, ಕ್ಷೇತ್ರ_name, buf);
    vim_free(buf);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    ಪೂರ್ಣಾಂಕ
get_ಟ್ಯಾಗುಗಳು(list_T *list, char_u *pat, char_u *buf_fname)
{
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_matches, i, ret;
    char_u	**matches, *p;
    char_u	*ತುಂಬಿದ_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    ದೀರ್ಘ	is_static;

    ret = ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗುಗಳು(pat, &ಅಂಕೆ_matches, &matches,
				ಟ್ಯಾಗ್_REGEXP | ಟ್ಯಾಗ್_NOIC, (ಪೂರ್ಣಾಂಕ)MAXCOL, buf_fname);
    ಆದರೆ (ret != OK || ಅಂಕೆ_matches <= 0)
	ಹಿಂತಿರುಗಿ ret;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_matches; ++i)
    {
	ಆದರೆ (parse_ಹೊಂದಿಕೆ(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    ಮುಂದುವರಿ;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ಆದರೆ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ ((dict = dict_alloc()) == ಖಾಲಿ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    ಮುಂದುವರಿ;
	}

	ತುಂಬಿದ_fname = ಟ್ಯಾಗ್_ತುಂಬಿದ_fname(&tp);
	ಆದರೆ (ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(dict, "name", tp.tagname, tp.tagname_ಕೊನೆ) == FAIL
		|| ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(dict, "filename", ತುಂಬಿದ_fname,
		    ಖಾಲಿ) == FAIL
		|| ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(dict, "cmd", tp.command,
		    tp.command_ಕೊನೆ) == FAIL
		|| ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(dict, "kind", tp.tagkind,
		    tp.tagkind_ಕೊನೆ) == FAIL
		|| dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(ತುಂಬಿದ_fname);

	ಆದರೆ (tp.command_ಕೊನೆ != ಖಾಲಿ)
	{
	    ಗಾಗಿ (p = tp.command_ಕೊನೆ + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ಆದರೆ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_ಕೊನೆ - 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    ಪೂರ್ಣಾಂಕ	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ಆಗ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (ಪೂರ್ಣಾಂಕ)(p - n);
		    ಆದರೆ (*p == ':' && len > 0)
		    {
			s = ++p;
			ಆಗ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ಆದರೆ (ಕೂಡಿಸು_ಟ್ಯಾಗ್_ಕ್ಷೇತ್ರ(dict, (ಅಕ್ಷರ *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// Skip field without colon.
			ಆಗ (*p != NUL && *p >= ' ')
			    ++p;
		    ಆದರೆ (*p == NUL)
			ನಿಲ್ಲಿಸು;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಟ್ಯಾಗ್_details(taggy_T *ಟ್ಯಾಗ್, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_ಕೂಡಿಸು_string(retdict, "tagname", ಟ್ಯಾಗ್->tagname);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "matchnr", ಟ್ಯಾಗ್->cur_ಹೊಂದಿಕೆ + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "bufnr", ಟ್ಯಾಗ್->cur_fnum);
    ಆದರೆ (ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ)
	dict_ಕೂಡಿಸು_string(retdict, "user_data", ಟ್ಯಾಗ್->user_ದತ್ತಾಂಶ);

    ಆದರೆ ((pos = list_alloc_id(aid_tagstack_from)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    dict_ಕೂಡಿಸು_list(retdict, "from", pos);

    fmark = &ಟ್ಯಾಗ್->fmark;
    list_append_ಸಂಖ್ಯೆ(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_ಸಂಖ್ಯೆ(pos, (varnumber_T)fmark->mark.lnum);
    list_append_ಸಂಖ್ಯೆ(pos, (varnumber_T)(fmark->mark.ಲಂಬಸಾಲು == MAXCOL ?
					MAXCOL : fmark->mark.ಲಂಬಸಾಲು + 1));
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
    ಪೂರ್ಣಾಂಕ		i;
    dict_T	*d;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "length", wp->w_tagstacklen);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_ವಸ್ತುಗಳು);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    dict_ಕೂಡಿಸು_list(retdict, "items", l);

    ಗಾಗಿ (i = 0; i < wp->w_tagstacklen; i++)
    {
	ಆದರೆ ((d = dict_alloc_id(aid_tagstack_details)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಹಿಂತಿರುಗಿ;
	}

	get_ಟ್ಯಾಗ್_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_clear(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ i;

    // Free the current tag stack
    ಗಾಗಿ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_ನಮೂದು(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_ಸರಿಸು(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ಪೂರ್ಣಾಂಕ		i;

    tagstack_clear_ನಮೂದು(&tagstack[0]);
    ಗಾಗಿ (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_ತಳ್ಳು_ಅಂಶ(
	win_T	*wp,
	char_u	*tagname,
	ಪೂರ್ಣಾಂಕ	cur_fnum,
	ಪೂರ್ಣಾಂಕ	cur_ಹೊಂದಿಕೆ,
	pos_T	mark,
	ಪೂರ್ಣಾಂಕ	fnum,
	char_u  *user_ದತ್ತಾಂಶ)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ಪೂರ್ಣಾಂಕ		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ಆದರೆ (idx >= TAGSTACKSIZE)
    {
	tagstack_ಸರಿಸು(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_ಹೊಂದಿಕೆ = cur_ಹೊಂದಿಕೆ;
    ಆದರೆ (tagstack[idx].cur_ಹೊಂದಿಕೆ < 0)
	tagstack[idx].cur_ಹೊಂದಿಕೆ = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_ದತ್ತಾಂಶ = user_ದತ್ತಾಂಶ;
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
    ಪೂರ್ಣಾಂಕ		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ಖಾಲಿ)
	    ಮುಂದುವರಿ;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(itemdict, (char_u *)"from", -1)) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (list2fpos(&di->di_tv, &mark, &fnum, ಖಾಲಿ, FALSE) != OK)
	    ಮುಂದುವರಿ;
	ಆದರೆ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (mark.ಲಂಬಸಾಲು > 0)
	    mark.ಲಂಬಸಾಲು--;
	tagstack_ತಳ್ಳು_ಅಂಶ(wp, tagname,
		(ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(itemdict, "bufnr"),
		(ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tagstack_ಗಣ_curidx(win_T *wp, ಪೂರ್ಣಾಂಕ curidx)
{
    wp->w_tagstackidx = curidx;
    ಆದರೆ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ಆದರೆ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_tagstack(win_T *wp, dict_T *d, ಪೂರ್ಣಾಂಕ action)
{
    dictitem_T	*di;
    list_T	*l = ಖಾಲಿ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ಆದರೆ (tfu_ಒಳ_use)
    {
	emsg(_(e_cannot_modify_ಟ್ಯಾಗ್_ರಾಶಿ_within_tagfunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }
# endif

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"items", -1)) != ಖಾಲಿ)
    {
	ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"curidx", -1)) != ಖಾಲಿ)
	tagstack_ಗಣ_curidx(wp, (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&di->di_tv) - 1);

    ಆದರೆ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	ಪೂರ್ಣಾಂಕ	tagstackidx = wp->w_tagstackidx;
	ಪೂರ್ಣಾಂಕ	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ಆಗ (tagstackidx < tagstacklen)
	    tagstack_clear_ನಮೂದು(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ಆದರೆ (l != ಖಾಲಿ)
    {
	ಆದರೆ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_ತಳ್ಳು_ವಸ್ತುಗಳು(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ
