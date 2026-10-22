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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ساکن صحیح
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		idx = instr->ga_len;

    جب (idx > 0)
    {
	اگر (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_ڈیٹا)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    جاری;
	}
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (((isn_T *)instr->ga_ڈیٹا)[idx - 1].isn_type == ISN_PROF_آغاز)
	{
	    --idx;
	    جاری;
	}
#اگر_ختم
	اگر (((isn_T *)instr->ga_ڈیٹا)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    جاری;
	}
	توڑو;
    }
    واپس idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ساکن خالی
unwind_locals(cctx_T *cctx, صحیح new_چوٹی, صحیح keep)
{
    اگر (cctx->ctx_locals.ga_len > new_چوٹی)
	برائے (صحیح idx = new_چوٹی; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_ڈیٹا) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    اگر (!keep)
	cctx->ctx_locals.ga_len = new_چوٹی;
}

/*
 * Free all local variables.
 */
    خالی
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
	semsg(_(e_دلیل_required_for_str), "unlet");
	واپس FAIL;
    }

    اگر (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == تہی)
    {
	// "unlet s:var" is allowed in legacy script.
	اگر (*name == 's' && !اندر_vim9script())
	    واپس OK;
	semsg(_(e_cannot_unlet_str), name);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ساکن صحیح
compile_unlet(
    lval_T  *lvp,
    char_u  *name_انتہا,
    exarg_T *eap,
    صحیح	    deep غیر_مستعمل,
    خالی    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    صحیح		cc = *name_انتہا;
    صحیح		ret = OK;

    اگر (cctx->ctx_skip == SKIP_YES)
	واپس OK;

    *name_انتہا = NUL;
    اگر (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    ورنہ اگر (vim_strchr(p, '.') != تہی || vim_strchr(p, '[') != تہی)
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
	    اگر (!lhs.lhs_has_اشاریہ)
	    {
		semsg(_(e_cannot_unlet_imported_شے_str), p);
		ret = FAIL;
	    }
	    ورنہ
		ret = compile_تفویض_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    ورنہ اگر (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    ورنہ
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_انتہا = cc;
    واپس ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ساکن صحیح
compile_تالا_unlock(
    lval_T  *lvp,
    char_u  *name_انتہا,
    exarg_T *eap,
    صحیح	    deep,
    خالی    *coookie)
{
    cctx_T	*cctx = coookie;
    صحیح		cc = *name_انتہا;
    char_u	*p = lvp->ll_name;
    صحیح		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    حرف	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    صحیح		is_arg = FALSE;

#اگر_تعریف_شدہ LOG_LOCKVAR
    ch_log(تہی, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#اگر_ختم

    اگر (cctx->ctx_skip == SKIP_YES)
	واپس OK;

    اگر (*p == NUL)
    {
	semsg(_(e_دلیل_required_for_str), cmd);
	واپس FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    اگر (p[1] != ':')
    {
	char_u *انتہا = ڈھونڈو_name_انتہا(p, تہی, تہی, FNE_CHECK_آغاز);

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

	char_u	*name = تہی;
	صحیح	len = انتہا - p;

	اگر (lookup_local(p, len, تہی, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    اگر (*انتہا != '.' && *انتہا != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_تالا_unlock_local_variable));
		واپس FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#اگر_تعریف_شدہ LOG_LOCKVAR
	    ch_log(تہی, "LKVAR:    ... lookup_local: name %s", name);
#اگر_ختم
	}
	اگر (name == تہی)
	{
	    class_T *cl;
	    اگر (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		اگر (*انتہا != '.' && *انتہا != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (صحیح)cl->class_name.لمبائی;
#اگر_تعریف_شدہ LOG_LOCKVAR
		    ch_log(تہی, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#اگر_ختم
		}
	    }
	}
	اگر (name == تہی)
	{
	    // Can lockvar any function arg.
	    اگر (arg_exists(p, len, تہی, تہی, تہی, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#اگر_تعریف_شدہ LOG_LOCKVAR
		ch_log(تہی, "LKVAR:    ... arg_exists: name %s", name);
#اگر_ختم
	    }
	}
	اگر (name == تہی)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    اگر (script_var_exists(p, len, cctx, تہی) == OK
		&& (*انتہا == '.' || *انتہا == '['))
	    {
		name = p;
#اگر_تعریف_شدہ LOG_LOCKVAR
		ch_log(تہی, "LKVAR:    ... script_var_exists: name %s", name);
#اگر_ختم
	    }
	}
	اگر (name != تہی)
	{
#اگر_تعریف_شدہ LOG_LOCKVAR
	    ch_log(تہی, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#اگر_ختم
	    اگر (compile_بھرو(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_انتہا = NUL;
    size_t len = name_انتہا - p + 20;
    buf = alloc(len);
    اگر (buf == تہی)
	ret = FAIL;
    ورنہ
    {
	اگر (deep < 0)
	    vim_snprintf((حرف *)buf, len, "%s! %s", cmd, p);
	ورنہ
	    vim_snprintf((حرف *)buf, len, "%s %d %s", cmd, deep, p);
#اگر_تعریف_شدہ LOG_LOCKVAR
	ch_log(تہی, "LKVAR:    ... buf %s", buf);
#اگر_ختم
	اگر (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	ورنہ
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_انتہا = cc;
    }
    واپس ret;
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
	ورنہ اگر (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	ورنہ
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_تالا_unlock,
	    cctx);
    واپس eap->nextcmd == تہی ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ساکن صحیح
compile_jump_مقصد_انتہا(
	endlabel_T  **el,
	jumpwhen_T  when,
	صحیح	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    اگر (endlabel == تہی)
	واپس FAIL;
    endlabel->el_اگلا = *el;
    *el = endlabel;
    endlabel->el_انتہا_label = instr->ga_len;

    اگر (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    ورنہ
	generate_JUMP(cctx, when, 0);
    واپس OK;
}

    ساکن خالی
compile_fill_jump_مقصد_انتہا(endlabel_T **el, صحیح jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    جب (*el != تہی)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_ڈیٹا) + cur->el_انتہا_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_اگلا;
	vim_free(cur);
    }
}

    ساکن خالی
compile_free_jump_مقصد_انتہا(endlabel_T **el)
{
    جب (*el != تہی)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_اگلا;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ساکن scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    اگر (scope == تہی)
	واپس تہی;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_گنتی = cctx->ctx_locals.ga_len;
    اگر (scope->se_outer != تہی)
	scope->se_loop_گہرائی = scope->se_outer->se_loop_گہرائی;
    واپس scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    خالی
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    اگر (scope == تہی)
    {
	iemsg("calling drop_scope() without a scope");
	واپس;
    }
    cctx->ctx_scope = scope->se_outer;
    انتخاب (scope->se_type)
    {
	حالت IF_SCOPE:
	    compile_free_jump_مقصد_انتہا(&scope->se_u.se_if.is_انتہا_label); توڑو;
	حالت FOR_SCOPE:
	    compile_free_jump_مقصد_انتہا(&scope->se_u.se_for.fs_انتہا_label); توڑو;
	حالت WHILE_SCOPE:
	    compile_free_jump_مقصد_انتہا(&scope->se_u.se_while.ws_انتہا_label); توڑو;
	حالت TRY_SCOPE:
	    compile_free_jump_مقصد_انتہا(&scope->se_u.se_try.ts_انتہا_label); توڑو;
	حالت NO_SCOPE:
	حالت بلاک_SCOPE:
	    توڑو;
    }
    vim_free(scope);
}

    ساکن صحیح
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    اگر (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	واپس TRUE;
    }
    واپس FALSE;
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
    صحیح		instr_گنتی = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_میدان(ppconst);
    اگر (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	واپس تہی;
    }
    اگر (!سرے_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	واپس تہی;
    }
    اگر (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    ورنہ اگر (instr->ga_len == instr_گنتی && ppconst.pp_مستعمل == 1)
    {
	صحیح error = FALSE;
	صحیح v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	اگر (error)
	    واپس تہی;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    ورنہ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	    واپس تہی;
	اگر (bool_on_ڈھیر(cctx) == FAIL)
	    واپس تہی;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    اگر (scope == تہی)
	واپس تہی;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    اگر (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    ورنہ
	scope->se_u.se_if.is_if_label = -1;

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_انتہا);
	cctx->ctx_skip = SKIP_YES;
    }
#اگر_ختم

    واپس p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		instr_گنتی;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    اگر (scope == تہی || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	واپس تہی;
    }
    اگر (scope->se_u.se_if.is_نظر_آیا_else)
    {
	emsg(_(e_elseif_after_else));
	واپس تہی;
    }
    unwind_locals(cctx, scope->se_local_گنتی, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    اگر (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_نظر_آیا_skip_not = TRUE;
    }
    اگر (scope->se_u.se_if.is_نظر_آیا_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_اظہاریہ_cctx(&p, cctx);
	واپس p;
    }

    اگر (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_اظہاریہ_cctx(&p, cctx);
	واپس p;
    }

    اگر (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	صحیح	    moved_cmdmod = FALSE;
	صحیح	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	اگر (((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    اگر (GA_GROW_FAILS(instr, 1))
		واپس تہی;
	    ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len] =
				  ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	اگر (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len];
	    saved_debug = TRUE;
	}

	اگر (compile_jump_مقصد_انتہا(&scope->se_u.se_if.is_انتہا_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    واپس تہی;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	اگر (moved_cmdmod)
	    ++instr->ga_len;

	اگر (saved_debug)
	{
	    // move the debug instruction here
	    اگر (GA_GROW_FAILS(instr, 1))
		واپس تہی;
	    ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_میدان(ppconst);
    اگر (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_آغاز);
#اگر_ختم
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_گنتی = instr->ga_len;
    اگر (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	واپس تہی;
    }
    cctx->ctx_skip = save_skip;
    اگر (!سرے_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	واپس تہی;
    }
    اگر (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    ورنہ اگر (instr->ga_len == instr_گنتی && ppconst.pp_مستعمل == 1)
    {
	صحیح error = FALSE;
	صحیح v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	اگر (error)
	{
	    clear_ppconst(&ppconst);
	    واپس تہی;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    ورنہ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	    واپس تہی;
	اگر (bool_on_ڈھیر(cctx) == FAIL)
	    واپس تہی;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    واپس p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    اگر (scope == تہی || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	واپس تہی;
    }
    اگر (scope->se_u.se_if.is_نظر_آیا_else)
    {
	emsg(_(e_مضرب_else));
	واپس تہی;
    }
    unwind_locals(cctx, scope->se_local_گنتی, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_نظر_آیا_else = TRUE;

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (cctx->ctx_compile_type == CT_PROFILE)
    {
	اگر (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_آغاز)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	اگر (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_نظر_آیا_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_انتہا);
	    generate_instr(cctx, ISN_PROF_آغاز);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#اگر_ختم

    اگر (!scope->se_u.se_if.is_نظر_آیا_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	اگر (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    اگر (!cctx->ctx_had_return
		    && compile_jump_مقصد_انتہا(&scope->se_u.se_if.is_انتہا_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		واپس تہی;
	}

	اگر (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    اگر (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	اگر (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    واپس p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    اگر (misplaced_cmdmod(cctx))
	واپس تہی;

    اگر (scope == تہی || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	واپس تہی;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_گنتی, TRUE);
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    اگر (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_مقصد_انتہا(&ifscope->is_انتہا_label, instr->ga_len, cctx);

#اگر_تعریف_شدہ FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
					    && scope->se_skip_save != SKIP_YES)
    {
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_آغاز);
    }
#اگر_ختم
    cctx->ctx_skip = scope->se_skip_save;

    // If all the blocks end in :return and there is an :else then the
    // had_return flag is set.
    cctx->ctx_had_return = ifscope->is_had_return && ifscope->is_نظر_آیا_else;

    drop_scope(cctx);
    واپس arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ساکن خالی
compile_fill_loop_info(loop_info_T *loop_info, صحیح funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_گنتی = cctx->ctx_locals.ga_len;
    loop_info->li_احاطہ_گنتی = cctx->ctx_احاطہ_گنتی;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ساکن type_T *
compile_for_tuple_get_vartype(type_T *vartype, صحیح var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    اگر (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	واپس &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_دلائل[0]->tt_member;
    اگر (member_type->tt_type == VAR_ANY)
	واپس &t_any;

    اگر (!var_list)
	// for x in tuple<...list<xxx>>
	واپس member_type;

    اگر (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	واپس member_type->tt_member;
    ورنہ اگر (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	واپس member_type->tt_دلائل[0]->tt_member;

    واپس &t_any;
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
compile_for(char_u *arg_آغاز, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_انتہا;
    char_u	*name = تہی;
    char_u	*p;
    char_u	*wp;
    صحیح		var_گنتی = 0;
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
    type_T	*شے_type = &t_any;
    صحیح		idx;
    صحیح		پچھلا_lnum = cctx->ctx_پچھلا_lnum;

    p = skip_var_list(arg_آغاز, TRUE, &var_گنتی, &semicolon, FALSE);
    اگر (p == تہی)
	واپس تہی;
    اگر (var_گنتی == 0)
	var_گنتی = 1;
    ورنہ
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    اگر (may_get_اگلا_سطر_error(wp, &p, cctx) == FAIL)
	واپس تہی;
    اگر (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	اگر (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	ورنہ
	    emsg(_(e_missing_اندر_after_for));
	واپس تہی;
    }
    wp = p + 2;
    اگر (may_get_اگلا_سطر_error(wp, &p, cctx) == FAIL)
	واپس تہی;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    اگر (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	پچھلا_lnum = ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    اگر (scope == تہی)
	واپس تہی;
    اگر (scope->se_loop_گہرائی == MAX_LOOP_گہرائی)
    {
	emsg(_(e_loop_nesting_too_deep));
	واپس تہی;
    }
    ++scope->se_loop_گہرائی;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, تفویض_VAR, &t_نمبر);
    اگر (loop_lvar == تہی)
    {
	drop_scope(cctx);
	واپس تہی;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, تفویض_VAR, &t_نمبر);
    اگر (funcref_lvar == تہی)
    {
	drop_scope(cctx);
	واپس تہی;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    اگر (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	واپس تہی;
    }
    arg_انتہا = arg;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ڈھیر(cctx, 0);
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
	    واپس تہی;
	}

	اگر (vartype->tt_type == VAR_STRING)
	    شے_type = &t_string;
	ورنہ اگر (vartype->tt_type == VAR_BLOB)
	    شے_type = &t_نمبر;
	ورنہ اگر (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    اگر (!var_list)
		شے_type = vartype->tt_member;
	    ورنہ اگر (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		شے_type = vartype->tt_member->tt_member;
	}
	ورنہ اگر (vartype->tt_type == VAR_TUPLE)
	    شے_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_چوٹی_label = current_instr_idx(cctx);

	اگر (cctx->ctx_compile_type == CT_DEBUG)
	{
	    صحیح		save_پچھلا_lnum = cctx->ctx_پچھلا_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_پچھلا_lnum = پچھلا_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_نام_len;
	    cctx->ctx_پچھلا_lnum = save_پچھلا_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_آغاز;
	اگر (var_list)
	{
	    generate_UNPACK(cctx, var_گنتی, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ڈھیر.ga_len;

	    // add type of the items
	    برائے (idx = 0; idx < var_گنتی; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : شے_type;

		اگر (ڈالو_type_ڈھیر(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    واپس تہی;
		}
	    }
	}

	برائے (idx = 0; idx < var_گنتی; ++idx)
	{
	    تفویض_dest_T	dest = dest_local;
	    صحیح			opt_flags = 0;
	    صحیح			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    اگر (name == تہی)
		جاؤ failed;
	    اگر (*skipwhite(p) == ':')
	    {
		اگر (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    جاؤ failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		اگر (lhs_type == تہی || !valid_declaration_type(lhs_type))
		    جاؤ failed;
	    }

	    اگر (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		جاؤ failed;
	    اگر (dest != dest_local)
	    {
		اگر (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, تہی) == FAIL)
		    جاؤ failed;
	    }
	    ورنہ اگر (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		اگر (generate_instr_drop(cctx, ISN_DROP, 1) == تہی)
		    جاؤ failed;
	    }
	    ورنہ
	    {
		// Script var is not supported.
		اگر (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_اندر_for_loop));
		    جاؤ failed;
		}

		اگر (!valid_varname(arg, (صحیح)varlen, FALSE))
		    جاؤ failed;
		اگر (lookup_local(arg, varlen, تہی, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    جاؤ failed;
		}

		// Reserve a variable to store "var".
		اگر (var_list)
		{
		    where.wt_اشاریہ = idx + 1;
		    where.wt_قسم = WT_VARIABLE;
		}
		اگر (lhs_type == &t_any)
		    lhs_type = شے_type;
		ورنہ اگر (شے_type != &t_unknown
			&& need_type_where(شے_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    جاؤ failed;
		var_lvar = reserve_local(cctx, arg, varlen, تفویض_FINAL,
								     lhs_type);
		اگر (var_lvar == تہی)
		    // out of memory or used as an argument
		    جاؤ failed;

		اگر (semicolon && idx == var_گنتی - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, تہی);
	    }

	    اگر (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_گہرائی = scope->se_loop_گہرائی - 1;
    }

    واپس arg_انتہا;

failed:
    vim_free(name);
    drop_scope(cctx);
    واپس تہی;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ساکن صحیح
compile_loop_انتہا(loop_info_T *loop_info, cctx_T *cctx)
{
    اگر (cctx->ctx_locals.ga_len > loop_info->li_local_گنتی
	    && cctx->ctx_احاطہ_گنتی > loop_info->li_احاطہ_گنتی)
	واپس generate_ENDLOOP(cctx, loop_info);
    واپس OK;
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
	واپس تہی;

    اگر (scope == تہی || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	واپس تہی;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	اگر (compile_loop_انتہا(&forscope->fs_loop_info, cctx) == FAIL)
	    واپس تہی;

	unwind_locals(cctx, scope->se_local_گنتی, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_چوٹی_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_ڈیٹا) + forscope->fs_چوٹی_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_انتہا = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_مقصد_انتہا(&forscope->fs_انتہا_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	اگر (generate_instr_drop(cctx, ISN_DROP, 1) == تہی)
	    واپس تہی;
    }

    vim_free(scope);

    واپس arg;
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
    اگر (scope == تہی)
	واپس تہی;
    اگر (scope->se_loop_گہرائی == MAX_LOOP_گہرائی)
    {
	emsg(_(e_loop_nesting_too_deep));
	واپس تہی;
    }
    ++scope->se_loop_گہرائی;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_چوٹی_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, تفویض_VAR, &t_نمبر);
    اگر (funcref_lvar == تہی)
    {
	drop_scope(cctx);
	واپس تہی;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_گہرائی = scope->se_loop_گہرائی - 1;

    // compile "expr"
    اگر (compile_expr0(&p, cctx) == FAIL)
	واپس تہی;

    اگر (!سرے_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	واپس تہی;
    }

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (bool_on_ڈھیر(cctx) == FAIL)
	    واپس FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	اگر (compile_jump_مقصد_انتہا(&whilescope->ws_انتہا_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    واپس FAIL;
    }

    واپس p;
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
	واپس تہی;
    اگر (scope == تہی || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	واپس تہی;
    }
    cctx->ctx_scope = scope->se_outer;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	اگر (compile_loop_انتہا(&whilescope->ws_loop_info, cctx) == FAIL)
	    واپس تہی;

	unwind_locals(cctx, scope->se_local_گنتی, TRUE);

#اگر_تعریف_شدہ FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_انتہا(cctx, cctx->ctx_lnum);
#اگر_ختم

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_چوٹی_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_مقصد_انتہا(&scope->se_u.se_while.ws_انتہا_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    واپس arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    صحیح
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    صحیح		پچھلا_local_گنتی = 0;

    CLEAR_اشارہ_گر(lvi);
    برائے (;;)
    {
	loop_info_T	*loopinfo;
	صحیح		cur_local_آخری;
	صحیح		آغاز_local_گنتی;

	جب (scope != تہی && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	اگر (scope == تہی)
	    توڑو;

	اگر (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_آخری = loopinfo->li_local_گنتی - 1;
	}
	ورنہ
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_آخری = loopinfo->li_local_گنتی - 3;
	}

	آغاز_local_گنتی = loopinfo->li_local_گنتی;
	اگر (cctx->ctx_locals.ga_len > آغاز_local_گنتی)
	{
	    lvi->lvi_loop[loopinfo->li_گہرائی].var_idx =
						      (مختصر)آغاز_local_گنتی;
	    lvi->lvi_loop[loopinfo->li_گہرائی].var_گنتی =
			  (مختصر)(cctx->ctx_locals.ga_len - آغاز_local_گنتی
							   - پچھلا_local_گنتی);
	    اگر (lvi->lvi_گہرائی == 0)
		lvi->lvi_گہرائی = loopinfo->li_گہرائی + 1;
	}

	scope = scope->se_outer;
	پچھلا_local_گنتی = cctx->ctx_locals.ga_len - cur_local_آخری;
    }
    واپس lvi->lvi_گہرائی > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    خالی
get_loop_var_idx(cctx_T *cctx, صحیح idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_گہرائی = -1;
    lvar->lv_loop_idx = -1;
    اگر (get_loop_var_info(cctx, &lvi))
    {
	صحیح گہرائی;

	برائے (گہرائی = lvi.lvi_گہرائی - 1; گہرائی >= 0; --گہرائی)
	    اگر (idx >= lvi.lvi_loop[گہرائی].var_idx
		    && idx < lvi.lvi_loop[گہرائی].var_idx
					       + lvi.lvi_loop[گہرائی].var_گنتی)
	    {
		lvar->lv_loop_گہرائی = گہرائی;
		lvar->lv_loop_idx = lvi.lvi_loop[گہرائی].var_idx;
		واپس;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ساکن صحیح
compile_ڈھونڈو_scope(
	صحیح	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	صحیح	    *try_scopes,    // :try scopes encountered or NULL
	حرف	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    برائے (;;)
    {
	اگر (scope == تہی)
	{
	    اگر (error != تہی)
		emsg(_(error));
	    واپس FAIL;
	}
	اگر (scope->se_type == FOR_SCOPE)
	{
	    اگر (compile_loop_انتہا(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		واپس FAIL;
	    اگر (loop_label != تہی)
		*loop_label = scope->se_u.se_for.fs_چوٹی_label;
	    اگر (el != تہی)
		*el = &scope->se_u.se_for.fs_انتہا_label;
	    توڑو;
	}
	اگر (scope->se_type == WHILE_SCOPE)
	{
	    اگر (compile_loop_انتہا(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		واپس FAIL;
	    اگر (loop_label != تہی)
		*loop_label = scope->se_u.se_while.ws_چوٹی_label;
	    اگر (el != تہی)
		*el = &scope->se_u.se_while.ws_انتہا_label;
	    توڑو;
	}
	اگر (try_scopes != تہی && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    واپس OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    صحیح		try_scopes = 0;
    صحیح		loop_label;

    اگر (compile_ڈھونڈو_scope(&loop_label, تہی, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	واپس تہی;
    اگر (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    ورنہ
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    واپس arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    صحیح		try_scopes = 0;
    endlabel_T	**el;

    اگر (compile_ڈھونڈو_scope(تہی, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	واپس تہی;

    اگر (cctx->ctx_skip == SKIP_YES)
	واپس arg;

    اگر (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    اگر (compile_jump_مقصد_انتہا(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	واپس تہی;

    واپس arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_بلاک(char_u *arg, cctx_T *cctx)
{
    اگر (new_scope(cctx, بلاک_SCOPE) == تہی)
	واپس تہی;
    واپس skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    خالی
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_گنتی, TRUE);
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
	واپس تہی;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    اگر (try_scope == تہی)
	واپس تہی;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	اگر ((isn = generate_instr(cctx, ISN_TRY)) == تہی)
	    واپس تہی;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	اگر (isn->isn_arg.tryref.try_ref == تہی)
	    واپس تہی;
    }

    // scope for the try block itself
    scope = new_scope(cctx, بلاک_SCOPE);
    اگر (scope == تہی)
	واپس تہی;

    واپس arg;
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
	واپس تہی;

    // end block scope from :try or :catch
    اگر (scope != تہی && scope->se_type == بلاک_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == تہی || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	واپس تہی;
    }

    اگر (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_رمز_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	واپس تہی;
    }
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
#اگر_تعریف_شدہ FEAT_PROFILE
	// the profile-start should be after the jump
	اگر (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_آغاز)
	    --instr->ga_len;
#اگر_ختم
	// Jump from end of previous block to :finally or :endtry
	اگر (compile_jump_مقصد_انتہا(&scope->se_u.se_try.ts_انتہا_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    واپس تہی;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_try.ts_try_label;
	اگر (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	اگر (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_انتہا);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_آغاز);
	}
#اگر_ختم
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    اگر (سرے_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    ورنہ
    {
	char_u *انتہا;
	char_u *pat;
	char_u *tofree = تہی;
	صحیح	dropped = 0;
	صحیح	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	انتہا = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, تہی);
	اگر (*انتہا != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    واپس تہی;
	}
	اگر (tofree == تہی)
	    len = (صحیح)(انتہا - (p + 1));
	ورنہ
	    len = (صحیح)(انتہا - tofree);
	pat = vim_strnsave(tofree == تہی ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	اگر (pat == تہی)
	    واپس تہی;
	اگر (generate_PUSHS(cctx, &pat) == FAIL)
	    واپس تہی;

	اگر (generate_COMPARE(cctx, اظہاریہ_مطابقت, FALSE) == FAIL)
	    واپس تہی;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	اگر (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    واپس تہی;
    }

    اگر (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == تہی)
	واپس تہی;

    اگر (new_scope(cctx, بلاک_SCOPE) == تہی)
	واپس تہی;
    واپس p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    صحیح		this_instr;

    اگر (misplaced_cmdmod(cctx))
	واپس تہی;

    // end block scope from :try or :catch
    اگر (scope != تہی && scope->se_type == بلاک_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == تہی || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	واپس تہی;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    اگر (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_try.ts_try_label;
	اگر (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_مضرب_finally));
	    واپس تہی;
	}

	this_instr = instr->ga_len;
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_ڈیٹا)[this_instr - 1]
						   .isn_type == ISN_PROF_آغاز)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    اگر (this_instr > 0 && ((isn_T *)instr->ga_ڈیٹا)[this_instr - 1]
						     .isn_type == ISN_PROF_انتہا)
		--this_instr;
	}
#اگر_ختم

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_مقصد_انتہا(&scope->se_u.se_try.ts_انتہا_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	اگر (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	اگر (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	اگر (generate_instr(cctx, ISN_FINALLY) == تہی)
	    واپس تہی;
    }

    واپس arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    اگر (misplaced_cmdmod(cctx))
	واپس تہی;

    // end block scope from :catch or :finally
    اگر (scope != تہی && scope->se_type == بلاک_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    اگر (scope == تہی || scope->se_type != TRY_SCOPE)
    {
	اگر (scope == تہی)
	    emsg(_(e_endtry_without_try));
	ورنہ اگر (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	ورنہ اگر (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	ورنہ
	    emsg(_(e_missing_endif));
	واپس تہی;
    }

    try_isn = ((isn_T *)instr->ga_ڈیٹا) + scope->se_u.se_try.ts_try_label;
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    واپس تہی;
	}

#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_ڈیٹا)[instr->ga_len - 1]
						.isn_type == ISN_PROF_آغاز)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#اگر_ختم

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_مقصد_انتہا(&scope->se_u.se_try.ts_انتہا_label,
							  instr->ga_len, cctx);

	اگر (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_ڈیٹا)
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
	اگر (generate_instr(cctx, ISN_ENDTRY) == تہی)
	    واپس تہی;
#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_آغاز);
#اگر_ختم
    }
    واپس arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    اگر (compile_expr0(&p, cctx) == FAIL)
	واپس تہی;
    اگر (cctx->ctx_skip == SKIP_YES)
	واپس p;
    اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	واپس تہی;
    اگر (generate_instr_drop(cctx, ISN_THROW, 1) == تہی)
	واپس تہی;

    واپس p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    صحیح		name_only;
    طویل	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    اگر (compile_expr0(&p, cctx) == FAIL)
	واپس تہی;

    اگر (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_عبارت_without_effect_str), arg);
	واپس تہی;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    واپس skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    صحیح
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    اگر (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	اگر (lvar == تہی)
	    واپس 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    واپس dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_آغاز, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_آغاز;
    صحیح		argcount = 0;
    صحیح		defer_var_idx;
    type_T	*type = تہی;
    صحیح		func_idx = -1;

    اگر (*arg == '(')
    {
	// a lambda function
	اگر (compile_lambda(&arg, cctx, تہی) != OK)
	    واپس تہی;
	paren = vim_strchr(arg, '(');
	اگر (paren == تہی)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    واپس تہی;
	}
    }
    ورنہ
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	اگر (paren == تہی)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    واپس تہی;
	}
	*paren = NUL;
	func_idx = ڈھونڈو_اندرونی_func(arg);
	اگر (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)اندرونی_func_name(func_idx),
		    &t_func_any, FALSE);
	ورنہ اگر (compile_expr0(&arg, cctx) == FAIL)
	    واپس تہی;
	*paren = '(';
    }

    // check for function type
    اگر (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ڈھیر(cctx, 0);
	اگر (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    واپس تہی;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    اگر (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	واپس تہی;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر (func_idx >= 0)
	{
	    type2_T	*argtypes = تہی;
	    type2_T	shuffled_argtypes[MAX_FUNC_دلائل];

	    اگر (check_اندرونی_func_دلائل(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		واپس تہی;
	}
	ورنہ اگر (check_func_دلائل_from_type(cctx, type, argcount, TRUE,
		    arg_آغاز) == FAIL)
	    واپس تہی;

	defer_var_idx = get_defer_var_idx(cctx);
	اگر (defer_var_idx == 0)
	    واپس تہی;
	اگر (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    واپس تہی;
    }

    واپس skipwhite(arg);
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
compile_mult_اظہاریہ(
	char_u	*arg,
	صحیح	cmdidx,
	طویل	cmd_گنتی غیر_مستعمل,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*پچھلا = arg;
    char_u	*اظہاریہ_آغاز;
    صحیح		گنتی = 0;
    صحیح		آغاز_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    صحیح		r = OK;

    برائے (;;)
    {
	اگر (سرے_excmd2(پچھلا, p))
	    توڑو;
	اظہاریہ_آغاز = p;
	اگر (compile_expr0(&p, cctx) == FAIL)
	    واپس تہی;

	اگر (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ڈھیر(cctx, 0);
	    اگر (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_عبارت_does_not_نتیجہ_اندر_قدر_str), اظہاریہ_آغاز);
		واپس تہی;
	    }
	}

	++گنتی;
	پچھلا = p;
	p = skipwhite(p);
    }

    اگر (گنتی > 0)
    {
	طویل save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = آغاز_ctx_lnum;

	اگر (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, گنتی);
	ورنہ اگر (cmdidx == CMD_execute)
	    r = generate_MULT_اظہاریہ(cctx, ISN_EXECUTE, گنتی);
	ورنہ اگر (cmdidx == CMD_echomsg)
	    r = generate_MULT_اظہاریہ(cctx, ISN_ECHOMSG, گنتی);
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	ورنہ اگر (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, گنتی, cmd_گنتی);
#اگر_ختم
	ورنہ اگر (cmdidx == CMD_echoconsole)
	    r = generate_MULT_اظہاریہ(cctx, ISN_ECHOCONSOLE, گنتی);
	ورنہ
	    r = generate_MULT_اظہاریہ(cctx, ISN_ECHOERR, گنتی);

	cctx->ctx_lnum = save_lnum;
    }
    واپس r == OK ? p : تہی;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ساکن صحیح
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_انتہا = skip_range(eap->cmd, TRUE, تہی);
    char_u *p = skipdigits(eap->cmd);

    اگر (p == range_انتہا)
	واپس FAIL;
    واپس generate_RANGE(cctx, vim_strnsave(eap->cmd, range_انتہا - eap->cmd));
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
    حرف	*errormsg;
    صحیح		above = eap->forceit;

    eap->regname = *سطر;

    اگر (eap->regname == '=')
    {
	char_u *p = skipwhite(سطر + 1);

	اگر (compile_expr0(&p, cctx) == FAIL)
	    واپس تہی;
	سطر = p;
    }
    ورنہ اگر (eap->regname != NUL)
	++سطر;

    اگر (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    ورنہ
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	اگر (parse_cmd_پتہ(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    واپس تہی;
	اگر (eap->addr_گنتی == 0)
	    lnum = -1;
	ورنہ
	    lnum = eap->line2;
	اگر (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    واپس سطر;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *سطر_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*سطر = سطر_arg;
    char_u	*p;
    صحیح		has_اظہاریہ = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = تہی;
    char_u	*cmd_arg = تہی;

    اگر (cctx->ctx_skip == SKIP_YES)
	جاؤ theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    اگر (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_ڈیٹا) + instr->ga_len - 1;

	اگر (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    اگر (eap->cmdidx >= 0 && eap->cmdidx < CMD_سائز)
    {
	طویل	argt = eap->argt;
	صحیح	usefilter = FALSE;

	has_اظہاریہ = argt & (EX_XFILE | EX_EXPAND);

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
	    اگر (eap->nextcmd != تہی)
		nextcmd = eap->nextcmd;
	}
	ورنہ اگر (eap->cmdidx == CMD_wincmd)
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
	ورنہ اگر (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    صحیح	    flags = 0;  //غیر_مستعمل
	    صحیح	    آغاز_lnum = SOURCING_LNUM;

	    CLEAR_میدان(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_بلاک(&ea, سطر, &tofree, &flags);
	    اگر (tofree != تہی)
	    {
		سطر = p;
		SOURCING_LNUM = آغاز_lnum;
	    }
	}
    }

    اگر (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_اظہاریہ = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	اگر (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    اگر ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	صحیح delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, تہی, تہی, تہی);
	اگر (*p == delim)
	    cmd_arg = p + 1;
    }

    اگر (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    اگر (cmd_arg != تہی)
    {
	exarg_T nea;

	CLEAR_میدان(nea);
	nea.cmd = cmd_arg;
	p = ڈھونڈو_ex_command(&nea, تہی, lookup_scriptitem, تہی);
	اگر (nea.cmdidx < CMD_سائز)
	{
	    has_اظہاریہ = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    اگر (has_اظہاریہ)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    اگر (has_اظہاریہ && (p = (char_u *)لڑی_تلاش((حرف *)eap->arg, "`=")) != تہی)
    {
	صحیح	گنتی = 0;
	char_u	*آغاز = skipwhite(سطر);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	برائے (;;)
	{
	    اگر (p > آغاز)
	    {
		char_u *val = vim_strnsave(آغاز, p - آغاز);

		generate_PUSHS(cctx, &val);
		++گنتی;
	    }
	    p += 2;
	    اگر (compile_expr0(&p, cctx) == FAIL)
		واپس تہی;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++گنتی;
	    p = skipwhite(p);
	    اگر (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		واپس تہی;
	    }
	    آغاز = p + 1;

	    p = (char_u *)لڑی_تلاش((حرف *)آغاز, "`=");
	    اگر (p == تہی)
	    {
		اگر (*skipwhite(آغاز) != NUL)
		{
		    char_u *val = vim_strsave(آغاز);

		    generate_PUSHS(cctx, &val);
		    ++گنتی;
		}
		توڑو;
	    }
	}
	generate_EXECCONCAT(cctx, گنتی);
    }
    ورنہ
	generate_EXEC_copy(cctx, ISN_EXEC, سطر);

theend:
    اگر (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    واپس nextcmd;
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

	اگر ((isn = generate_instr(cctx, ISN_EXEC_بانٹو)) == تہی)
	    واپس تہی;
	isn->isn_arg.string = vim_strsave(سطر);
    }
    واپس (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *اظہاریہ = (char_u *)لڑی_تلاش((حرف *)cmd, "\\=");

    اگر (اظہاریہ != تہی)
    {
	صحیح delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), تہی, تہی, تہی);
	اگر (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*انتہا;
	    صحیح		اظہاریہ_res;
	    صحیح		trailing_error;
	    صحیح		instr_گنتی;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    انتہا = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_ڈیٹا = تہی;
	    اظہاریہ_res = compile_expr0(&cmd, cctx);
	    اگر (انتہا[-1] == NUL)
		انتہا[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    اگر (اظہاریہ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		اگر (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		واپس تہی;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_گنتی = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_ڈیٹا;
	    instr[instr_گنتی].isn_type = ISN_اختتام;

	    cctx->ctx_instr = save_ga;
	    اگر ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == تہی)
	    {
		صحیح idx;

		برائے (idx = 0; idx < instr_گنتی; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		واپس تہی;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    اگر (*انتہا == '&')
		++انتہا;
	    جب (ASCII_ISALPHA(*انتہا) || *انتہا == '#')
		++انتہا;
	    واپس انتہا;
	}
    }

    واپس compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *سطر, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    اگر (lhs->lhs_name != تہی)
    {
	اگر (STRNCMP(arg, "END", 3) == 0)
	{
	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		اگر (lhs->lhs_append)
		{
		    // First load the current variable value.
		    اگر (compile_بھرو_lhs_with_اشاریہ(lhs, lhs->lhs_پورا,
								 cctx) == FAIL)
			واپس تہی;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		اگر (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		اگر (lhs->lhs_has_اشاریہ)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    اگر (compile_تفویض_unlet(lhs->lhs_پورا, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			واپس تہی;
		}
		ورنہ اگر (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    واپس تہی;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_پورا);
	    }
	    واپس arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	واپس تہی;
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

	اگر (compile_تفویض_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    واپس تہی;
	اگر (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    واپس تہی;
	اگر (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	ورنہ
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    اگر (lhs->lhs_has_اشاریہ)
	    {
		lhs->lhs_پورا = vim_strnsave(arg, lhs->lhs_varlen_کل);
		اگر (lhs->lhs_پورا == تہی)
		    واپس تہی;
	    }
	}

	واپس arg + lhs->lhs_varlen_کل;
    }

    // other redirects are handled like at script level
    واپس compile_exec(سطر, eap, cctx);
}

#اگر defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *سطر, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    اگر (isn == تہی)
	واپس تہی;
    isn->isn_arg.نمبر = eap->cmdidx;

    p = eap->arg;
    اگر (compile_expr0(&p, cctx) == FAIL)
	واپس تہی;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    اگر (isn == تہی)
	واپس تہی;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    اگر (isn->isn_arg.cexpr.cexpr_ref == تہی)
	واپس تہی;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(سطر));

    واپس p;
}
#اگر_ختم

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, صحیح check_return_type, صحیح legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ڈھیر_type;

    اگر (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_آغاز(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	اگر (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_قدر_اندر_function_without_return_type));
	    واپس تہی;
	}
	اگر (legacy)
	{
	    صحیح save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    اگر (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		واپس تہی;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (خالی)skip_اظہاریہ(&p, تہی);
	    cmdmod.cmod_flags = save_flags;
	}
	ورنہ
	{
	    // compile return argument into instructions
	    اگر (compile_expr0(&p, cctx) == FAIL)
		واپس تہی;
	}

	اگر (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ڈھیر_type = get_type_on_ڈھیر(cctx, 0);
	    اگر (check_type_is_قدر(ڈھیر_type) == FAIL)
		واپس تہی;
	    اگر ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == تہی
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ڈھیر_type;
	    }
	    ورنہ
	    {
		اگر (need_type(ڈھیر_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    واپس تہی;
	    }
	}
    }
    ورنہ
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	اگر (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_قدر));
	    واپس تہی;
	}

	اگر (IS_CONSTRUCTOR_طریقہ(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	ورنہ
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    اگر (compile_ڈھونڈو_scope(تہی, تہی, تہی, تہی, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    اگر (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == تہی)
	واپس تہی;

    // "return val | endif" is possible
    واپس skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    صحیح
check_global_and_subst(char_u *cmd, char_u *arg)
{
    اگر (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != تہی)
    {
	semsg(_(e_separator_not_supported_str), arg);
	واپس FAIL;
    }
    اگر (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	واپس FAIL;
    }
    واپس OK;
}


#اگر_ختم  // defined(FEAT_EVAL)
