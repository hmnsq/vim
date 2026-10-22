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

#include "vim.h"

#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

// This flag is set whenever the argument list is being changed and calling a
// function that might trigger an autocommand.
static 정수 arglist_locked = FALSE;

    static 정수
check_arglist_locked(void)
{
    만약 (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Clear an argument list: free all file names and reset it to zero entries.
 */
    void
alist_clear(alist_T *al)
{
    만약 (check_arglist_locked() == FAIL)
	반환;
    동안 (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}

/*
 * Init an argument list.
 */
    void
alist_init(alist_T *al)
{
    ga_init2(&al->al_ga, sizeof(aentry_T), 5);
}

/*
 * Remove a reference from an argument list.
 * Ignored when the argument list is the global one.
 * If the argument list is no longer used by any window, free it.
 */
    void
alist_unlink(alist_T *al)
{
    만약 (al != &global_alist && --al->al_refcount <= 0)
    {
	alist_clear(al);
	vim_free(al);
    }
}

/*
 * Create a new argument list and use it for the current window.
 */
    void
alist_new(void)
{
    curwin->w_alist = ALLOC_ONE(alist_T);
    만약 (curwin->w_alist == NULL)
    {
	curwin->w_alist = &global_alist;
	++global_alist.al_refcount;
    }
    아니면
    {
	curwin->w_alist->al_refcount = 1;
	curwin->w_alist->id = ++max_alist_id;
	alist_init(curwin->w_alist);
    }
}

#만약 !defined(UNIX)
/*
 * Expand the file names in the global argument list.
 * If "fnum_list" is not NULL, use "fnum_list[fnum_len]" as a list of buffer
 * numbers to be re-used.
 */
    void
alist_expand(정수 *fnum_list, 정수 fnum_len)
{
    char_u	**old_arg_files;
    정수		old_arg_count;
    char_u	**new_arg_files;
    정수		new_arg_file_count;
    char_u	*save_p_su = p_su;
    정수		i;

    old_arg_files = ALLOC_MULT(char_u *, GARGCOUNT);
    만약 (old_arg_files == NULL)
	반환;

    // Don't use 'suffixes' here.  This should work like the shell did the
    // expansion.  Also, the vimrc file isn't read yet, thus the user
    // can't set the options.
    p_su = empty_option;
    위해 (i = 0; i < GARGCOUNT; ++i)
	old_arg_files[i] = vim_strsave(GARGLIST[i].ae_fname);
    old_arg_count = GARGCOUNT;
    만약 (expand_wildcards(old_arg_count, old_arg_files,
		&new_arg_file_count, &new_arg_files,
		EW_FILE|EW_NOTFOUND|EW_ADDSLASH|EW_NOERROR) == OK
	    && new_arg_file_count > 0)
    {
	alist_set(&global_alist, new_arg_file_count, new_arg_files,
		TRUE, fnum_list, fnum_len);
	FreeWild(old_arg_count, old_arg_files);
    }
    p_su = save_p_su;
}
#endif

/*
 * Set the argument list for the current window.
 * Takes over the allocated files[] and the allocated fnames in it.
 */
    void
alist_set(
    alist_T	*al,
    정수		count,
    char_u	**files,
    정수		use_curbuf,
    정수		*fnum_list,
    정수		fnum_len)
{
    정수		i;

    만약 (check_arglist_locked() == FAIL)
	반환;

    alist_clear(al);
    만약 (GA_GROW_OK(&al->al_ga, count))
    {
	위해 (i = 0; i < count; ++i)
	{
	    만약 (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		동안 (i < count)
		    vim_free(files[i++]);
		break;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    만약 (fnum_list != NULL && i < fnum_len)
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
    아니면
	FreeWild(count, files);
    만약 (al == &global_alist)
	arg_had_last = FALSE;
}

/*
 * Add file "fname" to argument list "al".
 * "fname" must have been allocated and "al" must have been checked for room.
 *
 * May trigger Buf* autocommands
 */
    void
alist_add(
    alist_T	*al,
    char_u	*fname,
    정수		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    win_T	*wp = curwin;

    만약 (fname == NULL)		// don't add NULL file names
	반환;
    만약 (check_arglist_locked() == FAIL)
	반환;
    arglist_locked = TRUE;
    ++wp->w_locked;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    만약 (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
    --wp->w_locked;
}

#만약 defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    void
alist_slash_adjust(void)
{
    정수		i;
    win_T	*wp;
    tabpage_T	*tp;

    위해 (i = 0; i < GARGCOUNT; ++i)
	만약 (GARGLIST[i].ae_fname != NULL)
	    slash_adjust(GARGLIST[i].ae_fname);
    FOR_ALL_TAB_WINDOWS(tp, wp)
	만약 (wp->w_alist != &global_alist)
	    위해 (i = 0; i < WARGCOUNT(wp); ++i)
		만약 (WARGLIST(wp)[i].ae_fname != NULL)
		    slash_adjust(WARGLIST(wp)[i].ae_fname);
}
#endif

/*
 * Isolate one argument, taking backticks.
 * Changes the argument in-place, puts a NUL after it.  Backticks remain.
 * Return a pointer to the start of the next argument.
 */
    static char_u *
do_one_arg(char_u *str)
{
    char_u	*p;
    정수		inbacktick;

    inbacktick = FALSE;
    위해 (p = str; *str; ++str)
    {
	// When the backslash is used for escaping the special meaning of a
	// character we need to keep it until wildcard expansion.
	만약 (rem_backslash(str))
	{
	    *p++ = *str++;
	    *p++ = *str;
	}
	아니면
	{
	    // An item ends at a space not in backticks
	    만약 (!inbacktick && vim_isspace(*str))
		break;
	    만약 (*str == '`')
		inbacktick ^= TRUE;
	    *p++ = *str;
	}
    }
    str = skipwhite(str);
    *p = NUL;

    반환 str;
}

/*
 * Separate the arguments in "str" and return a list of pointers in the
 * growarray "gap".
 */
    static 정수
get_arglist(garray_T *gap, char_u *str, 정수 escaped)
{
    ga_init2(gap, sizeof(char_u *), 20);
    동안 (*str != NUL)
    {
	만약 (ga_grow(gap, 1) == FAIL)
	{
	    ga_clear(gap);
	    반환 FAIL;
	}
	((char_u **)gap->ga_data)[gap->ga_len++] = str;

	// If str is escaped, don't handle backslashes or spaces
	만약 (!escaped)
	    반환 OK;

	// Isolate one argument, change it in-place, put a NUL after it.
	str = do_one_arg(str);
    }
    반환 OK;
}

#만약 defined(FEAT_QUICKFIX) || defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
/*
 * Parse a list of arguments (file names), expand them and return in
 * "fnames[fcountp]".  When "wig" is TRUE, removes files matching 'wildignore'.
 * Return FAIL or OK.
 */
    정수
get_arglist_exp(
    char_u	*str,
    정수		*fcountp,
    char_u	***fnamesp,
    정수		wig)
{
    garray_T	ga;
    정수		i;

    만약 (get_arglist(&ga, str, TRUE) == FAIL)
	반환 FAIL;
    만약 (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    아니면
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    반환 i;
}
#endif

/*
 * Check the validity of the arg_idx for each other window.
 */
    static void
alist_check_arg_idx(void)
{
    win_T	*win;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, win)
	만약 (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 */
    static void
alist_add_list(
    정수		count,
    char_u	**files,
    정수		after,	    // where to add: 0 = before first one
    정수		will_edit)  // will edit adding argument
{
    정수		i;
    정수		old_argcount = ARGCOUNT;

    만약 (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, count))
    {
	win_T	*wp = curwin;

	만약 (after < 0)
	    after = 0;
	만약 (after > ARGCOUNT)
	    after = ARGCOUNT;
	만약 (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + count]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * sizeof(aentry_T));
	arglist_locked = TRUE;
	++wp->w_locked;
	위해 (i = 0; i < count; ++i)
	{
	    정수 flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	--wp->w_locked;
	ALIST(wp)->al_ga.ga_len += count;
	만약 (old_argcount > 0 && wp->w_arg_idx >= after)
	    wp->w_arg_idx += count;
	반환;
    }

    위해 (i = 0; i < count; ++i)
	vim_free(files[i]);
}

/*
 * Delete the file names in 'alist_ga' from the argument list.
 */
    static void
arglist_del_files(garray_T *alist_ga)
{
    regmatch_T	regmatch;
    정수		didone;
    정수		i;
    char_u	*p;
    정수		match;

    // Delete the items: use each item as a regexp and find a match in the
    // argument list.
    regmatch.rm_ic = p_fic;	// ignore case when 'fileignorecase' is set
    위해 (i = 0; i < alist_ga->ga_len && !got_int; ++i)
    {
	p = ((char_u **)alist_ga->ga_data)[i];
	p = file_pat_to_reg_pat(p, NULL, NULL, FALSE);
	만약 (p == NULL)
	    break;
	regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);
	만약 (regmatch.regprog == NULL)
	{
	    vim_free(p);
	    break;
	}

	didone = FALSE;
	위해 (match = 0; match < ARGCOUNT; ++match)
	    만약 (vim_regexec(&regmatch, alist_name(&ARGLIST[match]), (colnr_T)0))
	    {
		didone = TRUE;
		vim_free(ARGLIST[match].ae_fname);
		mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			(ARGCOUNT - match - 1) * sizeof(aentry_T));
		--ARGCOUNT;
		만약 (curwin->w_arg_idx > match)
		    --curwin->w_arg_idx;
		--match;
	    }

	vim_regfree(regmatch.regprog);
	vim_free(p);
	만약 (!didone)
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
    static 정수
do_arglist(
    char_u	*str,
    정수		what,
    정수		after UNUSED,	// 0 means before first one
    정수		will_edit)	// will edit added argument
{
    garray_T	new_ga;
    정수		exp_count;
    char_u	**exp_files;
    정수		i;
    정수		arg_escaped = TRUE;

    만약 (check_arglist_locked() == FAIL)
	반환 FAIL;

    // Set default argument for ":argadd" command.
    만약 (what == AL_ADD && *str == NUL)
    {
	만약 (curbuf->b_ffname == NULL)
	    반환 FAIL;
	str = curbuf->b_fname;
	arg_escaped = FALSE;
    }

    // Collect all file name arguments in "new_ga".
    만약 (get_arglist(&new_ga, str, arg_escaped) == FAIL)
	반환 FAIL;

    만약 (what == AL_DEL)
	arglist_del_files(&new_ga);
    아니면
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_count, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	만약 (i == FAIL || exp_count == 0)
	{
	    emsg(_(e_no_match));
	    반환 FAIL;
	}

	만약 (what == AL_ADD)
	{
	    alist_add_list(exp_count, exp_files, after, will_edit);
	    vim_free(exp_files);
	}
	아니면 // what == AL_SET
	    alist_set(ALIST(curwin), exp_count, exp_files, will_edit, NULL, 0);
    }

    alist_check_arg_idx();

    반환 OK;
}

/*
 * Redefine the argument list.
 */
    void
set_arglist(char_u *str)
{
    do_arglist(str, AL_SET, 0, TRUE);
}

/*
 * Return TRUE if window "win" is editing the file at the current argument
 * index.
 */
    정수
editing_arg_idx(win_T *win)
{
    반환 !(win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == NULL
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))));
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    void
check_arg_idx(win_T *win)
{
    만약 (WARGCOUNT(win) > 1 && !editing_arg_idx(win))
    {
	// We are not editing the current entry in the argument list.
	// Set "arg_had_last" if we are editing the last one.
	win->w_arg_idx_invalid = true;
	만약 (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
		&& ALIST(win) == &global_alist
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != NULL
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
			  win->w_buffer->b_ffname, TRUE, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    아니면
    {
	// We are editing the current entry in the argument list.
	// Set "arg_had_last" if it's also the last one
	win->w_arg_idx_invalid = false;
	만약 (win->w_arg_idx == WARGCOUNT(win) - 1
					      && win->w_alist == &global_alist)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":arglocal" and ":argglobal".
 */
    void
ex_args(exarg_T *eap)
{
    정수		i;

    만약 (eap->cmdidx != CMD_args)
    {
	만약 (check_arglist_locked() == FAIL)
	    반환;
	alist_unlink(ALIST(curwin));
	만약 (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	아니면 // eap->cmdidx == CMD_arglocal
	    alist_new();
    }

    // ":args file ..": define new argument list, handle like ":next"
    // Also for ":argslocal file .." and ":argsglobal file ..".
    만약 (*eap->arg != NUL)
    {
	만약 (check_arglist_locked() == FAIL)
	    반환;
	ex_next(eap);
	반환;
    }

    // ":args": list arguments.
    만약 (eap->cmdidx == CMD_args)
    {
	char_u **items;

	만약 (ARGCOUNT <= 0)
	    반환;		// empty argument list

	items = ALLOC_MULT(char_u *, ARGCOUNT);
	만약 (items == NULL)
	    반환;

	// Overwrite the command, for a short list there is no scrolling
	// required and no wait_return().
	gotocmdline(TRUE);

	위해 (i = 0; i < ARGCOUNT; ++i)
	    items[i] = alist_name(&ARGLIST[i]);
	list_in_columns(items, ARGCOUNT, curwin->w_arg_idx);
	vim_free(items);

	반환;
    }

    // ":argslocal": make a local copy of the global argument list.
    만약 (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	만약 (GA_GROW_FAILS(gap, GARGCOUNT))
	    반환;

	위해 (i = 0; i < GARGCOUNT; ++i)
	    만약 (GARGLIST[i].ae_fname != NULL)
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
    void
ex_previous(exarg_T *eap)
{
    // If past the last one already, go to the last one.
    만약 (curwin->w_arg_idx - (정수)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    아니면
	do_argfile(eap, curwin->w_arg_idx - (정수)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    void
ex_rewind(exarg_T *eap)
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    void
ex_last(exarg_T *eap)
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    void
ex_argument(exarg_T *eap)
{
    정수		i;

    만약 (eap->addr_count > 0)
	i = eap->line2 - 1;
    아니면
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    void
do_argfile(exarg_T *eap, 정수 argn)
{
    정수		other;
    char_u	*p;
    정수		old_arg_idx = curwin->w_arg_idx;
    정수 is_split_cmd = *eap->cmd == 's';

    만약 (ERROR_IF_ANY_POPUP_WINDOW)
	반환;
    만약 (argn < 0 || argn >= ARGCOUNT)
    {
	만약 (ARGCOUNT <= 1)
	    emsg(_(e_there_is_only_one_file_to_edit));
	아니면 만약 (argn < 0)
	    emsg(_(e_cannot_go_before_first_file));
	아니면
	    emsg(_(e_cannot_go_beyond_last_file));

	반환;
    }

    만약 (!is_split_cmd
	    && (&ARGLIST[argn])->ae_fnum != curbuf->b_fnum
	    && !check_can_set_curbuf_forceit(eap->forceit))
	반환;

    setpcmark();
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // split window or create new tab page first
    만약 (is_split_cmd || cmdmod.cmod_tab != 0)
    {
	만약 (win_split(0, 0) == FAIL)
	    반환;
	RESET_BINDING(curwin);
    }
    아니면
    {
	// if 'hidden' set, only check for changed file when re-editing
	// the same buffer
	other = TRUE;
	만약 (buf_hide(curbuf))
	{
	    p = fix_fname(alist_name(&ARGLIST[argn]));
	    other = otherfile(p);
	    vim_free(p);
	}
	만약 ((!buf_hide(curbuf) || !other)
		&& check_changed(curbuf, CCGD_AW
		    | (other ? 0 : CCGD_MULTWIN)
		    | (eap->forceit ? CCGD_FORCEIT : 0)
		    | CCGD_EXCMD))
	    반환;
    }

    curwin->w_arg_idx = argn;
    만약 (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	arg_had_last = TRUE;

    // Edit the file; always use the last known line number.
    // When it fails (e.g. Abort for already edited file) restore the
    // argument index.
    만약 (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), NULL,
		eap, ECMD_LAST,
		(buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
		+ (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	curwin->w_arg_idx = old_arg_idx;
    // like Vi: set the mark where the cursor is in the file.
    아니면 만약 (eap->cmdidx != CMD_argdo)
	setmark('\'');
}

/*
 * ":next", and commands that behave like it.
 */
    void
ex_next(exarg_T *eap)
{
    정수		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    만약 (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	만약 (*eap->arg != NUL)		    // redefine file list
	{
	    만약 (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		반환;
	    i = 0;
	}
	아니면
	    i = curwin->w_arg_idx + (정수)eap->line2;
	do_argfile(eap, i);
    }
}

/*
 * ":argdedupe"
 */
    void
ex_argdedupe(exarg_T *eap UNUSED)
{
    정수 i;
    정수 j;

    위해 (i = 0; i < ARGCOUNT; ++i)
    {
	// Expand each argument to a full path to catch different paths leading
	// to the same file.
	char_u *firstFullname = FullName_save(ARGLIST[i].ae_fname, FALSE);
	만약 (firstFullname == NULL)
	    반환;  // out of memory

	위해 (j = i + 1; j < ARGCOUNT; ++j)
	{
	    char_u *secondFullname = FullName_save(ARGLIST[j].ae_fname, FALSE);
	    만약 (secondFullname == NULL)
		break;  // out of memory
	    정수 areNamesDuplicate =
				  fnamecmp(firstFullname, secondFullname) == 0;
	    vim_free(secondFullname);

	    만약 (areNamesDuplicate)
	    {
		// remove one duplicate argument
		vim_free(ARGLIST[j].ae_fname);
		mch_memmove(ARGLIST + j, ARGLIST + j + 1,
					(ARGCOUNT - j - 1) * sizeof(aentry_T));
		--ARGCOUNT;

		만약 (curwin->w_arg_idx == j)
		    curwin->w_arg_idx = i;
		아니면 만약 (curwin->w_arg_idx > j)
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
    void
ex_argedit(exarg_T *eap)
{
    정수 i = eap->addr_count ? (정수)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    정수 curbuf_is_reusable = curbuf_reusable();

    만약 (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	반환;
    maketitle();

    만약 (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == NULL || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    만약 (i < ARGCOUNT)
	do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    void
ex_argadd(exarg_T *eap)
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_count > 0 ? (정수)eap->line2 : curwin->w_arg_idx + 1,
	       FALSE);
    maketitle();
}

/*
 * ":argdelete"
 */
    void
ex_argdelete(exarg_T *eap)
{
    정수		i;
    정수		n;

    만약 (check_arglist_locked() == FAIL)
	반환;

    만약 (eap->addr_count > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	만약 (eap->addr_count == 0)
	{
	    만약 (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_(e_no_argument_to_delete));
		반환;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	아니면 만약 (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	만약 (*eap->arg != NUL)
	{
	    // Can't have both a range and an argument.
	    emsg(_(e_invalid_argument));
	    반환;
	}
	만약 (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    만약 (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	    반환;
	}

	위해 (i = eap->line1; i <= eap->line2; ++i)
	    vim_free(ARGLIST[i - 1].ae_fname);
	mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
		    (size_t)((ARGCOUNT - eap->line2) * sizeof(aentry_T)));
	ARGCOUNT -= n;
	만약 (curwin->w_arg_idx >= eap->line2)
	    curwin->w_arg_idx -= n;
	아니면 만약 (curwin->w_arg_idx > eap->line1)
	    curwin->w_arg_idx = eap->line1;
    }
    아니면
	do_arglist(eap->arg, AL_DEL, 0, FALSE);

    만약 (ARGCOUNT == 0)
	curwin->w_arg_idx = 0;
    아니면 만약 (curwin->w_arg_idx >= ARGCOUNT)
	curwin->w_arg_idx = ARGCOUNT - 1;

    maketitle();
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * argedit and argdelete commands.
 */
    char_u *
get_arglist_name(expand_T *xp UNUSED, 정수 idx)
{
    만약 (idx >= ARGCOUNT)
	반환 NULL;

    반환 alist_name(&ARGLIST[idx]);
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
    만약 (bp == NULL || bp->b_fname == NULL)
	반환 aep->ae_fname;
    반환 bp->b_fname;
}

/*
 * State used by the :all command to open all the files in the argument list in
 * separate windows.
 */
typedef struct {
    alist_T	*alist;		// argument list to be used
    정수		had_tab;
    정수		keep_tabs;
    정수		forceit;

    정수		use_firstwin;	// use first window for arglist
    char_u	*opened;	// Array of weight for which args are open:
				//  0: not opened
				//  1: opened in other tab
				//  2: opened in curtab
				//  3: opened in curtab and curwin
    정수		opened_len;	// length of opened[]
    win_T	*new_curwin;
    tabpage_T	*new_curtab;
} arg_all_state_T;

/*
 * Close all the windows containing files which are not in the argument list.
 * Used by the ":all" command.
 */
    static void
arg_all_close_unused_windows(arg_all_state_T *aall)
{
    win_T	*wp;
    win_T	*wpnext;
    tabpage_T	*tpnext;
    buf_T	*buf;
    정수		i;
    win_T	*old_curwin;
    tabpage_T	*old_curtab;

    old_curwin = curwin;
    old_curtab = curtab;

    만약 (aall->had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);

    // moving tabpages around in an autocommand may cause an endless loop
    tabpage_move_disallowed++;
    위해 (;;)
    {
	tpnext = curtab->tp_next;
	위해 (wp = firstwin; wp != NULL; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    buf = wp->w_buffer;
	    만약 (buf->b_ffname == NULL
		    || (!aall->keep_tabs && (buf->b_nwindows > 1
			    || wp->w_width != cmdline_width)))
		i = aall->opened_len;
	    아니면
	    {
		// check if the buffer in this window is in the arglist
		위해 (i = 0; i < aall->opened_len; ++i)
		{
		    만약 (i < aall->alist->al_ga.ga_len
			    && (AARGLIST(aall->alist)[i].ae_fnum == buf->b_fnum
				|| fullpathcmp(alist_name(
						    &AARGLIST(aall->alist)[i]),
					buf->b_ffname, TRUE, TRUE) & FPC_SAME))
		    {
			정수 weight = 1;

			만약 (old_curtab == curtab)
			{
			    ++weight;
			    만약 (old_curwin == wp)
				++weight;
			}

			만약 (weight > (정수)aall->opened[i])
			{
			    aall->opened[i] = (char_u)weight;
			    만약 (i == 0)
			    {
				만약 (aall->new_curwin != NULL)
				    aall->new_curwin->w_arg_idx =
							      aall->opened_len;
				aall->new_curwin = wp;
				aall->new_curtab = curtab;
			    }
			}
			아니면 만약 (aall->keep_tabs)
			    i = aall->opened_len;

			만약 (wp->w_alist != aall->alist)
			{
			    // Use the current argument list for all windows
			    // containing a file from it.
			    alist_unlink(wp->w_alist);
			    wp->w_alist = aall->alist;
			    ++wp->w_alist->al_refcount;
			}
			break;
		    }
		}
	    }
	    wp->w_arg_idx = i;

	    만약 (i == aall->opened_len && !aall->keep_tabs)// close this window
	    {
		만약 (buf_hide(buf) || aall->forceit || buf->b_nwindows > 1
							|| !bufIsChanged(buf))
		{
		    // If the buffer was changed, and we would like to hide it,
		    // try autowriting.
		    만약 (!buf_hide(buf) && buf->b_nwindows <= 1
							 && bufIsChanged(buf))
		    {
			bufref_T    bufref;

			set_bufref(&bufref, buf);

			(void)autowrite(buf, FALSE);

			// check if autocommands removed the window
			만약 (!win_valid(wp) || !bufref_valid(&bufref))
			{
			    wpnext = firstwin;	// start all over...
			    continue;
			}
		    }
		    // don't close last window
		    만약 (ONE_WINDOW
			    && (first_tabpage->tp_next == NULL
				|| !aall->had_tab))
			aall->use_firstwin = TRUE;
		    아니면
		    {
			win_close(wp, !buf_hide(buf) && !bufIsChanged(buf));

			// check if autocommands removed the next window
			만약 (!win_valid(wpnext))
			    wpnext = firstwin;	// start all over...
		    }
		}
	    }
	}

	// Without the ":tab" modifier only do the current tab page.
	만약 (aall->had_tab == 0 || tpnext == NULL)
	    break;

	// check if autocommands removed the next tab page
	만약 (!valid_tabpage(tpnext))
	    tpnext = first_tabpage;	// start all over...

	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }
    tabpage_move_disallowed--;
}

/*
 * Open upto "count" windows for the files in the argument list 'aall->alist'.
 */
    static void
arg_all_open_windows(arg_all_state_T *aall, 정수 count)
{
    win_T	*wp;
    정수		tab_drop_empty_window = FALSE;
    정수		i;
    정수		split_ret = OK;
    정수		p_ea_save;

    // ":tab drop file" should re-use an empty window to avoid "--remote-tab"
    // leaving an empty tab page when executed locally.
    만약 (aall->keep_tabs && BUFEMPTY() && curbuf->b_nwindows == 1
			    && curbuf->b_ffname == NULL && !curbuf->b_changed)
    {
	aall->use_firstwin = TRUE;
	tab_drop_empty_window = TRUE;
    }

    위해 (i = 0; i < count && !got_int; ++i)
    {
	만약 (aall->alist == &global_alist && i == global_alist.al_ga.ga_len - 1)
	    arg_had_last = TRUE;
	만약 (aall->opened[i] > 0)
	{
	    // Move the already present window to below the current window
	    만약 (curwin->w_arg_idx != i)
	    {
		FOR_ALL_WINDOWS(wp)
		{
		    만약 (wp->w_arg_idx == i)
		    {
			만약 (aall->keep_tabs)
			{
			    aall->new_curwin = wp;
			    aall->new_curtab = curtab;
			}
			아니면 만약 (wp->w_frame->fr_parent
				!= curwin->w_frame->fr_parent)
			{
			    emsg(_(e_window_layout_changed_unexpectedly));
			    i = count;
			    break;
			}
			아니면
			    win_move_after(wp, curwin);
			break;
		    }
		}
	    }
	}
	아니면 만약 (split_ret == OK)
	{
	    // trigger events for tab drop
	    만약 (tab_drop_empty_window && i == count - 1)
		--autocmd_no_enter;
	    만약 (!aall->use_firstwin)		// split current window
	    {
		p_ea_save = p_ea;
		p_ea = TRUE;		// use space from all windows
		split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
		p_ea = p_ea_save;
		만약 (split_ret == FAIL)
		    continue;
	    }
	    아니면    // first window: do autocmd for leaving this buffer
		--autocmd_no_leave;

	    // edit file "i"
	    curwin->w_arg_idx = i;
	    만약 (i == 0)
	    {
		aall->new_curwin = curwin;
		aall->new_curtab = curtab;
	    }
	    (void)do_ecmd(0, alist_name(&AARGLIST(aall->alist)[i]), NULL, NULL,
		    ECMD_ONE,
		    ((buf_hide(curwin->w_buffer)
		      || bufIsChanged(curwin->w_buffer)) ? ECMD_HIDE : 0)
		    + ECMD_OLDBUF, curwin);
	    만약 (tab_drop_empty_window && i == count - 1)
		++autocmd_no_enter;
	    만약 (aall->use_firstwin)
		++autocmd_no_leave;
	    aall->use_firstwin = FALSE;
	}
	ui_breakcheck();

	// When ":tab" was used open a new tab for a new window repeatedly.
	만약 (aall->had_tab > 0 && tabpage_index(NULL) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
}

/*
 * do_arg_all(): Open up to "count" windows, one for each argument.
 */
    static void
do_arg_all(
    정수	count,
    정수	forceit,		// hide buffers in current windows
    정수 keep_tabs)		// keep current tabs, for ":tab drop file"
{
    arg_all_state_T	aall;
    win_T		*last_curwin;
    tabpage_T		*last_curtab;
    정수			prev_arglist_locked = arglist_locked;

    만약 (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	반환;
    }
    만약 (ARGCOUNT <= 0)
    {
	// Don't give an error message.  We don't want it when the ":all"
	// command is in the .vimrc.
	반환;
    }
    setpcmark();

    aall.use_firstwin = FALSE;
    aall.had_tab = cmdmod.cmod_tab;
    aall.new_curwin = NULL;
    aall.new_curtab = NULL;
    aall.forceit = forceit;
    aall.keep_tabs = keep_tabs;
    aall.opened_len = ARGCOUNT;
    aall.opened = alloc_clear(aall.opened_len);
    만약 (aall.opened == NULL)
	반환;

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
    만약 (count > aall.opened_len || count <= 0)
	count = aall.opened_len;

    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    ++autocmd_no_leave;
    last_curwin = curwin;
    last_curtab = curtab;
    win_enter(lastwin, FALSE);

    /*
     * Open upto "count" windows.
     */
    arg_all_open_windows(&aall, count);

    // Remove the "lock" on the argument list.
    alist_unlink(aall.alist);
    arglist_locked = prev_arglist_locked;

    --autocmd_no_enter;

    // restore last referenced tabpage's curwin
    만약 (last_curtab != aall.new_curtab)
    {
	만약 (valid_tabpage(last_curtab))
	    goto_tabpage_tp(last_curtab, TRUE, TRUE);
	만약 (win_valid(last_curwin))
	    win_enter(last_curwin, FALSE);
    }
    // to window with first arg
    만약 (valid_tabpage(aall.new_curtab))
	goto_tabpage_tp(aall.new_curtab, TRUE, TRUE);

    // Now set the last used tabpage to where we started.
    만약 (valid_tabpage(new_lu_tp))
	lastused_tabpage = new_lu_tp;

    만약 (win_valid(aall.new_curwin))
	win_enter(aall.new_curwin, FALSE);

    --autocmd_no_leave;
    vim_free(aall.opened);
}

/*
 * ":all" and ":sall".
 * Also used for ":tab drop file ..." after setting the argument list.
 */
    void
ex_all(exarg_T *eap)
{
    만약 (eap->addr_count == 0)
	eap->line2 = 9999;
    do_arg_all((정수)eap->line2, eap->forceit, eap->cmdidx == CMD_drop);
}

/*
 * Concatenate all files in the argument list, separated by spaces, and return
 * it in one allocated string.
 * Spaces and backslashes in the file names are escaped with a backslash.
 * Returns NULL when out of memory.
 */
    char_u *
arg_all(void)
{
    정수		len;
    정수		idx;
    char_u	*retval = NULL;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    위해 (;;)
    {
	len = 0;
	위해 (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    만약 (p == NULL)
		continue;
	    만약 (len > 0)
	    {
		// insert a space in between names
		만약 (retval != NULL)
		    retval[len] = ' ';
		++len;
	    }
	    위해 ( ; *p != NUL; ++p)
	    {
		만약 (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			|| *p == '\\'
#endif
			|| *p == '`')
		{
		    // insert a backslash
		    만약 (retval != NULL)
			retval[len] = '\\';
		    ++len;
		}
		만약 (retval != NULL)
		    retval[len] = *p;
		++len;
	    }
	}

	// second time: break here
	만약 (retval != NULL)
	{
	    retval[len] = NUL;
	    break;
	}

	// allocate memory
	retval = alloc(len + 1);
	만약 (retval == NULL)
	    break;
    }

    반환 retval;
}

#만약 defined(FEAT_EVAL)
/*
 * "argc([window id])" function
 */
    void
f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_number = ARGCOUNT;
    아니면 만약 (argvars[0].v_type == VAR_NUMBER
					   && tv_get_number(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_number = GARGCOUNT;
    아니면
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	만약 (wp != NULL)
	    rettv->vval.v_number = WARGCOUNT(wp);
	아니면
	    rettv->vval.v_number = -1;
    }
}

/*
 * "argidx()" function
 */
    void
f_argidx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = curwin->w_arg_idx;
}

/*
 * "arglistid()" function
 */
    void
f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    만약 (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	반환;

    rettv->vval.v_number = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], NULL);
    만약 (wp != NULL)
	rettv->vval.v_number = wp->w_alist->id;
}

/*
 * Get the argument list for a given window
 */
    static void
get_arglist_as_rettv(aentry_T *arglist, 정수 argcount, typval_T *rettv)
{
    정수		idx;

    만약 (rettv_list_alloc(rettv) == OK && arglist != NULL)
	위해 (idx = 0; idx < argcount; ++idx)
	    list_append_string(rettv->vval.v_list,
						alist_name(&arglist[idx]), -1);
}

/*
 * "argv(nr)" function
 */
    void
f_argv(typval_T *argvars, typval_T *rettv)
{
    정수		idx;
    aentry_T	*arglist = NULL;
    정수		argcount = -1;

    만약 (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	반환;

    만약 (argvars[0].v_type == VAR_UNKNOWN)
    {
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
	반환;
    }

    만약 (argvars[1].v_type == VAR_UNKNOWN)
    {
	arglist = ARGLIST;
	argcount = ARGCOUNT;
    }
    아니면 만약 (argvars[1].v_type == VAR_NUMBER
	    && tv_get_number(&argvars[1]) == -1)
    {
	arglist = GARGLIST;
	argcount = GARGCOUNT;
    }
    아니면
    {
	win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	만약 (wp != NULL)
	{
	    // Use the argument list of the specified window
	    arglist = WARGLIST(wp);
	    argcount = WARGCOUNT(wp);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    idx = tv_get_number_chk(&argvars[0], NULL);
    만약 (arglist != NULL && idx >= 0 && idx < argcount)
	rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
    아니면 만약 (idx == -1)
	get_arglist_as_rettv(arglist, argcount, rettv);
}
#endif
