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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    পূর্ণাঙ্ক
in_old_script(পূর্ণাঙ্ক max_version)
{
    প্রত্যাগম (current_sctx.sc_version < max_version
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    পূর্ণাঙ্ক
current_script_is_vim9(শূন্য)
{
    প্রত্যাগম SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
						       == SCRIPT_VERSION_VIM9;
}
#endif

#ifdef FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    শূন্য
clear_vim9_scriptlocal_vars(পূর্ণাঙ্ক sid)
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
    শূন্য
ex_vim9script(exarg_T *eap UNUSED)
{
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    পূর্ণাঙ্ক		    found_noclear = FALSE;
    char_u	    *p;

    যদি (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_used_in_script));
	প্রত্যাগম;
    }

    si = SCRIPT_ITEM(sid);
    যদি (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_first_command_in_script));
	প্রত্যাগম;
    }

    প্রতি (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	যদি (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    যদি (found_noclear)
	    {
		semsg(_(e_duplicate_argument_str), p);
		প্রত্যাগম;
	    }
	    found_noclear = TRUE;
	}
	অন্যথা
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    প্রত্যাগম;
	}
    }

    যদি (si->sn_state == SN_STATE_RELOAD && !found_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    যদি (si->sn_autoload_prefix == রিক্ত)
	si->sn_autoload_prefix = get_autoload_prefix(si);

    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    si->sn_version = SCRIPT_VERSION_VIM9;

    যদি (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	set_option_মান_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#অন্যথা
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
#endif
}

#যদি defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    পূর্ণাঙ্ক
not_in_vim9(exarg_T *eap, bool silent)
{
    যদি (in_vim9script())
	বিকল্প (eap->cmdidx)
	{
	    পক্ষ CMD_k:
		যদি (eap->addr_গুনতি > 0)
		{
		    যদি (!silent)
			emsg(_(e_no_range_allowed));
		    প্রত্যাগম FAIL;
		}
		// FALLTHROUGH
	    পক্ষ CMD_append:
	    পক্ষ CMD_change:
	    পক্ষ CMD_ঢোকাও:
	    পক্ষ CMD_open:
	    পক্ষ CMD_t:
	    পক্ষ CMD_xit:
		যদি (!silent)
		    semsg(_(e_command_not_supported_in_vim9_script_missing_var_str), eap->cmd);
		প্রত্যাগম FAIL;
	    স্বভাব: ভঙ্গ;
	}
    প্রত্যাগম OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    পূর্ণাঙ্ক
vim9_bad_comment(char_u *p)
{
    যদি (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	যদি (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_to_start_comment));
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}
#endif

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    পূর্ণাঙ্ক
vim9_comment_start(char_u *p)
{
#ifdef FEAT_EVAL
    প্রত্যাগম p[0] == '#' && !vim9_bad_comment(p);
#অন্যথা
    প্রত্যাগম p[0] == '#' && (p[1] != '{' || p[2] == '{');
#endif
}

#যদি defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    শূন্য
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    যদি (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	প্রত্যাগম;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    যদি (eap->cmd == রিক্ত)
	প্রত্যাগম;
    vim_snprintf((বর্ণ *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = রিক্ত;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    শূন্য
ex_export(exarg_T *eap UNUSED)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_used_in_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    স্থির imported_T *
new_imported(garray_T *gap)
{
    যদি (ga_grow(gap, 1) == OK)
	প্রত্যাগম ((imported_T *)gap->ga_data + gap->ga_len++);
    প্রত্যাগম রিক্ত;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    স্থির শূন্য
free_all_script_vars(scriptitem_T *si)
{
    পূর্ণাঙ্ক		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_next;
    পূর্ণাঙ্ক		idx;

    hash_lock(ht);
    todo = (পূর্ণাঙ্ক)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    যাবত্ (sav != রিক্ত)
	    {
		sav_next = sav->sav_next;
		যদি (sav->sav_di == রিক্ত)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_next;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    প্রতি (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	যদি (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    শূন্য
free_imports_and_script_vars(পূর্ণাঙ্ক sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    পূর্ণাঙ্ক		    idx;

    প্রতি (idx = 0; idx < si->sn_imports.ga_len; ++idx)
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
    শূন্য
mark_imports_for_reload(পূর্ণাঙ্ক sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    পূর্ণাঙ্ক		    idx;

    প্রতি (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
handle_import_fname(char_u *fname, পূর্ণাঙ্ক is_autoload, পূর্ণাঙ্ক *sid)
{
    যদি (is_autoload)
    {
	scriptitem_T	*si;

	*sid = find_script_by_name(fname);
	যদি (*sid < 0)
	{
	    পূর্ণাঙ্ক error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    যদি (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		প্রত্যাগম FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    যদি (error == FAIL)
		প্রত্যাগম FAIL;
	}

	si = SCRIPT_ITEM(*sid);
	si->sn_import_autoload = TRUE;

	যদি (si->sn_autoload_prefix == রিক্ত)
	    si->sn_autoload_prefix = get_autoload_prefix(si);

	// with testing override: load autoload script right away
	যদি (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    প্রত্যাগম OK;
    }
    প্রত্যাগম do_source(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    স্থির char_u *
handle_import(
	char_u	    *arg_start,
	garray_T    *gap,
	পূর্ণাঙ্ক	    import_sid,
	evalarg_T   *evalarg,
	শূন্য	    *cctx)
{
    char_u	*arg = arg_start;
    char_u	*nextarg;
    পূর্ণাঙ্ক		is_autoload = FALSE;
    পূর্ণাঙ্ক		getnext;
    char_u	*রাশিমালা_end;
    পূর্ণাঙ্ক		ret = FAIL;
    char_u	*as_name = রিক্ত;
    typval_T	tv;
    পূর্ণাঙ্ক		sid = -2;
    পূর্ণাঙ্ক		res;
    long	start_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    পূর্ণাঙ্ক		i;

    যদি (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, রিক্ত, evalarg, &রাশিমালা_end);
    যদি (ret == FAIL)
	গচ্ছ erret;
    যদি (tv.v_type != VAR_STRING
		       || tv.vval.v_string == রিক্ত || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	গচ্ছ erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = start_lnum;

    /*
     * find script file
     */
    যদি (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	char_u		*tail = gettail(si->sn_name);
	char_u		*from_name;
	পূর্ণাঙ্ক		sourced_from_nofile_buf = FALSE;

	যদি (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	যদি (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(tail) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    যদি (from_name == রিক্ত)
		গচ্ছ erret;
	    vim_strncpy(from_name, si->sn_name, tail - si->sn_name);
	    add_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	অন্যথা
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    অন্যথা যদি (mch_isFullName(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    অন্যথা যদি (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	যদি (from_name == রিক্ত)
	    গচ্ছ erret;
	vim_snprintf((বর্ণ *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = find_script_in_rtp(from_name);
	vim_free(from_name);
	যদি (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ITEM(sid);

	    যদি (si->sn_autoload_prefix == রিক্ত)
		si->sn_autoload_prefix = get_autoload_prefix(si);
	    res = OK;
	    যদি (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(শূন্য)do_source(si->sn_name, FALSE, DOSO_NONE, রিক্ত);
	}
	অন্যথা
	    res = FAIL;
    }
    অন্যথা
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	যদি (from_name == রিক্ত)
	    গচ্ছ erret;
	vim_snprintf((বর্ণ *)from_name, len, "import/%s", tv.vval.v_string);
	res = source_in_path(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    যদি (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_path
		    : e_could_not_import_str), tv.vval.v_string);
	গচ্ছ erret;
    }

    যদি (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	গচ্ছ erret;
    }

    import_gap = gap != রিক্ত ? gap : &SCRIPT_ITEM(import_sid)->sn_imports;
    প্রতি (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_data + i;

	যদি (import->imp_sid == sid)
	{
	    যদি (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		ভঙ্গ;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    গচ্ছ erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_next_non_blank(রাশিমালা_end, evalarg, &getnext);
    যদি (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	যদি (getnext)
	    arg = eval_next_সারি(রাশিমালা_end, evalarg);
	অন্যথা
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	যদি (eval_isnamec1(*arg))
	    যাবত্ (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	যদি (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_in_import_str), p);
	    গচ্ছ erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    অন্যথা
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *end = (char_u *)strstr((বর্ণ *)p, ".vim");

	যদি (!ends_excmd2(arg_start, রাশিমালা_end))
	{
	    semsg(_(e_trailing_characters_str), রাশিমালা_end);
	    গচ্ছ erret;
	}
	যদি (end == রিক্ত || end[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_end_in_dot_vim_str), p);
	    গচ্ছ erret;
	}
	যদি (end == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    গচ্ছ erret;
	}
	as_name = vim_strnsave(p, end - p);
    }

    যদি (as_name != রিক্ত)
    {
	imported_T  *imported;

	imported = find_imported(as_name, STRLEN(as_name), FALSE);
	যদি (imported != রিক্ত && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    গচ্ছ erret;
	}
	অন্যথা যদি (imported == রিক্ত
		&& check_defined(as_name, STRLEN(as_name), cctx, রিক্ত,
								FALSE) == FAIL)
	    গচ্ছ erret;

	যদি (imported == রিক্ত)
	{
	    imported = new_imported(import_gap);
	    যদি (imported == রিক্ত)
		গচ্ছ erret;
	    imported->imp_name = as_name;
	    as_name = রিক্ত;
	    imported->imp_sid = sid;
	    যদি (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    প্রত্যাগম arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    শূন্য
ex_import(exarg_T *eap)
{
    char_u	*cmd_end;
    evalarg_T	evalarg;

    যদি (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_used_in_script));
	প্রত্যাগম;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_end = handle_import(eap->arg, রিক্ত, current_sctx.sc_sid,
							       &evalarg, রিক্ত);
    যদি (cmd_end != রিক্ত)
	set_nextcmd(eap, cmd_end);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    শূন্য
import_check_sourced_sid(পূর্ণাঙ্ক *sid)
{
    scriptitem_T *script = SCRIPT_ITEM(*sid);

    যদি (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    পূর্ণাঙ্ক
find_exported(
	পূর্ণাঙ্ক	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	পূর্ণাঙ্ক	    verbose)
{
    পূর্ণাঙ্ক		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ITEM(sid);

    *ufunc = রিক্ত;

    যদি (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	যদি (do_source(script->sn_name, FALSE, DOSO_NONE, রিক্ত) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    প্রত্যাগম -1;
	}
    }

    // Find name in "script".
    idx = get_script_item_idx(sid, name, 0, cctx, cstack);
    যদি (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_data) + idx;
	যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    যদি (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    প্রত্যাগম -1;
	}
	*type = sv->sv_type;
    }
    অন্যথা
    {
	size_t	len = STRLEN(name);
	char_u	buffer[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	যদি (script->sn_autoload_prefix != রিক্ত)
	    len += STRLEN(script->sn_autoload_prefix) + 2;
	অন্যথা
	    len += 15;

	যদি (len < পরিমাণ(buffer))
	    funcname = buffer;
	অন্যথা
	{
	    funcname = alloc(len);
	    যদি (funcname == রিক্ত)
		প্রত্যাগম -1;
	}
	যদি (script->sn_autoload_prefix != রিক্ত)
	{
	    sprintf((বর্ণ *)funcname, "%s%s", script->sn_autoload_prefix, name);
	}
	অন্যথা
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (পূর্ণাঙ্ক)KE_SNR;
	    sprintf((বর্ণ *)funcname + 3, "%ld_%s", (long)sid, name);
	}
	*ufunc = find_func(funcname, FALSE);

	যদি (*ufunc == রিক্ত)
	{
	    যদি (verbose)
	    {
		ufunc_T *alt_ufunc = রিক্ত;

		যদি (script->sn_autoload_prefix != রিক্ত)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (পূর্ণাঙ্ক)KE_SNR;
		    sprintf((বর্ণ *)funcname + 3, "%ld_%s", (long)sid, name);
		    alt_ufunc = find_func(funcname, FALSE);
		}
		যদি (alt_ufunc != রিক্ত)
		    semsg(_(e_item_not_exported_in_script_str), name);
		অন্যথা
		    semsg(_(e_item_not_found_in_script_str), name);
	    }
	}
	অন্যথা যদি (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    যদি (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    *ufunc = রিক্ত;
	}
	যদি (funcname != buffer)
	    vim_free(funcname);
    }

    প্রত্যাগম idx;
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

    যদি (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	যদি (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_মান));
	অন্যথা
	    emsg(_(e_const_requires_a_মান));
	প্রত্যাগম arg + STRLEN(arg);
    }

    // Check for valid starting character.
    যদি (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	প্রত্যাগম arg + STRLEN(arg);
    }

    প্রতি (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	যদি (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    ভঙ্গ;

    যদি (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	প্রত্যাগম arg + STRLEN(arg);
    }
    যদি (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	প্রত্যাগম arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, রিক্ত, রিক্ত, TRUE);
    যদি (type == রিক্ত
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	প্রত্যাগম p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_FIELD(init_tv);
    যদি (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_সংখ্যা;
    অন্যথা
	init_tv.v_type = type->tt_type;
    set_var_const(name, 0, type, &init_tv, FALSE, ASSIGN_INIT, 0);

    vim_free(name);
    প্রত্যাগম p;
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
    শূন্য
update_vim9_script_var(
	পূর্ণাঙ্ক	    create,
	dictitem_T  *di,
	char_u	    *name,
	পূর্ণাঙ্ক	    flags,
	typval_T    *tv,
	type_T	    **type,
	পূর্ণাঙ্ক	    do_member)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = রিক্ত;

    যদি (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = রিক্ত;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	যদি (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    প্রত্যাগম;

	hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	যদি (!HASHITEM_EMPTY(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    প্রতি (sav = HI2SAV(hi); ; sav = sav->sav_next)
	    {
		যদি (sav->sav_block_id == si->sn_current_block_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_data)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    যদি (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    ভঙ্গ;
		}
		যদি (sav->sav_next == রিক্ত)
		    ভঙ্গ;
	    }
	}

	যদি (sv == রিক্ত)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_data) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_চাবি) + STRLEN(name) + 1);
	    যদি (newsav == রিক্ত)
		প্রত্যাগম;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ASSIGN_FINAL) ? ASSIGN_FINAL
				   : (flags & ASSIGN_CONST) ? ASSIGN_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    যদি ((flags & ASSIGN_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_চাবি[0], name);
	    sv->sv_name = newsav->sav_চাবি;
	    newsav->sav_di = di;
	    newsav->sav_block_id = si->sn_current_block_id;

	    যদি (HASHITEM_EMPTY(hi))
		// new variable name
		hash_add(&si->sn_all_vars.dv_hashtab, newsav->sav_চাবি,
							       "add variable");
	    অন্যথা যদি (sav != রিক্ত)
		// existing name in a new block, append to the list
		sav->sav_next = newsav;
	}
    }
    অন্যথা
    {
	sv = find_typval_in_script(&di->di_tv, 0, TRUE);
    }
    যদি (sv != রিক্ত)
    {
	যদি (*type == রিক্ত)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	অন্যথা যদি ((flags & ASSIGN_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == রিক্ত)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	যদি (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	যদি (*type != রিক্ত && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	অন্যথা
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
    শূন্য
hide_script_var(scriptitem_T *si, পূর্ণাঙ্ক idx, পূর্ণাঙ্ক func_defined)
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

    যদি (HASHITEM_EMPTY(script_hi) || HASHITEM_EMPTY(all_hi))
	প্রত্যাগম;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = রিক্ত;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    যাবত্ (sav != রিক্ত && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_next;
    }
    যদি (sav == রিক্ত)
	প্রত্যাগম;

    যদি (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = রিক্ত;
	sv->sv_tv = &sav->sav_tv;
    }
    অন্যথা
    {
	যদি (sav_prev == রিক্ত)
	    hash_remove(all_ht, all_hi, "hide variable");
	অন্যথা
	    sav_prev->sav_next = sav->sav_next;
	sv->sv_name = রিক্ত;
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
find_typval_in_script(typval_T *dest, scid_T sid, পূর্ণাঙ্ক must_find)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid == 0 ? current_sctx.sc_sid : sid);
    পূর্ণাঙ্ক		    idx;

    যদি (si->sn_version != SCRIPT_VERSION_VIM9)
	// legacy script doesn't store variable types
	প্রত্যাগম রিক্ত;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    প্রতি (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	যদি (sv->sv_name != রিক্ত && sv->sv_tv == dest)
	    প্রত্যাগম sv;
    }
    যদি (must_find)
	iemsg("find_typval_in_script(): not found");
    প্রত্যাগম রিক্ত;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    পূর্ণাঙ্ক
check_script_var_type(
	svar_T	    *sv,
	typval_T    *মান,
	char_u	    *name,
	where_T	    where)
{
    পূর্ণাঙ্ক	    ret;

    যদি (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	প্রত্যাগম FAIL;
    }
    ret = check_typval_type(sv->sv_type, মান, where);
    যদি (ret == OK && need_convert_to_bool(sv->sv_type, মান))
    {
	পূর্ণাঙ্ক	val = tv2bool(মান);

	clear_tv(মান);
	মান->v_type = VAR_BOOL;
	মান->v_lock = 0;
	মান->vval.v_সংখ্যা = val ? VVAL_TRUE : VVAL_FALSE;
    }
    প্রত্যাগম ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
স্থির বর্ণ *reserved[] = {
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
    স্থির পূর্ণাঙ্ক
comp_names(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2)
{
    প্রত্যাগম STRCMP(*(বর্ণ **)s1, *(বর্ণ **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    পূর্ণাঙ্ক
check_reserved_name(char_u *name, পূর্ণাঙ্ক is_objm_access)
{
    // "this" can be used in an object method
    যদি (is_objm_access && STRCMP("this", name) == 0)
	প্রত্যাগম OK;

    যদি (bsearch(&name, reserved, ARRAY_LENGTH(reserved),
				পরিমাণ(reserved[0]), comp_names) != রিক্ত)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

#endif // FEAT_EVAL
