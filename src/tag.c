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

#అన్తర్భావ "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
రకనిర్వచనం నిర్మాణం ట్యాగ్_సూచికలు
{
    // filled in by parse_tag_line():
    char_u	*tagname;	// start of tag name (skip "file:")
    char_u	*tagname_చివర;	// char after tag name
    char_u	*fname;		// first char of file name
    char_u	*fname_చివర;	// char after file name
    char_u	*command;	// first char of command
    // filled in by parse_match():
    char_u	*command_చివర;	// first char after command
    char_u	*ట్యాగ్_fname;	// file name of the tags file. This is used
				// when 'tr' is set.
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    పూర్ణాంకం		is_etag;	// TRUE for emacs tag
#అయితే_ముగింపు
    char_u	*tagkind;	// "kind:" value
    char_u	*tagkind_చివర;	// end of tagkind
    char_u	*user_దత్తాంశం;	// user_data string
    char_u	*user_దత్తాంశం_చివర;	// end of user_data
    linenr_T	tagline;	// "line:" value
} tagptrs_T;

/*
 * Return values used when reading lines from a tags file.
 */
రకనిర్వచనం గణన
{
    ట్యాగులు_READ_SUCCESS = 1,
    ట్యాగులు_READ_EOF,
    ట్యాగులు_READ_IGNORE,
} ట్యాగులు_read_స్థితి_T;

/*
 * States used during a tags search
 */
రకనిర్వచనం గణన
{
    TS_ప్రారంభం,		// at start of file
    TS_రేఖీయ,		// linear searching forward, till EOF
    TS_ద్విమాన,		// binary searching
    TS_SKIP_వెనుక,	// skipping backwards
    TS_అడుగు_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
రకనిర్వచనం నిర్మాణం
{
    off_T	దిగువ_స్థానభ్రంశం;	// offset for first char of first line that
				// could match
    off_T	ఎగువ_స్థానభ్రంశం;	// offset of char after last line that could
				//సరిపోలిక
    off_T	curr_స్థానభ్రంశం;	// Current file offset in search range
    off_T	curr_స్థానభ్రంశం_వాడిన; // curr_offset used when skipping back
    off_T	సరిపోలిక_స్థానభ్రంశం;	// Where the binary search found a tag
    పూర్ణాంకం	దిగువ_char;		// first char at low_offset
    పూర్ణాంకం	ఎగువ_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
రకనిర్వచనం గణన
{
    ట్యాగ్_సరిపోలిక_SUCCESS = 1,
    ట్యాగ్_సరిపోలిక_FAIL,
    ట్యాగ్_సరిపోలిక_STOP,
    ట్యాగ్_సరిపోలిక_తదుపరి
} tagmatch_స్థితి_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	matchoff;		// tag match offset
    పూర్ణాంకం	సరిపోలిక_re;		// TRUE if the tag matches a regexp
    పూర్ణాంకం	సరిపోలిక_no_ic;		// TRUE if the tag matches with case
    పూర్ణాంకం	has_re;			// regular expression used
    పూర్ణాంకం	sortic;			// tags file sorted ignoring case (foldcase)
    పూర్ణాంకం	క్రమీకరించు_error;		// tags file not sorted
} findtags_సరిపోలిక_వాదనలు_T;

/*
 * The matching tags are first stored in one of the hash tables.  In
 * which one depends on the priority of the match.
 * ht_match[] is used to find duplicates, ga_match[] to keep them in sequence.
 * At the end, all the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#నిర్వచించు MT_ST_CUR	0		// static match in current file
#నిర్వచించు MT_GL_CUR	1		// global match in current file
#నిర్వచించు MT_GL_OTH	2		// global match in other file
#నిర్వచించు MT_ST_OTH	3		// static match in other file
#నిర్వచించు MT_IC_OFF	4		// add for icase match
#నిర్వచించు MT_RE_OFF	8		// add for regexp match
#నిర్వచించు MT_ముసుగు		7		// mask for printing priority
#నిర్వచించు MT_లెక్క	16

స్థిర అక్షరం	*mt_పేర్లు[MT_లెక్క/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#నిర్వచించు NOTAGFILE	99		// return value for jumpto_tag
స్థిర char_u	*nofile_fname = ఖాళీ;	// fname for NOTAGFILE error

స్థిర శూన్య taglen_advance(పూర్ణాంకం l);

స్థిర పూర్ణాంకం jumpto_ట్యాగ్(char_u *lbuf, పూర్ణాంకం forceit, పూర్ణాంకం keep_help);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
స్థిర పూర్ణాంకం parse_ట్యాగ్_పంక్తి(char_u *lbuf, పూర్ణాంకం is_etag, tagptrs_T *tagp);
#లేకపోతే
స్థిర పూర్ణాంకం parse_ట్యాగ్_పంక్తి(char_u *lbuf, tagptrs_T *tagp);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం test_for_static(tagptrs_T *);
స్థిర పూర్ణాంకం parse_సరిపోలిక(char_u *lbuf, tagptrs_T *tagp);
స్థిర char_u *ట్యాగ్_నిండిన_fname(tagptrs_T *tagp);
స్థిర char_u *expand_ట్యాగ్_fname(char_u *fname, char_u *ట్యాగ్_fname, పూర్ణాంకం expand);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
స్థిర పూర్ణాంకం test_for_current(పూర్ణాంకం, char_u *, char_u *, char_u *, char_u *);
#లేకపోతే
స్థిర పూర్ణాంకం test_for_current(char_u *, char_u *, char_u *, char_u *);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం కనుగొను_extra(char_u **pp);
స్థిర శూన్య print_ట్యాగ్_list(పూర్ణాంకం new_ట్యాగ్, పూర్ణాంకం use_tagstack, పూర్ణాంకం అంకె_matches, char_u **matches);
#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
స్థిర పూర్ణాంకం కూడు_llist_ట్యాగులు(char_u *ట్యాగ్, పూర్ణాంకం అంకె_matches, char_u **matches);
#అయితే_ముగింపు

స్థిర char_u	*tagmatchname = ఖాళీ;	// name of last used tag

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
స్థిర taggy_T ptag_నమోదు = {ఖాళీ, {{0, 0, 0}, 0}, 0, 0, ఖాళీ};
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం  tfu_లోపల_use = FALSE;	    // disallow recursive call of tagfunc
స్థిర callback_T tfu_cb;	    // 'tagfunc' callback function
#అయితే_ముగింపు

// Used instead of NUL to separate tag fields in the growarrays.
#నిర్వచించు ట్యాగ్_SEP 0x02

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    అక్షరం *
did_సమితి_tagfunc(optset_T *వాదనలు)
{
    పూర్ణాంకం	retval;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string,
							    &curbuf->b_tfu_cb);
    లేకపోతే
    {
	retval = option_సమితి_callback_func(వాదనలు->os_newval.string, &tfu_cb);
	ఒకవేళ (retval == OK && !(వాదనలు->os_flags & OPT_GLOBAL))
	    సమితి_buflocal_tfu_callback(curbuf);
    }

    తిరిగి retval == FAIL ? e_invalid_వాదం : ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_tagfunc_option(శూన్య)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_tagfunc(పూర్ణాంకం copyID వాడని)
{
    పూర్ణాంకం	abort = FALSE;

    abort = సమితి_ref_లోపల_callback(&tfu_cb, copyID);

    తిరిగి abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    శూన్య
సమితి_buflocal_tfu_callback(buf_T *buf వాడని)
{
    free_callback(&buf->b_tfu_cb);
    ఒకవేళ (tfu_cb.cb_name != ఖాళీ && *tfu_cb.cb_name != NUL)
	copy_callback(&buf->b_tfu_cb, &tfu_cb);
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
do_ట్యాగ్(
    char_u	*ట్యాగ్,		// tag (pattern) to jump to
    పూర్ణాంకం		type,
    పూర్ణాంకం		లెక్క,
    పూర్ణాంకం		forceit,	// :ta with !
    పూర్ణాంకం		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    పూర్ణాంకం		tagstackidx = curwin->w_tagstackidx;
    పూర్ణాంకం		tagstacklen = curwin->w_tagstacklen;
    పూర్ణాంకం		cur_సరిపోలిక = 0;
    పూర్ణాంకం		cur_fnum = curbuf->b_fnum;
    పూర్ణాంకం		oldtagstackidx = tagstackidx;
    పూర్ణాంకం		prevtagstackidx = tagstackidx;
    పూర్ణాంకం		prev_అంకె_matches;
    పూర్ణాంకం		new_ట్యాగ్ = FALSE;
    పూర్ణాంకం		i;
    పూర్ణాంకం		ic;
    పూర్ణాంకం		no_regexp = FALSE;
    పూర్ణాంకం		error_cur_సరిపోలిక = 0;
    పూర్ణాంకం		save_pos = FALSE;
    fmark_T	saved_fmark;
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం		jumped_గమ్యం_ట్యాగ్ = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		new_అంకె_matches;
    char_u	**new_matches;
    పూర్ణాంకం		use_tagstack;
    పూర్ణాంకం		skip_సందేశం = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    పూర్ణాంకం		use_tfu = 1;
    char_u	*tofree = ఖాళీ;

    // remember the matches for the last used tag
    స్థిర పూర్ణాంకం		అంకె_matches = 0;
    స్థిర పూర్ణాంకం		max_అంకె_matches = 0;  // limit used for match search
    స్థిర char_u	**matches = ఖాళీ;
    స్థిర పూర్ణాంకం		flags;

#నిర్వచితమైతే EXITFREE
    ఒకవేళ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(అంకె_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_ట్యాగులు();
# endif
	అంకె_matches = 0;
	తిరిగి FALSE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (tfu_లోపల_use)
    {
	emsg(_(e_cannot_modify_ట్యాగ్_దొంతర_within_tagfunc));
	తిరిగి FALSE;
    }
#అయితే_ముగింపు

    ఒకవేళ (postponed_విభజించు == 0 && !check_can_సమితి_curbuf_forceit(forceit))
	తిరిగి FALSE;

    ఒకవేళ (type == DT_HELP)
    {
	type = DT_ట్యాగ్;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_అంకె_matches = అంకె_matches;
    free_string_option(nofile_fname);
    nofile_fname = ఖాళీ;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ఒకవేళ ((!p_tgst && *ట్యాగ్ != NUL))
    {
	use_tagstack = FALSE;
	new_ట్యాగ్ = TRUE;
#ఒకవేళ defined(FEAT_QUICKFIX)
	ఒకవేళ (g_do_tagpreview != 0)
	{
	    tagstack_clear_నమోదు(&ptag_నమోదు);
	    ఒకవేళ ((ptag_నమోదు.tagname = vim_strsave(ట్యాగ్)) == ఖాళీ)
		వెళ్ళు చివర_do_ట్యాగ్;
	}
#అయితే_ముగింపు
    }
    లేకపోతే
    {
#ఒకవేళ defined(FEAT_QUICKFIX)
	ఒకవేళ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	లేకపోతే
#అయితే_ముగింపు
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ఒకవేళ (*ట్యాగ్ != NUL
		&& (type == DT_ట్యాగ్ || type == DT_SELECT || type == DT_JUMP
#నిర్వచితమైతే FEAT_QUICKFIX
		    || type == DT_LTAG
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
		    || type == DT_CSCOPE
#అయితే_ముగింపు
		    ))
	{
#ఒకవేళ defined(FEAT_QUICKFIX)
	    ఒకవేళ (g_do_tagpreview != 0)
	    {
		ఒకవేళ (ptag_నమోదు.tagname != ఖాళీ
			&& STRCMP(ptag_నమోదు.tagname, ట్యాగ్) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_సరిపోలిక = ptag_నమోదు.cur_సరిపోలిక;
		    cur_fnum = ptag_నమోదు.cur_fnum;
		}
		లేకపోతే
		{
		    tagstack_clear_నమోదు(&ptag_నమోదు);
		    ఒకవేళ ((ptag_నమోదు.tagname = vim_strsave(ట్యాగ్)) == ఖాళీ)
			వెళ్ళు చివర_do_ట్యాగ్;
		}
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		అయితే (tagstackidx < tagstacklen)
		    tagstack_clear_నమోదు(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ఒకవేళ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_నమోదు(&tagstack[0]);
		    కొరకు (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_దత్తాంశం = ఖాళీ;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ఒకవేళ ((tagstack[tagstackidx].tagname = vim_strsave(ట్యాగ్)) == ఖాళీ)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    వెళ్ళు చివర_do_ట్యాగ్;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_ట్యాగ్ = TRUE;
	}
	లేకపోతే
	{
	    ఒకవేళ (
#ఒకవేళ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_నమోదు.tagname == ఖాళీ :
#అయితే_ముగింపు
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_ట్యాగ్_దొంతర_ఖాళీలు));
		వెళ్ళు చివర_do_ట్యాగ్;
	    }

	    ఒకవేళ (type == DT_తీయి)		// go to older position
	    {
#నిర్వచితమైతే FEAT_FOLDING
		పూర్ణాంకం	old_కీలకం_Typed = కీలకం_Typed;
#అయితే_ముగింపు
		ఒకవేళ ((tagstackidx -= లెక్క) < 0)
		{
		    emsg(_(e_at_bottom_యొక్క_ట్యాగ్_దొంతర));
		    ఒకవేళ (tagstackidx + లెక్క == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			వెళ్ళు చివర_do_ట్యాగ్;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		లేకపోతే ఒకవేళ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_శిఖరం_యొక్క_ట్యాగ్_దొంతర));
		    వెళ్ళు చివర_do_ట్యాగ్;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ఒకవేళ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ఒకవేళ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			వెళ్ళు చివర_do_ట్యాగ్;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		లేకపోతే
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.నిలువు = saved_fmark.mark.నిలువు;
		curwin->w_సమితి_curswant = true;
		check_cursor();
#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ ((fdo_flags & FDO_ట్యాగ్) && old_కీలకం_Typed)
		    foldOpenCursor();
#అయితే_ముగింపు

		// remove the old list of matches
		FreeWild(అంకె_matches, matches);
#నిర్వచితమైతే FEAT_CSCOPE
		cs_free_ట్యాగులు();
#అయితే_ముగింపు
		అంకె_matches = 0;
		ట్యాగ్_freematch();
		వెళ్ళు చివర_do_ట్యాగ్;
	    }

	    ఒకవేళ (type == DT_ట్యాగ్
#ఒకవేళ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#అయితే_ముగింపు
	       )
	    {
#ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (g_do_tagpreview != 0)
		{
		    cur_సరిపోలిక = ptag_నమోదు.cur_సరిపోలిక;
		    cur_fnum = ptag_నమోదు.cur_fnum;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ఒకవేళ ((tagstackidx += లెక్క - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_శిఖరం_యొక్క_ట్యాగ్_దొంతర));
			save_pos = FALSE;
		    }
		    లేకపోతే ఒకవేళ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_యొక్క_ట్యాగ్_దొంతర));
			tagstackidx = 0;
			వెళ్ళు చివర_do_ట్యాగ్;
		    }
		    cur_సరిపోలిక = tagstack[tagstackidx].cur_సరిపోలిక;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_ట్యాగ్ = TRUE;
	    }
	    లేకపోతే				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (g_do_tagpreview != 0)
		{
		    cur_సరిపోలిక = ptag_నమోదు.cur_సరిపోలిక;
		    cur_fnum = ptag_నమోదు.cur_fnum;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    ఒకవేళ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_సరిపోలిక = tagstack[tagstackidx].cur_సరిపోలిక;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		ఎంపిక (type)
		{
		    స్థితి DT_మొదటి: cur_సరిపోలిక = లెక్క - 1; ఆపు;
		    స్థితి DT_SELECT:
		    స్థితి DT_JUMP:
#నిర్వచితమైతే FEAT_CSCOPE
		    స్థితి DT_CSCOPE:
#అయితే_ముగింపు
		    స్థితి DT_చివరి:  cur_సరిపోలిక = MAXCOL - 1; ఆపు;
		    స్థితి DT_తదుపరి:  cur_సరిపోలిక += లెక్క; ఆపు;
		    స్థితి DT_PREV:  cur_సరిపోలిక -= లెక్క; ఆపు;
		}
		ఒకవేళ (cur_సరిపోలిక >= MAXCOL)
		    cur_సరిపోలిక = MAXCOL - 1;
		లేకపోతే ఒకవేళ (cur_సరిపోలిక < 0)
		{
		    emsg(_(e_cannot_go_before_మొదటి_matching_ట్యాగ్));
		    skip_సందేశం = TRUE;
		    cur_సరిపోలిక = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ఒకవేళ defined(FEAT_QUICKFIX)
	ఒకవేళ (g_do_tagpreview != 0)
	{
	    ఒకవేళ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_నమోదు.cur_సరిపోలిక = cur_సరిపోలిక;
		ptag_నమోదు.cur_fnum = cur_fnum;
	    }
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ఒకవేళ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ఒకవేళ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_సరిపోలిక = cur_సరిపోలిక;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ఒకవేళ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ఒకవేళ (buf != ఖాళీ)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ఒకవేళ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ఒకవేళ defined(FEAT_QUICKFIX)
	లేకపోతే ఒకవేళ (g_do_tagpreview != 0)
	    name = ptag_నమోదు.tagname;
#అయితే_ముగింపు
	లేకపోతే
	    name = ట్యాగ్;
	other_name = (tagmatchname == ఖాళీ || STRCMP(tagmatchname, name) != 0);
	ఒకవేళ (new_ట్యాగ్
		|| (cur_సరిపోలిక >= అంకె_matches && max_అంకె_matches != MAXCOL)
		|| other_name)
	{
	    ఒకవేళ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ఒకవేళ (type == DT_SELECT || type == DT_JUMP
#ఒకవేళ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#అయితే_ముగింపు
		)
		cur_సరిపోలిక = MAXCOL - 1;
	    ఒకవేళ (type == DT_ట్యాగ్)
		max_అంకె_matches = MAXCOL;
	    లేకపోతే
		max_అంకె_matches = cur_సరిపోలిక + 1;

	    // when the argument starts with '/', use it as a regexp
	    ఒకవేళ (!no_regexp && *name == '/')
	    {
		flags = ట్యాగ్_REGEXP;
		++name;
	    }
	    లేకపోతే
		flags = ట్యాగ్_NOIC;

#నిర్వచితమైతే FEAT_CSCOPE
	    ఒకవేళ (type == DT_CSCOPE)
		flags = ట్యాగ్_CSCOPE;
#అయితే_ముగింపు
	    ఒకవేళ (verbose)
		flags |= ట్యాగ్_VERBOSE;

	    ఒకవేళ (!use_tfu)
		flags |= ట్యాగ్_NO_TAGFUNC;

	    ఒకవేళ (కనుగొను_ట్యాగులు(name, &new_అంకె_matches, &new_matches, flags,
					    max_అంకె_matches, buf_ffname) == OK
		    && new_అంకె_matches < max_అంకె_matches)
		max_అంకె_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ఒకవేళ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_కిటికీ_unexpectedly_close_while_searching_for_ట్యాగులు));
		FreeWild(new_అంకె_matches, new_matches);
		ఆపు;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ఒకవేళ (!new_ట్యాగ్ && !other_name)
	    {
		పూర్ణాంకం	    j, k;
		పూర్ణాంకం	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		కొరకు (j = 0; j < అంకె_matches; ++j)
		{
		    parse_సరిపోలిక(matches[j], &tagp);
		    కొరకు (i = idx; i < new_అంకె_matches; ++i)
		    {
			parse_సరిపోలిక(new_matches[i], &tagp2);
			ఒకవేళ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    కొరకు (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    ఆపు;
			}
		    }
		}
	    }
	    FreeWild(అంకె_matches, matches);
	    అంకె_matches = new_అంకె_matches;
	    matches = new_matches;
	}

	ఒకవేళ (అంకె_matches <= 0)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_ట్యాగ్_not_దొరికింది_str), name);
#ఒకవేళ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    పూర్ణాంకం ask_for_selection = FALSE;
#ఒకవేళ defined(FEAT_EVAL)
	    size_t  IObufflen;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CSCOPE
	    ఒకవేళ (type == DT_CSCOPE && అంకె_matches > 1)
	    {
		cs_print_ట్యాగులు();
		ask_for_selection = TRUE;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    ఒకవేళ (type == DT_ట్యాగ్ && *ట్యాగ్ != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_సరిపోలిక = లెక్క > 0 ? లెక్క - 1 : 0;
	    లేకపోతే ఒకవేళ (type == DT_SELECT || (type == DT_JUMP && అంకె_matches > 1))
	    {
		print_ట్యాగ్_list(new_ట్యాగ్, use_tagstack, అంకె_matches, matches);
		ask_for_selection = TRUE;
	    }
#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    లేకపోతే ఒకవేళ (type == DT_LTAG)
	    {
		ఒకవేళ (కూడు_llist_ట్యాగులు(ట్యాగ్, అంకె_matches, matches) == FAIL)
		    వెళ్ళు చివర_do_ట్యాగ్;
		cur_సరిపోలిక = 0;		// Jump to the first tag
	    }
#అయితే_ముగింపు

	    ఒకవేళ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_సంఖ్య(ఖాళీ);
		ఒకవేళ (i <= 0 || i > అంకె_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ఒకవేళ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#నిర్వచితమైతే FEAT_CSCOPE
		    cs_free_ట్యాగులు();
		    jumped_గమ్యం_ట్యాగ్ = TRUE;
#అయితే_ముగింపు
		    ఆపు;
		}
		cur_సరిపోలిక = i - 1;
	    }

	    ఒకవేళ (cur_సరిపోలిక >= అంకె_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ఒకవేళ ((type == DT_తదుపరి || type == DT_మొదటి)
						      && nofile_fname == ఖాళీ)
		{
		    ఒకవేళ (అంకె_matches == 1)
			emsg(_(e_there_is_only_one_matching_ట్యాగ్));
		    లేకపోతే
			emsg(_(e_cannot_go_beyond_చివరి_matching_ట్యాగ్));
		    skip_సందేశం = TRUE;
		}
		cur_సరిపోలిక = అంకె_matches - 1;
	    }
	    ఒకవేళ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_సరిపోలిక = cur_సరిపోలిక;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ఒకవేళ (use_tfu && parse_సరిపోలిక(matches[cur_సరిపోలిక], &tagp) == OK
			&& tagp.user_దత్తాంశం)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_దత్తాంశం);
		    tagstack[tagstackidx].user_దత్తాంశం = vim_strnsave(
			  tagp.user_దత్తాంశం, tagp.user_దత్తాంశం_చివర - tagp.user_దత్తాంశం);
		}

		++tagstackidx;
	    }
#ఒకవేళ defined(FEAT_QUICKFIX)
	    లేకపోతే ఒకవేళ (g_do_tagpreview != 0)
	    {
		ptag_నమోదు.cur_సరిపోలిక = cur_సరిపోలిక;
		ptag_నమోదు.cur_fnum = cur_fnum;
	    }
#అయితే_ముగింపు

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ఒకవేళ (nofile_fname != ఖాళీ && error_cur_సరిపోలిక != cur_సరిపోలిక)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_సరిపోలిక][0] & MT_IC_OFF);
	    ఒకవేళ (type != DT_ట్యాగ్ && type != DT_SELECT && type != DT_JUMP
#నిర్వచితమైతే FEAT_CSCOPE
		&& type != DT_CSCOPE
#అయితే_ముగింపు
		&& (అంకె_matches > 1 || ic)
		&& !skip_సందేశం)
	    {
		// Give an indication of the number of matching tags
		sprintf((అక్షరం *)IObuff, _("tag %d of %d%s"),
				cur_సరిపోలిక + 1,
				అంకె_matches,
				max_అంకె_matches != MAXCOL ? _(" or more") : "");
		ఒకవేళ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ఒకవేళ ((అంకె_matches > prev_అంకె_matches || new_ట్యాగ్)
							   && అంకె_matches > 1)
		{
		    ఒకవేళ (ic)
			సందేశం_attr((అక్షరం *)IObuff, HL_ATTR(HLF_W));
		    లేకపోతే
			సందేశం((అక్షరం *)IObuff);
		    సందేశం_scroll = TRUE;	// don't overwrite this message
		}
		లేకపోతే
		    give_warning(IObuff, ic);
		ఒకవేళ (ic && !సందేశం_scrolled && సందేశం_silent == 0)
		{
		    బహిర్గతం_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ఒకవేళ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE, ":ta %s\r", name);
	    సమితి_vim_var_string(VV_SWAPCOMMAND, IObuff, (పూర్ణాంకం)IObufflen);
#అయితే_ముగింపు

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_ట్యాగ్(matches[cur_సరిపోలిక], forceit, type != DT_CSCOPE);

#ఒకవేళ defined(FEAT_EVAL)
	    సమితి_vim_var_string(VV_SWAPCOMMAND, ఖాళీ, -1);
#అయితే_ముగింపు

	    ఒకవేళ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ఒకవేళ ((type == DT_PREV && cur_సరిపోలిక > 0)
			|| ((type == DT_ట్యాగ్ || type == DT_తదుపరి
							  || type == DT_మొదటి)
			    && (max_అంకె_matches != MAXCOL
					     || cur_సరిపోలిక < అంకె_matches - 1)))
		{
		    error_cur_సరిపోలిక = cur_సరిపోలిక;
		    ఒకవేళ (use_tagstack)
			--tagstackidx;
		    ఒకవేళ (type == DT_PREV)
			--cur_సరిపోలిక;
		    లేకపోతే
		    {
			type = DT_తదుపరి;
			++cur_సరిపోలిక;
		    }
		    కొనసాగు;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    లేకపోతే
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ఒకవేళ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#నిర్వచితమైతే FEAT_CSCOPE
		jumped_గమ్యం_ట్యాగ్ = TRUE;
#అయితే_ముగింపు
	    }
	}
	ఆపు;
    }

చివర_do_ట్యాగ్:
    // Only store the new index when using the tagstack and it's valid.
    ఒకవేళ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_విభజించు = 0;	// don't split next time
#నిర్వచితమైతే FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#అయితే_ముగింపు

    vim_free(tofree);
#నిర్వచితమైతే FEAT_CSCOPE
    తిరిగి jumped_గమ్యం_ట్యాగ్;
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * List all the matching tags.
 */
    స్థిర శూన్య
print_ట్యాగ్_list(
    పూర్ణాంకం		new_ట్యాగ్,
    పూర్ణాంకం		use_tagstack,
    పూర్ణాంకం		అంకె_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    పూర్ణాంకం		tagstackidx = curwin->w_tagstackidx;
    పూర్ణాంకం		i;
    char_u	*p;
    char_u	*command_చివర;
    tagptrs_T	tagp;
    పూర్ణాంకం		taglen;
    పూర్ణాంకం		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_సరిపోలిక(matches[0], &tagp);
    taglen = (పూర్ణాంకం)(tagp.tagname_చివర - tagp.tagname + 2);
    ఒకవేళ (taglen < 18)
	taglen = 18;
    ఒకవేళ (taglen > cmdline_వెడల్పు - 25)
	taglen = MAXCOL;
    ఒకవేళ (సందేశం_నిలువు == 0)
	సందేశం_didout = FALSE;	// overwrite previous message
    సందేశం_ప్రారంభం();
    సందేశం_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    సందేశం_clr_eos();
    taglen_advance(taglen);
    సందేశం_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    కొరకు (i = 0; i < అంకె_matches && !got_int; ++i)
    {
	parse_సరిపోలిక(matches[i], &tagp);
	ఒకవేళ (!new_ట్యాగ్ && (
#ఒకవేళ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_నమోదు.cur_సరిపోలిక) ||
#అయితే_ముగింపు
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_సరిపోలిక)))
	    *IObuff = '>';
	లేకపోతే
	    *IObuff = ' ';
	vim_snprintf((అక్షరం *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_పేర్లు[matches[i][0] & MT_ముసుగు]);
	సందేశం_puts((అక్షరం *)IObuff);
	ఒకవేళ (tagp.tagkind != ఖాళీ)
	    సందేశం_outtrans_len(tagp.tagkind,
			  (పూర్ణాంకం)(tagp.tagkind_చివర - tagp.tagkind));
	సందేశం_advance(13);
	సందేశం_outtrans_len_attr(tagp.tagname,
			   (పూర్ణాంకం)(tagp.tagname_చివర - tagp.tagname),
						  HL_ATTR(HLF_T));
	సందేశం_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = ట్యాగ్_నిండిన_fname(&tagp);
	ఒకవేళ (p != ఖాళీ)
	{
	    సందేశం_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ఒకవేళ (సందేశం_నిలువు > 0)
	    సందేశం_putchar('\n');
	ఒకవేళ (got_int)
	    ఆపు;
	సందేశం_advance(15);

	// print any extra fields
	command_చివర = tagp.command_చివర;
	ఒకవేళ (command_చివర != ఖాళీ)
	{
	    p = command_చివర + 3;
	    అయితే (*p && *p != '\r' && *p != '\n')
	    {
		అయితే (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ఒకవేళ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    కొనసాగు;
		}
		// skip "kind:<kind>" and "<kind>"
		ఒకవేళ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_చివర;
		    కొనసాగు;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		అయితే (*p && *p != '\r' && *p != '\n')
		{
		    ఒకవేళ (సందేశం_నిలువు + ptr2cells(p) >= cmdline_వెడల్పు)
		    {
			సందేశం_putchar('\n');
			ఒకవేళ (got_int)
			    ఆపు;
			సందేశం_advance(15);
		    }
		    p = సందేశం_outtrans_one(p, attr);
		    ఒకవేళ (*p == TAB)
		    {
			సందేశం_puts_attr(" ", attr);
			ఆపు;
		    }
		    ఒకవేళ (*p == ':')
			attr = 0;
		}
	    }
	    ఒకవేళ (సందేశం_నిలువు > 15)
	    {
		సందేశం_putchar('\n');
		ఒకవేళ (got_int)
		    ఆపు;
		సందేశం_advance(15);
	    }
	}
	లేకపోతే
	{
	    కొరకు (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_చివర = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ఒకవేళ (*p == '/' || *p == '?')
	{
	    ++p;
	    ఒకవేళ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	అయితే (p != command_చివర && vim_isspace(*p))
	    ++p;

	అయితే (p != command_చివర)
	{
	    ఒకవేళ (సందేశం_నిలువు + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_వెడల్పు)
		సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
		ఆపు;
	    సందేశం_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ఒకవేళ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ఒకవేళ (*p == TAB)
	    {
		సందేశం_putchar(' ');
		++p;
	    }
	    లేకపోతే
		p = సందేశం_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ఒకవేళ (p == command_చివర - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		ఆపు;
	    // don't display matching '/' or '?'
	    ఒకవేళ (p == command_చివర - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		ఆపు;
	}
	ఒకవేళ (సందేశం_నిలువు)
	    సందేశం_putchar('\n');
	ui_breakcheck();
    }
    ఒకవేళ (got_int)
	got_int = FALSE;	// only stop the listing
}

#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    స్థిర పూర్ణాంకం
కూడు_llist_ట్యాగులు(
    char_u	*ట్యాగ్,
    పూర్ణాంకం		అంకె_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	ట్యాగ్_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    పూర్ణాంకం		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ఒకవేళ (list == ఖాళీ || fname == ఖాళీ || cmd == ఖాళీ)
    {
	vim_free(cmd);
	vim_free(fname);
	ఒకవేళ (list != ఖాళీ)
	    list_free(list);
	తిరిగి FAIL;
    }

    కొరకు (i = 0; i < అంకె_matches; ++i)
    {
	పూర్ణాంకం	    len, cmd_len;
	దీర్ఘ    lnum;
	dict_T  *dict;

	parse_సరిపోలిక(matches[i], &tagp);

	// Save the tag name
	len = (పూర్ణాంకం)(tagp.tagname_చివర - tagp.tagname);
	ఒకవేళ (len > 128)
	    len = 128;
	vim_strncpy(ట్యాగ్_name, tagp.tagname, len);
	ట్యాగ్_name[len] = NUL;

	// Save the tag file name
	p = ట్యాగ్_నిండిన_fname(&tagp);
	ఒకవేళ (p == ఖాళీ)
	    కొనసాగు;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ఒకవేళ (సురక్షిత_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((అక్షరం *)tagp.command);
	లేకపోతే
	{
	    char_u *cmd_ప్రారంభం, *cmd_చివర;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_ప్రారంభం = tagp.command;
	    cmd_చివర = tagp.command_చివర;
	    ఒకవేళ (cmd_చివర == ఖాళీ)
	    {
		కొరకు (p = tagp.command;
		     *p && *p != '\r' && *p != '\n'; ++p)
		    ;
		cmd_చివర = p;
	    }

	    // Now, cmd_end points to the character after the
	    // command. Adjust it to point to the last
	    // character of the command.
	    cmd_చివర--;

	    // Skip the '/' and '?' characters at the
	    // beginning and end of the search pattern.
	    ఒకవేళ (*cmd_ప్రారంభం == '/' || *cmd_ప్రారంభం == '?')
		cmd_ప్రారంభం++;

	    ఒకవేళ (*cmd_చివర == '/' || *cmd_చివర == '?')
		cmd_చివర--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ఒకవేళ (*cmd_ప్రారంభం == '^')
	    {
		STRCPY(cmd, "^");
		cmd_ప్రారంభం++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (పూర్ణాంకం)(cmd_చివర - cmd_ప్రారంభం + 1);
	    ఒకవేళ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_ప్రారంభం, cmd_len);
	    len += cmd_len;

	    ఒకవేళ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ఒకవేళ ((dict = dict_alloc()) == ఖాళీ)
	    కొనసాగు;
	ఒకవేళ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    కొనసాగు;
	}

	dict_కూడు_string(dict, "text", ట్యాగ్_name);
	dict_కూడు_string(dict, "filename", fname);
	dict_కూడు_సంఖ్య(dict, "lnum", lnum);
	ఒకవేళ (lnum == 0)
	    dict_కూడు_string(dict, "pattern", cmd);
    }

    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "ltag %s", ట్యాగ్);
    సమితి_errorlist(curwin, list, ' ', IObuff, ఖాళీ);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Free cached tags.
 */
    శూన్య
ట్యాగ్_freematch(శూన్య)
{
    VIM_CLEAR(tagmatchname);
}

    స్థిర శూన్య
taglen_advance(పూర్ణాంకం l)
{
    ఒకవేళ (l == MAXCOL)
    {
	సందేశం_putchar('\n');
	సందేశం_advance(24);
    }
    లేకపోతే
	సందేశం_advance(13 + l);
}

/*
 * Print the tag stack
 */
    శూన్య
do_ట్యాగులు(exarg_T *eap వాడని)
{
    పూర్ణాంకం		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    పూర్ణాంకం		tagstackidx = curwin->w_tagstackidx;
    పూర్ణాంకం		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    సందేశం_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    కొరకు (i = 0; i < tagstacklen; ++i)
    {
	ఒకవేళ (tagstack[i].tagname != ఖాళీ)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ఒకవేళ (name == ఖాళీ)	    // file name not available
		కొనసాగు;

	    సందేశం_putchar('\n');
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_సరిపోలిక + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    సందేశం_outtrans(IObuff);
	    సందేశం_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	బహిర్గతం_flush();		    // show one line at a time
    }
    ఒకవేళ (tagstackidx == tagstacklen)	// idx at top of stack
	సందేశం_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    స్థిర పూర్ణాంకం
ట్యాగ్_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    పూర్ణాంకం		i;

    అయితే (len > 0)
    {
	i = (పూర్ణాంకం)TOUPPER_ASC(*s1) - (పూర్ణాంకం)TOUPPER_ASC(*s2);
	ఒకవేళ (i != 0)
	    తిరిగి i;			// this character different
	ఒకవేళ (*s1 == NUL)
	    ఆపు;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    తిరిగి 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*pat;		// the pattern
    పూర్ణాంకం		len;		// length of pat[]
    char_u	*తల;		// start of pattern head
    పూర్ణాంకం		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    స్థిర శూన్య
prepare_pats(pat_T *pats, పూర్ణాంకం has_re)
{
    pats->తల = pats->pat;
    pats->headlen = pats->len;
    ఒకవేళ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ఒకవేళ (pats->pat[0] == '^')
	    pats->తల = pats->pat + 1;
	లేకపోతే ఒకవేళ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->తల = pats->pat + 2;
	ఒకవేళ (pats->తల == pats->pat)
	    pats->headlen = 0;
	లేకపోతే
	    కొరకు (pats->headlen = 0; pats->తల[pats->headlen] != NUL;
							      ++pats->headlen)
		ఒకవేళ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->తల[pats->headlen]) != ఖాళీ)
		    ఆపు;
	ఒకవేళ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ఒకవేళ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    లేకపోతే
	pats->regmatch.regprog = ఖాళీ;
}

#నిర్వచితమైతే FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
రకనిర్వచనం గణన {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_చిరునామా,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    స్థిర tagcmd_T
tagfunc_cmd_రకం(char_u *cmd)
{
    ఒకవేళ (VIM_ISDIGIT(*cmd))
	తిరిగి *skipdigits(cmd) == NUL ? TAGCMD_చిరునామా : TAGCMD_INVALID;
    ఒకవేళ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*చివర = skip_regexp(cmd + 1, *cmd, FALSE);

	తిరిగి (*చివర == *cmd && చివర[1] == NUL)
					? TAGCMD_చిరునామా : TAGCMD_INVALID;
    }
    ఒకవేళ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != ఖాళీ)
	తిరిగి TAGCMD_INVALID;
    తిరిగి TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
కనుగొను_tagfunc_ట్యాగులు(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    పూర్ణాంకం		*సరిపోలిక_లెక్క,	// here the number of tags found will be placed
    పూర్ణాంకం		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *అంశం;
    పూర్ణాంకం		ntags = 0;
    పూర్ణాంకం		ఫలితం = FAIL;
    typval_T	వాదనలు[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*ట్యాగ్ = &curwin->w_tagstack[curwin->w_tagstackidx];

    ఒకవేళ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == ఖాళీ
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	తిరిగి FAIL;

    వాదనలు[0].v_type = VAR_STRING;
    వాదనలు[0].vval.v_string = pat;
    వాదనలు[1].v_type = VAR_STRING;
    వాదనలు[1].vval.v_string = flagString;

    // create 'info' dict argument
    ఒకవేళ ((d = dict_alloc_తాళం(VAR_FIXED)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (!(flags & ట్యాగ్_INS_COMP) && ట్యాగ్->user_దత్తాంశం != ఖాళీ)
	dict_కూడు_string(d, "user_data", ట్యాగ్->user_దత్తాంశం);
    ఒకవేళ (buf_ffname != ఖాళీ)
	dict_కూడు_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    వాదనలు[2].v_type = VAR_DICT;
    వాదనలు[2].vval.v_dict = d;

    వాదనలు[3].v_type = VAR_UNKNOWN;

    vim_snprintf((అక్షరం *)flagString, పరిమాణం(flagString),
		 "%s%s%s",
		 g_ట్యాగ్_at_cursor      ? "c": "",
		 flags & ట్యాగ్_INS_COMP ? "i": "",
		 flags & ట్యాగ్_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    ఫలితం = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, వాదనలు);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ఒకవేళ (ఫలితం == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_సంఖ్య == VVAL_NULL)
    {
	clear_tv(&rettv);
	తిరిగి NOTDONE;
    }
    ఒకవేళ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_విలువ_from_tagfunc));
	తిరిగి FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_వస్తువులు(taglist, అంశం)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_రకం;
	పూర్ణాంకం		len;
	dict_iterator_T	iter;
	char_u		*dict_కీలకం;
	typval_T	*tv;
	పూర్ణాంకం		has_extra = 0;
	పూర్ణాంకం		name_only = flags & ట్యాగ్_పేర్లు;

	ఒకవేళ (అంశం->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_విలువ_from_tagfunc));
	    ఆపు;
	}

# ifdef FEAT_EMACS_ట్యాగులు
	len = 3;
# లేకపోతే
	len = 2;
# endif
	res_name = ఖాళీ;
	res_fname = ఖాళీ;
	res_cmd = ఖాళీ;
	res_రకం = ఖాళీ;

	dict_iterate_ప్రారంభం(&అంశం->li_tv, &iter);
	అయితే (ఖాళీ != (dict_కీలకం = dict_iterate_తదుపరి(&iter, &tv)))
	{
	    ఒకవేళ (tv->v_type != VAR_STRING || tv->vval.v_string == ఖాళీ)
		కొనసాగు;

	    len += (పూర్ణాంకం)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ఒకవేళ (STRCMP(dict_కీలకం, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		కొనసాగు;
	    }
	    ఒకవేళ (STRCMP(dict_కీలకం, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		కొనసాగు;
	    }
	    ఒకవేళ (STRCMP(dict_కీలకం, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		కొనసాగు;
	    }
	    has_extra = 1;
	    ఒకవేళ (STRCMP(dict_కీలకం, "kind") == 0)
	    {
		res_రకం = tv->vval.v_string;
		కొనసాగు;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (పూర్ణాంకం)STRLEN(dict_కీలకం) + 1;
	}

	ఒకవేళ (has_extra)
	    len += 2;	// need space for ;"

	ఒకవేళ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_విలువ_from_tagfunc));
	    ఆపు;
	}

	tagcmd_T cmdkind = tagfunc_cmd_రకం(res_cmd);
	ఒకవేళ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_విలువ_from_tagfunc));
	    ఆపు;
	}
	ఒకవేళ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ఒకవేళ (name_only)
	    mfp = vim_strsave(res_name);
	లేకపోతే
	    mfp = alloc(పరిమాణం(char_u) + len + 1);

	ఒకవేళ (mfp == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (!name_only)
	{
	    char_u *p = mfp;

	    *p++ = MT_GL_OTH + 1;   // mtt
	    *p++ = ట్యాగ్_SEP;	    // no tag file name
# ifdef FEAT_EMACS_ట్యాగులు
	    *p++ = ట్యాగ్_SEP;
# endif

	    STRCPY(p, res_name);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_fname);
	    p += STRLEN(p);

	    *p++ = TAB;
	    STRCPY(p, res_cmd);
	    p += STRLEN(p);

	    ఒకవేళ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ఒకవేళ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ఒకవేళ (res_రకం)
		{
		    *p++ = TAB;
		    STRCPY(p, res_రకం);
		    p += STRLEN(p);
		}

		dict_iterate_ప్రారంభం(&అంశం->li_tv, &iter);
		అయితే (ఖాళీ != (dict_కీలకం = dict_iterate_తదుపరి(&iter, &tv)))
		{
		    ఒకవేళ (tv->v_type != VAR_STRING || tv->vval.v_string == ఖాళీ)
			కొనసాగు;

		    ఒకవేళ (STRCMP(dict_కీలకం, "name") == 0)
			కొనసాగు;
		    ఒకవేళ (STRCMP(dict_కీలకం, "filename") == 0)
			కొనసాగు;
		    ఒకవేళ (STRCMP(dict_కీలకం, "cmd") == 0)
			కొనసాగు;
		    ఒకవేళ (STRCMP(dict_కీలకం, "kind") == 0)
			కొనసాగు;

		    *p++ = TAB;
		    STRCPY(p, dict_కీలకం);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	ఒకవేళ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_దత్తాంశం))[ga->ga_len++] = mfp;
	    ++ntags;
	    ఫలితం = OK;
	}
	లేకపోతే
	{
	    vim_free(mfp);
	    ఆపు;
	}
    }

    clear_tv(&rettv);

    *సరిపోలిక_లెక్క = ntags;
    తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * State information used during a tag search
 */
రకనిర్వచనం నిర్మాణం
{
    tagsearch_state_T	state;		// tag search state
    పూర్ణాంకం		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    పూర్ణాంకం		lbuf_కొలత;		// length of lbuf
    char_u	*ట్యాగ్_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    పూర్ణాంకం		flags;			// flags used for tag search
    పూర్ణాంకం		ట్యాగ్_file_sorted;	// !_TAG_FILE_SORTED value
    పూర్ణాంకం		get_searchpat;		// used for 'showfulltag'
    పూర్ణాంకం		help_only;		// only search for help tags
    పూర్ణాంకం		did_open;		// did open a tag file
    పూర్ణాంకం		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    పూర్ణాంకం		రేఖీయ;			// do a linear search
    vimconv_T	vimconv;
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    పూర్ణాంకం		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    పూర్ణాంకం		help_pri;		// help language priority
    char_u	*help_lang_కనుగొను;	// lang to be found
    పూర్ణాంకం		is_txt;			// flag of file extension
#అయితే_ముగింపు
    పూర్ణాంకం		సరిపోలిక_లెక్క;		// number of matches found
    garray_T	ga_సరిపోలిక[MT_లెక్క];	// stores matches in sequence
    hashtab_T	ht_సరిపోలిక[MT_లెక్క];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    స్థిర పూర్ణాంకం
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    పూర్ణాంకం			flags,
    పూర్ణాంకం			mincount)
{
    పూర్ణాంకం		mtt;

    st->ట్యాగ్_fname = alloc(MAXPATHL + 1);
    st->fp = ఖాళీ;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (పూర్ణాంకం)STRLEN(pat);
    st->orgpat->regmatch.regprog = ఖాళీ;
    st->flags = flags;
    st->ట్యాగ్_file_sorted = NUL;
    st->help_only = (flags & ట్యాగ్_HELP);
    st->get_searchpat = FALSE;
#నిర్వచితమైతే FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_కనుగొను = ఖాళీ;
    st->is_txt = FALSE;
#అయితే_ముగింపు
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_కొలత = LSIZE;
    st->lbuf = alloc(st->lbuf_కొలత);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    st->ebuf = alloc(LSIZE);
#అయితే_ముగింపు
    st->సరిపోలిక_లెక్క = 0;
    st->stop_searching = FALSE;

    కొరకు (mtt = 0; mtt < MT_లెక్క; ++mtt)
    {
	ga_init2(&st->ga_సరిపోలిక[mtt], పరిమాణం(char_u *), 100);
	hash_init(&st->ht_సరిపోలిక[mtt]);
    }

    // check for out of memory situation
    ఒకవేళ (st->ట్యాగ్_fname == ఖాళీ
	    || st->lbuf == ఖాళీ
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	    || st->ebuf == ఖాళీ
#అయితే_ముగింపు
       )
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Free the state used by find_tags()
 */
    స్థిర శూన్య
findtags_state_free(findtags_state_T *st)
{
    vim_free(st->ట్యాగ్_fname);
    vim_free(st->lbuf);
    vim_regfree(st->orgpat->regmatch.regprog);
    vim_free(st->orgpat);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    vim_free(st->ebuf);
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_MULTI_LANG
/*
 * Initialize the language and priority used for searching tags in a Vim help
 * file.
 * Returns TRUE to process the help file for tags and FALSE to skip the file.
 */
    స్థిర పూర్ణాంకం
findtags_లోపల_help_init(findtags_state_T *st)
{
    పూర్ణాంకం		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ఒకవేళ (st->is_txt)
	STRCPY(st->help_lang, "en");
    లేకపోతే
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (పూర్ణాంకం)STRLEN(st->ట్యాగ్_fname);
	ఒకవేళ (i > 3 && st->ట్యాగ్_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->ట్యాగ్_fname + i - 2, 2);
	లేకపోతే
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ఒకవేళ (st->help_lang_కనుగొను != ఖాళీ
	    && STRICMP(st->help_lang, st->help_lang_కనుగొను) != 0)
	తిరిగి FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ఒకవేళ ((st->flags & ట్యాగ్_KEEP_LANG)
	    && st->help_lang_కనుగొను == ఖాళీ
	    && curbuf->b_fname != ఖాళీ
	    && (i = (పూర్ణాంకం)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    లేకపోతే
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	కొరకు (s = p_hlg; *s != NUL; ++s)
	{
	    ఒకవేళ (STRNICMP(s, st->help_lang, 2) == 0)
		ఆపు;
	    ++st->help_pri;
	    ఒకవేళ ((s = vim_strchr(s, ',')) == ఖాళీ)
		ఆపు;
	}
	ఒకవేళ (s == ఖాళీ || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ఒకవేళ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    తిరిగి TRUE;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
/*
 * Use the function set in 'tagfunc' (if configured and enabled) to get the
 * tags.
 * Return OK if at least 1 tag has been successfully found, NOTDONE if the
 * 'tagfunc' is not used, still executing or the 'tagfunc' returned v:null and
 * FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    పూర్ణాంకం		use_tfu = ((st->flags & ట్యాగ్_NO_TAGFUNC) == 0);
    పూర్ణాంకం		retval;

    ఒకవేళ (!use_tfu || tfu_లోపల_use || *curbuf->b_p_tfu == NUL)
	తిరిగి NOTDONE;

    tfu_లోపల_use = TRUE;
    retval = కనుగొను_tagfunc_ట్యాగులు(pat, st->ga_సరిపోలిక, &st->సరిపోలిక_లెక్క,
						st->flags, buf_ffname);
    tfu_లోపల_use = FALSE;

    తిరిగి retval;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_కొలత 42
స్థిర నిర్మాణం
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_కొలత];
స్థిర పూర్ణాంకం incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    స్థిర శూన్య
emacs_ట్యాగులు_incstack_free(శూన్య)
{
    అయితే (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = ఖాళీ;
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
    స్థిర శూన్య
emacs_ట్యాగులు_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ఒకవేళ (vim_fgets(st->ebuf, LSIZE, st->fp))
	తిరిగి;

    కొరకు (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ఒకవేళ (*p == NUL)
	తిరిగి;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ఒకవేళ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_కొలత)
	తిరిగి;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->ట్యాగ్_fname);
    ఒకవేళ (incstack[incstack_idx].etag_fname == ఖాళీ)
	తిరిగి;

    incstack[incstack_idx].fp = st->fp;
    st->fp = ఖాళీ;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_ట్యాగ్_fname(st->ebuf, st->ట్యాగ్_fname, FALSE);
    ఒకవేళ (fullpath_ebuf != ఖాళీ)
    {
	st->fp = mch_fopen((అక్షరం *)fullpath_ebuf, "r");
	ఒకవేళ (st->fp != ఖాళీ)
	{
	    ఒకవేళ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_ట్యాగ్_file_మార్గం_truncated_for_str), st->ebuf);
	    vim_strncpy(st->ట్యాగ్_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ఒకవేళ (st->fp == ఖాళీ)
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
    స్థిర పూర్ణాంకం
emacs_ట్యాగులు_file_eof(findtags_state_T *st)
{
    ఒకవేళ (!incstack_idx)	// reached end of file. stop processing.
	తిరిగి FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->ట్యాగ్_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    తిరిగి TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    స్థిర పూర్ణాంకం
emacs_ట్యాగులు_parse_పంక్తి(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ఒకవేళ (p_7f == ఖాళీ)
    {
etag_fail:
	ఒకవేళ (vim_strchr(lbuf, '\n') != ఖాళీ)
	    తిరిగి FAIL;

	// Truncated line.  Ignore it.
	ఒకవేళ (p_verbose >= 5)
	{
	    verbose_enter();
	    సందేశం(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_చివర = lbuf;
	తిరిగి OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ఒకవేళ (p == ఖాళీ)
	p = p_7f + 1;
    లేకపోతే
	++p;

    ఒకవేళ (!VIM_ISDIGIT(*p))	    // check for start of line number
	వెళ్ళు etag_fail;
    tagp->command = p;

    ఒకవేళ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_చివర = p - 1;
    }
    లేకపోతే			    // second format: isolate tagname
    {
	ఒకవేళ (p_7f == lbuf)
	    వెళ్ళు etag_fail;

	// find end of tagname
	కొరకు (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ఒకవేళ (p == lbuf)
		వెళ్ళు etag_fail;
	tagp->tagname_చివర = p + 1;
	అయితే (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Read the next line from a tags file.
 * Returns TAGS_READ_SUCCESS if a tags line is successfully read and should be
 * processed.
 * Returns TAGS_READ_EOF if the end of file is reached.
 * Returns TAGS_READ_IGNORE if the current line should be ignored (used when
 * reached end of a emacs included tags file)
 */
    స్థిర ట్యాగులు_read_స్థితి_T
findtags_get_తదుపరి_పంక్తి(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    పూర్ణాంకం		eof;
    off_T	స్థానభ్రంశం;

    // For binary search: compute the next offset to use.
    ఒకవేళ (st->state == TS_ద్విమాన)
    {
	స్థానభ్రంశం = sinfo_p->దిగువ_స్థానభ్రంశం + ((sinfo_p->ఎగువ_స్థానభ్రంశం
						- sinfo_p->దిగువ_స్థానభ్రంశం) / 2);
	ఒకవేళ (స్థానభ్రంశం == sinfo_p->curr_స్థానభ్రంశం)
	    తిరిగి ట్యాగులు_READ_EOF; // End the binary search without a match.
	లేకపోతే
	    sinfo_p->curr_స్థానభ్రంశం = స్థానభ్రంశం;
    }

    // Skipping back (after a match during binary search).
    లేకపోతే ఒకవేళ (st->state == TS_SKIP_వెనుక)
    {
	sinfo_p->curr_స్థానభ్రంశం -= st->lbuf_కొలత * 2;
	ఒకవేళ (sinfo_p->curr_స్థానభ్రంశం < 0)
	{
	    sinfo_p->curr_స్థానభ్రంశం = 0;
	    rewind(st->fp);
	    st->state = TS_అడుగు_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ఒకవేళ (st->state == TS_ద్విమాన || st->state == TS_SKIP_వెనుక)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_స్థానభ్రంశం_వాడిన = sinfo_p->curr_స్థానభ్రంశం;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_స్థానభ్రంశం, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_కొలత, st->fp);
	ఒకవేళ (!eof && sinfo_p->curr_స్థానభ్రంశం != 0)
	{
	    sinfo_p->curr_స్థానభ్రంశం = vim_ftell(st->fp);
	    ఒకవేళ (sinfo_p->curr_స్థానభ్రంశం == sinfo_p->ఎగువ_స్థానభ్రంశం)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->దిగువ_స్థానభ్రంశం, SEEK_SET);
		sinfo_p->curr_స్థానభ్రంశం = sinfo_p->దిగువ_స్థానభ్రంశం;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_కొలత, st->fp);
	}
	// skip empty and blank lines
	అయితే (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_స్థానభ్రంశం = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_కొలత, st->fp);
	}
	ఒకవేళ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_వెనుక;
	    sinfo_p->సరిపోలిక_స్థానభ్రంశం = vim_ftell(st->fp);
	    sinfo_p->curr_స్థానభ్రంశం = sinfo_p->curr_స్థానభ్రంశం_వాడిన;
	    తిరిగి ట్యాగులు_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    లేకపోతే
    {
	// skip empty and blank lines
	చేయి
	{
#నిర్వచితమైతే FEAT_CSCOPE
	    ఒకవేళ (st->flags & ట్యాగ్_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_కొలత);
	    లేకపోతే
#అయితే_ముగింపు
		eof = vim_fgets(st->lbuf, st->lbuf_కొలత, st->fp);
	} అయితే (!eof && vim_isblankline(st->lbuf));

	ఒకవేళ (eof)
	{
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	    ఒకవేళ (emacs_ట్యాగులు_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		తిరిగి ట్యాగులు_READ_IGNORE;
#అయితే_ముగింపు
	    తిరిగి ట్యాగులు_READ_EOF;
	}
    }

    తిరిగి ట్యాగులు_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    స్థిర పూర్ణాంకం
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ఒకవేళ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	తిరిగి TRUE;

    // Process the header line.
    ఒకవేళ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->ట్యాగ్_file_sorted = st->lbuf[18];
    ఒకవేళ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	కొరకు (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_అమరిక(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    తిరిగి FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    స్థిర పూర్ణాంకం
findtags_ప్రారంభం_state_handler(
    findtags_state_T	*st,
    పూర్ణాంకం			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం		use_cscope = (st->flags & ట్యాగ్_CSCOPE);
#అయితే_ముగింపు
    పూర్ణాంకం		noic = (st->flags & ట్యాగ్_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ఒకవేళ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	తిరిగి findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#నిర్వచితమైతే FEAT_CSCOPE
    ఒకవేళ (st->రేఖీయ || use_cscope)
#లేకపోతే
    ఒకవేళ (st->రేఖీయ)
#అయితే_ముగింపు
	st->state = TS_రేఖీయ;
    లేకపోతే ఒకవేళ (st->ట్యాగ్_file_sorted == NUL)
	st->state = TS_ద్విమాన;
    లేకపోతే ఒకవేళ (st->ట్యాగ్_file_sorted == '1')
	st->state = TS_ద్విమాన;
    లేకపోతే ఒకవేళ (st->ట్యాగ్_file_sorted == '2')
    {
	st->state = TS_ద్విమాన;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    లేకపోతే
	st->state = TS_రేఖీయ;

    ఒకవేళ (st->state == TS_ద్విమాన && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->రేఖీయ = TRUE;
	st->state = TS_రేఖీయ;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ఒకవేళ (st->state == TS_ద్విమాన)
    {
	ఒకవేళ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_రేఖీయ;
	లేకపోతే
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->దిగువ_స్థానభ్రంశం = 0;
	    sinfo_p->దిగువ_char = 0;
	    sinfo_p->ఎగువ_స్థానభ్రంశం = filesize;
	    sinfo_p->curr_స్థానభ్రంశం = 0;
	    sinfo_p->ఎగువ_char = 0xff;
	}
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
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
    స్థిర tagmatch_స్థితి_T
findtags_parse_పంక్తి(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_సరిపోలిక_వాదనలు_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    పూర్ణాంకం		స్థితి_2;
    పూర్ణాంకం		i;
    పూర్ణాంకం		cmplen;
    పూర్ణాంకం		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ఒకవేళ (st->orgpat->headlen
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	    && !st->is_etag
#అయితే_ముగింపు
       )
    {
	CLEAR_క్షేత్రం(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_చివర = vim_strchr(st->lbuf, TAB);
	ఒకవేళ (tagpp->tagname_చివర == ఖాళీ)
	    // Corrupted tag line.
	    తిరిగి ట్యాగ్_సరిపోలిక_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (పూర్ణాంకం)(tagpp->tagname_చివర - tagpp->tagname);
	ఒకవేళ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ఒకవేళ ((st->flags & ట్యాగ్_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	లేకపోతే ఒకవేళ (st->state == TS_రేఖీయ && st->orgpat->headlen != cmplen)
	    తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;

	ఒకవేళ (st->state == TS_ద్విమాన)
	{
	    // Simplistic check for unsorted tags file.
	    i = (పూర్ణాంకం)tagpp->tagname[0];
	    ఒకవేళ (margs->sortic)
		i = (పూర్ణాంకం)TOUPPER_ASC(tagpp->tagname[0]);
	    ఒకవేళ (i < sinfo_p->దిగువ_char || i > sinfo_p->ఎగువ_char)
		margs->క్రమీకరించు_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ఒకవేళ (margs->sortic)
		tagcmp = ట్యాగ్_strnicmp(tagpp->tagname, st->orgpat->తల,
							(size_t)cmplen);
	    లేకపోతే
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->తల, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ఒకవేళ (tagcmp == 0)
	    {
		ఒకవేళ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		లేకపోతే ఒకవేళ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ఒకవేళ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_వెనుక;
		sinfo_p->సరిపోలిక_స్థానభ్రంశం = sinfo_p->curr_స్థానభ్రంశం;
		తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;
	    }
	    ఒకవేళ (tagcmp < 0)
	    {
		sinfo_p->curr_స్థానభ్రంశం = vim_ftell(st->fp);
		ఒకవేళ (sinfo_p->curr_స్థానభ్రంశం < sinfo_p->ఎగువ_స్థానభ్రంశం)
		{
		    sinfo_p->దిగువ_స్థానభ్రంశం = sinfo_p->curr_స్థానభ్రంశం;
		    ఒకవేళ (margs->sortic)
			sinfo_p->దిగువ_char = TOUPPER_ASC(tagpp->tagname[0]);
		    లేకపోతే
			sinfo_p->దిగువ_char = tagpp->tagname[0];
		    తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;
		}
	    }
	    ఒకవేళ (tagcmp > 0 && sinfo_p->curr_స్థానభ్రంశం != sinfo_p->ఎగువ_స్థానభ్రంశం)
	    {
		sinfo_p->ఎగువ_స్థానభ్రంశం = sinfo_p->curr_స్థానభ్రంశం;
		ఒకవేళ (margs->sortic)
		    sinfo_p->ఎగువ_char = TOUPPER_ASC(tagpp->tagname[0]);
		లేకపోతే
		    sinfo_p->ఎగువ_char = tagpp->tagname[0];
		తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;
	    }

	    // No match yet and are at the end of the binary search.
	    తిరిగి ట్యాగ్_సరిపోలిక_STOP;
	}
	లేకపోతే ఒకవేళ (st->state == TS_SKIP_వెనుక)
	{
	    ఒకవేళ (MB_STRNICMP(tagpp->tagname, st->orgpat->తల, cmplen) != 0)
		st->state = TS_అడుగు_FORWARD;
	    లేకపోతే
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_స్థానభ్రంశం = sinfo_p->curr_స్థానభ్రంశం_వాడిన;
	    తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;
	}
	లేకపోతే ఒకవేళ (st->state == TS_అడుగు_FORWARD)
	{
	    ఒకవేళ (MB_STRNICMP(tagpp->tagname, st->orgpat->తల, cmplen) != 0)
	    {
		ఒకవేళ ((off_T)vim_ftell(st->fp) > sinfo_p->సరిపోలిక_స్థానభ్రంశం)
		    తిరిగి ట్యాగ్_సరిపోలిక_STOP;	// past last match
		లేకపోతే
		    తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;	// before first match
	    }
	}
	లేకపోతే
	    // skip this match if it can't match
	    ఒకవేళ (MB_STRNICMP(tagpp->tagname, st->orgpat->తల, cmplen) != 0)
		తిరిగి ట్యాగ్_సరిపోలిక_తదుపరి;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_చివర + 1;
	tagpp->fname_చివర = vim_strchr(tagpp->fname, TAB);
	ఒకవేళ (tagpp->fname_చివర == ఖాళీ)
	    స్థితి_2 = FAIL;
	లేకపోతే
	{
	    tagpp->command = tagpp->fname_చివర + 1;
	    స్థితి_2 = OK;
	}
    }
    లేకపోతే
	స్థితి_2 = parse_ట్యాగ్_పంక్తి(st->lbuf,
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
		st->is_etag,
#అయితే_ముగింపు
		tagpp);

    ఒకవేళ (స్థితి_2 == FAIL)
	తిరిగి ట్యాగ్_సరిపోలిక_FAIL;

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (st->is_etag)
	tagpp->fname = st->ebuf;
#అయితే_ముగింపు

    తిరిగి ట్యాగ్_సరిపోలిక_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    స్థిర శూన్య
findtags_matchargs_init(findtags_సరిపోలిక_వాదనలు_T *margs, పూర్ణాంకం flags)
{
    margs->matchoff = 0;			// match offset
    margs->సరిపోలిక_re = FALSE;			// match with regexp
    margs->సరిపోలిక_no_ic = FALSE;			// matches with case
    margs->has_re = (flags & ట్యాగ్_REGEXP);	// regexp used
    margs->sortic = FALSE;			// tag file sorted in nocase
    margs->క్రమీకరించు_error = FALSE;			// tags file not sorted
}

/*
 * Compares the tag name in "tagpp->tagname" with a search pattern in
 * "st->orgpat->pat".
 * Returns TRUE if the tag matches, FALSE if the tag doesn't match.
 * Uses the values in "margs" for doing the comparison.
 */
    స్థిర పూర్ణాంకం
findtags_సరిపోలిక_ట్యాగ్(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_సరిపోలిక_వాదనలు_T *margs)
{
    పూర్ణాంకం		సరిపోలిక = FALSE;
    పూర్ణాంకం		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (పూర్ణాంకం)(tagpp->tagname_చివర - tagpp->tagname);
    ఒకవేళ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ఒకవేళ (st->orgpat->len != cmplen)
	సరిపోలిక = FALSE;
    లేకపోతే
    {
	ఒకవేళ (st->orgpat->regmatch.rm_ic)
	{
	    సరిపోలిక =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ఒకవేళ (సరిపోలిక)
		margs->సరిపోలిక_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	లేకపోతే
	    సరిపోలిక = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->సరిపోలిక_re = FALSE;
    ఒకవేళ (!సరిపోలిక && st->orgpat->regmatch.regprog != ఖాళీ)
    {
	పూర్ణాంకం	cc;

	cc = *tagpp->tagname_చివర;
	*tagpp->tagname_చివర = NUL;
	సరిపోలిక = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ఒకవేళ (సరిపోలిక)
	{
	    margs->matchoff = (పూర్ణాంకం)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ఒకవేళ (st->orgpat->regmatch.rm_ic)
	    {
		st->orgpat->regmatch.rm_ic = FALSE;
		margs->సరిపోలిక_no_ic = vim_regexec(&st->orgpat->regmatch,
			tagpp->tagname, (colnr_T)0);
		st->orgpat->regmatch.rm_ic = TRUE;
	    }
	}
	*tagpp->tagname_చివర = cc;
	margs->సరిపోలిక_re = TRUE;
    }

    తిరిగి సరిపోలిక;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    స్థిర శూన్య
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_పంక్తి;
    పూర్ణాంకం		len;

    conv_పంక్తి = string_convert(&st->vimconv, st->lbuf, ఖాళీ);
    ఒకవేళ (conv_పంక్తి == ఖాళీ)
	తిరిగి;

    // Copy or swap lbuf and conv_line.
    len = (పూర్ణాంకం)STRLEN(conv_పంక్తి) + 1;
    ఒకవేళ (len > st->lbuf_కొలత)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_పంక్తి;
	st->lbuf_కొలత = len;
    }
    లేకపోతే
    {
	STRCPY(st->lbuf, conv_పంక్తి);
	vim_free(conv_పంక్తి);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    స్థిర పూర్ణాంకం
findtags_కూడు_సరిపోలిక(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_సరిపోలిక_వాదనలు_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం		use_cscope = (st->flags & ట్యాగ్_CSCOPE);
#అయితే_ముగింపు
    పూర్ణాంకం		name_only = (st->flags & ట్యాగ్_పేర్లు);
    పూర్ణాంకం		mtt;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		is_current;		// file name matches
    పూర్ణాంకం		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#నిర్వచితమైతే FEAT_CSCOPE
    ఒకవేళ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	// Decide in which array to store this match.
	is_current = test_for_current(
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
		st->is_etag,
#అయితే_ముగింపు
		tagpp->fname, tagpp->fname_చివర, st->ట్యాగ్_fname, buf_ffname);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	is_static = FALSE;
	ఒకవేళ (!st->is_etag)	// emacs tags are never static
#అయితే_ముగింపు
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	//సరిపోలిక
	ఒకవేళ (is_static)
	{
	    ఒకవేళ (is_current)
		mtt = MT_ST_CUR;
	    లేకపోతే
		mtt = MT_ST_OTH;
	}
	లేకపోతే
	{
	    ఒకవేళ (is_current)
		mtt = MT_GL_CUR;
	    లేకపోతే
		mtt = MT_GL_OTH;
	}
	ఒకవేళ (st->orgpat->regmatch.rm_ic && !margs->సరిపోలిక_no_ic)
	    mtt += MT_IC_OFF;
	ఒకవేళ (margs->సరిపోలిక_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ఒకవేళ (st->help_only)
    {
#నిర్వచితమైతే FEAT_MULTI_LANG
# define ML_EXTRA 3
#లేకపోతే
# define ML_EXTRA 0
#అయితే_ముగింపు
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_చివర = NUL;
	len = (పూర్ణాంకం)(tagpp->tagname_చివర - tagpp->tagname);
	mfp = alloc(పరిమాణం(char_u) + len + 10 + ML_EXTRA + 1);
	ఒకవేళ (mfp != ఖాళీ)
	{
	    పూర్ణాంకం heuristic;

	    p = mfp;
	    STRCPY(p, tagpp->tagname);
#నిర్వచితమైతే FEAT_MULTI_LANG
	    p[len] = '@';
	    STRCPY(p + len + 1, st->help_lang);
#అయితే_ముగింపు

	    heuristic = help_heuristic(tagpp->tagname,
				margs->సరిపోలిక_re ? margs->matchoff : 0,
				!margs->సరిపోలిక_no_ic);
#నిర్వచితమైతే FEAT_MULTI_LANG
	    heuristic += st->help_pri;
#అయితే_ముగింపు
	    sprintf((అక్షరం *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_చివర = TAB;
    }
    లేకపోతే ఒకవేళ (name_only)
    {
	ఒకవేళ (st->get_searchpat)
	{
	    char_u *తాత్కాలిక_చివర = tagpp->command;

	    ఒకవేళ (*తాత్కాలిక_చివర == '/')
		అయితే (*తాత్కాలిక_చివర && *తాత్కాలిక_చివర != '\r'
			&& *తాత్కాలిక_చివర != '\n'
			&& *తాత్కాలిక_చివర != '$')
		    తాత్కాలిక_చివర++;

	    ఒకవేళ (tagpp->command + 2 < తాత్కాలిక_చివర)
	    {
		len = (పూర్ణాంకం)(తాత్కాలిక_చివర - tagpp->command - 2);
		mfp = alloc(len + 2);
		ఒకవేళ (mfp != ఖాళీ)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    లేకపోతే
		mfp = ఖాళీ;
	    st->get_searchpat = FALSE;
	}
	లేకపోతే
	{
	    len = (పూర్ణాంకం)(tagpp->tagname_చివర - tagpp->tagname);
	    mfp = alloc(పరిమాణం(char_u) + len + 1);
	    ఒకవేళ (mfp != ఖాళీ)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ఒకవేళ (State & బహుళకం_చొప్పించు)
		st->get_searchpat = p_sft;
	}
    }
    లేకపోతే
    {
	size_t ట్యాగ్_fname_len = STRLEN(st->ట్యాగ్_fname);
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	size_t ebuf_len = 0;
#అయితే_ముగింపు

	// Save the tag in a buffer.
	// Use 0x02 to separate fields (Can't use NUL because the
	// hash key is terminated by NUL, or Ctrl_A because that is
	// part of some Emacs tag files -- see parse_tag_line).
	// Emacs tag: <mtt><tag_fname><0x02><ebuf><0x02><lbuf><NUL>
	// other tag: <mtt><tag_fname><0x02><0x02><lbuf><NUL>
	// without Emacs tags: <mtt><tag_fname><0x02><lbuf><NUL>
	// Here <mtt> is the "mtt" value plus 1 to avoid NUL.
	len = (పూర్ణాంకం)ట్యాగ్_fname_len + (పూర్ణాంకం)STRLEN(st->lbuf) + 3;
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	ఒకవేళ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (పూర్ణాంకం)ebuf_len + 1;
	}
	లేకపోతే
	    ++len;
#అయితే_ముగింపు
	mfp = alloc(పరిమాణం(char_u) + len + 1);
	ఒకవేళ (mfp != ఖాళీ)
	{
	    p = mfp;
	    p[0] = mtt + 1;
	    STRCPY(p + 1, st->ట్యాగ్_fname);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    // Ignore differences in slashes, avoid adding
	    // both path/file and path\file.
	    slash_adjust(p + 1);
#అయితే_ముగింపు
	    p[ట్యాగ్_fname_len + 1] = ట్యాగ్_SEP;
	    s = p + 1 + ట్యాగ్_fname_len + 1;
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	    ఒకవేళ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = ట్యాగ్_SEP;
		s += ebuf_len + 1;
	    }
	    లేకపోతే
		*s++ = ట్యాగ్_SEP;
#అయితే_ముగింపు
	    STRCPY(s, st->lbuf);
	}
    }

    ఒకవేళ (mfp != ఖాళీ)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#నిర్వచితమైతే FEAT_CSCOPE
	ఒకవేళ (use_cscope)
	    ++*hash;
	లేకపోతే
#అయితే_ముగింపు
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_సరిపోలిక[mtt], mfp, *hash);
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	{
	    ఒకవేళ (hash_కూడు_అంశం(&st->ht_సరిపోలిక[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_సరిపోలిక[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		తిరిగి FAIL;
	    }

	    ((char_u **)(st->ga_సరిపోలిక[mtt].ga_దత్తాంశం))
		[st->ga_సరిపోలిక[mtt].ga_len++] = mfp;
	    st->సరిపోలిక_లెక్క++;
	}
	లేకపోతే
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    తిరిగి OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    స్థిర శూన్య
findtags_get_all_ట్యాగులు(
    findtags_state_T		*st,
    findtags_సరిపోలిక_వాదనలు_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	శోధన_info;
    పూర్ణాంకం			retval;
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం			use_cscope = (st->flags & ట్యాగ్_CSCOPE);
#అయితే_ముగింపు
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_క్షేత్రం(శోధన_info);

    // Read and parse the lines in the file one by one
    కొరకు (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ఒకవేళ (st->state == TS_ద్విమాన || st->state == TS_SKIP_వెనుక)
	    పంక్తి_breakcheck();
	లేకపోతే
	    fast_breakcheck();
	ఒకవేళ ((st->flags & ట్యాగ్_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_కీలకాలు(30, FALSE);
	ఒకవేళ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    ఆపు;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ఒకవేళ (st->mincount == ట్యాగ్_MANY && st->సరిపోలిక_లెక్క >= ట్యాగ్_MANY)
	{
	    st->stop_searching = TRUE;
	    ఆపు;
	}
	ఒకవేళ (st->get_searchpat)
	    వెళ్ళు పంక్తి_read_లోపల;

	retval = findtags_get_తదుపరి_పంక్తి(st, &శోధన_info);
	ఒకవేళ (retval == ట్యాగులు_READ_IGNORE)
	    కొనసాగు;
	ఒకవేళ (retval == ట్యాగులు_READ_EOF)
	    ఆపు;

పంక్తి_read_లోపల:

	ఒకవేళ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ఒకవేళ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_రేఖీయ;
	    emacs_ట్యాగులు_new_filename(st);
	    కొనసాగు;
	}
#అయితే_ముగింపు

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ఒకవేళ (st->state == TS_ప్రారంభం)
	{
	    ఒకవేళ (findtags_ప్రారంభం_state_handler(st, &margs->sortic, &శోధన_info) == FALSE)
		కొనసాగు;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ఒకవేళ (st->lbuf[st->lbuf_కొలత - 2] != NUL
#నిర్వచితమైతే FEAT_CSCOPE
		&& !use_cscope
#అయితే_ముగింపు
	   )
	{
	    st->lbuf_కొలత *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_కొలత);
	    ఒకవేళ (st->lbuf == ఖాళీ)
	    {
		ఒకవేళ (st->fp != ఖాళీ)
		    fclose(st->fp);
		st->fp = ఖాళీ;
		st->stop_searching = TRUE;
		తిరిగి;
	    }

	    ఒకవేళ (st->state == TS_అడుగు_FORWARD || st->state == TS_రేఖీయ)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, శోధన_info.curr_స్థానభ్రంశం,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    శోధన_info.curr_స్థానభ్రంశం = 0;
	    కొనసాగు;
	}

	retval = findtags_parse_పంక్తి(st, &tagp, margs, &శోధన_info);
	ఒకవేళ (retval == ట్యాగ్_సరిపోలిక_తదుపరి)
	    కొనసాగు;
	ఒకవేళ (retval == ట్యాగ్_సరిపోలిక_STOP)
	    ఆపు;
	ఒకవేళ (retval == ట్యాగ్_సరిపోలిక_FAIL)
	{
	    semsg(_(e_format_error_లోపల_ట్యాగులు_file_str), st->ట్యాగ్_fname);
#నిర్వచితమైతే FEAT_CSCOPE
	    ఒకవేళ (!use_cscope)
#అయితే_ముగింపు
		semsg(_("Before byte %ld"), (దీర్ఘ)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    తిరిగి;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ఒకవేళ (findtags_సరిపోలిక_ట్యాగ్(st, &tagp, margs))
	{
	    ఒకవేళ (findtags_కూడు_సరిపోలిక(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		ఆపు;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    స్థిర శూన్య
findtags_లోపల_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_సరిపోలిక_వాదనలు_T margs;
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం		use_cscope = (st->flags & ట్యాగ్_CSCOPE);
#అయితే_ముగింపు

    st->vimconv.vc_type = CONV_NONE;
    st->ట్యాగ్_file_sorted = NUL;
    st->fp = ఖాళీ;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#నిర్వచితమైతే FEAT_CSCOPE
    ఒకవేళ (use_cscope)
	st->fp = ఖాళీ;	    // avoid GCC warning
    లేకపోతే
#అయితే_ముగింపు
    {
#నిర్వచితమైతే FEAT_MULTI_LANG
	ఒకవేళ (curbuf->b_help)
	{
	    ఒకవేళ (!findtags_లోపల_help_init(st))
		తిరిగి;
	}
#అయితే_ముగింపు

	st->fp = mch_fopen((అక్షరం *)st->ట్యాగ్_fname, "r");
	ఒకవేళ (st->fp == ఖాళీ)
	    తిరిగి;

	ఒకవేళ (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->ట్యాగ్_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_ప్రారంభం;	// we're at the start of the file
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    st->is_etag = FALSE;	// default is: not emacs style
#అయితే_ముగింపు

    // Read and parse the lines in the file one by one
    findtags_get_all_ట్యాగులు(st, &margs, buf_ffname);

    ఒకవేళ (st->fp != ఖాళీ)
    {
	fclose(st->fp);
	st->fp = ఖాళీ;
    }
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    emacs_ట్యాగులు_incstack_free();
#అయితే_ముగింపు
    ఒకవేళ (st->vimconv.vc_type != CONV_NONE)
	convert_అమరిక(&st->vimconv, ఖాళీ, ఖాళీ);

    ఒకవేళ (margs.క్రమీకరించు_error)
	semsg(_(e_ట్యాగులు_file_not_sorted_str), st->ట్యాగ్_fname);

    // Stop searching if sufficient tags have been found.
    ఒకవేళ (st->సరిపోలిక_లెక్క >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    స్థిర పూర్ణాంకం
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    పూర్ణాంకం		name_only = (st->flags & ట్యాగ్_పేర్లు);
    char_u	**matches;
    పూర్ణాంకం		mtt;
    పూర్ణాంకం		i;
    char_u	*mfp;
    char_u	*p;

    ఒకవేళ (st->సరిపోలిక_లెక్క > 0)
	matches = ALLOC_MULT(char_u *, st->సరిపోలిక_లెక్క);
    లేకపోతే
	matches = ఖాళీ;
    st->సరిపోలిక_లెక్క = 0;
    కొరకు (mtt = 0; mtt < MT_లెక్క; ++mtt)
    {
	కొరకు (i = 0; i < st->ga_సరిపోలిక[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_సరిపోలిక[mtt].ga_దత్తాంశం))[i];
	    ఒకవేళ (matches == ఖాళీ)
		vim_free(mfp);
	    లేకపోతే
	    {
		ఒకవేళ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    కొరకు (p = mfp + 1; *p != NUL; ++p)
			ఒకవేళ (*p == ట్యాగ్_SEP)
			    *p = NUL;
		}
		matches[st->సరిపోలిక_లెక్క++] = mfp;
	    }
	}

	ga_clear(&st->ga_సరిపోలిక[mtt]);
	hash_clear(&st->ht_సరిపోలిక[mtt]);
    }

    *matchesp = matches;
    తిరిగి st->సరిపోలిక_లెక్క;
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
    పూర్ణాంకం
కనుగొను_ట్యాగులు(
    char_u	*pat,			// pattern to search for
    పూర్ణాంకం		*అంకె_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    పూర్ణాంకం		flags,
    పూర్ణాంకం		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    పూర్ణాంకం		మొదటి_file;		// trying first tag file
    పూర్ణాంకం		retval = FAIL;		// return value
    పూర్ణాంకం		విడత;

    పూర్ణాంకం		save_emsg_off;

    bool	help_save;
#నిర్వచితమైతే FEAT_MULTI_LANG
    పూర్ణాంకం		i;
    char_u	*saved_pat = ఖాళీ;		// copy of pat[]
#అయితే_ముగింపు

    పూర్ణాంకం		findall = (mincount == MAXCOL || mincount == ట్యాగ్_MANY);
						// find all matching tags
    పూర్ణాంకం		has_re = (flags & ట్యాగ్_REGEXP);	// regexp used
    పూర్ణాంకం		noic = (flags & ట్యాగ్_NOIC);
#నిర్వచితమైతే FEAT_CSCOPE
    పూర్ణాంకం		use_cscope = (flags & ట్యాగ్_CSCOPE);
#అయితే_ముగింపు
    పూర్ణాంకం		verbose = (flags & ట్యాగ్_VERBOSE);
    పూర్ణాంకం		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    ఎంపిక (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	స్థితి TC_FOLLOWIC:		 ఆపు;
	స్థితి TC_IGNORE:    p_ic = TRUE;  ఆపు;
	స్థితి TC_సరిపోలిక:     p_ic = FALSE; ఆపు;
	స్థితి TC_FOLLOWSCS: p_ic = ignorecase(pat); ఆపు;
	స్థితి TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); ఆపు;
    }

    help_save = curbuf->b_help;

    ఒకవేళ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	వెళ్ళు findtag_చివర;

#నిర్వచితమైతే FEAT_CSCOPE
    STRCPY(st.ట్యాగ్_fname, "from cscope");	// for error messages
#అయితే_ముగింపు

    /*
     * Initialize a few variables
     */
    ఒకవేళ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#నిర్వచితమైతే FEAT_CSCOPE
    లేకపోతే ఒకవేళ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MULTI_LANG
    ఒకవేళ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ఒకవేళ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ఒకవేళ (saved_pat != ఖాళీ)
	    {
		st.help_lang_కనుగొను = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#అయితే_ముగింపు
    ఒకవేళ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ఒకవేళ (has_re && st.orgpat->regmatch.regprog == ఖాళీ)
	వెళ్ళు findtag_చివర;

#నిర్వచితమైతే FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ఒకవేళ (retval != NOTDONE)
	వెళ్ళు findtag_చివర;

    // re-initialize the default return value
    retval = FAIL;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ఒకవేళ ((flags & ట్యాగ్_KEEP_LANG)
	    && st.help_lang_కనుగొను == ఖాళీ
	    && curbuf->b_fname != ఖాళీ
	    && (i = (పూర్ణాంకం)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#అయితే_ముగింపు

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
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	st.రేఖీయ = (st.orgpat->headlen == 0 || !p_tbs || విడత == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      కొరకు (మొదటి_file = TRUE;
#నిర్వచితమైతే FEAT_CSCOPE
	    use_cscope ||
#అయితే_ముగింపు
		get_tagfname(&tn, మొదటి_file, st.ట్యాగ్_fname) == OK;
							   మొదటి_file = FALSE)
      {
	  findtags_లోపల_file(&st, buf_ffname);
	  ఒకవేళ (st.stop_searching
#నిర్వచితమైతే FEAT_CSCOPE
		  || use_cscope
#అయితే_ముగింపు
	     )
	  {
	      retval = OK;
	      ఆపు;
	  }
      } // end of for-each-file loop

#నిర్వచితమైతే FEAT_CSCOPE
	ఒకవేళ (!use_cscope)
#అయితే_ముగింపు
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ఒకవేళ (st.stop_searching || st.రేఖీయ || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    ఆపు;
#నిర్వచితమైతే FEAT_CSCOPE
	ఒకవేళ (use_cscope)
	    ఆపు;
#అయితే_ముగింపు

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ఒకవేళ (!st.stop_searching)
    {
	ఒకవేళ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_ట్యాగులు_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_చివర:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ఒకవేళ (retval == FAIL)
	st.సరిపోలిక_లెక్క = 0;

    *అంకె_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#నిర్వచితమైతే FEAT_MULTI_LANG
    vim_free(saved_pat);
#అయితే_ముగింపు

    p_ic = save_p_ic;

    తిరిగి retval;
}

స్థిర garray_T ట్యాగ్_fnames = GA_ఖాళీలు;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    స్థిర శూన్య
దొరికింది_tagfile_cb(char_u *fname, శూన్య *cookie వాడని)
{
    ఒకవేళ (ga_grow(&ట్యాగ్_fnames, 1) == FAIL)
	తిరిగి;

    char_u	*ట్యాగ్_fname = vim_strsave(fname);

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    slash_adjust(ట్యాగ్_fname);
#అయితే_ముగింపు
    simplify_filename(ట్యాగ్_fname);
    ((char_u **)(ట్యాగ్_fnames.ga_దత్తాంశం))[ట్యాగ్_fnames.ga_len++] = ట్యాగ్_fname;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_ట్యాగ్_stuff(శూన్య)
{
    ga_clear_అక్షరశ్రేణులు(&ట్యాగ్_fnames);
    ఒకవేళ (curwin != ఖాళీ)
	do_ట్యాగ్(ఖాళీ, DT_FREE, 0, 0, 0);
    ట్యాగ్_freematch();

# ఒకవేళ defined(FEAT_QUICKFIX)
    tagstack_clear_నమోదు(&ptag_నమోదు);
# endif
}
#అయితే_ముగింపు

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    పూర్ణాంకం
get_tagfname(
    tagname_T	*tnp,	// holds status info
    పూర్ణాంకం		మొదటి,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = ఖాళీ;
    char_u		*r_ptr;
    పూర్ణాంకం			i;

    ఒకవేళ (మొదటి)
	CLEAR_సూచిక(tnp);

    ఒకవేళ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ఒకవేళ (మొదటి)
	{
	    ga_clear_అక్షరశ్రేణులు(&ట్యాగ్_fnames);
	    ga_init2(&ట్యాగ్_fnames, పరిమాణం(char_u *), 10);
	    do_లోపల_runtimepath((char_u *)
#నిర్వచితమైతే FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# లేకపోతే
		    "doc/tags doc/tags-??"
# endif
#లేకపోతే
		    "doc/tags"
#అయితే_ముగింపు
					   , DIP_ALL, దొరికింది_tagfile_cb, ఖాళీ);
	}

	ఒకవేళ (tnp->tn_hf_idx >= ట్యాగ్_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ఒకవేళ (tnp->tn_hf_idx > ట్యాగ్_fnames.ga_len || *p_hf == NUL)
		తిరిగి FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    slash_adjust(buf);
#అయితే_ముగింపు
	    simplify_filename(buf);

	    కొరకు (i = 0; i < ట్యాగ్_fnames.ga_len; ++i)
		ఒకవేళ (STRCMP(buf, ((char_u **)(ట్యాగ్_fnames.ga_దత్తాంశం))[i]) == 0)
		    తిరిగి FAIL; // avoid duplicate file names
	}
	లేకపోతే
	    vim_strncpy(buf, ((char_u **)(ట్యాగ్_fnames.ga_దత్తాంశం))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	తిరిగి OK;
    }

    ఒకవేళ (మొదటి)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_ట్యాగులు = vim_strsave((*curbuf->b_p_ట్యాగులు != NUL)
						 ? curbuf->b_p_ట్యాగులు : p_ట్యాగులు);
	ఒకవేళ (tnp->tn_ట్యాగులు == ఖాళీ)
	    తిరిగి FAIL;
	tnp->tn_np = tnp->tn_ట్యాగులు;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    కొరకు (;;)
    {
	ఒకవేళ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_శోధన_ctx);
	    ఒకవేళ (fname != ఖాళీ)
		ఆపు;

	    tnp->tn_did_filefind_init = FALSE;
	}
	లేకపోతే
	{
	    char_u  *filename = ఖాళీ;

	    // Stop when used all parts of 'tags'.
	    ఒకవేళ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_శోధన_ctx);
		tnp->tn_శోధన_ctx = ఖాళీ;
		తిరిగి FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (శూన్య)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    ఒకవేళ (r_ptr != ఖాళీ)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_శోధన_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_శోధన_ctx, TRUE, curbuf->b_ffname);
	    ఒకవేళ (tnp->tn_శోధన_ctx != ఖాళీ)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    తిరిగి OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    శూన్య
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_ట్యాగులు);
    vim_findfile_cleanup(tnp->tn_శోధన_ctx);
    tnp->tn_శోధన_ctx = ఖాళీ;
    ga_clear_అక్షరశ్రేణులు(&ట్యాగ్_fnames);
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    స్థిర పూర్ణాంకం
parse_ట్యాగ్_పంక్తి(
    char_u	*lbuf,		// line to be parsed
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    పూర్ణాంకం		is_etag,
#అయితే_ముగింపు
    tagptrs_T	*tagp)
{
    char_u	*p;

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (is_etag)
	// emacs-style tag file
	తిరిగి emacs_ట్యాగులు_parse_పంక్తి(lbuf, tagp);
#అయితే_ముగింపు

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;
    tagp->tagname_చివర = p;

    // Isolate file name, from first to second white space
    ఒకవేళ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;
    tagp->fname_చివర = p;

    // find start of search command, after second white space
    ఒకవేళ (*p != NUL)
	++p;
    ఒకవేళ (*p == NUL)
	తిరిగి FAIL;
    tagp->command = p;

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    అయితే ((p = vim_strchr(p, '\t')) != ఖాళీ)
    {
	++p;
	ఒకవేళ (STRNCMP(p, "file:", 5) == 0)
	    తిరిగి TRUE;
    }

    తిరిగి FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    స్థిర size_t
matching_పంక్తి_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    p += STRLEN(p) + 1;
#అయితే_ముగింపు
    తిరిగి (p - lbuf) + STRLEN(p);
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
    స్థిర పూర్ణాంకం
parse_సరిపోలిక(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    పూర్ణాంకం		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->ట్యాగ్_fname = lbuf + 1;
    lbuf += STRLEN(tagp->ట్యాగ్_fname) + 2;
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_చివర = lbuf++;
    }
    లేకపోతే
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#అయితే_ముగింపు

    // Find search pattern and the file name for non-etags.
    retval = parse_ట్యాగ్_పంక్తి(lbuf,
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
			tagp->is_etag,
#అయితే_ముగింపు
			tagp);

    tagp->tagkind = ఖాళీ;
    tagp->user_దత్తాంశం = ఖాళీ;
    tagp->tagline = 0;
    tagp->command_చివర = ఖాళీ;

    ఒకవేళ (retval != OK)
	తిరిగి retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ఒకవేళ (కనుగొను_extra(&p) == OK)
    {
	ఒకవేళ (p > tagp->command && p[-1] == '|')
	    tagp->command_చివర = p - 1;  // drop trailing bar
	లేకపోతే
	    tagp->command_చివర = p;
	p += 2;	// skip ";\""
	ఒకవేళ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    అయితే (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ఒకవేళ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		లేకపోతే ఒకవేళ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_దత్తాంశం = p + 10;
		లేకపోతే ఒకవేళ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((అక్షరం *)p + 5);
		ఒకవేళ (tagp->tagkind != ఖాళీ && tagp->user_దత్తాంశం != ఖాళీ)
		    ఆపు;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ఒకవేళ (pc == ఖాళీ || (pt != ఖాళీ && pc > pt))
		    tagp->tagkind = p;
		ఒకవేళ (pt == ఖాళీ)
		    ఆపు;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ఒకవేళ (tagp->tagkind != ఖాళీ)
    {
	కొరకు (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_చివర = p;
    }
    ఒకవేళ (tagp->user_దత్తాంశం != ఖాళీ)
    {
	కొరకు (p = tagp->user_దత్తాంశం;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_దత్తాంశం_చివర = p;
    }
    తిరిగి retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    స్థిర char_u *
ట్యాగ్_నిండిన_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    పూర్ణాంకం		c;

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    లేకపోతే
#అయితే_ముగింపు
    {
	c = *tagp->fname_చివర;
	*tagp->fname_చివర = NUL;
    }
    fullname = expand_ట్యాగ్_fname(tagp->fname, tagp->ట్యాగ్_fname, FALSE);

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (!tagp->is_etag)
#అయితే_ముగింపు
	*tagp->fname_చివర = c;

    తిరిగి fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
jumpto_ట్యాగ్(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    పూర్ణాంకం		forceit,	// :ta with !
    పూర్ణాంకం		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    పూర్ణాంకం		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_చివర;
    char_u	*tofree_fname = ఖాళీ;
    char_u	*fname;
    tagptrs_T	tagp;
    పూర్ణాంకం		retval = FAIL;
    పూర్ణాంకం		getfile_ఫలితం = GETFILE_వాడని;
    పూర్ణాంకం		శోధన_options;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    పూర్ణాంకం		save_no_hlsearch;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = ఖాళీ;
#అయితే_ముగింపు
    char_u	*నిండిన_fname = ఖాళీ;
#నిర్వచితమైతే FEAT_FOLDING
    పూర్ణాంకం		old_కీలకం_Typed = కీలకం_Typed;    // getting the file may reset it
#అయితే_ముగింపు
    size_t	len;
    char_u	*lbuf;
    పూర్ణాంకం		అంకెనా = FALSE;

    ఒకవేళ (postponed_విభజించు == 0 && !check_can_సమితి_curbuf_forceit(forceit))
	తిరిగి FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_పంక్తి_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ఒకవేళ (lbuf != ఖాళీ)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ఒకవేళ (pbuf == ఖాళీ || lbuf == ఖాళీ || parse_సరిపోలిక(lbuf, &tagp) == FAIL)
    {
	tagp.fname_చివర = ఖాళీ;
	వెళ్ళు erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_చివర = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ఒకవేళ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	అంకెనా = TRUE;
	pbuf[0] = ':';
    }
    కొరకు (pbuf_చివర = pbuf + అంకెనా;
	    *str && *str != '\n' && *str != '\r'; )
    {
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	ఒకవేళ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    ఆపు;
#అయితే_ముగింపు
	*pbuf_చివర++ = *str++;
	ఒకవేళ (pbuf_చివర - pbuf + 1 + అంకెనా >= LSIZE)
	    ఆపు;
    }
    *pbuf_చివర = NUL;

#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    ఒకవేళ (!tagp.is_etag)
#అయితే_ముగింపు
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ఒకవేళ (అంకెనా)
	    str++;
	ఒకవేళ (కనుగొను_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ఒకవేళ (str > pbuf && str[-1] == '|')
		pbuf_చివర = str - 1;
	    లేకపోతే
		pbuf_చివర = str;
	    *pbuf_చివర = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_ట్యాగ్_fname(fname, tagp.ట్యాగ్_fname, TRUE);
    ఒకవేళ (fname == ఖాళీ)
	వెళ్ళు erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ఒకవేళ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, ఖాళీ))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ఒకవేళ (nofile_fname == ఖాళీ)
	    nofile_fname = ఖాళీలు_option;
	వెళ్ళు erret;
    }

    ++RedrawingDisabled;

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_QUICKFIX)
    ఒకవేళ (g_do_tagpreview != 0)
    {
	postponed_విభజించు = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ఒకవేళ (!curwin->w_p_pvw)
	{
	    నిండిన_fname = నిండిన_Name_save(fname, FALSE);
	    fname = నిండిన_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    ఒకవేళ (postponed_విభజించు && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ఒకవేళ (existing_buf != ఖాళీ)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ఒకవేళ (swbuf_goto_win_with_buf(existing_buf) != ఖాళీ)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_ఫలితం = GETFILE_SAME_FILE;
	}
    }
    ఒకవేళ (getfile_ఫలితం == GETFILE_వాడని
				  && (postponed_విభజించు || cmdmod.cmod_tab != 0))
    {
	ఒకవేళ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_విభజించు |= WSP_VERT;

	ఒకవేళ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_సూచీ(curtab) + 1;

	ఒకవేళ (win_విభజించు(postponed_విభజించు > 0 ? postponed_విభజించు : 0,
						postponed_విభజించు_flags) == FAIL)
	{
	    ఒకవేళ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    వెళ్ళు erret;
	}
	RESET_BINDING(curwin);
    }
#అయితే_ముగింపు

    ఒకవేళ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ఒకవేళ defined(FEAT_QUICKFIX)
	ఒకవేళ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_బఫరు);
	లేకపోతే
#అయితే_ముగింపు
	    keep_help_flag = curbuf->b_help;
    }

    ఒకవేళ (getfile_ఫలితం == GETFILE_వాడని)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_ఫలితం = getfile(0, fname, ఖాళీ, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ఒకవేళ (GETFILE_SUCCESS(getfile_ఫలితం))	// got to the right file
    {
	curwin->w_సమితి_curswant = true;
	postponed_విభజించు = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ఒకవేళ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#అయితే_ముగింపు

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ఒకవేళ (vim_strchr(p_cpo, CPO_TAGPAT) != ఖాళీ)
	    శోధన_options = 0;
	లేకపోతే
	    శోధన_options = శోధన_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	ఒకవేళ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ఒకవేళ (str > pbuf_చివర - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_చివర - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ఒకవేళ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    లేకపోతే
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ఒకవేళ (do_శోధన(ఖాళీ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (దీర్ఘ)1,
							 శోధన_options, ఖాళీ))
		retval = OK;
	    లేకపోతే
	    {
		పూర్ణాంకం	దొరికింది = 1;
		పూర్ణాంకం	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ఒకవేళ (!do_శోధన(ఖాళీ, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (దీర్ఘ)1,
							 శోధన_options, ఖాళీ))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    దొరికింది = 2;
		    (శూన్య)test_for_static(&tagp);
		    cc = *tagp.tagname_చివర;
		    *tagp.tagname_చివర = NUL;
		    pbuflen = vim_snprintf((అక్షరం *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ఒకవేళ (!do_శోధన(ఖాళీ, '/', '/', pbuf, pbuflen, (దీర్ఘ)1,
							 శోధన_options, ఖాళీ))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((అక్షరం *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ఒకవేళ (!do_శోధన(ఖాళీ, '/', '/', pbuf, pbuflen, (దీర్ఘ)1,
							 శోధన_options, ఖాళీ))
			    దొరికింది = 0;
		    }
		    *tagp.tagname_చివర = cc;
		}
		ఒకవేళ (దొరికింది == 0)
		{
		    emsg(_(e_cannot_కనుగొను_ట్యాగ్_నమూనా));
		    curwin->w_cursor.lnum = save_lnum;
		}
		లేకపోతే
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ఒకవేళ (దొరికింది == 2 || !save_p_ic)
		    {
			సందేశం(_(e_couldnt_కనుగొను_ట్యాగ్_just_guessing));
			ఒకవేళ (!సందేశం_scrolled && సందేశం_silent == 0)
			{
			    బహిర్గతం_flush();
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
	లేకపోతే
	{
	    పూర్ణాంకం		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#నిర్వచితమైతే HAVE_SANDBOX
	    ++sandbox;
#అయితే_ముగింపు
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    curwin->w_cursor.నిలువు = 0;
	    curwin->w_cursor.coladd = 0;
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    ఒకవేళ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#నిర్వచితమైతే HAVE_SANDBOX
	    --sandbox;
#అయితే_ముగింపు
	}

	magic_overruled = save_magic_overruled;
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ఒకవేళ (శోధన_options)
	    సమితి_no_hlsearch(save_no_hlsearch);
#అయితే_ముగింపు

	// Return OK if jumped to another file (at least we found the file!).
	ఒకవేళ (getfile_ఫలితం == GETFILE_OPEN_OTHER)
	    retval = OK;

	ఒకవేళ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ఒకవేళ (curbuf->b_help)
		సమితి_topline(curwin, curwin->w_cursor.lnum);
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ ((fdo_flags & FDO_ట్యాగ్) && old_కీలకం_Typed)
		foldOpenCursor();
#అయితే_ముగింపు
	}

#ఒకవేళ defined(FEAT_QUICKFIX)
	ఒకవేళ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#అయితే_ముగింపు

	ఒకవేళ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    లేకపోతే
    {
	ఒకవేళ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ఒకవేళ (postponed_విభజించు)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_విభజించు = 0;
	}
#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	లేకపోతే ఒకవేళ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ఒకవేళ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#అయితే_ముగింపు
    }
#ఒకవేళ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ఒకవేళ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#అయితే_ముగింపు

erret:
#ఒకవేళ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#అయితే_ముగింపు
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(నిండిన_fname);

    తిరిగి retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    స్థిర char_u *
expand_ట్యాగ్_fname(char_u *fname, char_u *ట్యాగ్_fname, పూర్ణాంకం expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = ఖాళీ;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ఒకవేళ (p_tagsecure && మార్గం_with_url(fname))
    {
       emsg(_(e_ట్యాగ్_file_నమోదు_must_not_be_url));
       తిరిగి ఖాళీ;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ఒకవేళ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == ఖాళీ)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, ఖాళీ,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ఒకవేళ (expanded_fname != ఖాళీ)
	    fname = expanded_fname;
    }

    ఒకవేళ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(ట్యాగ్_fname)) != ట్యాగ్_fname)
    {
	retval = alloc(MAXPATHL);
	ఒకవేళ (retval != ఖాళీ)
	{
	    STRCPY(retval, ట్యాగ్_fname);
	    vim_strncpy(retval + (p - ట్యాగ్_fname), fname,
					      MAXPATHL - (p - ట్యాగ్_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    లేకపోతే
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    తిరిగి retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    స్థిర పూర్ణాంకం
test_for_current(
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
    పూర్ణాంకం	    is_etag,
#అయితే_ముగింపు
    char_u  *fname,
    char_u  *fname_చివర,
    char_u  *ట్యాగ్_fname,
    char_u  *buf_ffname)
{
    పూర్ణాంకం	    c;
    పూర్ణాంకం	    retval = FALSE;
    char_u  *fullname;

    ఒకవేళ (buf_ffname != ఖాళీ)	// if the buffer has a name
    {
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	ఒకవేళ (is_etag)
	    c = 0;	    // to shut up GCC
	లేకపోతే
#అయితే_ముగింపు
	{
	    c = *fname_చివర;
	    *fname_చివర = NUL;
	}
	fullname = expand_ట్యాగ్_fname(fname, ట్యాగ్_fname, TRUE);
	ఒకవేళ (fullname != ఖాళీ)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
	ఒకవేళ (!is_etag)
#అయితే_ముగింపు
	    *fname_చివర = c;
    }

    తిరిగి retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
కనుగొను_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	మొదటి_char = **pp;

    // Repeat for addresses separated with ';'
    కొరకు (;;)
    {
	ఒకవేళ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	లేకపోతే ఒకవేళ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ఒకవేళ (*str != మొదటి_char)
		str = ఖాళీ;
	    లేకపోతే
		++str;
	}
	లేకపోతే
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)str, "|;\"");
	    ఒకవేళ (str != ఖాళీ)
	    {
		++str;
		ఆపు;
	    }

	}
	ఒకవేళ (str == ఖాళీ || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    ఆపు;
	++str;	// skip ';'
	మొదటి_char = *str;
    }

    ఒకవేళ (str != ఖాళీ && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    శూన్య
tagstack_clear_నమోదు(taggy_T *అంశం)
{
    VIM_CLEAR(అంశం->tagname);
    VIM_CLEAR(అంశం->user_దత్తాంశం);
}

    పూర్ణాంకం
expand_ట్యాగులు(
    పూర్ణాంకం		tagnames,	// expand tag names
    char_u	*pat,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_కొలత = 100;
    tagptrs_T	t_p;
    పూర్ణాంకం		ret;

    name_buf = alloc(name_buf_కొలత);
    ఒకవేళ (name_buf == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (tagnames)
	extra_flag = ట్యాగ్_పేర్లు;
    లేకపోతే
	extra_flag = 0;
    ఒకవేళ (pat[0] == '/')
	ret = కనుగొను_ట్యాగులు(pat + 1, అంకె_file, file,
		ట్యాగ్_REGEXP | extra_flag | ట్యాగ్_VERBOSE,
		ట్యాగ్_MANY, curbuf->b_ffname);
    లేకపోతే
	ret = కనుగొను_ట్యాగులు(pat, అంకె_file, file,
	      ట్యాగ్_REGEXP | extra_flag | ట్యాగ్_VERBOSE | ట్యాగ్_NOIC,
		ట్యాగ్_MANY, curbuf->b_ffname);
    ఒకవేళ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 కొరకు (i = 0; i < *అంకె_file; i++)
	 {
	     size_t	len;

	     parse_సరిపోలిక((*file)[i], &t_p);
	     len = t_p.tagname_చివర - t_p.tagname;
	     ఒకవేళ (len > name_buf_కొలత - 3)
	     {
		 char_u *buf;

		 name_buf_కొలత = len + 3;
		 buf = vim_realloc(name_buf, name_buf_కొలత);
		 ఒకవేళ (buf == ఖాళీ)
		 {
		     vim_free(name_buf);
		     తిరిగి FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != ఖాళీ && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_చివర - t_p.fname);
	     (*file)[i][len + (t_p.fname_చివర - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    తిరిగి ret;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
కూడు_ట్యాగ్_క్షేత్రం(
    dict_T  *dict,
    అక్షరం    *క్షేత్రం_name,
    char_u  *ప్రారంభం,		// start of the value
    char_u  *చివర)		// after the value; can be NULL
{
    char_u	*buf;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		retval;

    // check that the field name doesn't exist yet
    ఒకవేళ (dict_has_కీలకం(dict, క్షేత్రం_name))
    {
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), క్షేత్రం_name);
	    verbose_leave();
	}
	తిరిగి FAIL;
    }
    buf = alloc(MAXPATHL);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (ప్రారంభం != ఖాళీ)
    {
	ఒకవేళ (చివర == ఖాళీ)
	{
	    చివర = ప్రారంభం + STRLEN(ప్రారంభం);
	    అయితే (చివర > ప్రారంభం && (చివర[-1] == '\r' || చివర[-1] == '\n'))
		--చివర;
	}
	len = (పూర్ణాంకం)(చివర - ప్రారంభం);
	ఒకవేళ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, ప్రారంభం, len);
    }
    buf[len] = NUL;
    retval = dict_కూడు_string(dict, క్షేత్రం_name, buf);
    vim_free(buf);
    తిరిగి retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    పూర్ణాంకం
get_ట్యాగులు(list_T *list, char_u *pat, char_u *buf_fname)
{
    పూర్ణాంకం		అంకె_matches, i, ret;
    char_u	**matches, *p;
    char_u	*నిండిన_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    దీర్ఘ	is_static;

    ret = కనుగొను_ట్యాగులు(pat, &అంకె_matches, &matches,
				ట్యాగ్_REGEXP | ట్యాగ్_NOIC, (పూర్ణాంకం)MAXCOL, buf_fname);
    ఒకవేళ (ret != OK || అంకె_matches <= 0)
	తిరిగి ret;

    కొరకు (i = 0; i < అంకె_matches; ++i)
    {
	ఒకవేళ (parse_సరిపోలిక(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    కొనసాగు;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ఒకవేళ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    కొనసాగు;
	}

	ఒకవేళ ((dict = dict_alloc()) == ఖాళీ)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    ఆపు;
	}
	ఒకవేళ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    కొనసాగు;
	}

	నిండిన_fname = ట్యాగ్_నిండిన_fname(&tp);
	ఒకవేళ (కూడు_ట్యాగ్_క్షేత్రం(dict, "name", tp.tagname, tp.tagname_చివర) == FAIL
		|| కూడు_ట్యాగ్_క్షేత్రం(dict, "filename", నిండిన_fname,
		    ఖాళీ) == FAIL
		|| కూడు_ట్యాగ్_క్షేత్రం(dict, "cmd", tp.command,
		    tp.command_చివర) == FAIL
		|| కూడు_ట్యాగ్_క్షేత్రం(dict, "kind", tp.tagkind,
		    tp.tagkind_చివర) == FAIL
		|| dict_కూడు_సంఖ్య(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(నిండిన_fname);

	ఒకవేళ (tp.command_చివర != ఖాళీ)
	{
	    కొరకు (p = tp.command_చివర + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ఒకవేళ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_చివర - 1;
		లేకపోతే ఒకవేళ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		లేకపోతే ఒకవేళ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    పూర్ణాంకం	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    అయితే (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (పూర్ణాంకం)(p - n);
		    ఒకవేళ (*p == ':' && len > 0)
		    {
			s = ++p;
			అయితే (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ఒకవేళ (కూడు_ట్యాగ్_క్షేత్రం(dict, (అక్షరం *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    లేకపోతే
			// Skip field without colon.
			అయితే (*p != NUL && *p >= ' ')
			    ++p;
		    ఒకవేళ (*p == NUL)
			ఆపు;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    తిరిగి ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    స్థిర శూన్య
get_ట్యాగ్_details(taggy_T *ట్యాగ్, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_కూడు_string(retdict, "tagname", ట్యాగ్->tagname);
    dict_కూడు_సంఖ్య(retdict, "matchnr", ట్యాగ్->cur_సరిపోలిక + 1);
    dict_కూడు_సంఖ్య(retdict, "bufnr", ట్యాగ్->cur_fnum);
    ఒకవేళ (ట్యాగ్->user_దత్తాంశం)
	dict_కూడు_string(retdict, "user_data", ట్యాగ్->user_దత్తాంశం);

    ఒకవేళ ((pos = list_alloc_id(aid_tagstack_from)) == ఖాళీ)
	తిరిగి;
    dict_కూడు_list(retdict, "from", pos);

    fmark = &ట్యాగ్->fmark;
    list_append_సంఖ్య(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_సంఖ్య(pos, (varnumber_T)fmark->mark.lnum);
    list_append_సంఖ్య(pos, (varnumber_T)(fmark->mark.నిలువు == MAXCOL ?
					MAXCOL : fmark->mark.నిలువు + 1));
    list_append_సంఖ్య(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    శూన్య
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    పూర్ణాంకం		i;
    dict_T	*d;

    dict_కూడు_సంఖ్య(retdict, "length", wp->w_tagstacklen);
    dict_కూడు_సంఖ్య(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_వస్తువులు);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;
    dict_కూడు_list(retdict, "items", l);

    కొరకు (i = 0; i < wp->w_tagstacklen; i++)
    {
	ఒకవేళ ((d = dict_alloc_id(aid_tagstack_details)) == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    తిరిగి;
	}

	get_ట్యాగ్_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    స్థిర శూన్య
tagstack_clear(win_T *wp)
{
    పూర్ణాంకం i;

    // Free the current tag stack
    కొరకు (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_నమోదు(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    స్థిర శూన్య
tagstack_జరుపు(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    పూర్ణాంకం		i;

    tagstack_clear_నమోదు(&tagstack[0]);
    కొరకు (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    స్థిర శూన్య
tagstack_నెట్టు_అంశం(
	win_T	*wp,
	char_u	*tagname,
	పూర్ణాంకం	cur_fnum,
	పూర్ణాంకం	cur_సరిపోలిక,
	pos_T	mark,
	పూర్ణాంకం	fnum,
	char_u  *user_దత్తాంశం)
{
    taggy_T	*tagstack = wp->w_tagstack;
    పూర్ణాంకం		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ఒకవేళ (idx >= TAGSTACKSIZE)
    {
	tagstack_జరుపు(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_సరిపోలిక = cur_సరిపోలిక;
    ఒకవేళ (tagstack[idx].cur_సరిపోలిక < 0)
	tagstack[idx].cur_సరిపోలిక = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_దత్తాంశం = user_దత్తాంశం;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    స్థిర శూన్య
tagstack_నెట్టు_వస్తువులు(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    పూర్ణాంకం		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == ఖాళీ)
	    కొనసాగు;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ఒకవేళ ((di = dict_కనుగొను(itemdict, (char_u *)"from", -1)) == ఖాళీ)
	    కొనసాగు;
	ఒకవేళ (list2fpos(&di->di_tv, &mark, &fnum, ఖాళీ, FALSE) != OK)
	    కొనసాగు;
	ఒకవేళ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (mark.నిలువు > 0)
	    mark.నిలువు--;
	tagstack_నెట్టు_అంశం(wp, tagname,
		(పూర్ణాంకం)dict_get_సంఖ్య(itemdict, "bufnr"),
		(పూర్ణాంకం)dict_get_సంఖ్య(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    స్థిర శూన్య
tagstack_సమితి_curidx(win_T *wp, పూర్ణాంకం curidx)
{
    wp->w_tagstackidx = curidx;
    ఒకవేళ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ఒకవేళ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    పూర్ణాంకం
సమితి_tagstack(win_T *wp, dict_T *d, పూర్ణాంకం action)
{
    dictitem_T	*di;
    list_T	*l = ఖాళీ;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ఒకవేళ (tfu_లోపల_use)
    {
	emsg(_(e_cannot_modify_ట్యాగ్_దొంతర_within_tagfunc));
	తిరిగి FAIL;
    }
# endif

    ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"items", -1)) != ఖాళీ)
    {
	ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    తిరిగి FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"curidx", -1)) != ఖాళీ)
	tagstack_సమితి_curidx(wp, (పూర్ణాంకం)tv_get_సంఖ్య(&di->di_tv) - 1);

    ఒకవేళ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	పూర్ణాంకం	tagstackidx = wp->w_tagstackidx;
	పూర్ణాంకం	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	అయితే (tagstackidx < tagstacklen)
	    tagstack_clear_నమోదు(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ఒకవేళ (l != ఖాళీ)
    {
	ఒకవేళ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_నెట్టు_వస్తువులు(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు
