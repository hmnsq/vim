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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    పూర్ణాంకం
లోపల_old_script(పూర్ణాంకం max_సంస్కరణ)
{
    తిరిగి (current_sctx.sc_సంస్కరణ < max_సంస్కరణ
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    పూర్ణాంకం
current_script_is_vim9(శూన్య)
{
    తిరిగి SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ
						       == SCRIPT_సంస్కరణ_VIM9;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    శూన్య
clear_vim9_scriptlocal_vars(పూర్ణాంకం sid)
{
    hashtab_T	*ht = &SCRIPT_VARS(sid);

    hashtab_free_contents(ht);
    hash_init(ht);
    delete_script_functions(sid);

    // also the exported variables of an autoload script
    delete_autoload_export_vars(SCRIPT_అంశం(sid)->sn_autoload_ఉపసర్గ);

    // old imports and script variables are no longer valid
    free_imports_and_script_vars(sid);
}
#అయితే_ముగింపు

/*
 * ":vim9script".
 */
    శూన్య
ex_vim9script(exarg_T *eap వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    పూర్ణాంకం		    దొరికింది_noclear = FALSE;
    char_u	    *p;

    ఒకవేళ (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_వాడిన_లోపల_script));
	తిరిగి;
    }

    si = SCRIPT_అంశం(sid);
    ఒకవేళ (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_మొదటి_command_లోపల_script));
	తిరిగి;
    }

    కొరకు (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	ఒకవేళ (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    ఒకవేళ (దొరికింది_noclear)
	    {
		semsg(_(e_duplicate_వాదం_str), p);
		తిరిగి;
	    }
	    దొరికింది_noclear = TRUE;
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_వాదం_str), eap->arg);
	    తిరిగి;
	}
    }

    ఒకవేళ (si->sn_state == SN_STATE_RELOAD && !దొరికింది_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    ఒకవేళ (si->sn_autoload_ఉపసర్గ == ఖాళీ)
	si->sn_autoload_ఉపసర్గ = get_autoload_ఉపసర్గ(si);

    current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
    si->sn_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;

    ఒకవేళ (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	సమితి_option_విలువ_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#లేకపోతే
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    పూర్ణాంకం
not_లోపల_vim9(exarg_T *eap, bool silent)
{
    ఒకవేళ (లోపల_vim9script())
	ఎంపిక (eap->cmdidx)
	{
	    స్థితి CMD_k:
		ఒకవేళ (eap->addr_లెక్క > 0)
		{
		    ఒకవేళ (!silent)
			emsg(_(e_no_range_allowed));
		    తిరిగి FAIL;
		}
		// FALLTHROUGH
	    స్థితి CMD_append:
	    స్థితి CMD_change:
	    స్థితి CMD_చొప్పించు:
	    స్థితి CMD_open:
	    స్థితి CMD_t:
	    స్థితి CMD_xit:
		ఒకవేళ (!silent)
		    semsg(_(e_command_not_supported_లోపల_vim9_script_missing_var_str), eap->cmd);
		తిరిగి FAIL;
	    అప్రమేయ: ఆపు;
	}
    తిరిగి OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    పూర్ణాంకం
vim9_bad_comment(char_u *p)
{
    ఒకవేళ (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	ఒకవేళ (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_గమ్యం_ప్రారంభం_comment));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    పూర్ణాంకం
vim9_comment_ప్రారంభం(char_u *p)
{
#నిర్వచితమైతే FEAT_EVAL
    తిరిగి p[0] == '#' && !vim9_bad_comment(p);
#లేకపోతే
    తిరిగి p[0] == '#' && (p[1] != '{' || p[2] == '{');
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    శూన్య
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    ఒకవేళ (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	తిరిగి;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    ఒకవేళ (eap->cmd == ఖాళీ)
	తిరిగి;
    vim_snprintf((అక్షరం *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = ఖాళీ;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    శూన్య
ex_export(exarg_T *eap వాడని)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_వాడిన_లోపల_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    స్థిర imported_T *
new_imported(garray_T *gap)
{
    ఒకవేళ (ga_grow(gap, 1) == OK)
	తిరిగి ((imported_T *)gap->ga_దత్తాంశం + gap->ga_len++);
    తిరిగి ఖాళీ;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    స్థిర శూన్య
free_all_script_vars(scriptitem_T *si)
{
    పూర్ణాంకం		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_తదుపరి;
    పూర్ణాంకం		idx;

    hash_తాళం(ht);
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    అయితే (sav != ఖాళీ)
	    {
		sav_తదుపరి = sav->sav_తదుపరి;
		ఒకవేళ (sav->sav_di == ఖాళీ)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_తదుపరి;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    కొరకు (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;

	ఒకవేళ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    శూన్య
free_imports_and_script_vars(పూర్ణాంకం sid)
{
    scriptitem_T    *si = SCRIPT_అంశం(sid);
    పూర్ణాంకం		    idx;

    కొరకు (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_దత్తాంశం) + idx;

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
    శూన్య
mark_imports_for_reload(పూర్ణాంకం sid)
{
    scriptitem_T    *si = SCRIPT_అంశం(sid);
    పూర్ణాంకం		    idx;

    కొరకు (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_దత్తాంశం) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
handle_import_fname(char_u *fname, పూర్ణాంకం is_autoload, పూర్ణాంకం *sid)
{
    ఒకవేళ (is_autoload)
    {
	scriptitem_T	*si;

	*sid = కనుగొను_script_by_name(fname);
	ఒకవేళ (*sid < 0)
	{
	    పూర్ణాంకం error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    ఒకవేళ (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		తిరిగి FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    ఒకవేళ (error == FAIL)
		తిరిగి FAIL;
	}

	si = SCRIPT_అంశం(*sid);
	si->sn_import_autoload = TRUE;

	ఒకవేళ (si->sn_autoload_ఉపసర్గ == ఖాళీ)
	    si->sn_autoload_ఉపసర్గ = get_autoload_ఉపసర్గ(si);

	// with testing override: load autoload script right away
	ఒకవేళ (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    తిరిగి OK;
    }
    తిరిగి do_ఆధారం(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    స్థిర char_u *
handle_import(
	char_u	    *arg_ప్రారంభం,
	garray_T    *gap,
	పూర్ణాంకం	    import_sid,
	evalarg_T   *evalarg,
	శూన్య	    *cctx)
{
    char_u	*arg = arg_ప్రారంభం;
    char_u	*nextarg;
    పూర్ణాంకం		is_autoload = FALSE;
    పూర్ణాంకం		getnext;
    char_u	*సమాసం_చివర;
    పూర్ణాంకం		ret = FAIL;
    char_u	*as_name = ఖాళీ;
    typval_T	tv;
    పూర్ణాంకం		sid = -2;
    పూర్ణాంకం		res;
    దీర్ఘ	ప్రారంభం_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    పూర్ణాంకం		i;

    ఒకవేళ (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, ఖాళీ, evalarg, &సమాసం_చివర);
    ఒకవేళ (ret == FAIL)
	వెళ్ళు erret;
    ఒకవేళ (tv.v_type != VAR_STRING
		       || tv.vval.v_string == ఖాళీ || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	వెళ్ళు erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = ప్రారంభం_lnum;

    /*
     * find script file
     */
    ఒకవేళ (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_అంశం(current_sctx.sc_sid);
	char_u		*తోక = gettail(si->sn_name);
	char_u		*from_name;
	పూర్ణాంకం		sourced_from_nofile_buf = FALSE;

	ఒకవేళ (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	ఒకవేళ (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(తోక) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    ఒకవేళ (from_name == ఖాళీ)
		వెళ్ళు erret;
	    vim_strncpy(from_name, si->sn_name, తోక - si->sn_name);
	    కూడు_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	లేకపోతే
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    లేకపోతే ఒకవేళ (mch_is_నిండిన_Name(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    లేకపోతే ఒకవేళ (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ఒకవేళ (from_name == ఖాళీ)
	    వెళ్ళు erret;
	vim_snprintf((అక్షరం *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = కనుగొను_script_లోపల_rtp(from_name);
	vim_free(from_name);
	ఒకవేళ (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_అంశం(sid);

	    ఒకవేళ (si->sn_autoload_ఉపసర్గ == ఖాళీ)
		si->sn_autoload_ఉపసర్గ = get_autoload_ఉపసర్గ(si);
	    res = OK;
	    ఒకవేళ (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(శూన్య)do_ఆధారం(si->sn_name, FALSE, DOSO_NONE, ఖాళీ);
	}
	లేకపోతే
	    res = FAIL;
    }
    లేకపోతే
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ఒకవేళ (from_name == ఖాళీ)
	    వెళ్ళు erret;
	vim_snprintf((అక్షరం *)from_name, len, "import/%s", tv.vval.v_string);
	res = ఆధారం_లోపల_మార్గం(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    ఒకవేళ (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_మార్గం
		    : e_could_not_import_str), tv.vval.v_string);
	వెళ్ళు erret;
    }

    ఒకవేళ (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	వెళ్ళు erret;
    }

    import_gap = gap != ఖాళీ ? gap : &SCRIPT_అంశం(import_sid)->sn_imports;
    కొరకు (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_దత్తాంశం + i;

	ఒకవేళ (import->imp_sid == sid)
	{
	    ఒకవేళ (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		ఆపు;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    వెళ్ళు erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_తదుపరి_non_blank(సమాసం_చివర, evalarg, &getnext);
    ఒకవేళ (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	ఒకవేళ (getnext)
	    arg = eval_తదుపరి_పంక్తి(సమాసం_చివర, evalarg);
	లేకపోతే
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	ఒకవేళ (eval_isnamec1(*arg))
	    అయితే (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	ఒకవేళ (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_లోపల_import_str), p);
	    వెళ్ళు erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    లేకపోతే
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *చివర = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)p, ".vim");

	ఒకవేళ (!చివర్లు_excmd2(arg_ప్రారంభం, సమాసం_చివర))
	{
	    semsg(_(e_trailing_characters_str), సమాసం_చివర);
	    వెళ్ళు erret;
	}
	ఒకవేళ (చివర == ఖాళీ || చివర[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_చివర_లోపల_dot_vim_str), p);
	    వెళ్ళు erret;
	}
	ఒకవేళ (చివర == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    వెళ్ళు erret;
	}
	as_name = vim_strnsave(p, చివర - p);
    }

    ఒకవేళ (as_name != ఖాళీ)
    {
	imported_T  *imported;

	imported = కనుగొను_imported(as_name, STRLEN(as_name), FALSE);
	ఒకవేళ (imported != ఖాళీ && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    వెళ్ళు erret;
	}
	లేకపోతే ఒకవేళ (imported == ఖాళీ
		&& check_defined(as_name, STRLEN(as_name), cctx, ఖాళీ,
								FALSE) == FAIL)
	    వెళ్ళు erret;

	ఒకవేళ (imported == ఖాళీ)
	{
	    imported = new_imported(import_gap);
	    ఒకవేళ (imported == ఖాళీ)
		వెళ్ళు erret;
	    imported->imp_name = as_name;
	    as_name = ఖాళీ;
	    imported->imp_sid = sid;
	    ఒకవేళ (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    తిరిగి arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    శూన్య
ex_import(exarg_T *eap)
{
    char_u	*cmd_చివర;
    evalarg_T	evalarg;

    ఒకవేళ (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_వాడిన_లోపల_script));
	తిరిగి;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_చివర = handle_import(eap->arg, ఖాళీ, current_sctx.sc_sid,
							       &evalarg, ఖాళీ);
    ఒకవేళ (cmd_చివర != ఖాళీ)
	సమితి_nextcmd(eap, cmd_చివర);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    శూన్య
import_check_sourced_sid(పూర్ణాంకం *sid)
{
    scriptitem_T *script = SCRIPT_అంశం(*sid);

    ఒకవేళ (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    పూర్ణాంకం
కనుగొను_exported(
	పూర్ణాంకం	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	పూర్ణాంకం	    verbose)
{
    పూర్ణాంకం		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_అంశం(sid);

    *ufunc = ఖాళీ;

    ఒకవేళ (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	ఒకవేళ (do_ఆధారం(script->sn_name, FALSE, DOSO_NONE, ఖాళీ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    తిరిగి -1;
	}
    }

    // Find name in "script".
    idx = get_script_అంశం_idx(sid, name, 0, cctx, cstack);
    ఒకవేళ (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_దత్తాంశం) + idx;
	ఒకవేళ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_అంశం_not_exported_లోపల_script_str), name);
	    తిరిగి -1;
	}
	*type = sv->sv_type;
    }
    లేకపోతే
    {
	size_t	len = STRLEN(name);
	char_u	బఫరు[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	ఒకవేళ (script->sn_autoload_ఉపసర్గ != ఖాళీ)
	    len += STRLEN(script->sn_autoload_ఉపసర్గ) + 2;
	లేకపోతే
	    len += 15;

	ఒకవేళ (len < పరిమాణం(బఫరు))
	    funcname = బఫరు;
	లేకపోతే
	{
	    funcname = alloc(len);
	    ఒకవేళ (funcname == ఖాళీ)
		తిరిగి -1;
	}
	ఒకవేళ (script->sn_autoload_ఉపసర్గ != ఖాళీ)
	{
	    sprintf((అక్షరం *)funcname, "%s%s", script->sn_autoload_ఉపసర్గ, name);
	}
	లేకపోతే
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (పూర్ణాంకం)KE_SNR;
	    sprintf((అక్షరం *)funcname + 3, "%ld_%s", (దీర్ఘ)sid, name);
	}
	*ufunc = కనుగొను_func(funcname, FALSE);

	ఒకవేళ (*ufunc == ఖాళీ)
	{
	    ఒకవేళ (verbose)
	    {
		ufunc_T *alt_ufunc = ఖాళీ;

		ఒకవేళ (script->sn_autoload_ఉపసర్గ != ఖాళీ)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (పూర్ణాంకం)KE_SNR;
		    sprintf((అక్షరం *)funcname + 3, "%ld_%s", (దీర్ఘ)sid, name);
		    alt_ufunc = కనుగొను_func(funcname, FALSE);
		}
		ఒకవేళ (alt_ufunc != ఖాళీ)
		    semsg(_(e_అంశం_not_exported_లోపల_script_str), name);
		లేకపోతే
		    semsg(_(e_అంశం_not_దొరికింది_లోపల_script_str), name);
	    }
	}
	లేకపోతే ఒకవేళ (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_అంశం_not_exported_లోపల_script_str), name);
	    *ufunc = ఖాళీ;
	}
	ఒకవేళ (funcname != బఫరు)
	    vim_free(funcname);
    }

    తిరిగి idx;
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
    scriptitem_T    *si = SCRIPT_అంశం(current_sctx.sc_sid);
    type_T	    *type;
    typval_T	    init_tv;

    ఒకవేళ (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	ఒకవేళ (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_విలువ));
	లేకపోతే
	    emsg(_(e_const_requires_a_విలువ));
	తిరిగి arg + STRLEN(arg);
    }

    // Check for valid starting character.
    ఒకవేళ (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_వాదం_str), arg);
	తిరిగి arg + STRLEN(arg);
    }

    కొరకు (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	ఒకవేళ (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    ఆపు;

    ఒకవేళ (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	తిరిగి arg + STRLEN(arg);
    }
    ఒకవేళ (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	తిరిగి arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, ఖాళీ, ఖాళీ, TRUE);
    ఒకవేళ (type == ఖాళీ
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	తిరిగి p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_క్షేత్రం(init_tv);
    ఒకవేళ (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_సంఖ్య;
    లేకపోతే
	init_tv.v_type = type->tt_type;
    సమితి_var_const(name, 0, type, &init_tv, FALSE, కేటాయించు_INIT, 0);

    vim_free(name);
    తిరిగి p;
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
    శూన్య
update_vim9_script_var(
	పూర్ణాంకం	    create,
	dictitem_T  *di,
	char_u	    *name,
	పూర్ణాంకం	    flags,
	typval_T    *tv,
	type_T	    **type,
	పూర్ణాంకం	    do_member)
{
    scriptitem_T    *si = SCRIPT_అంశం(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = ఖాళీ;

    ఒకవేళ (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = ఖాళీ;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	ఒకవేళ (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    తిరిగి;

	hi = hash_కనుగొను(&si->sn_all_vars.dv_hashtab, name);
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    కొరకు (sav = HI2SAV(hi); ; sav = sav->sav_తదుపరి)
	    {
		ఒకవేళ (sav->sav_ఖండం_id == si->sn_current_ఖండం_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    ఒకవేళ (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    ఆపు;
		}
		ఒకవేళ (sav->sav_తదుపరి == ఖాళీ)
		    ఆపు;
	    }
	}

	ఒకవేళ (sv == ఖాళీ)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_కీలకం) + STRLEN(name) + 1);
	    ఒకవేళ (newsav == ఖాళీ)
		తిరిగి;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & కేటాయించు_FINAL) ? కేటాయించు_FINAL
				   : (flags & కేటాయించు_CONST) ? కేటాయించు_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    ఒకవేళ ((flags & కేటాయించు_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_కీలకం[0], name);
	    sv->sv_name = newsav->sav_కీలకం;
	    newsav->sav_di = di;
	    newsav->sav_ఖండం_id = si->sn_current_ఖండం_id;

	    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
		// new variable name
		hash_కూడు(&si->sn_all_vars.dv_hashtab, newsav->sav_కీలకం,
							       "add variable");
	    లేకపోతే ఒకవేళ (sav != ఖాళీ)
		// existing name in a new block, append to the list
		sav->sav_తదుపరి = newsav;
	}
    }
    లేకపోతే
    {
	sv = కనుగొను_typval_లోపల_script(&di->di_tv, 0, TRUE);
    }
    ఒకవేళ (sv != ఖాళీ)
    {
	ఒకవేళ (*type == ఖాళీ)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	లేకపోతే ఒకవేళ ((flags & కేటాయించు_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == ఖాళీ)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	ఒకవేళ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	ఒకవేళ (*type != ఖాళీ && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	లేకపోతే
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
    శూన్య
hide_script_var(scriptitem_T *si, పూర్ణాంకం idx, పూర్ణాంకం func_defined)
{
    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;
    hashtab_T	*script_ht = get_script_local_ht();
    hashtab_T	*all_ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*script_hi;
    hashitem_T	*all_hi;

    // Remove a variable declared inside the block, if it still exists.
    // If it was added in a nested block it will already have been removed.
    // The typval is moved into the sallvar_T.
    script_hi = hash_కనుగొను(script_ht, sv->sv_name);
    all_hi = hash_కనుగొను(all_ht, sv->sv_name);

    ఒకవేళ (HASHITEM_ఖాళీలు(script_hi) || HASHITEM_ఖాళీలు(all_hi))
	తిరిగి;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = ఖాళీ;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    అయితే (sav != ఖాళీ && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_తదుపరి;
    }
    ఒకవేళ (sav == ఖాళీ)
	తిరిగి;

    ఒకవేళ (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = ఖాళీ;
	sv->sv_tv = &sav->sav_tv;
    }
    లేకపోతే
    {
	ఒకవేళ (sav_prev == ఖాళీ)
	    hash_remove(all_ht, all_hi, "hide variable");
	లేకపోతే
	    sav_prev->sav_తదుపరి = sav->sav_తదుపరి;
	sv->sv_name = ఖాళీ;
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
కనుగొను_typval_లోపల_script(typval_T *dest, scid_T sid, పూర్ణాంకం must_కనుగొను)
{
    scriptitem_T    *si = SCRIPT_అంశం(sid == 0 ? current_sctx.sc_sid : sid);
    పూర్ణాంకం		    idx;

    ఒకవేళ (si->sn_సంస్కరణ != SCRIPT_సంస్కరణ_VIM9)
	// legacy script doesn't store variable types
	తిరిగి ఖాళీ;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    కొరకు (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	ఒకవేళ (sv->sv_name != ఖాళీ && sv->sv_tv == dest)
	    తిరిగి sv;
    }
    ఒకవేళ (must_కనుగొను)
	iemsg("find_typval_in_script(): not found");
    తిరిగి ఖాళీ;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    పూర్ణాంకం
check_script_var_type(
	svar_T	    *sv,
	typval_T    *విలువ,
	char_u	    *name,
	where_T	    where)
{
    పూర్ణాంకం	    ret;

    ఒకవేళ (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	తిరిగి FAIL;
    }
    ret = check_typval_type(sv->sv_type, విలువ, where);
    ఒకవేళ (ret == OK && need_convert_గమ్యం_bool(sv->sv_type, విలువ))
    {
	పూర్ణాంకం	val = tv2bool(విలువ);

	clear_tv(విలువ);
	విలువ->v_type = VAR_BOOL;
	విలువ->v_తాళం = 0;
	విలువ->vval.v_సంఖ్య = val ? VVAL_TRUE : VVAL_FALSE;
    }
    తిరిగి ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
స్థిర అక్షరం *reserved[] = {
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
    స్థిర పూర్ణాంకం
comp_పేర్లు(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    తిరిగి STRCMP(*(అక్షరం **)s1, *(అక్షరం **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    పూర్ణాంకం
check_reserved_name(char_u *name, పూర్ణాంకం is_objm_access)
{
    // "this" can be used in an object method
    ఒకవేళ (is_objm_access && STRCMP("this", name) == 0)
	తిరిగి OK;

    ఒకవేళ (bsearch(&name, reserved, ARRAY_నిడివి(reserved),
				పరిమాణం(reserved[0]), comp_పేర్లు) != ఖాళీ)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

#అయితే_ముగింపు // FEAT_EVAL
