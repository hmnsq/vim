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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    స్థిర పూర్ణాంకం
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    పూర్ణాంకం		idx = instr->ga_len;

    అయితే (idx > 0)
    {
	ఒకవేళ (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_దత్తాంశం)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    కొనసాగు;
	}
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (((isn_T *)instr->ga_దత్తాంశం)[idx - 1].isn_type == ISN_PROF_ప్రారంభం)
	{
	    --idx;
	    కొనసాగు;
	}
#అయితే_ముగింపు
	ఒకవేళ (((isn_T *)instr->ga_దత్తాంశం)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    కొనసాగు;
	}
	ఆపు;
    }
    తిరిగి idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    స్థిర శూన్య
unwind_locals(cctx_T *cctx, పూర్ణాంకం new_శిఖరం, పూర్ణాంకం keep)
{
    ఒకవేళ (cctx->ctx_locals.ga_len > new_శిఖరం)
	కొరకు (పూర్ణాంకం idx = new_శిఖరం; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_దత్తాంశం) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    ఒకవేళ (!keep)
	cctx->ctx_locals.ga_len = new_శిఖరం;
}

/*
 * Free all local variables.
 */
    శూన్య
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    పూర్ణాంకం
check_vim9_unlet(char_u *name)
{
    ఒకవేళ (*name == NUL)
    {
	semsg(_(e_వాదం_required_for_str), "unlet");
	తిరిగి FAIL;
    }

    ఒకవేళ (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ఖాళీ)
    {
	// "unlet s:var" is allowed in legacy script.
	ఒకవేళ (*name == 's' && !లోపల_vim9script())
	    తిరిగి OK;
	semsg(_(e_cannot_unlet_str), name);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    స్థిర పూర్ణాంకం
compile_unlet(
    lval_T  *lvp,
    char_u  *name_చివర,
    exarg_T *eap,
    పూర్ణాంకం	    deep వాడని,
    శూన్య    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    పూర్ణాంకం		cc = *name_చివర;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి OK;

    *name_చివర = NUL;
    ఒకవేళ (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    లేకపోతే ఒకవేళ (vim_strchr(p, '.') != ఖాళీ || vim_strchr(p, '[') != ఖాళీ)
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
	ఒకవేళ (ret == OK)
	{
	    ఒకవేళ (!lhs.lhs_has_సూచీ)
	    {
		semsg(_(e_cannot_unlet_imported_అంశం_str), p);
		ret = FAIL;
	    }
	    లేకపోతే
		ret = compile_కేటాయించు_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    లేకపోతే ఒకవేళ (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    లేకపోతే
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_చివర = cc;
    తిరిగి ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    స్థిర పూర్ణాంకం
compile_తాళం_unlock(
    lval_T  *lvp,
    char_u  *name_చివర,
    exarg_T *eap,
    పూర్ణాంకం	    deep,
    శూన్య    *coookie)
{
    cctx_T	*cctx = coookie;
    పూర్ణాంకం		cc = *name_చివర;
    char_u	*p = lvp->ll_name;
    పూర్ణాంకం		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    అక్షరం	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    పూర్ణాంకం		is_arg = FALSE;

#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#అయితే_ముగింపు

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి OK;

    ఒకవేళ (*p == NUL)
    {
	semsg(_(e_వాదం_required_for_str), cmd);
	తిరిగి FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    ఒకవేళ (p[1] != ':')
    {
	char_u *చివర = కనుగొను_name_చివర(p, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);

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

	char_u	*name = ఖాళీ;
	పూర్ణాంకం	len = చివర - p;

	ఒకవేళ (lookup_local(p, len, ఖాళీ, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    ఒకవేళ (*చివర != '.' && *చివర != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_తాళం_unlock_local_variable));
		తిరిగి FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#నిర్వచితమైతే LOG_LOCKVAR
	    ch_log(ఖాళీ, "LKVAR:    ... lookup_local: name %s", name);
#అయితే_ముగింపు
	}
	ఒకవేళ (name == ఖాళీ)
	{
	    class_T *cl;
	    ఒకవేళ (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		ఒకవేళ (*చివర != '.' && *చివర != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (పూర్ణాంకం)cl->class_name.నిడివి;
#నిర్వచితమైతే LOG_LOCKVAR
		    ch_log(ఖాళీ, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#అయితే_ముగింపు
		}
	    }
	}
	ఒకవేళ (name == ఖాళీ)
	{
	    // Can lockvar any function arg.
	    ఒకవేళ (arg_exists(p, len, ఖాళీ, ఖాళీ, ఖాళీ, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR:    ... arg_exists: name %s", name);
#అయితే_ముగింపు
	    }
	}
	ఒకవేళ (name == ఖాళీ)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    ఒకవేళ (script_var_exists(p, len, cctx, ఖాళీ) == OK
		&& (*చివర == '.' || *చివర == '['))
	    {
		name = p;
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR:    ... script_var_exists: name %s", name);
#అయితే_ముగింపు
	    }
	}
	ఒకవేళ (name != ఖాళీ)
	{
#నిర్వచితమైతే LOG_LOCKVAR
	    ch_log(ఖాళీ, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#అయితే_ముగింపు
	    ఒకవేళ (compile_ఎక్కించు(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_చివర = NUL;
    size_t len = name_చివర - p + 20;
    buf = alloc(len);
    ఒకవేళ (buf == ఖాళీ)
	ret = FAIL;
    లేకపోతే
    {
	ఒకవేళ (deep < 0)
	    vim_snprintf((అక్షరం *)buf, len, "%s! %s", cmd, p);
	లేకపోతే
	    vim_snprintf((అక్షరం *)buf, len, "%s %d %s", cmd, deep, p);
#నిర్వచితమైతే LOG_LOCKVAR
	ch_log(ఖాళీ, "LKVAR:    ... buf %s", buf);
#అయితే_ముగింపు
	ఒకవేళ (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	లేకపోతే
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_చివర = cc;
    }
    తిరిగి ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    పూర్ణాంకం	    deep = 0;
    char_u  *p = arg;

    ఒకవేళ (eap->cmdidx != CMD_unlet)
    {
	ఒకవేళ (eap->forceit)
	    deep = -1;
	లేకపోతే ఒకవేళ (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	లేకపోతే
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_తాళం_unlock,
	    cctx);
    తిరిగి eap->nextcmd == ఖాళీ ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    స్థిర పూర్ణాంకం
compile_jump_గమ్యం_చివర(
	endlabel_T  **el,
	jumpwhen_T  when,
	పూర్ణాంకం	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    ఒకవేళ (endlabel == ఖాళీ)
	తిరిగి FAIL;
    endlabel->el_తదుపరి = *el;
    *el = endlabel;
    endlabel->el_చివర_label = instr->ga_len;

    ఒకవేళ (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    లేకపోతే
	generate_JUMP(cctx, when, 0);
    తిరిగి OK;
}

    స్థిర శూన్య
compile_fill_jump_గమ్యం_చివర(endlabel_T **el, పూర్ణాంకం jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    అయితే (*el != ఖాళీ)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_దత్తాంశం) + cur->el_చివర_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_తదుపరి;
	vim_free(cur);
    }
}

    స్థిర శూన్య
compile_free_jump_గమ్యం_చివర(endlabel_T **el)
{
    అయితే (*el != ఖాళీ)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_తదుపరి;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    స్థిర scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    ఒకవేళ (scope == ఖాళీ)
	తిరిగి ఖాళీ;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_లెక్క = cctx->ctx_locals.ga_len;
    ఒకవేళ (scope->se_outer != ఖాళీ)
	scope->se_loop_లోతు = scope->se_outer->se_loop_లోతు;
    తిరిగి scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    శూన్య
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    ఒకవేళ (scope == ఖాళీ)
    {
	iemsg("calling drop_scope() without a scope");
	తిరిగి;
    }
    cctx->ctx_scope = scope->se_outer;
    ఎంపిక (scope->se_type)
    {
	స్థితి IF_SCOPE:
	    compile_free_jump_గమ్యం_చివర(&scope->se_u.se_if.is_చివర_label); ఆపు;
	స్థితి FOR_SCOPE:
	    compile_free_jump_గమ్యం_చివర(&scope->se_u.se_for.fs_చివర_label); ఆపు;
	స్థితి WHILE_SCOPE:
	    compile_free_jump_గమ్యం_చివర(&scope->se_u.se_while.ws_చివర_label); ఆపు;
	స్థితి TRY_SCOPE:
	    compile_free_jump_గమ్యం_చివర(&scope->se_u.se_try.ts_చివర_label); ఆపు;
	స్థితి NO_SCOPE:
	స్థితి ఖండం_SCOPE:
	    ఆపు;
    }
    vim_free(scope);
}

    స్థిర పూర్ణాంకం
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ఒకవేళ (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
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
    పూర్ణాంకం		instr_లెక్క = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_క్షేత్రం(ppconst);
    ఒకవేళ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (!చివర్లు_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    లేకపోతే ఒకవేళ (instr->ga_len == instr_లెక్క && ppconst.pp_వాడిన == 1)
    {
	పూర్ణాంకం error = FALSE;
	పూర్ణాంకం v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	ఒకవేళ (error)
	    తిరిగి ఖాళీ;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    లేకపోతే
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ఒకవేళ (generate_ppconst(cctx, &ppconst) == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (bool_on_దొంతర(cctx) == FAIL)
	    తిరిగి ఖాళీ;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    ఒకవేళ (scope == ఖాళీ)
	తిరిగి ఖాళీ;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    ఒకవేళ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    లేకపోతే
	scope->se_u.se_if.is_if_label = -1;

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_చివర);
	cctx->ctx_skip = SKIP_YES;
    }
#అయితే_ముగింపు

    తిరిగి p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    పూర్ణాంకం		instr_లెక్క;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    ఒకవేళ (scope == ఖాళీ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (scope->se_u.se_if.is_చూసిన_else)
    {
	emsg(_(e_elseif_after_else));
	తిరిగి ఖాళీ;
    }
    unwind_locals(cctx, scope->se_local_లెక్క, TRUE);
    ఒకవేళ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    ఒకవేళ (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_చూసిన_skip_not = TRUE;
    }
    ఒకవేళ (scope->se_u.se_if.is_చూసిన_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_సమాసం_cctx(&p, cctx);
	తిరిగి p;
    }

    ఒకవేళ (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_సమాసం_cctx(&p, cctx);
	తిరిగి p;
    }

    ఒకవేళ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	పూర్ణాంకం	    moved_cmdmod = FALSE;
	పూర్ణాంకం	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	ఒకవేళ (((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    ఒకవేళ (GA_GROW_FAILS(instr, 1))
		తిరిగి ఖాళీ;
	    ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len] =
				  ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len];
	    saved_debug = TRUE;
	}

	ఒకవేళ (compile_jump_గమ్యం_చివర(&scope->se_u.se_if.is_చివర_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    తిరిగి ఖాళీ;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	ఒకవేళ (moved_cmdmod)
	    ++instr->ga_len;

	ఒకవేళ (saved_debug)
	{
	    // move the debug instruction here
	    ఒకవేళ (GA_GROW_FAILS(instr, 1))
		తిరిగి ఖాళీ;
	    ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_క్షేత్రం(ppconst);
    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_ప్రారంభం);
#అయితే_ముగింపు
	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_లెక్క = instr->ga_len;
    ఒకవేళ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	తిరిగి ఖాళీ;
    }
    cctx->ctx_skip = save_skip;
    ఒకవేళ (!చివర్లు_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    లేకపోతే ఒకవేళ (instr->ga_len == instr_లెక్క && ppconst.pp_వాడిన == 1)
    {
	పూర్ణాంకం error = FALSE;
	పూర్ణాంకం v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	ఒకవేళ (error)
	{
	    clear_ppconst(&ppconst);
	    తిరిగి ఖాళీ;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    లేకపోతే
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ఒకవేళ (generate_ppconst(cctx, &ppconst) == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (bool_on_దొంతర(cctx) == FAIL)
	    తిరిగి ఖాళీ;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    తిరిగి p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    ఒకవేళ (scope == ఖాళీ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (scope->se_u.se_if.is_చూసిన_else)
    {
	emsg(_(e_గుణిజం_else));
	తిరిగి ఖాళీ;
    }
    unwind_locals(cctx, scope->se_local_లెక్క, TRUE);
    ఒకవేళ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_చూసిన_else = TRUE;

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
    {
	ఒకవేళ (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_ప్రారంభం)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	ఒకవేళ (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_చూసిన_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_చివర);
	    generate_instr(cctx, ISN_PROF_ప్రారంభం);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (!scope->se_u.se_if.is_చూసిన_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	ఒకవేళ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ఒకవేళ (!cctx->ctx_had_return
		    && compile_jump_గమ్యం_చివర(&scope->se_u.se_if.is_చివర_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		తిరిగి ఖాళీ;
	}

	ఒకవేళ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ఒకవేళ (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	ఒకవేళ (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    తిరిగి p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    ఒకవేళ (scope == ఖాళీ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	తిరిగి ఖాళీ;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_లెక్క, TRUE);
    ఒకవేళ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    ఒకవేళ (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_గమ్యం_చివర(&ifscope->is_చివర_label, instr->ga_len, cctx);

#నిర్వచితమైతే FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
					    && scope->se_skip_save != SKIP_YES)
    {
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_ప్రారంభం);
    }
#అయితే_ముగింపు
    cctx->ctx_skip = scope->se_skip_save;

    // If all the blocks end in :return and there is an :else then the
    // had_return flag is set.
    cctx->ctx_had_return = ifscope->is_had_return && ifscope->is_చూసిన_else;

    drop_scope(cctx);
    తిరిగి arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    స్థిర శూన్య
compile_fill_loop_info(loop_info_T *loop_info, పూర్ణాంకం funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_లెక్క = cctx->ctx_locals.ga_len;
    loop_info->li_సంవృతం_లెక్క = cctx->ctx_సంవృతం_లెక్క;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    స్థిర type_T *
compile_for_tuple_get_vartype(type_T *vartype, పూర్ణాంకం var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    ఒకవేళ (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	తిరిగి &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_వాదనలు[0]->tt_member;
    ఒకవేళ (member_type->tt_type == VAR_ANY)
	తిరిగి &t_any;

    ఒకవేళ (!var_list)
	// for x in tuple<...list<xxx>>
	తిరిగి member_type;

    ఒకవేళ (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	తిరిగి member_type->tt_member;
    లేకపోతే ఒకవేళ (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	తిరిగి member_type->tt_వాదనలు[0]->tt_member;

    తిరిగి &t_any;
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
compile_for(char_u *arg_ప్రారంభం, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_చివర;
    char_u	*name = ఖాళీ;
    char_u	*p;
    char_u	*wp;
    పూర్ణాంకం		var_లెక్క = 0;
    పూర్ణాంకం		var_list = FALSE;
    పూర్ణాంకం		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    పూర్ణాంకం		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    పూర్ణాంకం		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*అంశం_type = &t_any;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_ప్రారంభం, TRUE, &var_లెక్క, &semicolon, FALSE);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (var_లెక్క == 0)
	var_లెక్క = 1;
    లేకపోతే
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(wp, &p, cctx) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	ఒకవేళ (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	లేకపోతే
	    emsg(_(e_missing_లోపల_after_for));
	తిరిగి ఖాళీ;
    }
    wp = p + 2;
    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(wp, &p, cctx) == FAIL)
	తిరిగి ఖాళీ;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    ఒకవేళ (scope == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (scope->se_loop_లోతు == MAX_LOOP_లోతు)
    {
	emsg(_(e_loop_nesting_too_deep));
	తిరిగి ఖాళీ;
    }
    ++scope->se_loop_లోతు;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, కేటాయించు_VAR, &t_సంఖ్య);
    ఒకవేళ (loop_lvar == ఖాళీ)
    {
	drop_scope(cctx);
	తిరిగి ఖాళీ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, కేటాయించు_VAR, &t_సంఖ్య);
    ఒకవేళ (funcref_lvar == ఖాళీ)
    {
	drop_scope(cctx);
	తిరిగి ఖాళీ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    ఒకవేళ (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	తిరిగి ఖాళీ;
    }
    arg_చివర = arg;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_దొంతర(cctx, 0);
	ఒకవేళ (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (vartype->tt_type == VAR_STRING)
	    అంశం_type = &t_string;
	లేకపోతే ఒకవేళ (vartype->tt_type == VAR_BLOB)
	    అంశం_type = &t_సంఖ్య;
	లేకపోతే ఒకవేళ (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    ఒకవేళ (!var_list)
		అంశం_type = vartype->tt_member;
	    లేకపోతే ఒకవేళ (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		అంశం_type = vartype->tt_member->tt_member;
	}
	లేకపోతే ఒకవేళ (vartype->tt_type == VAR_TUPLE)
	    అంశం_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_శిఖరం_label = current_instr_idx(cctx);

	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    పూర్ణాంకం		save_prev_lnum = cctx->ctx_prev_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_prev_lnum = prev_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_పేర్లు_len;
	    cctx->ctx_prev_lnum = save_prev_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_ప్రారంభం;
	ఒకవేళ (var_list)
	{
	    generate_UNPACK(cctx, var_లెక్క, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_దొంతర.ga_len;

	    // add type of the items
	    కొరకు (idx = 0; idx < var_లెక్క; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : అంశం_type;

		ఒకవేళ (నెట్టు_type_దొంతర(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    తిరిగి ఖాళీ;
		}
	    }
	}

	కొరకు (idx = 0; idx < var_లెక్క; ++idx)
	{
	    కేటాయించు_dest_T	dest = dest_local;
	    పూర్ణాంకం			opt_flags = 0;
	    పూర్ణాంకం			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    ఒకవేళ (name == ఖాళీ)
		వెళ్ళు failed;
	    ఒకవేళ (*skipwhite(p) == ':')
	    {
		ఒకవేళ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    వెళ్ళు failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		ఒకవేళ (lhs_type == ఖాళీ || !valid_declaration_type(lhs_type))
		    వెళ్ళు failed;
	    }

	    ఒకవేళ (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		వెళ్ళు failed;
	    ఒకవేళ (dest != dest_local)
	    {
		ఒకవేళ (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ఖాళీ) == FAIL)
		    వెళ్ళు failed;
	    }
	    లేకపోతే ఒకవేళ (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		ఒకవేళ (generate_instr_drop(cctx, ISN_DROP, 1) == ఖాళీ)
		    వెళ్ళు failed;
	    }
	    లేకపోతే
	    {
		// Script var is not supported.
		ఒకవేళ (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_లోపల_for_loop));
		    వెళ్ళు failed;
		}

		ఒకవేళ (!valid_varname(arg, (పూర్ణాంకం)varlen, FALSE))
		    వెళ్ళు failed;
		ఒకవేళ (lookup_local(arg, varlen, ఖాళీ, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    వెళ్ళు failed;
		}

		// Reserve a variable to store "var".
		ఒకవేళ (var_list)
		{
		    where.wt_సూచీ = idx + 1;
		    where.wt_రకం = WT_VARIABLE;
		}
		ఒకవేళ (lhs_type == &t_any)
		    lhs_type = అంశం_type;
		లేకపోతే ఒకవేళ (అంశం_type != &t_unknown
			&& need_type_where(అంశం_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    వెళ్ళు failed;
		var_lvar = reserve_local(cctx, arg, varlen, కేటాయించు_FINAL,
								     lhs_type);
		ఒకవేళ (var_lvar == ఖాళీ)
		    // out of memory or used as an argument
		    వెళ్ళు failed;

		ఒకవేళ (semicolon && idx == var_లెక్క - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ఖాళీ);
	    }

	    ఒకవేళ (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_లోతు = scope->se_loop_లోతు - 1;
    }

    తిరిగి arg_చివర;

failed:
    vim_free(name);
    drop_scope(cctx);
    తిరిగి ఖాళీ;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    స్థిర పూర్ణాంకం
compile_loop_చివర(loop_info_T *loop_info, cctx_T *cctx)
{
    ఒకవేళ (cctx->ctx_locals.ga_len > loop_info->li_local_లెక్క
	    && cctx->ctx_సంవృతం_లెక్క > loop_info->li_సంవృతం_లెక్క)
	తిరిగి generate_ENDLOOP(cctx, loop_info);
    తిరిగి OK;
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

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    ఒకవేళ (scope == ఖాళీ || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	తిరిగి ఖాళీ;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ఒకవేళ (compile_loop_చివర(&forscope->fs_loop_info, cctx) == FAIL)
	    తిరిగి ఖాళీ;

	unwind_locals(cctx, scope->se_local_లెక్క, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_శిఖరం_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_దత్తాంశం) + forscope->fs_శిఖరం_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_చివర = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_గమ్యం_చివర(&forscope->fs_చివర_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	ఒకవేళ (generate_instr_drop(cctx, ISN_DROP, 1) == ఖాళీ)
	    తిరిగి ఖాళీ;
    }

    vim_free(scope);

    తిరిగి arg;
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
    పూర్ణాంకం		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    ఒకవేళ (scope == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (scope->se_loop_లోతు == MAX_LOOP_లోతు)
    {
	emsg(_(e_loop_nesting_too_deep));
	తిరిగి ఖాళీ;
    }
    ++scope->se_loop_లోతు;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_శిఖరం_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, కేటాయించు_VAR, &t_సంఖ్య);
    ఒకవేళ (funcref_lvar == ఖాళీ)
    {
	drop_scope(cctx);
	తిరిగి ఖాళీ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_లోతు = scope->se_loop_లోతు - 1;

    // compile "expr"
    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (!చివర్లు_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ (bool_on_దొంతర(cctx) == FAIL)
	    తిరిగి FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	ఒకవేళ (compile_jump_గమ్యం_చివర(&whilescope->ws_చివర_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;
    ఒకవేళ (scope == ఖాళీ || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	తిరిగి ఖాళీ;
    }
    cctx->ctx_scope = scope->se_outer;
    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ఒకవేళ (compile_loop_చివర(&whilescope->ws_loop_info, cctx) == FAIL)
	    తిరిగి ఖాళీ;

	unwind_locals(cctx, scope->se_local_లెక్క, TRUE);

#నిర్వచితమైతే FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_చివర(cctx, cctx->ctx_lnum);
#అయితే_ముగింపు

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_శిఖరం_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_గమ్యం_చివర(&scope->se_u.se_while.ws_చివర_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    తిరిగి arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    పూర్ణాంకం
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    పూర్ణాంకం		prev_local_లెక్క = 0;

    CLEAR_సూచిక(lvi);
    కొరకు (;;)
    {
	loop_info_T	*loopinfo;
	పూర్ణాంకం		cur_local_చివరి;
	పూర్ణాంకం		ప్రారంభం_local_లెక్క;

	అయితే (scope != ఖాళీ && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	ఒకవేళ (scope == ఖాళీ)
	    ఆపు;

	ఒకవేళ (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_చివరి = loopinfo->li_local_లెక్క - 1;
	}
	లేకపోతే
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_చివరి = loopinfo->li_local_లెక్క - 3;
	}

	ప్రారంభం_local_లెక్క = loopinfo->li_local_లెక్క;
	ఒకవేళ (cctx->ctx_locals.ga_len > ప్రారంభం_local_లెక్క)
	{
	    lvi->lvi_loop[loopinfo->li_లోతు].var_idx =
						      (పొట్టి)ప్రారంభం_local_లెక్క;
	    lvi->lvi_loop[loopinfo->li_లోతు].var_లెక్క =
			  (పొట్టి)(cctx->ctx_locals.ga_len - ప్రారంభం_local_లెక్క
							   - prev_local_లెక్క);
	    ఒకవేళ (lvi->lvi_లోతు == 0)
		lvi->lvi_లోతు = loopinfo->li_లోతు + 1;
	}

	scope = scope->se_outer;
	prev_local_లెక్క = cctx->ctx_locals.ga_len - cur_local_చివరి;
    }
    తిరిగి lvi->lvi_లోతు > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    శూన్య
get_loop_var_idx(cctx_T *cctx, పూర్ణాంకం idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_లోతు = -1;
    lvar->lv_loop_idx = -1;
    ఒకవేళ (get_loop_var_info(cctx, &lvi))
    {
	పూర్ణాంకం లోతు;

	కొరకు (లోతు = lvi.lvi_లోతు - 1; లోతు >= 0; --లోతు)
	    ఒకవేళ (idx >= lvi.lvi_loop[లోతు].var_idx
		    && idx < lvi.lvi_loop[లోతు].var_idx
					       + lvi.lvi_loop[లోతు].var_లెక్క)
	    {
		lvar->lv_loop_లోతు = లోతు;
		lvar->lv_loop_idx = lvi.lvi_loop[లోతు].var_idx;
		తిరిగి;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    స్థిర పూర్ణాంకం
compile_కనుగొను_scope(
	పూర్ణాంకం	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	పూర్ణాంకం	    *try_scopes,    // :try scopes encountered or NULL
	అక్షరం	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    కొరకు (;;)
    {
	ఒకవేళ (scope == ఖాళీ)
	{
	    ఒకవేళ (error != ఖాళీ)
		emsg(_(error));
	    తిరిగి FAIL;
	}
	ఒకవేళ (scope->se_type == FOR_SCOPE)
	{
	    ఒకవేళ (compile_loop_చివర(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (loop_label != ఖాళీ)
		*loop_label = scope->se_u.se_for.fs_శిఖరం_label;
	    ఒకవేళ (el != ఖాళీ)
		*el = &scope->se_u.se_for.fs_చివర_label;
	    ఆపు;
	}
	ఒకవేళ (scope->se_type == WHILE_SCOPE)
	{
	    ఒకవేళ (compile_loop_చివర(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (loop_label != ఖాళీ)
		*loop_label = scope->se_u.se_while.ws_శిఖరం_label;
	    ఒకవేళ (el != ఖాళీ)
		*el = &scope->se_u.se_while.ws_చివర_label;
	    ఆపు;
	}
	ఒకవేళ (try_scopes != ఖాళీ && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    తిరిగి OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    పూర్ణాంకం		try_scopes = 0;
    పూర్ణాంకం		loop_label;

    ఒకవేళ (compile_కనుగొను_scope(&loop_label, ఖాళీ, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    లేకపోతే
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    తిరిగి arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    పూర్ణాంకం		try_scopes = 0;
    endlabel_T	**el;

    ఒకవేళ (compile_కనుగొను_scope(ఖాళీ, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి arg;

    ఒకవేళ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    ఒకవేళ (compile_jump_గమ్యం_చివర(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	తిరిగి ఖాళీ;

    తిరిగి arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_ఖండం(char_u *arg, cctx_T *cctx)
{
    ఒకవేళ (new_scope(cctx, ఖండం_SCOPE) == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    శూన్య
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_లెక్క, TRUE);
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

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    ఒకవేళ (try_scope == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	ఒకవేళ ((isn = generate_instr(cctx, ISN_TRY)) == ఖాళీ)
	    తిరిగి ఖాళీ;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	ఒకవేళ (isn->isn_arg.tryref.try_ref == ఖాళీ)
	    తిరిగి ఖాళీ;
    }

    // scope for the try block itself
    scope = new_scope(cctx, ఖండం_SCOPE);
    ఒకవేళ (scope == ఖాళీ)
	తిరిగి ఖాళీ;

    తిరిగి arg;
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

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    // end block scope from :try or :catch
    ఒకవేళ (scope != ఖాళీ && scope->se_type == ఖండం_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ఒకవేళ (scope == ఖాళీ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_సంకేతం_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
#నిర్వచితమైతే FEAT_PROFILE
	// the profile-start should be after the jump
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_ప్రారంభం)
	    --instr->ga_len;
#అయితే_ముగింపు
	// Jump from end of previous block to :finally or :endtry
	ఒకవేళ (compile_jump_గమ్యం_చివర(&scope->se_u.se_try.ts_చివర_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    తిరిగి ఖాళీ;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_try.ts_try_label;
	ఒకవేళ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	ఒకవేళ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_చివర);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_ప్రారంభం);
	}
#అయితే_ముగింపు
	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    ఒకవేళ (చివర్లు_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    లేకపోతే
    {
	char_u *చివర;
	char_u *pat;
	char_u *tofree = ఖాళీ;
	పూర్ణాంకం	dropped = 0;
	పూర్ణాంకం	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	చివర = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ఖాళీ);
	ఒకవేళ (*చివర != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (tofree == ఖాళీ)
	    len = (పూర్ణాంకం)(చివర - (p + 1));
	లేకపోతే
	    len = (పూర్ణాంకం)(చివర - tofree);
	pat = vim_strnsave(tofree == ఖాళీ ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	ఒకవేళ (pat == ఖాళీ)
	    తిరిగి ఖాళీ;
	ఒకవేళ (generate_PUSHS(cctx, &pat) == FAIL)
	    తిరిగి ఖాళీ;

	ఒకవేళ (generate_COMPARE(cctx, సమాసం_సరిపోలిక, FALSE) == FAIL)
	    తిరిగి ఖాళీ;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	ఒకవేళ (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    తిరిగి ఖాళీ;
    }

    ఒకవేళ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (new_scope(cctx, ఖండం_SCOPE) == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    పూర్ణాంకం		this_instr;

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    // end block scope from :try or :catch
    ఒకవేళ (scope != ఖాళీ && scope->se_type == ఖండం_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ఒకవేళ (scope == ఖాళీ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	తిరిగి ఖాళీ;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    ఒకవేళ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_try.ts_try_label;
	ఒకవేళ (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_గుణిజం_finally));
	    తిరిగి ఖాళీ;
	}

	this_instr = instr->ga_len;
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_దత్తాంశం)[this_instr - 1]
						   .isn_type == ISN_PROF_ప్రారంభం)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    ఒకవేళ (this_instr > 0 && ((isn_T *)instr->ga_దత్తాంశం)[this_instr - 1]
						     .isn_type == ISN_PROF_చివర)
		--this_instr;
	}
#అయితే_ముగింపు

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_గమ్యం_చివర(&scope->se_u.se_try.ts_చివర_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	ఒకవేళ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	ఒకవేళ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	ఒకవేళ (generate_instr(cctx, ISN_FINALLY) == ఖాళీ)
	    తిరిగి ఖాళీ;
    }

    తిరిగి arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    ఒకవేళ (misplaced_cmdmod(cctx))
	తిరిగి ఖాళీ;

    // end block scope from :catch or :finally
    ఒకవేళ (scope != ఖాళీ && scope->se_type == ఖండం_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ఒకవేళ (scope == ఖాళీ || scope->se_type != TRY_SCOPE)
    {
	ఒకవేళ (scope == ఖాళీ)
	    emsg(_(e_endtry_without_try));
	లేకపోతే ఒకవేళ (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	లేకపోతే ఒకవేళ (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	లేకపోతే
	    emsg(_(e_missing_endif));
	తిరిగి ఖాళీ;
    }

    try_isn = ((isn_T *)instr->ga_దత్తాంశం) + scope->se_u.se_try.ts_try_label;
    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    తిరిగి ఖాళీ;
	}

#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_దత్తాంశం)[instr->ga_len - 1]
						.isn_type == ISN_PROF_ప్రారంభం)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#అయితే_ముగింపు

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_గమ్యం_చివర(&scope->se_u.se_try.ts_చివర_label,
							  instr->ga_len, cctx);

	ఒకవేళ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_దత్తాంశం)
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

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	ఒకవేళ (generate_instr(cctx, ISN_ENDTRY) == ఖాళీ)
	    తిరిగి ఖాళీ;
#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_ప్రారంభం);
#అయితే_ముగింపు
    }
    తిరిగి arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి p;
    ఒకవేళ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (generate_instr_drop(cctx, ISN_THROW, 1) == ఖాళీ)
	తిరిగి ఖాళీ;

    తిరిగి p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    పూర్ణాంకం		name_only;
    దీర్ఘ	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_వ్యక్తీకరణ_without_effect_str), arg);
	తిరిగి ఖాళీ;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    తిరిగి skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    పూర్ణాంకం
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    ఒకవేళ (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	ఒకవేళ (lvar == ఖాళీ)
	    తిరిగి 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    తిరిగి dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_ప్రారంభం, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_ప్రారంభం;
    పూర్ణాంకం		argcount = 0;
    పూర్ణాంకం		defer_var_idx;
    type_T	*type = ఖాళీ;
    పూర్ణాంకం		func_idx = -1;

    ఒకవేళ (*arg == '(')
    {
	// a lambda function
	ఒకవేళ (compile_lambda(&arg, cctx, ఖాళీ) != OK)
	    తిరిగి ఖాళీ;
	paren = vim_strchr(arg, '(');
	ఒకవేళ (paren == ఖాళీ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	ఒకవేళ (paren == ఖాళీ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    తిరిగి ఖాళీ;
	}
	*paren = NUL;
	func_idx = కనుగొను_అంతర్గత_func(arg);
	ఒకవేళ (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)అంతర్గత_func_name(func_idx),
		    &t_func_any, FALSE);
	లేకపోతే ఒకవేళ (compile_expr0(&arg, cctx) == FAIL)
	    తిరిగి ఖాళీ;
	*paren = '(';
    }

    // check for function type
    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_దొంతర(cctx, 0);
	ఒకవేళ (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    తిరిగి ఖాళీ;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    ఒకవేళ (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ (func_idx >= 0)
	{
	    type2_T	*argtypes = ఖాళీ;
	    type2_T	shuffled_argtypes[MAX_FUNC_వాదనలు];

	    ఒకవేళ (check_అంతర్గత_func_వాదనలు(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		తిరిగి ఖాళీ;
	}
	లేకపోతే ఒకవేళ (check_func_వాదనలు_from_type(cctx, type, argcount, TRUE,
		    arg_ప్రారంభం) == FAIL)
	    తిరిగి ఖాళీ;

	defer_var_idx = get_defer_var_idx(cctx);
	ఒకవేళ (defer_var_idx == 0)
	    తిరిగి ఖాళీ;
	ఒకవేళ (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    తిరిగి ఖాళీ;
    }

    తిరిగి skipwhite(arg);
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
compile_mult_సమాసం(
	char_u	*arg,
	పూర్ణాంకం	cmdidx,
	దీర్ఘ	cmd_లెక్క వాడని,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*సమాసం_ప్రారంభం;
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		ప్రారంభం_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    పూర్ణాంకం		r = OK;

    కొరకు (;;)
    {
	ఒకవేళ (చివర్లు_excmd2(prev, p))
	    ఆపు;
	సమాసం_ప్రారంభం = p;
	ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	    తిరిగి ఖాళీ;

	ఒకవేళ (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_వ్యక్తీకరణ_does_not_ఫలితం_లోపల_విలువ_str), సమాసం_ప్రారంభం);
		తిరిగి ఖాళీ;
	    }
	}

	++లెక్క;
	prev = p;
	p = skipwhite(p);
    }

    ఒకవేళ (లెక్క > 0)
    {
	దీర్ఘ save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = ప్రారంభం_ctx_lnum;

	ఒకవేళ (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, లెక్క);
	లేకపోతే ఒకవేళ (cmdidx == CMD_execute)
	    r = generate_MULT_సమాసం(cctx, ISN_EXECUTE, లెక్క);
	లేకపోతే ఒకవేళ (cmdidx == CMD_echomsg)
	    r = generate_MULT_సమాసం(cctx, ISN_ECHOMSG, లెక్క);
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	లేకపోతే ఒకవేళ (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, లెక్క, cmd_లెక్క);
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (cmdidx == CMD_echoconsole)
	    r = generate_MULT_సమాసం(cctx, ISN_ECHOCONSOLE, లెక్క);
	లేకపోతే
	    r = generate_MULT_సమాసం(cctx, ISN_ECHOERR, లెక్క);

	cctx->ctx_lnum = save_lnum;
    }
    తిరిగి r == OK ? p : ఖాళీ;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    స్థిర పూర్ణాంకం
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_చివర = skip_range(eap->cmd, TRUE, ఖాళీ);
    char_u *p = skipdigits(eap->cmd);

    ఒకవేళ (p == range_చివర)
	తిరిగి FAIL;
    తిరిగి generate_RANGE(cctx, vim_strnsave(eap->cmd, range_చివర - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, పూర్ణాంకం fixindent)
{
    char_u	*పంక్తి = arg;
    linenr_T	lnum;
    అక్షరం	*errormsg;
    పూర్ణాంకం		above = eap->forceit;

    eap->regname = *పంక్తి;

    ఒకవేళ (eap->regname == '=')
    {
	char_u *p = skipwhite(పంక్తి + 1);

	ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	    తిరిగి ఖాళీ;
	పంక్తి = p;
    }
    లేకపోతే ఒకవేళ (eap->regname != NUL)
	++పంక్తి;

    ఒకవేళ (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    లేకపోతే
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	ఒకవేళ (parse_cmd_చిరునామా(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    తిరిగి ఖాళీ;
	ఒకవేళ (eap->addr_లెక్క == 0)
	    lnum = -1;
	లేకపోతే
	    lnum = eap->line2;
	ఒకవేళ (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    తిరిగి పంక్తి;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *పంక్తి_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*పంక్తి = పంక్తి_arg;
    char_u	*p;
    పూర్ణాంకం		has_సమాసం = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ఖాళీ;
    char_u	*cmd_arg = ఖాళీ;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	వెళ్ళు theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    ఒకవేళ (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_దత్తాంశం) + instr->ga_len - 1;

	ఒకవేళ (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    ఒకవేళ (eap->cmdidx >= 0 && eap->cmdidx < CMD_కొలత)
    {
	దీర్ఘ	argt = eap->argt;
	పూర్ణాంకం	usefilter = FALSE;

	has_సమాసం = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	ఒకవేళ ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	ఒకవేళ ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    ఒకవేళ (eap->nextcmd != ఖాళీ)
		nextcmd = eap->nextcmd;
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    ఒకవేళ (*p != NUL)
		++p;
	    ఒకవేళ (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    ఒకవేళ (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    పూర్ణాంకం	    flags = 0;  //వాడని
	    పూర్ణాంకం	    ప్రారంభం_lnum = SOURCING_LNUM;

	    CLEAR_క్షేత్రం(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_ఖండం(&ea, పంక్తి, &tofree, &flags);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		పంక్తి = p;
		SOURCING_LNUM = ప్రారంభం_lnum;
	    }
	}
    }

    ఒకవేళ (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_సమాసం = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	ఒకవేళ (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    ఒకవేళ ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	పూర్ణాంకం delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ఖాళీ, ఖాళీ, ఖాళీ);
	ఒకవేళ (*p == delim)
	    cmd_arg = p + 1;
    }

    ఒకవేళ (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    ఒకవేళ (cmd_arg != ఖాళీ)
    {
	exarg_T nea;

	CLEAR_క్షేత్రం(nea);
	nea.cmd = cmd_arg;
	p = కనుగొను_ex_command(&nea, ఖాళీ, lookup_scriptitem, ఖాళీ);
	ఒకవేళ (nea.cmdidx < CMD_కొలత)
	{
	    has_సమాసం = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    ఒకవేళ (has_సమాసం)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    ఒకవేళ (has_సమాసం && (p = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)eap->arg, "`=")) != ఖాళీ)
    {
	పూర్ణాంకం	లెక్క = 0;
	char_u	*ప్రారంభం = skipwhite(పంక్తి);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	కొరకు (;;)
	{
	    ఒకవేళ (p > ప్రారంభం)
	    {
		char_u *val = vim_strnsave(ప్రారంభం, p - ప్రారంభం);

		generate_PUSHS(cctx, &val);
		++లెక్క;
	    }
	    p += 2;
	    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
		తిరిగి ఖాళీ;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++లెక్క;
	    p = skipwhite(p);
	    ఒకవేళ (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		తిరిగి ఖాళీ;
	    }
	    ప్రారంభం = p + 1;

	    p = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)ప్రారంభం, "`=");
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (*skipwhite(ప్రారంభం) != NUL)
		{
		    char_u *val = vim_strsave(ప్రారంభం);

		    generate_PUSHS(cctx, &val);
		    ++లెక్క;
		}
		ఆపు;
	    }
	}
	generate_EXECCONCAT(cctx, లెక్క);
    }
    లేకపోతే
	generate_EXEC_copy(cctx, ISN_EXEC, పంక్తి);

theend:
    ఒకవేళ (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    తిరిగి nextcmd;
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
compile_script(char_u *పంక్తి, cctx_T *cctx)
{
    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	ఒకవేళ ((isn = generate_instr(cctx, ISN_EXEC_విభజించు)) == ఖాళీ)
	    తిరిగి ఖాళీ;
	isn->isn_arg.string = vim_strsave(పంక్తి);
    }
    తిరిగి (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *సమాసం = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)cmd, "\\=");

    ఒకవేళ (సమాసం != ఖాళీ)
    {
	పూర్ణాంకం delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ఖాళీ, ఖాళీ, ఖాళీ);
	ఒకవేళ (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*చివర;
	    పూర్ణాంకం		సమాసం_res;
	    పూర్ణాంకం		trailing_error;
	    పూర్ణాంకం		instr_లెక్క;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    చివర = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_దత్తాంశం = ఖాళీ;
	    సమాసం_res = compile_expr0(&cmd, cctx);
	    ఒకవేళ (చివర[-1] == NUL)
		చివర[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    ఒకవేళ (సమాసం_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		ఒకవేళ (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		తిరిగి ఖాళీ;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_లెక్క = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_దత్తాంశం;
	    instr[instr_లెక్క].isn_type = ISN_ముగింపు;

	    cctx->ctx_instr = save_ga;
	    ఒకవేళ ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ఖాళీ)
	    {
		పూర్ణాంకం idx;

		కొరకు (idx = 0; idx < instr_లెక్క; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		తిరిగి ఖాళీ;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    ఒకవేళ (*చివర == '&')
		++చివర;
	    అయితే (ASCII_ISALPHA(*చివర) || *చివర == '#')
		++చివర;
	    తిరిగి చివర;
	}
    }

    తిరిగి compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *పంక్తి, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    ఒకవేళ (lhs->lhs_name != ఖాళీ)
    {
	ఒకవేళ (STRNCMP(arg, "END", 3) == 0)
	{
	    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
	    {
		ఒకవేళ (lhs->lhs_append)
		{
		    // First load the current variable value.
		    ఒకవేళ (compile_ఎక్కించు_lhs_with_సూచీ(lhs, lhs->lhs_సంపూర్ణ,
								 cctx) == FAIL)
			తిరిగి ఖాళీ;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		ఒకవేళ (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		ఒకవేళ (lhs->lhs_has_సూచీ)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    ఒకవేళ (compile_కేటాయించు_unlet(lhs->lhs_సంపూర్ణ, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			తిరిగి ఖాళీ;
		}
		లేకపోతే ఒకవేళ (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    తిరిగి ఖాళీ;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_సంపూర్ణ);
	    }
	    తిరిగి arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (arg[0] == '=' && arg[1] == '>')
    {
	పూర్ణాంకం	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	ఒకవేళ (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	ఒకవేళ (compile_కేటాయించు_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	లేకపోతే
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    ఒకవేళ (lhs->lhs_has_సూచీ)
	    {
		lhs->lhs_సంపూర్ణ = vim_strnsave(arg, lhs->lhs_varlen_మొత్తం);
		ఒకవేళ (lhs->lhs_సంపూర్ణ == ఖాళీ)
		    తిరిగి ఖాళీ;
	    }
	}

	తిరిగి arg + lhs->lhs_varlen_మొత్తం;
    }

    // other redirects are handled like at script level
    తిరిగి compile_exec(పంక్తి, eap, cctx);
}

#ఒకవేళ defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *పంక్తి, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి ఖాళీ;
    isn->isn_arg.సంఖ్య = eap->cmdidx;

    p = eap->arg;
    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	తిరిగి ఖాళీ;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి ఖాళీ;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    ఒకవేళ (isn->isn_arg.cexpr.cexpr_ref == ఖాళీ)
	తిరిగి ఖాళీ;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(పంక్తి));

    తిరిగి p;
}
#అయితే_ముగింపు

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, పూర్ణాంకం check_return_type, పూర్ణాంకం legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*దొంతర_type;

    ఒకవేళ (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_ప్రారంభం(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	ఒకవేళ (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_విలువ_లోపల_function_without_return_type));
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (legacy)
	{
	    పూర్ణాంకం save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    ఒకవేళ (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి ఖాళీ;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (శూన్య)skip_సమాసం(&p, ఖాళీ);
	    cmdmod.cmod_flags = save_flags;
	}
	లేకపోతే
	{
	    // compile return argument into instructions
	    ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
		తిరిగి ఖాళీ;
	}

	ఒకవేళ (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    దొంతర_type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (check_type_is_విలువ(దొంతర_type) == FAIL)
		తిరిగి ఖాళీ;
	    ఒకవేళ ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ఖాళీ
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = దొంతర_type;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (need_type(దొంతర_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    తిరిగి ఖాళీ;
	    }
	}
    }
    లేకపోతే
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	ఒకవేళ (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_విలువ));
	    తిరిగి ఖాళీ;
	}

	ఒకవేళ (IS_CONSTRUCTOR_పద్ధతి(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	లేకపోతే
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    ఒకవేళ (compile_కనుగొను_scope(ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    ఒకవేళ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ఖాళీ)
	తిరిగి ఖాళీ;

    // "return val | endif" is possible
    తిరిగి skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    పూర్ణాంకం
check_global_and_subst(char_u *cmd, char_u *arg)
{
    ఒకవేళ (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ఖాళీ)
    {
	semsg(_(e_separator_not_supported_str), arg);
	తిరిగి FAIL;
    }
    ఒకవేళ (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}


#అయితే_ముగింపు  // defined(FEAT_EVAL)
