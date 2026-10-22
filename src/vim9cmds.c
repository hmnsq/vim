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
#include "vim.h"

#если defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    static целое
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    целое		idx = instr->ga_len;

    пока (idx > 0)
    {
	если (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    continue;
	}
#ifdef FEAT_PROFILE
	если (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    continue;
	}
#endif
	если (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    continue;
	}
	break;
    }
    вернуть idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    static void
unwind_locals(cctx_T *cctx, целое new_top, целое keep)
{
    если (cctx->ctx_locals.ga_len > new_top)
	для (целое idx = new_top; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    если (!keep)
	cctx->ctx_locals.ga_len = new_top;
}

/*
 * Free all local variables.
 */
    void
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    целое
check_vim9_unlet(char_u *name)
{
    если (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	вернуть FAIL;
    }

    если (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == NULL)
    {
	// "unlet s:var" is allowed in legacy script.
	если (*name == 's' && !in_vim9script())
	    вернуть OK;
	semsg(_(e_cannot_unlet_str), name);
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    static целое
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    целое	    deep UNUSED,
    void    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    целое		cc = *name_end;
    целое		ret = OK;

    если (cctx->ctx_skip == SKIP_YES)
	вернуть OK;

    *name_end = NUL;
    если (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    иначе если (vim_strchr(p, '.') != NULL || vim_strchr(p, '[') != NULL)
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
	если (ret == OK)
	{
	    если (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    иначе
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    иначе если (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    иначе
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    вернуть ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    static целое
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    целое	    deep,
    void    *coookie)
{
    cctx_T	*cctx = coookie;
    целое		cc = *name_end;
    char_u	*p = lvp->ll_name;
    целое		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    char	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    целое		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    если (cctx->ctx_skip == SKIP_YES)
	вернуть OK;

    если (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	вернуть FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    если (p[1] != ':')
    {
	char_u *end = find_name_end(p, NULL, NULL, FNE_CHECK_START);

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

	char_u	*name = NULL;
	целое	len = end - p;

	если (lookup_local(p, len, NULL, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    если (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		вернуть FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(NULL, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	если (name == NULL)
	{
	    class_T *cl;
	    если (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		если (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (целое)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(NULL, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	если (name == NULL)
	{
	    // Can lockvar any function arg.
	    если (arg_exists(p, len, NULL, NULL, NULL, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	если (name == NULL)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    если (script_var_exists(p, len, cctx, NULL) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	если (name != NULL)
	{
#ifdef LOG_LOCKVAR
	    ch_log(NULL, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    если (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		вернуть FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    если (buf == NULL)
	ret = FAIL;
    иначе
    {
	если (deep < 0)
	    vim_snprintf((char *)buf, len, "%s! %s", cmd, p);
	иначе
	    vim_snprintf((char *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(NULL, "LKVAR:    ... buf %s", buf);
#endif
	если (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	иначе
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    вернуть ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    целое	    deep = 0;
    char_u  *p = arg;

    если (eap->cmdidx != CMD_unlet)
    {
	если (eap->forceit)
	    deep = -1;
	иначе если (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	иначе
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    вернуть eap->nextcmd == NULL ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    static целое
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	целое	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    если (endlabel == NULL)
	вернуть FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    если (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    иначе
	generate_JUMP(cctx, when, 0);
    вернуть OK;
}

    static void
compile_fill_jump_to_end(endlabel_T **el, целое jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    пока (*el != NULL)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    static void
compile_free_jump_to_end(endlabel_T **el)
{
    пока (*el != NULL)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    static scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    если (scope == NULL)
	вернуть NULL;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_count = cctx->ctx_locals.ga_len;
    если (scope->se_outer != NULL)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    вернуть scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    void
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    если (scope == NULL)
    {
	iemsg("calling drop_scope() without a scope");
	вернуть;
    }
    cctx->ctx_scope = scope->se_outer;
    switch (scope->se_type)
    {
	case IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); break;
	case FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); break;
	case WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); break;
	case TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); break;
	case NO_SCOPE:
	case BLOCK_SCOPE:
	    break;
    }
    vim_free(scope);
}

    static целое
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    если (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	вернуть TRUE;
    }
    вернуть FALSE;
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
    целое		instr_count = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    если (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	вернуть NULL;
    }
    если (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	вернуть NULL;
    }
    если (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    иначе если (instr->ga_len == instr_count && ppconst.pp_used == 1)
    {
	целое error = FALSE;
	целое v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	если (error)
	    вернуть NULL;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    иначе
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	если (generate_ppconst(cctx, &ppconst) == FAIL)
	    вернуть NULL;
	если (bool_on_stack(cctx) == FAIL)
	    вернуть NULL;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    если (scope == NULL)
	вернуть NULL;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    если (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    иначе
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    если (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    вернуть p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    целое		instr_count;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    если (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	вернуть NULL;
    }
    если (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	вернуть NULL;
    }
    unwind_locals(cctx, scope->se_local_count, TRUE);
    если (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    если (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    если (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_expr_cctx(&p, cctx);
	вернуть p;
    }

    если (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_expr_cctx(&p, cctx);
	вернуть p;
    }

    если (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	целое	    moved_cmdmod = FALSE;
	целое	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	если (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    если (GA_GROW_FAILS(instr, 1))
		вернуть NULL;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	если (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	если (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    вернуть NULL;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	если (moved_cmdmod)
	    ++instr->ga_len;

	если (saved_debug)
	{
	    // move the debug instruction here
	    если (GA_GROW_FAILS(instr, 1))
		вернуть NULL;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    если (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	если (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_count = instr->ga_len;
    если (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	вернуть NULL;
    }
    cctx->ctx_skip = save_skip;
    если (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	вернуть NULL;
    }
    если (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    иначе если (instr->ga_len == instr_count && ppconst.pp_used == 1)
    {
	целое error = FALSE;
	целое v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	если (error)
	{
	    clear_ppconst(&ppconst);
	    вернуть NULL;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    иначе
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	если (generate_ppconst(cctx, &ppconst) == FAIL)
	    вернуть NULL;
	если (bool_on_stack(cctx) == FAIL)
	    вернуть NULL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    вернуть p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    если (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	вернуть NULL;
    }
    если (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	вернуть NULL;
    }
    unwind_locals(cctx, scope->se_local_count, TRUE);
    если (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    если (cctx->ctx_compile_type == CT_PROFILE)
    {
	если (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	если (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    если (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	если (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    если (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		вернуть NULL;
	}

	если (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    если (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	если (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    вернуть p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    если (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	вернуть NULL;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_count, TRUE);
    если (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    если (scope->se_u.se_if.is_if_label >= 0)
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
    если (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    вернуть arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    static void
compile_fill_loop_info(loop_info_T *loop_info, целое funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_count = cctx->ctx_locals.ga_len;
    loop_info->li_closure_count = cctx->ctx_closure_count;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    static type_T *
compile_for_tuple_get_vartype(type_T *vartype, целое var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    если (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	вернуть &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    если (member_type->tt_type == VAR_ANY)
	вернуть &t_any;

    если (!var_list)
	// for x in tuple<...list<xxx>>
	вернуть member_type;

    если (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	вернуть member_type->tt_member;
    иначе если (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	вернуть member_type->tt_args[0]->tt_member;

    вернуть &t_any;
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
    char_u	*name = NULL;
    char_u	*p;
    char_u	*wp;
    целое		var_count = 0;
    целое		var_list = FALSE;
    целое		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    целое		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    целое		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    целое		idx;
    целое		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_count, &semicolon, FALSE);
    если (p == NULL)
	вернуть NULL;
    если (var_count == 0)
	var_count = 1;
    иначе
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    если (may_get_next_line_error(wp, &p, cctx) == FAIL)
	вернуть NULL;
    если (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	если (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	иначе
	    emsg(_(e_missing_in_after_for));
	вернуть NULL;
    }
    wp = p + 2;
    если (may_get_next_line_error(wp, &p, cctx) == FAIL)
	вернуть NULL;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    если (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    если (scope == NULL)
	вернуть NULL;
    если (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	вернуть NULL;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    если (loop_lvar == NULL)
    {
	drop_scope(cctx);
	вернуть NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    если (funcref_lvar == NULL)
    {
	drop_scope(cctx);
	вернуть NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    если (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	вернуть NULL;
    }
    arg_end = arg;

    если (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_stack(cctx, 0);
	если (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    вернуть NULL;
	}

	если (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	иначе если (vartype->tt_type == VAR_BLOB)
	    item_type = &t_number;
	иначе если (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    если (!var_list)
		item_type = vartype->tt_member;
	    иначе если (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	иначе если (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_top_label = current_instr_idx(cctx);

	если (cctx->ctx_compile_type == CT_DEBUG)
	{
	    целое		save_prev_lnum = cctx->ctx_prev_lnum;
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
	если (var_list)
	{
	    generate_UNPACK(cctx, var_count, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_stack.ga_len;

	    // add type of the items
	    для (idx = 0; idx < var_count; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		если (push_type_stack(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    вернуть NULL;
		}
	    }
	}

	для (idx = 0; idx < var_count; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    целое			opt_flags = 0;
	    целое			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    если (name == NULL)
		goto failed;
	    если (*skipwhite(p) == ':')
	    {
		если (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    goto failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		если (lhs_type == NULL || !valid_declaration_type(lhs_type))
		    goto failed;
	    }

	    если (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		goto failed;
	    если (dest != dest_local)
	    {
		если (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, NULL) == FAIL)
		    goto failed;
	    }
	    иначе если (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		если (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
		    goto failed;
	    }
	    иначе
	    {
		// Script var is not supported.
		если (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    goto failed;
		}

		если (!valid_varname(arg, (целое)varlen, FALSE))
		    goto failed;
		если (lookup_local(arg, varlen, NULL, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    goto failed;
		}

		// Reserve a variable to store "var".
		если (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		если (lhs_type == &t_any)
		    lhs_type = item_type;
		иначе если (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    goto failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		если (var_lvar == NULL)
		    // out of memory or used as an argument
		    goto failed;

		если (semicolon && idx == var_count - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, NULL);
	    }

	    если (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    вернуть arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    вернуть NULL;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    static целое
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    если (cctx->ctx_locals.ga_len > loop_info->li_local_count
	    && cctx->ctx_closure_count > loop_info->li_closure_count)
	вернуть generate_ENDLOOP(cctx, loop_info);
    вернуть OK;
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

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    если (scope == NULL || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	вернуть NULL;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    если (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	если (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    вернуть NULL;

	unwind_locals(cctx, scope->se_local_count, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_top_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_top_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	если (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
	    вернуть NULL;
    }

    vim_free(scope);

    вернуть arg;
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
    целое		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    если (scope == NULL)
	вернуть NULL;
    если (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	вернуть NULL;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_top_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    если (funcref_lvar == NULL)
    {
	drop_scope(cctx);
	вернуть NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    если (compile_expr0(&p, cctx) == FAIL)
	вернуть NULL;

    если (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	вернуть NULL;
    }

    если (cctx->ctx_skip != SKIP_YES)
    {
	если (bool_on_stack(cctx) == FAIL)
	    вернуть FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	если (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    вернуть FAIL;
    }

    вернуть p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    если (misplaced_cmdmod(cctx))
	вернуть NULL;
    если (scope == NULL || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	вернуть NULL;
    }
    cctx->ctx_scope = scope->se_outer;
    если (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	если (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    вернуть NULL;

	unwind_locals(cctx, scope->se_local_count, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_top_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    вернуть arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    целое
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    целое		prev_local_count = 0;

    CLEAR_POINTER(lvi);
    для (;;)
    {
	loop_info_T	*loopinfo;
	целое		cur_local_last;
	целое		start_local_count;

	пока (scope != NULL && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	если (scope == NULL)
	    break;

	если (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_count - 1;
	}
	иначе
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_count - 3;
	}

	start_local_count = loopinfo->li_local_count;
	если (cctx->ctx_locals.ga_len > start_local_count)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_count;
	    lvi->lvi_loop[loopinfo->li_depth].var_count =
			  (short)(cctx->ctx_locals.ga_len - start_local_count
							   - prev_local_count);
	    если (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_count = cctx->ctx_locals.ga_len - cur_local_last;
    }
    вернуть lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    void
get_loop_var_idx(cctx_T *cctx, целое idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    если (get_loop_var_info(cctx, &lvi))
    {
	целое depth;

	для (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    если (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_count)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		вернуть;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    static целое
compile_find_scope(
	целое	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	целое	    *try_scopes,    // :try scopes encountered or NULL
	char	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    для (;;)
    {
	если (scope == NULL)
	{
	    если (error != NULL)
		emsg(_(error));
	    вернуть FAIL;
	}
	если (scope->se_type == FOR_SCOPE)
	{
	    если (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		вернуть FAIL;
	    если (loop_label != NULL)
		*loop_label = scope->se_u.se_for.fs_top_label;
	    если (el != NULL)
		*el = &scope->se_u.se_for.fs_end_label;
	    break;
	}
	если (scope->se_type == WHILE_SCOPE)
	{
	    если (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		вернуть FAIL;
	    если (loop_label != NULL)
		*loop_label = scope->se_u.se_while.ws_top_label;
	    если (el != NULL)
		*el = &scope->se_u.se_while.ws_end_label;
	    break;
	}
	если (try_scopes != NULL && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    вернуть OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    целое		try_scopes = 0;
    целое		loop_label;

    если (compile_find_scope(&loop_label, NULL, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	вернуть NULL;
    если (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    иначе
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    вернуть arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    целое		try_scopes = 0;
    endlabel_T	**el;

    если (compile_find_scope(NULL, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	вернуть NULL;

    если (cctx->ctx_skip == SKIP_YES)
	вернуть arg;

    если (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    если (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	вернуть NULL;

    вернуть arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    если (new_scope(cctx, BLOCK_SCOPE) == NULL)
	вернуть NULL;
    вернуть skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    void
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_count, TRUE);
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

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    если (try_scope == NULL)
	вернуть NULL;

    если (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	если ((isn = generate_instr(cctx, ISN_TRY)) == NULL)
	    вернуть NULL;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	если (isn->isn_arg.tryref.try_ref == NULL)
	    вернуть NULL;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    если (scope == NULL)
	вернуть NULL;

    вернуть arg;
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

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    // end block scope from :try or :catch
    если (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    если (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	вернуть NULL;
    }

    если (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	вернуть NULL;
    }
    если (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    если (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	если (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	если (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    вернуть NULL;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	если (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	если (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	если (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    если (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    иначе
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = NULL;
	целое	dropped = 0;
	целое	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, NULL);
	если (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    вернуть NULL;
	}
	если (tofree == NULL)
	    len = (целое)(end - (p + 1));
	иначе
	    len = (целое)(end - tofree);
	pat = vim_strnsave(tofree == NULL ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	если (pat == NULL)
	    вернуть NULL;
	если (generate_PUSHS(cctx, &pat) == FAIL)
	    вернуть NULL;

	если (generate_COMPARE(cctx, EXPR_MATCH, FALSE) == FAIL)
	    вернуть NULL;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	если (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    вернуть NULL;
    }

    если (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == NULL)
	вернуть NULL;

    если (new_scope(cctx, BLOCK_SCOPE) == NULL)
	вернуть NULL;
    вернуть p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    целое		this_instr;

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    // end block scope from :try or :catch
    если (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    если (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	вернуть NULL;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    если (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    если (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	если (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    вернуть NULL;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    если (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	если (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	если (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	если (generate_instr(cctx, ISN_FINALLY) == NULL)
	    вернуть NULL;
    }

    вернуть arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    если (misplaced_cmdmod(cctx))
	вернуть NULL;

    // end block scope from :catch or :finally
    если (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    если (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	если (scope == NULL)
	    emsg(_(e_endtry_without_try));
	иначе если (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	иначе если (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	иначе
	    emsg(_(e_missing_endif));
	вернуть NULL;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    если (cctx->ctx_skip != SKIP_YES)
    {
	если (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    вернуть NULL;
	}

#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE
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

	если (scope->se_u.se_try.ts_catch_label != 0)
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

    если (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	если (generate_instr(cctx, ISN_ENDTRY) == NULL)
	    вернуть NULL;
#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    вернуть arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    если (compile_expr0(&p, cctx) == FAIL)
	вернуть NULL;
    если (cctx->ctx_skip == SKIP_YES)
	вернуть p;
    если (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	вернуть NULL;
    если (generate_instr_drop(cctx, ISN_THROW, 1) == NULL)
	вернуть NULL;

    вернуть p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    целое		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    если (compile_expr0(&p, cctx) == FAIL)
	вернуть NULL;

    если (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	вернуть NULL;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    вернуть skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    целое
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    если (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	если (lvar == NULL)
	    вернуть 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    вернуть dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    целое		argcount = 0;
    целое		defer_var_idx;
    type_T	*type = NULL;
    целое		func_idx = -1;

    если (*arg == '(')
    {
	// a lambda function
	если (compile_lambda(&arg, cctx, NULL) != OK)
	    вернуть NULL;
	paren = vim_strchr(arg, '(');
	если (paren == NULL)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    вернуть NULL;
	}
    }
    иначе
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	если (paren == NULL)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    вернуть NULL;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	если (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	иначе если (compile_expr0(&arg, cctx) == FAIL)
	    вернуть NULL;
	*paren = '(';
    }

    // check for function type
    если (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_stack(cctx, 0);
	если (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    вернуть NULL;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    если (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	вернуть NULL;

    если (cctx->ctx_skip != SKIP_YES)
    {
	если (func_idx >= 0)
	{
	    type2_T	*argtypes = NULL;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    если (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		вернуть NULL;
	}
	иначе если (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    вернуть NULL;

	defer_var_idx = get_defer_var_idx(cctx);
	если (defer_var_idx == 0)
	    вернуть NULL;
	если (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    вернуть NULL;
    }

    вернуть skipwhite(arg);
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
compile_mult_expr(
	char_u	*arg,
	целое	cmdidx,
	long	cmd_count UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*expr_start;
    целое		count = 0;
    целое		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    целое		r = OK;

    для (;;)
    {
	если (ends_excmd2(prev, p))
	    break;
	expr_start = p;
	если (compile_expr0(&p, cctx) == FAIL)
	    вернуть NULL;

	если (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_stack(cctx, 0);
	    если (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_value_str), expr_start);
		вернуть NULL;
	    }
	}

	++count;
	prev = p;
	p = skipwhite(p);
    }

    если (count > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	если (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, count);
	иначе если (cmdidx == CMD_execute)
	    r = generate_MULT_EXPR(cctx, ISN_EXECUTE, count);
	иначе если (cmdidx == CMD_echomsg)
	    r = generate_MULT_EXPR(cctx, ISN_ECHOMSG, count);
#ifdef HAS_MESSAGE_WINDOW
	иначе если (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, count, cmd_count);
#endif
	иначе если (cmdidx == CMD_echoconsole)
	    r = generate_MULT_EXPR(cctx, ISN_ECHOCONSOLE, count);
	иначе
	    r = generate_MULT_EXPR(cctx, ISN_ECHOERR, count);

	cctx->ctx_lnum = save_lnum;
    }
    вернуть r == OK ? p : NULL;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    static целое
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, NULL);
    char_u *p = skipdigits(eap->cmd);

    если (p == range_end)
	вернуть FAIL;
    вернуть generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, целое fixindent)
{
    char_u	*line = arg;
    linenr_T	lnum;
    char	*errormsg;
    целое		above = eap->forceit;

    eap->regname = *line;

    если (eap->regname == '=')
    {
	char_u *p = skipwhite(line + 1);

	если (compile_expr0(&p, cctx) == FAIL)
	    вернуть NULL;
	line = p;
    }
    иначе если (eap->regname != NUL)
	++line;

    если (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    иначе
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	если (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    вернуть NULL;
	если (eap->addr_count == 0)
	    lnum = -1;
	иначе
	    lnum = eap->line2;
	если (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    вернуть line;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *line_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*line = line_arg;
    char_u	*p;
    целое		has_expr = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = NULL;
    char_u	*cmd_arg = NULL;

    если (cctx->ctx_skip == SKIP_YES)
	goto theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    если (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	если (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    если (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	целое	usefilter = FALSE;

	has_expr = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	если ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	если ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    если (eap->nextcmd != NULL)
		nextcmd = eap->nextcmd;
	}
	иначе если (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    если (*p != NUL)
		++p;
	    если (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    если (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	иначе если (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    целое	    flags = 0;  // unused
	    целое	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, line, &tofree, &flags);
	    если (tofree != NULL)
	    {
		line = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    если (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_expr = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	если (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    если ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	целое delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, NULL, NULL, NULL);
	если (*p == delim)
	    cmd_arg = p + 1;
    }

    если (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    если (cmd_arg != NULL)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, NULL, lookup_scriptitem, NULL);
	если (nea.cmdidx < CMD_SIZE)
	{
	    has_expr = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    если (has_expr)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    если (has_expr && (p = (char_u *)strstr((char *)eap->arg, "`=")) != NULL)
    {
	целое	count = 0;
	char_u	*start = skipwhite(line);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	для (;;)
	{
	    если (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++count;
	    }
	    p += 2;
	    если (compile_expr0(&p, cctx) == FAIL)
		вернуть NULL;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++count;
	    p = skipwhite(p);
	    если (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		вернуть NULL;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((char *)start, "`=");
	    если (p == NULL)
	    {
		если (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++count;
		}
		break;
	    }
	}
	generate_EXECCONCAT(cctx, count);
    }
    иначе
	generate_EXEC_copy(cctx, ISN_EXEC, line);

theend:
    если (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    вернуть nextcmd;
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
compile_script(char_u *line, cctx_T *cctx)
{
    если (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	если ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == NULL)
	    вернуть NULL;
	isn->isn_arg.string = vim_strsave(line);
    }
    вернуть (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *expr = (char_u *)strstr((char *)cmd, "\\=");

    если (expr != NULL)
    {
	целое delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), NULL, NULL, NULL);
	если (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    целое		expr_res;
	    целое		trailing_error;
	    целое		instr_count;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = NULL;
	    expr_res = compile_expr0(&cmd, cctx);
	    если (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    если (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		если (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		вернуть NULL;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_count = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_count].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    если ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == NULL)
	    {
		целое idx;

		для (idx = 0; idx < instr_count; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		вернуть NULL;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    если (*end == '&')
		++end;
	    пока (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    вернуть end;
	}
    }

    вернуть compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    если (lhs->lhs_name != NULL)
    {
	если (STRNCMP(arg, "END", 3) == 0)
	{
	    если (cctx->ctx_skip != SKIP_YES)
	    {
		если (lhs->lhs_append)
		{
		    // First load the current variable value.
		    если (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			вернуть NULL;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		если (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		если (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    если (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			вернуть NULL;
		}
		иначе если (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    вернуть NULL;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    вернуть arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	вернуть NULL;
    }

    если (arg[0] == '=' && arg[1] == '>')
    {
	целое	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	если (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	если (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    вернуть NULL;
	если (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    вернуть NULL;
	если (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	иначе
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    если (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		если (lhs->lhs_whole == NULL)
		    вернуть NULL;
	    }
	}

	вернуть arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    вернуть compile_exec(line, eap, cctx);
}

#если defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    если (isn == NULL)
	вернуть NULL;
    isn->isn_arg.number = eap->cmdidx;

    p = eap->arg;
    если (compile_expr0(&p, cctx) == FAIL)
	вернуть NULL;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    если (isn == NULL)
	вернуть NULL;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    если (isn->isn_arg.cexpr.cexpr_ref == NULL)
	вернуть NULL;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(line));

    вернуть p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, целое check_return_type, целое legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*stack_type;

    если (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	если (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_value_in_function_without_return_type));
	    вернуть NULL;
	}
	если (legacy)
	{
	    целое save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    если (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		вернуть NULL;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (void)skip_expr(&p, NULL);
	    cmdmod.cmod_flags = save_flags;
	}
	иначе
	{
	    // compile return argument into instructions
	    если (compile_expr0(&p, cctx) == FAIL)
		вернуть NULL;
	}

	если (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    stack_type = get_type_on_stack(cctx, 0);
	    если (check_type_is_value(stack_type) == FAIL)
		вернуть NULL;
	    если ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == NULL
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = stack_type;
	    }
	    иначе
	    {
		если (need_type(stack_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    вернуть NULL;
	    }
	}
    }
    иначе
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	если (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_value));
	    вернуть NULL;
	}

	если (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	иначе
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    если (compile_find_scope(NULL, NULL, NULL, NULL, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    если (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == NULL)
	вернуть NULL;

    // "return val | endif" is possible
    вернуть skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    целое
check_global_and_subst(char_u *cmd, char_u *arg)
{
    если (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != NULL)
    {
	semsg(_(e_separator_not_supported_str), arg);
	вернуть FAIL;
    }
    если (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	вернуть FAIL;
    }
    вернуть OK;
}


#endif  // defined(FEAT_EVAL)
