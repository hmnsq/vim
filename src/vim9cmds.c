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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ایستا صحیح
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		idx = instr->ga_len;

    وقتی (idx > 0)
    {
	اگر (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    ادامه;
	}
#ifdef FEAT_PROFILE
	اگر (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    ادامه;
	}
#endif
	اگر (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    ادامه;
	}
	بشکن;
    }
    بازگشت idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ایستا تهی
unwind_locals(cctx_T *cctx, صحیح new_بالاترین, صحیح keep)
{
    اگر (cctx->ctx_locals.ga_len > new_بالاترین)
	برای (صحیح idx = new_بالاترین; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    اگر (!keep)
	cctx->ctx_locals.ga_len = new_بالاترین;
}

/*
 * Free all local variables.
 */
    تهی
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    صحیح
check_vim9_unlet(char_u *name)
{
    اگر (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	بازگشت FAIL;
    }

    اگر (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == پوچ)
    {
	// "unlet s:var" is allowed in legacy script.
	اگر (*name == 's' && !in_vim9script())
	    بازگشت OK;
	semsg(_(e_cannot_unlet_str), name);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ایستا صحیح
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    صحیح	    deep UNUSED,
    تهی    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    صحیح		cc = *name_end;
    صحیح		ret = OK;

    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت OK;

    *name_end = NUL;
    اگر (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    وگرنه اگر (vim_strchr(p, '.') != پوچ || vim_strchr(p, '[') != پوچ)
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
	اگر (ret == OK)
	{
	    اگر (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    وگرنه
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    وگرنه اگر (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    وگرنه
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    بازگشت ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ایستا صحیح
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    صحیح	    deep,
    تهی    *coookie)
{
    cctx_T	*cctx = coookie;
    صحیح		cc = *name_end;
    char_u	*p = lvp->ll_name;
    صحیح		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    نویسه	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    صحیح		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت OK;

    اگر (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	بازگشت FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    اگر (p[1] != ':')
    {
	char_u *end = find_name_end(p, پوچ, پوچ, FNE_CHECK_START);

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

	char_u	*name = پوچ;
	صحیح	len = end - p;

	اگر (lookup_local(p, len, پوچ, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    اگر (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		بازگشت FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(پوچ, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	اگر (name == پوچ)
	{
	    class_T *cl;
	    اگر (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		اگر (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (صحیح)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(پوچ, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	اگر (name == پوچ)
	{
	    // Can lockvar any function arg.
	    اگر (arg_exists(p, len, پوچ, پوچ, پوچ, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	اگر (name == پوچ)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    اگر (script_var_exists(p, len, cctx, پوچ) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	اگر (name != پوچ)
	{
#ifdef LOG_LOCKVAR
	    ch_log(پوچ, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    اگر (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    اگر (buf == پوچ)
	ret = FAIL;
    وگرنه
    {
	اگر (deep < 0)
	    vim_snprintf((نویسه *)buf, len, "%s! %s", cmd, p);
	وگرنه
	    vim_snprintf((نویسه *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(پوچ, "LKVAR:    ... buf %s", buf);
#endif
	اگر (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	وگرنه
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    بازگشت ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    صحیح	    deep = 0;
    char_u  *p = arg;

    اگر (eap->cmdidx != CMD_unlet)
    {
	اگر (eap->forceit)
	    deep = -1;
	وگرنه اگر (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	وگرنه
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    بازگشت eap->nextcmd == پوچ ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ایستا صحیح
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	صحیح	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    اگر (endlabel == پوچ)
	بازگشت FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    اگر (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    وگرنه
	generate_JUMP(cctx, when, 0);
    بازگشت OK;
}

    ایستا تهی
compile_fill_jump_to_end(endlabel_T **el, صحیح jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    وقتی (*el != پوچ)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    ایستا تهی
compile_free_jump_to_end(endlabel_T **el)
{
    وقتی (*el != پوچ)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ایستا scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    اگر (scope == پوچ)
	بازگشت پوچ;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_شمار = cctx->ctx_locals.ga_len;
    اگر (scope->se_outer != پوچ)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    بازگشت scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    تهی
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    اگر (scope == پوچ)
    {
	iemsg("calling drop_scope() without a scope");
	بازگشت;
    }
    cctx->ctx_scope = scope->se_outer;
    انتخاب (scope->se_type)
    {
	حالت IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); بشکن;
	حالت FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); بشکن;
	حالت WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); بشکن;
	حالت TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); بشکن;
	حالت NO_SCOPE:
	حالت BLOCK_SCOPE:
	    بشکن;
    }
    vim_free(scope);
}

    ایستا صحیح
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    اگر (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	بازگشت TRUE;
    }
    بازگشت FALSE;
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
    صحیح		instr_شمار = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    اگر (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	بازگشت پوچ;
    }
    اگر (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	بازگشت پوچ;
    }
    اگر (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    وگرنه اگر (instr->ga_len == instr_شمار && ppconst.pp_used == 1)
    {
	صحیح error = FALSE;
	صحیح v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	اگر (error)
	    بازگشت پوچ;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    وگرنه
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	    بازگشت پوچ;
	اگر (bool_on_پشته(cctx) == FAIL)
	    بازگشت پوچ;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    اگر (scope == پوچ)
	بازگشت پوچ;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    اگر (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    وگرنه
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    بازگشت p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		instr_شمار;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    اگر (scope == پوچ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	بازگشت پوچ;
    }
    اگر (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	بازگشت پوچ;
    }
    unwind_locals(cctx, scope->se_local_شمار, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    اگر (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    اگر (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_عبارت_cctx(&p, cctx);
	بازگشت p;
    }

    اگر (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_عبارت_cctx(&p, cctx);
	بازگشت p;
    }

    اگر (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	صحیح	    moved_cmdmod = FALSE;
	صحیح	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	اگر (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    اگر (GA_GROW_FAILS(instr, 1))
		بازگشت پوچ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	اگر (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	اگر (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    بازگشت پوچ;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	اگر (moved_cmdmod)
	    ++instr->ga_len;

	اگر (saved_debug)
	{
	    // move the debug instruction here
	    اگر (GA_GROW_FAILS(instr, 1))
		بازگشت پوچ;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    اگر (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_شمار = instr->ga_len;
    اگر (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	بازگشت پوچ;
    }
    cctx->ctx_skip = save_skip;
    اگر (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	بازگشت پوچ;
    }
    اگر (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    وگرنه اگر (instr->ga_len == instr_شمار && ppconst.pp_used == 1)
    {
	صحیح error = FALSE;
	صحیح v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	اگر (error)
	{
	    clear_ppconst(&ppconst);
	    بازگشت پوچ;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    وگرنه
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	    بازگشت پوچ;
	اگر (bool_on_پشته(cctx) == FAIL)
	    بازگشت پوچ;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    بازگشت p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    اگر (scope == پوچ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	بازگشت پوچ;
    }
    اگر (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	بازگشت پوچ;
    }
    unwind_locals(cctx, scope->se_local_شمار, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    اگر (cctx->ctx_compile_type == CT_PROFILE)
    {
	اگر (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	اگر (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    اگر (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	اگر (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    اگر (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		بازگشت پوچ;
	}

	اگر (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    اگر (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	اگر (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    بازگشت p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    اگر (scope == پوچ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	بازگشت پوچ;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_شمار, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    اگر (scope->se_u.se_if.is_if_label >= 0)
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
    اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    بازگشت arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ایستا تهی
compile_fill_loop_info(loop_info_T *loop_info, صحیح funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_شمار = cctx->ctx_locals.ga_len;
    loop_info->li_closure_شمار = cctx->ctx_closure_شمار;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ایستا type_T *
compile_for_tuple_get_vartype(type_T *vartype, صحیح var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    اگر (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	بازگشت &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    اگر (member_type->tt_type == VAR_ANY)
	بازگشت &t_any;

    اگر (!var_list)
	// for x in tuple<...list<xxx>>
	بازگشت member_type;

    اگر (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	بازگشت member_type->tt_member;
    وگرنه اگر (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	بازگشت member_type->tt_args[0]->tt_member;

    بازگشت &t_any;
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
    char_u	*name = پوچ;
    char_u	*p;
    char_u	*wp;
    صحیح		var_شمار = 0;
    صحیح		var_list = FALSE;
    صحیح		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    صحیح		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    صحیح		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    صحیح		idx;
    صحیح		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_شمار, &semicolon, FALSE);
    اگر (p == پوچ)
	بازگشت پوچ;
    اگر (var_شمار == 0)
	var_شمار = 1;
    وگرنه
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    اگر (may_get_next_سطر_error(wp, &p, cctx) == FAIL)
	بازگشت پوچ;
    اگر (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	اگر (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	وگرنه
	    emsg(_(e_missing_in_after_for));
	بازگشت پوچ;
    }
    wp = p + 2;
    اگر (may_get_next_سطر_error(wp, &p, cctx) == FAIL)
	بازگشت پوچ;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    اگر (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    اگر (scope == پوچ)
	بازگشت پوچ;
    اگر (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	بازگشت پوچ;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_عدد);
    اگر (loop_lvar == پوچ)
    {
	drop_scope(cctx);
	بازگشت پوچ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_عدد);
    اگر (funcref_lvar == پوچ)
    {
	drop_scope(cctx);
	بازگشت پوچ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    اگر (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	بازگشت پوچ;
    }
    arg_end = arg;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_پشته(cctx, 0);
	اگر (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    بازگشت پوچ;
	}

	اگر (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	وگرنه اگر (vartype->tt_type == VAR_BLOB)
	    item_type = &t_عدد;
	وگرنه اگر (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    اگر (!var_list)
		item_type = vartype->tt_member;
	    وگرنه اگر (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	وگرنه اگر (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_بالاترین_label = current_instr_idx(cctx);

	اگر (cctx->ctx_compile_type == CT_DEBUG)
	{
	    صحیح		save_prev_lnum = cctx->ctx_prev_lnum;
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
	اگر (var_list)
	{
	    generate_UNPACK(cctx, var_شمار, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_پشته.ga_len;

	    // add type of the items
	    برای (idx = 0; idx < var_شمار; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		اگر (افزودن_type_پشته(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    بازگشت پوچ;
		}
	    }
	}

	برای (idx = 0; idx < var_شمار; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    صحیح			opt_flags = 0;
	    صحیح			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    اگر (name == پوچ)
		برو failed;
	    اگر (*skipwhite(p) == ':')
	    {
		اگر (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    برو failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		اگر (lhs_type == پوچ || !valid_declaration_type(lhs_type))
		    برو failed;
	    }

	    اگر (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		برو failed;
	    اگر (dest != dest_local)
	    {
		اگر (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, پوچ) == FAIL)
		    برو failed;
	    }
	    وگرنه اگر (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		اگر (generate_instr_drop(cctx, ISN_DROP, 1) == پوچ)
		    برو failed;
	    }
	    وگرنه
	    {
		// Script var is not supported.
		اگر (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    برو failed;
		}

		اگر (!valid_varname(arg, (صحیح)varlen, FALSE))
		    برو failed;
		اگر (lookup_local(arg, varlen, پوچ, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    برو failed;
		}

		// Reserve a variable to store "var".
		اگر (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		اگر (lhs_type == &t_any)
		    lhs_type = item_type;
		وگرنه اگر (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    برو failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		اگر (var_lvar == پوچ)
		    // out of memory or used as an argument
		    برو failed;

		اگر (semicolon && idx == var_شمار - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, پوچ);
	    }

	    اگر (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    بازگشت arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    بازگشت پوچ;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ایستا صحیح
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    اگر (cctx->ctx_locals.ga_len > loop_info->li_local_شمار
	    && cctx->ctx_closure_شمار > loop_info->li_closure_شمار)
	بازگشت generate_ENDLOOP(cctx, loop_info);
    بازگشت OK;
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

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    اگر (scope == پوچ || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	بازگشت پوچ;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	اگر (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    بازگشت پوچ;

	unwind_locals(cctx, scope->se_local_شمار, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_بالاترین_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_بالاترین_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	اگر (generate_instr_drop(cctx, ISN_DROP, 1) == پوچ)
	    بازگشت پوچ;
    }

    vim_free(scope);

    بازگشت arg;
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
    صحیح		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    اگر (scope == پوچ)
	بازگشت پوچ;
    اگر (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	بازگشت پوچ;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_بالاترین_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_عدد);
    اگر (funcref_lvar == پوچ)
    {
	drop_scope(cctx);
	بازگشت پوچ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    اگر (compile_expr0(&p, cctx) == FAIL)
	بازگشت پوچ;

    اگر (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	بازگشت پوچ;
    }

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (bool_on_پشته(cctx) == FAIL)
	    بازگشت FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	اگر (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;
    اگر (scope == پوچ || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	بازگشت پوچ;
    }
    cctx->ctx_scope = scope->se_outer;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	اگر (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    بازگشت پوچ;

	unwind_locals(cctx, scope->se_local_شمار, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_بالاترین_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    بازگشت arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    صحیح
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    صحیح		prev_local_شمار = 0;

    CLEAR_POINTER(lvi);
    برای (;;)
    {
	loop_info_T	*loopinfo;
	صحیح		cur_local_last;
	صحیح		start_local_شمار;

	وقتی (scope != پوچ && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	اگر (scope == پوچ)
	    بشکن;

	اگر (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_شمار - 1;
	}
	وگرنه
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_شمار - 3;
	}

	start_local_شمار = loopinfo->li_local_شمار;
	اگر (cctx->ctx_locals.ga_len > start_local_شمار)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_شمار;
	    lvi->lvi_loop[loopinfo->li_depth].var_شمار =
			  (short)(cctx->ctx_locals.ga_len - start_local_شمار
							   - prev_local_شمار);
	    اگر (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_شمار = cctx->ctx_locals.ga_len - cur_local_last;
    }
    بازگشت lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    تهی
get_loop_var_idx(cctx_T *cctx, صحیح idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    اگر (get_loop_var_info(cctx, &lvi))
    {
	صحیح depth;

	برای (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    اگر (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_شمار)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		بازگشت;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ایستا صحیح
compile_find_scope(
	صحیح	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	صحیح	    *try_scopes,    // :try scopes encountered or NULL
	نویسه	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    برای (;;)
    {
	اگر (scope == پوچ)
	{
	    اگر (error != پوچ)
		emsg(_(error));
	    بازگشت FAIL;
	}
	اگر (scope->se_type == FOR_SCOPE)
	{
	    اگر (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		بازگشت FAIL;
	    اگر (loop_label != پوچ)
		*loop_label = scope->se_u.se_for.fs_بالاترین_label;
	    اگر (el != پوچ)
		*el = &scope->se_u.se_for.fs_end_label;
	    بشکن;
	}
	اگر (scope->se_type == WHILE_SCOPE)
	{
	    اگر (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		بازگشت FAIL;
	    اگر (loop_label != پوچ)
		*loop_label = scope->se_u.se_while.ws_بالاترین_label;
	    اگر (el != پوچ)
		*el = &scope->se_u.se_while.ws_end_label;
	    بشکن;
	}
	اگر (try_scopes != پوچ && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    بازگشت OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    صحیح		try_scopes = 0;
    صحیح		loop_label;

    اگر (compile_find_scope(&loop_label, پوچ, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	بازگشت پوچ;
    اگر (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    وگرنه
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    بازگشت arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    صحیح		try_scopes = 0;
    endlabel_T	**el;

    اگر (compile_find_scope(پوچ, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	بازگشت پوچ;

    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت arg;

    اگر (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    اگر (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	بازگشت پوچ;

    بازگشت arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    اگر (new_scope(cctx, BLOCK_SCOPE) == پوچ)
	بازگشت پوچ;
    بازگشت skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    تهی
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_شمار, TRUE);
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

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    اگر (try_scope == پوچ)
	بازگشت پوچ;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	اگر ((isn = generate_instr(cctx, ISN_TRY)) == پوچ)
	    بازگشت پوچ;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	اگر (isn->isn_arg.tryref.try_ref == پوچ)
	    بازگشت پوچ;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    اگر (scope == پوچ)
	بازگشت پوچ;

    بازگشت arg;
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

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    // end block scope from :try or :catch
    اگر (scope != پوچ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == پوچ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	بازگشت پوچ;
    }

    اگر (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	بازگشت پوچ;
    }
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	اگر (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	اگر (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    بازگشت پوچ;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	اگر (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	اگر (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    اگر (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    وگرنه
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = پوچ;
	صحیح	dropped = 0;
	صحیح	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, پوچ);
	اگر (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    بازگشت پوچ;
	}
	اگر (tofree == پوچ)
	    len = (صحیح)(end - (p + 1));
	وگرنه
	    len = (صحیح)(end - tofree);
	pat = vim_strnsave(tofree == پوچ ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	اگر (pat == پوچ)
	    بازگشت پوچ;
	اگر (generate_PUSHS(cctx, &pat) == FAIL)
	    بازگشت پوچ;

	اگر (generate_COMPARE(cctx, عبارت_MATCH, FALSE) == FAIL)
	    بازگشت پوچ;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	اگر (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    بازگشت پوچ;
    }

    اگر (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == پوچ)
	بازگشت پوچ;

    اگر (new_scope(cctx, BLOCK_SCOPE) == پوچ)
	بازگشت پوچ;
    بازگشت p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    صحیح		this_instr;

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    // end block scope from :try or :catch
    اگر (scope != پوچ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == پوچ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	بازگشت پوچ;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	اگر (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    بازگشت پوچ;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    اگر (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	اگر (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	اگر (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	اگر (generate_instr(cctx, ISN_FINALLY) == پوچ)
	    بازگشت پوچ;
    }

    بازگشت arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    اگر (misplaced_cmdmod(cctx))
	بازگشت پوچ;

    // end block scope from :catch or :finally
    اگر (scope != پوچ && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == پوچ || scope->se_type != TRY_SCOPE)
    {
	اگر (scope == پوچ)
	    emsg(_(e_endtry_without_try));
	وگرنه اگر (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	وگرنه اگر (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	وگرنه
	    emsg(_(e_missing_endif));
	بازگشت پوچ;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    بازگشت پوچ;
	}

#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE
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

	اگر (scope->se_u.se_try.ts_catch_label != 0)
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

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	اگر (generate_instr(cctx, ISN_ENDTRY) == پوچ)
	    بازگشت پوچ;
#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    بازگشت arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    اگر (compile_expr0(&p, cctx) == FAIL)
	بازگشت پوچ;
    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت p;
    اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	بازگشت پوچ;
    اگر (generate_instr_drop(cctx, ISN_THROW, 1) == پوچ)
	بازگشت پوچ;

    بازگشت p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    صحیح		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    اگر (compile_expr0(&p, cctx) == FAIL)
	بازگشت پوچ;

    اگر (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	بازگشت پوچ;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    بازگشت skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    صحیح
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    اگر (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	اگر (lvar == پوچ)
	    بازگشت 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    بازگشت dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    صحیح		argcount = 0;
    صحیح		defer_var_idx;
    type_T	*type = پوچ;
    صحیح		func_idx = -1;

    اگر (*arg == '(')
    {
	// a lambda function
	اگر (compile_lambda(&arg, cctx, پوچ) != OK)
	    بازگشت پوچ;
	paren = vim_strchr(arg, '(');
	اگر (paren == پوچ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    بازگشت پوچ;
	}
    }
    وگرنه
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	اگر (paren == پوچ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    بازگشت پوچ;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	اگر (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	وگرنه اگر (compile_expr0(&arg, cctx) == FAIL)
	    بازگشت پوچ;
	*paren = '(';
    }

    // check for function type
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_پشته(cctx, 0);
	اگر (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    بازگشت پوچ;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    اگر (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	بازگشت پوچ;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (func_idx >= 0)
	{
	    type2_T	*argtypes = پوچ;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    اگر (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		بازگشت پوچ;
	}
	وگرنه اگر (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    بازگشت پوچ;

	defer_var_idx = get_defer_var_idx(cctx);
	اگر (defer_var_idx == 0)
	    بازگشت پوچ;
	اگر (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    بازگشت پوچ;
    }

    بازگشت skipwhite(arg);
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
compile_mult_عبارت(
	char_u	*arg,
	صحیح	cmdidx,
	long	cmd_شمار UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*عبارت_start;
    صحیح		شمار = 0;
    صحیح		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    صحیح		r = OK;

    برای (;;)
    {
	اگر (ends_excmd2(prev, p))
	    بشکن;
	عبارت_start = p;
	اگر (compile_expr0(&p, cctx) == FAIL)
	    بازگشت پوچ;

	اگر (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_پشته(cctx, 0);
	    اگر (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_مقدار_str), عبارت_start);
		بازگشت پوچ;
	    }
	}

	++شمار;
	prev = p;
	p = skipwhite(p);
    }

    اگر (شمار > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	اگر (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, شمار);
	وگرنه اگر (cmdidx == CMD_execute)
	    r = generate_MULT_عبارت(cctx, ISN_EXECUTE, شمار);
	وگرنه اگر (cmdidx == CMD_echomsg)
	    r = generate_MULT_عبارت(cctx, ISN_ECHOMSG, شمار);
#ifdef HAS_MESSAGE_WINDOW
	وگرنه اگر (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, شمار, cmd_شمار);
#endif
	وگرنه اگر (cmdidx == CMD_echoconsole)
	    r = generate_MULT_عبارت(cctx, ISN_ECHOCONSOLE, شمار);
	وگرنه
	    r = generate_MULT_عبارت(cctx, ISN_ECHOERR, شمار);

	cctx->ctx_lnum = save_lnum;
    }
    بازگشت r == OK ? p : پوچ;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ایستا صحیح
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, پوچ);
    char_u *p = skipdigits(eap->cmd);

    اگر (p == range_end)
	بازگشت FAIL;
    بازگشت generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, صحیح fixindent)
{
    char_u	*سطر = arg;
    linenr_T	lnum;
    نویسه	*errormsg;
    صحیح		above = eap->forceit;

    eap->regname = *سطر;

    اگر (eap->regname == '=')
    {
	char_u *p = skipwhite(سطر + 1);

	اگر (compile_expr0(&p, cctx) == FAIL)
	    بازگشت پوچ;
	سطر = p;
    }
    وگرنه اگر (eap->regname != NUL)
	++سطر;

    اگر (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    وگرنه
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	اگر (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    بازگشت پوچ;
	اگر (eap->addr_شمار == 0)
	    lnum = -1;
	وگرنه
	    lnum = eap->line2;
	اگر (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    بازگشت سطر;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *سطر_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*سطر = سطر_arg;
    char_u	*p;
    صحیح		has_عبارت = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = پوچ;
    char_u	*cmd_arg = پوچ;

    اگر (cctx->ctx_skip == SKIP_YES)
	برو theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    اگر (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	اگر (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    اگر (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	صحیح	usefilter = FALSE;

	has_عبارت = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	اگر ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	اگر ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    اگر (eap->nextcmd != پوچ)
		nextcmd = eap->nextcmd;
	}
	وگرنه اگر (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    اگر (*p != NUL)
		++p;
	    اگر (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    اگر (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	وگرنه اگر (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    صحیح	    flags = 0;  // unused
	    صحیح	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, سطر, &tofree, &flags);
	    اگر (tofree != پوچ)
	    {
		سطر = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    اگر (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_عبارت = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	اگر (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    اگر ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	صحیح delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, پوچ, پوچ, پوچ);
	اگر (*p == delim)
	    cmd_arg = p + 1;
    }

    اگر (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    اگر (cmd_arg != پوچ)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, پوچ, lookup_scriptitem, پوچ);
	اگر (nea.cmdidx < CMD_SIZE)
	{
	    has_عبارت = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    اگر (has_عبارت)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    اگر (has_عبارت && (p = (char_u *)strstr((نویسه *)eap->arg, "`=")) != پوچ)
    {
	صحیح	شمار = 0;
	char_u	*start = skipwhite(سطر);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	برای (;;)
	{
	    اگر (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++شمار;
	    }
	    p += 2;
	    اگر (compile_expr0(&p, cctx) == FAIL)
		بازگشت پوچ;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++شمار;
	    p = skipwhite(p);
	    اگر (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		بازگشت پوچ;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((نویسه *)start, "`=");
	    اگر (p == پوچ)
	    {
		اگر (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++شمار;
		}
		بشکن;
	    }
	}
	generate_EXECCONCAT(cctx, شمار);
    }
    وگرنه
	generate_EXEC_copy(cctx, ISN_EXEC, سطر);

theend:
    اگر (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    بازگشت nextcmd;
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
compile_script(char_u *سطر, cctx_T *cctx)
{
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	اگر ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == پوچ)
	    بازگشت پوچ;
	isn->isn_arg.string = vim_strsave(سطر);
    }
    بازگشت (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *عبارت = (char_u *)strstr((نویسه *)cmd, "\\=");

    اگر (عبارت != پوچ)
    {
	صحیح delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), پوچ, پوچ, پوچ);
	اگر (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    صحیح		عبارت_res;
	    صحیح		trailing_error;
	    صحیح		instr_شمار;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = پوچ;
	    عبارت_res = compile_expr0(&cmd, cctx);
	    اگر (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    اگر (عبارت_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		اگر (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		بازگشت پوچ;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_شمار = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_شمار].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    اگر ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == پوچ)
	    {
		صحیح idx;

		برای (idx = 0; idx < instr_شمار; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		بازگشت پوچ;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    اگر (*end == '&')
		++end;
	    وقتی (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    بازگشت end;
	}
    }

    بازگشت compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *سطر, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    اگر (lhs->lhs_name != پوچ)
    {
	اگر (STRNCMP(arg, "END", 3) == 0)
	{
	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		اگر (lhs->lhs_append)
		{
		    // First load the current variable value.
		    اگر (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			بازگشت پوچ;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		اگر (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		اگر (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    اگر (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			بازگشت پوچ;
		}
		وگرنه اگر (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    بازگشت پوچ;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    بازگشت arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	بازگشت پوچ;
    }

    اگر (arg[0] == '=' && arg[1] == '>')
    {
	صحیح	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	اگر (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	اگر (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    بازگشت پوچ;
	اگر (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    بازگشت پوچ;
	اگر (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	وگرنه
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    اگر (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		اگر (lhs->lhs_whole == پوچ)
		    بازگشت پوچ;
	    }
	}

	بازگشت arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    بازگشت compile_exec(سطر, eap, cctx);
}

#اگر defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *سطر, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    اگر (isn == پوچ)
	بازگشت پوچ;
    isn->isn_arg.عدد = eap->cmdidx;

    p = eap->arg;
    اگر (compile_expr0(&p, cctx) == FAIL)
	بازگشت پوچ;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    اگر (isn == پوچ)
	بازگشت پوچ;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    اگر (isn->isn_arg.cexpr.cexpr_ref == پوچ)
	بازگشت پوچ;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(سطر));

    بازگشت p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, صحیح check_return_type, صحیح legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*پشته_type;

    اگر (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	اگر (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_مقدار_in_function_without_return_type));
	    بازگشت پوچ;
	}
	اگر (legacy)
	{
	    صحیح save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    اگر (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		بازگشت پوچ;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (تهی)skip_عبارت(&p, پوچ);
	    cmdmod.cmod_flags = save_flags;
	}
	وگرنه
	{
	    // compile return argument into instructions
	    اگر (compile_expr0(&p, cctx) == FAIL)
		بازگشت پوچ;
	}

	اگر (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    پشته_type = get_type_on_پشته(cctx, 0);
	    اگر (check_type_is_مقدار(پشته_type) == FAIL)
		بازگشت پوچ;
	    اگر ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == پوچ
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = پشته_type;
	    }
	    وگرنه
	    {
		اگر (need_type(پشته_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    بازگشت پوچ;
	    }
	}
    }
    وگرنه
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	اگر (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_مقدار));
	    بازگشت پوچ;
	}

	اگر (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	وگرنه
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    اگر (compile_find_scope(پوچ, پوچ, پوچ, پوچ, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    اگر (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == پوچ)
	بازگشت پوچ;

    // "return val | endif" is possible
    بازگشت skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    صحیح
check_global_and_subst(char_u *cmd, char_u *arg)
{
    اگر (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != پوچ)
    {
	semsg(_(e_separator_not_supported_str), arg);
	بازگشت FAIL;
    }
    اگر (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	بازگشت FAIL;
    }
    بازگشت OK;
}


#endif  // defined(FEAT_EVAL)
