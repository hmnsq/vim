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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AL_ਸੈੱਟ	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AL_ਜੋੜੋ	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ਸਥਿਰ ਪੂਰਨ arglist_locked = FALSE;

    ਸਥਿਰ ਪੂਰਨ
check_arglist_locked(ਖਾਲੀ)
{
    ਜੇ (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    ਖਾਲੀ
alist_clear(alist_T *al)
{
    ਜੇ (check_arglist_locked() == FAIL)
	ਵਾਪਸ;
    ਜਦੋਂ (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    ਖਾਲੀ
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, ਆਕਾਰ(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    ਖਾਲੀ
alist_unlink(alist_T *al)
{
    ਜੇ (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    ਖਾਲੀ
alist_new(ਖਾਲੀ)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    ਜੇ (curwin->w_alist == ਨਿਰਾ)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    ਨਹੀਂਤਾਂ
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#ਜੇ !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    ਖਾਲੀ
alist_expand(ਪੂਰਨ *fnum_list, ਪੂਰਨ fnum_len)
{
    char_u	**old_arg_files;
    ਪੂਰਨ		old_arg_ਗਿਣਤੀ;
    char_u	**new_arg_files;
    ਪੂਰਨ		new_arg_file_ਗਿਣਤੀ;
    char_u	*save_p_su = p_su;
    ਪੂਰਨ		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    ਜੇ (old_arg_files == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਲਈ (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_ਗਿਣਤੀ = GARGCOUNT;
    ਜੇ (expand_wildcards(old_arg_ਗਿਣਤੀ, old_arg_files,
		&new_arg_file_ਗਿਣਤੀ, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_ਗਿਣਤੀ > 0)
    {
	alist_ਸੈੱਟ(&global_alist, new_arg_file_ਗਿਣਤੀ, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_ਗਿਣਤੀ, old_arg_files);
    }
    p_su = save_p_su;
}
#ਜੇ_ਅੰਤ

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    ਖਾਲੀ
alist_ਸੈੱਟ(
    alist_T	*al,
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    char_u	**files,
    ਪੂਰਨ		use_curbuf,
    ਪੂਰਨ		*fnum_list,
    ਪੂਰਨ		fnum_len)
{
    ਪੂਰਨ		i;

    ਜੇ (check_arglist_locked() == FAIL)
	ਵਾਪਸ;

    alist_clear(al);
    ਜੇ (GA_GROW_OK(&al->al_ga, ਗਿਣਤੀ_2))
    {
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	{
	    ਜੇ (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		ਜਦੋਂ (i < ਗਿਣਤੀ_2)
		    vim_free(files[i++]);
		ਰੋਕੋ;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    ਜੇ (fnum_list != ਨਿਰਾ && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_ਸੈੱਟ_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_ਜੋੜੋ(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    ਨਹੀਂਤਾਂ
	FreeWild(ਗਿਣਤੀ_2, files);
    ਜੇ (al == &global_alist)
	arg_had_ਆਖਰੀ = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    ਖਾਲੀ
alist_ਜੋੜੋ(
    alist_T	*al,
    char_u	*fname,
    ਪੂਰਨ		ਸੈੱਟ_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    ਜੇ (fname == ਨਿਰਾ)		// don't add NULL file names
	ਵਾਪਸ;
    ਜੇ (check_arglist_locked() == FAIL)
	ਵਾਪਸ;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    slash_adjust(fname);
#ਜੇ_ਅੰਤ
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    ਜੇ (ਸੈੱਟ_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_ਜੋੜੋ(fname, BLN_LISTED | (ਸੈੱਟ_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ਖਾਲੀ
alist_slash_adjust(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ਲਈ (i = 0; i < GARGCOUNT; ++i)
	ਜੇ (GARGLIST[i].ae_fname != ਨਿਰਾ)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_alist != &global_alist)
	    ਲਈ (i = 0; i < WARGCOUNT(wp); ++i)
		ਜੇ (WARGLIST(wp)[i].ae_fname != ਨਿਰਾ)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#ਜੇ_ਅੰਤ

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ਸਥਿਰ char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    ਪੂਰਨ		inbacktick;

    inbacktick = FALSE;
    ਲਈ (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	ਜੇ (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	ਨਹੀਂਤਾਂ
	{
	    // An item ends at a space not in backticks
	    ਜੇ (!inbacktick && vim_isspace(*str))
		ਰੋਕੋ;
	    ਜੇ (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    ਵਾਪਸ str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ਸਥਿਰ ਪੂਰਨ
get_arglist(garray_T *gap, char_u *str, ਪੂਰਨ escaped)
{
    ga_init2(gap, ਆਕਾਰ(char_u *), 20);
    ਜਦੋਂ (*str != NUL)
    {
	ਜੇ (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    ਵਾਪਸ FAIL;
	}
	((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	ਜੇ (!escaped)
	    ਵਾਪਸ OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    ਪੂਰਨ
get_arglist_exp(
    char_u	*str,
    ਪੂਰਨ		*fcountp,
    char_u	***fnamesp,
    ਪੂਰਨ		wig)
{
    garray_T	ga;
    ਪੂਰਨ		i;

    ਜੇ (get_arglist(&ga, str, TRUE) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_ਅੰਕੜੇ,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    ਨਹੀਂਤਾਂ
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_ਅੰਕੜੇ,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    ਵਾਪਸ i;
}
#ਜੇ_ਅੰਤ

/*
 * Check the validity of the arg_idx for each other window.
 */
    ਸਥਿਰ ਖਾਲੀ
alist_check_arg_idx(ਖਾਲੀ)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ਜੇ (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    ਸਥਿਰ ਖਾਲੀ
alist_ਜੋੜੋ_list(
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    char_u	**files,
    ਪੂਰਨ		after,	    // where to add: 0 = before first one
    ਪੂਰਨ		will_edit)  // will edit adding argument
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		old_argcount = ARGCOUNT;

    ਜੇ (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, ਗਿਣਤੀ_2))
    {
	win_T	*wp = curwin;

	ਜੇ (after < 0)
	    after = 0;
	ਜੇ (after > ARGCOUNT)
	    after = ARGCOUNT;
	ਜੇ (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + ਗਿਣਤੀ_2]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * ਆਕਾਰ(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	{
	    ਪੂਰਨ flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_ਜੋੜੋ(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += ਗਿਣਤੀ_2;
	ਜੇ (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += ਗਿਣਤੀ_2;
	ਵਾਪਸ;
    }

    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ਸਥਿਰ ਖਾਲੀ
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    ਪੂਰਨ		didone;
    ਪੂਰਨ		i;
    char_u	*p;
    ਪੂਰਨ		ਮੇਲ;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ਲਈ (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_ਅੰਕੜੇ)[i];
	p = file_pat_ਮੰਜ਼ਿਲ_reg_pat(p, ਨਿਰਾ, ਨਿਰਾ, FALSE);
	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	{
	    vim_free(p);
	    ਰੋਕੋ;
	}

	didone = FALSE;
	ਲਈ (ਮੇਲ = 0; ਮੇਲ < ARGCOUNT; ++ਮੇਲ)
	    ਜੇ (vim_regexec(&regmatch, alist_name(&ARGLIST[ਮੇਲ]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[ਮੇਲ].ae_fname);
		mch_memmove(ARGLIST + ਮੇਲ, ARGLIST + ਮੇਲ + 1,
			(ARGCOUNT - ਮੇਲ - 1) * ਆਕਾਰ(aentry_T));
		--ARGCOUNT;
		ਜੇ (curwin->w_arg_idx > ਮੇਲ)
		    --curwin->w_arg_idx;
		--ਮੇਲ;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	ਜੇ (!didone)
	    semsg(_(e_no_ਮੇਲ_str_2), ((char_u **)alist_ga->ga_ਅੰਕੜੇ)[i]);
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
    ਸਥਿਰ ਪੂਰਨ
do_arglist(
    char_u	*str,
    ਪੂਰਨ		what,
    ਪੂਰਨ		after ਅਣਵਰਤਿਆ,	// 0 means before first one
    ਪੂਰਨ		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    ਪੂਰਨ		exp_ਗਿਣਤੀ;
    char_u	**exp_files;
    ਪੂਰਨ		i;
    ਪੂਰਨ		arg_escaped = TRUE;

    ਜੇ (check_arglist_locked() == FAIL)
	ਵਾਪਸ FAIL;

    // Set default argument for ":argadd" command.
    ਜੇ (what == AL_ਜੋੜੋ && *str == NUL)
    {
	ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    ਜੇ (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (what == AL_DEL)
	arglist_del_files(&new_ga);
    ਨਹੀਂਤਾਂ
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_ਅੰਕੜੇ,
		&exp_ਗਿਣਤੀ, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	ਜੇ (i == FAIL || exp_ਗਿਣਤੀ == 0)
	{
	    emsg(_(e_no_ਮੇਲ));
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (what == AL_ਜੋੜੋ)
	{
	    alist_ਜੋੜੋ_list(exp_ਗਿਣਤੀ, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	ਨਹੀਂਤਾਂ // what == AL_SET
	    alist_ਸੈੱਟ(ALIST(curwin), exp_ਗਿਣਤੀ, exp_files, will_edit, ਨਿਰਾ, 0);
    }

    alist_check_arg_idx();

    ਵਾਪਸ OK;
}

/*
 * Redefine the argument list.
 */
    ਖਾਲੀ
ਸੈੱਟ_arglist(char_u *str)
{
    do_arglist(str, AL_ਸੈੱਟ, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    ਪੂਰਨ
editing_arg_idx(win_T *win)
{
    ਵਾਪਸ !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_ਬਫਰ->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_ਬਫਰ->b_ffname == ਨਿਰਾ
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_ਬਫਰ->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    ਖਾਲੀ
check_arg_idx(win_T *win)
{
    ਜੇ (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	ਜੇ (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_ਆਖਰੀ == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_ਬਫਰ->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_ਬਫਰ->b_ffname != ਨਿਰਾ
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_ਬਫਰ->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_ਆਖਰੀ = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	ਜੇ (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_ਆਖਰੀ = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    ਖਾਲੀ
ex_ਦਲੀਲਾਂ(exarg_T *eap)
{
    ਪੂਰਨ		i;

    ਜੇ (eap->cmdidx != CMD_ਦਲੀਲਾਂ)
    {
	ਜੇ (check_arglist_locked() == FAIL)
	    ਵਾਪਸ;
	alist_unlink(ALIST(curwin));
	ਜੇ (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	ਨਹੀਂਤਾਂ // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    ਜੇ (*eap->arg != NUL)
    {
	ਜੇ (check_arglist_locked() == FAIL)
	    ਵਾਪਸ;
	ex_ਅਗਲਾ(eap);
	ਵਾਪਸ;
    }

    // ":args": list arguments.
    ਜੇ (eap->cmdidx == CMD_ਦਲੀਲਾਂ)
    {
	char_u **ਚੀਜ਼ਾਂ;

	ਜੇ (ARGCOUNT <= 0)
	    ਵਾਪਸ;		// empty argument list

	ਚੀਜ਼ਾਂ = ALLOC_MULT(char_u *, ARGCOUNT);
	ਜੇ (ਚੀਜ਼ਾਂ == ਨਿਰਾ)
	    ਵਾਪਸ;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	ਲਈ (i = 0; i < ARGCOUNT; ++i)
	    ਚੀਜ਼ਾਂ[i] = alist_name(&ARGLIST[i]);
	list_ਵਿੱਚ_columns(ਚੀਜ਼ਾਂ, ARGCOUNT, curwin->w_arg_idx);
	vim_free(ਚੀਜ਼ਾਂ);

	ਵਾਪਸ;
    }

    // ":argslocal": make a local copy of the global argument list.
    ਜੇ (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	ਜੇ (GA_GROW_FAILS(gap, GARGCOUNT))
	    ਵਾਪਸ;

	ਲਈ (i = 0; i < GARGCOUNT; ++i)
	    ਜੇ (GARGLIST[i].ae_fname != ਨਿਰਾ)
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
    ਖਾਲੀ
ex_ਪਿਛਲਾ(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    ਜੇ (curwin->w_arg_idx - (ਪੂਰਨ)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    ਨਹੀਂਤਾਂ
	do_argfile(eap, curwin->w_arg_idx - (ਪੂਰਨ)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    ਖਾਲੀ
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    ਖਾਲੀ
ex_ਆਖਰੀ(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    ਖਾਲੀ
ex_ਦਲੀਲ(exarg_T *eap)
{
    ਪੂਰਨ		i;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	i = eap->line2 - 1;
    ਨਹੀਂਤਾਂ
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    ਖਾਲੀ
do_argfile(exarg_T *eap, ਪੂਰਨ argn)
{
    ਪੂਰਨ		other;
    char_u	*p;
    ਪੂਰਨ		old_arg_idx = curwin->w_arg_idx;
    ਪੂਰਨ is_ਵੰਡੋ_cmd = *eap->cmd == 's';

    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    ਜੇ (argn < 0 || argn >= ARGCOUNT)
    {
	ਜੇ (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_ਮੰਜ਼ਿਲ_edit));
	ਨਹੀਂਤਾਂ ਜੇ (argn < 0)
	    emsg(_(e_cannot_go_before_ਪਹਿਲਾ_file));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_cannot_go_beyond_ਆਖਰੀ_file));

	ਵਾਪਸ;
    }

    ਜੇ (!is_ਵੰਡੋ_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_ਸੈੱਟ_curbuf_forceit(eap->forceit))
	ਵਾਪਸ;

    setpcmark();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // split window or create new tab page first
    ਜੇ (is_ਵੰਡੋ_cmd || cmdmod.cmod_tab != 0)
    {
	ਜੇ (win_ਵੰਡੋ(0, 0) == FAIL)
	    ਵਾਪਸ;
	RESET_BINDING(curwin);
    }
    ਨਹੀਂਤਾਂ
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	ਜੇ (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	ਜੇ ((!buf_hide(curbuf) || !other)
		&& check_ਬਦਲਿਆ(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    ਵਾਪਸ;
    }

    curwin->w_arg_idx = argn;
    ਜੇ (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_ਆਖਰੀ = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    ਜੇ (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), ਨਿਰਾ,
		eap, ECMD_ਆਖਰੀ,
		(buf_hide(curwin->w_ਬਫਰ) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    ਖਾਲੀ
ex_ਅਗਲਾ(exarg_T *eap)
{
    ਪੂਰਨ		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    ਜੇ (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_ਬਦਲਿਆ(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	ਜੇ (*eap->arg != NUL)		    // redefine file list
	{
	    ਜੇ (do_arglist(eap->arg, AL_ਸੈੱਟ, 0, TRUE) == FAIL)
		ਵਾਪਸ;
	    i = 0;
	}
	ਨਹੀਂਤਾਂ
	    i = curwin->w_arg_idx + (ਪੂਰਨ)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    ਖਾਲੀ
ex_argdedupe(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ j;

    ਲਈ (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *ਪਹਿਲਾ_Fullname = ਭਰਿਆ_Name_save(ARGLIST[i].ae_fname, FALSE);
	ਜੇ (ਪਹਿਲਾ_Fullname == ਨਿਰਾ)
	    ਵਾਪਸ;  // out of memory

	ਲਈ (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *ਦੂਜਾ_Fullname = ਭਰਿਆ_Name_save(ARGLIST[j].ae_fname, FALSE);
	    ਜੇ (ਦੂਜਾ_Fullname == ਨਿਰਾ)
		ਰੋਕੋ;  // out of memory
	    ਪੂਰਨ are_ਨਾਂ_Duplicate =
				  fnamecmp(ਪਹਿਲਾ_Fullname, ਦੂਜਾ_Fullname) == 0;
	    vim_free(ਦੂਜਾ_Fullname);

	    ਜੇ (are_ਨਾਂ_Duplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * ਆਕਾਰ(aentry_T));
		--ARGCOUNT;

		ਜੇ (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		ਨਹੀਂਤਾਂ ਜੇ (curwin->w_arg_idx > j)
		    --curwin->w_arg_idx;

		--j;
	    }
	}

	vim_free(ਪਹਿਲਾ_Fullname);
    }
}

/*
 * ":argedit"
 */
    ਖਾਲੀ
ex_argedit(exarg_T *eap)
{
    ਪੂਰਨ i = eap->addr_ਗਿਣਤੀ ? (ਪੂਰਨ)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    ਪੂਰਨ curbuf_is_reusable = curbuf_reusable();

    ਜੇ (do_arglist(eap->arg, AL_ਜੋੜੋ, i, TRUE) == FAIL)
	ਵਾਪਸ;
    maketitle();

    ਜੇ (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	    && (curbuf->b_ffname == ਨਿਰਾ || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    ਜੇ (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    ਖਾਲੀ
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ਜੋੜੋ,
	       eap->addr_ਗਿਣਤੀ > 0 ? (ਪੂਰਨ)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    ਖਾਲੀ
ex_argdelete(exarg_T *eap)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		n;

    ਜੇ (check_arglist_locked() == FAIL)
	ਵਾਪਸ;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
	{
	    ਜੇ (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_ਦਲੀਲ_ਮੰਜ਼ਿਲ_delete));
		ਵਾਪਸ;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	ਜੇ (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	ਜੇ (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    ਜੇ (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    ਵਾਪਸ;
	}

	ਲਈ (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * ਆਕਾਰ(aentry_T)));
	ARGCOUNT -= n;
	ਜੇ (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    ਨਹੀਂਤਾਂ
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    ਜੇ (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx >= ARGCOUNT)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ alist_name(&ARGLIST[idx]);
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
    ਜੇ (bp == ਨਿਰਾ || bp->b_fname == ਨਿਰਾ)
	ਵਾਪਸ aep->ae_fname;
    ਵਾਪਸ bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    alist_T	*alist;		// argument list to be used
    ਪੂਰਨ		had_tab;
    ਪੂਰਨ		keep_tabs;
    ਪੂਰਨ		forceit;

    ਪੂਰਨ		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    ਪੂਰਨ		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    ਸਥਿਰ ਖਾਲੀ
arg_all_close_ਅਣਵਰਤਿਆ_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    ਪੂਰਨ		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    ਜੇ (aall->had_tab > 0)
	goto_tabpage_tp(ਪਹਿਲਾ_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    ਲਈ (;;)
    {
	tpnext = curtab->tp_ਅਗਲਾ;
	ਲਈ (wp = firstwin; wp != ਨਿਰਾ; wp = wpnext)
	{
	    wpnext = wp->w_ਅਗਲਾ;
	    buf = wp->w_ਬਫਰ;
	    ਜੇ (buf->b_ffname == ਨਿਰਾ
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_ਚੌੜਾਈ != cmdline_ਚੌੜਾਈ)))
		i = aall->opened_len;
	    ਨਹੀਂਤਾਂ
	    {
		// check if the buffer in this window is in the arglist
		ਲਈ (i = 0; i < aall->opened_len; ++i)
		{
		    ਜੇ (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			ਪੂਰਨ ਭਾਰ = 1;

			ਜੇ (old_curtab == curtab)
			{
			    ++ਭਾਰ;
			    ਜੇ (old_curwin == wp)
				++ਭਾਰ;
			}

			ਜੇ (ਭਾਰ > (ਪੂਰਨ)aall->opened[i])
			{
			    aall->opened[i] = (char_u)ਭਾਰ;
			    ਜੇ (i == 0)
			    {
				ਜੇ (aall->new_curwin != ਨਿਰਾ)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (aall->keep_tabs)
			    i = aall->opened_len;

			ਜੇ (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			ਰੋਕੋ;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    ਜੇ (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		ਜੇ (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !buf_Is_ਬਦਲਿਆ(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    ਜੇ (!buf_hide(buf) && buf->b_nwindows <= 1
							 && buf_Is_ਬਦਲਿਆ(buf))
		    {
			bufref_T    bufref;

			ਸੈੱਟ_bufref(&bufref, buf);

			(ਖਾਲੀ)autowrite(buf, FALSE);

			// check if autocommands removed the window
			ਜੇ (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    ਜਾਰੀ;
			}
		    }
		    // don't close last window
		    ਜੇ (ONE_ਖਿੜਕੀ
			    && (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    ਨਹੀਂਤਾਂ
		    {
			win_close(wp, !buf_hide(buf) && !buf_Is_ਬਦਲਿਆ(buf));

			// check if autocommands removed the next window
			ਜੇ (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	ਜੇ (aall->had_tab == 0 || tpnext == ਨਿਰਾ)
	    ਰੋਕੋ;

	// check if autocommands removed the next tab page
	ਜੇ (!valid_tabpage(tpnext))
	    tpnext = ਪਹਿਲਾ_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ਸਥਿਰ ਖਾਲੀ
arg_all_open_windows(arg_all_state_T *aall, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    win_T	*wp;
    ਪੂਰਨ		tab_drop_ਖਾਲੀ_ਥਾਵਾਂ_ਖਿੜਕੀ = FALSE;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਵੰਡੋ_ret = OK;
    ਪੂਰਨ		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    ਜੇ (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == ਨਿਰਾ && !curbuf->b_ਬਦਲਿਆ)
    {
	aall->use_firstwin = TRUE;
	tab_drop_ਖਾਲੀ_ਥਾਵਾਂ_ਖਿੜਕੀ = TRUE;
    }

    ਲਈ (i = 0; i < ਗਿਣਤੀ_2 && !got_int; ++i)
    {
	ਜੇ (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_ਆਖਰੀ = TRUE;
	ਜੇ (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    ਜੇ (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    ਜੇ (wp->w_arg_idx == i)
		    {
			ਜੇ (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਚੌਖਟਾ->fr_ਮਾਪਾ
				!= curwin->w_ਚੌਖਟਾ->fr_ਮਾਪਾ)
			{
			    emsg(_(e_ਖਿੜਕੀ_layout_ਬਦਲਿਆ_unexpectedly));
			    i = ਗਿਣਤੀ_2;
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ
			    win_move_after(wp, curwin);
			ਰੋਕੋ;
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵੰਡੋ_ret == OK)
	{
	    // trigger events for tab drop
	    ਜੇ (tab_drop_ਖਾਲੀ_ਥਾਵਾਂ_ਖਿੜਕੀ && i == ਗਿਣਤੀ_2 - 1)
		--autocmd_no_enter;
	    ਜੇ (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		ਵੰਡੋ_ret = win_ਵੰਡੋ(0, WSP_ਗੁੰਜਾਇਸ਼ | WSP_ਹੇਠਾਂ);
		p_ea = p_ea_save;
		ਜੇ (ਵੰਡੋ_ret == FAIL)
		    ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    ਜੇ (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (ਖਾਲੀ)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), ਨਿਰਾ, ਨਿਰਾ,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_ਬਫਰ)
		      || buf_Is_ਬਦਲਿਆ(curwin->w_ਬਫਰ)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    ਜੇ (tab_drop_ਖਾਲੀ_ਥਾਵਾਂ_ਖਿੜਕੀ && i == ਗਿਣਤੀ_2 - 1)
		++autocmd_no_enter;
	    ਜੇ (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	ਜੇ (aall->had_tab > 0 && tabpage_ਸੂਚਕਾਂਕ(ਨਿਰਾ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ਸਥਿਰ ਖਾਲੀ
do_arg_all(
    ਪੂਰਨ	ਗਿਣਤੀ_2,
    ਪੂਰਨ	forceit,		// hide buffers in current windows
    ਪੂਰਨ keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*ਆਖਰੀ_curwin;
    tabpage_T		*ਆਖਰੀ_curtab;
    ਪੂਰਨ			prev_arglist_locked = arglist_locked;

    ਜੇ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_ਵਿੱਚ_cmdline_ਖਿੜਕੀ));
	ਵਾਪਸ;
    }
    ਜੇ (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	ਵਾਪਸ;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = ਨਿਰਾ;
    aall.new_curtab = ਨਿਰਾ;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    ਜੇ (aall.opened == ਨਿਰਾ)
	ਵਾਪਸ;

    // Autocommands may do anything to the argument list.  Make sure it's not
    // freed while we are working here by "locking" it.  We still have to
    // watch out for its size being changed.
    aall.alist = curwin->w_alist;
    ++aall.alist->al_refcount;
    arglist_locked = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    tabpage_T *new_lu_tp = curtab;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    // Try closing all windows that are not in the argument list.
    // Also close windows that are not full width;
    // When 'hidden' or "forceit" set the buffer becomes hidden.
    // Windows that have a changed buffer and can't be hidden won't be closed.
    // When the ":tab" modifier was used do this for all tab pages.
    arg_all_close_ਅਣਵਰਤਿਆ_windows(&aall);

    // Open a window for files in the argument list that don't have one.
    // ARGCOUNT may change while doing this, because of autocommands.
    ਜੇ (ਗਿਣਤੀ_2 > aall.opened_len || ਗਿਣਤੀ_2 <= 0)
	ਗਿਣਤੀ_2 = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    ਆਖਰੀ_curwin = curwin;
    ਆਖਰੀ_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, ਗਿਣਤੀ_2);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    ਜੇ (ਆਖਰੀ_curtab != aall.new_curtab)
    {
	ਜੇ (valid_tabpage(ਆਖਰੀ_curtab))
	    goto_tabpage_tp(ਆਖਰੀ_curtab, TRUE, TRUE);
	ਜੇ (win_valid(ਆਖਰੀ_curwin))
	    win_enter(ਆਖਰੀ_curwin, FALSE);
    }
    // to window with first arg
    ਜੇ (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    ਜੇ (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    ਜੇ (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    ਖਾਲੀ
ex_all(exarg_T *eap)
{
    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
	eap->line2 = 9999;
    do_arg_all((ਪੂਰਨ)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(ਖਾਲੀ)
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		idx;
    char_u	*retval = ਨਿਰਾ;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    ਲਈ (;;)
    {
	len = 0;
	ਲਈ (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਰੀ;
	    ਜੇ (len > 0)
	    {
		// insert a space in between names
		ਜੇ (retval != ਨਿਰਾ)
		    retval[len] = ' ';
		++len;
	    }
	    ਲਈ ( ; *p != NUL; ++p)
	    {
		ਜੇ (*p == ' '
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
			|| *p == '\\'
#ਜੇ_ਅੰਤ
			|| *p == '`')
		{
		    // insert a backslash
		    ਜੇ (retval != ਨਿਰਾ)
			retval[len] = '\\';
		    ++len;
		}
		ਜੇ (retval != ਨਿਰਾ)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	ਜੇ (retval != ਨਿਰਾ)
	{
	    retval[len] = NUL;
	    ਰੋਕੋ;
	}

	// allocate memory
	retval = alloc(len + 1);
	ਜੇ (retval == ਨਿਰਾ)
	    ਰੋਕੋ;
    }

    ਵਾਪਸ retval;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    ਖਾਲੀ
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_ਸੰਖਿਆ = ARGCOUNT;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_ਸੰਖਿਆ
					   && tv_get_ਸੰਖਿਆ(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_ਸੰਖਿਆ = GARGCOUNT;
    ਨਹੀਂਤਾਂ
    {
	// use the argument list of the specified window
	wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
	ਜੇ (wp != ਨਿਰਾ)
	    rettv->vval.v_ਸੰਖਿਆ = WARGCOUNT(wp);
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_ਸੰਖਿਆ = -1;
    }
}

/*
 * "argidx()" function
 */
    ਖਾਲੀ
f_argidx(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    ਖਾਲੀ
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    wp = ਲੱਭੋ_tabwin(&argvars[0], &argvars[1], ਨਿਰਾ);
    ਜੇ (wp != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ਸਥਿਰ ਖਾਲੀ
get_arglist_as_rettv(aentry_T *arglist, ਪੂਰਨ argcount, typval_T *rettv)
{
    ਪੂਰਨ		idx;

    ਜੇ (rettv_list_alloc(rettv) == OK && arglist != ਨਿਰਾ)
	ਲਈ (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    ਖਾਲੀ
f_argv(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		idx;
    aentry_T	*arglist = ਨਿਰਾ;
    ਪੂਰਨ		argcount = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	ਵਾਪਸ;
    }

    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_ਸੰਖਿਆ
	    && tv_get_ਸੰਖਿਆ(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    ਨਹੀਂਤਾਂ
    {
	win_T	*wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[1]);

	ਜੇ (wp != ਨਿਰਾ)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
    idx = tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ);
    ਜੇ (arglist != ਨਿਰਾ && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    ਨਹੀਂਤਾਂ ਜੇ (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#ਜੇ_ਅੰਤ
