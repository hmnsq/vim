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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    पूर्णाङ्क
in_old_script(पूर्णाङ्क max_version)
{
    प्रत्यागम (current_sctx.sc_version < max_version
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    पूर्णाङ्क
current_script_is_vim9(शून्य)
{
    प्रत्यागम SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
						       == SCRIPT_VERSION_VIM9;
}
#endif

#ifdef FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    शून्य
clear_vim9_scriptlocal_vars(पूर्णाङ्क sid)
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
    शून्य
ex_vim9script(exarg_T *eap UNUSED)
{
#ifdef FEAT_EVAL
    पूर्णाङ्क		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    पूर्णाङ्क		    found_noclear = FALSE;
    char_u	    *p;

    यदि (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_used_in_script));
	प्रत्यागम;
    }

    si = SCRIPT_ITEM(sid);
    यदि (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_first_command_in_script));
	प्रत्यागम;
    }

    प्रति (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	यदि (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    यदि (found_noclear)
	    {
		semsg(_(e_duplicate_argument_str), p);
		प्रत्यागम;
	    }
	    found_noclear = TRUE;
	}
	अन्यथा
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    प्रत्यागम;
	}
    }

    यदि (si->sn_state == SN_STATE_RELOAD && !found_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    यदि (si->sn_autoload_prefix == रिक्त)
	si->sn_autoload_prefix = get_autoload_prefix(si);

    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    si->sn_version = SCRIPT_VERSION_VIM9;

    यदि (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	set_option_मूल्य_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#अन्यथा
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
#endif
}

#यदि defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    पूर्णाङ्क
not_in_vim9(exarg_T *eap, bool silent)
{
    यदि (in_vim9script())
	विकल्प (eap->cmdidx)
	{
	    पक्ष CMD_k:
		यदि (eap->addr_गिनती > 0)
		{
		    यदि (!silent)
			emsg(_(e_no_range_allowed));
		    प्रत्यागम FAIL;
		}
		// FALLTHROUGH
	    पक्ष CMD_append:
	    पक्ष CMD_change:
	    पक्ष CMD_जोड़ो:
	    पक्ष CMD_open:
	    पक्ष CMD_t:
	    पक्ष CMD_xit:
		यदि (!silent)
		    semsg(_(e_command_not_supported_in_vim9_script_missing_var_str), eap->cmd);
		प्रत्यागम FAIL;
	    स्वभाव: भङ्ग;
	}
    प्रत्यागम OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    पूर्णाङ्क
vim9_bad_comment(char_u *p)
{
    यदि (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	यदि (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_to_start_comment));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    पूर्णाङ्क
vim9_comment_start(char_u *p)
{
#ifdef FEAT_EVAL
    प्रत्यागम p[0] == '#' && !vim9_bad_comment(p);
#अन्यथा
    प्रत्यागम p[0] == '#' && (p[1] != '{' || p[2] == '{');
#endif
}

#यदि defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    शून्य
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    यदि (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	प्रत्यागम;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    यदि (eap->cmd == रिक्त)
	प्रत्यागम;
    vim_snprintf((वर्ण *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = रिक्त;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    शून्य
ex_export(exarg_T *eap UNUSED)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_used_in_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    स्थिर imported_T *
new_imported(garray_T *gap)
{
    यदि (ga_grow(gap, 1) == OK)
	प्रत्यागम ((imported_T *)gap->ga_data + gap->ga_len++);
    प्रत्यागम रिक्त;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    स्थिर शून्य
free_all_script_vars(scriptitem_T *si)
{
    पूर्णाङ्क		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_next;
    पूर्णाङ्क		idx;

    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुएँ(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    यावत् (sav != रिक्त)
	    {
		sav_next = sav->sav_next;
		यदि (sav->sav_di == रिक्त)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_next;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    प्रति (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	यदि (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    शून्य
free_imports_and_script_vars(पूर्णाङ्क sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    पूर्णाङ्क		    idx;

    प्रति (idx = 0; idx < si->sn_imports.ga_len; ++idx)
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
    शून्य
mark_imports_for_reload(पूर्णाङ्क sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    पूर्णाङ्क		    idx;

    प्रति (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
handle_import_fname(char_u *fname, पूर्णाङ्क is_autoload, पूर्णाङ्क *sid)
{
    यदि (is_autoload)
    {
	scriptitem_T	*si;

	*sid = find_script_by_name(fname);
	यदि (*sid < 0)
	{
	    पूर्णाङ्क error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    यदि (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		प्रत्यागम FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    यदि (error == FAIL)
		प्रत्यागम FAIL;
	}

	si = SCRIPT_ITEM(*sid);
	si->sn_import_autoload = TRUE;

	यदि (si->sn_autoload_prefix == रिक्त)
	    si->sn_autoload_prefix = get_autoload_prefix(si);

	// with testing override: load autoload script right away
	यदि (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    प्रत्यागम OK;
    }
    प्रत्यागम do_source(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    स्थिर char_u *
handle_import(
	char_u	    *arg_start,
	garray_T    *gap,
	पूर्णाङ्क	    import_sid,
	evalarg_T   *evalarg,
	शून्य	    *cctx)
{
    char_u	*arg = arg_start;
    char_u	*nextarg;
    पूर्णाङ्क		is_autoload = FALSE;
    पूर्णाङ्क		getnext;
    char_u	*व्यंजक_end;
    पूर्णाङ्क		ret = FAIL;
    char_u	*as_name = रिक्त;
    typval_T	tv;
    पूर्णाङ्क		sid = -2;
    पूर्णाङ्क		res;
    long	start_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    पूर्णाङ्क		i;

    यदि (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, रिक्त, evalarg, &व्यंजक_end);
    यदि (ret == FAIL)
	गच्छ erret;
    यदि (tv.v_type != VAR_STRING
		       || tv.vval.v_string == रिक्त || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	गच्छ erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = start_lnum;

    /*
     * find script file
     */
    यदि (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	char_u		*tail = gettail(si->sn_name);
	char_u		*from_name;
	पूर्णाङ्क		sourced_from_nofile_buf = FALSE;

	यदि (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	यदि (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(tail) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    यदि (from_name == रिक्त)
		गच्छ erret;
	    vim_strncpy(from_name, si->sn_name, tail - si->sn_name);
	    add_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	अन्यथा
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    अन्यथा यदि (mch_isFullName(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    अन्यथा यदि (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	यदि (from_name == रिक्त)
	    गच्छ erret;
	vim_snprintf((वर्ण *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = find_script_in_rtp(from_name);
	vim_free(from_name);
	यदि (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ITEM(sid);

	    यदि (si->sn_autoload_prefix == रिक्त)
		si->sn_autoload_prefix = get_autoload_prefix(si);
	    res = OK;
	    यदि (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(शून्य)do_source(si->sn_name, FALSE, DOSO_NONE, रिक्त);
	}
	अन्यथा
	    res = FAIL;
    }
    अन्यथा
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	यदि (from_name == रिक्त)
	    गच्छ erret;
	vim_snprintf((वर्ण *)from_name, len, "import/%s", tv.vval.v_string);
	res = source_in_path(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    यदि (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_path
		    : e_could_not_import_str), tv.vval.v_string);
	गच्छ erret;
    }

    यदि (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	गच्छ erret;
    }

    import_gap = gap != रिक्त ? gap : &SCRIPT_ITEM(import_sid)->sn_imports;
    प्रति (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_data + i;

	यदि (import->imp_sid == sid)
	{
	    यदि (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		भङ्ग;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    गच्छ erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_next_non_blank(व्यंजक_end, evalarg, &getnext);
    यदि (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	यदि (getnext)
	    arg = eval_next_पंक्ति(व्यंजक_end, evalarg);
	अन्यथा
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	यदि (eval_isnamec1(*arg))
	    यावत् (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	यदि (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_in_import_str), p);
	    गच्छ erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    अन्यथा
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *end = (char_u *)strstr((वर्ण *)p, ".vim");

	यदि (!ends_excmd2(arg_start, व्यंजक_end))
	{
	    semsg(_(e_trailing_characters_str), व्यंजक_end);
	    गच्छ erret;
	}
	यदि (end == रिक्त || end[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_end_in_dot_vim_str), p);
	    गच्छ erret;
	}
	यदि (end == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    गच्छ erret;
	}
	as_name = vim_strnsave(p, end - p);
    }

    यदि (as_name != रिक्त)
    {
	imported_T  *imported;

	imported = find_imported(as_name, STRLEN(as_name), FALSE);
	यदि (imported != रिक्त && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    गच्छ erret;
	}
	अन्यथा यदि (imported == रिक्त
		&& check_defined(as_name, STRLEN(as_name), cctx, रिक्त,
								FALSE) == FAIL)
	    गच्छ erret;

	यदि (imported == रिक्त)
	{
	    imported = new_imported(import_gap);
	    यदि (imported == रिक्त)
		गच्छ erret;
	    imported->imp_name = as_name;
	    as_name = रिक्त;
	    imported->imp_sid = sid;
	    यदि (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    प्रत्यागम arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    शून्य
ex_import(exarg_T *eap)
{
    char_u	*cmd_end;
    evalarg_T	evalarg;

    यदि (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_used_in_script));
	प्रत्यागम;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_end = handle_import(eap->arg, रिक्त, current_sctx.sc_sid,
							       &evalarg, रिक्त);
    यदि (cmd_end != रिक्त)
	set_nextcmd(eap, cmd_end);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    शून्य
import_check_sourced_sid(पूर्णाङ्क *sid)
{
    scriptitem_T *script = SCRIPT_ITEM(*sid);

    यदि (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    पूर्णाङ्क
find_exported(
	पूर्णाङ्क	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	पूर्णाङ्क	    verbose)
{
    पूर्णाङ्क		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ITEM(sid);

    *ufunc = रिक्त;

    यदि (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	यदि (do_source(script->sn_name, FALSE, DOSO_NONE, रिक्त) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    प्रत्यागम -1;
	}
    }

    // Find name in "script".
    idx = get_script_item_idx(sid, name, 0, cctx, cstack);
    यदि (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_data) + idx;
	यदि ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    यदि (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    प्रत्यागम -1;
	}
	*type = sv->sv_type;
    }
    अन्यथा
    {
	size_t	len = STRLEN(name);
	char_u	buffer[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	यदि (script->sn_autoload_prefix != रिक्त)
	    len += STRLEN(script->sn_autoload_prefix) + 2;
	अन्यथा
	    len += 15;

	यदि (len < परिमाण(buffer))
	    funcname = buffer;
	अन्यथा
	{
	    funcname = alloc(len);
	    यदि (funcname == रिक्त)
		प्रत्यागम -1;
	}
	यदि (script->sn_autoload_prefix != रिक्त)
	{
	    sprintf((वर्ण *)funcname, "%s%s", script->sn_autoload_prefix, name);
	}
	अन्यथा
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (पूर्णाङ्क)KE_SNR;
	    sprintf((वर्ण *)funcname + 3, "%ld_%s", (long)sid, name);
	}
	*ufunc = find_func(funcname, FALSE);

	यदि (*ufunc == रिक्त)
	{
	    यदि (verbose)
	    {
		ufunc_T *alt_ufunc = रिक्त;

		यदि (script->sn_autoload_prefix != रिक्त)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (पूर्णाङ्क)KE_SNR;
		    sprintf((वर्ण *)funcname + 3, "%ld_%s", (long)sid, name);
		    alt_ufunc = find_func(funcname, FALSE);
		}
		यदि (alt_ufunc != रिक्त)
		    semsg(_(e_item_not_exported_in_script_str), name);
		अन्यथा
		    semsg(_(e_item_not_found_in_script_str), name);
	    }
	}
	अन्यथा यदि (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    यदि (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    *ufunc = रिक्त;
	}
	यदि (funcname != buffer)
	    vim_free(funcname);
    }

    प्रत्यागम idx;
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

    यदि (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	यदि (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_मूल्य));
	अन्यथा
	    emsg(_(e_const_requires_a_मूल्य));
	प्रत्यागम arg + STRLEN(arg);
    }

    // Check for valid starting character.
    यदि (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	प्रत्यागम arg + STRLEN(arg);
    }

    प्रति (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	यदि (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    भङ्ग;

    यदि (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	प्रत्यागम arg + STRLEN(arg);
    }
    यदि (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	प्रत्यागम arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, रिक्त, रिक्त, TRUE);
    यदि (type == रिक्त
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	प्रत्यागम p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_FIELD(init_tv);
    यदि (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_संख्या;
    अन्यथा
	init_tv.v_type = type->tt_type;
    set_var_const(name, 0, type, &init_tv, FALSE, ASSIGN_INIT, 0);

    vim_free(name);
    प्रत्यागम p;
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
    शून्य
update_vim9_script_var(
	पूर्णाङ्क	    create,
	dictitem_T  *di,
	char_u	    *name,
	पूर्णाङ्क	    flags,
	typval_T    *tv,
	type_T	    **type,
	पूर्णाङ्क	    do_member)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = रिक्त;

    यदि (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = रिक्त;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	यदि (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    प्रत्यागम;

	hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	यदि (!HASHITEM_EMPTY(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    प्रति (sav = HI2SAV(hi); ; sav = sav->sav_next)
	    {
		यदि (sav->sav_block_id == si->sn_current_block_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_data)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    यदि (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    भङ्ग;
		}
		यदि (sav->sav_next == रिक्त)
		    भङ्ग;
	    }
	}

	यदि (sv == रिक्त)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_data) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_कुंजी) + STRLEN(name) + 1);
	    यदि (newsav == रिक्त)
		प्रत्यागम;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ASSIGN_FINAL) ? ASSIGN_FINAL
				   : (flags & ASSIGN_CONST) ? ASSIGN_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    यदि ((flags & ASSIGN_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_कुंजी[0], name);
	    sv->sv_name = newsav->sav_कुंजी;
	    newsav->sav_di = di;
	    newsav->sav_block_id = si->sn_current_block_id;

	    यदि (HASHITEM_EMPTY(hi))
		// new variable name
		hash_add(&si->sn_all_vars.dv_hashtab, newsav->sav_कुंजी,
							       "add variable");
	    अन्यथा यदि (sav != रिक्त)
		// existing name in a new block, append to the list
		sav->sav_next = newsav;
	}
    }
    अन्यथा
    {
	sv = find_typval_in_script(&di->di_tv, 0, TRUE);
    }
    यदि (sv != रिक्त)
    {
	यदि (*type == रिक्त)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	अन्यथा यदि ((flags & ASSIGN_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == रिक्त)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	यदि (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	यदि (*type != रिक्त && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	अन्यथा
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
    शून्य
hide_script_var(scriptitem_T *si, पूर्णाङ्क idx, पूर्णाङ्क func_defined)
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

    यदि (HASHITEM_EMPTY(script_hi) || HASHITEM_EMPTY(all_hi))
	प्रत्यागम;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = रिक्त;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    यावत् (sav != रिक्त && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_next;
    }
    यदि (sav == रिक्त)
	प्रत्यागम;

    यदि (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = रिक्त;
	sv->sv_tv = &sav->sav_tv;
    }
    अन्यथा
    {
	यदि (sav_prev == रिक्त)
	    hash_remove(all_ht, all_hi, "hide variable");
	अन्यथा
	    sav_prev->sav_next = sav->sav_next;
	sv->sv_name = रिक्त;
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
find_typval_in_script(typval_T *dest, scid_T sid, पूर्णाङ्क must_find)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid == 0 ? current_sctx.sc_sid : sid);
    पूर्णाङ्क		    idx;

    यदि (si->sn_version != SCRIPT_VERSION_VIM9)
	// legacy script doesn't store variable types
	प्रत्यागम रिक्त;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    प्रति (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	यदि (sv->sv_name != रिक्त && sv->sv_tv == dest)
	    प्रत्यागम sv;
    }
    यदि (must_find)
	iemsg("find_typval_in_script(): not found");
    प्रत्यागम रिक्त;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    पूर्णाङ्क
check_script_var_type(
	svar_T	    *sv,
	typval_T    *मूल्य,
	char_u	    *name,
	where_T	    where)
{
    पूर्णाङ्क	    ret;

    यदि (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	प्रत्यागम FAIL;
    }
    ret = check_typval_type(sv->sv_type, मूल्य, where);
    यदि (ret == OK && need_convert_to_bool(sv->sv_type, मूल्य))
    {
	पूर्णाङ्क	val = tv2bool(मूल्य);

	clear_tv(मूल्य);
	मूल्य->v_type = VAR_BOOL;
	मूल्य->v_lock = 0;
	मूल्य->vval.v_संख्या = val ? VVAL_TRUE : VVAL_FALSE;
    }
    प्रत्यागम ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
स्थिर वर्ण *reserved[] = {
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
    स्थिर पूर्णाङ्क
comp_names(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    प्रत्यागम STRCMP(*(वर्ण **)s1, *(वर्ण **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    पूर्णाङ्क
check_reserved_name(char_u *name, पूर्णाङ्क is_objm_access)
{
    // "this" can be used in an object method
    यदि (is_objm_access && STRCMP("this", name) == 0)
	प्रत्यागम OK;

    यदि (bsearch(&name, reserved, ARRAY_LENGTH(reserved),
				परिमाण(reserved[0]), comp_names) != रिक्त)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

#endif // FEAT_EVAL
