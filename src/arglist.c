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

#अन्तर्भाव "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
स्थिर पूर्णाङ्क arglist_locked = FALSE;

    स्थिर पूर्णाङ्क
check_arglist_locked(शून्य)
{
    यदि (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    शून्य
alist_clear(alist_T *al)
{
    यदि (check_arglist_locked() == FAIL)
	प्रत्यागम;
    यावत् (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    शून्य
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, परिमाण(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    शून्य
alist_unlink(alist_T *al)
{
    यदि (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    शून्य
alist_new(शून्य)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    यदि (curwin->w_alist == रिक्त)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    अन्यथा
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#यदि !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    शून्य
alist_expand(पूर्णाङ्क *fnum_list, पूर्णाङ्क fnum_len)
{
    char_u	**old_arg_files;
    पूर्णाङ्क		old_arg_गिनती;
    char_u	**new_arg_files;
    पूर्णाङ्क		new_arg_file_गिनती;
    char_u	*save_p_su = p_su;
    पूर्णाङ्क		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    यदि (old_arg_files == रिक्त)
	प्रत्यागम;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    प्रति (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_गिनती = GARGCOUNT;
    यदि (expand_wildcards(old_arg_गिनती, old_arg_files,
		&new_arg_file_गिनती, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_गिनती > 0)
    {
	alist_set(&global_alist, new_arg_file_गिनती, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_गिनती, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    शून्य
alist_set(
    alist_T	*al,
    पूर्णाङ्क		गिनती,
    char_u	**files,
    पूर्णाङ्क		use_curbuf,
    पूर्णाङ्क		*fnum_list,
    पूर्णाङ्क		fnum_len)
{
    पूर्णाङ्क		i;

    यदि (check_arglist_locked() == FAIL)
	प्रत्यागम;

    alist_clear(al);
    यदि (GA_GROW_OK(&al->al_ga, गिनती))
    {
	प्रति (i = 0; i < गिनती; ++i)
	{
	    यदि (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		यावत् (i < गिनती)
		    vim_free(files[i++]);
		भङ्ग;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    यदि (fnum_list != रिक्त && i < fnum_len)
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
    अन्यथा
	FreeWild(गिनती, files);
    यदि (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    शून्य
alist_add(
    alist_T	*al,
    char_u	*fname,
    पूर्णाङ्क		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    यदि (fname == रिक्त)		// don't add NULL file names
	प्रत्यागम;
    यदि (check_arglist_locked() == FAIL)
	प्रत्यागम;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    यदि (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#यदि defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    शून्य
alist_slash_adjust(शून्य)
{
    पूर्णाङ्क		i;
    win_T	*wp;
    tabpage_T	*tp;

    प्रति (i = 0; i < GARGCOUNT; ++i)
	यदि (GARGLIST[i].ae_fname != रिक्त)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	यदि (wp->w_alist != &global_alist)
	    प्रति (i = 0; i < WARGCOUNT(wp); ++i)
		यदि (WARGLIST(wp)[i].ae_fname != रिक्त)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    स्थिर char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    पूर्णाङ्क		inbacktick;

    inbacktick = FALSE;
    प्रति (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	यदि (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	अन्यथा
	{
	    // An item ends at a space not in backticks
	    यदि (!inbacktick && vim_isspace(*str))
		भङ्ग;
	    यदि (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    प्रत्यागम str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    स्थिर पूर्णाङ्क
get_arglist(garray_T *gap, char_u *str, पूर्णाङ्क escaped)
{
    ga_init2(gap, परिमाण(char_u *), 20);
    यावत् (*str != NUL)
    {
	यदि (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    प्रत्यागम FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	यदि (!escaped)
	    प्रत्यागम OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    प्रत्यागम OK;
}

#यदि defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    पूर्णाङ्क
get_arglist_exp(
    char_u	*str,
    पूर्णाङ्क		*fcountp,
    char_u	***fnamesp,
    पूर्णाङ्क		wig)
{
    garray_T	ga;
    पूर्णाङ्क		i;

    यदि (get_arglist(&ga, str, TRUE) == FAIL)
	प्रत्यागम FAIL;
    यदि (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    अन्यथा
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    प्रत्यागम i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    स्थिर शून्य
alist_check_arg_idx(शून्य)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	यदि (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    स्थिर शून्य
alist_add_list(
    पूर्णाङ्क		गिनती,
    char_u	**files,
    पूर्णाङ्क		after,	    // where to add: 0 = before first one
    पूर्णाङ्क		will_edit)  // will edit adding argument
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		old_argcount = ARGCOUNT;

    यदि (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, गिनती))
    {
	win_T	*wp = curwin;

	यदि (after < 0)
	    after = 0;
	यदि (after > ARGCOUNT)
	    after = ARGCOUNT;
	यदि (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + गिनती]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * परिमाण(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	प्रति (i = 0; i < गिनती; ++i)
	{
	    पूर्णाङ्क flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += गिनती;
	यदि (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += गिनती;
	प्रत्यागम;
    }

    प्रति (i = 0; i < गिनती; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    स्थिर शून्य
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    पूर्णाङ्क		didone;
    पूर्णाङ्क		i;
    char_u	*p;
    पूर्णाङ्क		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    प्रति (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, रिक्त, रिक्त, FALSE);
	यदि (p == रिक्त)
	    भङ्ग;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	यदि (regmatch.regprog == रिक्त)
	{
	    vim_free(p);
	    भङ्ग;
	}

	didone = FALSE;
	प्रति (match = 0; match < ARGCOUNT; ++match)
	    यदि (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * परिमाण(aentry_T));
		--ARGCOUNT;
		यदि (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	यदि (!didone)
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
    स्थिर पूर्णाङ्क
do_arglist(
    char_u	*str,
    पूर्णाङ्क		what,
    पूर्णाङ्क		after UNUSED,	// 0 means before first one
    पूर्णाङ्क		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    पूर्णाङ्क		exp_गिनती;
    char_u	**exp_files;
    पूर्णाङ्क		i;
    पूर्णाङ्क		arg_escaped = TRUE;

    यदि (check_arglist_locked() == FAIL)
	प्रत्यागम FAIL;

    // Set default argument for ":argadd" command.
    यदि (what == AL_ADD && *str == NUL)
    {
	यदि (curbuf->b_ffname == रिक्त)
	    प्रत्यागम FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    यदि (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	प्रत्यागम FAIL;

    यदि (what == AL_DEL)
	arglist_del_files(&new_ga);
    अन्यथा
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_गिनती, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	यदि (i == FAIL || exp_गिनती == 0)
	{
	    emsg(_(e_no_match));
	    प्रत्यागम FAIL;
	}

	यदि (what == AL_ADD)
	{
	    alist_add_list(exp_गिनती, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	अन्यथा // what == AL_SET
	    alist_set(ALIST(curwin), exp_गिनती, exp_files, will_edit, रिक्त, 0);
    }

    alist_check_arg_idx();

    प्रत्यागम OK;
}

/*
 * Redefine the argument list.
 */
    शून्य
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    पूर्णाङ्क
editing_arg_idx(win_T *win)
{
    प्रत्यागम !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == रिक्त
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    शून्य
check_arg_idx(win_T *win)
{
    यदि (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	यदि (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != रिक्त
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    अन्यथा
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	यदि (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    शून्य
ex_args(exarg_T *eap)
{
    पूर्णाङ्क		i;

    यदि (eap->cmdidx != CMD_args)
    {
	यदि (check_arglist_locked() == FAIL)
	    प्रत्यागम;
	alist_unlink(ALIST(curwin));
	यदि (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	अन्यथा // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    यदि (*eap->arg != NUL)
    {
	यदि (check_arglist_locked() == FAIL)
	    प्रत्यागम;
	ex_next(eap);
	प्रत्यागम;
    }

    // ":args": list arguments.
    यदि (eap->cmdidx == CMD_args)
    {
	char_u **वस्तुएँ;

	यदि (ARGCOUNT <= 0)
	    प्रत्यागम;		// empty argument list

	वस्तुएँ = ALLOC_MULT(char_u *, ARGCOUNT);
	यदि (वस्तुएँ == रिक्त)
	    प्रत्यागम;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	प्रति (i = 0; i < ARGCOUNT; ++i)
	    वस्तुएँ[i] = alist_name(&ARGLIST[i]);
	list_in_columns(वस्तुएँ, ARGCOUNT, curwin->w_arg_idx);
	vim_free(वस्तुएँ);

	प्रत्यागम;
    }

    // ":argslocal": make a local copy of the global argument list.
    यदि (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	यदि (GA_GROW_FAILS(gap, GARGCOUNT))
	    प्रत्यागम;

	प्रति (i = 0; i < GARGCOUNT; ++i)
	    यदि (GARGLIST[i].ae_fname != रिक्त)
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
    शून्य
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    यदि (curwin->w_arg_idx - (पूर्णाङ्क)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    अन्यथा
	do_argfile(eap, curwin->w_arg_idx - (पूर्णाङ्क)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    शून्य
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    शून्य
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    शून्य
ex_argument(exarg_T *eap)
{
    पूर्णाङ्क		i;

    यदि (eap->addr_गिनती > 0)
	i = eap->line2 - 1;
    अन्यथा
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    शून्य
do_argfile(exarg_T *eap, पूर्णाङ्क argn)
{
    पूर्णाङ्क		other;
    char_u	*p;
    पूर्णाङ्क		old_arg_idx = curwin->w_arg_idx;
    पूर्णाङ्क is_split_cmd = *eap->cmd == 's';

    यदि (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;
    यदि (argn < 0 || argn >= ARGCOUNT)
    {
	यदि (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	अन्यथा यदि (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	अन्यथा
	    emsg(_(e_cannot_go_beyond_last_file));

	प्रत्यागम;
    }

    यदि (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	प्रत्यागम;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    यदि (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	यदि (win_split(0, 0) == FAIL)
	    प्रत्यागम;
	RESET_BINDING(curwin);
    }
    अन्यथा
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	यदि (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	यदि ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    प्रत्यागम;
    }

    curwin->w_arg_idx = argn;
    यदि (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    यदि (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), रिक्त,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    अन्यथा यदि (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    शून्य
ex_next(exarg_T *eap)
{
    पूर्णाङ्क		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    यदि (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	यदि (*eap->arg != NUL)		    // redefine file list
	{
	    यदि (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		प्रत्यागम;
	    i = 0;
	}
	अन्यथा
	    i = curwin->w_arg_idx + (पूर्णाङ्क)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    शून्य
ex_argdedupe(exarg_T *eap UNUSED)
{
    पूर्णाङ्क i;
    पूर्णाङ्क j;

    प्रति (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	यदि (firstFullname == रिक्त)
	    प्रत्यागम;  // out of memory

	प्रति (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    यदि (secondFullname == रिक्त)
		भङ्ग;  // out of memory
	    पूर्णाङ्क areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    यदि (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * परिमाण(aentry_T));
		--ARGCOUNT;

		यदि (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		अन्यथा यदि (curwin->w_arg_idx > j)
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
    शून्य
ex_argedit(exarg_T *eap)
{
    पूर्णाङ्क i = eap->addr_गिनती ? (पूर्णाङ्क)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    पूर्णाङ्क curbuf_is_reusable = curbuf_reusable();

    यदि (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	प्रत्यागम;
    maketitle();

    यदि (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == रिक्त || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    यदि (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    शून्य
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_गिनती > 0 ? (पूर्णाङ्क)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    शून्य
ex_argdelete(exarg_T *eap)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		n;

    यदि (check_arglist_locked() == FAIL)
	प्रत्यागम;

    यदि (eap->addr_गिनती > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	यदि (eap->addr_गिनती == 0)
	{
	    यदि (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		प्रत्यागम;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	अन्यथा यदि (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	यदि (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	यदि (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    यदि (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    प्रत्यागम;
	}

	प्रति (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (माप_प्रकार)((ARGCOUNT - eap->line2) * परिमाण(aentry_T)));
	ARGCOUNT -= n;
	यदि (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	अन्यथा यदि (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    अन्यथा
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    यदि (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    अन्यथा यदि (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    यदि (idx >= ARGCOUNT)
	प्रत्यागम रिक्त;

    प्रत्यागम alist_name(&ARGLIST[idx]);
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
    यदि (bp == रिक्त || bp->b_fname == रिक्त)
	प्रत्यागम aep->ae_fname;
    प्रत्यागम bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
प्रकारनिर्देश संरचना {
    alist_T	*alist;		// argument list to be used
    पूर्णाङ्क		had_tab;
    पूर्णाङ्क		keep_tabs;
    पूर्णाङ्क		forceit;

    पूर्णाङ्क		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    पूर्णाङ्क		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    स्थिर शून्य
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    पूर्णाङ्क		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    यदि (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    प्रति (;;)
    {
	tpnext = curtab->tp_next;
	प्रति (wp = firstwin; wp != रिक्त; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    यदि (buf->b_ffname == रिक्त
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    अन्यथा
	    {
		// check if the buffer in this window is in the arglist
		प्रति (i = 0; i < aall->opened_len; ++i)
		{
		    यदि (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			पूर्णाङ्क weight = 1;

			यदि (old_curtab == curtab)
			{
			    ++weight;
			    यदि (old_curwin == wp)
				++weight;
			}

			यदि (weight > (पूर्णाङ्क)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    यदि (i == 0)
			    {
				यदि (aall->new_curwin != रिक्त)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			अन्यथा यदि (aall->keep_tabs)
			    i = aall->opened_len;

			यदि (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			भङ्ग;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    यदि (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		यदि (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    यदि (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(शून्य)autowrite(buf, FALSE);

			// check if autocommands removed the window
			यदि (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    अनुवर्त;
			}
		    }
		    // don't close last window
		    यदि (ONE_WINDOW
			    && (first_tabpage->tp_next == रिक्त
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    अन्यथा
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			यदि (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	यदि (aall->had_tab == 0 || tpnext == रिक्त)
	    भङ्ग;

	// check if autocommands removed the next tab page
	यदि (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    स्थिर शून्य
arg_all_open_windows(arg_all_state_T *aall, पूर्णाङ्क गिनती)
{
    win_T	*wp;
    पूर्णाङ्क		tab_drop_empty_window = FALSE;
    पूर्णाङ्क		i;
    पूर्णाङ्क		split_ret = OK;
    पूर्णाङ्क		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    यदि (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == रिक्त && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    प्रति (i = 0; i < गिनती && !got_int; ++i)
    {
	यदि (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	यदि (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    यदि (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    यदि (wp->w_arg_idx == i)
		    {
			यदि (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			अन्यथा यदि (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = गिनती;
			    भङ्ग;
			}
			अन्यथा
			    win_move_after(wp, curwin);
			भङ्ग;
		    }
		}
	    }
	}
	अन्यथा यदि (split_ret == OK)
	{
	    // trigger events for tab drop
	    यदि (tab_drop_empty_window && i == गिनती - 1)
		--autocmd_no_enter;
	    यदि (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		यदि (split_ret == FAIL)
		    अनुवर्त;
	    }
	    अन्यथा    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    यदि (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (शून्य)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), रिक्त, रिक्त,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    यदि (tab_drop_empty_window && i == गिनती - 1)
		++autocmd_no_enter;
	    यदि (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	यदि (aall->had_tab > 0 && tabpage_index(रिक्त) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    स्थिर शून्य
do_arg_all(
    पूर्णाङ्क	गिनती,
    पूर्णाङ्क	forceit,		// hide buffers in current windows
    पूर्णाङ्क keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    पूर्णाङ्क			prev_arglist_locked = arglist_locked;

    यदि (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	प्रत्यागम;
    }
    यदि (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	प्रत्यागम;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = रिक्त;
    aall.new_curtab = रिक्त;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    यदि (aall.opened == रिक्त)
	प्रत्यागम;

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
    यदि (गिनती > aall.opened_len || गिनती <= 0)
	गिनती = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, गिनती);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    यदि (last_curtab != aall.new_curtab)
    {
	यदि (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	यदि (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    यदि (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    यदि (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    यदि (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    शून्य
ex_all(exarg_T *eap)
{
    यदि (eap->addr_गिनती == 0)
	eap->line2 = 9999;
    do_arg_all((पूर्णाङ्क)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(शून्य)
{
    पूर्णाङ्क		len;
    पूर्णाङ्क		idx;
    char_u	*retval = रिक्त;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    प्रति (;;)
    {
	len = 0;
	प्रति (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    यदि (p == रिक्त)
		अनुवर्त;
	    यदि (len > 0)
	    {
		// insert a space in between names
		यदि (retval != रिक्त)
		    retval[len] = ' ';
		++len;
	    }
	    प्रति ( ; *p != NUL; ++p)
	    {
		यदि (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    यदि (retval != रिक्त)
			retval[len] = '\\';
		    ++len;
		}
		यदि (retval != रिक्त)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	यदि (retval != रिक्त)
	{
	    retval[len] = NUL;
	    भङ्ग;
	}

	// allocate memory
	retval = alloc(len + 1);
	यदि (retval == रिक्त)
	    भङ्ग;
    }

    प्रत्यागम retval;
}

#यदि defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    शून्य
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    यदि (in_vim9script() && check_for_opt_संख्या_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_संख्या = ARGCOUNT;
    अन्यथा यदि (argvars[0].v_type == VAR_संख्या
					   && tv_get_संख्या(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_संख्या = GARGCOUNT;
    अन्यथा
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	यदि (wp != रिक्त)
	    rettv->vval.v_संख्या = WARGCOUNT(wp);
	अन्यथा
	    rettv->vval.v_संख्या = -1;
    }
}

/*
 * "argidx()" function
 */
    शून्य
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_संख्या = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    शून्य
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    यदि (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    rettv->vval.v_संख्या = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], रिक्त);
    यदि (wp != रिक्त)
	rettv->vval.v_संख्या = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    स्थिर शून्य
get_arglist_as_rettv(aentry_T *arglist, पूर्णाङ्क argcount, typval_T *rettv)
{
    पूर्णाङ्क		idx;

    यदि (rettv_list_alloc(rettv) == OK && arglist != रिक्त)
	प्रति (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    शून्य
f_argv(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		idx;
    aentry_T	*arglist = रिक्त;
    पूर्णाङ्क		argcount = -1;

    यदि (in_vim9script()
	    && (check_for_opt_संख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	प्रत्यागम;
    }

    यदि (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    अन्यथा यदि (argvars[1].v_type == VAR_संख्या
	    && tv_get_संख्या(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    अन्यथा
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	यदि (wp != रिक्त)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    idx = tv_get_संख्या_chk(&argvars[0], रिक्त);
    यदि (arglist != रिक्त && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    अन्यथा यदि (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
