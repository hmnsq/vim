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

#تعریف_کرو AL_سیٹ	1
#تعریف_کرو AL_جوڑو	2
#تعریف_کرو AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ساکن صحیح arglist_locked = FALSE;

    ساکن صحیح
check_arglist_locked(خالی)
{
    اگر (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    خالی
alist_clear(alist_T *al)
{
    اگر (check_arglist_locked() == FAIL)
	واپس;
    جب (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    خالی
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, حجم(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    خالی
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
    خالی
alist_new(خالی)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    اگر (curwin->w_alist == تہی)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    ورنہ
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
    خالی
alist_expand(صحیح *fnum_list, صحیح fnum_len)
{
    char_u	**old_arg_files;
    صحیح		old_arg_گنتی;
    char_u	**new_arg_files;
    صحیح		new_arg_file_گنتی;
    char_u	*save_p_su = p_su;
    صحیح		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    اگر (old_arg_files == تہی)
	واپس;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = خالی_خانے_option;
    برائے (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_گنتی = GARGCOUNT;
    اگر (expand_wildcards(old_arg_گنتی, old_arg_files,
		&new_arg_file_گنتی, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_گنتی > 0)
    {
	alist_سیٹ(&global_alist, new_arg_file_گنتی, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_گنتی, old_arg_files);
    }
    p_su = save_p_su;
}
#اگر_ختم

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    خالی
alist_سیٹ(
    alist_T	*al,
    صحیح		گنتی,
    char_u	**files,
    صحیح		use_curbuf,
    صحیح		*fnum_list,
    صحیح		fnum_len)
{
    صحیح		i;

    اگر (check_arglist_locked() == FAIL)
	واپس;

    alist_clear(al);
    اگر (GA_GROW_OK(&al->al_ga, گنتی))
    {
	برائے (i = 0; i < گنتی; ++i)
	{
	    اگر (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		جب (i < گنتی)
		    vim_free(files[i++]);
		توڑو;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    اگر (fnum_list != تہی && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_سیٹ_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_جوڑو(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    ورنہ
	FreeWild(گنتی, files);
    اگر (al == &global_alist)
	arg_had_آخری = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    خالی
alist_جوڑو(
    alist_T	*al,
    char_u	*fname,
    صحیح		سیٹ_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    اگر (fname == تہی)		// don't add NULL file names
	واپس;
    اگر (check_arglist_locked() == FAIL)
	واپس;
    arglist_locked = TRUE;
    ++wp->w_locked;

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    slash_adjust(fname);
#اگر_ختم
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    اگر (سیٹ_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_جوڑو(fname, BLN_LISTED | (سیٹ_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    خالی
alist_slash_adjust(خالی)
{
    صحیح		i;
    win_T	*wp;
    tabpage_T	*tp;

    برائے (i = 0; i < GARGCOUNT; ++i)
	اگر (GARGLIST[i].ae_fname != تہی)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_alist != &global_alist)
	    برائے (i = 0; i < WARGCOUNT(wp); ++i)
		اگر (WARGLIST(wp)[i].ae_fname != تہی)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#اگر_ختم

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ساکن char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    صحیح		inbacktick;

    inbacktick = FALSE;
    برائے (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	اگر (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	ورنہ
	{
	    // An item ends at a space not in backticks
	    اگر (!inbacktick && vim_isspace(*str))
		توڑو;
	    اگر (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    واپس str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ساکن صحیح
get_arglist(garray_T *gap, char_u *str, صحیح escaped)
{
    ga_init2(gap, حجم(char_u *), 20);
    جب (*str != NUL)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    واپس FAIL;
	}
	((char_u **)gap->ga_ڈیٹا)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	اگر (!escaped)
	    واپس OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    واپس OK;
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
	واپس FAIL;
    اگر (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_ڈیٹا,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    ورنہ
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_ڈیٹا,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    واپس i;
}
#اگر_ختم

/*
 * Check the validity of the arg_idx for each other window.
 */
    ساکن خالی
alist_check_arg_idx(خالی)
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
    ساکن خالی
alist_جوڑو_list(
    صحیح		گنتی,
    char_u	**files,
    صحیح		after,	    // where to add: 0 = before first one
    صحیح		will_edit)  // will edit adding argument
{
    صحیح		i;
    صحیح		old_argcount = ARGCOUNT;

    اگر (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, گنتی))
    {
	win_T	*wp = curwin;

	اگر (after < 0)
	    after = 0;
	اگر (after > ARGCOUNT)
	    after = ARGCOUNT;
	اگر (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + گنتی]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * حجم(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	برائے (i = 0; i < گنتی; ++i)
	{
	    صحیح flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_جوڑو(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += گنتی;
	اگر (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += گنتی;
	واپس;
    }

    برائے (i = 0; i < گنتی; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ساکن خالی
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    صحیح		didone;
    صحیح		i;
    char_u	*p;
    صحیح		مطابقت;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    برائے (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_ڈیٹا)[i];
	p = file_pat_مقصد_reg_pat(p, تہی, تہی, FALSE);
	اگر (p == تہی)
	    توڑو;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog == تہی)
	{
	    vim_free(p);
	    توڑو;
	}

	didone = FALSE;
	برائے (مطابقت = 0; مطابقت < ARGCOUNT; ++مطابقت)
	    اگر (vim_regexec(&regmatch, alist_name(&ARGLIST[مطابقت]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[مطابقت].ae_fname);
		mch_memmove(ARGLIST + مطابقت, ARGLIST + مطابقت + 1,
			(ARGCOUNT - مطابقت - 1) * حجم(aentry_T));
		--ARGCOUNT;
		اگر (curwin->w_arg_idx > مطابقت)
		    --curwin->w_arg_idx;
		--مطابقت;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	اگر (!didone)
	    semsg(_(e_no_مطابقت_str_2), ((char_u **)alist_ga->ga_ڈیٹا)[i]);
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
    ساکن صحیح
do_arglist(
    char_u	*str,
    صحیح		what,
    صحیح		after غیر_مستعمل,	// 0 means before first one
    صحیح		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    صحیح		exp_گنتی;
    char_u	**exp_files;
    صحیح		i;
    صحیح		arg_escaped = TRUE;

    اگر (check_arglist_locked() == FAIL)
	واپس FAIL;

    // Set default argument for ":argadd" command.
    اگر (what == AL_جوڑو && *str == NUL)
    {
	اگر (curbuf->b_ffname == تہی)
	    واپس FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    اگر (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	واپس FAIL;

    اگر (what == AL_DEL)
	arglist_del_files(&new_ga);
    ورنہ
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_ڈیٹا,
		&exp_گنتی, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	اگر (i == FAIL || exp_گنتی == 0)
	{
	    emsg(_(e_no_مطابقت));
	    واپس FAIL;
	}

	اگر (what == AL_جوڑو)
	{
	    alist_جوڑو_list(exp_گنتی, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	ورنہ // what == AL_SET
	    alist_سیٹ(ALIST(curwin), exp_گنتی, exp_files, will_edit, تہی, 0);
    }

    alist_check_arg_idx();

    واپس OK;
}

/*
 * Redefine the argument list.
 */
    خالی
سیٹ_arglist(char_u *str)
{
    do_arglist(str, AL_سیٹ, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    صحیح
editing_arg_idx(win_T *win)
{
    واپس !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_بفر->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_بفر->b_ffname == تہی
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_بفر->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    خالی
check_arg_idx(win_T *win)
{
    اگر (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	اگر (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_آخری == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_بفر->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_بفر->b_ffname != تہی
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_بفر->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_آخری = TRUE;
    }
    ورنہ
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	اگر (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_آخری = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    خالی
ex_دلائل(exarg_T *eap)
{
    صحیح		i;

    اگر (eap->cmdidx != CMD_دلائل)
    {
	اگر (check_arglist_locked() == FAIL)
	    واپس;
	alist_unlink(ALIST(curwin));
	اگر (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	ورنہ // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    اگر (*eap->arg != NUL)
    {
	اگر (check_arglist_locked() == FAIL)
	    واپس;
	ex_اگلا(eap);
	واپس;
    }

    // ":args": list arguments.
    اگر (eap->cmdidx == CMD_دلائل)
    {
	char_u **اشیاء;

	اگر (ARGCOUNT <= 0)
	    واپس;		// empty argument list

	اشیاء = ALLOC_MULT(char_u *, ARGCOUNT);
	اگر (اشیاء == تہی)
	    واپس;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	برائے (i = 0; i < ARGCOUNT; ++i)
	    اشیاء[i] = alist_name(&ARGLIST[i]);
	list_اندر_columns(اشیاء, ARGCOUNT, curwin->w_arg_idx);
	vim_free(اشیاء);

	واپس;
    }

    // ":argslocal": make a local copy of the global argument list.
    اگر (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	اگر (GA_GROW_FAILS(gap, GARGCOUNT))
	    واپس;

	برائے (i = 0; i < GARGCOUNT; ++i)
	    اگر (GARGLIST[i].ae_fname != تہی)
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
    خالی
ex_پچھلا(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    اگر (curwin->w_arg_idx - (صحیح)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    ورنہ
	do_argfile(eap, curwin->w_arg_idx - (صحیح)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    خالی
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    خالی
ex_آخری(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    خالی
ex_دلیل(exarg_T *eap)
{
    صحیح		i;

    اگر (eap->addr_گنتی > 0)
	i = eap->line2 - 1;
    ورنہ
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    خالی
do_argfile(exarg_T *eap, صحیح argn)
{
    صحیح		other;
    char_u	*p;
    صحیح		old_arg_idx = curwin->w_arg_idx;
    صحیح is_بانٹو_cmd = *eap->cmd == 's';

    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    اگر (argn < 0 || argn >= ARGCOUNT)
    {
	اگر (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_مقصد_edit));
	ورنہ اگر (argn < 0)
	    emsg(_(e_cannot_go_before_پہلا_file));
	ورنہ
	    emsg(_(e_cannot_go_beyond_آخری_file));

	واپس;
    }

    اگر (!is_بانٹو_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_سیٹ_curbuf_forceit(eap->forceit))
	واپس;

    setpcmark();
#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    // split window or create new tab page first
    اگر (is_بانٹو_cmd || cmdmod.cmod_tab != 0)
    {
	اگر (win_بانٹو(0, 0) == FAIL)
	    واپس;
	RESET_BINDING(curwin);
    }
    ورنہ
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
		&& check_بدلا(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    واپس;
    }

    curwin->w_arg_idx = argn;
    اگر (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_آخری = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    اگر (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), تہی,
		eap, ECMD_آخری,
		(buf_hide(curwin->w_بفر) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    ورنہ اگر (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    خالی
ex_اگلا(exarg_T *eap)
{
    صحیح		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    اگر (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_بدلا(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	اگر (*eap->arg != NUL)		    // redefine file list
	{
	    اگر (do_arglist(eap->arg, AL_سیٹ, 0, TRUE) == FAIL)
		واپس;
	    i = 0;
	}
	ورنہ
	    i = curwin->w_arg_idx + (صحیح)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    خالی
ex_argdedupe(exarg_T *eap غیر_مستعمل)
{
    صحیح i;
    صحیح j;

    برائے (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *پہلا_Fullname = بھرا_Name_save(ARGLIST[i].ae_fname, FALSE);
	اگر (پہلا_Fullname == تہی)
	    واپس;  // out of memory

	برائے (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *دوسرا_Fullname = بھرا_Name_save(ARGLIST[j].ae_fname, FALSE);
	    اگر (دوسرا_Fullname == تہی)
		توڑو;  // out of memory
	    صحیح are_نام_Duplicate =
				  fnamecmp(پہلا_Fullname, دوسرا_Fullname) == 0;
	    vim_free(دوسرا_Fullname);

	    اگر (are_نام_Duplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * حجم(aentry_T));
		--ARGCOUNT;

		اگر (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		ورنہ اگر (curwin->w_arg_idx > j)
		    --curwin->w_arg_idx;

		--j;
	    }
	}

	vim_free(پہلا_Fullname);
    }
}

/*
 * ":argedit"
 */
    خالی
ex_argedit(exarg_T *eap)
{
    صحیح i = eap->addr_گنتی ? (صحیح)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    صحیح curbuf_is_reusable = curbuf_reusable();

    اگر (do_arglist(eap->arg, AL_جوڑو, i, TRUE) == FAIL)
	واپس;
    maketitle();

    اگر (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	    && (curbuf->b_ffname == تہی || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    اگر (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    خالی
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_جوڑو,
	       eap->addr_گنتی > 0 ? (صحیح)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    خالی
ex_argdelete(exarg_T *eap)
{
    صحیح		i;
    صحیح		n;

    اگر (check_arglist_locked() == FAIL)
	واپس;

    اگر (eap->addr_گنتی > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	اگر (eap->addr_گنتی == 0)
	{
	    اگر (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_دلیل_مقصد_delete));
		واپس;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	ورنہ اگر (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	اگر (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_دلیل));
	    واپس;
	}
	اگر (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    اگر (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    واپس;
	}

	برائے (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * حجم(aentry_T)));
	ARGCOUNT -= n;
	اگر (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	ورنہ اگر (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    ورنہ
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    اگر (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    ورنہ اگر (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx >= ARGCOUNT)
	واپس تہی;

    واپس alist_name(&ARGLIST[idx]);
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
    اگر (bp == تہی || bp->b_fname == تہی)
	واپس aep->ae_fname;
    واپس bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
قسمتعریف ساخت {
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
    ساکن خالی
arg_all_close_غیر_مستعمل_windows(arg_all_state_T *aall)
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
	goto_tabpage_tp(پہلا_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    برائے (;;)
    {
	tpnext = curtab->tp_اگلا;
	برائے (wp = firstwin; wp != تہی; wp = wpnext)
	{
	    wpnext = wp->w_اگلا;
	    buf = wp->w_بفر;
	    اگر (buf->b_ffname == تہی
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_چوڑائی != cmdline_چوڑائی)))
		i = aall->opened_len;
	    ورنہ
	    {
		// check if the buffer in this window is in the arglist
		برائے (i = 0; i < aall->opened_len; ++i)
		{
		    اگر (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			صحیح وزن = 1;

			اگر (old_curtab == curtab)
			{
			    ++وزن;
			    اگر (old_curwin == wp)
				++وزن;
			}

			اگر (وزن > (صحیح)aall->opened[i])
			{
			    aall->opened[i] = (char_u)وزن;
			    اگر (i == 0)
			    {
				اگر (aall->new_curwin != تہی)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			ورنہ اگر (aall->keep_tabs)
			    i = aall->opened_len;

			اگر (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			توڑو;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    اگر (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		اگر (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !buf_Is_بدلا(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    اگر (!buf_hide(buf) && buf->b_nwindows <= 1
							 && buf_Is_بدلا(buf))
		    {
			bufref_T    bufref;

			سیٹ_bufref(&bufref, buf);

			(خالی)autowrite(buf, FALSE);

			// check if autocommands removed the window
			اگر (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    جاری;
			}
		    }
		    // don't close last window
		    اگر (ONE_کھڑکی
			    && (پہلا_tabpage->tp_اگلا == تہی
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    ورنہ
		    {
			win_close(wp, !buf_hide(buf) && !buf_Is_بدلا(buf));

			// check if autocommands removed the next window
			اگر (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	اگر (aall->had_tab == 0 || tpnext == تہی)
	    توڑو;

	// check if autocommands removed the next tab page
	اگر (!valid_tabpage(tpnext))
	    tpnext = پہلا_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ساکن خالی
arg_all_open_windows(arg_all_state_T *aall, صحیح گنتی)
{
    win_T	*wp;
    صحیح		tab_drop_خالی_خانے_کھڑکی = FALSE;
    صحیح		i;
    صحیح		بانٹو_ret = OK;
    صحیح		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    اگر (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == تہی && !curbuf->b_بدلا)
    {
	aall->use_firstwin = TRUE;
	tab_drop_خالی_خانے_کھڑکی = TRUE;
    }

    برائے (i = 0; i < گنتی && !got_int; ++i)
    {
	اگر (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_آخری = TRUE;
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
			ورنہ اگر (wp->w_چوکھٹا->fr_والد
				!= curwin->w_چوکھٹا->fr_والد)
			{
			    emsg(_(e_کھڑکی_layout_بدلا_unexpectedly));
			    i = گنتی;
			    توڑو;
			}
			ورنہ
			    win_move_after(wp, curwin);
			توڑو;
		    }
		}
	    }
	}
	ورنہ اگر (بانٹو_ret == OK)
	{
	    // trigger events for tab drop
	    اگر (tab_drop_خالی_خانے_کھڑکی && i == گنتی - 1)
		--autocmd_no_enter;
	    اگر (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		بانٹو_ret = win_بانٹو(0, WSP_گنجائش | WSP_نیچے);
		p_ea = p_ea_save;
		اگر (بانٹو_ret == FAIL)
		    جاری;
	    }
	    ورنہ    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    اگر (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (خالی)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), تہی, تہی,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_بفر)
		      || buf_Is_بدلا(curwin->w_بفر)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    اگر (tab_drop_خالی_خانے_کھڑکی && i == گنتی - 1)
		++autocmd_no_enter;
	    اگر (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	اگر (aall->had_tab > 0 && tabpage_اشاریہ(تہی) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ساکن خالی
do_arg_all(
    صحیح	گنتی,
    صحیح	forceit,		// hide buffers in current windows
    صحیح keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*آخری_curwin;
    tabpage_T		*آخری_curtab;
    صحیح			پچھلا_arglist_locked = arglist_locked;

    اگر (cmdwin_type != 0)
    {
	emsg(_(e_invalid_اندر_cmdline_کھڑکی));
	واپس;
    }
    اگر (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	واپس;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = تہی;
    aall.new_curtab = تہی;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    اگر (aall.opened == تہی)
	واپس;

    // Autocommands may do anything to the argument list.  Make sure it's not
    // freed while we are working here by "locking" it.  We still have to
    // watch out for its size being changed.
    aall.alist = curwin->w_alist;
    ++aall.alist->al_refcount;
    arglist_locked = TRUE;

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    tabpage_T *new_lu_tp = curtab;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    // Try closing all windows that are not in the argument list.
    // Also close windows that are not full width;
    // When 'hidden' or "forceit" set the buffer becomes hidden.
    // Windows that have a changed buffer and can't be hidden won't be closed.
    // When the ":tab" modifier was used do this for all tab pages.
    arg_all_close_غیر_مستعمل_windows(&aall);

    // Open a window for files in the argument list that don't have one.
    // ARGCOUNT may change while doing this, because of autocommands.
    اگر (گنتی > aall.opened_len || گنتی <= 0)
	گنتی = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    آخری_curwin = curwin;
    آخری_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, گنتی);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = پچھلا_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    اگر (آخری_curtab != aall.new_curtab)
    {
	اگر (valid_tabpage(آخری_curtab))
	    goto_tabpage_tp(آخری_curtab, TRUE, TRUE);
	اگر (win_valid(آخری_curwin))
	    win_enter(آخری_curwin, FALSE);
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
    خالی
ex_all(exarg_T *eap)
{
    اگر (eap->addr_گنتی == 0)
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
arg_all(خالی)
{
    صحیح		len;
    صحیح		idx;
    char_u	*retval = تہی;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    برائے (;;)
    {
	len = 0;
	برائے (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    اگر (p == تہی)
		جاری;
	    اگر (len > 0)
	    {
		// insert a space in between names
		اگر (retval != تہی)
		    retval[len] = ' ';
		++len;
	    }
	    برائے ( ; *p != NUL; ++p)
	    {
		اگر (*p == ' '
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
			|| *p == '\\'
#اگر_ختم
			|| *p == '`')
		{
		    // insert a backslash
		    اگر (retval != تہی)
			retval[len] = '\\';
		    ++len;
		}
		اگر (retval != تہی)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	اگر (retval != تہی)
	{
	    retval[len] = NUL;
	    توڑو;
	}

	// allocate memory
	retval = alloc(len + 1);
	اگر (retval == تہی)
	    توڑو;
    }

    واپس retval;
}

#اگر defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    خالی
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_نمبر = ARGCOUNT;
    ورنہ اگر (argvars[0].v_type == VAR_نمبر
					   && tv_get_نمبر(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_نمبر = GARGCOUNT;
    ورنہ
    {
	// use the argument list of the specified window
	wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
	اگر (wp != تہی)
	    rettv->vval.v_نمبر = WARGCOUNT(wp);
	ورنہ
	    rettv->vval.v_نمبر = -1;
    }
}

/*
 * "argidx()" function
 */
    خالی
f_argidx(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    خالی
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    rettv->vval.v_نمبر = -1;
    wp = ڈھونڈو_tabwin(&argvars[0], &argvars[1], تہی);
    اگر (wp != تہی)
	rettv->vval.v_نمبر = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ساکن خالی
get_arglist_as_rettv(aentry_T *arglist, صحیح argcount, typval_T *rettv)
{
    صحیح		idx;

    اگر (rettv_list_alloc(rettv) == OK && arglist != تہی)
	برائے (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    خالی
f_argv(typval_T *argvars, typval_T *rettv)
{
    صحیح		idx;
    aentry_T	*arglist = تہی;
    صحیح		argcount = -1;

    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	واپس;
    }

    اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    ورنہ اگر (argvars[1].v_type == VAR_نمبر
	    && tv_get_نمبر(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    ورنہ
    {
	win_T	*wp = ڈھونڈو_win_by_nr_or_id(&argvars[1]);

	اگر (wp != تہی)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    idx = tv_get_نمبر_chk(&argvars[0], تہی);
    اگر (arglist != تہی && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    ورنہ اگر (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#اگر_ختم
