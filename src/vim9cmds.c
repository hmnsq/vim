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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    স্থির পূর্ণসংখ্যা
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    পূর্ণসংখ্যা		idx = instr->ga_len;

    যখন (idx > 0)
    {
	যদি (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_তথ্য)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    চালু;
	}
#ifdef FEAT_PROFILE
	যদি (((isn_T *)instr->ga_তথ্য)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    চালু;
	}
#endif
	যদি (((isn_T *)instr->ga_তথ্য)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    চালু;
	}
	বিরতি;
    }
    ফেরত idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    স্থির শূন্য
unwind_locals(cctx_T *cctx, পূর্ণসংখ্যা new_শীর্ষ_2, পূর্ণসংখ্যা keep)
{
    যদি (cctx->ctx_locals.ga_len > new_শীর্ষ_2)
	জন্য (পূর্ণসংখ্যা idx = new_শীর্ষ_2; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_তথ্য) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    যদি (!keep)
	cctx->ctx_locals.ga_len = new_শীর্ষ_2;
}

/*
 * Free all local variables.
 */
    শূন্য
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    পূর্ণসংখ্যা
check_vim9_unlet(char_u *name)
{
    যদি (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	ফেরত FAIL;
    }

    যদি (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ফাঁকা)
    {
	// "unlet s:var" is allowed in legacy script.
	যদি (*name == 's' && !in_vim9script())
	    ফেরত OK;
	semsg(_(e_cannot_unlet_str), name);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    স্থির পূর্ণসংখ্যা
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    পূর্ণসংখ্যা	    deep UNUSED,
    শূন্য    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    পূর্ণসংখ্যা		cc = *name_end;
    পূর্ণসংখ্যা		ret = OK;

    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত OK;

    *name_end = NUL;
    যদি (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    অন্যথা যদি (vim_strchr(p, '.') != ফাঁকা || vim_strchr(p, '[') != ফাঁকা)
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
	যদি (ret == OK)
	{
	    যদি (!lhs.lhs_has_সূচক)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    অন্যথা
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    অন্যথা যদি (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    অন্যথা
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    ফেরত ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    স্থির পূর্ণসংখ্যা
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    পূর্ণসংখ্যা	    deep,
    শূন্য    *coookie)
{
    cctx_T	*cctx = coookie;
    পূর্ণসংখ্যা		cc = *name_end;
    char_u	*p = lvp->ll_name;
    পূর্ণসংখ্যা		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    অক্ষর	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    পূর্ণসংখ্যা		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(ফাঁকা, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত OK;

    যদি (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	ফেরত FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    যদি (p[1] != ':')
    {
	char_u *end = find_name_end(p, ফাঁকা, ফাঁকা, FNE_CHECK_START);

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

	char_u	*name = ফাঁকা;
	পূর্ণসংখ্যা	len = end - p;

	যদি (lookup_local(p, len, ফাঁকা, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    যদি (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		ফেরত FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(ফাঁকা, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	যদি (name == ফাঁকা)
	{
	    class_T *cl;
	    যদি (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		যদি (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (পূর্ণসংখ্যা)cl->class_name.দৈর্ঘ্য;
#ifdef LOG_LOCKVAR
		    ch_log(ফাঁকা, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	যদি (name == ফাঁকা)
	{
	    // Can lockvar any function arg.
	    যদি (arg_exists(p, len, ফাঁকা, ফাঁকা, ফাঁকা, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(ফাঁকা, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	যদি (name == ফাঁকা)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    যদি (script_var_exists(p, len, cctx, ফাঁকা) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(ফাঁকা, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	যদি (name != ফাঁকা)
	{
#ifdef LOG_LOCKVAR
	    ch_log(ফাঁকা, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    যদি (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		ফেরত FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    যদি (buf == ফাঁকা)
	ret = FAIL;
    অন্যথা
    {
	যদি (deep < 0)
	    vim_snprintf((অক্ষর *)buf, len, "%s! %s", cmd, p);
	অন্যথা
	    vim_snprintf((অক্ষর *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(ফাঁকা, "LKVAR:    ... buf %s", buf);
#endif
	যদি (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	অন্যথা
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    ফেরত ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    পূর্ণসংখ্যা	    deep = 0;
    char_u  *p = arg;

    যদি (eap->cmdidx != CMD_unlet)
    {
	যদি (eap->forceit)
	    deep = -1;
	অন্যথা যদি (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	অন্যথা
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    ফেরত eap->nextcmd == ফাঁকা ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    স্থির পূর্ণসংখ্যা
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	পূর্ণসংখ্যা	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    যদি (endlabel == ফাঁকা)
	ফেরত FAIL;
    endlabel->el_পরবর্তী = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    যদি (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    অন্যথা
	generate_JUMP(cctx, when, 0);
    ফেরত OK;
}

    স্থির শূন্য
compile_fill_jump_to_end(endlabel_T **el, পূর্ণসংখ্যা jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    যখন (*el != ফাঁকা)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_তথ্য) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_পরবর্তী;
	vim_free(cur);
    }
}

    স্থির শূন্য
compile_free_jump_to_end(endlabel_T **el)
{
    যখন (*el != ফাঁকা)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_পরবর্তী;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    স্থির scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    যদি (scope == ফাঁকা)
	ফেরত ফাঁকা;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_গুনতি = cctx->ctx_locals.ga_len;
    যদি (scope->se_outer != ফাঁকা)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    ফেরত scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    শূন্য
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    যদি (scope == ফাঁকা)
    {
	iemsg("calling drop_scope() without a scope");
	ফেরত;
    }
    cctx->ctx_scope = scope->se_outer;
    নির্বাচন (scope->se_type)
    {
	ক্ষেত্র IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); বিরতি;
	ক্ষেত্র FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); বিরতি;
	ক্ষেত্র WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); বিরতি;
	ক্ষেত্র TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); বিরতি;
	ক্ষেত্র NO_SCOPE:
	ক্ষেত্র BLOCK_SCOPE:
	    বিরতি;
    }
    vim_free(scope);
}

    স্থির পূর্ণসংখ্যা
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    যদি (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	ফেরত TRUE;
    }
    ফেরত FALSE;
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
    পূর্ণসংখ্যা		instr_গুনতি = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    যদি (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ফেরত ফাঁকা;
    }
    যদি (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ফেরত ফাঁকা;
    }
    যদি (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    অন্যথা যদি (instr->ga_len == instr_গুনতি && ppconst.pp_used == 1)
    {
	পূর্ণসংখ্যা error = FALSE;
	পূর্ণসংখ্যা v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	যদি (error)
	    ফেরত ফাঁকা;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    অন্যথা
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	যদি (generate_ppconst(cctx, &ppconst) == FAIL)
	    ফেরত ফাঁকা;
	যদি (bool_on_স্তূপ(cctx) == FAIL)
	    ফেরত ফাঁকা;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    যদি (scope == ফাঁকা)
	ফেরত ফাঁকা;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    যদি (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    অন্যথা
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    যদি (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    ফেরত p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    পূর্ণসংখ্যা		instr_গুনতি;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    যদি (scope == ফাঁকা || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	ফেরত ফাঁকা;
    }
    যদি (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	ফেরত ফাঁকা;
    }
    unwind_locals(cctx, scope->se_local_গুনতি, TRUE);
    যদি (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    যদি (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    যদি (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_রাশিমালা_cctx(&p, cctx);
	ফেরত p;
    }

    যদি (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_রাশিমালা_cctx(&p, cctx);
	ফেরত p;
    }

    যদি (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	পূর্ণসংখ্যা	    moved_cmdmod = FALSE;
	পূর্ণসংখ্যা	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	যদি (((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    যদি (GA_GROW_FAILS(instr, 1))
		ফেরত ফাঁকা;
	    ((isn_T *)instr->ga_তথ্য)[instr->ga_len] =
				  ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	যদি (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_তথ্য)[instr->ga_len];
	    saved_debug = TRUE;
	}

	যদি (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ফেরত ফাঁকা;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	যদি (moved_cmdmod)
	    ++instr->ga_len;

	যদি (saved_debug)
	{
	    // move the debug instruction here
	    যদি (GA_GROW_FAILS(instr, 1))
		ফেরত ফাঁকা;
	    ((isn_T *)instr->ga_তথ্য)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    যদি (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	যদি (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_গুনতি = instr->ga_len;
    যদি (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ফেরত ফাঁকা;
    }
    cctx->ctx_skip = save_skip;
    যদি (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	ফেরত ফাঁকা;
    }
    যদি (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    অন্যথা যদি (instr->ga_len == instr_গুনতি && ppconst.pp_used == 1)
    {
	পূর্ণসংখ্যা error = FALSE;
	পূর্ণসংখ্যা v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	যদি (error)
	{
	    clear_ppconst(&ppconst);
	    ফেরত ফাঁকা;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    অন্যথা
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	যদি (generate_ppconst(cctx, &ppconst) == FAIL)
	    ফেরত ফাঁকা;
	যদি (bool_on_স্তূপ(cctx) == FAIL)
	    ফেরত ফাঁকা;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    ফেরত p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    যদি (scope == ফাঁকা || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	ফেরত ফাঁকা;
    }
    যদি (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	ফেরত ফাঁকা;
    }
    unwind_locals(cctx, scope->se_local_গুনতি, TRUE);
    যদি (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    যদি (cctx->ctx_compile_type == CT_PROFILE)
    {
	যদি (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	যদি (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    যদি (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	যদি (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    যদি (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		ফেরত ফাঁকা;
	}

	যদি (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    যদি (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	যদি (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    ফেরত p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    যদি (scope == ফাঁকা || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	ফেরত ফাঁকা;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_গুনতি, TRUE);
    যদি (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    যদি (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_to_end(&ifscope->is_end_label, instr->ga_len, cctx);

#ifdef FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    যদি (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    ফেরত arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    স্থির শূন্য
compile_fill_loop_info(loop_info_T *loop_info, পূর্ণসংখ্যা funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_গুনতি = cctx->ctx_locals.ga_len;
    loop_info->li_closure_গুনতি = cctx->ctx_closure_গুনতি;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    স্থির type_T *
compile_for_tuple_get_vartype(type_T *vartype, পূর্ণসংখ্যা var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    যদি (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	ফেরত &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    যদি (member_type->tt_type == VAR_ANY)
	ফেরত &t_any;

    যদি (!var_list)
	// for x in tuple<...list<xxx>>
	ফেরত member_type;

    যদি (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	ফেরত member_type->tt_member;
    অন্যথা যদি (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	ফেরত member_type->tt_args[0]->tt_member;

    ফেরত &t_any;
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
    char_u	*name = ফাঁকা;
    char_u	*p;
    char_u	*wp;
    পূর্ণসংখ্যা		var_গুনতি = 0;
    পূর্ণসংখ্যা		var_list = FALSE;
    পূর্ণসংখ্যা		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    পূর্ণসংখ্যা		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    পূর্ণসংখ্যা		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    পূর্ণসংখ্যা		idx;
    পূর্ণসংখ্যা		পূর্ব_lnum = cctx->ctx_পূর্ব_lnum;

    p = skip_var_list(arg_start, TRUE, &var_গুনতি, &semicolon, FALSE);
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (var_গুনতি == 0)
	var_গুনতি = 1;
    অন্যথা
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    যদি (may_get_পরবর্তী_সারি_error(wp, &p, cctx) == FAIL)
	ফেরত ফাঁকা;
    যদি (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	যদি (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	অন্যথা
	    emsg(_(e_missing_in_after_for));
	ফেরত ফাঁকা;
    }
    wp = p + 2;
    যদি (may_get_পরবর্তী_সারি_error(wp, &p, cctx) == FAIL)
	ফেরত ফাঁকা;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    যদি (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	পূর্ব_lnum = ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    যদি (scope == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ফেরত ফাঁকা;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_সংখ্যা);
    যদি (loop_lvar == ফাঁকা)
    {
	drop_scope(cctx);
	ফেরত ফাঁকা;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_সংখ্যা);
    যদি (funcref_lvar == ফাঁকা)
    {
	drop_scope(cctx);
	ফেরত ফাঁকা;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    যদি (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	ফেরত ফাঁকা;
    }
    arg_end = arg;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_স্তূপ(cctx, 0);
	যদি (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    ফেরত ফাঁকা;
	}

	যদি (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	অন্যথা যদি (vartype->tt_type == VAR_BLOB)
	    item_type = &t_সংখ্যা;
	অন্যথা যদি (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    যদি (!var_list)
		item_type = vartype->tt_member;
	    অন্যথা যদি (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	অন্যথা যদি (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_শীর্ষ_label = current_instr_idx(cctx);

	যদি (cctx->ctx_compile_type == CT_DEBUG)
	{
	    পূর্ণসংখ্যা		save_পূর্ব_lnum = cctx->ctx_পূর্ব_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_পূর্ব_lnum = পূর্ব_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_names_len;
	    cctx->ctx_পূর্ব_lnum = save_পূর্ব_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_start;
	যদি (var_list)
	{
	    generate_UNPACK(cctx, var_গুনতি, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_স্তূপ.ga_len;

	    // add type of the items
	    জন্য (idx = 0; idx < var_গুনতি; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		যদি (ঠেলো_type_স্তূপ(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    ফেরত ফাঁকা;
		}
	    }
	}

	জন্য (idx = 0; idx < var_গুনতি; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    পূর্ণসংখ্যা			opt_flags = 0;
	    পূর্ণসংখ্যা			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    যদি (name == ফাঁকা)
		যাও failed;
	    যদি (*skipwhite(p) == ':')
	    {
		যদি (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    যাও failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		যদি (lhs_type == ফাঁকা || !valid_declaration_type(lhs_type))
		    যাও failed;
	    }

	    যদি (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		যাও failed;
	    যদি (dest != dest_local)
	    {
		যদি (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ফাঁকা) == FAIL)
		    যাও failed;
	    }
	    অন্যথা যদি (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		যদি (generate_instr_drop(cctx, ISN_DROP, 1) == ফাঁকা)
		    যাও failed;
	    }
	    অন্যথা
	    {
		// Script var is not supported.
		যদি (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    যাও failed;
		}

		যদি (!valid_varname(arg, (পূর্ণসংখ্যা)varlen, FALSE))
		    যাও failed;
		যদি (lookup_local(arg, varlen, ফাঁকা, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    যাও failed;
		}

		// Reserve a variable to store "var".
		যদি (var_list)
		{
		    where.wt_সূচক = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		যদি (lhs_type == &t_any)
		    lhs_type = item_type;
		অন্যথা যদি (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    যাও failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		যদি (var_lvar == ফাঁকা)
		    // out of memory or used as an argument
		    যাও failed;

		যদি (semicolon && idx == var_গুনতি - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ফাঁকা);
	    }

	    যদি (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    ফেরত arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    ফেরত ফাঁকা;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    স্থির পূর্ণসংখ্যা
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    যদি (cctx->ctx_locals.ga_len > loop_info->li_local_গুনতি
	    && cctx->ctx_closure_গুনতি > loop_info->li_closure_গুনতি)
	ফেরত generate_ENDLOOP(cctx, loop_info);
    ফেরত OK;
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

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    যদি (scope == ফাঁকা || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	ফেরত ফাঁকা;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    যদি (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	যদি (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    ফেরত ফাঁকা;

	unwind_locals(cctx, scope->se_local_গুনতি, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_শীর্ষ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_তথ্য) + forscope->fs_শীর্ষ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	যদি (generate_instr_drop(cctx, ISN_DROP, 1) == ফাঁকা)
	    ফেরত ফাঁকা;
    }

    vim_free(scope);

    ফেরত arg;
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
    পূর্ণসংখ্যা		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    যদি (scope == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ফেরত ফাঁকা;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_শীর্ষ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_সংখ্যা);
    যদি (funcref_lvar == ফাঁকা)
    {
	drop_scope(cctx);
	ফেরত ফাঁকা;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    যদি (compile_expr0(&p, cctx) == FAIL)
	ফেরত ফাঁকা;

    যদি (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ফেরত ফাঁকা;
    }

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি (bool_on_স্তূপ(cctx) == FAIL)
	    ফেরত FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	যদি (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    ফেরত FAIL;
    }

    ফেরত p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;
    যদি (scope == ফাঁকা || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	ফেরত ফাঁকা;
    }
    cctx->ctx_scope = scope->se_outer;
    যদি (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	যদি (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    ফেরত ফাঁকা;

	unwind_locals(cctx, scope->se_local_গুনতি, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_শীর্ষ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    ফেরত arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    পূর্ণসংখ্যা
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    পূর্ণসংখ্যা		পূর্ব_local_গুনতি = 0;

    CLEAR_POINTER(lvi);
    জন্য (;;)
    {
	loop_info_T	*loopinfo;
	পূর্ণসংখ্যা		cur_local_last;
	পূর্ণসংখ্যা		start_local_গুনতি;

	যখন (scope != ফাঁকা && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	যদি (scope == ফাঁকা)
	    বিরতি;

	যদি (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_গুনতি - 1;
	}
	অন্যথা
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_গুনতি - 3;
	}

	start_local_গুনতি = loopinfo->li_local_গুনতি;
	যদি (cctx->ctx_locals.ga_len > start_local_গুনতি)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_গুনতি;
	    lvi->lvi_loop[loopinfo->li_depth].var_গুনতি =
			  (short)(cctx->ctx_locals.ga_len - start_local_গুনতি
							   - পূর্ব_local_গুনতি);
	    যদি (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	পূর্ব_local_গুনতি = cctx->ctx_locals.ga_len - cur_local_last;
    }
    ফেরত lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    শূন্য
get_loop_var_idx(cctx_T *cctx, পূর্ণসংখ্যা idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    যদি (get_loop_var_info(cctx, &lvi))
    {
	পূর্ণসংখ্যা depth;

	জন্য (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    যদি (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_গুনতি)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		ফেরত;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    স্থির পূর্ণসংখ্যা
compile_find_scope(
	পূর্ণসংখ্যা	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	পূর্ণসংখ্যা	    *try_scopes,    // :try scopes encountered or NULL
	অক্ষর	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    জন্য (;;)
    {
	যদি (scope == ফাঁকা)
	{
	    যদি (error != ফাঁকা)
		emsg(_(error));
	    ফেরত FAIL;
	}
	যদি (scope->se_type == FOR_SCOPE)
	{
	    যদি (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		ফেরত FAIL;
	    যদি (loop_label != ফাঁকা)
		*loop_label = scope->se_u.se_for.fs_শীর্ষ_label;
	    যদি (el != ফাঁকা)
		*el = &scope->se_u.se_for.fs_end_label;
	    বিরতি;
	}
	যদি (scope->se_type == WHILE_SCOPE)
	{
	    যদি (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		ফেরত FAIL;
	    যদি (loop_label != ফাঁকা)
		*loop_label = scope->se_u.se_while.ws_শীর্ষ_label;
	    যদি (el != ফাঁকা)
		*el = &scope->se_u.se_while.ws_end_label;
	    বিরতি;
	}
	যদি (try_scopes != ফাঁকা && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    ফেরত OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    পূর্ণসংখ্যা		try_scopes = 0;
    পূর্ণসংখ্যা		loop_label;

    যদি (compile_find_scope(&loop_label, ফাঁকা, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	ফেরত ফাঁকা;
    যদি (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    অন্যথা
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    ফেরত arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    পূর্ণসংখ্যা		try_scopes = 0;
    endlabel_T	**el;

    যদি (compile_find_scope(ফাঁকা, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	ফেরত ফাঁকা;

    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত arg;

    যদি (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    যদি (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	ফেরত ফাঁকা;

    ফেরত arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    যদি (new_scope(cctx, BLOCK_SCOPE) == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    শূন্য
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_গুনতি, TRUE);
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

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    যদি (try_scope == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	যদি ((isn = generate_instr(cctx, ISN_TRY)) == ফাঁকা)
	    ফেরত ফাঁকা;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	যদি (isn->isn_arg.tryref.try_ref == ফাঁকা)
	    ফেরত ফাঁকা;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    যদি (scope == ফাঁকা)
	ফেরত ফাঁকা;

    ফেরত arg;
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

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    // end block scope from :try or :catch
    যদি (scope != ফাঁকা && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    যদি (scope == ফাঁকা || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	ফেরত ফাঁকা;
    }

    যদি (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	ফেরত ফাঁকা;
    }
    যদি (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	যদি (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	যদি (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ফেরত ফাঁকা;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_try.ts_try_label;
	যদি (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	যদি (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	যদি (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    যদি (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    অন্যথা
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = ফাঁকা;
	পূর্ণসংখ্যা	dropped = 0;
	পূর্ণসংখ্যা	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ফাঁকা);
	যদি (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    ফেরত ফাঁকা;
	}
	যদি (tofree == ফাঁকা)
	    len = (পূর্ণসংখ্যা)(end - (p + 1));
	অন্যথা
	    len = (পূর্ণসংখ্যা)(end - tofree);
	pat = vim_strnsave(tofree == ফাঁকা ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	যদি (pat == ফাঁকা)
	    ফেরত ফাঁকা;
	যদি (generate_PUSHS(cctx, &pat) == FAIL)
	    ফেরত ফাঁকা;

	যদি (generate_COMPARE(cctx, রাশিমালা_MATCH, FALSE) == FAIL)
	    ফেরত ফাঁকা;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	যদি (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    ফেরত ফাঁকা;
    }

    যদি (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (new_scope(cctx, BLOCK_SCOPE) == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    পূর্ণসংখ্যা		this_instr;

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    // end block scope from :try or :catch
    যদি (scope != ফাঁকা && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    যদি (scope == ফাঁকা || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	ফেরত ফাঁকা;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    যদি (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_try.ts_try_label;
	যদি (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    ফেরত ফাঁকা;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_তথ্য)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    যদি (this_instr > 0 && ((isn_T *)instr->ga_তথ্য)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	যদি (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	যদি (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	যদি (generate_instr(cctx, ISN_FINALLY) == ফাঁকা)
	    ফেরত ফাঁকা;
    }

    ফেরত arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    যদি (misplaced_cmdmod(cctx))
	ফেরত ফাঁকা;

    // end block scope from :catch or :finally
    যদি (scope != ফাঁকা && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    যদি (scope == ফাঁকা || scope->se_type != TRY_SCOPE)
    {
	যদি (scope == ফাঁকা)
	    emsg(_(e_endtry_without_try));
	অন্যথা যদি (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	অন্যথা যদি (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	অন্যথা
	    emsg(_(e_missing_endif));
	ফেরত ফাঁকা;
    }

    try_isn = ((isn_T *)instr->ga_তথ্য) + scope->se_u.se_try.ts_try_label;
    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    ফেরত ফাঁকা;
	}

#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_তথ্য)[instr->ga_len - 1]
						.isn_type == ISN_PROF_START)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#endif

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							  instr->ga_len, cctx);

	যদি (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_তথ্য)
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

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	যদি (generate_instr(cctx, ISN_ENDTRY) == ফাঁকা)
	    ফেরত ফাঁকা;
#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    ফেরত arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    যদি (compile_expr0(&p, cctx) == FAIL)
	ফেরত ফাঁকা;
    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত p;
    যদি (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	ফেরত ফাঁকা;
    যদি (generate_instr_drop(cctx, ISN_THROW, 1) == ফাঁকা)
	ফেরত ফাঁকা;

    ফেরত p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    পূর্ণসংখ্যা		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    যদি (compile_expr0(&p, cctx) == FAIL)
	ফেরত ফাঁকা;

    যদি (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	ফেরত ফাঁকা;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    ফেরত skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    পূর্ণসংখ্যা
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    যদি (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	যদি (lvar == ফাঁকা)
	    ফেরত 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    ফেরত dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    পূর্ণসংখ্যা		argcount = 0;
    পূর্ণসংখ্যা		defer_var_idx;
    type_T	*type = ফাঁকা;
    পূর্ণসংখ্যা		func_idx = -1;

    যদি (*arg == '(')
    {
	// a lambda function
	যদি (compile_lambda(&arg, cctx, ফাঁকা) != OK)
	    ফেরত ফাঁকা;
	paren = vim_strchr(arg, '(');
	যদি (paren == ফাঁকা)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	যদি (paren == ফাঁকা)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ফেরত ফাঁকা;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	যদি (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	অন্যথা যদি (compile_expr0(&arg, cctx) == FAIL)
	    ফেরত ফাঁকা;
	*paren = '(';
    }

    // check for function type
    যদি (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_স্তূপ(cctx, 0);
	যদি (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    ফেরত ফাঁকা;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    যদি (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	ফেরত ফাঁকা;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি (func_idx >= 0)
	{
	    type2_T	*argtypes = ফাঁকা;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    যদি (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		ফেরত ফাঁকা;
	}
	অন্যথা যদি (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    ফেরত ফাঁকা;

	defer_var_idx = get_defer_var_idx(cctx);
	যদি (defer_var_idx == 0)
	    ফেরত ফাঁকা;
	যদি (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    ফেরত ফাঁকা;
    }

    ফেরত skipwhite(arg);
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
compile_mult_রাশিমালা(
	char_u	*arg,
	পূর্ণসংখ্যা	cmdidx,
	long	cmd_গুনতি UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*পূর্ব = arg;
    char_u	*রাশিমালা_start;
    পূর্ণসংখ্যা		গুনতি = 0;
    পূর্ণসংখ্যা		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    পূর্ণসংখ্যা		r = OK;

    জন্য (;;)
    {
	যদি (ends_excmd2(পূর্ব, p))
	    বিরতি;
	রাশিমালা_start = p;
	যদি (compile_expr0(&p, cctx) == FAIL)
	    ফেরত ফাঁকা;

	যদি (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_স্তূপ(cctx, 0);
	    যদি (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_ফলাফল_in_মান_str), রাশিমালা_start);
		ফেরত ফাঁকা;
	    }
	}

	++গুনতি;
	পূর্ব = p;
	p = skipwhite(p);
    }

    যদি (গুনতি > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	যদি (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, গুনতি);
	অন্যথা যদি (cmdidx == CMD_execute)
	    r = generate_MULT_রাশিমালা(cctx, ISN_EXECUTE, গুনতি);
	অন্যথা যদি (cmdidx == CMD_echomsg)
	    r = generate_MULT_রাশিমালা(cctx, ISN_ECHOMSG, গুনতি);
#ifdef HAS_MESSAGE_WINDOW
	অন্যথা যদি (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, গুনতি, cmd_গুনতি);
#endif
	অন্যথা যদি (cmdidx == CMD_echoconsole)
	    r = generate_MULT_রাশিমালা(cctx, ISN_ECHOCONSOLE, গুনতি);
	অন্যথা
	    r = generate_MULT_রাশিমালা(cctx, ISN_ECHOERR, গুনতি);

	cctx->ctx_lnum = save_lnum;
    }
    ফেরত r == OK ? p : ফাঁকা;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    স্থির পূর্ণসংখ্যা
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, ফাঁকা);
    char_u *p = skipdigits(eap->cmd);

    যদি (p == range_end)
	ফেরত FAIL;
    ফেরত generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, পূর্ণসংখ্যা fixindent)
{
    char_u	*সারি = arg;
    linenr_T	lnum;
    অক্ষর	*errormsg;
    পূর্ণসংখ্যা		above = eap->forceit;

    eap->regname = *সারি;

    যদি (eap->regname == '=')
    {
	char_u *p = skipwhite(সারি + 1);

	যদি (compile_expr0(&p, cctx) == FAIL)
	    ফেরত ফাঁকা;
	সারি = p;
    }
    অন্যথা যদি (eap->regname != NUL)
	++সারি;

    যদি (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    অন্যথা
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	যদি (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    ফেরত ফাঁকা;
	যদি (eap->addr_গুনতি == 0)
	    lnum = -1;
	অন্যথা
	    lnum = eap->line2;
	যদি (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    ফেরত সারি;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *সারি_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*সারি = সারি_arg;
    char_u	*p;
    পূর্ণসংখ্যা		has_রাশিমালা = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ফাঁকা;
    char_u	*cmd_arg = ফাঁকা;

    যদি (cctx->ctx_skip == SKIP_YES)
	যাও theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    যদি (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_তথ্য) + instr->ga_len - 1;

	যদি (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    যদি (eap->cmdidx >= 0 && eap->cmdidx < CMD_আকার)
    {
	long	argt = eap->argt;
	পূর্ণসংখ্যা	usefilter = FALSE;

	has_রাশিমালা = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	যদি ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	যদি ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    যদি (eap->nextcmd != ফাঁকা)
		nextcmd = eap->nextcmd;
	}
	অন্যথা যদি (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    যদি (*p != NUL)
		++p;
	    যদি (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    যদি (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	অন্যথা যদি (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    পূর্ণসংখ্যা	    flags = 0;  // unused
	    পূর্ণসংখ্যা	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, সারি, &tofree, &flags);
	    যদি (tofree != ফাঁকা)
	    {
		সারি = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    যদি (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_রাশিমালা = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	যদি (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    যদি ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	পূর্ণসংখ্যা delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ফাঁকা, ফাঁকা, ফাঁকা);
	যদি (*p == delim)
	    cmd_arg = p + 1;
    }

    যদি (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    যদি (cmd_arg != ফাঁকা)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, ফাঁকা, lookup_scriptitem, ফাঁকা);
	যদি (nea.cmdidx < CMD_আকার)
	{
	    has_রাশিমালা = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    যদি (has_রাশিমালা)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    যদি (has_রাশিমালা && (p = (char_u *)strstr((অক্ষর *)eap->arg, "`=")) != ফাঁকা)
    {
	পূর্ণসংখ্যা	গুনতি = 0;
	char_u	*start = skipwhite(সারি);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	জন্য (;;)
	{
	    যদি (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++গুনতি;
	    }
	    p += 2;
	    যদি (compile_expr0(&p, cctx) == FAIL)
		ফেরত ফাঁকা;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++গুনতি;
	    p = skipwhite(p);
	    যদি (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		ফেরত ফাঁকা;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((অক্ষর *)start, "`=");
	    যদি (p == ফাঁকা)
	    {
		যদি (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++গুনতি;
		}
		বিরতি;
	    }
	}
	generate_EXECCONCAT(cctx, গুনতি);
    }
    অন্যথা
	generate_EXEC_copy(cctx, ISN_EXEC, সারি);

theend:
    যদি (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    ফেরত nextcmd;
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
compile_script(char_u *সারি, cctx_T *cctx)
{
    যদি (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	যদি ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == ফাঁকা)
	    ফেরত ফাঁকা;
	isn->isn_arg.string = vim_strsave(সারি);
    }
    ফেরত (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *রাশিমালা = (char_u *)strstr((অক্ষর *)cmd, "\\=");

    যদি (রাশিমালা != ফাঁকা)
    {
	পূর্ণসংখ্যা delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ফাঁকা, ফাঁকা, ফাঁকা);
	যদি (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    পূর্ণসংখ্যা		রাশিমালা_res;
	    পূর্ণসংখ্যা		trailing_error;
	    পূর্ণসংখ্যা		instr_গুনতি;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_তথ্য = ফাঁকা;
	    রাশিমালা_res = compile_expr0(&cmd, cctx);
	    যদি (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    যদি (রাশিমালা_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		যদি (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		ফেরত ফাঁকা;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_গুনতি = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_তথ্য;
	    instr[instr_গুনতি].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    যদি ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ফাঁকা)
	    {
		পূর্ণসংখ্যা idx;

		জন্য (idx = 0; idx < instr_গুনতি; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		ফেরত ফাঁকা;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    যদি (*end == '&')
		++end;
	    যখন (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    ফেরত end;
	}
    }

    ফেরত compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *সারি, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    যদি (lhs->lhs_name != ফাঁকা)
    {
	যদি (STRNCMP(arg, "END", 3) == 0)
	{
	    যদি (cctx->ctx_skip != SKIP_YES)
	    {
		যদি (lhs->lhs_append)
		{
		    // First load the current variable value.
		    যদি (compile_load_lhs_with_সূচক(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			ফেরত ফাঁকা;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		যদি (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		যদি (lhs->lhs_has_সূচক)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    যদি (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			ফেরত ফাঁকা;
		}
		অন্যথা যদি (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    ফেরত ফাঁকা;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    ফেরত arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	ফেরত ফাঁকা;
    }

    যদি (arg[0] == '=' && arg[1] == '>')
    {
	পূর্ণসংখ্যা	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	যদি (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	যদি (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    ফেরত ফাঁকা;
	যদি (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ফেরত ফাঁকা;
	যদি (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	অন্যথা
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    যদি (lhs->lhs_has_সূচক)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_যোগ);
		যদি (lhs->lhs_whole == ফাঁকা)
		    ফেরত ফাঁকা;
	    }
	}

	ফেরত arg + lhs->lhs_varlen_যোগ;
    }

    // other redirects are handled like at script level
    ফেরত compile_exec(সারি, eap, cctx);
}

#যদি defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *সারি, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    যদি (isn == ফাঁকা)
	ফেরত ফাঁকা;
    isn->isn_arg.সংখ্যা = eap->cmdidx;

    p = eap->arg;
    যদি (compile_expr0(&p, cctx) == FAIL)
	ফেরত ফাঁকা;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    যদি (isn == ফাঁকা)
	ফেরত ফাঁকা;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    যদি (isn->isn_arg.cexpr.cexpr_ref == ফাঁকা)
	ফেরত ফাঁকা;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(সারি));

    ফেরত p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, পূর্ণসংখ্যা check_return_type, পূর্ণসংখ্যা legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*স্তূপ_type;

    যদি (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	যদি (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_মান_in_function_without_return_type));
	    ফেরত ফাঁকা;
	}
	যদি (legacy)
	{
	    পূর্ণসংখ্যা save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    যদি (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		ফেরত ফাঁকা;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (শূন্য)skip_রাশিমালা(&p, ফাঁকা);
	    cmdmod.cmod_flags = save_flags;
	}
	অন্যথা
	{
	    // compile return argument into instructions
	    যদি (compile_expr0(&p, cctx) == FAIL)
		ফেরত ফাঁকা;
	}

	যদি (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    স্তূপ_type = get_type_on_স্তূপ(cctx, 0);
	    যদি (check_type_is_মান(স্তূপ_type) == FAIL)
		ফেরত ফাঁকা;
	    যদি ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ফাঁকা
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = স্তূপ_type;
	    }
	    অন্যথা
	    {
		যদি (need_type(স্তূপ_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    ফেরত ফাঁকা;
	    }
	}
    }
    অন্যথা
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	যদি (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_মান));
	    ফেরত ফাঁকা;
	}

	যদি (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	অন্যথা
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    যদি (compile_find_scope(ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    যদি (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ফাঁকা)
	ফেরত ফাঁকা;

    // "return val | endif" is possible
    ফেরত skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    পূর্ণসংখ্যা
check_global_and_subst(char_u *cmd, char_u *arg)
{
    যদি (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ফাঁকা)
    {
	semsg(_(e_separator_not_supported_str), arg);
	ফেরত FAIL;
    }
    যদি (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	ফেরত FAIL;
    }
    ফেরত OK;
}


#endif  // defined(FEAT_EVAL)
