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

#شامل "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ایستا صحیح arglist_locked = FALSE;

    ایستا صحیح
check_arglist_locked(تهی)
{
    اگر (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    تهی
alist_clear(alist_T *al)
{
    اگر (check_arglist_locked() == FAIL)
	بازگشت;
    وقتی (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    تهی
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, اندازه(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    تهی
alist_unlink(alist_T *al)
{
    اگر (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    تهی
alist_new(تهی)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    اگر (curwin->w_alist == پوچ)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    وگرنه
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#اگر !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    تهی
alist_expand(صحیح *fnum_list, صحیح fnum_len)
{
    char_u	**old_arg_files;
    صحیح		old_arg_شمار;
    char_u	**new_arg_files;
    صحیح		new_arg_file_شمار;
    char_u	*save_p_su = p_su;
    صحیح		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    اگر (old_arg_files == پوچ)
	بازگشت;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    برای (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_شمار = GARGCOUNT;
    اگر (expand_wildcards(old_arg_شمار, old_arg_files,
		&new_arg_file_شمار, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_شمار > 0)
    {
	alist_set(&global_alist, new_arg_file_شمار, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_شمار, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    تهی
alist_set(
    alist_T	*al,
    صحیح		شمار,
    char_u	**files,
    صحیح		use_curbuf,
    صحیح		*fnum_list,
    صحیح		fnum_len)
{
    صحیح		i;

    اگر (check_arglist_locked() == FAIL)
	بازگشت;

    alist_clear(al);
    اگر (GA_GROW_OK(&al->al_ga, شمار))
    {
	برای (i = 0; i < شمار; ++i)
	{
	    اگر (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		وقتی (i < شمار)
		    vim_free(files[i++]);
		بشکن;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    اگر (fnum_list != پوچ && i < fnum_len)
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
    وگرنه
	FreeWild(شمار, files);
    اگر (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    تهی
alist_add(
    alist_T	*al,
    char_u	*fname,
    صحیح		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    اگر (fname == پوچ)		// don't add NULL file names
	بازگشت;
    اگر (check_arglist_locked() == FAIL)
	بازگشت;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    اگر (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#اگر defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    تهی
alist_slash_adjust(تهی)
{
    صحیح		i;
    win_T	*wp;
    tabpage_T	*tp;

    برای (i = 0; i < GARGCOUNT; ++i)
	اگر (GARGLIST[i].ae_fname != پوچ)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_alist != &global_alist)
	    برای (i = 0; i < WARGCOUNT(wp); ++i)
		اگر (WARGLIST(wp)[i].ae_fname != پوچ)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ایستا char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    صحیح		inbacktick;

    inbacktick = FALSE;
    برای (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	اگر (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	وگرنه
	{
	    // An item ends at a space not in backticks
	    اگر (!inbacktick && vim_isspace(*str))
		بشکن;
	    اگر (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    بازگشت str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ایستا صحیح
get_arglist(garray_T *gap, char_u *str, صحیح escaped)
{
    ga_init2(gap, اندازه(char_u *), 20);
    وقتی (*str != NUL)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    بازگشت FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	اگر (!escaped)
	    بازگشت OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    بازگشت OK;
}

#اگر defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    صحیح
get_arglist_exp(
    char_u	*str,
    صحیح		*fcountp,
    char_u	***fnamesp,
    صحیح		wig)
{
    garray_T	ga;
    صحیح		i;

    اگر (get_arglist(&ga, str, TRUE) == FAIL)
	بازگشت FAIL;
    اگر (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    وگرنه
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    بازگشت i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    ایستا تهی
alist_check_arg_idx(تهی)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	اگر (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    ایستا تهی
alist_add_list(
    صحیح		شمار,
    char_u	**files,
    صحیح		after,	    // where to add: 0 = before first one
    صحیح		will_edit)  // will edit adding argument
{
    صحیح		i;
    صحیح		old_argcount = ARGCOUNT;

    اگر (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, شمار))
    {
	win_T	*wp = curwin;

	اگر (after < 0)
	    after = 0;
	اگر (after > ARGCOUNT)
	    after = ARGCOUNT;
	اگر (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + شمار]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * اندازه(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	برای (i = 0; i < شمار; ++i)
	{
	    صحیح flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += شمار;
	اگر (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += شمار;
	بازگشت;
    }

    برای (i = 0; i < شمار; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ایستا تهی
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    صحیح		didone;
    صحیح		i;
    char_u	*p;
    صحیح		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    برای (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, پوچ, پوچ, FALSE);
	اگر (p == پوچ)
	    بشکن;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog == پوچ)
	{
	    vim_free(p);
	    بشکن;
	}

	didone = FALSE;
	برای (match = 0; match < ARGCOUNT; ++match)
	    اگر (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * اندازه(aentry_T));
		--ARGCOUNT;
		اگر (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	اگر (!didone)
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
    ایستا صحیح
do_arglist(
    char_u	*str,
    صحیح		what,
    صحیح		after UNUSED,	// 0 means before first one
    صحیح		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    صحیح		exp_شمار;
    char_u	**exp_files;
    صحیح		i;
    صحیح		arg_escaped = TRUE;

    اگر (check_arglist_locked() == FAIL)
	بازگشت FAIL;

    // Set default argument for ":argadd" command.
    اگر (what == AL_ADD && *str == NUL)
    {
	اگر (curbuf->b_ffname == پوچ)
	    بازگشت FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    اگر (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	بازگشت FAIL;

    اگر (what == AL_DEL)
	arglist_del_files(&new_ga);
    وگرنه
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_شمار, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	اگر (i == FAIL || exp_شمار == 0)
	{
	    emsg(_(e_no_match));
	    بازگشت FAIL;
	}

	اگر (what == AL_ADD)
	{
	    alist_add_list(exp_شمار, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	وگرنه // what == AL_SET
	    alist_set(ALIST(curwin), exp_شمار, exp_files, will_edit, پوچ, 0);
    }

    alist_check_arg_idx();

    بازگشت OK;
}

/*
 * Redefine the argument list.
 */
    تهی
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    صحیح
editing_arg_idx(win_T *win)
{
    بازگشت !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == پوچ
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    تهی
check_arg_idx(win_T *win)
{
    اگر (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	اگر (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != پوچ
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    وگرنه
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	اگر (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    تهی
ex_args(exarg_T *eap)
{
    صحیح		i;

    اگر (eap->cmdidx != CMD_args)
    {
	اگر (check_arglist_locked() == FAIL)
	    بازگشت;
	alist_unlink(ALIST(curwin));
	اگر (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	وگرنه // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    اگر (*eap->arg != NUL)
    {
	اگر (check_arglist_locked() == FAIL)
	    بازگشت;
	ex_next(eap);
	بازگشت;
    }

    // ":args": list arguments.
    اگر (eap->cmdidx == CMD_args)
    {
	char_u **اقلام;

	اگر (ARGCOUNT <= 0)
	    بازگشت;		// empty argument list

	اقلام = ALLOC_MULT(char_u *, ARGCOUNT);
	اگر (اقلام == پوچ)
	    بازگشت;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	برای (i = 0; i < ARGCOUNT; ++i)
	    اقلام[i] = alist_name(&ARGLIST[i]);
	list_in_columns(اقلام, ARGCOUNT, curwin->w_arg_idx);
	vim_free(اقلام);

	بازگشت;
    }

    // ":argslocal": make a local copy of the global argument list.
    اگر (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	اگر (GA_GROW_FAILS(gap, GARGCOUNT))
	    بازگشت;

	برای (i = 0; i < GARGCOUNT; ++i)
	    اگر (GARGLIST[i].ae_fname != پوچ)
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
    تهی
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    اگر (curwin->w_arg_idx - (صحیح)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    وگرنه
	do_argfile(eap, curwin->w_arg_idx - (صحیح)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    تهی
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    تهی
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    تهی
ex_argument(exarg_T *eap)
{
    صحیح		i;

    اگر (eap->addr_شمار > 0)
	i = eap->line2 - 1;
    وگرنه
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    تهی
do_argfile(exarg_T *eap, صحیح argn)
{
    صحیح		other;
    char_u	*p;
    صحیح		old_arg_idx = curwin->w_arg_idx;
    صحیح is_split_cmd = *eap->cmd == 's';

    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    اگر (argn < 0 || argn >= ARGCOUNT)
    {
	اگر (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	وگرنه اگر (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	وگرنه
	    emsg(_(e_cannot_go_beyond_last_file));

	بازگشت;
    }

    اگر (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	بازگشت;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    اگر (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	اگر (win_split(0, 0) == FAIL)
	    بازگشت;
	RESET_BINDING(curwin);
    }
    وگرنه
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	اگر (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	اگر ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    بازگشت;
    }

    curwin->w_arg_idx = argn;
    اگر (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    اگر (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), پوچ,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    وگرنه اگر (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    تهی
ex_next(exarg_T *eap)
{
    صحیح		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    اگر (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	اگر (*eap->arg != NUL)		    // redefine file list
	{
	    اگر (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		بازگشت;
	    i = 0;
	}
	وگرنه
	    i = curwin->w_arg_idx + (صحیح)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    تهی
ex_argdedupe(exarg_T *eap UNUSED)
{
    صحیح i;
    صحیح j;

    برای (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	اگر (firstFullname == پوچ)
	    بازگشت;  // out of memory

	برای (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    اگر (secondFullname == پوچ)
		بشکن;  // out of memory
	    صحیح areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    اگر (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * اندازه(aentry_T));
		--ARGCOUNT;

		اگر (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		وگرنه اگر (curwin->w_arg_idx > j)
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
    تهی
ex_argedit(exarg_T *eap)
{
    صحیح i = eap->addr_شمار ? (صحیح)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    صحیح curbuf_is_reusable = curbuf_reusable();

    اگر (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	بازگشت;
    maketitle();

    اگر (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == پوچ || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    اگر (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    تهی
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_شمار > 0 ? (صحیح)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    تهی
ex_argdelete(exarg_T *eap)
{
    صحیح		i;
    صحیح		n;

    اگر (check_arglist_locked() == FAIL)
	بازگشت;

    اگر (eap->addr_شمار > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	اگر (eap->addr_شمار == 0)
	{
	    اگر (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		بازگشت;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	وگرنه اگر (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	اگر (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    بازگشت;
	}
	اگر (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    اگر (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    بازگشت;
	}

	برای (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * اندازه(aentry_T)));
	ARGCOUNT -= n;
	اگر (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	وگرنه اگر (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    وگرنه
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    اگر (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    وگرنه اگر (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx >= ARGCOUNT)
	بازگشت پوچ;

    بازگشت alist_name(&ARGLIST[idx]);
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
    اگر (bp == پوچ || bp->b_fname == پوچ)
	بازگشت aep->ae_fname;
    بازگشت bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
تعریفنوع ساختار {
    alist_T	*alist;		// argument list to be used
    صحیح		had_tab;
    صحیح		keep_tabs;
    صحیح		forceit;

    صحیح		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    صحیح		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    ایستا تهی
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    صحیح		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    اگر (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    برای (;;)
    {
	tpnext = curtab->tp_next;
	برای (wp = firstwin; wp != پوچ; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    اگر (buf->b_ffname == پوچ
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    وگرنه
	    {
		// check if the buffer in this window is in the arglist
		برای (i = 0; i < aall->opened_len; ++i)
		{
		    اگر (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			صحیح weight = 1;

			اگر (old_curtab == curtab)
			{
			    ++weight;
			    اگر (old_curwin == wp)
				++weight;
			}

			اگر (weight > (صحیح)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    اگر (i == 0)
			    {
				اگر (aall->new_curwin != پوچ)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			وگرنه اگر (aall->keep_tabs)
			    i = aall->opened_len;

			اگر (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			بشکن;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    اگر (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		اگر (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    اگر (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(تهی)autowrite(buf, FALSE);

			// check if autocommands removed the window
			اگر (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    ادامه;
			}
		    }
		    // don't close last window
		    اگر (ONE_WINDOW
			    && (first_tabpage->tp_next == پوچ
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    وگرنه
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			اگر (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	اگر (aall->had_tab == 0 || tpnext == پوچ)
	    بشکن;

	// check if autocommands removed the next tab page
	اگر (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ایستا تهی
arg_all_open_windows(arg_all_state_T *aall, صحیح شمار)
{
    win_T	*wp;
    صحیح		tab_drop_empty_window = FALSE;
    صحیح		i;
    صحیح		split_ret = OK;
    صحیح		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    اگر (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == پوچ && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    برای (i = 0; i < شمار && !got_int; ++i)
    {
	اگر (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	اگر (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    اگر (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    اگر (wp->w_arg_idx == i)
		    {
			اگر (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			وگرنه اگر (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = شمار;
			    بشکن;
			}
			وگرنه
			    win_move_after(wp, curwin);
			بشکن;
		    }
		}
	    }
	}
	وگرنه اگر (split_ret == OK)
	{
	    // trigger events for tab drop
	    اگر (tab_drop_empty_window && i == شمار - 1)
		--autocmd_no_enter;
	    اگر (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		اگر (split_ret == FAIL)
		    ادامه;
	    }
	    وگرنه    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    اگر (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (تهی)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), پوچ, پوچ,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    اگر (tab_drop_empty_window && i == شمار - 1)
		++autocmd_no_enter;
	    اگر (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	اگر (aall->had_tab > 0 && tabpage_index(پوچ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ایستا تهی
do_arg_all(
    صحیح	شمار,
    صحیح	forceit,		// hide buffers in current windows
    صحیح keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    صحیح			prev_arglist_locked = arglist_locked;

    اگر (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	بازگشت;
    }
    اگر (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	بازگشت;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = پوچ;
    aall.new_curtab = پوچ;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    اگر (aall.opened == پوچ)
	بازگشت;

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
    اگر (شمار > aall.opened_len || شمار <= 0)
	شمار = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, شمار);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    اگر (last_curtab != aall.new_curtab)
    {
	اگر (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	اگر (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    اگر (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    اگر (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    اگر (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    تهی
ex_all(exarg_T *eap)
{
    اگر (eap->addr_شمار == 0)
	eap->line2 = 9999;
    do_arg_all((صحیح)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(تهی)
{
    صحیح		len;
    صحیح		idx;
    char_u	*retval = پوچ;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    برای (;;)
    {
	len = 0;
	برای (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    اگر (p == پوچ)
		ادامه;
	    اگر (len > 0)
	    {
		// insert a space in between names
		اگر (retval != پوچ)
		    retval[len] = ' ';
		++len;
	    }
	    برای ( ; *p != NUL; ++p)
	    {
		اگر (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    اگر (retval != پوچ)
			retval[len] = '\\';
		    ++len;
		}
		اگر (retval != پوچ)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	اگر (retval != پوچ)
	{
	    retval[len] = NUL;
	    بشکن;
	}

	// allocate memory
	retval = alloc(len + 1);
	اگر (retval == پوچ)
	    بشکن;
    }

    بازگشت retval;
}

#اگر defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    تهی
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_عدد = ARGCOUNT;
    وگرنه اگر (argvars[0].v_type == VAR_عدد
					   && tv_get_عدد(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_عدد = GARGCOUNT;
    وگرنه
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	اگر (wp != پوچ)
	    rettv->vval.v_عدد = WARGCOUNT(wp);
	وگرنه
	    rettv->vval.v_عدد = -1;
    }
}

/*
 * "argidx()" function
 */
    تهی
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    تهی
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    rettv->vval.v_عدد = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], پوچ);
    اگر (wp != پوچ)
	rettv->vval.v_عدد = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ایستا تهی
get_arglist_as_rettv(aentry_T *arglist, صحیح argcount, typval_T *rettv)
{
    صحیح		idx;

    اگر (rettv_list_alloc(rettv) == OK && arglist != پوچ)
	برای (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    تهی
f_argv(typval_T *argvars, typval_T *rettv)
{
    صحیح		idx;
    aentry_T	*arglist = پوچ;
    صحیح		argcount = -1;

    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	بازگشت;
    }

    اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    وگرنه اگر (argvars[1].v_type == VAR_عدد
	    && tv_get_عدد(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    وگرنه
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	اگر (wp != پوچ)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    idx = tv_get_عدد_chk(&argvars[0], پوچ);
    اگر (arglist != پوچ && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    وگرنه اگر (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
