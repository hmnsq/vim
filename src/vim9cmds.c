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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Get the index of the current instruction.
 * This compensates for a preceding ISN_CMDMOD and ISN_PROF_START.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
current_instr_idx(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಂಕ		idx = instr->ga_len;

    ಆಗ (idx > 0)
    {
	ಆದರೆ (cctx->ctx_has_cmdmod && ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[idx - 1]
						       .isn_type == ISN_CMDMOD)
	{
	    --idx;
	    ಮುಂದುವರಿ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (((isn_T *)instr->ga_ದತ್ತಾಂಶ)[idx - 1].isn_type == ISN_PROF_ಆರಂಭ)
	{
	    --idx;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (((isn_T *)instr->ga_ದತ್ತಾಂಶ)[idx - 1].isn_type == ISN_DEBUG)
	{
	    --idx;
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ idx;
}
/*
 * Remove local variables above "new_top".
 * Do this by clearing the name.  If "keep" is TRUE do not reset the length, a
 * closure may still need location of the variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unwind_locals(cctx_T *cctx, ಪೂರ್ಣಾಂಕ new_ತುದಿ, ಪೂರ್ಣಾಂಕ keep)
{
    ಆದರೆ (cctx->ctx_locals.ga_len > new_ತುದಿ)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = new_ತುದಿ; idx < cctx->ctx_locals.ga_len; ++idx)
	{
	    lvar_T *lvar = ((lvar_T *)cctx->ctx_locals.ga_ದತ್ತಾಂಶ) + idx;
	    VIM_CLEAR(lvar->lv_name);
	}
    ಆದರೆ (!keep)
	cctx->ctx_locals.ga_len = new_ತುದಿ;
}

/*
 * Free all local variables.
 */
    ಶೂನ್ಯ
free_locals(cctx_T *cctx)
{
    unwind_locals(cctx, 0, FALSE);
    ga_clear(&cctx->ctx_locals);
}


/*
 * Check if "name" can be "unlet".
 */
    ಪೂರ್ಣಾಂಕ
check_vim9_unlet(char_u *name)
{
    ಆದರೆ (*name == NUL)
    {
	semsg(_(e_ವಾದ_required_for_str), "unlet");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (name[1] != ':' || vim_strchr((char_u *)"gwtb", *name) == ಖಾಲಿ)
    {
	// "unlet s:var" is allowed in legacy script.
	ಆದರೆ (*name == 's' && !ಒಳ_vim9script())
	    ಹಿಂತಿರುಗಿ OK;
	semsg(_(e_cannot_unlet_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Callback passed to ex_unletlock().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_unlet(
    lval_T  *lvp,
    char_u  *name_ಕೊನೆ,
    exarg_T *eap,
    ಪೂರ್ಣಾಂಕ	    deep ಬಳಸದ,
    ಶೂನ್ಯ    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    ಪೂರ್ಣಾಂಕ		cc = *name_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ OK;

    *name_ಕೊನೆ = NUL;
    ಆದರೆ (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(p, '.') != ಖಾಲಿ || vim_strchr(p, '[') != ಖಾಲಿ)
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
	ಆದರೆ (ret == OK)
	{
	    ಆದರೆ (!lhs.lhs_has_ಸೂಚ್ಯಂಕ)
	    {
		semsg(_(e_cannot_unlet_imported_ಅಂಶ_str), p);
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ret = compile_ನಿಯೋಜಿಸು_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_ಕೊನೆ = cc;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Callback passed to ex_unletlock().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ಬೀಗ_unlock(
    lval_T  *lvp,
    char_u  *name_ಕೊನೆ,
    exarg_T *eap,
    ಪೂರ್ಣಾಂಕ	    deep,
    ಶೂನ್ಯ    *coookie)
{
    cctx_T	*cctx = coookie;
    ಪೂರ್ಣಾಂಕ		cc = *name_ಕೊನೆ;
    char_u	*p = lvp->ll_name;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    char_u	*buf;
    isntype_T	isn = ISN_EXEC;
    ಅಕ್ಷರ	*cmd = eap->cmdidx == CMD_lockvar ? "lockvar" : "unlockvar";
    ಪೂರ್ಣಾಂಕ		is_arg = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: compile_lock_unlock(): cookie %p, name %s",
								coookie, p);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (*p == NUL)
    {
	semsg(_(e_ವಾದ_required_for_str), cmd);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Cannot use :lockvar and :unlockvar on local variables.
    ಆದರೆ (p[1] != ':')
    {
	char_u *ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(p, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);

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

	char_u	*name = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	len = ಕೊನೆ - p;

	ಆದರೆ (lookup_local(p, len, ಖಾಲಿ, cctx) == OK)
	{
	    // Handle "this", "this.val", "anyvar[idx]"
	    ಆದರೆ (*ಕೊನೆ != '.' && *ಕೊನೆ != '['
				&& (len != 4 || STRNCMP("this", p, len) != 0))
	    {
		emsg(_(e_cannot_ಬೀಗ_unlock_local_variable));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    // Push the local on the stack, could be "this".
	    name = p;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
	    ch_log(ಖಾಲಿ, "LKVAR:    ... lookup_local: name %s", name);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (name == ಖಾಲಿ)
	{
	    class_T *cl;
	    ಆದರೆ (cctx_class_member_idx(cctx, p, len, &cl) >= 0)
	    {
		ಆದರೆ (*ಕೊನೆ != '.' && *ಕೊನೆ != '[')
		{
		    // Push the class of the bare class variable name
		    name = cl->class_name.string;
		    len = (ಪೂರ್ಣಾಂಕ)cl->class_name.ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		    ch_log(ಖಾಲಿ, "LKVAR:    ... cctx_class_member: name %s",
			   name);
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	}
	ಆದರೆ (name == ಖಾಲಿ)
	{
	    // Can lockvar any function arg.
	    ಆದರೆ (arg_exists(p, len, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) == OK)
	    {
		name = p;
		is_arg = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR:    ... arg_exists: name %s", name);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಆದರೆ (name == ಖಾಲಿ)
	{
	    // No special handling for a bare script variable; but
	    // if followed by '[' or '.', it's a root for get_lval().
	    ಆದರೆ (script_var_exists(p, len, cctx, ಖಾಲಿ) == OK
		&& (*ಕೊನೆ == '.' || *ಕೊನೆ == '['))
	    {
		name = p;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR:    ... script_var_exists: name %s", name);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಆದರೆ (name != ಖಾಲಿ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
	    ch_log(ಖಾಲಿ, "LKVAR:    ... INS_LOCKUNLOCK %s", name);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (compile_ತುಂಬು(&name, len, name + len, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    isn = ISN_LOCKUNLOCK;
	}
    }

    // Checking is done at runtime.
    *name_ಕೊನೆ = NUL;
    size_t len = name_ಕೊನೆ - p + 20;
    buf = alloc(len);
    ಆದರೆ (buf == ಖಾಲಿ)
	ret = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (deep < 0)
	    vim_snprintf((ಅಕ್ಷರ *)buf, len, "%s! %s", cmd, p);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_snprintf((ಅಕ್ಷರ *)buf, len, "%s %d %s", cmd, deep, p);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
	ch_log(ಖಾಲಿ, "LKVAR:    ... buf %s", buf);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (isn == ISN_LOCKUNLOCK)
	    ret = generate_LOCKUNLOCK(cctx, buf, is_arg);
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = generate_EXEC_copy(cctx, isn, buf);

	vim_free(buf);
	*name_ಕೊನೆ = cc;
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * compile "unlet var", "lock var" and "unlock var"
 * "arg" points to "var".
 */
    char_u *
compile_unletlock(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ	    deep = 0;
    char_u  *p = arg;

    ಆದರೆ (eap->cmdidx != CMD_unlet)
    {
	ಆದರೆ (eap->forceit)
	    deep = -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_isdigit(*p))
	{
	    deep = getdigits(&p);
	    p = skipwhite(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    deep = 2;
    }

    ex_unletlock(eap, p, deep, GLV_NO_AUTOLOAD | GLV_COMPILING,
	    eap->cmdidx == CMD_unlet ? compile_unlet : compile_ಬೀಗ_unlock,
	    cctx);
    ಹಿಂತಿರುಗಿ eap->nextcmd == ಖಾಲಿ ? (char_u *)"" : eap->nextcmd;
}

/*
 * Generate a jump to the ":endif"/":endfor"/":endwhile"/":finally"/":endtry".
 * "funcref_idx" is used for JUMP_WHILE_FALSE
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_jump_ಗಮ್ಯ_ಕೊನೆ(
	endlabel_T  **el,
	jumpwhen_T  when,
	ಪೂರ್ಣಾಂಕ	    funcref_idx,
	cctx_T	    *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    ಆದರೆ (endlabel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    endlabel->el_ಮುಂದಿನ = *el;
    *el = endlabel;
    endlabel->el_ಕೊನೆ_label = instr->ga_len;

    ಆದರೆ (when == JUMP_WHILE_FALSE)
	generate_WHILE(cctx, funcref_idx);
    ಇಲ್ಲದಿದ್ದರೆ
	generate_JUMP(cctx, when, 0);
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(endlabel_T **el, ಪೂರ್ಣಾಂಕ jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ಆಗ (*el != ಖಾಲಿ)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + cur->el_ಕೊನೆ_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_ಮುಂದಿನ;
	vim_free(cur);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
compile_free_jump_ಗಮ್ಯ_ಕೊನೆ(endlabel_T **el)
{
    ಆಗ (*el != ಖಾಲಿ)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_ಮುಂದಿನ;
	vim_free(cur);
    }
}

/*
 * Create a new scope and set up the generic items.
 */
    ಸ್ಥಿರ scope_T *
new_scope(cctx_T *cctx, scopetype_T type)
{
    scope_T *scope = ALLOC_CLEAR_ONE(scope_T);

    ಆದರೆ (scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    scope->se_outer = cctx->ctx_scope;
    cctx->ctx_scope = scope;
    scope->se_type = type;
    scope->se_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len;
    ಆದರೆ (scope->se_outer != ಖಾಲಿ)
	scope->se_loop_ಆಳ = scope->se_outer->se_loop_ಆಳ;
    ಹಿಂತಿರುಗಿ scope;
}

/*
 * Free the current scope and go back to the outer scope.
 */
    ಶೂನ್ಯ
drop_scope(cctx_T *cctx)
{
    scope_T *scope = cctx->ctx_scope;

    ಆದರೆ (scope == ಖಾಲಿ)
    {
	iemsg("calling drop_scope() without a scope");
	ಹಿಂತಿರುಗಿ;
    }
    cctx->ctx_scope = scope->se_outer;
    ಆಯ್ಕೆ (scope->se_type)
    {
	ಸ್ಥಿತಿ IF_SCOPE:
	    compile_free_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_if.is_ಕೊನೆ_label); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FOR_SCOPE:
	    compile_free_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_for.fs_ಕೊನೆ_label); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WHILE_SCOPE:
	    compile_free_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_while.ws_ಕೊನೆ_label); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TRY_SCOPE:
	    compile_free_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_try.ts_ಕೊನೆ_label); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NO_SCOPE:
	ಸ್ಥಿತಿ ಖಂಡ_SCOPE:
	    ನಿಲ್ಲಿಸು;
    }
    vim_free(scope);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
misplaced_cmdmod(cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    ಆದರೆ (cctx->ctx_has_cmdmod
	    && ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1].isn_type
								 == ISN_CMDMOD)
    {
	emsg(_(e_misplaced_command_modifier));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ		instr_ಎಣಿಕೆ = instr->ga_len;
    scope_T	*scope;
    skip_T	skip_save = cctx->ctx_skip;
    ppconst_T	ppconst;

    CLEAR_ಕ್ಷೇತ್ರ(ppconst);
    ಆದರೆ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (!ತುದಿಗಳು_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	clear_ppconst(&ppconst);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (instr->ga_len == instr_ಎಣಿಕೆ && ppconst.pp_ಬಳಸಿದ == 1)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;
	ಪೂರ್ಣಾಂಕ v;

	// The expression results in a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	clear_ppconst(&ppconst);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ಆದರೆ (generate_ppconst(cctx, &ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // CMDMOD_REV must come before the jump
    generate_undo_cmdmods(cctx);

    scope = new_scope(cctx, IF_SCOPE);
    ಆದರೆ (scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    scope->se_skip_save = skip_save;
    // "is_had_return" will be reset if any block does not end in :return
    scope->se_u.se_if.is_had_return = TRUE;

    ಆದರೆ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	scope->se_u.se_if.is_if_label = -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
						      && skip_save != SKIP_YES)
    {
	// generated a profile start, need to generate a profile end, since it
	// won't be done after returning
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_ಕೊನೆ);
	cctx->ctx_skip = SKIP_YES;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ p;
}

    char_u *
compile_elseif(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಂಕ		instr_ಎಣಿಕೆ;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;
    ppconst_T	ppconst;
    skip_T	save_skip = cctx->ctx_skip;

    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_elseif_without_if));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (scope->se_u.se_if.is_ನೋಡಿದ_else)
    {
	emsg(_(e_elseif_after_else));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಆದರೆ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;

    ಆದರೆ (cctx->ctx_skip == SKIP_NOT)
    {
	// previous block was executed, this one and following will not
	cctx->ctx_skip = SKIP_YES;
	scope->se_u.se_if.is_ನೋಡಿದ_skip_not = TRUE;
    }
    ಆದರೆ (scope->se_u.se_if.is_ನೋಡಿದ_skip_not)
    {
	// A previous block was executed, skip over expression and bail out.
	// Do not count the "elseif" for profiling and cmdmod
	instr->ga_len = current_instr_idx(cctx);

	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(&p, cctx);
	ಹಿಂತಿರುಗಿ p;
    }

    ಆದರೆ (scope->se_skip_save == SKIP_YES)
    {
	// Enclosing outer block is dead, skip this elseif
	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(&p, cctx);
	ಹಿಂತಿರುಗಿ p;
    }

    ಆದರೆ (cctx->ctx_skip == SKIP_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ	    moved_cmdmod = FALSE;
	ಪೂರ್ಣಾಂಕ	    saved_debug = FALSE;
	isn_T	    debug_isn;

	// Move any CMDMOD instruction to after the jump
	ಆದರೆ (((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1].isn_type == ISN_CMDMOD)
	{
	    ಆದರೆ (GA_GROW_FAILS(instr, 1))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len] =
				  ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1];
	    --instr->ga_len;
	    moved_cmdmod = TRUE;
	}

	// Remove the already generated ISN_DEBUG, it is written below the
	// ISN_FOR instruction.
	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
		&& ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
	{
	    --instr->ga_len;
	    debug_isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len];
	    saved_debug = TRUE;
	}

	ಆದರೆ (compile_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_if.is_ಕೊನೆ_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;

	ಆದರೆ (moved_cmdmod)
	    ++instr->ga_len;

	ಆದರೆ (saved_debug)
	{
	    // move the debug instruction here
	    ಆದರೆ (GA_GROW_FAILS(instr, 1))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len] = debug_isn;
	    ++instr->ga_len;
	}
    }

    // compile "expr"; if we know it evaluates to FALSE skip the block
    CLEAR_ಕ್ಷೇತ್ರ(ppconst);
    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
    {
	cctx->ctx_skip = SKIP_UNKNOWN;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
	    // the previous block was skipped, need to profile this line
	    generate_instr(cctx, ISN_PROF_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG)
	    // the previous block was skipped, may want to debug this line
	    generate_instr_debug(cctx);
    }

    instr_ಎಣಿಕೆ = instr->ga_len;
    ಆದರೆ (compile_expr1(&p, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    cctx->ctx_skip = save_skip;
    ಆದರೆ (!ತುದಿಗಳು_excmd2(arg, skipwhite(p)))
    {
	clear_ppconst(&ppconst);
	semsg(_(e_trailing_characters_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (scope->se_skip_save == SKIP_YES)
	clear_ppconst(&ppconst);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (instr->ga_len == instr_ಎಣಿಕೆ && ppconst.pp_ಬಳಸಿದ == 1)
    {
	ಪೂರ್ಣಾಂಕ error = FALSE;
	ಪೂರ್ಣಾಂಕ v;

	// The expression result is a constant.
	v = tv_get_bool_chk(&ppconst.pp_tv[0], &error);
	ಆದರೆ (error)
	{
	    clear_ppconst(&ppconst);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	cctx->ctx_skip = v ? SKIP_NOT : SKIP_YES;
	clear_ppconst(&ppconst);
	scope->se_u.se_if.is_if_label = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Not a constant, generate instructions for the expression.
	cctx->ctx_skip = SKIP_UNKNOWN;
	ಆದರೆ (generate_ppconst(cctx, &ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "where" is set when ":elseif", "else" or ":endif" is found
	scope->se_u.se_if.is_if_label = instr->ga_len;
	generate_JUMP(cctx, JUMP_IF_FALSE, 0);
    }

    ಹಿಂತಿರುಗಿ p;
}

    char_u *
compile_else(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    scope_T	*scope = cctx->ctx_scope;

    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_else_without_if));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (scope->se_u.se_if.is_ನೋಡಿದ_else)
    {
	emsg(_(e_ಅಪವರ್ತ್ಯ_else));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಆದರೆ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	scope->se_u.se_if.is_had_return = FALSE;
    scope->se_u.se_if.is_ನೋಡಿದ_else = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
    {
	ಆದರೆ (cctx->ctx_skip == SKIP_NOT
		&& ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_ಆರಂಭ)
	    // the previous block was executed, do not count "else" for
	    // profiling
	    --instr->ga_len;
	ಆದರೆ (cctx->ctx_skip == SKIP_YES && !scope->se_u.se_if.is_ನೋಡಿದ_skip_not)
	{
	    // the previous block was not executed, this one will, do count the
	    // "else" for profiling
	    cctx->ctx_skip = SKIP_NOT;
	    generate_instr(cctx, ISN_PROF_ಕೊನೆ);
	    generate_instr(cctx, ISN_PROF_ಆರಂಭ);
	    cctx->ctx_skip = SKIP_YES;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!scope->se_u.se_if.is_ನೋಡಿದ_skip_not && scope->se_skip_save != SKIP_YES)
    {
	// jump from previous block to the end, unless the else block is empty
	ಆದರೆ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ಆದರೆ (!cctx->ctx_had_return
		    && compile_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_if.is_ಕೊನೆ_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (cctx->ctx_skip == SKIP_UNKNOWN)
	{
	    ಆದರೆ (scope->se_u.se_if.is_if_label >= 0)
	    {
		// previous "if" or "elseif" jumps here
		isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_if.is_if_label;
		isn->isn_arg.jump.jump_where = instr->ga_len;
		scope->se_u.se_if.is_if_label = -1;
	    }
	}

	ಆದರೆ (cctx->ctx_skip != SKIP_UNKNOWN)
	    cctx->ctx_skip = cctx->ctx_skip == SKIP_YES ? SKIP_NOT : SKIP_YES;
    }

    ಹಿಂತಿರುಗಿ p;
}

    char_u *
compile_endif(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    ifscope_T	*ifscope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != IF_SCOPE)
    {
	emsg(_(e_endif_without_if));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ifscope = &scope->se_u.se_if;
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
    ಆದರೆ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	// the previous if block didn't end in a "return" or a "throw"
	// statement.
	ifscope->is_had_return = FALSE;

    ಆದರೆ (scope->se_u.se_if.is_if_label >= 0)
    {
	// previous "if" or "elseif" jumps here
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_if.is_if_label;
	isn->isn_arg.jump.jump_where = instr->ga_len;
    }
    // Fill in the "end" label in jumps at the end of the blocks.
    compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(&ifscope->is_ಕೊನೆ_label, instr->ga_len, cctx);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    // even when skipping we count the endif as executed, unless the block it's
    // in is skipped
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_skip == SKIP_YES
					    && scope->se_skip_save != SKIP_YES)
    {
	cctx->ctx_skip = SKIP_NOT;
	generate_instr(cctx, ISN_PROF_ಆರಂಭ);
    }
#ಆದರೆ_ಅಂತ್ಯ
    cctx->ctx_skip = scope->se_skip_save;

    // If all the blocks end in :return and there is an :else then the
    // had_return flag is set.
    cctx->ctx_had_return = ifscope->is_had_return && ifscope->is_ನೋಡಿದ_else;

    drop_scope(cctx);
    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Save the info needed for ENDLOOP.  Used by :for and :while.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_fill_loop_info(loop_info_T *loop_info, ಪೂರ್ಣಾಂಕ funcref_idx, cctx_T *cctx)
{
    loop_info->li_funcref_idx = funcref_idx;
    loop_info->li_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len;
    loop_info->li_ಆವರಣ_ಎಣಿಕೆ = cctx->ctx_ಆವರಣ_ಎಣಿಕೆ;
}

/*
 * When compiling a for loop to iterate over a tuple, get the type of the loop
 * variable to use.
 */
    ಸ್ಥಿರ type_T *
compile_for_tuple_get_vartype(type_T *vartype, ಪೂರ್ಣಾಂಕ var_list)
{
    // If this is not a variadic tuple, or all the tuple items don't have
    // the same type, then use t_any
    ಆದರೆ (!(vartype->tt_flags & TTFLAG_VARARGS) || vartype->tt_argcount != 1)
	ಹಿಂತಿರುಗಿ &t_any;

    // variadic tuple
    type_T *member_type = vartype->tt_ವಾದಗಳು[0]->tt_member;
    ಆದರೆ (member_type->tt_type == VAR_ANY)
	ಹಿಂತಿರುಗಿ &t_any;

    ಆದರೆ (!var_list)
	// for x in tuple<...list<xxx>>
	ಹಿಂತಿರುಗಿ member_type;

    ಆದರೆ (member_type->tt_type == VAR_LIST
	    && member_type->tt_member->tt_type != VAR_ANY)
	// for [x, y] in tuple<...list<list<xxx>>>
	ಹಿಂತಿರುಗಿ member_type->tt_member;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (member_type->tt_type == VAR_TUPLE
				&& member_type->tt_flags & TTFLAG_VARARGS
				&& member_type->tt_argcount == 1)
	// for [x, y] in tuple<...list<tuple<...list<xxx>>>>
	ಹಿಂತಿರುಗಿ member_type->tt_ವಾದಗಳು[0]->tt_member;

    ಹಿಂತಿರುಗಿ &t_any;
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
compile_for(char_u *arg_ಆರಂಭ, cctx_T *cctx)
{
    char_u	*arg;
    char_u	*arg_ಕೊನೆ;
    char_u	*name = ಖಾಲಿ;
    char_u	*p;
    char_u	*wp;
    ಪೂರ್ಣಾಂಕ		var_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		var_list = FALSE;
    ಪೂರ್ಣಾಂಕ		semicolon = FALSE;
    size_t	varlen;
    garray_T	*instr = &cctx->ctx_instr;
    scope_T	*scope;
    forscope_T	*forscope;
    lvar_T	*loop_lvar;	// loop iteration variable
    ಪೂರ್ಣಾಂಕ		loop_lvar_idx;
    lvar_T	*funcref_lvar;
    ಪೂರ್ಣಾಂಕ		funcref_lvar_idx;
    lvar_T	*var_lvar;	// variable for "var"
    type_T	*vartype;
    type_T	*ಅಂಶ_type = &t_any;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		prev_lnum = cctx->ctx_prev_lnum;

    p = skip_var_list(arg_ಆರಂಭ, TRUE, &var_ಎಣಿಕೆ, &semicolon, FALSE);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (var_ಎಣಿಕೆ == 0)
	var_ಎಣಿಕೆ = 1;
    ಇಲ್ಲದಿದ್ದರೆ
	var_list = TRUE;  // can also be a list of one variable

    // consume "in"
    wp = p;
    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(wp, &p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (STRNCMP(p, "in", 2) != 0 || !IS_WHITE_OR_NUL(p[2]))
    {
	ಆದರೆ (*p == ':' && wp != p)
	    semsg(_(e_no_white_space_allowed_before_colon_str), p);
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_missing_ಒಳ_after_for));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    wp = p + 2;
    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(wp, &p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Find the already generated ISN_DEBUG to get the line number for the
    // instruction written below the ISN_FOR instruction.
    ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG && instr->ga_len > 0
	    && ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
							.isn_type == ISN_DEBUG)
    {
	prev_lnum = ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
						 .isn_arg.debug.dbg_break_lnum;
    }

    scope = new_scope(cctx, FOR_SCOPE);
    ಆದರೆ (scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (scope->se_loop_ಆಳ == MAX_LOOP_ಆಳ)
    {
	emsg(_(e_loop_nesting_too_deep));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++scope->se_loop_ಆಳ;
    forscope = &scope->se_u.se_for;

    // Reserve a variable to store the loop iteration counter and initialize it
    // to -1.
    loop_lvar = reserve_local(cctx, (char_u *)"", 0, ನಿಯೋಜಿಸು_VAR, &t_ಸಂಖ್ಯೆ);
    ಆದರೆ (loop_lvar == ಖಾಲಿ)
    {
	drop_scope(cctx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    loop_lvar_idx = loop_lvar->lv_idx;
    generate_STORENR(cctx, loop_lvar_idx, -1);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // The variable index is always the loop var index plus one.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ನಿಯೋಜಿಸು_VAR, &t_ಸಂಖ್ಯೆ);
    ಆದರೆ (funcref_lvar == ಖಾಲಿ)
    {
	drop_scope(cctx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // compile "expr", it remains on the stack until "endfor"
    arg = p;
    ಆದರೆ (compile_expr0(&arg, cctx) == FAIL)
    {
	drop_scope(cctx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    arg_ಕೊನೆ = arg;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	// If we know the type of "var" and it is not a supported type we can
	// give an error now.
	vartype = get_type_on_ರಾಶಿ(cctx, 0);
	ಆದರೆ (vartype->tt_type != VAR_LIST
		&& vartype->tt_type != VAR_TUPLE
		&& vartype->tt_type != VAR_STRING
		&& vartype->tt_type != VAR_BLOB
		&& vartype->tt_type != VAR_ANY
		&& vartype->tt_type != VAR_UNKNOWN)
	{
	    semsg(_(e_for_loop_on_str_not_supported),
					       vartype_name(vartype->tt_type));
	    drop_scope(cctx);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (vartype->tt_type == VAR_STRING)
	    ಅಂಶ_type = &t_string;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype->tt_type == VAR_BLOB)
	    ಅಂಶ_type = &t_ಸಂಖ್ಯೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype->tt_type == VAR_LIST
				     && vartype->tt_member->tt_type != VAR_ANY)
	{
	    ಆದರೆ (!var_list)
		ಅಂಶ_type = vartype->tt_member;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype->tt_member->tt_type == VAR_LIST
			  && vartype->tt_member->tt_member->tt_type != VAR_ANY)
		ಅಂಶ_type = vartype->tt_member->tt_member;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype->tt_type == VAR_TUPLE)
	    ಅಂಶ_type = compile_for_tuple_get_vartype(vartype, var_list);

	// CMDMOD_REV must come before the FOR instruction.
	generate_undo_cmdmods(cctx);

	// "for_end" is set when ":endfor" is found
	forscope->fs_ತುದಿ_label = current_instr_idx(cctx);

	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    ಪೂರ್ಣಾಂಕ		save_prev_lnum = cctx->ctx_prev_lnum;
	    isn_T	*isn;

	    // Add ISN_DEBUG here, before deciding to end the loop.  There will
	    // be another ISN_DEBUG before the next instruction.
	    // Use the prev_lnum from the ISN_DEBUG instruction removed above.
	    // Increment the variable count so that the loop variable can be
	    // inspected.
	    cctx->ctx_prev_lnum = prev_lnum;
	    isn = generate_instr_debug(cctx);
	    ++isn->isn_arg.debug.dbg_var_ಹೆಸರುಗಳು_len;
	    cctx->ctx_prev_lnum = save_prev_lnum;
	}

	generate_FOR(cctx, loop_lvar_idx);

	arg = arg_ಆರಂಭ;
	ಆದರೆ (var_list)
	{
	    generate_UNPACK(cctx, var_ಎಣಿಕೆ, semicolon);
	    arg = skipwhite(arg + 1);	// skip white after '['

	    // drop the list item
	    --cctx->ctx_type_ರಾಶಿ.ga_len;

	    // add type of the items
	    ಗಾಗಿ (idx = 0; idx < var_ಎಣಿಕೆ; ++idx)
	    {
		type_T *type = (semicolon && idx == 0) ? vartype : ಅಂಶ_type;

		ಆದರೆ (ತಳ್ಳು_type_ರಾಶಿ(cctx, type) == FAIL)
		{
		    drop_scope(cctx);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}

	ಗಾಗಿ (idx = 0; idx < var_ಎಣಿಕೆ; ++idx)
	{
	    ನಿಯೋಜಿಸು_dest_T	dest = dest_local;
	    ಪೂರ್ಣಾಂಕ			opt_flags = 0;
	    ಪೂರ್ಣಾಂಕ			vimvaridx = -1;
	    type_T		*type = &t_any;
	    type_T		*lhs_type = &t_any;
	    where_T		where = WHERE_INIT;

	    p = skip_var_one(arg, FALSE);
	    varlen = p - arg;
	    name = vim_strnsave(arg, varlen);
	    ಆದರೆ (name == ಖಾಲಿ)
		ಹೋಗು failed;
	    ಆದರೆ (*skipwhite(p) == ':')
	    {
		ಆದರೆ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ಹೋಗು failed;
		}
		p = skipwhite(p + 1);
		lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc,
								cctx, TRUE);
		ಆದರೆ (lhs_type == ಖಾಲಿ || !valid_declaration_type(lhs_type))
		    ಹೋಗು failed;
	    }

	    ಆದರೆ (get_var_dest(name, &dest, CMD_for, FALSE, &opt_flags,
					      &vimvaridx, &type, cctx) == FAIL)
		ಹೋಗು failed;
	    ಆದರೆ (dest != dest_local)
	    {
		ಆದರೆ (generate_store_var(cctx, dest, opt_flags, vimvaridx,
						     type, name, ಖಾಲಿ) == FAIL)
		    ಹೋಗು failed;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varlen == 1 && *arg == '_')
	    {
		// Assigning to "_": drop the value.
		ಆದರೆ (generate_instr_drop(cctx, ISN_DROP, 1) == ಖಾಲಿ)
		    ಹೋಗು failed;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Script var is not supported.
		ಆದರೆ (STRNCMP(name, "s:", 2) == 0)
		{
		    emsg(_(e_cannot_use_script_variable_ಒಳ_for_loop));
		    ಹೋಗು failed;
		}

		ಆದರೆ (!valid_varname(arg, (ಪೂರ್ಣಾಂಕ)varlen, FALSE))
		    ಹೋಗು failed;
		ಆದರೆ (lookup_local(arg, varlen, ಖಾಲಿ, cctx) == OK)
		{
		    semsg(_(e_variable_already_declared_str), arg);
		    ಹೋಗು failed;
		}

		// Reserve a variable to store "var".
		ಆದರೆ (var_list)
		{
		    where.wt_ಸೂಚ್ಯಂಕ = idx + 1;
		    where.wt_ಬಗೆ = WT_VARIABLE;
		}
		ಆದರೆ (lhs_type == &t_any)
		    lhs_type = ಅಂಶ_type;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ_type != &t_unknown
			&& need_type_where(ಅಂಶ_type, lhs_type, 0, -1,
					    where, cctx, FALSE, FALSE) == FAIL)
		    ಹೋಗು failed;
		var_lvar = reserve_local(cctx, arg, varlen, ನಿಯೋಜಿಸು_FINAL,
								     lhs_type);
		ಆದರೆ (var_lvar == ಖಾಲಿ)
		    // out of memory or used as an argument
		    ಹೋಗು failed;

		ಆದರೆ (semicolon && idx == var_ಎಣಿಕೆ - 1)
		    var_lvar->lv_type = vartype;
		generate_STORE(cctx, ISN_STORE, var_lvar->lv_idx, ಖಾಲಿ);
	    }

	    ಆದರೆ (*p == ',' || *p == ';')
		++p;
	    arg = skipwhite(p);
	    vim_free(name);
	}

	// remember the number of variables and closures, used for ENDLOOP
	compile_fill_loop_info(&forscope->fs_loop_info, funcref_lvar_idx, cctx);
	forscope->fs_loop_info.li_ಆಳ = scope->se_loop_ಆಳ - 1;
    }

    ಹಿಂತಿರುಗಿ arg_ಕೊನೆ;

failed:
    vim_free(name);
    drop_scope(cctx);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Used when ending a loop of :for and :while: Generate an ISN_ENDLOOP
 * instruction if any variable was declared that could be used by a new
 * closure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_loop_ಕೊನೆ(loop_info_T *loop_info, cctx_T *cctx)
{
    ಆದರೆ (cctx->ctx_locals.ga_len > loop_info->li_local_ಎಣಿಕೆ
	    && cctx->ctx_ಆವರಣ_ಎಣಿಕೆ > loop_info->li_ಆವರಣ_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ generate_ENDLOOP(cctx, loop_info);
    ಹಿಂತಿರುಗಿ OK;
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

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != FOR_SCOPE)
    {
	emsg(_(e_endfor_without_for));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    forscope = &scope->se_u.se_for;
    cctx->ctx_scope = scope->se_outer;
    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ಆದರೆ (compile_loop_ಕೊನೆ(&forscope->fs_loop_info, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, forscope->fs_ತುದಿ_label);

	// Fill in the "end" label in the FOR statement so it can jump here.
	// In debug mode an ISN_DEBUG was inserted.
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + forscope->fs_ತುದಿ_label
				+ (cctx->ctx_compile_type == CT_DEBUG ? 1 : 0);
	isn->isn_arg.forloop.for_ಕೊನೆ = instr->ga_len;

	// Fill in the "end" label any BREAK statements
	compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(&forscope->fs_ಕೊನೆ_label, instr->ga_len, cctx);

	// Below the ":for" scope drop the "expr" list from the stack.
	ಆದರೆ (generate_instr_drop(cctx, ISN_DROP, 1) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    vim_free(scope);

    ಹಿಂತಿರುಗಿ arg;
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
    ಪೂರ್ಣಾಂಕ		    funcref_lvar_idx;

    scope = new_scope(cctx, WHILE_SCOPE);
    ಆದರೆ (scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (scope->se_loop_ಆಳ == MAX_LOOP_ಆಳ)
    {
	emsg(_(e_loop_nesting_too_deep));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++scope->se_loop_ಆಳ;
    whilescope = &scope->se_u.se_while;

    // "endwhile" jumps back here, one before when profiling or using cmdmods
    whilescope->ws_ತುದಿ_label = current_instr_idx(cctx);

    // Reserve a variable to store ec_funcrefs.ga_len, used in ISN_ENDLOOP.
    // It is not used when no closures are encountered, we don't know yet.
    funcref_lvar = reserve_local(cctx, (char_u *)"", 0, ನಿಯೋಜಿಸು_VAR, &t_ಸಂಖ್ಯೆ);
    ಆದರೆ (funcref_lvar == ಖಾಲಿ)
    {
	drop_scope(cctx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // out of memory
    }
    // get the index before a following reserve_local() makes the lval invalid
    funcref_lvar_idx = funcref_lvar->lv_idx;

    // remember the number of variables and closures, used for ENDLOOP
    compile_fill_loop_info(&whilescope->ws_loop_info, funcref_lvar_idx, cctx);
    whilescope->ws_loop_info.li_ಆಳ = scope->se_loop_ಆಳ - 1;

    // compile "expr"
    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!ತುದಿಗಳು_excmd2(arg, skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// CMDMOD_REV must come before the jump
	generate_undo_cmdmods(cctx);

	// "while_end" is set when ":endwhile" is found
	ಆದರೆ (compile_jump_ಗಮ್ಯ_ಕೊನೆ(&whilescope->ws_ಕೊನೆ_label,
			     JUMP_WHILE_FALSE, funcref_lvar_idx, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ p;
}

/*
 * compile "endwhile"
 */
    char_u *
compile_endwhile(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != WHILE_SCOPE)
    {
	emsg(_(e_endwhile_without_while));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    cctx->ctx_scope = scope->se_outer;
    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	whilescope_T	*whilescope = &scope->se_u.se_while;

	// Handle the case that any local variables were declared that might be
	// used in a closure.
	ಆದರೆ (compile_loop_ಕೊನೆ(&whilescope->ws_loop_info, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	// count the endwhile before jumping
	may_generate_prof_ಕೊನೆ(cctx, cctx->ctx_lnum);
#ಆದರೆ_ಅಂತ್ಯ

	// At end of ":for" scope jump back to the FOR instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, scope->se_u.se_while.ws_ತುದಿ_label);

	// Fill in the "end" label in the WHILE statement so it can jump here.
	// And in any jumps for ":break"
	compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_while.ws_ಕೊನೆ_label,
							  instr->ga_len, cctx);
    }

    vim_free(scope);

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Get the current information about variables declared inside a loop.
 * Returns TRUE if there are any and fills "lvi".
 */
    ಪೂರ್ಣಾಂಕ
get_loop_var_info(cctx_T *cctx, loopvarinfo_T *lvi)
{
    scope_T	*scope = cctx->ctx_scope;
    ಪೂರ್ಣಾಂಕ		prev_local_ಎಣಿಕೆ = 0;

    CLEAR_ಸೂಚಕ(lvi);
    ಗಾಗಿ (;;)
    {
	loop_info_T	*loopinfo;
	ಪೂರ್ಣಾಂಕ		cur_local_ಕೊನೆಯ;
	ಪೂರ್ಣಾಂಕ		ಆರಂಭ_local_ಎಣಿಕೆ;

	ಆಗ (scope != ಖಾಲಿ && scope->se_type != WHILE_SCOPE
						&& scope->se_type != FOR_SCOPE)
	    scope = scope->se_outer;
	ಆದರೆ (scope == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (scope->se_type == WHILE_SCOPE)
	{
	    loopinfo = &scope->se_u.se_while.ws_loop_info;
	    // :while reserves one variable for funcref count
	    cur_local_ಕೊನೆಯ = loopinfo->li_local_ಎಣಿಕೆ - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    loopinfo = &scope->se_u.se_for.fs_loop_info;
	    // :for reserves three variable: loop count, funcref count and loop
	    // var
	    cur_local_ಕೊನೆಯ = loopinfo->li_local_ಎಣಿಕೆ - 3;
	}

	ಆರಂಭ_local_ಎಣಿಕೆ = loopinfo->li_local_ಎಣಿಕೆ;
	ಆದರೆ (cctx->ctx_locals.ga_len > ಆರಂಭ_local_ಎಣಿಕೆ)
	{
	    lvi->lvi_loop[loopinfo->li_ಆಳ].var_idx =
						      (ಗಿಡ್ಡ)ಆರಂಭ_local_ಎಣಿಕೆ;
	    lvi->lvi_loop[loopinfo->li_ಆಳ].var_ಎಣಿಕೆ =
			  (ಗಿಡ್ಡ)(cctx->ctx_locals.ga_len - ಆರಂಭ_local_ಎಣಿಕೆ
							   - prev_local_ಎಣಿಕೆ);
	    ಆದರೆ (lvi->lvi_ಆಳ == 0)
		lvi->lvi_ಆಳ = loopinfo->li_ಆಳ + 1;
	}

	scope = scope->se_outer;
	prev_local_ಎಣಿಕೆ = cctx->ctx_locals.ga_len - cur_local_ಕೊನೆಯ;
    }
    ಹಿಂತಿರುಗಿ lvi->lvi_ಆಳ > 0;
}

/*
 * Get the index of the variable "idx" in a loop, if any.
 */
    ಶೂನ್ಯ
get_loop_var_idx(cctx_T *cctx, ಪೂರ್ಣಾಂಕ idx, lvar_T *lvar)
{
    loopvarinfo_T lvi;

    lvar->lv_loop_ಆಳ = -1;
    lvar->lv_loop_idx = -1;
    ಆದರೆ (get_loop_var_info(cctx, &lvi))
    {
	ಪೂರ್ಣಾಂಕ ಆಳ;

	ಗಾಗಿ (ಆಳ = lvi.lvi_ಆಳ - 1; ಆಳ >= 0; --ಆಳ)
	    ಆದರೆ (idx >= lvi.lvi_loop[ಆಳ].var_idx
		    && idx < lvi.lvi_loop[ಆಳ].var_idx
					       + lvi.lvi_loop[ಆಳ].var_ಎಣಿಕೆ)
	    {
		lvar->lv_loop_ಆಳ = ಆಳ;
		lvar->lv_loop_idx = lvi.lvi_loop[ಆಳ].var_idx;
		ಹಿಂತಿರುಗಿ;
	    }
    }
}

/*
 * Common for :break, :continue and :return
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ಕಂಡುಹಿಡಿ_scope(
	ಪೂರ್ಣಾಂಕ	    *loop_label,    // where to jump to or NULL
	endlabel_T  ***el,	    // end label or NULL
	ಪೂರ್ಣಾಂಕ	    *try_scopes,    // :try scopes encountered or NULL
	ಅಕ್ಷರ	    *error,	    // error to use when no scope found
	cctx_T	    *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (scope == ಖಾಲಿ)
	{
	    ಆದರೆ (error != ಖಾಲಿ)
		emsg(_(error));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (scope->se_type == FOR_SCOPE)
	{
	    ಆದರೆ (compile_loop_ಕೊನೆ(&scope->se_u.se_for.fs_loop_info, cctx)
								       == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (loop_label != ಖಾಲಿ)
		*loop_label = scope->se_u.se_for.fs_ತುದಿ_label;
	    ಆದರೆ (el != ಖಾಲಿ)
		*el = &scope->se_u.se_for.fs_ಕೊನೆ_label;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (scope->se_type == WHILE_SCOPE)
	{
	    ಆದರೆ (compile_loop_ಕೊನೆ(&scope->se_u.se_while.ws_loop_info, cctx)
								       == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (loop_label != ಖಾಲಿ)
		*loop_label = scope->se_u.se_while.ws_ತುದಿ_label;
	    ಆದರೆ (el != ಖಾಲಿ)
		*el = &scope->se_u.se_while.ws_ಕೊನೆ_label;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (try_scopes != ಖಾಲಿ && scope->se_type == TRY_SCOPE)
	    ++*try_scopes;
	scope = scope->se_outer;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * compile "continue"
 */
    char_u *
compile_continue(char_u *arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ		try_scopes = 0;
    ಪೂರ್ಣಾಂಕ		loop_label;

    ಆದರೆ (compile_ಕಂಡುಹಿಡಿ_scope(&loop_label, ಖಾಲಿ, &try_scopes,
				e_continue_without_while_or_for, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.
	generate_TRYCONT(cctx, try_scopes, loop_label);
    ಇಲ್ಲದಿದ್ದರೆ
	// Jump back to the FOR or WHILE instruction.
	generate_JUMP(cctx, JUMP_ALWAYS, loop_label);

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * compile "break"
 */
    char_u *
compile_break(char_u *arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ		try_scopes = 0;
    endlabel_T	**el;

    ಆದರೆ (compile_ಕಂಡುಹಿಡಿ_scope(ಖಾಲಿ, &el, &try_scopes,
				   e_break_without_while_or_for, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ arg;

    ಆದರೆ (try_scopes > 0)
	// Inside one or more try/catch blocks we first need to jump to the
	// "finally" or "endtry" to cleanup.  Then come to the next JUMP
	// instruction, which we don't know the index of yet.
	generate_TRYCONT(cctx, try_scopes, cctx->ctx_instr.ga_len + 1);

    // Jump to the end of the FOR or WHILE loop.  The instruction index will be
    // filled in later.
    ಆದರೆ (compile_jump_ಗಮ್ಯ_ಕೊನೆ(el, JUMP_ALWAYS, 0, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * compile "{" start of block
 */
    char_u *
compile_ಖಂಡ(char_u *arg, cctx_T *cctx)
{
    ಆದರೆ (new_scope(cctx, ಖಂಡ_SCOPE) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ skipwhite(arg + 1);
}

/*
 * compile end of block: drop one scope
 */
    ಶೂನ್ಯ
compile_endblock(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cctx->ctx_scope = scope->se_outer;
    unwind_locals(cctx, scope->se_local_ಎಣಿಕೆ, TRUE);
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

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // scope that holds the jumps that go to catch/finally/endtry
    try_scope = new_scope(cctx, TRY_SCOPE);
    ಆದರೆ (try_scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	// "try_catch" is set when the first ":catch" is found or when no catch
	// is found and ":finally" is found.
	// "try_finally" is set when ":finally" is found
	// "try_endtry" is set when ":endtry" is found
	try_scope->se_u.se_try.ts_try_label = instr->ga_len;
	ಆದರೆ ((isn = generate_instr(cctx, ISN_TRY)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	isn->isn_arg.tryref.try_ref = ALLOC_CLEAR_ONE(tryref_T);
	ಆದರೆ (isn->isn_arg.tryref.try_ref == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // scope for the try block itself
    scope = new_scope(cctx, ಖಂಡ_SCOPE);
    ಆದರೆ (scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ arg;
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

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // end block scope from :try or :catch
    ಆದರೆ (scope != ಖಾಲಿ && scope->se_type == ಖಂಡ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_catch_without_try));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (scope->se_u.se_try.ts_caught_all
				       && !ignore_unreachable_ಸಂಕೇತ_for_testing)
    {
	emsg(_(e_catch_unreachable_after_catch_all));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	// the profile-start should be after the jump
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE
		&& instr->ga_len > 0
		&& ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
						   .isn_type == ISN_PROF_ಆರಂಭ)
	    --instr->ga_len;
#ಆದರೆ_ಅಂತ್ಯ
	// Jump from end of previous block to :finally or :endtry
	ಆದರೆ (compile_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_try.ts_ಕೊನೆ_label,
						 JUMP_ALWAYS, 0, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// End :try or :catch scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_try.ts_try_label;
	ಆದರೆ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = instr->ga_len;
	ಆದರೆ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
	{
	    // a "throw" that jumps here needs to be counted
	    generate_instr(cctx, ISN_PROF_ಕೊನೆ);
	    // the "catch" is also counted
	    generate_instr(cctx, ISN_PROF_ಆರಂಭ);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG)
	    generate_instr_debug(cctx);
    }

    p = skipwhite(arg);
    ಆದರೆ (ತುದಿಗಳು_excmd2(arg, p))
    {
	scope->se_u.se_try.ts_caught_all = TRUE;
	scope->se_u.se_try.ts_catch_label = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *ಕೊನೆ;
	char_u *pat;
	char_u *tofree = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	dropped = 0;
	ಪೂರ್ಣಾಂಕ	len;

	// Push v:exception, push {expr} and MATCH
	generate_instr_type(cctx, ISN_PUSHEXC, &t_string);

	ಕೊನೆ = skip_regexp_ex(p + 1, *p, TRUE, &tofree, &dropped, ಖಾಲಿ);
	ಆದರೆ (*ಕೊನೆ != *p)
	{
	    semsg(_(e_separator_mismatch_str), p);
	    vim_free(tofree);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (tofree == ಖಾಲಿ)
	    len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - (p + 1));
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - tofree);
	pat = vim_strnsave(tofree == ಖಾಲಿ ? p + 1 : tofree, len);
	vim_free(tofree);
	p += len + 2 + dropped;
	ಆದರೆ (pat == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (generate_PUSHS(cctx, &pat) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (generate_COMPARE(cctx, ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	scope->se_u.se_try.ts_catch_label = instr->ga_len;
	ಆದರೆ (generate_JUMP(cctx, JUMP_IF_FALSE, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_CATCH) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (new_scope(cctx, ಖಂಡ_SCOPE) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ p;
}

    char_u *
compile_finally(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;
    ಪೂರ್ಣಾಂಕ		this_instr;

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // end block scope from :try or :catch
    ಆದರೆ (scope != ಖಾಲಿ && scope->se_type == ಖಂಡ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != TRY_SCOPE)
    {
	emsg(_(e_finally_without_try));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // If the preceding "try" or "catch" block didn't end in a "return" or a
    // "throw" statement, a pending return/throw from before "finally" cannot
    // be relied on to terminate the try statement.
    ಆದರೆ (!cctx->ctx_had_return && !cctx->ctx_had_throw)
	scope->se_u.se_try.ts_no_return = TRUE;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set value in ISN_TRY instruction
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_try.ts_try_label;
	ಆದರೆ (isn->isn_arg.tryref.try_ref->try_finally != 0)
	{
	    emsg(_(e_ಅಪವರ್ತ್ಯ_finally));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	this_instr = instr->ga_len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[this_instr - 1]
						   .isn_type == ISN_PROF_ಆರಂಭ)
	{
	    // jump to the profile start of the "finally"
	    --this_instr;

	    // jump to the profile end above it
	    ಆದರೆ (this_instr > 0 && ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[this_instr - 1]
						     .isn_type == ISN_PROF_ಕೊನೆ)
		--this_instr;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Fill in the "end" label in jumps at the end of the blocks.
	compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_try.ts_ಕೊನೆ_label,
							     this_instr, cctx);

	// If there is no :catch then an exception jumps to :finally.
	ಆದರೆ (isn->isn_arg.tryref.try_ref->try_catch == 0)
	    isn->isn_arg.tryref.try_ref->try_catch = this_instr;
	isn->isn_arg.tryref.try_ref->try_finally = this_instr;
	ಆದರೆ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Previous catch without match jumps here
	    isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_try.ts_catch_label;
	    isn->isn_arg.jump.jump_where = this_instr;
	    scope->se_u.se_try.ts_catch_label = 0;
	}
	scope->se_u.se_try.ts_has_finally = TRUE;
	ಆದರೆ (generate_instr(cctx, ISN_FINALLY) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ arg;
}

    char_u *
compile_endtry(char_u *arg, cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*try_isn;

    ಆದರೆ (misplaced_cmdmod(cctx))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // end block scope from :catch or :finally
    ಆದರೆ (scope != ಖಾಲಿ && scope->se_type == ಖಂಡ_SCOPE)
	compile_endblock(cctx);
    scope = cctx->ctx_scope;

    // Error if not in a :try scope
    ಆದರೆ (scope == ಖಾಲಿ || scope->se_type != TRY_SCOPE)
    {
	ಆದರೆ (scope == ಖಾಲಿ)
	    emsg(_(e_endtry_without_try));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scope->se_type == WHILE_SCOPE)
	    emsg(_(e_missing_endwhile));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scope->se_type == FOR_SCOPE)
	    emsg(_(e_missing_endfor));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_missing_endif));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    try_isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + scope->se_u.se_try.ts_try_label;
    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ (try_isn->isn_arg.tryref.try_ref->try_catch == 0
			  && try_isn->isn_arg.tryref.try_ref->try_finally == 0)
	{
	    emsg(_(e_missing_catch_or_finally));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE
		&& ((isn_T *)instr->ga_ದತ್ತಾಂಶ)[instr->ga_len - 1]
						.isn_type == ISN_PROF_ಆರಂಭ)
	    // move the profile start after "endtry" so that it's not counted when
	    // the exception is rethrown.
	    --instr->ga_len;
#ಆದರೆ_ಅಂತ್ಯ

	// Fill in the "end" label in jumps at the end of the blocks, if not
	// done by ":finally".
	compile_fill_jump_ಗಮ್ಯ_ಕೊನೆ(&scope->se_u.se_try.ts_ಕೊನೆ_label,
							  instr->ga_len, cctx);

	ಆದರೆ (scope->se_u.se_try.ts_catch_label != 0)
	{
	    // Last catch without match jumps here
	    isn_T *isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ)
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

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	// End :catch or :finally scope: set instruction index in ISN_TRY
	// instruction
	try_isn->isn_arg.tryref.try_ref->try_endtry = instr->ga_len;
	ಆದರೆ (generate_instr(cctx, ISN_ENDTRY) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
	    generate_instr(cctx, ISN_PROF_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ arg;
}

/*
 * compile "throw {expr}"
 */
    char_u *
compile_throw(char_u *arg, cctx_T *cctx)
{
    char_u *p = skipwhite(arg);

    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ p;
    ಆದರೆ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (generate_instr_drop(cctx, ISN_THROW, 1) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ p;
}

/*
 * Compile an expression or function call.
 */
    char_u *
compile_eval(char_u *arg, cctx_T *cctx)
{
    char_u	*p = arg;
    ಪೂರ್ಣಾಂಕ		name_only;
    ದೀರ್ಘ	lnum = SOURCING_LNUM;

    // find_ex_command() will consider a variable name an expression, assuming
    // that something follows on the next line.  Check that something actually
    // follows, otherwise it's probably a misplaced command.
    name_only = cmd_is_name_only(arg);

    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (name_only && lnum == SOURCING_LNUM)
    {
	semsg(_(e_ಅಭಿವ್ಯಕ್ತಿ_without_effect_str), arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // drop the result
    generate_instr_drop(cctx, ISN_DROP, 1);

    ಹಿಂತಿರುಗಿ skipwhite(p);
}

/*
 * Get the local variable index for deferred function calls.
 * Reserve it when not done already.
 * Returns zero for failure.
 */
    ಪೂರ್ಣಾಂಕ
get_defer_var_idx(cctx_T *cctx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					       + cctx->ctx_ufunc->uf_dfunc_idx;
    ಆದರೆ (dfunc->df_defer_var_idx == 0)
    {
	lvar_T *lvar = reserve_local(cctx, (char_u *)"@defer@", 7,
							    TRUE, &t_list_any);
	ಆದರೆ (lvar == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 0;
	dfunc->df_defer_var_idx = lvar->lv_idx + 1;
    }
    ಹಿಂತಿರುಗಿ dfunc->df_defer_var_idx;
}

/*
 * Compile "defer func(arg)".
 */
    char_u *
compile_defer(char_u *arg_ಆರಂಭ, cctx_T *cctx)
{
    char_u	*paren;
    char_u	*arg = arg_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		argcount = 0;
    ಪೂರ್ಣಾಂಕ		defer_var_idx;
    type_T	*type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		func_idx = -1;

    ಆದರೆ (*arg == '(')
    {
	// a lambda function
	ಆದರೆ (compile_lambda(&arg, cctx, ಖಾಲಿ) != OK)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	paren = vim_strchr(arg, '(');
	ಆದರೆ (paren == ಖಾಲಿ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Get a funcref for the function name.
	// TODO: better way to find the "(".
	paren = vim_strchr(arg, '(');
	ಆದರೆ (paren == ಖಾಲಿ)
	{
	    semsg(_(e_missing_parenthesis_str), arg);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	*paren = NUL;
	func_idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(arg);
	ಆದರೆ (func_idx >= 0)
	    // TODO: better type
	    generate_PUSHFUNC(cctx, (char_u *)ಆಂತರಿಕ_func_name(func_idx),
		    &t_func_any, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (compile_expr0(&arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*paren = '(';
    }

    // check for function type
    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	type = get_type_on_ರಾಶಿ(cctx, 0);
	ಆದರೆ (type->tt_type != VAR_FUNC)
	{
	    emsg(_(e_function_name_required));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    // compile the arguments
    arg = skipwhite(paren + 1);
    ಆದರೆ (compile_arguments(&arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ (func_idx >= 0)
	{
	    type2_T	*argtypes = ಖಾಲಿ;
	    type2_T	shuffled_argtypes[MAX_FUNC_ವಾದಗಳು];

	    ಆದರೆ (check_ಆಂತರಿಕ_func_ವಾದಗಳು(cctx, func_idx, argcount, FALSE,
			&argtypes, shuffled_argtypes) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_func_ವಾದಗಳು_from_type(cctx, type, argcount, TRUE,
		    arg_ಆರಂಭ) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	defer_var_idx = get_defer_var_idx(cctx);
	ಆದರೆ (defer_var_idx == 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (generate_DEFER(cctx, defer_var_idx - 1, argcount) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ skipwhite(arg);
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
compile_mult_ಅಭಿವ್ಯಕ್ತಿ(
	char_u	*arg,
	ಪೂರ್ಣಾಂಕ	cmdidx,
	ದೀರ್ಘ	cmd_ಎಣಿಕೆ ಬಳಸದ,
	cctx_T	*cctx)
{
    char_u	*p = arg;
    char_u	*prev = arg;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ctx_lnum = cctx->ctx_lnum;
    type_T	*type;
    ಪೂರ್ಣಾಂಕ		r = OK;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (ತುದಿಗಳು_excmd2(prev, p))
	    ನಿಲ್ಲಿಸು;
	ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = p;
	ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (cctx->ctx_skip != SKIP_YES)
	{
	    // check for non-void type
	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (type->tt_type == VAR_VOID)
	    {
		semsg(_(e_ಅಭಿವ್ಯಕ್ತಿ_does_not_ಫಲಿತಾಂಶ_ಒಳ_ಮೌಲ್ಯ_str), ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}

	++ಎಣಿಕೆ_2;
	prev = p;
	p = skipwhite(p);
    }

    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
    {
	ದೀರ್ಘ save_lnum = cctx->ctx_lnum;

	// Use the line number where the command started.
	cctx->ctx_lnum = ಆರಂಭ_ctx_lnum;

	ಆದರೆ (cmdidx == CMD_echo || cmdidx == CMD_echon)
	    r = generate_ECHO(cctx, cmdidx == CMD_echo, ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdidx == CMD_execute)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_EXECUTE, ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdidx == CMD_echomsg)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOMSG, ಎಣಿಕೆ_2);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdidx == CMD_echowindow)
	    r = generate_ECHOWINDOW(cctx, ಎಣಿಕೆ_2, cmd_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdidx == CMD_echoconsole)
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOCONSOLE, ಎಣಿಕೆ_2);
	ಇಲ್ಲದಿದ್ದರೆ
	    r = generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx, ISN_ECHOERR, ಎಣಿಕೆ_2);

	cctx->ctx_lnum = save_lnum;
    }
    ಹಿಂತಿರುಗಿ r == OK ? p : ಖಾಲಿ;
}

/*
 * If "eap" has a range that is not a constant generate an ISN_RANGE
 * instruction to compute it and return OK.
 * Otherwise return FAIL, the caller must deal with any range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_variable_range(exarg_T *eap, cctx_T *cctx)
{
    char_u *range_ಕೊನೆ = skip_range(eap->cmd, TRUE, ಖಾಲಿ);
    char_u *p = skipdigits(eap->cmd);

    ಆದರೆ (p == range_ಕೊನೆ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ generate_RANGE(cctx, vim_strnsave(eap->cmd, range_ಕೊನೆ - eap->cmd));
}

/*
 * :put r
 * :put ={expr}
 * or if fixindent == TRUE
 * :iput r
 * :iput ={expr}
 */
    char_u *
compile_put(char_u *arg, exarg_T *eap, cctx_T *cctx, ಪೂರ್ಣಾಂಕ fixindent)
{
    char_u	*ಸಾಲು = arg;
    linenr_T	lnum;
    ಅಕ್ಷರ	*errormsg;
    ಪೂರ್ಣಾಂಕ		above = eap->forceit;

    eap->regname = *ಸಾಲು;

    ಆದರೆ (eap->regname == '=')
    {
	char_u *p = skipwhite(ಸಾಲು + 1);

	ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಸಾಲು = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->regname != NUL)
	++ಸಾಲು;

    ಆದರೆ (compile_variable_range(eap, cctx) == OK)
    {
	lnum = above ? LNUM_VARIABLE_RANGE_ABOVE : LNUM_VARIABLE_RANGE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Either no range or a number.
	// "errormsg" will not be set because the range is ADDR_LINES.
	ಆದರೆ (parse_cmd_ವಿಳಾಸ(eap, &errormsg, FALSE) == FAIL)
	    // cannot happen
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
	    lnum = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    lnum = eap->line2;
	ಆದರೆ (above)
	    --lnum;
    }

    generate_PUT(cctx, eap->regname, lnum, fixindent);

    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * A command that is not compiled, execute with legacy code.
 */
    char_u *
compile_exec(char_u *ಸಾಲು_arg, exarg_T *eap, cctx_T *cctx)
{
    char_u	*ಸಾಲು = ಸಾಲು_arg;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		has_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
    char_u	*nextcmd = (char_u *)"";
    char_u	*tofree = ಖಾಲಿ;
    char_u	*cmd_arg = ಖಾಲಿ;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹೋಗು theend;

    // If there was a prececing command modifier, drop it and include it in the
    // EXEC command.
    ಆದರೆ (cctx->ctx_has_cmdmod)
    {
	garray_T	*instr = &cctx->ctx_instr;
	isn_T		*isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + instr->ga_len - 1;

	ಆದರೆ (isn->isn_type == ISN_CMDMOD)
	{
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
					       ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    --instr->ga_len;
	    cctx->ctx_has_cmdmod = FALSE;
	}
    }

    ಆದರೆ (eap->cmdidx >= 0 && eap->cmdidx < CMD_ಗಾತ್ರ)
    {
	ದೀರ್ಘ	argt = eap->argt;
	ಪೂರ್ಣಾಂಕ	usefilter = FALSE;

	has_ಅಭಿವ್ಯಕ್ತಿ = argt & (EX_XFILE | EX_EXPAND);

	// If the command can be followed by a bar, find the bar and truncate
	// it, so that the following command can be compiled.
	// The '|' is overwritten with a NUL, it is put back below.
	ಆದರೆ ((eap->cmdidx == CMD_write || eap->cmdidx == CMD_read)
							   && *eap->arg == '!')
	    // :w !filter or :r !filter or :r! filter
	    usefilter = TRUE;
	ಆದರೆ ((argt & EX_TRLBAR) && !usefilter)
	{
	    eap->argt = argt;
	    separate_nextcmd(eap, TRUE);
	    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
		nextcmd = eap->nextcmd;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_wincmd)
	{
	    p = eap->arg;
	    ಆದರೆ (*p != NUL)
		++p;
	    ಆದರೆ (*p == 'g' || *p == Ctrl_G)
		++p;
	    p = skipwhite(p);
	    ಆದರೆ (*p == '|')
	    {
		*p = NUL;
		nextcmd = p + 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_command || eap->cmdidx == CMD_autocmd)
	{
	    exarg_T ea;
	    ಪೂರ್ಣಾಂಕ	    flags = 0;  //ಬಳಸದ
	    ಪೂರ್ಣಾಂಕ	    ಆರಂಭ_lnum = SOURCING_LNUM;

	    CLEAR_ಕ್ಷೇತ್ರ(ea);
	    ea.arg = eap->arg;
	    fill_exarg_from_cctx(&ea, cctx);
	    p = may_get_cmd_ಖಂಡ(&ea, ಸಾಲು, &tofree, &flags);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		ಸಾಲು = p;
		SOURCING_LNUM = ಆರಂಭ_lnum;
	    }
	}
    }

    ಆದರೆ (eap->cmdidx == CMD_syntax && STRNCMP(eap->arg, "include ", 8) == 0)
    {
	// expand filename in "syntax include [@group] filename"
	has_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	eap->arg = skipwhite(eap->arg + 7);
	ಆದರೆ (*eap->arg == '@')
	    eap->arg = skiptowhite(eap->arg);
    }

    ಆದರೆ ((eap->cmdidx == CMD_global || eap->cmdidx == CMD_vglobal)
						       && STRLEN(eap->arg) > 4)
    {
	ಪೂರ್ಣಾಂಕ delim = *eap->arg;

	p = skip_regexp_ex(eap->arg + 1, delim, TRUE, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (*p == delim)
	    cmd_arg = p + 1;
    }

    ಆದರೆ (eap->cmdidx == CMD_folddoopen || eap->cmdidx == CMD_folddoclosed)
	cmd_arg = eap->arg;

    ಆದರೆ (cmd_arg != ಖಾಲಿ)
    {
	exarg_T nea;

	CLEAR_ಕ್ಷೇತ್ರ(nea);
	nea.cmd = cmd_arg;
	p = ಕಂಡುಹಿಡಿ_ex_command(&nea, ಖಾಲಿ, lookup_scriptitem, ಖಾಲಿ);
	ಆದರೆ (nea.cmdidx < CMD_ಗಾತ್ರ)
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ = excmd_get_argt(nea.cmdidx) & (EX_XFILE | EX_EXPAND);
	    ಆದರೆ (has_ಅಭಿವ್ಯಕ್ತಿ)
		eap->arg = skiptowhite(eap->arg);
	}
    }

    ಆದರೆ (has_ಅಭಿವ್ಯಕ್ತಿ && (p = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)eap->arg, "`=")) != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = 0;
	char_u	*ಆರಂಭ = skipwhite(ಸಾಲು);

	// :cmd xxx`=expr1`yyy`=expr2`zzz
	// PUSHS ":cmd xxx"
	// eval expr1
	// PUSHS "yyy"
	// eval expr2
	// PUSHS "zzz"
	// EXECCONCAT 5
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (p > ಆರಂಭ)
	    {
		char_u *val = vim_strnsave(ಆರಂಭ, p - ಆರಂಭ);

		generate_PUSHS(cctx, &val);
		++ಎಣಿಕೆ_2;
	    }
	    p += 2;
	    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    may_generate_2STRING(-1, TOSTRING_TOLERANT, cctx);
	    ++ಎಣಿಕೆ_2;
	    p = skipwhite(p);
	    ಆದರೆ (*p != '`')
	    {
		emsg(_(e_missing_backtick));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆರಂಭ = p + 1;

	    p = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)ಆರಂಭ, "`=");
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (*skipwhite(ಆರಂಭ) != NUL)
		{
		    char_u *val = vim_strsave(ಆರಂಭ);

		    generate_PUSHS(cctx, &val);
		    ++ಎಣಿಕೆ_2;
		}
		ನಿಲ್ಲಿಸು;
	    }
	}
	generate_EXECCONCAT(cctx, ಎಣಿಕೆ_2);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	generate_EXEC_copy(cctx, ISN_EXEC, ಸಾಲು);

theend:
    ಆದರೆ (*nextcmd != NUL)
    {
	// the parser expects a pointer to the bar, put it back
	--nextcmd;
	*nextcmd = '|';
    }
    vim_free(tofree);

    ಹಿಂತಿರುಗಿ nextcmd;
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
compile_script(char_u *ಸಾಲು, cctx_T *cctx)
{
    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	ಆದರೆ ((isn = generate_instr(cctx, ISN_EXEC_ವಿಭಜಿಸು)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	isn->isn_arg.string = vim_strsave(ಸಾಲು);
    }
    ಹಿಂತಿರುಗಿ (char_u *)"";
}


/*
 * :s/pat/repl/
 */
    char_u *
compile_substitute(char_u *arg, exarg_T *eap, cctx_T *cctx)
{
    char_u  *cmd = eap->arg;
    char_u  *ಅಭಿವ್ಯಕ್ತಿ = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)cmd, "\\=");

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ delimiter = *cmd++;

	// There is a \=expr, find it in the substitute part.
	cmd = skip_regexp_ex(cmd, delimiter, magic_isset(), ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (cmd[0] == delimiter && cmd[1] == '\\' && cmd[2] == '=')
	{
	    garray_T	save_ga = cctx->ctx_instr;
	    char_u	*ಕೊನೆ;
	    ಪೂರ್ಣಾಂಕ		ಅಭಿವ್ಯಕ್ತಿ_res;
	    ಪೂರ್ಣಾಂಕ		trailing_error;
	    ಪೂರ್ಣಾಂಕ		instr_ಎಣಿಕೆ;
	    isn_T	*instr;
	    isn_T	*isn;

	    cmd += 3;
	    ಕೊನೆ = skip_substitute(cmd, delimiter);

	    // Temporarily reset the list of instructions so that the jump
	    // labels are correct.
	    cctx->ctx_instr.ga_len = 0;
	    cctx->ctx_instr.ga_maxlen = 0;
	    cctx->ctx_instr.ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
	    ಅಭಿವ್ಯಕ್ತಿ_res = compile_expr0(&cmd, cctx);
	    ಆದರೆ (ಕೊನೆ[-1] == NUL)
		ಕೊನೆ[-1] = delimiter;
	    cmd = skipwhite(cmd);
	    trailing_error = *cmd != delimiter && *cmd != NUL;

	    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
	    {
		ಆದರೆ (trailing_error)
		    semsg(_(e_trailing_characters_str), cmd);
		clear_instr_ga(&cctx->ctx_instr);
		cctx->ctx_instr = save_ga;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }

	    // Move the generated instructions into the ISN_SUBSTITUTE
	    // instructions, then restore the list of instructions before
	    // adding the ISN_SUBSTITUTE instruction.
	    instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
	    instr = cctx->ctx_instr.ga_ದತ್ತಾಂಶ;
	    instr[instr_ಎಣಿಕೆ].isn_type = ISN_ಮುಕ್ತಾಯ;

	    cctx->ctx_instr = save_ga;
	    ಆದರೆ ((isn = generate_instr(cctx, ISN_SUBSTITUTE)) == ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ idx;

		ಗಾಗಿ (idx = 0; idx < instr_ಎಣಿಕೆ; ++idx)
		    delete_instr(instr + idx);
		vim_free(instr);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    isn->isn_arg.subs.subs_cmd = vim_strsave(arg);
	    isn->isn_arg.subs.subs_instr = instr;

	    // skip over flags
	    ಆದರೆ (*ಕೊನೆ == '&')
		++ಕೊನೆ;
	    ಆಗ (ASCII_ISALPHA(*ಕೊನೆ) || *ಕೊನೆ == '#')
		++ಕೊನೆ;
	    ಹಿಂತಿರುಗಿ ಕೊನೆ;
	}
    }

    ಹಿಂತಿರುಗಿ compile_exec(arg, eap, cctx);
}

    char_u *
compile_redir(char_u *ಸಾಲು, exarg_T *eap, cctx_T *cctx)
{
    char_u  *arg = eap->arg;
    lhs_T   *lhs = &cctx->ctx_redir_lhs;

    ಆದರೆ (lhs->lhs_name != ಖಾಲಿ)
    {
	ಆದರೆ (STRNCMP(arg, "END", 3) == 0)
	{
	    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
	    {
		ಆದರೆ (lhs->lhs_append)
		{
		    // First load the current variable value.
		    ಆದರೆ (compile_ತುಂಬು_lhs_with_ಸೂಚ್ಯಂಕ(lhs, lhs->lhs_ಸಂಪೂರ್ಣ,
								 cctx) == FAIL)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		// Gets the redirected text and put it on the stack, then store
		// it in the variable.
		generate_instr_type(cctx, ISN_REDIREND, &t_string);

		ಆದರೆ (lhs->lhs_append)
		    generate_CONCAT(cctx, 2);

		ಆದರೆ (lhs->lhs_has_ಸೂಚ್ಯಂಕ)
		{
		    // Use the info in "lhs" to store the value at the index in
		    // the list or dict.
		    ಆದರೆ (compile_ನಿಯೋಜಿಸು_unlet(lhs->lhs_ಸಂಪೂರ್ಣ, lhs, TRUE,
						      &t_string, cctx) == FAIL)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (generate_store_lhs(cctx, lhs, -1, FALSE) == FAIL)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

		VIM_CLEAR(lhs->lhs_name);
		VIM_CLEAR(lhs->lhs_ಸಂಪೂರ್ಣ);
	    }
	    ಹಿಂತಿರುಗಿ arg + 3;
	}
	emsg(_(e_cannot_nest_redir));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (arg[0] == '=' && arg[1] == '>')
    {
	ಪೂರ್ಣಾಂಕ	    append = FALSE;

	// redirect to a variable is compiled
	arg += 2;
	ಆದರೆ (*arg == '>')
	{
	    ++arg;
	    append = TRUE;
	}
	arg = skipwhite(arg);

	ಆದರೆ (compile_ನಿಯೋಜಿಸು_lhs(arg, lhs, CMD_redir,
					 FALSE, FALSE, FALSE, 1, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (need_type(&t_string, lhs->lhs_member_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	{
	    VIM_CLEAR(lhs->lhs_name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    generate_instr(cctx, ISN_REDIRSTART);
	    lhs->lhs_append = append;
	    ಆದರೆ (lhs->lhs_has_ಸೂಚ್ಯಂಕ)
	    {
		lhs->lhs_ಸಂಪೂರ್ಣ = vim_strnsave(arg, lhs->lhs_varlen_ಒಟ್ಟು);
		ಆದರೆ (lhs->lhs_ಸಂಪೂರ್ಣ == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}

	ಹಿಂತಿರುಗಿ arg + lhs->lhs_varlen_ಒಟ್ಟು;
    }

    // other redirects are handled like at script level
    ಹಿಂತಿರುಗಿ compile_exec(ಸಾಲು, eap, cctx);
}

#ಆದರೆ defined(FEAT_QUICKFIX)
    char_u *
compile_cexpr(char_u *ಸಾಲು, exarg_T *eap, cctx_T *cctx)
{
    isn_T	*isn;
    char_u	*p;

    isn = generate_instr(cctx, ISN_CEXPR_AUCMD);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    isn->isn_arg.ಸಂಖ್ಯೆ = eap->cmdidx;

    p = eap->arg;
    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    isn = generate_instr(cctx, ISN_CEXPR_CORE);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    isn->isn_arg.cexpr.cexpr_ref = ALLOC_ONE(cexprref_T);
    ಆದರೆ (isn->isn_arg.cexpr.cexpr_ref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdidx = eap->cmdidx;
    isn->isn_arg.cexpr.cexpr_ref->cer_forceit = eap->forceit;
    isn->isn_arg.cexpr.cexpr_ref->cer_cmdline = vim_strsave(skipwhite(ಸಾಲು));

    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Compile "return [expr]".
 * When "legacy" is TRUE evaluate [expr] with legacy syntax
 */
    char_u *
compile_return(char_u *arg, ಪೂರ್ಣಾಂಕ check_return_type, ಪೂರ್ಣಾಂಕ legacy, cctx_T *cctx)
{
    char_u	*p = arg;
    type_T	*ರಾಶಿ_type;

    ಆದರೆ (*p != NUL && *p != '|' && *p != '\n'
					&& (legacy || !vim9_comment_ಆರಂಭ(p)))
    {
	// For a lambda, "return expr" is always used, also when "expr" results
	// in a void.
	ಆದರೆ (cctx->ctx_ufunc->uf_ret_type->tt_type == VAR_VOID
		&& (cctx->ctx_ufunc->uf_flags & FC_LAMBDA) == 0)
	{
	    emsg(_(e_returning_ಮೌಲ್ಯ_ಒಳ_function_without_return_type));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (legacy)
	{
	    ಪೂರ್ಣಾಂಕ save_flags = cmdmod.cmod_flags;

	    generate_LEGACY_EVAL(cctx, p);
	    ಆದರೆ (need_type(&t_any, cctx->ctx_ufunc->uf_ret_type, 0, -1,
						0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    cmdmod.cmod_flags |= CMOD_LEGACY;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ಖಾಲಿ);
	    cmdmod.cmod_flags = save_flags;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // compile return argument into instructions
	    ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (cctx->ctx_skip != SKIP_YES)
	{
	    // "check_return_type" with uf_ret_type set to &t_unknown is used
	    // for an inline function without a specified return type.  Set the
	    // return type here.
	    ರಾಶಿ_type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (check_type_is_ಮೌಲ್ಯ(ರಾಶಿ_type) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಆದರೆ ((check_return_type && (cctx->ctx_ufunc->uf_ret_type == ಖಾಲಿ
				|| cctx->ctx_ufunc->uf_ret_type == &t_unknown))
		    || (!check_return_type
				&& cctx->ctx_ufunc->uf_ret_type == &t_unknown))
	    {
		cctx->ctx_ufunc->uf_ret_type = ರಾಶಿ_type;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (need_type(ರಾಶಿ_type, cctx->ctx_ufunc->uf_ret_type, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// "check_return_type" cannot be TRUE, only used for a lambda which
	// always has an argument.
	ಆದರೆ (cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_VOID
		&& cctx->ctx_ufunc->uf_ret_type->tt_type != VAR_UNKNOWN)
	{
	    emsg(_(e_missing_return_ಮೌಲ್ಯ));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ (IS_CONSTRUCTOR_ವಿಧಾನ(cctx->ctx_ufunc))
	{
	    // For a class new() constructor, return an object of the class.
	    generate_instr(cctx, ISN_RETURN_OBJECT);
	    cctx->ctx_ufunc->uf_ret_type =
		&cctx->ctx_ufunc->uf_class->class_object_type;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // No argument, return zero.
	    generate_PUSHNR(cctx, 0);
    }

    // may need ENDLOOP when inside a :for or :while loop
    ಆದರೆ (compile_ಕಂಡುಹಿಡಿ_scope(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) == FAIL)

    // Undo any command modifiers.
    generate_undo_cmdmods(cctx);

    ಆದರೆ (cctx->ctx_skip != SKIP_YES && generate_instr(cctx, ISN_RETURN) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // "return val | endif" is possible
    ಹಿಂತಿರುಗಿ skipwhite(p);
}

/*
 * Check if the separator for a :global or :substitute command is OK.
 */
    ಪೂರ್ಣಾಂಕ
check_global_and_subst(char_u *cmd, char_u *arg)
{
    ಆದರೆ (arg == cmd + 1 && vim_strchr((char_u *)":-.", *arg) != ಖಾಲಿ)
    {
	semsg(_(e_separator_not_supported_str), arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (VIM_ISWHITE(cmd[1]))
    {
	semsg(_(e_no_white_space_allowed_before_separator_str), cmd);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}


#ಆದರೆ_ಅಂತ್ಯ  // defined(FEAT_EVAL)
