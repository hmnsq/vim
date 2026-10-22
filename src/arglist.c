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

#అన్తర్భావ "vim.h"

#నిర్వచించు AL_సమితి	1
#నిర్వచించు AL_కూడు	2
#నిర్వచించు AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
స్థిర పూర్ణాంకం arglist_locked = FALSE;

    స్థిర పూర్ణాంకం
check_arglist_locked(శూన్య)
{
    ఒకవేళ (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    శూన్య
alist_clear(alist_T *al)
{
    ఒకవేళ (check_arglist_locked() == FAIL)
	తిరిగి;
    అయితే (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    శూన్య
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, పరిమాణం(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    శూన్య
alist_unlink(alist_T *al)
{
    ఒకవేళ (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    శూన్య
alist_new(శూన్య)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    ఒకవేళ (curwin->w_alist == ఖాళీ)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    లేకపోతే
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#ఒకవేళ !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    శూన్య
alist_expand(పూర్ణాంకం *fnum_list, పూర్ణాంకం fnum_len)
{
    char_u	**old_arg_files;
    పూర్ణాంకం		old_arg_లెక్క;
    char_u	**new_arg_files;
    పూర్ణాంకం		new_arg_file_లెక్క;
    char_u	*save_p_su = p_su;
    పూర్ణాంకం		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    ఒకవేళ (old_arg_files == ఖాళీ)
	తిరిగి;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = ఖాళీలు_option;
    కొరకు (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_లెక్క = GARGCOUNT;
    ఒకవేళ (expand_wildcards(old_arg_లెక్క, old_arg_files,
		&new_arg_file_లెక్క, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_లెక్క > 0)
    {
	alist_సమితి(&global_alist, new_arg_file_లెక్క, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_లెక్క, old_arg_files);
    }
    p_su = save_p_su;
}
#అయితే_ముగింపు

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    శూన్య
alist_సమితి(
    alist_T	*al,
    పూర్ణాంకం		లెక్క,
    char_u	**files,
    పూర్ణాంకం		use_curbuf,
    పూర్ణాంకం		*fnum_list,
    పూర్ణాంకం		fnum_len)
{
    పూర్ణాంకం		i;

    ఒకవేళ (check_arglist_locked() == FAIL)
	తిరిగి;

    alist_clear(al);
    ఒకవేళ (GA_GROW_OK(&al->al_ga, లెక్క))
    {
	కొరకు (i = 0; i < లెక్క; ++i)
	{
	    ఒకవేళ (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		అయితే (i < లెక్క)
		    vim_free(files[i++]);
		ఆపు;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    ఒకవేళ (fnum_list != ఖాళీ && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_సమితి_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_కూడు(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    లేకపోతే
	FreeWild(లెక్క, files);
    ఒకవేళ (al == &global_alist)
	arg_had_చివరి = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    శూన్య
alist_కూడు(
    alist_T	*al,
    char_u	*fname,
    పూర్ణాంకం		సమితి_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    ఒకవేళ (fname == ఖాళీ)		// don't add NULL file names
	తిరిగి;
    ఒకవేళ (check_arglist_locked() == FAIL)
	తిరిగి;
    arglist_locked = TRUE;
    ++wp->w_locked;

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    slash_adjust(fname);
#అయితే_ముగింపు
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    ఒకవేళ (సమితి_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_కూడు(fname, BLN_LISTED | (సమితి_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    శూన్య
alist_slash_adjust(శూన్య)
{
    పూర్ణాంకం		i;
    win_T	*wp;
    tabpage_T	*tp;

    కొరకు (i = 0; i < GARGCOUNT; ++i)
	ఒకవేళ (GARGLIST[i].ae_fname != ఖాళీ)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_alist != &global_alist)
	    కొరకు (i = 0; i < WARGCOUNT(wp); ++i)
		ఒకవేళ (WARGLIST(wp)[i].ae_fname != ఖాళీ)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#అయితే_ముగింపు

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    స్థిర char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    పూర్ణాంకం		inbacktick;

    inbacktick = FALSE;
    కొరకు (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	ఒకవేళ (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	లేకపోతే
	{
	    // An item ends at a space not in backticks
	    ఒకవేళ (!inbacktick && vim_isspace(*str))
		ఆపు;
	    ఒకవేళ (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    తిరిగి str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    స్థిర పూర్ణాంకం
get_arglist(garray_T *gap, char_u *str, పూర్ణాంకం escaped)
{
    ga_init2(gap, పరిమాణం(char_u *), 20);
    అయితే (*str != NUL)
    {
	ఒకవేళ (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    తిరిగి FAIL;
	}
	((char_u **)gap->ga_దత్తాంశం)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	ఒకవేళ (!escaped)
	    తిరిగి OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
get_arglist_exp(
    char_u	*str,
    పూర్ణాంకం		*fcountp,
    char_u	***fnamesp,
    పూర్ణాంకం		wig)
{
    garray_T	ga;
    పూర్ణాంకం		i;

    ఒకవేళ (get_arglist(&ga, str, TRUE) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_దత్తాంశం,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    లేకపోతే
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_దత్తాంశం,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    తిరిగి i;
}
#అయితే_ముగింపు

/*
 * Check the validity of the arg_idx for each other window.
 */
    స్థిర శూన్య
alist_check_arg_idx(శూన్య)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ఒకవేళ (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    స్థిర శూన్య
alist_కూడు_list(
    పూర్ణాంకం		లెక్క,
    char_u	**files,
    పూర్ణాంకం		after,	    // where to add: 0 = before first one
    పూర్ణాంకం		will_edit)  // will edit adding argument
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		old_argcount = ARGCOUNT;

    ఒకవేళ (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, లెక్క))
    {
	win_T	*wp = curwin;

	ఒకవేళ (after < 0)
	    after = 0;
	ఒకవేళ (after > ARGCOUNT)
	    after = ARGCOUNT;
	ఒకవేళ (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + లెక్క]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * పరిమాణం(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	కొరకు (i = 0; i < లెక్క; ++i)
	{
	    పూర్ణాంకం flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_కూడు(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += లెక్క;
	ఒకవేళ (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += లెక్క;
	తిరిగి;
    }

    కొరకు (i = 0; i < లెక్క; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    స్థిర శూన్య
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    పూర్ణాంకం		didone;
    పూర్ణాంకం		i;
    char_u	*p;
    పూర్ణాంకం		సరిపోలిక;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    కొరకు (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_దత్తాంశం)[i];
	p = file_pat_గమ్యం_reg_pat(p, ఖాళీ, ఖాళీ, FALSE);
	ఒకవేళ (p == ఖాళీ)
	    ఆపు;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	{
	    vim_free(p);
	    ఆపు;
	}

	didone = FALSE;
	కొరకు (సరిపోలిక = 0; సరిపోలిక < ARGCOUNT; ++సరిపోలిక)
	    ఒకవేళ (vim_regexec(&regmatch, alist_name(&ARGLIST[సరిపోలిక]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[సరిపోలిక].ae_fname);
		mch_memmove(ARGLIST + సరిపోలిక, ARGLIST + సరిపోలిక + 1,
			(ARGCOUNT - సరిపోలిక - 1) * పరిమాణం(aentry_T));
		--ARGCOUNT;
		ఒకవేళ (curwin->w_arg_idx > సరిపోలిక)
		    --curwin->w_arg_idx;
		--సరిపోలిక;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	ఒకవేళ (!didone)
	    semsg(_(e_no_సరిపోలిక_str_2), ((char_u **)alist_ga->ga_దత్తాంశం)[i]);
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
    స్థిర పూర్ణాంకం
do_arglist(
    char_u	*str,
    పూర్ణాంకం		what,
    పూర్ణాంకం		after వాడని,	// 0 means before first one
    పూర్ణాంకం		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    పూర్ణాంకం		exp_లెక్క;
    char_u	**exp_files;
    పూర్ణాంకం		i;
    పూర్ణాంకం		arg_escaped = TRUE;

    ఒకవేళ (check_arglist_locked() == FAIL)
	తిరిగి FAIL;

    // Set default argument for ":argadd" command.
    ఒకవేళ (what == AL_కూడు && *str == NUL)
    {
	ఒకవేళ (curbuf->b_ffname == ఖాళీ)
	    తిరిగి FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    ఒకవేళ (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (what == AL_DEL)
	arglist_del_files(&new_ga);
    లేకపోతే
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_దత్తాంశం,
		&exp_లెక్క, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	ఒకవేళ (i == FAIL || exp_లెక్క == 0)
	{
	    emsg(_(e_no_సరిపోలిక));
	    తిరిగి FAIL;
	}

	ఒకవేళ (what == AL_కూడు)
	{
	    alist_కూడు_list(exp_లెక్క, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	లేకపోతే // what == AL_SET
	    alist_సమితి(ALIST(curwin), exp_లెక్క, exp_files, will_edit, ఖాళీ, 0);
    }

    alist_check_arg_idx();

    తిరిగి OK;
}

/*
 * Redefine the argument list.
 */
    శూన్య
సమితి_arglist(char_u *str)
{
    do_arglist(str, AL_సమితి, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    పూర్ణాంకం
editing_arg_idx(win_T *win)
{
    తిరిగి !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_బఫరు->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_బఫరు->b_ffname == ఖాళీ
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_బఫరు->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    శూన్య
check_arg_idx(win_T *win)
{
    ఒకవేళ (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	ఒకవేళ (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_చివరి == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_బఫరు->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_బఫరు->b_ffname != ఖాళీ
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_బఫరు->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_చివరి = TRUE;
    }
    లేకపోతే
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	ఒకవేళ (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_చివరి = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    శూన్య
ex_వాదనలు(exarg_T *eap)
{
    పూర్ణాంకం		i;

    ఒకవేళ (eap->cmdidx != CMD_వాదనలు)
    {
	ఒకవేళ (check_arglist_locked() == FAIL)
	    తిరిగి;
	alist_unlink(ALIST(curwin));
	ఒకవేళ (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	లేకపోతే // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    ఒకవేళ (*eap->arg != NUL)
    {
	ఒకవేళ (check_arglist_locked() == FAIL)
	    తిరిగి;
	ex_తదుపరి(eap);
	తిరిగి;
    }

    // ":args": list arguments.
    ఒకవేళ (eap->cmdidx == CMD_వాదనలు)
    {
	char_u **వస్తువులు;

	ఒకవేళ (ARGCOUNT <= 0)
	    తిరిగి;		// empty argument list

	వస్తువులు = ALLOC_MULT(char_u *, ARGCOUNT);
	ఒకవేళ (వస్తువులు == ఖాళీ)
	    తిరిగి;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	కొరకు (i = 0; i < ARGCOUNT; ++i)
	    వస్తువులు[i] = alist_name(&ARGLIST[i]);
	list_లోపల_columns(వస్తువులు, ARGCOUNT, curwin->w_arg_idx);
	vim_free(వస్తువులు);

	తిరిగి;
    }

    // ":argslocal": make a local copy of the global argument list.
    ఒకవేళ (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	ఒకవేళ (GA_GROW_FAILS(gap, GARGCOUNT))
	    తిరిగి;

	కొరకు (i = 0; i < GARGCOUNT; ++i)
	    ఒకవేళ (GARGLIST[i].ae_fname != ఖాళీ)
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
    శూన్య
ex_మునుపటి(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    ఒకవేళ (curwin->w_arg_idx - (పూర్ణాంకం)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    లేకపోతే
	do_argfile(eap, curwin->w_arg_idx - (పూర్ణాంకం)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    శూన్య
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    శూన్య
ex_చివరి(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    శూన్య
ex_వాదం(exarg_T *eap)
{
    పూర్ణాంకం		i;

    ఒకవేళ (eap->addr_లెక్క > 0)
	i = eap->line2 - 1;
    లేకపోతే
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    శూన్య
do_argfile(exarg_T *eap, పూర్ణాంకం argn)
{
    పూర్ణాంకం		other;
    char_u	*p;
    పూర్ణాంకం		old_arg_idx = curwin->w_arg_idx;
    పూర్ణాంకం is_విభజించు_cmd = *eap->cmd == 's';

    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    ఒకవేళ (argn < 0 || argn >= ARGCOUNT)
    {
	ఒకవేళ (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_గమ్యం_edit));
	లేకపోతే ఒకవేళ (argn < 0)
	    emsg(_(e_cannot_go_before_మొదటి_file));
	లేకపోతే
	    emsg(_(e_cannot_go_beyond_చివరి_file));

	తిరిగి;
    }

    ఒకవేళ (!is_విభజించు_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_సమితి_curbuf_forceit(eap->forceit))
	తిరిగి;

    setpcmark();
#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // split window or create new tab page first
    ఒకవేళ (is_విభజించు_cmd || cmdmod.cmod_tab != 0)
    {
	ఒకవేళ (win_విభజించు(0, 0) == FAIL)
	    తిరిగి;
	RESET_BINDING(curwin);
    }
    లేకపోతే
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	ఒకవేళ (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	ఒకవేళ ((!buf_hide(curbuf) || !other)
		&& check_మారింది(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    తిరిగి;
    }

    curwin->w_arg_idx = argn;
    ఒకవేళ (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_చివరి = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    ఒకవేళ (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), ఖాళీ,
		eap, ECMD_చివరి,
		(buf_hide(curwin->w_బఫరు) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    లేకపోతే ఒకవేళ (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    శూన్య
ex_తదుపరి(exarg_T *eap)
{
    పూర్ణాంకం		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    ఒకవేళ (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_మారింది(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	ఒకవేళ (*eap->arg != NUL)		    // redefine file list
	{
	    ఒకవేళ (do_arglist(eap->arg, AL_సమితి, 0, TRUE) == FAIL)
		తిరిగి;
	    i = 0;
	}
	లేకపోతే
	    i = curwin->w_arg_idx + (పూర్ణాంకం)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    శూన్య
ex_argdedupe(exarg_T *eap వాడని)
{
    పూర్ణాంకం i;
    పూర్ణాంకం j;

    కొరకు (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *మొదటి_Fullname = నిండిన_Name_save(ARGLIST[i].ae_fname, FALSE);
	ఒకవేళ (మొదటి_Fullname == ఖాళీ)
	    తిరిగి;  // out of memory

	కొరకు (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *రెండవ_Fullname = నిండిన_Name_save(ARGLIST[j].ae_fname, FALSE);
	    ఒకవేళ (రెండవ_Fullname == ఖాళీ)
		ఆపు;  // out of memory
	    పూర్ణాంకం are_పేర్లు_Duplicate =
				  fnamecmp(మొదటి_Fullname, రెండవ_Fullname) == 0;
	    vim_free(రెండవ_Fullname);

	    ఒకవేళ (are_పేర్లు_Duplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * పరిమాణం(aentry_T));
		--ARGCOUNT;

		ఒకవేళ (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		లేకపోతే ఒకవేళ (curwin->w_arg_idx > j)
		    --curwin->w_arg_idx;

		--j;
	    }
	}

	vim_free(మొదటి_Fullname);
    }
}

/*
 * ":argedit"
 */
    శూన్య
ex_argedit(exarg_T *eap)
{
    పూర్ణాంకం i = eap->addr_లెక్క ? (పూర్ణాంకం)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    పూర్ణాంకం curbuf_is_reusable = curbuf_reusable();

    ఒకవేళ (do_arglist(eap->arg, AL_కూడు, i, TRUE) == FAIL)
	తిరిగి;
    maketitle();

    ఒకవేళ (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	    && (curbuf->b_ffname == ఖాళీ || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    ఒకవేళ (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    శూన్య
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_కూడు,
	       eap->addr_లెక్క > 0 ? (పూర్ణాంకం)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    శూన్య
ex_argdelete(exarg_T *eap)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		n;

    ఒకవేళ (check_arglist_locked() == FAIL)
	తిరిగి;

    ఒకవేళ (eap->addr_లెక్క > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	ఒకవేళ (eap->addr_లెక్క == 0)
	{
	    ఒకవేళ (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_వాదం_గమ్యం_delete));
		తిరిగి;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	లేకపోతే ఒకవేళ (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	ఒకవేళ (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	ఒకవేళ (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    ఒకవేళ (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    తిరిగి;
	}

	కొరకు (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * పరిమాణం(aentry_T)));
	ARGCOUNT -= n;
	ఒకవేళ (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	లేకపోతే ఒకవేళ (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    లేకపోతే
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    ఒకవేళ (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    లేకపోతే ఒకవేళ (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx >= ARGCOUNT)
	తిరిగి ఖాళీ;

    తిరిగి alist_name(&ARGLIST[idx]);
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
    ఒకవేళ (bp == ఖాళీ || bp->b_fname == ఖాళీ)
	తిరిగి aep->ae_fname;
    తిరిగి bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
రకనిర్వచనం నిర్మాణం {
    alist_T	*alist;		// argument list to be used
    పూర్ణాంకం		had_tab;
    పూర్ణాంకం		keep_tabs;
    పూర్ణాంకం		forceit;

    పూర్ణాంకం		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    పూర్ణాంకం		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    స్థిర శూన్య
arg_all_close_వాడని_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    పూర్ణాంకం		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    ఒకవేళ (aall->had_tab > 0)
	goto_tabpage_tp(మొదటి_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    కొరకు (;;)
    {
	tpnext = curtab->tp_తదుపరి;
	కొరకు (wp = firstwin; wp != ఖాళీ; wp = wpnext)
	{
	    wpnext = wp->w_తదుపరి;
	    buf = wp->w_బఫరు;
	    ఒకవేళ (buf->b_ffname == ఖాళీ
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_వెడల్పు != cmdline_వెడల్పు)))
		i = aall->opened_len;
	    లేకపోతే
	    {
		// check if the buffer in this window is in the arglist
		కొరకు (i = 0; i < aall->opened_len; ++i)
		{
		    ఒకవేళ (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			పూర్ణాంకం బరువు = 1;

			ఒకవేళ (old_curtab == curtab)
			{
			    ++బరువు;
			    ఒకవేళ (old_curwin == wp)
				++బరువు;
			}

			ఒకవేళ (బరువు > (పూర్ణాంకం)aall->opened[i])
			{
			    aall->opened[i] = (char_u)బరువు;
			    ఒకవేళ (i == 0)
			    {
				ఒకవేళ (aall->new_curwin != ఖాళీ)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			లేకపోతే ఒకవేళ (aall->keep_tabs)
			    i = aall->opened_len;

			ఒకవేళ (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			ఆపు;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    ఒకవేళ (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		ఒకవేళ (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !buf_Is_మారింది(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    ఒకవేళ (!buf_hide(buf) && buf->b_nwindows <= 1
							 && buf_Is_మారింది(buf))
		    {
			bufref_T    bufref;

			సమితి_bufref(&bufref, buf);

			(శూన్య)autowrite(buf, FALSE);

			// check if autocommands removed the window
			ఒకవేళ (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    కొనసాగు;
			}
		    }
		    // don't close last window
		    ఒకవేళ (ONE_కిటికీ
			    && (మొదటి_tabpage->tp_తదుపరి == ఖాళీ
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    లేకపోతే
		    {
			win_close(wp, !buf_hide(buf) && !buf_Is_మారింది(buf));

			// check if autocommands removed the next window
			ఒకవేళ (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	ఒకవేళ (aall->had_tab == 0 || tpnext == ఖాళీ)
	    ఆపు;

	// check if autocommands removed the next tab page
	ఒకవేళ (!valid_tabpage(tpnext))
	    tpnext = మొదటి_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    స్థిర శూన్య
arg_all_open_windows(arg_all_state_T *aall, పూర్ణాంకం లెక్క)
{
    win_T	*wp;
    పూర్ణాంకం		tab_drop_ఖాళీలు_కిటికీ = FALSE;
    పూర్ణాంకం		i;
    పూర్ణాంకం		విభజించు_ret = OK;
    పూర్ణాంకం		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    ఒకవేళ (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == ఖాళీ && !curbuf->b_మారింది)
    {
	aall->use_firstwin = TRUE;
	tab_drop_ఖాళీలు_కిటికీ = TRUE;
    }

    కొరకు (i = 0; i < లెక్క && !got_int; ++i)
    {
	ఒకవేళ (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_చివరి = TRUE;
	ఒకవేళ (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    ఒకవేళ (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    ఒకవేళ (wp->w_arg_idx == i)
		    {
			ఒకవేళ (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			లేకపోతే ఒకవేళ (wp->w_చట్రం->fr_మాతృ
				!= curwin->w_చట్రం->fr_మాతృ)
			{
			    emsg(_(e_కిటికీ_layout_మారింది_unexpectedly));
			    i = లెక్క;
			    ఆపు;
			}
			లేకపోతే
			    win_move_after(wp, curwin);
			ఆపు;
		    }
		}
	    }
	}
	లేకపోతే ఒకవేళ (విభజించు_ret == OK)
	{
	    // trigger events for tab drop
	    ఒకవేళ (tab_drop_ఖాళీలు_కిటికీ && i == లెక్క - 1)
		--autocmd_no_enter;
	    ఒకవేళ (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		విభజించు_ret = win_విభజించు(0, WSP_స్థలం | WSP_క్రింద);
		p_ea = p_ea_save;
		ఒకవేళ (విభజించు_ret == FAIL)
		    కొనసాగు;
	    }
	    లేకపోతే    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    ఒకవేళ (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (శూన్య)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), ఖాళీ, ఖాళీ,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_బఫరు)
		      || buf_Is_మారింది(curwin->w_బఫరు)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    ఒకవేళ (tab_drop_ఖాళీలు_కిటికీ && i == లెక్క - 1)
		++autocmd_no_enter;
	    ఒకవేళ (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	ఒకవేళ (aall->had_tab > 0 && tabpage_సూచీ(ఖాళీ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    స్థిర శూన్య
do_arg_all(
    పూర్ణాంకం	లెక్క,
    పూర్ణాంకం	forceit,		// hide buffers in current windows
    పూర్ణాంకం keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*చివరి_curwin;
    tabpage_T		*చివరి_curtab;
    పూర్ణాంకం			prev_arglist_locked = arglist_locked;

    ఒకవేళ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_లోపల_cmdline_కిటికీ));
	తిరిగి;
    }
    ఒకవేళ (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	తిరిగి;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = ఖాళీ;
    aall.new_curtab = ఖాళీ;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    ఒకవేళ (aall.opened == ఖాళీ)
	తిరిగి;

    // Autocommands may do anything to the argument list.  Make sure it's not
    // freed while we are working here by "locking" it.  We still have to
    // watch out for its size being changed.
    aall.alist = curwin->w_alist;
    ++aall.alist->al_refcount;
    arglist_locked = TRUE;

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    tabpage_T *new_lu_tp = curtab;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    // Try closing all windows that are not in the argument list.
    // Also close windows that are not full width;
    // When 'hidden' or "forceit" set the buffer becomes hidden.
    // Windows that have a changed buffer and can't be hidden won't be closed.
    // When the ":tab" modifier was used do this for all tab pages.
    arg_all_close_వాడని_windows(&aall);

    // Open a window for files in the argument list that don't have one.
    // ARGCOUNT may change while doing this, because of autocommands.
    ఒకవేళ (లెక్క > aall.opened_len || లెక్క <= 0)
	లెక్క = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    చివరి_curwin = curwin;
    చివరి_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, లెక్క);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    ఒకవేళ (చివరి_curtab != aall.new_curtab)
    {
	ఒకవేళ (valid_tabpage(చివరి_curtab))
	    goto_tabpage_tp(చివరి_curtab, TRUE, TRUE);
	ఒకవేళ (win_valid(చివరి_curwin))
	    win_enter(చివరి_curwin, FALSE);
    }
    // to window with first arg
    ఒకవేళ (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    ఒకవేళ (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    ఒకవేళ (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    శూన్య
ex_all(exarg_T *eap)
{
    ఒకవేళ (eap->addr_లెక్క == 0)
	eap->line2 = 9999;
    do_arg_all((పూర్ణాంకం)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(శూన్య)
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		idx;
    char_u	*retval = ఖాళీ;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    కొరకు (;;)
    {
	len = 0;
	కొరకు (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    ఒకవేళ (p == ఖాళీ)
		కొనసాగు;
	    ఒకవేళ (len > 0)
	    {
		// insert a space in between names
		ఒకవేళ (retval != ఖాళీ)
		    retval[len] = ' ';
		++len;
	    }
	    కొరకు ( ; *p != NUL; ++p)
	    {
		ఒకవేళ (*p == ' '
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
			|| *p == '\\'
#అయితే_ముగింపు
			|| *p == '`')
		{
		    // insert a backslash
		    ఒకవేళ (retval != ఖాళీ)
			retval[len] = '\\';
		    ++len;
		}
		ఒకవేళ (retval != ఖాళీ)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	ఒకవేళ (retval != ఖాళీ)
	{
	    retval[len] = NUL;
	    ఆపు;
	}

	// allocate memory
	retval = alloc(len + 1);
	ఒకవేళ (retval == ఖాళీ)
	    ఆపు;
    }

    తిరిగి retval;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    శూన్య
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_సంఖ్య = ARGCOUNT;
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_సంఖ్య
					   && tv_get_సంఖ్య(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_సంఖ్య = GARGCOUNT;
    లేకపోతే
    {
	// use the argument list of the specified window
	wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
	ఒకవేళ (wp != ఖాళీ)
	    rettv->vval.v_సంఖ్య = WARGCOUNT(wp);
	లేకపోతే
	    rettv->vval.v_సంఖ్య = -1;
    }
}

/*
 * "argidx()" function
 */
    శూన్య
f_argidx(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    శూన్య
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    rettv->vval.v_సంఖ్య = -1;
    wp = కనుగొను_tabwin(&argvars[0], &argvars[1], ఖాళీ);
    ఒకవేళ (wp != ఖాళీ)
	rettv->vval.v_సంఖ్య = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    స్థిర శూన్య
get_arglist_as_rettv(aentry_T *arglist, పూర్ణాంకం argcount, typval_T *rettv)
{
    పూర్ణాంకం		idx;

    ఒకవేళ (rettv_list_alloc(rettv) == OK && arglist != ఖాళీ)
	కొరకు (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    శూన్య
f_argv(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		idx;
    aentry_T	*arglist = ఖాళీ;
    పూర్ణాంకం		argcount = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	తిరిగి;
    }

    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_సంఖ్య
	    && tv_get_సంఖ్య(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    లేకపోతే
    {
	win_T	*wp = కనుగొను_win_by_nr_or_id(&argvars[1]);

	ఒకవేళ (wp != ఖాళీ)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    idx = tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ);
    ఒకవేళ (arglist != ఖాళీ && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    లేకపోతే ఒకవేళ (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#అయితే_ముగింపు
