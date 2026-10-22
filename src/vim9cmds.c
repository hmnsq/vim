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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    સ્થિર પૂર્ણાંક
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    પૂર્ણાંક		idx = instr->ga_len;

    જ્યારે (idx > 0)
    {
	જો (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    ચાલુ;
	}
#ifdef FEAT_PROFILE
	જો (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    ચાલુ;
	}
#endif
	જો (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    ચાલુ;
	}
	તોડો;
    }
    પાછા idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    સ્થિર શૂન્ય
unwind_locals(cctx_T *cctx, પૂર્ણાંક new_ટોચ, પૂર્ણાંક keep)
{
    જો (cctx->ctx_locals.ga_len > new_ટોચ)
	માટે (પૂર્ણાંક idx = new_ટોચ; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    જો (!keep)
	cctx->ctx_locals.ga_len = new_ટોચ;
}

/*
 * Free all local variables.
 */
    શૂન્ય
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    પૂર્ણાંક
check_vim9_unlet(char_u *name)
{
    જો (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	પાછા FAIL;
    }

    જો (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ખાલી)
    {
	// "unlet s:var" is allowed in legacy script.
	જો (*name == 's' && !in_vim9script())
	    પાછા OK;
	semsg(_(e_cannot_unlet_str), name);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    સ્થિર પૂર્ણાંક
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    પૂર્ણાંક	    deep UNUSED,
    શૂન્ય    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    પૂર્ણાંક		cc = *name_end;
    પૂર્ણાંક		ret = OK;

    જો (cctx->ctx_skip == SKIP_YES)
	પાછા OK;

    *name_end = NUL;
    જો (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    નહીંતર જો (vim_strchr(p, '.') != ખાલી || vim_strchr(p, '[') != ખાલી)
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
	જો (ret == OK)
	{
	    જો (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    નહીંતર
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    નહીંતર જો (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    નહીંતર
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    પાછા ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    સ્થિર પૂર્ણાંક
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    પૂર્ણાંક	    deep,
    શૂન્ય    *coookie)
{
    cctx_T	*cctx = coookie;
    પૂર્ણાંક		cc = *name_end;
    char_u	*p = lvp->ll_name;
    પૂર્ણાંક		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    અક્ષર	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    પૂર્ણાંક		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    જો (cctx->ctx_skip == SKIP_YES)
	પાછા OK;

    જો (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	પાછા FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    જો (p[1] != ':')
    {
	char_u *end = find_name_end(p, ખાલી, ખાલી, FNE_CHECK_START);

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

	char_u	*name = ખાલી;
	પૂર્ણાંક	len = end - p;

	જો (lookup_local(p, len, ખાલી, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    જો (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		પાછા FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(ખાલી, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	જો (name == ખાલી)
	{
	    class_T *cl;
	    જો (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		જો (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (પૂર્ણાંક)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(ખાલી, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	જો (name == ખાલી)
	{
	    // Can lockvar any function arg.
	    જો (arg_exists(p, len, ખાલી, ખાલી, ખાલી, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	જો (name == ખાલી)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    જો (script_var_exists(p, len, cctx, ખાલી) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	જો (name != ખાલી)
	{
#ifdef LOG_LOCKVAR
	    ch_log(ખાલી, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    જો (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    જો (buf == ખાલી)
	ret = FAIL;
    નહીંતર
    {
	જો (deep < 0)
	    vim_snprintf((અક્ષર *)buf, len, "%s! %s", cmd, p);
	નહીંતર
	    vim_snprintf((અક્ષર *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(ખાલી, "LKVAR:    ... buf %s", buf);
#endif
	જો (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	નહીંતર
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    પાછા ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    પૂર્ણાંક	    deep = 0;
    char_u  *p = arg;

    જો (eap->cmdidx != CMD_unlet)
    {
	જો (eap->forceit)
	    deep = -1;
	નહીંતર જો (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	નહીંતર
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    પાછા eap->nextcmd == ખાલી ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    સ્થિર પૂર્ણાંક
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	પૂર્ણાંક	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    જો (endlabel == ખાલી)
	પાછા FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    જો (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    નહીંતર
	generate_JUMP(cctx, when, 0);
    પાછા OK;
}

    સ્થિર શૂન્ય
compile_fill_jump_to_end(endlabel_T **el, પૂર્ણાંક jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    જ્યારે (*el != ખાલી)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    સ્થિર શૂન્ય
compile_free_jump_to_end(endlabel_T **el)
{
    જ્યારે (*el != ખાલી)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    સ્થિર scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    જો (scope == ખાલી)
	પાછા ખાલી;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_ગણતરી = cctx->ctx_locals.ga_len;
    જો (scope->se_outer != ખાલી)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    પાછા scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    શૂન્ય
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    જો (scope == ખાલી)
    {
	iemsg("calling drop_scope() without a scope");
	પાછા;
    }
    cctx->ctx_scope = scope->se_outer;
    પસંદગી (scope->se_type)
    {
	સ્થિતિ IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); તોડો;
	સ્થિતિ FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); તોડો;
	સ્થિતિ WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); તોડો;
	સ્થિતિ TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); તોડો;
	સ્થિતિ NO_SCOPE:
	સ્થિતિ BLOCK_SCOPE:
	    તોડો;
    }
    vim_free(scope);
}

    સ્થિર પૂર્ણાંક
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    જો (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	પાછા TRUE;
    }
    પાછા FALSE;
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
    પૂર્ણાંક		instr_ગણતરી = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    જો (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	પાછા ખાલી;
    }
    જો (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	પાછા ખાલી;
    }
    જો (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    નહીંતર જો (instr->ga_len == instr_ગણતરી && ppconst.pp_used == 1)
    {
	પૂર્ણાંક error = FALSE;
	પૂર્ણાંક v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	જો (error)
	    પાછા ખાલી;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    નહીંતર
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	જો (generate_ppconst(cctx, &ppconst) == FAIL)
	    પાછા ખાલી;
	જો (bool_on_ઢગલો(cctx) == FAIL)
	    પાછા ખાલી;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    જો (scope == ખાલી)
	પાછા ખાલી;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    જો (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    નહીંતર
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    જો (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    પાછા p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    પૂર્ણાંક		instr_ગણતરી;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    જો (scope == ખાલી || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	પાછા ખાલી;
    }
    જો (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	પાછા ખાલી;
    }
    unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);
    જો (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    જો (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    જો (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_પદાવલિ_cctx(&p, cctx);
	પાછા p;
    }

    જો (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_પદાવલિ_cctx(&p, cctx);
	પાછા p;
    }

    જો (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	પૂર્ણાંક	    moved_cmdmod = FALSE;
	પૂર્ણાંક	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	જો (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    જો (GA_GROW_FAILS(instr, 1))
		પાછા ખાલી;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	જો (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	જો (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    પાછા ખાલી;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	જો (moved_cmdmod)
	    ++instr->ga_len;

	જો (saved_debug)
	{
	    // move the debug instruction here
	    જો (GA_GROW_FAILS(instr, 1))
		પાછા ખાલી;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    જો (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	જો (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_ગણતરી = instr->ga_len;
    જો (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	પાછા ખાલી;
    }
    cctx->ctx_skip = save_skip;
    જો (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	પાછા ખાલી;
    }
    જો (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    નહીંતર જો (instr->ga_len == instr_ગણતરી && ppconst.pp_used == 1)
    {
	પૂર્ણાંક error = FALSE;
	પૂર્ણાંક v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	જો (error)
	{
	    clear_ppconst(&ppconst);
	    પાછા ખાલી;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    નહીંતર
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	જો (generate_ppconst(cctx, &ppconst) == FAIL)
	    પાછા ખાલી;
	જો (bool_on_ઢગલો(cctx) == FAIL)
	    પાછા ખાલી;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    પાછા p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    જો (scope == ખાલી || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	પાછા ખાલી;
    }
    જો (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	પાછા ખાલી;
    }
    unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);
    જો (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    જો (cctx->ctx_compile_type == CT_PROFILE)
    {
	જો (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	જો (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    જો (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	જો (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    જો (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		પાછા ખાલી;
	}

	જો (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    જો (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	જો (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    પાછા p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    જો (scope == ખાલી || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	પાછા ખાલી;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);
    જો (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    જો (scope->se_u.se_if.is_if_label >= 0)
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
    જો (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    પાછા arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    સ્થિર શૂન્ય
compile_fill_loop_info(loop_info_T *loop_info, પૂર્ણાંક funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_ગણતરી = cctx->ctx_locals.ga_len;
    loop_info->li_closure_ગણતરી = cctx->ctx_closure_ગણતરી;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    સ્થિર type_T *
compile_for_tuple_get_vartype(type_T *vartype, પૂર્ણાંક var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    જો (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	પાછા &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    જો (member_type->tt_type == VAR_ANY)
	પાછા &t_any;

    જો (!var_list)
	// for x in tuple<...list<xxx>>
	પાછા member_type;

    જો (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	પાછા member_type->tt_member;
    નહીંતર જો (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	પાછા member_type->tt_args[0]->tt_member;

    પાછા &t_any;
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
    char_u	*name = ખાલી;
    char_u	*p;
    char_u	*wp;
    પૂર્ણાંક		var_ગણતરી = 0;
    પૂર્ણાંક		var_list = FALSE;
    પૂર્ણાંક		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    પૂર્ણાંક		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    પૂર્ણાંક		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_ગણતરી, &semicolon, FALSE);
    જો (p == ખાલી)
	પાછા ખાલી;
    જો (var_ગણતરી == 0)
	var_ગણતરી = 1;
    નહીંતર
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    જો (may_get_next_લીટી_error(wp, &p, cctx) == FAIL)
	પાછા ખાલી;
    જો (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	જો (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	નહીંતર
	    emsg(_(e_missing_in_after_for));
	પાછા ખાલી;
    }
    wp = p + 2;
    જો (may_get_next_લીટી_error(wp, &p, cctx) == FAIL)
	પાછા ખાલી;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    જો (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    જો (scope == ખાલી)
	પાછા ખાલી;
    જો (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	પાછા ખાલી;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_સંખ્યા);
    જો (loop_lvar == ખાલી)
    {
	drop_scope(cctx);
	પાછા ખાલી;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_સંખ્યા);
    જો (funcref_lvar == ખાલી)
    {
	drop_scope(cctx);
	પાછા ખાલી;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    જો (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	પાછા ખાલી;
    }
    arg_end = arg;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ઢગલો(cctx, 0);
	જો (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    પાછા ખાલી;
	}

	જો (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	નહીંતર જો (vartype->tt_type == VAR_BLOB)
	    item_type = &t_સંખ્યા;
	નહીંતર જો (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    જો (!var_list)
		item_type = vartype->tt_member;
	    નહીંતર જો (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	નહીંતર જો (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_ટોચ_label = current_instr_idx(cctx);

	જો (cctx->ctx_compile_type == CT_DEBUG)
	{
	    પૂર્ણાંક		save_prev_lnum = cctx->ctx_prev_lnum;
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
	જો (var_list)
	{
	    generate_UNPACK(cctx, var_ગણતરી, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ઢગલો.ga_len;

	    // add type of the items
	    માટે (idx = 0; idx < var_ગણતરી; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		જો (ધકેલો_type_ઢગલો(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    પાછા ખાલી;
		}
	    }
	}

	માટે (idx = 0; idx < var_ગણતરી; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    પૂર્ણાંક			opt_flags = 0;
	    પૂર્ણાંક			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    જો (name == ખાલી)
		જાઓ failed;
	    જો (*skipwhite(p) == ':')
	    {
		જો (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    જાઓ failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		જો (lhs_type == ખાલી || !valid_declaration_type(lhs_type))
		    જાઓ failed;
	    }

	    જો (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		જાઓ failed;
	    જો (dest != dest_local)
	    {
		જો (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ખાલી) == FAIL)
		    જાઓ failed;
	    }
	    નહીંતર જો (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		જો (generate_instr_drop(cctx, ISN_DROP, 1) == ખાલી)
		    જાઓ failed;
	    }
	    નહીંતર
	    {
		// Script var is not supported.
		જો (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    જાઓ failed;
		}

		જો (!valid_varname(arg, (પૂર્ણાંક)varlen, FALSE))
		    જાઓ failed;
		જો (lookup_local(arg, varlen, ખાલી, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    જાઓ failed;
		}

		// Reserve a variable to store "var".
		જો (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		જો (lhs_type == &t_any)
		    lhs_type = item_type;
		નહીંતર જો (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    જાઓ failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		જો (var_lvar == ખાલી)
		    // out of memory or used as an argument
		    જાઓ failed;

		જો (semicolon && idx == var_ગણતરી - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ખાલી);
	    }

	    જો (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    પાછા arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    પાછા ખાલી;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    સ્થિર પૂર્ણાંક
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    જો (cctx->ctx_locals.ga_len > loop_info->li_local_ગણતરી
	    && cctx->ctx_closure_ગણતરી > loop_info->li_closure_ગણતરી)
	પાછા generate_ENDLOOP(cctx, loop_info);
    પાછા OK;
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

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    જો (scope == ખાલી || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	પાછા ખાલી;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    જો (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	જો (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    પાછા ખાલી;

	unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_ટોચ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_ટોચ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	જો (generate_instr_drop(cctx, ISN_DROP, 1) == ખાલી)
	    પાછા ખાલી;
    }

    vim_free(scope);

    પાછા arg;
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
    પૂર્ણાંક		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    જો (scope == ખાલી)
	પાછા ખાલી;
    જો (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	પાછા ખાલી;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_ટોચ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_સંખ્યા);
    જો (funcref_lvar == ખાલી)
    {
	drop_scope(cctx);
	પાછા ખાલી;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    જો (compile_expr0(&p, cctx) == FAIL)
	પાછા ખાલી;

    જો (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	પાછા ખાલી;
    }

    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો (bool_on_ઢગલો(cctx) == FAIL)
	    પાછા FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	જો (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    પાછા FAIL;
    }

    પાછા p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;
    જો (scope == ખાલી || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	પાછા ખાલી;
    }
    cctx->ctx_scope = scope->se_outer;
    જો (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	જો (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    પાછા ખાલી;

	unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_ટોચ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    પાછા arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    પૂર્ણાંક
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    પૂર્ણાંક		prev_local_ગણતરી = 0;

    CLEAR_POINTER(lvi);
    માટે (;;)
    {
	loop_info_T	*loopinfo;
	પૂર્ણાંક		cur_local_last;
	પૂર્ણાંક		start_local_ગણતરી;

	જ્યારે (scope != ખાલી && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	જો (scope == ખાલી)
	    તોડો;

	જો (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_ગણતરી - 1;
	}
	નહીંતર
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_ગણતરી - 3;
	}

	start_local_ગણતરી = loopinfo->li_local_ગણતરી;
	જો (cctx->ctx_locals.ga_len > start_local_ગણતરી)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_ગણતરી;
	    lvi->lvi_loop[loopinfo->li_depth].var_ગણતરી =
			  (short)(cctx->ctx_locals.ga_len - start_local_ગણતરી
							   - prev_local_ગણતરી);
	    જો (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_ગણતરી = cctx->ctx_locals.ga_len - cur_local_last;
    }
    પાછા lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    શૂન્ય
get_loop_var_idx(cctx_T *cctx, પૂર્ણાંક idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    જો (get_loop_var_info(cctx, &lvi))
    {
	પૂર્ણાંક depth;

	માટે (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    જો (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_ગણતરી)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		પાછા;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    સ્થિર પૂર્ણાંક
compile_find_scope(
	પૂર્ણાંક	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	પૂર્ણાંક	    *try_scopes,    // :try scopes encountered or NULL
	અક્ષર	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    માટે (;;)
    {
	જો (scope == ખાલી)
	{
	    જો (error != ખાલી)
		emsg(_(error));
	    પાછા FAIL;
	}
	જો (scope->se_type == FOR_SCOPE)
	{
	    જો (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		પાછા FAIL;
	    જો (loop_label != ખાલી)
		*loop_label = scope->se_u.se_for.fs_ટોચ_label;
	    જો (el != ખાલી)
		*el = &scope->se_u.se_for.fs_end_label;
	    તોડો;
	}
	જો (scope->se_type == WHILE_SCOPE)
	{
	    જો (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		પાછા FAIL;
	    જો (loop_label != ખાલી)
		*loop_label = scope->se_u.se_while.ws_ટોચ_label;
	    જો (el != ખાલી)
		*el = &scope->se_u.se_while.ws_end_label;
	    તોડો;
	}
	જો (try_scopes != ખાલી && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    પાછા OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    પૂર્ણાંક		try_scopes = 0;
    પૂર્ણાંક		loop_label;

    જો (compile_find_scope(&loop_label, ખાલી, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	પાછા ખાલી;
    જો (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    નહીંતર
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    પાછા arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    પૂર્ણાંક		try_scopes = 0;
    endlabel_T	**el;

    જો (compile_find_scope(ખાલી, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	પાછા ખાલી;

    જો (cctx->ctx_skip == SKIP_YES)
	પાછા arg;

    જો (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    જો (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	પાછા ખાલી;

    પાછા arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    જો (new_scope(cctx, BLOCK_SCOPE) == ખાલી)
	પાછા ખાલી;
    પાછા skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    શૂન્ય
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_ગણતરી, TRUE);
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

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    જો (try_scope == ખાલી)
	પાછા ખાલી;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	જો ((isn = generate_instr(cctx, ISN_TRY)) == ખાલી)
	    પાછા ખાલી;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	જો (isn->isn_arg.tryref.try_ref == ખાલી)
	    પાછા ખાલી;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    જો (scope == ખાલી)
	પાછા ખાલી;

    પાછા arg;
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

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    // end block scope from :try or :catch
    જો (scope != ખાલી && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    જો (scope == ખાલી || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	પાછા ખાલી;
    }

    જો (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	પાછા ખાલી;
    }
    જો (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    જો (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	જો (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	જો (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    પાછા ખાલી;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	જો (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	જો (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	જો (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    જો (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    નહીંતર
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = ખાલી;
	પૂર્ણાંક	dropped = 0;
	પૂર્ણાંક	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ખાલી);
	જો (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    પાછા ખાલી;
	}
	જો (tofree == ખાલી)
	    len = (પૂર્ણાંક)(end - (p + 1));
	નહીંતર
	    len = (પૂર્ણાંક)(end - tofree);
	pat = vim_strnsave(tofree == ખાલી ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	જો (pat == ખાલી)
	    પાછા ખાલી;
	જો (generate_PUSHS(cctx, &pat) == FAIL)
	    પાછા ખાલી;

	જો (generate_COMPARE(cctx, પદાવલિ_MATCH, FALSE) == FAIL)
	    પાછા ખાલી;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	જો (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    પાછા ખાલી;
    }

    જો (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ખાલી)
	પાછા ખાલી;

    જો (new_scope(cctx, BLOCK_SCOPE) == ખાલી)
	પાછા ખાલી;
    પાછા p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    પૂર્ણાંક		this_instr;

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    // end block scope from :try or :catch
    જો (scope != ખાલી && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    જો (scope == ખાલી || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	પાછા ખાલી;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    જો (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	જો (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    પાછા ખાલી;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    જો (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	જો (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	જો (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	જો (generate_instr(cctx, ISN_FINALLY) == ખાલી)
	    પાછા ખાલી;
    }

    પાછા arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    જો (misplaced_cmdmod(cctx))
	પાછા ખાલી;

    // end block scope from :catch or :finally
    જો (scope != ખાલી && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    જો (scope == ખાલી || scope->se_type != TRY_SCOPE)
    {
	જો (scope == ખાલી)
	    emsg(_(e_endtry_without_try));
	નહીંતર જો (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	નહીંતર જો (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	નહીંતર
	    emsg(_(e_missing_endif));
	પાછા ખાલી;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    પાછા ખાલી;
	}

#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE
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

	જો (scope->se_u.se_try.ts_catch_label != 0)
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

    જો (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	જો (generate_instr(cctx, ISN_ENDTRY) == ખાલી)
	    પાછા ખાલી;
#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    પાછા arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    જો (compile_expr0(&p, cctx) == FAIL)
	પાછા ખાલી;
    જો (cctx->ctx_skip == SKIP_YES)
	પાછા p;
    જો (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	પાછા ખાલી;
    જો (generate_instr_drop(cctx, ISN_THROW, 1) == ખાલી)
	પાછા ખાલી;

    પાછા p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    પૂર્ણાંક		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    જો (compile_expr0(&p, cctx) == FAIL)
	પાછા ખાલી;

    જો (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	પાછા ખાલી;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    પાછા skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    પૂર્ણાંક
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    જો (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	જો (lvar == ખાલી)
	    પાછા 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    પાછા dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    પૂર્ણાંક		argcount = 0;
    પૂર્ણાંક		defer_var_idx;
    type_T	*type = ખાલી;
    પૂર્ણાંક		func_idx = -1;

    જો (*arg == '(')
    {
	// a lambda function
	જો (compile_lambda(&arg, cctx, ખાલી) != OK)
	    પાછા ખાલી;
	paren = vim_strchr(arg, '(');
	જો (paren == ખાલી)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    પાછા ખાલી;
	}
    }
    નહીંતર
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	જો (paren == ખાલી)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    પાછા ખાલી;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	જો (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	નહીંતર જો (compile_expr0(&arg, cctx) == FAIL)
	    પાછા ખાલી;
	*paren = '(';
    }

    // check for function type
    જો (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ઢગલો(cctx, 0);
	જો (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    પાછા ખાલી;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    જો (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	પાછા ખાલી;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો (func_idx >= 0)
	{
	    type2_T	*argtypes = ખાલી;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    જો (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		પાછા ખાલી;
	}
	નહીંતર જો (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    પાછા ખાલી;

	defer_var_idx = get_defer_var_idx(cctx);
	જો (defer_var_idx == 0)
	    પાછા ખાલી;
	જો (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    પાછા ખાલી;
    }

    પાછા skipwhite(arg);
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
compile_mult_પદાવલિ(
	char_u	*arg,
	પૂર્ણાંક	cmdidx,
	long	cmd_ગણતરી UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*પદાવલિ_start;
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    પૂર્ણાંક		r = OK;

    માટે (;;)
    {
	જો (ends_excmd2(prev, p))
	    તોડો;
	પદાવલિ_start = p;
	જો (compile_expr0(&p, cctx) == FAIL)
	    પાછા ખાલી;

	જો (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ઢગલો(cctx, 0);
	    જો (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_કિંમત_str), પદાવલિ_start);
		પાછા ખાલી;
	    }
	}

	++ગણતરી;
	prev = p;
	p = skipwhite(p);
    }

    જો (ગણતરી > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	જો (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, ગણતરી);
	નહીંતર જો (cmdidx == CMD_execute)
	    r = generate_MULT_પદાવલિ(cctx, ISN_EXECUTE, ગણતરી);
	નહીંતર જો (cmdidx == CMD_echomsg)
	    r = generate_MULT_પદાવલિ(cctx, ISN_ECHOMSG, ગણતરી);
#ifdef HAS_MESSAGE_WINDOW
	નહીંતર જો (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, ગણતરી, cmd_ગણતરી);
#endif
	નહીંતર જો (cmdidx == CMD_echoconsole)
	    r = generate_MULT_પદાવલિ(cctx, ISN_ECHOCONSOLE, ગણતરી);
	નહીંતર
	    r = generate_MULT_પદાવલિ(cctx, ISN_ECHOERR, ગણતરી);

	cctx->ctx_lnum = save_lnum;
    }
    પાછા r == OK ? p : ખાલી;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    સ્થિર પૂર્ણાંક
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, ખાલી);
    char_u *p = skipdigits(eap->cmd);

    જો (p == range_end)
	પાછા FAIL;
    પાછા generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, પૂર્ણાંક fixindent)
{
    char_u	*લીટી = arg;
    linenr_T	lnum;
    અક્ષર	*errormsg;
    પૂર્ણાંક		above = eap->forceit;

    eap->regname = *લીટી;

    જો (eap->regname == '=')
    {
	char_u *p = skipwhite(લીટી + 1);

	જો (compile_expr0(&p, cctx) == FAIL)
	    પાછા ખાલી;
	લીટી = p;
    }
    નહીંતર જો (eap->regname != NUL)
	++લીટી;

    જો (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    નહીંતર
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	જો (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    પાછા ખાલી;
	જો (eap->addr_ગણતરી == 0)
	    lnum = -1;
	નહીંતર
	    lnum = eap->line2;
	જો (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    પાછા લીટી;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *લીટી_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*લીટી = લીટી_arg;
    char_u	*p;
    પૂર્ણાંક		has_પદાવલિ = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ખાલી;
    char_u	*cmd_arg = ખાલી;

    જો (cctx->ctx_skip == SKIP_YES)
	જાઓ theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    જો (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	જો (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    જો (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	પૂર્ણાંક	usefilter = FALSE;

	has_પદાવલિ = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	જો ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	જો ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    જો (eap->nextcmd != ખાલી)
		nextcmd = eap->nextcmd;
	}
	નહીંતર જો (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    જો (*p != NUL)
		++p;
	    જો (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    જો (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	નહીંતર જો (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    પૂર્ણાંક	    flags = 0;  // unused
	    પૂર્ણાંક	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, લીટી, &tofree, &flags);
	    જો (tofree != ખાલી)
	    {
		લીટી = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    જો (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_પદાવલિ = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	જો (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    જો ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	પૂર્ણાંક delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ખાલી, ખાલી, ખાલી);
	જો (*p == delim)
	    cmd_arg = p + 1;
    }

    જો (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    જો (cmd_arg != ખાલી)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, ખાલી, lookup_scriptitem, ખાલી);
	જો (nea.cmdidx < CMD_SIZE)
	{
	    has_પદાવલિ = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    જો (has_પદાવલિ)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    જો (has_પદાવલિ && (p = (char_u *)strstr((અક્ષર *)eap->arg, "`=")) != ખાલી)
    {
	પૂર્ણાંક	ગણતરી = 0;
	char_u	*start = skipwhite(લીટી);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	માટે (;;)
	{
	    જો (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++ગણતરી;
	    }
	    p += 2;
	    જો (compile_expr0(&p, cctx) == FAIL)
		પાછા ખાલી;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++ગણતરી;
	    p = skipwhite(p);
	    જો (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		પાછા ખાલી;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((અક્ષર *)start, "`=");
	    જો (p == ખાલી)
	    {
		જો (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++ગણતરી;
		}
		તોડો;
	    }
	}
	generate_EXECCONCAT(cctx, ગણતરી);
    }
    નહીંતર
	generate_EXEC_copy(cctx, ISN_EXEC, લીટી);

theend:
    જો (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    પાછા nextcmd;
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
compile_script(char_u *લીટી, cctx_T *cctx)
{
    જો (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	જો ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == ખાલી)
	    પાછા ખાલી;
	isn->isn_arg.string = vim_strsave(લીટી);
    }
    પાછા (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *પદાવલિ = (char_u *)strstr((અક્ષર *)cmd, "\\=");

    જો (પદાવલિ != ખાલી)
    {
	પૂર્ણાંક delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ખાલી, ખાલી, ખાલી);
	જો (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    પૂર્ણાંક		પદાવલિ_res;
	    પૂર્ણાંક		trailing_error;
	    પૂર્ણાંક		instr_ગણતરી;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = ખાલી;
	    પદાવલિ_res = compile_expr0(&cmd, cctx);
	    જો (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    જો (પદાવલિ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		જો (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		પાછા ખાલી;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_ગણતરી = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_ગણતરી].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    જો ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ખાલી)
	    {
		પૂર્ણાંક idx;

		માટે (idx = 0; idx < instr_ગણતરી; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		પાછા ખાલી;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    જો (*end == '&')
		++end;
	    જ્યારે (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    પાછા end;
	}
    }

    પાછા compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *લીટી, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    જો (lhs->lhs_name != ખાલી)
    {
	જો (STRNCMP(arg, "END", 3) == 0)
	{
	    જો (cctx->ctx_skip != SKIP_YES)
	    {
		જો (lhs->lhs_append)
		{
		    // First load the current variable value.
		    જો (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			પાછા ખાલી;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		જો (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		જો (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    જો (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			પાછા ખાલી;
		}
		નહીંતર જો (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    પાછા ખાલી;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    પાછા arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	પાછા ખાલી;
    }

    જો (arg[0] == '=' && arg[1] == '>')
    {
	પૂર્ણાંક	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	જો (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	જો (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    પાછા ખાલી;
	જો (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    પાછા ખાલી;
	જો (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	નહીંતર
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    જો (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		જો (lhs->lhs_whole == ખાલી)
		    પાછા ખાલી;
	    }
	}

	પાછા arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    પાછા compile_exec(લીટી, eap, cctx);
}

#જો defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *લીટી, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    જો (isn == ખાલી)
	પાછા ખાલી;
    isn->isn_arg.સંખ્યા = eap->cmdidx;

    p = eap->arg;
    જો (compile_expr0(&p, cctx) == FAIL)
	પાછા ખાલી;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    જો (isn == ખાલી)
	પાછા ખાલી;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    જો (isn->isn_arg.cexpr.cexpr_ref == ખાલી)
	પાછા ખાલી;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(લીટી));

    પાછા p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, પૂર્ણાંક check_return_type, પૂર્ણાંક legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ઢગલો_type;

    જો (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	જો (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_કિંમત_in_function_without_return_type));
	    પાછા ખાલી;
	}
	જો (legacy)
	{
	    પૂર્ણાંક save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    જો (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		પાછા ખાલી;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (શૂન્ય)skip_પદાવલિ(&p, ખાલી);
	    cmdmod.cmod_flags = save_flags;
	}
	નહીંતર
	{
	    // compile return argument into instructions
	    જો (compile_expr0(&p, cctx) == FAIL)
		પાછા ખાલી;
	}

	જો (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ઢગલો_type = get_type_on_ઢગલો(cctx, 0);
	    જો (check_type_is_કિંમત(ઢગલો_type) == FAIL)
		પાછા ખાલી;
	    જો ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ખાલી
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ઢગલો_type;
	    }
	    નહીંતર
	    {
		જો (need_type(ઢગલો_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    પાછા ખાલી;
	    }
	}
    }
    નહીંતર
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	જો (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_કિંમત));
	    પાછા ખાલી;
	}

	જો (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	નહીંતર
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    જો (compile_find_scope(ખાલી, ખાલી, ખાલી, ખાલી, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    જો (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ખાલી)
	પાછા ખાલી;

    // "return val | endif" is possible
    પાછા skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    પૂર્ણાંક
check_global_and_subst(char_u *cmd, char_u *arg)
{
    જો (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ખાલી)
    {
	semsg(_(e_separator_not_supported_str), arg);
	પાછા FAIL;
    }
    જો (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	પાછા FAIL;
    }
    પાછા OK;
}


#endif  // defined(FEAT_EVAL)
