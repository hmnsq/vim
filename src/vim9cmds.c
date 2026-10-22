/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9cmds.c: Dealing with commands of a compiled function
 */

#define USING_FLOAT_STUFF
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    പൂർണ്ണസംഖ്യ		idx = instr->ga_len;

    ആകുമ്പോൾ (idx > 0)
    {
	എങ്കിൽ (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    തുടരുക;
	}
#ifdef FEAT_PROFILE
	എങ്കിൽ (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    തുടരുക;
	}
#endif
	എങ്കിൽ (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    തുടരുക;
	}
	നിർത്തുക;
    }
    തിരികെ idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    സ്ഥിതം ശൂന്യം
unwind_locals(cctx_T *cctx, പൂർണ്ണസംഖ്യ new_മുകൾ, പൂർണ്ണസംഖ്യ keep)
{
    എങ്കിൽ (cctx->ctx_locals.ga_len > new_മുകൾ)
	വേണ്ടി (പൂർണ്ണസംഖ്യ idx = new_മുകൾ; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    എങ്കിൽ (!keep)
	cctx->ctx_locals.ga_len = new_മുകൾ;
}

/*
 * Free all local variables.
 */
    ശൂന്യം
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    പൂർണ്ണസംഖ്യ
check_vim9_unlet(char_u *name)
{
    എങ്കിൽ (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	തിരികെ FAIL;
    }

    എങ്കിൽ (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ശൂന്യ)
    {
	// "unlet s:var" is allowed in legacy script.
	എങ്കിൽ (*name == 's' && !in_vim9script())
	    തിരികെ OK;
	semsg(_(e_cannot_unlet_str), name);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    പൂർണ്ണസംഖ്യ	    deep UNUSED,
    ശൂന്യം    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    പൂർണ്ണസംഖ്യ		cc = *name_end;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ OK;

    *name_end = NUL;
    എങ്കിൽ (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(p, '.') != ശൂന്യ || vim_strchr(p, '[') != ശൂന്യ)
    {
	lhs_T	    lhs;

	// This is similar to assigning: lookup the list/dict, compile the
	// idx/key.  Then instead of storing the value unlet the item.
	// unlet {list}[idx]
	// unlet {dict}[key]  dict.key
	//
	// Figure out the LHS type and other properties.
	//
	ret = compile_lhs(p, &lhs, CMD_unlet, FALSE, FALSE, 0, cctx);

	// Use the info in "lhs" to unlet the item at the index in the
	// list or dict.
	എങ്കിൽ (ret == OK)
	{
	    എങ്കിൽ (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    അല്ലെങ്കിൽ
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    തിരികെ ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    പൂർണ്ണസംഖ്യ	    deep,
    ശൂന്യം    *coookie)
{
    cctx_T	*cctx = coookie;
    പൂർണ്ണസംഖ്യ		cc = *name_end;
    char_u	*p = lvp->ll_name;
    പൂർണ്ണസംഖ്യ		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    അക്ഷരം	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    പൂർണ്ണസംഖ്യ		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ OK;

    എങ്കിൽ (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	തിരികെ FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    എങ്കിൽ (p[1] != ':')
    {
	char_u *end = find_name_end(p, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);

	// The most important point is that something like
	// name[idx].member... needs to be resolved at runtime, get_lval(),
	// starting from the root "name".

	// These checks are reminiscent of the variable_exists function.
	// But most of the matches require special handling.

	// If bare name is locally accessible, except for local var,
	// then put it on the stack to use with ISN_LOCKUNLOCK.
	// This could be v.memb, v[idx_key]; bare class variable,
	// function arg. The item on the stack, will be passed
	// to ex_lockvar() indirectly and be used as the root for get_lval.
	// A bare script variable name needs no special handling.

	char_u	*name = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	len = end - p;

	എങ്കിൽ (lookup_local(p, len, ശൂന്യ, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    എങ്കിൽ (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		തിരികെ FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(ശൂന്യ, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	എങ്കിൽ (name == ശൂന്യ)
	{
	    class_T *cl;
	    എങ്കിൽ (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		എങ്കിൽ (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (പൂർണ്ണസംഖ്യ)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(ശൂന്യ, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	എങ്കിൽ (name == ശൂന്യ)
	{
	    // Can lockvar any function arg.
	    എങ്കിൽ (arg_exists(p, len, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	എങ്കിൽ (name == ശൂന്യ)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    എങ്കിൽ (script_var_exists(p, len, cctx, ശൂന്യ) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	എങ്കിൽ (name != ശൂന്യ)
	{
#ifdef LOG_LOCKVAR
	    ch_log(ശൂന്യ, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    എങ്കിൽ (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    എങ്കിൽ (buf == ശൂന്യ)
	ret = FAIL;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (deep < 0)
	    vim_snprintf((അക്ഷരം *)buf, len, "%s! %s", cmd, p);
	അല്ലെങ്കിൽ
	    vim_snprintf((അക്ഷരം *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(ശൂന്യ, "LKVAR:    ... buf %s", buf);
#endif
	എങ്കിൽ (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	അല്ലെങ്കിൽ
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    തിരികെ ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ	    deep = 0;
    char_u  *p = arg;

    എങ്കിൽ (eap->cmdidx != CMD_unlet)
    {
	എങ്കിൽ (eap->forceit)
	    deep = -1;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	അല്ലെങ്കിൽ
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    തിരികെ eap->nextcmd == ശൂന്യ ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	പൂർണ്ണസംഖ്യ	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    എങ്കിൽ (endlabel == ശൂന്യ)
	തിരികെ FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    എങ്കിൽ (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    അല്ലെങ്കിൽ
	generate_JUMP(cctx, when, 0);
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
compile_fill_jump_to_end(endlabel_T **el, പൂർണ്ണസംഖ്യ jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ആകുമ്പോൾ (*el != ശൂന്യ)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    സ്ഥിതം ശൂന്യം
compile_free_jump_to_end(endlabel_T **el)
{
    ആകുമ്പോൾ (*el != ശൂന്യ)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    സ്ഥിതം scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    എങ്കിൽ (scope == ശൂന്യ)
	തിരികെ ശൂന്യ;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_എണ്ണം = cctx->ctx_locals.ga_len;
    എങ്കിൽ (scope->se_outer != ശൂന്യ)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    തിരികെ scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    ശൂന്യം
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    എങ്കിൽ (scope == ശൂന്യ)
    {
	iemsg("calling drop_scope() without a scope");
	തിരികെ;
    }
    cctx->ctx_scope = scope->se_outer;
    തിരഞ്ഞെടുപ്പ് (scope->se_type)
    {
	അവസ്ഥ IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); നിർത്തുക;
	അവസ്ഥ FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); നിർത്തുക;
	അവസ്ഥ WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); നിർത്തുക;
	അവസ്ഥ TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); നിർത്തുക;
	അവസ്ഥ NO_SCOPE:
	അവസ്ഥ BLOCK_SCOPE:
	    നിർത്തുക;
    }
    vim_free(scope);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    എങ്കിൽ (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * compile "if expr"
 *
 * "if expr" Produces instructions:
 *	EVAL expr		Push result of "expr"
 *	JUMP_IF_FALSE end
 *	... body ...
 * end:
 *
 * "if expr | else" Produces instructions:
 *	EVAL expr		Push result of "expr"
 *	JUMP_IF_FALSE else
 *	... body ...
 *	JUMP_ALWAYS end
 * else:
 *	... body ...
 * end:
 *
 * "if expr1 | elseif expr2 | else" Produces instructions:
 *	EVAL expr		Push result of "expr"
 *	JUMP_IF_FALSE elseif
 *	... body ...
 *	JUMP_ALWAYS end
 * elseif:
 *	EVAL expr		Push result of "expr"
 *	JUMP_IF_FALSE else
 *	... body ...
 *	JUMP_ALWAYS end
 * else:
 *	... body ...
 * end:
 */
    char_u *
compile_if(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    പൂർണ്ണസംഖ്യ		instr_എണ്ണം = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    എങ്കിൽ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    അല്ലെങ്കിൽ എങ്കിൽ (instr->ga_len == instr_എണ്ണം && ppconst.pp_used == 1)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;
	പൂർണ്ണസംഖ്യ v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	എങ്കിൽ (error)
	    തിരികെ ശൂന്യ;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    അല്ലെങ്കിൽ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	എങ്കിൽ (generate_ppconst(cctx, &ppconst) == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (bool_on_അട്ടി(cctx) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    എങ്കിൽ (scope == ശൂന്യ)
	തിരികെ ശൂന്യ;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    എങ്കിൽ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    അല്ലെങ്കിൽ
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    തിരികെ p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    പൂർണ്ണസംഖ്യ		instr_എണ്ണം;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	തിരികെ ശൂന്യ;
    }
    unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);
    എങ്കിൽ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    എങ്കിൽ (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    എങ്കിൽ (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_വ്യഞ്ജകം_cctx(&p, cctx);
	തിരികെ p;
    }

    എങ്കിൽ (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_വ്യഞ്ജകം_cctx(&p, cctx);
	തിരികെ p;
    }

    എങ്കിൽ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ	    moved_cmdmod = FALSE;
	പൂർണ്ണസംഖ്യ	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	എങ്കിൽ (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    എങ്കിൽ (GA_GROW_FAILS(instr, 1))
		തിരികെ ശൂന്യ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	എങ്കിൽ (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    തിരികെ ശൂന്യ;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	എങ്കിൽ (moved_cmdmod)
	    ++instr->ga_len;

	എങ്കിൽ (saved_debug)
	{
	    // move the debug instruction here
	    എങ്കിൽ (GA_GROW_FAILS(instr, 1))
		തിരികെ ശൂന്യ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_എണ്ണം = instr->ga_len;
    എങ്കിൽ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	തിരികെ ശൂന്യ;
    }
    cctx->ctx_skip = save_skip;
    എങ്കിൽ (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    അല്ലെങ്കിൽ എങ്കിൽ (instr->ga_len == instr_എണ്ണം && ppconst.pp_used == 1)
    {
	പൂർണ്ണസംഖ്യ error = FALSE;
	പൂർണ്ണസംഖ്യ v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	എങ്കിൽ (error)
	{
	    clear_ppconst(&ppconst);
	    തിരികെ ശൂന്യ;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    അല്ലെങ്കിൽ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	എങ്കിൽ (generate_ppconst(cctx, &ppconst) == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (bool_on_അട്ടി(cctx) == FAIL)
	    തിരികെ ശൂന്യ;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    തിരികെ p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	തിരികെ ശൂന്യ;
    }
    unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);
    എങ്കിൽ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
    {
	എങ്കിൽ (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	എങ്കിൽ (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_END);
	    generate_instr(cctx, ISN_PROF_START);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#endif

    എങ്കിൽ (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	എങ്കിൽ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    എങ്കിൽ (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    എങ്കിൽ (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	എങ്കിൽ (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    തിരികെ p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	തിരികെ ശൂന്യ;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);
    എങ്കിൽ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    എങ്കിൽ (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_to_end(&ifscope->is_end_label, instr->ga_len, cctx);

#ifdef FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
					    && scope->se_skip_save != SKIP_YES)
    {
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_START);
    }
#endif
    cctx->ctx_skip = scope->se_skip_save;

    // If all the blocks end in :return and there is an :else then the
    // had_return flag is set.
    cctx->ctx_had_return = ifscope->is_had_return && ifscope->is_seen_else;

    drop_scope(cctx);
    തിരികെ arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    സ്ഥിതം ശൂന്യം
compile_fill_loop_info(loop_info_T *loop_info, പൂർണ്ണസംഖ്യ funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_എണ്ണം = cctx->ctx_locals.ga_len;
    loop_info->li_closure_എണ്ണം = cctx->ctx_closure_എണ്ണം;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    സ്ഥിതം type_T *
compile_for_tuple_get_vartype(type_T *vartype, പൂർണ്ണസംഖ്യ var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    എങ്കിൽ (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	തിരികെ &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    എങ്കിൽ (member_type->tt_type == VAR_ANY)
	തിരികെ &t_any;

    എങ്കിൽ (!var_list)
	// for x in tuple<...list<xxx>>
	തിരികെ member_type;

    എങ്കിൽ (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	തിരികെ member_type->tt_member;
    അല്ലെങ്കിൽ എങ്കിൽ (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	തിരികെ member_type->tt_args[0]->tt_member;

    തിരികെ &t_any;
}

/*
 * Compile "for var in expr":
 *
 * Produces instructions:
 *       STORE -1 in loop-idx	Set index to -1
 *       EVAL expr		Result of "expr" on top of stack
 * top:  FOR loop-idx, end	Increment index, use list on bottom of stack
 *				- if beyond end, jump to "end"
 *				- otherwise get item from list and push it
 *				- store ec_funcrefs in var "loop-idx" + 1
 *       STORE var		Store item in "var"
 *       ... body ...
 *       ENDLOOP funcref-idx off count	Only if closure uses local var
 *       JUMP top			Jump back to repeat
 * end:	 DROP				Drop the result of "expr"
 *
 * Compile "for [var1, var2] in expr" - as above, but instead of "STORE var":
 *	 UNPACK 2		Split item in 2
 *       STORE var1		Store item in "var1"
 *       STORE var2		Store item in "var2"
 */
    char_u *
compile_for(char_u *arg_start, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_end;
    char_u	*name = ശൂന്യ;
    char_u	*p;
    char_u	*wp;
    പൂർണ്ണസംഖ്യ		var_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		var_list = FALSE;
    പൂർണ്ണസംഖ്യ		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    പൂർണ്ണസംഖ്യ		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    പൂർണ്ണസംഖ്യ		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_എണ്ണം, &semicolon, FALSE);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (var_എണ്ണം == 0)
	var_എണ്ണം = 1;
    അല്ലെങ്കിൽ
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    എങ്കിൽ (may_get_next_വരി_error(wp, &p, cctx) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	എങ്കിൽ (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	അല്ലെങ്കിൽ
	    emsg(_(e_missing_in_after_for));
	തിരികെ ശൂന്യ;
    }
    wp = p + 2;
    എങ്കിൽ (may_get_next_വരി_error(wp, &p, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    എങ്കിൽ (scope == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	തിരികെ ശൂന്യ;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_സംഖ്യ);
    എങ്കിൽ (loop_lvar == ശൂന്യ)
    {
	drop_scope(cctx);
	തിരികെ ശൂന്യ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_സംഖ്യ);
    എങ്കിൽ (funcref_lvar == ശൂന്യ)
    {
	drop_scope(cctx);
	തിരികെ ശൂന്യ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    എങ്കിൽ (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	തിരികെ ശൂന്യ;
    }
    arg_end = arg;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_അട്ടി(cctx, 0);
	എങ്കിൽ (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	അല്ലെങ്കിൽ എങ്കിൽ (vartype->tt_type == VAR_BLOB)
	    item_type = &t_സംഖ്യ;
	അല്ലെങ്കിൽ എങ്കിൽ (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    എങ്കിൽ (!var_list)
		item_type = vartype->tt_member;
	    അല്ലെങ്കിൽ എങ്കിൽ (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_മുകൾ_label = current_instr_idx(cctx);

	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    പൂർണ്ണസംഖ്യ		save_prev_lnum = cctx->ctx_prev_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_prev_lnum = prev_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_names_len;
	    cctx->ctx_prev_lnum = save_prev_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_start;
	എങ്കിൽ (var_list)
	{
	    generate_UNPACK(cctx, var_എണ്ണം, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_അട്ടി.ga_len;

	    // add type of the items
	    വേണ്ടി (idx = 0; idx < var_എണ്ണം; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		എങ്കിൽ (തള്ളുക_type_അട്ടി(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    തിരികെ ശൂന്യ;
		}
	    }
	}

	വേണ്ടി (idx = 0; idx < var_എണ്ണം; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    പൂർണ്ണസംഖ്യ			opt_flags = 0;
	    പൂർണ്ണസംഖ്യ			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    എങ്കിൽ (name == ശൂന്യ)
		പോകുക failed;
	    എങ്കിൽ (*skipwhite(p) == ':')
	    {
		എങ്കിൽ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    പോകുക failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		എങ്കിൽ (lhs_type == ശൂന്യ || !valid_declaration_type(lhs_type))
		    പോകുക failed;
	    }

	    എങ്കിൽ (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		പോകുക failed;
	    എങ്കിൽ (dest != dest_local)
	    {
		എങ്കിൽ (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ശൂന്യ) == FAIL)
		    പോകുക failed;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		എങ്കിൽ (generate_instr_drop(cctx, ISN_DROP, 1) == ശൂന്യ)
		    പോകുക failed;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Script var is not supported.
		എങ്കിൽ (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    പോകുക failed;
		}

		എങ്കിൽ (!valid_varname(arg, (പൂർണ്ണസംഖ്യ)varlen, FALSE))
		    പോകുക failed;
		എങ്കിൽ (lookup_local(arg, varlen, ശൂന്യ, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    പോകുക failed;
		}

		// Reserve a variable to store "var".
		എങ്കിൽ (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		എങ്കിൽ (lhs_type == &t_any)
		    lhs_type = item_type;
		അല്ലെങ്കിൽ എങ്കിൽ (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    പോകുക failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		എങ്കിൽ (var_lvar == ശൂന്യ)
		    // out of memory or used as an argument
		    പോകുക failed;

		എങ്കിൽ (semicolon && idx == var_എണ്ണം - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ശൂന്യ);
	    }

	    എങ്കിൽ (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    തിരികെ arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    തിരികെ ശൂന്യ;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    എങ്കിൽ (cctx->ctx_locals.ga_len > loop_info->li_local_എണ്ണം
	    && cctx->ctx_closure_എണ്ണം > loop_info->li_closure_എണ്ണം)
	തിരികെ generate_ENDLOOP(cctx, loop_info);
    തിരികെ OK;
}

/*
 * compile "endfor"
 */
    char_u *
compile_endfor(char_u *arg, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope = cctx->ctx_scope;
    forscope_T	*forscope;
    isn_T	*isn;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	തിരികെ ശൂന്യ;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	എങ്കിൽ (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    തിരികെ ശൂന്യ;

	unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_മുകൾ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_മുകൾ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	എങ്കിൽ (generate_instr_drop(cctx, ISN_DROP, 1) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }

    vim_free(scope);

    തിരികെ arg;
}

/*
 * compile "while expr"
 *
 * Produces instructions:
 * top:  EVAL expr			Push result of "expr"
 *	 WHILE funcref-idx  end		Jump if false
 *	 ... body ...
 *       ENDLOOP funcref-idx off count	only if closure uses local var
 *	 JUMP top			Jump back to repeat
 * end:
 *
 */
    char_u *
compile_while(char_u *arg, cctx_T *cctx)
{
    char_u	    *p = arg;
    scope_T	    *scope;
    whilescope_T    *whilescope;
    lvar_T	    *funcref_lvar;
    പൂർണ്ണസംഖ്യ		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    എങ്കിൽ (scope == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	തിരികെ ശൂന്യ;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_മുകൾ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_സംഖ്യ);
    എങ്കിൽ (funcref_lvar == ശൂന്യ)
    {
	drop_scope(cctx);
	തിരികെ ശൂന്യ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ (bool_on_അട്ടി(cctx) == FAIL)
	    തിരികെ FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	എങ്കിൽ (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;
    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	തിരികെ ശൂന്യ;
    }
    cctx->ctx_scope = scope->se_outer;
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	എങ്കിൽ (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    തിരികെ ശൂന്യ;

	unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_മുകൾ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    തിരികെ arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    പൂർണ്ണസംഖ്യ
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    പൂർണ്ണസംഖ്യ		prev_local_എണ്ണം = 0;

    CLEAR_POINTER(lvi);
    വേണ്ടി (;;)
    {
	loop_info_T	*loopinfo;
	പൂർണ്ണസംഖ്യ		cur_local_last;
	പൂർണ്ണസംഖ്യ		start_local_എണ്ണം;

	ആകുമ്പോൾ (scope != ശൂന്യ && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	എങ്കിൽ (scope == ശൂന്യ)
	    നിർത്തുക;

	എങ്കിൽ (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_എണ്ണം - 1;
	}
	അല്ലെങ്കിൽ
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_എണ്ണം - 3;
	}

	start_local_എണ്ണം = loopinfo->li_local_എണ്ണം;
	എങ്കിൽ (cctx->ctx_locals.ga_len > start_local_എണ്ണം)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_എണ്ണം;
	    lvi->lvi_loop[loopinfo->li_depth].var_എണ്ണം =
			  (short)(cctx->ctx_locals.ga_len - start_local_എണ്ണം
							   - prev_local_എണ്ണം);
	    എങ്കിൽ (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_എണ്ണം = cctx->ctx_locals.ga_len - cur_local_last;
    }
    തിരികെ lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    ശൂന്യം
get_loop_var_idx(cctx_T *cctx, പൂർണ്ണസംഖ്യ idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    എങ്കിൽ (get_loop_var_info(cctx, &lvi))
    {
	പൂർണ്ണസംഖ്യ depth;

	വേണ്ടി (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    എങ്കിൽ (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_എണ്ണം)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		തിരികെ;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_find_scope(
	പൂർണ്ണസംഖ്യ	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	പൂർണ്ണസംഖ്യ	    *try_scopes,    // :try scopes encountered or NULL
	അക്ഷരം	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (scope == ശൂന്യ)
	{
	    എങ്കിൽ (error != ശൂന്യ)
		emsg(_(error));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (scope->se_type == FOR_SCOPE)
	{
	    എങ്കിൽ (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (loop_label != ശൂന്യ)
		*loop_label = scope->se_u.se_for.fs_മുകൾ_label;
	    എങ്കിൽ (el != ശൂന്യ)
		*el = &scope->se_u.se_for.fs_end_label;
	    നിർത്തുക;
	}
	എങ്കിൽ (scope->se_type == WHILE_SCOPE)
	{
	    എങ്കിൽ (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (loop_label != ശൂന്യ)
		*loop_label = scope->se_u.se_while.ws_മുകൾ_label;
	    എങ്കിൽ (el != ശൂന്യ)
		*el = &scope->se_u.se_while.ws_end_label;
	    നിർത്തുക;
	}
	എങ്കിൽ (try_scopes != ശൂന്യ && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    തിരികെ OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ		try_scopes = 0;
    പൂർണ്ണസംഖ്യ		loop_label;

    എങ്കിൽ (compile_find_scope(&loop_label, ശൂന്യ, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    അല്ലെങ്കിൽ
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    തിരികെ arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ		try_scopes = 0;
    endlabel_T	**el;

    എങ്കിൽ (compile_find_scope(ശൂന്യ, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ arg;

    എങ്കിൽ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    എങ്കിൽ (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    തിരികെ arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    എങ്കിൽ (new_scope(cctx, BLOCK_SCOPE) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    ശൂന്യം
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_എണ്ണം, TRUE);
    vim_free(scope);
}

/*
 * Compile "try".
 * Creates a new scope for the try-endtry, pointing to the first catch and
 * finally.
 * Creates another scope for the "try" block itself.
 * TRY instruction sets up exception handling at runtime.
 *
 *	"try"
 *	    TRY -> catch1, -> finally  push trystack entry
 *	    ... try block
 *	"throw {exception}"
 *	    EVAL {exception}
 *	    THROW		create exception
 *	    ... try block
 *	" catch {expr}"
 *	    JUMP -> finally
 * catch1:  PUSH exception
 *	    EVAL {expr}
 *	    MATCH
 *	    JUMP nomatch -> catch2
 *	    CATCH   remove exception
 *	    ... catch block
 *	" catch"
 *	    JUMP -> finally
 * catch2:  CATCH   remove exception
 *	    ... catch block
 *	" finally"
 * finally:
 *	    ... finally block
 *	" endtry"
 *	    ENDTRY  pop trystack entry, may rethrow
 */
    char_u *
compile_try(char_u *arg, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*try_scope;
    scope_T	*scope;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    എങ്കിൽ (try_scope == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	എങ്കിൽ ((isn = generate_instr(cctx, ISN_TRY)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	എങ്കിൽ (isn->isn_arg.tryref.try_ref == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    എങ്കിൽ (scope == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരികെ arg;
}

/*
 * Compile "catch {expr}".
 */
    char_u *
compile_catch(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    char_u	*p;
    isn_T	*isn;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    // end block scope from :try or :catch
    എങ്കിൽ (scope != ശൂന്യ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	എങ്കിൽ (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    തിരികെ ശൂന്യ;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	എങ്കിൽ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	എങ്കിൽ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    എങ്കിൽ (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    അല്ലെങ്കിൽ
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	dropped = 0;
	പൂർണ്ണസംഖ്യ	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ശൂന്യ);
	എങ്കിൽ (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (tofree == ശൂന്യ)
	    len = (പൂർണ്ണസംഖ്യ)(end - (p + 1));
	അല്ലെങ്കിൽ
	    len = (പൂർണ്ണസംഖ്യ)(end - tofree);
	pat = vim_strnsave(tofree == ശൂന്യ ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	എങ്കിൽ (pat == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (generate_PUSHS(cctx, &pat) == FAIL)
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (generate_COMPARE(cctx, വ്യഞ്ജകം_MATCH, FALSE) == FAIL)
	    തിരികെ ശൂന്യ;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	എങ്കിൽ (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (new_scope(cctx, BLOCK_SCOPE) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    പൂർണ്ണസംഖ്യ		this_instr;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    // end block scope from :try or :catch
    എങ്കിൽ (scope != ശൂന്യ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	തിരികെ ശൂന്യ;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    എങ്കിൽ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	എങ്കിൽ (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    തിരികെ ശൂന്യ;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    എങ്കിൽ (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	എങ്കിൽ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	എങ്കിൽ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	എങ്കിൽ (generate_instr(cctx, ISN_FINALLY) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }

    തിരികെ arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    എങ്കിൽ (misplaced_cmdmod(cctx))
	തിരികെ ശൂന്യ;

    // end block scope from :catch or :finally
    എങ്കിൽ (scope != ശൂന്യ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    എങ്കിൽ (scope == ശൂന്യ || scope->se_type != TRY_SCOPE)
    {
	എങ്കിൽ (scope == ശൂന്യ)
	    emsg(_(e_endtry_without_try));
	അല്ലെങ്കിൽ എങ്കിൽ (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	അല്ലെങ്കിൽ എങ്കിൽ (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	അല്ലെങ്കിൽ
	    emsg(_(e_missing_endif));
	തിരികെ ശൂന്യ;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    തിരികെ ശൂന്യ;
	}

#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						.isn_type == ISN_PROF_START)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#endif

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							  instr->ga_len, cctx);

	എങ്കിൽ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_data)
					   + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
    }

    // The try statement terminates (always returns or throws) if:
    // - a "finally" clause always returns or throws, overriding everything
    //   that came before it, or
    // - every earlier block (the "try" body and each "catch") terminated
    //   and, when there is no "finally" to override it, the last such block
    //   also terminates.  A "catch" pattern not matching an exception still
    //   terminates: the exception then simply propagates instead of falling
    //   through to what follows "endtry".
    cctx->ctx_had_return =
	(scope->se_u.se_try.ts_has_finally
			   && (cctx->ctx_had_return || cctx->ctx_had_throw))
	|| (!scope->se_u.se_try.ts_no_return
			   && (scope->se_u.se_try.ts_has_finally
			       || cctx->ctx_had_return || cctx->ctx_had_throw));

    compile_endblock(cctx);

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	എങ്കിൽ (generate_instr(cctx, ISN_ENDTRY) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    തിരികെ arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ p;
    എങ്കിൽ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (generate_instr_drop(cctx, ISN_THROW, 1) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരികെ p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    പൂർണ്ണസംഖ്യ		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	തിരികെ ശൂന്യ;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    തിരികെ skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    പൂർണ്ണസംഖ്യ
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    എങ്കിൽ (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	എങ്കിൽ (lvar == ശൂന്യ)
	    തിരികെ 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    തിരികെ dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    പൂർണ്ണസംഖ്യ		argcount = 0;
    പൂർണ്ണസംഖ്യ		defer_var_idx;
    type_T	*type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		func_idx = -1;

    എങ്കിൽ (*arg == '(')
    {
	// a lambda function
	എങ്കിൽ (compile_lambda(&arg, cctx, ശൂന്യ) != OK)
	    തിരികെ ശൂന്യ;
	paren = vim_strchr(arg, '(');
	എങ്കിൽ (paren == ശൂന്യ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	എങ്കിൽ (paren == ശൂന്യ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    തിരികെ ശൂന്യ;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	എങ്കിൽ (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	അല്ലെങ്കിൽ എങ്കിൽ (compile_expr0(&arg, cctx) == FAIL)
	    തിരികെ ശൂന്യ;
	*paren = '(';
    }

    // check for function type
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_അട്ടി(cctx, 0);
	എങ്കിൽ (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    തിരികെ ശൂന്യ;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    എങ്കിൽ (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ (func_idx >= 0)
	{
	    type2_T	*argtypes = ശൂന്യ;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    എങ്കിൽ (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		തിരികെ ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    തിരികെ ശൂന്യ;

	defer_var_idx = get_defer_var_idx(cctx);
	എങ്കിൽ (defer_var_idx == 0)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    തിരികെ skipwhite(arg);
}

/*
 * compile "echo expr"
 * compile "echomsg expr"
 * compile "echoerr expr"
 * compile "echoconsole expr"
 * compile "echowindow expr" - may have cmd_count set
 * compile "execute expr"
 */
    char_u *
compile_mult_വ്യഞ്ജകം(
	char_u	*arg,
	പൂർണ്ണസംഖ്യ	cmdidx,
	long	cmd_എണ്ണം UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*വ്യഞ്ജകം_start;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    പൂർണ്ണസംഖ്യ		r = OK;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (ends_excmd2(prev, p))
	    നിർത്തുക;
	വ്യഞ്ജകം_start = p;
	എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_മൂല്യം_str), വ്യഞ്ജകം_start);
		തിരികെ ശൂന്യ;
	    }
	}

	++എണ്ണം;
	prev = p;
	p = skipwhite(p);
    }

    എങ്കിൽ (എണ്ണം > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	എങ്കിൽ (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (cmdidx == CMD_execute)
	    r = generate_MULT_വ്യഞ്ജകം(cctx, ISN_EXECUTE, എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (cmdidx == CMD_echomsg)
	    r = generate_MULT_വ്യഞ്ജകം(cctx, ISN_ECHOMSG, എണ്ണം);
#ifdef HAS_MESSAGE_WINDOW
	അല്ലെങ്കിൽ എങ്കിൽ (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, എണ്ണം, cmd_എണ്ണം);
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (cmdidx == CMD_echoconsole)
	    r = generate_MULT_വ്യഞ്ജകം(cctx, ISN_ECHOCONSOLE, എണ്ണം);
	അല്ലെങ്കിൽ
	    r = generate_MULT_വ്യഞ്ജകം(cctx, ISN_ECHOERR, എണ്ണം);

	cctx->ctx_lnum = save_lnum;
    }
    തിരികെ r == OK ? p : ശൂന്യ;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, ശൂന്യ);
    char_u *p = skipdigits(eap->cmd);

    എങ്കിൽ (p == range_end)
	തിരികെ FAIL;
    തിരികെ generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, പൂർണ്ണസംഖ്യ fixindent)
{
    char_u	*വരി = arg;
    linenr_T	lnum;
    അക്ഷരം	*errormsg;
    പൂർണ്ണസംഖ്യ		above = eap->forceit;

    eap->regname = *വരി;

    എങ്കിൽ (eap->regname == '=')
    {
	char_u *p = skipwhite(വരി + 1);

	എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	    തിരികെ ശൂന്യ;
	വരി = p;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->regname != NUL)
	++വരി;

    എങ്കിൽ (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    അല്ലെങ്കിൽ
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	എങ്കിൽ (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (eap->addr_എണ്ണം == 0)
	    lnum = -1;
	അല്ലെങ്കിൽ
	    lnum = eap->line2;
	എങ്കിൽ (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    തിരികെ വരി;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *വരി_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*വരി = വരി_arg;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		has_വ്യഞ്ജകം = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ശൂന്യ;
    char_u	*cmd_arg = ശൂന്യ;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	പോകുക theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    എങ്കിൽ (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	എങ്കിൽ (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    എങ്കിൽ (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	പൂർണ്ണസംഖ്യ	usefilter = FALSE;

	has_വ്യഞ്ജകം = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	എങ്കിൽ ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	എങ്കിൽ ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
		nextcmd = eap->nextcmd;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    എങ്കിൽ (*p != NUL)
		++p;
	    എങ്കിൽ (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    എങ്കിൽ (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    പൂർണ്ണസംഖ്യ	    flags = 0;  // unused
	    പൂർണ്ണസംഖ്യ	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, വരി, &tofree, &flags);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		വരി = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    എങ്കിൽ (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_വ്യഞ്ജകം = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	എങ്കിൽ (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    എങ്കിൽ ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	പൂർണ്ണസംഖ്യ delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (*p == delim)
	    cmd_arg = p + 1;
    }

    എങ്കിൽ (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    എങ്കിൽ (cmd_arg != ശൂന്യ)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, ശൂന്യ, lookup_scriptitem, ശൂന്യ);
	എങ്കിൽ (nea.cmdidx < CMD_SIZE)
	{
	    has_വ്യഞ്ജകം = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    എങ്കിൽ (has_വ്യഞ്ജകം)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    എങ്കിൽ (has_വ്യഞ്ജകം && (p = (char_u *)strstr((അക്ഷരം *)eap->arg, "`=")) != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	എണ്ണം = 0;
	char_u	*start = skipwhite(വരി);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++എണ്ണം;
	    }
	    p += 2;
	    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
		തിരികെ ശൂന്യ;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++എണ്ണം;
	    p = skipwhite(p);
	    എങ്കിൽ (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		തിരികെ ശൂന്യ;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((അക്ഷരം *)start, "`=");
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		എങ്കിൽ (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++എണ്ണം;
		}
		നിർത്തുക;
	    }
	}
	generate_EXECCONCAT(cctx, എണ്ണം);
    }
    അല്ലെങ്കിൽ
	generate_EXEC_copy(cctx, ISN_EXEC, വരി);

theend:
    എങ്കിൽ (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    തിരികെ nextcmd;
}

/*
 * A script command with heredoc, e.g.
 *	ruby << EOF
 *	   command
 *	EOF
 * Has been turned into one long line with NL characters by
 * get_function_body():
 *	ruby << EOF<NL>   command<NL>EOF
 */
    char_u *
compile_script(char_u *വരി, cctx_T *cctx)
{
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	എങ്കിൽ ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	isn->isn_arg.string = vim_strsave(വരി);
    }
    തിരികെ (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *വ്യഞ്ജകം = (char_u *)strstr((അക്ഷരം *)cmd, "\\=");

    എങ്കിൽ (വ്യഞ്ജകം != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ശൂന്യ, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    പൂർണ്ണസംഖ്യ		വ്യഞ്ജകം_res;
	    പൂർണ്ണസംഖ്യ		trailing_error;
	    പൂർണ്ണസംഖ്യ		instr_എണ്ണം;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = ശൂന്യ;
	    വ്യഞ്ജകം_res = compile_expr0(&cmd, cctx);
	    എങ്കിൽ (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    എങ്കിൽ (വ്യഞ്ജകം_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		എങ്കിൽ (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		തിരികെ ശൂന്യ;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_എണ്ണം = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_എണ്ണം].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    എങ്കിൽ ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ idx;

		വേണ്ടി (idx = 0; idx < instr_എണ്ണം; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		തിരികെ ശൂന്യ;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    എങ്കിൽ (*end == '&')
		++end;
	    ആകുമ്പോൾ (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    തിരികെ end;
	}
    }

    തിരികെ compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *വരി, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    എങ്കിൽ (lhs->lhs_name != ശൂന്യ)
    {
	എങ്കിൽ (STRNCMP(arg, "END", 3) == 0)
	{
	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
	    {
		എങ്കിൽ (lhs->lhs_append)
		{
		    // First load the current variable value.
		    എങ്കിൽ (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			തിരികെ ശൂന്യ;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		എങ്കിൽ (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		എങ്കിൽ (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    എങ്കിൽ (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			തിരികെ ശൂന്യ;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    തിരികെ ശൂന്യ;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    തിരികെ arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (arg[0] == '=' && arg[1] == '>')
    {
	പൂർണ്ണസംഖ്യ	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	എങ്കിൽ (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	എങ്കിൽ (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	അല്ലെങ്കിൽ
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    എങ്കിൽ (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		എങ്കിൽ (lhs->lhs_whole == ശൂന്യ)
		    തിരികെ ശൂന്യ;
	    }
	}

	തിരികെ arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    തിരികെ compile_exec(വരി, eap, cctx);
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *വരി, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ ശൂന്യ;
    isn->isn_arg.സംഖ്യ = eap->cmdidx;

    p = eap->arg;
    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ ശൂന്യ;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    എങ്കിൽ (isn->isn_arg.cexpr.cexpr_ref == ശൂന്യ)
	തിരികെ ശൂന്യ;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(വരി));

    തിരികെ p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, പൂർണ്ണസംഖ്യ check_return_type, പൂർണ്ണസംഖ്യ legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*അട്ടി_type;

    എങ്കിൽ (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	എങ്കിൽ (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_മൂല്യം_in_function_without_return_type));
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (legacy)
	{
	    പൂർണ്ണസംഖ്യ save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    എങ്കിൽ (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ ശൂന്യ;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (ശൂന്യം)skip_വ്യഞ്ജകം(&p, ശൂന്യ);
	    cmdmod.cmod_flags = save_flags;
	}
	അല്ലെങ്കിൽ
	{
	    // compile return argument into instructions
	    എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
		തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    അട്ടി_type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (check_type_is_മൂല്യം(അട്ടി_type) == FAIL)
		തിരികെ ശൂന്യ;
	    എങ്കിൽ ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ശൂന്യ
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = അട്ടി_type;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (need_type(അട്ടി_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    തിരികെ ശൂന്യ;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	എങ്കിൽ (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_മൂല്യം));
	    തിരികെ ശൂന്യ;
	}

	എങ്കിൽ (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	അല്ലെങ്കിൽ
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    എങ്കിൽ (compile_find_scope(ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // "return val | endif" is possible
    തിരികെ skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    പൂർണ്ണസംഖ്യ
check_global_and_subst(char_u *cmd, char_u *arg)
{
    എങ്കിൽ (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ശൂന്യ)
    {
	semsg(_(e_separator_not_supported_str), arg);
	തിരികെ FAIL;
    }
    എങ്കിൽ (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}


#endif  // defined(FEAT_EVAL)
