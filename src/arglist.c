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

#ضمن "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ساكن صحيح arglist_locked = FALSE;

    ساكن صحيح
check_arglist_locked(فراغ)
{
    إذا (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    فراغ
alist_clear(alist_T *al)
{
    إذا (check_arglist_locked() == FAIL)
	أرجع;
    طالما (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    فراغ
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, حجم(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    فراغ
alist_unlink(alist_T *al)
{
    إذا (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    فراغ
alist_new(فراغ)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    إذا (curwin->w_alist == عدم)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    وإلا
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#إذا !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    فراغ
alist_expand(صحيح *fnum_list, صحيح fnum_len)
{
    char_u	**old_arg_files;
    صحيح		old_arg_گنتی;
    char_u	**new_arg_files;
    صحيح		new_arg_file_گنتی;
    char_u	*save_p_su = p_su;
    صحيح		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    إذا (old_arg_files == عدم)
	أرجع;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    لكل (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_گنتی = GARGCOUNT;
    إذا (expand_wildcards(old_arg_گنتی, old_arg_files,
		&new_arg_file_گنتی, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_گنتی > 0)
    {
	alist_set(&global_alist, new_arg_file_گنتی, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_گنتی, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    فراغ
alist_set(
    alist_T	*al,
    صحيح		گنتی,
    char_u	**files,
    صحيح		use_curbuf,
    صحيح		*fnum_list,
    صحيح		fnum_len)
{
    صحيح		i;

    إذا (check_arglist_locked() == FAIL)
	أرجع;

    alist_clear(al);
    إذا (GA_GROW_OK(&al->al_ga, گنتی))
    {
	لكل (i = 0; i < گنتی; ++i)
	{
	    إذا (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		طالما (i < گنتی)
		    vim_free(files[i++]);
		اكسر;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    إذا (fnum_list != عدم && i < fnum_len)
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
    وإلا
	FreeWild(گنتی, files);
    إذا (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    فراغ
alist_add(
    alist_T	*al,
    char_u	*fname,
    صحيح		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    إذا (fname == عدم)		// don't add NULL file names
	أرجع;
    إذا (check_arglist_locked() == FAIL)
	أرجع;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    إذا (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#إذا defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    فراغ
alist_slash_adjust(فراغ)
{
    صحيح		i;
    win_T	*wp;
    tabpage_T	*tp;

    لكل (i = 0; i < GARGCOUNT; ++i)
	إذا (GARGLIST[i].ae_fname != عدم)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_alist != &global_alist)
	    لكل (i = 0; i < WARGCOUNT(wp); ++i)
		إذا (WARGLIST(wp)[i].ae_fname != عدم)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ساكن char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    صحيح		inbacktick;

    inbacktick = FALSE;
    لكل (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	إذا (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	وإلا
	{
	    // An item ends at a space not in backticks
	    إذا (!inbacktick && vim_isspace(*str))
		اكسر;
	    إذا (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    أرجع str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ساكن صحيح
get_arglist(garray_T *gap, char_u *str, صحيح escaped)
{
    ga_init2(gap, حجم(char_u *), 20);
    طالما (*str != NUL)
    {
	إذا (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    أرجع FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	إذا (!escaped)
	    أرجع OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    أرجع OK;
}

#إذا defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    صحيح
get_arglist_exp(
    char_u	*str,
    صحيح		*fcountp,
    char_u	***fnamesp,
    صحيح		wig)
{
    garray_T	ga;
    صحيح		i;

    إذا (get_arglist(&ga, str, TRUE) == FAIL)
	أرجع FAIL;
    إذا (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    وإلا
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    أرجع i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    ساكن فراغ
alist_check_arg_idx(فراغ)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	إذا (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    ساكن فراغ
alist_add_list(
    صحيح		گنتی,
    char_u	**files,
    صحيح		after,	    // where to add: 0 = before first one
    صحيح		will_edit)  // will edit adding argument
{
    صحيح		i;
    صحيح		old_argcount = ARGCOUNT;

    إذا (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, گنتی))
    {
	win_T	*wp = curwin;

	إذا (after < 0)
	    after = 0;
	إذا (after > ARGCOUNT)
	    after = ARGCOUNT;
	إذا (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + گنتی]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * حجم(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	لكل (i = 0; i < گنتی; ++i)
	{
	    صحيح flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += گنتی;
	إذا (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += گنتی;
	أرجع;
    }

    لكل (i = 0; i < گنتی; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ساكن فراغ
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    صحيح		didone;
    صحيح		i;
    char_u	*p;
    صحيح		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    لكل (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, عدم, عدم, FALSE);
	إذا (p == عدم)
	    اكسر;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	إذا (regmatch.regprog == عدم)
	{
	    vim_free(p);
	    اكسر;
	}

	didone = FALSE;
	لكل (match = 0; match < ARGCOUNT; ++match)
	    إذا (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * حجم(aentry_T));
		--ARGCOUNT;
		إذا (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	إذا (!didone)
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
    ساكن صحيح
do_arglist(
    char_u	*str,
    صحيح		what,
    صحيح		after UNUSED,	// 0 means before first one
    صحيح		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    صحيح		exp_گنتی;
    char_u	**exp_files;
    صحيح		i;
    صحيح		arg_escaped = TRUE;

    إذا (check_arglist_locked() == FAIL)
	أرجع FAIL;

    // Set default argument for ":argadd" command.
    إذا (what == AL_ADD && *str == NUL)
    {
	إذا (curbuf->b_ffname == عدم)
	    أرجع FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    إذا (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	أرجع FAIL;

    إذا (what == AL_DEL)
	arglist_del_files(&new_ga);
    وإلا
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_گنتی, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	إذا (i == FAIL || exp_گنتی == 0)
	{
	    emsg(_(e_no_match));
	    أرجع FAIL;
	}

	إذا (what == AL_ADD)
	{
	    alist_add_list(exp_گنتی, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	وإلا // what == AL_SET
	    alist_set(ALIST(curwin), exp_گنتی, exp_files, will_edit, عدم, 0);
    }

    alist_check_arg_idx();

    أرجع OK;
}

/*
 * Redefine the argument list.
 */
    فراغ
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    صحيح
editing_arg_idx(win_T *win)
{
    أرجع !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == عدم
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    فراغ
check_arg_idx(win_T *win)
{
    إذا (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	إذا (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != عدم
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    وإلا
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	إذا (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    فراغ
ex_args(exarg_T *eap)
{
    صحيح		i;

    إذا (eap->cmdidx != CMD_args)
    {
	إذا (check_arglist_locked() == FAIL)
	    أرجع;
	alist_unlink(ALIST(curwin));
	إذا (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	وإلا // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    إذا (*eap->arg != NUL)
    {
	إذا (check_arglist_locked() == FAIL)
	    أرجع;
	ex_next(eap);
	أرجع;
    }

    // ":args": list arguments.
    إذا (eap->cmdidx == CMD_args)
    {
	char_u **اشیاء;

	إذا (ARGCOUNT <= 0)
	    أرجع;		// empty argument list

	اشیاء = ALLOC_MULT(char_u *, ARGCOUNT);
	إذا (اشیاء == عدم)
	    أرجع;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	لكل (i = 0; i < ARGCOUNT; ++i)
	    اشیاء[i] = alist_name(&ARGLIST[i]);
	list_in_columns(اشیاء, ARGCOUNT, curwin->w_arg_idx);
	vim_free(اشیاء);

	أرجع;
    }

    // ":argslocal": make a local copy of the global argument list.
    إذا (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	إذا (GA_GROW_FAILS(gap, GARGCOUNT))
	    أرجع;

	لكل (i = 0; i < GARGCOUNT; ++i)
	    إذا (GARGLIST[i].ae_fname != عدم)
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
    فراغ
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    إذا (curwin->w_arg_idx - (صحيح)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    وإلا
	do_argfile(eap, curwin->w_arg_idx - (صحيح)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    فراغ
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    فراغ
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    فراغ
ex_argument(exarg_T *eap)
{
    صحيح		i;

    إذا (eap->addr_گنتی > 0)
	i = eap->line2 - 1;
    وإلا
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    فراغ
do_argfile(exarg_T *eap, صحيح argn)
{
    صحيح		other;
    char_u	*p;
    صحيح		old_arg_idx = curwin->w_arg_idx;
    صحيح is_split_cmd = *eap->cmd == 's';

    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;
    إذا (argn < 0 || argn >= ARGCOUNT)
    {
	إذا (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	وإلا إذا (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	وإلا
	    emsg(_(e_cannot_go_beyond_last_file));

	أرجع;
    }

    إذا (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	أرجع;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    إذا (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	إذا (win_split(0, 0) == FAIL)
	    أرجع;
	RESET_BINDING(curwin);
    }
    وإلا
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	إذا (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	إذا ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    أرجع;
    }

    curwin->w_arg_idx = argn;
    إذا (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    إذا (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), عدم,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    وإلا إذا (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    فراغ
ex_next(exarg_T *eap)
{
    صحيح		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    إذا (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	إذا (*eap->arg != NUL)		    // redefine file list
	{
	    إذا (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		أرجع;
	    i = 0;
	}
	وإلا
	    i = curwin->w_arg_idx + (صحيح)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    فراغ
ex_argdedupe(exarg_T *eap UNUSED)
{
    صحيح i;
    صحيح j;

    لكل (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	إذا (firstFullname == عدم)
	    أرجع;  // out of memory

	لكل (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    إذا (secondFullname == عدم)
		اكسر;  // out of memory
	    صحيح areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    إذا (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * حجم(aentry_T));
		--ARGCOUNT;

		إذا (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		وإلا إذا (curwin->w_arg_idx > j)
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
    فراغ
ex_argedit(exarg_T *eap)
{
    صحيح i = eap->addr_گنتی ? (صحيح)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    صحيح curbuf_is_reusable = curbuf_reusable();

    إذا (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	أرجع;
    maketitle();

    إذا (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == عدم || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    إذا (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    فراغ
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_گنتی > 0 ? (صحيح)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    فراغ
ex_argdelete(exarg_T *eap)
{
    صحيح		i;
    صحيح		n;

    إذا (check_arglist_locked() == FAIL)
	أرجع;

    إذا (eap->addr_گنتی > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	إذا (eap->addr_گنتی == 0)
	{
	    إذا (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		أرجع;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	وإلا إذا (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	إذا (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    أرجع;
	}
	إذا (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    إذا (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    أرجع;
	}

	لكل (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * حجم(aentry_T)));
	ARGCOUNT -= n;
	إذا (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	وإلا إذا (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    وإلا
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    إذا (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    وإلا إذا (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx >= ARGCOUNT)
	أرجع عدم;

    أرجع alist_name(&ARGLIST[idx]);
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
    إذا (bp == عدم || bp->b_fname == عدم)
	أرجع aep->ae_fname;
    أرجع bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
تعريفنوع بنية {
    alist_T	*alist;		// argument list to be used
    صحيح		had_tab;
    صحيح		keep_tabs;
    صحيح		forceit;

    صحيح		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    صحيح		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    ساكن فراغ
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    صحيح		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    إذا (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    لكل (;;)
    {
	tpnext = curtab->tp_next;
	لكل (wp = firstwin; wp != عدم; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    إذا (buf->b_ffname == عدم
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    وإلا
	    {
		// check if the buffer in this window is in the arglist
		لكل (i = 0; i < aall->opened_len; ++i)
		{
		    إذا (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			صحيح weight = 1;

			إذا (old_curtab == curtab)
			{
			    ++weight;
			    إذا (old_curwin == wp)
				++weight;
			}

			إذا (weight > (صحيح)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    إذا (i == 0)
			    {
				إذا (aall->new_curwin != عدم)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			وإلا إذا (aall->keep_tabs)
			    i = aall->opened_len;

			إذا (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			اكسر;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    إذا (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		إذا (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    إذا (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(فراغ)autowrite(buf, FALSE);

			// check if autocommands removed the window
			إذا (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    استمر;
			}
		    }
		    // don't close last window
		    إذا (ONE_WINDOW
			    && (first_tabpage->tp_next == عدم
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    وإلا
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			إذا (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	إذا (aall->had_tab == 0 || tpnext == عدم)
	    اكسر;

	// check if autocommands removed the next tab page
	إذا (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ساكن فراغ
arg_all_open_windows(arg_all_state_T *aall, صحيح گنتی)
{
    win_T	*wp;
    صحيح		tab_drop_empty_window = FALSE;
    صحيح		i;
    صحيح		split_ret = OK;
    صحيح		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    إذا (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == عدم && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    لكل (i = 0; i < گنتی && !got_int; ++i)
    {
	إذا (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	إذا (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    إذا (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    إذا (wp->w_arg_idx == i)
		    {
			إذا (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			وإلا إذا (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = گنتی;
			    اكسر;
			}
			وإلا
			    win_move_after(wp, curwin);
			اكسر;
		    }
		}
	    }
	}
	وإلا إذا (split_ret == OK)
	{
	    // trigger events for tab drop
	    إذا (tab_drop_empty_window && i == گنتی - 1)
		--autocmd_no_enter;
	    إذا (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		إذا (split_ret == FAIL)
		    استمر;
	    }
	    وإلا    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    إذا (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (فراغ)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), عدم, عدم,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    إذا (tab_drop_empty_window && i == گنتی - 1)
		++autocmd_no_enter;
	    إذا (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	إذا (aall->had_tab > 0 && tabpage_index(عدم) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ساكن فراغ
do_arg_all(
    صحيح	گنتی,
    صحيح	forceit,		// hide buffers in current windows
    صحيح keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    صحيح			prev_arglist_locked = arglist_locked;

    إذا (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	أرجع;
    }
    إذا (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	أرجع;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = عدم;
    aall.new_curtab = عدم;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    إذا (aall.opened == عدم)
	أرجع;

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
    إذا (گنتی > aall.opened_len || گنتی <= 0)
	گنتی = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, گنتی);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    إذا (last_curtab != aall.new_curtab)
    {
	إذا (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	إذا (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    إذا (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    إذا (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    إذا (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    فراغ
ex_all(exarg_T *eap)
{
    إذا (eap->addr_گنتی == 0)
	eap->line2 = 9999;
    do_arg_all((صحيح)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(فراغ)
{
    صحيح		len;
    صحيح		idx;
    char_u	*retval = عدم;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    لكل (;;)
    {
	len = 0;
	لكل (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    إذا (p == عدم)
		استمر;
	    إذا (len > 0)
	    {
		// insert a space in between names
		إذا (retval != عدم)
		    retval[len] = ' ';
		++len;
	    }
	    لكل ( ; *p != NUL; ++p)
	    {
		إذا (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    إذا (retval != عدم)
			retval[len] = '\\';
		    ++len;
		}
		إذا (retval != عدم)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	إذا (retval != عدم)
	{
	    retval[len] = NUL;
	    اكسر;
	}

	// allocate memory
	retval = alloc(len + 1);
	إذا (retval == عدم)
	    اكسر;
    }

    أرجع retval;
}

#إذا defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    فراغ
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    إذا (in_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_نمبر = ARGCOUNT;
    وإلا إذا (argvars[0].v_type == VAR_نمبر
					   && tv_get_نمبر(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_نمبر = GARGCOUNT;
    وإلا
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	إذا (wp != عدم)
	    rettv->vval.v_نمبر = WARGCOUNT(wp);
	وإلا
	    rettv->vval.v_نمبر = -1;
    }
}

/*
 * "argidx()" function
 */
    فراغ
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_نمبر = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    فراغ
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    إذا (in_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	أرجع;

    rettv->vval.v_نمبر = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], عدم);
    إذا (wp != عدم)
	rettv->vval.v_نمبر = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ساكن فراغ
get_arglist_as_rettv(aentry_T *arglist, صحيح argcount, typval_T *rettv)
{
    صحيح		idx;

    إذا (rettv_list_alloc(rettv) == OK && arglist != عدم)
	لكل (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    فراغ
f_argv(typval_T *argvars, typval_T *rettv)
{
    صحيح		idx;
    aentry_T	*arglist = عدم;
    صحيح		argcount = -1;

    إذا (in_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	أرجع;
    }

    إذا (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    وإلا إذا (argvars[1].v_type == VAR_نمبر
	    && tv_get_نمبر(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    وإلا
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	إذا (wp != عدم)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    idx = tv_get_نمبر_chk(&argvars[0], عدم);
    إذا (arglist != عدم && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    وإلا إذا (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
