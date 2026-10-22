/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * arglist.c: functions for dealing with the argument list
 */

#অন্তর্ভাৱ "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
স্থির পূর্ণাঙ্ক arglist_locked = FALSE;

    স্থির পূর্ণাঙ্ক
check_arglist_locked(শূন্য)
{
    যদি (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    শূন্য
alist_clear(alist_T *al)
{
    যদি (check_arglist_locked() == FAIL)
	ঘূৰাই;
    যেতিয়ালৈ (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    শূন্য
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, পরিমাণ(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    শূন্য
alist_unlink(alist_T *al)
{
    যদি (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    শূন্য
alist_new(শূন্য)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    যদি (curwin->w_alist == রিক্ত)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    অন্যথা
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#যদি !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    শূন্য
alist_expand(পূর্ণাঙ্ক *fnum_list, পূর্ণাঙ্ক fnum_len)
{
    char_u	**old_arg_files;
    পূর্ণাঙ্ক		old_arg_গুনতি;
    char_u	**new_arg_files;
    পূর্ণাঙ্ক		new_arg_file_গুনতি;
    char_u	*save_p_su = p_su;
    পূর্ণাঙ্ক		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    যদি (old_arg_files == রিক্ত)
	ঘূৰাই;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    বাবে (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_গুনতি = GARGCOUNT;
    যদি (expand_wildcards(old_arg_গুনতি, old_arg_files,
		&new_arg_file_গুনতি, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_গুনতি > 0)
    {
	alist_set(&global_alist, new_arg_file_গুনতি, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_গুনতি, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    শূন্য
alist_set(
    alist_T	*al,
    পূর্ণাঙ্ক		গুনতি,
    char_u	**files,
    পূর্ণাঙ্ক		use_curbuf,
    পূর্ণাঙ্ক		*fnum_list,
    পূর্ণাঙ্ক		fnum_len)
{
    পূর্ণাঙ্ক		i;

    যদি (check_arglist_locked() == FAIL)
	ঘূৰাই;

    alist_clear(al);
    যদি (GA_GROW_OK(&al->al_ga, গুনতি))
    {
	বাবে (i = 0; i < গুনতি; ++i)
	{
	    যদি (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		যেতিয়ালৈ (i < গুনতি)
		    vim_free(files[i++]);
		ভঙ্গ;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    যদি (fnum_list != রিক্ত && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_set_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_add(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    অন্যথা
	FreeWild(গুনতি, files);
    যদি (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    শূন্য
alist_add(
    alist_T	*al,
    char_u	*fname,
    পূর্ণাঙ্ক		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    যদি (fname == রিক্ত)		// don't add NULL file names
	ঘূৰাই;
    যদি (check_arglist_locked() == FAIL)
	ঘূৰাই;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    যদি (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#যদি defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    শূন্য
alist_slash_adjust(শূন্য)
{
    পূর্ণাঙ্ক		i;
    win_T	*wp;
    tabpage_T	*tp;

    বাবে (i = 0; i < GARGCOUNT; ++i)
	যদি (GARGLIST[i].ae_fname != রিক্ত)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_alist != &global_alist)
	    বাবে (i = 0; i < WARGCOUNT(wp); ++i)
		যদি (WARGLIST(wp)[i].ae_fname != রিক্ত)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    স্থির char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    পূর্ণাঙ্ক		inbacktick;

    inbacktick = FALSE;
    বাবে (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	যদি (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	অন্যথা
	{
	    // An item ends at a space not in backticks
	    যদি (!inbacktick && vim_isspace(*str))
		ভঙ্গ;
	    যদি (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    ঘূৰাই str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    স্থির পূর্ণাঙ্ক
get_arglist(garray_T *gap, char_u *str, পূর্ণাঙ্ক escaped)
{
    ga_init2(gap, পরিমাণ(char_u *), 20);
    যেতিয়ালৈ (*str != NUL)
    {
	যদি (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    ঘূৰাই FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	যদি (!escaped)
	    ঘূৰাই OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    ঘূৰাই OK;
}

#যদি defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    পূর্ণাঙ্ক
get_arglist_exp(
    char_u	*str,
    পূর্ণাঙ্ক		*fcountp,
    char_u	***fnamesp,
    পূর্ণাঙ্ক		wig)
{
    garray_T	ga;
    পূর্ণাঙ্ক		i;

    যদি (get_arglist(&ga, str, TRUE) == FAIL)
	ঘূৰাই FAIL;
    যদি (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    অন্যথা
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    ঘূৰাই i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    স্থির শূন্য
alist_check_arg_idx(শূন্য)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	যদি (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    স্থির শূন্য
alist_add_list(
    পূর্ণাঙ্ক		গুনতি,
    char_u	**files,
    পূর্ণাঙ্ক		after,	    // where to add: 0 = before first one
    পূর্ণাঙ্ক		will_edit)  // will edit adding argument
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		old_argcount = ARGCOUNT;

    যদি (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, গুনতি))
    {
	win_T	*wp = curwin;

	যদি (after < 0)
	    after = 0;
	যদি (after > ARGCOUNT)
	    after = ARGCOUNT;
	যদি (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + গুনতি]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * পরিমাণ(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	বাবে (i = 0; i < গুনতি; ++i)
	{
	    পূর্ণাঙ্ক flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += গুনতি;
	যদি (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += গুনতি;
	ঘূৰাই;
    }

    বাবে (i = 0; i < গুনতি; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    স্থির শূন্য
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		didone;
    পূর্ণাঙ্ক		i;
    char_u	*p;
    পূর্ণাঙ্ক		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    বাবে (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, রিক্ত, রিক্ত, FALSE);
	যদি (p == রিক্ত)
	    ভঙ্গ;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	যদি (regmatch.regprog == রিক্ত)
	{
	    vim_free(p);
	    ভঙ্গ;
	}

	didone = FALSE;
	বাবে (match = 0; match < ARGCOUNT; ++match)
	    যদি (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * পরিমাণ(aentry_T));
		--ARGCOUNT;
		যদি (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	যদি (!didone)
	    semsg(_(e_no_match_str_2), ((char_u **)alist_ga->ga_data)[i]);
    }
    ga_clear(alist_ga);
}

/*
 * "what" == AL_SET: Redefine the argument list to 'str'.
 * "what" == AL_ADD: add files in 'str' to the argument list after "after".
 * "what" == AL_DEL: remove files in 'str' from the argument list.
 *
 * Return FAIL for failure, OK otherwise.
 */
    স্থির পূর্ণাঙ্ক
do_arglist(
    char_u	*str,
    পূর্ণাঙ্ক		what,
    পূর্ণাঙ্ক		after UNUSED,	// 0 means before first one
    পূর্ণাঙ্ক		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    পূর্ণাঙ্ক		exp_গুনতি;
    char_u	**exp_files;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		arg_escaped = TRUE;

    যদি (check_arglist_locked() == FAIL)
	ঘূৰাই FAIL;

    // Set default argument for ":argadd" command.
    যদি (what == AL_ADD && *str == NUL)
    {
	যদি (curbuf->b_ffname == রিক্ত)
	    ঘূৰাই FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    যদি (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	ঘূৰাই FAIL;

    যদি (what == AL_DEL)
	arglist_del_files(&new_ga);
    অন্যথা
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_গুনতি, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	যদি (i == FAIL || exp_গুনতি == 0)
	{
	    emsg(_(e_no_match));
	    ঘূৰাই FAIL;
	}

	যদি (what == AL_ADD)
	{
	    alist_add_list(exp_গুনতি, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	অন্যথা // what == AL_SET
	    alist_set(ALIST(curwin), exp_গুনতি, exp_files, will_edit, রিক্ত, 0);
    }

    alist_check_arg_idx();

    ঘূৰাই OK;
}

/*
 * Redefine the argument list.
 */
    শূন্য
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    পূর্ণাঙ্ক
editing_arg_idx(win_T *win)
{
    ঘূৰাই !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == রিক্ত
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    শূন্য
check_arg_idx(win_T *win)
{
    যদি (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	যদি (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != রিক্ত
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    অন্যথা
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	যদি (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    শূন্য
ex_args(exarg_T *eap)
{
    পূর্ণাঙ্ক		i;

    যদি (eap->cmdidx != CMD_args)
    {
	যদি (check_arglist_locked() == FAIL)
	    ঘূৰাই;
	alist_unlink(ALIST(curwin));
	যদি (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	অন্যথা // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    যদি (*eap->arg != NUL)
    {
	যদি (check_arglist_locked() == FAIL)
	    ঘূৰাই;
	ex_next(eap);
	ঘূৰাই;
    }

    // ":args": list arguments.
    যদি (eap->cmdidx == CMD_args)
    {
	char_u **বস্তুসমূহ;

	যদি (ARGCOUNT <= 0)
	    ঘূৰাই;		// empty argument list

	বস্তুসমূহ = ALLOC_MULT(char_u *, ARGCOUNT);
	যদি (বস্তুসমূহ == রিক্ত)
	    ঘূৰাই;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	বাবে (i = 0; i < ARGCOUNT; ++i)
	    বস্তুসমূহ[i] = alist_name(&ARGLIST[i]);
	list_in_columns(বস্তুসমূহ, ARGCOUNT, curwin->w_arg_idx);
	vim_free(বস্তুসমূহ);

	ঘূৰাই;
    }

    // ":argslocal": make a local copy of the global argument list.
    যদি (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	যদি (GA_GROW_FAILS(gap, GARGCOUNT))
	    ঘূৰাই;

	বাবে (i = 0; i < GARGCOUNT; ++i)
	    যদি (GARGLIST[i].ae_fname != রিক্ত)
	    {
		AARGLIST(curwin->w_alist)[gap->ga_len].ae_fname =
		    vim_strsave(GARGLIST[i].ae_fname);
		AARGLIST(curwin->w_alist)[gap->ga_len].ae_fnum =
		    GARGLIST[i].ae_fnum;
		++gap->ga_len;
	    }
    }
}

/*
 * ":previous", ":sprevious", ":Next" and ":sNext".
 */
    শূন্য
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    যদি (curwin->w_arg_idx - (পূর্ণাঙ্ক)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    অন্যথা
	do_argfile(eap, curwin->w_arg_idx - (পূর্ণাঙ্ক)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    শূন্য
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    শূন্য
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    শূন্য
ex_argument(exarg_T *eap)
{
    পূর্ণাঙ্ক		i;

    যদি (eap->addr_গুনতি > 0)
	i = eap->line2 - 1;
    অন্যথা
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    শূন্য
do_argfile(exarg_T *eap, পূর্ণাঙ্ক argn)
{
    পূর্ণাঙ্ক		other;
    char_u	*p;
    পূর্ণাঙ্ক		old_arg_idx = curwin->w_arg_idx;
    পূর্ণাঙ্ক is_split_cmd = *eap->cmd == 's';

    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ঘূৰাই;
    যদি (argn < 0 || argn >= ARGCOUNT)
    {
	যদি (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	অন্যথা যদি (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	অন্যথা
	    emsg(_(e_cannot_go_beyond_last_file));

	ঘূৰাই;
    }

    যদি (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	ঘূৰাই;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    যদি (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	যদি (win_split(0, 0) == FAIL)
	    ঘূৰাই;
	RESET_BINDING(curwin);
    }
    অন্যথা
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	যদি (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	যদি ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    ঘূৰাই;
    }

    curwin->w_arg_idx = argn;
    যদি (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    যদি (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), রিক্ত,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    অন্যথা যদি (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    শূন্য
ex_next(exarg_T *eap)
{
    পূর্ণাঙ্ক		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    যদি (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	যদি (*eap->arg != NUL)		    // redefine file list
	{
	    যদি (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		ঘূৰাই;
	    i = 0;
	}
	অন্যথা
	    i = curwin->w_arg_idx + (পূর্ণাঙ্ক)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    শূন্য
ex_argdedupe(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক j;

    বাবে (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	যদি (firstFullname == রিক্ত)
	    ঘূৰাই;  // out of memory

	বাবে (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    যদি (secondFullname == রিক্ত)
		ভঙ্গ;  // out of memory
	    পূর্ণাঙ্ক areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    যদি (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * পরিমাণ(aentry_T));
		--ARGCOUNT;

		যদি (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		অন্যথা যদি (curwin->w_arg_idx > j)
		    --curwin->w_arg_idx;

		--j;
	    }
	}

	vim_free(firstFullname);
    }
}

/*
 * ":argedit"
 */
    শূন্য
ex_argedit(exarg_T *eap)
{
    পূর্ণাঙ্ক i = eap->addr_গুনতি ? (পূর্ণাঙ্ক)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    পূর্ণাঙ্ক curbuf_is_reusable = curbuf_reusable();

    যদি (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	ঘূৰাই;
    maketitle();

    যদি (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == রিক্ত || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    যদি (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    শূন্য
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_গুনতি > 0 ? (পূর্ণাঙ্ক)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    শূন্য
ex_argdelete(exarg_T *eap)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		n;

    যদি (check_arglist_locked() == FAIL)
	ঘূৰাই;

    যদি (eap->addr_গুনতি > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	যদি (eap->addr_গুনতি == 0)
	{
	    যদি (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		ঘূৰাই;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	অন্যথা যদি (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	যদি (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    ঘূৰাই;
	}
	যদি (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    যদি (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    ঘূৰাই;
	}

	বাবে (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * পরিমাণ(aentry_T)));
	ARGCOUNT -= n;
	যদি (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	অন্যথা যদি (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    অন্যথা
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    যদি (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    অন্যথা যদি (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx >= ARGCOUNT)
	ঘূৰাই রিক্ত;

    ঘূৰাই alist_name(&ARGLIST[idx]);
}

/*
 * Get the file name for an argument list entry.
 */
    char_u *
alist_name(aentry_T *aep)
{
    buf_T	*bp;

    // Use the name from the associated buffer if it exists.
    bp = buflist_findnr(aep->ae_fnum);
    যদি (bp == রিক্ত || bp->b_fname == রিক্ত)
	ঘূৰাই aep->ae_fname;
    ঘূৰাই bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
প্রকারনির্দেশ সংরচনা {
    alist_T	*alist;		// argument list to be used
    পূর্ণাঙ্ক		had_tab;
    পূর্ণাঙ্ক		keep_tabs;
    পূর্ণাঙ্ক		forceit;

    পূর্ণাঙ্ক		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    পূর্ণাঙ্ক		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    স্থির শূন্য
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    পূর্ণাঙ্ক		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    যদি (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    বাবে (;;)
    {
	tpnext = curtab->tp_next;
	বাবে (wp = firstwin; wp != রিক্ত; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    যদি (buf->b_ffname == রিক্ত
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    অন্যথা
	    {
		// check if the buffer in this window is in the arglist
		বাবে (i = 0; i < aall->opened_len; ++i)
		{
		    যদি (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			পূর্ণাঙ্ক weight = 1;

			যদি (old_curtab == curtab)
			{
			    ++weight;
			    যদি (old_curwin == wp)
				++weight;
			}

			যদি (weight > (পূর্ণাঙ্ক)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    যদি (i == 0)
			    {
				যদি (aall->new_curwin != রিক্ত)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			অন্যথা যদি (aall->keep_tabs)
			    i = aall->opened_len;

			যদি (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			ভঙ্গ;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    যদি (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		যদি (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    যদি (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(শূন্য)autowrite(buf, FALSE);

			// check if autocommands removed the window
			যদি (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    অনুৱর্ত;
			}
		    }
		    // don't close last window
		    যদি (ONE_WINDOW
			    && (first_tabpage->tp_next == রিক্ত
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    অন্যথা
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			যদি (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	যদি (aall->had_tab == 0 || tpnext == রিক্ত)
	    ভঙ্গ;

	// check if autocommands removed the next tab page
	যদি (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    স্থির শূন্য
arg_all_open_windows(arg_all_state_T *aall, পূর্ণাঙ্ক গুনতি)
{
    win_T	*wp;
    পূর্ণাঙ্ক		tab_drop_empty_window = FALSE;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		split_ret = OK;
    পূর্ণাঙ্ক		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    যদি (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == রিক্ত && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    বাবে (i = 0; i < গুনতি && !got_int; ++i)
    {
	যদি (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	যদি (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    যদি (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    যদি (wp->w_arg_idx == i)
		    {
			যদি (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			অন্যথা যদি (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = গুনতি;
			    ভঙ্গ;
			}
			অন্যথা
			    win_move_after(wp, curwin);
			ভঙ্গ;
		    }
		}
	    }
	}
	অন্যথা যদি (split_ret == OK)
	{
	    // trigger events for tab drop
	    যদি (tab_drop_empty_window && i == গুনতি - 1)
		--autocmd_no_enter;
	    যদি (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		যদি (split_ret == FAIL)
		    অনুৱর্ত;
	    }
	    অন্যথা    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    যদি (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (শূন্য)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), রিক্ত, রিক্ত,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    যদি (tab_drop_empty_window && i == গুনতি - 1)
		++autocmd_no_enter;
	    যদি (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	যদি (aall->had_tab > 0 && tabpage_index(রিক্ত) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    স্থির শূন্য
do_arg_all(
    পূর্ণাঙ্ক	গুনতি,
    পূর্ণাঙ্ক	forceit,		// hide buffers in current windows
    পূর্ণাঙ্ক keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    পূর্ণাঙ্ক			prev_arglist_locked = arglist_locked;

    যদি (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ঘূৰাই;
    }
    যদি (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	ঘূৰাই;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = রিক্ত;
    aall.new_curtab = রিক্ত;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    যদি (aall.opened == রিক্ত)
	ঘূৰাই;

    // Autocommands may do anything to the argument list.  Make sure it's not
    // freed while we are working here by "locking" it.  We still have to
    // watch out for its size being changed.
    aall.alist = curwin->w_alist;
    ++aall.alist->al_refcount;
    arglist_locked = TRUE;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    tabpage_T *new_lu_tp = curtab;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    // Try closing all windows that are not in the argument list.
    // Also close windows that are not full width;
    // When 'hidden' or "forceit" set the buffer becomes hidden.
    // Windows that have a changed buffer and can't be hidden won't be closed.
    // When the ":tab" modifier was used do this for all tab pages.
    arg_all_close_unused_windows(&aall);

    // Open a window for files in the argument list that don't have one.
    // ARGCOUNT may change while doing this, because of autocommands.
    যদি (গুনতি > aall.opened_len || গুনতি <= 0)
	গুনতি = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, গুনতি);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    যদি (last_curtab != aall.new_curtab)
    {
	যদি (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	যদি (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    যদি (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    যদি (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    যদি (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    শূন্য
ex_all(exarg_T *eap)
{
    যদি (eap->addr_গুনতি == 0)
	eap->line2 = 9999;
    do_arg_all((পূর্ণাঙ্ক)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(শূন্য)
{
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		idx;
    char_u	*retval = রিক্ত;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    বাবে (;;)
    {
	len = 0;
	বাবে (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    যদি (p == রিক্ত)
		অনুৱর্ত;
	    যদি (len > 0)
	    {
		// insert a space in between names
		যদি (retval != রিক্ত)
		    retval[len] = ' ';
		++len;
	    }
	    বাবে ( ; *p != NUL; ++p)
	    {
		যদি (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    যদি (retval != রিক্ত)
			retval[len] = '\\';
		    ++len;
		}
		যদি (retval != রিক্ত)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	যদি (retval != রিক্ত)
	{
	    retval[len] = NUL;
	    ভঙ্গ;
	}

	// allocate memory
	retval = alloc(len + 1);
	যদি (retval == রিক্ত)
	    ভঙ্গ;
    }

    ঘূৰাই retval;
}

#যদি defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    শূন্য
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_সংখ্যা = ARGCOUNT;
    অন্যথা যদি (argvars[0].v_type == VAR_সংখ্যা
					   && tv_get_সংখ্যা(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_সংখ্যা = GARGCOUNT;
    অন্যথা
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	যদি (wp != রিক্ত)
	    rettv->vval.v_সংখ্যা = WARGCOUNT(wp);
	অন্যথা
	    rettv->vval.v_সংখ্যা = -1;
    }
}

/*
 * "argidx()" function
 */
    শূন্য
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    শূন্য
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], রিক্ত);
    যদি (wp != রিক্ত)
	rettv->vval.v_সংখ্যা = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    স্থির শূন্য
get_arglist_as_rettv(aentry_T *arglist, পূর্ণাঙ্ক argcount, typval_T *rettv)
{
    পূর্ণাঙ্ক		idx;

    যদি (rettv_list_alloc(rettv) == OK && arglist != রিক্ত)
	বাবে (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    শূন্য
f_argv(typval_T *argvars, typval_T *rettv)
{
    পূর্ণাঙ্ক		idx;
    aentry_T	*arglist = রিক্ত;
    পূর্ণাঙ্ক		argcount = -1;

    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ঘূৰাই;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	ঘূৰাই;
    }

    যদি (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    অন্যথা যদি (argvars[1].v_type == VAR_সংখ্যা
	    && tv_get_সংখ্যা(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    অন্যথা
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	যদি (wp != রিক্ত)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
    idx = tv_get_সংখ্যা_chk(&argvars[0], রিক্ত);
    যদি (arglist != রিক্ত && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    অন্যথা যদি (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
