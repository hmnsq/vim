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

#include "vim.h"

#නම් defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    පූර්ණ_සංඛ්‍යා
in_old_script(පූර්ණ_සංඛ්‍යා max_version)
{
    ආපසු (current_sctx.sc_version < max_version
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    පූර්ණ_සංඛ්‍යා
current_script_is_vim9(void)
{
    ආපසු SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
						       == SCRIPT_VERSION_VIM9;
}
#endif

#ifdef FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    void
clear_vim9_scriptlocal_vars(පූර්ණ_සංඛ්‍යා sid)
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
    void
ex_vim9script(exarg_T *eap UNUSED)
{
#ifdef FEAT_EVAL
    පූර්ණ_සංඛ්‍යා		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    පූර්ණ_සංඛ්‍යා		    found_noclear = FALSE;
    char_u	    *p;

    නම් (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_used_in_script));
	ආපසු;
    }

    si = SCRIPT_ITEM(sid);
    නම් (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_first_command_in_script));
	ආපසු;
    }

    සඳහා (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	නම් (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    නම් (found_noclear)
	    {
		semsg(_(e_duplicate_argument_str), p);
		ආපසු;
	    }
	    found_noclear = TRUE;
	}
	නැතහොත්
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ආපසු;
	}
    }

    නම් (si->sn_state == SN_STATE_RELOAD && !found_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    නම් (si->sn_autoload_prefix == NULL)
	si->sn_autoload_prefix = get_autoload_prefix(si);

    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    si->sn_version = SCRIPT_VERSION_VIM9;

    නම් (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	set_option_value_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#නැතහොත්
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
#endif
}

#නම් defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    පූර්ණ_සංඛ්‍යා
not_in_vim9(exarg_T *eap, bool silent)
{
    නම් (in_vim9script())
	switch (eap->cmdidx)
	{
	    pak_sa CMD_k:
		නම් (eap->addr_count > 0)
		{
		    නම් (!silent)
			emsg(_(e_no_range_allowed));
		    ආපසු FAIL;
		}
		// FALLTHROUGH
	    pak_sa CMD_append:
	    pak_sa CMD_change:
	    pak_sa CMD_insert:
	    pak_sa CMD_open:
	    pak_sa CMD_t:
	    pak_sa CMD_xit:
		නම් (!silent)
		    semsg(_(e_command_not_supported_in_vim9_script_missing_var_str), eap->cmd);
		ආපසු FAIL;
	    default: bha_nga;
	}
    ආපසු OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    පූර්ණ_සංඛ්‍යා
vim9_bad_comment(char_u *p)
{
    නම් (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	නම් (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_to_start_comment));
	ආපසු TRUE;
    }
    ආපසු FALSE;
}
#endif

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    පූර්ණ_සංඛ්‍යා
vim9_comment_start(char_u *p)
{
#ifdef FEAT_EVAL
    ආපසු p[0] == '#' && !vim9_bad_comment(p);
#නැතහොත්
    ආපසු p[0] == '#' && (p[1] != '{' || p[2] == '{');
#endif
}

#නම් defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    void
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    නම් (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ආපසු;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    නම් (eap->cmd == NULL)
	ආපසු;
    vim_snprintf((var_na *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = NULL;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    void
ex_export(exarg_T *eap UNUSED)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_used_in_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    static imported_T *
new_imported(garray_T *gap)
{
    නම් (ga_grow(gap, 1) == OK)
	ආපසු ((imported_T *)gap->ga_data + gap->ga_len++);
    ආපසු NULL;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    static void
free_all_script_vars(scriptitem_T *si)
{
    පූර්ණ_සංඛ්‍යා		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_next;
    පූර්ණ_සංඛ්‍යා		idx;

    hash_lock(ht);
    todo = (පූර්ණ_සංඛ්‍යා)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    අතරතුර (sav != NULL)
	    {
		sav_next = sav->sav_next;
		නම් (sav->sav_di == NULL)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_next;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    සඳහා (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	නම් (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    void
free_imports_and_script_vars(පූර්ණ_සංඛ්‍යා sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    පූර්ණ_සංඛ්‍යා		    idx;

    සඳහා (idx = 0; idx < si->sn_imports.ga_len; ++idx)
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
    void
mark_imports_for_reload(පූර්ණ_සංඛ්‍යා sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    පූර්ණ_සංඛ්‍යා		    idx;

    සඳහා (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
handle_import_fname(char_u *fname, පූර්ණ_සංඛ්‍යා is_autoload, පූර්ණ_සංඛ්‍යා *sid)
{
    නම් (is_autoload)
    {
	scriptitem_T	*si;

	*sid = find_script_by_name(fname);
	නම් (*sid < 0)
	{
	    පූර්ණ_සංඛ්‍යා error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    නම් (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		ආපසු FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    නම් (error == FAIL)
		ආපසු FAIL;
	}

	si = SCRIPT_ITEM(*sid);
	si->sn_import_autoload = TRUE;

	නම් (si->sn_autoload_prefix == NULL)
	    si->sn_autoload_prefix = get_autoload_prefix(si);

	// with testing override: load autoload script right away
	නම් (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    ආපසු OK;
    }
    ආපසු do_source(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    static char_u *
handle_import(
	char_u	    *arg_start,
	garray_T    *gap,
	පූර්ණ_සංඛ්‍යා	    import_sid,
	evalarg_T   *evalarg,
	void	    *cctx)
{
    char_u	*arg = arg_start;
    char_u	*nextarg;
    පූර්ණ_සංඛ්‍යා		is_autoload = FALSE;
    පූර්ණ_සංඛ්‍යා		getnext;
    char_u	*expr_end;
    පූර්ණ_සංඛ්‍යා		ret = FAIL;
    char_u	*as_name = NULL;
    typval_T	tv;
    පූර්ණ_සංඛ්‍යා		sid = -2;
    පූර්ණ_සංඛ්‍යා		res;
    long	start_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, NULL, evalarg, &expr_end);
    නම් (ret == FAIL)
	goto erret;
    නම් (tv.v_type != VAR_STRING
		       || tv.vval.v_string == NULL || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	goto erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = start_lnum;

    /*
     * find script file
     */
    නම් (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	char_u		*tail = gettail(si->sn_name);
	char_u		*from_name;
	පූර්ණ_සංඛ්‍යා		sourced_from_nofile_buf = FALSE;

	නම් (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	නම් (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(tail) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    නම් (from_name == NULL)
		goto erret;
	    vim_strncpy(from_name, si->sn_name, tail - si->sn_name);
	    add_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	නැතහොත්
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    නැතහොත් නම් (mch_isFullName(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    නැතහොත් නම් (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	නම් (from_name == NULL)
	    goto erret;
	vim_snprintf((var_na *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = find_script_in_rtp(from_name);
	vim_free(from_name);
	නම් (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ITEM(sid);

	    නම් (si->sn_autoload_prefix == NULL)
		si->sn_autoload_prefix = get_autoload_prefix(si);
	    res = OK;
	    නම් (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(void)do_source(si->sn_name, FALSE, DOSO_NONE, NULL);
	}
	නැතහොත්
	    res = FAIL;
    }
    නැතහොත්
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	නම් (from_name == NULL)
	    goto erret;
	vim_snprintf((var_na *)from_name, len, "import/%s", tv.vval.v_string);
	res = source_in_path(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    නම් (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_path
		    : e_could_not_import_str), tv.vval.v_string);
	goto erret;
    }

    නම් (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	goto erret;
    }

    import_gap = gap != NULL ? gap : &SCRIPT_ITEM(import_sid)->sn_imports;
    සඳහා (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_data + i;

	නම් (import->imp_sid == sid)
	{
	    නම් (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		bha_nga;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    goto erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_next_non_blank(expr_end, evalarg, &getnext);
    නම් (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	නම් (getnext)
	    arg = eval_next_line(expr_end, evalarg);
	නැතහොත්
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	නම් (eval_isnamec1(*arg))
	    අතරතුර (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	නම් (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_in_import_str), p);
	    goto erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    නැතහොත්
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *end = (char_u *)strstr((var_na *)p, ".vim");

	නම් (!ends_excmd2(arg_start, expr_end))
	{
	    semsg(_(e_trailing_characters_str), expr_end);
	    goto erret;
	}
	නම් (end == NULL || end[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_end_in_dot_vim_str), p);
	    goto erret;
	}
	නම් (end == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    goto erret;
	}
	as_name = vim_strnsave(p, end - p);
    }

    නම් (as_name != NULL)
    {
	imported_T  *imported;

	imported = find_imported(as_name, STRLEN(as_name), FALSE);
	නම් (imported != NULL && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    goto erret;
	}
	නැතහොත් නම් (imported == NULL
		&& check_defined(as_name, STRLEN(as_name), cctx, NULL,
								FALSE) == FAIL)
	    goto erret;

	නම් (imported == NULL)
	{
	    imported = new_imported(import_gap);
	    නම් (imported == NULL)
		goto erret;
	    imported->imp_name = as_name;
	    as_name = NULL;
	    imported->imp_sid = sid;
	    නම් (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    ආපසු arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    void
ex_import(exarg_T *eap)
{
    char_u	*cmd_end;
    evalarg_T	evalarg;

    නම් (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_used_in_script));
	ආපසු;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_end = handle_import(eap->arg, NULL, current_sctx.sc_sid,
							       &evalarg, NULL);
    නම් (cmd_end != NULL)
	set_nextcmd(eap, cmd_end);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    void
import_check_sourced_sid(පූර්ණ_සංඛ්‍යා *sid)
{
    scriptitem_T *script = SCRIPT_ITEM(*sid);

    නම් (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    පූර්ණ_සංඛ්‍යා
find_exported(
	පූර්ණ_සංඛ්‍යා	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	පූර්ණ_සංඛ්‍යා	    verbose)
{
    පූර්ණ_සංඛ්‍යා		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ITEM(sid);

    *ufunc = NULL;

    නම් (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	නම් (do_source(script->sn_name, FALSE, DOSO_NONE, NULL) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    ආපසු -1;
	}
    }

    // Find name in "script".
    idx = get_script_item_idx(sid, name, 0, cctx, cstack);
    නම් (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_data) + idx;
	නම් ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    නම් (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    ආපසු -1;
	}
	*type = sv->sv_type;
    }
    නැතහොත්
    {
	size_t	len = STRLEN(name);
	char_u	buffer[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	නම් (script->sn_autoload_prefix != NULL)
	    len += STRLEN(script->sn_autoload_prefix) + 2;
	නැතහොත්
	    len += 15;

	නම් (len < parimaa_na(buffer))
	    funcname = buffer;
	නැතහොත්
	{
	    funcname = alloc(len);
	    නම් (funcname == NULL)
		ආපසු -1;
	}
	නම් (script->sn_autoload_prefix != NULL)
	{
	    sprintf((var_na *)funcname, "%s%s", script->sn_autoload_prefix, name);
	}
	නැතහොත්
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (පූර්ණ_සංඛ්‍යා)KE_SNR;
	    sprintf((var_na *)funcname + 3, "%ld_%s", (long)sid, name);
	}
	*ufunc = find_func(funcname, FALSE);

	නම් (*ufunc == NULL)
	{
	    නම් (verbose)
	    {
		ufunc_T *alt_ufunc = NULL;

		නම් (script->sn_autoload_prefix != NULL)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (පූර්ණ_සංඛ්‍යා)KE_SNR;
		    sprintf((var_na *)funcname + 3, "%ld_%s", (long)sid, name);
		    alt_ufunc = find_func(funcname, FALSE);
		}
		නම් (alt_ufunc != NULL)
		    semsg(_(e_item_not_exported_in_script_str), name);
		නැතහොත්
		    semsg(_(e_item_not_found_in_script_str), name);
	    }
	}
	නැතහොත් නම් (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    නම් (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    *ufunc = NULL;
	}
	නම් (funcname != buffer)
	    vim_free(funcname);
    }

    ආපසු idx;
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

    නම් (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	නම් (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_value));
	නැතහොත්
	    emsg(_(e_const_requires_a_value));
	ආපසු arg + STRLEN(arg);
    }

    // Check for valid starting character.
    නම් (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ආපසු arg + STRLEN(arg);
    }

    සඳහා (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	නම් (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    bha_nga;

    නම් (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	ආපසු arg + STRLEN(arg);
    }
    නම් (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	ආපසු arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, NULL, NULL, TRUE);
    නම් (type == NULL
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	ආපසු p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_FIELD(init_tv);
    නම් (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_NUMBER;
    නැතහොත්
	init_tv.v_type = type->tt_type;
    set_var_const(name, 0, type, &init_tv, FALSE, ASSIGN_INIT, 0);

    vim_free(name);
    ආපසු p;
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
    void
update_vim9_script_var(
	පූර්ණ_සංඛ්‍යා	    create,
	dictitem_T  *di,
	char_u	    *name,
	පූර්ණ_සංඛ්‍යා	    flags,
	typval_T    *tv,
	type_T	    **type,
	පූර්ණ_සංඛ්‍යා	    do_member)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = NULL;

    නම් (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = NULL;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	නම් (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    ආපසු;

	hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	නම් (!HASHITEM_EMPTY(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    සඳහා (sav = HI2SAV(hi); ; sav = sav->sav_next)
	    {
		නම් (sav->sav_block_id == si->sn_current_block_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_data)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    නම් (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    bha_nga;
		}
		නම් (sav->sav_next == NULL)
		    bha_nga;
	    }
	}

	නම් (sv == NULL)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_data) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_key) + STRLEN(name) + 1);
	    නම් (newsav == NULL)
		ආපසු;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ASSIGN_FINAL) ? ASSIGN_FINAL
				   : (flags & ASSIGN_CONST) ? ASSIGN_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    නම් ((flags & ASSIGN_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_key[0], name);
	    sv->sv_name = newsav->sav_key;
	    newsav->sav_di = di;
	    newsav->sav_block_id = si->sn_current_block_id;

	    නම් (HASHITEM_EMPTY(hi))
		// new variable name
		hash_add(&si->sn_all_vars.dv_hashtab, newsav->sav_key,
							       "add variable");
	    නැතහොත් නම් (sav != NULL)
		// existing name in a new block, append to the list
		sav->sav_next = newsav;
	}
    }
    නැතහොත්
    {
	sv = find_typval_in_script(&di->di_tv, 0, TRUE);
    }
    නම් (sv != NULL)
    {
	නම් (*type == NULL)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	නැතහොත් නම් ((flags & ASSIGN_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == NULL)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	නම් (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	නම් (*type != NULL && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	නැතහොත්
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
    void
hide_script_var(scriptitem_T *si, පූර්ණ_සංඛ්‍යා idx, පූර්ණ_සංඛ්‍යා func_defined)
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

    නම් (HASHITEM_EMPTY(script_hi) || HASHITEM_EMPTY(all_hi))
	ආපසු;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = NULL;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    අතරතුර (sav != NULL && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_next;
    }
    නම් (sav == NULL)
	ආපසු;

    නම් (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = NULL;
	sv->sv_tv = &sav->sav_tv;
    }
    නැතහොත්
    {
	නම් (sav_prev == NULL)
	    hash_remove(all_ht, all_hi, "hide variable");
	නැතහොත්
	    sav_prev->sav_next = sav->sav_next;
	sv->sv_name = NULL;
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
find_typval_in_script(typval_T *dest, scid_T sid, පූර්ණ_සංඛ්‍යා must_find)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid == 0 ? current_sctx.sc_sid : sid);
    පූර්ණ_සංඛ්‍යා		    idx;

    නම් (si->sn_version != SCRIPT_VERSION_VIM9)
	// legacy script doesn't store variable types
	ආපසු NULL;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    සඳහා (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	නම් (sv->sv_name != NULL && sv->sv_tv == dest)
	    ආපසු sv;
    }
    නම් (must_find)
	iemsg("find_typval_in_script(): not found");
    ආපසු NULL;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    පූර්ණ_සංඛ්‍යා
check_script_var_type(
	svar_T	    *sv,
	typval_T    *value,
	char_u	    *name,
	where_T	    where)
{
    පූර්ණ_සංඛ්‍යා	    ret;

    නම් (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ආපසු FAIL;
    }
    ret = check_typval_type(sv->sv_type, value, where);
    නම් (ret == OK && need_convert_to_bool(sv->sv_type, value))
    {
	පූර්ණ_සංඛ්‍යා	val = tv2bool(value);

	clear_tv(value);
	value->v_type = VAR_BOOL;
	value->v_lock = 0;
	value->vval.v_number = val ? VVAL_TRUE : VVAL_FALSE;
    }
    ආපසු ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
static var_na *reserved[] = {
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
    static පූර්ණ_සංඛ්‍යා
comp_names(const void *s1, const void *s2)
{
    ආපසු STRCMP(*(var_na **)s1, *(var_na **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    පූර්ණ_සංඛ්‍යා
check_reserved_name(char_u *name, පූර්ණ_සංඛ්‍යා is_objm_access)
{
    // "this" can be used in an object method
    නම් (is_objm_access && STRCMP("this", name) == 0)
	ආපසු OK;

    නම් (bsearch(&name, reserved, ARRAY_LENGTH(reserved),
				parimaa_na(reserved[0]), comp_names) != NULL)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	ආපසු FAIL;
    }

    ආපසු OK;
}

#endif // FEAT_EVAL
