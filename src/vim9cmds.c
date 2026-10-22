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

#ถ้า defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    static จำนวนเต็ม
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    จำนวนเต็ม		idx = instr->ga_len;

    ขณะ (idx > 0)
    {
	ถ้า (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    continue;
	}
#ifdef FEAT_PROFILE
	ถ้า (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    continue;
	}
#endif
	ถ้า (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    continue;
	}
	bha_nga;
    }
    คืน idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    static void
unwind_locals(cctx_T *cctx, จำนวนเต็ม new_top, จำนวนเต็ม keep)
{
    ถ้า (cctx->ctx_locals.ga_len > new_top)
	สำหรับ (จำนวนเต็ม idx = new_top; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    ถ้า (!keep)
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
    จำนวนเต็ม
check_vim9_unlet(char_u *name)
{
    ถ้า (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	คืน FAIL;
    }

    ถ้า (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == NULL)
    {
	// "unlet s:var" is allowed in legacy script.
	ถ้า (*name == 's' && !in_vim9script())
	    คืน OK;
	semsg(_(e_cannot_unlet_str), name);
	คืน FAIL;
    }
    คืน OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    static จำนวนเต็ม
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    จำนวนเต็ม	    deep UNUSED,
    void    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    จำนวนเต็ม		cc = *name_end;
    จำนวนเต็ม		ret = OK;

    ถ้า (cctx->ctx_skip == SKIP_YES)
	คืน OK;

    *name_end = NUL;
    ถ้า (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    มิฉะนั้น ถ้า (vim_strchr(p, '.') != NULL || vim_strchr(p, '[') != NULL)
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
	ถ้า (ret == OK)
	{
	    ถ้า (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    มิฉะนั้น
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    มิฉะนั้น ถ้า (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    มิฉะนั้น
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    คืน ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    static จำนวนเต็ม
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    จำนวนเต็ม	    deep,
    void    *coookie)
{
    cctx_T	*cctx = coookie;
    จำนวนเต็ม		cc = *name_end;
    char_u	*p = lvp->ll_name;
    จำนวนเต็ม		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    var_na	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    จำนวนเต็ม		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    ถ้า (cctx->ctx_skip == SKIP_YES)
	คืน OK;

    ถ้า (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	คืน FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    ถ้า (p[1] != ':')
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
	จำนวนเต็ม	len = end - p;

	ถ้า (lookup_local(p, len, NULL, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    ถ้า (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		คืน FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(NULL, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	ถ้า (name == NULL)
	{
	    class_T *cl;
	    ถ้า (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		ถ้า (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (จำนวนเต็ม)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(NULL, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	ถ้า (name == NULL)
	{
	    // Can lockvar any function arg.
	    ถ้า (arg_exists(p, len, NULL, NULL, NULL, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	ถ้า (name == NULL)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    ถ้า (script_var_exists(p, len, cctx, NULL) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	ถ้า (name != NULL)
	{
#ifdef LOG_LOCKVAR
	    ch_log(NULL, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    ถ้า (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		คืน FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    ถ้า (buf == NULL)
	ret = FAIL;
    มิฉะนั้น
    {
	ถ้า (deep < 0)
	    vim_snprintf((var_na *)buf, len, "%s! %s", cmd, p);
	มิฉะนั้น
	    vim_snprintf((var_na *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(NULL, "LKVAR:    ... buf %s", buf);
#endif
	ถ้า (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	มิฉะนั้น
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    คืน ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    จำนวนเต็ม	    deep = 0;
    char_u  *p = arg;

    ถ้า (eap->cmdidx != CMD_unlet)
    {
	ถ้า (eap->forceit)
	    deep = -1;
	มิฉะนั้น ถ้า (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	มิฉะนั้น
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    คืน eap->nextcmd == NULL ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    static จำนวนเต็ม
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	จำนวนเต็ม	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    ถ้า (endlabel == NULL)
	คืน FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    ถ้า (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    มิฉะนั้น
	generate_JUMP(cctx, when, 0);
    คืน OK;
}

    static void
compile_fill_jump_to_end(endlabel_T **el, จำนวนเต็ม jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ขณะ (*el != NULL)
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
    ขณะ (*el != NULL)
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

    ถ้า (scope == NULL)
	คืน NULL;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_count = cctx->ctx_locals.ga_len;
    ถ้า (scope->se_outer != NULL)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    คืน scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    void
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    ถ้า (scope == NULL)
    {
	iemsg("calling drop_scope() without a scope");
	คืน;
    }
    cctx->ctx_scope = scope->se_outer;
    switch (scope->se_type)
    {
	pak_sa IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); bha_nga;
	pak_sa FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); bha_nga;
	pak_sa WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); bha_nga;
	pak_sa TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); bha_nga;
	pak_sa NO_SCOPE:
	pak_sa BLOCK_SCOPE:
	    bha_nga;
    }
    vim_free(scope);
}

    static จำนวนเต็ม
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ถ้า (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	คืน TRUE;
    }
    คืน FALSE;
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
    จำนวนเต็ม		instr_count = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ถ้า (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	คืน NULL;
    }
    ถ้า (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	คืน NULL;
    }
    ถ้า (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    มิฉะนั้น ถ้า (instr->ga_len == instr_count && ppconst.pp_used == 1)
    {
	จำนวนเต็ม error = FALSE;
	จำนวนเต็ม v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	ถ้า (error)
	    คืน NULL;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    มิฉะนั้น
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ถ้า (generate_ppconst(cctx, &ppconst) == FAIL)
	    คืน NULL;
	ถ้า (bool_on_stack(cctx) == FAIL)
	    คืน NULL;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    ถ้า (scope == NULL)
	คืน NULL;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    ถ้า (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    มิฉะนั้น
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    ถ้า (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    คืน p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    จำนวนเต็ม		instr_count;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    ถ้า (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	คืน NULL;
    }
    ถ้า (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	คืน NULL;
    }
    unwind_locals(cctx, scope->se_local_count, TRUE);
    ถ้า (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    ถ้า (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    ถ้า (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_expr_cctx(&p, cctx);
	คืน p;
    }

    ถ้า (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_expr_cctx(&p, cctx);
	คืน p;
    }

    ถ้า (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	จำนวนเต็ม	    moved_cmdmod = FALSE;
	จำนวนเต็ม	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	ถ้า (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    ถ้า (GA_GROW_FAILS(instr, 1))
		คืน NULL;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	ถ้า (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	ถ้า (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    คืน NULL;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	ถ้า (moved_cmdmod)
	    ++instr->ga_len;

	ถ้า (saved_debug)
	{
	    // move the debug instruction here
	    ถ้า (GA_GROW_FAILS(instr, 1))
		คืน NULL;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    ถ้า (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	ถ้า (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	ถ้า (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_count = instr->ga_len;
    ถ้า (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	คืน NULL;
    }
    cctx->ctx_skip = save_skip;
    ถ้า (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	คืน NULL;
    }
    ถ้า (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    มิฉะนั้น ถ้า (instr->ga_len == instr_count && ppconst.pp_used == 1)
    {
	จำนวนเต็ม error = FALSE;
	จำนวนเต็ม v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	ถ้า (error)
	{
	    clear_ppconst(&ppconst);
	    คืน NULL;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    มิฉะนั้น
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ถ้า (generate_ppconst(cctx, &ppconst) == FAIL)
	    คืน NULL;
	ถ้า (bool_on_stack(cctx) == FAIL)
	    คืน NULL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    คืน p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    ถ้า (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	คืน NULL;
    }
    ถ้า (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	คืน NULL;
    }
    unwind_locals(cctx, scope->se_local_count, TRUE);
    ถ้า (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    ถ้า (cctx->ctx_compile_type == CT_PROFILE)
    {
	ถ้า (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	ถ้า (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    ถ้า (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	ถ้า (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ถ้า (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		คืน NULL;
	}

	ถ้า (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ถ้า (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	ถ้า (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    คืน p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    ถ้า (scope == NULL || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	คืน NULL;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_count, TRUE);
    ถ้า (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    ถ้า (scope->se_u.se_if.is_if_label >= 0)
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
    ถ้า (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    คืน arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    static void
compile_fill_loop_info(loop_info_T *loop_info, จำนวนเต็ม funcref_idx, cctx_T *cctx)
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
compile_for_tuple_get_vartype(type_T *vartype, จำนวนเต็ม var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    ถ้า (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	คืน &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    ถ้า (member_type->tt_type == VAR_ANY)
	คืน &t_any;

    ถ้า (!var_list)
	// for x in tuple<...list<xxx>>
	คืน member_type;

    ถ้า (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	คืน member_type->tt_member;
    มิฉะนั้น ถ้า (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	คืน member_type->tt_args[0]->tt_member;

    คืน &t_any;
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
    จำนวนเต็ม		var_count = 0;
    จำนวนเต็ม		var_list = FALSE;
    จำนวนเต็ม		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    จำนวนเต็ม		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    จำนวนเต็ม		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    จำนวนเต็ม		idx;
    จำนวนเต็ม		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_count, &semicolon, FALSE);
    ถ้า (p == NULL)
	คืน NULL;
    ถ้า (var_count == 0)
	var_count = 1;
    มิฉะนั้น
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    ถ้า (may_get_next_line_error(wp, &p, cctx) == FAIL)
	คืน NULL;
    ถ้า (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	ถ้า (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	มิฉะนั้น
	    emsg(_(e_missing_in_after_for));
	คืน NULL;
    }
    wp = p + 2;
    ถ้า (may_get_next_line_error(wp, &p, cctx) == FAIL)
	คืน NULL;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    ถ้า (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    ถ้า (scope == NULL)
	คืน NULL;
    ถ้า (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	คืน NULL;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    ถ้า (loop_lvar == NULL)
    {
	drop_scope(cctx);
	คืน NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    ถ้า (funcref_lvar == NULL)
    {
	drop_scope(cctx);
	คืน NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    ถ้า (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	คืน NULL;
    }
    arg_end = arg;

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_stack(cctx, 0);
	ถ้า (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    คืน NULL;
	}

	ถ้า (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	มิฉะนั้น ถ้า (vartype->tt_type == VAR_BLOB)
	    item_type = &t_number;
	มิฉะนั้น ถ้า (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    ถ้า (!var_list)
		item_type = vartype->tt_member;
	    มิฉะนั้น ถ้า (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	มิฉะนั้น ถ้า (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_top_label = current_instr_idx(cctx);

	ถ้า (cctx->ctx_compile_type == CT_DEBUG)
	{
	    จำนวนเต็ม		save_prev_lnum = cctx->ctx_prev_lnum;
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
	ถ้า (var_list)
	{
	    generate_UNPACK(cctx, var_count, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_stack.ga_len;

	    // add type of the items
	    สำหรับ (idx = 0; idx < var_count; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		ถ้า (push_type_stack(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    คืน NULL;
		}
	    }
	}

	สำหรับ (idx = 0; idx < var_count; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    จำนวนเต็ม			opt_flags = 0;
	    จำนวนเต็ม			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    ถ้า (name == NULL)
		goto failed;
	    ถ้า (*skipwhite(p) == ':')
	    {
		ถ้า (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    goto failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		ถ้า (lhs_type == NULL || !valid_declaration_type(lhs_type))
		    goto failed;
	    }

	    ถ้า (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		goto failed;
	    ถ้า (dest != dest_local)
	    {
		ถ้า (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, NULL) == FAIL)
		    goto failed;
	    }
	    มิฉะนั้น ถ้า (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		ถ้า (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
		    goto failed;
	    }
	    มิฉะนั้น
	    {
		// Script var is not supported.
		ถ้า (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    goto failed;
		}

		ถ้า (!valid_varname(arg, (จำนวนเต็ม)varlen, FALSE))
		    goto failed;
		ถ้า (lookup_local(arg, varlen, NULL, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    goto failed;
		}

		// Reserve a variable to store "var".
		ถ้า (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		ถ้า (lhs_type == &t_any)
		    lhs_type = item_type;
		มิฉะนั้น ถ้า (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    goto failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		ถ้า (var_lvar == NULL)
		    // out of memory or used as an argument
		    goto failed;

		ถ้า (semicolon && idx == var_count - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, NULL);
	    }

	    ถ้า (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    คืน arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    คืน NULL;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    static จำนวนเต็ม
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    ถ้า (cctx->ctx_locals.ga_len > loop_info->li_local_count
	    && cctx->ctx_closure_count > loop_info->li_closure_count)
	คืน generate_ENDLOOP(cctx, loop_info);
    คืน OK;
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

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    ถ้า (scope == NULL || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	คืน NULL;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ถ้า (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    คืน NULL;

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
	ถ้า (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
	    คืน NULL;
    }

    vim_free(scope);

    คืน arg;
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
    จำนวนเต็ม		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    ถ้า (scope == NULL)
	คืน NULL;
    ถ้า (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	คืน NULL;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_top_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_number);
    ถ้า (funcref_lvar == NULL)
    {
	drop_scope(cctx);
	คืน NULL;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    ถ้า (compile_expr0(&p, cctx) == FAIL)
	คืน NULL;

    ถ้า (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	คืน NULL;
    }

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	ถ้า (bool_on_stack(cctx) == FAIL)
	    คืน FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	ถ้า (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    คืน FAIL;
    }

    คืน p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;
    ถ้า (scope == NULL || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	คืน NULL;
    }
    cctx->ctx_scope = scope->se_outer;
    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ถ้า (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    คืน NULL;

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

    คืน arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    จำนวนเต็ม
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    จำนวนเต็ม		prev_local_count = 0;

    CLEAR_POINTER(lvi);
    สำหรับ (;;)
    {
	loop_info_T	*loopinfo;
	จำนวนเต็ม		cur_local_last;
	จำนวนเต็ม		start_local_count;

	ขณะ (scope != NULL && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	ถ้า (scope == NULL)
	    bha_nga;

	ถ้า (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_count - 1;
	}
	มิฉะนั้น
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_count - 3;
	}

	start_local_count = loopinfo->li_local_count;
	ถ้า (cctx->ctx_locals.ga_len > start_local_count)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_count;
	    lvi->lvi_loop[loopinfo->li_depth].var_count =
			  (short)(cctx->ctx_locals.ga_len - start_local_count
							   - prev_local_count);
	    ถ้า (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_count = cctx->ctx_locals.ga_len - cur_local_last;
    }
    คืน lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    void
get_loop_var_idx(cctx_T *cctx, จำนวนเต็ม idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    ถ้า (get_loop_var_info(cctx, &lvi))
    {
	จำนวนเต็ม depth;

	สำหรับ (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    ถ้า (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_count)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		คืน;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    static จำนวนเต็ม
compile_find_scope(
	จำนวนเต็ม	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	จำนวนเต็ม	    *try_scopes,    // :try scopes encountered or NULL
	var_na	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    สำหรับ (;;)
    {
	ถ้า (scope == NULL)
	{
	    ถ้า (error != NULL)
		emsg(_(error));
	    คืน FAIL;
	}
	ถ้า (scope->se_type == FOR_SCOPE)
	{
	    ถ้า (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		คืน FAIL;
	    ถ้า (loop_label != NULL)
		*loop_label = scope->se_u.se_for.fs_top_label;
	    ถ้า (el != NULL)
		*el = &scope->se_u.se_for.fs_end_label;
	    bha_nga;
	}
	ถ้า (scope->se_type == WHILE_SCOPE)
	{
	    ถ้า (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		คืน FAIL;
	    ถ้า (loop_label != NULL)
		*loop_label = scope->se_u.se_while.ws_top_label;
	    ถ้า (el != NULL)
		*el = &scope->se_u.se_while.ws_end_label;
	    bha_nga;
	}
	ถ้า (try_scopes != NULL && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    คืน OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    จำนวนเต็ม		try_scopes = 0;
    จำนวนเต็ม		loop_label;

    ถ้า (compile_find_scope(&loop_label, NULL, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	คืน NULL;
    ถ้า (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    มิฉะนั้น
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    คืน arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    จำนวนเต็ม		try_scopes = 0;
    endlabel_T	**el;

    ถ้า (compile_find_scope(NULL, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	คืน NULL;

    ถ้า (cctx->ctx_skip == SKIP_YES)
	คืน arg;

    ถ้า (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    ถ้า (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	คืน NULL;

    คืน arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    ถ้า (new_scope(cctx, BLOCK_SCOPE) == NULL)
	คืน NULL;
    คืน skipwhite(arg + 1);
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

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    ถ้า (try_scope == NULL)
	คืน NULL;

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	ถ้า ((isn = generate_instr(cctx, ISN_TRY)) == NULL)
	    คืน NULL;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	ถ้า (isn->isn_arg.tryref.try_ref == NULL)
	    คืน NULL;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    ถ้า (scope == NULL)
	คืน NULL;

    คืน arg;
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

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    // end block scope from :try or :catch
    ถ้า (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ถ้า (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	คืน NULL;
    }

    ถ้า (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	คืน NULL;
    }
    ถ้า (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	ถ้า (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	ถ้า (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    คืน NULL;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ถ้า (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	ถ้า (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	ถ้า (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	ถ้า (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    ถ้า (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    มิฉะนั้น
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = NULL;
	จำนวนเต็ม	dropped = 0;
	จำนวนเต็ม	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, NULL);
	ถ้า (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    คืน NULL;
	}
	ถ้า (tofree == NULL)
	    len = (จำนวนเต็ม)(end - (p + 1));
	มิฉะนั้น
	    len = (จำนวนเต็ม)(end - tofree);
	pat = vim_strnsave(tofree == NULL ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	ถ้า (pat == NULL)
	    คืน NULL;
	ถ้า (generate_PUSHS(cctx, &pat) == FAIL)
	    คืน NULL;

	ถ้า (generate_COMPARE(cctx, EXPR_MATCH, FALSE) == FAIL)
	    คืน NULL;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	ถ้า (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    คืน NULL;
    }

    ถ้า (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == NULL)
	คืน NULL;

    ถ้า (new_scope(cctx, BLOCK_SCOPE) == NULL)
	คืน NULL;
    คืน p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    จำนวนเต็ม		this_instr;

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    // end block scope from :try or :catch
    ถ้า (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ถ้า (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	คืน NULL;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    ถ้า (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ถ้า (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    คืน NULL;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	ถ้า (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    ถ้า (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	ถ้า (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	ถ้า (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	ถ้า (generate_instr(cctx, ISN_FINALLY) == NULL)
	    คืน NULL;
    }

    คืน arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    ถ้า (misplaced_cmdmod(cctx))
	คืน NULL;

    // end block scope from :catch or :finally
    ถ้า (scope != NULL && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ถ้า (scope == NULL || scope->se_type != TRY_SCOPE)
    {
	ถ้า (scope == NULL)
	    emsg(_(e_endtry_without_try));
	มิฉะนั้น ถ้า (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	มิฉะนั้น ถ้า (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	มิฉะนั้น
	    emsg(_(e_missing_endif));
	คืน NULL;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	ถ้า (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    คืน NULL;
	}

#ifdef FEAT_PROFILE
	ถ้า (cctx->ctx_compile_type == CT_PROFILE
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

	ถ้า (scope->se_u.se_try.ts_catch_label != 0)
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

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	ถ้า (generate_instr(cctx, ISN_ENDTRY) == NULL)
	    คืน NULL;
#ifdef FEAT_PROFILE
	ถ้า (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    คืน arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    ถ้า (compile_expr0(&p, cctx) == FAIL)
	คืน NULL;
    ถ้า (cctx->ctx_skip == SKIP_YES)
	คืน p;
    ถ้า (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	คืน NULL;
    ถ้า (generate_instr_drop(cctx, ISN_THROW, 1) == NULL)
	คืน NULL;

    คืน p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    จำนวนเต็ม		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    ถ้า (compile_expr0(&p, cctx) == FAIL)
	คืน NULL;

    ถ้า (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	คืน NULL;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    คืน skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    จำนวนเต็ม
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    ถ้า (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	ถ้า (lvar == NULL)
	    คืน 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    คืน dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    จำนวนเต็ม		argcount = 0;
    จำนวนเต็ม		defer_var_idx;
    type_T	*type = NULL;
    จำนวนเต็ม		func_idx = -1;

    ถ้า (*arg == '(')
    {
	// a lambda function
	ถ้า (compile_lambda(&arg, cctx, NULL) != OK)
	    คืน NULL;
	paren = vim_strchr(arg, '(');
	ถ้า (paren == NULL)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    คืน NULL;
	}
    }
    มิฉะนั้น
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	ถ้า (paren == NULL)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    คืน NULL;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	ถ้า (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	มิฉะนั้น ถ้า (compile_expr0(&arg, cctx) == FAIL)
	    คืน NULL;
	*paren = '(';
    }

    // check for function type
    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_stack(cctx, 0);
	ถ้า (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    คืน NULL;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    ถ้า (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	คืน NULL;

    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	ถ้า (func_idx >= 0)
	{
	    type2_T	*argtypes = NULL;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    ถ้า (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		คืน NULL;
	}
	มิฉะนั้น ถ้า (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    คืน NULL;

	defer_var_idx = get_defer_var_idx(cctx);
	ถ้า (defer_var_idx == 0)
	    คืน NULL;
	ถ้า (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    คืน NULL;
    }

    คืน skipwhite(arg);
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
	จำนวนเต็ม	cmdidx,
	long	cmd_count UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*expr_start;
    จำนวนเต็ม		count = 0;
    จำนวนเต็ม		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    จำนวนเต็ม		r = OK;

    สำหรับ (;;)
    {
	ถ้า (ends_excmd2(prev, p))
	    bha_nga;
	expr_start = p;
	ถ้า (compile_expr0(&p, cctx) == FAIL)
	    คืน NULL;

	ถ้า (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_stack(cctx, 0);
	    ถ้า (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_value_str), expr_start);
		คืน NULL;
	    }
	}

	++count;
	prev = p;
	p = skipwhite(p);
    }

    ถ้า (count > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	ถ้า (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, count);
	มิฉะนั้น ถ้า (cmdidx == CMD_execute)
	    r = generate_MULT_EXPR(cctx, ISN_EXECUTE, count);
	มิฉะนั้น ถ้า (cmdidx == CMD_echomsg)
	    r = generate_MULT_EXPR(cctx, ISN_ECHOMSG, count);
#ifdef HAS_MESSAGE_WINDOW
	มิฉะนั้น ถ้า (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, count, cmd_count);
#endif
	มิฉะนั้น ถ้า (cmdidx == CMD_echoconsole)
	    r = generate_MULT_EXPR(cctx, ISN_ECHOCONSOLE, count);
	มิฉะนั้น
	    r = generate_MULT_EXPR(cctx, ISN_ECHOERR, count);

	cctx->ctx_lnum = save_lnum;
    }
    คืน r == OK ? p : NULL;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    static จำนวนเต็ม
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, NULL);
    char_u *p = skipdigits(eap->cmd);

    ถ้า (p == range_end)
	คืน FAIL;
    คืน generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, จำนวนเต็ม fixindent)
{
    char_u	*line = arg;
    linenr_T	lnum;
    var_na	*errormsg;
    จำนวนเต็ม		above = eap->forceit;

    eap->regname = *line;

    ถ้า (eap->regname == '=')
    {
	char_u *p = skipwhite(line + 1);

	ถ้า (compile_expr0(&p, cctx) == FAIL)
	    คืน NULL;
	line = p;
    }
    มิฉะนั้น ถ้า (eap->regname != NUL)
	++line;

    ถ้า (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    มิฉะนั้น
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	ถ้า (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    คืน NULL;
	ถ้า (eap->addr_count == 0)
	    lnum = -1;
	มิฉะนั้น
	    lnum = eap->line2;
	ถ้า (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    คืน line;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *line_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*line = line_arg;
    char_u	*p;
    จำนวนเต็ม		has_expr = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = NULL;
    char_u	*cmd_arg = NULL;

    ถ้า (cctx->ctx_skip == SKIP_YES)
	goto theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    ถ้า (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	ถ้า (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    ถ้า (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	จำนวนเต็ม	usefilter = FALSE;

	has_expr = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	ถ้า ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	ถ้า ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    ถ้า (eap->nextcmd != NULL)
		nextcmd = eap->nextcmd;
	}
	มิฉะนั้น ถ้า (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    ถ้า (*p != NUL)
		++p;
	    ถ้า (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    ถ้า (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	มิฉะนั้น ถ้า (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    จำนวนเต็ม	    flags = 0;  // unused
	    จำนวนเต็ม	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, line, &tofree, &flags);
	    ถ้า (tofree != NULL)
	    {
		line = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    ถ้า (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_expr = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	ถ้า (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    ถ้า ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	จำนวนเต็ม delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, NULL, NULL, NULL);
	ถ้า (*p == delim)
	    cmd_arg = p + 1;
    }

    ถ้า (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    ถ้า (cmd_arg != NULL)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, NULL, lookup_scriptitem, NULL);
	ถ้า (nea.cmdidx < CMD_SIZE)
	{
	    has_expr = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    ถ้า (has_expr)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    ถ้า (has_expr && (p = (char_u *)strstr((var_na *)eap->arg, "`=")) != NULL)
    {
	จำนวนเต็ม	count = 0;
	char_u	*start = skipwhite(line);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	สำหรับ (;;)
	{
	    ถ้า (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++count;
	    }
	    p += 2;
	    ถ้า (compile_expr0(&p, cctx) == FAIL)
		คืน NULL;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++count;
	    p = skipwhite(p);
	    ถ้า (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		คืน NULL;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((var_na *)start, "`=");
	    ถ้า (p == NULL)
	    {
		ถ้า (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++count;
		}
		bha_nga;
	    }
	}
	generate_EXECCONCAT(cctx, count);
    }
    มิฉะนั้น
	generate_EXEC_copy(cctx, ISN_EXEC, line);

theend:
    ถ้า (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    คืน nextcmd;
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
    ถ้า (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	ถ้า ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == NULL)
	    คืน NULL;
	isn->isn_arg.string = vim_strsave(line);
    }
    คืน (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *expr = (char_u *)strstr((var_na *)cmd, "\\=");

    ถ้า (expr != NULL)
    {
	จำนวนเต็ม delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), NULL, NULL, NULL);
	ถ้า (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    จำนวนเต็ม		expr_res;
	    จำนวนเต็ม		trailing_error;
	    จำนวนเต็ม		instr_count;
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
	    ถ้า (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    ถ้า (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		ถ้า (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		คืน NULL;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_count = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_count].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    ถ้า ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == NULL)
	    {
		จำนวนเต็ม idx;

		สำหรับ (idx = 0; idx < instr_count; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		คืน NULL;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    ถ้า (*end == '&')
		++end;
	    ขณะ (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    คืน end;
	}
    }

    คืน compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    ถ้า (lhs->lhs_name != NULL)
    {
	ถ้า (STRNCMP(arg, "END", 3) == 0)
	{
	    ถ้า (cctx->ctx_skip != SKIP_YES)
	    {
		ถ้า (lhs->lhs_append)
		{
		    // First load the current variable value.
		    ถ้า (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			คืน NULL;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		ถ้า (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		ถ้า (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    ถ้า (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			คืน NULL;
		}
		มิฉะนั้น ถ้า (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    คืน NULL;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    คืน arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	คืน NULL;
    }

    ถ้า (arg[0] == '=' && arg[1] == '>')
    {
	จำนวนเต็ม	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	ถ้า (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	ถ้า (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    คืน NULL;
	ถ้า (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    คืน NULL;
	ถ้า (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	มิฉะนั้น
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    ถ้า (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		ถ้า (lhs->lhs_whole == NULL)
		    คืน NULL;
	    }
	}

	คืน arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    คืน compile_exec(line, eap, cctx);
}

#ถ้า defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *line, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    ถ้า (isn == NULL)
	คืน NULL;
    isn->isn_arg.number = eap->cmdidx;

    p = eap->arg;
    ถ้า (compile_expr0(&p, cctx) == FAIL)
	คืน NULL;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    ถ้า (isn == NULL)
	คืน NULL;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    ถ้า (isn->isn_arg.cexpr.cexpr_ref == NULL)
	คืน NULL;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(line));

    คืน p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, จำนวนเต็ม check_return_type, จำนวนเต็ม legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*stack_type;

    ถ้า (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	ถ้า (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_value_in_function_without_return_type));
	    คืน NULL;
	}
	ถ้า (legacy)
	{
	    จำนวนเต็ม save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    ถ้า (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		คืน NULL;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (void)skip_expr(&p, NULL);
	    cmdmod.cmod_flags = save_flags;
	}
	มิฉะนั้น
	{
	    // compile return argument into instructions
	    ถ้า (compile_expr0(&p, cctx) == FAIL)
		คืน NULL;
	}

	ถ้า (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    stack_type = get_type_on_stack(cctx, 0);
	    ถ้า (check_type_is_value(stack_type) == FAIL)
		คืน NULL;
	    ถ้า ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == NULL
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = stack_type;
	    }
	    มิฉะนั้น
	    {
		ถ้า (need_type(stack_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    คืน NULL;
	    }
	}
    }
    มิฉะนั้น
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	ถ้า (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_value));
	    คืน NULL;
	}

	ถ้า (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	มิฉะนั้น
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    ถ้า (compile_find_scope(NULL, NULL, NULL, NULL, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    ถ้า (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == NULL)
	คืน NULL;

    // "return val | endif" is possible
    คืน skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    จำนวนเต็ม
check_global_and_subst(char_u *cmd, char_u *arg)
{
    ถ้า (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != NULL)
    {
	semsg(_(e_separator_not_supported_str), arg);
	คืน FAIL;
    }
    ถ้า (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	คืน FAIL;
    }
    คืน OK;
}


#endif  // defined(FEAT_EVAL)
