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

#وصف_ڪريو USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ساكن صحيح
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحيح		idx = instr->ga_len;

    طالما (idx > 0)
    {
	إذا (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_معلومات)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    استمر;
	}
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (((isn_T *)instr->ga_معلومات)[idx - 1].isn_type == ISN_PROF_شروعات)
	{
	    --idx;
	    استمر;
	}
#جيڪڏهن_پورو
	إذا (((isn_T *)instr->ga_معلومات)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    استمر;
	}
	اكسر;
    }
    أرجع idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ساكن فراغ
unwind_locals(cctx_T *cctx, صحيح new_چوٽي, صحيح keep)
{
    إذا (cctx->ctx_locals.ga_len > new_چوٽي)
	لكل (صحيح idx = new_چوٽي; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_معلومات) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    إذا (!keep)
	cctx->ctx_locals.ga_len = new_چوٽي;
}

/*
 * Free all local variables.
 */
    فراغ
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    صحيح
check_vim9_unlet(char_u *name)
{
    إذا (*name == NUL)
    {
	semsg(_(e_دليل_required_for_str), "unlet");
	أرجع FAIL;
    }

    إذا (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == عدم)
    {
	// "unlet s:var" is allowed in legacy script.
	إذا (*name == 's' && !اندريون_vim9script())
	    أرجع OK;
	semsg(_(e_cannot_unlet_str), name);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ساكن صحيح
compile_unlet(
    lval_T  *lvp,
    char_u  *name_پڇاڙي,
    exarg_T *eap,
    صحيح	    deep اڻ_استعمال_ٿيل,
    فراغ    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    صحيح		cc = *name_پڇاڙي;
    صحيح		ret = OK;

    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    *name_پڇاڙي = NUL;
    إذا (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    وإلا إذا (vim_strchr(p, '.') != عدم || vim_strchr(p, '[') != عدم)
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
	إذا (ret == OK)
	{
	    إذا (!lhs.lhs_has_اشاريو)
	    {
		semsg(_(e_cannot_unlet_imported_شي_str), p);
		ret = FAIL;
	    }
	    وإلا
		ret = compile_سونپيو_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    وإلا إذا (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    وإلا
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_پڇاڙي = cc;
    أرجع ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ساكن صحيح
compile_تالو_unlock(
    lval_T  *lvp,
    char_u  *name_پڇاڙي,
    exarg_T *eap,
    صحيح	    deep,
    فراغ    *coookie)
{
    cctx_T	*cctx = coookie;
    صحيح		cc = *name_پڇاڙي;
    char_u	*p = lvp->ll_name;
    صحيح		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    حرف	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    صحيح		is_arg = FALSE;

#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
    ch_log(عدم, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#جيڪڏهن_پورو

    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    إذا (*p == NUL)
    {
	semsg(_(e_دليل_required_for_str), cmd);
	أرجع FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    إذا (p[1] != ':')
    {
	char_u *پڇاڙي = ڳوليو_name_پڇاڙي(p, عدم, عدم, FNE_CHECK_شروعات);

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

	char_u	*name = عدم;
	صحيح	len = پڇاڙي - p;

	إذا (lookup_local(p, len, عدم, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    إذا (*پڇاڙي != '.' && *پڇاڙي != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_تالو_unlock_local_variable));
		أرجع FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
	    ch_log(عدم, "LKVAR:    ... lookup_local: name %s", name);
#جيڪڏهن_پورو
	}
	إذا (name == عدم)
	{
	    class_T *cl;
	    إذا (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		إذا (*پڇاڙي != '.' && *پڇاڙي != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (صحيح)cl->class_name.ڊيگهائي;
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
		    ch_log(عدم, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#جيڪڏهن_پورو
		}
	    }
	}
	إذا (name == عدم)
	{
	    // Can lockvar any function arg.
	    إذا (arg_exists(p, len, عدم, عدم, عدم, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
		ch_log(عدم, "LKVAR:    ... arg_exists: name %s", name);
#جيڪڏهن_پورو
	    }
	}
	إذا (name == عدم)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    إذا (script_var_exists(p, len, cctx, عدم) == OK
		&& (*پڇاڙي == '.' || *پڇاڙي == '['))
	    {
		name = p;
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
		ch_log(عدم, "LKVAR:    ... script_var_exists: name %s", name);
#جيڪڏهن_پورو
	    }
	}
	إذا (name != عدم)
	{
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
	    ch_log(عدم, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#جيڪڏهن_پورو
	    إذا (compile_ڀريو(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_پڇاڙي = NUL;
    size_t len = name_پڇاڙي - p + 20;
    buf = alloc(len);
    إذا (buf == عدم)
	ret = FAIL;
    وإلا
    {
	إذا (deep < 0)
	    vim_snprintf((حرف *)buf, len, "%s! %s", cmd, p);
	وإلا
	    vim_snprintf((حرف *)buf, len, "%s %d %s", cmd, deep, p);
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
	ch_log(عدم, "LKVAR:    ... buf %s", buf);
#جيڪڏهن_پورو
	إذا (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	وإلا
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_پڇاڙي = cc;
    }
    أرجع ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    صحيح	    deep = 0;
    char_u  *p = arg;

    إذا (eap->cmdidx != CMD_unlet)
    {
	إذا (eap->forceit)
	    deep = -1;
	وإلا إذا (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	وإلا
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_تالو_unlock,
	    cctx);
    أرجع eap->nextcmd == عدم ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ساكن صحيح
compile_jump_منزل_پڇاڙي(
	endlabel_T  **el,
	jumpwhen_T  when,
	صحيح	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    إذا (endlabel == عدم)
	أرجع FAIL;
    endlabel->el_ايندڙ = *el;
    *el = endlabel;
    endlabel->el_پڇاڙي_label = instr->ga_len;

    إذا (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    وإلا
	generate_JUMP(cctx, when, 0);
    أرجع OK;
}

    ساكن فراغ
compile_fill_jump_منزل_پڇاڙي(endlabel_T **el, صحيح jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    طالما (*el != عدم)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_معلومات) + cur->el_پڇاڙي_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_ايندڙ;
	vim_free(cur);
    }
}

    ساكن فراغ
compile_free_jump_منزل_پڇاڙي(endlabel_T **el)
{
    طالما (*el != عدم)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_ايندڙ;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ساكن scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    إذا (scope == عدم)
	أرجع عدم;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_ڳڻپ = cctx->ctx_locals.ga_len;
    إذا (scope->se_outer != عدم)
	scope->se_loop_اونهائي = scope->se_outer->se_loop_اونهائي;
    أرجع scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    فراغ
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    إذا (scope == عدم)
    {
	iemsg("calling drop_scope() without a scope");
	أرجع;
    }
    cctx->ctx_scope = scope->se_outer;
    بدل (scope->se_type)
    {
	حالة IF_SCOPE:
	    compile_free_jump_منزل_پڇاڙي(&scope->se_u.se_if.is_پڇاڙي_label); اكسر;
	حالة FOR_SCOPE:
	    compile_free_jump_منزل_پڇاڙي(&scope->se_u.se_for.fs_پڇاڙي_label); اكسر;
	حالة WHILE_SCOPE:
	    compile_free_jump_منزل_پڇاڙي(&scope->se_u.se_while.ws_پڇاڙي_label); اكسر;
	حالة TRY_SCOPE:
	    compile_free_jump_منزل_پڇاڙي(&scope->se_u.se_try.ts_پڇاڙي_label); اكسر;
	حالة NO_SCOPE:
	حالة بلاڪ_SCOPE:
	    اكسر;
    }
    vim_free(scope);
}

    ساكن صحيح
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    إذا (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	أرجع TRUE;
    }
    أرجع FALSE;
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
    صحيح		instr_ڳڻپ = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_ميدان(ppconst);
    إذا (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	أرجع عدم;
    }
    إذا (!پڇاڙيون_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	أرجع عدم;
    }
    إذا (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    وإلا إذا (instr->ga_len == instr_ڳڻپ && ppconst.pp_استعمال_ٿيل == 1)
    {
	صحيح error = FALSE;
	صحيح v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	إذا (error)
	    أرجع عدم;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    وإلا
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	إذا (generate_ppconst(cctx, &ppconst) == FAIL)
	    أرجع عدم;
	إذا (bool_on_ڍير(cctx) == FAIL)
	    أرجع عدم;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    إذا (scope == عدم)
	أرجع عدم;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    إذا (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    وإلا
	scope->se_u.se_if.is_if_label = -1;

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_پڇاڙي);
	cctx->ctx_skip = SKIP_YES;
    }
#جيڪڏهن_پورو

    أرجع p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    صحيح		instr_ڳڻپ;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    إذا (scope == عدم || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	أرجع عدم;
    }
    إذا (scope->se_u.se_if.is_ڏٺل_else)
    {
	emsg(_(e_elseif_after_else));
	أرجع عدم;
    }
    unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);
    إذا (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    إذا (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_ڏٺل_skip_not = TRUE;
    }
    إذا (scope->se_u.se_if.is_ڏٺل_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_اظهار_cctx(&p, cctx);
	أرجع p;
    }

    إذا (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_اظهار_cctx(&p, cctx);
	أرجع p;
    }

    إذا (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	صحيح	    moved_cmdmod = FALSE;
	صحيح	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	إذا (((isn_T *)instr->ga_معلومات)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    إذا (GA_GROW_FAILS(instr, 1))
		أرجع عدم;
	    ((isn_T *)instr->ga_معلومات)[instr->ga_len] =
				  ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	إذا (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_معلومات)[instr->ga_len];
	    saved_debug = TRUE;
	}

	إذا (compile_jump_منزل_پڇاڙي(&scope->se_u.se_if.is_پڇاڙي_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    أرجع عدم;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	إذا (moved_cmdmod)
	    ++instr->ga_len;

	إذا (saved_debug)
	{
	    // move the debug instruction here
	    إذا (GA_GROW_FAILS(instr, 1))
		أرجع عدم;
	    ((isn_T *)instr->ga_معلومات)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_ميدان(ppconst);
    إذا (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_شروعات);
#جيڪڏهن_پورو
	إذا (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_ڳڻپ = instr->ga_len;
    إذا (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	أرجع عدم;
    }
    cctx->ctx_skip = save_skip;
    إذا (!پڇاڙيون_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	أرجع عدم;
    }
    إذا (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    وإلا إذا (instr->ga_len == instr_ڳڻپ && ppconst.pp_استعمال_ٿيل == 1)
    {
	صحيح error = FALSE;
	صحيح v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	إذا (error)
	{
	    clear_ppconst(&ppconst);
	    أرجع عدم;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    وإلا
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	إذا (generate_ppconst(cctx, &ppconst) == FAIL)
	    أرجع عدم;
	إذا (bool_on_ڍير(cctx) == FAIL)
	    أرجع عدم;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    أرجع p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    إذا (scope == عدم || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	أرجع عدم;
    }
    إذا (scope->se_u.se_if.is_ڏٺل_else)
    {
	emsg(_(e_گڻو_else));
	أرجع عدم;
    }
    unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);
    إذا (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_ڏٺل_else = TRUE;

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (cctx->ctx_compile_type == CT_PROFILE)
    {
	إذا (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_شروعات)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	إذا (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_ڏٺل_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_پڇاڙي);
	    generate_instr(cctx, ISN_PROF_شروعات);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#جيڪڏهن_پورو

    إذا (!scope->se_u.se_if.is_ڏٺل_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	إذا (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    إذا (!cctx->ctx_had_return
		    && compile_jump_منزل_پڇاڙي(&scope->se_u.se_if.is_پڇاڙي_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		أرجع عدم;
	}

	إذا (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    إذا (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	إذا (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    أرجع p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    إذا (scope == عدم || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	أرجع عدم;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);
    إذا (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    إذا (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_منزل_پڇاڙي(&ifscope->is_پڇاڙي_label, instr->ga_len, cctx);

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    إذا (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
					    && scope->se_skip_save != SKIP_YES)
    {
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_شروعات);
    }
#جيڪڏهن_پورو
    cctx->ctx_skip = scope->se_skip_save;

    // If all the blocks end in :return and there is an :else then the
    // had_return flag is set.
    cctx->ctx_had_return = ifscope->is_had_return && ifscope->is_ڏٺل_else;

    drop_scope(cctx);
    أرجع arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ساكن فراغ
compile_fill_loop_info(loop_info_T *loop_info, صحيح funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_ڳڻپ = cctx->ctx_locals.ga_len;
    loop_info->li_گهيرو_ڳڻپ = cctx->ctx_گهيرو_ڳڻپ;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ساكن type_T *
compile_for_tuple_get_vartype(type_T *vartype, صحيح var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    إذا (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	أرجع &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_دليلون[0]->tt_member;
    إذا (member_type->tt_type == VAR_ANY)
	أرجع &t_any;

    إذا (!var_list)
	// for x in tuple<...list<xxx>>
	أرجع member_type;

    إذا (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	أرجع member_type->tt_member;
    وإلا إذا (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	أرجع member_type->tt_دليلون[0]->tt_member;

    أرجع &t_any;
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
compile_for(char_u *arg_شروعات, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_پڇاڙي;
    char_u	*name = عدم;
    char_u	*p;
    char_u	*wp;
    صحيح		var_ڳڻپ = 0;
    صحيح		var_list = FALSE;
    صحيح		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    صحيح		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    صحيح		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*شي_type = &t_any;
    صحيح		idx;
    صحيح		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_شروعات, TRUE, &var_ڳڻپ, &semicolon, FALSE);
    إذا (p == عدم)
	أرجع عدم;
    إذا (var_ڳڻپ == 0)
	var_ڳڻپ = 1;
    وإلا
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    إذا (may_get_ايندڙ_سٽ_error(wp, &p, cctx) == FAIL)
	أرجع عدم;
    إذا (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	إذا (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	وإلا
	    emsg(_(e_missing_اندريون_after_for));
	أرجع عدم;
    }
    wp = p + 2;
    إذا (may_get_ايندڙ_سٽ_error(wp, &p, cctx) == FAIL)
	أرجع عدم;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    إذا (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    إذا (scope == عدم)
	أرجع عدم;
    إذا (scope->se_loop_اونهائي == MAX_LOOP_اونهائي)
    {
	emsg(_(e_loop_nesting_too_deep));
	أرجع عدم;
    }
    ++scope->se_loop_اونهائي;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, سونپيو_VAR, &t_عدد);
    إذا (loop_lvar == عدم)
    {
	drop_scope(cctx);
	أرجع عدم;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, سونپيو_VAR, &t_عدد);
    إذا (funcref_lvar == عدم)
    {
	drop_scope(cctx);
	أرجع عدم;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    إذا (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	أرجع عدم;
    }
    arg_پڇاڙي = arg;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ڍير(cctx, 0);
	إذا (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    أرجع عدم;
	}

	إذا (vartype->tt_type == VAR_STRING)
	    شي_type = &t_string;
	وإلا إذا (vartype->tt_type == VAR_BLOB)
	    شي_type = &t_عدد;
	وإلا إذا (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    إذا (!var_list)
		شي_type = vartype->tt_member;
	    وإلا إذا (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		شي_type = vartype->tt_member->tt_member;
	}
	وإلا إذا (vartype->tt_type == VAR_TUPLE)
	    شي_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_چوٽي_label = current_instr_idx(cctx);

	إذا (cctx->ctx_compile_type == CT_DEBUG)
	{
	    صحيح		save_prev_lnum = cctx->ctx_prev_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_prev_lnum = prev_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_نالا_len;
	    cctx->ctx_prev_lnum = save_prev_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_شروعات;
	إذا (var_list)
	{
	    generate_UNPACK(cctx, var_ڳڻپ, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ڍير.ga_len;

	    // add type of the items
	    لكل (idx = 0; idx < var_ڳڻپ; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : شي_type;

		إذا (ڌڪو_type_ڍير(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    أرجع عدم;
		}
	    }
	}

	لكل (idx = 0; idx < var_ڳڻپ; ++idx)
	{
	    سونپيو_dest_T	dest = dest_local;
	    صحيح			opt_flags = 0;
	    صحيح			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    إذا (name == عدم)
		اذهب failed;
	    إذا (*skipwhite(p) == ':')
	    {
		إذا (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    اذهب failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		إذا (lhs_type == عدم || !valid_declaration_type(lhs_type))
		    اذهب failed;
	    }

	    إذا (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		اذهب failed;
	    إذا (dest != dest_local)
	    {
		إذا (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, عدم) == FAIL)
		    اذهب failed;
	    }
	    وإلا إذا (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		إذا (generate_instr_drop(cctx, ISN_DROP, 1) == عدم)
		    اذهب failed;
	    }
	    وإلا
	    {
		// Script var is not supported.
		إذا (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_اندريون_for_loop));
		    اذهب failed;
		}

		إذا (!valid_varname(arg, (صحيح)varlen, FALSE))
		    اذهب failed;
		إذا (lookup_local(arg, varlen, عدم, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    اذهب failed;
		}

		// Reserve a variable to store "var".
		إذا (var_list)
		{
		    where.wt_اشاريو = idx + 1;
		    where.wt_قسم = WT_VARIABLE;
		}
		إذا (lhs_type == &t_any)
		    lhs_type = شي_type;
		وإلا إذا (شي_type != &t_unknown
			&& need_type_where(شي_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    اذهب failed;
		var_lvar = reserve_local(cctx, arg, varlen, سونپيو_FINAL,
								     lhs_type);
		إذا (var_lvar == عدم)
		    // out of memory or used as an argument
		    اذهب failed;

		إذا (semicolon && idx == var_ڳڻپ - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, عدم);
	    }

	    إذا (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_اونهائي = scope->se_loop_اونهائي - 1;
    }

    أرجع arg_پڇاڙي;

failed:
    vim_free(name);
    drop_scope(cctx);
    أرجع عدم;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ساكن صحيح
compile_loop_پڇاڙي(loop_info_T *loop_info, cctx_T *cctx)
{
    إذا (cctx->ctx_locals.ga_len > loop_info->li_local_ڳڻپ
	    && cctx->ctx_گهيرو_ڳڻپ > loop_info->li_گهيرو_ڳڻپ)
	أرجع generate_ENDLOOP(cctx, loop_info);
    أرجع OK;
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

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    إذا (scope == عدم || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	أرجع عدم;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    إذا (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	إذا (compile_loop_پڇاڙي(&forscope->fs_loop_info, cctx) == FAIL)
	    أرجع عدم;

	unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_چوٽي_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_معلومات) + forscope->fs_چوٽي_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_پڇاڙي = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_منزل_پڇاڙي(&forscope->fs_پڇاڙي_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	إذا (generate_instr_drop(cctx, ISN_DROP, 1) == عدم)
	    أرجع عدم;
    }

    vim_free(scope);

    أرجع arg;
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
    صحيح		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    إذا (scope == عدم)
	أرجع عدم;
    إذا (scope->se_loop_اونهائي == MAX_LOOP_اونهائي)
    {
	emsg(_(e_loop_nesting_too_deep));
	أرجع عدم;
    }
    ++scope->se_loop_اونهائي;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_چوٽي_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, سونپيو_VAR, &t_عدد);
    إذا (funcref_lvar == عدم)
    {
	drop_scope(cctx);
	أرجع عدم;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_اونهائي = scope->se_loop_اونهائي - 1;

    // compile "expr"
    إذا (compile_expr0(&p, cctx) == FAIL)
	أرجع عدم;

    إذا (!پڇاڙيون_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	أرجع عدم;
    }

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا (bool_on_ڍير(cctx) == FAIL)
	    أرجع FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	إذا (compile_jump_منزل_پڇاڙي(&whilescope->ws_پڇاڙي_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    أرجع FAIL;
    }

    أرجع p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;
    إذا (scope == عدم || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	أرجع عدم;
    }
    cctx->ctx_scope = scope->se_outer;
    إذا (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	إذا (compile_loop_پڇاڙي(&whilescope->ws_loop_info, cctx) == FAIL)
	    أرجع عدم;

	unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_پڇاڙي(cctx, cctx->ctx_lnum);
#جيڪڏهن_پورو

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_چوٽي_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_منزل_پڇاڙي(&scope->se_u.se_while.ws_پڇاڙي_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    أرجع arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    صحيح
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    صحيح		prev_local_ڳڻپ = 0;

    CLEAR_اشارو(lvi);
    لكل (;;)
    {
	loop_info_T	*loopinfo;
	صحيح		cur_local_آخري;
	صحيح		شروعات_local_ڳڻپ;

	طالما (scope != عدم && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	إذا (scope == عدم)
	    اكسر;

	إذا (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_آخري = loopinfo->li_local_ڳڻپ - 1;
	}
	وإلا
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_آخري = loopinfo->li_local_ڳڻپ - 3;
	}

	شروعات_local_ڳڻپ = loopinfo->li_local_ڳڻپ;
	إذا (cctx->ctx_locals.ga_len > شروعات_local_ڳڻپ)
	{
	    lvi->lvi_loop[loopinfo->li_اونهائي].var_idx =
						      (ننڍو)شروعات_local_ڳڻپ;
	    lvi->lvi_loop[loopinfo->li_اونهائي].var_ڳڻپ =
			  (ننڍو)(cctx->ctx_locals.ga_len - شروعات_local_ڳڻپ
							   - prev_local_ڳڻپ);
	    إذا (lvi->lvi_اونهائي == 0)
		lvi->lvi_اونهائي = loopinfo->li_اونهائي + 1;
	}

	scope = scope->se_outer;
	prev_local_ڳڻپ = cctx->ctx_locals.ga_len - cur_local_آخري;
    }
    أرجع lvi->lvi_اونهائي > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    فراغ
get_loop_var_idx(cctx_T *cctx, صحيح idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_اونهائي = -1;
    lvar->lv_loop_idx = -1;
    إذا (get_loop_var_info(cctx, &lvi))
    {
	صحيح اونهائي;

	لكل (اونهائي = lvi.lvi_اونهائي - 1; اونهائي >= 0; --اونهائي)
	    إذا (idx >= lvi.lvi_loop[اونهائي].var_idx
		    && idx < lvi.lvi_loop[اونهائي].var_idx
					       + lvi.lvi_loop[اونهائي].var_ڳڻپ)
	    {
		lvar->lv_loop_اونهائي = اونهائي;
		lvar->lv_loop_idx = lvi.lvi_loop[اونهائي].var_idx;
		أرجع;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ساكن صحيح
compile_ڳوليو_scope(
	صحيح	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	صحيح	    *try_scopes,    // :try scopes encountered or NULL
	حرف	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    لكل (;;)
    {
	إذا (scope == عدم)
	{
	    إذا (error != عدم)
		emsg(_(error));
	    أرجع FAIL;
	}
	إذا (scope->se_type == FOR_SCOPE)
	{
	    إذا (compile_loop_پڇاڙي(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		أرجع FAIL;
	    إذا (loop_label != عدم)
		*loop_label = scope->se_u.se_for.fs_چوٽي_label;
	    إذا (el != عدم)
		*el = &scope->se_u.se_for.fs_پڇاڙي_label;
	    اكسر;
	}
	إذا (scope->se_type == WHILE_SCOPE)
	{
	    إذا (compile_loop_پڇاڙي(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		أرجع FAIL;
	    إذا (loop_label != عدم)
		*loop_label = scope->se_u.se_while.ws_چوٽي_label;
	    إذا (el != عدم)
		*el = &scope->se_u.se_while.ws_پڇاڙي_label;
	    اكسر;
	}
	إذا (try_scopes != عدم && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    أرجع OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    صحيح		try_scopes = 0;
    صحيح		loop_label;

    إذا (compile_ڳوليو_scope(&loop_label, عدم, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	أرجع عدم;
    إذا (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    وإلا
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    أرجع arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    صحيح		try_scopes = 0;
    endlabel_T	**el;

    إذا (compile_ڳوليو_scope(عدم, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	أرجع عدم;

    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع arg;

    إذا (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    إذا (compile_jump_منزل_پڇاڙي(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	أرجع عدم;

    أرجع arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_بلاڪ(char_u *arg, cctx_T *cctx)
{
    إذا (new_scope(cctx, بلاڪ_SCOPE) == عدم)
	أرجع عدم;
    أرجع skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    فراغ
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_ڳڻپ, TRUE);
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

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    إذا (try_scope == عدم)
	أرجع عدم;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	إذا ((isn = generate_instr(cctx, ISN_TRY)) == عدم)
	    أرجع عدم;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	إذا (isn->isn_arg.tryref.try_ref == عدم)
	    أرجع عدم;
    }

    // scope for the try block itself
    scope = new_scope(cctx, بلاڪ_SCOPE);
    إذا (scope == عدم)
	أرجع عدم;

    أرجع arg;
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

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    // end block scope from :try or :catch
    إذا (scope != عدم && scope->se_type == بلاڪ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    إذا (scope == عدم || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	أرجع عدم;
    }

    إذا (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_ڪوڊ_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	أرجع عدم;
    }
    إذا (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	// the profile-start should be after the jump
	إذا (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_شروعات)
	    --instr->ga_len;
#جيڪڏهن_پورو
	// Jump from end of previous block to :finally or :endtry
	إذا (compile_jump_منزل_پڇاڙي(&scope->se_u.se_try.ts_پڇاڙي_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    أرجع عدم;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_try.ts_try_label;
	إذا (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	إذا (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_پڇاڙي);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_شروعات);
	}
#جيڪڏهن_پورو
	إذا (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    إذا (پڇاڙيون_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    وإلا
    {
	char_u *پڇاڙي;
	char_u *pat;
	char_u *tofree = عدم;
	صحيح	dropped = 0;
	صحيح	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	پڇاڙي = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, عدم);
	إذا (*پڇاڙي != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    أرجع عدم;
	}
	إذا (tofree == عدم)
	    len = (صحيح)(پڇاڙي - (p + 1));
	وإلا
	    len = (صحيح)(پڇاڙي - tofree);
	pat = vim_strnsave(tofree == عدم ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	إذا (pat == عدم)
	    أرجع عدم;
	إذا (generate_PUSHS(cctx, &pat) == FAIL)
	    أرجع عدم;

	إذا (generate_COMPARE(cctx, اظهار_ميلاپ, FALSE) == FAIL)
	    أرجع عدم;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	إذا (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    أرجع عدم;
    }

    إذا (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == عدم)
	أرجع عدم;

    إذا (new_scope(cctx, بلاڪ_SCOPE) == عدم)
	أرجع عدم;
    أرجع p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    صحيح		this_instr;

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    // end block scope from :try or :catch
    إذا (scope != عدم && scope->se_type == بلاڪ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    إذا (scope == عدم || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	أرجع عدم;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    إذا (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_try.ts_try_label;
	إذا (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_گڻو_finally));
	    أرجع عدم;
	}

	this_instr = instr->ga_len;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_معلومات)[this_instr - 1]
						   .isn_type == ISN_PROF_شروعات)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    إذا (this_instr > 0 && ((isn_T *)instr->ga_معلومات)[this_instr - 1]
						     .isn_type == ISN_PROF_پڇاڙي)
		--this_instr;
	}
#جيڪڏهن_پورو

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_منزل_پڇاڙي(&scope->se_u.se_try.ts_پڇاڙي_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	إذا (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	إذا (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	إذا (generate_instr(cctx, ISN_FINALLY) == عدم)
	    أرجع عدم;
    }

    أرجع arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    إذا (misplaced_cmdmod(cctx))
	أرجع عدم;

    // end block scope from :catch or :finally
    إذا (scope != عدم && scope->se_type == بلاڪ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    إذا (scope == عدم || scope->se_type != TRY_SCOPE)
    {
	إذا (scope == عدم)
	    emsg(_(e_endtry_without_try));
	وإلا إذا (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	وإلا إذا (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	وإلا
	    emsg(_(e_missing_endif));
	أرجع عدم;
    }

    try_isn = ((isn_T *)instr->ga_معلومات) + scope->se_u.se_try.ts_try_label;
    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    أرجع عدم;
	}

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_معلومات)[instr->ga_len - 1]
						.isn_type == ISN_PROF_شروعات)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#جيڪڏهن_پورو

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_منزل_پڇاڙي(&scope->se_u.se_try.ts_پڇاڙي_label,
							  instr->ga_len, cctx);

	إذا (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_معلومات)
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

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	إذا (generate_instr(cctx, ISN_ENDTRY) == عدم)
	    أرجع عدم;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_شروعات);
#جيڪڏهن_پورو
    }
    أرجع arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    إذا (compile_expr0(&p, cctx) == FAIL)
	أرجع عدم;
    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع p;
    إذا (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	أرجع عدم;
    إذا (generate_instr_drop(cctx, ISN_THROW, 1) == عدم)
	أرجع عدم;

    أرجع p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    صحيح		name_only;
    ڊگهو	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    إذا (compile_expr0(&p, cctx) == FAIL)
	أرجع عدم;

    إذا (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_اظهار_without_effect_str), arg);
	أرجع عدم;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    أرجع skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    صحيح
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_معلومات)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    إذا (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	إذا (lvar == عدم)
	    أرجع 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    أرجع dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_شروعات, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_شروعات;
    صحيح		argcount = 0;
    صحيح		defer_var_idx;
    type_T	*type = عدم;
    صحيح		func_idx = -1;

    إذا (*arg == '(')
    {
	// a lambda function
	إذا (compile_lambda(&arg, cctx, عدم) != OK)
	    أرجع عدم;
	paren = vim_strchr(arg, '(');
	إذا (paren == عدم)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    أرجع عدم;
	}
    }
    وإلا
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	إذا (paren == عدم)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    أرجع عدم;
	}
	*paren = NUL;
	func_idx = ڳوليو_اندروني_func(arg);
	إذا (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)اندروني_func_name(func_idx),
		    &t_func_any, FALSE);
	وإلا إذا (compile_expr0(&arg, cctx) == FAIL)
	    أرجع عدم;
	*paren = '(';
    }

    // check for function type
    إذا (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ڍير(cctx, 0);
	إذا (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    أرجع عدم;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    إذا (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	أرجع عدم;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا (func_idx >= 0)
	{
	    type2_T	*argtypes = عدم;
	    type2_T	shuffled_argtypes[MAX_FUNC_دليلون];

	    إذا (check_اندروني_func_دليلون(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		أرجع عدم;
	}
	وإلا إذا (check_func_دليلون_from_type(cctx, type, argcount, TRUE,
		    arg_شروعات) == FAIL)
	    أرجع عدم;

	defer_var_idx = get_defer_var_idx(cctx);
	إذا (defer_var_idx == 0)
	    أرجع عدم;
	إذا (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    أرجع عدم;
    }

    أرجع skipwhite(arg);
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
compile_mult_اظهار(
	char_u	*arg,
	صحيح	cmdidx,
	ڊگهو	cmd_ڳڻپ اڻ_استعمال_ٿيل,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*اظهار_شروعات;
    صحيح		ڳڻپ = 0;
    صحيح		شروعات_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    صحيح		r = OK;

    لكل (;;)
    {
	إذا (پڇاڙيون_excmd2(prev, p))
	    اكسر;
	اظهار_شروعات = p;
	إذا (compile_expr0(&p, cctx) == FAIL)
	    أرجع عدم;

	إذا (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ڍير(cctx, 0);
	    إذا (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_اظهار_does_not_نتيجو_اندريون_قدر_str), اظهار_شروعات);
		أرجع عدم;
	    }
	}

	++ڳڻپ;
	prev = p;
	p = skipwhite(p);
    }

    إذا (ڳڻپ > 0)
    {
	ڊگهو save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = شروعات_ctx_lnum;

	إذا (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, ڳڻپ);
	وإلا إذا (cmdidx == CMD_execute)
	    r = generate_MULT_اظهار(cctx, ISN_EXECUTE, ڳڻپ);
	وإلا إذا (cmdidx == CMD_echomsg)
	    r = generate_MULT_اظهار(cctx, ISN_ECHOMSG, ڳڻپ);
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	وإلا إذا (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, ڳڻپ, cmd_ڳڻپ);
#جيڪڏهن_پورو
	وإلا إذا (cmdidx == CMD_echoconsole)
	    r = generate_MULT_اظهار(cctx, ISN_ECHOCONSOLE, ڳڻپ);
	وإلا
	    r = generate_MULT_اظهار(cctx, ISN_ECHOERR, ڳڻپ);

	cctx->ctx_lnum = save_lnum;
    }
    أرجع r == OK ? p : عدم;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ساكن صحيح
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_پڇاڙي = skip_range(eap->cmd, TRUE, عدم);
    char_u *p = skipdigits(eap->cmd);

    إذا (p == range_پڇاڙي)
	أرجع FAIL;
    أرجع generate_RANGE(cctx, vim_strnsave(eap->cmd, range_پڇاڙي - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, صحيح fixindent)
{
    char_u	*سٽ = arg;
    linenr_T	lnum;
    حرف	*errormsg;
    صحيح		above = eap->forceit;

    eap->regname = *سٽ;

    إذا (eap->regname == '=')
    {
	char_u *p = skipwhite(سٽ + 1);

	إذا (compile_expr0(&p, cctx) == FAIL)
	    أرجع عدم;
	سٽ = p;
    }
    وإلا إذا (eap->regname != NUL)
	++سٽ;

    إذا (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    وإلا
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	إذا (parse_cmd_پتو(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    أرجع عدم;
	إذا (eap->addr_ڳڻپ == 0)
	    lnum = -1;
	وإلا
	    lnum = eap->line2;
	إذا (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    أرجع سٽ;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *سٽ_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*سٽ = سٽ_arg;
    char_u	*p;
    صحيح		has_اظهار = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = عدم;
    char_u	*cmd_arg = عدم;

    إذا (cctx->ctx_skip == SKIP_YES)
	اذهب theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    إذا (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_معلومات) + instr->ga_len - 1;

	إذا (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    إذا (eap->cmdidx >= 0 && eap->cmdidx < CMD_سائيز)
    {
	ڊگهو	argt = eap->argt;
	صحيح	usefilter = FALSE;

	has_اظهار = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	إذا ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	إذا ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    إذا (eap->nextcmd != عدم)
		nextcmd = eap->nextcmd;
	}
	وإلا إذا (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    إذا (*p != NUL)
		++p;
	    إذا (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    إذا (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	وإلا إذا (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    صحيح	    flags = 0;  //اڻ_استعمال_ٿيل
	    صحيح	    شروعات_lnum = SOURCING_LNUM;

	    CLEAR_ميدان(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_بلاڪ(&ea, سٽ, &tofree, &flags);
	    إذا (tofree != عدم)
	    {
		سٽ = p;
		SOURCING_LNUM = شروعات_lnum;
	    }
	}
    }

    إذا (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_اظهار = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	إذا (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    إذا ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	صحيح delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, عدم, عدم, عدم);
	إذا (*p == delim)
	    cmd_arg = p + 1;
    }

    إذا (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    إذا (cmd_arg != عدم)
    {
	exarg_T nea;

	CLEAR_ميدان(nea);
	nea.cmd = cmd_arg;
	p = ڳوليو_ex_command(&nea, عدم, lookup_scriptitem, عدم);
	إذا (nea.cmdidx < CMD_سائيز)
	{
	    has_اظهار = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    إذا (has_اظهار)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    إذا (has_اظهار && (p = (char_u *)لڙي_ڳولا((حرف *)eap->arg, "`=")) != عدم)
    {
	صحيح	ڳڻپ = 0;
	char_u	*شروعات = skipwhite(سٽ);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	لكل (;;)
	{
	    إذا (p > شروعات)
	    {
		char_u *val = vim_strnsave(شروعات, p - شروعات);

		generate_PUSHS(cctx, &val);
		++ڳڻپ;
	    }
	    p += 2;
	    إذا (compile_expr0(&p, cctx) == FAIL)
		أرجع عدم;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++ڳڻپ;
	    p = skipwhite(p);
	    إذا (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		أرجع عدم;
	    }
	    شروعات = p + 1;

	    p = (char_u *)لڙي_ڳولا((حرف *)شروعات, "`=");
	    إذا (p == عدم)
	    {
		إذا (*skipwhite(شروعات) != NUL)
		{
		    char_u *val = vim_strsave(شروعات);

		    generate_PUSHS(cctx, &val);
		    ++ڳڻپ;
		}
		اكسر;
	    }
	}
	generate_EXECCONCAT(cctx, ڳڻپ);
    }
    وإلا
	generate_EXEC_copy(cctx, ISN_EXEC, سٽ);

theend:
    إذا (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    أرجع nextcmd;
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
compile_script(char_u *سٽ, cctx_T *cctx)
{
    إذا (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	إذا ((isn = generate_instr(cctx, ISN_EXEC_ورهايو)) == عدم)
	    أرجع عدم;
	isn->isn_arg.string = vim_strsave(سٽ);
    }
    أرجع (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *اظهار = (char_u *)لڙي_ڳولا((حرف *)cmd, "\\=");

    إذا (اظهار != عدم)
    {
	صحيح delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), عدم, عدم, عدم);
	إذا (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*پڇاڙي;
	    صحيح		اظهار_res;
	    صحيح		trailing_error;
	    صحيح		instr_ڳڻپ;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    پڇاڙي = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_معلومات = عدم;
	    اظهار_res = compile_expr0(&cmd, cctx);
	    إذا (پڇاڙي[-1] == NUL)
		پڇاڙي[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    إذا (اظهار_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		إذا (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		أرجع عدم;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_ڳڻپ = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_معلومات;
	    instr[instr_ڳڻپ].isn_type = ISN_پڄاڻي;

	    cctx->ctx_instr = save_ga;
	    إذا ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == عدم)
	    {
		صحيح idx;

		لكل (idx = 0; idx < instr_ڳڻپ; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		أرجع عدم;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    إذا (*پڇاڙي == '&')
		++پڇاڙي;
	    طالما (ASCII_ISALPHA(*پڇاڙي) || *پڇاڙي == '#')
		++پڇاڙي;
	    أرجع پڇاڙي;
	}
    }

    أرجع compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *سٽ, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    إذا (lhs->lhs_name != عدم)
    {
	إذا (STRNCMP(arg, "END", 3) == 0)
	{
	    إذا (cctx->ctx_skip != SKIP_YES)
	    {
		إذا (lhs->lhs_append)
		{
		    // First load the current variable value.
		    إذا (compile_ڀريو_lhs_with_اشاريو(lhs, lhs->lhs_سڄو,
								 cctx) == FAIL)
			أرجع عدم;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		إذا (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		إذا (lhs->lhs_has_اشاريو)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    إذا (compile_سونپيو_unlet(lhs->lhs_سڄو, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			أرجع عدم;
		}
		وإلا إذا (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    أرجع عدم;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_سڄو);
	    }
	    أرجع arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	أرجع عدم;
    }

    إذا (arg[0] == '=' && arg[1] == '>')
    {
	صحيح	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	إذا (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	إذا (compile_سونپيو_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    أرجع عدم;
	إذا (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    أرجع عدم;
	إذا (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	وإلا
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    إذا (lhs->lhs_has_اشاريو)
	    {
		lhs->lhs_سڄو = vim_strnsave(arg, lhs->lhs_varlen_ڪل);
		إذا (lhs->lhs_سڄو == عدم)
		    أرجع عدم;
	    }
	}

	أرجع arg + lhs->lhs_varlen_ڪل;
    }

    // other redirects are handled like at script level
    أرجع compile_exec(سٽ, eap, cctx);
}

#إذا defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *سٽ, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    إذا (isn == عدم)
	أرجع عدم;
    isn->isn_arg.عدد = eap->cmdidx;

    p = eap->arg;
    إذا (compile_expr0(&p, cctx) == FAIL)
	أرجع عدم;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    إذا (isn == عدم)
	أرجع عدم;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    إذا (isn->isn_arg.cexpr.cexpr_ref == عدم)
	أرجع عدم;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(سٽ));

    أرجع p;
}
#جيڪڏهن_پورو

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, صحيح check_return_type, صحيح legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ڍير_type;

    إذا (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_شروعات(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	إذا (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_قدر_اندريون_function_without_return_type));
	    أرجع عدم;
	}
	إذا (legacy)
	{
	    صحيح save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    إذا (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		أرجع عدم;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (فراغ)skip_اظهار(&p, عدم);
	    cmdmod.cmod_flags = save_flags;
	}
	وإلا
	{
	    // compile return argument into instructions
	    إذا (compile_expr0(&p, cctx) == FAIL)
		أرجع عدم;
	}

	إذا (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ڍير_type = get_type_on_ڍير(cctx, 0);
	    إذا (check_type_is_قدر(ڍير_type) == FAIL)
		أرجع عدم;
	    إذا ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == عدم
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ڍير_type;
	    }
	    وإلا
	    {
		إذا (need_type(ڍير_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    أرجع عدم;
	    }
	}
    }
    وإلا
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	إذا (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_قدر));
	    أرجع عدم;
	}

	إذا (IS_CONSTRUCTOR_طريقو(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	وإلا
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    إذا (compile_ڳوليو_scope(عدم, عدم, عدم, عدم, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    إذا (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == عدم)
	أرجع عدم;

    // "return val | endif" is possible
    أرجع skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    صحيح
check_global_and_subst(char_u *cmd, char_u *arg)
{
    إذا (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != عدم)
    {
	semsg(_(e_separator_not_supported_str), arg);
	أرجع FAIL;
    }
    إذا (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	أرجع FAIL;
    }
    أرجع OK;
}


#جيڪڏهن_پورو  // defined(FEAT_EVAL)
