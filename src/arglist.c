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

#ಅನ್ತರ್ಭಾವ "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ arglist_locked = FALSE;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_arglist_locked(ಶೂನ್ಯ)
{
    ಯದಿ (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    ಶೂನ್ಯ
alist_clear(alist_T *al)
{
    ಯದಿ (check_arglist_locked() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    ಶೂನ್ಯ
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, ಪರಿಮಾಣ(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    ಶೂನ್ಯ
alist_unlink(alist_T *al)
{
    ಯದಿ (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    ಶೂನ್ಯ
alist_new(ಶೂನ್ಯ)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    ಯದಿ (curwin->w_alist == ರಿಕ್ತ)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    ಅನ್ಯಥಾ
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#ಯದಿ !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    ಶೂನ್ಯ
alist_expand(ಪೂರ್ಣಾಙ್ಕ *fnum_list, ಪೂರ್ಣಾಙ್ಕ fnum_len)
{
    char_u	**old_arg_files;
    ಪೂರ್ಣಾಙ್ಕ		old_arg_ಎಣಿಕೆ;
    char_u	**new_arg_files;
    ಪೂರ್ಣಾಙ್ಕ		new_arg_file_ಎಣಿಕೆ;
    char_u	*save_p_su = p_su;
    ಪೂರ್ಣಾಙ್ಕ		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    ಯದಿ (old_arg_files == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    ಪ್ರತಿ (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_ಎಣಿಕೆ = GARGCOUNT;
    ಯದಿ (expand_wildcards(old_arg_ಎಣಿಕೆ, old_arg_files,
		&new_arg_file_ಎಣಿಕೆ, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_ಎಣಿಕೆ > 0)
    {
	alist_set(&global_alist, new_arg_file_ಎಣಿಕೆ, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_ಎಣಿಕೆ, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    ಶೂನ್ಯ
alist_set(
    alist_T	*al,
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    char_u	**files,
    ಪೂರ್ಣಾಙ್ಕ		use_curbuf,
    ಪೂರ್ಣಾಙ್ಕ		*fnum_list,
    ಪೂರ್ಣಾಙ್ಕ		fnum_len)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (check_arglist_locked() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    alist_clear(al);
    ಯದಿ (GA_GROW_OK(&al->al_ga, ಎಣಿಕೆ))
    {
	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	{
	    ಯದಿ (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		ಯಾವತ್ (i < ಎಣಿಕೆ)
		    vim_free(files[i++]);
		ಭಙ್ಗ;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    ಯದಿ (fnum_list != ರಿಕ್ತ && i < fnum_len)
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
    ಅನ್ಯಥಾ
	FreeWild(ಎಣಿಕೆ, files);
    ಯದಿ (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    ಶೂನ್ಯ
alist_add(
    alist_T	*al,
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    ಯದಿ (fname == ರಿಕ್ತ)		// don't add NULL file names
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_arglist_locked() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    ಯದಿ (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#ಯದಿ defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ಶೂನ್ಯ
alist_slash_adjust(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ಪ್ರತಿ (i = 0; i < GARGCOUNT; ++i)
	ಯದಿ (GARGLIST[i].ae_fname != ರಿಕ್ತ)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_alist != &global_alist)
	    ಪ್ರತಿ (i = 0; i < WARGCOUNT(wp); ++i)
		ಯದಿ (WARGLIST(wp)[i].ae_fname != ರಿಕ್ತ)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ಸ್ಥಿರ char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		inbacktick;

    inbacktick = FALSE;
    ಪ್ರತಿ (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	ಯದಿ (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	ಅನ್ಯಥಾ
	{
	    // An item ends at a space not in backticks
	    ಯದಿ (!inbacktick && vim_isspace(*str))
		ಭಙ್ಗ;
	    ಯದಿ (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    ಪ್ರತ್ಯಾಗಮ str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_arglist(garray_T *gap, char_u *str, ಪೂರ್ಣಾಙ್ಕ escaped)
{
    ga_init2(gap, ಪರಿಮಾಣ(char_u *), 20);
    ಯಾವತ್ (*str != NUL)
    {
	ಯದಿ (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	ಯದಿ (!escaped)
	    ಪ್ರತ್ಯಾಗಮ OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_arglist_exp(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		*fcountp,
    char_u	***fnamesp,
    ಪೂರ್ಣಾಙ್ಕ		wig)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (get_arglist(&ga, str, TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    ಅನ್ಯಥಾ
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alist_check_arg_idx(ಶೂನ್ಯ)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ಯದಿ (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alist_add_list(
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    char_u	**files,
    ಪೂರ್ಣಾಙ್ಕ		after,	    // where to add: 0 = before first one
    ಪೂರ್ಣಾಙ್ಕ		will_edit)  // will edit adding argument
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		old_argcount = ARGCOUNT;

    ಯದಿ (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, ಎಣಿಕೆ))
    {
	win_T	*wp = curwin;

	ಯದಿ (after < 0)
	    after = 0;
	ಯದಿ (after > ARGCOUNT)
	    after = ARGCOUNT;
	ಯದಿ (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + ಎಣಿಕೆ]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * ಪರಿಮಾಣ(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	{
	    ಪೂರ್ಣಾಙ್ಕ flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += ಎಣಿಕೆ;
	ಯದಿ (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += ಎಣಿಕೆ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		didone;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ಪ್ರತಿ (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	{
	    vim_free(p);
	    ಭಙ್ಗ;
	}

	didone = FALSE;
	ಪ್ರತಿ (match = 0; match < ARGCOUNT; ++match)
	    ಯದಿ (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * ಪರಿಮಾಣ(aentry_T));
		--ARGCOUNT;
		ಯದಿ (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	ಯದಿ (!didone)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_arglist(
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		what,
    ಪೂರ್ಣಾಙ್ಕ		after UNUSED,	// 0 means before first one
    ಪೂರ್ಣಾಙ್ಕ		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    ಪೂರ್ಣಾಙ್ಕ		exp_ಎಣಿಕೆ;
    char_u	**exp_files;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		arg_escaped = TRUE;

    ಯದಿ (check_arglist_locked() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Set default argument for ":argadd" command.
    ಯದಿ (what == AL_ADD && *str == NUL)
    {
	ಯದಿ (curbuf->b_ffname == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    ಯದಿ (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (what == AL_DEL)
	arglist_del_files(&new_ga);
    ಅನ್ಯಥಾ
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_ಎಣಿಕೆ, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	ಯದಿ (i == FAIL || exp_ಎಣಿಕೆ == 0)
	{
	    emsg(_(e_no_match));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (what == AL_ADD)
	{
	    alist_add_list(exp_ಎಣಿಕೆ, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	ಅನ್ಯಥಾ // what == AL_SET
	    alist_set(ALIST(curwin), exp_ಎಣಿಕೆ, exp_files, will_edit, ರಿಕ್ತ, 0);
    }

    alist_check_arg_idx();

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Redefine the argument list.
 */
    ಶೂನ್ಯ
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    ಪೂರ್ಣಾಙ್ಕ
editing_arg_idx(win_T *win)
{
    ಪ್ರತ್ಯಾಗಮ !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == ರಿಕ್ತ
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    ಶೂನ್ಯ
check_arg_idx(win_T *win)
{
    ಯದಿ (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	ಯದಿ (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != ರಿಕ್ತ
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	ಯದಿ (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    ಶೂನ್ಯ
ex_args(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (eap->cmdidx != CMD_args)
    {
	ಯದಿ (check_arglist_locked() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	alist_unlink(ALIST(curwin));
	ಯದಿ (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	ಅನ್ಯಥಾ // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    ಯದಿ (*eap->arg != NUL)
    {
	ಯದಿ (check_arglist_locked() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ex_next(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // ":args": list arguments.
    ಯದಿ (eap->cmdidx == CMD_args)
    {
	char_u **ವಸ್ತುಗಳು;

	ಯದಿ (ARGCOUNT <= 0)
	    ಪ್ರತ್ಯಾಗಮ;		// empty argument list

	ವಸ್ತುಗಳು = ALLOC_MULT(char_u *, ARGCOUNT);
	ಯದಿ (ವಸ್ತುಗಳು == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	ಪ್ರತಿ (i = 0; i < ARGCOUNT; ++i)
	    ವಸ್ತುಗಳು[i] = alist_name(&ARGLIST[i]);
	list_in_columns(ವಸ್ತುಗಳು, ARGCOUNT, curwin->w_arg_idx);
	vim_free(ವಸ್ತುಗಳು);

	ಪ್ರತ್ಯಾಗಮ;
    }

    // ":argslocal": make a local copy of the global argument list.
    ಯದಿ (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	ಯದಿ (GA_GROW_FAILS(gap, GARGCOUNT))
	    ಪ್ರತ್ಯಾಗಮ;

	ಪ್ರತಿ (i = 0; i < GARGCOUNT; ++i)
	    ಯದಿ (GARGLIST[i].ae_fname != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    ಯದಿ (curwin->w_arg_idx - (ಪೂರ್ಣಾಙ್ಕ)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    ಅನ್ಯಥಾ
	do_argfile(eap, curwin->w_arg_idx - (ಪೂರ್ಣಾಙ್ಕ)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    ಶೂನ್ಯ
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    ಶೂನ್ಯ
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    ಶೂನ್ಯ
ex_argument(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	i = eap->line2 - 1;
    ಅನ್ಯಥಾ
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    ಶೂನ್ಯ
do_argfile(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ argn)
{
    ಪೂರ್ಣಾಙ್ಕ		other;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		old_arg_idx = curwin->w_arg_idx;
    ಪೂರ್ಣಾಙ್ಕ is_split_cmd = *eap->cmd == 's';

    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argn < 0 || argn >= ARGCOUNT)
    {
	ಯದಿ (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	ಅನ್ಯಥಾ ಯದಿ (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	ಅನ್ಯಥಾ
	    emsg(_(e_cannot_go_beyond_last_file));

	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	ಪ್ರತ್ಯಾಗಮ;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    ಯದಿ (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	ಯದಿ (win_split(0, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	RESET_BINDING(curwin);
    }
    ಅನ್ಯಥಾ
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	ಯದಿ (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	ಯದಿ ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    ಪ್ರತ್ಯಾಗಮ;
    }

    curwin->w_arg_idx = argn;
    ಯದಿ (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    ಯದಿ (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), ರಿಕ್ತ,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    ಶೂನ್ಯ
ex_next(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    ಯದಿ (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	ಯದಿ (*eap->arg != NUL)		    // redefine file list
	{
	    ಯದಿ (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	    i = 0;
	}
	ಅನ್ಯಥಾ
	    i = curwin->w_arg_idx + (ಪೂರ್ಣಾಙ್ಕ)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    ಶೂನ್ಯ
ex_argdedupe(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ j;

    ಪ್ರತಿ (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	ಯದಿ (firstFullname == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;  // out of memory

	ಪ್ರತಿ (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    ಯದಿ (secondFullname == ರಿಕ್ತ)
		ಭಙ್ಗ;  // out of memory
	    ಪೂರ್ಣಾಙ್ಕ areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    ಯದಿ (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * ಪರಿಮಾಣ(aentry_T));
		--ARGCOUNT;

		ಯದಿ (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		ಅನ್ಯಥಾ ಯದಿ (curwin->w_arg_idx > j)
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
    ಶೂನ್ಯ
ex_argedit(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ i = eap->addr_ಎಣಿಕೆ ? (ಪೂರ್ಣಾಙ್ಕ)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    ಪೂರ್ಣಾಙ್ಕ curbuf_is_reusable = curbuf_reusable();

    ಯದಿ (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    maketitle();

    ಯದಿ (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == ರಿಕ್ತ || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    ಯದಿ (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    ಶೂನ್ಯ
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಙ್ಕ)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    ಶೂನ್ಯ
ex_argdelete(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (check_arglist_locked() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
	{
	    ಯದಿ (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	ಯದಿ (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    ಯದಿ (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * ಪರಿಮಾಣ(aentry_T)));
	ARGCOUNT -= n;
	ಯದಿ (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    ಅನ್ಯಥಾ
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    ಯದಿ (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    ಅನ್ಯಥಾ ಯದಿ (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= ARGCOUNT)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ alist_name(&ARGLIST[idx]);
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
    ಯದಿ (bp == ರಿಕ್ತ || bp->b_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ aep->ae_fname;
    ಪ್ರತ್ಯಾಗಮ bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    alist_T	*alist;		// argument list to be used
    ಪೂರ್ಣಾಙ್ಕ		had_tab;
    ಪೂರ್ಣಾಙ್ಕ		keep_tabs;
    ಪೂರ್ಣಾಙ್ಕ		forceit;

    ಪೂರ್ಣಾಙ್ಕ		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    ಪೂರ್ಣಾಙ್ಕ		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    ಯದಿ (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    ಪ್ರತಿ (;;)
    {
	tpnext = curtab->tp_next;
	ಪ್ರತಿ (wp = firstwin; wp != ರಿಕ್ತ; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    ಯದಿ (buf->b_ffname == ರಿಕ್ತ
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    ಅನ್ಯಥಾ
	    {
		// check if the buffer in this window is in the arglist
		ಪ್ರತಿ (i = 0; i < aall->opened_len; ++i)
		{
		    ಯದಿ (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			ಪೂರ್ಣಾಙ್ಕ weight = 1;

			ಯದಿ (old_curtab == curtab)
			{
			    ++weight;
			    ಯದಿ (old_curwin == wp)
				++weight;
			}

			ಯದಿ (weight > (ಪೂರ್ಣಾಙ್ಕ)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    ಯದಿ (i == 0)
			    {
				ಯದಿ (aall->new_curwin != ರಿಕ್ತ)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ (aall->keep_tabs)
			    i = aall->opened_len;

			ಯದಿ (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			ಭಙ್ಗ;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    ಯದಿ (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		ಯದಿ (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    ಯದಿ (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(ಶೂನ್ಯ)autowrite(buf, FALSE);

			// check if autocommands removed the window
			ಯದಿ (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    ಅನುವರ್ತ;
			}
		    }
		    // don't close last window
		    ಯದಿ (ONE_WINDOW
			    && (first_tabpage->tp_next == ರಿಕ್ತ
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    ಅನ್ಯಥಾ
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			ಯದಿ (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	ಯದಿ (aall->had_tab == 0 || tpnext == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// check if autocommands removed the next tab page
	ಯದಿ (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arg_all_open_windows(arg_all_state_T *aall, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		tab_drop_empty_window = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		split_ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    ಯದಿ (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == ರಿಕ್ತ && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ && !got_int; ++i)
    {
	ಯದಿ (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	ಯದಿ (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    ಯದಿ (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    ಯದಿ (wp->w_arg_idx == i)
		    {
			ಯದಿ (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			ಅನ್ಯಥಾ ಯದಿ (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = ಎಣಿಕೆ;
			    ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ
			    win_move_after(wp, curwin);
			ಭಙ್ಗ;
		    }
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (split_ret == OK)
	{
	    // trigger events for tab drop
	    ಯದಿ (tab_drop_empty_window && i == ಎಣಿಕೆ - 1)
		--autocmd_no_enter;
	    ಯದಿ (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		ಯದಿ (split_ret == FAIL)
		    ಅನುವರ್ತ;
	    }
	    ಅನ್ಯಥಾ    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    ಯದಿ (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (ಶೂನ್ಯ)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), ರಿಕ್ತ, ರಿಕ್ತ,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    ಯದಿ (tab_drop_empty_window && i == ಎಣಿಕೆ - 1)
		++autocmd_no_enter;
	    ಯದಿ (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	ಯದಿ (aall->had_tab > 0 && tabpage_index(ರಿಕ್ತ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_arg_all(
    ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ	forceit,		// hide buffers in current windows
    ಪೂರ್ಣಾಙ್ಕ keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    ಪೂರ್ಣಾಙ್ಕ			prev_arglist_locked = arglist_locked;

    ಯದಿ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	ಪ್ರತ್ಯಾಗಮ;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = ರಿಕ್ತ;
    aall.new_curtab = ರಿಕ್ತ;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    ಯದಿ (aall.opened == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಯದಿ (ಎಣಿಕೆ > aall.opened_len || ಎಣಿಕೆ <= 0)
	ಎಣಿಕೆ = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, ಎಣಿಕೆ);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    ಯದಿ (last_curtab != aall.new_curtab)
    {
	ಯದಿ (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	ಯದಿ (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    ಯದಿ (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    ಯದಿ (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    ಯದಿ (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    ಶೂನ್ಯ
ex_all(exarg_T *eap)
{
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
	eap->line2 = 9999;
    do_arg_all((ಪೂರ್ಣಾಙ್ಕ)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*retval = ರಿಕ್ತ;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    ಪ್ರತಿ (;;)
    {
	len = 0;
	ಪ್ರತಿ (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    ಯದಿ (p == ರಿಕ್ತ)
		ಅನುವರ್ತ;
	    ಯದಿ (len > 0)
	    {
		// insert a space in between names
		ಯದಿ (retval != ರಿಕ್ತ)
		    retval[len] = ' ';
		++len;
	    }
	    ಪ್ರತಿ ( ; *p != NUL; ++p)
	    {
		ಯದಿ (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    ಯದಿ (retval != ರಿಕ್ತ)
			retval[len] = '\\';
		    ++len;
		}
		ಯದಿ (retval != ರಿಕ್ತ)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	ಯದಿ (retval != ರಿಕ್ತ)
	{
	    retval[len] = NUL;
	    ಭಙ್ಗ;
	}

	// allocate memory
	retval = alloc(len + 1);
	ಯದಿ (retval == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    ಶೂನ್ಯ
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_ಸಂಖ್ಯೆ = ARGCOUNT;
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
					   && tv_get_ಸಂಖ್ಯೆ(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_ಸಂಖ್ಯೆ = GARGCOUNT;
    ಅನ್ಯಥಾ
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	ಯದಿ (wp != ರಿಕ್ತ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = WARGCOUNT(wp);
	ಅನ್ಯಥಾ
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    }
}

/*
 * "argidx()" function
 */
    ಶೂನ್ಯ
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    ಶೂನ್ಯ
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], ರಿಕ್ತ);
    ಯದಿ (wp != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_arglist_as_rettv(aentry_T *arglist, ಪೂರ್ಣಾಙ್ಕ argcount, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಯದಿ (rettv_list_alloc(rettv) == OK && arglist != ರಿಕ್ತ)
	ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    ಶೂನ್ಯ
f_argv(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    aentry_T	*arglist = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argcount = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ
	    && tv_get_ಸಂಖ್ಯೆ(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    ಅನ್ಯಥಾ
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
    idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ);
    ಯದಿ (arglist != ರಿಕ್ತ && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    ಅನ್ಯಥಾ ಯದಿ (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
