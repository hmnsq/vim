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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    പൂർണ്ണസംഖ്യ
in_old_script(പൂർണ്ണസംഖ്യ max_version)
{
    തിരികെ (current_sctx.sc_version < max_version
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    പൂർണ്ണസംഖ്യ
current_script_is_vim9(ശൂന്യം)
{
    തിരികെ SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
						       == SCRIPT_VERSION_VIM9;
}
#endif

#ifdef FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    ശൂന്യം
clear_vim9_scriptlocal_vars(പൂർണ്ണസംഖ്യ sid)
{
    hashtab_T	*ht = &SCRIPT_VARS(sid);

    hashtab_free_contents(ht);
    hash_init(ht);
    delete_script_functions(sid);

    // also the exported variables of an autoload script
    delete_autoload_export_vars(SCRIPT_ITEM(sid)->sn_autoload_prefix);

    // old imports and script variables are no longer valid
    free_imports_and_script_vars(sid);
}
#endif

/*
 * ":vim9script".
 */
    ശൂന്യം
ex_vim9script(exarg_T *eap UNUSED)
{
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    പൂർണ്ണസംഖ്യ		    found_noclear = FALSE;
    char_u	    *p;

    എങ്കിൽ (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_used_in_script));
	തിരികെ;
    }

    si = SCRIPT_ITEM(sid);
    എങ്കിൽ (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_first_command_in_script));
	തിരികെ;
    }

    വേണ്ടി (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	എങ്കിൽ (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    എങ്കിൽ (found_noclear)
	    {
		semsg(_(e_duplicate_argument_str), p);
		തിരികെ;
	    }
	    found_noclear = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    തിരികെ;
	}
    }

    എങ്കിൽ (si->sn_state == SN_STATE_RELOAD && !found_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    എങ്കിൽ (si->sn_autoload_prefix == ശൂന്യ)
	si->sn_autoload_prefix = get_autoload_prefix(si);

    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    si->sn_version = SCRIPT_VERSION_VIM9;

    എങ്കിൽ (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	set_option_മൂല്യം_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#അല്ലെങ്കിൽ
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    പൂർണ്ണസംഖ്യ
not_in_vim9(exarg_T *eap, bool silent)
{
    എങ്കിൽ (in_vim9script())
	തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
	{
	    അവസ്ഥ CMD_k:
		എങ്കിൽ (eap->addr_എണ്ണം > 0)
		{
		    എങ്കിൽ (!silent)
			emsg(_(e_no_range_allowed));
		    തിരികെ FAIL;
		}
		// FALLTHROUGH
	    അവസ്ഥ CMD_append:
	    അവസ്ഥ CMD_change:
	    അവസ്ഥ CMD_തിരുകുക:
	    അവസ്ഥ CMD_open:
	    അവസ്ഥ CMD_t:
	    അവസ്ഥ CMD_xit:
		എങ്കിൽ (!silent)
		    semsg(_(e_command_not_supported_in_vim9_script_missing_var_str), eap->cmd);
		തിരികെ FAIL;
	    സ്ഥിരസ്ഥിതി: നിർത്തുക;
	}
    തിരികെ OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    പൂർണ്ണസംഖ്യ
vim9_bad_comment(char_u *p)
{
    എങ്കിൽ (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	എങ്കിൽ (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_to_start_comment));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    പൂർണ്ണസംഖ്യ
vim9_comment_start(char_u *p)
{
#ifdef FEAT_EVAL
    തിരികെ p[0] == '#' && !vim9_bad_comment(p);
#അല്ലെങ്കിൽ
    തിരികെ p[0] == '#' && (p[1] != '{' || p[2] == '{');
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    ശൂന്യം
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    എങ്കിൽ (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	തിരികെ;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    എങ്കിൽ (eap->cmd == ശൂന്യ)
	തിരികെ;
    vim_snprintf((അക്ഷരം *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = ശൂന്യ;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    ശൂന്യം
ex_export(exarg_T *eap UNUSED)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_used_in_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    സ്ഥിതം imported_T *
new_imported(garray_T *gap)
{
    എങ്കിൽ (ga_grow(gap, 1) == OK)
	തിരികെ ((imported_T *)gap->ga_data + gap->ga_len++);
    തിരികെ ശൂന്യ;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    സ്ഥിതം ശൂന്യം
free_all_script_vars(scriptitem_T *si)
{
    പൂർണ്ണസംഖ്യ		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_next;
    പൂർണ്ണസംഖ്യ		idx;

    hash_lock(ht);
    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    ആകുമ്പോൾ (sav != ശൂന്യ)
	    {
		sav_next = sav->sav_next;
		എങ്കിൽ (sav->sav_di == ശൂന്യ)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_next;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    വേണ്ടി (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	എങ്കിൽ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    ശൂന്യം
free_imports_and_script_vars(പൂർണ്ണസംഖ്യ sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    പൂർണ്ണസംഖ്യ		    idx;

    വേണ്ടി (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

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
    ശൂന്യം
mark_imports_for_reload(പൂർണ്ണസംഖ്യ sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    പൂർണ്ണസംഖ്യ		    idx;

    വേണ്ടി (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_import_fname(char_u *fname, പൂർണ്ണസംഖ്യ is_autoload, പൂർണ്ണസംഖ്യ *sid)
{
    എങ്കിൽ (is_autoload)
    {
	scriptitem_T	*si;

	*sid = find_script_by_name(fname);
	എങ്കിൽ (*sid < 0)
	{
	    പൂർണ്ണസംഖ്യ error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    എങ്കിൽ (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		തിരികെ FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    എങ്കിൽ (error == FAIL)
		തിരികെ FAIL;
	}

	si = SCRIPT_ITEM(*sid);
	si->sn_import_autoload = TRUE;

	എങ്കിൽ (si->sn_autoload_prefix == ശൂന്യ)
	    si->sn_autoload_prefix = get_autoload_prefix(si);

	// with testing override: load autoload script right away
	എങ്കിൽ (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    തിരികെ OK;
    }
    തിരികെ do_source(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    സ്ഥിതം char_u *
handle_import(
	char_u	    *arg_start,
	garray_T    *gap,
	പൂർണ്ണസംഖ്യ	    import_sid,
	evalarg_T   *evalarg,
	ശൂന്യം	    *cctx)
{
    char_u	*arg = arg_start;
    char_u	*nextarg;
    പൂർണ്ണസംഖ്യ		is_autoload = FALSE;
    പൂർണ്ണസംഖ്യ		getnext;
    char_u	*വ്യഞ്ജകം_end;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    char_u	*as_name = ശൂന്യ;
    typval_T	tv;
    പൂർണ്ണസംഖ്യ		sid = -2;
    പൂർണ്ണസംഖ്യ		res;
    long	start_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, ശൂന്യ, evalarg, &വ്യഞ്ജകം_end);
    എങ്കിൽ (ret == FAIL)
	പോകുക erret;
    എങ്കിൽ (tv.v_type != VAR_STRING
		       || tv.vval.v_string == ശൂന്യ || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	പോകുക erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = start_lnum;

    /*
     * find script file
     */
    എങ്കിൽ (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	char_u		*tail = gettail(si->sn_name);
	char_u		*from_name;
	പൂർണ്ണസംഖ്യ		sourced_from_nofile_buf = FALSE;

	എങ്കിൽ (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	എങ്കിൽ (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(tail) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    എങ്കിൽ (from_name == ശൂന്യ)
		പോകുക erret;
	    vim_strncpy(from_name, si->sn_name, tail - si->sn_name);
	    add_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	അല്ലെങ്കിൽ
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (mch_isFullName(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	എങ്കിൽ (from_name == ശൂന്യ)
	    പോകുക erret;
	vim_snprintf((അക്ഷരം *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = find_script_in_rtp(from_name);
	vim_free(from_name);
	എങ്കിൽ (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ITEM(sid);

	    എങ്കിൽ (si->sn_autoload_prefix == ശൂന്യ)
		si->sn_autoload_prefix = get_autoload_prefix(si);
	    res = OK;
	    എങ്കിൽ (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(ശൂന്യം)do_source(si->sn_name, FALSE, DOSO_NONE, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	    res = FAIL;
    }
    അല്ലെങ്കിൽ
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	എങ്കിൽ (from_name == ശൂന്യ)
	    പോകുക erret;
	vim_snprintf((അക്ഷരം *)from_name, len, "import/%s", tv.vval.v_string);
	res = source_in_path(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    എങ്കിൽ (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_path
		    : e_could_not_import_str), tv.vval.v_string);
	പോകുക erret;
    }

    എങ്കിൽ (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	പോകുക erret;
    }

    import_gap = gap != ശൂന്യ ? gap : &SCRIPT_ITEM(import_sid)->sn_imports;
    വേണ്ടി (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_data + i;

	എങ്കിൽ (import->imp_sid == sid)
	{
	    എങ്കിൽ (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		നിർത്തുക;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    പോകുക erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_next_non_blank(വ്യഞ്ജകം_end, evalarg, &getnext);
    എങ്കിൽ (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	എങ്കിൽ (getnext)
	    arg = eval_next_വരി(വ്യഞ്ജകം_end, evalarg);
	അല്ലെങ്കിൽ
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	എങ്കിൽ (eval_isnamec1(*arg))
	    ആകുമ്പോൾ (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	എങ്കിൽ (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_in_import_str), p);
	    പോകുക erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    അല്ലെങ്കിൽ
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *end = (char_u *)strstr((അക്ഷരം *)p, ".vim");

	എങ്കിൽ (!ends_excmd2(arg_start, വ്യഞ്ജകം_end))
	{
	    semsg(_(e_trailing_characters_str), വ്യഞ്ജകം_end);
	    പോകുക erret;
	}
	എങ്കിൽ (end == ശൂന്യ || end[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_end_in_dot_vim_str), p);
	    പോകുക erret;
	}
	എങ്കിൽ (end == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    പോകുക erret;
	}
	as_name = vim_strnsave(p, end - p);
    }

    എങ്കിൽ (as_name != ശൂന്യ)
    {
	imported_T  *imported;

	imported = find_imported(as_name, STRLEN(as_name), FALSE);
	എങ്കിൽ (imported != ശൂന്യ && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    പോകുക erret;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (imported == ശൂന്യ
		&& check_defined(as_name, STRLEN(as_name), cctx, ശൂന്യ,
								FALSE) == FAIL)
	    പോകുക erret;

	എങ്കിൽ (imported == ശൂന്യ)
	{
	    imported = new_imported(import_gap);
	    എങ്കിൽ (imported == ശൂന്യ)
		പോകുക erret;
	    imported->imp_name = as_name;
	    as_name = ശൂന്യ;
	    imported->imp_sid = sid;
	    എങ്കിൽ (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    തിരികെ arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    ശൂന്യം
ex_import(exarg_T *eap)
{
    char_u	*cmd_end;
    evalarg_T	evalarg;

    എങ്കിൽ (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_used_in_script));
	തിരികെ;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_end = handle_import(eap->arg, ശൂന്യ, current_sctx.sc_sid,
							       &evalarg, ശൂന്യ);
    എങ്കിൽ (cmd_end != ശൂന്യ)
	set_nextcmd(eap, cmd_end);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    ശൂന്യം
import_check_sourced_sid(പൂർണ്ണസംഖ്യ *sid)
{
    scriptitem_T *script = SCRIPT_ITEM(*sid);

    എങ്കിൽ (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    പൂർണ്ണസംഖ്യ
find_exported(
	പൂർണ്ണസംഖ്യ	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	പൂർണ്ണസംഖ്യ	    verbose)
{
    പൂർണ്ണസംഖ്യ		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ITEM(sid);

    *ufunc = ശൂന്യ;

    എങ്കിൽ (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	എങ്കിൽ (do_source(script->sn_name, FALSE, DOSO_NONE, ശൂന്യ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    തിരികെ -1;
	}
    }

    // Find name in "script".
    idx = get_script_item_idx(sid, name, 0, cctx, cstack);
    എങ്കിൽ (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_data) + idx;
	എങ്കിൽ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    തിരികെ -1;
	}
	*type = sv->sv_type;
    }
    അല്ലെങ്കിൽ
    {
	size_t	len = STRLEN(name);
	char_u	buffer[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	എങ്കിൽ (script->sn_autoload_prefix != ശൂന്യ)
	    len += STRLEN(script->sn_autoload_prefix) + 2;
	അല്ലെങ്കിൽ
	    len += 15;

	എങ്കിൽ (len < വലുപ്പം(buffer))
	    funcname = buffer;
	അല്ലെങ്കിൽ
	{
	    funcname = alloc(len);
	    എങ്കിൽ (funcname == ശൂന്യ)
		തിരികെ -1;
	}
	എങ്കിൽ (script->sn_autoload_prefix != ശൂന്യ)
	{
	    sprintf((അക്ഷരം *)funcname, "%s%s", script->sn_autoload_prefix, name);
	}
	അല്ലെങ്കിൽ
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
	    sprintf((അക്ഷരം *)funcname + 3, "%ld_%s", (long)sid, name);
	}
	*ufunc = find_func(funcname, FALSE);

	എങ്കിൽ (*ufunc == ശൂന്യ)
	{
	    എങ്കിൽ (verbose)
	    {
		ufunc_T *alt_ufunc = ശൂന്യ;

		എങ്കിൽ (script->sn_autoload_prefix != ശൂന്യ)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
		    sprintf((അക്ഷരം *)funcname + 3, "%ld_%s", (long)sid, name);
		    alt_ufunc = find_func(funcname, FALSE);
		}
		എങ്കിൽ (alt_ufunc != ശൂന്യ)
		    semsg(_(e_item_not_exported_in_script_str), name);
		അല്ലെങ്കിൽ
		    semsg(_(e_item_not_found_in_script_str), name);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    *ufunc = ശൂന്യ;
	}
	എങ്കിൽ (funcname != buffer)
	    vim_free(funcname);
    }

    തിരികെ idx;
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
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    type_T	    *type;
    typval_T	    init_tv;

    എങ്കിൽ (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	എങ്കിൽ (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_മൂല്യം));
	അല്ലെങ്കിൽ
	    emsg(_(e_const_requires_a_മൂല്യം));
	തിരികെ arg + STRLEN(arg);
    }

    // Check for valid starting character.
    എങ്കിൽ (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	തിരികെ arg + STRLEN(arg);
    }

    വേണ്ടി (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	എങ്കിൽ (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    നിർത്തുക;

    എങ്കിൽ (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	തിരികെ arg + STRLEN(arg);
    }
    എങ്കിൽ (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	തിരികെ arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, ശൂന്യ, ശൂന്യ, TRUE);
    എങ്കിൽ (type == ശൂന്യ
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	തിരികെ p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_FIELD(init_tv);
    എങ്കിൽ (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_സംഖ്യ;
    അല്ലെങ്കിൽ
	init_tv.v_type = type->tt_type;
    set_var_const(name, 0, type, &init_tv, FALSE, ASSIGN_INIT, 0);

    vim_free(name);
    തിരികെ p;
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
    ശൂന്യം
update_vim9_script_var(
	പൂർണ്ണസംഖ്യ	    create,
	dictitem_T  *di,
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    flags,
	typval_T    *tv,
	type_T	    **type,
	പൂർണ്ണസംഖ്യ	    do_member)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = ശൂന്യ;

    എങ്കിൽ (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = ശൂന്യ;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	എങ്കിൽ (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    തിരികെ;

	hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    വേണ്ടി (sav = HI2SAV(hi); ; sav = sav->sav_next)
	    {
		എങ്കിൽ (sav->sav_block_id == si->sn_current_block_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_data)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    എങ്കിൽ (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    നിർത്തുക;
		}
		എങ്കിൽ (sav->sav_next == ശൂന്യ)
		    നിർത്തുക;
	    }
	}

	എങ്കിൽ (sv == ശൂന്യ)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_data) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_താക്കോൽ) + STRLEN(name) + 1);
	    എങ്കിൽ (newsav == ശൂന്യ)
		തിരികെ;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ASSIGN_FINAL) ? ASSIGN_FINAL
				   : (flags & ASSIGN_CONST) ? ASSIGN_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    എങ്കിൽ ((flags & ASSIGN_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_താക്കോൽ[0], name);
	    sv->sv_name = newsav->sav_താക്കോൽ;
	    newsav->sav_di = di;
	    newsav->sav_block_id = si->sn_current_block_id;

	    എങ്കിൽ (HASHITEM_EMPTY(hi))
		// new variable name
		hash_add(&si->sn_all_vars.dv_hashtab, newsav->sav_താക്കോൽ,
							       "add variable");
	    അല്ലെങ്കിൽ എങ്കിൽ (sav != ശൂന്യ)
		// existing name in a new block, append to the list
		sav->sav_next = newsav;
	}
    }
    അല്ലെങ്കിൽ
    {
	sv = find_typval_in_script(&di->di_tv, 0, TRUE);
    }
    എങ്കിൽ (sv != ശൂന്യ)
    {
	എങ്കിൽ (*type == ശൂന്യ)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	അല്ലെങ്കിൽ എങ്കിൽ ((flags & ASSIGN_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == ശൂന്യ)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	എങ്കിൽ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	എങ്കിൽ (*type != ശൂന്യ && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	അല്ലെങ്കിൽ
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
    ശൂന്യം
hide_script_var(scriptitem_T *si, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ func_defined)
{
    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
    hashtab_T	*script_ht = get_script_local_ht();
    hashtab_T	*all_ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*script_hi;
    hashitem_T	*all_hi;

    // Remove a variable declared inside the block, if it still exists.
    // If it was added in a nested block it will already have been removed.
    // The typval is moved into the sallvar_T.
    script_hi = hash_find(script_ht, sv->sv_name);
    all_hi = hash_find(all_ht, sv->sv_name);

    എങ്കിൽ (HASHITEM_EMPTY(script_hi) || HASHITEM_EMPTY(all_hi))
	തിരികെ;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = ശൂന്യ;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    ആകുമ്പോൾ (sav != ശൂന്യ && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_next;
    }
    എങ്കിൽ (sav == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = ശൂന്യ;
	sv->sv_tv = &sav->sav_tv;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (sav_prev == ശൂന്യ)
	    hash_remove(all_ht, all_hi, "hide variable");
	അല്ലെങ്കിൽ
	    sav_prev->sav_next = sav->sav_next;
	sv->sv_name = ശൂന്യ;
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
find_typval_in_script(typval_T *dest, scid_T sid, പൂർണ്ണസംഖ്യ must_find)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid == 0 ? current_sctx.sc_sid : sid);
    പൂർണ്ണസംഖ്യ		    idx;

    എങ്കിൽ (si->sn_version != SCRIPT_VERSION_VIM9)
	// legacy script doesn't store variable types
	തിരികെ ശൂന്യ;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    വേണ്ടി (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	എങ്കിൽ (sv->sv_name != ശൂന്യ && sv->sv_tv == dest)
	    തിരികെ sv;
    }
    എങ്കിൽ (must_find)
	iemsg("find_typval_in_script(): not found");
    തിരികെ ശൂന്യ;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    പൂർണ്ണസംഖ്യ
check_script_var_type(
	svar_T	    *sv,
	typval_T    *മൂല്യം,
	char_u	    *name,
	where_T	    where)
{
    പൂർണ്ണസംഖ്യ	    ret;

    എങ്കിൽ (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	തിരികെ FAIL;
    }
    ret = check_typval_type(sv->sv_type, മൂല്യം, where);
    എങ്കിൽ (ret == OK && need_convert_to_bool(sv->sv_type, മൂല്യം))
    {
	പൂർണ്ണസംഖ്യ	val = tv2bool(മൂല്യം);

	clear_tv(മൂല്യം);
	മൂല്യം->v_type = VAR_BOOL;
	മൂല്യം->v_lock = 0;
	മൂല്യം->vval.v_സംഖ്യ = val ? VVAL_TRUE : VVAL_FALSE;
    }
    തിരികെ ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
സ്ഥിതം അക്ഷരം *reserved[] = {
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
comp_names(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    തിരികെ STRCMP(*(അക്ഷരം **)s1, *(അക്ഷരം **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    പൂർണ്ണസംഖ്യ
check_reserved_name(char_u *name, പൂർണ്ണസംഖ്യ is_objm_access)
{
    // "this" can be used in an object method
    എങ്കിൽ (is_objm_access && STRCMP("this", name) == 0)
	തിരികെ OK;

    എങ്കിൽ (bsearch(&name, reserved, ARRAY_LENGTH(reserved),
				വലുപ്പം(reserved[0]), comp_names) != ശൂന്യ)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

#endif // FEAT_EVAL
