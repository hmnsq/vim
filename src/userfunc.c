/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * userfunc.c: User defined function support
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ਸਥਿਰ hashtab_T	func_hashtab;

// Used by get_func_tv()
ਸਥਿਰ garray_T funcargs = GA_ਖਾਲੀ_ਥਾਵਾਂ;

// pointer to funccal for currently active function
ਸਥਿਰ funccall_T *current_funccal = ਨਿਰਾ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ਸਥਿਰ funccall_T *ਪਿਛਲਾ_funccal = ਨਿਰਾ;

ਸਥਿਰ ਖਾਲੀ funccal_unref(funccall_T *fc, ufunc_T *fp, ਪੂਰਨ force);
ਸਥਿਰ ਖਾਲੀ func_clear(ufunc_T *fp, ਪੂਰਨ force);
ਸਥਿਰ ਪੂਰਨ func_free(ufunc_T *fp, ਪੂਰਨ force);
ਸਥਿਰ char_u *untrans_function_name(char_u *name);
ਸਥਿਰ ਖਾਲੀ handle_defer_one(funccall_T *funccal);
ਸਥਿਰ char_u *trans_function_name_ext(char_u **pp, ਪੂਰਨ *is_global, ਪੂਰਨ skip, ਪੂਰਨ flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    ਖਾਲੀ
func_init(ਖਾਲੀ)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(ਖਾਲੀ)
{
    ਵਾਪਸ &func_hashtab;
}

/*
 * Get one function argument.
 * If "argtypes" is not NULL also get the type: "arg: type" (:def function).
 * If "types_optional" is TRUE a missing type is OK, use "any".
 * If "evalarg" is not NULL use it to check for an already declared name.
 * If "eap" is not NULL use it to check for an already declared name.
 * Return a pointer to after the type.
 * When something is wrong return "arg".
 */
    ਸਥਿਰ char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ਪੂਰਨ	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	ਪੂਰਨ	    is_vararg,
	ਪੂਰਨ	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ਨਿਰਾ;
    ਪੂਰਨ		is_underscore = FALSE;

    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ਜੇ (arg == p || ਸੁਰੱਖਿਅਤ_isdigit(*arg)
	    || (argtypes == ਨਿਰਾ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ਜੇ (!skip)
	    semsg(_(e_illegal_ਦਲੀਲ_str), arg);
	ਵਾਪਸ arg;
    }

    // Extra checks in Vim9 script.
    ਜੇ (!skip && argtypes != ਨਿਰਾ)
    {
	ਪੂਰਨ c = *p;
	*p = NUL;
	ਪੂਰਨ r = check_reserved_name(arg, FALSE);
	*p = c;
	ਜੇ (r == FAIL)
	    ਵਾਪਸ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ਜੇ (check_defined(arg, p - arg,
				   evalarg == ਨਿਰਾ ? ਨਿਰਾ : evalarg->eval_cctx,
			       eap == ਨਿਰਾ ? ਨਿਰਾ : eap->cstack, TRUE) == FAIL)
	    ਵਾਪਸ arg;
    }

    ਜੇ (newargs != ਨਿਰਾ && ga_grow(newargs, 1) == FAIL)
	ਵਾਪਸ arg;
    ਜੇ (newargs != ਨਿਰਾ)
    {
	ਪੂਰਨ	c;
	ਪੂਰਨ	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ਜੇ (arg_copy == ਨਿਰਾ)
	{
	    *p = c;
	    ਵਾਪਸ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ਜੇ (argtypes == ਨਿਰਾ || !is_underscore)
	    // Check for duplicate argument name.
	    ਲਈ (i = 0; i < newargs->ga_len; ++i)
		ਜੇ (STRCMP(((char_u **)(newargs->ga_ਅੰਕੜੇ))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_ਦਲੀਲ_name_str), arg_copy);
		    vim_free(arg_copy);
		    ਵਾਪਸ arg;
		}
	((char_u **)(newargs->ga_ਅੰਕੜੇ))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ਜੇ (argtypes != ਨਿਰਾ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ਨਿਰਾ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ਨਿਰਾ;

	ਜੇ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ਨਿਰਾ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ਜੇ (*p == ':')
	{
	    ++p;
	    ਜੇ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ਵਾਪਸ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ਜੇ (!skip)
		type = vim_strnsave(type, p - type);
	}
	ਨਹੀਂਤਾਂ ਜੇ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_ਦਲੀਲ_type_for_str),
					    arg_copy == ਨਿਰਾ ? arg : arg_copy);
	    ਵਾਪਸ arg;
	}
	ਜੇ (!skip)
	{
	    ਜੇ (type == ਨਿਰਾ && types_optional)
	    {
		// lambda arguments default to "any" type
		ਜੇ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ਨਹੀਂਤਾਂ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_ਅੰਕੜੇ)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_ਅੰਕੜੇ)[arg_objm->ga_len++] = FALSE;
	}
    }

    ਵਾਪਸ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ਸਥਿਰ char_u *
get_function_ਸਤਰ(
	exarg_T		*eap,
	garray_T	*ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free,
	ਪੂਰਨ		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ਜੇ (eap->ea_getline == ਨਿਰਾ)
	theline = getcmdline(':', 0L, indent, 0);
    ਨਹੀਂਤਾਂ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ਜੇ (theline != ਨਿਰਾ)
    {
	ਜੇ (ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len > 0
		&& eap->cmdlinep != ਨਿਰਾ
		&& *eap->cmdlinep == ((char_u **)ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_ਅੰਕੜੇ)
						   [ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(ਖਾਲੀ)ga_ਜੋੜੋ_string(ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, theline);
    }

    ਵਾਪਸ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ਸਥਿਰ ਪੂਰਨ
get_function_ਦਲੀਲਾਂ(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    ਪੂਰਨ		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    ਪੂਰਨ		*varargs,
    garray_T	*default_ਦਲੀਲਾਂ,
    ਪੂਰਨ		skip,
    exarg_T	*eap,		// can be NULL
    ਪੂਰਨ		ਵਿੱਚ_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free)
{
    ਪੂਰਨ		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		any_default = FALSE;
    char_u	*whitep = *argp;
    ਪੂਰਨ		need_ਵਿਅੰਜਕ = FALSE;

    ਜੇ (newargs != ਨਿਰਾ)
	ga_init2(newargs, ਆਕਾਰ(char_u *), 3);
    ਜੇ (argtypes != ਨਿਰਾ)
	ga_init2(argtypes, ਆਕਾਰ(char_u *), 3);
    ਜੇ (arg_objm != ਨਿਰਾ)
	ga_init2(arg_objm, ਆਕਾਰ(int8_T), 3);
    ਜੇ (!skip && default_ਦਲੀਲਾਂ != ਨਿਰਾ)
	ga_init2(default_ਦਲੀਲਾਂ, ਆਕਾਰ(char_u *), 3);

    ਜੇ (varargs != ਨਿਰਾ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ਜਦੋਂ (*p != endchar)
    {
	ਜਦੋਂ (eap != ਨਿਰਾ && eap->ea_getline != ਨਿਰਾ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_ਸਤਰ(eap, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, 0,
							  GETLINE_CONCAT_CONT);

	    ਜੇ (theline == ਨਿਰਾ)
		ਰੋਕੋ;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ਜੇ (mustend && *p != endchar)
	{
	    ਜੇ (!skip)
		semsg(_(e_invalid_ਦਲੀਲ_str), *argp);
	    ਜਾਓ err_ret;
	}
	ਜੇ (*p == endchar && !need_ਵਿਅੰਜਕ)
	    ਰੋਕੋ;

	ਜੇ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ਜੇ (varargs != ਨਿਰਾ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ਜੇ (argtypes != ਨਿਰਾ)
	    {
		// ...name: list<type>
		ਜੇ (!eval_isnamec1(*p))
		{
		    ਜੇ (!skip)
			emsg(_(e_missing_name_after_dots));
		    ਜਾਓ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ਜੇ (p == arg)
		    ਰੋਕੋ;
		ਜੇ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ਜਦੋਂ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ਜੇ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_ਵਿਧੀ_str), arg);
		*argend = c;
		ਰੋਕੋ;
	    }

	    ਜੇ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ਜੇ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_ਮੁੱਲ_must_be_vnone_str), p);
		    ਜਾਓ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ਜੇ (ga_grow(default_ਦਲੀਲਾਂ, 1) == FAIL)
		    ਜਾਓ err_ret;

		char_u *ਵਿਅੰਜਕ = vim_strnsave((char_u *)"v:none", 6);
		ਜੇ (ਵਿਅੰਜਕ == ਨਿਰਾ)
		    ਜਾਓ err_ret;
		((char_u **)(default_ਦਲੀਲਾਂ->ga_ਅੰਕੜੇ))
						 [default_ਦਲੀਲਾਂ->ga_len] = ਵਿਅੰਜਕ;
		default_ਦਲੀਲਾਂ->ga_len++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (any_default)
	    {
		emsg(_(e_non_default_ਦਲੀਲ_follows_default_ਦਲੀਲ));
		ਜਾਓ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ਜੇ (newargs != ਨਿਰਾ && ga_grow(newargs, 1) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (newargs != ਨਿਰਾ)
	    {
		((char_u **)(newargs->ga_ਅੰਕੜੇ))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ਜੇ (argtypes != ਨਿਰਾ && ga_grow(argtypes, 1) == OK
			    && arg_objm != ਨਿਰਾ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_ਅੰਕੜੇ)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_ਅੰਕੜੇ)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ਜੇ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			ਪੂਰਨ len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ਜੇ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ਜੇ (assignment != ਨਿਰਾ)
			{
			    c = *argend;
			    *argend = NUL;
			    ਜੇ (any_default)
				vim_snprintf((ਅੱਖਰ *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_ਦਲੀਲਾਂ->ga_len - 1, arg, arg);
			    ਨਹੀਂਤਾਂ
				vim_snprintf((ਅੱਖਰ *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_ਅੰਕੜੇ))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ਜੇ (*p == ',')
		++p;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ਜੇ (p == arg)
		ਰੋਕੋ;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ਜੇ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_ਦਲੀਲਾਂ != ਨਿਰਾ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *ਵਿਅੰਜਕ = p;
		ਜੇ (eval1(&p, &rettv, ਨਿਰਾ) != FAIL)
		{
		    ਜੇ (!skip)
		    {
			ਜੇ (ga_grow(default_ਦਲੀਲਾਂ, 1) == FAIL)
			    ਜਾਓ err_ret;

			ਜੇ (need_ਵਿਅੰਜਕ)
			    need_ਵਿਅੰਜਕ = FALSE;
			// trim trailing whitespace
			ਜਦੋਂ (p > ਵਿਅੰਜਕ && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			ਵਿਅੰਜਕ = vim_strsave(ਵਿਅੰਜਕ);
			ਜੇ (ਵਿਅੰਜਕ == ਨਿਰਾ)
			{
			    *p = c;
			    ਜਾਓ err_ret;
			}
			((char_u **)(default_ਦਲੀਲਾਂ->ga_ਅੰਕੜੇ))
						 [default_ਦਲੀਲਾਂ->ga_len] = ਵਿਅੰਜਕ;
			default_ਦਲੀਲਾਂ->ga_len++;
			*p = c;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    mustend = TRUE;
		    ਜੇ (*skipwhite(p) == NUL)
			need_ਵਿਅੰਜਕ = TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (any_default)
	    {
		emsg(_(e_non_default_ਦਲੀਲ_follows_default_ਦਲੀਲ));
		ਜਾਓ err_ret;
	    }

	    ਜੇ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ਜੇ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    ਜਾਓ err_ret;
		}
		p = skipwhite(p);
	    }
	    ਜੇ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ਜੇ (!skip && argtypes != ਨਿਰਾ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ਜਾਓ err_ret;
		}
	    }
	    ਨਹੀਂਤਾਂ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ਜੇ (*p != endchar)
	ਜਾਓ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ਵਾਪਸ OK;

err_ret:
    ਜੇ (newargs != ਨਿਰਾ)
	ga_clear_ਲੜੀਆਂ(newargs);
    ਜੇ (!skip && default_ਦਲੀਲਾਂ != ਨਿਰਾ)
	ga_clear_ਲੜੀਆਂ(default_ਦਲੀਲਾਂ);
    ਵਾਪਸ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_ਦਲੀਲ_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    ਪੂਰਨ		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    ਪੂਰਨ		obj_member_ਗਿਣਤੀ,
    cctx_T	*cctx)
{
    ਪੂਰਨ len = 0;

    ga_init2(&fp->uf_type_list, ਆਕਾਰ(type_T *), 10);
    ਜੇ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ਜੇ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ਜੇ (fp->uf_arg_types != ਨਿਰਾ)
	{
	    ਪੂਰਨ	i;
	    type_T	*type;

	    ਲਈ (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_ਅੰਕੜੇ)[i];

		ਜੇ (p == ਨਿਰਾ)
		    // will get the type from the default value
		    type = &t_unknown;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (arg_objm != ਨਿਰਾ && ((int8_T *)arg_objm->ga_ਅੰਕੜੇ)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)[i];

			type = &t_any;
			ਲਈ (ਪੂਰਨ om = 0; om < obj_member_ਗਿਣਤੀ; ++om)
			{
			    ਜੇ (obj_members != ਨਿਰਾ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ਰੋਕੋ;
			    }
			}
		    }
		    ਨਹੀਂਤਾਂ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ਜੇ (type == ਨਿਰਾ || !valid_declaration_type(type))
		    ਵਾਪਸ FAIL;
		fp->uf_arg_types[i] = type;
		ਜੇ (i < fp->uf_ਦਲੀਲਾਂ.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)[i];
		    ਜੇ (obj_members != ਨਿਰਾ && *name == '_')
			// protected object method
			name++;

		    ਜੇ (var_wrong_func_name(name, TRUE))
			ਵਾਪਸ FAIL;
		}
	    }
	}
    }

    ਜੇ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_ਦਲੀਲਾਂ.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)[fp->uf_ਦਲੀਲਾਂ.ga_len];
	((char_u **)fp->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)[fp->uf_ਦਲੀਲਾਂ.ga_len] = ਨਿਰਾ;
	p = ((char_u **)argtypes->ga_ਅੰਕੜੇ)[len];
	ਜੇ (p == ਨਿਰਾ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ਨਹੀਂਤਾਂ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ਜੇ (fp->uf_va_type != ਨਿਰਾ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_ਅੰਕੜੇ)[len]);
		ਵਾਪਸ FAIL;
	    }
	}
	ਜੇ (fp->uf_va_type == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ਜੇ (ret_type == ਨਿਰਾ)
	fp->uf_ret_type = &t_void;
    ਨਹੀਂਤਾਂ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ਜੇ (fp->uf_ret_type == ਨਿਰਾ)
	{
	    fp->uf_ret_type = &t_void;
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ਸਥਿਰ ਪੂਰਨ
ਰਜਿਸਟਰ_ਘੇਰਾ(ufunc_T *fp)
{
    ਜੇ (fp->uf_scoped == current_funccal)
	// no change
	ਵਾਪਸ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ਜੇ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_ਅੰਕੜੇ)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ਵਾਪਸ OK;
}

/*
 * If "name" starts with K_SPECIAL and "buf[bufsize]" is big enough
 * return "buf" filled with a readable function name.
 * Otherwise just return "name", thus the return value can always be used.
 * "name" and "buf" may be equal.
 */
    char_u *
make_ufunc_name_readable(char_u *name, char_u *buf, size_t bufsize)
{
    size_t len;

    ਜੇ (name[0] != K_SPECIAL)
	ਵਾਪਸ name;
    len = STRLEN(name);
    ਜੇ (len + 3 > bufsize)
	ਵਾਪਸ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ਵਾਪਸ buf;
}

ਸਥਿਰ char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	lambda_no = 0;
    ਪੂਰਨ		n;
    string_T	ret;

    n = vim_snprintf((ਅੱਖਰ *)lambda_name, ਆਕਾਰ(lambda_name), "<lambda>%d", ++lambda_no);
    ਜੇ (n < 1)
	ret.ਲੰਮਾਈ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (n >= (ਪੂਰਨ)ਆਕਾਰ(lambda_name))
	ret.ਲੰਮਾਈ = ਆਕਾਰ(lambda_name) - 1;
    ਨਹੀਂਤਾਂ
	ret.ਲੰਮਾਈ = (size_t)n;

    ret.string = lambda_name;
    ਵਾਪਸ ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ਸਥਿਰ ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < ਆਕਾਰ(ufunc_T) ? ਆਕਾਰ(ufunc_T) : len);
    ਜੇ (fp != ਨਿਰਾ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((ਖਾਲੀ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ਜੇ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ਜੇ (fp->uf_name_exp != ਨਿਰਾ)
		vim_snprintf((ਅੱਖਰ *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ਵਾਪਸ fp;
}

#ਜੇ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
ਰਜਿਸਟਰ_cfunc(cfunc_T cb, cfunc_free_T cb_free, ਖਾਲੀ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.ਲੰਮਾਈ);
    ਜੇ (fp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    fp->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_ਜੋੜੋ(&func_hashtab, UF2HIKEY(fp), "add C function");

    ਵਾਪਸ name.string;
}
#ਜੇ_ਅੰਤ

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ਸਥਿਰ char_u *
skip_arrow(
	char_u	*ਸ਼ੁਰੂ,
	ਪੂਰਨ	equal_arrow,
	char_u	**ret_type,
	ਪੂਰਨ	*white_error)
{
    char_u  *s = ਸ਼ੁਰੂ;
    char_u  *bef = ਸ਼ੁਰੂ - 2; // "start" points to > of ->

    ਜੇ (equal_arrow)
    {
	ਜੇ (*s == ':')
	{
	    ਜੇ (white_error != ਨਿਰਾ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ਜੇ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ਜੇ (*s != '=')
	    ਵਾਪਸ ਨਿਰਾ;
	++s;
    }
    ਜੇ (*s != '>')
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (white_error != ਨਿਰਾ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ਸਥਿਰ ਪੂਰਨ
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ਜੇ (checkforcmd(&p, "function", 2))
    {
	ਜੇ (*p == '(')
	    ਵਾਪਸ FALSE;
	*cmd = p;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ਸਥਿਰ ਖਾਲੀ
function_using_ਖੰਡ_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ਜੇ (cstack == ਨਿਰਾ || cstack->cs_idx < 0)
	ਵਾਪਸ;

    ਪੂਰਨ	    ਗਿਣਤੀ_2 = cstack->cs_idx + 1;
    ਪੂਰਨ	    i;

    fp->uf_ਖੰਡ_ids = ALLOC_MULT(ਪੂਰਨ, ਗਿਣਤੀ_2);
    ਜੇ (fp->uf_ਖੰਡ_ids != ਨਿਰਾ)
    {
	mch_memmove(fp->uf_ਖੰਡ_ids, cstack->cs_ਖੰਡ_id,
		ਆਕਾਰ(ਪੂਰਨ) * ਗਿਣਤੀ_2);
	fp->uf_ਖੰਡ_ਡੂੰਘਾਈ = ਗਿਣਤੀ_2;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    ਲਈ (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ਸਥਿਰ char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    ਜਦੋਂ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ਜੇ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    MB_PTR_ADV(p);
    }

    ਵਾਪਸ p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ਸਥਿਰ char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    ਜਦੋਂ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ਜੇ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    MB_PTR_ADV(p);
    }

    ਵਾਪਸ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_ਸ਼ੁਰੂ_ਦਾ_vim9_comment(char_u *ਸਤਰ)
{
    char_u	*p = ਸਤਰ;

    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p == '#')
		// Found the start of a Vim9 comment
		ਰੋਕੋ;
	    MB_PTR_ADV(p);
	}
    }

    ਵਾਪਸ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ਸਥਿਰ ਪੂਰਨ
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *ਸਤਰ_arg_ਵਿੱਚ,
	garray_T    *ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free)
{
    linenr_T	sourcing_lnum_ਸਿਖਰ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    ਪੂਰਨ		saved_wait_return = need_wait_return;
    char_u	*ਸਤਰ_arg = ਸਤਰ_arg_ਵਿੱਚ;
    ਪੂਰਨ		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_ਖੰਡ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FUNC_NESTING 50
    ਅੱਖਰ	nesting_def[MAX_FUNC_NESTING];
    ਅੱਖਰ	nesting_inline[MAX_FUNC_NESTING];
    ਪੂਰਨ		nesting = 0;
    getline_opt_T getline_options;
    ਪੂਰਨ		indent = 2;
    char_u	*skip_until = ਨਿਰਾ;
    ਪੂਰਨ		ret = FAIL;
    ਪੂਰਨ		is_heredoc = FALSE;
    ਪੂਰਨ		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ਨਿਰਾ;
    size_t	heredoc_trimmedlen = 0;
    ਪੂਰਨ		did_emsg_ਸ਼ੁਰੂ = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ਜੇ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ਜੇ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    ਜਾਓ theend;
	ਜਦੋਂ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ਅੰਕੜੇ))[newlines->ga_len++] = ਨਿਰਾ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_ਖੰਡ;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    ਲਈ (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ਜੇ (ਕੁੰਜੀ_Typed)
	{
	    ਸੁਨੇਹਾ_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ਜੇ (ਸਤਰ_arg != ਨਿਰਾ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = ਸਤਰ_arg;
	    p = vim_strchr(theline, '\n');
	    ਜੇ (p == ਨਿਰਾ)
		ਸਤਰ_arg += STRLEN(ਸਤਰ_arg);
	    ਨਹੀਂਤਾਂ
	    {
		*p = NUL;
		ਸਤਰ_arg = p + 1;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    theline = get_function_ਸਤਰ(eap, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, indent,
							      getline_options);
	}
	ਜੇ (ਕੁੰਜੀ_Typed)
	    ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;
	ਜੇ (theline == ਨਿਰਾ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ;
	    ਜੇ (skip_until != ਨਿਰਾ)
		semsg(_(e_missing_heredoc_ਸਿਰਾ_marker_str), skip_until);
	    ਨਹੀਂਤਾਂ ਜੇ (nesting_inline[nesting])
		emsg(_(e_missing_ਸਿਰਾ_ਖੰਡ));
	    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_missing_endfunction));
	    ਜਾਓ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ਜੇ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ਨਹੀਂਤਾਂ
	    sourcing_lnum_off = 0;

	ਜੇ (skip_until != ਨਿਰਾ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ਜੇ (heredoc_trimmed == ਨਿਰਾ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ਜੇ (heredoc_trimmed == ਨਿਰਾ)
		    p = theline;
		ਨਹੀਂਤਾਂ ਜੇ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ਨਹੀਂਤਾਂ
		    p = theline + heredoc_trimmedlen;
		ਜੇ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ਜੇ (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_ਅੰਕੜੇ))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_ਅੰਕੜੇ))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_ਅੰਕੜੇ;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ	    c;
	    char_u  *ਸਿਰਾ;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    ਲਈ (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ਜੇ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ਜੇ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ਜੇ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ਜੇ (nesting-- == 0)
		{
		    char_u *nextcmd = ਨਿਰਾ;

		    ਜੇ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_arg != ਨਿਰਾ && *skipwhite(ਸਤਰ_arg) != NUL)
			nextcmd = ਸਤਰ_arg;
		    ਨਹੀਂਤਾਂ ਜੇ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ
							+ newlines->ga_len + 1;
			ਜੇ (eap->cmdidx == CMD_def)
			    semsg(_(e_ਪਾਠ_ਮਿਲਿਆ_after_str_str), "enddef", p);
			ਨਹੀਂਤਾਂ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ਜੇ (nextcmd != ਨਿਰਾ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ਜੇ (ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_ਅੰਕੜੇ)
						   [ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_ਅੰਕੜੇ)
						   [ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len - 1];
			    --ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free->ga_len;
			}
		    }
		    ਰੋਕੋ;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ਨਹੀਂਤਾਂ ਜੇ (nesting_def[nesting])
	    {
		ਜੇ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ਜੇ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ਜੇ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ਜੇ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ਨਿਰਾ, TRUE, 0));
		ਜੇ (vim9_function && *p == '<')
		{
		    // skip generic function
		    ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&p) == FAIL)
			ਜਾਓ theend;
		}
		ਜੇ (*skipwhite(p) == '(')
		{
		    ਜੇ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ਨਹੀਂਤਾਂ
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ਜੇ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ਜੇ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    ਸਿਰਾ = ਲੱਭੋ_ਸ਼ੁਰੂ_ਦਾ_vim9_comment(p) - 1;
		}
		ਨਹੀਂਤਾਂ
		    ਸਿਰਾ = p + STRLEN(p) - 1;

		ਜਦੋਂ (ਸਿਰਾ > p && VIM_ISWHITE(*ਸਿਰਾ))
		    --ਸਿਰਾ;
		ਜੇ (ਸਿਰਾ > p + 1 && *ਸਿਰਾ == '{' && VIM_ISWHITE(ਸਿਰਾ[-1]))
		{
		    ਪੂਰਨ	    is_ਖੰਡ;

		    // check for trailing "=> {": start of an inline function
		    --ਸਿਰਾ;
		    ਜਦੋਂ (ਸਿਰਾ > p && VIM_ISWHITE(*ਸਿਰਾ))
			--ਸਿਰਾ;
		    is_ਖੰਡ = ਸਿਰਾ > p + 2 && ਸਿਰਾ[-1] == '=' && ਸਿਰਾ[0] == '>';
		    ਜੇ (!is_ਖੰਡ)
			is_ਖੰਡ = ਲੱਭੋ_cmd_ਖੰਡ_ਸ਼ੁਰੂ(p) != ਨਿਰਾ;

		    ਜੇ (is_ਖੰਡ)
		    {
			ਜੇ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ਨਹੀਂਤਾਂ
			{
			    ++nesting;
			    nesting_def[nesting] = TRUE;
			    nesting_inline[nesting] = TRUE;
			    indent += 2;
			}
		    }
		}
	    }

	    // Check for ":append", ":change", ":insert".  Not for :def.
	    // In Vim9 script a range comes after a colon; without one a
	    // leading "'" starts a string, not a mark.
	    ਜੇ (!vim9_function || colon)
		p = skip_range(p, FALSE, ਨਿਰਾ);
	    char_u *tp = p;
	    ਜੇ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ਨਹੀਂਤਾਂ
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ਜੇ (arg[0] == '<' && arg[1] =='<'
		    && ((p[0] == 'p' && p[1] == 'y'
				    && (!ASCII_ISALNUM(p[2]) || p[2] == 't'
					|| ((p[2] == '3' || p[2] == 'x')
						   && !ASCII_ISALPHA(p[3]))))
			|| (p[0] == 'p' && p[1] == 'e'
				    && (!ASCII_ISALPHA(p[2]) || p[2] == 'r'))
			|| (p[0] == 't' && p[1] == 'c'
				    && (!ASCII_ISALPHA(p[2]) || p[2] == 'l'))
			|| (p[0] == 'l' && p[1] == 'u' && p[2] == 'a'
				    && !ASCII_ISALPHA(p[3]))
			|| (p[0] == 'r' && p[1] == 'u' && p[2] == 'b'
				    && (!ASCII_ISALPHA(p[3]) || p[3] == 'y'))
			|| (p[0] == 'm' && p[1] == 'z'
				    && (!ASCII_ISALPHA(p[2]) || p[2] == 's'))
			))
	    {
		// ":python <<" continues until a dot, like ":append"
		p = skipwhite(arg + 2);
		ਜੇ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ਜੇ (heredoc_trimmed == ਨਿਰਾ)
			heredoc_trimmedlen = 0;
		}
		ਜੇ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ਨਹੀਂਤਾਂ
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ਜੇ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ਜੇ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ਜੇ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    ਪੂਰਨ		save_sc_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;
		    ਪੂਰਨ		var_ਗਿਣਤੀ = 0;
		    ਪੂਰਨ		semicolon = 0;

		    current_sctx.sc_ਸੰਸਕਰਣ
				     = vim9_function ? SCRIPT_ਸੰਸਕਰਣ_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_ਗਿਣਤੀ, &semicolon,
									 TRUE);
		    ਜੇ (arg != ਨਿਰਾ)
			arg = skipwhite(arg);
		    current_sctx.sc_ਸੰਸਕਰਣ = save_sc_ਸੰਸਕਰਣ;
		    ਜੇ (arg != ਨਿਰਾ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			ਪੂਰਨ has_trim = FALSE;

			p = skipwhite(arg + 3);
			ਜਦੋਂ (TRUE)
			{
			    ਜੇ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ਜਾਰੀ;
			    }
			    ਜੇ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ਜਾਰੀ;
			    }
			    ਰੋਕੋ;
			}
			ਜੇ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ਜੇ (heredoc_trimmed == ਨਿਰਾ)
				heredoc_trimmedlen = 0;
			}
			skip_until = vim_strnsave(p, skiptowhite(p) - p);
			getline_options = GETLINE_NONE;
			is_heredoc = TRUE;
		    }
		}
	    }
	}

	// Add the line to the function.
	ਜੇ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    ਜਾਓ theend;

	ਜੇ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ theend;
	((char_u **)(newlines->ga_ਅੰਕੜੇ))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ਜਦੋਂ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_ਅੰਕੜੇ))[newlines->ga_len++] = ਨਿਰਾ;

	// Check for end of eap->arg.
	ਜੇ (ਸਤਰ_arg != ਨਿਰਾ && *ਸਤਰ_arg == NUL)
	    ਸਤਰ_arg = ਨਿਰਾ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ਜੇ (did_emsg == did_emsg_ਸ਼ੁਰੂ)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_ਅੰਕੜੇ);
    need_wait_return |= saved_wait_return;
    ਵਾਪਸ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ਸਥਿਰ ਪੂਰਨ
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ਪੂਰਨ	    varargs,
	garray_T    *default_ਦਲੀਲਾਂ,
	char_u	    *ret_type)
{
    char_u	*ਸ਼ੁਰੂ = *arg;
    ਪੂਰਨ		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ਨਿਰਾ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ਨਿਰਾ;
    ਪੂਰਨ		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    ਪੂਰਨ		lnum_save = -1;
    linenr_T	sourcing_lnum_ਸਿਖਰ = SOURCING_LNUM;
    char_u	*ਸਤਰ_arg = ਨਿਰਾ;

    *arg = skipwhite(*arg + 1);
    ਜੇ (**arg == '|' || !ਸਿਰੇ_excmd2(ਸ਼ੁਰੂ, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ਵਾਪਸ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ਜੇ (**arg == '\n')
	ਸਤਰ_arg = *arg + 1;

    CLEAR_ਖੇਤਰ(eap);
    eap.cmdidx = CMD_ਖੰਡ;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ਜੇ (evalarg->eval_cctx != ਨਿਰਾ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ਨਹੀਂਤਾਂ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, ਆਕਾਰ(char_u *), 10);
    ਜੇ (get_function_body(&eap, &newlines, ਸਤਰ_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	ਜਾਓ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_ਗਿਣਤੀ += newlines.ga_len;
    ਜੇ (gap->ga_itemsize > 0)
    {
	ਪੂਰਨ	idx;
	char_u	*ਆਖਰੀ;
	size_t  plen;
	char_u  *pnl;

	ਲਈ (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_ਅੰਕੜੇ)[idx];
	    ਜੇ (p == ਨਿਰਾ)
		// comment line in the lambda body
		ਜਾਰੀ;

	    p = skipwhite(p);

	    ਜੇ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		ਜਾਓ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ਜੇ (*p == NUL || vim9_comment_ਸ਼ੁਰੂ(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ਨਹੀਂਤਾਂ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ਜੇ (pnl != ਨਿਰਾ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_ਅੰਕੜੇ)[freegap->ga_len++] = pnl;
	}
	ਜੇ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    ਜਾਓ erret;
	ਜੇ (eap.nextcmd != ਨਿਰਾ)
	{
	    // more is following after the "}", which was skipped
	    ਆਖਰੀ = cmdline;
	    plen = STRLEN(ਆਖਰੀ);
	}
	ਨਹੀਂਤਾਂ
	{
	    // nothing is following the "}"
	    ਆਖਰੀ = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ਜੇ (pnl != ਨਿਰਾ)
	    mch_memmove(pnl + 1, ਆਖਰੀ, plen + 1);
	((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_ਅੰਕੜੇ)[freegap->ga_len++] = pnl;
    }

    ਜੇ (eap.nextcmd != ਨਿਰਾ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ਜੇ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_ਅੰਕੜੇ))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
	*arg = (char_u *)"";

    ਜੇ (!evaluate)
    {
	ret = OK;
	ਜਾਓ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.ਲੰਮਾਈ);
    ਜੇ (ufunc == ਨਿਰਾ)
	ਜਾਓ erret;
    ਜੇ (hash_ਜੋੜੋ(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	ਜਾਓ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_ਦਲੀਲਾਂ = *newargs;
    newargs->ga_ਅੰਕੜੇ = ਨਿਰਾ;
    ufunc->uf_def_ਦਲੀਲਾਂ = *default_ਦਲੀਲਾਂ;
    default_ਦਲੀਲਾਂ->ga_ਅੰਕੜੇ = ਨਿਰਾ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ਜੇ (parse_ਦਲੀਲ_types(ufunc, argtypes, varargs, ਨਿਰਾ, ਨਿਰਾ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	ਜਾਓ erret;
    }

    // parse the return type, if any
    ਜੇ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	ਜਾਓ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ਜੇ (pt == ਨਿਰਾ)
	ਜਾਓ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_ਸਤਰਾਂ = newlines;
    newlines.ga_ਅੰਕੜੇ = ਨਿਰਾ;
    ਜੇ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ਜੇ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_ਸਿਖਰ;
    ਸੈੱਟ_function_type(ufunc);

    function_using_ਖੰਡ_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ਨਿਰਾ;
    ret = OK;

erret:
    ਜੇ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_ਲੜੀਆਂ(&newlines);
    ਜੇ (newargs != ਨਿਰਾ)
	ga_clear_ਲੜੀਆਂ(newargs);
    ga_clear_ਲੜੀਆਂ(default_ਦਲੀਲਾਂ);
    ਜੇ (ufunc != ਨਿਰਾ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ਵਾਪਸ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    ਪੂਰਨ
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	ਪੂਰਨ	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_ਦਲੀਲਾਂ;
    garray_T	arg_objm;
    ufunc_T	*fp = ਨਿਰਾ;
    partial_T   *pt = ਨਿਰਾ;
    ਪੂਰਨ		varargs;
    char_u	*ret_type = ਨਿਰਾ;
    ਪੂਰਨ		ret;
    char_u	*s;
    char_u	*ਸ਼ੁਰੂ, *ਸਿਰਾ;
    ਪੂਰਨ		*old_eval_lavars = eval_lavars_ਵਰਤਿਆ;
    ਪੂਰਨ		eval_lavars = FALSE;
    char_u	*tofree2 = ਨਿਰਾ;
    ਪੂਰਨ		equal_arrow = **arg == '(';
    ਪੂਰਨ		white_error = FALSE;
    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਲੰਮਾ	ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;

    ਜੇ (equal_arrow && !vim9script)
	ਵਾਪਸ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_ਦਲੀਲਾਂ(&s, equal_arrow ? ')' : '-', ਨਿਰਾ,
	    types_optional ? &argtypes : ਨਿਰਾ, types_optional,
			types_optional ? &arg_objm : ਨਿਰਾ, evalarg,
			ਨਿਰਾ, &default_ਦਲੀਲਾਂ, TRUE, ਨਿਰਾ, FALSE, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ਨਿਰਾ) == ਨਿਰਾ)
    {
	ਜੇ (types_optional)
	{
	    ga_clear_ਲੜੀਆਂ(&argtypes);
	    ga_clear(&arg_objm);
	}
	ਵਾਪਸ called_emsg == called_emsg_ਸ਼ੁਰੂ ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ਜੇ (evaluate)
	pnewargs = &newargs;
    ਨਹੀਂਤਾਂ
	pnewargs = ਨਿਰਾ;
    *arg += 1;
    ret = get_function_ਦਲੀਲਾਂ(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ਨਿਰਾ, types_optional,
			types_optional ? &arg_objm : ਨਿਰਾ, evalarg,
					    &varargs, &default_ਦਲੀਲਾਂ,
					    FALSE, ਨਿਰਾ, FALSE, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ਨਿਰਾ)) == ਨਿਰਾ)
    {
	ਜੇ (types_optional)
	{
	    ga_clear_ਲੜੀਆਂ(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_ਲੜੀਆਂ(&newargs);
	ਵਾਪਸ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ਜੇ (ret_type != ਨਿਰਾ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ਜੇ (evaluate)
	eval_lavars_ਵਰਤਿਆ = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ਜੇ (equal_arrow && **arg == '{')
    {
	ਜੇ (evalarg == ਨਿਰਾ)
	    // cannot happen?
	    ਜਾਓ theend;
	SOURCING_LNUM = ਸ਼ੁਰੂ_lnum;  // used for where lambda is defined
	ਜੇ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ਨਿਰਾ, varargs,
			   &default_ਦਲੀਲਾਂ, ret_type) == FAIL)
	    ਜਾਓ errret;
	ਜਾਓ theend;
    }
    ਜੇ (default_ਦਲੀਲਾਂ.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_ਮੁੱਲਾਂ_ਵਿੱਚ_lambda));
	ਜਾਓ errret;
    }

    // Get the start and the end of the expression.
    ਸ਼ੁਰੂ = *arg;
    ret = skip_ਵਿਅੰਜਕ_concatenate(arg, &ਸ਼ੁਰੂ, &ਸਿਰਾ, evalarg);
    ਜੇ (ret == FAIL)
	ਜਾਓ errret;

    ਜੇ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg != '}')
	{
	    semsg(_(e_expected_ਸੱਜਾ_curly_str), *arg);
	    ਜਾਓ errret;
	}
	++*arg;
    }

    ਜੇ (evaluate)
    {
	ਪੂਰਨ	    len;
	ਪੂਰਨ	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *ਸਤਰ_ਸਿਰਾ;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.ਲੰਮਾਈ);
	ਜੇ (fp == ਨਿਰਾ)
	    ਜਾਓ errret;
	fp->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ਜੇ (pt == ਨਿਰਾ)
	    ਜਾਓ errret;

	ga_init2(&newlines, ਆਕਾਰ(char_u *), 1);
	ਜੇ (ga_grow(&newlines, 1) == FAIL)
	    ਜਾਓ errret;

	// If there are line breaks, we need to split up the string.
	ਸਤਰ_ਸਿਰਾ = vim_strchr(ਸ਼ੁਰੂ, '\n');
	ਜੇ (ਸਤਰ_ਸਿਰਾ == ਨਿਰਾ || ਸਤਰ_ਸਿਰਾ > ਸਿਰਾ)
	    ਸਤਰ_ਸਿਰਾ = ਸਿਰਾ;

	// Add "return " before the expression (or the first line).
	len = 7 + (ਪੂਰਨ)(ਸਤਰ_ਸਿਰਾ - ਸ਼ੁਰੂ) + 1;
	p = alloc(len);
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ errret;
	((char_u **)(newlines.ga_ਅੰਕੜੇ))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, ਸ਼ੁਰੂ, ਸਤਰ_ਸਿਰਾ - ਸ਼ੁਰੂ);

	ਜੇ (ਸਤਰ_ਸਿਰਾ != ਸਿਰਾ)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ਜਦੋਂ (ਸਤਰ_ਸਿਰਾ < ਸਿਰਾ && *ਸਤਰ_ਸਿਰਾ == '\n')
	    {
		ਜੇ (ga_grow(&newlines, 1) == FAIL)
		    ਜਾਓ errret;
		ਸ਼ੁਰੂ = ਸਤਰ_ਸਿਰਾ + 1;
		ਸਤਰ_ਸਿਰਾ = vim_strchr(ਸ਼ੁਰੂ, '\n');
		ਜੇ (ਸਤਰ_ਸਿਰਾ == ਨਿਰਾ || ਸਤਰ_ਸਿਰਾ > ਸਿਰਾ)
		    ਸਤਰ_ਸਿਰਾ = ਸਿਰਾ;
		((char_u **)(newlines.ga_ਅੰਕੜੇ))[newlines.ga_len++] =
					 vim_strnsave(ਸ਼ੁਰੂ, ਸਤਰ_ਸਿਰਾ - ਸ਼ੁਰੂ);
	    }
	}

	ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)p + 7, "a:") == ਨਿਰਾ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_ਦਲੀਲਾਂ = newargs;
	ga_init(&fp->uf_def_ਦਲੀਲਾਂ);
	ਜੇ (types_optional)
	{
	    ਜੇ (parse_ਦਲੀਲ_types(fp, &argtypes,
				vim9script && varargs, ਨਿਰਾ, ਨਿਰਾ, 0,
				cctx) == FAIL)
		ਜਾਓ errret;
	    ਜੇ (ret_type != ਨਿਰਾ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ਨਿਰਾ, cctx, TRUE);
		ਜੇ (fp->uf_ret_type == ਨਿਰਾ)
		    ਜਾਓ errret;
	    }
	    ਨਹੀਂਤਾਂ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_ਸਤਰਾਂ = newlines;
	ਜੇ (current_funccal != ਨਿਰਾ && eval_lavars)
	{
	    flags |= FC_ਘੇਰਾ;
	    ਜੇ (ਰਜਿਸਟਰ_ਘੇਰਾ(fp) == FAIL)
		ਜਾਓ errret;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ਜੇ (prof_def_func())
	    func_do_profile(fp);
#ਜੇ_ਅੰਤ
	ਜੇ (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += ਸ਼ੁਰੂ_lnum;

	function_using_ਖੰਡ_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_ਜੋੜੋ(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_ਵਰਤਿਆ = old_eval_lavars;
    vim_free(tofree2);
    ਜੇ (types_optional)
    {
	ga_clear_ਲੜੀਆਂ(&argtypes);
	ga_clear(&arg_objm);
    }

    ਵਾਪਸ OK;

errret:
    ga_clear_ਲੜੀਆਂ(&newargs);
    ga_clear_ਲੜੀਆਂ(&newlines);
    ga_clear_ਲੜੀਆਂ(&default_ਦਲੀਲਾਂ);
    ਜੇ (types_optional)
    {
	ga_clear_ਲੜੀਆਂ(&argtypes);
	ga_clear(&arg_objm);
    }
    ਜੇ (fp != ਨਿਰਾ)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_ਵਰਤਿਆ = old_eval_lavars;
    ਵਾਪਸ FAIL;
}

/*
 * Check if "name" is a variable of type VAR_FUNC.  If so, return the function
 * name it contains, otherwise return "name".
 * If "partialp" is not NULL, and "name" is of type VAR_PARTIAL also set
 * "partialp".
 * If "type" is not NULL and a Vim9 script-local variable is found look up the
 * type of the variable.
 * If "new_function" is TRUE the name is for a new function.
 * If "found_var" is not NULL and a variable was found set it to TRUE.
 */
    char_u *
deref_func_name(
	char_u	    *name,
	ਪੂਰਨ	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	ਪੂਰਨ	    no_autoload,
	ਪੂਰਨ	    new_function,
	ਪੂਰਨ	    *ਮਿਲਿਆ_var)
{
    dictitem_T	*v;
    typval_T	*tv = ਨਿਰਾ;
    ਪੂਰਨ		cc;
    char_u	*s = ਨਿਰਾ;
    hashtab_T	*ht;
    ਪੂਰਨ		did_type = FALSE;

    ਜੇ (partialp != ਨਿਰਾ)
	*partialp = ਨਿਰਾ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = ਲੱਭੋ_var_also_ਵਿੱਚ_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ਜੇ (v != ਨਿਰਾ)
    {
	tv = &v->di_tv;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	ਪੂਰਨ	    len = *lenp;

	ਜੇ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = ਲੱਭੋ_imported(p, len, FALSE);

	// imported function from another script
	ਜੇ (import != ਨਿਰਾ)
	{
	    name[len] = NUL;
	    ਜੇ (new_function)
		semsg(_(e_redefining_imported_ਵਸਤੂ_str), name);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ਵਾਪਸ (char_u *)"";	// just in case
	}
    }

    ਜੇ (tv != ਨਿਰਾ)
    {
	ਜੇ (ਮਿਲਿਆ_var != ਨਿਰਾ)
	    *ਮਿਲਿਆ_var = TRUE;
	ਜੇ (tv->v_type == VAR_FUNC)
	{
	    ਜੇ (tv->vval.v_string == ਨਿਰਾ)
	    {
		*lenp = 0;
		ਵਾਪਸ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_ਲੱਭੋ_open_bracket(s);
	    ਜੇ (p != ਨਿਰਾ && generic_func_ਲੱਭੋ_open_bracket(name) != ਨਿਰਾ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ਵਾਪਸ (char_u *)"";
	    }
	    ਜੇ (p != ਨਿਰਾ)
		*lenp = p - s;
	    ਨਹੀਂਤਾਂ
		*lenp = (ਪੂਰਨ)STRLEN(s);
	}
	ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ਜੇ (pt == ਨਿਰਾ)
	    {
		*lenp = 0;
		ਵਾਪਸ (char_u *)"";	// just in case
	    }
	    ਜੇ (partialp != ਨਿਰਾ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (ਪੂਰਨ)STRLEN(s);
	}

	ਜੇ (s != ਨਿਰਾ)
	{
	    ਜੇ (!did_type && type != ਨਿਰਾ && ht == get_script_local_ht())
	    {
		svar_T  *sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(tv, 0, TRUE);

		ਜੇ (sv != ਨਿਰਾ)
		    *type = sv->sv_type;
	    }
	    ਵਾਪਸ s;
	}
    }

    ਵਾਪਸ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    ਖਾਲੀ
emsg_funcname(ਅੱਖਰ *ermsg, char_u *name)
{
    char_u	*p = name;

    ਜੇ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ਜੇ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    ਪੂਰਨ
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	ਪੂਰਨ	    partial_argc,
	typval_T    *argvars,
	ਪੂਰਨ	    *argcount,
	ਪੂਰਨ	    is_builtin)
{
    char_u	*argp = *arg;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ਜਦੋਂ (*argcount < MAX_FUNC_ਦਲੀਲਾਂ - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ਜੇ (*argp == ')' || *argp == ',' || *argp == NUL)
	    ਰੋਕੋ;

	ਪੂਰਨ arg_idx = *argcount;
	ਜੇ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ਰੋਕੋ;
	}
	++*argcount;
	ਜੇ (!is_builtin && check_typval_is_ਮੁੱਲ(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ਰੋਕੋ;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ਜੇ (vim9script)
	{
	    ਜੇ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ਜੇ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    argp = skipwhite(argp);
	ਜੇ (*argp != ',')
	    ਰੋਕੋ;
	ਜੇ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ਜੇ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ਰੋਕੋ;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ਜੇ (*argp == ')')
	++argp;
    ਨਹੀਂਤਾਂ
	ret = FAIL;
    *arg = argp;
    ਵਾਪਸ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
get_func_tv(
    char_u	*name,		// name of the function
    ਪੂਰਨ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    ਪੂਰਨ		ret;
    typval_T	argvars[MAX_FUNC_ਦਲੀਲਾਂ + 1];	// vars for arguments
    ਪੂਰਨ		argcount = 0;			// number of arguments found
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ਨਿਰਾ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ਜੇ (ret == OK)
    {
	ਪੂਰਨ	i = 0;
	ਪੂਰਨ	did_emsg_before = did_emsg;

	ਜੇ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ਜੇ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, ਆਕਾਰ(typval_T *), 50);
	    ਲਈ (i = 0; i < argcount; ++i)
		ਜੇ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_ਅੰਕੜੇ)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ਜੇ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!aborting() && evaluate)
    {
	ਜੇ (argcount == MAX_FUNC_ਦਲੀਲਾਂ)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ਨਹੀਂਤਾਂ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ਜਦੋਂ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ਜੇ (vim9script)
	*arg = argp;
    ਨਹੀਂਤਾਂ
	*arg = skipwhite(argp);
    ਵਾਪਸ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ਸਥਿਰ ਪੂਰਨ
eval_fname_sid(char_u *p)
{
    ਵਾਪਸ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
}

/*
 * In a script change <SID>name() and s:name() to K_SNR 123_name().
 * Change <SNR>123_name() to K_SNR 123_name().
 * Use "fname_buf[FLEN_FIXED + 1]" when it fits, otherwise allocate memory
 * and set "tofree".
 */
    char_u *
fname_trans_sid(
	char_u	    *name,
	char_u	    *fname_buf,
	char_u	    **tofree,
	funcerror_T *error)
{
    char_u	*script_name;
    char_u	*fname;
    size_t	fnamelen;
    size_t	fname_buflen;

    script_name = name + eval_fname_script(name);
    ਜੇ (script_name == name)
	ਵਾਪਸ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (ਪੂਰਨ)KE_SNR;
    fname_buflen = 3;
    ਜੇ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ਨਹੀਂਤਾਂ
	{
	    fname_buflen += vim_snprintf((ਅੱਖਰ *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(ਲੰਮਾ)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ਜੇ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ਨਹੀਂਤਾਂ
    {
	fname = alloc(fnamelen + 1);
	ਜੇ (fname == ਨਿਰਾ)
	    *error = FCERR_OTHER;
	ਨਹੀਂਤਾਂ
	{
	    *tofree = fname;
	    vim_snprintf((ਅੱਖਰ *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ਵਾਪਸ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    ਖਾਲੀ
func_name_with_sid(char_u *name, ਪੂਰਨ sid, char_u *ਬਫਰ)
{
    // A script-local function is stored as "<SNR>99_name".
    ਬਫਰ[0] = K_SPECIAL;
    ਬਫਰ[1] = KS_EXTRA;
    ਬਫਰ[2] = (ਪੂਰਨ)KE_SNR;
    vim_snprintf((ਅੱਖਰ *)ਬਫਰ + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (ਲੰਮਾ)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ਸਥਿਰ ufunc_T *
ਲੱਭੋ_func_with_sid(char_u *name, ਪੂਰਨ sid)
{
    hashitem_T	    *hi;
    char_u	    ਬਫਰ[MAX_FUNC_NAME_LEN];

    ਜੇ (!SCRIPT_ID_VALID(sid))
	ਵਾਪਸ ਨਿਰਾ;	// not in a script

    func_name_with_sid(name, sid, ਬਫਰ);
    hi = hash_ਲੱਭੋ(&func_hashtab, ਬਫਰ);
    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ HI2UF(hi);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ਸਥਿਰ ufunc_T *
ਲੱਭੋ_func_with_ਅਗੇਤਰ(char_u *name, ਪੂਰਨ sid)
{
    hashitem_T	    *hi;
    char_u	    ਬਫਰ[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ਜੇ (vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;	// already has the prefix
    ਜੇ (!SCRIPT_ID_VALID(sid))
	ਵਾਪਸ ਨਿਰਾ;	// not in a script
    si = SCRIPT_ਵਸਤੂ(sid);
    ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
    {
	size_t	len = STRLEN(si->sn_autoload_ਅਗੇਤਰ) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ਜੇ (namep == ਨਿਰਾ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ਜੇ (len < ਆਕਾਰ(ਬਫਰ))
	    auto_name = ਬਫਰ;
	ਨਹੀਂਤਾਂ
	    auto_name = alloc(len);
	ਜੇ (auto_name != ਨਿਰਾ)
	{
	    vim_snprintf((ਅੱਖਰ *)auto_name, len, "%s%s",
						si->sn_autoload_ਅਗੇਤਰ, namep);
	    hi = hash_ਲੱਭੋ(&func_hashtab, auto_name);
	    ਜੇ (auto_name != ਬਫਰ)
		vim_free(auto_name);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		ਵਾਪਸ HI2UF(hi);
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
ਲੱਭੋ_func_even_dead(char_u *name, ਪੂਰਨ flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ਜੇ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ਜੇ (ਵਿੱਚ_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = ਲੱਭੋ_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ਜੇ (func != ਨਿਰਾ)
		ਵਾਪਸ func;
	}
	ਜੇ (ਵਿੱਚ_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    ਲੰਮਾ    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ਜੇ (*p == '_')
	    {
		func = ਲੱਭੋ_func_with_sid(p + 1, (ਪੂਰਨ)sid);
		ਜੇ (func != ਨਿਰਾ)
		    ਵਾਪਸ func;
	    }
	}
    }

    ਜੇ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_ਲੱਭੋ(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਵਾਪਸ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ਵਾਪਸ ਲੱਭੋ_func_with_ਅਗੇਤਰ(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
ਲੱਭੋ_func(char_u *name, ਪੂਰਨ is_global)
{
    ufunc_T	*fp = ਲੱਭੋ_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ਜੇ (fp != ਨਿਰਾ && (fp->uf_flags & FC_DEAD) == 0)
	ਵਾਪਸ fp;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    ਪੂਰਨ
func_is_global(ufunc_T *ufunc)
{
    ਵਾਪਸ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    ਪੂਰਨ
func_requires_g_ਅਗੇਤਰ(ufunc_T *ufunc)
{
    ਵਾਪਸ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ਨਿਰਾ
	    && !ਸੁਰੱਖਿਅਤ_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ਸਥਿਰ ਪੂਰਨ
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    ਪੂਰਨ	len;

    ਜੇ (!func_is_global(fp))
	len = vim_snprintf((ਅੱਖਰ *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ਨਹੀਂਤਾਂ
	len = vim_snprintf((ਅੱਖਰ *)buf, bufsize, "%s", fp->uf_name);

    ਵਾਪਸ (len >= (ਪੂਰਨ)bufsize) ? (ਪੂਰਨ)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    ਅੱਖਰ	*name,
    varnumber_T nr)
{
    STRCPY(v->di_ਕੁੰਜੀ, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_ਜੋੜੋ(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_ਸੰਖਿਆ;
    v->di_tv.v_ਤਾਲਾ = VAR_FIXED;
    v->di_tv.vval.v_ਸੰਖਿਆ = nr;
}

/*
 * Free "fc".
 */
    ਸਥਿਰ ਖਾਲੀ
free_funccal(funccall_T *fc)
{
    ਪੂਰਨ	i;

    ਲਈ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_ਅੰਕੜੇ))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ਜੇ (fp != ਨਿਰਾ && fp->uf_scoped == fc)
	    fp->uf_scoped = ਨਿਰਾ;
    }
    ga_clear(&fc->fc_ufuncs);

    func_ptr_unref(fc->fc_func);
    vim_free(fc);
}

/*
 * Free "fc" and what it contains.
 * Can be called only when "fc" is kept beyond the period of it called,
 * i.e. after cleanup_function_call(fc).
 */
   ਸਥਿਰ ਖਾਲੀ
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    ਸਥਿਰ ਖਾਲੀ
cleanup_function_call(funccall_T *fc)
{
    ਪੂਰਨ	may_free_fc = fc->fc_refcount <= 0;
    ਪੂਰਨ	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ਜੇ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ਨਹੀਂਤਾਂ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ਜੇ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (ਪੂਰਨ)fc->fc_l_avars.dv_hashtab.ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ਜੇ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_ਪਹਿਲਾ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ਜੇ (free_fc)
	free_funccal(fc);
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਰ ਪੂਰਨ made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = ਪਿਛਲਾ_funccal;
	ਪਿਛਲਾ_funccal = fc;

	ਜੇ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ਨਹੀਂਤਾਂ ਜੇ (++made_copy >= (ਪੂਰਨ)((4096 * 1024) / ਆਕਾਰ(*fc)))
	{
	    // We have made a lot of copies, worth 4 Mbyte.  This can happen
	    // when repetitively calling a function that creates a reference to
	    // itself somehow.  Call the garbage collector soon to avoid using
	    // too much memory.
	    made_copy = 0;
	    want_garbage_collect = TRUE;
	}
    }
}

/*
 * Return TRUE if "name" is a numbered function, ignoring a "g:" prefix.
 */
    ਸਥਿਰ ਪੂਰਨ
numbered_function(char_u *name)
{
    ਵਾਪਸ ਸੁਰੱਖਿਅਤ_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && ਸੁਰੱਖਿਅਤ_isdigit(name[2]));
}

/*
 * There are two kinds of function names:
 * 1. ordinary names, function defined with :function or :def;
 *    can start with "<SNR>123_" literally or with K_SPECIAL.
 * 2. Numbered functions and lambdas: "<lambda>123"
 * For the first we only count the name stored in func_hashtab as a reference,
 * using function() does not count as a reference, because the function is
 * looked up by name.
 */
    ਪੂਰਨ
func_name_refcount(char_u *name)
{
    ਵਾਪਸ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ਸਥਿਰ ਖਾਲੀ
funccal_unref(funccall_T *fc, ufunc_T *fp, ਪੂਰਨ force)
{
    funccall_T	**pfc;
    ਪੂਰਨ		i;

    ਜੇ (fc == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	ਲਈ (pfc = &ਪਿਛਲਾ_funccal; *pfc != ਨਿਰਾ; pfc = &(*pfc)->fc_caller)
	{
	    ਜੇ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ਵਾਪਸ;
	    }
	}
    ਲਈ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ਜੇ (((ufunc_T **)(fc->fc_ufuncs.ga_ਅੰਕੜੇ))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_ਅੰਕੜੇ))[i] = ਨਿਰਾ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ਸਥਿਰ ਪੂਰਨ
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ਜੇ (fp->uf_flags & FC_DEAD)
	ਵਾਪਸ FALSE;

    hi = hash_ਲੱਭੋ(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi) || HI2UF(hi) != fp)
	ਵਾਪਸ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ਜੇ (fp->uf_def_ਸਥਿਤੀ == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ਵਾਪਸ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
func_clear_ਚੀਜ਼ਾਂ(ufunc_T *fp)
{
    ga_clear_ਲੜੀਆਂ(&(fp->uf_ਦਲੀਲਾਂ));
    ga_clear_ਲੜੀਆਂ(&(fp->uf_def_ਦਲੀਲਾਂ));
    ga_clear_ਲੜੀਆਂ(&(fp->uf_ਸਤਰਾਂ));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_ਖੰਡ_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ਜੇ (IS_GENERIC_FUNC(fp))
	generic_func_clear_ਚੀਜ਼ਾਂ(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ਨਿਰਾ;
    fp->uf_refcount -= 3;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    ਜੇ (fp->uf_cb_free != ਨਿਰਾ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ਨਿਰਾ;
    }

    fp->uf_cb_state = ਨਿਰਾ;
    fp->uf_cb = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_ਗਿਣਤੀ);
    VIM_CLEAR(fp->uf_tml_ਕੁੱਲ);
    VIM_CLEAR(fp->uf_tml_self);
#ਜੇ_ਅੰਤ
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ਸਥਿਰ ਖਾਲੀ
func_clear(ufunc_T *fp, ਪੂਰਨ force)
{
    ਜੇ (fp->uf_cleared)
	ਵਾਪਸ;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_ਚੀਜ਼ਾਂ(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    ਸਥਿਰ ਪੂਰਨ
func_free(ufunc_T *fp, ਪੂਰਨ force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ਜੇ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ਜੇ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ਜੇ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    ਖਾਲੀ
func_clear_free(ufunc_T *fp, ਪੂਰਨ force)
{
    func_clear(fp, force);
    ਜੇ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ਨਹੀਂਤਾਂ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    ਪੂਰਨ
copy_lambda_ਮੰਜ਼ਿਲ_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = ਲੱਭੋ_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ਨਿਰਾ;

    ਜੇ (ufunc == ਨਿਰਾ)
    {
	semsg(_(e_lambda_function_not_ਮਿਲਿਆ_str), lambda);
	ਵਾਪਸ FAIL;
    }

    fp = ਲੱਭੋ_func(global, TRUE);
    ਜੇ (fp != ਨਿਰਾ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_replace), global);
	ਵਾਪਸ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ਜੇ (fp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_ਸਥਿਤੀ = ufunc->uf_def_ਸਥਿਤੀ;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ਜੇ (ga_copy_ਲੜੀਆਂ(&ufunc->uf_ਦਲੀਲਾਂ, &fp->uf_ਦਲੀਲਾਂ) == FAIL
	    || ga_copy_ਲੜੀਆਂ(&ufunc->uf_def_ਦਲੀਲਾਂ, &fp->uf_def_ਦਲੀਲਾਂ)
								    == FAIL
	    || ga_copy_ਲੜੀਆਂ(&ufunc->uf_ਸਤਰਾਂ, &fp->uf_ਸਤਰਾਂ) == FAIL)
	ਜਾਓ failed;

    ਜੇ (ufunc->uf_arg_types != ਨਿਰਾ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_ਦਲੀਲਾਂ.ga_len);
	ਜੇ (fp->uf_arg_types == ਨਿਰਾ)
	    ਜਾਓ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    ਆਕਾਰ(type_T *) * fp->uf_ਦਲੀਲਾਂ.ga_len);
    }
    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ਜੇ (fp->uf_va_name == ਨਿਰਾ)
	    ਜਾਓ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_ਜੋੜੋ(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ਜੇ ((ufunc->uf_flags & FC_ਘੇਰਾ) && ufunc->uf_partial == ਨਿਰਾ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ਜੇ (pt == ਨਿਰਾ)
	    ਜਾਓ failed;
	ਜੇ (fill_partial_and_ਘੇਰਾ(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    ਜਾਓ failed;
	}
	ufunc->uf_partial = pt;
    }

    ਵਾਪਸ OK;

failed:
    func_clear_free(fp, TRUE);
    ਵਾਪਸ FAIL;
}

ਸਥਿਰ ਪੂਰਨ	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    ਪੂਰਨ
funcdepth_increment(ਖਾਲੀ)
{
    ਜੇ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_ਡੂੰਘਾਈ_is_higher_than_maxfuncdepth));
	ਵਾਪਸ FAIL;
    }
    ++funcdepth;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
funcdepth_decrement(ਖਾਲੀ)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    ਪੂਰਨ
funcdepth_get(ਖਾਲੀ)
{
    ਵਾਪਸ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    ਖਾਲੀ
funcdepth_restore(ਪੂਰਨ ਡੂੰਘਾਈ)
{
    funcdepth = ਡੂੰਘਾਈ;
}

/*
 * Allocate a funccall_T, link it in current_funccal and fill in "fp" and
 * "rettv".
 * Must be followed by one call to remove_funccal() or cleanup_function_call().
 * Returns NULL when allocation fails.
 */
    funccall_T *
create_funccal(ufunc_T *fp, typval_T *rettv)
{
    funccall_T *fc = ALLOC_CLEAR_ONE(funccall_T);

    ਜੇ (fc == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ਵਾਪਸ fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    ਖਾਲੀ
remove_funccal(ਖਾਲੀ)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ਸਥਿਰ funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    ਪੂਰਨ		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    ਪੂਰਨ		using_sandbox = FALSE;
    ਪੂਰਨ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    ਪੂਰਨ		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    ਪੂਰਨ		default_arg_err = FALSE;
    dictitem_T	*v;
    ਪੂਰਨ		fixvar_idx = 0;	// index in fc_fixvar[]
    ਪੂਰਨ		i;
    ਪੂਰਨ		ai;
    ਪੂਰਨ		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_ਮੰਜ਼ਿਲ_free[MAX_FUNC_ਦਲੀਲਾਂ];
    ਪੂਰਨ		tv_ਮੰਜ਼ਿਲ_free_len = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    profinfo_T	profile_info;
#ਜੇ_ਅੰਤ
    ESTACK_CHECK_DECLARATION;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    CLEAR_ਖੇਤਰ(profile_info);
#ਜੇ_ਅੰਤ

    // If depth of calling is getting too high, don't execute the function.
    ਜੇ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = -1;
	ਵਾਪਸ FCERR_FAILED;
    }

    ਸਤਰ_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ਜੇ (fc == ਨਿਰਾ)
	ਵਾਪਸ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, ਆਕਾਰ(ufunc_T *), 1);

    ਜੇ (fp->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED)
    {
	ਜੇ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ਨਿਰਾ ? ਨਿਰਾ : fc->fc_caller->fc_func;
#ਜੇ_ਅੰਤ
	// Execute the function, possibly compiling it first.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ਜੇ (do_profiling == PROF_YES)
	    profile_may_ਸ਼ੁਰੂ_func(&profile_info, fp, caller);
#ਜੇ_ਅੰਤ
	sticky_cmdmod_flags = 0;
	ਜੇ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ਜੇ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ਜੇ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ਨਿਰਾ && caller->uf_profiling)))
	    profile_may_ਸਿਰਾ_func(&profile_info, fp, caller);
#ਜੇ_ਅੰਤ
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ਵਾਪਸ retval;
    }

    islambda = fp->uf_flags & FC_LAMBDA;

    /*
     * Note about using fc->fc_fixvar[]: This is an array of FIXVAR_CNT
     * variables with names up to VAR_SHORT_LEN long.  This avoids having to
     * alloc/free each argument variable and saves a lot of time.
     */
    /*
     * Init l: variables.
     */
    init_var_dict(&fc->fc_l_vars, &fc->fc_l_vars_var, VAR_DEF_SCOPE);
    ਜੇ (selfdict != ਨਿਰਾ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ਕੁੰਜੀ;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_ਜੋੜੋ(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_ਤਾਲਾ = 0;
	v->di_tv.vval.v_dict = selfdict;
	++selfdict->dv_refcount;
    }

    /*
     * Init a: variables, unless none found (in lambda).
     * Set a:0 to "argcount" less number of named arguments, if >= 0.
     * Set a:000 to a list with room for the "..." arguments.
     */
    init_var_dict(&fc->fc_l_avars, &fc->fc_l_avars_var, VAR_SCOPE);
    ਜੇ ((fp->uf_flags & FC_NOARGS) == 0)
	ਜੋੜੋ_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_ਦਲੀਲਾਂ.ga_len
				    ? argcount - fp->uf_ਦਲੀਲਾਂ.ga_len : 0));
    fc->fc_l_avars.dv_ਤਾਲਾ = VAR_FIXED;
    ਜੇ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ਕੁੰਜੀ;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_ਜੋੜੋ(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_ਤਾਲਾ = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_ਖੇਤਰ(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_ਤਾਲਾ = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    ਜੇ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	ਜੋੜੋ_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	ਜੋੜੋ_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    ਲਈ (i = 0; i < argcount || i < fp->uf_ਦਲੀਲਾਂ.ga_len; ++i)
    {
	ਪੂਰਨ	    addlocal = FALSE;
	typval_T    def_rettv;
	ਪੂਰਨ	    isdefault = FALSE;

	ai = i - fp->uf_ਦਲੀਲਾਂ.ga_len;
	ਜੇ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ਜੇ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_ਦਲੀਲਾਂ.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_ਸੰਖਿਆ == VVAL_NONE));
	    ਜੇ (isdefault)
	    {
		char_u	    *default_ਵਿਅੰਜਕ = ਨਿਰਾ;

		def_rettv.v_type = VAR_ਸੰਖਿਆ;
		def_rettv.vval.v_ਸੰਖਿਆ = -1;

		default_ਵਿਅੰਜਕ = ((char_u **)(fp->uf_def_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))
						 [ai + fp->uf_def_ਦਲੀਲਾਂ.ga_len];
		ਜੇ (eval1(&default_ਵਿਅੰਜਕ, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ਰੋਕੋ;
		}
	    }

	    namelen = STRLEN(name);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ਰੋਕੋ;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((ਅੱਖਰ *)numbuf, ਆਕਾਰ(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ਜੇ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_ਕੁੰਜੀ, name);
	}
	ਨਹੀਂਤਾਂ
	{
	    v = dictitem_alloc(name);
	    ਜੇ (v == ਨਿਰਾ)
		ਰੋਕੋ;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_ਤਾਲਾ = VAR_FIXED;

	ਜੇ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_ਮੰਜ਼ਿਲ_free[tv_ਮੰਜ਼ਿਲ_free_len++] = &v->di_tv;

	ਜੇ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_ਜੋੜੋ(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ਨਹੀਂਤਾਂ
	    hash_ਜੋੜੋ(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ਜੇ (ai >= 0 && ai < MAX_FUNC_ਦਲੀਲਾਂ)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_ਤਾਲਾ = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ਜੇ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ਧੱਕੋ_ufunc(fp, 1);
    ESTACK_CHECK_ਤਿਆਰੀ;
    ਜੇ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ਜੇ (p_verbose >= 14)
	{
	    char_u	buf[ਸੁਨੇਹਾ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    ਸੁਨੇਹਾ_puts("(");
	    ਲਈ (i = 0; i < argcount; ++i)
	    {
		ਜੇ (i > 0)
		    ਸੁਨੇਹਾ_puts(", ");
		ਜੇ (argvars[i].v_type == VAR_ਸੰਖਿਆ)
		    ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)argvars[i].vval.v_ਸੰਖਿਆ);
		ਨਹੀਂਤਾਂ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ਜੇ (s != ਨਿਰਾ)
		    {
			ਜੇ (vim_strsize(s) > ਸੁਨੇਹਾ_BUF_CLEN)
			{
			    trunc_string(s, buf, ਸੁਨੇਹਾ_BUF_CLEN, ਸੁਨੇਹਾ_BUF_LEN);
			    s = buf;
			}
			ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)s);
			vim_free(tofree);
		    }
		}
	    }
	    ਸੁਨੇਹਾ_puts(")");
	}
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	profile_may_ਸ਼ੁਰੂ_func(&profile_info, fp,
			fc->fc_caller == ਨਿਰਾ ? ਨਿਰਾ : fc->fc_caller->fc_func);
#ਜੇ_ਅੰਤ

    // "legacy" does not apply to commands in the function
    sticky_cmdmod_flags = 0;

    // If called from a compiled :def function the execution context must be
    // hidden, any deferred functions need to be added to the function being
    // executed here.
    save_current_ectx = clear_current_ectx();

    save_current_sctx = current_sctx;
    current_sctx = fp->uf_script_ctx;
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ਜੇ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    ਨਹੀਂਤਾਂ ਜੇ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(ਖਾਲੀ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ਨਹੀਂਤਾਂ
	// call do_cmdline() to execute the lines
	do_cmdline(ਨਿਰਾ, get_func_ਸਤਰ, (ਖਾਲੀ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ਜੇ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_ਸੰਖਿਆ;
	rettv->vval.v_ਸੰਖਿਆ = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ਜੇ (ਵਿੱਚ_vim9script())
	    retval = FCERR_FAILED;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ਨਿਰਾ ? ਨਿਰਾ : fc->fc_caller->fc_func;

	ਜੇ (fp->uf_profiling || (caller != ਨਿਰਾ && caller->uf_profiling))
	    profile_may_ਸਿਰਾ_func(&profile_info, fp, caller);
    }
#ਜੇ_ਅੰਤ

    // when being verbose, mention the return value
    ਜੇ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ਜੇ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ਨਹੀਂਤਾਂ ਜੇ (fc->fc_rettv->v_type == VAR_ਸੰਖਿਆ)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (ਲੰਮਾ)fc->fc_rettv->vval.v_ਸੰਖਿਆ);
	ਨਹੀਂਤਾਂ
	{
	    char_u	buf[ਸੁਨੇਹਾ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    ਜੇ (s != ਨਿਰਾ)
	    {
		ਜੇ (vim_strsize(s) > ਸੁਨੇਹਾ_BUF_CLEN)
		{
		    trunc_string(s, buf, ਸੁਨੇਹਾ_BUF_CLEN, ਸੁਨੇਹਾ_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ
    ਜੇ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ਜੇ (p_verbose >= 12 && SOURCING_NAME != ਨਿਰਾ)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    ਲਈ (i = 0; i < tv_ਮੰਜ਼ਿਲ_free_len; ++i)
	clear_tv(tv_ਮੰਜ਼ਿਲ_free[i]);
    cleanup_function_call(fc);

    ਵਾਪਸ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, ਪੂਰਨ argcount)
{
    ਪੂਰਨ regular_ਦਲੀਲਾਂ = fp->uf_ਦਲੀਲਾਂ.ga_len;

    ਜੇ (argcount < regular_ਦਲੀਲਾਂ - fp->uf_def_ਦਲੀਲਾਂ.ga_len)
	ਵਾਪਸ FCERR_TOOFEW;
    ਨਹੀਂਤਾਂ ਜੇ (!has_varargs(fp) && argcount > regular_ਦਲੀਲਾਂ)
	ਵਾਪਸ FCERR_TOOMANY;
    ਵਾਪਸ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	ਪੂਰਨ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    ਜੇ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ਵਾਪਸ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ਨਿਰਾ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ਜੇ (error != FCERR_UNKNOWN)
	ਵਾਪਸ error;

    ਜੇ ((fp->uf_flags & FC_DICT) && selfdict == ਨਿਰਾ)
    {
	error = FCERR_DICT;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_ਖੋਜ_patterns();
	ਜੇ (!ins_compl_ਸਰਗਰਮ())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ਨਿਰਾ);
	ਜੇ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ਜੇ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_ਖੋਜ_patterns();
    }

    ਵਾਪਸ error;
}

ਸਥਿਰ funccal_ਇੰਦਰਾਜ_T *funccal_ਢੇਰ = ਨਿਰਾ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    ਖਾਲੀ
save_funccal(funccal_ਇੰਦਰਾਜ_T *ਇੰਦਰਾਜ)
{
    ਇੰਦਰਾਜ->ਸਿਖਰ_funccal = current_funccal;
    ਇੰਦਰਾਜ->ਅਗਲਾ = funccal_ਢੇਰ;
    funccal_ਢੇਰ = ਇੰਦਰਾਜ;
    current_funccal = ਨਿਰਾ;
}

    ਖਾਲੀ
restore_funccal(ਖਾਲੀ)
{
    ਜੇ (funccal_ਢੇਰ == ਨਿਰਾ)
	ਅੰਦਰੂਨੀ_error("restore_funccal()");
    ਨਹੀਂਤਾਂ
    {
	current_funccal = funccal_ਢੇਰ->ਸਿਖਰ_funccal;
	funccal_ਢੇਰ = funccal_ਢੇਰ->ਅਗਲਾ;
    }
}

    funccall_T *
get_current_funccal(ਖਾਲੀ)
{
    ਵਾਪਸ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    ਪੂਰਨ
at_script_level(ਖਾਲੀ)
{
    ਵਾਪਸ current_funccal == ਨਿਰਾ && autocmd_ਮੇਲ == ਨਿਰਾ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    ਖਾਲੀ
delete_script_functions(ਪੂਰਨ sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (ਪੂਰਨ)KE_SNR;
    len = 3 + vim_snprintf((ਅੱਖਰ *)buf + 3, ਆਕਾਰ(buf) - 3, "%d_", sid);

    ਜਦੋਂ (todo > 0)
    {
	todo = func_hashtab.ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&func_hashtab, hi, todo)
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		fp = HI2UF(hi);
		ਜੇ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    ਪੂਰਨ ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;

		    fp->uf_flags |= FC_DEAD;

		    ਜੇ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ਜੇ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ਜੇ (ਬਦਲਿਆ != func_hashtab.ht_ਬਦਲਿਆ)
			    ਰੋਕੋ;
		    }
		}
		--todo;
	    }
    }
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_all_functions(ਖਾਲੀ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    ਪੂਰਨ		ਬਦਲਿਆ;

    // Clean up the current_funccal chain and the funccal stack.
    ਜਦੋਂ (current_funccal != ਨਿਰਾ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ਜੇ (current_funccal == ਨਿਰਾ && funccal_ਢੇਰ != ਨਿਰਾ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ਜਦੋਂ (todo > 0)
    {
	todo = func_hashtab.ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&func_hashtab, hi, todo)
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ਜੇ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ਨਹੀਂਤਾਂ
		{
		    ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
		    func_clear(fp, TRUE);
		    ਜੇ (ਬਦਲਿਆ != func_hashtab.ht_ਬਦਲਿਆ)
		    {
			skipped = 0;
			ਰੋਕੋ;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ਜਦੋਂ (func_hashtab.ht_ਵਰਤਿਆ > skipped)
    {
	todo = func_hashtab.ht_ਵਰਤਿਆ;
	FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&func_hashtab, hi, todo)
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ਜੇ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			ਰੋਕੋ;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ਜੇ (skipped == 0)
	hash_clear(&func_hashtab);

    free_def_functions();
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "name" looks like a builtin function name: starts with a
 * lower case letter, doesn't contain AUTOLOAD_CHAR or ':', no "." after the
 * name.
 * "len" is the length of "name", or -1 for NUL terminated.
 */
    ਪੂਰਨ
builtin_function(char_u *name, ਪੂਰਨ len)
{
    ਪੂਰਨ i;

    ਜੇ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ਵਾਪਸ FALSE;
    ਲਈ (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ਜੇ (name[i] == AUTOLOAD_CHAR)
	    ਵਾਪਸ FALSE;
	ਜੇ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ਜੇ (name[i] == '.')
		ਵਾਪਸ FALSE;
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ TRUE;
}

    ਪੂਰਨ
func_call(
    char_u	*name,
    typval_T	*ਦਲੀਲਾਂ,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = ਦਲੀਲਾਂ->vval.v_list;
    listitem_T	*ਵਸਤੂ;
    typval_T	argv[MAX_FUNC_ਦਲੀਲਾਂ + 1];
    ਪੂਰਨ		argc = 0;
    ਪੂਰਨ		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, ਵਸਤੂ)
    {
	ਜੇ (argc == MAX_FUNC_ਦਲੀਲਾਂ - (partial == ਨਿਰਾ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ਰੋਕੋ;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&ਵਸਤੂ->li_tv, &argv[argc++]);
    }

    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
    {
	funcexe_T	funcexe;
	ਪੂਰਨ		namelen = -1;

	ਜੇ (ਵਿੱਚ_vim9script())
	{
	    char_u *p = generic_func_ਲੱਭੋ_open_bracket(name);

	    ਜੇ (p != ਨਿਰਾ)
	    {
		ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&p) == FAIL)
		    ਜਾਓ ਪੂਰਾ;
		namelen = p - name + 1;
	    }
	}

	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ਜੇ (partial != ਨਿਰਾ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ਜੇ (funcexe.fe_object != ਨਿਰਾ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ਜਦੋਂ (argc > 0)
	clear_tv(&argv[--argc]);

ਪੂਰਾ:
    ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ callback_ਡੂੰਘਾਈ = 0;

    ਪੂਰਨ
get_callback_ਡੂੰਘਾਈ(ਖਾਲੀ)
{
    ਵਾਪਸ callback_ਡੂੰਘਾਈ;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    ਪੂਰਨ
call_callback(
    callback_T	*callback,
    ਪੂਰਨ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ਪੂਰਨ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    ਪੂਰਨ		ret;

    ਜੇ (callback->cb_name == ਨਿਰਾ || *callback->cb_name == NUL)
	ਵਾਪਸ FAIL;

    ਜੇ (callback_ਡੂੰਘਾਈ > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ਵਾਪਸ FAIL;
    }

    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ਜੇ (callback->cb_partial != ਨਿਰਾ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ਜੇ (funcexe.fe_object != ਨਿਰਾ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_ਡੂੰਘਾਈ;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_ਡੂੰਘਾਈ;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ਜੇ (need_rethrow && current_exception != ਨਿਰਾ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ਵਾਪਸ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    ਪੂਰਨ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ਜੇ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ਵਾਪਸ -2;

    retval = tv_get_ਸੰਖਿਆ_chk(&rettv, ਨਿਰਾ);
    clear_tv(&rettv);
    ਵਾਪਸ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    ਖਾਲੀ
user_func_error(funcerror_T error, char_u *name, ਪੂਰਨ ਮਿਲਿਆ_var)
{
    ਚੋਣ (error)
    {
	ਹਾਲਤ FCERR_UNKNOWN:
		ਜੇ (ਮਿਲਿਆ_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ਨਹੀਂਤਾਂ
		    emsg_funcname(e_unknown_function_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_ਵਿਧੀ_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_ਵਿੱਚ_script_context_str, name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_OTHER:
	ਹਾਲਤ FCERR_FAILED:
		// assume the error message was already given
		ਰੋਕੋ;
	ਹਾਲਤ FCERR_NONE:
		ਰੋਕੋ;
    }
}

/*
 * Check the argument types "argvars[argcount]" for "name" using the
 * information in "funcexe".  When "base_included" then "funcexe->fe_basetv"
 * is already included in "argvars[]".
 * Will do nothing if "funcexe->fe_check_type" is NULL or
 * "funcexe->fe_evaluate" is FALSE;
 * Returns an FCERR_ value.
 */
    ਸਥਿਰ funcerror_T
may_check_ਦਲੀਲ_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	ਪੂਰਨ	    argcount,
	ਪੂਰਨ	    ਅਧਾਰ_included,
	char_u	    *name)
{
    ਜੇ (funcexe->fe_check_type != ਨਿਰਾ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ਜੇ (check_ਦਲੀਲ_types(funcexe->fe_check_type,
			  argvars, argcount,
			  ਅਧਾਰ_included ? ਨਿਰਾ : funcexe->fe_basetv,
			  name) == FAIL)
	    ਵਾਪਸ FCERR_OTHER;
    }
    ਵਾਪਸ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    ਪੂਰਨ
call_func(
    char_u	*funcname,	// name of the function
    ਪੂਰਨ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ਪੂਰਨ		argcount_ਵਿੱਚ,	// number of "argvars"
    typval_T	*argvars_ਵਿੱਚ,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    ਪੂਰਨ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    ਪੂਰਨ		i;
    ufunc_T	*fp = ਨਿਰਾ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ਨਿਰਾ;
    char_u	*fname = ਨਿਰਾ;
    char_u	*name = ਨਿਰਾ;
    ਪੂਰਨ		argcount = argcount_ਵਿੱਚ;
    typval_T	*argvars = argvars_ਵਿੱਚ;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ਦਲੀਲਾਂ + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    ਪੂਰਨ		argv_clear = 0;
    ਪੂਰਨ		argv_ਅਧਾਰ = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_ਦਲੀਲਾਂ[MAX_FUNC_ਦਲੀਲਾਂ];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    ਜੇ (partial != ਨਿਰਾ)
	fp = partial->pt_func;
    ਜੇ (fp == ਨਿਰਾ)
	fp = funcexe->fe_ufunc;

    ਜੇ (fp == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	{
	    char_u *p = generic_func_ਲੱਭੋ_open_bracket(funcname);

	    ਜੇ (p != ਨਿਰਾ)
	    {
		len = p - funcname;
		ਜੇ (parse_generic_func_type_ਦਲੀਲਾਂ(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ਨਿਰਾ)
		    ਜਾਓ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ਜੇ (name == ਨਿਰਾ)
	    ਜਾਓ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ਜੇ (funcexe->fe_doesrange != ਨਿਰਾ)
	*funcexe->fe_doesrange = FALSE;

    ਜੇ (partial != ਨਿਰਾ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ਜੇ (partial->pt_dict != ਨਿਰਾ && (selfdict == ਨਿਰਾ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ਜੇ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    ਲਈ (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ਜੇ (argv_clear + argcount_ਵਿੱਚ >= MAX_FUNC_ਦਲੀਲਾਂ)
		{
		    error = FCERR_TOOMANY;
		    ਜਾਓ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    ਲਈ (i = 0; i < argcount_ਵਿੱਚ; ++i)
		argv[i + argv_clear] = argvars_ਵਿੱਚ[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_ਵਿੱਚ;

	    ਜੇ (funcexe->fe_check_type != ਨਿਰਾ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_ਦਲੀਲਾਂ = check_type_ਦਲੀਲਾਂ;
		CLEAR_ਖੇਤਰ(check_type_ਦਲੀਲਾਂ);
		ਲਈ (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_ਦਲੀਲਾਂ[i + partial->pt_argc] =
							 check_type.tt_ਦਲੀਲਾਂ[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ਜੇ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_ਦਲੀਲ_types(funcexe, argvars, argcount, FALSE,
					     (name != ਨਿਰਾ) ? name : funcname);

    ਜੇ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	ਪੂਰਨ	is_global = FALSE;

	// Skip "g:" before a function name.
	ਜੇ (fp == ਨਿਰਾ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_ਸੰਖਿਆ;	// default rettv is number zero
	rettv->vval.v_ਸੰਖਿਆ = 0;
	error = FCERR_UNKNOWN;

	ਜੇ (fp != ਨਿਰਾ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ਜੇ (fp == ਨਿਰਾ)
	    {
		fp = ਲੱਭੋ_func(rfname, is_global);
		ਜੇ (fp != ਨਿਰਾ && !is_global && ਵਿੱਚ_vim9script()
						 && func_requires_g_ਅਗੇਤਰ(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ਨਿਰਾ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ਜੇ (fp == ਨਿਰਾ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ਨਿਰਾ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = ਲੱਭੋ_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ਜੇ (fp == ਨਿਰਾ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = ਲੱਭੋ_func(rfname, is_global);
	    }
	    ਜੇ (fp == ਨਿਰਾ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ਜੇ (p != ਨਿਰਾ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = ਲੱਭੋ_func(p, is_global);
	    }

	    ਜੇ (partial == ਨਿਰਾ && fp != ਨਿਰਾ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ਜੇ (fp == ਨਿਰਾ)
		{
		    error = FCERR_FAILED;
		    ਜਾਓ theend;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_ਮਾਪ(&gfatab) > 0)
	    {
		emsg_funcname(fp != ਨਿਰਾ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		ਜਾਓ theend;
	    }

	    ਜੇ (fp != ਨਿਰਾ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ਨਹੀਂਤਾਂ ਜੇ (fp != ਨਿਰਾ)
	    {
		ਪੂਰਨ need_arg_check = FALSE;
		ਜੇ (funcexe->fe_check_type == ਨਿਰਾ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ਜੇ (funcexe->fe_argv_func != ਨਿਰਾ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ਜੇ (funcexe->fe_basetv != ਨਿਰਾ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, ਆਕਾਰ(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_ਅਧਾਰ = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ਜੇ (need_arg_check)
		    error = may_check_ਦਲੀਲ_types(funcexe, argvars, argcount,
				       TRUE, (name != ਨਿਰਾ) ? name : funcname);

		ਜੇ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (funcexe->fe_basetv != ਨਿਰਾ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_ਅੰਦਰੂਨੀ_ਵਿਧੀ(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_ਅੰਦਰੂਨੀ_func(fname, argcount, argvars, rettv);
	}

	/*
	 * The function call (or "FuncUndefined" autocommand sequence) might
	 * have been aborted by an error, an interrupt, or an explicitly thrown
	 * exception that has not been caught so far.  This situation can be
	 * tested for by calling aborting().  For an error in an internal
	 * function or for the "E132" error in call_user_func(), however, the
	 * throw point at which the "force_abort" flag (temporarily reset by
	 * emsg()) is normally updated has not been reached yet. We need to
	 * update that flag first to make aborting() reliable.
	 */
	update_force_abort();
    }
    ਜੇ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ਜੇ (!aborting())
	user_func_error(error, (name != ਨਿਰਾ) ? name : funcname,
							funcexe->fe_ਮਿਲਿਆ_var);

    // clear the copies made from the partial
    ਜਦੋਂ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_ਅਧਾਰ]);

    vim_free(tofree);
    vim_free(name);
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);

    ਵਾਪਸ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    ਪੂਰਨ
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    ਪੂਰਨ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ਨਿਰਾ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    ਪੂਰਨ		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_ਸੰਖਿਆ;	// default rettv is number zero
    rettv->vval.v_ਸੰਖਿਆ = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ਜੇ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ਨਹੀਂਤਾਂ
	rfname = fname;
    fp = ਲੱਭੋ_func(rfname, is_global);
    ਜੇ (fp != ਨਿਰਾ && !is_global && ਵਿੱਚ_vim9script()
						 && func_requires_g_ਅਗੇਤਰ(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ਨਿਰਾ;
    ਜੇ (fp == ਨਿਰਾ)
	ret = NOTDONE;
    ਨਹੀਂਤਾਂ ਜੇ (fp != ਨਿਰਾ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ਨਹੀਂਤਾਂ ਜੇ (fp != ਨਿਰਾ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ਨਿਰਾ);
	ਜੇ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ਵਾਪਸ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ਵਾਪਸ fp->uf_name_exp != ਨਿਰਾ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
function_list_modified(ਪੂਰਨ prev_ht_ਬਦਲਿਆ)
{
    ਜੇ (prev_ht_ਬਦਲਿਆ != func_hashtab.ht_ਬਦਲਿਆ)
    {
	emsg(_(e_function_list_was_modified));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ਸਥਿਰ ਪੂਰਨ
list_func_ਸਿਰ(ufunc_T *fp, ਪੂਰਨ indent)
{
    ਪੂਰਨ		prev_ht_ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
    ਪੂਰਨ		j;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();

    // a timer at the more prompt may have deleted the function
    ਜੇ (function_list_modified(prev_ht_ਬਦਲਿਆ))
	ਵਾਪਸ FAIL;

    ਜੇ (indent)
	ਸੁਨੇਹਾ_puts("   ");
    ਜੇ (fp->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED)
	ਸੁਨੇਹਾ_puts("def ");
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts("function ");
    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)printable_func_name(fp));
    ਸੁਨੇਹਾ_putchar('(');
    ਲਈ (j = 0; j < fp->uf_ਦਲੀਲਾਂ.ga_len; ++j)
    {
	ਜੇ (j)
	    ਸੁਨੇਹਾ_puts(", ");
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)FUNCARG(fp, j));
	ਜੇ (fp->uf_arg_types != ਨਿਰਾ)
	{
	    ਅੱਖਰ *tofree;

	    ਸੁਨੇਹਾ_puts(": ");
	    ਸੁਨੇਹਾ_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ਜੇ (j >= fp->uf_ਦਲੀਲਾਂ.ga_len - fp->uf_def_ਦਲੀਲਾਂ.ga_len)
	{
	    ਸੁਨੇਹਾ_puts(" = ");
	    ਸੁਨੇਹਾ_puts(((ਅੱਖਰ **)(fp->uf_def_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))
		       [j - fp->uf_ਦਲੀਲਾਂ.ga_len + fp->uf_def_ਦਲੀਲਾਂ.ga_len]);
	}
    }
    ਜੇ (fp->uf_varargs)
    {
	ਜੇ (j)
	    ਸੁਨੇਹਾ_puts(", ");
	ਸੁਨੇਹਾ_puts("...");
    }
    ਜੇ (fp->uf_va_name != ਨਿਰਾ)
    {
	ਜੇ (!fp->uf_varargs)
	{
	    ਜੇ (j)
		ਸੁਨੇਹਾ_puts(", ");
	    ਸੁਨੇਹਾ_puts("...");
	}
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)fp->uf_va_name);
	ਜੇ (fp->uf_va_type != ਨਿਰਾ)
	{
	    ਅੱਖਰ *tofree;

	    ਸੁਨੇਹਾ_puts(": ");
	    ਸੁਨੇਹਾ_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ਸੁਨੇਹਾ_putchar(')');

    ਜੇ (fp->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED)
    {
	ਜੇ (fp->uf_ret_type != &t_void)
	{
	    ਅੱਖਰ *tofree;

	    ਸੁਨੇਹਾ_puts(": ");
	    ਸੁਨੇਹਾ_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (fp->uf_flags & FC_ABORT)
	ਸੁਨੇਹਾ_puts(" abort");
    ਜੇ (fp->uf_flags & FC_RANGE)
	ਸੁਨੇਹਾ_puts(" range");
    ਜੇ (fp->uf_flags & FC_DICT)
	ਸੁਨੇਹਾ_puts(" dict");
    ਜੇ (fp->uf_flags & FC_ਘੇਰਾ)
	ਸੁਨੇਹਾ_puts(" closure");
    ਸੁਨੇਹਾ_clr_eos();
    ਜੇ (p_verbose > 0)
	ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(fp->uf_script_ctx);

    ਵਾਪਸ OK;
}

/*
 * Get a function name, translating "<SID>" and "<SNR>".
 * Also handles a Funcref in a List or Dictionary.
 * Returns the function name in allocated memory, or NULL for failure.
 * Set "*is_global" to TRUE when the function must be global, unless
 * "is_global" is NULL.
 * flags:
 * TFN_INT:	    internal function name OK
 * TFN_IN_CLASS:    function in a class
 * TFN_QUIET:	    be quiet
 * TFN_NO_AUTOLOAD: do not use script autoloading
 * TFN_NO_DEREF:    do not dereference a Funcref
 * Advances "pp" to just after the function name (if no error).
 */
    char_u *
trans_function_name(
    char_u	**pp,
    ਪੂਰਨ		*is_global,
    ਪੂਰਨ		skip,		// only find the end, don't evaluate
    ਪੂਰਨ		flags)
{
    ਵਾਪਸ trans_function_name_ext(pp, is_global, skip, flags,
	    ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ਸਥਿਰ char_u *
trans_function_name_ext(
    char_u	**pp,
    ਪੂਰਨ		*is_global,
    ਪੂਰਨ		skip,		// only find the end, don't evaluate
    ਪੂਰਨ		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ਨਿਰਾ;
    char_u	*ਸ਼ੁਰੂ;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		lead;
    char_u	sid_buf[20];
    ਪੂਰਨ		len;
    ਪੂਰਨ		extra = 0;
    ਪੂਰਨ		ਅਗੇਤਰ_g = FALSE;
    lval_T	lv;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		vim9_local;

    ਜੇ (fdp != ਨਿਰਾ)
	CLEAR_ਸੂਚਕ(fdp);
    ਸ਼ੁਰੂ = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ਜੇ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (ਪੂਰਨ)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ਵਾਪਸ vim_strnsave(ਸ਼ੁਰੂ, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(ਸ਼ੁਰੂ);
    ਜੇ (lead > 2)
	ਸ਼ੁਰੂ += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    ਸਿਰਾ = get_lval(ਸ਼ੁਰੂ, ਨਿਰਾ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (ਸਿਰਾ == ਸ਼ੁਰੂ || (vim9script && ਸਿਰਾ != ਨਿਰਾ
				   && ਸਿਰਾ[-1] == AUTOLOAD_CHAR && *ਸਿਰਾ == '('))
    {
	ਜੇ (!skip)
	    emsg(_(e_function_name_required));
	ਜਾਓ theend;
    }
    ਜੇ (ਸਿਰਾ == ਨਿਰਾ || (lv.ll_tv != ਨਿਰਾ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ਜੇ (!aborting())
	{
	    ਜੇ (ਸਿਰਾ != ਨਿਰਾ)
		semsg(_(e_invalid_ਦਲੀਲ_str), ਸ਼ੁਰੂ);
	}
	ਨਹੀਂਤਾਂ
	    *pp = ਲੱਭੋ_name_ਸਿਰਾ(ਸ਼ੁਰੂ, ਨਿਰਾ, ਨਿਰਾ, FNE_INCL_BR);
	ਜਾਓ theend;
    }

    ਜੇ (lv.ll_ufunc != ਨਿਰਾ)
    {
	ਜੇ (ufunc != ਨਿਰਾ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = ਸਿਰਾ;
	ਜਾਓ theend;
    }

    ਜੇ (lv.ll_tv != ਨਿਰਾ)
    {
	ਜੇ (fdp != ਨਿਰਾ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ਨਿਰਾ;
	    fdp->fd_di = lv.ll_di;
	}
	ਜੇ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ਨਿਰਾ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = ਸਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ਨਿਰਾ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.ਲੰਮਾਈ);
	    *pp = ਸਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ਨਿਰਾ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = ਸਿਰਾ;
	    ਜੇ (partial != ਨਿਰਾ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!skip && !(flags & TFN_QUIET) && (fdp == ਨਿਰਾ
			     || lv.ll_dict == ਨਿਰਾ || fdp->fd_newkey == ਨਿਰਾ))
		emsg(_(e_funcref_required));
	    ਨਹੀਂਤਾਂ
		*pp = ਸਿਰਾ;
	    name = ਨਿਰਾ;
	}
	ਜਾਓ theend;
    }

    ਜੇ (lv.ll_name == ਨਿਰਾ)
    {
	// Error found, but continue after the function name.
	*pp = ਸਿਰਾ;
	ਜਾਓ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ਜੇ (lv.ll_exp_name != ਨਿਰਾ)
    {
	len = (ਪੂਰਨ)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ਨਿਰਾ);
	ਜੇ (name == lv.ll_exp_name)
	    name = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ਵਸਤੂ(lv.ll_sid);
	ਪੂਰਨ		cc = *lv.ll_name_ਸਿਰਾ;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_ਸਿਰਾ = NUL;
	ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	{
	    name = concat_str(si->sn_autoload_ਅਗੇਤਰ, lv.ll_name);
	}
	ਨਹੀਂਤਾਂ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (ਪੂਰਨ)KE_SNR;
	    vim_snprintf((ਅੱਖਰ *)sid_buf + 3, ਆਕਾਰ(sid_buf) - 3,
						      "%ld_", (ਲੰਮਾ)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_ਸਿਰਾ = cc;
	*pp = ਸਿਰਾ;
	ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!(flags & TFN_NO_DEREF))
    {
	len = (ਪੂਰਨ)(ਸਿਰਾ - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ਨਿਰਾ);
	ਜੇ (name == *pp)
	    name = ਨਿਰਾ;
    }
    ਜੇ (name != ਨਿਰਾ)
    {
	name = vim_strsave(name);
	*pp = ਸਿਰਾ;
	ਜੇ (name != ਨਿਰਾ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ਪੂਰਨ)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	ਜਾਓ theend;
    }

    ਜੇ (lv.ll_exp_name != ਨਿਰਾ)
    {
	len = (ਪੂਰਨ)STRLEN(lv.ll_exp_name);
	ਜੇ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// skip over "s:" and "g:"
	ਜੇ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ਜੇ (lv.ll_name[0] == 'g')
	    {
		ਜੇ (is_global != ਨਿਰਾ)
		{
		    *is_global = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    ਅਗੇਤਰ_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (ਪੂਰਨ)(ਸਿਰਾ - lv.ll_name);
    }
    ਜੇ (len <= 0)
    {
	ਜੇ (!skip)
	    emsg(_(e_function_name_required));
	ਜਾਓ theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*ਸ਼ੁਰੂ) && vim9script
						&& (flags & TFN_ਵਿੱਚ_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    ਜੇ (skip)
	lead = 0;	// do nothing
    ਨਹੀਂਤਾਂ ਜੇ (lead > 0 || vim9_local)
    {
	ਜੇ (!vim9_local)
	{
	    ਜੇ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_ਸ਼ੁਰੂ_with_capital_str), ਸ਼ੁਰੂ);
		ਜਾਓ theend;
	    }
	    lead = 3;
	}
	ਜੇ (vim9_local || (lv.ll_exp_name != ਨਿਰਾ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ਜੇ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_ਵਿੱਚ_script_context));
		ਜਾਓ theend;
	    }
	    sid_buflen = vim_snprintf((ਅੱਖਰ *)sid_buf, ਆਕਾਰ(sid_buf), "%ld_", (ਲੰਮਾ)current_sctx.sc_sid);
	    ਜੇ (vim9_local)
		extra = 3 + (ਪੂਰਨ)sid_buflen;
	    ਨਹੀਂਤਾਂ
		lead += (ਪੂਰਨ)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ਨਹੀਂਤਾਂ ਜੇ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_ਵਿੱਚ_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_ਸ਼ੁਰੂ_with_capital_str
			   : e_function_name_must_ਸ਼ੁਰੂ_with_capital_or_s_str),
									ਸ਼ੁਰੂ);
	ਜਾਓ theend;
    }
    ਜੇ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ਜੇ (cp != ਨਿਰਾ && cp < ਸਿਰਾ)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), ਸ਼ੁਰੂ);
	    ਜਾਓ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ਜੇ (name != ਨਿਰਾ)
    {
	ਜੇ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ਪੂਰਨ)KE_SNR;
	    ਜੇ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਅਗੇਤਰ_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = ਸਿਰਾ;

theend:
    clear_lval(&lv);
    ਵਾਪਸ name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ਸਥਿਰ char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    ਜੇ (*name == K_SPECIAL && ਵਿੱਚ_vim9script())
    {
	p = vim_strchr(name, '_');
	ਜੇ (p != ਨਿਰਾ)
	    ਵਾਪਸ p + 1;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * If the 'funcname' starts with "s:" or "<SID>", then expands it to the
 * current script ID and returns the expanded function name. The caller should
 * free the returned name. If not called from a script context or the function
 * name doesn't start with these prefixes, then returns NULL.
 * This doesn't check whether the script-local function exists or not.
 */
    char_u *
get_scriptlocal_funcname(char_u *funcname)
{
    ਅੱਖਰ	sid_buf[25];
    size_t	sid_buflen;
    ਪੂਰਨ		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ਜੇ (funcname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ਜੇ (!ਵਿੱਚ_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ਵਾਪਸ ਨਿਰਾ;
	ufunc = ਲੱਭੋ_func(funcname, FALSE);
	ਜੇ (ufunc == ਨਿਰਾ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	++p;
	off = 0;
    }
    ਨਹੀਂਤਾਂ
	off = *funcname == 's' ? 2 : 5;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_ਵਿੱਚ_script_context));
	ਵਾਪਸ ਨਿਰਾ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, ਆਕਾਰ(sid_buf), "<SNR>%ld_",
	    (ਲੰਮਾ)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ਜੇ (newname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    vim_snprintf((ਅੱਖਰ *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ਵਾਪਸ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ਜੇ (n != ਨਿਰਾ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ਵਾਪਸ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	ਪੂਰਨ	    *is_global,
	ਪੂਰਨ	    skip,
	ਪੂਰਨ	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ਜੇ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(ਖਾਲੀ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ਜੇ (fudi != ਨਿਰਾ)
	    CLEAR_ਸੂਚਕ(fudi);
    }
    ਨਹੀਂਤਾਂ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    *name = p;
    ਵਾਪਸ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    ਖਾਲੀ
list_functions(regmatch_T *regmatch)
{
    ਪੂਰਨ		prev_ht_ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
    long_u	todo = func_hashtab.ht_ਵਰਤਿਆ;
    hashitem_T	*hi;

    ਲਈ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ਜੇ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ਨਿਰਾ
			? !ਸੁਨੇਹਾ_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !ਸੁਰੱਖਿਅਤ_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ਜੇ (list_func_ਸਿਰ(fp, FALSE) == FAIL)
		    ਵਾਪਸ;
		ਜੇ (function_list_modified(prev_ht_ਬਦਲਿਆ))
		    ਵਾਪਸ;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ਸਥਿਰ char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    ਜੇ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ਜੇ (regmatch.regprog != ਨਿਰਾ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ਜੇ (*p == '/')
	++p;

    ਵਾਪਸ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ਸਥਿਰ ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, ਪੂਰਨ is_global)
{
    ufunc_T	*fp = ਨਿਰਾ;
    ਪੂਰਨ		j;

    ਜੇ (!ਸਿਰੇ_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਸੈੱਟ_nextcmd(eap, p);

    ਜੇ (eap->nextcmd != ਨਿਰਾ)
	*p = NUL;

    ਜੇ (eap->skip || got_int)
	ਵਾਪਸ ਨਿਰਾ;

    fp = ਲੱਭੋ_func(name, is_global);
    ਜੇ (fp == ਨਿਰਾ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *ਉੱਪਰ = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ਜੇ (ਉੱਪਰ != ਨਿਰਾ)
	    fp = ਲੱਭੋ_func(ਉੱਪਰ, FALSE);
    }

    ਜੇ (fp == ਨਿਰਾ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    ਪੂਰਨ prev_ht_ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;

    ਜੇ (list_func_ਸਿਰ(fp, TRUE) != OK)
	ਵਾਪਸ fp;

    ਲਈ (j = 0; j < fp->uf_ਸਤਰਾਂ.ga_len && !got_int; ++j)
    {
	ਜੇ (FUNCLINE(fp, j) == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਸੁਨੇਹਾ_putchar('\n');
	ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)(j + 1));
	ਜੇ (j < 9)
	    ਸੁਨੇਹਾ_putchar(' ');
	ਜੇ (j < 99)
	    ਸੁਨੇਹਾ_putchar(' ');
	ਜੇ (function_list_modified(prev_ht_ਬਦਲਿਆ))
	    ਰੋਕੋ;
	ਸੁਨੇਹਾ_prt_ਸਤਰ(FUNCLINE(fp, j), FALSE);
	ਬਾਹਰਲਾ_flush();	// show a line at a time
	ui_breakcheck();
    }

    ਜੇ (!got_int)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (!function_list_modified(prev_ht_ਬਦਲਿਆ))
	{
	    ਜੇ (fp->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED)
		ਸੁਨੇਹਾ_puts("   enddef");
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts("   endfunction");
	}
    }

    ਵਾਪਸ fp;
}

/*
 * ":function" also supporting nested ":def".
 * When "name_arg" is not NULL this is a nested function, using "name_arg" for
 * the function name.
 * "lines_to_free" is a list of strings to be freed later.
 * If "class_flags" has CF_CLASS then the function is defined inside a class.
 * With CF_INTERFACE the function is define inside an interface, only the
 * ":def"/":function" line is expected, no function body.
 * Returns a pointer to the function or NULL if no function defined.
 */
    ufunc_T *
define_function(
	exarg_T	    *eap,
	char_u	    *name_arg,
	garray_T    *ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free,
	ਪੂਰਨ	    class_flags,
	ocmember_T  *obj_members,
	ਪੂਰਨ         obj_member_ਗਿਣਤੀ,
	cctx_T	    *cctx)
{
    ਪੂਰਨ		j;
    ਪੂਰਨ		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    ਪੂਰਨ		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*ਸਤਰ_arg = ਨਿਰਾ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_ਦਲੀਲਾਂ;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    ਪੂਰਨ		varargs = FALSE;
    ਪੂਰਨ		flags = 0;
    char_u	*ret_type = ਨਿਰਾ;
    ufunc_T	*fp = ਨਿਰਾ;
    ਪੂਰਨ		fp_allocated = FALSE;
    ਪੂਰਨ		free_fp = FALSE;
    ਪੂਰਨ		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ਸਥਿਰ ਪੂਰਨ	func_nr = 0;	    // number for nameless function
    ਪੂਰਨ		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_ਸਿਖਰ;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    imported_T	*import = ਨਿਰਾ;

    // ":function" without argument: list functions.
    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, eap->arg))
    {
	ਜੇ (!eap->skip)
	    list_functions(ਨਿਰਾ);
	ਸੈੱਟ_nextcmd(eap, eap->arg);
	ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ਜੇ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	ਸੈੱਟ_nextcmd(eap, p);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_ਦਲੀਲਾਂ);
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    /*
     * Get the function name.  There are these situations:
     * func	    normal function name, also when "class_flags" is non-zero
     *		    "name" == func, "fudi.fd_dict" == NULL
     * dict.func    new dictionary entry
     *		    "name" == NULL, "fudi.fd_dict" set,
     *		    "fudi.fd_di" == NULL, "fudi.fd_newkey" == func
     * dict.func    existing dict entry with a Funcref
     *		    "name" == func, "fudi.fd_dict" set,
     *		    "fudi.fd_di" set, "fudi.fd_newkey" == NULL
     * dict.func    existing dict entry that's not a Funcref
     *		    "name" == NULL, "fudi.fd_dict" set,
     *		    "fudi.fd_di" set, "fudi.fd_newkey" == NULL
     * s:func	    script-local function name
     * g:func	    global function name, same as "func"
     */
    p = eap->arg;
    ਜੇ (name_arg != ਨਿਰਾ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_ਖੇਤਰ(fudi);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (vim9script)
	{
	    ਜੇ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_ਵਿੱਚ_vim9_script_str), p);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, TRUE);
	    ਜੇ (*skipwhite(p) == '.' && vim_strchr(p, '(') != ਨਿਰਾ)
	    {
		semsg(_(e_cannot_define_dict_func_ਵਿੱਚ_vim9_script_str),
								     eap->arg);
		ਵਾਪਸ ਨਿਰਾ;
	    }

	    p = eap->arg;
	}

	ਪੂਰਨ tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_ਵਿੱਚ_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ਨਿਰਾ);
	ਜੇ (name == ਨਿਰਾ && (fudi.fd_dict == ਨਿਰਾ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ਜੇ (!aborting())
	    {
		ਜੇ (!eap->skip && fudi.fd_newkey != ਨਿਰਾ)
		    semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ਜੇ (is_export && name != ਨਿਰਾ)
	{
	    char_u *prefixed = may_ਅਗੇਤਰ_autoload(name);

	    ਜੇ (prefixed != ਨਿਰਾ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (paren && vim9script && name != ਨਿਰਾ
				    && vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ)
	{
	    emsg(_(e_cannot_use_name_with_hash_ਵਿੱਚ_vim9_script_use_export_instead));
	    ਜਾਓ ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ਜੇ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	ਜਾਓ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ਜੇ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ਜੇ (*p != '(')
    {
	ਜੇ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    ਜਾਓ ret_free;
	}
	// attempt to continue by skipping some text
	ਜੇ (vim_strchr(p, '(') != ਨਿਰਾ)
	    p = vim_strchr(p, '(');
    }

    ਜੇ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	ਜਾਓ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ਜੇ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	ਜਾਓ ret_free;
    }

    ga_init2(&newlines, ਆਕਾਰ(char_u *), 10);

    ਜੇ (!eap->skip && name_arg == ਨਿਰਾ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ਜੇ (name != ਨਿਰਾ)
	    arg = name;
	ਨਹੀਂਤਾਂ
	    arg = fudi.fd_newkey;
	ਜੇ (arg != ਨਿਰਾ && (fudi.fd_di == ਨਿਰਾ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_ਅਧਾਰ = arg;
	    ਪੂਰਨ	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ਜੇ (arg != fudi.fd_newkey)
	    {
		ਜੇ (*arg == K_SPECIAL)
		{
		    name_ਅਧਾਰ = vim_strchr(arg, '_');
		    ਜੇ (name_ਅਧਾਰ == ਨਿਰਾ)
			name_ਅਧਾਰ = arg + 3;
		    ਨਹੀਂਤਾਂ
			++name_ਅਧਾਰ;
		}
		ਲਈ (i = 0; name_ਅਧਾਰ[i] != NUL && (i == 0
					    ? eval_isnamec1(name_ਅਧਾਰ[i])
					    : eval_isnamec(name_ਅਧਾਰ[i])); ++i)
		    ;
		ਜੇ (name_ਅਧਾਰ[i] != NUL)
		{
		    emsg_funcname(e_invalid_ਦਲੀਲ_str, arg);
		    ਜਾਓ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ਜੇ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_ਅਧਾਰ, i, 0, ਨਿਰਾ,
			ਨਿਰਾ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_ਵਸਤੂ_str), name_ਅਧਾਰ);
		    ਜਾਓ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ਜੇ (fudi.fd_dict != ਨਿਰਾ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_ਇੱਥੇ));
	    ਜਾਓ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ਜੇ (get_function_ਦਲੀਲਾਂ(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ਨਿਰਾ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ਨਿਰਾ,
			 ਨਿਰਾ, &varargs, &default_ਦਲੀਲਾਂ, eap->skip,
			 eap, class_flags, &newlines, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free) == FAIL)
	ਜਾਓ errret_2;
    whitep = p;

    ਜੇ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ਜੇ (*skipwhite(p) == ':')
	{
	    ਜੇ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ਜੇ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ਨਿਰਾ;
	    }
	}
	p = skipwhite(p);
    }
    ਨਹੀਂਤਾਂ
	// find extra arguments "range", "dict", "abort" and "closure"
	ਲਈ (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ਜੇ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_ਘੇਰਾ;
		p += 7;
		ਜੇ (current_funccal == ਨਿਰਾ)
		{
		    emsg_funcname(e_ਘੇਰਾ_function_should_not_be_at_ਸਿਖਰ_level_str,
			    name == ਨਿਰਾ ? (char_u *)"" : name);
		    ਜਾਓ erret;
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ਜੇ (*p == '\n')
	ਸਤਰ_arg = p + 1;
    ਨਹੀਂਤਾਂ ਜੇ (*p != NUL
	    && !(*p == '"' && (!vim9script || eap->cmdidx == CMD_function)
						     && eap->cmdidx != CMD_def)
	    && !(VIM_ISWHITE(*whitep) && *p == '#'
				     && (vim9script || eap->cmdidx == CMD_def))
	    && !eap->skip
	    && !did_emsg)
	semsg(_(e_trailing_characters_str), p);

    /*
     * Read the body of the function, until "}", ":endfunction" or ":enddef" is
     * found.
     */
    ਜੇ (ਕੁੰਜੀ_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ਜੇ (!eap->skip && !eap->forceit)
	{
	    ਜੇ (fudi.fd_dict != ਨਿਰਾ && fudi.fd_newkey == ਨਿਰਾ)
		emsg(_(e_dictionary_ਇੰਦਰਾਜ_already_exists));
	    ਨਹੀਂਤਾਂ ਜੇ (name != ਨਿਰਾ && ਲੱਭੋ_func(name, is_global) != ਨਿਰਾ)
		emsg_funcname(e_function_str_already_exists_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_replace, name);
	}

	ਜੇ (!eap->skip && did_emsg)
	    ਜਾਓ erret;

	ਸੁਨੇਹਾ_putchar('\n');	    // don't overwrite the function name
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    }

    // Save the starting line number.
    sourcing_lnum_ਸਿਖਰ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ਜੇ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_ਵਿਧੀ) == 0
		&& (get_function_body(eap, &newlines, ਸਤਰ_arg, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free)
							== FAIL || did_emsg))
	    || eap->skip)
	ਜਾਓ erret;

    /*
     * If there are no errors, add the function
     */
    ਜੇ (fudi.fd_dict != ਨਿਰਾ)
    {
	ਅੱਖਰ	numbuf[NUMBUFLEN];

	fp = ਨਿਰਾ;
	ਜੇ (fudi.fd_newkey == ਨਿਰਾ && !eap->forceit)
	{
	    emsg(_(e_dictionary_ਇੰਦਰਾਜ_already_exists));
	    ਜਾਓ erret;
	}
	ਜੇ (fudi.fd_di == ਨਿਰਾ)
	{
	    // Can't add a function to a locked dictionary
	    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(fudi.fd_dict->dv_ਤਾਲਾ, eap->arg, FALSE))
		ਜਾਓ erret;
	}
	    // Can't change an existing function if it is locked
	ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(fudi.fd_di->di_tv.v_ਤਾਲਾ, eap->arg, FALSE))
	    ਜਾਓ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, ਆਕਾਰ(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ਜੇ (name == ਨਿਰਾ)
	    ਜਾਓ erret;
    }
    ਨਹੀਂਤਾਂ ਜੇ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*ਲੱਭੋ_name = name;
	ਪੂਰਨ		var_conflict = FALSE;
	ਪੂਰਨ		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = ਲੱਭੋ_var(name, &ht, TRUE);
	ਜੇ (v != ਨਿਰਾ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ਜੇ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);

	    ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	    {
		ਜੇ (is_export)
		{
		    ਲੱਭੋ_name = name + STRLEN(si->sn_autoload_ਅਗੇਤਰ);
		    v = ਲੱਭੋ_var(ਲੱਭੋ_name, &ht, TRUE);
		    ਜੇ (v != ਨਿਰਾ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ਨਹੀਂਤਾਂ
		{
		    char_u *prefixed = may_ਅਗੇਤਰ_autoload(name);

		    ਜੇ (prefixed != ਨਿਰਾ && prefixed != name)
		    {
			v = ਲੱਭੋ_var(prefixed, &ht, TRUE);
			ਜੇ (v != ਨਿਰਾ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ਜੇ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    ਜਾਓ erret;
	}

	fp = ਲੱਭੋ_func_even_dead(ਲੱਭੋ_name, ffed_flags);
	ਜੇ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = ਲੱਭੋ_imported(uname == ਨਿਰਾ ? name : uname, 0, FALSE);
	}

	ਜੇ (fp != ਨਿਰਾ || import != ਨਿਰਾ)
	{
	    ਪੂਰਨ dead = fp != ਨਿਰਾ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ਜੇ (import != ਨਿਰਾ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ;
		ਜੇ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ਨਹੀਂਤਾਂ
		    emsg_funcname(e_function_str_already_exists_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_replace, name);
		ਜਾਓ errret_keep;
	    }
	    ਜੇ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_ਵਿੱਚ_use, name);
		ਜਾਓ errret_keep;
	    }
	    ਜੇ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ਨਿਰਾ;
		overwrite = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ਨਿਰਾ;
		func_clear_ਚੀਜ਼ਾਂ(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#ਜੇ_ਅੰਤ
		fp->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;
	    }
	}
    }

    ਜੇ (fp == ਨਿਰਾ)
    {
	ਜੇ (fudi.fd_dict == ਨਿਰਾ && vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ)
	{
	    ਪੂਰਨ	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ਜੇ (SOURCING_NAME != ਨਿਰਾ)
	    {
		scriptname = autoload_name(name);
		ਜੇ (scriptname != ਨਿਰਾ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (ਪੂਰਨ)STRLEN(p);
		    slen = (ਪੂਰਨ)STRLEN(SOURCING_NAME);
		    ਜੇ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ਜੇ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ;
		semsg(_(e_function_name_does_not_ਮੇਲ_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		ਜਾਓ erret;
	    }
	}

	ਜੇ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ਜੇ (fp == ਨਿਰਾ)
	    ਜਾਓ erret;
	fp_allocated = TRUE;

	ਜੇ (fudi.fd_dict != ਨਿਰਾ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ਜੇ (func_name == ਨਿਰਾ)
	    {
		VIM_CLEAR(fp);
		ਜਾਓ erret;
	    }
	    ਜੇ (fudi.fd_di == ਨਿਰਾ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ਜੇ (fudi.fd_di == ਨਿਰਾ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ਜਾਓ erret;
		}
		ਜੇ (dict_ਜੋੜੋ(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ਜਾਓ erret;
		}
	    }
	    ਨਹੀਂਤਾਂ
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_ਦਲੀਲਾਂ = newargs;
    fp->uf_def_ਦਲੀਲਾਂ = default_ਦਲੀਲਾਂ;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    ਜੇ (eap->cmdidx == CMD_def)
    {
	ਪੂਰਨ	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_ਸਥਿਤੀ = UF_ਮੰਜ਼ਿਲ_BE_COMPILED;

	ਜੇ (generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_ਮਾਪ(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_ਸਿਖਰ;

	// The function may use script variables from the context.
	function_using_ਖੰਡ_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	ਪੂਰਨ save_is_export = is_export;
	is_export = FALSE;

	ਜੇ (parse_ਦਲੀਲ_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_ਗਿਣਤੀ, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ਜਾਓ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ਜੇ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ਜਾਓ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ਨਹੀਂਤਾਂ
	fp->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;

    ਜੇ (fp_allocated)
    {
	// insert the new function in the function list
	ਜੇ (overwrite)
	{
	    hi = hash_ਲੱਭੋ(&func_hashtab, name);
	    hi->hi_ਕੁੰਜੀ = UF2HIKEY(fp);
	}
	ਨਹੀਂਤਾਂ ਜੇ (class_flags == 0 && hash_ਜੋੜੋ(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    ਜਾਓ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_ਸਤਰਾਂ = newlines;
    newlines.ga_ਅੰਕੜੇ = ਨਿਰਾ;
    ਜੇ ((flags & FC_ਘੇਰਾ) != 0)
    {
	ਜੇ (ਰਜਿਸਟਰ_ਘੇਰਾ(fp) == FAIL)
	    ਜਾਓ erret;
    }
    ਨਹੀਂਤਾਂ
	fp->uf_scoped = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (prof_def_func())
	func_do_profile(fp);
#ਜੇ_ਅੰਤ
    fp->uf_varargs = varargs;
    ਜੇ (sandbox)
	flags |= FC_SANDBOX;
    ਜੇ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_ਸਿਖਰ;
    ਜੇ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ਜੇ (eap->cmdidx == CMD_def)
	ਸੈੱਟ_function_type(fp);
    ਨਹੀਂਤਾਂ ਜੇ (fp->uf_script_ctx.sc_ਸੰਸਕਰਣ == SCRIPT_ਸੰਸਕਰਣ_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ਜੇ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ਨਿਰਾ);

    ਜਾਓ ret_free;

erret:
    ਜੇ (fp != ਨਿਰਾ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	//ਹੇਠਾਂ
	ga_init(&fp->uf_ਦਲੀਲਾਂ);
	ga_init(&fp->uf_def_ਦਲੀਲਾਂ);
    }
errret_2:
    ਜੇ (fp != ਨਿਰਾ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ਜੇ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_ਚੀਜ਼ਾਂ(fp);
    }
    ਜੇ (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_ਲੜੀਆਂ(&newargs);
    ga_clear_ਲੜੀਆਂ(&default_ਦਲੀਲਾਂ);
    ga_clear_ਲੜੀਆਂ(&newlines);
ret_free:
    ga_clear_ਲੜੀਆਂ(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    ਜੇ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ਵਾਪਸ fp;
}

/*
 * ":function"
 */
    ਖਾਲੀ
ex_function(exarg_T *eap)
{
    garray_T ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free;

    ga_init2(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(char_u *), 50);
    (ਖਾਲੀ)define_function(eap, ਨਿਰਾ, &ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, 0, ਨਿਰਾ, 0, ਨਿਰਾ);
    ga_clear_ਲੜੀਆਂ(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);
}

    ਪੂਰਨ
get_func_arity(char_u *name, ਪੂਰਨ *required, ਪੂਰਨ *optional, ਪੂਰਨ *varargs)
{
    ufunc_T	*ufunc = ਨਿਰਾ;
    ਪੂਰਨ		argcount = 0;
    ਪੂਰਨ		min_argcount = 0;
    ਪੂਰਨ		idx;

    idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);
    ਜੇ (idx >= 0)
    {
	ਅੰਦਰੂਨੀ_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ਨਿਰਾ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ਜੇ (error == FCERR_NONE)
	    ufunc = ਲੱਭੋ_func(fname, FALSE);
	vim_free(tofree);

	ਜੇ (ufunc == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	argcount = ufunc->uf_ਦਲੀਲਾਂ.ga_len;
	min_argcount = ufunc->uf_ਦਲੀਲਾਂ.ga_len - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ਵਾਪਸ OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
ਲੱਭੋ_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    ਪੂਰਨ		is_global = FALSE;
    char_u	*bracket_ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u	*generic_func_name = name;

    ਜੇ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ਜੇ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(ਖਾਲੀ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ਨਹੀਂਤਾਂ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = ਲੱਭੋ_class_func(&arg);
	ਜੇ (ufunc != ਨਿਰਾ)
	    ਵਾਪਸ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, &ufunc);
	ਜੇ (ufunc != ਨਿਰਾ)
	{
	    vim_free(fname);
	    ਵਾਪਸ ufunc;
	}
	ਜੇ (fname != ਨਿਰਾ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_ਸ਼ੁਰੂ = arg;
	    ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&arg) == FAIL)
	    {
		vim_free(fname);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }
    ਜੇ (fname == ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (!ਸਿਰੇ_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ufunc = ਲੱਭੋ_func(fname, is_global);
    ਜੇ (ufunc == ਨਿਰਾ)
    {
	char_u *p = untrans_function_name(fname);

	ਜੇ (p != ਨਿਰਾ)
	    // Try again without making it script-local.
	    ufunc = ਲੱਭੋ_func(p, FALSE);

	ਜੇ (ufunc == ਨਿਰਾ)
	{
	    bracket_ਸ਼ੁਰੂ = generic_func_ਲੱਭੋ_open_bracket(fname);
	    ਜੇ (bracket_ਸ਼ੁਰੂ != ਨਿਰਾ)
	    {
		generic_func_name = fname;
		*bracket_ਸ਼ੁਰੂ = NUL;
		ufunc = ਲੱਭੋ_func(fname, FALSE);
		*bracket_ਸ਼ੁਰੂ = '<';
	    }
	}
    }

    ਜੇ (ufunc != ਨਿਰਾ)
    {
	// handle generic functions
	ਜੇ (bracket_ਸ਼ੁਰੂ == ਨਿਰਾ)
	    bracket_ਸ਼ੁਰੂ = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_ਸ਼ੁਰੂ);
    }

    vim_free(fname);

    ਜੇ (ufunc == ਨਿਰਾ)
	semsg(_(e_cannot_ਲੱਭੋ_function_str), name);
    ਵਾਪਸ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    ਖਾਲੀ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ਜੇ (func_needs_compiling(ufunc, compile_type))
	(ਖਾਲੀ)compile_def_function(ufunc, FALSE, compile_type, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ਨਿਰਾ ? cl->class_name.string : (char_u *)"",
				cl != ਨਿਰਾ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ਸਥਿਰ ਖਾਲੀ
defcompile_funcs_ਵਿੱਚ_script(ਖਾਲੀ)
{
    ਲੰਮਾ	todo = (ਲੰਮਾ)func_hashtab.ht_ਵਰਤਿਆ;
    ਪੂਰਨ		ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
    hashitem_T	*hi;

    ਲਈ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ਜੇ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_ਸਥਿਤੀ == UF_ਮੰਜ਼ਿਲ_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(ਖਾਲੀ)compile_def_function(ufunc, FALSE, CT_NONE, ਨਿਰਾ);

		ਜੇ (func_hashtab.ht_ਬਦਲਿਆ != ਬਦਲਿਆ)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (ਲੰਮਾ)func_hashtab.ht_ਵਰਤਿਆ;
		    ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
		    hi = func_hashtab.ht_array;
		    --hi;
		}
	    }
	}
    }
}

/*
 * :defcompile - compile all :def functions in the current script that need to
 * be compiled or the one specified by the argument.
 * Skips dead functions.  Doesn't do profiling.
 */
    ਖਾਲੀ
ex_defcompile(exarg_T *eap)
{
    ਜੇ (*eap->arg != NUL)
    {
	typval_T tv;

	ਜੇ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ਜੇ (cl != ਨਿਰਾ)
		defcompile_class(cl);
	}
	ਨਹੀਂਤਾਂ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = ਲੱਭੋ_func_by_name(eap->arg, &compile_type);
	    ਜੇ (ufunc != ਨਿਰਾ)
		defcompile_function(ufunc, ਨਿਰਾ);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	defcompile_funcs_ਵਿੱਚ_script();

	// compile all the class defined in the current script
	defcompile_classes_ਵਿੱਚ_script();
    }
}

/*
 * Return 5 if "p" starts with "<SID>" or "<SNR>" (ignoring case).
 * Return 2 if "p" starts with "s:".
 * Return 0 otherwise.
 */
    ਪੂਰਨ
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ਜੇ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ਵਾਪਸ 5;
    ਜੇ (p[0] == 's' && p[1] == ':')
	ਵਾਪਸ 2;
    ਵਾਪਸ 0;
}

    ਪੂਰਨ
translated_function_exists(char_u *name, ਪੂਰਨ is_global)
{
    ਜੇ (builtin_function(name, -1))
	ਵਾਪਸ has_ਅੰਦਰੂਨੀ_func(name);
    ਵਾਪਸ ਲੱਭੋ_func(name, is_global) != ਨਿਰਾ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    ਪੂਰਨ
has_varargs(ufunc_T *ufunc)
{
    ਵਾਪਸ ufunc->uf_varargs || ufunc->uf_va_name != ਨਿਰਾ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    ਪੂਰਨ
function_exists(char_u *name, ਪੂਰਨ no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    ਪੂਰਨ	    n = FALSE;
    ਪੂਰਨ	    flag;
    ਪੂਰਨ	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ਜੇ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ਜੇ (p != ਨਿਰਾ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ਵਾਪਸ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    ਪੂਰਨ
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    ਪੂਰਨ		is_global = FALSE;
    ufunc_T	*fp = ਨਿਰਾ;
    list_T	*ਦਲੀਲਾਂ;
    ਪੂਰਨ		is_def;
    ਅੱਖਰ	*tofree;

    ਜੇ (*name == NUL)
	ਵਾਪਸ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ਜੇ (p != ਨਿਰਾ && *skipwhite(nm) == NUL)
	fp = ਲੱਭੋ_func(p, is_global);
    vim_free(p);
    ਜੇ (fp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    is_def = fp->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED;
    dict_ਜੋੜੋ_string(d, "name", printable_func_name(fp));
    dict_ਜੋੜੋ_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_ਜੋੜੋ_bool(d, "available", TRUE);
    ਦਲੀਲਾਂ = list_alloc();
    ਜੇ (ਦਲੀਲਾਂ == ਨਿਰਾ || dict_ਜੋੜੋ_list(d, "args", ਦਲੀਲਾਂ) == FAIL)
    {
	list_unref(ਦਲੀਲਾਂ);
	ਵਾਪਸ FAIL;
    }
    ਲਈ (ਪੂਰਨ j = 0; j < fp->uf_ਦਲੀਲਾਂ.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ਜੇ (arg == ਨਿਰਾ || list_append_dict(ਦਲੀਲਾਂ, arg) == FAIL)
	{
	    dict_unref(arg);
	    ਵਾਪਸ FAIL;
	}
	dict_ਜੋੜੋ_string(arg, "name", FUNCARG(fp, j));
	ਜੇ (fp->uf_arg_types == ਨਿਰਾ)
	    dict_ਜੋੜੋ_string(arg, "type", (char_u *)"any");
	ਨਹੀਂਤਾਂ
	{
	    dict_ਜੋੜੋ_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ਜੇ (j >= fp->uf_ਦਲੀਲਾਂ.ga_len - fp->uf_def_ਦਲੀਲਾਂ.ga_len)
	    dict_ਜੋੜੋ_string(arg, "default", ((char_u **)fp->uf_def_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ)
			     [j - fp->uf_ਦਲੀਲਾਂ.ga_len + fp->uf_def_ਦਲੀਲਾਂ.ga_len]);
    }
    ਜੇ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ਜੇ (va == ਨਿਰਾ || dict_ਜੋੜੋ_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ਵਾਪਸ FAIL;
	}
	dict_ਜੋੜੋ_string(va, "name",
			 fp->uf_va_name == ਨਿਰਾ ? (char_u *)"" : fp->uf_va_name);
	ਜੇ (fp->uf_va_type == ਨਿਰਾ)
	    dict_ਜੋੜੋ_string(va, "type", (char_u *)"list<any>");
	ਨਹੀਂਤਾਂ
	{
	    dict_ਜੋੜੋ_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ਜੇ (is_def)
    {
	dict_ਜੋੜੋ_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_string(d, "returns", (char_u *)"any");
    dict_ਜੋੜੋ_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_ਜੋੜੋ_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_ਜੋੜੋ_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_ਜੋੜੋ_bool(d, "closure", (fp->uf_flags & FC_ਘੇਰਾ) != 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, ਪੂਰਨ check)
{
    char_u	*nm = name;
    char_u	*p;
    ਪੂਰਨ		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ਜੇ (p != ਨਿਰਾ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ਵਾਪਸ p;

    vim_free(p);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਸਥਿਰ long_u	ਪੂਰਾ;
    ਸਥਿਰ ਪੂਰਨ		ਬਦਲਿਆ;
    ਸਥਿਰ hashitem_T	*hi;
    ufunc_T		*fp;

    ਜੇ (idx == 0)
    {
	ਪੂਰਾ = 0;
	hi = func_hashtab.ht_array;
	ਬਦਲਿਆ = func_hashtab.ht_ਬਦਲਿਆ;
    }
    ਜੇ (ਬਦਲਿਆ == func_hashtab.ht_ਬਦਲਿਆ && ਪੂਰਾ < func_hashtab.ht_ਵਰਤਿਆ)
    {
	ਪੂਰਨ len;

	ਜੇ (ਪੂਰਾ++ > 0)
	    ++hi;
	ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ਜੇ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ਵਾਪਸ (char_u *)"";

	ਜੇ (fp->uf_namelen + 4 >= IOSIZE)
	    ਵਾਪਸ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ਜੇ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ਜੇ (!has_varargs(fp) && fp->uf_ਦਲੀਲਾਂ.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ਵਾਪਸ IObuff;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, ਪੂਰਨ extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ਜੇ (ufunc == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_ਸਥਿਤੀ = UF_ਮੰਜ਼ਿਲ_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_ਗਿਣਤੀ = ਨਿਰਾ;
    ufunc->uf_tml_ਕੁੱਲ = ਨਿਰਾ;
    ufunc->uf_tml_self = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ga_copy_ਲੜੀਆਂ(&fp->uf_ਦਲੀਲਾਂ, &ufunc->uf_ਦਲੀਲਾਂ);
    ga_copy_ਲੜੀਆਂ(&fp->uf_def_ਦਲੀਲਾਂ, &ufunc->uf_def_ਦਲੀਲਾਂ);

    ਜੇ (ufunc->uf_arg_types != ਨਿਰਾ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_ਦਲੀਲਾਂ.ga_len);
	ਜੇ (at != ਨਿਰਾ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     ਆਕਾਰ(type_T *) * ufunc->uf_ਦਲੀਲਾਂ.ga_len);
	    ufunc->uf_arg_types = at;
	}
    }

    // TODO: how about the types themselves? they can be freed when the
    // original function is freed:
    //    type_T	**uf_arg_types;
    //    type_T	*uf_ret_type;

    // make uf_type_list empty
    ga_init(&ufunc->uf_type_list);

    // TODO:   partial_T	*uf_partial;

    // copy generic function related state
    copy_generic_function(fp, ufunc);

    ਜੇ (ufunc->uf_va_name != ਨਿਰਾ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ = 0;
    ufunc->uf_ਖੰਡ_ids = ਨਿਰਾ;

    ga_copy_ਲੜੀਆਂ(&fp->uf_ਸਤਰਾਂ, &ufunc->uf_ਸਤਰਾਂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ਜੇ (fp->uf_profiling)
	func_do_profile(ufunc);
#ਜੇ_ਅੰਤ

    ufunc->uf_refcount = 1;

    ਵਾਪਸ ufunc;
}

/*
 * ":delfunction {name}"
 */
    ਖਾਲੀ
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ਨਿਰਾ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    ਪੂਰਨ		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    vim_free(fudi.fd_newkey);
    ਜੇ (name == ਨਿਰਾ)
    {
	ਜੇ (fudi.fd_dict != ਨਿਰਾ && !eap->skip)
	    emsg(_(e_funcref_required));
	ਵਾਪਸ;
    }
    ਜੇ (!ਸਿਰੇ_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ਵਾਪਸ;
    }
    ਸੈੱਟ_nextcmd(eap, p);
    ਜੇ (eap->nextcmd != ਨਿਰਾ)
	*p = NUL;

    ਜੇ (numbered_function(name) && fudi.fd_dict == ਨਿਰਾ)
    {
	ਜੇ (!eap->skip)
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	vim_free(name);
	ਵਾਪਸ;
    }
    ਜੇ (!eap->skip)
	fp = ਲੱਭੋ_func(name, is_global);
    vim_free(name);

    ਜੇ (!eap->skip)
    {
	ਜੇ (fp == ਨਿਰਾ)
	{
	    ਜੇ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ਵਾਪਸ;
	}
	ਜੇ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_ਵਿੱਚ_use), eap->arg);
	    ਵਾਪਸ;
	}
	ਜੇ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ਵਾਪਸ;
	}

	ਜੇ (fudi.fd_dict != ਨਿਰਾ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ਨਹੀਂਤਾਂ
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ਜੇ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ਜੇ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    ਨਹੀਂਤਾਂ
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    ਖਾਲੀ
func_unref(char_u *name)
{
    ufunc_T *fp = ਨਿਰਾ;

    ਜੇ (name == ਨਿਰਾ || !func_name_refcount(name))
	ਵਾਪਸ;
    fp = ਲੱਭੋ_func(name, FALSE);
    ਜੇ (fp == ਨਿਰਾ && numbered_function(name))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EXITFREE
	ਜੇ (!entered_free_all_mem)
#ਜੇ_ਅੰਤ
	    ਅੰਦਰੂਨੀ_error("func_unref()");
    }
    func_ptr_unref(fp);
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 * Also when it becomes one and uf_partial points to the function.
 */
    ਖਾਲੀ
func_ptr_unref(ufunc_T *fp)
{
    ਜੇ (fp != ਨਿਰਾ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ਨਿਰਾ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ਜੇ (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    ਖਾਲੀ
func_ref(char_u *name)
{
    ufunc_T *fp;

    ਜੇ (name == ਨਿਰਾ || !func_name_refcount(name))
	ਵਾਪਸ;
    fp = ਲੱਭੋ_func(name, FALSE);
    ਜੇ (fp != ਨਿਰਾ)
	++fp->uf_refcount;
    ਨਹੀਂਤਾਂ ਜੇ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	ਅੰਦਰੂਨੀ_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    ਖਾਲੀ
func_ptr_ref(ufunc_T *fp)
{
    ਜੇ (fp != ਨਿਰਾ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ਸਥਿਰ ਪੂਰਨ
can_free_funccal(funccall_T *fc, ਪੂਰਨ copyID)
{
    ਵਾਪਸ (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    ਖਾਲੀ
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ਪੂਰਨ		returning = FALSE;
    evalarg_T	evalarg;

    ਜੇ (current_funccal == ਨਿਰਾ)
    {
	emsg(_(e_return_not_inside_function));
	ਵਾਪਸ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ਜੇ (eap->skip)
	++emsg_skip;

    eap->nextcmd = ਨਿਰਾ;
    ਜੇ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ਜੇ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ਨਹੀਂਤਾਂ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ਨਹੀਂਤਾਂ ਜੇ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ਜੇ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ਨਿਰਾ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ਜੇ (returning)
	eap->nextcmd = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (eap->nextcmd == ਨਿਰਾ)	    // no argument
	ਸੈੱਟ_nextcmd(eap, arg);

    ਜੇ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ਸਥਿਰ ਪੂਰਨ
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    ਪੂਰਨ		doesrange;
    typval_T	rettv;
    ਪੂਰਨ		failed = FALSE;

    lnum = eap->line1;
    ਲਈ ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	{
	    ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ਰੋਕੋ;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ਜੇ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    ਰੋਕੋ;
	}
	ਜੇ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ਜੇ (handle_subscript(arg, ਨਿਰਾ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ਰੋਕੋ;
	}

	clear_tv(&rettv);
	ਜੇ (doesrange)
	    ਰੋਕੋ;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ਜੇ (aborting())
	    ਰੋਕੋ;
    }
    ਵਾਪਸ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ਸਥਿਰ ਪੂਰਨ
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ਦਲੀਲਾਂ + 1];	// vars for arguments
    ਪੂਰਨ		partial_argc = 0;		// number of partial arguments
    ਪੂਰਨ		argcount = 0;			// number of arguments found
    ਪੂਰਨ		r;

    ਜੇ (current_funccal == ਨਿਰਾ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ਵਾਪਸ FAIL;
    }
    ਜੇ (partial != ਨਿਰਾ)
    {
	ਜੇ (partial->pt_dict != ਨਿਰਾ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (partial->pt_argc > 0)
	{
	    ਪੂਰਨ i;

	    partial_argc = partial->pt_argc;
	    ਲਈ (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    ਪੂਰਨ is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ਜੇ (r == OK)
    {
	ਜੇ (type != ਨਿਰਾ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_ਦਲੀਲ_types(type, argvars, argcount, ਨਿਰਾ, name);
	}
	ਨਹੀਂਤਾਂ ਜੇ (is_builtin)
	{
	    ਪੂਰਨ idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);

	    ਜੇ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (check_ਅੰਦਰੂਨੀ_func(idx, argcount) == -1)
		r = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ufunc_T *ufunc = ਲੱਭੋ_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ਜੇ (ufunc != ਨਿਰਾ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ਜੇ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ਜੇ (r == FAIL)
    {
	ਜਦੋਂ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ ਜੋੜੋ_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    ਪੂਰਨ
can_ਜੋੜੋ_defer(ਖਾਲੀ)
{
    ਜੇ (!ਵਿੱਚ_def_function() && get_current_funccal() == ਨਿਰਾ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
ਜੋੜੋ_defer(char_u *name, ਪੂਰਨ argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    ਪੂਰਨ		argcount = argcount_arg;
    defer_T	*dr;
    ਪੂਰਨ		ret = FAIL;

    ਜੇ (saved_name == ਨਿਰਾ)
	ਜਾਓ theend;
    ਜੇ (ਵਿੱਚ_def_function())
    {
	ਜੇ (ਜੋੜੋ_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, ਆਕਾਰ(defer_T), 10);
	ਜੇ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    ਜਾਓ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_ਅੰਕੜੇ)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ਜਦੋਂ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ਜਦੋਂ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ਵਾਪਸ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_defer_one(funccall_T *funccal)
{
    ਪੂਰਨ	    idx;

    ਲਈ (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_ਅੰਕੜੇ) + idx;

	ਜੇ (dr->dr_name == ਨਿਰਾ)
	    // already being called, can happen if function does ":qa"
	    ਜਾਰੀ;

	funcexe_T   funcexe;
	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ਨਿਰਾ;

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	call_func(name, -1, &rettv, dr->dr_argcount, dr->dr_argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	vim_free(name);
	ਲਈ (ਪੂਰਨ i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ਸਥਿਰ ਖਾਲੀ
invoke_funccall_defer(funccall_T *fc)
{
    ਜੇ (fc->fc_ectx != ਨਿਰਾ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ਨਹੀਂਤਾਂ
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    ਖਾਲੀ
invoke_all_defer(ਖਾਲੀ)
{
    ਲਈ (funccall_T *fc = current_funccal; fc != ਨਿਰਾ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    ਲਈ (funccal_ਇੰਦਰਾਜ_T *fce = funccal_ਢੇਰ; fce != ਨਿਰਾ; fce = fce->ਅਗਲਾ)
	ਲਈ (funccall_T *fc = fce->ਸਿਖਰ_funccal; fc != ਨਿਰਾ; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    ਖਾਲੀ
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    ਪੂਰਨ		len;
    ਪੂਰਨ		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ਨਿਰਾ;
    partial_T	*partial = ਨਿਰਾ;
    evalarg_T	evalarg;
    type_T	*type = ਨਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ_var = FALSE;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ਜੇ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ਜੇ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ਵਾਪਸ;
    }

    tofree = trans_function_name_ext(&arg, ਨਿਰਾ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ਨਿਰਾ, &ufunc);
    ਜੇ (fudi.fd_newkey != ਨਿਰਾ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ਜੇ (tofree == ਨਿਰਾ)
	ਵਾਪਸ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ਜੇ (fudi.fd_dict != ਨਿਰਾ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (ਪੂਰਨ)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ਨਿਰਾ ? ਨਿਰਾ : &partial,
				vim9script && type == ਨਿਰਾ ? &type : ਨਿਰਾ,
						     FALSE, FALSE, &ਮਿਲਿਆ_var);

    ਜੇ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_ਦਲੀਲਾਂ(name, STRLEN(name), &arg);
	ਜੇ (name == ਨਿਰਾ)
	    ਜਾਓ ਸਿਰਾ;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ਜੇ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	ਜਾਓ ਸਿਰਾ;
    }
    ਜੇ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	ਜਾਓ ਸਿਰਾ;
    }

    ਜੇ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	funcexe_T funcexe;

	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_ਮਿਲਿਆ_var = ਮਿਲਿਆ_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    ਜੇ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ਜੇ (!ਸਿਰੇ_excmd2(eap->arg, arg))
	{
	    ਜੇ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

ਸਿਰਾ:
    dict_unref(fudi.fd_dict);
    vim_free(tofree);
}

/*
 * Return from a function.  Possibly makes the return pending.  Also called
 * for a pending return at the ":endtry" or after returning from an extra
 * do_cmdline().  "reanimate" is used in the latter case.  "is_cmd" is set
 * when called due to a ":return" command.  "rettv" may point to a typval_T
 * with the return rettv.  Returns TRUE when the return can be carried out,
 * FALSE when the return gets pending.
 */
    ਪੂਰਨ
do_return(
    exarg_T	*eap,
    ਪੂਰਨ		reanimate,
    ਪੂਰਨ		is_cmd,
    ਖਾਲੀ	*rettv)
{
    ਪੂਰਨ		idx;
    cstack_T	*cstack = eap->cstack;

    ਜੇ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ਜੇ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ਜੇ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	ਨਹੀਂਤਾਂ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ਜੇ (reanimate)
		rettv = current_funccal->fc_rettv;

	    ਜੇ (rettv != ਨਿਰਾ)
	    {
		// Store the value of the pending return.
		ਜੇ ((cstack->cs_rettv[idx] = alloc_tv()) != ਨਿਰਾ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ਨਹੀਂਤਾਂ
		    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
	    }
	    ਨਹੀਂਤਾਂ
		cstack->cs_rettv[idx] = ਨਿਰਾ;

	    ਜੇ (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_ਸੰਖਿਆ;
		current_funccal->fc_rettv->vval.v_ਸੰਖਿਆ = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    ਨਹੀਂਤਾਂ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ਜੇ (!reanimate && rettv != ਨਿਰਾ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ਜੇ (!is_cmd)
		vim_free(rettv);
	}
    }

    ਵਾਪਸ idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    ਖਾਲੀ
discard_pending_return(ਖਾਲੀ *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(ਖਾਲੀ *rettv)
{
    char_u	*s = ਨਿਰਾ;
    char_u	*tofree = ਨਿਰਾ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ਜੇ (rettv != ਨਿਰਾ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ਜੇ (s == ਨਿਰਾ)
	s = (char_u *)"";
    ਨਹੀਂਤਾਂ
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ਜੇ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ਵਾਪਸ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_ਸਤਰ(
    ਪੂਰਨ	    c ਅਣਵਰਤਿਆ,
    ਖਾਲੀ    *cookie,
    ਪੂਰਨ	    indent ਅਣਵਰਤਿਆ,
    getline_opt_T options ਅਣਵਰਤਿਆ)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ਜੇ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	func_ਸਤਰ_ਸਿਰਾ(cookie);
#ਜੇ_ਅੰਤ

    gap = &fp->uf_ਸਤਰਾਂ;
    ਜੇ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_ਵਿੱਚ_try())
	    || fcp->fc_returned)
	retval = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	// Skip NULL lines (continuation lines).
	ਜਦੋਂ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_ਅੰਕੜੇ))[fcp->fc_linenr] == ਨਿਰਾ)
	    ++fcp->fc_linenr;
	ਜੇ (fcp->fc_linenr >= gap->ga_len)
	    retval = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_ਅੰਕੜੇ))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    ਜੇ (do_profiling == PROF_YES)
		func_ਸਤਰ_ਸ਼ੁਰੂ(cookie, SOURCING_LNUM);
#ਜੇ_ਅੰਤ
	}
    }

    // Did we encounter a breakpoint?
    ਜੇ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ਵਾਪਸ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    ਪੂਰਨ
func_has_ended(ਖਾਲੀ *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ਵਾਪਸ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_ਵਿੱਚ_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    ਪੂਰਨ
func_has_abort(
    ਖਾਲੀ    *cookie)
{
    ਵਾਪਸ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_ਵਿੱਚ, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = ਨਿਰਾ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_ਵਿੱਚ;

    ਜੇ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ਨਿਰਾ
				     && rettv->vval.v_partial->pt_func != ਨਿਰਾ)
	fp = rettv->vval.v_partial->pt_func;
    ਨਹੀਂਤਾਂ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ਨਿਰਾ ? ਨਿਰਾ
					      : rettv->vval.v_partial->pt_name;
	ਜੇ (fname == ਨਿਰਾ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u	*tofree = ਨਿਰਾ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = ਲੱਭੋ_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ਜੇ (fp != ਨਿਰਾ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ਜੇ (pt != ਨਿਰਾ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ਨਿਰਾ;
	    ਜੇ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		ਪੂਰਨ		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ਜੇ (ret_pt->pt_name != ਨਿਰਾ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ਨਹੀਂਤਾਂ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ਜੇ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ਜੇ (pt->pt_argv == ਨਿਰਾ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ਨਹੀਂਤਾਂ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			ਲਈ (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ਵਾਪਸ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    ਪੂਰਨ *
func_dbg_tick(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    ਪੂਰਨ
func_level(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    ਪੂਰਨ
current_func_returned(ਖਾਲੀ)
{
    ਵਾਪਸ current_funccal->fc_returned;
}

    ਪੂਰਨ
free_unref_funccal(ਪੂਰਨ copyID, ਪੂਰਨ testing)
{
    ਪੂਰਨ		did_free = FALSE;
    ਪੂਰਨ		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    ਲਈ (pfc = &ਪਿਛਲਾ_funccal; *pfc != ਨਿਰਾ; )
    {
	ਜੇ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    pfc = &(*pfc)->fc_caller;
    }
    ਜੇ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(ਖਾਲੀ)garbage_collect(testing);

    ਵਾਪਸ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ਸਥਿਰ funccall_T *
get_funccal(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    funccall_T	*funccal;
    funccall_T	*ਆਰਜ਼ੀ_funccal;

    funccal = current_funccal;
    ਜੇ (debug_backtrace_level > 0)
    {
	ਲਈ (i = 0; i < debug_backtrace_level; i++)
	{
	    ਆਰਜ਼ੀ_funccal = funccal->fc_caller;
	    ਜੇ (ਆਰਜ਼ੀ_funccal)
		funccal = ਆਰਜ਼ੀ_funccal;
	    ਨਹੀਂਤਾਂ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ਵਾਪਸ funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ਸਥਿਰ funccall_T *
get_funccal_for_vars(ਖਾਲੀ)
{
    funccall_T	*funccal = ਨਿਰਾ;

    ਜੇ (current_funccal == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    funccal = get_funccal();
    ਜੇ (funccal == ਨਿਰਾ || funccal->fc_l_vars.dv_refcount == 0)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(ਖਾਲੀ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ਵਾਪਸ funccal == ਨਿਰਾ ? ਨਿਰਾ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(ਖਾਲੀ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ਵਾਪਸ funccal == ਨਿਰਾ ? ਨਿਰਾ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_ਦਲੀਲਾਂ_ht(ਖਾਲੀ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ਵਾਪਸ funccal == ਨਿਰਾ ? ਨਿਰਾ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_ਦਲੀਲਾਂ_var(ਖਾਲੀ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ਵਾਪਸ funccal == ਨਿਰਾ ? ਨਿਰਾ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    ਖਾਲੀ
list_func_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    ਜੇ (current_funccal != ਨਿਰਾ && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, ਪਹਿਲਾ);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    ਜੇ (current_funccal != ਨਿਰਾ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ਵਾਪਸ &current_funccal->fc_l_vars;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
ਲੱਭੋ_hi_ਵਿੱਚ_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ਨਿਰਾ;
    char_u	*varname;

    ਜੇ (current_funccal == ਨਿਰਾ || current_funccal->fc_func->uf_scoped == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ਜਦੋਂ (current_funccal != ਨਿਰਾ)
    {
	ht = ਲੱਭੋ_var_ht(name, &varname);
	ਜੇ (ht != ਨਿਰਾ && *varname != NUL)
	{
	    hi = hash_ਲੱਭੋ(ht, varname);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    {
		*pht = ht;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ਰੋਕੋ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ਵਾਪਸ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
ਲੱਭੋ_var_ਵਿੱਚ_scoped_ht(char_u *name, ਪੂਰਨ no_autoload)
{
    dictitem_T	*v = ਨਿਰਾ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ਜੇ (current_funccal == ਨਿਰਾ || current_funccal->fc_func->uf_scoped == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ਜਦੋਂ (current_funccal)
    {
	ht = ਲੱਭੋ_var_ht(name, &varname);
	ਜੇ (ht != ਨਿਰਾ && *varname != NUL)
	{
	    v = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, *name, varname, no_autoload);
	    ਜੇ (v != ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਜੇ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ਰੋਕੋ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ਵਾਪਸ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਪਿਛਲਾ_funccal(ਪੂਰਨ copyID)
{
    funccall_T	*fc;

    ਲਈ (fc = ਪਿਛਲਾ_funccal; fc != ਨਿਰਾ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ਨਿਰਾ, ਨਿਰਾ)
		|| ਸੈੱਟ_ref_ਵਿੱਚ_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ਨਿਰਾ, ਨਿਰਾ)
		|| ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(&fc->fc_l_varlist, copyID + 1, ਨਿਰਾ, ਨਿਰਾ))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_funccal(funccall_T *fc, ਪੂਰਨ copyID)
{
    ਜੇ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_ht(&fc->fc_l_vars.dv_hashtab, copyID, ਨਿਰਾ, ਨਿਰਾ)
		|| ਸੈੱਟ_ref_ਵਿੱਚ_ht(&fc->fc_l_avars.dv_hashtab, copyID, ਨਿਰਾ, ਨਿਰਾ)
		|| ਸੈੱਟ_ref_ਵਿੱਚ_list_ਚੀਜ਼ਾਂ(&fc->fc_l_varlist, copyID, ਨਿਰਾ, ਨਿਰਾ)
		|| ਸੈੱਟ_ref_ਵਿੱਚ_func(ਨਿਰਾ, fc->fc_func, copyID))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_call_ਢੇਰ(ਪੂਰਨ copyID)
{
    funccall_T		*fc;
    funccal_ਇੰਦਰਾਜ_T	*ਇੰਦਰਾਜ;

    ਲਈ (fc = current_funccal; fc != ਨਿਰਾ; fc = fc->fc_caller)
	ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_funccal(fc, copyID))
	    ਵਾਪਸ TRUE;

    // Also go through the funccal_stack.
    ਲਈ (ਇੰਦਰਾਜ = funccal_ਢੇਰ; ਇੰਦਰਾਜ != ਨਿਰਾ; ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->ਅਗਲਾ)
	ਲਈ (fc = ਇੰਦਰਾਜ->ਸਿਖਰ_funccal; fc != ਨਿਰਾ; fc = fc->fc_caller)
	    ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_funccal(fc, copyID))
		ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_functions(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		todo;
    hashitem_T	*hi = ਨਿਰਾ;
    ufunc_T	*fp;

    todo = (ਪੂਰਨ)func_hashtab.ht_ਵਰਤਿਆ;
    ਲਈ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ਜੇ (!func_name_refcount(fp->uf_name)
					  && ਸੈੱਟ_ref_ਵਿੱਚ_func(ਨਿਰਾ, fp, copyID))
		ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_func_ਦਲੀਲਾਂ(ਪੂਰਨ copyID)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < funcargs.ga_len; ++i)
	ਜੇ (ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(((typval_T **)funcargs.ga_ਅੰਕੜੇ)[i],
						copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_func(char_u *name, ufunc_T *fp_ਵਿੱਚ, ਪੂਰਨ copyID)
{
    ufunc_T	*fp = fp_ਵਿੱਚ;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ਨਿਰਾ;
    char_u	*fname;
    ਪੂਰਨ		abort = FALSE;

    ਜੇ (name == ਨਿਰਾ && fp_ਵਿੱਚ == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (fp_ਵਿੱਚ == ਨਿਰਾ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = ਲੱਭੋ_func(fname, FALSE);
    }
    ਜੇ (fp != ਨਿਰਾ)
    {
	ਲਈ (fc = fp->uf_scoped; fc != ਨਿਰਾ; fc = fc->fc_func->uf_scoped)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_funccal(fc, copyID);
    }

    vim_free(tofree);
    ਵਾਪਸ abort;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
