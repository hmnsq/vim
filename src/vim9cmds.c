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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಙ್ಕ		idx = instr->ga_len;

    ಯಾವತ್ (idx > 0)
    {
	ಯದಿ (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    ಅನುವರ್ತ;
	}
#ifdef FEAT_PROFILE
	ಯದಿ (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    ಅನುವರ್ತ;
	}
#endif
	ಯದಿ (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unwind_locals(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ new_ತುದಿ, ಪೂರ್ಣಾಙ್ಕ keep)
{
    ಯದಿ (cctx->ctx_locals.ga_len > new_ತುದಿ)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = new_ತುದಿ; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    ಯದಿ (!keep)
	cctx->ctx_locals.ga_len = new_ತುದಿ;
}

/*
 * Free all local variables.
 */
    ಶೂನ್ಯ
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    ಪೂರ್ಣಾಙ್ಕ
check_vim9_unlet(char_u *name)
{
    ಯದಿ (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ರಿಕ್ತ)
    {
	// "unlet s:var" is allowed in legacy script.
	ಯದಿ (*name == 's' && !in_vim9script())
	    ಪ್ರತ್ಯಾಗಮ OK;
	semsg(_(e_cannot_unlet_str), name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    ಪೂರ್ಣಾಙ್ಕ	    deep UNUSED,
    ಶೂನ್ಯ    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    ಪೂರ್ಣಾಙ್ಕ		cc = *name_end;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಪ್ರತ್ಯಾಗಮ OK;

    *name_end = NUL;
    ಯದಿ (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    ಅನ್ಯಥಾ ಯದಿ (vim_strchr(p, '.') != ರಿಕ್ತ || vim_strchr(p, '[') != ರಿಕ್ತ)
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
	ಯದಿ (ret == OK)
	{
	    ಯದಿ (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    ಅನ್ಯಥಾ ಯದಿ (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    ಅನ್ಯಥಾ
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    ಪೂರ್ಣಾಙ್ಕ	    deep,
    ಶೂನ್ಯ    *coookie)
{
    cctx_T	*cctx = coookie;
    ಪೂರ್ಣಾಙ್ಕ		cc = *name_end;
    char_u	*p = lvp->ll_name;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    ವರ್ಣ	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    ಪೂರ್ಣಾಙ್ಕ		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    ಯದಿ (p[1] != ':')
    {
	char_u *end = find_name_end(p, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);

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

	char_u	*name = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	len = end - p;

	ಯದಿ (lookup_local(p, len, ರಿಕ್ತ, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    ಯದಿ (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(ರಿಕ್ತ, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	ಯದಿ (name == ರಿಕ್ತ)
	{
	    class_T *cl;
	    ಯದಿ (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		ಯದಿ (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (ಪೂರ್ಣಾಙ್ಕ)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(ರಿಕ್ತ, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	ಯದಿ (name == ರಿಕ್ತ)
	{
	    // Can lockvar any function arg.
	    ಯದಿ (arg_exists(p, len, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	ಯದಿ (name == ರಿಕ್ತ)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    ಯದಿ (script_var_exists(p, len, cctx, ರಿಕ್ತ) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	ಯದಿ (name != ರಿಕ್ತ)
	{
#ifdef LOG_LOCKVAR
	    ch_log(ರಿಕ್ತ, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    ಯದಿ (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    ಯದಿ (buf == ರಿಕ್ತ)
	ret = FAIL;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (deep < 0)
	    vim_snprintf((ವರ್ಣ *)buf, len, "%s! %s", cmd, p);
	ಅನ್ಯಥಾ
	    vim_snprintf((ವರ್ಣ *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(ರಿಕ್ತ, "LKVAR:    ... buf %s", buf);
#endif
	ಯದಿ (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	ಅನ್ಯಥಾ
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ	    deep = 0;
    char_u  *p = arg;

    ಯದಿ (eap->cmdidx != CMD_unlet)
    {
	ಯದಿ (eap->forceit)
	    deep = -1;
	ಅನ್ಯಥಾ ಯದಿ (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	ಅನ್ಯಥಾ
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    ಪ್ರತ್ಯಾಗಮ eap->nextcmd == ರಿಕ್ತ ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	ಪೂರ್ಣಾಙ್ಕ	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    ಯದಿ (endlabel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    ಯದಿ (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    ಅನ್ಯಥಾ
	generate_JUMP(cctx, when, 0);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
compile_fill_jump_to_end(endlabel_T **el, ಪೂರ್ಣಾಙ್ಕ jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ಯಾವತ್ (*el != ರಿಕ್ತ)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
compile_free_jump_to_end(endlabel_T **el)
{
    ಯಾವತ್ (*el != ರಿಕ್ತ)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ಸ್ಥಿರ scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    ಯದಿ (scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len;
    ಯದಿ (scope->se_outer != ರಿಕ್ತ)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    ಪ್ರತ್ಯಾಗಮ scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    ಶೂನ್ಯ
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    ಯದಿ (scope == ರಿಕ್ತ)
    {
	iemsg("calling drop_scope() without a scope");
	ಪ್ರತ್ಯಾಗಮ;
    }
    cctx->ctx_scope = scope->se_outer;
    ವಿಕಲ್ಪ (scope->se_type)
    {
	ಪಕ್ಷ IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); ಭಙ್ಗ;
	ಪಕ್ಷ FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); ಭಙ್ಗ;
	ಪಕ್ಷ WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); ಭಙ್ಗ;
	ಪಕ್ಷ TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); ಭಙ್ಗ;
	ಪಕ್ಷ NO_SCOPE:
	ಪಕ್ಷ BLOCK_SCOPE:
	    ಭಙ್ಗ;
    }
    vim_free(scope);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ಯದಿ (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ		instr_ಎಣಿಕೆ = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ಯದಿ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    ಅನ್ಯಥಾ ಯದಿ (instr->ga_len == instr_ಎಣಿಕೆ && ppconst.pp_used == 1)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    ಅನ್ಯಥಾ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ಯದಿ (generate_ppconst(cctx, &ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    ಯದಿ (scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    ಯದಿ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    ಅನ್ಯಥಾ
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    ಯದಿ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಙ್ಕ		instr_ಎಣಿಕೆ;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಯದಿ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    ಯದಿ (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    ಯದಿ (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(&p, cctx);
	ಪ್ರತ್ಯಾಗಮ p;
    }

    ಯದಿ (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(&p, cctx);
	ಪ್ರತ್ಯಾಗಮ p;
    }

    ಯದಿ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ	    moved_cmdmod = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	ಯದಿ (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    ಯದಿ (GA_GROW_FAILS(instr, 1))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	ಯದಿ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	ಯದಿ (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	ಯದಿ (moved_cmdmod)
	    ++instr->ga_len;

	ಯದಿ (saved_debug)
	{
	    // move the debug instruction here
	    ಯದಿ (GA_GROW_FAILS(instr, 1))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    ಯದಿ (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	ಯದಿ (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_ಎಣಿಕೆ = instr->ga_len;
    ಯದಿ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    cctx->ctx_skip = save_skip;
    ಯದಿ (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    ಅನ್ಯಥಾ ಯದಿ (instr->ga_len == instr_ಎಣಿಕೆ && ppconst.pp_used == 1)
    {
	ಪೂರ್ಣಾಙ್ಕ error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	ಯದಿ (error)
	{
	    clear_ppconst(&ppconst);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    ಅನ್ಯಥಾ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ಯದಿ (generate_ppconst(cctx, &ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಯದಿ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    ಯದಿ (cctx->ctx_compile_type == CT_PROFILE)
    {
	ಯದಿ (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	ಯದಿ (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    ಯದಿ (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	ಯದಿ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ಯದಿ (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ಯದಿ (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	ಯದಿ (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಯದಿ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    ಯದಿ (scope->se_u.se_if.is_if_label >= 0)
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
    ಯದಿ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_fill_loop_info(loop_info_T *loop_info, ಪೂರ್ಣಾಙ್ಕ funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len;
    loop_info->li_closure_ಎಣಿಕೆ = cctx->ctx_closure_ಎಣಿಕೆ;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ಸ್ಥಿರ type_T *
compile_for_tuple_get_vartype(type_T *vartype, ಪೂರ್ಣಾಙ್ಕ var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    ಯದಿ (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	ಪ್ರತ್ಯಾಗಮ &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    ಯದಿ (member_type->tt_type == VAR_ANY)
	ಪ್ರತ್ಯಾಗಮ &t_any;

    ಯದಿ (!var_list)
	// for x in tuple<...list<xxx>>
	ಪ್ರತ್ಯಾಗಮ member_type;

    ಯದಿ (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	ಪ್ರತ್ಯಾಗಮ member_type->tt_member;
    ಅನ್ಯಥಾ ಯದಿ (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	ಪ್ರತ್ಯಾಗಮ member_type->tt_args[0]->tt_member;

    ಪ್ರತ್ಯಾಗಮ &t_any;
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
    char_u	*name = ರಿಕ್ತ;
    char_u	*p;
    char_u	*wp;
    ಪೂರ್ಣಾಙ್ಕ		var_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		var_list = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    ಪೂರ್ಣಾಙ್ಕ		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    ಪೂರ್ಣಾಙ್ಕ		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_ಎಣಿಕೆ, &semicolon, FALSE);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (var_ಎಣಿಕೆ == 0)
	var_ಎಣಿಕೆ = 1;
    ಅನ್ಯಥಾ
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    ಯದಿ (may_get_next_ಸಾಲು_error(wp, &p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	ಯದಿ (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	ಅನ್ಯಥಾ
	    emsg(_(e_missing_in_after_for));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    wp = p + 2;
    ಯದಿ (may_get_next_ಸಾಲು_error(wp, &p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    ಯದಿ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    ಯದಿ (scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ಸಂಖ್ಯೆ);
    ಯದಿ (loop_lvar == ರಿಕ್ತ)
    {
	drop_scope(cctx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ಸಂಖ್ಯೆ);
    ಯದಿ (funcref_lvar == ರಿಕ್ತ)
    {
	drop_scope(cctx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    ಯದಿ (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    arg_end = arg;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ರಾಶಿ(cctx, 0);
	ಯದಿ (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	ಅನ್ಯಥಾ ಯದಿ (vartype->tt_type == VAR_BLOB)
	    item_type = &t_ಸಂಖ್ಯೆ;
	ಅನ್ಯಥಾ ಯದಿ (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    ಯದಿ (!var_list)
		item_type = vartype->tt_member;
	    ಅನ್ಯಥಾ ಯದಿ (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	ಅನ್ಯಥಾ ಯದಿ (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_ತುದಿ_label = current_instr_idx(cctx);

	ಯದಿ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    ಪೂರ್ಣಾಙ್ಕ		save_prev_lnum = cctx->ctx_prev_lnum;
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
	ಯದಿ (var_list)
	{
	    generate_UNPACK(cctx, var_ಎಣಿಕೆ, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ರಾಶಿ.ga_len;

	    // add type of the items
	    ಪ್ರತಿ (idx = 0; idx < var_ಎಣಿಕೆ; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		ಯದಿ (ತಳ್ಳು_type_ರಾಶಿ(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}

	ಪ್ರತಿ (idx = 0; idx < var_ಎಣಿಕೆ; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    ಪೂರ್ಣಾಙ್ಕ			opt_flags = 0;
	    ಪೂರ್ಣಾಙ್ಕ			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    ಯದಿ (name == ರಿಕ್ತ)
		ಗಚ್ಛ failed;
	    ಯದಿ (*skipwhite(p) == ':')
	    {
		ಯದಿ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ಗಚ್ಛ failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		ಯದಿ (lhs_type == ರಿಕ್ತ || !valid_declaration_type(lhs_type))
		    ಗಚ್ಛ failed;
	    }

	    ಯದಿ (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		ಗಚ್ಛ failed;
	    ಯದಿ (dest != dest_local)
	    {
		ಯದಿ (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ರಿಕ್ತ) == FAIL)
		    ಗಚ್ಛ failed;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		ಯದಿ (generate_instr_drop(cctx, ISN_DROP, 1) == ರಿಕ್ತ)
		    ಗಚ್ಛ failed;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Script var is not supported.
		ಯದಿ (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    ಗಚ್ಛ failed;
		}

		ಯದಿ (!valid_varname(arg, (ಪೂರ್ಣಾಙ್ಕ)varlen, FALSE))
		    ಗಚ್ಛ failed;
		ಯದಿ (lookup_local(arg, varlen, ರಿಕ್ತ, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    ಗಚ್ಛ failed;
		}

		// Reserve a variable to store "var".
		ಯದಿ (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		ಯದಿ (lhs_type == &t_any)
		    lhs_type = item_type;
		ಅನ್ಯಥಾ ಯದಿ (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    ಗಚ್ಛ failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		ಯದಿ (var_lvar == ರಿಕ್ತ)
		    // out of memory or used as an argument
		    ಗಚ್ಛ failed;

		ಯದಿ (semicolon && idx == var_ಎಣಿಕೆ - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ರಿಕ್ತ);
	    }

	    ಯದಿ (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    ಪ್ರತ್ಯಾಗಮ arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    ಯದಿ (cctx->ctx_locals.ga_len > loop_info->li_local_ಎಣಿಕೆ
	    && cctx->ctx_closure_ಎಣಿಕೆ > loop_info->li_closure_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ generate_ENDLOOP(cctx, loop_info);
    ಪ್ರತ್ಯಾಗಮ OK;
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

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ಯದಿ (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_ತುದಿ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_ತುದಿ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	ಯದಿ (generate_instr_drop(cctx, ISN_DROP, 1) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    vim_free(scope);

    ಪ್ರತ್ಯಾಗಮ arg;
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
    ಪೂರ್ಣಾಙ್ಕ		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    ಯದಿ (scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_ತುದಿ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ಸಂಖ್ಯೆ);
    ಯದಿ (funcref_lvar == ರಿಕ್ತ)
    {
	drop_scope(cctx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	ಯದಿ (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    cctx->ctx_scope = scope->se_outer;
    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ಯದಿ (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_ತುದಿ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    ಪೂರ್ಣಾಙ್ಕ
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    ಪೂರ್ಣಾಙ್ಕ		prev_local_ಎಣಿಕೆ = 0;

    CLEAR_POINTER(lvi);
    ಪ್ರತಿ (;;)
    {
	loop_info_T	*loopinfo;
	ಪೂರ್ಣಾಙ್ಕ		cur_local_last;
	ಪೂರ್ಣಾಙ್ಕ		start_local_ಎಣಿಕೆ;

	ಯಾವತ್ (scope != ರಿಕ್ತ && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	ಯದಿ (scope == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	ಯದಿ (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_ಎಣಿಕೆ - 1;
	}
	ಅನ್ಯಥಾ
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_ಎಣಿಕೆ - 3;
	}

	start_local_ಎಣಿಕೆ = loopinfo->li_local_ಎಣಿಕೆ;
	ಯದಿ (cctx->ctx_locals.ga_len > start_local_ಎಣಿಕೆ)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_ಎಣಿಕೆ;
	    lvi->lvi_loop[loopinfo->li_depth].var_ಎಣಿಕೆ =
			  (short)(cctx->ctx_locals.ga_len - start_local_ಎಣಿಕೆ
							   - prev_local_ಎಣಿಕೆ);
	    ಯದಿ (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len - cur_local_last;
    }
    ಪ್ರತ್ಯಾಗಮ lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    ಶೂನ್ಯ
get_loop_var_idx(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    ಯದಿ (get_loop_var_info(cctx, &lvi))
    {
	ಪೂರ್ಣಾಙ್ಕ depth;

	ಪ್ರತಿ (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    ಯದಿ (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_ಎಣಿಕೆ)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		ಪ್ರತ್ಯಾಗಮ;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_find_scope(
	ಪೂರ್ಣಾಙ್ಕ	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	ಪೂರ್ಣಾಙ್ಕ	    *try_scopes,    // :try scopes encountered or NULL
	ವರ್ಣ	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (scope == ರಿಕ್ತ)
	{
	    ಯದಿ (error != ರಿಕ್ತ)
		emsg(_(error));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (scope->se_type == FOR_SCOPE)
	{
	    ಯದಿ (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (loop_label != ರಿಕ್ತ)
		*loop_label = scope->se_u.se_for.fs_ತುದಿ_label;
	    ಯದಿ (el != ರಿಕ್ತ)
		*el = &scope->se_u.se_for.fs_end_label;
	    ಭಙ್ಗ;
	}
	ಯದಿ (scope->se_type == WHILE_SCOPE)
	{
	    ಯದಿ (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (loop_label != ರಿಕ್ತ)
		*loop_label = scope->se_u.se_while.ws_ತುದಿ_label;
	    ಯದಿ (el != ರಿಕ್ತ)
		*el = &scope->se_u.se_while.ws_end_label;
	    ಭಙ್ಗ;
	}
	ಯದಿ (try_scopes != ರಿಕ್ತ && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		try_scopes = 0;
    ಪೂರ್ಣಾಙ್ಕ		loop_label;

    ಯದಿ (compile_find_scope(&loop_label, ರಿಕ್ತ, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    ಅನ್ಯಥಾ
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		try_scopes = 0;
    endlabel_T	**el;

    ಯದಿ (compile_find_scope(ರಿಕ್ತ, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಪ್ರತ್ಯಾಗಮ arg;

    ಯದಿ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    ಯದಿ (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    ಯದಿ (new_scope(cctx, BLOCK_SCOPE) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    ಶೂನ್ಯ
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
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

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    ಯದಿ (try_scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	ಯದಿ ((isn = generate_instr(cctx, ISN_TRY)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	ಯದಿ (isn->isn_arg.tryref.try_ref == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    ಯದಿ (scope == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ arg;
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

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // end block scope from :try or :catch
    ಯದಿ (scope != ರಿಕ್ತ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	ಯದಿ (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ಯದಿ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	ಯದಿ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	ಯದಿ (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    ಯದಿ (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    ಅನ್ಯಥಾ
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	dropped = 0;
	ಪೂರ್ಣಾಙ್ಕ	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ರಿಕ್ತ);
	ಯದಿ (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (tofree == ರಿಕ್ತ)
	    len = (ಪೂರ್ಣಾಙ್ಕ)(end - (p + 1));
	ಅನ್ಯಥಾ
	    len = (ಪೂರ್ಣಾಙ್ಕ)(end - tofree);
	pat = vim_strnsave(tofree == ರಿಕ್ತ ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	ಯದಿ (pat == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (generate_PUSHS(cctx, &pat) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (generate_COMPARE(cctx, ಅಭಿವ್ಯಕ್ತಿ_MATCH, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	ಯದಿ (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (new_scope(cctx, BLOCK_SCOPE) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    ಪೂರ್ಣಾಙ್ಕ		this_instr;

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // end block scope from :try or :catch
    ಯದಿ (scope != ರಿಕ್ತ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    ಯದಿ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ಯದಿ (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    ಯದಿ (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	ಯದಿ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	ಯದಿ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	ಯದಿ (generate_instr(cctx, ISN_FINALLY) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    ಯದಿ (misplaced_cmdmod(cctx))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // end block scope from :catch or :finally
    ಯದಿ (scope != ರಿಕ್ತ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಯದಿ (scope == ರಿಕ್ತ || scope->se_type != TRY_SCOPE)
    {
	ಯದಿ (scope == ರಿಕ್ತ)
	    emsg(_(e_endtry_without_try));
	ಅನ್ಯಥಾ ಯದಿ (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	ಅನ್ಯಥಾ ಯದಿ (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	ಅನ್ಯಥಾ
	    emsg(_(e_missing_endif));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

#ifdef FEAT_PROFILE
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE
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

	ಯದಿ (scope->se_u.se_try.ts_catch_label != 0)
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

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	ಯದಿ (generate_instr(cctx, ISN_ENDTRY) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ifdef FEAT_PROFILE
	ಯದಿ (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಪ್ರತ್ಯಾಗಮ p;
    ಯದಿ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (generate_instr_drop(cctx, ISN_THROW, 1) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    ಪೂರ್ಣಾಙ್ಕ		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    ಪ್ರತ್ಯಾಗಮ skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    ಯದಿ (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	ಯದಿ (lvar == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    ಪ್ರತ್ಯಾಗಮ dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;
    ಪೂರ್ಣಾಙ್ಕ		defer_var_idx;
    type_T	*type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		func_idx = -1;

    ಯದಿ (*arg == '(')
    {
	// a lambda function
	ಯದಿ (compile_lambda(&arg, cctx, ರಿಕ್ತ) != OK)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	paren = vim_strchr(arg, '(');
	ಯದಿ (paren == ರಿಕ್ತ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	ಯದಿ (paren == ರಿಕ್ತ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	ಯದಿ (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	ಅನ್ಯಥಾ ಯದಿ (compile_expr0(&arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*paren = '(';
    }

    // check for function type
    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ರಾಶಿ(cctx, 0);
	ಯದಿ (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    ಯದಿ (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ (func_idx >= 0)
	{
	    type2_T	*argtypes = ರಿಕ್ತ;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    ಯದಿ (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	defer_var_idx = get_defer_var_idx(cctx);
	ಯದಿ (defer_var_idx == 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ skipwhite(arg);
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
compile_mult_ಅಭಿವ್ಯಕ್ತಿ(
	char_u	*arg,
	ಪೂರ್ಣಾಙ್ಕ	cmdidx,
	long	cmd_ಎಣಿಕೆ UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_start;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    ಪೂರ್ಣಾಙ್ಕ		r = OK;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (ends_excmd2(prev, p))
	    ಭಙ್ಗ;
	ಅಭಿವ್ಯಕ್ತಿ_start = p;
	ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಯದಿ (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_ಮೌಲ್ಯ_str), ಅಭಿವ್ಯಕ್ತಿ_start);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}

	++ಎಣಿಕೆ;
	prev = p;
	p = skipwhite(p);
    }

    ಯದಿ (ಎಣಿಕೆ > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	ಯದಿ (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (cmdidx == CMD_execute)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_EXECUTE, ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (cmdidx == CMD_echomsg)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOMSG, ಎಣಿಕೆ);
#ifdef HAS_MESSAGE_WINDOW
	ಅನ್ಯಥಾ ಯದಿ (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, ಎಣಿಕೆ, cmd_ಎಣಿಕೆ);
#endif
	ಅನ್ಯಥಾ ಯದಿ (cmdidx == CMD_echoconsole)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOCONSOLE, ಎಣಿಕೆ);
	ಅನ್ಯಥಾ
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOERR, ಎಣಿಕೆ);

	cctx->ctx_lnum = save_lnum;
    }
    ಪ್ರತ್ಯಾಗಮ r == OK ? p : ರಿಕ್ತ;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, ರಿಕ್ತ);
    char_u *p = skipdigits(eap->cmd);

    ಯದಿ (p == range_end)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ fixindent)
{
    char_u	*ಸಾಲು = arg;
    linenr_T	lnum;
    ವರ್ಣ	*errormsg;
    ಪೂರ್ಣಾಙ್ಕ		above = eap->forceit;

    eap->regname = *ಸಾಲು;

    ಯದಿ (eap->regname == '=')
    {
	char_u *p = skipwhite(ಸಾಲು + 1);

	ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಸಾಲು = p;
    }
    ಅನ್ಯಥಾ ಯದಿ (eap->regname != NUL)
	++ಸಾಲು;

    ಯದಿ (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    ಅನ್ಯಥಾ
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	ಯದಿ (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
	    lnum = -1;
	ಅನ್ಯಥಾ
	    lnum = eap->line2;
	ಯದಿ (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *ಸಾಲು_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*ಸಾಲು = ಸಾಲು_arg;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		has_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ರಿಕ್ತ;
    char_u	*cmd_arg = ರಿಕ್ತ;

    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಗಚ್ಛ theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    ಯದಿ (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	ಯದಿ (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    ಯದಿ (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	ಪೂರ್ಣಾಙ್ಕ	usefilter = FALSE;

	has_ಅಭಿವ್ಯಕ್ತಿ = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	ಯದಿ ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	ಯದಿ ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
		nextcmd = eap->nextcmd;
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    ಯದಿ (*p != NUL)
		++p;
	    ಯದಿ (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    ಯದಿ (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    ಪೂರ್ಣಾಙ್ಕ	    flags = 0;  // unused
	    ಪೂರ್ಣಾಙ್ಕ	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, ಸಾಲು, &tofree, &flags);
	    ಯದಿ (tofree != ರಿಕ್ತ)
	    {
		ಸಾಲು = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    ಯದಿ (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	ಯದಿ (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    ಯದಿ ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	ಪೂರ್ಣಾಙ್ಕ delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (*p == delim)
	    cmd_arg = p + 1;
    }

    ಯದಿ (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    ಯದಿ (cmd_arg != ರಿಕ್ತ)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, ರಿಕ್ತ, lookup_scriptitem, ರಿಕ್ತ);
	ಯದಿ (nea.cmdidx < CMD_SIZE)
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    ಯದಿ (has_ಅಭಿವ್ಯಕ್ತಿ)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    ಯದಿ (has_ಅಭಿವ್ಯಕ್ತಿ && (p = (char_u *)strstr((ವರ್ಣ *)eap->arg, "`=")) != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	ಎಣಿಕೆ = 0;
	char_u	*start = skipwhite(ಸಾಲು);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++ಎಣಿಕೆ;
	    }
	    p += 2;
	    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++ಎಣಿಕೆ;
	    p = skipwhite(p);
	    ಯದಿ (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((ವರ್ಣ *)start, "`=");
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++ಎಣಿಕೆ;
		}
		ಭಙ್ಗ;
	    }
	}
	generate_EXECCONCAT(cctx, ಎಣಿಕೆ);
    }
    ಅನ್ಯಥಾ
	generate_EXEC_copy(cctx, ISN_EXEC, ಸಾಲು);

theend:
    ಯದಿ (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    ಪ್ರತ್ಯಾಗಮ nextcmd;
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
compile_script(char_u *ಸಾಲು, cctx_T *cctx)
{
    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	ಯದಿ ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	isn->isn_arg.string = vim_strsave(ಸಾಲು);
    }
    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *ಅಭಿವ್ಯಕ್ತಿ = (char_u *)strstr((ವರ್ಣ *)cmd, "\\=");

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    ಪೂರ್ಣಾಙ್ಕ		ಅಭಿವ್ಯಕ್ತಿ_res;
	    ಪೂರ್ಣಾಙ್ಕ		trailing_error;
	    ಪೂರ್ಣಾಙ್ಕ		instr_ಎಣಿಕೆ;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = ರಿಕ್ತ;
	    ಅಭಿವ್ಯಕ್ತಿ_res = compile_expr0(&cmd, cctx);
	    ಯದಿ (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		ಯದಿ (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_ಎಣಿಕೆ].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    ಯದಿ ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ idx;

		ಪ್ರತಿ (idx = 0; idx < instr_ಎಣಿಕೆ; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    ಯದಿ (*end == '&')
		++end;
	    ಯಾವತ್ (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    ಪ್ರತ್ಯಾಗಮ end;
	}
    }

    ಪ್ರತ್ಯಾಗಮ compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *ಸಾಲು, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    ಯದಿ (lhs->lhs_name != ರಿಕ್ತ)
    {
	ಯದಿ (STRNCMP(arg, "END", 3) == 0)
	{
	    ಯದಿ (cctx->ctx_skip != SKIP_YES)
	    {
		ಯದಿ (lhs->lhs_append)
		{
		    // First load the current variable value.
		    ಯದಿ (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		ಯದಿ (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		ಯದಿ (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    ಯದಿ (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    ಪ್ರತ್ಯಾಗಮ arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (arg[0] == '=' && arg[1] == '>')
    {
	ಪೂರ್ಣಾಙ್ಕ	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	ಯದಿ (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	ಯದಿ (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	ಅನ್ಯಥಾ
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    ಯದಿ (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		ಯದಿ (lhs->lhs_whole == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}

	ಪ್ರತ್ಯಾಗಮ arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    ಪ್ರತ್ಯಾಗಮ compile_exec(ಸಾಲು, eap, cctx);
}

#ಯದಿ defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *ಸಾಲು, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    isn->isn_arg.ಸಂಖ್ಯೆ = eap->cmdidx;

    p = eap->arg;
    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    ಯದಿ (isn->isn_arg.cexpr.cexpr_ref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(ಸಾಲು));

    ಪ್ರತ್ಯಾಗಮ p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, ಪೂರ್ಣಾಙ್ಕ check_return_type, ಪೂರ್ಣಾಙ್ಕ legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ರಾಶಿ_type;

    ಯದಿ (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	ಯದಿ (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_ಮೌಲ್ಯ_in_function_without_return_type));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (legacy)
	{
	    ಪೂರ್ಣಾಙ್ಕ save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    ಯದಿ (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ರಿಕ್ತ);
	    cmdmod.cmod_flags = save_flags;
	}
	ಅನ್ಯಥಾ
	{
	    // compile return argument into instructions
	    ಯದಿ (compile_expr0(&p, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ರಾಶಿ_type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಯದಿ (check_type_is_ಮೌಲ್ಯ(ರಾಶಿ_type) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಯದಿ ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ರಿಕ್ತ
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ರಾಶಿ_type;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (need_type(ರಾಶಿ_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	ಯದಿ (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_ಮೌಲ್ಯ));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಯದಿ (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	ಅನ್ಯಥಾ
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    ಯದಿ (compile_find_scope(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    ಯದಿ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // "return val | endif" is possible
    ಪ್ರತ್ಯಾಗಮ skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_global_and_subst(char_u *cmd, char_u *arg)
{
    ಯದಿ (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ರಿಕ್ತ)
    {
	semsg(_(e_separator_not_supported_str), arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}


#endif  // defined(FEAT_EVAL)
