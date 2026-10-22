/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9script.c: :vim9script, :import, :export and friends
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    ਪੂਰਨ
ਵਿੱਚ_old_script(ਪੂਰਨ max_ਸੰਸਕਰਣ)
{
    ਵਾਪਸ (current_sctx.sc_ਸੰਸਕਰਣ < max_ਸੰਸਕਰਣ
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    ਪੂਰਨ
current_script_is_vim9(ਖਾਲੀ)
{
    ਵਾਪਸ SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ
						       == SCRIPT_ਸੰਸਕਰਣ_VIM9;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    ਖਾਲੀ
clear_vim9_scriptlocal_vars(ਪੂਰਨ sid)
{
    hashtab_T	*ht = &SCRIPT_VARS(sid);

    hashtab_free_contents(ht);
    hash_init(ht);
    delete_script_functions(sid);

    // also the exported variables of an autoload script
    delete_autoload_export_vars(SCRIPT_ਵਸਤੂ(sid)->sn_autoload_ਅਗੇਤਰ);

    // old imports and script variables are no longer valid
    free_imports_and_script_vars(sid);
}
#ਜੇ_ਅੰਤ

/*
 * ":vim9script".
 */
    ਖਾਲੀ
ex_vim9script(exarg_T *eap ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    ਪੂਰਨ		    ਮਿਲਿਆ_noclear = FALSE;
    char_u	    *p;

    ਜੇ (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
	ਵਾਪਸ;
    }

    si = SCRIPT_ਵਸਤੂ(sid);
    ਜੇ (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_ਪਹਿਲਾ_command_ਵਿੱਚ_script));
	ਵਾਪਸ;
    }

    ਲਈ (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	ਜੇ (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    ਜੇ (ਮਿਲਿਆ_noclear)
	    {
		semsg(_(e_duplicate_ਦਲੀਲ_str), p);
		ਵਾਪਸ;
	    }
	    ਮਿਲਿਆ_noclear = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (si->sn_state == SN_STATE_RELOAD && !ਮਿਲਿਆ_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    ਜੇ (si->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
	si->sn_autoload_ਅਗੇਤਰ = get_autoload_ਅਗੇਤਰ(si);

    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
    si->sn_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;

    ਜੇ (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#ਨਹੀਂਤਾਂ
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    ਪੂਰਨ
not_ਵਿੱਚ_vim9(exarg_T *eap, bool silent)
{
    ਜੇ (ਵਿੱਚ_vim9script())
	ਚੋਣ (eap->cmdidx)
	{
	    ਹਾਲਤ CMD_k:
		ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
		{
		    ਜੇ (!silent)
			emsg(_(e_no_range_allowed));
		    ਵਾਪਸ FAIL;
		}
		// FALLTHROUGH
	    ਹਾਲਤ CMD_append:
	    ਹਾਲਤ CMD_change:
	    ਹਾਲਤ CMD_ਪਾਓ:
	    ਹਾਲਤ CMD_open:
	    ਹਾਲਤ CMD_t:
	    ਹਾਲਤ CMD_xit:
		ਜੇ (!silent)
		    semsg(_(e_command_not_supported_ਵਿੱਚ_vim9_script_missing_var_str), eap->cmd);
		ਵਾਪਸ FAIL;
	    ਮੂਲ: ਰੋਕੋ;
	}
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    ਪੂਰਨ
vim9_bad_comment(char_u *p)
{
    ਜੇ (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	ਜੇ (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_comment));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    ਪੂਰਨ
vim9_comment_ਸ਼ੁਰੂ(char_u *p)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਵਾਪਸ p[0] == '#' && !vim9_bad_comment(p);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ p[0] == '#' && (p[1] != '{' || p[2] == '{');
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    ਖਾਲੀ
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    ਜੇ (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ਵਾਪਸ;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    ਜੇ (eap->cmd == ਨਿਰਾ)
	ਵਾਪਸ;
    vim_snprintf((ਅੱਖਰ *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = ਨਿਰਾ;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    ਖਾਲੀ
ex_export(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    ਸਥਿਰ imported_T *
new_imported(garray_T *gap)
{
    ਜੇ (ga_grow(gap, 1) == OK)
	ਵਾਪਸ ((imported_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len++);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    ਸਥਿਰ ਖਾਲੀ
free_all_script_vars(scriptitem_T *si)
{
    ਪੂਰਨ		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_ਅਗਲਾ;
    ਪੂਰਨ		idx;

    hash_ਤਾਲਾ(ht);
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    ਜਦੋਂ (sav != ਨਿਰਾ)
	    {
		sav_ਅਗਲਾ = sav->sav_ਅਗਲਾ;
		ਜੇ (sav->sav_di == ਨਿਰਾ)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_ਅਗਲਾ;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    ਲਈ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

	ਜੇ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    ਖਾਲੀ
free_imports_and_script_vars(ਪੂਰਨ sid)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sid);
    ਪੂਰਨ		    idx;

    ਲਈ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ਅੰਕੜੇ) + idx;

	vim_free(imp->imp_name);
    }
    ga_clear(&si->sn_imports);

    free_all_script_vars(si);

    clear_type_list(&si->sn_type_list);
}

/*
 * Mark all imports as possible to redefine.  Used when a script is loaded
 * again but not cleared.
 */
    ਖਾਲੀ
mark_imports_for_reload(ਪੂਰਨ sid)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sid);
    ਪੂਰਨ		    idx;

    ਲਈ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ਅੰਕੜੇ) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_import_fname(char_u *fname, ਪੂਰਨ is_autoload, ਪੂਰਨ *sid)
{
    ਜੇ (is_autoload)
    {
	scriptitem_T	*si;

	*sid = ਲੱਭੋ_script_by_name(fname);
	ਜੇ (*sid < 0)
	{
	    ਪੂਰਨ error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    ਜੇ (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		ਵਾਪਸ FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    ਜੇ (error == FAIL)
		ਵਾਪਸ FAIL;
	}

	si = SCRIPT_ਵਸਤੂ(*sid);
	si->sn_import_autoload = TRUE;

	ਜੇ (si->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
	    si->sn_autoload_ਅਗੇਤਰ = get_autoload_ਅਗੇਤਰ(si);

	// with testing override: load autoload script right away
	ਜੇ (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    ਵਾਪਸ OK;
    }
    ਵਾਪਸ do_ਸਰੋਤ(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    ਸਥਿਰ char_u *
handle_import(
	char_u	    *arg_ਸ਼ੁਰੂ,
	garray_T    *gap,
	ਪੂਰਨ	    import_sid,
	evalarg_T   *evalarg,
	ਖਾਲੀ	    *cctx)
{
    char_u	*arg = arg_ਸ਼ੁਰੂ;
    char_u	*nextarg;
    ਪੂਰਨ		is_autoload = FALSE;
    ਪੂਰਨ		getnext;
    char_u	*ਵਿਅੰਜਕ_ਸਿਰਾ;
    ਪੂਰਨ		ret = FAIL;
    char_u	*as_name = ਨਿਰਾ;
    typval_T	tv;
    ਪੂਰਨ		sid = -2;
    ਪੂਰਨ		res;
    ਲੰਮਾ	ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    ਪੂਰਨ		i;

    ਜੇ (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, ਨਿਰਾ, evalarg, &ਵਿਅੰਜਕ_ਸਿਰਾ);
    ਜੇ (ret == FAIL)
	ਜਾਓ erret;
    ਜੇ (tv.v_type != VAR_STRING
		       || tv.vval.v_string == ਨਿਰਾ || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	ਜਾਓ erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = ਸ਼ੁਰੂ_lnum;

    /*
     * find script file
     */
    ਜੇ (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
	char_u		*ਪੂਛ = gettail(si->sn_name);
	char_u		*from_name;
	ਪੂਰਨ		sourced_from_nofile_buf = FALSE;

	ਜੇ (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	ਜੇ (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(ਪੂਛ) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    ਜੇ (from_name == ਨਿਰਾ)
		ਜਾਓ erret;
	    vim_strncpy(from_name, si->sn_name, ਪੂਛ - si->sn_name);
	    ਜੋੜੋ_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	ਨਹੀਂਤਾਂ
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    ਨਹੀਂਤਾਂ ਜੇ (mch_is_ਭਰਿਆ_Name(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    ਨਹੀਂਤਾਂ ਜੇ (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ਜੇ (from_name == ਨਿਰਾ)
	    ਜਾਓ erret;
	vim_snprintf((ਅੱਖਰ *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = ਲੱਭੋ_script_ਵਿੱਚ_rtp(from_name);
	vim_free(from_name);
	ਜੇ (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sid);

	    ਜੇ (si->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
		si->sn_autoload_ਅਗੇਤਰ = get_autoload_ਅਗੇਤਰ(si);
	    res = OK;
	    ਜੇ (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(ਖਾਲੀ)do_ਸਰੋਤ(si->sn_name, FALSE, DOSO_NONE, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    res = FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ਜੇ (from_name == ਨਿਰਾ)
	    ਜਾਓ erret;
	vim_snprintf((ਅੱਖਰ *)from_name, len, "import/%s", tv.vval.v_string);
	res = ਸਰੋਤ_ਵਿੱਚ_ਰਸਤਾ(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    ਜੇ (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_ਰਸਤਾ
		    : e_could_not_import_str), tv.vval.v_string);
	ਜਾਓ erret;
    }

    ਜੇ (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	ਜਾਓ erret;
    }

    import_gap = gap != ਨਿਰਾ ? gap : &SCRIPT_ਵਸਤੂ(import_sid)->sn_imports;
    ਲਈ (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_ਅੰਕੜੇ + i;

	ਜੇ (import->imp_sid == sid)
	{
	    ਜੇ (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		ਰੋਕੋ;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    ਜਾਓ erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_ਅਗਲਾ_non_blank(ਵਿਅੰਜਕ_ਸਿਰਾ, evalarg, &getnext);
    ਜੇ (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	ਜੇ (getnext)
	    arg = eval_ਅਗਲਾ_ਸਤਰ(ਵਿਅੰਜਕ_ਸਿਰਾ, evalarg);
	ਨਹੀਂਤਾਂ
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	ਜੇ (eval_isnamec1(*arg))
	    ਜਦੋਂ (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	ਜੇ (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_ਵਿੱਚ_import_str), p);
	    ਜਾਓ erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *ਸਿਰਾ = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)p, ".vim");

	ਜੇ (!ਸਿਰੇ_excmd2(arg_ਸ਼ੁਰੂ, ਵਿਅੰਜਕ_ਸਿਰਾ))
	{
	    semsg(_(e_trailing_characters_str), ਵਿਅੰਜਕ_ਸਿਰਾ);
	    ਜਾਓ erret;
	}
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ || ਸਿਰਾ[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_ਸਿਰਾ_ਵਿੱਚ_dot_vim_str), p);
	    ਜਾਓ erret;
	}
	ਜੇ (ਸਿਰਾ == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    ਜਾਓ erret;
	}
	as_name = vim_strnsave(p, ਸਿਰਾ - p);
    }

    ਜੇ (as_name != ਨਿਰਾ)
    {
	imported_T  *imported;

	imported = ਲੱਭੋ_imported(as_name, STRLEN(as_name), FALSE);
	ਜੇ (imported != ਨਿਰਾ && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    ਜਾਓ erret;
	}
	ਨਹੀਂਤਾਂ ਜੇ (imported == ਨਿਰਾ
		&& check_defined(as_name, STRLEN(as_name), cctx, ਨਿਰਾ,
								FALSE) == FAIL)
	    ਜਾਓ erret;

	ਜੇ (imported == ਨਿਰਾ)
	{
	    imported = new_imported(import_gap);
	    ਜੇ (imported == ਨਿਰਾ)
		ਜਾਓ erret;
	    imported->imp_name = as_name;
	    as_name = ਨਿਰਾ;
	    imported->imp_sid = sid;
	    ਜੇ (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    ਵਾਪਸ arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    ਖਾਲੀ
ex_import(exarg_T *eap)
{
    char_u	*cmd_ਸਿਰਾ;
    evalarg_T	evalarg;

    ਜੇ (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
	ਵਾਪਸ;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_ਸਿਰਾ = handle_import(eap->arg, ਨਿਰਾ, current_sctx.sc_sid,
							       &evalarg, ਨਿਰਾ);
    ਜੇ (cmd_ਸਿਰਾ != ਨਿਰਾ)
	ਸੈੱਟ_nextcmd(eap, cmd_ਸਿਰਾ);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    ਖਾਲੀ
import_check_sourced_sid(ਪੂਰਨ *sid)
{
    scriptitem_T *script = SCRIPT_ਵਸਤੂ(*sid);

    ਜੇ (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    ਪੂਰਨ
ਲੱਭੋ_exported(
	ਪੂਰਨ	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ਪੂਰਨ	    verbose)
{
    ਪੂਰਨ		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ਵਸਤੂ(sid);

    *ufunc = ਨਿਰਾ;

    ਜੇ (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	ਜੇ (do_ਸਰੋਤ(script->sn_name, FALSE, DOSO_NONE, ਨਿਰਾ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    ਵਾਪਸ -1;
	}
    }

    // Find name in "script".
    idx = get_script_ਵਸਤੂ_idx(sid, name, 0, cctx, cstack);
    ਜੇ (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_ਅੰਕੜੇ) + idx;
	ਜੇ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    ਜੇ (verbose)
		semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str), name);
	    ਵਾਪਸ -1;
	}
	*type = sv->sv_type;
    }
    ਨਹੀਂਤਾਂ
    {
	size_t	len = STRLEN(name);
	char_u	ਬਫਰ[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	ਜੇ (script->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	    len += STRLEN(script->sn_autoload_ਅਗੇਤਰ) + 2;
	ਨਹੀਂਤਾਂ
	    len += 15;

	ਜੇ (len < ਆਕਾਰ(ਬਫਰ))
	    funcname = ਬਫਰ;
	ਨਹੀਂਤਾਂ
	{
	    funcname = alloc(len);
	    ਜੇ (funcname == ਨਿਰਾ)
		ਵਾਪਸ -1;
	}
	ਜੇ (script->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	{
	    sprintf((ਅੱਖਰ *)funcname, "%s%s", script->sn_autoload_ਅਗੇਤਰ, name);
	}
	ਨਹੀਂਤਾਂ
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (ਪੂਰਨ)KE_SNR;
	    sprintf((ਅੱਖਰ *)funcname + 3, "%ld_%s", (ਲੰਮਾ)sid, name);
	}
	*ufunc = ਲੱਭੋ_func(funcname, FALSE);

	ਜੇ (*ufunc == ਨਿਰਾ)
	{
	    ਜੇ (verbose)
	    {
		ufunc_T *alt_ufunc = ਨਿਰਾ;

		ਜੇ (script->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (ਪੂਰਨ)KE_SNR;
		    sprintf((ਅੱਖਰ *)funcname + 3, "%ld_%s", (ਲੰਮਾ)sid, name);
		    alt_ufunc = ਲੱਭੋ_func(funcname, FALSE);
		}
		ਜੇ (alt_ufunc != ਨਿਰਾ)
		    semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str), name);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_ਵਸਤੂ_not_ਮਿਲਿਆ_ਵਿੱਚ_script_str), name);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    ਜੇ (verbose)
		semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str), name);
	    *ufunc = ਨਿਰਾ;
	}
	ਜੇ (funcname != ਬਫਰ)
	    vim_free(funcname);
    }

    ਵਾਪਸ idx;
}

/*
 * Declare a script-local variable without init: "var name: type".
 * "const" is an error since the value is missing.
 * Returns a pointer to after the type.
 */
    char_u *
vim9_declare_scriptvar(exarg_T *eap, char_u *arg)
{
    char_u	    *p;
    char_u	    *name;
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    type_T	    *type;
    typval_T	    init_tv;

    ਜੇ (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	ਜੇ (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_ਮੁੱਲ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_const_requires_a_ਮੁੱਲ));
	ਵਾਪਸ arg + STRLEN(arg);
    }

    // Check for valid starting character.
    ਜੇ (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	ਵਾਪਸ arg + STRLEN(arg);
    }

    ਲਈ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	ਜੇ (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    ਰੋਕੋ;

    ਜੇ (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	ਵਾਪਸ arg + STRLEN(arg);
    }
    ਜੇ (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	ਵਾਪਸ arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, ਨਿਰਾ, ਨਿਰਾ, TRUE);
    ਜੇ (type == ਨਿਰਾ
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	ਵਾਪਸ p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_ਖੇਤਰ(init_tv);
    ਜੇ (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_ਸੰਖਿਆ;
    ਨਹੀਂਤਾਂ
	init_tv.v_type = type->tt_type;
    ਸੈੱਟ_var_const(name, 0, type, &init_tv, FALSE, ਸੌਂਪੋ_INIT, 0);

    vim_free(name);
    ਵਾਪਸ p;
}

/*
 * Vim9 part of adding a script variable: add it to sn_all_vars (lookup by name
 * with a hashtable) and sn_var_vals (lookup by index).
 * When "create" is TRUE this is a new variable, otherwise find and update an
 * existing variable.
 * "flags" can have ASSIGN_FINAL, ASSIGN_CONST or ASSIGN_INIT.
 * When "*type" is NULL use "tv" for the type and update "*type".  If
 * "do_member" is TRUE also use the member type, otherwise use "any".
 */
    ਖਾਲੀ
update_vim9_script_var(
	ਪੂਰਨ	    create,
	dictitem_T  *di,
	char_u	    *name,
	ਪੂਰਨ	    flags,
	typval_T    *tv,
	type_T	    **type,
	ਪੂਰਨ	    do_member)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = ਨਿਰਾ;

    ਜੇ (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = ਨਿਰਾ;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	ਜੇ (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    ਵਾਪਸ;

	hi = hash_ਲੱਭੋ(&si->sn_all_vars.dv_hashtab, name);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    ਲਈ (sav = HI2SAV(hi); ; sav = sav->sav_ਅਗਲਾ)
	    {
		ਜੇ (sav->sav_ਖੰਡ_id == si->sn_current_ਖੰਡ_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    ਜੇ (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    ਰੋਕੋ;
		}
		ਜੇ (sav->sav_ਅਗਲਾ == ਨਿਰਾ)
		    ਰੋਕੋ;
	    }
	}

	ਜੇ (sv == ਨਿਰਾ)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_ਕੁੰਜੀ) + STRLEN(name) + 1);
	    ਜੇ (newsav == ਨਿਰਾ)
		ਵਾਪਸ;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ਸੌਂਪੋ_FINAL) ? ਸੌਂਪੋ_FINAL
				   : (flags & ਸੌਂਪੋ_CONST) ? ਸੌਂਪੋ_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    ਜੇ ((flags & ਸੌਂਪੋ_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_ਕੁੰਜੀ[0], name);
	    sv->sv_name = newsav->sav_ਕੁੰਜੀ;
	    newsav->sav_di = di;
	    newsav->sav_ਖੰਡ_id = si->sn_current_ਖੰਡ_id;

	    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		// new variable name
		hash_ਜੋੜੋ(&si->sn_all_vars.dv_hashtab, newsav->sav_ਕੁੰਜੀ,
							       "add variable");
	    ਨਹੀਂਤਾਂ ਜੇ (sav != ਨਿਰਾ)
		// existing name in a new block, append to the list
		sav->sav_ਅਗਲਾ = newsav;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(&di->di_tv, 0, TRUE);
    }
    ਜੇ (sv != ਨਿਰਾ)
    {
	ਜੇ (*type == ਨਿਰਾ)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	ਨਹੀਂਤਾਂ ਜੇ ((flags & ਸੌਂਪੋ_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == ਨਿਰਾ)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	ਜੇ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	ਜੇ (*type != ਨਿਰਾ && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	ਨਹੀਂਤਾਂ
	{
	    sv->sv_type = *type;
	    sv->sv_flags &= ~SVFLAG_TYPE_ALLOCATED;
	}
    }

    // let ex_export() know the export worked.
    is_export = FALSE;
}

/*
 * Hide a script variable when leaving a block.
 * "idx" is de index in sn_var_vals.
 * When "func_defined" is non-zero then a function was defined in this block,
 * the variable may be accessed by it.  Otherwise the variable can be cleared.
 */
    ਖਾਲੀ
hide_script_var(scriptitem_T *si, ਪੂਰਨ idx, ਪੂਰਨ func_defined)
{
    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;
    hashtab_T	*script_ht = get_script_local_ht();
    hashtab_T	*all_ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*script_hi;
    hashitem_T	*all_hi;

    // Remove a variable declared inside the block, if it still exists.
    // If it was added in a nested block it will already have been removed.
    // The typval is moved into the sallvar_T.
    script_hi = hash_ਲੱਭੋ(script_ht, sv->sv_name);
    all_hi = hash_ਲੱਭੋ(all_ht, sv->sv_name);

    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(script_hi) || HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(all_hi))
	ਵਾਪਸ;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = ਨਿਰਾ;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    ਜਦੋਂ (sav != ਨਿਰਾ && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_ਅਗਲਾ;
    }
    ਜੇ (sav == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = ਨਿਰਾ;
	sv->sv_tv = &sav->sav_tv;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (sav_prev == ਨਿਰਾ)
	    hash_remove(all_ht, all_hi, "hide variable");
	ਨਹੀਂਤਾਂ
	    sav_prev->sav_ਅਗਲਾ = sav->sav_ਅਗਲਾ;
	sv->sv_name = ਨਿਰਾ;
	vim_free(sav);
    }
    delete_var(script_ht, script_hi);
}

/*
 * Find the script-local variable that links to "dest".
 * If "sid" is zero use the current script.
 * if "must_find" is TRUE and "dest" cannot be found report an internal error.
 * Returns NULL if not found and give an internal error.
 */
    svar_T *
ਲੱਭੋ_typval_ਵਿੱਚ_script(typval_T *dest, scid_T sid, ਪੂਰਨ must_ਲੱਭੋ)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sid == 0 ? current_sctx.sc_sid : sid);
    ਪੂਰਨ		    idx;

    ਜੇ (si->sn_ਸੰਸਕਰਣ != SCRIPT_ਸੰਸਕਰਣ_VIM9)
	// legacy script doesn't store variable types
	ਵਾਪਸ ਨਿਰਾ;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    ਲਈ (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	ਜੇ (sv->sv_name != ਨਿਰਾ && sv->sv_tv == dest)
	    ਵਾਪਸ sv;
    }
    ਜੇ (must_ਲੱਭੋ)
	iemsg("find_typval_in_script(): not found");
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    ਪੂਰਨ
check_script_var_type(
	svar_T	    *sv,
	typval_T    *ਮੁੱਲ,
	char_u	    *name,
	where_T	    where)
{
    ਪੂਰਨ	    ret;

    ਜੇ (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ਵਾਪਸ FAIL;
    }
    ret = check_typval_type(sv->sv_type, ਮੁੱਲ, where);
    ਜੇ (ret == OK && need_convert_ਮੰਜ਼ਿਲ_bool(sv->sv_type, ਮੁੱਲ))
    {
	ਪੂਰਨ	val = tv2bool(ਮੁੱਲ);

	clear_tv(ਮੁੱਲ);
	ਮੁੱਲ->v_type = VAR_BOOL;
	ਮੁੱਲ->v_ਤਾਲਾ = 0;
	ਮੁੱਲ->vval.v_ਸੰਖਿਆ = val ? VVAL_TRUE : VVAL_FALSE;
    }
    ਵਾਪਸ ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
ਸਥਿਰ ਅੱਖਰ *reserved[] = {
    "false",
    "null",
    "null_blob",
    "null_channel",
    "null_class",
    "null_dict",
    "null_function",
    "null_job",
    "null_list",
    "null_object",
    "null_partial",
    "null_string",
    "null_tuple",
    "super",
    "this",
    "true",
};

/*
 * String compare function used for bsearch()
 */
    ਸਥਿਰ ਪੂਰਨ
comp_ਨਾਂ(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਵਾਪਸ STRCMP(*(ਅੱਖਰ **)s1, *(ਅੱਖਰ **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    ਪੂਰਨ
check_reserved_name(char_u *name, ਪੂਰਨ is_objm_access)
{
    // "this" can be used in an object method
    ਜੇ (is_objm_access && STRCMP("this", name) == 0)
	ਵਾਪਸ OK;

    ਜੇ (bsearch(&name, reserved, ARRAY_ਲੰਮਾਈ(reserved),
				ਆਕਾਰ(reserved[0]), comp_ਨਾਂ) != ਨਿਰਾ)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
