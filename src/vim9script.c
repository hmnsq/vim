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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE when currently in a script with script version smaller than
 * "max_version" or command modifiers forced it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_old_script(ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_version)
{
    ଫେରନ୍ତୁ (current_sctx.sc_version < max_version
					&& !(cmdmod.cmod_flags & CMOD_VIM9CMD))
		|| (cmdmod.cmod_flags & CMOD_LEGACY);
}

/*
 * Return TRUE if the current script is Vim9 script.
 * This also returns TRUE in a legacy function in a Vim9 script.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_script_is_vim9(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
						       == SCRIPT_VERSION_VIM9;
}
#endif

#ifdef FEAT_EVAL
/*
 * Clear Vim9 script-local variables and functions.
 */
    ଶୂନ୍ୟ
clear_vim9_scriptlocal_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
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
    ଶୂନ୍ୟ
ex_vim9script(exarg_T *eap UNUSED)
{
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    sid = current_sctx.sc_sid;
    scriptitem_T    *si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    found_noclear = FALSE;
    char_u	    *p;

    ଯଦି (!sourcing_a_script(eap))
    {
	emsg(_(e_vim9script_can_only_be_used_in_script));
	ଫେରନ୍ତୁ;
    }

    si = SCRIPT_ITEM(sid);
    ଯଦି (si->sn_state == SN_STATE_HAD_COMMAND)
    {
	emsg(_(e_vim9script_must_be_first_command_in_script));
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (p = eap->arg; !IS_WHITE_OR_NUL(*p); p = skipwhite(skiptowhite(p)))
    {
	ଯଦି (STRNCMP(p, "noclear", 7) == 0 && IS_WHITE_OR_NUL(p[7]))
	{
	    ଯଦି (found_noclear)
	    {
		semsg(_(e_duplicate_argument_str), p);
		ଫେରନ୍ତୁ;
	    }
	    found_noclear = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (si->sn_state == SN_STATE_RELOAD && !found_noclear)
	// Reloading a script without the "noclear" argument: clear
	// script-local variables and functions.
	clear_vim9_scriptlocal_vars(sid);
    si->sn_state = SN_STATE_HAD_COMMAND;

    // Store the prefix with the script, it is used to find exported functions.
    ଯଦି (si->sn_autoload_prefix == ଖାଲି)
	si->sn_autoload_prefix = get_autoload_prefix(si);

    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    si->sn_version = SCRIPT_VERSION_VIM9;

    ଯଦି (STRCMP(p_cpo, CPO_VIM) != 0)
    {
	si->sn_save_cpo = vim_strsave(p_cpo);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"cpo",
					 0L, (char_u *)CPO_VIM, OPT_NO_REDRAW);
    }
#ଅନ୍ୟଥା
    // No check for this being the first command, the information is not
    // available.
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
#endif
}

#ଯଦି defined(FEAT_EVAL)
/*
 * When in Vim9 script give an error (unless "silent" is true) and return FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
not_in_vim9(exarg_T *eap, bool silent)
{
    ଯଦି (in_vim9script())
	ବାଛ (eap->cmdidx)
	{
	    ସ୍ଥିତି CMD_k:
		ଯଦି (eap->addr_ଗଣନା > 0)
		{
		    ଯଦି (!silent)
			emsg(_(e_no_range_allowed));
		    ଫେରନ୍ତୁ FAIL;
		}
		// FALLTHROUGH
	    ସ୍ଥିତି CMD_append:
	    ସ୍ଥିତି CMD_change:
	    ସ୍ଥିତି CMD_ଭର୍ତ୍ତି:
	    ସ୍ଥିତି CMD_open:
	    ସ୍ଥିତି CMD_t:
	    ସ୍ଥିତି CMD_xit:
		ଯଦି (!silent)
		    semsg(_(e_command_not_supported_in_vim9_script_missing_var_str), eap->cmd);
		ଫେରନ୍ତୁ FAIL;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	}
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "p" points at "#{", not "#{{".
 * Give an error message if not done already.
 * This avoids that using a legacy style #{} dictionary leads to difficult to
 * understand errors.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim9_bad_comment(char_u *p)
{
    ଯଦି (p[0] == '#' && p[1] == '{' && p[2] != '{')
    {
	ଯଦି (!did_emsg)
	    emsg(_(e_cannot_use_hash_curly_to_start_comment));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Return TRUE if "p" points at a "#" not followed by one '{'.
 * Gives an error for using "#{", not for "#{{".
 * Does not check for white space.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim9_comment_start(char_u *p)
{
#ifdef FEAT_EVAL
    ଫେରନ୍ତୁ p[0] == '#' && !vim9_bad_comment(p);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ p[0] == '#' && (p[1] != '{' || p[2] == '{');
#endif
}

#ଯଦି defined(FEAT_EVAL)

/*
 * "++nr" and "--nr" commands.
 */
    ଶୂନ୍ୟ
ex_incdec(exarg_T *eap)
{
    char_u	*cmd = eap->cmd;
    char_u	*nextcmd = eap->nextcmd;
    size_t	len = STRLEN(eap->cmd) + 8;

    ଯଦି (VIM_ISWHITE(cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
			 eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ଫେରନ୍ତୁ;
    }

    // This works like "nr += 1" or "nr -= 1".
    // Add a '|' to avoid looking in the next line.
    eap->cmd = alloc(len);
    ଯଦି (eap->cmd == ଖାଲି)
	ଫେରନ୍ତୁ;
    vim_snprintf((ଅକ୍ଷର *)eap->cmd, len, "%s %c= 1 |", cmd + 2,
				     eap->cmdidx == CMD_increment ? '+' : '-');
    eap->arg = eap->cmd;
    eap->cmdidx = CMD_var;
    eap->nextcmd = ଖାଲି;
    ex_let(eap);
    vim_free(eap->cmd);
    eap->cmd = cmd;
    eap->nextcmd = nextcmd;
}

/*
 * ":export cmd"
 */
    ଶୂନ୍ୟ
ex_export(exarg_T *eap UNUSED)
{
    // can only get here when "export" wasn't caught in do_cmdline()
    emsg(_(e_export_can_only_be_used_in_vim9script));
}

/*
 * Add a new imported item entry to the current script.
 */
    ସ୍ଥିର imported_T *
new_imported(garray_T *gap)
{
    ଯଦି (ga_grow(gap, 1) == OK)
	ଫେରନ୍ତୁ ((imported_T *)gap->ga_data + gap->ga_len++);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Free the script variables from "sn_all_vars".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_all_script_vars(scriptitem_T *si)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashtab_T	*ht = &si->sn_all_vars.dv_hashtab;
    hashitem_T	*hi;
    sallvar_T	*sav;
    sallvar_T	*sav_next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    hash_lock(ht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab, ht_array
	    // might change then.  hash_clear() takes care of it later.
	    sav = HI2SAV(hi);
	    ଯେତେବେଳେ (sav != ଖାଲି)
	    {
		sav_next = sav->sav_next;
		ଯଦି (sav->sav_di == ଖାଲି)
		    clear_tv(&sav->sav_tv);
		vim_free(sav);
		sav = sav_next;
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);

    ପାଇଁ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	ଯଦି (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
    }
    ga_clear(&si->sn_var_vals);

    // existing commands using script variable indexes are no longer valid
    si->sn_script_seq = current_sctx.sc_seq;
}

/*
 * Free all imported items in script "sid".
 */
    ଶୂନ୍ୟ
free_imports_and_script_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;

    ପାଇଁ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
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
    ଶୂନ୍ୟ
mark_imports_for_reload(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;

    ପାଇଁ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *imp = ((imported_T *)si->sn_imports.ga_data) + idx;

	imp->imp_flags |= IMP_FLAGS_RELOAD;
    }
}

/*
 * Part of "import" that handles a relative or absolute file name/
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_import_fname(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_autoload, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sid)
{
    ଯଦି (is_autoload)
    {
	scriptitem_T	*si;

	*sid = find_script_by_name(fname);
	ଯଦି (*sid < 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = OK;

	    // Script does not exist yet, check name and create a new
	    // scriptitem.
	    ଯଦି (!file_is_readable(fname))
	    {
		semsg(_(mch_isdir(fname) ? e_str_is_directory
					  : e_cannot_read_from_str_2), fname);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *sid = get_new_scriptitem_for_fname(&error, fname);
	    ଯଦି (error == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	si = SCRIPT_ITEM(*sid);
	si->sn_import_autoload = TRUE;

	ଯଦି (si->sn_autoload_prefix == ଖାଲି)
	    si->sn_autoload_prefix = get_autoload_prefix(si);

	// with testing override: load autoload script right away
	ଯଦି (!override_autoload || si->sn_state != SN_STATE_NOT_LOADED)
	    ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ do_source(fname, FALSE, DOSO_NONE, sid);
}

/*
 * Handle an ":import" command and add the resulting imported_T to "gap", when
 * not NULL, or script "import_sid" sn_imports.
 * "cctx" is NULL at the script level.
 * Returns a pointer to after the command or NULL in case of failure
 */
    ସ୍ଥିର char_u *
handle_import(
	char_u	    *arg_start,
	garray_T    *gap,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    import_sid,
	evalarg_T   *evalarg,
	ଶୂନ୍ୟ	    *cctx)
{
    char_u	*arg = arg_start;
    char_u	*nextarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_autoload = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;
    char_u	*ପଦସମଷ୍ଟି_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    char_u	*as_name = ଖାଲି;
    typval_T	tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sid = -2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    long	start_lnum = SOURCING_LNUM;
    garray_T	*import_gap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (STRNCMP(arg, "autoload", 8) == 0 && VIM_ISWHITE(arg[8]))
    {
	is_autoload = TRUE;
	arg = skipwhite(arg + 8);
    }

    // The name of the file can be an expression, which must evaluate to a
    // string.
    ret = eval0_retarg(arg, &tv, ଖାଲି, evalarg, &ପଦସମଷ୍ଟି_end);
    ଯଦି (ret == FAIL)
	ଯାଅ erret;
    ଯଦି (tv.v_type != VAR_STRING
		       || tv.vval.v_string == ଖାଲି || *tv.vval.v_string == NUL)
    {
	semsg(_(e_invalid_string_for_import_str), arg);
	ଯାଅ erret;
    }

    // Give error messages for the start of the line.
    SOURCING_LNUM = start_lnum;

    /*
     * find script file
     */
    ଯଦି (*tv.vval.v_string == '.')
    {
	size_t		len;
	scriptitem_T	*si = SCRIPT_ITEM(current_sctx.sc_sid);
	char_u		*tail = gettail(si->sn_name);
	char_u		*from_name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sourced_from_nofile_buf = FALSE;

	ଯଦି (STRNCMP(si->sn_name, ":source buffer=", 15) == 0)
	    sourced_from_nofile_buf = TRUE;

	ଯଦି (!sourced_from_nofile_buf)
	{
	    // Relative to current script: "./name.vim", "../../name.vim".
	    len = STRLEN(si->sn_name) - STRLEN(tail) + STRLEN(tv.vval.v_string) + 2;
	    from_name = alloc(len);
	    ଯଦି (from_name == ଖାଲି)
		ଯାଅ erret;
	    vim_strncpy(from_name, si->sn_name, tail - si->sn_name);
	    add_pathsep(from_name);
	    STRCAT(from_name, tv.vval.v_string);
	}
	ଅନ୍ୟଥା
	    from_name = vim_strsave(tv.vval.v_string);
	simplify_filename(from_name);

	res = handle_import_fname(from_name, is_autoload, &sid);
	vim_free(from_name);
    }
    ଅନ୍ୟଥା ଯଦି (mch_isFullName(tv.vval.v_string))
    {
	// Absolute path: "/tmp/name.vim"
	res = handle_import_fname(tv.vval.v_string, is_autoload, &sid);
    }
    ଅନ୍ୟଥା ଯଦି (is_autoload)
    {
	size_t	    len = 9 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "autoload" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ଯଦି (from_name == ଖାଲି)
	    ଯାଅ erret;
	vim_snprintf((ଅକ୍ଷର *)from_name, len, "autoload/%s", tv.vval.v_string);
	// we need a scriptitem without loading the script
	sid = find_script_in_rtp(from_name);
	vim_free(from_name);
	ଯଦି (SCRIPT_ID_VALID(sid))
	{
	    scriptitem_T    *si = SCRIPT_ITEM(sid);

	    ଯଦି (si->sn_autoload_prefix == ଖାଲି)
		si->sn_autoload_prefix = get_autoload_prefix(si);
	    res = OK;
	    ଯଦି (override_autoload && si->sn_state == SN_STATE_NOT_LOADED)
		// testing override: load autoload script right away
		(ଶୂନ୍ୟ)do_source(si->sn_name, FALSE, DOSO_NONE, ଖାଲି);
	}
	ଅନ୍ୟଥା
	    res = FAIL;
    }
    ଅନ୍ୟଥା
    {
	size_t	    len = 7 + STRLEN(tv.vval.v_string) + 1;
	char_u	    *from_name;

	// Find file in "import" subdirs in 'runtimepath'.
	from_name = alloc(len);
	ଯଦି (from_name == ଖାଲି)
	    ଯାଅ erret;
	vim_snprintf((ଅକ୍ଷର *)from_name, len, "import/%s", tv.vval.v_string);
	res = source_in_path(p_rtp, from_name, DIP_NOAFTER, &sid);
	vim_free(from_name);
    }

    ଯଦି (res == FAIL || sid <= 0)
    {
	semsg(_(is_autoload && sid == -2
		    ? e_autoload_import_cannot_use_absolute_or_relative_path
		    : e_could_not_import_str), tv.vval.v_string);
	ଯାଅ erret;
    }

    ଯଦି (sid == current_sctx.sc_sid)
    {
	emsg(_(e_script_cannot_import_itself));
	ଯାଅ erret;
    }

    import_gap = gap != ଖାଲି ? gap : &SCRIPT_ITEM(import_sid)->sn_imports;
    ପାଇଁ (i = 0; i < import_gap->ga_len; ++i)
    {
	imported_T *import = (imported_T *)import_gap->ga_data + i;

	ଯଦି (import->imp_sid == sid)
	{
	    ଯଦି (import->imp_flags & IMP_FLAGS_RELOAD)
	    {
		// encountering same script first time on a reload is OK
		import->imp_flags &= ~IMP_FLAGS_RELOAD;
		ଭାଙ୍ଗ;
	    }
	    semsg(_(e_cannot_import_same_script_twice_str), tv.vval.v_string);
	    ଯାଅ erret;
	}
    }

    // Allow for the "as Name" to be in the next line.
    nextarg = eval_next_non_blank(ପଦସମଷ୍ଟି_end, evalarg, &getnext);
    ଯଦି (STRNCMP("as", nextarg, 2) == 0 && IS_WHITE_OR_NUL(nextarg[2]))
    {
	char_u *p;

	ଯଦି (getnext)
	    arg = eval_next_ଧାଡ଼ି(ପଦସମଷ୍ଟି_end, evalarg);
	ଅନ୍ୟଥା
	    arg = nextarg;

	// Skip over "as Name "; no line break allowed after "as".
	// Do not allow for ':' and '#'.
	arg = skipwhite(arg + 2);
	p = arg;
	ଯଦି (eval_isnamec1(*arg))
	    ଯେତେବେଳେ (ASCII_ISALNUM(*arg) || *arg == '_')
		++arg;
	ଯଦି (p == arg || !IS_WHITE_OR_NUL(*arg))
	{
	    semsg(_(e_syntax_error_in_import_str), p);
	    ଯାଅ erret;
	}
	as_name = vim_strnsave(p, arg - p);
	arg = skipwhite(arg);
    }
    ଅନ୍ୟଥା
    {
	char_u *p = gettail(tv.vval.v_string);
	char_u *end = (char_u *)strstr((ଅକ୍ଷର *)p, ".vim");

	ଯଦି (!ends_excmd2(arg_start, ପଦସମଷ୍ଟି_end))
	{
	    semsg(_(e_trailing_characters_str), ପଦସମଷ୍ଟି_end);
	    ଯାଅ erret;
	}
	ଯଦି (end == ଖାଲି || end[4] != NUL)
	{
	    semsg(_(e_imported_script_must_use_as_or_end_in_dot_vim_str), p);
	    ଯାଅ erret;
	}
	ଯଦି (end == p)
	{
	    semsg(_(e_cannot_import_dot_vim_without_using_as), p);
	    ଯାଅ erret;
	}
	as_name = vim_strnsave(p, end - p);
    }

    ଯଦି (as_name != ଖାଲି)
    {
	imported_T  *imported;

	imported = find_imported(as_name, STRLEN(as_name), FALSE);
	ଯଦି (imported != ଖାଲି && imported->imp_sid != sid)
	{
	    semsg(_(e_name_already_defined_str), as_name);
	    ଯାଅ erret;
	}
	ଅନ୍ୟଥା ଯଦି (imported == ଖାଲି
		&& check_defined(as_name, STRLEN(as_name), cctx, ଖାଲି,
								FALSE) == FAIL)
	    ଯାଅ erret;

	ଯଦି (imported == ଖାଲି)
	{
	    imported = new_imported(import_gap);
	    ଯଦି (imported == ଖାଲି)
		ଯାଅ erret;
	    imported->imp_name = as_name;
	    as_name = ଖାଲି;
	    imported->imp_sid = sid;
	    ଯଦି (is_autoload)
		imported->imp_flags = IMP_FLAGS_AUTOLOAD;
	}
    }

erret:
    clear_tv(&tv);
    vim_free(as_name);
    ଫେରନ୍ତୁ arg;
}

/*
 * ":import 'filename'"
 * ":import 'filename' as Name"
 */
    ଶୂନ୍ୟ
ex_import(exarg_T *eap)
{
    char_u	*cmd_end;
    evalarg_T	evalarg;

    ଯଦି (!sourcing_a_script(eap))
    {
	emsg(_(e_import_can_only_be_used_in_script));
	ଫେରନ୍ତୁ;
    }
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    cmd_end = handle_import(eap->arg, ଖାଲି, current_sctx.sc_sid,
							       &evalarg, ଖାଲି);
    ଯଦି (cmd_end != ଖାଲି)
	set_nextcmd(eap, cmd_end);
    clear_evalarg(&evalarg, eap);
}

/*
 * When a script is a symlink it may be imported with one name and sourced
 * under another name.  Adjust the import script ID if needed.
 * "*sid" must be a valid script ID.
 */
    ଶୂନ୍ୟ
import_check_sourced_sid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sid)
{
    scriptitem_T *script = SCRIPT_ITEM(*sid);

    ଯଦି (script->sn_sourced_sid > 0)
	*sid = script->sn_sourced_sid;
}

/*
 * Find an exported item in "sid" matching "name".
 * Either "cctx" or "cstack" is NULL.
 * When it is a variable return the index.
 * When it is a user function return "*ufunc".
 * When not found returns -1 and "*ufunc" is NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_exported(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid,
	char_u	    *name,
	ufunc_T	    **ufunc,
	type_T	    **type,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = -1;
    svar_T	*sv;
    scriptitem_T *script = SCRIPT_ITEM(sid);

    *ufunc = ଖାଲି;

    ଯଦି (script->sn_import_autoload && script->sn_state == SN_STATE_NOT_LOADED)
    {
	ଯଦି (do_source(script->sn_name, FALSE, DOSO_NONE, ଖାଲି) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), script->sn_name);
	    ଫେରନ୍ତୁ -1;
	}
    }

    // Find name in "script".
    idx = get_script_item_idx(sid, name, 0, cctx, cstack);
    ଯଦି (idx >= 0)
    {
	sv = ((svar_T *)script->sn_var_vals.ga_data) + idx;
	ଯଦି ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
	{
	    ଯଦି (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    ଫେରନ୍ତୁ -1;
	}
	*type = sv->sv_type;
    }
    ଅନ୍ୟଥା
    {
	size_t	len = STRLEN(name);
	char_u	buffer[200];
	char_u	*funcname;

	// It could be a user function.  Normally this is stored as
	// "<SNR>99_name".  For an autoload script a function is stored with
	// the autoload prefix: "dir#script#name".
	ଯଦି (script->sn_autoload_prefix != ଖାଲି)
	    len += STRLEN(script->sn_autoload_prefix) + 2;
	ଅନ୍ୟଥା
	    len += 15;

	ଯଦି (len < ଆକାର(buffer))
	    funcname = buffer;
	ଅନ୍ୟଥା
	{
	    funcname = alloc(len);
	    ଯଦି (funcname == ଖାଲି)
		ଫେରନ୍ତୁ -1;
	}
	ଯଦି (script->sn_autoload_prefix != ଖାଲି)
	{
	    sprintf((ଅକ୍ଷର *)funcname, "%s%s", script->sn_autoload_prefix, name);
	}
	ଅନ୍ୟଥା
	{
	    funcname[0] = K_SPECIAL;
	    funcname[1] = KS_EXTRA;
	    funcname[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
	    sprintf((ଅକ୍ଷର *)funcname + 3, "%ld_%s", (long)sid, name);
	}
	*ufunc = find_func(funcname, FALSE);

	ଯଦି (*ufunc == ଖାଲି)
	{
	    ଯଦି (verbose)
	    {
		ufunc_T *alt_ufunc = ଖାଲି;

		ଯଦି (script->sn_autoload_prefix != ଖାଲି)
		{
		    // try find the function by the script-local name
		    funcname[0] = K_SPECIAL;
		    funcname[1] = KS_EXTRA;
		    funcname[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR;
		    sprintf((ଅକ୍ଷର *)funcname + 3, "%ld_%s", (long)sid, name);
		    alt_ufunc = find_func(funcname, FALSE);
		}
		ଯଦି (alt_ufunc != ଖାଲି)
		    semsg(_(e_item_not_exported_in_script_str), name);
		ଅନ୍ୟଥା
		    semsg(_(e_item_not_found_in_script_str), name);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (((*ufunc)->uf_flags & FC_EXPORT) == 0)
	{
	    ଯଦି (verbose)
		semsg(_(e_item_not_exported_in_script_str), name);
	    *ufunc = ଖାଲି;
	}
	ଯଦି (funcname != buffer)
	    vim_free(funcname);
    }

    ଫେରନ୍ତୁ idx;
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

    ଯଦି (eap->cmdidx == CMD_final || eap->cmdidx == CMD_const)
    {
	ଯଦି (eap->cmdidx == CMD_final)
	    emsg(_(e_final_requires_a_ମୂଲ୍ୟ));
	ଅନ୍ୟଥା
	    emsg(_(e_const_requires_a_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ arg + STRLEN(arg);
    }

    // Check for valid starting character.
    ଯଦି (!eval_isnamec1(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ଫେରନ୍ତୁ arg + STRLEN(arg);
    }

    ପାଇଁ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	ଯଦି (*p == ':' && (VIM_ISWHITE(p[1]) || p != arg + 1))
	    ଭାଙ୍ଗ;

    ଯଦି (*p != ':')
    {
	emsg(_(e_type_or_initialization_required));
	ଫେରନ୍ତୁ arg + STRLEN(arg);
    }
    ଯଦି (!VIM_ISWHITE(p[1]))
    {
	semsg(_(e_white_space_required_after_str_str), ":", p);
	ଫେରନ୍ତୁ arg + STRLEN(arg);
    }
    name = vim_strnsave(arg, p - arg);

    // parse type, check for reserved name
    p = skipwhite(p + 1);
    type = parse_type(&p, &si->sn_type_list, ଖାଲି, ଖାଲି, TRUE);
    ଯଦି (type == ଖାଲି
	    || check_reserved_name(name, FALSE) == FAIL
	    || !valid_declaration_type(type))
    {
	vim_free(name);
	ଫେରନ୍ତୁ p;
    }

    // Create the variable with 0/NULL value.
    CLEAR_FIELD(init_tv);
    ଯଦି (type->tt_type == VAR_ANY)
	// A variable of type "any" is not possible, just use zero instead
	init_tv.v_type = VAR_ସଂଖ୍ୟା;
    ଅନ୍ୟଥା
	init_tv.v_type = type->tt_type;
    set_var_const(name, 0, type, &init_tv, FALSE, ASSIGN_INIT, 0);

    vim_free(name);
    ଫେରନ୍ତୁ p;
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
    ଶୂନ୍ୟ
update_vim9_script_var(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    create,
	dictitem_T  *di,
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags,
	typval_T    *tv,
	type_T	    **type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    do_member)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    svar_T	    *sv = ଖାଲି;

    ଯଦି (create)
    {
	sallvar_T   *newsav;
	sallvar_T   *sav = ଖାଲି;

	// Store a pointer to the typval_T, so that it can be found by index
	// instead of using a hastab lookup.
	ଯଦି (ga_grow(&si->sn_var_vals, 1) == FAIL)
	    ଫେରନ୍ତୁ;

	hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    // Variable with this name exists, either in this block or in
	    // another block.
	    ପାଇଁ (sav = HI2SAV(hi); ; sav = sav->sav_next)
	    {
		ଯଦି (sav->sav_block_id == si->sn_current_block_id)
		{
		    // variable defined in a loop, re-use the entry
		    sv = ((svar_T *)si->sn_var_vals.ga_data)
						       + sav->sav_var_vals_idx;
		    // unhide the variable
		    ଯଦି (sv->sv_tv == &sav->sav_tv)
		    {
			clear_tv(&sav->sav_tv);
			sv->sv_tv = &di->di_tv;
			sav->sav_di = di;
		    }
		    ଭାଙ୍ଗ;
		}
		ଯଦି (sav->sav_next == ଖାଲି)
		    ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (sv == ଖାଲି)
	{
	    // Variable not defined or not defined in current block: Add a
	    // svar_T and create a new sallvar_T.
	    sv = ((svar_T *)si->sn_var_vals.ga_data) + si->sn_var_vals.ga_len;
	    newsav = (sallvar_T *)alloc_clear(
			      offsetof(sallvar_T, sav_ଚାବି) + STRLEN(name) + 1);
	    ଯଦି (newsav == ଖାଲି)
		ଫେରନ୍ତୁ;

	    sv->sv_tv = &di->di_tv;
	    sv->sv_const = (flags & ASSIGN_FINAL) ? ASSIGN_FINAL
				   : (flags & ASSIGN_CONST) ? ASSIGN_CONST : 0;
	    sv->sv_flags = is_export ? SVFLAG_EXPORTED : 0;
	    ଯଦି ((flags & ASSIGN_INIT) == 0)
		sv->sv_flags |= SVFLAG_ASSIGNED;
	    newsav->sav_var_vals_idx = si->sn_var_vals.ga_len;
	    ++si->sn_var_vals.ga_len;
	    // a pointer to the first char avoids a FORTIFY_SOURCE problem
	    STRCPY(&newsav->sav_ଚାବି[0], name);
	    sv->sv_name = newsav->sav_ଚାବି;
	    newsav->sav_di = di;
	    newsav->sav_block_id = si->sn_current_block_id;

	    ଯଦି (HASHITEM_EMPTY(hi))
		// new variable name
		hash_add(&si->sn_all_vars.dv_hashtab, newsav->sav_ଚାବି,
							       "add variable");
	    ଅନ୍ୟଥା ଯଦି (sav != ଖାଲି)
		// existing name in a new block, append to the list
		sav->sav_next = newsav;
	}
    }
    ଅନ୍ୟଥା
    {
	sv = find_typval_in_script(&di->di_tv, 0, TRUE);
    }
    ଯଦି (sv != ଖାଲି)
    {
	ଯଦି (*type == ଖାଲି)
	    *type = typval2type(tv, get_copyID(), &si->sn_type_list,
					       do_member ? TVTT_DO_MEMBER : 0);
	ଅନ୍ୟଥା ଯଦି ((flags & ASSIGN_INIT) == 0
		&& (*type)->tt_type == VAR_BLOB && tv->v_type == VAR_BLOB
						    && tv->vval.v_blob == ଖାଲି)
	{
	    // "var b: blob = null_blob" has a different type.
	    *type = &t_blob_null;
	}
	ଯଦି (sv->sv_flags & SVFLAG_TYPE_ALLOCATED)
	    free_type(sv->sv_type);
	ଯଦି (*type != ଖାଲି && ((*type)->tt_type == VAR_FUNC
					   || (*type)->tt_type == VAR_PARTIAL))
	{
	    // The type probably uses uf_type_list, which is cleared when the
	    // function is freed, but the script variable may keep the type.
	    // Make a copy to avoid using freed memory.
	    sv->sv_type = alloc_type(*type);
	    sv->sv_flags |= SVFLAG_TYPE_ALLOCATED;
	}
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
hide_script_var(scriptitem_T *si, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_defined)
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

    ଯଦି (HASHITEM_EMPTY(script_hi) || HASHITEM_EMPTY(all_hi))
	ଫେରନ୍ତୁ;

    dictitem_T	*di = HI2DI(script_hi);
    sallvar_T	*sav = HI2SAV(all_hi);
    sallvar_T	*sav_prev = ଖାଲି;

    // There can be multiple entries with the same name in different
    // blocks, find the right one.
    ଯେତେବେଳେ (sav != ଖାଲି && sav->sav_var_vals_idx != idx)
    {
	sav_prev = sav;
	sav = sav->sav_next;
    }
    ଯଦି (sav == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (func_defined)
    {
	// move the typval from the dictitem to the sallvar
	sav->sav_tv = di->di_tv;
	di->di_tv.v_type = VAR_UNKNOWN;
	sav->sav_flags = di->di_flags;
	sav->sav_di = ଖାଲି;
	sv->sv_tv = &sav->sav_tv;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (sav_prev == ଖାଲି)
	    hash_remove(all_ht, all_hi, "hide variable");
	ଅନ୍ୟଥା
	    sav_prev->sav_next = sav->sav_next;
	sv->sv_name = ଖାଲି;
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
find_typval_in_script(typval_T *dest, scid_T sid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା must_find)
{
    scriptitem_T    *si = SCRIPT_ITEM(sid == 0 ? current_sctx.sc_sid : sid);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;

    ଯଦି (si->sn_version != SCRIPT_VERSION_VIM9)
	// legacy script doesn't store variable types
	ଫେରନ୍ତୁ ଖାଲି;

    // Find the svar_T in sn_var_vals.  Start at the end, in a for loop the
    // variable was added at the end.
    ପାଇଁ (idx = si->sn_var_vals.ga_len - 1; idx >= 0; --idx)
    {
	svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	// If "sv_name" is NULL the variable was hidden when leaving a block,
	// don't check "sv_tv" then, it might be used for another variable now.
	ଯଦି (sv->sv_name != ଖାଲି && sv->sv_tv == dest)
	    ଫେରନ୍ତୁ sv;
    }
    ଯଦି (must_find)
	iemsg("find_typval_in_script(): not found");
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Check if the type of script variable "dest" allows assigning "value".
 * If needed convert "value" to a bool.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_script_var_type(
	svar_T	    *sv,
	typval_T    *ମୂଲ୍ୟ,
	char_u	    *name,
	where_T	    where)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret;

    ଯଦି (sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    ret = check_typval_type(sv->sv_type, ମୂଲ୍ୟ, where);
    ଯଦି (ret == OK && need_convert_to_bool(sv->sv_type, ମୂଲ୍ୟ))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	val = tv2bool(ମୂଲ୍ୟ);

	clear_tv(ମୂଲ୍ୟ);
	ମୂଲ୍ୟ->v_type = VAR_BOOL;
	ମୂଲ୍ୟ->v_lock = 0;
	ମୂଲ୍ୟ->vval.v_ସଂଖ୍ୟା = val ? VVAL_TRUE : VVAL_FALSE;
    }
    ଫେରନ୍ତୁ ret;
}

// words that cannot be used as a variable
// Keep this array sorted, as bsearch() is used to search this array.
ସ୍ଥିର ଅକ୍ଷର *reserved[] = {
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
comp_names(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ଫେରନ୍ତୁ STRCMP(*(ଅକ୍ଷର **)s1, *(ଅକ୍ଷର **)s2);
}

/*
 * Returns OK if "name" is not a reserved keyword.  Otherwise returns FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_reserved_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_objm_access)
{
    // "this" can be used in an object method
    ଯଦି (is_objm_access && STRCMP("this", name) == 0)
	ଫେରନ୍ତୁ OK;

    ଯଦି (bsearch(&name, reserved, ARRAY_LENGTH(reserved),
				ଆକାର(reserved[0]), comp_names) != ଖାଲି)
    {
	semsg(_(e_cannot_use_reserved_name_str), name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

#endif // FEAT_EVAL
