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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    صحیح
اندر_old_script(صحیح max_نسخہ)
{
    واپس (current_sctx.sc_نسخہ < max_نسخہ
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    صحیح
current_script_is_vim9(خالی)
{
    واپس SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ
						       == SCRIPT_نسخہ_VIM9;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    خالی
clear_vim9_scriptlocal_vars(صحیح sid)
{
    hashtab_T	*ht = &SCRIPT_VARS(sid);

    hashtab_free_contents(ht);
    hash_init(ht);
    delete_script_functions(sid);

    // also the exported variables of an autoload script
    delete_autoload_export_vars(SCRIPT_شے(sid)->sn_autoload_سابقہ);

    // old imports and script variables are no longer valid
    free_imports_and_script_vars(sid);
}
#اگر_ختم

/*
 * ":vim9script".
 */
    خالی
ex_vim9script(exarg_T *eap غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    صحیح		    ملا_noclear = FALSE;
    char_u	    *p;

    اگر (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_مستعمل_اندر_script));
	واپس;
    }

    si = SCRIPT_شے(sid);
    اگر (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_پہلا_command_اندر_script));
	واپس;
    }

    برائے (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	اگر (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    اگر (ملا_noclear)
	    {
		semsg(_(e_duplicate_دلیل_str), p);
		واپس;
	    }
	    ملا_noclear = TRUE;
	}
	ورنہ
	{
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	    واپس;
	}
    }

    اگر (si->sn_state == SN_STATE_RELOAD && !ملا_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    اگر (si->sn_autoload_سابقہ == تہی)
	si->sn_autoload_سابقہ = get_autoload_سابقہ(si);

    current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
    si->sn_نسخہ = SCRIPT_نسخہ_VIM9;

    اگر (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	سیٹ_option_قدر_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#ورنہ
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
#اگر_ختم
}

#اگر defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    صحیح
not_اندر_vim9(exarg_T *eap, bool silent)
{
    اگر (اندر_vim9script())
	انتخاب (eap->cmdidx)
	{
	    حالت CMD_k:
		اگر (eap->addr_گنتی > 0)
		{
		    اگر (!silent)
			emsg(_(e_no_range_allowed));
		    واپس FAIL;
		}
		// FALLTHROUGH
	    حالت CMD_append:
	    حالت CMD_change:
	    حالت CMD_داخل_کرو:
	    حالت CMD_open:
	    حالت CMD_t:
	    حالت CMD_xit:
		اگر (!silent)
		    semsg(_(e_command_not_supported_اندر_vim9_script_missing_var_str), eap->cmd);
		واپس FAIL;
	    طےشدہ: توڑو;
	}
    واپس OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    صحیح
vim9_bad_comment(char_u *p)
{
    اگر (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	اگر (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_مقصد_آغاز_comment));
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    صحیح
vim9_comment_آغاز(char_u *p)
{
#اگر_تعریف_شدہ FEAT_EVAL
    واپس p[0] == '#' && !vim9_bad_comment(p);
#ورنہ
    واپس p[0] == '#' && (p[1] != '{' || p[2] == '{');
#اگر_ختم
}

#اگر defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    خالی
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    اگر (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	واپس;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    اگر (eap->cmd == تہی)
	واپس;
    vim_snprintf((حرف *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = تہی;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    خالی
ex_export(exarg_T *eap غیر_مستعمل)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_مستعمل_اندر_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    ساکن imported_T *
new_imported(garray_T *gap)
{
    اگر (ga_grow(gap, 1) == OK)
	واپس ((imported_T *)gap->ga_ڈیٹا + gap->ga_len++);
    واپس تہی;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    ساکن خالی
free_all_script_vars(scriptitem_T *si)
{
    صحیح		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_اگلا;
    صحیح		idx;

    hash_تالا(ht);
    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    جب (sav != تہی)
	    {
		sav_اگلا = sav->sav_اگلا;
		اگر (sav->sav_di == تہی)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_اگلا;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    برائے (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;

	اگر (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    خالی
free_imports_and_script_vars(صحیح sid)
{
    scriptitem_T    *si = SCRIPT_شے(sid);
    صحیح		    idx;

    برائے (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ڈیٹا) + idx;

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
    خالی
mark_imports_for_reload(صحیح sid)
{
    scriptitem_T    *si = SCRIPT_شے(sid);
    صحیح		    idx;

    برائے (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ڈیٹا) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    ساکن صحیح
handle_import_fname(char_u *fname, صحیح is_autoload, صحیح *sid)
{
    اگر (is_autoload)
    {
	scriptitem_T	*si;

	*sid = ڈھونڈو_script_by_name(fname);
	اگر (*sid < 0)
	{
	    صحیح error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    اگر (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		واپس FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    اگر (error == FAIL)
		واپس FAIL;
	}

	si = SCRIPT_شے(*sid);
	si->sn_import_autoload = TRUE;

	اگر (si->sn_autoload_سابقہ == تہی)
	    si->sn_autoload_سابقہ = get_autoload_سابقہ(si);

	// with testing override: load autoload script right away
	اگر (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    واپس OK;
    }
    واپس do_ماخذ(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    ساکن char_u *
handle_import(
	char_u	    *arg_آغاز,
	garray_T    *gap,
	صحیح	    import_sid,
	evalarg_T   *evalarg,
	خالی	    *cctx)
{
    char_u	*arg = arg_آغاز;
    char_u	*nextarg;
    صحیح		is_autoload = FALSE;
    صحیح		getnext;
    char_u	*اظہاریہ_انتہا;
    صحیح		ret = FAIL;
    char_u	*as_name = تہی;
    typval_T	tv;
    صحیح		sid = -2;
    صحیح		res;
    طویل	آغاز_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    صحیح		i;

    اگر (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, تہی, evalarg, &اظہاریہ_انتہا);
    اگر (ret == FAIL)
	جاؤ erret;
    اگر (tv.v_type != VAR_STRING
		       || tv.vval.v_string == تہی || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	جاؤ erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = آغاز_lnum;

    /*
     * find script file
     */
    اگر (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_شے(current_sctx.sc_sid);
	char_u		*دم = gettail(si->sn_name);
	char_u		*from_name;
	صحیح		sourced_from_nofile_buf = FALSE;

	اگر (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	اگر (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(دم) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    اگر (from_name == تہی)
		جاؤ erret;
	    vim_strncpy(from_name, si->sn_name, دم - si->sn_name);
	    جوڑو_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	ورنہ
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    ورنہ اگر (mch_is_بھرا_Name(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    ورنہ اگر (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	اگر (from_name == تہی)
	    جاؤ erret;
	vim_snprintf((حرف *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = ڈھونڈو_script_اندر_rtp(from_name);
	vim_free(from_name);
	اگر (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_شے(sid);

	    اگر (si->sn_autoload_سابقہ == تہی)
		si->sn_autoload_سابقہ = get_autoload_سابقہ(si);
	    res = OK;
	    اگر (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(خالی)do_ماخذ(si->sn_name, FALSE, DOSO_NONE, تہی);
	}
	ورنہ
	    res = FAIL;
    }
    ورنہ
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	اگر (from_name == تہی)
	    جاؤ erret;
	vim_snprintf((حرف *)from_name, len, "import/%s", tv.vval.v_string);
	res = ماخذ_اندر_راستہ(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    اگر (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_راستہ
		    : e_could_not_import_str), tv.vval.v_string);
	جاؤ erret;
    }

    اگر (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	جاؤ erret;
    }

    import_gap = gap != تہی ? gap : &SCRIPT_شے(import_sid)->sn_imports;
    برائے (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_ڈیٹا + i;

	اگر (import->imp_sid == sid)
	{
	    اگر (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		توڑو;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    جاؤ erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_اگلا_non_blank(اظہاریہ_انتہا, evalarg, &getnext);
    اگر (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	اگر (getnext)
	    arg = eval_اگلا_سطر(اظہاریہ_انتہا, evalarg);
	ورنہ
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	اگر (eval_isnamec1(*arg))
	    جب (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	اگر (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_اندر_import_str), p);
	    جاؤ erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    ورنہ
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *انتہا = (char_u *)لڑی_تلاش((حرف *)p, ".vim");

	اگر (!سرے_excmd2(arg_آغاز, اظہاریہ_انتہا))
	{
	    semsg(_(e_trailing_characters_str), اظہاریہ_انتہا);
	    جاؤ erret;
	}
	اگر (انتہا == تہی || انتہا[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_انتہا_اندر_dot_vim_str), p);
	    جاؤ erret;
	}
	اگر (انتہا == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    جاؤ erret;
	}
	as_name = vim_strnsave(p, انتہا - p);
    }

    اگر (as_name != تہی)
    {
	imported_T  *imported;

	imported = ڈھونڈو_imported(as_name, STRLEN(as_name), FALSE);
	اگر (imported != تہی && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    جاؤ erret;
	}
	ورنہ اگر (imported == تہی
		&& check_defined(as_name, STRLEN(as_name), cctx, تہی,
								FALSE) == FAIL)
	    جاؤ erret;

	اگر (imported == تہی)
	{
	    imported = new_imported(import_gap);
	    اگر (imported == تہی)
		جاؤ erret;
	    imported->imp_name = as_name;
	    as_name = تہی;
	    imported->imp_sid = sid;
	    اگر (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    واپس arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    خالی
ex_import(exarg_T *eap)
{
    char_u	*cmd_انتہا;
    evalarg_T	evalarg;

    اگر (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_مستعمل_اندر_script));
	واپس;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_انتہا = handle_import(eap->arg, تہی, current_sctx.sc_sid,
							       &evalarg, تہی);
    اگر (cmd_انتہا != تہی)
	سیٹ_nextcmd(eap, cmd_انتہا);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    خالی
import_check_sourced_sid(صحیح *sid)
{
    scriptitem_T *script = SCRIPT_شے(*sid);

    اگر (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    صحیح
ڈھونڈو_exported(
	صحیح	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	صحیح	    verbose)
{
    صحیح		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_شے(sid);

    *ufunc = تہی;

    اگر (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	اگر (do_ماخذ(script->sn_name, FALSE, DOSO_NONE, تہی) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    واپس -1;
	}
    }

    // Find name in "script".
    idx = get_script_شے_idx(sid, name, 0, cctx, cstack);
    اگر (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_ڈیٹا) + idx;
	اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    اگر (verbose)
		semsg(_(e_شے_not_exported_اندر_script_str), name);
	    واپس -1;
	}
	*type = sv->sv_type;
    }
    ورنہ
    {
	size_t	len = STRLEN(name);
	char_u	بفر[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	اگر (script->sn_autoload_سابقہ != تہی)
	    len += STRLEN(script->sn_autoload_سابقہ) + 2;
	ورنہ
	    len += 15;

	اگر (len < حجم(بفر))
	    funcname = بفر;
	ورنہ
	{
	    funcname = alloc(len);
	    اگر (funcname == تہی)
		واپس -1;
	}
	اگر (script->sn_autoload_سابقہ != تہی)
	{
	    sprintf((حرف *)funcname, "%s%s", script->sn_autoload_سابقہ, name);
	}
	ورنہ
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (صحیح)KE_SNR;
	    sprintf((حرف *)funcname + 3, "%ld_%s", (طویل)sid, name);
	}
	*ufunc = ڈھونڈو_func(funcname, FALSE);

	اگر (*ufunc == تہی)
	{
	    اگر (verbose)
	    {
		ufunc_T *alt_ufunc = تہی;

		اگر (script->sn_autoload_سابقہ != تہی)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (صحیح)KE_SNR;
		    sprintf((حرف *)funcname + 3, "%ld_%s", (طویل)sid, name);
		    alt_ufunc = ڈھونڈو_func(funcname, FALSE);
		}
		اگر (alt_ufunc != تہی)
		    semsg(_(e_شے_not_exported_اندر_script_str), name);
		ورنہ
		    semsg(_(e_شے_not_ملا_اندر_script_str), name);
	    }
	}
	ورنہ اگر (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    اگر (verbose)
		semsg(_(e_شے_not_exported_اندر_script_str), name);
	    *ufunc = تہی;
	}
	اگر (funcname != بفر)
	    vim_free(funcname);
    }

    واپس idx;
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
    scriptitem_T    *si = SCRIPT_شے(current_sctx.sc_sid);
    type_T	    *type;
    typval_T	    init_tv;

    اگر (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	اگر (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_قدر));
	ورنہ
	    emsg(_(e_const_requires_a_قدر));
	واپس arg + STRLEN(arg);
    }

    // Check for valid starting character.
    اگر (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_دلیل_str), arg);
	واپس arg + STRLEN(arg);
    }

    برائے (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	اگر (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    توڑو;

    اگر (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	واپس arg + STRLEN(arg);
    }
    اگر (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	واپس arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, تہی, تہی, TRUE);
    اگر (type == تہی
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	واپس p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_میدان(init_tv);
    اگر (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_نمبر;
    ورنہ
	init_tv.v_type = type->tt_type;
    سیٹ_var_const(name, 0, type, &init_tv, FALSE, تفویض_INIT, 0);

    vim_free(name);
    واپس p;
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
    خالی
update_vim9_script_var(
	صحیح	    create,
	dictitem_T  *di,
	char_u	    *name,
	صحیح	    flags,
	typval_T    *tv,
	type_T	    **type,
	صحیح	    do_member)
{
    scriptitem_T    *si = SCRIPT_شے(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = تہی;

    اگر (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = تہی;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	اگر (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    واپس;

	hi = hash_ڈھونڈو(&si->sn_all_vars.dv_hashtab, name);
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    برائے (sav = HI2SAV(hi); ; sav = sav->sav_اگلا)
	    {
		اگر (sav->sav_بلاک_id == si->sn_current_بلاک_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    اگر (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    توڑو;
		}
		اگر (sav->sav_اگلا == تہی)
		    توڑو;
	    }
	}

	اگر (sv == تہی)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_کلید) + STRLEN(name) + 1);
	    اگر (newsav == تہی)
		واپس;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & تفویض_FINAL) ? تفویض_FINAL
				   : (flags & تفویض_CONST) ? تفویض_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    اگر ((flags & تفویض_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_کلید[0], name);
	    sv->sv_name = newsav->sav_کلید;
	    newsav->sav_di = di;
	    newsav->sav_بلاک_id = si->sn_current_بلاک_id;

	    اگر (HASHITEM_خالی_خانے(hi))
		// new variable name
		hash_جوڑو(&si->sn_all_vars.dv_hashtab, newsav->sav_کلید,
							       "add variable");
	    ورنہ اگر (sav != تہی)
		// existing name in a new block, append to the list
		sav->sav_اگلا = newsav;
	}
    }
    ورنہ
    {
	sv = ڈھونڈو_typval_اندر_script(&di->di_tv, 0, TRUE);
    }
    اگر (sv != تہی)
    {
	اگر (*type == تہی)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	ورنہ اگر ((flags & تفویض_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == تہی)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	اگر (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	اگر (*type != تہی && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	ورنہ
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
    خالی
hide_script_var(scriptitem_T *si, صحیح idx, صحیح func_defined)
{
    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;
    hashtab_T	*script_ht = get_script_local_ht();
    hashtab_T	*all_ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*script_hi;
    hashitem_T	*all_hi;

    // Remove a variable declared inside the block, if it still exists.
    // If it was added in a nested block it will already have been removed.
    // The typval is moved into the sallvar_T.
    script_hi = hash_ڈھونڈو(script_ht, sv->sv_name);
    all_hi = hash_ڈھونڈو(all_ht, sv->sv_name);

    اگر (HASHITEM_خالی_خانے(script_hi) || HASHITEM_خالی_خانے(all_hi))
	واپس;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_پچھلا = تہی;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    جب (sav != تہی && sav->sav_var_vals_idx != idx)
    {
	sav_پچھلا = sav;
	sav = sav->sav_اگلا;
    }
    اگر (sav == تہی)
	واپس;

    اگر (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = تہی;
	sv->sv_tv = &sav->sav_tv;
    }
    ورنہ
    {
	اگر (sav_پچھلا == تہی)
	    hash_remove(all_ht, all_hi, "hide variable");
	ورنہ
	    sav_پچھلا->sav_اگلا = sav->sav_اگلا;
	sv->sv_name = تہی;
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
ڈھونڈو_typval_اندر_script(typval_T *dest, scid_T sid, صحیح must_ڈھونڈو)
{
    scriptitem_T    *si = SCRIPT_شے(sid == 0 ? current_sctx.sc_sid : sid);
    صحیح		    idx;

    اگر (si->sn_نسخہ != SCRIPT_نسخہ_VIM9)
	// legacy script doesn't store variable types
	واپس تہی;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    برائے (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	اگر (sv->sv_name != تہی && sv->sv_tv == dest)
	    واپس sv;
    }
    اگر (must_ڈھونڈو)
	iemsg("find_typval_in_script(): not found");
    واپس تہی;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    صحیح
check_script_var_type(
	svar_T	    *sv,
	typval_T    *قدر,
	char_u	    *name,
	where_T	    where)
{
    صحیح	    ret;

    اگر (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	واپس FAIL;
    }
    ret = check_typval_type(sv->sv_type, قدر, where);
    اگر (ret == OK && need_convert_مقصد_bool(sv->sv_type, قدر))
    {
	صحیح	val = tv2bool(قدر);

	clear_tv(قدر);
	قدر->v_type = VAR_BOOL;
	قدر->v_تالا = 0;
	قدر->vval.v_نمبر = val ? VVAL_TRUE : VVAL_FALSE;
    }
    واپس ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
ساکن حرف *reserved[] = {
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
    ساکن صحیح
comp_نام(ثابت خالی *s1, ثابت خالی *s2)
{
    واپس STRCMP(*(حرف **)s1, *(حرف **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    صحیح
check_reserved_name(char_u *name, صحیح is_objm_access)
{
    // "this" can be used in an object method
    اگر (is_objm_access && STRCMP("this", name) == 0)
	واپس OK;

    اگر (bsearch(&name, reserved, ARRAY_لمبائی(reserved),
				حجم(reserved[0]), comp_نام) != تہی)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	واپس FAIL;
    }

    واپس OK;
}

#اگر_ختم // FEAT_EVAL
