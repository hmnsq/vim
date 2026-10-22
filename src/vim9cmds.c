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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = instr->ga_len;

    ଯେତେବେଳେ (idx > 0)
    {
	ଯଦି (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_data)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    ଜାରି;
	}
#ifdef FEAT_PROFILE
	ଯଦି (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_PROF_START)
	{
	    --idx;
	    ଜାରି;
	}
#endif
	ଯଦି (((isn_T *)instr->ga_data)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
unwind_locals(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ଶୀର୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep)
{
    ଯଦି (cctx->ctx_locals.ga_len > new_ଶୀର୍ଷ)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = new_ଶୀର୍ଷ; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    ଯଦି (!keep)
	cctx->ctx_locals.ga_len = new_ଶୀର୍ଷ;
}

/*
 * Free all local variables.
 */
    ଶୂନ୍ୟ
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_vim9_unlet(char_u *name)
{
    ଯଦି (*name == NUL)
    {
	semsg(_(e_argument_required_for_str), "unlet");
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ଖାଲି)
    {
	// "unlet s:var" is allowed in legacy script.
	ଯଦି (*name == 's' && !in_vim9script())
	    ଫେରନ୍ତୁ OK;
	semsg(_(e_cannot_unlet_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    deep UNUSED,
    ଶୂନ୍ୟ    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc = *name_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ OK;

    *name_end = NUL;
    ଯଦି (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    ଅନ୍ୟଥା ଯଦି (vim_strchr(p, '.') != ଖାଲି || vim_strchr(p, '[') != ଖାଲି)
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
	ଯଦି (ret == OK)
	{
	    ଯଦି (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    ଅନ୍ୟଥା ଯଦି (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    ଅନ୍ୟଥା
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    ଫେରନ୍ତୁ ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lock_unlock(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    deep,
    ଶୂନ୍ୟ    *coookie)
{
    cctx_T	*cctx = coookie;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc = *name_end;
    char_u	*p = lvp->ll_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    ଅକ୍ଷର	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_arg = FALSE;

#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#endif

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ OK;

    ଯଦି (*p == NUL)
    {
	semsg(_(e_argument_required_for_str), cmd);
	ଫେରନ୍ତୁ FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    ଯଦି (p[1] != ':')
    {
	char_u *end = find_name_end(p, ଖାଲି, ଖାଲି, FNE_CHECK_START);

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

	char_u	*name = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = end - p;

	ଯଦି (lookup_local(p, len, ଖାଲି, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    ଯଦି (*end != '.' && *end != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_lock_unlock_local_variable));
		ଫେରନ୍ତୁ FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ifdef LOG_LOCKVAR
	    ch_log(ଖାଲି, "LKVAR:    ... lookup_local: name %s", name);
#endif
	}
	ଯଦି (name == ଖାଲି)
	{
	    class_T *cl;
	    ଯଦି (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		ଯଦି (*end != '.' && *end != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cl->class_name.length;
#ifdef LOG_LOCKVAR
		    ch_log(ଖାଲି, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#endif
		}
	    }
	}
	ଯଦି (name == ଖାଲି)
	{
	    // Can lockvar any function arg.
	    ଯଦି (arg_exists(p, len, ଖାଲି, ଖାଲି, ଖାଲି, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR:    ... arg_exists: name %s", name);
#endif
	    }
	}
	ଯଦି (name == ଖାଲି)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    ଯଦି (script_var_exists(p, len, cctx, ଖାଲି) == OK
		&& (*end == '.' || *end == '['))
	    {
		name = p;
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR:    ... script_var_exists: name %s", name);
#endif
	    }
	}
	ଯଦି (name != ଖାଲି)
	{
#ifdef LOG_LOCKVAR
	    ch_log(ଖାଲି, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#endif
	    ଯଦି (compile_load(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_end = NUL;
    size_t len = name_end - p + 20;
    buf = alloc(len);
    ଯଦି (buf == ଖାଲି)
	ret = FAIL;
    ଅନ୍ୟଥା
    {
	ଯଦି (deep < 0)
	    vim_snprintf((ଅକ୍ଷର *)buf, len, "%s! %s", cmd, p);
	ଅନ୍ୟଥା
	    vim_snprintf((ଅକ୍ଷର *)buf, len, "%s %d %s", cmd, deep, p);
#ifdef LOG_LOCKVAR
	ch_log(ଖାଲି, "LKVAR:    ... buf %s", buf);
#endif
	ଯଦି (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	ଅନ୍ୟଥା
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_end = cc;
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    deep = 0;
    char_u  *p = arg;

    ଯଦି (eap->cmdidx != CMD_unlet)
    {
	ଯଦି (eap->forceit)
	    deep = -1;
	ଅନ୍ୟଥା ଯଦି (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	ଅନ୍ୟଥା
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_lock_unlock,
	    cctx);
    ଫେରନ୍ତୁ eap->nextcmd == ଖାଲି ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_jump_to_end(
	endlabel_T  **el,
	jumpwhen_T  when,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    ଯଦି (endlabel == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    ଯଦି (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    ଅନ୍ୟଥା
	generate_JUMP(cctx, when, 0);
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
compile_fill_jump_to_end(endlabel_T **el, ପୂର୍ଣ୍ଣସଂଖ୍ୟା jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ଯେତେବେଳେ (*el != ଖାଲି)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
compile_free_jump_to_end(endlabel_T **el)
{
    ଯେତେବେଳେ (*el != ଖାଲି)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ସ୍ଥିର scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    ଯଦି (scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_ଗଣନା = cctx->ctx_locals.ga_len;
    ଯଦି (scope->se_outer != ଖାଲି)
	scope->se_loop_depth = scope->se_outer->se_loop_depth;
    ଫେରନ୍ତୁ scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    ଶୂନ୍ୟ
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    ଯଦି (scope == ଖାଲି)
    {
	iemsg("calling drop_scope() without a scope");
	ଫେରନ୍ତୁ;
    }
    cctx->ctx_scope = scope->se_outer;
    ବାଛ (scope->se_type)
    {
	ସ୍ଥିତି IF_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_if.is_end_label); ଭାଙ୍ଗ;
	ସ୍ଥିତି FOR_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_for.fs_end_label); ଭାଙ୍ଗ;
	ସ୍ଥିତି WHILE_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_while.ws_end_label); ଭାଙ୍ଗ;
	ସ୍ଥିତି TRY_SCOPE:
	    compile_free_jump_to_end(&scope->se_u.se_try.ts_end_label); ଭାଙ୍ଗ;
	ସ୍ଥିତି NO_SCOPE:
	ସ୍ଥିତି BLOCK_SCOPE:
	    ଭାଙ୍ଗ;
    }
    vim_free(scope);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ଯଦି (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		instr_ଗଣନା = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ଯଦି (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    ଅନ୍ୟଥା ଯଦି (instr->ga_len == instr_ଗଣନା && ppconst.pp_used == 1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	ଯଦି (error)
	    ଫେରନ୍ତୁ ଖାଲି;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    ଅନ୍ୟଥା
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ଯଦି (generate_ppconst(cctx, &ppconst) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (bool_on_ଗଦା(cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    ଯଦି (scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    ଯଦି (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    ଅନ୍ୟଥା
	scope->se_u.se_if.is_if_label = -1;

#ifdef FEAT_PROFILE
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_END);
	cctx->ctx_skip = SKIP_YES;
    }
#endif

    ଫେରନ୍ତୁ p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		instr_ଗଣନା;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    ଯଦି (scope == ଖାଲି || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_elseif_after_else));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);
    ଯଦି (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    ଯଦି (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_seen_skip_not = TRUE;
    }
    ଯଦି (scope->se_u.se_if.is_seen_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_ପଦସମଷ୍ଟି_cctx(&p, cctx);
	ଫେରନ୍ତୁ p;
    }

    ଯଦି (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_ପଦସମଷ୍ଟି_cctx(&p, cctx);
	ଫେରନ୍ତୁ p;
    }

    ଯଦି (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    moved_cmdmod = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	ଯଦି (((isn_T *)instr->ga_data)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    ଯଦି (GA_GROW_FAILS(instr, 1))
		ଫେରନ୍ତୁ ଖାଲି;
	    ((isn_T *)instr->ga_data)[instr->ga_len] =
				  ((isn_T *)instr->ga_data)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	ଯଦି (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_data)[instr->ga_len];
	    saved_debug = TRUE;
	}

	ଯଦି (compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	ଯଦି (moved_cmdmod)
	    ++instr->ga_len;

	ଯଦି (saved_debug)
	{
	    // move the debug instruction here
	    ଯଦି (GA_GROW_FAILS(instr, 1))
		ଫେରନ୍ତୁ ଖାଲି;
	    ((isn_T *)instr->ga_data)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_FIELD(ppconst);
    ଯଦି (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_START);
#endif
	ଯଦି (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_ଗଣନା = instr->ga_len;
    ଯଦି (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    cctx->ctx_skip = save_skip;
    ଯଦି (!ends_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    ଅନ୍ୟଥା ଯଦି (instr->ga_len == instr_ଗଣନା && ppconst.pp_used == 1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	ଯଦି (error)
	{
	    clear_ppconst(&ppconst);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    ଅନ୍ୟଥା
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ଯଦି (generate_ppconst(cctx, &ppconst) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (bool_on_ଗଦା(cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    ଫେରନ୍ତୁ p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    ଯଦି (scope == ଖାଲି || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (scope->se_u.se_if.is_seen_else)
    {
	emsg(_(e_multiple_else));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);
    ଯଦି (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_seen_else = TRUE;

#ifdef FEAT_PROFILE
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
    {
	ଯଦି (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	ଯଦି (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_seen_skip_not)
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

    ଯଦି (!scope->se_u.se_if.is_seen_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	ଯଦି (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ଯଦି (!cctx->ctx_had_return
		    && compile_jump_to_end(&scope->se_u.se_if.is_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ଯଦି (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_data) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	ଯଦି (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    ଫେରନ୍ତୁ p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (scope == ଖାଲି || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);
    ଯଦି (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    ଯଦି (scope->se_u.se_if.is_if_label >= 0)
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
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
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
    ଫେରନ୍ତୁ arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compile_fill_loop_info(loop_info_T *loop_info, ପୂର୍ଣ୍ଣସଂଖ୍ୟା funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_ଗଣନା = cctx->ctx_locals.ga_len;
    loop_info->li_closure_ଗଣନା = cctx->ctx_closure_ଗଣନା;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ସ୍ଥିର type_T *
compile_for_tuple_get_vartype(type_T *vartype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    ଯଦି (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	ଫେରନ୍ତୁ &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_args[0]->tt_member;
    ଯଦି (member_type->tt_type == VAR_ANY)
	ଫେରନ୍ତୁ &t_any;

    ଯଦି (!var_list)
	// for x in tuple<...list<xxx>>
	ଫେରନ୍ତୁ member_type;

    ଯଦି (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	ଫେରନ୍ତୁ member_type->tt_member;
    ଅନ୍ୟଥା ଯଦି (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	ଫେରନ୍ତୁ member_type->tt_args[0]->tt_member;

    ଫେରନ୍ତୁ &t_any;
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
    char_u	*name = ଖାଲି;
    char_u	*p;
    char_u	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_list = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*item_type = &t_any;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_start, TRUE, &var_ଗଣନା, &semicolon, FALSE);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (var_ଗଣନା == 0)
	var_ଗଣନା = 1;
    ଅନ୍ୟଥା
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    ଯଦି (may_get_next_ଧାଡ଼ି_error(wp, &p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	ଯଦି (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	ଅନ୍ୟଥା
	    emsg(_(e_missing_in_after_for));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    wp = p + 2;
    ଯଦି (may_get_next_ଧାଡ଼ି_error(wp, &p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    ଯଦି (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_data)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    ଯଦି (scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++scope->se_loop_depth;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ସଂଖ୍ୟା);
    ଯଦି (loop_lvar == ଖାଲି)
    {
	drop_scope(cctx);
	ଫେରନ୍ତୁ ଖାଲି;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ସଂଖ୍ୟା);
    ଯଦି (funcref_lvar == ଖାଲି)
    {
	drop_scope(cctx);
	ଫେରନ୍ତୁ ଖାଲି;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    ଯଦି (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    arg_end = arg;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ଗଦା(cctx, 0);
	ଯଦି (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (vartype->tt_type == VAR_STRING)
	    item_type = &t_string;
	ଅନ୍ୟଥା ଯଦି (vartype->tt_type == VAR_BLOB)
	    item_type = &t_ସଂଖ୍ୟା;
	ଅନ୍ୟଥା ଯଦି (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    ଯଦି (!var_list)
		item_type = vartype->tt_member;
	    ଅନ୍ୟଥା ଯଦି (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		item_type = vartype->tt_member->tt_member;
	}
	ଅନ୍ୟଥା ଯଦି (vartype->tt_type == VAR_TUPLE)
	    item_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_ଶୀର୍ଷ_label = current_instr_idx(cctx);

	ଯଦି (cctx->ctx_compile_type == CT_DEBUG)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_prev_lnum = cctx->ctx_prev_lnum;
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
	ଯଦି (var_list)
	{
	    generate_UNPACK(cctx, var_ଗଣନା, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ଗଦା.ga_len;

	    // add type of the items
	    ପାଇଁ (idx = 0; idx < var_ଗଣନା; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : item_type;

		ଯଦି (ଠେଲ_type_ଗଦା(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	}

	ପାଇଁ (idx = 0; idx < var_ଗଣନା; ++idx)
	{
	    assign_dest_T	dest = dest_local;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			opt_flags = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    ଯଦି (name == ଖାଲି)
		ଯାଅ failed;
	    ଯଦି (*skipwhite(p) == ':')
	    {
		ଯଦି (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ଯାଅ failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		ଯଦି (lhs_type == ଖାଲି || !valid_declaration_type(lhs_type))
		    ଯାଅ failed;
	    }

	    ଯଦି (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		ଯାଅ failed;
	    ଯଦି (dest != dest_local)
	    {
		ଯଦି (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ଖାଲି) == FAIL)
		    ଯାଅ failed;
	    }
	    ଅନ୍ୟଥା ଯଦି (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		ଯଦି (generate_instr_drop(cctx, ISN_DROP, 1) == ଖାଲି)
		    ଯାଅ failed;
	    }
	    ଅନ୍ୟଥା
	    {
		// Script var is not supported.
		ଯଦି (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_in_for_loop));
		    ଯାଅ failed;
		}

		ଯଦି (!valid_varname(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)varlen, FALSE))
		    ଯାଅ failed;
		ଯଦି (lookup_local(arg, varlen, ଖାଲି, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    ଯାଅ failed;
		}

		// Reserve a variable to store "var".
		ଯଦି (var_list)
		{
		    where.wt_index = idx + 1;
		    where.wt_kind = WT_VARIABLE;
		}
		ଯଦି (lhs_type == &t_any)
		    lhs_type = item_type;
		ଅନ୍ୟଥା ଯଦି (item_type != &t_unknown
			&& need_type_where(item_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    ଯାଅ failed;
		var_lvar = reserve_local(cctx, arg, varlen, ASSIGN_FINAL,
								     lhs_type);
		ଯଦି (var_lvar == ଖାଲି)
		    // out of memory or used as an argument
		    ଯାଅ failed;

		ଯଦି (semicolon && idx == var_ଗଣନା - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ଖାଲି);
	    }

	    ଯଦି (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_depth = scope->se_loop_depth - 1;
    }

    ଫେରନ୍ତୁ arg_end;

failed:
    vim_free(name);
    drop_scope(cctx);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_loop_end(loop_info_T *loop_info, cctx_T *cctx)
{
    ଯଦି (cctx->ctx_locals.ga_len > loop_info->li_local_ଗଣନା
	    && cctx->ctx_closure_ଗଣନା > loop_info->li_closure_ଗଣନା)
	ଫେରନ୍ତୁ generate_ENDLOOP(cctx, loop_info);
    ଫେରନ୍ତୁ OK;
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

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (scope == ଖାଲି || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ଯଦି (compile_loop_end(&forscope->fs_loop_info, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_ଶୀର୍ଷ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_data) + forscope->fs_ଶୀର୍ଷ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_end = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_to_end(&forscope->fs_end_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	ଯଦି (generate_instr_drop(cctx, ISN_DROP, 1) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    vim_free(scope);

    ଫେରନ୍ତୁ arg;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    ଯଦି (scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (scope->se_loop_depth == MAX_LOOP_DEPTH)
    {
	emsg(_(e_loop_nesting_too_deep));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++scope->se_loop_depth;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_ଶୀର୍ଷ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ASSIGN_VAR, &t_ସଂଖ୍ୟା);
    ଯଦି (funcref_lvar == ଖାଲି)
    {
	drop_scope(cctx);
	ଫେରନ୍ତୁ ଖାଲି;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_depth = scope->se_loop_depth - 1;

    // compile "expr"
    ଯଦି (compile_expr0(&p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!ends_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି (bool_on_ଗଦା(cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	ଯଦି (compile_jump_to_end(&whilescope->ws_end_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (scope == ଖାଲି || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    cctx->ctx_scope = scope->se_outer;
    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ଯଦି (compile_loop_end(&whilescope->ws_loop_info, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);

#ifdef FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_end(cctx, cctx->ctx_lnum);
#endif

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_ଶୀର୍ଷ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_to_end(&scope->se_u.se_while.ws_end_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    ଫେରନ୍ତୁ arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_local_ଗଣନା = 0;

    CLEAR_POINTER(lvi);
    ପାଇଁ (;;)
    {
	loop_info_T	*loopinfo;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_local_last;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_local_ଗଣନା;

	ଯେତେବେଳେ (scope != ଖାଲି && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	ଯଦି (scope == ଖାଲି)
	    ଭାଙ୍ଗ;

	ଯଦି (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_last = loopinfo->li_local_ଗଣନା - 1;
	}
	ଅନ୍ୟଥା
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_last = loopinfo->li_local_ଗଣନା - 3;
	}

	start_local_ଗଣନା = loopinfo->li_local_ଗଣନା;
	ଯଦି (cctx->ctx_locals.ga_len > start_local_ଗଣନା)
	{
	    lvi->lvi_loop[loopinfo->li_depth].var_idx =
						      (short)start_local_ଗଣନା;
	    lvi->lvi_loop[loopinfo->li_depth].var_ଗଣନା =
			  (short)(cctx->ctx_locals.ga_len - start_local_ଗଣନା
							   - prev_local_ଗଣନା);
	    ଯଦି (lvi->lvi_depth == 0)
		lvi->lvi_depth = loopinfo->li_depth + 1;
	}

	scope = scope->se_outer;
	prev_local_ଗଣନା = cctx->ctx_locals.ga_len - cur_local_last;
    }
    ଫେରନ୍ତୁ lvi->lvi_depth > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    ଶୂନ୍ୟ
get_loop_var_idx(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_depth = -1;
    lvar->lv_loop_idx = -1;
    ଯଦି (get_loop_var_info(cctx, &lvi))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth;

	ପାଇଁ (depth = lvi.lvi_depth - 1; depth >= 0; --depth)
	    ଯଦି (idx >= lvi.lvi_loop[depth].var_idx
		    && idx < lvi.lvi_loop[depth].var_idx
					       + lvi.lvi_loop[depth].var_ଗଣନା)
	    {
		lvar->lv_loop_depth = depth;
		lvar->lv_loop_idx = lvi.lvi_loop[depth].var_idx;
		ଫେରନ୍ତୁ;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_find_scope(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *try_scopes,    // :try scopes encountered or NULL
	ଅକ୍ଷର	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ପାଇଁ (;;)
    {
	ଯଦି (scope == ଖାଲି)
	{
	    ଯଦି (error != ଖାଲି)
		emsg(_(error));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (scope->se_type == FOR_SCOPE)
	{
	    ଯଦି (compile_loop_end(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (loop_label != ଖାଲି)
		*loop_label = scope->se_u.se_for.fs_ଶୀର୍ଷ_label;
	    ଯଦି (el != ଖାଲି)
		*el = &scope->se_u.se_for.fs_end_label;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (scope->se_type == WHILE_SCOPE)
	{
	    ଯଦି (compile_loop_end(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (loop_label != ଖାଲି)
		*loop_label = scope->se_u.se_while.ws_ଶୀର୍ଷ_label;
	    ଯଦି (el != ଖାଲି)
		*el = &scope->se_u.se_while.ws_end_label;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (try_scopes != ଖାଲି && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try_scopes = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop_label;

    ଯଦି (compile_find_scope(&loop_label, ଖାଲି, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    ଅନ୍ୟଥା
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    ଫେରନ୍ତୁ arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		try_scopes = 0;
    endlabel_T	**el;

    ଯଦି (compile_find_scope(ଖାଲି, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ arg;

    ଯଦି (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    ଯଦି (compile_jump_to_end(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_block(char_u *arg, cctx_T *cctx)
{
    ଯଦି (new_scope(cctx, BLOCK_SCOPE) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    ଶୂନ୍ୟ
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_ଗଣନା, TRUE);
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

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    ଯଦି (try_scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	ଯଦି ((isn = generate_instr(cctx, ISN_TRY)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	ଯଦି (isn->isn_arg.tryref.try_ref == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    // scope for the try block itself
    scope = new_scope(cctx, BLOCK_SCOPE);
    ଯଦି (scope == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ arg;
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

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    // end block scope from :try or :catch
    ଯଦି (scope != ଖାଲି && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ଯଦି (scope == ଖାଲି || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_code_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
#ifdef FEAT_PROFILE
	// the profile-start should be after the jump
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_data)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_START)
	    --instr->ga_len;
#endif
	// Jump from end of previous block to :finally or :endtry
	ଯଦି (compile_jump_to_end(&scope->se_u.se_try.ts_end_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ଯଦି (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	ଯଦି (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_END);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	ଯଦି (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    ଯଦି (ends_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    ଅନ୍ୟଥା
    {
	char_u *end;
	char_u *pat;
	char_u *tofree = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dropped = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	end = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ଖାଲି);
	ଯଦି (*end != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (tofree == ଖାଲି)
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - (p + 1));
	ଅନ୍ୟଥା
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - tofree);
	pat = vim_strnsave(tofree == ଖାଲି ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	ଯଦି (pat == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (generate_PUSHS(cctx, &pat) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (generate_COMPARE(cctx, ପଦସମଷ୍ଟି_MATCH, FALSE) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	ଯଦି (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (new_scope(cctx, BLOCK_SCOPE) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		this_instr;

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    // end block scope from :try or :catch
    ଯଦି (scope != ଖାଲି && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ଯଦି (scope == ଖାଲି || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    ଯଦି (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
	ଯଦି (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_multiple_finally));
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	this_instr = instr->ga_len;
#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_data)[this_instr - 1]
						   .isn_type == ISN_PROF_START)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    ଯଦି (this_instr > 0 && ((isn_T *)instr->ga_data)[this_instr - 1]
						     .isn_type == ISN_PROF_END)
		--this_instr;
	}
#endif

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_to_end(&scope->se_u.se_try.ts_end_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	ଯଦି (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	ଯଦି (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	ଯଦି (generate_instr(cctx, ISN_FINALLY) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    ଯଦି (misplaced_cmdmod(cctx))
	ଫେରନ୍ତୁ ଖାଲି;

    // end block scope from :catch or :finally
    ଯଦି (scope != ଖାଲି && scope->se_type == BLOCK_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ଯଦି (scope == ଖାଲି || scope->se_type != TRY_SCOPE)
    {
	ଯଦି (scope == ଖାଲି)
	    emsg(_(e_endtry_without_try));
	ଅନ୍ୟଥା ଯଦି (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	ଅନ୍ୟଥା ଯଦି (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	ଅନ୍ୟଥା
	    emsg(_(e_missing_endif));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    try_isn = ((isn_T *)instr->ga_data) + scope->se_u.se_try.ts_try_label;
    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    ଫେରନ୍ତୁ ଖାଲି;
	}

#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE
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

	ଯଦି (scope->se_u.se_try.ts_catch_label != 0)
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

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	ଯଦି (generate_instr(cctx, ISN_ENDTRY) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_START);
#endif
    }
    ଫେରନ୍ତୁ arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    ଯଦି (compile_expr0(&p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ p;
    ଯଦି (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (generate_instr_drop(cctx, ISN_THROW, 1) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		name_only;
    long	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    ଯଦି (compile_expr0(&p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_expression_without_effect_str), arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    ଫେରନ୍ତୁ skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    ଯଦି (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	ଯଦି (lvar == ଖାଲି)
	    ଫେରନ୍ତୁ 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    ଫେରନ୍ତୁ dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_start, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		defer_var_idx;
    type_T	*type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		func_idx = -1;

    ଯଦି (*arg == '(')
    {
	// a lambda function
	ଯଦି (compile_lambda(&arg, cctx, ଖାଲି) != OK)
	    ଫେରନ୍ତୁ ଖାଲି;
	paren = vim_strchr(arg, '(');
	ଯଦି (paren == ଖାଲି)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	ଯଦି (paren == ଖାଲି)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	*paren = NUL;
	func_idx = find_internal_func(arg);
	ଯଦି (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)internal_func_name(func_idx),
		    &t_func_any, FALSE);
	ଅନ୍ୟଥା ଯଦି (compile_expr0(&arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	*paren = '(';
    }

    // check for function type
    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ଗଦା(cctx, 0);
	ଯଦି (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    ଯଦି (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି (func_idx >= 0)
	{
	    type2_T	*argtypes = ଖାଲି;
	    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];

	    ଯଦି (check_internal_func_args(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (check_func_args_from_type(cctx, type, argcount, TRUE,
		    arg_start) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	defer_var_idx = get_defer_var_idx(cctx);
	ଯଦି (defer_var_idx == 0)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ skipwhite(arg);
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
compile_mult_ପଦସମଷ୍ଟି(
	char_u	*arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmdidx,
	long	cmd_ଗଣନା UNUSED,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*ପଦସମଷ୍ଟି_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = OK;

    ପାଇଁ (;;)
    {
	ଯଦି (ends_excmd2(prev, p))
	    ଭାଙ୍ଗ;
	ପଦସମଷ୍ଟି_start = p;
	ଯଦି (compile_expr0(&p, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_ମୂଲ୍ୟ_str), ପଦସମଷ୍ଟି_start);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}

	++ଗଣନା_2;
	prev = p;
	p = skipwhite(p);
    }

    ଯଦି (ଗଣନା_2 > 0)
    {
	long save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = start_ctx_lnum;

	ଯଦି (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (cmdidx == CMD_execute)
	    r = generate_MULT_ପଦସମଷ୍ଟି(cctx, ISN_EXECUTE, ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (cmdidx == CMD_echomsg)
	    r = generate_MULT_ପଦସମଷ୍ଟି(cctx, ISN_ECHOMSG, ଗଣନା_2);
#ifdef HAS_MESSAGE_WINDOW
	ଅନ୍ୟଥା ଯଦି (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, ଗଣନା_2, cmd_ଗଣନା);
#endif
	ଅନ୍ୟଥା ଯଦି (cmdidx == CMD_echoconsole)
	    r = generate_MULT_ପଦସମଷ୍ଟି(cctx, ISN_ECHOCONSOLE, ଗଣନା_2);
	ଅନ୍ୟଥା
	    r = generate_MULT_ପଦସମଷ୍ଟି(cctx, ISN_ECHOERR, ଗଣନା_2);

	cctx->ctx_lnum = save_lnum;
    }
    ଫେରନ୍ତୁ r == OK ? p : ଖାଲି;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_end = skip_range(eap->cmd, TRUE, ଖାଲି);
    char_u *p = skipdigits(eap->cmd);

    ଯଦି (p == range_end)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ generate_RANGE(cctx, vim_strnsave(eap->cmd, range_end - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixindent)
{
    char_u	*ଧାଡ଼ି = arg;
    linenr_T	lnum;
    ଅକ୍ଷର	*errormsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		above = eap->forceit;

    eap->regname = *ଧାଡ଼ି;

    ଯଦି (eap->regname == '=')
    {
	char_u *p = skipwhite(ଧାଡ଼ି + 1);

	ଯଦି (compile_expr0(&p, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଧାଡ଼ି = p;
    }
    ଅନ୍ୟଥା ଯଦି (eap->regname != NUL)
	++ଧାଡ଼ି;

    ଯଦି (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    ଅନ୍ୟଥା
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	ଯଦି (parse_cmd_address(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (eap->addr_ଗଣନା == 0)
	    lnum = -1;
	ଅନ୍ୟଥା
	    lnum = eap->line2;
	ଯଦି (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *ଧାଡ଼ି_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*ଧାଡ଼ି = ଧାଡ଼ି_arg;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_ପଦସମଷ୍ଟି = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ଖାଲି;
    char_u	*cmd_arg = ଖାଲି;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଯାଅ theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    ଯଦି (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

	ଯଦି (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    ଯଦି (eap->cmdidx >= 0 && eap->cmdidx < CMD_SIZE)
    {
	long	argt = eap->argt;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	usefilter = FALSE;

	has_ପଦସମଷ୍ଟି = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	ଯଦି ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	ଯଦି ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    ଯଦି (eap->nextcmd != ଖାଲି)
		nextcmd = eap->nextcmd;
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    ଯଦି (*p != NUL)
		++p;
	    ଯଦି (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    ଯଦି (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = 0;  // unused
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    start_lnum = SOURCING_LNUM;

	    CLEAR_FIELD(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_block(&ea, ଧାଡ଼ି, &tofree, &flags);
	    ଯଦି (tofree != ଖାଲି)
	    {
		ଧାଡ଼ି = p;
		SOURCING_LNUM = start_lnum;
	    }
	}
    }

    ଯଦି (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_ପଦସମଷ୍ଟି = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	ଯଦି (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    ଯଦି ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ଖାଲି, ଖାଲି, ଖାଲି);
	ଯଦି (*p == delim)
	    cmd_arg = p + 1;
    }

    ଯଦି (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    ଯଦି (cmd_arg != ଖାଲି)
    {
	exarg_T nea;

	CLEAR_FIELD(nea);
	nea.cmd = cmd_arg;
	p = find_ex_command(&nea, ଖାଲି, lookup_scriptitem, ଖାଲି);
	ଯଦି (nea.cmdidx < CMD_SIZE)
	{
	    has_ପଦସମଷ୍ଟି = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    ଯଦି (has_ପଦସମଷ୍ଟି)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    ଯଦି (has_ପଦସମଷ୍ଟି && (p = (char_u *)strstr((ଅକ୍ଷର *)eap->arg, "`=")) != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = 0;
	char_u	*start = skipwhite(ଧାଡ଼ି);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	ପାଇଁ (;;)
	{
	    ଯଦି (p > start)
	    {
		char_u *val = vim_strnsave(start, p - start);

		generate_PUSHS(cctx, &val);
		++ଗଣନା_2;
	    }
	    p += 2;
	    ଯଦି (compile_expr0(&p, cctx) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++ଗଣନା_2;
	    p = skipwhite(p);
	    ଯଦି (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    start = p + 1;

	    p = (char_u *)strstr((ଅକ୍ଷର *)start, "`=");
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (*skipwhite(start) != NUL)
		{
		    char_u *val = vim_strsave(start);

		    generate_PUSHS(cctx, &val);
		    ++ଗଣନା_2;
		}
		ଭାଙ୍ଗ;
	    }
	}
	generate_EXECCONCAT(cctx, ଗଣନା_2);
    }
    ଅନ୍ୟଥା
	generate_EXEC_copy(cctx, ISN_EXEC, ଧାଡ଼ି);

theend:
    ଯଦି (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    ଫେରନ୍ତୁ nextcmd;
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
compile_script(char_u *ଧାଡ଼ି, cctx_T *cctx)
{
    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	ଯଦି ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	isn->isn_arg.string = vim_strsave(ଧାଡ଼ି);
    }
    ଫେରନ୍ତୁ (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *ପଦସମଷ୍ଟି = (char_u *)strstr((ଅକ୍ଷର *)cmd, "\\=");

    ଯଦି (ପଦସମଷ୍ଟି != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ଖାଲି, ଖାଲି, ଖାଲି);
	ଯଦି (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*end;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ପଦସମଷ୍ଟି_res;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trailing_error;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		instr_ଗଣନା;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    end = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_data = ଖାଲି;
	    ପଦସମଷ୍ଟି_res = compile_expr0(&cmd, cctx);
	    ଯଦି (end[-1] == NUL)
		end[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    ଯଦି (ପଦସମଷ୍ଟି_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		ଯଦି (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		ଫେରନ୍ତୁ ଖାଲି;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_ଗଣନା = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_data;
	    instr[instr_ଗଣନା].isn_type = ISN_FINISH;

	    cctx->ctx_instr = save_ga;
	    ଯଦି ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

		ପାଇଁ (idx = 0; idx < instr_ଗଣନା; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    ଯଦି (*end == '&')
		++end;
	    ଯେତେବେଳେ (ASCII_ISALPHA(*end) || *end == '#')
		++end;
	    ଫେରନ୍ତୁ end;
	}
    }

    ଫେରନ୍ତୁ compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *ଧାଡ଼ି, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    ଯଦି (lhs->lhs_name != ଖାଲି)
    {
	ଯଦି (STRNCMP(arg, "END", 3) == 0)
	{
	    ଯଦି (cctx->ctx_skip != SKIP_YES)
	    {
		ଯଦି (lhs->lhs_append)
		{
		    // First load the current variable value.
		    ଯଦି (compile_load_lhs_with_index(lhs, lhs->lhs_whole,
								 cctx) == FAIL)
			ଫେରନ୍ତୁ ଖାଲି;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		ଯଦି (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		ଯଦି (lhs->lhs_has_index)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    ଯଦି (compile_assign_unlet(lhs->lhs_whole, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			ଫେରନ୍ତୁ ଖାଲି;
		}
		ଅନ୍ୟଥା ଯଦି (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    ଫେରନ୍ତୁ ଖାଲି;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_whole);
	    }
	    ଫେରନ୍ତୁ arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (arg[0] == '=' && arg[1] == '>')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	ଯଦି (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	ଯଦି (compile_assign_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	ଅନ୍ୟଥା
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    ଯଦି (lhs->lhs_has_index)
	    {
		lhs->lhs_whole = vim_strnsave(arg, lhs->lhs_varlen_total);
		ଯଦି (lhs->lhs_whole == ଖାଲି)
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	}

	ଫେରନ୍ତୁ arg + lhs->lhs_varlen_total;
    }

    // other redirects are handled like at script level
    ଫେରନ୍ତୁ compile_exec(ଧାଡ଼ି, eap, cctx);
}

#ଯଦି defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *ଧାଡ଼ି, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    isn->isn_arg.ସଂଖ୍ୟା = eap->cmdidx;

    p = eap->arg;
    ଯଦି (compile_expr0(&p, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    ଯଦି (isn->isn_arg.cexpr.cexpr_ref == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(ଧାଡ଼ି));

    ଫେରନ୍ତୁ p;
}
#endif

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_return_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ଗଦା_type;

    ଯଦି (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_start(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	ଯଦି (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_ମୂଲ୍ୟ_in_function_without_return_type));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (legacy)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    ଯଦି (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି(&p, ଖାଲି);
	    cmdmod.cmod_flags = save_flags;
	}
	ଅନ୍ୟଥା
	{
	    // compile return argument into instructions
	    ଯଦି (compile_expr0(&p, cctx) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ଗଦା_type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (check_type_is_ମୂଲ୍ୟ(ଗଦା_type) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	    ଯଦି ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ଖାଲି
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ଗଦା_type;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (need_type(ଗଦା_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	ଯଦି (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_ମୂଲ୍ୟ));
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	ଯଦି (IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	ଅନ୍ୟଥା
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    ଯଦି (compile_find_scope(ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    ଯଦି (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // "return val | endif" is possible
    ଫେରନ୍ତୁ skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_global_and_subst(char_u *cmd, char_u *arg)
{
    ଯଦି (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ଖାଲି)
    {
	semsg(_(e_separator_not_supported_str), arg);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}


#endif  // defined(FEAT_EVAL)
