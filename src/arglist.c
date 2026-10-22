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

#ವ್ಯಾಖ್ಯಾನಿಸು AL_ಗಣ	1
#ವ್ಯಾಖ್ಯಾನಿಸು AL_ಕೂಡಿಸು	2
#ವ್ಯಾಖ್ಯಾನಿಸು AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ arglist_locked = FALSE;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_arglist_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    ಶೂನ್ಯ
alist_clear(alist_T *al)
{
    ಆದರೆ (check_arglist_locked() == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆಗ (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    ಶೂನ್ಯ
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, ಗಾತ್ರ(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    ಶೂನ್ಯ
alist_unlink(alist_T *al)
{
    ಆದರೆ (al != &global_alist && --al->al_refcount <= 0)
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
    ಆದರೆ (curwin->w_alist == ಖಾಲಿ)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#ಆದರೆ !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    ಶೂನ್ಯ
alist_expand(ಪೂರ್ಣಾಂಕ *fnum_list, ಪೂರ್ಣಾಂಕ fnum_len)
{
    char_u	**old_arg_files;
    ಪೂರ್ಣಾಂಕ		old_arg_ಎಣಿಕೆ;
    char_u	**new_arg_files;
    ಪೂರ್ಣಾಂಕ		new_arg_file_ಎಣಿಕೆ;
    char_u	*save_p_su = p_su;
    ಪೂರ್ಣಾಂಕ		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    ಆದರೆ (old_arg_files == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = ಖಾಲಿಗಳು_option;
    ಗಾಗಿ (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_ಎಣಿಕೆ = GARGCOUNT;
    ಆದರೆ (expand_wildcards(old_arg_ಎಣಿಕೆ, old_arg_files,
		&new_arg_file_ಎಣಿಕೆ, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_ಎಣಿಕೆ > 0)
    {
	alist_ಗಣ(&global_alist, new_arg_file_ಎಣಿಕೆ, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_ಎಣಿಕೆ, old_arg_files);
    }
    p_su = save_p_su;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    ಶೂನ್ಯ
alist_ಗಣ(
    alist_T	*al,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    char_u	**files,
    ಪೂರ್ಣಾಂಕ		use_curbuf,
    ಪೂರ್ಣಾಂಕ		*fnum_list,
    ಪೂರ್ಣಾಂಕ		fnum_len)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (check_arglist_locked() == FAIL)
	ಹಿಂತಿರುಗಿ;

    alist_clear(al);
    ಆದರೆ (GA_GROW_OK(&al->al_ga, ಎಣಿಕೆ_2))
    {
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	{
	    ಆದರೆ (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		ಆಗ (i < ಎಣಿಕೆ_2)
		    vim_free(files[i++]);
		ನಿಲ್ಲಿಸು;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    ಆದರೆ (fnum_list != ಖಾಲಿ && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_ಗಣ_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_ಕೂಡಿಸು(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	FreeWild(ಎಣಿಕೆ_2, files);
    ಆದರೆ (al == &global_alist)
	arg_had_ಕೊನೆಯ = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    ಶೂನ್ಯ
alist_ಕೂಡಿಸು(
    alist_T	*al,
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		ಗಣ_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    ಆದರೆ (fname == ಖಾಲಿ)		// don't add NULL file names
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_arglist_locked() == FAIL)
	ಹಿಂತಿರುಗಿ;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    slash_adjust(fname);
#ಆದರೆ_ಅಂತ್ಯ
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    ಆದರೆ (ಗಣ_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_ಕೂಡಿಸು(fname, BLN_LISTED | (ಗಣ_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ಶೂನ್ಯ
alist_slash_adjust(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;
    tabpage_T	*tp;

    ಗಾಗಿ (i = 0; i < GARGCOUNT; ++i)
	ಆದರೆ (GARGLIST[i].ae_fname != ಖಾಲಿ)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_alist != &global_alist)
	    ಗಾಗಿ (i = 0; i < WARGCOUNT(wp); ++i)
		ಆದರೆ (WARGLIST(wp)[i].ae_fname != ಖಾಲಿ)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    ಸ್ಥಿರ char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		inbacktick;

    inbacktick = FALSE;
    ಗಾಗಿ (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	ಆದರೆ (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // An item ends at a space not in backticks
	    ಆದರೆ (!inbacktick && vim_isspace(*str))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    ಹಿಂತಿರುಗಿ str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_arglist(garray_T *gap, char_u *str, ಪೂರ್ಣಾಂಕ escaped)
{
    ga_init2(gap, ಗಾತ್ರ(char_u *), 20);
    ಆಗ (*str != NUL)
    {
	ಆದರೆ (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	ಆದರೆ (!escaped)
	    ಹಿಂತಿರುಗಿ OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
get_arglist_exp(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		*fcountp,
    char_u	***fnamesp,
    ಪೂರ್ಣಾಂಕ		wig)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (get_arglist(&ga, str, TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_ದತ್ತಾಂಶ,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    ಇಲ್ಲದಿದ್ದರೆ
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_ದತ್ತಾಂಶ,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ i;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check the validity of the arg_idx for each other window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alist_check_arg_idx(ಶೂನ್ಯ)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	ಆದರೆ (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alist_ಕೂಡಿಸು_list(
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    char_u	**files,
    ಪೂರ್ಣಾಂಕ		after,	    // where to add: 0 = before first one
    ಪೂರ್ಣಾಂಕ		will_edit)  // will edit adding argument
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		old_argcount = ARGCOUNT;

    ಆದರೆ (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, ಎಣಿಕೆ_2))
    {
	win_T	*wp = curwin;

	ಆದರೆ (after < 0)
	    after = 0;
	ಆದರೆ (after > ARGCOUNT)
	    after = ARGCOUNT;
	ಆದರೆ (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + ಎಣಿಕೆ_2]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * ಗಾತ್ರ(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	{
	    ಪೂರ್ಣಾಂಕ flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_ಕೂಡಿಸು(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += ಎಣಿಕೆ_2;
	ಆದರೆ (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += ಎಣಿಕೆ_2;
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		didone;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    ಗಾಗಿ (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_ದತ್ತಾಂಶ)[i];
	p = file_pat_ಗಮ್ಯ_reg_pat(p, ಖಾಲಿ, ಖಾಲಿ, FALSE);
	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	{
	    vim_free(p);
	    ನಿಲ್ಲಿಸು;
	}

	didone = FALSE;
	ಗಾಗಿ (ಹೊಂದಿಕೆ = 0; ಹೊಂದಿಕೆ < ARGCOUNT; ++ಹೊಂದಿಕೆ)
	    ಆದರೆ (vim_regexec(&regmatch, alist_name(&ARGLIST[ಹೊಂದಿಕೆ]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[ಹೊಂದಿಕೆ].ae_fname);
		mch_memmove(ARGLIST + ಹೊಂದಿಕೆ, ARGLIST + ಹೊಂದಿಕೆ + 1,
			(ARGCOUNT - ಹೊಂದಿಕೆ - 1) * ಗಾತ್ರ(aentry_T));
		--ARGCOUNT;
		ಆದರೆ (curwin->w_arg_idx > ಹೊಂದಿಕೆ)
		    --curwin->w_arg_idx;
		--ಹೊಂದಿಕೆ;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	ಆದರೆ (!didone)
	    semsg(_(e_no_ಹೊಂದಿಕೆ_str_2), ((char_u **)alist_ga->ga_ದತ್ತಾಂಶ)[i]);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_arglist(
    char_u	*str,
    ಪೂರ್ಣಾಂಕ		what,
    ಪೂರ್ಣಾಂಕ		after ಬಳಸದ,	// 0 means before first one
    ಪೂರ್ಣಾಂಕ		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    ಪೂರ್ಣಾಂಕ		exp_ಎಣಿಕೆ;
    char_u	**exp_files;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		arg_escaped = TRUE;

    ಆದರೆ (check_arglist_locked() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Set default argument for ":argadd" command.
    ಆದರೆ (what == AL_ಕೂಡಿಸು && *str == NUL)
    {
	ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    ಆದರೆ (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (what == AL_DEL)
	arglist_del_files(&new_ga);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_ದತ್ತಾಂಶ,
		&exp_ಎಣಿಕೆ, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	ಆದರೆ (i == FAIL || exp_ಎಣಿಕೆ == 0)
	{
	    emsg(_(e_no_ಹೊಂದಿಕೆ));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (what == AL_ಕೂಡಿಸು)
	{
	    alist_ಕೂಡಿಸು_list(exp_ಎಣಿಕೆ, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	ಇಲ್ಲದಿದ್ದರೆ // what == AL_SET
	    alist_ಗಣ(ALIST(curwin), exp_ಎಣಿಕೆ, exp_files, will_edit, ಖಾಲಿ, 0);
    }

    alist_check_arg_idx();

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Redefine the argument list.
 */
    ಶೂನ್ಯ
ಗಣ_arglist(char_u *str)
{
    do_arglist(str, AL_ಗಣ, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    ಪೂರ್ಣಾಂಕ
editing_arg_idx(win_T *win)
{
    ಹಿಂತಿರುಗಿ !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_ಬಫರ್->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_ಬಫರ್->b_ffname == ಖಾಲಿ
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_ಬಫರ್->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    ಶೂನ್ಯ
check_arg_idx(win_T *win)
{
    ಆದರೆ (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	ಆದರೆ (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_ಕೊನೆಯ == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_ಬಫರ್->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_ಬಫರ್->b_ffname != ಖಾಲಿ
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_ಬಫರ್->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_ಕೊನೆಯ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	ಆದರೆ (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_ಕೊನೆಯ = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    ಶೂನ್ಯ
ex_ವಾದಗಳು(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (eap->cmdidx != CMD_ವಾದಗಳು)
    {
	ಆದರೆ (check_arglist_locked() == FAIL)
	    ಹಿಂತಿರುಗಿ;
	alist_unlink(ALIST(curwin));
	ಆದರೆ (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	ಇಲ್ಲದಿದ್ದರೆ // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    ಆದರೆ (*eap->arg != NUL)
    {
	ಆದರೆ (check_arglist_locked() == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ex_ಮುಂದಿನ(eap);
	ಹಿಂತಿರುಗಿ;
    }

    // ":args": list arguments.
    ಆದರೆ (eap->cmdidx == CMD_ವಾದಗಳು)
    {
	char_u **ವಸ್ತುಗಳು;

	ಆದರೆ (ARGCOUNT <= 0)
	    ಹಿಂತಿರುಗಿ;		// empty argument list

	ವಸ್ತುಗಳು = ALLOC_MULT(char_u *, ARGCOUNT);
	ಆದರೆ (ವಸ್ತುಗಳು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	ಗಾಗಿ (i = 0; i < ARGCOUNT; ++i)
	    ವಸ್ತುಗಳು[i] = alist_name(&ARGLIST[i]);
	list_ಒಳ_columns(ವಸ್ತುಗಳು, ARGCOUNT, curwin->w_arg_idx);
	vim_free(ವಸ್ತುಗಳು);

	ಹಿಂತಿರುಗಿ;
    }

    // ":argslocal": make a local copy of the global argument list.
    ಆದರೆ (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	ಆದರೆ (GA_GROW_FAILS(gap, GARGCOUNT))
	    ಹಿಂತಿರುಗಿ;

	ಗಾಗಿ (i = 0; i < GARGCOUNT; ++i)
	    ಆದರೆ (GARGLIST[i].ae_fname != ಖಾಲಿ)
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
ex_ಹಿಂದಿನ(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    ಆದರೆ (curwin->w_arg_idx - (ಪೂರ್ಣಾಂಕ)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    ಇಲ್ಲದಿದ್ದರೆ
	do_argfile(eap, curwin->w_arg_idx - (ಪೂರ್ಣಾಂಕ)eap->line2);
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
ex_ಕೊನೆಯ(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    ಶೂನ್ಯ
ex_ವಾದ(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	i = eap->line2 - 1;
    ಇಲ್ಲದಿದ್ದರೆ
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    ಶೂನ್ಯ
do_argfile(exarg_T *eap, ಪೂರ್ಣಾಂಕ argn)
{
    ಪೂರ್ಣಾಂಕ		other;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		old_arg_idx = curwin->w_arg_idx;
    ಪೂರ್ಣಾಂಕ is_ವಿಭಜಿಸು_cmd = *eap->cmd == 's';

    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argn < 0 || argn >= ARGCOUNT)
    {
	ಆದರೆ (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_ಗಮ್ಯ_edit));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argn < 0)
	    emsg(_(e_cannot_go_before_ಮೊದಲ_file));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_cannot_go_beyond_ಕೊನೆಯ_file));

	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!is_ವಿಭಜಿಸು_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_ಗಣ_curbuf_forceit(eap->forceit))
	ಹಿಂತಿರುಗಿ;

    setpcmark();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // split window or create new tab page first
    ಆದರೆ (is_ವಿಭಜಿಸು_cmd || cmdmod.cmod_tab != 0)
    {
	ಆದರೆ (win_ವಿಭಜಿಸು(0, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	RESET_BINDING(curwin);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	ಆದರೆ (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	ಆದರೆ ((!buf_hide(curbuf) || !other)
		&& check_ಬದಲಾಯಿತು(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    ಹಿಂತಿರುಗಿ;
    }

    curwin->w_arg_idx = argn;
    ಆದರೆ (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_ಕೊನೆಯ = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    ಆದರೆ (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), ಖಾಲಿ,
		eap, ECMD_ಕೊನೆಯ,
		(buf_hide(curwin->w_ಬಫರ್) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    ಶೂನ್ಯ
ex_ಮುಂದಿನ(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    ಆದರೆ (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_ಬದಲಾಯಿತು(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	ಆದರೆ (*eap->arg != NUL)		    // redefine file list
	{
	    ಆದರೆ (do_arglist(eap->arg, AL_ಗಣ, 0, TRUE) == FAIL)
		ಹಿಂತಿರುಗಿ;
	    i = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    i = curwin->w_arg_idx + (ಪೂರ್ಣಾಂಕ)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    ಶೂನ್ಯ
ex_argdedupe(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ j;

    ಗಾಗಿ (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *ಮೊದಲ_Fullname = ತುಂಬಿದ_Name_save(ARGLIST[i].ae_fname, FALSE);
	ಆದರೆ (ಮೊದಲ_Fullname == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;  // out of memory

	ಗಾಗಿ (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *ಎರಡನೇ_Fullname = ತುಂಬಿದ_Name_save(ARGLIST[j].ae_fname, FALSE);
	    ಆದರೆ (ಎರಡನೇ_Fullname == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;  // out of memory
	    ಪೂರ್ಣಾಂಕ are_ಹೆಸರುಗಳು_Duplicate =
				  fnamecmp(ಮೊದಲ_Fullname, ಎರಡನೇ_Fullname) == 0;
	    vim_free(ಎರಡನೇ_Fullname);

	    ಆದರೆ (are_ಹೆಸರುಗಳು_Duplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * ಗಾತ್ರ(aentry_T));
		--ARGCOUNT;

		ಆದರೆ (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_arg_idx > j)
		    --curwin->w_arg_idx;

		--j;
	    }
	}

	vim_free(ಮೊದಲ_Fullname);
    }
}

/*
 * ":argedit"
 */
    ಶೂನ್ಯ
ex_argedit(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ i = eap->addr_ಎಣಿಕೆ ? (ಪೂರ್ಣಾಂಕ)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    ಪೂರ್ಣಾಂಕ curbuf_is_reusable = curbuf_reusable();

    ಆದರೆ (do_arglist(eap->arg, AL_ಕೂಡಿಸು, i, TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ;
    maketitle();

    ಆದರೆ (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	    && (curbuf->b_ffname == ಖಾಲಿ || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    ಆದರೆ (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    ಶೂನ್ಯ
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ಕೂಡಿಸು,
	       eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಂಕ)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    ಶೂನ್ಯ
ex_argdelete(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (check_arglist_locked() == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
	{
	    ಆದರೆ (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_ವಾದ_ಗಮ್ಯ_delete));
		ಹಿಂತಿರುಗಿ;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	ಆದರೆ (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    ಆದರೆ (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * ಗಾತ್ರ(aentry_T)));
	ARGCOUNT -= n;
	ಆದರೆ (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    ಆದರೆ (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx >= ARGCOUNT)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ alist_name(&ARGLIST[idx]);
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
    ಆದರೆ (bp == ಖಾಲಿ || bp->b_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ aep->ae_fname;
    ಹಿಂತಿರುಗಿ bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    alist_T	*alist;		// argument list to be used
    ಪೂರ್ಣಾಂಕ		had_tab;
    ಪೂರ್ಣಾಂಕ		keep_tabs;
    ಪೂರ್ಣಾಂಕ		forceit;

    ಪೂರ್ಣಾಂಕ		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    ಪೂರ್ಣಾಂಕ		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arg_all_close_ಬಳಸದ_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    ಆದರೆ (aall->had_tab > 0)
	goto_tabpage_tp(ಮೊದಲ_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    ಗಾಗಿ (;;)
    {
	tpnext = curtab->tp_ಮುಂದಿನ;
	ಗಾಗಿ (wp = firstwin; wp != ಖಾಲಿ; wp = wpnext)
	{
	    wpnext = wp->w_ಮುಂದಿನ;
	    buf = wp->w_ಬಫರ್;
	    ಆದರೆ (buf->b_ffname == ಖಾಲಿ
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_ಅಗಲ != cmdline_ಅಗಲ)))
		i = aall->opened_len;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// check if the buffer in this window is in the arglist
		ಗಾಗಿ (i = 0; i < aall->opened_len; ++i)
		{
		    ಆದರೆ (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			ಪೂರ್ಣಾಂಕ ತೂಕ = 1;

			ಆದರೆ (old_curtab == curtab)
			{
			    ++ತೂಕ;
			    ಆದರೆ (old_curwin == wp)
				++ತೂಕ;
			}

			ಆದರೆ (ತೂಕ > (ಪೂರ್ಣಾಂಕ)aall->opened[i])
			{
			    aall->opened[i] = (char_u)ತೂಕ;
			    ಆದರೆ (i == 0)
			    {
				ಆದರೆ (aall->new_curwin != ಖಾಲಿ)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aall->keep_tabs)
			    i = aall->opened_len;

			ಆದರೆ (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    ಆದರೆ (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		ಆದರೆ (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !buf_Is_ಬದಲಾಯಿತು(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    ಆದರೆ (!buf_hide(buf) && buf->b_nwindows <= 1
							 && buf_Is_ಬದಲಾಯಿತು(buf))
		    {
			bufref_T    bufref;

			ಗಣ_bufref(&bufref, buf);

			(ಶೂನ್ಯ)autowrite(buf, FALSE);

			// check if autocommands removed the window
			ಆದರೆ (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    ಮುಂದುವರಿ;
			}
		    }
		    // don't close last window
		    ಆದರೆ (ONE_ಕಿಟಕಿ
			    && (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			win_close(wp, !buf_hide(buf) && !buf_Is_ಬದಲಾಯಿತು(buf));

			// check if autocommands removed the next window
			ಆದರೆ (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	ಆದರೆ (aall->had_tab == 0 || tpnext == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// check if autocommands removed the next tab page
	ಆದರೆ (!valid_tabpage(tpnext))
	    tpnext = ಮೊದಲ_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
arg_all_open_windows(arg_all_state_T *aall, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		tab_drop_ಖಾಲಿಗಳು_ಕಿಟಕಿ = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು_ret = OK;
    ಪೂರ್ಣಾಂಕ		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    ಆದರೆ (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == ಖಾಲಿ && !curbuf->b_ಬದಲಾಯಿತು)
    {
	aall->use_firstwin = TRUE;
	tab_drop_ಖಾಲಿಗಳು_ಕಿಟಕಿ = TRUE;
    }

    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2 && !got_int; ++i)
    {
	ಆದರೆ (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_ಕೊನೆಯ = TRUE;
	ಆದರೆ (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    ಆದರೆ (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    ಆದರೆ (wp->w_arg_idx == i)
		    {
			ಆದರೆ (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ
				!= curwin->w_ಚೌಕಟ್ಟು->fr_ಪೋಷಕ)
			{
			    emsg(_(e_ಕಿಟಕಿ_layout_ಬದಲಾಯಿತು_unexpectedly));
			    i = ಎಣಿಕೆ_2;
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    win_move_after(wp, curwin);
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಿಭಜಿಸು_ret == OK)
	{
	    // trigger events for tab drop
	    ಆದರೆ (tab_drop_ಖಾಲಿಗಳು_ಕಿಟಕಿ && i == ಎಣಿಕೆ_2 - 1)
		--autocmd_no_enter;
	    ಆದರೆ (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		ವಿಭಜಿಸು_ret = win_ವಿಭಜಿಸು(0, WSP_ಸ್ಥಳ | WSP_ಕೆಳಗೆ);
		p_ea = p_ea_save;
		ಆದರೆ (ವಿಭಜಿಸು_ret == FAIL)
		    ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    ಆದರೆ (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (ಶೂನ್ಯ)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), ಖಾಲಿ, ಖಾಲಿ,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_ಬಫರ್)
		      || buf_Is_ಬದಲಾಯಿತು(curwin->w_ಬಫರ್)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    ಆದರೆ (tab_drop_ಖಾಲಿಗಳು_ಕಿಟಕಿ && i == ಎಣಿಕೆ_2 - 1)
		++autocmd_no_enter;
	    ಆದರೆ (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	ಆದರೆ (aall->had_tab > 0 && tabpage_ಸೂಚ್ಯಂಕ(ಖಾಲಿ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_arg_all(
    ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ	forceit,		// hide buffers in current windows
    ಪೂರ್ಣಾಂಕ keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*ಕೊನೆಯ_curwin;
    tabpage_T		*ಕೊನೆಯ_curtab;
    ಪೂರ್ಣಾಂಕ			prev_arglist_locked = arglist_locked;

    ಆದರೆ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	ಹಿಂತಿರುಗಿ;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = ಖಾಲಿ;
    aall.new_curtab = ಖಾಲಿ;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    ಆದರೆ (aall.opened == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Autocommands may do anything to the argument list.  Make sure it's not
    // freed while we are working here by "locking" it.  We still have to
    // watch out for its size being changed.
    aall.alist = curwin->w_alist;
    ++aall.alist->al_refcount;
    arglist_locked = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    tabpage_T *new_lu_tp = curtab;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    // Try closing all windows that are not in the argument list.
    // Also close windows that are not full width;
    // When 'hidden' or "forceit" set the buffer becomes hidden.
    // Windows that have a changed buffer and can't be hidden won't be closed.
    // When the ":tab" modifier was used do this for all tab pages.
    arg_all_close_ಬಳಸದ_windows(&aall);

    // Open a window for files in the argument list that don't have one.
    // ARGCOUNT may change while doing this, because of autocommands.
    ಆದರೆ (ಎಣಿಕೆ_2 > aall.opened_len || ಎಣಿಕೆ_2 <= 0)
	ಎಣಿಕೆ_2 = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    ಕೊನೆಯ_curwin = curwin;
    ಕೊನೆಯ_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, ಎಣಿಕೆ_2);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    ಆದರೆ (ಕೊನೆಯ_curtab != aall.new_curtab)
    {
	ಆದರೆ (valid_tabpage(ಕೊನೆಯ_curtab))
	    goto_tabpage_tp(ಕೊನೆಯ_curtab, TRUE, TRUE);
	ಆದರೆ (win_valid(ಕೊನೆಯ_curwin))
	    win_enter(ಕೊನೆಯ_curwin, FALSE);
    }
    // to window with first arg
    ಆದರೆ (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    ಆದರೆ (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    ಆದರೆ (win_valid(aall.new_curwin))
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
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
	eap->line2 = 9999;
    do_arg_all((ಪೂರ್ಣಾಂಕ)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
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
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*retval = ಖಾಲಿ;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    ಗಾಗಿ (;;)
    {
	len = 0;
	ಗಾಗಿ (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    ಆದರೆ (p == ಖಾಲಿ)
		ಮುಂದುವರಿ;
	    ಆದರೆ (len > 0)
	    {
		// insert a space in between names
		ಆದರೆ (retval != ಖಾಲಿ)
		    retval[len] = ' ';
		++len;
	    }
	    ಗಾಗಿ ( ; *p != NUL; ++p)
	    {
		ಆದರೆ (*p == ' '
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
			|| *p == '\\'
#ಆದರೆ_ಅಂತ್ಯ
			|| *p == '`')
		{
		    // insert a backslash
		    ಆದರೆ (retval != ಖಾಲಿ)
			retval[len] = '\\';
		    ++len;
		}
		ಆದರೆ (retval != ಖಾಲಿ)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	ಆದರೆ (retval != ಖಾಲಿ)
	{
	    retval[len] = NUL;
	    ನಿಲ್ಲಿಸು;
	}

	// allocate memory
	retval = alloc(len + 1);
	ಆದರೆ (retval == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    ಶೂನ್ಯ
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_ಸಂಖ್ಯೆ = ARGCOUNT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_ಸಂಖ್ಯೆ
					   && tv_get_ಸಂಖ್ಯೆ(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_ಸಂಖ್ಯೆ = GARGCOUNT;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// use the argument list of the specified window
	wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
	ಆದರೆ (wp != ಖಾಲಿ)
	    rettv->vval.v_ಸಂಖ್ಯೆ = WARGCOUNT(wp);
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    }
}

/*
 * "argidx()" function
 */
    ಶೂನ್ಯ
f_argidx(typval_T *argvars ಬಳಸದ, typval_T *rettv)
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

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    wp = ಕಂಡುಹಿಡಿ_tabwin(&argvars[0], &argvars[1], ಖಾಲಿ);
    ಆದರೆ (wp != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_arglist_as_rettv(aentry_T *arglist, ಪೂರ್ಣಾಂಕ argcount, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (rettv_list_alloc(rettv) == OK && arglist != ಖಾಲಿ)
	ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    ಶೂನ್ಯ
f_argv(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		idx;
    aentry_T	*arglist = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argcount = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ
	    && tv_get_ಸಂಖ್ಯೆ(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	win_T	*wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[1]);

	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    idx = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ);
    ಆದರೆ (arglist != ಖಾಲಿ && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#ಆದರೆ_ಅಂತ್ಯ
