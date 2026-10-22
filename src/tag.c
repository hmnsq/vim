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

#include "vim.h"

/*
 * Structure to hold pointers to various items in a tag line.
 */
typedef sa_nracanaa tag_pointers
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
    ຈຳນວນເຕັມ		is_etag;	// TRUE for emacs tag
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
typedef ga_nanaa
{
    TAGS_READ_SUCCESS = 1,
    TAGS_READ_EOF,
    TAGS_READ_IGNORE,
} tags_read_status_T;

/*
 * States used during a tags search
 */
typedef ga_nanaa
{
    TS_START,		// at start of file
    TS_LINEAR,		// linear searching forward, till EOF
    TS_BINARY,		// binary searching
    TS_SKIP_BACK,	// skipping backwards
    TS_STEP_FORWARD	// stepping forwards
} tagsearch_state_T;	// Current search state

/*
 * Binary search file offsets in a tags file
 */
typedef sa_nracanaa
{
    off_T	low_offset;	// offset for first char of first line that
				// could match
    off_T	high_offset;	// offset of char after last line that could
				// match
    off_T	curr_offset;	// Current file offset in search range
    off_T	curr_offset_used; // curr_offset used when skipping back
    off_T	match_offset;	// Where the binary search found a tag
    ຈຳນວນເຕັມ	low_char;		// first char at low_offset
    ຈຳນວນເຕັມ	high_char;		// first char at high_offset
} tagsearch_info_T;

/*
 * Return values used when matching tags against a pattern.
 */
typedef ga_nanaa
{
    TAG_MATCH_SUCCESS = 1,
    TAG_MATCH_FAIL,
    TAG_MATCH_STOP,
    TAG_MATCH_NEXT
} tagmatch_status_T;

/*
 * Arguments used for matching tags read from a tags file against a pattern.
 */
typedef sa_nracanaa
{
    ຈຳນວນເຕັມ	matchoff;		// tag match offset
    ຈຳນວນເຕັມ	match_re;		// TRUE if the tag matches a regexp
    ຈຳນວນເຕັມ	match_no_ic;		// TRUE if the tag matches with case
    ຈຳນວນເຕັມ	has_re;			// regular expression used
    ຈຳນວນເຕັມ	sortic;			// tags file sorted ignoring case (foldcase)
    ຈຳນວນເຕັມ	sort_error;		// tags file not sorted
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
#define MT_COUNT	16

static var_na	*mt_names[MT_COUNT/2] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		// return value for jumpto_tag
static char_u	*nofile_fname = NULL;	// fname for NOTAGFILE error

static void taglen_advance(ຈຳນວນເຕັມ l);

static ຈຳນວນເຕັມ jumpto_tag(char_u *lbuf, ຈຳນວນເຕັມ forceit, ຈຳນວນເຕັມ keep_help);
#ifdef FEAT_EMACS_TAGS
static ຈຳນວນເຕັມ parse_tag_line(char_u *lbuf, ຈຳນວນເຕັມ is_etag, tagptrs_T *tagp);
#ບໍ່ດັ່ງນັ້ນ
static ຈຳນວນເຕັມ parse_tag_line(char_u *lbuf, tagptrs_T *tagp);
#endif
static ຈຳນວນເຕັມ test_for_static(tagptrs_T *);
static ຈຳນວນເຕັມ parse_match(char_u *lbuf, tagptrs_T *tagp);
static char_u *tag_full_fname(tagptrs_T *tagp);
static char_u *expand_tag_fname(char_u *fname, char_u *tag_fname, ຈຳນວນເຕັມ expand);
#ifdef FEAT_EMACS_TAGS
static ຈຳນວນເຕັມ test_for_current(ຈຳນວນເຕັມ, char_u *, char_u *, char_u *, char_u *);
#ບໍ່ດັ່ງນັ້ນ
static ຈຳນວນເຕັມ test_for_current(char_u *, char_u *, char_u *, char_u *);
#endif
static ຈຳນວນເຕັມ find_extra(char_u **pp);
static void print_tag_list(ຈຳນວນເຕັມ new_tag, ຈຳນວນເຕັມ use_tagstack, ຈຳນວນເຕັມ num_matches, char_u **matches);
#ຖ້າ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
static ຈຳນວນເຕັມ add_llist_tags(char_u *tag, ຈຳນວນເຕັມ num_matches, char_u **matches);
#endif

static char_u	*tagmatchname = NULL;	// name of last used tag

#ຖ້າ defined(FEAT_QUICKFIX)
/*
 * Tag for preview window is remembered separately, to avoid messing up the
 * normal tagstack.
 */
static taggy_T ptag_entry = {NULL, {{0, 0, 0}, 0}, 0, 0, NULL};
#endif

#ifdef FEAT_EVAL
static ຈຳນວນເຕັມ  tfu_in_use = FALSE;	    // disallow recursive call of tagfunc
static callback_T tfu_cb;	    // 'tagfunc' callback function
#endif

// Used instead of NUL to separate tag fields in the growarrays.
#define TAG_SEP 0x02

#ຖ້າ defined(FEAT_EVAL)
/*
 * Reads the 'tagfunc' option value and convert that to a callback value.
 * Invoked when the 'tagfunc' option is set. The option value can be a name of
 * a function (string), or function(<name>) or funcref(<name>) or a lambda.
 */
    var_na *
did_set_tagfunc(optset_T *args)
{
    ຈຳນວນເຕັມ	retval;

    ຖ້າ (args->os_flags & OPT_LOCAL)
	retval = option_set_callback_func(args->os_newval.string,
							    &curbuf->b_tfu_cb);
    ບໍ່ດັ່ງນັ້ນ
    {
	retval = option_set_callback_func(args->os_newval.string, &tfu_cb);
	ຖ້າ (retval == OK && !(args->os_flags & OPT_GLOBAL))
	    set_buflocal_tfu_callback(curbuf);
    }

    ສົ່ງຄືນ retval == FAIL ? e_invalid_argument : NULL;
}
#endif

#ຖ້າ defined(EXITFREE)
    void
free_tagfunc_option(void)
{
# ifdef FEAT_EVAL
    free_callback(&tfu_cb);
# endif
}
#endif

#ຖ້າ defined(FEAT_EVAL)
/*
 * Mark the global 'tagfunc' callback with "copyID" so that it is not garbage
 * collected.
 */
    ຈຳນວນເຕັມ
set_ref_in_tagfunc(ຈຳນວນເຕັມ copyID UNUSED)
{
    ຈຳນວນເຕັມ	abort = FALSE;

    abort = set_ref_in_callback(&tfu_cb, copyID);

    ສົ່ງຄືນ abort;
}

/*
 * Copy the global 'tagfunc' callback function to the buffer-local 'tagfunc'
 * callback for 'buf'.
 */
    void
set_buflocal_tfu_callback(buf_T *buf UNUSED)
{
    free_callback(&buf->b_tfu_cb);
    ຖ້າ (tfu_cb.cb_name != NULL && *tfu_cb.cb_name != NUL)
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
    ຈຳນວນເຕັມ
do_tag(
    char_u	*tag,		// tag (pattern) to jump to
    ຈຳນວນເຕັມ		type,
    ຈຳນວນເຕັມ		count,
    ຈຳນວນເຕັມ		forceit,	// :ta with !
    ຈຳນວນເຕັມ		verbose)	// print "tag not found" message
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ຈຳນວນເຕັມ		tagstackidx = curwin->w_tagstackidx;
    ຈຳນວນເຕັມ		tagstacklen = curwin->w_tagstacklen;
    ຈຳນວນເຕັມ		cur_match = 0;
    ຈຳນວນເຕັມ		cur_fnum = curbuf->b_fnum;
    ຈຳນວນເຕັມ		oldtagstackidx = tagstackidx;
    ຈຳນວນເຕັມ		prevtagstackidx = tagstackidx;
    ຈຳນວນເຕັມ		prev_num_matches;
    ຈຳນວນເຕັມ		new_tag = FALSE;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		ic;
    ຈຳນວນເຕັມ		no_regexp = FALSE;
    ຈຳນວນເຕັມ		error_cur_match = 0;
    ຈຳນວນເຕັມ		save_pos = FALSE;
    fmark_T	saved_fmark;
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		jumped_to_tag = FALSE;
#endif
    ຈຳນວນເຕັມ		new_num_matches;
    char_u	**new_matches;
    ຈຳນວນເຕັມ		use_tagstack;
    ຈຳນວນເຕັມ		skip_msg = FALSE;
    char_u	*buf_ffname = curbuf->b_ffname;	    // name to use for
						    // priority computation
    ຈຳນວນເຕັມ		use_tfu = 1;
    char_u	*tofree = NULL;

    // remember the matches for the last used tag
    static ຈຳນວນເຕັມ		num_matches = 0;
    static ຈຳນວນເຕັມ		max_num_matches = 0;  // limit used for match search
    static char_u	**matches = NULL;
    static ຈຳນວນເຕັມ		flags;

#ifdef EXITFREE
    ຖ້າ (type == DT_FREE)
    {
	// remove the list of matches
	FreeWild(num_matches, matches);
# ifdef FEAT_CSCOPE
	cs_free_tags();
# endif
	num_matches = 0;
	ສົ່ງຄືນ FALSE;
    }
#endif

#ifdef FEAT_EVAL
    ຖ້າ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_stack_within_tagfunc));
	ສົ່ງຄືນ FALSE;
    }
#endif

    ຖ້າ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ສົ່ງຄືນ FALSE;

    ຖ້າ (type == DT_HELP)
    {
	type = DT_TAG;
	no_regexp = TRUE;
	use_tfu = 0;
    }

    prev_num_matches = num_matches;
    free_string_option(nofile_fname);
    nofile_fname = NULL;

    CLEAR_POS(&saved_fmark.mark);	// shutup gcc 4.0
    saved_fmark.fnum = 0;

    /*
     * Don't add a tag to the tagstack if 'tagstack' has been reset.
     */
    ຖ້າ ((!p_tgst && *tag != NUL))
    {
	use_tagstack = FALSE;
	new_tag = TRUE;
#ຖ້າ defined(FEAT_QUICKFIX)
	ຖ້າ (g_do_tagpreview != 0)
	{
	    tagstack_clear_entry(&ptag_entry);
	    ຖ້າ ((ptag_entry.tagname = vim_strsave(tag)) == NULL)
		goto end_do_tag;
	}
#endif
    }
    ບໍ່ດັ່ງນັ້ນ
    {
#ຖ້າ defined(FEAT_QUICKFIX)
	ຖ້າ (g_do_tagpreview != 0)
	    use_tagstack = FALSE;
	ບໍ່ດັ່ງນັ້ນ
#endif
	    use_tagstack = TRUE;

	// new pattern, add to the tag stack
	ຖ້າ (*tag != NUL
		&& (type == DT_TAG || type == DT_SELECT || type == DT_JUMP
#ifdef FEAT_QUICKFIX
		    || type == DT_LTAG
#endif
#ifdef FEAT_CSCOPE
		    || type == DT_CSCOPE
#endif
		    ))
	{
#ຖ້າ defined(FEAT_QUICKFIX)
	    ຖ້າ (g_do_tagpreview != 0)
	    {
		ຖ້າ (ptag_entry.tagname != NULL
			&& STRCMP(ptag_entry.tagname, tag) == 0)
		{
		    // Jumping to same tag: keep the current match, so that
		    // the CursorHold autocommand example works.
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    tagstack_clear_entry(&ptag_entry);
		    ຖ້າ ((ptag_entry.tagname = vim_strsave(tag)) == NULL)
			goto end_do_tag;
		}
	    }
	    ບໍ່ດັ່ງນັ້ນ
#endif
	    {
		/*
		 * If the last used entry is not at the top, delete all tag
		 * stack entries above it.
		 */
		ໃນຂະນະທີ່ (tagstackidx < tagstacklen)
		    tagstack_clear_entry(&tagstack[--tagstacklen]);

		// if the tagstack is full: remove oldest entry
		ຖ້າ (++tagstacklen > TAGSTACKSIZE)
		{
		    tagstacklen = TAGSTACKSIZE;
		    tagstack_clear_entry(&tagstack[0]);
		    ສຳລັບ (i = 1; i < tagstacklen; ++i)
			tagstack[i - 1] = tagstack[i];
		    tagstack[--tagstackidx].user_data = NULL;
		}

		/*
		 * put the tag name in the tag stack
		 */
		ຖ້າ ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == NULL)
		{
		    curwin->w_tagstacklen = tagstacklen - 1;
		    goto end_do_tag;
		}
		curwin->w_tagstacklen = tagstacklen;

		save_pos = TRUE;	// save the cursor position below
	    }

	    new_tag = TRUE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (
#ຖ້າ defined(FEAT_QUICKFIX)
		    g_do_tagpreview != 0 ? ptag_entry.tagname == NULL :
#endif
		    tagstacklen == 0)
	    {
		// empty stack
		emsg(_(e_tag_stack_empty));
		goto end_do_tag;
	    }

	    ຖ້າ (type == DT_POP)		// go to older position
	    {
#ifdef FEAT_FOLDING
		ຈຳນວນເຕັມ	old_KeyTyped = KeyTyped;
#endif
		ຖ້າ ((tagstackidx -= count) < 0)
		{
		    emsg(_(e_at_bottom_of_tag_stack));
		    ຖ້າ (tagstackidx + count == 0)
		    {
			// We did [num]^T from the bottom of the stack
			tagstackidx = 0;
			goto end_do_tag;
		    }
		    // We weren't at the bottom of the stack, so jump all the
		    // way to the bottom now.
		    tagstackidx = 0;
		}
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (tagstackidx >= tagstacklen)    // count == 0?
		{
		    emsg(_(e_at_top_of_tag_stack));
		    goto end_do_tag;
		}

		// Make a copy of the fmark, autocommands may invalidate the
		// tagstack before it's used.
		saved_fmark = tagstack[tagstackidx].fmark;
		ຖ້າ (saved_fmark.fnum != curbuf->b_fnum)
		{
		    /*
		     * Jump to other file. If this fails (e.g. because the
		     * file was changed) keep original position in tag stack.
		     */
		    ຖ້າ (buflist_getfile(saved_fmark.fnum, saved_fmark.mark.lnum,
					       GETF_SETMARK, forceit) == FAIL)
		    {
			tagstackidx = oldtagstackidx;  // back to old posn
			goto end_do_tag;
		    }
		    // An BufReadPost autocommand may jump to the '" mark, but
		    // we don't what that here.
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    setpcmark();
		    curwin->w_cursor.lnum = saved_fmark.mark.lnum;
		}
		curwin->w_cursor.col = saved_fmark.mark.col;
		curwin->w_set_curswant = true;
		check_cursor();
#ifdef FEAT_FOLDING
		ຖ້າ ((fdo_flags & FDO_TAG) && old_KeyTyped)
		    foldOpenCursor();
#endif

		// remove the old list of matches
		FreeWild(num_matches, matches);
#ifdef FEAT_CSCOPE
		cs_free_tags();
#endif
		num_matches = 0;
		tag_freematch();
		goto end_do_tag;
	    }

	    ຖ້າ (type == DT_TAG
#ຖ້າ defined(FEAT_QUICKFIX)
		    || type == DT_LTAG
#endif
	       )
	    {
#ຖ້າ defined(FEAT_QUICKFIX)
		ຖ້າ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ບໍ່ດັ່ງນັ້ນ
#endif
		{
		    // ":tag" (no argument): go to newer pattern
		    save_pos = TRUE;	// save the cursor position below
		    ຖ້າ ((tagstackidx += count - 1) >= tagstacklen)
		    {
			/*
			 * Beyond the last one, just give an error message and
			 * go to the last one.  Don't store the cursor
			 * position.
			 */
			tagstackidx = tagstacklen - 1;
			emsg(_(e_at_top_of_tag_stack));
			save_pos = FALSE;
		    }
		    ບໍ່ດັ່ງນັ້ນ ຖ້າ (tagstackidx < 0)	// must have been count == 0
		    {
			emsg(_(e_at_bottom_of_tag_stack));
			tagstackidx = 0;
			goto end_do_tag;
		    }
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		new_tag = TRUE;
	    }
	    ບໍ່ດັ່ງນັ້ນ				// go to other matching tag
	    {
		// Save index for when selection is cancelled.
		prevtagstackidx = tagstackidx;

#ຖ້າ defined(FEAT_QUICKFIX)
		ຖ້າ (g_do_tagpreview != 0)
		{
		    cur_match = ptag_entry.cur_match;
		    cur_fnum = ptag_entry.cur_fnum;
		}
		ບໍ່ດັ່ງນັ້ນ
#endif
		{
		    ຖ້າ (--tagstackidx < 0)
			tagstackidx = 0;
		    cur_match = tagstack[tagstackidx].cur_match;
		    cur_fnum = tagstack[tagstackidx].cur_fnum;
		}
		switch (type)
		{
		    pak_sa DT_FIRST: cur_match = count - 1; bha_nga;
		    pak_sa DT_SELECT:
		    pak_sa DT_JUMP:
#ifdef FEAT_CSCOPE
		    pak_sa DT_CSCOPE:
#endif
		    pak_sa DT_LAST:  cur_match = MAXCOL - 1; bha_nga;
		    pak_sa DT_NEXT:  cur_match += count; bha_nga;
		    pak_sa DT_PREV:  cur_match -= count; bha_nga;
		}
		ຖ້າ (cur_match >= MAXCOL)
		    cur_match = MAXCOL - 1;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (cur_match < 0)
		{
		    emsg(_(e_cannot_go_before_first_matching_tag));
		    skip_msg = TRUE;
		    cur_match = 0;
		    cur_fnum = curbuf->b_fnum;
		}
	    }
	}

#ຖ້າ defined(FEAT_QUICKFIX)
	ຖ້າ (g_do_tagpreview != 0)
	{
	    ຖ້າ (type != DT_SELECT && type != DT_JUMP)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
#endif
	{
	    /*
	     * For ":tag [arg]" or ":tselect" remember position before the jump.
	     */
	    saved_fmark = tagstack[tagstackidx].fmark;
	    ຖ້າ (save_pos)
	    {
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	    }

	    // Curwin will change in the call to jumpto_tag() if ":stag" was
	    // used or an autocommand jumps to another window; store value of
	    // tagstackidx now.
	    curwin->w_tagstackidx = tagstackidx;
	    ຖ້າ (type != DT_SELECT && type != DT_JUMP)
	    {
		curwin->w_tagstack[tagstackidx].cur_match = cur_match;
		curwin->w_tagstack[tagstackidx].cur_fnum = cur_fnum;
	    }
	}
    }

    // When not using the current buffer get the name of buffer "cur_fnum".
    // Makes sure that the tag order doesn't change when using a remembered
    // position for "cur_match".
    ຖ້າ (cur_fnum != curbuf->b_fnum)
    {
	buf_T *buf = buflist_findnr(cur_fnum);

	ຖ້າ (buf != NULL)
	    buf_ffname = buf->b_ffname;
    }

    /*
     * Repeat searching for tags, when a file has not been found.
     */
    ສຳລັບ (;;)
    {
	ຈຳນວນເຕັມ	other_name;
	char_u	*name;

	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	ຖ້າ (use_tagstack)
	{
	    // make a copy, the tagstack may change in 'tagfunc'
	    name = vim_strsave(tagstack[tagstackidx].tagname);
	    vim_free(tofree);
	    tofree = name;
	}
#ຖ້າ defined(FEAT_QUICKFIX)
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (g_do_tagpreview != 0)
	    name = ptag_entry.tagname;
#endif
	ບໍ່ດັ່ງນັ້ນ
	    name = tag;
	other_name = (tagmatchname == NULL || STRCMP(tagmatchname, name) != 0);
	ຖ້າ (new_tag
		|| (cur_match >= num_matches && max_num_matches != MAXCOL)
		|| other_name)
	{
	    ຖ້າ (other_name)
	    {
		vim_free(tagmatchname);
		tagmatchname = vim_strsave(name);
	    }

	    ຖ້າ (type == DT_SELECT || type == DT_JUMP
#ຖ້າ defined(FEAT_QUICKFIX)
		|| type == DT_LTAG
#endif
		)
		cur_match = MAXCOL - 1;
	    ຖ້າ (type == DT_TAG)
		max_num_matches = MAXCOL;
	    ບໍ່ດັ່ງນັ້ນ
		max_num_matches = cur_match + 1;

	    // when the argument starts with '/', use it as a regexp
	    ຖ້າ (!no_regexp && *name == '/')
	    {
		flags = TAG_REGEXP;
		++name;
	    }
	    ບໍ່ດັ່ງນັ້ນ
		flags = TAG_NOIC;

#ifdef FEAT_CSCOPE
	    ຖ້າ (type == DT_CSCOPE)
		flags = TAG_CSCOPE;
#endif
	    ຖ້າ (verbose)
		flags |= TAG_VERBOSE;

	    ຖ້າ (!use_tfu)
		flags |= TAG_NO_TAGFUNC;

	    ຖ້າ (find_tags(name, &new_num_matches, &new_matches, flags,
					    max_num_matches, buf_ffname) == OK
		    && new_num_matches < max_num_matches)
		max_num_matches = MAXCOL; // If less than max_num_matches
					  // found: all matches found.

	    // A tag function may do anything, which may cause various
	    // information to become invalid.  At least check for the tagstack
	    // to still be the same.
	    ຖ້າ (tagstack != curwin->w_tagstack)
	    {
		emsg(_(e_window_unexpectedly_close_while_searching_for_tags));
		FreeWild(new_num_matches, new_matches);
		bha_nga;
	    }

	    // If there already were some matches for the same name, move them
	    // to the start.  Avoids that the order changes when using
	    // ":tnext" and jumping to another file.
	    ຖ້າ (!new_tag && !other_name)
	    {
		ຈຳນວນເຕັມ	    j, k;
		ຈຳນວນເຕັມ	    idx = 0;
		tagptrs_T   tagp, tagp2;

		// Find the position of each old match in the new list.  Need
		// to use parse_match() to find the tag line.
		ສຳລັບ (j = 0; j < num_matches; ++j)
		{
		    parse_match(matches[j], &tagp);
		    ສຳລັບ (i = idx; i < new_num_matches; ++i)
		    {
			parse_match(new_matches[i], &tagp2);
			ຖ້າ (STRCMP(tagp.tagname, tagp2.tagname) == 0)
			{
			    char_u *p = new_matches[i];
			    ສຳລັບ (k = i; k > idx; --k)
				new_matches[k] = new_matches[k - 1];
			    new_matches[idx++] = p;
			    bha_nga;
			}
		    }
		}
	    }
	    FreeWild(num_matches, matches);
	    num_matches = new_num_matches;
	    matches = new_matches;
	}

	ຖ້າ (num_matches <= 0)
	{
	    ຖ້າ (verbose)
		semsg(_(e_tag_not_found_str), name);
#ຖ້າ defined(FEAT_QUICKFIX)
	    g_do_tagpreview = 0;
#endif
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຈຳນວນເຕັມ ask_for_selection = FALSE;
#ຖ້າ defined(FEAT_EVAL)
	    size_t  IObufflen;
#endif

#ifdef FEAT_CSCOPE
	    ຖ້າ (type == DT_CSCOPE && num_matches > 1)
	    {
		cs_print_tags();
		ask_for_selection = TRUE;
	    }
	    ບໍ່ດັ່ງນັ້ນ
#endif
	    ຖ້າ (type == DT_TAG && *tag != NUL)
		// If a count is supplied to the ":tag <name>" command, then
		// jump to count'th matching tag.
		cur_match = count > 0 ? count - 1 : 0;
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == DT_SELECT || (type == DT_JUMP && num_matches > 1))
	    {
		print_tag_list(new_tag, use_tagstack, num_matches, matches);
		ask_for_selection = TRUE;
	    }
#ຖ້າ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type == DT_LTAG)
	    {
		ຖ້າ (add_llist_tags(tag, num_matches, matches) == FAIL)
		    goto end_do_tag;
		cur_match = 0;		// Jump to the first tag
	    }
#endif

	    ຖ້າ (ask_for_selection == TRUE)
	    {
		/*
		 * Ask to select a tag from the list.
		 */
		i = prompt_for_number(NULL);
		ຖ້າ (i <= 0 || i > num_matches || got_int)
		{
		    // no valid choice: don't change anything
		    ຖ້າ (use_tagstack)
		    {
			tagstack[tagstackidx].fmark = saved_fmark;
			tagstackidx = prevtagstackidx;
		    }
#ifdef FEAT_CSCOPE
		    cs_free_tags();
		    jumped_to_tag = TRUE;
#endif
		    bha_nga;
		}
		cur_match = i - 1;
	    }

	    ຖ້າ (cur_match >= num_matches)
	    {
		// Avoid giving this error when a file wasn't found and we're
		// looking for a match in another file, which wasn't found.
		// There will be an emsg("file doesn't exist") below then.
		ຖ້າ ((type == DT_NEXT || type == DT_FIRST)
						      && nofile_fname == NULL)
		{
		    ຖ້າ (num_matches == 1)
			emsg(_(e_there_is_only_one_matching_tag));
		    ບໍ່ດັ່ງນັ້ນ
			emsg(_(e_cannot_go_beyond_last_matching_tag));
		    skip_msg = TRUE;
		}
		cur_match = num_matches - 1;
	    }
	    ຖ້າ (use_tagstack)
	    {
		tagptrs_T   tagp;

		tagstack[tagstackidx].cur_match = cur_match;
		tagstack[tagstackidx].cur_fnum = cur_fnum;

		// store user-provided data originating from tagfunc
		ຖ້າ (use_tfu && parse_match(matches[cur_match], &tagp) == OK
			&& tagp.user_data)
		{
		    VIM_CLEAR(tagstack[tagstackidx].user_data);
		    tagstack[tagstackidx].user_data = vim_strnsave(
			  tagp.user_data, tagp.user_data_end - tagp.user_data);
		}

		++tagstackidx;
	    }
#ຖ້າ defined(FEAT_QUICKFIX)
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (g_do_tagpreview != 0)
	    {
		ptag_entry.cur_match = cur_match;
		ptag_entry.cur_fnum = cur_fnum;
	    }
#endif

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an emsg() is given below.
	     */
	    ຖ້າ (nofile_fname != NULL && error_cur_match != cur_match)
		smsg(_("File \"%s\" does not exist"), nofile_fname);


	    ic = (matches[cur_match][0] & MT_IC_OFF);
	    ຖ້າ (type != DT_TAG && type != DT_SELECT && type != DT_JUMP
#ifdef FEAT_CSCOPE
		&& type != DT_CSCOPE
#endif
		&& (num_matches > 1 || ic)
		&& !skip_msg)
	    {
		// Give an indication of the number of matching tags
		sprintf((var_na *)IObuff, _("tag %d of %d%s"),
				cur_match + 1,
				num_matches,
				max_num_matches != MAXCOL ? _(" or more") : "");
		ຖ້າ (ic)
		    STRCAT(IObuff, _("  Using tag with different case!"));
		ຖ້າ ((num_matches > prev_num_matches || new_tag)
							   && num_matches > 1)
		{
		    ຖ້າ (ic)
			msg_attr((var_na *)IObuff, HL_ATTR(HLF_W));
		    ບໍ່ດັ່ງນັ້ນ
			msg((var_na *)IObuff);
		    msg_scroll = TRUE;	// don't overwrite this message
		}
		ບໍ່ດັ່ງນັ້ນ
		    give_warning(IObuff, ic);
		ຖ້າ (ic && !msg_scrolled && msg_silent == 0)
		{
		    out_flush();
		    ui_delay(1007L, TRUE);
		}
	    }

#ຖ້າ defined(FEAT_EVAL)
	    // Let the SwapExists event know what tag we are jumping to.
	    IObufflen = vim_snprintf_safelen((var_na *)IObuff, IOSIZE, ":ta %s\r", name);
	    set_vim_var_string(VV_SWAPCOMMAND, IObuff, (ຈຳນວນເຕັມ)IObufflen);
#endif

	    /*
	     * Jump to the desired match.
	     */
	    i = jumpto_tag(matches[cur_match], forceit, type != DT_CSCOPE);

#ຖ້າ defined(FEAT_EVAL)
	    set_vim_var_string(VV_SWAPCOMMAND, NULL, -1);
#endif

	    ຖ້າ (i == NOTAGFILE)
	    {
		// File not found: try again with another matching tag
		ຖ້າ ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_num_matches != MAXCOL
					     || cur_match < num_matches - 1)))
		{
		    error_cur_match = cur_match;
		    ຖ້າ (use_tagstack)
			--tagstackidx;
		    ຖ້າ (type == DT_PREV)
			--cur_match;
		    ບໍ່ດັ່ງນັ້ນ
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    continue;
		}
		semsg(_(e_file_str_does_not_exist), nofile_fname);
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		// We may have jumped to another window, check that
		// tagstackidx is still valid.
		ຖ້າ (use_tagstack && tagstackidx > curwin->w_tagstacklen)
		    tagstackidx = curwin->w_tagstackidx;
#ifdef FEAT_CSCOPE
		jumped_to_tag = TRUE;
#endif
	    }
	}
	bha_nga;
    }

end_do_tag:
    // Only store the new index when using the tagstack and it's valid.
    ຖ້າ (use_tagstack && tagstackidx <= curwin->w_tagstacklen)
	curwin->w_tagstackidx = tagstackidx;
    postponed_split = 0;	// don't split next time
#ifdef FEAT_QUICKFIX
    g_do_tagpreview = 0;	// don't do tag preview next time
#endif

    vim_free(tofree);
#ifdef FEAT_CSCOPE
    ສົ່ງຄືນ jumped_to_tag;
#ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ FALSE;
#endif
}

/*
 * List all the matching tags.
 */
    static void
print_tag_list(
    ຈຳນວນເຕັມ		new_tag,
    ຈຳນວນເຕັມ		use_tagstack,
    ຈຳນວນເຕັມ		num_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    ຈຳນວນເຕັມ		tagstackidx = curwin->w_tagstackidx;
    ຈຳນວນເຕັມ		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    ຈຳນວນເຕັມ		taglen;
    ຈຳນວນເຕັມ		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (ຈຳນວນເຕັມ)(tagp.tagname_end - tagp.tagname + 2);
    ຖ້າ (taglen < 18)
	taglen = 18;
    ຖ້າ (taglen > cmdline_width - 25)
	taglen = MAXCOL;
    ຖ້າ (msg_col == 0)
	msg_didout = FALSE;	// overwrite previous message
    msg_start();
    msg_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    msg_clr_eos();
    taglen_advance(taglen);
    msg_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    ສຳລັບ (i = 0; i < num_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	ຖ້າ (!new_tag && (
#ຖ້າ defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	ບໍ່ດັ່ງນັ້ນ
	    *IObuff = ' ';
	vim_snprintf((var_na *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	msg_puts((var_na *)IObuff);
	ຖ້າ (tagp.tagkind != NULL)
	    msg_outtrans_len(tagp.tagkind,
			  (ຈຳນວນເຕັມ)(tagp.tagkind_end - tagp.tagkind));
	msg_advance(13);
	msg_outtrans_len_attr(tagp.tagname,
			   (ຈຳນວນເຕັມ)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	msg_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	ຖ້າ (p != NULL)
	{
	    msg_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	ຖ້າ (msg_col > 0)
	    msg_putchar('\n');
	ຖ້າ (got_int)
	    bha_nga;
	msg_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	ຖ້າ (command_end != NULL)
	{
	    p = command_end + 3;
	    ໃນຂະນະທີ່ (*p && *p != '\r' && *p != '\n')
	    {
		ໃນຂະນະທີ່ (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		ຖ້າ (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    continue;
		}
		// skip "kind:<kind>" and "<kind>"
		ຖ້າ (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    continue;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		ໃນຂະນະທີ່ (*p && *p != '\r' && *p != '\n')
		{
		    ຖ້າ (msg_col + ptr2cells(p) >= cmdline_width)
		    {
			msg_putchar('\n');
			ຖ້າ (got_int)
			    bha_nga;
			msg_advance(15);
		    }
		    p = msg_outtrans_one(p, attr);
		    ຖ້າ (*p == TAB)
		    {
			msg_puts_attr(" ", attr);
			bha_nga;
		    }
		    ຖ້າ (*p == ':')
			attr = 0;
		}
	    }
	    ຖ້າ (msg_col > 15)
	    {
		msg_putchar('\n');
		ຖ້າ (got_int)
		    bha_nga;
		msg_advance(15);
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ສຳລັບ (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	ຖ້າ (*p == '/' || *p == '?')
	{
	    ++p;
	    ຖ້າ (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	ໃນຂະນະທີ່ (p != command_end && vim_isspace(*p))
	    ++p;

	ໃນຂະນະທີ່ (p != command_end)
	{
	    ຖ້າ (msg_col + (*p == TAB ? 1 : ptr2cells(p)) > cmdline_width)
		msg_putchar('\n');
	    ຖ້າ (got_int)
		bha_nga;
	    msg_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    ຖ້າ (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    ຖ້າ (*p == TAB)
	    {
		msg_putchar(' ');
		++p;
	    }
	    ບໍ່ດັ່ງນັ້ນ
		p = msg_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    ຖ້າ (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		bha_nga;
	    // don't display matching '/' or '?'
	    ຖ້າ (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		bha_nga;
	}
	ຖ້າ (msg_col)
	    msg_putchar('\n');
	ui_breakcheck();
    }
    ຖ້າ (got_int)
	got_int = FALSE;	// only stop the listing
}

#ຖ້າ defined(FEAT_QUICKFIX) && defined(FEAT_EVAL)
/*
 * Add the matching tags to the location list for the current
 * window.
 */
    static ຈຳນວນເຕັມ
add_llist_tags(
    char_u	*tag,
    ຈຳນວນເຕັມ		num_matches,
    char_u	**matches)
{
    list_T	*list;
    char_u	tag_name[128 + 1];
    char_u	*fname;
    char_u	*cmd;
    ຈຳນວນເຕັມ		i;
    char_u	*p;
    tagptrs_T	tagp;

    fname = alloc(MAXPATHL + 1);
    cmd = alloc(CMDBUFFSIZE + 1);
    list = list_alloc();
    ຖ້າ (list == NULL || fname == NULL || cmd == NULL)
    {
	vim_free(cmd);
	vim_free(fname);
	ຖ້າ (list != NULL)
	    list_free(list);
	ສົ່ງຄືນ FAIL;
    }

    ສຳລັບ (i = 0; i < num_matches; ++i)
    {
	ຈຳນວນເຕັມ	    len, cmd_len;
	long    lnum;
	dict_T  *dict;

	parse_match(matches[i], &tagp);

	// Save the tag name
	len = (ຈຳນວນເຕັມ)(tagp.tagname_end - tagp.tagname);
	ຖ້າ (len > 128)
	    len = 128;
	vim_strncpy(tag_name, tagp.tagname, len);
	tag_name[len] = NUL;

	// Save the tag file name
	p = tag_full_fname(&tagp);
	ຖ້າ (p == NULL)
	    continue;
	vim_strncpy(fname, p, MAXPATHL);
	vim_free(p);

	// Get the line number or the search pattern used to locate
	// the tag.
	lnum = 0;
	ຖ້າ (SAFE_isdigit(*tagp.command))
	    // Line number is used to locate the tag
	    lnum = atol((var_na *)tagp.command);
	ບໍ່ດັ່ງນັ້ນ
	{
	    char_u *cmd_start, *cmd_end;

	    // Search pattern is used to locate the tag

	    // Locate the end of the command
	    cmd_start = tagp.command;
	    cmd_end = tagp.command_end;
	    ຖ້າ (cmd_end == NULL)
	    {
		ສຳລັບ (p = tagp.command;
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
	    ຖ້າ (*cmd_start == '/' || *cmd_start == '?')
		cmd_start++;

	    ຖ້າ (*cmd_end == '/' || *cmd_end == '?')
		cmd_end--;

	    len = 0;
	    cmd[0] = NUL;

	    // If "^" is present in the tag search pattern, then
	    // copy it first.
	    ຖ້າ (*cmd_start == '^')
	    {
		STRCPY(cmd, "^");
		cmd_start++;
		len++;
	    }

	    // Precede the tag pattern with \V to make it very
	    // nomagic.
	    STRCAT(cmd, "\\V");
	    len += 2;

	    cmd_len = (ຈຳນວນເຕັມ)(cmd_end - cmd_start + 1);
	    ຖ້າ (cmd_len > (CMDBUFFSIZE - 5))
		cmd_len = CMDBUFFSIZE - 5;
	    STRNCAT(cmd, cmd_start, cmd_len);
	    len += cmd_len;

	    ຖ້າ (cmd[len - 1] == '$')
	    {
		// Replace '$' at the end of the search pattern
		// with '\$'
		cmd[len - 1] = '\\';
		cmd[len] = '$';
		len++;
	    }

	    cmd[len] = NUL;
	}

	ຖ້າ ((dict = dict_alloc()) == NULL)
	    continue;
	ຖ້າ (list_append_dict(list, dict) == FAIL)
	{
	    dict_unref(dict);
	    continue;
	}

	dict_add_string(dict, "text", tag_name);
	dict_add_string(dict, "filename", fname);
	dict_add_number(dict, "lnum", lnum);
	ຖ້າ (lnum == 0)
	    dict_add_string(dict, "pattern", cmd);
    }

    vim_snprintf((var_na *)IObuff, IOSIZE, "ltag %s", tag);
    set_errorlist(curwin, list, ' ', IObuff, NULL);

    list_free(list);
    vim_free(fname);
    vim_free(cmd);

    ສົ່ງຄືນ OK;
}
#endif

/*
 * Free cached tags.
 */
    void
tag_freematch(void)
{
    VIM_CLEAR(tagmatchname);
}

    static void
taglen_advance(ຈຳນວນເຕັມ l)
{
    ຖ້າ (l == MAXCOL)
    {
	msg_putchar('\n');
	msg_advance(24);
    }
    ບໍ່ດັ່ງນັ້ນ
	msg_advance(13 + l);
}

/*
 * Print the tag stack
 */
    void
do_tags(exarg_T *eap UNUSED)
{
    ຈຳນວນເຕັມ		i;
    char_u	*name;
    taggy_T	*tagstack = curwin->w_tagstack;
    ຈຳນວນເຕັມ		tagstackidx = curwin->w_tagstackidx;
    ຈຳນວນເຕັມ		tagstacklen = curwin->w_tagstacklen;

    // Highlight title
    msg_puts_title(_("\n  # TO tag         FROM line  in file/text"));
    ສຳລັບ (i = 0; i < tagstacklen; ++i)
    {
	ຖ້າ (tagstack[i].tagname != NULL)
	{
	    name = fm_getname(&(tagstack[i].fmark), 30);
	    ຖ້າ (name == NULL)	    // file name not available
		continue;

	    msg_putchar('\n');
	    vim_snprintf((var_na *)IObuff, IOSIZE, "%c%2d %2d %-15s %5ld  ",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum);
	    msg_outtrans(IObuff);
	    msg_outtrans_attr(name, tagstack[i].fmark.fnum == curbuf->b_fnum
							? HL_ATTR(HLF_D) : 0);
	    vim_free(name);
	}
	out_flush();		    // show one line at a time
    }
    ຖ້າ (tagstackidx == tagstacklen)	// idx at top of stack
	msg_puts("\n>");
}

/*
 * Compare two strings, for length "len", ignoring case the ASCII way.
 * return 0 for match, < 0 for smaller, > 0 for bigger
 * Make sure case is folded to uppercase in comparison (like for 'sort -f')
 */
    static ຈຳນວນເຕັມ
tag_strnicmp(char_u *s1, char_u *s2, size_t len)
{
    ຈຳນວນເຕັມ		i;

    ໃນຂະນະທີ່ (len > 0)
    {
	i = (ຈຳນວນເຕັມ)TOUPPER_ASC(*s1) - (ຈຳນວນເຕັມ)TOUPPER_ASC(*s2);
	ຖ້າ (i != 0)
	    ສົ່ງຄືນ i;			// this character different
	ຖ້າ (*s1 == NUL)
	    bha_nga;			// strings match until NUL
	++s1;
	++s2;
	--len;
    }
    ສົ່ງຄືນ 0;				// strings match
}

/*
 * Structure to hold info about the tag pattern being used.
 */
typedef sa_nracanaa
{
    char_u	*pat;		// the pattern
    ຈຳນວນເຕັມ		len;		// length of pat[]
    char_u	*head;		// start of pattern head
    ຈຳນວນເຕັມ		headlen;	// length of head[]
    regmatch_T	regmatch;	// regexp program, may be NULL
} pat_T;

/*
 * Extract info from the tag search pattern "pats->pat".
 */
    static void
prepare_pats(pat_T *pats, ຈຳນວນເຕັມ has_re)
{
    pats->head = pats->pat;
    pats->headlen = pats->len;
    ຖ້າ (has_re)
    {
	// When the pattern starts with '^' or "\\<", binary searching can be
	// used (much faster).
	ຖ້າ (pats->pat[0] == '^')
	    pats->head = pats->pat + 1;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (pats->pat[0] == '\\' && pats->pat[1] == '<')
	    pats->head = pats->pat + 2;
	ຖ້າ (pats->head == pats->pat)
	    pats->headlen = 0;
	ບໍ່ດັ່ງນັ້ນ
	    ສຳລັບ (pats->headlen = 0; pats->head[pats->headlen] != NUL;
							      ++pats->headlen)
		ຖ້າ (vim_strchr((char_u *)(magic_isset() ? ".[~*\\$" : "\\$"),
					   pats->head[pats->headlen]) != NULL)
		    bha_nga;
	ຖ້າ (p_tl != 0 && pats->headlen > p_tl)	// adjust for 'taglength'
	    pats->headlen = p_tl;
    }

    ຖ້າ (has_re)
	pats->regmatch.regprog = vim_regcomp(pats->pat,
						 magic_isset() ? RE_MAGIC : 0);
    ບໍ່ດັ່ງນັ້ນ
	pats->regmatch.regprog = NULL;
}

#ifdef FEAT_EVAL
// Classification of a "cmd" value returned by a tagfunc.
typedef ga_nanaa {
    TAGCMD_INVALID,	// cannot be stored in a tag line
    TAGCMD_ADDRESS,	// line number or /pat/ or ?pat? (no "|" needed)
    TAGCMD_GENERIC	// any other Ex command: needs a "|" terminator
} tagcmd_T;

/*
 * Classify "cmd" from a tagfunc result (see tagcmd_T), like a tags file
 * address, to decide whether a "|" terminator must be appended before storing
 * it in a tag line.
 */
    static tagcmd_T
tagfunc_cmd_kind(char_u *cmd)
{
    ຖ້າ (VIM_ISDIGIT(*cmd))
	ສົ່ງຄືນ *skipdigits(cmd) == NUL ? TAGCMD_ADDRESS : TAGCMD_INVALID;
    ຖ້າ (*cmd == '/' || *cmd == '?')
    {
	// A Tab inside the pattern is fine (Universal Ctags emits one for a
	// tab-indented line); only trailing content after the closing
	// delimiter, which would break the tag line fields, is rejected.
	char_u	*end = skip_regexp(cmd + 1, *cmd, FALSE);

	ສົ່ງຄືນ (*end == *cmd && end[1] == NUL)
					? TAGCMD_ADDRESS : TAGCMD_INVALID;
    }
    ຖ້າ (vim_strpbrk(cmd, (char_u *)"\t\r\n") != NULL)
	ສົ່ງຄືນ TAGCMD_INVALID;
    ສົ່ງຄືນ TAGCMD_GENERIC;
}

/*
 * Call the user-defined function to generate a list of tags used by
 * find_tags().
 *
 * Return OK if at least 1 tag has been successfully found,
 * NOTDONE if the function returns v:null, and FAIL otherwise.
 */
    static ຈຳນວນເຕັມ
find_tagfunc_tags(
    char_u	*pat,		// pattern supplied to the user-defined function
    garray_T	*ga,		// the tags will be placed here
    ຈຳນວນເຕັມ		*match_count,	// here the number of tags found will be placed
    ຈຳນວນເຕັມ		flags,		// flags from find_tags (TAG_*)
    char_u	*buf_ffname)	// name of buffer for priority
{
    pos_T       save_pos;
    list_T      *taglist;
    listitem_T  *item;
    ຈຳນວນເຕັມ		ntags = 0;
    ຈຳນວນເຕັມ		result = FAIL;
    typval_T	args[4];
    typval_T	rettv;
    char_u      flagString[4];
    dict_T	*d;
    taggy_T	*tag = &curwin->w_tagstack[curwin->w_tagstackidx];

    ຖ້າ (*curbuf->b_p_tfu == NUL || curbuf->b_tfu_cb.cb_name == NULL
					   || *curbuf->b_tfu_cb.cb_name == NUL)
	ສົ່ງຄືນ FAIL;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_STRING;
    args[1].vval.v_string = flagString;

    // create 'info' dict argument
    ຖ້າ ((d = dict_alloc_lock(VAR_FIXED)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (!(flags & TAG_INS_COMP) && tag->user_data != NULL)
	dict_add_string(d, "user_data", tag->user_data);
    ຖ້າ (buf_ffname != NULL)
	dict_add_string(d, "buf_ffname", buf_ffname);

    ++d->dv_refcount;
    args[2].v_type = VAR_DICT;
    args[2].vval.v_dict = d;

    args[3].v_type = VAR_UNKNOWN;

    vim_snprintf((var_na *)flagString, parimaa_na(flagString),
		 "%s%s%s",
		 g_tag_at_cursor      ? "c": "",
		 flags & TAG_INS_COMP ? "i": "",
		 flags & TAG_REGEXP   ? "r": "");

    save_pos = curwin->w_cursor;
    result = call_callback(&curbuf->b_tfu_cb, 0, &rettv, 3, args);
    curwin->w_cursor = save_pos;	// restore the cursor position
    check_cursor();			// make sure cursor position is valid
    --d->dv_refcount;

    ຖ້າ (result == FAIL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (rettv.v_type == VAR_SPECIAL && rettv.vval.v_number == VVAL_NULL)
    {
	clear_tv(&rettv);
	ສົ່ງຄືນ NOTDONE;
    }
    ຖ້າ (rettv.v_type != VAR_LIST || !rettv.vval.v_list)
    {
	clear_tv(&rettv);
	emsg(_(e_invalid_return_value_from_tagfunc));
	ສົ່ງຄືນ FAIL;
    }
    taglist = rettv.vval.v_list;

    FOR_ALL_LIST_ITEMS(taglist, item)
    {
	char_u		*mfp;
	char_u		*res_name, *res_fname, *res_cmd, *res_kind;
	ຈຳນວນເຕັມ		len;
	dict_iterator_T	iter;
	char_u		*dict_key;
	typval_T	*tv;
	ຈຳນວນເຕັມ		has_extra = 0;
	ຈຳນວນເຕັມ		name_only = flags & TAG_NAMES;

	ຖ້າ (item->li_tv.v_type != VAR_DICT)
	{
	    emsg(_(e_invalid_return_value_from_tagfunc));
	    bha_nga;
	}

# ifdef FEAT_EMACS_TAGS
	len = 3;
# ບໍ່ດັ່ງນັ້ນ
	len = 2;
# endif
	res_name = NULL;
	res_fname = NULL;
	res_cmd = NULL;
	res_kind = NULL;

	dict_iterate_start(&item->li_tv, &iter);
	ໃນຂະນະທີ່ (NULL != (dict_key = dict_iterate_next(&iter, &tv)))
	{
	    ຖ້າ (tv->v_type != VAR_STRING || tv->vval.v_string == NULL)
		continue;

	    len += (ຈຳນວນເຕັມ)STRLEN(tv->vval.v_string) + 1;   // Space for "\tVALUE"
	    ຖ້າ (STRCMP(dict_key, "name") == 0)
	    {
		res_name = tv->vval.v_string;
		continue;
	    }
	    ຖ້າ (STRCMP(dict_key, "filename") == 0)
	    {
		res_fname = tv->vval.v_string;
		continue;
	    }
	    ຖ້າ (STRCMP(dict_key, "cmd") == 0)
	    {
		res_cmd = tv->vval.v_string;
		continue;
	    }
	    has_extra = 1;
	    ຖ້າ (STRCMP(dict_key, "kind") == 0)
	    {
		res_kind = tv->vval.v_string;
		continue;
	    }
	    // Other elements will be stored as "\tKEY:VALUE"
	    // Allocate space for the key and the colon
	    len += (ຈຳນວນເຕັມ)STRLEN(dict_key) + 1;
	}

	ຖ້າ (has_extra)
	    len += 2;	// need space for ;"

	ຖ້າ (!res_name || !res_fname || !res_cmd)
	{
	    emsg(_(e_invalid_return_value_from_tagfunc));
	    bha_nga;
	}

	tagcmd_T cmdkind = tagfunc_cmd_kind(res_cmd);
	ຖ້າ (cmdkind == TAGCMD_INVALID)
	{
	    emsg(_(e_invalid_return_value_from_tagfunc));
	    bha_nga;
	}
	ຖ້າ (cmdkind == TAGCMD_GENERIC)
	    len += 3;	// need space for "|;\""

	ຖ້າ (name_only)
	    mfp = vim_strsave(res_name);
	ບໍ່ດັ່ງນັ້ນ
	    mfp = alloc(parimaa_na(char_u) + len + 1);

	ຖ້າ (mfp == NULL)
	    continue;

	ຖ້າ (!name_only)
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

	    ຖ້າ (cmdkind == TAGCMD_GENERIC)
		*p++ = '|';	// terminate the command

	    ຖ້າ (cmdkind == TAGCMD_GENERIC || has_extra)
	    {
		STRCPY(p, ";\"");
		p += STRLEN(p);

		ຖ້າ (res_kind)
		{
		    *p++ = TAB;
		    STRCPY(p, res_kind);
		    p += STRLEN(p);
		}

		dict_iterate_start(&item->li_tv, &iter);
		ໃນຂະນະທີ່ (NULL != (dict_key = dict_iterate_next(&iter, &tv)))
		{
		    ຖ້າ (tv->v_type != VAR_STRING || tv->vval.v_string == NULL)
			continue;

		    ຖ້າ (STRCMP(dict_key, "name") == 0)
			continue;
		    ຖ້າ (STRCMP(dict_key, "filename") == 0)
			continue;
		    ຖ້າ (STRCMP(dict_key, "cmd") == 0)
			continue;
		    ຖ້າ (STRCMP(dict_key, "kind") == 0)
			continue;

		    *p++ = TAB;
		    STRCPY(p, dict_key);
		    p += STRLEN(p);
		    STRCPY(p, ":");
		    p += STRLEN(p);
		    STRCPY(p, tv->vval.v_string);
		    p += STRLEN(p);
		}
	    }
	}

	// Add all matches because tagfunc should do filtering.
	ຖ້າ (ga_grow(ga, 1) == OK)
	{
	    ((char_u **)(ga->ga_data))[ga->ga_len++] = mfp;
	    ++ntags;
	    result = OK;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    vim_free(mfp);
	    bha_nga;
	}
    }

    clear_tv(&rettv);

    *match_count = ntags;
    ສົ່ງຄືນ result;
}
#endif

/*
 * State information used during a tag search
 */
typedef sa_nracanaa
{
    tagsearch_state_T	state;		// tag search state
    ຈຳນວນເຕັມ		stop_searching;		// stop when match found or error
    pat_T	*orgpat;		// holds unconverted pattern info
    char_u     *lbuf;			// line buffer
    ຈຳນວນເຕັມ		lbuf_size;		// length of lbuf
    char_u	*tag_fname;		// name of the tag file
    FILE	*fp;			// current tags file pointer
    ຈຳນວນເຕັມ		flags;			// flags used for tag search
    ຈຳນວນເຕັມ		tag_file_sorted;	// !_TAG_FILE_SORTED value
    ຈຳນວນເຕັມ		get_searchpat;		// used for 'showfulltag'
    ຈຳນວນເຕັມ		help_only;		// only search for help tags
    ຈຳນວນເຕັມ		did_open;		// did open a tag file
    ຈຳນວນເຕັມ		mincount;		// MAXCOL: find all matches
					// other: minimal number of matches
    ຈຳນວນເຕັມ		linear;			// do a linear search
    vimconv_T	vimconv;
#ifdef FEAT_EMACS_TAGS
    ຈຳນວນເຕັມ		is_etag;		// current file is emacs style
    char_u	*ebuf;			// additional buffer for etag fname
#endif
#ifdef FEAT_MULTI_LANG
    char_u	help_lang[3];		// lang of current tags file
    ຈຳນວນເຕັມ		help_pri;		// help language priority
    char_u	*help_lang_find;	// lang to be found
    ຈຳນວນເຕັມ		is_txt;			// flag of file extension
#endif
    ຈຳນວນເຕັມ		match_count;		// number of matches found
    garray_T	ga_match[MT_COUNT];	// stores matches in sequence
    hashtab_T	ht_match[MT_COUNT];	// stores matches by key
} findtags_state_T;

/*
 * Initialize the state used by find_tags().
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static ຈຳນວນເຕັມ
findtags_state_init(
    findtags_state_T	*st,
    char_u		*pat,
    ຈຳນວນເຕັມ			flags,
    ຈຳນວນເຕັມ			mincount)
{
    ຈຳນວນເຕັມ		mtt;

    st->tag_fname = alloc(MAXPATHL + 1);
    st->fp = NULL;
    st->orgpat = ALLOC_ONE(pat_T);
    st->orgpat->pat = pat;
    st->orgpat->len = (ຈຳນວນເຕັມ)STRLEN(pat);
    st->orgpat->regmatch.regprog = NULL;
    st->flags = flags;
    st->tag_file_sorted = NUL;
    st->help_only = (flags & TAG_HELP);
    st->get_searchpat = FALSE;
#ifdef FEAT_MULTI_LANG
    st->help_lang[0] = NUL;
    st->help_pri = 0;
    st->help_lang_find = NULL;
    st->is_txt = FALSE;
#endif
    st->did_open = FALSE;
    st->mincount = mincount;
    st->lbuf_size = LSIZE;
    st->lbuf = alloc(st->lbuf_size);
#ifdef FEAT_EMACS_TAGS
    st->ebuf = alloc(LSIZE);
#endif
    st->match_count = 0;
    st->stop_searching = FALSE;

    ສຳລັບ (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	ga_init2(&st->ga_match[mtt], parimaa_na(char_u *), 100);
	hash_init(&st->ht_match[mtt]);
    }

    // check for out of memory situation
    ຖ້າ (st->tag_fname == NULL
	    || st->lbuf == NULL
#ifdef FEAT_EMACS_TAGS
	    || st->ebuf == NULL
#endif
       )
	ສົ່ງຄືນ FAIL;

    ສົ່ງຄືນ OK;
}

/*
 * Free the state used by find_tags()
 */
    static void
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
    static ຈຳນວນເຕັມ
findtags_in_help_init(findtags_state_T *st)
{
    ຈຳນວນເຕັມ		i;
    char_u	*s;

    // Keep "en" as the language if the file extension is ".txt"
    ຖ້າ (st->is_txt)
	STRCPY(st->help_lang, "en");
    ບໍ່ດັ່ງນັ້ນ
    {
	// Prefer help tags according to 'helplang'.  Put the two-letter
	// language name in help_lang[].
	i = (ຈຳນວນເຕັມ)STRLEN(st->tag_fname);
	ຖ້າ (i > 3 && st->tag_fname[i - 3] == '-')
	    vim_strncpy(st->help_lang, st->tag_fname + i - 2, 2);
	ບໍ່ດັ່ງນັ້ນ
	    STRCPY(st->help_lang, "en");
    }
    // When searching for a specific language skip tags files for other
    // languages.
    ຖ້າ (st->help_lang_find != NULL
	    && STRICMP(st->help_lang, st->help_lang_find) != 0)
	ສົ່ງຄືນ FALSE;

    // For CTRL-] in a help file prefer a match with the same language.
    ຖ້າ ((st->flags & TAG_KEEP_LANG)
	    && st->help_lang_find == NULL
	    && curbuf->b_fname != NULL
	    && (i = (ຈຳນວນເຕັມ)STRLEN(curbuf->b_fname)) > 4
	    && curbuf->b_fname[i - 1] == 'x'
	    && curbuf->b_fname[i - 4] == '.'
	    && STRNICMP(curbuf->b_fname + i - 3, st->help_lang, 2) == 0)
	st->help_pri = 0;
    ບໍ່ດັ່ງນັ້ນ
    {
	// search for the language in 'helplang'
	st->help_pri = 1;
	ສຳລັບ (s = p_hlg; *s != NUL; ++s)
	{
	    ຖ້າ (STRNICMP(s, st->help_lang, 2) == 0)
		bha_nga;
	    ++st->help_pri;
	    ຖ້າ ((s = vim_strchr(s, ',')) == NULL)
		bha_nga;
	}
	ຖ້າ (s == NULL || *s == NUL)
	{
	    // Language not in 'helplang': use last, prefer English, unless
	    // found already.
	    ++st->help_pri;
	    ຖ້າ (STRICMP(st->help_lang, "en") != 0)
		++st->help_pri;
	}
    }

    ສົ່ງຄືນ TRUE;
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
    static ຈຳນວນເຕັມ
findtags_apply_tfu(findtags_state_T *st, char_u *pat, char_u *buf_ffname)
{
    ຈຳນວນເຕັມ		use_tfu = ((st->flags & TAG_NO_TAGFUNC) == 0);
    ຈຳນວນເຕັມ		retval;

    ຖ້າ (!use_tfu || tfu_in_use || *curbuf->b_p_tfu == NUL)
	ສົ່ງຄືນ NOTDONE;

    tfu_in_use = TRUE;
    retval = find_tagfunc_tags(pat, st->ga_match, &st->match_count,
						st->flags, buf_ffname);
    tfu_in_use = FALSE;

    ສົ່ງຄືນ retval;
}
#endif

#ifdef FEAT_EMACS_TAGS
/*
 * Stack for included emacs-tags file.
 * It has a fixed size, to truncate cyclic includes. jw
 */
# define INCSTACK_SIZE 42
static sa_nracanaa
{
    FILE	*fp;
    char_u	*etag_fname;
} incstack[INCSTACK_SIZE];
static ຈຳນວນເຕັມ incstack_idx = 0;	// index in incstack

/*
 * Free the emacs include tags file stack.
 */
    static void
emacs_tags_incstack_free(void)
{
    ໃນຂະນະທີ່ (incstack_idx)
    {
	--incstack_idx;
	fclose(incstack[incstack_idx].fp);
	incstack[incstack_idx].fp = NULL;
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
    static void
emacs_tags_new_filename(findtags_state_T *st)
{
    char_u	*p;
    char_u	*fullpath_ebuf;

    ຖ້າ (vim_fgets(st->ebuf, LSIZE, st->fp))
	ສົ່ງຄືນ;

    ສຳລັບ (p = st->ebuf; *p && *p != ','; p++)
	;
    // invalid
    ຖ້າ (*p == NUL)
	ສົ່ງຄືນ;
    *p = NUL;

    // check for an included tags file.
    // atoi(p+1) is the number of bytes before the next ^L unless it is an
    // include statement. Skip the included tags file if it exceeds the
    // maximum.
    ຖ້າ (STRNCMP(p + 1, "include", 7) != 0 || incstack_idx >= INCSTACK_SIZE)
	ສົ່ງຄືນ;

    // Save current "fp" and "tag_fname" in the stack.
    incstack[incstack_idx].etag_fname = vim_strsave(st->tag_fname);
    ຖ້າ (incstack[incstack_idx].etag_fname == NULL)
	ສົ່ງຄືນ;

    incstack[incstack_idx].fp = st->fp;
    st->fp = NULL;

    // Figure out "tag_fname" and "fp" to use for
    // included file.
    fullpath_ebuf = expand_tag_fname(st->ebuf, st->tag_fname, FALSE);
    ຖ້າ (fullpath_ebuf != NULL)
    {
	st->fp = mch_fopen((var_na *)fullpath_ebuf, "r");
	ຖ້າ (st->fp != NULL)
	{
	    ຖ້າ (STRLEN(fullpath_ebuf) > LSIZE)
		semsg(_(e_tag_file_path_truncated_for_str), st->ebuf);
	    vim_strncpy(st->tag_fname, fullpath_ebuf, MAXPATHL);
	    ++incstack_idx;
	    st->is_etag = FALSE; // we can include anything
	}
	vim_free(fullpath_ebuf);
    }
    ຖ້າ (st->fp == NULL)
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
    static ຈຳນວນເຕັມ
emacs_tags_file_eof(findtags_state_T *st)
{
    ຖ້າ (!incstack_idx)	// reached end of file. stop processing.
	ສົ່ງຄືນ FALSE;

    // reached the end of an included tags file. pop it.
    --incstack_idx;
    fclose(st->fp);	// end of this file ...
    st->fp = incstack[incstack_idx].fp;
    STRCPY(st->tag_fname, incstack[incstack_idx].etag_fname);
    vim_free(incstack[incstack_idx].etag_fname);
    st->is_etag = TRUE;	// (only etags can include)

    ສົ່ງຄືນ TRUE;
}

/*
 * Parse a line from an emacs-style tags file.
 * Returns OK if the line is parsed successfully, returns FAIL if the line is
 * not terminated by a newline.
 */
    static ຈຳນວນເຕັມ
emacs_tags_parse_line(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    ຖ້າ (p_7f == NULL)
    {
etag_fail:
	ຖ້າ (vim_strchr(lbuf, '\n') != NULL)
	    ສົ່ງຄືນ FAIL;

	// Truncated line.  Ignore it.
	ຖ້າ (p_verbose >= 5)
	{
	    verbose_enter();
	    msg(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	ສົ່ງຄືນ OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    ຖ້າ (p == NULL)
	p = p_7f + 1;
    ບໍ່ດັ່ງນັ້ນ
	++p;

    ຖ້າ (!VIM_ISDIGIT(*p))	    // check for start of line number
	goto etag_fail;
    tagp->command = p;

    ຖ້າ (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    ບໍ່ດັ່ງນັ້ນ			    // second format: isolate tagname
    {
	ຖ້າ (p_7f == lbuf)
	    goto etag_fail;

	// find end of tagname
	ສຳລັບ (p = p_7f - 1; !vim_iswordc(*p); --p)
	    ຖ້າ (p == lbuf)
		goto etag_fail;
	tagp->tagname_end = p + 1;
	ໃນຂະນະທີ່ (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    ສົ່ງຄືນ OK;
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
    static tags_read_status_T
findtags_get_next_line(findtags_state_T *st, tagsearch_info_T *sinfo_p)
{
    ຈຳນວນເຕັມ		eof;
    off_T	offset;

    // For binary search: compute the next offset to use.
    ຖ້າ (st->state == TS_BINARY)
    {
	offset = sinfo_p->low_offset + ((sinfo_p->high_offset
						- sinfo_p->low_offset) / 2);
	ຖ້າ (offset == sinfo_p->curr_offset)
	    ສົ່ງຄືນ TAGS_READ_EOF; // End the binary search without a match.
	ບໍ່ດັ່ງນັ້ນ
	    sinfo_p->curr_offset = offset;
    }

    // Skipping back (after a match during binary search).
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->state == TS_SKIP_BACK)
    {
	sinfo_p->curr_offset -= st->lbuf_size * 2;
	ຖ້າ (sinfo_p->curr_offset < 0)
	{
	    sinfo_p->curr_offset = 0;
	    rewind(st->fp);
	    st->state = TS_STEP_FORWARD;
	}
    }

    // When jumping around in the file, first read a line to find the
    // start of the next line.
    ຖ້າ (st->state == TS_BINARY || st->state == TS_SKIP_BACK)
    {
	// Adjust the search file offset to the correct position
	sinfo_p->curr_offset_used = sinfo_p->curr_offset;
	vim_ignored = vim_fseek(st->fp, sinfo_p->curr_offset, SEEK_SET);
	eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	ຖ້າ (!eof && sinfo_p->curr_offset != 0)
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    ຖ້າ (sinfo_p->curr_offset == sinfo_p->high_offset)
	    {
		// oops, gone a bit too far; try from low offset
		vim_ignored = vim_fseek(st->fp, sinfo_p->low_offset, SEEK_SET);
		sinfo_p->curr_offset = sinfo_p->low_offset;
	    }
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	// skip empty and blank lines
	ໃນຂະນະທີ່ (!eof && vim_isblankline(st->lbuf))
	{
	    sinfo_p->curr_offset = vim_ftell(st->fp);
	    eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	}
	ຖ້າ (eof)
	{
	    // Hit end of file.  Skip backwards.
	    st->state = TS_SKIP_BACK;
	    sinfo_p->match_offset = vim_ftell(st->fp);
	    sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ສົ່ງຄືນ TAGS_READ_IGNORE;
	}
    }
    // Not jumping around in the file: Read the next line.
    ບໍ່ດັ່ງນັ້ນ
    {
	// skip empty and blank lines
	do
	{
#ifdef FEAT_CSCOPE
	    ຖ້າ (st->flags & TAG_CSCOPE)
		eof = cs_fgets(st->lbuf, st->lbuf_size);
	    ບໍ່ດັ່ງນັ້ນ
#endif
		eof = vim_fgets(st->lbuf, st->lbuf_size, st->fp);
	} ໃນຂະນະທີ່ (!eof && vim_isblankline(st->lbuf));

	ຖ້າ (eof)
	{
#ifdef FEAT_EMACS_TAGS
	    ຖ້າ (emacs_tags_file_eof(st) == TRUE)
		// an included tags file. Continue processing the parent
		// tags file.
		ສົ່ງຄືນ TAGS_READ_IGNORE;
#endif
	    ສົ່ງຄືນ TAGS_READ_EOF;
	}
    }

    ສົ່ງຄືນ TAGS_READ_SUCCESS;
}

/*
 * Parse a tags file header line in "st->lbuf".
 * Returns TRUE if the current line in st->lbuf is not a tags header line and
 * should be parsed as a regular tag line. Returns FALSE if the line is a
 * header line and the next header line should be read.
 */
    static ຈຳນວນເຕັມ
findtags_hdr_parse(findtags_state_T *st)
{
    char_u	*p;

    // Header lines in a tags file start with "!_TAG_"
    ຖ້າ (STRNCMP(st->lbuf, "!_TAG_", 6) != 0)
	// Non-header item before the header, e.g. "!" itself.
	ສົ່ງຄືນ TRUE;

    // Process the header line.
    ຖ້າ (STRNCMP(st->lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
	st->tag_file_sorted = st->lbuf[18];
    ຖ້າ (STRNCMP(st->lbuf, "!_TAG_FILE_ENCODING\t", 20) == 0)
    {
	// Prepare to convert every line from the specified encoding to
	// 'encoding'.
	ສຳລັບ (p = st->lbuf + 20; *p > ' ' && *p < 127; ++p)
	    ;
	*p = NUL;
	convert_setup(&st->vimconv, st->lbuf + 20, p_enc);
    }

    // Read the next line.  Unrecognized flags are ignored.
    ສົ່ງຄືນ FALSE;
}

/*
 * Handler to initialize the state when starting to process a new tags file.
 * Called in the TS_START state when finding tags from a tags file.
 * Returns TRUE if the line read from the tags file should be parsed and
 * FALSE if the line should be ignored.
 */
    static ຈຳນວນເຕັມ
findtags_start_state_handler(
    findtags_state_T	*st,
    ຈຳນວນເຕັມ			*sortic,
    tagsearch_info_T	*sinfo_p)
{
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ຈຳນວນເຕັມ		noic = (st->flags & TAG_NOIC);
    off_T	filesize;

    // The header ends when the line sorts below "!_TAG_".  When case is
    // folded lower case letters sort before "_".
    ຖ້າ (STRNCMP(st->lbuf, "!_TAG_", 6) <= 0
	    || (st->lbuf[0] == '!' && ASCII_ISLOWER(st->lbuf[1])))
	ສົ່ງຄືນ findtags_hdr_parse(st);

    // Headers ends.

    // When there is no tag head, or ignoring case, need to do a
    // linear search.
    // When no "!_TAG_" is found, default to binary search.  If
    // the tag file isn't sorted, the second loop will find it.
    // When "!_TAG_FILE_SORTED" found: start binary search if
    // flag set.
    // For cscope, it's always linear.
#ifdef FEAT_CSCOPE
    ຖ້າ (st->linear || use_cscope)
#ບໍ່ດັ່ງນັ້ນ
    ຖ້າ (st->linear)
#endif
	st->state = TS_LINEAR;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->tag_file_sorted == NUL)
	st->state = TS_BINARY;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->tag_file_sorted == '1')
	st->state = TS_BINARY;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->tag_file_sorted == '2')
    {
	st->state = TS_BINARY;
	*sortic = TRUE;
	st->orgpat->regmatch.rm_ic = (p_ic || !noic);
    }
    ບໍ່ດັ່ງນັ້ນ
	st->state = TS_LINEAR;

    ຖ້າ (st->state == TS_BINARY && st->orgpat->regmatch.rm_ic && !*sortic)
    {
	// Binary search won't work for ignoring case, use linear
	// search.
	st->linear = TRUE;
	st->state = TS_LINEAR;
    }

    // When starting a binary search, get the size of the file and
    // compute the first offset.
    ຖ້າ (st->state == TS_BINARY)
    {
	ຖ້າ (vim_fseek(st->fp, 0L, SEEK_END) != 0)
	    // can't seek, don't use binary search
	    st->state = TS_LINEAR;
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Get the tag file size (don't use mch_fstat(), it's
	    // not portable).  Don't use lseek(), it doesn't work
	    // properly on MacOS Catalina.
	    filesize = vim_ftell(st->fp);
	    vim_ignored = vim_fseek(st->fp, 0L, SEEK_SET);

	    // Calculate the first read offset in the file.  Start
	    // the search in the middle of the file.
	    sinfo_p->low_offset = 0;
	    sinfo_p->low_char = 0;
	    sinfo_p->high_offset = filesize;
	    sinfo_p->curr_offset = 0;
	    sinfo_p->high_char = 0xff;
	}
	ສົ່ງຄືນ FALSE;
    }

    ສົ່ງຄືນ TRUE;
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
    static tagmatch_status_T
findtags_parse_line(
    findtags_state_T		*st,
    tagptrs_T			*tagpp,
    findtags_match_args_T	*margs,
    tagsearch_info_T		*sinfo_p)
{
    ຈຳນວນເຕັມ		status;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		cmplen;
    ຈຳນວນເຕັມ		tagcmp;

    // Figure out where the different strings are in this line.
    // For "normal" tags: Do a quick check if the tag matches.
    // This speeds up tag searching a lot!
    ຖ້າ (st->orgpat->headlen
#ifdef FEAT_EMACS_TAGS
	    && !st->is_etag
#endif
       )
    {
	CLEAR_FIELD(*tagpp);
	tagpp->tagname = st->lbuf;
	tagpp->tagname_end = vim_strchr(st->lbuf, TAB);
	ຖ້າ (tagpp->tagname_end == NULL)
	    // Corrupted tag line.
	    ສົ່ງຄືນ TAG_MATCH_FAIL;

	// Skip this line if the length of the tag is different and
	// there is no regexp, or the tag is too short.
	cmplen = (ຈຳນວນເຕັມ)(tagpp->tagname_end - tagpp->tagname);
	ຖ້າ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	    cmplen = p_tl;
	ຖ້າ ((st->flags & TAG_REGEXP) && st->orgpat->headlen < cmplen)
	    cmplen = st->orgpat->headlen;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->state == TS_LINEAR && st->orgpat->headlen != cmplen)
	    ສົ່ງຄືນ TAG_MATCH_NEXT;

	ຖ້າ (st->state == TS_BINARY)
	{
	    // Simplistic check for unsorted tags file.
	    i = (ຈຳນວນເຕັມ)tagpp->tagname[0];
	    ຖ້າ (margs->sortic)
		i = (ຈຳນວນເຕັມ)TOUPPER_ASC(tagpp->tagname[0]);
	    ຖ້າ (i < sinfo_p->low_char || i > sinfo_p->high_char)
		margs->sort_error = TRUE;

	    // Compare the current tag with the searched tag.
	    ຖ້າ (margs->sortic)
		tagcmp = tag_strnicmp(tagpp->tagname, st->orgpat->head,
							(size_t)cmplen);
	    ບໍ່ດັ່ງນັ້ນ
		tagcmp = STRNCMP(tagpp->tagname, st->orgpat->head, cmplen);

	    // A match with a shorter tag means to search forward.
	    // A match with a longer tag means to search backward.
	    ຖ້າ (tagcmp == 0)
	    {
		ຖ້າ (cmplen < st->orgpat->headlen)
		    tagcmp = -1;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (cmplen > st->orgpat->headlen)
		    tagcmp = 1;
	    }

	    ຖ້າ (tagcmp == 0)
	    {
		// We've located the tag, now skip back and search
		// forward until the first matching tag is found.
		st->state = TS_SKIP_BACK;
		sinfo_p->match_offset = sinfo_p->curr_offset;
		ສົ່ງຄືນ TAG_MATCH_NEXT;
	    }
	    ຖ້າ (tagcmp < 0)
	    {
		sinfo_p->curr_offset = vim_ftell(st->fp);
		ຖ້າ (sinfo_p->curr_offset < sinfo_p->high_offset)
		{
		    sinfo_p->low_offset = sinfo_p->curr_offset;
		    ຖ້າ (margs->sortic)
			sinfo_p->low_char = TOUPPER_ASC(tagpp->tagname[0]);
		    ບໍ່ດັ່ງນັ້ນ
			sinfo_p->low_char = tagpp->tagname[0];
		    ສົ່ງຄືນ TAG_MATCH_NEXT;
		}
	    }
	    ຖ້າ (tagcmp > 0 && sinfo_p->curr_offset != sinfo_p->high_offset)
	    {
		sinfo_p->high_offset = sinfo_p->curr_offset;
		ຖ້າ (margs->sortic)
		    sinfo_p->high_char = TOUPPER_ASC(tagpp->tagname[0]);
		ບໍ່ດັ່ງນັ້ນ
		    sinfo_p->high_char = tagpp->tagname[0];
		ສົ່ງຄືນ TAG_MATCH_NEXT;
	    }

	    // No match yet and are at the end of the binary search.
	    ສົ່ງຄືນ TAG_MATCH_STOP;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->state == TS_SKIP_BACK)
	{
	    ຖ້າ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		st->state = TS_STEP_FORWARD;
	    ບໍ່ດັ່ງນັ້ນ
		// Have to skip back more.  Restore the curr_offset
		// used, otherwise we get stuck at a long line.
		sinfo_p->curr_offset = sinfo_p->curr_offset_used;
	    ສົ່ງຄືນ TAG_MATCH_NEXT;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (st->state == TS_STEP_FORWARD)
	{
	    ຖ້າ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
	    {
		ຖ້າ ((off_T)vim_ftell(st->fp) > sinfo_p->match_offset)
		    ສົ່ງຄືນ TAG_MATCH_STOP;	// past last match
		ບໍ່ດັ່ງນັ້ນ
		    ສົ່ງຄືນ TAG_MATCH_NEXT;	// before first match
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	    // skip this match if it can't match
	    ຖ້າ (MB_STRNICMP(tagpp->tagname, st->orgpat->head, cmplen) != 0)
		ສົ່ງຄືນ TAG_MATCH_NEXT;

	// Can be a matching tag, isolate the file name and command.
	tagpp->fname = tagpp->tagname_end + 1;
	tagpp->fname_end = vim_strchr(tagpp->fname, TAB);
	ຖ້າ (tagpp->fname_end == NULL)
	    status = FAIL;
	ບໍ່ດັ່ງນັ້ນ
	{
	    tagpp->command = tagpp->fname_end + 1;
	    status = OK;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	status = parse_tag_line(st->lbuf,
#ifdef FEAT_EMACS_TAGS
		st->is_etag,
#endif
		tagpp);

    ຖ້າ (status == FAIL)
	ສົ່ງຄືນ TAG_MATCH_FAIL;

#ifdef FEAT_EMACS_TAGS
    ຖ້າ (st->is_etag)
	tagpp->fname = st->ebuf;
#endif

    ສົ່ງຄືນ TAG_MATCH_SUCCESS;
}

/*
 * Initialize the structure used for tag matching.
 */
    static void
findtags_matchargs_init(findtags_match_args_T *margs, ຈຳນວນເຕັມ flags)
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
    static ຈຳນວນເຕັມ
findtags_match_tag(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T *margs)
{
    ຈຳນວນເຕັມ		match = FALSE;
    ຈຳນວນເຕັມ		cmplen;

    // First try matching with the pattern literally (also when it is
    // a regexp).
    cmplen = (ຈຳນວນເຕັມ)(tagpp->tagname_end - tagpp->tagname);
    ຖ້າ (p_tl != 0 && cmplen > p_tl)	    // adjust for 'taglength'
	cmplen = p_tl;
    // if tag length does not match, don't try comparing
    ຖ້າ (st->orgpat->len != cmplen)
	match = FALSE;
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (st->orgpat->regmatch.rm_ic)
	{
	    match =
		(MB_STRNICMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	    ຖ້າ (match)
		margs->match_no_ic =
		    (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
	}
	ບໍ່ດັ່ງນັ້ນ
	    match = (STRNCMP(tagpp->tagname, st->orgpat->pat, cmplen) == 0);
    }

    // Has a regexp: Also find tags matching regexp.
    margs->match_re = FALSE;
    ຖ້າ (!match && st->orgpat->regmatch.regprog != NULL)
    {
	ຈຳນວນເຕັມ	cc;

	cc = *tagpp->tagname_end;
	*tagpp->tagname_end = NUL;
	match = vim_regexec(&st->orgpat->regmatch, tagpp->tagname, (colnr_T)0);
	ຖ້າ (match)
	{
	    margs->matchoff = (ຈຳນວນເຕັມ)(st->orgpat->regmatch.startp[0] -
							tagpp->tagname);
	    ຖ້າ (st->orgpat->regmatch.rm_ic)
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

    ສົ່ງຄືນ match;
}

/*
 * Convert the encoding of a line read from a tags file in "st->lbuf".
 * Converting the pattern from 'enc' to the tags file encoding doesn't work,
 * because characters are not recognized. The converted line is saved in
 * st->lbuf.
 */
    static void
findtags_string_convert(findtags_state_T *st)
{
    char_u	*conv_line;
    ຈຳນວນເຕັມ		len;

    conv_line = string_convert(&st->vimconv, st->lbuf, NULL);
    ຖ້າ (conv_line == NULL)
	ສົ່ງຄືນ;

    // Copy or swap lbuf and conv_line.
    len = (ຈຳນວນເຕັມ)STRLEN(conv_line) + 1;
    ຖ້າ (len > st->lbuf_size)
    {
	vim_free(st->lbuf);
	st->lbuf = conv_line;
	st->lbuf_size = len;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	STRCPY(st->lbuf, conv_line);
	vim_free(conv_line);
    }
}

/*
 * Add a matching tag found in a tags file to st->ht_match and st->ga_match.
 * Returns OK if successfully added the match and FAIL on memory allocation
 * failure.
 */
    static ຈຳນວນເຕັມ
findtags_add_match(
    findtags_state_T	*st,
    tagptrs_T		*tagpp,
    findtags_match_args_T   *margs,
    char_u		*buf_ffname,
    hash_T		*hash)
{
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		use_cscope = (st->flags & TAG_CSCOPE);
#endif
    ຈຳນວນເຕັມ		name_only = (st->flags & TAG_NAMES);
    ຈຳນວນເຕັມ		mtt;
    ຈຳນວນເຕັມ		len = 0;
    ຈຳນວນເຕັມ		is_current;		// file name matches
    ຈຳນວນເຕັມ		is_static;		// current tag line is static
    char_u	*mfp;
    char_u	*p;
    char_u	*s;

#ifdef FEAT_CSCOPE
    ຖ້າ (use_cscope)
    {
	// Don't change the ordering, always use the same table.
	mtt = MT_GL_OTH;
    }
    ບໍ່ດັ່ງນັ້ນ
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
	ຖ້າ (!st->is_etag)	// emacs tags are never static
#endif
	    is_static = test_for_static(tagpp);

	// decide in which of the sixteen tables to store this
	// match
	ຖ້າ (is_static)
	{
	    ຖ້າ (is_current)
		mtt = MT_ST_CUR;
	    ບໍ່ດັ່ງນັ້ນ
		mtt = MT_ST_OTH;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (is_current)
		mtt = MT_GL_CUR;
	    ບໍ່ດັ່ງນັ້ນ
		mtt = MT_GL_OTH;
	}
	ຖ້າ (st->orgpat->regmatch.rm_ic && !margs->match_no_ic)
	    mtt += MT_IC_OFF;
	ຖ້າ (margs->match_re)
	    mtt += MT_RE_OFF;
    }

    // Add the found match in ht_match[mtt] and ga_match[mtt].
    // Store the info we need later, which depends on the kind of
    // tags we are dealing with.
    ຖ້າ (st->help_only)
    {
#ifdef FEAT_MULTI_LANG
# define ML_EXTRA 3
#ບໍ່ດັ່ງນັ້ນ
# define ML_EXTRA 0
#endif
	// Append the help-heuristic number after the tagname, for
	// sorting it later.  The heuristic is ignored for
	// detecting duplicates.
	// The format is {tagname}@{lang}NUL{heuristic}NUL
	*tagpp->tagname_end = NUL;
	len = (ຈຳນວນເຕັມ)(tagpp->tagname_end - tagpp->tagname);
	mfp = alloc(parimaa_na(char_u) + len + 10 + ML_EXTRA + 1);
	ຖ້າ (mfp != NULL)
	{
	    ຈຳນວນເຕັມ heuristic;

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
	    sprintf((var_na *)p + len + 1 + ML_EXTRA, "%06d",
		    heuristic);
	}
	*tagpp->tagname_end = TAB;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (name_only)
    {
	ຖ້າ (st->get_searchpat)
	{
	    char_u *temp_end = tagpp->command;

	    ຖ້າ (*temp_end == '/')
		ໃນຂະນະທີ່ (*temp_end && *temp_end != '\r'
			&& *temp_end != '\n'
			&& *temp_end != '$')
		    temp_end++;

	    ຖ້າ (tagpp->command + 2 < temp_end)
	    {
		len = (ຈຳນວນເຕັມ)(temp_end - tagpp->command - 2);
		mfp = alloc(len + 2);
		ຖ້າ (mfp != NULL)
		    vim_strncpy(mfp, tagpp->command + 2, len);
	    }
	    ບໍ່ດັ່ງນັ້ນ
		mfp = NULL;
	    st->get_searchpat = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    len = (ຈຳນວນເຕັມ)(tagpp->tagname_end - tagpp->tagname);
	    mfp = alloc(parimaa_na(char_u) + len + 1);
	    ຖ້າ (mfp != NULL)
		vim_strncpy(mfp, tagpp->tagname, len);

	    // if wanted, re-read line to get long form too
	    ຖ້າ (State & MODE_INSERT)
		st->get_searchpat = p_sft;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
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
	len = (ຈຳນວນເຕັມ)tag_fname_len + (ຈຳນວນເຕັມ)STRLEN(st->lbuf) + 3;
#ifdef FEAT_EMACS_TAGS
	ຖ້າ (st->is_etag)
	{
	    ebuf_len = STRLEN(st->ebuf);
	    len += (ຈຳນວນເຕັມ)ebuf_len + 1;
	}
	ບໍ່ດັ່ງນັ້ນ
	    ++len;
#endif
	mfp = alloc(parimaa_na(char_u) + len + 1);
	ຖ້າ (mfp != NULL)
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
	    ຖ້າ (st->is_etag)
	    {
		STRCPY(s, st->ebuf);
		s[ebuf_len] = TAG_SEP;
		s += ebuf_len + 1;
	    }
	    ບໍ່ດັ່ງນັ້ນ
		*s++ = TAG_SEP;
#endif
	    STRCPY(s, st->lbuf);
	}
    }

    ຖ້າ (mfp != NULL)
    {
	hashitem_T	*hi;

	// Don't add identical matches.
	// Add all cscope tags, because they are all listed.
	// "mfp" is used as a hash key, there is a NUL byte to end
	// the part that matters for comparing, more bytes may
	// follow after it.  E.g. help tags store the priority
	// after the NUL.
#ifdef FEAT_CSCOPE
	ຖ້າ (use_cscope)
	    ++*hash;
	ບໍ່ດັ່ງນັ້ນ
#endif
	    *hash = hash_hash(mfp);
	hi = hash_lookup(&st->ht_match[mtt], mfp, *hash);
	ຖ້າ (HASHITEM_EMPTY(hi))
	{
	    ຖ້າ (hash_add_item(&st->ht_match[mtt], hi, mfp, *hash) == FAIL
		    || ga_grow(&st->ga_match[mtt], 1) == FAIL)
	    {
		// Out of memory! Just forget about the rest.
		st->stop_searching = TRUE;
		ສົ່ງຄືນ FAIL;
	    }

	    ((char_u **)(st->ga_match[mtt].ga_data))
		[st->ga_match[mtt].ga_len++] = mfp;
	    st->match_count++;
	}
	ບໍ່ດັ່ງນັ້ນ
	    // duplicate tag, drop it
	    vim_free(mfp);
    }

    ສົ່ງຄືນ OK;
}

/*
 * Read and get all the tags from file st->tag_fname.
 * Sets "st->stop_searching" to TRUE to stop searching for additional tags.
 */
    static void
findtags_get_all_tags(
    findtags_state_T		*st,
    findtags_match_args_T	*margs,
    char_u			*buf_ffname)
{
    tagptrs_T		tagp;
    tagsearch_info_T	search_info;
    ຈຳນວນເຕັມ			retval;
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ			use_cscope = (st->flags & TAG_CSCOPE);
#endif
    hash_T		hash = 0;

    // This is only to avoid a compiler warning for using search_info
    // uninitialised.
    CLEAR_FIELD(search_info);

    // Read and parse the lines in the file one by one
    ສຳລັບ (;;)
    {
	// check for CTRL-C typed, more often when jumping around
	ຖ້າ (st->state == TS_BINARY || st->state == TS_SKIP_BACK)
	    line_breakcheck();
	ບໍ່ດັ່ງນັ້ນ
	    fast_breakcheck();
	ຖ້າ ((st->flags & TAG_INS_COMP))	// Double brackets for gcc
	    ins_compl_check_keys(30, FALSE);
	ຖ້າ (got_int || ins_compl_interrupted())
	{
	    st->stop_searching = TRUE;
	    bha_nga;
	}
	// When mincount is TAG_MANY, stop when enough matches have been
	// found (for completion).
	ຖ້າ (st->mincount == TAG_MANY && st->match_count >= TAG_MANY)
	{
	    st->stop_searching = TRUE;
	    bha_nga;
	}
	ຖ້າ (st->get_searchpat)
	    goto line_read_in;

	retval = findtags_get_next_line(st, &search_info);
	ຖ້າ (retval == TAGS_READ_IGNORE)
	    continue;
	ຖ້າ (retval == TAGS_READ_EOF)
	    bha_nga;

line_read_in:

	ຖ້າ (st->vimconv.vc_type != CONV_NONE)
	    findtags_string_convert(st);

#ifdef FEAT_EMACS_TAGS
	// Emacs tags line with CTRL-L: New file name on next line.
	// The file name is followed by a ','.
	// Remember etag file name in ebuf.
	ຖ້າ (*st->lbuf == Ctrl_L
# ifdef FEAT_CSCOPE
		&& !use_cscope
# endif
	   )
	{
	    st->is_etag = TRUE;		// in case at the start
	    st->state = TS_LINEAR;
	    emacs_tags_new_filename(st);
	    continue;
	}
#endif

	// When still at the start of the file, check for Emacs tags file
	// format, and for "not sorted" flag.
	ຖ້າ (st->state == TS_START)
	{
	    ຖ້າ (findtags_start_state_handler(st, &margs->sortic, &search_info) == FALSE)
		continue;
	}

	// When the line is too long the NUL will not be in the
	// last-but-one byte (see vim_fgets()).
	// Has been reported for Mozilla JS with extremely long names.
	// In that case we need to increase lbuf_size.
	ຖ້າ (st->lbuf[st->lbuf_size - 2] != NUL
#ifdef FEAT_CSCOPE
		&& !use_cscope
#endif
	   )
	{
	    st->lbuf_size *= 2;
	    vim_free(st->lbuf);
	    st->lbuf = alloc(st->lbuf_size);
	    ຖ້າ (st->lbuf == NULL)
	    {
		ຖ້າ (st->fp != NULL)
		    fclose(st->fp);
		st->fp = NULL;
		st->stop_searching = TRUE;
		ສົ່ງຄືນ;
	    }

	    ຖ້າ (st->state == TS_STEP_FORWARD || st->state == TS_LINEAR)
		// Seek to the same position to read the same line again
		vim_ignored = vim_fseek(st->fp, search_info.curr_offset,
								     SEEK_SET);
	    // this will try the same thing again, make sure the offset is
	    // different
	    search_info.curr_offset = 0;
	    continue;
	}

	retval = findtags_parse_line(st, &tagp, margs, &search_info);
	ຖ້າ (retval == TAG_MATCH_NEXT)
	    continue;
	ຖ້າ (retval == TAG_MATCH_STOP)
	    bha_nga;
	ຖ້າ (retval == TAG_MATCH_FAIL)
	{
	    semsg(_(e_format_error_in_tags_file_str), st->tag_fname);
#ifdef FEAT_CSCOPE
	    ຖ້າ (!use_cscope)
#endif
		semsg(_("Before byte %ld"), (long)vim_ftell(st->fp));
	    st->stop_searching = TRUE;
	    ສົ່ງຄືນ;
	}

	// If a match is found, add it to ht_match[] and ga_match[].
	ຖ້າ (findtags_match_tag(st, &tagp, margs))
	{
	    ຖ້າ (findtags_add_match(st, &tagp, margs, buf_ffname, &hash)
								== FAIL)
		bha_nga;
	}
    } // forever
}

/*
 * Search for tags matching "st->orgpat->pat" in the "st->tag_fname" tags file.
 * Information needed to search for the tags is in the "st" state structure.
 * The matching tags are returned in "st". If an error is encountered, then
 * "st->stop_searching" is set to TRUE.
 */
    static void
findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = NULL;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    ຖ້າ (use_cscope)
	st->fp = NULL;	    // avoid GCC warning
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
#ifdef FEAT_MULTI_LANG
	ຖ້າ (curbuf->b_help)
	{
	    ຖ້າ (!findtags_in_help_init(st))
		ສົ່ງຄືນ;
	}
#endif

	st->fp = mch_fopen((var_na *)st->tag_fname, "r");
	ຖ້າ (st->fp == NULL)
	    ສົ່ງຄືນ;

	ຖ້າ (p_verbose >= 5)
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

    ຖ້າ (st->fp != NULL)
    {
	fclose(st->fp);
	st->fp = NULL;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    ຖ້າ (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, NULL, NULL);

    ຖ້າ (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    ຖ້າ (st->match_count >= st->mincount)
	st->stop_searching = TRUE;
}

/*
 * Copy the tags found by find_tags() to "matchesp".
 * Returns the number of matches copied.
 */
    static ຈຳນວນເຕັມ
findtags_copy_matches(findtags_state_T *st, char_u ***matchesp)
{
    ຈຳນວນເຕັມ		name_only = (st->flags & TAG_NAMES);
    char_u	**matches;
    ຈຳນວນເຕັມ		mtt;
    ຈຳນວນເຕັມ		i;
    char_u	*mfp;
    char_u	*p;

    ຖ້າ (st->match_count > 0)
	matches = ALLOC_MULT(char_u *, st->match_count);
    ບໍ່ດັ່ງນັ້ນ
	matches = NULL;
    st->match_count = 0;
    ສຳລັບ (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	ສຳລັບ (i = 0; i < st->ga_match[mtt].ga_len; ++i)
	{
	    mfp = ((char_u **)(st->ga_match[mtt].ga_data))[i];
	    ຖ້າ (matches == NULL)
		vim_free(mfp);
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຖ້າ (!name_only)
		{
		    // Change mtt back to zero-based.
		    *mfp = *mfp - 1;

		    // change the TAG_SEP back to NUL
		    ສຳລັບ (p = mfp + 1; *p != NUL; ++p)
			ຖ້າ (*p == TAG_SEP)
			    *p = NUL;
		}
		matches[st->match_count++] = mfp;
	    }
	}

	ga_clear(&st->ga_match[mtt]);
	hash_clear(&st->ht_match[mtt]);
    }

    *matchesp = matches;
    ສົ່ງຄືນ st->match_count;
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
    ຈຳນວນເຕັມ
find_tags(
    char_u	*pat,			// pattern to search for
    ຈຳນວນເຕັມ		*num_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    ຈຳນວນເຕັມ		flags,
    ຈຳນວນເຕັມ		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    ຈຳນວນເຕັມ		first_file;		// trying first tag file
    ຈຳນວນເຕັມ		retval = FAIL;		// return value
    ຈຳນວນເຕັມ		round;

    ຈຳນວນເຕັມ		save_emsg_off;

    bool	help_save;
#ifdef FEAT_MULTI_LANG
    ຈຳນວນເຕັມ		i;
    char_u	*saved_pat = NULL;		// copy of pat[]
#endif

    ຈຳນວນເຕັມ		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    ຈຳນວນເຕັມ		has_re = (flags & TAG_REGEXP);	// regexp used
    ຈຳນວນເຕັມ		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    ຈຳນວນເຕັມ		use_cscope = (flags & TAG_CSCOPE);
#endif
    ຈຳນວນເຕັມ		verbose = (flags & TAG_VERBOSE);
    ຈຳນວນເຕັມ		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    switch (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	pak_sa TC_FOLLOWIC:		 bha_nga;
	pak_sa TC_IGNORE:    p_ic = TRUE;  bha_nga;
	pak_sa TC_MATCH:     p_ic = FALSE; bha_nga;
	pak_sa TC_FOLLOWSCS: p_ic = ignorecase(pat); bha_nga;
	pak_sa TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); bha_nga;
    }

    help_save = curbuf->b_help;

    ຖ້າ (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	goto findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    ຖ້າ (st.help_only)				// want tags from help file
	curbuf->b_help = true;			// will be restored later
#ifdef FEAT_CSCOPE
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = false;
    }
#endif

#ifdef FEAT_MULTI_LANG
    ຖ້າ (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	ຖ້າ (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    ຖ້າ (saved_pat != NULL)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    ຖ້າ (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    ຖ້າ (has_re && st.orgpat->regmatch.regprog == NULL)
	goto findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    ຖ້າ (retval != NOTDONE)
	goto findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    ຖ້າ ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == NULL
	    && curbuf->b_fname != NULL
	    && (i = (ຈຳນວນເຕັມ)STRLEN(curbuf->b_fname)) > 4
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
    ສຳລັບ (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      ສຳລັບ (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  ຖ້າ (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      bha_nga;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	ຖ້າ (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	ຖ້າ (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    bha_nga;
#ifdef FEAT_CSCOPE
	ຖ້າ (use_cscope)
	    bha_nga;
#endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    ຖ້າ (!st.stop_searching)
    {
	ຖ້າ (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    ຖ້າ (retval == FAIL)
	st.match_count = 0;

    *num_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    ສົ່ງຄືນ retval;
}

static garray_T tag_fnames = GA_EMPTY;

/*
 * Callback function for finding all "tags" and "tags-??" files in
 * 'runtimepath' doc directories.
 */
    static void
found_tagfile_cb(char_u *fname, void *cookie UNUSED)
{
    ຖ້າ (ga_grow(&tag_fnames, 1) == FAIL)
	ສົ່ງຄືນ;

    char_u	*tag_fname = vim_strsave(fname);

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(tag_fname);
#endif
    simplify_filename(tag_fname);
    ((char_u **)(tag_fnames.ga_data))[tag_fnames.ga_len++] = tag_fname;
}

#ຖ້າ defined(EXITFREE)
    void
free_tag_stuff(void)
{
    ga_clear_strings(&tag_fnames);
    ຖ້າ (curwin != NULL)
	do_tag(NULL, DT_FREE, 0, 0, 0);
    tag_freematch();

# ຖ້າ defined(FEAT_QUICKFIX)
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
    ຈຳນວນເຕັມ
get_tagfname(
    tagname_T	*tnp,	// holds status info
    ຈຳນວນເຕັມ		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = NULL;
    char_u		*r_ptr;
    ຈຳນວນເຕັມ			i;

    ຖ້າ (first)
	CLEAR_POINTER(tnp);

    ຖ້າ (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	ຖ້າ (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, parimaa_na(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# ບໍ່ດັ່ງນັ້ນ
		    "doc/tags doc/tags-??"
# endif
#ບໍ່ດັ່ງນັ້ນ
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, NULL);
	}

	ຖ້າ (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    ຖ້າ (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		ສົ່ງຄືນ FAIL;
	    ++tnp->tn_hf_idx;
	    vim_strncpy(buf, p_hf, MAXPATHL - STRLEN_LITERAL("tags") - 1);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    ສຳລັບ (i = 0; i < tag_fnames.ga_len; ++i)
		ຖ້າ (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    ສົ່ງຄືນ FAIL; // avoid duplicate file names
	}
	ບໍ່ດັ່ງນັ້ນ
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	ສົ່ງຄືນ OK;
    }

    ຖ້າ (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	ຖ້າ (tnp->tn_tags == NULL)
	    ສົ່ງຄືນ FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    ສຳລັບ (;;)
    {
	ຖ້າ (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_search_ctx);
	    ຖ້າ (fname != NULL)
		bha_nga;

	    tnp->tn_did_filefind_init = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    char_u  *filename = NULL;

	    // Stop when used all parts of 'tags'.
	    ຖ້າ (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_search_ctx);
		tnp->tn_search_ctx = NULL;
		ສົ່ງຄືນ FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (void)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

	    r_ptr = vim_findfile_stopdir(buf);
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    ຖ້າ (r_ptr != NULL)
	    {
		STRMOVE(r_ptr + 1, r_ptr);
		++r_ptr;
	    }
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_search_ctx = vim_findfile_init(buf, filename,
		    STRLEN(filename),
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_search_ctx, TRUE, curbuf->b_ffname);
	    ຖ້າ (tnp->tn_search_ctx != NULL)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    ສົ່ງຄືນ OK;
}

/*
 * Free the contents of a tagname_T that was filled by get_tagfname().
 */
    void
tagname_free(tagname_T *tnp)
{
    vim_free(tnp->tn_tags);
    vim_findfile_cleanup(tnp->tn_search_ctx);
    tnp->tn_search_ctx = NULL;
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
    static ຈຳນວນເຕັມ
parse_tag_line(
    char_u	*lbuf,		// line to be parsed
#ifdef FEAT_EMACS_TAGS
    ຈຳນວນເຕັມ		is_etag,
#endif
    tagptrs_T	*tagp)
{
    char_u	*p;

#ifdef FEAT_EMACS_TAGS
    ຖ້າ (is_etag)
	// emacs-style tag file
	ສົ່ງຄືນ emacs_tags_parse_line(lbuf, tagp);
#endif

    // Isolate the tagname, from lbuf up to the first white
    tagp->tagname = lbuf;
    p = vim_strchr(lbuf, TAB);
    ຖ້າ (p == NULL)
	ສົ່ງຄືນ FAIL;
    tagp->tagname_end = p;

    // Isolate file name, from first to second white space
    ຖ້າ (*p != NUL)
	++p;
    tagp->fname = p;
    p = vim_strchr(p, TAB);
    ຖ້າ (p == NULL)
	ສົ່ງຄືນ FAIL;
    tagp->fname_end = p;

    // find start of search command, after second white space
    ຖ້າ (*p != NUL)
	++p;
    ຖ້າ (*p == NUL)
	ສົ່ງຄືນ FAIL;
    tagp->command = p;

    ສົ່ງຄືນ OK;
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
    static ຈຳນວນເຕັມ
test_for_static(tagptrs_T *tagp)
{
    char_u	*p;

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    ໃນຂະນະທີ່ ((p = vim_strchr(p, '\t')) != NULL)
    {
	++p;
	ຖ້າ (STRNCMP(p, "file:", 5) == 0)
	    ສົ່ງຄືນ TRUE;
    }

    ສົ່ງຄືນ FALSE;
}

/*
 * Returns the length of a matching tag line.
 */
    static size_t
matching_line_len(char_u *lbuf)
{
    char_u	*p = lbuf + 1;

    // does the same thing as parse_match()
    p += STRLEN(p) + 1;
#ifdef FEAT_EMACS_TAGS
    p += STRLEN(p) + 1;
#endif
    ສົ່ງຄືນ (p - lbuf) + STRLEN(p);
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
    static ຈຳນວນເຕັມ
parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    ຈຳນວນເຕັມ		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    ຖ້າ (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_line(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = NULL;
    tagp->user_data = NULL;
    tagp->tagline = 0;
    tagp->command_end = NULL;

    ຖ້າ (retval != OK)
	ສົ່ງຄືນ retval;

    // Try to find a kind field: "kind:<kind>" or just "<kind>"
    p = tagp->command;
    ຖ້າ (find_extra(&p) == OK)
    {
	ຖ້າ (p > tagp->command && p[-1] == '|')
	    tagp->command_end = p - 1;  // drop trailing bar
	ບໍ່ດັ່ງນັ້ນ
	    tagp->command_end = p;
	p += 2;	// skip ";\""
	ຖ້າ (*p++ == TAB)
	    // Accept ASCII alphabetic kind characters and any multi-byte
	    // character.
	    ໃນຂະນະທີ່ (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
	    {
		ຖ້າ (STRNCMP(p, "kind:", 5) == 0)
		    tagp->tagkind = p + 5;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNCMP(p, "user_data:", 10) == 0)
		    tagp->user_data = p + 10;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNCMP(p, "line:", 5) == 0)
		    tagp->tagline = atoi((var_na *)p + 5);
		ຖ້າ (tagp->tagkind != NULL && tagp->user_data != NULL)
		    bha_nga;
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		ຖ້າ (pc == NULL || (pt != NULL && pc > pt))
		    tagp->tagkind = p;
		ຖ້າ (pt == NULL)
		    bha_nga;
		p = pt;
		MB_PTR_ADV(p);
	    }
    }
    ຖ້າ (tagp->tagkind != NULL)
    {
	ສຳລັບ (p = tagp->tagkind;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->tagkind_end = p;
    }
    ຖ້າ (tagp->user_data != NULL)
    {
	ສຳລັບ (p = tagp->user_data;
		*p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
	    ;
	tagp->user_data_end = p;
    }
    ສົ່ງຄືນ retval;
}

/*
 * Find out the actual file name of a tag.  Concatenate the tags file name
 * with the matching tag file name.
 * Returns an allocated string or NULL (out of memory).
 */
    static char_u *
tag_full_fname(tagptrs_T *tagp)
{
    char_u	*fullname;
    ຈຳນວນເຕັມ		c;

#ifdef FEAT_EMACS_TAGS
    ຖ້າ (tagp->is_etag)
	c = 0;	    // to shut up GCC
    ບໍ່ດັ່ງນັ້ນ
#endif
    {
	c = *tagp->fname_end;
	*tagp->fname_end = NUL;
    }
    fullname = expand_tag_fname(tagp->fname, tagp->tag_fname, FALSE);

#ifdef FEAT_EMACS_TAGS
    ຖ້າ (!tagp->is_etag)
#endif
	*tagp->fname_end = c;

    ສົ່ງຄືນ fullname;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    static ຈຳນວນເຕັມ
jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    ຈຳນວນເຕັມ		forceit,	// :ta with !
    ຈຳນວນເຕັມ		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    ຈຳນວນເຕັມ		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = NULL;
    char_u	*fname;
    tagptrs_T	tagp;
    ຈຳນວນເຕັມ		retval = FAIL;
    ຈຳນວນເຕັມ		getfile_result = GETFILE_UNUSED;
    ຈຳນວນເຕັມ		search_options;
#ifdef FEAT_SEARCH_EXTRA
    ຈຳນວນເຕັມ		save_no_hlsearch;
#endif
#ຖ້າ defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
#endif
    char_u	*full_fname = NULL;
#ifdef FEAT_FOLDING
    ຈຳນວນເຕັມ		old_KeyTyped = KeyTyped;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;
    ຈຳນວນເຕັມ		isdigit = FALSE;

    ຖ້າ (postponed_split == 0 && !check_can_set_curbuf_forceit(forceit))
	ສົ່ງຄືນ FAIL;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_line_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    ຖ້າ (lbuf != NULL)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc_clear(LSIZE);

    // parse the match line into the tagp structure
    ຖ້າ (pbuf == NULL || lbuf == NULL || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = NULL;
	goto erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    ຖ້າ (VIM_ISDIGIT(*str))
    {
	// need to inject a ':' for a proper Vim9 :nr command
	isdigit = TRUE;
	pbuf[0] = ':';
    }
    ສຳລັບ (pbuf_end = pbuf + isdigit;
	    *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	ຖ້າ (tagp.is_etag && *str == ',')// stop at ',' after line number
	    bha_nga;
#endif
	*pbuf_end++ = *str++;
	ຖ້າ (pbuf_end - pbuf + 1 + isdigit >= LSIZE)
	    bha_nga;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    ຖ້າ (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	// skip over the ':'
	ຖ້າ (isdigit)
	    str++;
	ຖ້າ (find_extra(&str) == OK)
	{
	    // Drop a trailing "|" that terminates a generic Ex command, so it
	    // is not executed as an empty command separator.
	    ຖ້າ (str > pbuf && str[-1] == '|')
		pbuf_end = str - 1;
	    ບໍ່ດັ່ງນັ້ນ
		pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    ຖ້າ (fname == NULL)
	goto erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    ຖ້າ (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, NULL))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	ຖ້າ (nofile_fname == NULL)
	    nofile_fname = empty_option;
	goto erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ຖ້າ defined(FEAT_QUICKFIX)
    ຖ້າ (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	ຖ້າ (!curwin->w_p_pvw)
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
    ຖ້າ (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	ຖ້າ (existing_buf != NULL)
	{
	    // If 'switchbuf' is set jump to the window containing "buf".
	    ຖ້າ (swbuf_goto_win_with_buf(existing_buf) != NULL)
		// We've switched to the buffer, the usual loading of the file
		// must be skipped.
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    ຖ້າ (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	ຖ້າ (swb_flags & SWB_VSPLIT)
	    // If 'switchbuf' contains 'vsplit', then use a new vertically
	    // split window.
	    cmdmod.cmod_split |= WSP_VERT;

	ຖ້າ ((swb_flags & SWB_NEWTAB) && cmdmod.cmod_tab == 0)
	    // If 'switchbuf' contains 'newtab', then use a new tabpage
	    cmdmod.cmod_tab = tabpage_index(curtab) + 1;

	ຖ້າ (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    ຖ້າ (RedrawingDisabled > 0)
		--RedrawingDisabled;
	    goto erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    ຖ້າ (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#ຖ້າ defined(FEAT_QUICKFIX)
	ຖ້າ (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	ບໍ່ດັ່ງນັ້ນ
#endif
	    keep_help_flag = curbuf->b_help;
    }

    ຖ້າ (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, NULL, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    ຖ້າ (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = true;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_SEARCH_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#ຖ້າ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	ຖ້າ (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	ຖ້າ (vim_strchr(p_cpo, CPO_TAGPAT) != NULL)
	    search_options = 0;
	ບໍ່ດັ່ງນັ້ນ
	    search_options = SEARCH_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	ຖ້າ (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	ຖ້າ (str > pbuf_end - 1)	// search command with nothing following
	{
	    size_t pbuflen = pbuf_end - pbuf;

	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    ຖ້າ (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    ບໍ່ດັ່ງນັ້ນ
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    ຖ້າ (do_search(NULL, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 search_options, NULL))
		retval = OK;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຈຳນວນເຕັມ	found = 1;
		ຈຳນວນເຕັມ	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		ຖ້າ (!do_search(NULL, pbuf[0], pbuf[0], pbuf + 1, pbuflen - 1, (long)1,
							 search_options, NULL))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (void)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    pbuflen = vim_snprintf((var_na *)pbuf, LSIZE, "^%s\\s\\*(", tagp.tagname);
		    ຖ້າ (!do_search(NULL, '/', '/', pbuf, pbuflen, (long)1,
							 search_options, NULL))
		    {
			// Guess again: "^char * \<func  ("
			pbuflen = vim_snprintf((var_na *)pbuf, LSIZE, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			ຖ້າ (!do_search(NULL, '/', '/', pbuf, pbuflen, (long)1,
							 search_options, NULL))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		ຖ້າ (found == 0)
		{
		    emsg(_(e_cannot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    ຖ້າ (found == 2 || !save_p_ic)
		    {
			msg(_(e_couldnt_find_tag_just_guessing));
			ຖ້າ (!msg_scrolled && msg_silent == 0)
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
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຈຳນວນເຕັມ		save_secure = secure;

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
	    ຖ້າ (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_SEARCH_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	ຖ້າ (search_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	ຖ້າ (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	ຖ້າ (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    ຖ້າ (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    ຖ້າ ((fdo_flags & FDO_TAG) && old_KeyTyped)
		foldOpenCursor();
#endif
	}

#ຖ້າ defined(FEAT_QUICKFIX)
	ຖ້າ (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	ຖ້າ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	ຖ້າ (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#ຖ້າ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    ຖ້າ (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#ຖ້າ defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    ຖ້າ (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#ຖ້າ defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    ສົ່ງຄືນ retval;
}

/*
 * If "expand" is TRUE, expand wildcards in fname.
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    static char_u *
expand_tag_fname(char_u *fname, char_u *tag_fname, ຈຳນວນເຕັມ expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = NULL;
    expand_T	xpc;

    // Refuse to follow URLs from tag files unless 'tagsecure' is false.
    // Tag entries are expected to reference local source files; a URL would
    // otherwise be passed to netrw and trigger a network request.
    ຖ້າ (p_tagsecure && path_with_url(fname))
    {
       emsg(_(e_tag_file_entry_must_not_be_url));
       ສົ່ງຄືນ NULL;
    }

    /*
     * Expand file name (for environment variables) when needed.
     * Disallow backticks, they could execute arbitrary shell
     * commands.  This is not needed for tag filenames.
     */
    ຖ້າ (expand && mch_has_wildcard(fname) && vim_strchr(fname, '`') == NULL)
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, NULL,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	ຖ້າ (expanded_fname != NULL)
	    fname = expanded_fname;
    }

    ຖ້າ ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	ຖ້າ (retval != NULL)
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
    ບໍ່ດັ່ງນັ້ນ
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    ສົ່ງຄືນ retval;
}

/*
 * Check if we have a tag for the buffer with name "buf_ffname".
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    static ຈຳນວນເຕັມ
test_for_current(
#ifdef FEAT_EMACS_TAGS
    ຈຳນວນເຕັມ	    is_etag,
#endif
    char_u  *fname,
    char_u  *fname_end,
    char_u  *tag_fname,
    char_u  *buf_ffname)
{
    ຈຳນວນເຕັມ	    c;
    ຈຳນວນເຕັມ	    retval = FALSE;
    char_u  *fullname;

    ຖ້າ (buf_ffname != NULL)	// if the buffer has a name
    {
#ifdef FEAT_EMACS_TAGS
	ຖ້າ (is_etag)
	    c = 0;	    // to shut up GCC
	ບໍ່ດັ່ງນັ້ນ
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	fullname = expand_tag_fname(fname, tag_fname, TRUE);
	ຖ້າ (fullname != NULL)
	{
	    retval = (fullpathcmp(fullname, buf_ffname, TRUE, TRUE) & FPC_SAME);
	    vim_free(fullname);
	}
#ifdef FEAT_EMACS_TAGS
	ຖ້າ (!is_etag)
#endif
	    *fname_end = c;
    }

    ສົ່ງຄືນ retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"" is following, FAIL otherwise.
 */
    static ຈຳນວນເຕັມ
find_extra(char_u **pp)
{
    char_u	*str = *pp;
    char_u	first_char = **pp;

    // Repeat for addresses separated with ';'
    ສຳລັບ (;;)
    {
	ຖ້າ (VIM_ISDIGIT(*str))
	    str = skipdigits(str + 1);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    ຖ້າ (*str != first_char)
		str = NULL;
	    ບໍ່ດັ່ງນັ້ນ
		++str;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // not a line number or search string, look for terminator.
	    str = (char_u *)strstr((var_na *)str, "|;\"");
	    ຖ້າ (str != NULL)
	    {
		++str;
		bha_nga;
	    }

	}
	ຖ້າ (str == NULL || *str != ';'
		  || !(VIM_ISDIGIT(str[1]) || str[1] == '/' || str[1] == '?'))
	    bha_nga;
	++str;	// skip ';'
	first_char = *str;
    }

    ຖ້າ (str != NULL && STRNCMP(str, ";\"", 2) == 0)
    {
	*pp = str;
	ສົ່ງຄືນ OK;
    }
    ສົ່ງຄືນ FAIL;
}

/*
 * Free a single entry in a tag stack
 */
    void
tagstack_clear_entry(taggy_T *item)
{
    VIM_CLEAR(item->tagname);
    VIM_CLEAR(item->user_data);
}

    ຈຳນວນເຕັມ
expand_tags(
    ຈຳນວນເຕັມ		tagnames,	// expand tag names
    char_u	*pat,
    ຈຳນວນເຕັມ		*num_file,
    char_u	***file)
{
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		extra_flag;
    char_u	*name_buf;
    size_t	name_buf_size = 100;
    tagptrs_T	t_p;
    ຈຳນວນເຕັມ		ret;

    name_buf = alloc(name_buf_size);
    ຖ້າ (name_buf == NULL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (tagnames)
	extra_flag = TAG_NAMES;
    ບໍ່ດັ່ງນັ້ນ
	extra_flag = 0;
    ຖ້າ (pat[0] == '/')
	ret = find_tags(pat + 1, num_file, file,
		TAG_REGEXP | extra_flag | TAG_VERBOSE,
		TAG_MANY, curbuf->b_ffname);
    ບໍ່ດັ່ງນັ້ນ
	ret = find_tags(pat, num_file, file,
	      TAG_REGEXP | extra_flag | TAG_VERBOSE | TAG_NOIC,
		TAG_MANY, curbuf->b_ffname);
    ຖ້າ (ret == OK && !tagnames)
    {
	 // Reorganize the tags for display and matching as strings of:
	 // "<tagname>\0<kind>\0<filename>\0"
	 ສຳລັບ (i = 0; i < *num_file; i++)
	 {
	     size_t	len;

	     parse_match((*file)[i], &t_p);
	     len = t_p.tagname_end - t_p.tagname;
	     ຖ້າ (len > name_buf_size - 3)
	     {
		 char_u *buf;

		 name_buf_size = len + 3;
		 buf = vim_realloc(name_buf, name_buf_size);
		 ຖ້າ (buf == NULL)
		 {
		     vim_free(name_buf);
		     ສົ່ງຄືນ FAIL;
		 }
		 name_buf = buf;
	     }

	     mch_memmove(name_buf, t_p.tagname, len);
	     name_buf[len++] = 0;
	     name_buf[len++] = (t_p.tagkind != NULL && *t_p.tagkind)
							  ? *t_p.tagkind : 'f';
	     name_buf[len++] = 0;
	     mch_memmove((*file)[i] + len, t_p.fname,
						    t_p.fname_end - t_p.fname);
	     (*file)[i][len + (t_p.fname_end - t_p.fname)] = 0;
	     mch_memmove((*file)[i], name_buf, len);
	}
    }

    vim_free(name_buf);
    ສົ່ງຄືນ ret;
}

#ຖ້າ defined(FEAT_EVAL)
/*
 * Add a tag field to the dictionary "dict".
 * Return OK or FAIL.
 */
    static ຈຳນວນເຕັມ
add_tag_field(
    dict_T  *dict,
    var_na    *field_name,
    char_u  *start,		// start of the value
    char_u  *end)		// after the value; can be NULL
{
    char_u	*buf;
    ຈຳນວນເຕັມ		len = 0;
    ຈຳນວນເຕັມ		retval;

    // check that the field name doesn't exist yet
    ຖ້າ (dict_has_key(dict, field_name))
    {
	ຖ້າ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("Duplicate field name: %s"), field_name);
	    verbose_leave();
	}
	ສົ່ງຄືນ FAIL;
    }
    buf = alloc(MAXPATHL);
    ຖ້າ (buf == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (start != NULL)
    {
	ຖ້າ (end == NULL)
	{
	    end = start + STRLEN(start);
	    ໃນຂະນະທີ່ (end > start && (end[-1] == '\r' || end[-1] == '\n'))
		--end;
	}
	len = (ຈຳນວນເຕັມ)(end - start);
	ຖ້າ (len > MAXPATHL - 1)
	    len = MAXPATHL - 1;
	vim_strncpy(buf, start, len);
    }
    buf[len] = NUL;
    retval = dict_add_string(dict, field_name, buf);
    vim_free(buf);
    ສົ່ງຄືນ retval;
}

/*
 * Add the tags matching the specified pattern "pat" to the list "list"
 * as a dictionary. Use "buf_fname" for priority, unless NULL.
 */
    ຈຳນວນເຕັມ
get_tags(list_T *list, char_u *pat, char_u *buf_fname)
{
    ຈຳນວນເຕັມ		num_matches, i, ret;
    char_u	**matches, *p;
    char_u	*full_fname;
    dict_T	*dict;
    tagptrs_T	tp;
    long	is_static;

    ret = find_tags(pat, &num_matches, &matches,
				TAG_REGEXP | TAG_NOIC, (ຈຳນວນເຕັມ)MAXCOL, buf_fname);
    ຖ້າ (ret != OK || num_matches <= 0)
	ສົ່ງຄືນ ret;

    ສຳລັບ (i = 0; i < num_matches; ++i)
    {
	ຖ້າ (parse_match(matches[i], &tp) == FAIL)
	{
	    vim_free(matches[i]);
	    continue;
	}

	is_static = test_for_static(&tp);

	// Skip pseudo-tag lines.
	ຖ້າ (STRNCMP(tp.tagname, "!_TAG_", 6) == 0)
	{
	    vim_free(matches[i]);
	    continue;
	}

	ຖ້າ ((dict = dict_alloc()) == NULL)
	{
	    ret = FAIL;
	    vim_free(matches[i]);
	    bha_nga;
	}
	ຖ້າ (list_append_dict(list, dict) == FAIL)
	{
	    ret = FAIL;
	    dict_unref(dict);
	    vim_free(matches[i]);
	    continue;
	}

	full_fname = tag_full_fname(&tp);
	ຖ້າ (add_tag_field(dict, "name", tp.tagname, tp.tagname_end) == FAIL
		|| add_tag_field(dict, "filename", full_fname,
		    NULL) == FAIL
		|| add_tag_field(dict, "cmd", tp.command,
		    tp.command_end) == FAIL
		|| add_tag_field(dict, "kind", tp.tagkind,
		    tp.tagkind_end) == FAIL
		|| dict_add_number(dict, "static", is_static) == FAIL)
	    ret = FAIL;

	vim_free(full_fname);

	ຖ້າ (tp.command_end != NULL)
	{
	    ສຳລັບ (p = tp.command_end + 3;
		    *p != NUL && *p != '\n' && *p != '\r'; MB_PTR_ADV(p))
	    {
		ຖ້າ (p == tp.tagkind || (p + 5 == tp.tagkind
			    && STRNCMP(p, "kind:", 5) == 0))
		    // skip "kind:<kind>" and "<kind>"
		    p = tp.tagkind_end - 1;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNCMP(p, "file:", 5) == 0)
		    // skip "file:" (static tag)
		    p += 4;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (!VIM_ISWHITE(*p))
		{
		    char_u	*s, *n;
		    ຈຳນວນເຕັມ	len;

		    // Add extra field as a dict entry.  Fields are
		    // separated by Tabs.
		    n = p;
		    ໃນຂະນະທີ່ (*p != NUL && *p >= ' ' && *p < 127 && *p != ':')
			++p;
		    len = (ຈຳນວນເຕັມ)(p - n);
		    ຖ້າ (*p == ':' && len > 0)
		    {
			s = ++p;
			ໃນຂະນະທີ່ (*p != NUL && *p >= ' ')
			    ++p;
			n[len] = NUL;
			ຖ້າ (add_tag_field(dict, (var_na *)n, s, p) == FAIL)
			    ret = FAIL;
			n[len] = ':';
		    }
		    ບໍ່ດັ່ງນັ້ນ
			// Skip field without colon.
			ໃນຂະນະທີ່ (*p != NUL && *p >= ' ')
			    ++p;
		    ຖ້າ (*p == NUL)
			bha_nga;
		}
	    }
	}

	vim_free(matches[i]);
    }
    vim_free(matches);
    ສົ່ງຄືນ ret;
}

/*
 * Return information about 'tag' in dict 'retdict'.
 */
    static void
get_tag_details(taggy_T *tag, dict_T *retdict)
{
    list_T	*pos;
    fmark_T	*fmark;

    dict_add_string(retdict, "tagname", tag->tagname);
    dict_add_number(retdict, "matchnr", tag->cur_match + 1);
    dict_add_number(retdict, "bufnr", tag->cur_fnum);
    ຖ້າ (tag->user_data)
	dict_add_string(retdict, "user_data", tag->user_data);

    ຖ້າ ((pos = list_alloc_id(aid_tagstack_from)) == NULL)
	ສົ່ງຄືນ;
    dict_add_list(retdict, "from", pos);

    fmark = &tag->fmark;
    list_append_number(pos,
			(varnumber_T)(fmark->fnum != -1 ? fmark->fnum : 0));
    list_append_number(pos, (varnumber_T)fmark->mark.lnum);
    list_append_number(pos, (varnumber_T)(fmark->mark.col == MAXCOL ?
					MAXCOL : fmark->mark.col + 1));
    list_append_number(pos, (varnumber_T)fmark->mark.coladd);
}

/*
 * Return the tag stack entries of the specified window 'wp' in dictionary
 * 'retdict'.
 */
    void
get_tagstack(win_T *wp, dict_T *retdict)
{
    list_T	*l;
    ຈຳນວນເຕັມ		i;
    dict_T	*d;

    dict_add_number(retdict, "length", wp->w_tagstacklen);
    dict_add_number(retdict, "curidx", wp->w_tagstackidx + 1);
    l = list_alloc_id(aid_tagstack_items);
    ຖ້າ (l == NULL)
	ສົ່ງຄືນ;
    dict_add_list(retdict, "items", l);

    ສຳລັບ (i = 0; i < wp->w_tagstacklen; i++)
    {
	ຖ້າ ((d = dict_alloc_id(aid_tagstack_details)) == NULL)
	    ສົ່ງຄືນ;
	ຖ້າ (list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ສົ່ງຄືນ;
	}

	get_tag_details(&wp->w_tagstack[i], d);
    }
}

/*
 * Free all the entries in the tag stack of the specified window
 */
    static void
tagstack_clear(win_T *wp)
{
    ຈຳນວນເຕັມ i;

    // Free the current tag stack
    ສຳລັບ (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    wp->w_tagstacklen = 0;
    wp->w_tagstackidx = 0;
}

/*
 * Remove the oldest entry from the tag stack and shift the rest of
 * the entries to free up the top of the stack.
 */
    static void
tagstack_shift(win_T *wp)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ຈຳນວນເຕັມ		i;

    tagstack_clear_entry(&tagstack[0]);
    ສຳລັບ (i = 1; i < wp->w_tagstacklen; ++i)
	tagstack[i - 1] = tagstack[i];
    wp->w_tagstacklen--;
}

/*
 * Push a new item to the tag stack
 */
    static void
tagstack_push_item(
	win_T	*wp,
	char_u	*tagname,
	ຈຳນວນເຕັມ	cur_fnum,
	ຈຳນວນເຕັມ	cur_match,
	pos_T	mark,
	ຈຳນວນເຕັມ	fnum,
	char_u  *user_data)
{
    taggy_T	*tagstack = wp->w_tagstack;
    ຈຳນວນເຕັມ		idx = wp->w_tagstacklen;	// top of the stack

    // if the tagstack is full: remove the oldest entry
    ຖ້າ (idx >= TAGSTACKSIZE)
    {
	tagstack_shift(wp);
	idx = TAGSTACKSIZE - 1;
    }

    wp->w_tagstacklen++;
    tagstack[idx].tagname = tagname;
    tagstack[idx].cur_fnum = cur_fnum;
    tagstack[idx].cur_match = cur_match;
    ຖ້າ (tagstack[idx].cur_match < 0)
	tagstack[idx].cur_match = 0;
    tagstack[idx].fmark.mark = mark;
    tagstack[idx].fmark.fnum = fnum;
    tagstack[idx].user_data = user_data;
}

/*
 * Add a list of items to the tag stack in the specified window
 */
    static void
tagstack_push_items(win_T *wp, list_T *l)
{
    listitem_T	*li;
    dictitem_T	*di;
    dict_T	*itemdict;
    char_u	*tagname;
    pos_T	mark;
    ຈຳນວນເຕັມ		fnum;

    // Add one entry at a time to the tag stack
    FOR_ALL_LIST_ITEMS(l, li)
    {
	ຖ້າ (li->li_tv.v_type != VAR_DICT || li->li_tv.vval.v_dict == NULL)
	    continue;				// Skip non-dict items
	itemdict = li->li_tv.vval.v_dict;

	// parse 'from' for the cursor position before the tag jump
	ຖ້າ ((di = dict_find(itemdict, (char_u *)"from", -1)) == NULL)
	    continue;
	ຖ້າ (list2fpos(&di->di_tv, &mark, &fnum, NULL, FALSE) != OK)
	    continue;
	ຖ້າ ((tagname = dict_get_string(itemdict, "tagname", TRUE)) == NULL)
	    continue;

	ຖ້າ (mark.col > 0)
	    mark.col--;
	tagstack_push_item(wp, tagname,
		(ຈຳນວນເຕັມ)dict_get_number(itemdict, "bufnr"),
		(ຈຳນວນເຕັມ)dict_get_number(itemdict, "matchnr") - 1,
		mark, fnum,
		dict_get_string(itemdict, "user_data", TRUE));
    }
}

/*
 * Set the current index in the tag stack. Valid values are between 0
 * and the stack length (inclusive).
 */
    static void
tagstack_set_curidx(win_T *wp, ຈຳນວນເຕັມ curidx)
{
    wp->w_tagstackidx = curidx;
    ຖ້າ (wp->w_tagstackidx < 0)			// sanity check
	wp->w_tagstackidx = 0;
    ຖ້າ (wp->w_tagstackidx > wp->w_tagstacklen)
	wp->w_tagstackidx = wp->w_tagstacklen;
}

/*
 * Set the tag stack entries of the specified window.
 * 'action' is set to one of:
 *	'a' for append
 *	'r' for replace
 *	't' for truncate
 */
    ຈຳນວນເຕັມ
set_tagstack(win_T *wp, dict_T *d, ຈຳນວນເຕັມ action)
{
    dictitem_T	*di;
    list_T	*l = NULL;

# ifdef FEAT_EVAL
    // not allowed to alter the tag stack entries from inside tagfunc
    ຖ້າ (tfu_in_use)
    {
	emsg(_(e_cannot_modify_tag_stack_within_tagfunc));
	ສົ່ງຄືນ FAIL;
    }
# endif

    ຖ້າ ((di = dict_find(d, (char_u *)"items", -1)) != NULL)
    {
	ຖ້າ (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    ສົ່ງຄືນ FAIL;
	}
	l = di->di_tv.vval.v_list;
    }

    ຖ້າ ((di = dict_find(d, (char_u *)"curidx", -1)) != NULL)
	tagstack_set_curidx(wp, (ຈຳນວນເຕັມ)tv_get_number(&di->di_tv) - 1);

    ຖ້າ (action == 't')		    // truncate the stack
    {
	taggy_T	*tagstack = wp->w_tagstack;
	ຈຳນວນເຕັມ	tagstackidx = wp->w_tagstackidx;
	ຈຳນວນເຕັມ	tagstacklen = wp->w_tagstacklen;

	// delete all the tag stack entries above the current entry
	ໃນຂະນະທີ່ (tagstackidx < tagstacklen)
	    tagstack_clear_entry(&tagstack[--tagstacklen]);
	wp->w_tagstacklen = tagstacklen;
    }

    ຖ້າ (l != NULL)
    {
	ຖ້າ (action == 'r')		// replace the stack
	    tagstack_clear(wp);

	tagstack_push_items(wp, l);
	// set the current index after the last entry
	wp->w_tagstackidx = wp->w_tagstacklen;
    }

    ສົ່ງຄືນ OK;
}
#endif
