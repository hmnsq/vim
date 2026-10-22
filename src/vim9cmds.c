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
#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    स्थिर पूर्णाङ्क
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    पूर्णाङ्क		idx = instr->ga_len;

    यावत् (idx > 0)
    {
	यदि (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    अनुवर्त;
	}
#ifdef FEAT_PROFILE
	यदि (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    अनुवर्त;
	}
#endif
	यदि (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    अनुवर्त;
	}
	भङ्ग;
    }
    प्रत्यागम idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    स्थिर शून्य
unwind_locals(cctx_T *cctx, पूर्णाङ्क new_शीर्ष, पूर्णाङ्क keep)
{
    यदि (cctx->ctx_locals.ga_len > new_शीर्ष)
	प्रति (पूर्णाङ्क idx = new_शीर्ष; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    यदि (!keep)
	cctx->ctx_locals.ga_len = new_शीर्ष;
}

/*
 * Free all local variables.
 */
    शून्य
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    पूर्णाङ्क
check_vim9_unlet(char_u *name)
{
    यदि (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	प्रत्यागम FAIL;
    }

    यदि (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == रिक्त)
    {
	// "unlet s:var" is allowed in legacy script.
	यदि (*name == 's' && !in_vim9script())
	    प्रत्यागम OK;
	semsg(_(e_cannot_unlet_str), name);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    स्थिर पूर्णाङ्क
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    पूर्णाङ्क	    deep UNUSED,
    शून्य    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    पूर्णाङ्क		cc = *name_end;
    पूर्णाङ्क		ret = OK;

    यदि (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    *name_end = NUL;
    यदि (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    अन्यथा यदि (vim_strchr(p, '.') != रिक्त || vim_strchr(p, '[') != रिक्त)
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
	यदि (ret == OK)
	{
	    यदि (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    अन्यथा
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    अन्यथा यदि (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    अन्यथा
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    प्रत्यागम ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    स्थिर पूर्णाङ्क
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    पूर्णाङ्क	    deep,
    शून्य    *coookie)
{
    cctx_T	*cctx = coookie;
    पूर्णाङ्क		cc = *name_end;
    char_u	*p = lvp->ll_name;
    पूर्णाङ्क		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    वर्ण	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    पूर्णाङ्क		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    यदि (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    यदि (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	प्रत्यागम FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    यदि (p[1] != ':')
    {
	char_u *end = find_name_end(p, रिक्त, रिक्त, FNE_CHECK_START);

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

	char_u	*name = रिक्त;
	पूर्णाङ्क	len = end - p;

	यदि (lookup_local(p, len, रिक्त, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    यदि (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		प्रत्यागम FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(रिक्त, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	यदि (name == रिक्त)
	{
	    class_T *cl;
	    यदि (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		यदि (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (पूर्णाङ्क)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(रिक्त, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	यदि (name == रिक्त)
	{
	    // Can lockvar any function arg.
	    यदि (arg_exists(p, len, रिक्त, रिक्त, रिक्त, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	यदि (name == रिक्त)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    यदि (script_var_exists(p, len, cctx, रिक्त) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	यदि (name != रिक्त)
	{
#ifdef LOG_LOCKVAR
	    ch_log(रिक्त, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    यदि (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    यदि (buf == रिक्त)
	ret = FAIL;
    अन्यथा
    {
	यदि (deep < 0)
	    vim_snprintf((वर्ण *)buf, len, "%s! %s", cmd, p);
	अन्यथा
	    vim_snprintf((वर्ण *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(रिक्त, "LKVAR:    ... buf %s", buf);
#endif
	यदि (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	अन्यथा
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    प्रत्यागम ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    पूर्णाङ्क	    deep = 0;
    char_u  *p = arg;

    यदि (eap->cmdidx != CMD_unlet)
    {
	यदि (eap->forceit)
	    deep = -1;
	अन्यथा यदि (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	अन्यथा
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    प्रत्यागम eap->nextcmd == रिक्त ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    स्थिर पूर्णाङ्क
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	पूर्णाङ्क	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    यदि (endlabel == रिक्त)
	प्रत्यागम FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    यदि (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    अन्यथा
	generate_JUMP(cctx, when, 0);
    प्रत्यागम OK;
}

    स्थिर शून्य
compile_fill_jump_to_end(endlabel_T **el, पूर्णाङ्क jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    यावत् (*el != रिक्त)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    स्थिर शून्य
compile_free_jump_to_end(endlabel_T **el)
{
    यावत् (*el != रिक्त)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    स्थिर scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    यदि (scope == रिक्त)
	प्रत्यागम रिक्त;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_मोजणी = cctx->ctx_locals.ga_len;
    यदि (scope->se_outer != रिक्त)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    प्रत्यागम scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    शून्य
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    यदि (scope == रिक्त)
    {
	iemsg("calling drop_scope() without a scope");
	प्रत्यागम;
    }
    cctx->ctx_scope = scope->se_outer;
    विकल्प (scope->se_type)
    {
	पक्ष IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); भङ्ग;
	पक्ष FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); भङ्ग;
	पक्ष WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); भङ्ग;
	पक्ष TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); भङ्ग;
	पक्ष NO_SCOPE:
	पक्ष BLOCK_SCOPE:
	    भङ्ग;
    }
    vim_free(scope);
}

    स्थिर पूर्णाङ्क
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    यदि (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    पूर्णाङ्क		instr_मोजणी = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    यदि (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	प्रत्यागम रिक्त;
    }
    यदि (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	प्रत्यागम रिक्त;
    }
    यदि (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    अन्यथा यदि (instr->ga_len == instr_मोजणी && ppconst.pp_used == 1)
    {
	पूर्णाङ्क error = FALSE;
	पूर्णाङ्क v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	यदि (error)
	    प्रत्यागम रिक्त;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    अन्यथा
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	यदि (generate_ppconst(cctx, &ppconst) == FAIL)
	    प्रत्यागम रिक्त;
	यदि (bool_on_चळत(cctx) == FAIL)
	    प्रत्यागम रिक्त;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    यदि (scope == रिक्त)
	प्रत्यागम रिक्त;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    यदि (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    अन्यथा
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    यदि (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    प्रत्यागम p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    पूर्णाङ्क		instr_मोजणी;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    यदि (scope == रिक्त || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	प्रत्यागम रिक्त;
    }
    यदि (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	प्रत्यागम रिक्त;
    }
    unwind_locals(cctx, scope->se_local_मोजणी, TRUE);
    यदि (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    यदि (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    यदि (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_अभिव्यक्ती_cctx(&p, cctx);
	प्रत्यागम p;
    }

    यदि (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_अभिव्यक्ती_cctx(&p, cctx);
	प्रत्यागम p;
    }

    यदि (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	पूर्णाङ्क	    moved_cmdmod = FALSE;
	पूर्णाङ्क	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	यदि (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    यदि (GA_GROW_FAILS(instr, 1))
		प्रत्यागम रिक्त;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	यदि (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	यदि (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    प्रत्यागम रिक्त;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	यदि (moved_cmdmod)
	    ++instr->ga_len;

	यदि (saved_debug)
	{
	    // move the debug instruction here
	    यदि (GA_GROW_FAILS(instr, 1))
		प्रत्यागम रिक्त;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    यदि (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	यदि (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	यदि (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_मोजणी = instr->ga_len;
    यदि (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	प्रत्यागम रिक्त;
    }
    cctx->ctx_skip = save_skip;
    यदि (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	प्रत्यागम रिक्त;
    }
    यदि (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    अन्यथा यदि (instr->ga_len == instr_मोजणी && ppconst.pp_used == 1)
    {
	पूर्णाङ्क error = FALSE;
	पूर्णाङ्क v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	यदि (error)
	{
	    clear_ppconst(&ppconst);
	    प्रत्यागम रिक्त;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    अन्यथा
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	यदि (generate_ppconst(cctx, &ppconst) == FAIL)
	    प्रत्यागम रिक्त;
	यदि (bool_on_चळत(cctx) == FAIL)
	    प्रत्यागम रिक्त;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    प्रत्यागम p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    यदि (scope == रिक्त || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	प्रत्यागम रिक्त;
    }
    यदि (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	प्रत्यागम रिक्त;
    }
    unwind_locals(cctx, scope->se_local_मोजणी, TRUE);
    यदि (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    यदि (cctx->ctx_compile_type == CT_PROFILE)
    {
	यदि (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	यदि (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    यदि (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	यदि (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    यदि (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		प्रत्यागम रिक्त;
	}

	यदि (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    यदि (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	यदि (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    प्रत्यागम p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    यदि (scope == रिक्त || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	प्रत्यागम रिक्त;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_मोजणी, TRUE);
    यदि (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    यदि (scope->se_u.se_if.is_if_label >= 0)
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
    यदि (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    प्रत्यागम arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    स्थिर शून्य
compile_fill_loop_info(loop_info_T *loop_info, पूर्णाङ्क funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_मोजणी = cctx->ctx_locals.ga_len;
    loop_info->li_closure_मोजणी = cctx->ctx_closure_मोजणी;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    स्थिर type_T *
compile_for_tuple_get_vartype(type_T *vartype, पूर्णाङ्क var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    यदि (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	प्रत्यागम &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    यदि (member_type->tt_type == VAR_ANY)
	प्रत्यागम &t_any;

    यदि (!var_list)
	// for x in tuple<...list<xxx>>
	प्रत्यागम member_type;

    यदि (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	प्रत्यागम member_type->tt_member;
    अन्यथा यदि (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	प्रत्यागम member_type->tt_args[0]->tt_member;

    प्रत्यागम &t_any;
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
    char_u	*name = रिक्त;
    char_u	*p;
    char_u	*wp;
    पूर्णाङ्क		var_मोजणी = 0;
    पूर्णाङ्क		var_list = FALSE;
    पूर्णाङ्क		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    पूर्णाङ्क		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    पूर्णाङ्क		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_मोजणी, &semicolon, FALSE);
    यदि (p == रिक्त)
	प्रत्यागम रिक्त;
    यदि (var_मोजणी == 0)
	var_मोजणी = 1;
    अन्यथा
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    यदि (may_get_next_ओळ_error(wp, &p, cctx) == FAIL)
	प्रत्यागम रिक्त;
    यदि (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	यदि (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	अन्यथा
	    emsg(_(e_missing_in_after_for));
	प्रत्यागम रिक्त;
    }
    wp = p + 2;
    यदि (may_get_next_ओळ_error(wp, &p, cctx) == FAIL)
	प्रत्यागम रिक्त;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    यदि (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    यदि (scope == रिक्त)
	प्रत्यागम रिक्त;
    यदि (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	प्रत्यागम रिक्त;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_संख्या);
    यदि (loop_lvar == रिक्त)
    {
	drop_scope(cctx);
	प्रत्यागम रिक्त;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_संख्या);
    यदि (funcref_lvar == रिक्त)
    {
	drop_scope(cctx);
	प्रत्यागम रिक्त;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    यदि (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	प्रत्यागम रिक्त;
    }
    arg_end = arg;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_चळत(cctx, 0);
	यदि (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    प्रत्यागम रिक्त;
	}

	यदि (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	अन्यथा यदि (vartype->tt_type == VAR_BLOB)
	    item_type = &t_संख्या;
	अन्यथा यदि (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    यदि (!var_list)
		item_type = vartype->tt_member;
	    अन्यथा यदि (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	अन्यथा यदि (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_शीर्ष_label = current_instr_idx(cctx);

	यदि (cctx->ctx_compile_type == CT_DEBUG)
	{
	    पूर्णाङ्क		save_prev_lnum = cctx->ctx_prev_lnum;
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
	यदि (var_list)
	{
	    generate_UNPACK(cctx, var_मोजणी, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_चळत.ga_len;

	    // add type of the items
	    प्रति (idx = 0; idx < var_मोजणी; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		यदि (ढकला_type_चळत(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    प्रत्यागम रिक्त;
		}
	    }
	}

	प्रति (idx = 0; idx < var_मोजणी; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    पूर्णाङ्क			opt_flags = 0;
	    पूर्णाङ्क			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    यदि (name == रिक्त)
		गच्छ failed;
	    यदि (*skipwhite(p) == ':')
	    {
		यदि (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    गच्छ failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		यदि (lhs_type == रिक्त || !valid_declaration_type(lhs_type))
		    गच्छ failed;
	    }

	    यदि (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		गच्छ failed;
	    यदि (dest != dest_local)
	    {
		यदि (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, रिक्त) == FAIL)
		    गच्छ failed;
	    }
	    अन्यथा यदि (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		यदि (generate_instr_drop(cctx, ISN_DROP, 1) == रिक्त)
		    गच्छ failed;
	    }
	    अन्यथा
	    {
		// Script var is not supported.
		यदि (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    गच्छ failed;
		}

		यदि (!valid_varname(arg, (पूर्णाङ्क)varlen, FALSE))
		    गच्छ failed;
		यदि (lookup_local(arg, varlen, रिक्त, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    गच्छ failed;
		}

		// Reserve a variable to store "var".
		यदि (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		यदि (lhs_type == &t_any)
		    lhs_type = item_type;
		अन्यथा यदि (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    गच्छ failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		यदि (var_lvar == रिक्त)
		    // out of memory or used as an argument
		    गच्छ failed;

		यदि (semicolon && idx == var_मोजणी - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, रिक्त);
	    }

	    यदि (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    प्रत्यागम arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    प्रत्यागम रिक्त;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    स्थिर पूर्णाङ्क
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    यदि (cctx->ctx_locals.ga_len > loop_info->li_local_मोजणी
	    && cctx->ctx_closure_मोजणी > loop_info->li_closure_मोजणी)
	प्रत्यागम generate_ENDLOOP(cctx, loop_info);
    प्रत्यागम OK;
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

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    यदि (scope == रिक्त || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	प्रत्यागम रिक्त;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    यदि (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	यदि (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    प्रत्यागम रिक्त;

	unwind_locals(cctx, scope->se_local_मोजणी, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_शीर्ष_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_शीर्ष_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	यदि (generate_instr_drop(cctx, ISN_DROP, 1) == रिक्त)
	    प्रत्यागम रिक्त;
    }

    vim_free(scope);

    प्रत्यागम arg;
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
    पूर्णाङ्क		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    यदि (scope == रिक्त)
	प्रत्यागम रिक्त;
    यदि (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	प्रत्यागम रिक्त;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_शीर्ष_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_संख्या);
    यदि (funcref_lvar == रिक्त)
    {
	drop_scope(cctx);
	प्रत्यागम रिक्त;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    यदि (compile_expr0(&p, cctx) == FAIL)
	प्रत्यागम रिक्त;

    यदि (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	प्रत्यागम रिक्त;
    }

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि (bool_on_चळत(cctx) == FAIL)
	    प्रत्यागम FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	यदि (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;
    यदि (scope == रिक्त || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	प्रत्यागम रिक्त;
    }
    cctx->ctx_scope = scope->se_outer;
    यदि (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	यदि (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    प्रत्यागम रिक्त;

	unwind_locals(cctx, scope->se_local_मोजणी, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_शीर्ष_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    प्रत्यागम arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    पूर्णाङ्क
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    पूर्णाङ्क		prev_local_मोजणी = 0;

    CLEAR_POINTER(lvi);
    प्रति (;;)
    {
	loop_info_T	*loopinfo;
	पूर्णाङ्क		cur_local_last;
	पूर्णाङ्क		start_local_मोजणी;

	यावत् (scope != रिक्त && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	यदि (scope == रिक्त)
	    भङ्ग;

	यदि (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_मोजणी - 1;
	}
	अन्यथा
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_मोजणी - 3;
	}

	start_local_मोजणी = loopinfo->li_local_मोजणी;
	यदि (cctx->ctx_locals.ga_len > start_local_मोजणी)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_मोजणी;
	    lvi->lvi_loop[loopinfo->li_depth].var_मोजणी =
			  (short)(cctx->ctx_locals.ga_len - start_local_मोजणी
							   - prev_local_मोजणी);
	    यदि (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_मोजणी = cctx->ctx_locals.ga_len - cur_local_last;
    }
    प्रत्यागम lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    शून्य
get_loop_var_idx(cctx_T *cctx, पूर्णाङ्क idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    यदि (get_loop_var_info(cctx, &lvi))
    {
	पूर्णाङ्क depth;

	प्रति (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    यदि (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_मोजणी)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		प्रत्यागम;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    स्थिर पूर्णाङ्क
compile_find_scope(
	पूर्णाङ्क	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	पूर्णाङ्क	    *try_scopes,    // :try scopes encountered or NULL
	वर्ण	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    प्रति (;;)
    {
	यदि (scope == रिक्त)
	{
	    यदि (error != रिक्त)
		emsg(_(error));
	    प्रत्यागम FAIL;
	}
	यदि (scope->se_type == FOR_SCOPE)
	{
	    यदि (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		प्रत्यागम FAIL;
	    यदि (loop_label != रिक्त)
		*loop_label = scope->se_u.se_for.fs_शीर्ष_label;
	    यदि (el != रिक्त)
		*el = &scope->se_u.se_for.fs_end_label;
	    भङ्ग;
	}
	यदि (scope->se_type == WHILE_SCOPE)
	{
	    यदि (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		प्रत्यागम FAIL;
	    यदि (loop_label != रिक्त)
		*loop_label = scope->se_u.se_while.ws_शीर्ष_label;
	    यदि (el != रिक्त)
		*el = &scope->se_u.se_while.ws_end_label;
	    भङ्ग;
	}
	यदि (try_scopes != रिक्त && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    प्रत्यागम OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    पूर्णाङ्क		try_scopes = 0;
    पूर्णाङ्क		loop_label;

    यदि (compile_find_scope(&loop_label, रिक्त, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	प्रत्यागम रिक्त;
    यदि (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    अन्यथा
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    प्रत्यागम arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    पूर्णाङ्क		try_scopes = 0;
    endlabel_T	**el;

    यदि (compile_find_scope(रिक्त, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	प्रत्यागम रिक्त;

    यदि (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम arg;

    यदि (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    यदि (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	प्रत्यागम रिक्त;

    प्रत्यागम arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    यदि (new_scope(cctx, BLOCK_SCOPE) == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    शून्य
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_मोजणी, TRUE);
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

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    यदि (try_scope == रिक्त)
	प्रत्यागम रिक्त;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	यदि ((isn = generate_instr(cctx, ISN_TRY)) == रिक्त)
	    प्रत्यागम रिक्त;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	यदि (isn->isn_arg.tryref.try_ref == रिक्त)
	    प्रत्यागम रिक्त;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    यदि (scope == रिक्त)
	प्रत्यागम रिक्त;

    प्रत्यागम arg;
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

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    // end block scope from :try or :catch
    यदि (scope != रिक्त && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    यदि (scope == रिक्त || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	प्रत्यागम रिक्त;
    }

    यदि (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	प्रत्यागम रिक्त;
    }
    यदि (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	यदि (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	यदि (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    प्रत्यागम रिक्त;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	यदि (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	यदि (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	यदि (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	यदि (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    यदि (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    अन्यथा
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = रिक्त;
	पूर्णाङ्क	dropped = 0;
	पूर्णाङ्क	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, रिक्त);
	यदि (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    प्रत्यागम रिक्त;
	}
	यदि (tofree == रिक्त)
	    len = (पूर्णाङ्क)(end - (p + 1));
	अन्यथा
	    len = (पूर्णाङ्क)(end - tofree);
	pat = vim_strnsave(tofree == रिक्त ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	यदि (pat == रिक्त)
	    प्रत्यागम रिक्त;
	यदि (generate_PUSHS(cctx, &pat) == FAIL)
	    प्रत्यागम रिक्त;

	यदि (generate_COMPARE(cctx, अभिव्यक्ती_MATCH, FALSE) == FAIL)
	    प्रत्यागम रिक्त;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	यदि (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    प्रत्यागम रिक्त;
    }

    यदि (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == रिक्त)
	प्रत्यागम रिक्त;

    यदि (new_scope(cctx, BLOCK_SCOPE) == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    पूर्णाङ्क		this_instr;

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    // end block scope from :try or :catch
    यदि (scope != रिक्त && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    यदि (scope == रिक्त || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	प्रत्यागम रिक्त;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    यदि (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	यदि (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    प्रत्यागम रिक्त;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	यदि (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    यदि (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	यदि (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	यदि (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	यदि (generate_instr(cctx, ISN_FINALLY) == रिक्त)
	    प्रत्यागम रिक्त;
    }

    प्रत्यागम arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    यदि (misplaced_cmdmod(cctx))
	प्रत्यागम रिक्त;

    // end block scope from :catch or :finally
    यदि (scope != रिक्त && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    यदि (scope == रिक्त || scope->se_type != TRY_SCOPE)
    {
	यदि (scope == रिक्त)
	    emsg(_(e_endtry_without_try));
	अन्यथा यदि (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	अन्यथा यदि (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	अन्यथा
	    emsg(_(e_missing_endif));
	प्रत्यागम रिक्त;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    प्रत्यागम रिक्त;
	}

#ifdef FEAT_PROFILE
	यदि (cctx->ctx_compile_type == CT_PROFILE
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

	यदि (scope->se_u.se_try.ts_catch_label != 0)
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

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	यदि (generate_instr(cctx, ISN_ENDTRY) == रिक्त)
	    प्रत्यागम रिक्त;
#ifdef FEAT_PROFILE
	यदि (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    प्रत्यागम arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    यदि (compile_expr0(&p, cctx) == FAIL)
	प्रत्यागम रिक्त;
    यदि (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम p;
    यदि (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	प्रत्यागम रिक्त;
    यदि (generate_instr_drop(cctx, ISN_THROW, 1) == रिक्त)
	प्रत्यागम रिक्त;

    प्रत्यागम p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    पूर्णाङ्क		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    यदि (compile_expr0(&p, cctx) == FAIL)
	प्रत्यागम रिक्त;

    यदि (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	प्रत्यागम रिक्त;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    प्रत्यागम skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    पूर्णाङ्क
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    यदि (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	यदि (lvar == रिक्त)
	    प्रत्यागम 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    प्रत्यागम dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    पूर्णाङ्क		argcount = 0;
    पूर्णाङ्क		defer_var_idx;
    type_T	*type = रिक्त;
    पूर्णाङ्क		func_idx = -1;

    यदि (*arg == '(')
    {
	// a lambda function
	यदि (compile_lambda(&arg, cctx, रिक्त) != OK)
	    प्रत्यागम रिक्त;
	paren = vim_strchr(arg, '(');
	यदि (paren == रिक्त)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    प्रत्यागम रिक्त;
	}
    }
    अन्यथा
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	यदि (paren == रिक्त)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    प्रत्यागम रिक्त;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	यदि (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	अन्यथा यदि (compile_expr0(&arg, cctx) == FAIL)
	    प्रत्यागम रिक्त;
	*paren = '(';
    }

    // check for function type
    यदि (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_चळत(cctx, 0);
	यदि (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    प्रत्यागम रिक्त;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    यदि (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	प्रत्यागम रिक्त;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि (func_idx >= 0)
	{
	    type2_T	*argtypes = रिक्त;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    यदि (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		प्रत्यागम रिक्त;
	}
	अन्यथा यदि (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    प्रत्यागम रिक्त;

	defer_var_idx = get_defer_var_idx(cctx);
	यदि (defer_var_idx == 0)
	    प्रत्यागम रिक्त;
	यदि (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    प्रत्यागम रिक्त;
    }

    प्रत्यागम skipwhite(arg);
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
compile_mult_अभिव्यक्ती(
	char_u	*arg,
	पूर्णाङ्क	cmdidx,
	long	cmd_मोजणी UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*अभिव्यक्ती_start;
    पूर्णाङ्क		मोजणी = 0;
    पूर्णाङ्क		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    पूर्णाङ्क		r = OK;

    प्रति (;;)
    {
	यदि (ends_excmd2(prev, p))
	    भङ्ग;
	अभिव्यक्ती_start = p;
	यदि (compile_expr0(&p, cctx) == FAIL)
	    प्रत्यागम रिक्त;

	यदि (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_चळत(cctx, 0);
	    यदि (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_मूल्य_str), अभिव्यक्ती_start);
		प्रत्यागम रिक्त;
	    }
	}

	++मोजणी;
	prev = p;
	p = skipwhite(p);
    }

    यदि (मोजणी > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	यदि (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, मोजणी);
	अन्यथा यदि (cmdidx == CMD_execute)
	    r = generate_MULT_अभिव्यक्ती(cctx, ISN_EXECUTE, मोजणी);
	अन्यथा यदि (cmdidx == CMD_echomsg)
	    r = generate_MULT_अभिव्यक्ती(cctx, ISN_ECHOMSG, मोजणी);
#ifdef HAS_MESSAGE_WINDOW
	अन्यथा यदि (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, मोजणी, cmd_मोजणी);
#endif
	अन्यथा यदि (cmdidx == CMD_echoconsole)
	    r = generate_MULT_अभिव्यक्ती(cctx, ISN_ECHOCONSOLE, मोजणी);
	अन्यथा
	    r = generate_MULT_अभिव्यक्ती(cctx, ISN_ECHOERR, मोजणी);

	cctx->ctx_lnum = save_lnum;
    }
    प्रत्यागम r == OK ? p : रिक्त;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    स्थिर पूर्णाङ्क
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, रिक्त);
    char_u *p = skipdigits(eap->cmd);

    यदि (p == range_end)
	प्रत्यागम FAIL;
    प्रत्यागम generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, पूर्णाङ्क fixindent)
{
    char_u	*ओळ = arg;
    linenr_T	lnum;
    वर्ण	*errormsg;
    पूर्णाङ्क		above = eap->forceit;

    eap->regname = *ओळ;

    यदि (eap->regname == '=')
    {
	char_u *p = skipwhite(ओळ + 1);

	यदि (compile_expr0(&p, cctx) == FAIL)
	    प्रत्यागम रिक्त;
	ओळ = p;
    }
    अन्यथा यदि (eap->regname != NUL)
	++ओळ;

    यदि (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    अन्यथा
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	यदि (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    प्रत्यागम रिक्त;
	यदि (eap->addr_मोजणी == 0)
	    lnum = -1;
	अन्यथा
	    lnum = eap->line2;
	यदि (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    प्रत्यागम ओळ;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *ओळ_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*ओळ = ओळ_arg;
    char_u	*p;
    पूर्णाङ्क		has_अभिव्यक्ती = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = रिक्त;
    char_u	*cmd_arg = रिक्त;

    यदि (cctx->ctx_skip == SKIP_YES)
	गच्छ theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    यदि (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	यदि (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    यदि (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	पूर्णाङ्क	usefilter = FALSE;

	has_अभिव्यक्ती = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	यदि ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	यदि ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    यदि (eap->nextcmd != रिक्त)
		nextcmd = eap->nextcmd;
	}
	अन्यथा यदि (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    यदि (*p != NUL)
		++p;
	    यदि (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    यदि (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	अन्यथा यदि (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    पूर्णाङ्क	    flags = 0;  // unused
	    पूर्णाङ्क	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, ओळ, &tofree, &flags);
	    यदि (tofree != रिक्त)
	    {
		ओळ = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    यदि (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_अभिव्यक्ती = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	यदि (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    यदि ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	पूर्णाङ्क delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, रिक्त, रिक्त, रिक्त);
	यदि (*p == delim)
	    cmd_arg = p + 1;
    }

    यदि (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    यदि (cmd_arg != रिक्त)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, रिक्त, lookup_scriptitem, रिक्त);
	यदि (nea.cmdidx < CMD_SIZE)
	{
	    has_अभिव्यक्ती = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    यदि (has_अभिव्यक्ती)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    यदि (has_अभिव्यक्ती && (p = (char_u *)strstr((वर्ण *)eap->arg, "`=")) != रिक्त)
    {
	पूर्णाङ्क	मोजणी = 0;
	char_u	*start = skipwhite(ओळ);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	प्रति (;;)
	{
	    यदि (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++मोजणी;
	    }
	    p += 2;
	    यदि (compile_expr0(&p, cctx) == FAIL)
		प्रत्यागम रिक्त;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++मोजणी;
	    p = skipwhite(p);
	    यदि (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		प्रत्यागम रिक्त;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((वर्ण *)start, "`=");
	    यदि (p == रिक्त)
	    {
		यदि (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++मोजणी;
		}
		भङ्ग;
	    }
	}
	generate_EXECCONCAT(cctx, मोजणी);
    }
    अन्यथा
	generate_EXEC_copy(cctx, ISN_EXEC, ओळ);

theend:
    यदि (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    प्रत्यागम nextcmd;
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
compile_script(char_u *ओळ, cctx_T *cctx)
{
    यदि (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	यदि ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == रिक्त)
	    प्रत्यागम रिक्त;
	isn->isn_arg.string = vim_strsave(ओळ);
    }
    प्रत्यागम (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *अभिव्यक्ती = (char_u *)strstr((वर्ण *)cmd, "\\=");

    यदि (अभिव्यक्ती != रिक्त)
    {
	पूर्णाङ्क delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), रिक्त, रिक्त, रिक्त);
	यदि (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    पूर्णाङ्क		अभिव्यक्ती_res;
	    पूर्णाङ्क		trailing_error;
	    पूर्णाङ्क		instr_मोजणी;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = रिक्त;
	    अभिव्यक्ती_res = compile_expr0(&cmd, cctx);
	    यदि (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    यदि (अभिव्यक्ती_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		यदि (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		प्रत्यागम रिक्त;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_मोजणी = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_मोजणी].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    यदि ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == रिक्त)
	    {
		पूर्णाङ्क idx;

		प्रति (idx = 0; idx < instr_मोजणी; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		प्रत्यागम रिक्त;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    यदि (*end == '&')
		++end;
	    यावत् (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    प्रत्यागम end;
	}
    }

    प्रत्यागम compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *ओळ, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    यदि (lhs->lhs_name != रिक्त)
    {
	यदि (STRNCMP(arg, "END", 3) == 0)
	{
	    यदि (cctx->ctx_skip != SKIP_YES)
	    {
		यदि (lhs->lhs_append)
		{
		    // First load the current variable value.
		    यदि (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			प्रत्यागम रिक्त;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		यदि (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		यदि (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    यदि (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			प्रत्यागम रिक्त;
		}
		अन्यथा यदि (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    प्रत्यागम रिक्त;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    प्रत्यागम arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	प्रत्यागम रिक्त;
    }

    यदि (arg[0] == '=' && arg[1] == '>')
    {
	पूर्णाङ्क	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	यदि (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	यदि (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    प्रत्यागम रिक्त;
	यदि (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम रिक्त;
	यदि (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	अन्यथा
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    यदि (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		यदि (lhs->lhs_whole == रिक्त)
		    प्रत्यागम रिक्त;
	    }
	}

	प्रत्यागम arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    प्रत्यागम compile_exec(ओळ, eap, cctx);
}

#यदि defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *ओळ, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    यदि (isn == रिक्त)
	प्रत्यागम रिक्त;
    isn->isn_arg.संख्या = eap->cmdidx;

    p = eap->arg;
    यदि (compile_expr0(&p, cctx) == FAIL)
	प्रत्यागम रिक्त;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    यदि (isn == रिक्त)
	प्रत्यागम रिक्त;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    यदि (isn->isn_arg.cexpr.cexpr_ref == रिक्त)
	प्रत्यागम रिक्त;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(ओळ));

    प्रत्यागम p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, पूर्णाङ्क check_return_type, पूर्णाङ्क legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*चळत_type;

    यदि (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	यदि (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_मूल्य_in_function_without_return_type));
	    प्रत्यागम रिक्त;
	}
	यदि (legacy)
	{
	    पूर्णाङ्क save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    यदि (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम रिक्त;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (शून्य)skip_अभिव्यक्ती(&p, रिक्त);
	    cmdmod.cmod_flags = save_flags;
	}
	अन्यथा
	{
	    // compile return argument into instructions
	    यदि (compile_expr0(&p, cctx) == FAIL)
		प्रत्यागम रिक्त;
	}

	यदि (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    चळत_type = get_type_on_चळत(cctx, 0);
	    यदि (check_type_is_मूल्य(चळत_type) == FAIL)
		प्रत्यागम रिक्त;
	    यदि ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == रिक्त
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = चळत_type;
	    }
	    अन्यथा
	    {
		यदि (need_type(चळत_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    प्रत्यागम रिक्त;
	    }
	}
    }
    अन्यथा
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	यदि (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_मूल्य));
	    प्रत्यागम रिक्त;
	}

	यदि (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	अन्यथा
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    यदि (compile_find_scope(रिक्त, रिक्त, रिक्त, रिक्त, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    यदि (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == रिक्त)
	प्रत्यागम रिक्त;

    // "return val | endif" is possible
    प्रत्यागम skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    पूर्णाङ्क
check_global_and_subst(char_u *cmd, char_u *arg)
{
    यदि (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != रिक्त)
    {
	semsg(_(e_separator_not_supported_str), arg);
	प्रत्यागम FAIL;
    }
    यदि (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}


#endif  // defined(FEAT_EVAL)
