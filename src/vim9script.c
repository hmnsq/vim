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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    ಪೂರ್ಣಾಂಕ
ಒಳ_old_script(ಪೂರ್ಣಾಂಕ max_ಆವೃತ್ತಿ)
{
    ಹಿಂತಿರುಗಿ (current_sctx.sc_ಆವೃತ್ತಿ < max_ಆವೃತ್ತಿ
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    ಪೂರ್ಣಾಂಕ
current_script_is_vim9(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ
						       == SCRIPT_ಆವೃತ್ತಿ_VIM9;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    ಶೂನ್ಯ
clear_vim9_scriptlocal_vars(ಪೂರ್ಣಾಂಕ sid)
{
    hashtab_T	*ht = &SCRIPT_VARS(sid);

    hashtab_free_contents(ht);
    hash_init(ht);
    delete_script_functions(sid);

    // also the exported variables of an autoload script
    delete_autoload_export_vars(SCRIPT_ಅಂಶ(sid)->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);

    // old imports and script variables are no longer valid
    free_imports_and_script_vars(sid);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":vim9script".
 */
    ಶೂನ್ಯ
ex_vim9script(exarg_T *eap ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    ಪೂರ್ಣಾಂಕ		    ಸಿಕ್ಕಿತು_noclear = FALSE;
    char_u	    *p;

    ಆದರೆ (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_ಬಳಸಿದ_ಒಳ_script));
	ಹಿಂತಿರುಗಿ;
    }

    si = SCRIPT_ಅಂಶ(sid);
    ಆದರೆ (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_ಮೊದಲ_command_ಒಳ_script));
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	ಆದರೆ (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    ಆದರೆ (ಸಿಕ್ಕಿತು_noclear)
	    {
		semsg(_(e_duplicate_ವಾದ_str), p);
		ಹಿಂತಿರುಗಿ;
	    }
	    ಸಿಕ್ಕಿತು_noclear = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (si->sn_state == SN_STATE_RELOAD && !ಸಿಕ್ಕಿತು_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ = get_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(si);

    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
    si->sn_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;

    ಆದರೆ (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    ಪೂರ್ಣಾಂಕ
not_ಒಳ_vim9(exarg_T *eap, bool silent)
{
    ಆದರೆ (ಒಳ_vim9script())
	ಆಯ್ಕೆ (eap->cmdidx)
	{
	    ಸ್ಥಿತಿ CMD_k:
		ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
		{
		    ಆದರೆ (!silent)
			emsg(_(e_no_range_allowed));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		// FALLTHROUGH
	    ಸ್ಥಿತಿ CMD_append:
	    ಸ್ಥಿತಿ CMD_change:
	    ಸ್ಥಿತಿ CMD_ಸೇರಿಸು:
	    ಸ್ಥಿತಿ CMD_open:
	    ಸ್ಥಿತಿ CMD_t:
	    ಸ್ಥಿತಿ CMD_xit:
		ಆದರೆ (!silent)
		    semsg(_(e_command_not_supported_ಒಳ_vim9_script_missing_var_str), eap->cmd);
		ಹಿಂತಿರುಗಿ FAIL;
	    ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	}
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    ಪೂರ್ಣಾಂಕ
vim9_bad_comment(char_u *p)
{
    ಆದರೆ (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	ಆದರೆ (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_ಗಮ್ಯ_ಆರಂಭ_comment));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    ಪೂರ್ಣಾಂಕ
vim9_comment_ಆರಂಭ(char_u *p)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಹಿಂತಿರುಗಿ p[0] == '#' && !vim9_bad_comment(p);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ p[0] == '#' && (p[1] != '{' || p[2] == '{');
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    ಶೂನ್ಯ
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    ಆದರೆ (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ಹಿಂತಿರುಗಿ;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    ಆದರೆ (eap->cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    vim_snprintf((ಅಕ್ಷರ *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = ಖಾಲಿ;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    ಶೂನ್ಯ
ex_export(exarg_T *eap ಬಳಸದ)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_ಬಳಸಿದ_ಒಳ_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    ಸ್ಥಿರ imported_T *
new_imported(garray_T *gap)
{
    ಆದರೆ (ga_grow(gap, 1) == OK)
	ಹಿಂತಿರುಗಿ ((imported_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len++);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_all_script_vars(scriptitem_T *si)
{
    ಪೂರ್ಣಾಂಕ		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		idx;

    hash_ಬೀಗ(ht);
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    ಆಗ (sav != ಖಾಲಿ)
	    {
		sav_ಮುಂದಿನ = sav->sav_ಮುಂದಿನ;
		ಆದರೆ (sav->sav_di == ಖಾಲಿ)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_ಮುಂದಿನ;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    ಗಾಗಿ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

	ಆದರೆ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    ಶೂನ್ಯ
free_imports_and_script_vars(ಪೂರ್ಣಾಂಕ sid)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(sid);
    ಪೂರ್ಣಾಂಕ		    idx;

    ಗಾಗಿ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ದತ್ತಾಂಶ) + idx;

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
    ಶೂನ್ಯ
mark_imports_for_reload(ಪೂರ್ಣಾಂಕ sid)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(sid);
    ಪೂರ್ಣಾಂಕ		    idx;

    ಗಾಗಿ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_ದತ್ತಾಂಶ) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_import_fname(char_u *fname, ಪೂರ್ಣಾಂಕ is_autoload, ಪೂರ್ಣಾಂಕ *sid)
{
    ಆದರೆ (is_autoload)
    {
	scriptitem_T	*si;

	*sid = ಕಂಡುಹಿಡಿ_script_by_name(fname);
	ಆದರೆ (*sid < 0)
	{
	    ಪೂರ್ಣಾಂಕ error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    ಆದರೆ (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    ಆದರೆ (error == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	si = SCRIPT_ಅಂಶ(*sid);
	si->sn_import_autoload = TRUE;

	ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	    si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ = get_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(si);

	// with testing override: load autoload script right away
	ಆದರೆ (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ do_ಮೂಲ(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    ಸ್ಥಿರ char_u *
handle_import(
	char_u	    *arg_ಆರಂಭ,
	garray_T    *gap,
	ಪೂರ್ಣಾಂಕ	    import_sid,
	evalarg_T   *evalarg,
	ಶೂನ್ಯ	    *cctx)
{
    char_u	*arg = arg_ಆರಂಭ;
    char_u	*nextarg;
    ಪೂರ್ಣಾಂಕ		is_autoload = FALSE;
    ಪೂರ್ಣಾಂಕ		getnext;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    char_u	*as_name = ಖಾಲಿ;
    typval_T	tv;
    ಪೂರ್ಣಾಂಕ		sid = -2;
    ಪೂರ್ಣಾಂಕ		res;
    ದೀರ್ಘ	ಆರಂಭ_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, ಖಾಲಿ, evalarg, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ);
    ಆದರೆ (ret == FAIL)
	ಹೋಗು erret;
    ಆದರೆ (tv.v_type != VAR_STRING
		       || tv.vval.v_string == ಖಾಲಿ || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	ಹೋಗು erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = ಆರಂಭ_lnum;

    /*
     * find script file
     */
    ಆದರೆ (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
	char_u		*ಬಾಲ = gettail(si->sn_name);
	char_u		*from_name;
	ಪೂರ್ಣಾಂಕ		sourced_from_nofile_buf = FALSE;

	ಆದರೆ (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	ಆದರೆ (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(ಬಾಲ) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    ಆದರೆ (from_name == ಖಾಲಿ)
		ಹೋಗು erret;
	    vim_strncpy(from_name, si->sn_name, ಬಾಲ - si->sn_name);
	    ಕೂಡಿಸು_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mch_is_ತುಂಬಿದ_Name(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ಆದರೆ (from_name == ಖಾಲಿ)
	    ಹೋಗು erret;
	vim_snprintf((ಅಕ್ಷರ *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = ಕಂಡುಹಿಡಿ_script_ಒಳ_rtp(from_name);
	vim_free(from_name);
	ಆದರೆ (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ಅಂಶ(sid);

	    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
		si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ = get_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(si);
	    res = OK;
	    ಆದರೆ (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(ಶೂನ್ಯ)do_ಮೂಲ(si->sn_name, FALSE, DOSO_NONE, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    res = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ಆದರೆ (from_name == ಖಾಲಿ)
	    ಹೋಗು erret;
	vim_snprintf((ಅಕ್ಷರ *)from_name, len, "import/%s", tv.vval.v_string);
	res = ಮೂಲ_ಒಳ_ಮಾರ್ಗ(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    ಆದರೆ (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_ಮಾರ್ಗ
		    : e_could_not_import_str), tv.vval.v_string);
	ಹೋಗು erret;
    }

    ಆದರೆ (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	ಹೋಗು erret;
    }

    import_gap = gap != ಖಾಲಿ ? gap : &SCRIPT_ಅಂಶ(import_sid)->sn_imports;
    ಗಾಗಿ (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_ದತ್ತಾಂಶ + i;

	ಆದರೆ (import->imp_sid == sid)
	{
	    ಆದರೆ (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		ನಿಲ್ಲಿಸು;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    ಹೋಗು erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_ಮುಂದಿನ_non_blank(ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, evalarg, &getnext);
    ಆದರೆ (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	ಆದರೆ (getnext)
	    arg = eval_ಮುಂದಿನ_ಸಾಲು(ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, evalarg);
	ಇಲ್ಲದಿದ್ದರೆ
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	ಆದರೆ (eval_isnamec1(*arg))
	    ಆಗ (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	ಆದರೆ (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_ಒಳ_import_str), p);
	    ಹೋಗು erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *ಕೊನೆ = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)p, ".vim");

	ಆದರೆ (!ತುದಿಗಳು_excmd2(arg_ಆರಂಭ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ))
	{
	    semsg(_(e_trailing_characters_str), ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ);
	    ಹೋಗು erret;
	}
	ಆದರೆ (ಕೊನೆ == ಖಾಲಿ || ಕೊನೆ[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_ಕೊನೆ_ಒಳ_dot_vim_str), p);
	    ಹೋಗು erret;
	}
	ಆದರೆ (ಕೊನೆ == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    ಹೋಗು erret;
	}
	as_name = vim_strnsave(p, ಕೊನೆ - p);
    }

    ಆದರೆ (as_name != ಖಾಲಿ)
    {
	imported_T  *imported;

	imported = ಕಂಡುಹಿಡಿ_imported(as_name, STRLEN(as_name), FALSE);
	ಆದರೆ (imported != ಖಾಲಿ && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    ಹೋಗು erret;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (imported == ಖಾಲಿ
		&& check_defined(as_name, STRLEN(as_name), cctx, ಖಾಲಿ,
								FALSE) == FAIL)
	    ಹೋಗು erret;

	ಆದರೆ (imported == ಖಾಲಿ)
	{
	    imported = new_imported(import_gap);
	    ಆದರೆ (imported == ಖಾಲಿ)
		ಹೋಗು erret;
	    imported->imp_name = as_name;
	    as_name = ಖಾಲಿ;
	    imported->imp_sid = sid;
	    ಆದರೆ (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    ಹಿಂತಿರುಗಿ arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    ಶೂನ್ಯ
ex_import(exarg_T *eap)
{
    char_u	*cmd_ಕೊನೆ;
    evalarg_T	evalarg;

    ಆದರೆ (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_ಬಳಸಿದ_ಒಳ_script));
	ಹಿಂತಿರುಗಿ;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_ಕೊನೆ = handle_import(eap->arg, ಖಾಲಿ, current_sctx.sc_sid,
							       &evalarg, ಖಾಲಿ);
    ಆದರೆ (cmd_ಕೊನೆ != ಖಾಲಿ)
	ಗಣ_nextcmd(eap, cmd_ಕೊನೆ);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    ಶೂನ್ಯ
import_check_sourced_sid(ಪೂರ್ಣಾಂಕ *sid)
{
    scriptitem_T *script = SCRIPT_ಅಂಶ(*sid);

    ಆದರೆ (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_exported(
	ಪೂರ್ಣಾಂಕ	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ಪೂರ್ಣಾಂಕ	    verbose)
{
    ಪೂರ್ಣಾಂಕ		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ಅಂಶ(sid);

    *ufunc = ಖಾಲಿ;

    ಆದರೆ (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	ಆದರೆ (do_ಮೂಲ(script->sn_name, FALSE, DOSO_NONE, ಖಾಲಿ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    ಹಿಂತಿರುಗಿ -1;
	}
    }

    // Find name in "script".
    idx = get_script_ಅಂಶ_idx(sid, name, 0, cctx, cstack);
    ಆದರೆ (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;
	ಆದರೆ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str), name);
	    ಹಿಂತಿರುಗಿ -1;
	}
	*type = sv->sv_type;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t	len = STRLEN(name);
	char_u	ಬಫರ್[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	ಆದರೆ (script->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	    len += STRLEN(script->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ) + 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    len += 15;

	ಆದರೆ (len < ಗಾತ್ರ(ಬಫರ್))
	    funcname = ಬಫರ್;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    funcname = alloc(len);
	    ಆದರೆ (funcname == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ (script->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	{
	    sprintf((ಅಕ್ಷರ *)funcname, "%s%s", script->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
	    sprintf((ಅಕ್ಷರ *)funcname + 3, "%ld_%s", (ದೀರ್ಘ)sid, name);
	}
	*ufunc = ಕಂಡುಹಿಡಿ_func(funcname, FALSE);

	ಆದರೆ (*ufunc == ಖಾಲಿ)
	{
	    ಆದರೆ (verbose)
	    {
		ufunc_T *alt_ufunc = ಖಾಲಿ;

		ಆದರೆ (script->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
		    sprintf((ಅಕ್ಷರ *)funcname + 3, "%ld_%s", (ದೀರ್ಘ)sid, name);
		    alt_ufunc = ಕಂಡುಹಿಡಿ_func(funcname, FALSE);
		}
		ಆದರೆ (alt_ufunc != ಖಾಲಿ)
		    semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str), name);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_ಅಂಶ_not_ಸಿಕ್ಕಿತು_ಒಳ_script_str), name);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str), name);
	    *ufunc = ಖಾಲಿ;
	}
	ಆದರೆ (funcname != ಬಫರ್)
	    vim_free(funcname);
    }

    ಹಿಂತಿರುಗಿ idx;
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
    scriptitem_T    *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    type_T	    *type;
    typval_T	    init_tv;

    ಆದರೆ (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	ಆದರೆ (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_ಮೌಲ್ಯ));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_const_requires_a_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ arg + STRLEN(arg);
    }

    // Check for valid starting character.
    ಆದರೆ (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_ವಾದ_str), arg);
	ಹಿಂತಿರುಗಿ arg + STRLEN(arg);
    }

    ಗಾಗಿ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	ಆದರೆ (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	ಹಿಂತಿರುಗಿ arg + STRLEN(arg);
    }
    ಆದರೆ (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	ಹಿಂತಿರುಗಿ arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, ಖಾಲಿ, ಖಾಲಿ, TRUE);
    ಆದರೆ (type == ಖಾಲಿ
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	ಹಿಂತಿರುಗಿ p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_ಕ್ಷೇತ್ರ(init_tv);
    ಆದರೆ (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    ಇಲ್ಲದಿದ್ದರೆ
	init_tv.v_type = type->tt_type;
    ಗಣ_var_const(name, 0, type, &init_tv, FALSE, ನಿಯೋಜಿಸು_INIT, 0);

    vim_free(name);
    ಹಿಂತಿರುಗಿ p;
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
    ಶೂನ್ಯ
update_vim9_script_var(
	ಪೂರ್ಣಾಂಕ	    create,
	dictitem_T  *di,
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    flags,
	typval_T    *tv,
	type_T	    **type,
	ಪೂರ್ಣಾಂಕ	    do_member)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = ಖಾಲಿ;

    ಆದರೆ (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = ಖಾಲಿ;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	ಆದರೆ (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	hi = hash_ಕಂಡುಹಿಡಿ(&si->sn_all_vars.dv_hashtab, name);
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    ಗಾಗಿ (sav = HI2SAV(hi); ; sav = sav->sav_ಮುಂದಿನ)
	    {
		ಆದರೆ (sav->sav_ಖಂಡ_id == si->sn_current_ಖಂಡ_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    ಆದರೆ (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (sav->sav_ಮುಂದಿನ == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (sv == ಖಾಲಿ)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_ಕೀಲಿ) + STRLEN(name) + 1);
	    ಆದರೆ (newsav == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ನಿಯೋಜಿಸು_FINAL) ? ನಿಯೋಜಿಸು_FINAL
				   : (flags & ನಿಯೋಜಿಸು_CONST) ? ನಿಯೋಜಿಸು_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    ಆದರೆ ((flags & ನಿಯೋಜಿಸು_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_ಕೀಲಿ[0], name);
	    sv->sv_name = newsav->sav_ಕೀಲಿ;
	    newsav->sav_di = di;
	    newsav->sav_ಖಂಡ_id = si->sn_current_ಖಂಡ_id;

	    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
		// new variable name
		hash_ಕೂಡಿಸು(&si->sn_all_vars.dv_hashtab, newsav->sav_ಕೀಲಿ,
							       "add variable");
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sav != ಖಾಲಿ)
		// existing name in a new block, append to the list
		sav->sav_ಮುಂದಿನ = newsav;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(&di->di_tv, 0, TRUE);
    }
    ಆದರೆ (sv != ಖಾಲಿ)
    {
	ಆದರೆ (*type == ಖಾಲಿ)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((flags & ನಿಯೋಜಿಸು_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == ಖಾಲಿ)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	ಆದರೆ (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	ಆದರೆ (*type != ಖಾಲಿ && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಶೂನ್ಯ
hide_script_var(scriptitem_T *si, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ func_defined)
{
    svar_T	*sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;
    hashtab_T	*script_ht = get_script_local_ht();
    hashtab_T	*all_ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*script_hi;
    hashitem_T	*all_hi;

    // Remove a variable declared inside the block, if it still exists.
    // If it was added in a nested block it will already have been removed.
    // The typval is moved into the sallvar_T.
    script_hi = hash_ಕಂಡುಹಿಡಿ(script_ht, sv->sv_name);
    all_hi = hash_ಕಂಡುಹಿಡಿ(all_ht, sv->sv_name);

    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(script_hi) || HASHITEM_ಖಾಲಿಗಳು(all_hi))
	ಹಿಂತಿರುಗಿ;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = ಖಾಲಿ;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    ಆಗ (sav != ಖಾಲಿ && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_ಮುಂದಿನ;
    }
    ಆದರೆ (sav == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = ಖಾಲಿ;
	sv->sv_tv = &sav->sav_tv;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (sav_prev == ಖಾಲಿ)
	    hash_remove(all_ht, all_hi, "hide variable");
	ಇಲ್ಲದಿದ್ದರೆ
	    sav_prev->sav_ಮುಂದಿನ = sav->sav_ಮುಂದಿನ;
	sv->sv_name = ಖಾಲಿ;
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
ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(typval_T *dest, scid_T sid, ಪೂರ್ಣಾಂಕ must_ಕಂಡುಹಿಡಿ)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(sid == 0 ? current_sctx.sc_sid : sid);
    ಪೂರ್ಣಾಂಕ		    idx;

    ಆದರೆ (si->sn_ಆವೃತ್ತಿ != SCRIPT_ಆವೃತ್ತಿ_VIM9)
	// legacy script doesn't store variable types
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    ಗಾಗಿ (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	ಆದರೆ (sv->sv_name != ಖಾಲಿ && sv->sv_tv == dest)
	    ಹಿಂತಿರುಗಿ sv;
    }
    ಆದರೆ (must_ಕಂಡುಹಿಡಿ)
	iemsg("find_typval_in_script(): not found");
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    ಪೂರ್ಣಾಂಕ
check_script_var_type(
	svar_T	    *sv,
	typval_T    *ಮೌಲ್ಯ,
	char_u	    *name,
	where_T	    where)
{
    ಪೂರ್ಣಾಂಕ	    ret;

    ಆದರೆ (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ret = check_typval_type(sv->sv_type, ಮೌಲ್ಯ, where);
    ಆದರೆ (ret == OK && need_convert_ಗಮ್ಯ_bool(sv->sv_type, ಮೌಲ್ಯ))
    {
	ಪೂರ್ಣಾಂಕ	val = tv2bool(ಮೌಲ್ಯ);

	clear_tv(ಮೌಲ್ಯ);
	ಮೌಲ್ಯ->v_type = VAR_BOOL;
	ಮೌಲ್ಯ->v_ಬೀಗ = 0;
	ಮೌಲ್ಯ->vval.v_ಸಂಖ್ಯೆ = val ? VVAL_TRUE : VVAL_FALSE;
    }
    ಹಿಂತಿರುಗಿ ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
ಸ್ಥಿರ ಅಕ್ಷರ *reserved[] = {
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
comp_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ಹಿಂತಿರುಗಿ STRCMP(*(ಅಕ್ಷರ **)s1, *(ಅಕ್ಷರ **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    ಪೂರ್ಣಾಂಕ
check_reserved_name(char_u *name, ಪೂರ್ಣಾಂಕ is_objm_access)
{
    // "this" can be used in an object method
    ಆದರೆ (is_objm_access && STRCMP("this", name) == 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (bsearch(&name, reserved, ARRAY_ಉದ್ದಳತೆ(reserved),
				ಗಾತ್ರ(reserved[0]), comp_ಹೆಸರುಗಳು) != ಖಾಲಿ)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
