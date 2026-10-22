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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
സ്ഥിതം hashtab_T	func_hashtab;

// Used by get_func_tv()
സ്ഥിതം garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
സ്ഥിതം funccall_T *current_funccal = ശൂന്യ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
സ്ഥിതം funccall_T *previous_funccal = ശൂന്യ;

സ്ഥിതം ശൂന്യം funccal_unref(funccall_T *fc, ufunc_T *fp, പൂർണ്ണസംഖ്യ force);
സ്ഥിതം ശൂന്യം func_clear(ufunc_T *fp, പൂർണ്ണസംഖ്യ force);
സ്ഥിതം പൂർണ്ണസംഖ്യ func_free(ufunc_T *fp, പൂർണ്ണസംഖ്യ force);
സ്ഥിതം char_u *untrans_function_name(char_u *name);
സ്ഥിതം ശൂന്യം handle_defer_one(funccall_T *funccal);
സ്ഥിതം char_u *trans_function_name_ext(char_u **pp, പൂർണ്ണസംഖ്യ *is_global, പൂർണ്ണസംഖ്യ skip, പൂർണ്ണസംഖ്യ flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    ശൂന്യം
func_init(ശൂന്യം)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(ശൂന്യം)
{
    തിരികെ &func_hashtab;
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
    സ്ഥിതം char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	പൂർണ്ണസംഖ്യ	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	പൂർണ്ണസംഖ്യ	    is_vararg,
	പൂർണ്ണസംഖ്യ	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		is_underscore = FALSE;

    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    എങ്കിൽ (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == ശൂന്യ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	എങ്കിൽ (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	തിരികെ arg;
    }

    // Extra checks in Vim9 script.
    എങ്കിൽ (!skip && argtypes != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ c = *p;
	*p = NUL;
	പൂർണ്ണസംഖ്യ r = check_reserved_name(arg, FALSE);
	*p = c;
	എങ്കിൽ (r == FAIL)
	    തിരികെ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	എങ്കിൽ (check_defined(arg, p - arg,
				   evalarg == ശൂന്യ ? ശൂന്യ : evalarg->eval_cctx,
			       eap == ശൂന്യ ? ശൂന്യ : eap->cstack, TRUE) == FAIL)
	    തിരികെ arg;
    }

    എങ്കിൽ (newargs != ശൂന്യ && ga_grow(newargs, 1) == FAIL)
	തിരികെ arg;
    എങ്കിൽ (newargs != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	c;
	പൂർണ്ണസംഖ്യ	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	എങ്കിൽ (arg_copy == ശൂന്യ)
	{
	    *p = c;
	    തിരികെ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	എങ്കിൽ (argtypes == ശൂന്യ || !is_underscore)
	    // Check for duplicate argument name.
	    വേണ്ടി (i = 0; i < newargs->ga_len; ++i)
		എങ്കിൽ (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    തിരികെ arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    എങ്കിൽ (argtypes != ശൂന്യ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ശൂന്യ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ശൂന്യ;

	എങ്കിൽ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ശൂന്യ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	എങ്കിൽ (*p == ':')
	{
	    ++p;
	    എങ്കിൽ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		തിരികെ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    എങ്കിൽ (!skip)
		type = vim_strnsave(type, p - type);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == ശൂന്യ ? arg : arg_copy);
	    തിരികെ arg;
	}
	എങ്കിൽ (!skip)
	{
	    എങ്കിൽ (type == ശൂന്യ && types_optional)
	    {
		// lambda arguments default to "any" type
		എങ്കിൽ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		അല്ലെങ്കിൽ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    തിരികെ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    സ്ഥിതം char_u *
get_function_വരി(
	exarg_T		*eap,
	garray_T	*വരികൾ_to_free,
	പൂർണ്ണസംഖ്യ		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    എങ്കിൽ (eap->ea_getline == ശൂന്യ)
	theline = getcmdline(':', 0L, indent, 0);
    അല്ലെങ്കിൽ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    എങ്കിൽ (theline != ശൂന്യ)
    {
	എങ്കിൽ (വരികൾ_to_free->ga_len > 0
		&& eap->cmdlinep != ശൂന്യ
		&& *eap->cmdlinep == ((char_u **)വരികൾ_to_free->ga_data)
						   [വരികൾ_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(ശൂന്യം)ga_add_string(വരികൾ_to_free, theline);
    }

    തിരികെ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    പൂർണ്ണസംഖ്യ		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    പൂർണ്ണസംഖ്യ		*varargs,
    garray_T	*default_args,
    പൂർണ്ണസംഖ്യ		skip,
    exarg_T	*eap,		// can be NULL
    പൂർണ്ണസംഖ്യ		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*വരികൾ_to_free)
{
    പൂർണ്ണസംഖ്യ		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		any_default = FALSE;
    char_u	*whitep = *argp;
    പൂർണ്ണസംഖ്യ		need_വ്യഞ്ജകം = FALSE;

    എങ്കിൽ (newargs != ശൂന്യ)
	ga_init2(newargs, വലുപ്പം(char_u *), 3);
    എങ്കിൽ (argtypes != ശൂന്യ)
	ga_init2(argtypes, വലുപ്പം(char_u *), 3);
    എങ്കിൽ (arg_objm != ശൂന്യ)
	ga_init2(arg_objm, വലുപ്പം(int8_T), 3);
    എങ്കിൽ (!skip && default_args != ശൂന്യ)
	ga_init2(default_args, വലുപ്പം(char_u *), 3);

    എങ്കിൽ (varargs != ശൂന്യ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ആകുമ്പോൾ (*p != endchar)
    {
	ആകുമ്പോൾ (eap != ശൂന്യ && eap->ea_getline != ശൂന്യ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_വരി(eap, വരികൾ_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    എങ്കിൽ (theline == ശൂന്യ)
		നിർത്തുക;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	എങ്കിൽ (mustend && *p != endchar)
	{
	    എങ്കിൽ (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    പോകുക err_ret;
	}
	എങ്കിൽ (*p == endchar && !need_വ്യഞ്ജകം)
	    നിർത്തുക;

	എങ്കിൽ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    എങ്കിൽ (varargs != ശൂന്യ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    എങ്കിൽ (argtypes != ശൂന്യ)
	    {
		// ...name: list<type>
		എങ്കിൽ (!eval_isnamec1(*p))
		{
		    എങ്കിൽ (!skip)
			emsg(_(e_missing_name_after_dots));
		    പോകുക err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		എങ്കിൽ (p == arg)
		    നിർത്തുക;
		എങ്കിൽ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    നിർത്തുക;
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    എങ്കിൽ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		നിർത്തുക;
	    }

	    എങ്കിൽ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		എങ്കിൽ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_മൂല്യം_must_be_vnone_str), p);
		    പോകുക err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		എങ്കിൽ (ga_grow(default_args, 1) == FAIL)
		    പോകുക err_ret;

		char_u *വ്യഞ്ജകം = vim_strnsave((char_u *)"v:none", 6);
		എങ്കിൽ (വ്യഞ്ജകം == ശൂന്യ)
		    പോകുക err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = വ്യഞ്ജകം;
		default_args->ga_len++;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		പോകുക err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    എങ്കിൽ (newargs != ശൂന്യ && ga_grow(newargs, 1) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (newargs != ശൂന്യ)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		എങ്കിൽ (argtypes != ശൂന്യ && ga_grow(argtypes, 1) == OK
			    && arg_objm != ശൂന്യ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    എങ്കിൽ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			പൂർണ്ണസംഖ്യ len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			എങ്കിൽ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			എങ്കിൽ (assignment != ശൂന്യ)
			{
			    c = *argend;
			    *argend = NUL;
			    എങ്കിൽ (any_default)
				vim_snprintf((അക്ഷരം *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    അല്ലെങ്കിൽ
				vim_snprintf((അക്ഷരം *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    എങ്കിൽ (*p == ',')
		++p;
	}
	അല്ലെങ്കിൽ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    എങ്കിൽ (p == arg)
		നിർത്തുക;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    എങ്കിൽ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != ശൂന്യ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *വ്യഞ്ജകം = p;
		എങ്കിൽ (eval1(&p, &rettv, ശൂന്യ) != FAIL)
		{
		    എങ്കിൽ (!skip)
		    {
			എങ്കിൽ (ga_grow(default_args, 1) == FAIL)
			    പോകുക err_ret;

			എങ്കിൽ (need_വ്യഞ്ജകം)
			    need_വ്യഞ്ജകം = FALSE;
			// trim trailing whitespace
			ആകുമ്പോൾ (p > വ്യഞ്ജകം && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			വ്യഞ്ജകം = vim_strsave(വ്യഞ്ജകം);
			എങ്കിൽ (വ്യഞ്ജകം == ശൂന്യ)
			{
			    *p = c;
			    പോകുക err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = വ്യഞ്ജകം;
			default_args->ga_len++;
			*p = c;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    mustend = TRUE;
		    എങ്കിൽ (*skipwhite(p) == NUL)
			need_വ്യഞ്ജകം = TRUE;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		പോകുക err_ret;
	    }

	    എങ്കിൽ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		എങ്കിൽ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    പോകുക err_ret;
		}
		p = skipwhite(p);
	    }
	    എങ്കിൽ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		എങ്കിൽ (!skip && argtypes != ശൂന്യ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    പോകുക err_ret;
		}
	    }
	    അല്ലെങ്കിൽ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    എങ്കിൽ (*p != endchar)
	പോകുക err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    തിരികെ OK;

err_ret:
    എങ്കിൽ (newargs != ശൂന്യ)
	ga_clear_strings(newargs);
    എങ്കിൽ (!skip && default_args != ശൂന്യ)
	ga_clear_strings(default_args);
    തിരികെ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    പൂർണ്ണസംഖ്യ		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    പൂർണ്ണസംഖ്യ		obj_member_എണ്ണം,
    cctx_T	*cctx)
{
    പൂർണ്ണസംഖ്യ len = 0;

    ga_init2(&fp->uf_type_list, വലുപ്പം(type_T *), 10);
    എങ്കിൽ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	എങ്കിൽ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	എങ്കിൽ (fp->uf_arg_types != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	i;
	    type_T	*type;

	    വേണ്ടി (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		എങ്കിൽ (p == ശൂന്യ)
		    // will get the type from the default value
		    type = &t_unknown;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (arg_objm != ശൂന്യ && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			വേണ്ടി (പൂർണ്ണസംഖ്യ om = 0; om < obj_member_എണ്ണം; ++om)
			{
			    എങ്കിൽ (obj_members != ശൂന്യ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				നിർത്തുക;
			    }
			}
		    }
		    അല്ലെങ്കിൽ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		എങ്കിൽ (type == ശൂന്യ || !valid_declaration_type(type))
		    തിരികെ FAIL;
		fp->uf_arg_types[i] = type;
		എങ്കിൽ (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    എങ്കിൽ (obj_members != ശൂന്യ && *name == '_')
			// protected object method
			name++;

		    എങ്കിൽ (var_wrong_func_name(name, TRUE))
			തിരികെ FAIL;
		}
	    }
	}
    }

    എങ്കിൽ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = ശൂന്യ;
	p = ((char_u **)argtypes->ga_data)[len];
	എങ്കിൽ (p == ശൂന്യ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	അല്ലെങ്കിൽ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    എങ്കിൽ (fp->uf_va_type != ശൂന്യ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		തിരികെ FAIL;
	    }
	}
	എങ്കിൽ (fp->uf_va_type == ശൂന്യ)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    എങ്കിൽ (ret_type == ശൂന്യ)
	fp->uf_ret_type = &t_void;
    അല്ലെങ്കിൽ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	എങ്കിൽ (fp->uf_ret_type == ശൂന്യ)
	{
	    fp->uf_ret_type = &t_void;
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
register_closure(ufunc_T *fp)
{
    എങ്കിൽ (fp->uf_scoped == current_funccal)
	// no change
	തിരികെ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    എങ്കിൽ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	തിരികെ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    തിരികെ OK;
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

    എങ്കിൽ (name[0] != K_SPECIAL)
	തിരികെ name;
    len = STRLEN(name);
    എങ്കിൽ (len + 3 > bufsize)
	തിരികെ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    തിരികെ buf;
}

സ്ഥിതം char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	lambda_no = 0;
    പൂർണ്ണസംഖ്യ		n;
    string_T	ret;

    n = vim_snprintf((അക്ഷരം *)lambda_name, വലുപ്പം(lambda_name), "<lambda>%d", ++lambda_no);
    എങ്കിൽ (n < 1)
	ret.length = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (n >= (പൂർണ്ണസംഖ്യ)വലുപ്പം(lambda_name))
	ret.length = വലുപ്പം(lambda_name) - 1;
    അല്ലെങ്കിൽ
	ret.length = (size_t)n;

    ret.string = lambda_name;
    തിരികെ ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    സ്ഥിതം ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < വലുപ്പം(ufunc_T) ? വലുപ്പം(ufunc_T) : len);
    എങ്കിൽ (fp != ശൂന്യ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((ശൂന്യം *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	എങ്കിൽ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    എങ്കിൽ (fp->uf_name_exp != ശൂന്യ)
		vim_snprintf((അക്ഷരം *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    തിരികെ fp;
}

#എങ്കിൽ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, ശൂന്യം *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    എങ്കിൽ (fp == ശൂന്യ)
	തിരികെ ശൂന്യ;

    fp->uf_def_status = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_add(&func_hashtab, UF2HIKEY(fp), "add C function");

    തിരികെ name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    സ്ഥിതം char_u *
skip_arrow(
	char_u	*start,
	പൂർണ്ണസംഖ്യ	equal_arrow,
	char_u	**ret_type,
	പൂർണ്ണസംഖ്യ	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    എങ്കിൽ (equal_arrow)
    {
	എങ്കിൽ (*s == ':')
	{
	    എങ്കിൽ (white_error != ശൂന്യ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		തിരികെ ശൂന്യ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    എങ്കിൽ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		തിരികെ ശൂന്യ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	എങ്കിൽ (*s != '=')
	    തിരികെ ശൂന്യ;
	++s;
    }
    എങ്കിൽ (*s != '>')
	തിരികെ ശൂന്യ;
    എങ്കിൽ (white_error != ശൂന്യ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	തിരികെ ശൂന്യ;
    }
    തിരികെ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    എങ്കിൽ (checkforcmd(&p, "function", 2))
    {
	എങ്കിൽ (*p == '(')
	    തിരികെ FALSE;
	*cmd = p;
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    സ്ഥിതം ശൂന്യം
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    എങ്കിൽ (cstack == ശൂന്യ || cstack->cs_idx < 0)
	തിരികെ;

    പൂർണ്ണസംഖ്യ	    എണ്ണം = cstack->cs_idx + 1;
    പൂർണ്ണസംഖ്യ	    i;

    fp->uf_block_ids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, എണ്ണം);
    എങ്കിൽ (fp->uf_block_ids != ശൂന്യ)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		വലുപ്പം(പൂർണ്ണസംഖ്യ) * എണ്ണം);
	fp->uf_block_depth = എണ്ണം;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    വേണ്ടി (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    സ്ഥിതം char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    ആകുമ്പോൾ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	എങ്കിൽ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    MB_PTR_ADV(p);
    }

    തിരികെ p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    സ്ഥിതം char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    ആകുമ്പോൾ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	എങ്കിൽ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    MB_PTR_ADV(p);
    }

    തിരികെ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    സ്ഥിതം char_u *
find_start_of_vim9_comment(char_u *വരി)
{
    char_u	*p = വരി;

    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p == '#')
		// Found the start of a Vim9 comment
		നിർത്തുക;
	    MB_PTR_ADV(p);
	}
    }

    തിരികെ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *വരി_arg_in,
	garray_T    *വരികൾ_to_free)
{
    linenr_T	sourcing_lnum_മുകൾ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    പൂർണ്ണസംഖ്യ		saved_wait_return = need_wait_return;
    char_u	*വരി_arg = വരി_arg_in;
    പൂർണ്ണസംഖ്യ		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    അക്ഷരം	nesting_def[MAX_FUNC_NESTING];
    അക്ഷരം	nesting_inline[MAX_FUNC_NESTING];
    പൂർണ്ണസംഖ്യ		nesting = 0;
    getline_opt_T getline_options;
    പൂർണ്ണസംഖ്യ		indent = 2;
    char_u	*skip_until = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    പൂർണ്ണസംഖ്യ		is_heredoc = FALSE;
    പൂർണ്ണസംഖ്യ		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ശൂന്യ;
    size_t	heredoc_trimmedlen = 0;
    പൂർണ്ണസംഖ്യ		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    എങ്കിൽ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	എങ്കിൽ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    പോകുക theend;
	ആകുമ്പോൾ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ശൂന്യ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    വേണ്ടി (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	എങ്കിൽ (താക്കോൽ_Typed)
	{
	    സന്ദേശം_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	എങ്കിൽ (വരി_arg != ശൂന്യ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = വരി_arg;
	    p = vim_strchr(theline, '\n');
	    എങ്കിൽ (p == ശൂന്യ)
		വരി_arg += STRLEN(വരി_arg);
	    അല്ലെങ്കിൽ
	    {
		*p = NUL;
		വരി_arg = p + 1;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    theline = get_function_വരി(eap, വരികൾ_to_free, indent,
							      getline_options);
	}
	എങ്കിൽ (താക്കോൽ_Typed)
	    വരികൾ_ഇടത് = Rows - 1;
	എങ്കിൽ (theline == ശൂന്യ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_മുകൾ;
	    എങ്കിൽ (skip_until != ശൂന്യ)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    അല്ലെങ്കിൽ എങ്കിൽ (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    അല്ലെങ്കിൽ
		emsg(_(e_missing_endfunction));
	    പോകുക theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	എങ്കിൽ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	അല്ലെങ്കിൽ
	    sourcing_lnum_off = 0;

	എങ്കിൽ (skip_until != ശൂന്യ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    എങ്കിൽ (heredoc_trimmed == ശൂന്യ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		എങ്കിൽ (heredoc_trimmed == ശൂന്യ)
		    p = theline;
		അല്ലെങ്കിൽ എങ്കിൽ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		അല്ലെങ്കിൽ
		    p = theline + heredoc_trimmedlen;
		എങ്കിൽ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    എങ്കിൽ (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_data))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_data))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_data;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    വേണ്ടി (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		എങ്കിൽ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    എങ്കിൽ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		എങ്കിൽ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		എങ്കിൽ (nesting-- == 0)
		{
		    char_u *nextcmd = ശൂന്യ;

		    എങ്കിൽ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    അല്ലെങ്കിൽ എങ്കിൽ (വരി_arg != ശൂന്യ && *skipwhite(വരി_arg) != NUL)
			nextcmd = വരി_arg;
		    അല്ലെങ്കിൽ എങ്കിൽ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_മുകൾ
							+ newlines->ga_len + 1;
			എങ്കിൽ (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			അല്ലെങ്കിൽ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    എങ്കിൽ (nextcmd != ശൂന്യ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			എങ്കിൽ (വരികൾ_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)വരികൾ_to_free->ga_data)
						   [വരികൾ_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)വരികൾ_to_free->ga_data)
						   [വരികൾ_to_free->ga_len - 1];
			    --വരികൾ_to_free->ga_len;
			}
		    }
		    നിർത്തുക;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    അല്ലെങ്കിൽ എങ്കിൽ (nesting_def[nesting])
	    {
		എങ്കിൽ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    എങ്കിൽ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    എങ്കിൽ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		എങ്കിൽ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ശൂന്യ, TRUE, 0));
		എങ്കിൽ (vim9_function && *p == '<')
		{
		    // skip generic function
		    എങ്കിൽ (skip_generic_func_type_args(&p) == FAIL)
			പോകുക theend;
		}
		എങ്കിൽ (*skipwhite(p) == '(')
		{
		    എങ്കിൽ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    അല്ലെങ്കിൽ
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    എങ്കിൽ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		എങ്കിൽ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		അല്ലെങ്കിൽ
		    end = p + STRLEN(p) - 1;

		ആകുമ്പോൾ (end > p && VIM_ISWHITE(*end))
		    --end;
		എങ്കിൽ (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    പൂർണ്ണസംഖ്യ	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    ആകുമ്പോൾ (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    എങ്കിൽ (!is_block)
			is_block = find_cmd_block_start(p) != ശൂന്യ;

		    എങ്കിൽ (is_block)
		    {
			എങ്കിൽ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			അല്ലെങ്കിൽ
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
	    എങ്കിൽ (!vim9_function || colon)
		p = skip_range(p, FALSE, ശൂന്യ);
	    char_u *tp = p;
	    എങ്കിൽ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    അല്ലെങ്കിൽ
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    എങ്കിൽ (arg[0] == '<' && arg[1] =='<'
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
		എങ്കിൽ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    എങ്കിൽ (heredoc_trimmed == ശൂന്യ)
			heredoc_trimmedlen = 0;
		}
		എങ്കിൽ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		അല്ലെങ്കിൽ
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		എങ്കിൽ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    എങ്കിൽ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		എങ്കിൽ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    പൂർണ്ണസംഖ്യ		save_sc_version = current_sctx.sc_version;
		    പൂർണ്ണസംഖ്യ		var_എണ്ണം = 0;
		    പൂർണ്ണസംഖ്യ		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_എണ്ണം, &semicolon,
									 TRUE);
		    എങ്കിൽ (arg != ശൂന്യ)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    എങ്കിൽ (arg != ശൂന്യ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			പൂർണ്ണസംഖ്യ has_trim = FALSE;

			p = skipwhite(arg + 3);
			ആകുമ്പോൾ (TRUE)
			{
			    എങ്കിൽ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				തുടരുക;
			    }
			    എങ്കിൽ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				തുടരുക;
			    }
			    നിർത്തുക;
			}
			എങ്കിൽ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    എങ്കിൽ (heredoc_trimmed == ശൂന്യ)
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
	എങ്കിൽ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    പോകുക theend;

	എങ്കിൽ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	അല്ലെങ്കിൽ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ആകുമ്പോൾ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ശൂന്യ;

	// Check for end of eap->arg.
	എങ്കിൽ (വരി_arg != ശൂന്യ && *വരി_arg == NUL)
	    വരി_arg = ശൂന്യ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    എങ്കിൽ (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    തിരികെ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	പൂർണ്ണസംഖ്യ	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    പൂർണ്ണസംഖ്യ		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ശൂന്യ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    പൂർണ്ണസംഖ്യ		lnum_save = -1;
    linenr_T	sourcing_lnum_മുകൾ = SOURCING_LNUM;
    char_u	*വരി_arg = ശൂന്യ;

    *arg = skipwhite(*arg + 1);
    എങ്കിൽ (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	തിരികെ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    എങ്കിൽ (**arg == '\n')
	വരി_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    എങ്കിൽ (evalarg->eval_cctx != ശൂന്യ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    അല്ലെങ്കിൽ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, വലുപ്പം(char_u *), 10);
    എങ്കിൽ (get_function_body(&eap, &newlines, വരി_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	പോകുക erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_എണ്ണം += newlines.ga_len;
    എങ്കിൽ (gap->ga_itemsize > 0)
    {
	പൂർണ്ണസംഖ്യ	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	വേണ്ടി (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    എങ്കിൽ (p == ശൂന്യ)
		// comment line in the lambda body
		തുടരുക;

	    p = skipwhite(p);

	    എങ്കിൽ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		പോകുക erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    എങ്കിൽ (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    അല്ലെങ്കിൽ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    എങ്കിൽ (pnl != ശൂന്യ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	എങ്കിൽ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    പോകുക erret;
	എങ്കിൽ (eap.nextcmd != ശൂന്യ)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	അല്ലെങ്കിൽ
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	എങ്കിൽ (pnl != ശൂന്യ)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    എങ്കിൽ (eap.nextcmd != ശൂന്യ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	എങ്കിൽ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    അല്ലെങ്കിൽ
	*arg = (char_u *)"";

    എങ്കിൽ (!evaluate)
    {
	ret = OK;
	പോകുക erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    എങ്കിൽ (ufunc == ശൂന്യ)
	പോകുക erret;
    എങ്കിൽ (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	പോകുക erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = ശൂന്യ;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = ശൂന്യ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_മുകൾ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    എങ്കിൽ (parse_argument_types(ufunc, argtypes, varargs, ശൂന്യ, ശൂന്യ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	പോകുക erret;
    }

    // parse the return type, if any
    എങ്കിൽ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	പോകുക erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    എങ്കിൽ (pt == ശൂന്യ)
	പോകുക erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_വരികൾ = newlines;
    newlines.ga_data = ശൂന്യ;
    എങ്കിൽ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    എങ്കിൽ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_മുകൾ;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ശൂന്യ;
    ret = OK;

erret:
    എങ്കിൽ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    എങ്കിൽ (newargs != ശൂന്യ)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    എങ്കിൽ (ufunc != ശൂന്യ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    തിരികെ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    പൂർണ്ണസംഖ്യ
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = ശൂന്യ;
    partial_T   *pt = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		varargs;
    char_u	*ret_type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*s;
    char_u	*start, *end;
    പൂർണ്ണസംഖ്യ		*old_eval_lavars = eval_lavars_used;
    പൂർണ്ണസംഖ്യ		eval_lavars = FALSE;
    char_u	*tofree2 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		equal_arrow = **arg == '(';
    പൂർണ്ണസംഖ്യ		white_error = FALSE;
    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    എങ്കിൽ (equal_arrow && !vim9script)
	തിരികെ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', ശൂന്യ,
	    types_optional ? &argtypes : ശൂന്യ, types_optional,
			types_optional ? &arg_objm : ശൂന്യ, evalarg,
			ശൂന്യ, &default_args, TRUE, ശൂന്യ, FALSE, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ശൂന്യ) == ശൂന്യ)
    {
	എങ്കിൽ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	തിരികെ called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    എങ്കിൽ (evaluate)
	pnewargs = &newargs;
    അല്ലെങ്കിൽ
	pnewargs = ശൂന്യ;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ശൂന്യ, types_optional,
			types_optional ? &arg_objm : ശൂന്യ, evalarg,
					    &varargs, &default_args,
					    FALSE, ശൂന്യ, FALSE, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ശൂന്യ)) == ശൂന്യ)
    {
	എങ്കിൽ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	തിരികെ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    എങ്കിൽ (ret_type != ശൂന്യ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    എങ്കിൽ (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    എങ്കിൽ (equal_arrow && **arg == '{')
    {
	എങ്കിൽ (evalarg == ശൂന്യ)
	    // cannot happen?
	    പോകുക theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	എങ്കിൽ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ശൂന്യ, varargs,
			   &default_args, ret_type) == FAIL)
	    പോകുക errret;
	പോകുക theend;
    }
    എങ്കിൽ (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_മൂല്യങ്ങൾ_in_lambda));
	പോകുക errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_വ്യഞ്ജകം_concatenate(arg, &start, &end, evalarg);
    എങ്കിൽ (ret == FAIL)
	പോകുക errret;

    എങ്കിൽ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg != '}')
	{
	    semsg(_(e_expected_വലത്_curly_str), *arg);
	    പോകുക errret;
	}
	++*arg;
    }

    എങ്കിൽ (evaluate)
    {
	പൂർണ്ണസംഖ്യ	    len;
	പൂർണ്ണസംഖ്യ	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *വരി_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	എങ്കിൽ (fp == ശൂന്യ)
	    പോകുക errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	എങ്കിൽ (pt == ശൂന്യ)
	    പോകുക errret;

	ga_init2(&newlines, വലുപ്പം(char_u *), 1);
	എങ്കിൽ (ga_grow(&newlines, 1) == FAIL)
	    പോകുക errret;

	// If there are line breaks, we need to split up the string.
	വരി_end = vim_strchr(start, '\n');
	എങ്കിൽ (വരി_end == ശൂന്യ || വരി_end > end)
	    വരി_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (പൂർണ്ണസംഖ്യ)(വരി_end - start) + 1;
	p = alloc(len);
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, വരി_end - start);

	എങ്കിൽ (വരി_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ആകുമ്പോൾ (വരി_end < end && *വരി_end == '\n')
	    {
		എങ്കിൽ (ga_grow(&newlines, 1) == FAIL)
		    പോകുക errret;
		start = വരി_end + 1;
		വരി_end = vim_strchr(start, '\n');
		എങ്കിൽ (വരി_end == ശൂന്യ || വരി_end > end)
		    വരി_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, വരി_end - start);
	    }
	}

	എങ്കിൽ (strstr((അക്ഷരം *)p + 7, "a:") == ശൂന്യ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	എങ്കിൽ (types_optional)
	{
	    എങ്കിൽ (parse_argument_types(fp, &argtypes,
				vim9script && varargs, ശൂന്യ, ശൂന്യ, 0,
				cctx) == FAIL)
		പോകുക errret;
	    എങ്കിൽ (ret_type != ശൂന്യ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ശൂന്യ, cctx, TRUE);
		എങ്കിൽ (fp->uf_ret_type == ശൂന്യ)
		    പോകുക errret;
	    }
	    അല്ലെങ്കിൽ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_വരികൾ = newlines;
	എങ്കിൽ (current_funccal != ശൂന്യ && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    എങ്കിൽ (register_closure(fp) == FAIL)
		പോകുക errret;
	}

#ifdef FEAT_PROFILE
	എങ്കിൽ (prof_def_func())
	    func_do_profile(fp);
#endif
	എങ്കിൽ (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += start_lnum;

	function_using_block_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_add(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_used = old_eval_lavars;
    vim_free(tofree2);
    എങ്കിൽ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    തിരികെ OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    എങ്കിൽ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    എങ്കിൽ (fp != ശൂന്യ)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_used = old_eval_lavars;
    തിരികെ FAIL;
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
	പൂർണ്ണസംഖ്യ	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	പൂർണ്ണസംഖ്യ	    no_autoload,
	പൂർണ്ണസംഖ്യ	    new_function,
	പൂർണ്ണസംഖ്യ	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		cc;
    char_u	*s = ശൂന്യ;
    hashtab_T	*ht;
    പൂർണ്ണസംഖ്യ		did_type = FALSE;

    എങ്കിൽ (partialp != ശൂന്യ)
	*partialp = ശൂന്യ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    എങ്കിൽ (v != ശൂന്യ)
    {
	tv = &v->di_tv;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	പൂർണ്ണസംഖ്യ	    len = *lenp;

	എങ്കിൽ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	എങ്കിൽ (import != ശൂന്യ)
	{
	    name[len] = NUL;
	    എങ്കിൽ (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    അല്ലെങ്കിൽ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    തിരികെ (char_u *)"";	// just in case
	}
    }

    എങ്കിൽ (tv != ശൂന്യ)
    {
	എങ്കിൽ (found_var != ശൂന്യ)
	    *found_var = TRUE;
	എങ്കിൽ (tv->v_type == VAR_FUNC)
	{
	    എങ്കിൽ (tv->vval.v_string == ശൂന്യ)
	    {
		*lenp = 0;
		തിരികെ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    എങ്കിൽ (p != ശൂന്യ && generic_func_find_open_bracket(name) != ശൂന്യ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		തിരികെ (char_u *)"";
	    }
	    എങ്കിൽ (p != ശൂന്യ)
		*lenp = p - s;
	    അല്ലെങ്കിൽ
		*lenp = (പൂർണ്ണസംഖ്യ)STRLEN(s);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    എങ്കിൽ (pt == ശൂന്യ)
	    {
		*lenp = 0;
		തിരികെ (char_u *)"";	// just in case
	    }
	    എങ്കിൽ (partialp != ശൂന്യ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (പൂർണ്ണസംഖ്യ)STRLEN(s);
	}

	എങ്കിൽ (s != ശൂന്യ)
	{
	    എങ്കിൽ (!did_type && type != ശൂന്യ && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		എങ്കിൽ (sv != ശൂന്യ)
		    *type = sv->sv_type;
	    }
	    തിരികെ s;
	}
    }

    തിരികെ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    ശൂന്യം
emsg_funcname(അക്ഷരം *ermsg, char_u *name)
{
    char_u	*p = name;

    എങ്കിൽ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    എങ്കിൽ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    പൂർണ്ണസംഖ്യ
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	പൂർണ്ണസംഖ്യ	    partial_argc,
	typval_T    *argvars,
	പൂർണ്ണസംഖ്യ	    *argcount,
	പൂർണ്ണസംഖ്യ	    is_builtin)
{
    char_u	*argp = *arg;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ആകുമ്പോൾ (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	എങ്കിൽ (*argp == ')' || *argp == ',' || *argp == NUL)
	    നിർത്തുക;

	പൂർണ്ണസംഖ്യ arg_idx = *argcount;
	എങ്കിൽ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    നിർത്തുക;
	}
	++*argcount;
	എങ്കിൽ (!is_builtin && check_typval_is_മൂല്യം(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    നിർത്തുക;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	എങ്കിൽ (vim9script)
	{
	    എങ്കിൽ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		എങ്കിൽ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	    argp = skipwhite(argp);
	എങ്കിൽ (*argp != ',')
	    നിർത്തുക;
	എങ്കിൽ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    എങ്കിൽ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    നിർത്തുക;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    എങ്കിൽ (*argp == ')')
	++argp;
    അല്ലെങ്കിൽ
	ret = FAIL;
    *arg = argp;
    തിരികെ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
get_func_tv(
    char_u	*name,		// name of the function
    പൂർണ്ണസംഖ്യ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    പൂർണ്ണസംഖ്യ		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    പൂർണ്ണസംഖ്യ		argcount = 0;			// number of arguments found
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ശൂന്യ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    എങ്കിൽ (ret == OK)
    {
	പൂർണ്ണസംഖ്യ	i = 0;
	പൂർണ്ണസംഖ്യ	did_emsg_before = did_emsg;

	എങ്കിൽ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    എങ്കിൽ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, വലുപ്പം(typval_T *), 50);
	    വേണ്ടി (i = 0; i < argcount; ++i)
		എങ്കിൽ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	എങ്കിൽ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!aborting() && evaluate)
    {
	എങ്കിൽ (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	അല്ലെങ്കിൽ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ആകുമ്പോൾ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    എങ്കിൽ (vim9script)
	*arg = argp;
    അല്ലെങ്കിൽ
	*arg = skipwhite(argp);
    തിരികെ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_fname_sid(char_u *p)
{
    തിരികെ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    എങ്കിൽ (script_name == name)
	തിരികെ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
    fname_buflen = 3;
    എങ്കിൽ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	അല്ലെങ്കിൽ
	{
	    fname_buflen += vim_snprintf((അക്ഷരം *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    എങ്കിൽ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    അല്ലെങ്കിൽ
    {
	fname = alloc(fnamelen + 1);
	എങ്കിൽ (fname == ശൂന്യ)
	    *error = FCERR_OTHER;
	അല്ലെങ്കിൽ
	{
	    *tofree = fname;
	    vim_snprintf((അക്ഷരം *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    തിരികെ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    ശൂന്യം
func_name_with_sid(char_u *name, പൂർണ്ണസംഖ്യ sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
    vim_snprintf((അക്ഷരം *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    സ്ഥിതം ufunc_T *
find_func_with_sid(char_u *name, പൂർണ്ണസംഖ്യ sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    എങ്കിൽ (!SCRIPT_ID_VALID(sid))
	തിരികെ ശൂന്യ;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	തിരികെ HI2UF(hi);
    തിരികെ ശൂന്യ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    സ്ഥിതം ufunc_T *
find_func_with_prefix(char_u *name, പൂർണ്ണസംഖ്യ sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    എങ്കിൽ (vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ)
	തിരികെ ശൂന്യ;	// already has the prefix
    എങ്കിൽ (!SCRIPT_ID_VALID(sid))
	തിരികെ ശൂന്യ;	// not in a script
    si = SCRIPT_ITEM(sid);
    എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	എങ്കിൽ (namep == ശൂന്യ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	എങ്കിൽ (len < വലുപ്പം(buffer))
	    auto_name = buffer;
	അല്ലെങ്കിൽ
	    auto_name = alloc(len);
	എങ്കിൽ (auto_name != ശൂന്യ)
	{
	    vim_snprintf((അക്ഷരം *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    എങ്കിൽ (auto_name != buffer)
		vim_free(auto_name);
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
		തിരികെ HI2UF(hi);
	}
    }

    തിരികെ ശൂന്യ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, പൂർണ്ണസംഖ്യ flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    എങ്കിൽ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	എങ്കിൽ (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    എങ്കിൽ (func != ശൂന്യ)
		തിരികെ func;
	}
	എങ്കിൽ (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    എങ്കിൽ (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (പൂർണ്ണസംഖ്യ)sid);
		എങ്കിൽ (func != ശൂന്യ)
		    തിരികെ func;
	    }
	}
    }

    എങ്കിൽ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    തിരികെ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    തിരികെ find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, പൂർണ്ണസംഖ്യ is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    എങ്കിൽ (fp != ശൂന്യ && (fp->uf_flags & FC_DEAD) == 0)
	തിരികെ fp;
    തിരികെ ശൂന്യ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    പൂർണ്ണസംഖ്യ
func_is_global(ufunc_T *ufunc)
{
    തിരികെ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    പൂർണ്ണസംഖ്യ
func_requires_g_prefix(ufunc_T *ufunc)
{
    തിരികെ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ശൂന്യ
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    പൂർണ്ണസംഖ്യ	len;

    എങ്കിൽ (!func_is_global(fp))
	len = vim_snprintf((അക്ഷരം *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    അല്ലെങ്കിൽ
	len = vim_snprintf((അക്ഷരം *)buf, bufsize, "%s", fp->uf_name);

    തിരികെ (len >= (പൂർണ്ണസംഖ്യ)bufsize) ? (പൂർണ്ണസംഖ്യ)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    സ്ഥിതം ശൂന്യം
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    അക്ഷരം	*name,
    varnumber_T nr)
{
    STRCPY(v->di_താക്കോൽ, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_സംഖ്യ;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_സംഖ്യ = nr;
}

/*
 * Free "fc".
 */
    സ്ഥിതം ശൂന്യം
free_funccal(funccall_T *fc)
{
    പൂർണ്ണസംഖ്യ	i;

    വേണ്ടി (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	എങ്കിൽ (fp != ശൂന്യ && fp->uf_scoped == fc)
	    fp->uf_scoped = ശൂന്യ;
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
   സ്ഥിതം ശൂന്യം
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_ഇനങ്ങൾ(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    സ്ഥിതം ശൂന്യം
cleanup_function_call(funccall_T *fc)
{
    പൂർണ്ണസംഖ്യ	may_free_fc = fc->fc_refcount <= 0;
    പൂർണ്ണസംഖ്യ	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    എങ്കിൽ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    അല്ലെങ്കിൽ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    എങ്കിൽ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (പൂർണ്ണസംഖ്യ)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    എങ്കിൽ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ഇനങ്ങൾ(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    എങ്കിൽ (free_fc)
	free_funccal(fc);
    അല്ലെങ്കിൽ
    {
	സ്ഥിതം പൂർണ്ണസംഖ്യ made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	എങ്കിൽ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (++made_copy >= (പൂർണ്ണസംഖ്യ)((4096 * 1024) / വലുപ്പം(*fc)))
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
numbered_function(char_u *name)
{
    തിരികെ SAFE_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && SAFE_isdigit(name[2]));
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
    പൂർണ്ണസംഖ്യ
func_name_refcount(char_u *name)
{
    തിരികെ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    സ്ഥിതം ശൂന്യം
funccal_unref(funccall_T *fc, ufunc_T *fp, പൂർണ്ണസംഖ്യ force)
{
    funccall_T	**pfc;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (fc == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	വേണ്ടി (pfc = &previous_funccal; *pfc != ശൂന്യ; pfc = &(*pfc)->fc_caller)
	{
	    എങ്കിൽ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		തിരികെ;
	    }
	}
    വേണ്ടി (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	എങ്കിൽ (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = ശൂന്യ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    എങ്കിൽ (fp->uf_flags & FC_DEAD)
	തിരികെ FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    എങ്കിൽ (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	തിരികെ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    എങ്കിൽ (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	തിരികെ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    തിരികെ TRUE;
}

    സ്ഥിതം ശൂന്യം
func_clear_ഇനങ്ങൾ(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_വരികൾ));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    എങ്കിൽ (IS_GENERIC_FUNC(fp))
	generic_func_clear_ഇനങ്ങൾ(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ശൂന്യ;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    എങ്കിൽ (fp->uf_cb_free != ശൂന്യ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ശൂന്യ;
    }

    fp->uf_cb_state = ശൂന്യ;
    fp->uf_cb = ശൂന്യ;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_എണ്ണം);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    സ്ഥിതം ശൂന്യം
func_clear(ufunc_T *fp, പൂർണ്ണസംഖ്യ force)
{
    എങ്കിൽ (fp->uf_cleared)
	തിരികെ;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_ഇനങ്ങൾ(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
func_free(ufunc_T *fp, പൂർണ്ണസംഖ്യ force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    എങ്കിൽ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    എങ്കിൽ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	എങ്കിൽ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	തിരികെ OK;
    }
    തിരികെ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    ശൂന്യം
func_clear_free(ufunc_T *fp, പൂർണ്ണസംഖ്യ force)
{
    func_clear(fp, force);
    എങ്കിൽ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    അല്ലെങ്കിൽ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    പൂർണ്ണസംഖ്യ
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ശൂന്യ;

    എങ്കിൽ (ufunc == ശൂന്യ)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	തിരികെ FAIL;
    }

    fp = find_func(global, TRUE);
    എങ്കിൽ (fp != ശൂന്യ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	തിരികെ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    എങ്കിൽ (fp == ശൂന്യ)
	തിരികെ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    എങ്കിൽ (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_വരികൾ, &fp->uf_വരികൾ) == FAIL)
	പോകുക failed;

    എങ്കിൽ (ufunc->uf_arg_types != ശൂന്യ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	എങ്കിൽ (fp->uf_arg_types == ശൂന്യ)
	    പോകുക failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    വലുപ്പം(type_T *) * fp->uf_args.ga_len);
    }
    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	എങ്കിൽ (fp->uf_va_name == ശൂന്യ)
	    പോകുക failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    എങ്കിൽ ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == ശൂന്യ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	എങ്കിൽ (pt == ശൂന്യ)
	    പോകുക failed;
	എങ്കിൽ (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    പോകുക failed;
	}
	ufunc->uf_partial = pt;
    }

    തിരികെ OK;

failed:
    func_clear_free(fp, TRUE);
    തിരികെ FAIL;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    പൂർണ്ണസംഖ്യ
funcdepth_increment(ശൂന്യം)
{
    എങ്കിൽ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	തിരികെ FAIL;
    }
    ++funcdepth;
    തിരികെ OK;
}

    ശൂന്യം
funcdepth_decrement(ശൂന്യം)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    പൂർണ്ണസംഖ്യ
funcdepth_get(ശൂന്യം)
{
    തിരികെ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    ശൂന്യം
funcdepth_restore(പൂർണ്ണസംഖ്യ depth)
{
    funcdepth = depth;
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

    എങ്കിൽ (fc == ശൂന്യ)
	തിരികെ ശൂന്യ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    തിരികെ fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    ശൂന്യം
remove_funccal(ശൂന്യം)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    സ്ഥിതം funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    പൂർണ്ണസംഖ്യ		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    പൂർണ്ണസംഖ്യ		using_sandbox = FALSE;
    പൂർണ്ണസംഖ്യ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    പൂർണ്ണസംഖ്യ		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    പൂർണ്ണസംഖ്യ		default_arg_err = FALSE;
    dictitem_T	*v;
    പൂർണ്ണസംഖ്യ		fixvar_idx = 0;	// index in fc_fixvar[]
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		ai;
    പൂർണ്ണസംഖ്യ		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    പൂർണ്ണസംഖ്യ		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    എങ്കിൽ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = -1;
	തിരികെ FCERR_FAILED;
    }

    വരി_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    എങ്കിൽ (fc == ശൂന്യ)
	തിരികെ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, വലുപ്പം(ufunc_T *), 1);

    എങ്കിൽ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	എങ്കിൽ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ശൂന്യ ? ശൂന്യ : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	എങ്കിൽ (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	എങ്കിൽ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	എങ്കിൽ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	എങ്കിൽ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ശൂന്യ && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	തിരികെ retval;
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
    എങ്കിൽ (selfdict != ശൂന്യ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_താക്കോൽ;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_lock = 0;
	v->di_tv.vval.v_dict = selfdict;
	++selfdict->dv_refcount;
    }

    /*
     * Init a: variables, unless none found (in lambda).
     * Set a:0 to "argcount" less number of named arguments, if >= 0.
     * Set a:000 to a list with room for the "..." arguments.
     */
    init_var_dict(&fc->fc_l_avars, &fc->fc_l_avars_var, VAR_SCOPE);
    എങ്കിൽ ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    എങ്കിൽ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_താക്കോൽ;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_lock = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_FIELD(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_lock = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    എങ്കിൽ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    വേണ്ടി (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	പൂർണ്ണസംഖ്യ	    addlocal = FALSE;
	typval_T    def_rettv;
	പൂർണ്ണസംഖ്യ	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	എങ്കിൽ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    എങ്കിൽ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_സംഖ്യ == VVAL_NONE));
	    എങ്കിൽ (isdefault)
	    {
		char_u	    *default_വ്യഞ്ജകം = ശൂന്യ;

		def_rettv.v_type = VAR_സംഖ്യ;
		def_rettv.vval.v_സംഖ്യ = -1;

		default_വ്യഞ്ജകം = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		എങ്കിൽ (eval1(&default_വ്യഞ്ജകം, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    നിർത്തുക;
		}
	    }

	    namelen = STRLEN(name);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		നിർത്തുക;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((അക്ഷരം *)numbuf, വലുപ്പം(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	എങ്കിൽ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_താക്കോൽ, name);
	}
	അല്ലെങ്കിൽ
	{
	    v = dictitem_alloc(name);
	    എങ്കിൽ (v == ശൂന്യ)
		നിർത്തുക;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	എങ്കിൽ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	എങ്കിൽ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	അല്ലെങ്കിൽ
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	എങ്കിൽ (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    എങ്കിൽ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_തള്ളുക_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    എങ്കിൽ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	എങ്കിൽ (p_verbose >= 14)
	{
	    char_u	buf[സന്ദേശം_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    സന്ദേശം_puts("(");
	    വേണ്ടി (i = 0; i < argcount; ++i)
	    {
		എങ്കിൽ (i > 0)
		    സന്ദേശം_puts(", ");
		എങ്കിൽ (argvars[i].v_type == VAR_സംഖ്യ)
		    സന്ദേശം_outnum((long)argvars[i].vval.v_സംഖ്യ);
		അല്ലെങ്കിൽ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    എങ്കിൽ (s != ശൂന്യ)
		    {
			എങ്കിൽ (vim_strsize(s) > സന്ദേശം_BUF_CLEN)
			{
			    trunc_string(s, buf, സന്ദേശം_BUF_CLEN, സന്ദേശം_BUF_LEN);
			    s = buf;
			}
			സന്ദേശം_puts((അക്ഷരം *)s);
			vim_free(tofree);
		    }
		}
	    }
	    സന്ദേശം_puts(")");
	}
	സന്ദേശം_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == ശൂന്യ ? ശൂന്യ : fc->fc_caller->fc_func);
#endif

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

    എങ്കിൽ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_വരികൾ.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(ശൂന്യം)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    അല്ലെങ്കിൽ
	// call do_cmdline() to execute the lines
	do_cmdline(ശൂന്യ, get_func_വരി, (ശൂന്യം *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    എങ്കിൽ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_സംഖ്യ;
	rettv->vval.v_സംഖ്യ = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	എങ്കിൽ (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ശൂന്യ ? ശൂന്യ : fc->fc_caller->fc_func;

	എങ്കിൽ (fp->uf_profiling || (caller != ശൂന്യ && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    എങ്കിൽ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	എങ്കിൽ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	അല്ലെങ്കിൽ എങ്കിൽ (fc->fc_rettv->v_type == VAR_സംഖ്യ)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_സംഖ്യ);
	അല്ലെങ്കിൽ
	{
	    char_u	buf[സന്ദേശം_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    എങ്കിൽ (s != ശൂന്യ)
	    {
		എങ്കിൽ (vim_strsize(s) > സന്ദേശം_BUF_CLEN)
		{
		    trunc_string(s, buf, സന്ദേശം_BUF_CLEN, സന്ദേശം_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	സന്ദേശം_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_എടുക്കുക();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    എങ്കിൽ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    എങ്കിൽ (p_verbose >= 12 && SOURCING_NAME != ശൂന്യ)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	സന്ദേശം_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    വേണ്ടി (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    തിരികെ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, പൂർണ്ണസംഖ്യ argcount)
{
    പൂർണ്ണസംഖ്യ regular_args = fp->uf_args.ga_len;

    എങ്കിൽ (argcount < regular_args - fp->uf_def_args.ga_len)
	തിരികെ FCERR_TOOFEW;
    അല്ലെങ്കിൽ എങ്കിൽ (!has_varargs(fp) && argcount > regular_args)
	തിരികെ FCERR_TOOMANY;
    തിരികെ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	പൂർണ്ണസംഖ്യ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    എങ്കിൽ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	തിരികെ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    എങ്കിൽ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ശൂന്യ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    എങ്കിൽ (error != FCERR_UNKNOWN)
	തിരികെ error;

    എങ്കിൽ ((fp->uf_flags & FC_DICT) && selfdict == ശൂന്യ)
    {
	error = FCERR_DICT;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_തിരയൽ_patterns();
	എങ്കിൽ (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ശൂന്യ);
	എങ്കിൽ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	എങ്കിൽ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_തിരയൽ_patterns();
    }

    തിരികെ error;
}

സ്ഥിതം funccal_entry_T *funccal_അട്ടി = ശൂന്യ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    ശൂന്യം
save_funccal(funccal_entry_T *entry)
{
    entry->മുകൾ_funccal = current_funccal;
    entry->next = funccal_അട്ടി;
    funccal_അട്ടി = entry;
    current_funccal = ശൂന്യ;
}

    ശൂന്യം
restore_funccal(ശൂന്യം)
{
    എങ്കിൽ (funccal_അട്ടി == ശൂന്യ)
	internal_error("restore_funccal()");
    അല്ലെങ്കിൽ
    {
	current_funccal = funccal_അട്ടി->മുകൾ_funccal;
	funccal_അട്ടി = funccal_അട്ടി->next;
    }
}

    funccall_T *
get_current_funccal(ശൂന്യം)
{
    തിരികെ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    പൂർണ്ണസംഖ്യ
at_script_level(ശൂന്യം)
{
    തിരികെ current_funccal == ശൂന്യ && autocmd_match == ശൂന്യ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    ശൂന്യം
delete_script_functions(പൂർണ്ണസംഖ്യ sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
    len = 3 + vim_snprintf((അക്ഷരം *)buf + 3, വലുപ്പം(buf) - 3, "%d_", sid);

    ആകുമ്പോൾ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&func_hashtab, hi, todo)
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		എങ്കിൽ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    പൂർണ്ണസംഖ്യ changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    എങ്കിൽ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			എങ്കിൽ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    അല്ലെങ്കിൽ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			എങ്കിൽ (changed != func_hashtab.ht_changed)
			    നിർത്തുക;
		    }
		}
		--todo;
	    }
    }
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_all_functions(ശൂന്യം)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    പൂർണ്ണസംഖ്യ		changed;

    // Clean up the current_funccal chain and the funccal stack.
    ആകുമ്പോൾ (current_funccal != ശൂന്യ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	എങ്കിൽ (current_funccal == ശൂന്യ && funccal_അട്ടി != ശൂന്യ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ആകുമ്പോൾ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&func_hashtab, hi, todo)
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		എങ്കിൽ (func_name_refcount(fp->uf_name))
		    ++skipped;
		അല്ലെങ്കിൽ
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    എങ്കിൽ (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			നിർത്തുക;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ആകുമ്പോൾ (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ഇനങ്ങൾ(&func_hashtab, hi, todo)
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		എങ്കിൽ (func_name_refcount(fp->uf_name))
		    ++skipped;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			നിർത്തുക;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    എങ്കിൽ (skipped == 0)
	hash_clear(&func_hashtab);

    free_def_functions();
}
#endif

/*
 * Return TRUE if "name" looks like a builtin function name: starts with a
 * lower case letter, doesn't contain AUTOLOAD_CHAR or ':', no "." after the
 * name.
 * "len" is the length of "name", or -1 for NUL terminated.
 */
    പൂർണ്ണസംഖ്യ
builtin_function(char_u *name, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	തിരികെ FALSE;
    വേണ്ടി (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	എങ്കിൽ (name[i] == AUTOLOAD_CHAR)
	    തിരികെ FALSE;
	എങ്കിൽ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    എങ്കിൽ (name[i] == '.')
		തിരികെ FALSE;
	    നിർത്തുക;
	}
    }
    തിരികെ TRUE;
}

    പൂർണ്ണസംഖ്യ
func_call(
    char_u	*name,
    typval_T	*args,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = args->vval.v_list;
    listitem_T	*item;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    പൂർണ്ണസംഖ്യ		argc = 0;
    പൂർണ്ണസംഖ്യ		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ഇനങ്ങൾ(l, item)
    {
	എങ്കിൽ (argc == MAX_FUNC_ARGS - (partial == ശൂന്യ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    നിർത്തുക;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    എങ്കിൽ (item == ശൂന്യ)
    {
	funcexe_T	funcexe;
	പൂർണ്ണസംഖ്യ		namelen = -1;

	എങ്കിൽ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    എങ്കിൽ (p != ശൂന്യ)
	    {
		എങ്കിൽ (skip_generic_func_type_args(&p) == FAIL)
		    പോകുക done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	എങ്കിൽ (partial != ശൂന്യ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    എങ്കിൽ (funcexe.fe_object != ശൂന്യ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ആകുമ്പോൾ (argc > 0)
	clear_tv(&argv[--argc]);

done:
    തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ callback_depth = 0;

    പൂർണ്ണസംഖ്യ
get_callback_depth(ശൂന്യം)
{
    തിരികെ callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    പൂർണ്ണസംഖ്യ
call_callback(
    callback_T	*callback,
    പൂർണ്ണസംഖ്യ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    പൂർണ്ണസംഖ്യ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (callback->cb_name == ശൂന്യ || *callback->cb_name == NUL)
	തിരികെ FAIL;

    എങ്കിൽ (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	തിരികെ FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    എങ്കിൽ (callback->cb_partial != ശൂന്യ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	എങ്കിൽ (funcexe.fe_object != ശൂന്യ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    എങ്കിൽ (need_rethrow && current_exception != ശൂന്യ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    തിരികെ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    പൂർണ്ണസംഖ്യ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    എങ്കിൽ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	തിരികെ -2;

    retval = tv_get_സംഖ്യ_chk(&rettv, ശൂന്യ);
    clear_tv(&rettv);
    തിരികെ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    ശൂന്യം
user_func_error(funcerror_T error, char_u *name, പൂർണ്ണസംഖ്യ found_var)
{
    തിരഞ്ഞെടുപ്പ് (error)
    {
	അവസ്ഥ FCERR_UNKNOWN:
		എങ്കിൽ (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		അല്ലെങ്കിൽ
		    emsg_funcname(e_unknown_function_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		നിർത്തുക;
	അവസ്ഥ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		നിർത്തുക;
	അവസ്ഥ FCERR_OTHER:
	അവസ്ഥ FCERR_FAILED:
		// assume the error message was already given
		നിർത്തുക;
	അവസ്ഥ FCERR_NONE:
		നിർത്തുക;
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
    സ്ഥിതം funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	പൂർണ്ണസംഖ്യ	    argcount,
	പൂർണ്ണസംഖ്യ	    base_included,
	char_u	    *name)
{
    എങ്കിൽ (funcexe->fe_check_type != ശൂന്യ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	എങ്കിൽ (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? ശൂന്യ : funcexe->fe_basetv,
			  name) == FAIL)
	    തിരികെ FCERR_OTHER;
    }
    തിരികെ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    പൂർണ്ണസംഖ്യ
call_func(
    char_u	*funcname,	// name of the function
    പൂർണ്ണസംഖ്യ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    പൂർണ്ണസംഖ്യ		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    പൂർണ്ണസംഖ്യ		i;
    ufunc_T	*fp = ശൂന്യ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ശൂന്യ;
    char_u	*fname = ശൂന്യ;
    char_u	*name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    പൂർണ്ണസംഖ്യ		argv_clear = 0;
    പൂർണ്ണസംഖ്യ		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    എങ്കിൽ (partial != ശൂന്യ)
	fp = partial->pt_func;
    എങ്കിൽ (fp == ശൂന്യ)
	fp = funcexe->fe_ufunc;

    എങ്കിൽ (fp == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    എങ്കിൽ (p != ശൂന്യ)
	    {
		len = p - funcname;
		എങ്കിൽ (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ശൂന്യ)
		    പോകുക theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	എങ്കിൽ (name == ശൂന്യ)
	    പോകുക theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    എങ്കിൽ (funcexe->fe_doesrange != ശൂന്യ)
	*funcexe->fe_doesrange = FALSE;

    എങ്കിൽ (partial != ശൂന്യ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	എങ്കിൽ (partial->pt_dict != ശൂന്യ && (selfdict == ശൂന്യ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	എങ്കിൽ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    വേണ്ടി (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		എങ്കിൽ (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    പോകുക theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    വേണ്ടി (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    എങ്കിൽ (funcexe->fe_check_type != ശൂന്യ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		വേണ്ടി (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    എങ്കിൽ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != ശൂന്യ) ? name : funcname);

    എങ്കിൽ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	പൂർണ്ണസംഖ്യ	is_global = FALSE;

	// Skip "g:" before a function name.
	എങ്കിൽ (fp == ശൂന്യ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_സംഖ്യ;	// default rettv is number zero
	rettv->vval.v_സംഖ്യ = 0;
	error = FCERR_UNKNOWN;

	എങ്കിൽ (fp != ശൂന്യ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    എങ്കിൽ (fp == ശൂന്യ)
	    {
		fp = find_func(rfname, is_global);
		എങ്കിൽ (fp != ശൂന്യ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ശൂന്യ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    എങ്കിൽ (fp == ശൂന്യ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ശൂന്യ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    എങ്കിൽ (fp == ശൂന്യ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    എങ്കിൽ (fp == ശൂന്യ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		എങ്കിൽ (p != ശൂന്യ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    എങ്കിൽ (partial == ശൂന്യ && fp != ശൂന്യ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		എങ്കിൽ (fp == ശൂന്യ)
		{
		    error = FCERR_FAILED;
		    പോകുക theend;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != ശൂന്യ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		പോകുക theend;
	    }

	    എങ്കിൽ (fp != ശൂന്യ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    അല്ലെങ്കിൽ എങ്കിൽ (fp != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ need_arg_check = FALSE;
		എങ്കിൽ (funcexe->fe_check_type == ശൂന്യ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		എങ്കിൽ (funcexe->fe_argv_func != ശൂന്യ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		എങ്കിൽ (funcexe->fe_basetv != ശൂന്യ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, വലുപ്പം(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		എങ്കിൽ (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != ശൂന്യ) ? name : funcname);

		എങ്കിൽ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (funcexe->fe_basetv != ശൂന്യ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_internal_func(fname, argcount, argvars, rettv);
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
    എങ്കിൽ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    എങ്കിൽ (!aborting())
	user_func_error(error, (name != ശൂന്യ) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    ആകുമ്പോൾ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    തിരികെ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    പൂർണ്ണസംഖ്യ
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ശൂന്യ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_സംഖ്യ;	// default rettv is number zero
    rettv->vval.v_സംഖ്യ = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    എങ്കിൽ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    അല്ലെങ്കിൽ
	rfname = fname;
    fp = find_func(rfname, is_global);
    എങ്കിൽ (fp != ശൂന്യ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ശൂന്യ;
    എങ്കിൽ (fp == ശൂന്യ)
	ret = NOTDONE;
    അല്ലെങ്കിൽ എങ്കിൽ (fp != ശൂന്യ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    അല്ലെങ്കിൽ എങ്കിൽ (fp != ശൂന്യ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ശൂന്യ);
	എങ്കിൽ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    തിരികെ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    തിരികെ fp->uf_name_exp != ശൂന്യ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
function_list_modified(പൂർണ്ണസംഖ്യ prev_ht_changed)
{
    എങ്കിൽ (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
list_func_head(ufunc_T *fp, പൂർണ്ണസംഖ്യ indent)
{
    പൂർണ്ണസംഖ്യ		prev_ht_changed = func_hashtab.ht_changed;
    പൂർണ്ണസംഖ്യ		j;

    സന്ദേശം_start();

    // a timer at the more prompt may have deleted the function
    എങ്കിൽ (function_list_modified(prev_ht_changed))
	തിരികെ FAIL;

    എങ്കിൽ (indent)
	സന്ദേശം_puts("   ");
    എങ്കിൽ (fp->uf_def_status != UF_NOT_COMPILED)
	സന്ദേശം_puts("def ");
    അല്ലെങ്കിൽ
	സന്ദേശം_puts("function ");
    സന്ദേശം_puts((അക്ഷരം *)printable_func_name(fp));
    സന്ദേശം_putchar('(');
    വേണ്ടി (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	എങ്കിൽ (j)
	    സന്ദേശം_puts(", ");
	സന്ദേശം_puts((അക്ഷരം *)FUNCARG(fp, j));
	എങ്കിൽ (fp->uf_arg_types != ശൂന്യ)
	{
	    അക്ഷരം *tofree;

	    സന്ദേശം_puts(": ");
	    സന്ദേശം_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	എങ്കിൽ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    സന്ദേശം_puts(" = ");
	    സന്ദേശം_puts(((അക്ഷരം **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    എങ്കിൽ (fp->uf_varargs)
    {
	എങ്കിൽ (j)
	    സന്ദേശം_puts(", ");
	സന്ദേശം_puts("...");
    }
    എങ്കിൽ (fp->uf_va_name != ശൂന്യ)
    {
	എങ്കിൽ (!fp->uf_varargs)
	{
	    എങ്കിൽ (j)
		സന്ദേശം_puts(", ");
	    സന്ദേശം_puts("...");
	}
	സന്ദേശം_puts((അക്ഷരം *)fp->uf_va_name);
	എങ്കിൽ (fp->uf_va_type != ശൂന്യ)
	{
	    അക്ഷരം *tofree;

	    സന്ദേശം_puts(": ");
	    സന്ദേശം_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    സന്ദേശം_putchar(')');

    എങ്കിൽ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	എങ്കിൽ (fp->uf_ret_type != &t_void)
	{
	    അക്ഷരം *tofree;

	    സന്ദേശം_puts(": ");
	    സന്ദേശം_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fp->uf_flags & FC_ABORT)
	സന്ദേശം_puts(" abort");
    എങ്കിൽ (fp->uf_flags & FC_RANGE)
	സന്ദേശം_puts(" range");
    എങ്കിൽ (fp->uf_flags & FC_DICT)
	സന്ദേശം_puts(" dict");
    എങ്കിൽ (fp->uf_flags & FC_CLOSURE)
	സന്ദേശം_puts(" closure");
    സന്ദേശം_clr_eos();
    എങ്കിൽ (p_verbose > 0)
	last_set_സന്ദേശം(fp->uf_script_ctx);

    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ		*is_global,
    പൂർണ്ണസംഖ്യ		skip,		// only find the end, don't evaluate
    പൂർണ്ണസംഖ്യ		flags)
{
    തിരികെ trans_function_name_ext(pp, is_global, skip, flags,
	    ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    സ്ഥിതം char_u *
trans_function_name_ext(
    char_u	**pp,
    പൂർണ്ണസംഖ്യ		*is_global,
    പൂർണ്ണസംഖ്യ		skip,		// only find the end, don't evaluate
    പൂർണ്ണസംഖ്യ		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ശൂന്യ;
    char_u	*start;
    char_u	*end;
    പൂർണ്ണസംഖ്യ		lead;
    char_u	sid_buf[20];
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		extra = 0;
    പൂർണ്ണസംഖ്യ		prefix_g = FALSE;
    lval_T	lv;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		vim9_local;

    എങ്കിൽ (fdp != ശൂന്യ)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    എങ്കിൽ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (പൂർണ്ണസംഖ്യ)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	തിരികെ vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    എങ്കിൽ (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, ശൂന്യ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    എങ്കിൽ (end == start || (vim9script && end != ശൂന്യ
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	എങ്കിൽ (!skip)
	    emsg(_(e_function_name_required));
	പോകുക theend;
    }
    എങ്കിൽ (end == ശൂന്യ || (lv.ll_tv != ശൂന്യ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	എങ്കിൽ (!aborting())
	{
	    എങ്കിൽ (end != ശൂന്യ)
		semsg(_(e_invalid_argument_str), start);
	}
	അല്ലെങ്കിൽ
	    *pp = find_name_end(start, ശൂന്യ, ശൂന്യ, FNE_INCL_BR);
	പോകുക theend;
    }

    എങ്കിൽ (lv.ll_ufunc != ശൂന്യ)
    {
	എങ്കിൽ (ufunc != ശൂന്യ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	പോകുക theend;
    }

    എങ്കിൽ (lv.ll_tv != ശൂന്യ)
    {
	എങ്കിൽ (fdp != ശൂന്യ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ശൂന്യ;
	    fdp->fd_di = lv.ll_di;
	}
	എങ്കിൽ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ശൂന്യ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ശൂന്യ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ശൂന്യ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    എങ്കിൽ (partial != ശൂന്യ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!skip && !(flags & TFN_QUIET) && (fdp == ശൂന്യ
			     || lv.ll_dict == ശൂന്യ || fdp->fd_newkey == ശൂന്യ))
		emsg(_(e_funcref_required));
	    അല്ലെങ്കിൽ
		*pp = end;
	    name = ശൂന്യ;
	}
	പോകുക theend;
    }

    എങ്കിൽ (lv.ll_name == ശൂന്യ)
    {
	// Error found, but continue after the function name.
	*pp = end;
	പോകുക theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    എങ്കിൽ (lv.ll_exp_name != ശൂന്യ)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ശൂന്യ);
	എങ്കിൽ (name == lv.ll_exp_name)
	    name = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	പൂർണ്ണസംഖ്യ		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	അല്ലെങ്കിൽ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
	    vim_snprintf((അക്ഷരം *)sid_buf + 3, വലുപ്പം(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!(flags & TFN_NO_DEREF))
    {
	len = (പൂർണ്ണസംഖ്യ)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ശൂന്യ);
	എങ്കിൽ (name == *pp)
	    name = ശൂന്യ;
    }
    എങ്കിൽ (name != ശൂന്യ)
    {
	name = vim_strsave(name);
	*pp = end;
	എങ്കിൽ (name != ശൂന്യ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	പോകുക theend;
    }

    എങ്കിൽ (lv.ll_exp_name != ശൂന്യ)
    {
	len = (പൂർണ്ണസംഖ്യ)STRLEN(lv.ll_exp_name);
	എങ്കിൽ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    അല്ലെങ്കിൽ
    {
	// skip over "s:" and "g:"
	എങ്കിൽ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    എങ്കിൽ (lv.ll_name[0] == 'g')
	    {
		എങ്കിൽ (is_global != ശൂന്യ)
		{
		    *is_global = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (പൂർണ്ണസംഖ്യ)(end - lv.ll_name);
    }
    എങ്കിൽ (len <= 0)
    {
	എങ്കിൽ (!skip)
	    emsg(_(e_function_name_required));
	പോകുക theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*start) && vim9script
						&& (flags & TFN_IN_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    എങ്കിൽ (skip)
	lead = 0;	// do nothing
    അല്ലെങ്കിൽ എങ്കിൽ (lead > 0 || vim9_local)
    {
	എങ്കിൽ (!vim9_local)
	{
	    എങ്കിൽ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		പോകുക theend;
	    }
	    lead = 3;
	}
	എങ്കിൽ (vim9_local || (lv.ll_exp_name != ശൂന്യ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    എങ്കിൽ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		പോകുക theend;
	    }
	    sid_buflen = vim_snprintf((അക്ഷരം *)sid_buf, വലുപ്പം(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    എങ്കിൽ (vim9_local)
		extra = 3 + (പൂർണ്ണസംഖ്യ)sid_buflen;
	    അല്ലെങ്കിൽ
		lead += (പൂർണ്ണസംഖ്യ)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    അല്ലെങ്കിൽ എങ്കിൽ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	പോകുക theend;
    }
    എങ്കിൽ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	എങ്കിൽ (cp != ശൂന്യ && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    പോകുക theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    എങ്കിൽ (name != ശൂന്യ)
    {
	എങ്കിൽ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (പൂർണ്ണസംഖ്യ)KE_SNR;
	    എങ്കിൽ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (prefix_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = end;

theend:
    clear_lval(&lv);
    തിരികെ name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    സ്ഥിതം char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    എങ്കിൽ (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	എങ്കിൽ (p != ശൂന്യ)
	    തിരികെ p + 1;
    }
    തിരികെ ശൂന്യ;
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
    അക്ഷരം	sid_buf[25];
    size_t	sid_buflen;
    പൂർണ്ണസംഖ്യ		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    എങ്കിൽ (funcname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	എങ്കിൽ (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    തിരികെ ശൂന്യ;
	ufunc = find_func(funcname, FALSE);
	എങ്കിൽ (ufunc == ശൂന്യ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	++p;
	off = 0;
    }
    അല്ലെങ്കിൽ
	off = *funcname == 's' ? 2 : 5;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	തിരികെ ശൂന്യ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, വലുപ്പം(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    എങ്കിൽ (newname == ശൂന്യ)
	തിരികെ ശൂന്യ;
    vim_snprintf((അക്ഷരം *)newname, newnamesize, "%s%s", sid_buf, p + off);

    തിരികെ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    എങ്കിൽ (n != ശൂന്യ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    തിരികെ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	പൂർണ്ണസംഖ്യ	    *is_global,
	പൂർണ്ണസംഖ്യ	    skip,
	പൂർണ്ണസംഖ്യ	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    എങ്കിൽ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(ശൂന്യം)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	എങ്കിൽ (fudi != ശൂന്യ)
	    CLEAR_POINTER(fudi);
    }
    അല്ലെങ്കിൽ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    *name = p;
    തിരികെ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    ശൂന്യം
list_functions(regmatch_T *regmatch)
{
    പൂർണ്ണസംഖ്യ		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    വേണ്ടി (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    എങ്കിൽ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ശൂന്യ
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		എങ്കിൽ (list_func_head(fp, FALSE) == FAIL)
		    തിരികെ;
		എങ്കിൽ (function_list_modified(prev_ht_changed))
		    തിരികെ;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    സ്ഥിതം char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    എങ്കിൽ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	എങ്കിൽ (regmatch.regprog != ശൂന്യ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    എങ്കിൽ (*p == '/')
	++p;

    തിരികെ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    സ്ഥിതം ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, പൂർണ്ണസംഖ്യ is_global)
{
    ufunc_T	*fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		j;

    എങ്കിൽ (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	തിരികെ ശൂന്യ;
    }

    set_nextcmd(eap, p);

    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
	*p = NUL;

    എങ്കിൽ (eap->skip || got_int)
	തിരികെ ശൂന്യ;

    fp = find_func(name, is_global);
    എങ്കിൽ (fp == ശൂന്യ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	എങ്കിൽ (up != ശൂന്യ)
	    fp = find_func(up, FALSE);
    }

    എങ്കിൽ (fp == ശൂന്യ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	തിരികെ ശൂന്യ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    പൂർണ്ണസംഖ്യ prev_ht_changed = func_hashtab.ht_changed;

    എങ്കിൽ (list_func_head(fp, TRUE) != OK)
	തിരികെ fp;

    വേണ്ടി (j = 0; j < fp->uf_വരികൾ.ga_len && !got_int; ++j)
    {
	എങ്കിൽ (FUNCLINE(fp, j) == ശൂന്യ)
	    തുടരുക;
	സന്ദേശം_putchar('\n');
	സന്ദേശം_outnum((long)(j + 1));
	എങ്കിൽ (j < 9)
	    സന്ദേശം_putchar(' ');
	എങ്കിൽ (j < 99)
	    സന്ദേശം_putchar(' ');
	എങ്കിൽ (function_list_modified(prev_ht_changed))
	    നിർത്തുക;
	സന്ദേശം_prt_വരി(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    എങ്കിൽ (!got_int)
    {
	സന്ദേശം_putchar('\n');
	എങ്കിൽ (!function_list_modified(prev_ht_changed))
	{
	    എങ്കിൽ (fp->uf_def_status != UF_NOT_COMPILED)
		സന്ദേശം_puts("   enddef");
	    അല്ലെങ്കിൽ
		സന്ദേശം_puts("   endfunction");
	}
    }

    തിരികെ fp;
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
	garray_T    *വരികൾ_to_free,
	പൂർണ്ണസംഖ്യ	    class_flags,
	ocmember_T  *obj_members,
	പൂർണ്ണസംഖ്യ         obj_member_എണ്ണം,
	cctx_T	    *cctx)
{
    പൂർണ്ണസംഖ്യ		j;
    പൂർണ്ണസംഖ്യ		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*വരി_arg = ശൂന്യ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    പൂർണ്ണസംഖ്യ		varargs = FALSE;
    പൂർണ്ണസംഖ്യ		flags = 0;
    char_u	*ret_type = ശൂന്യ;
    ufunc_T	*fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		fp_allocated = FALSE;
    പൂർണ്ണസംഖ്യ		free_fp = FALSE;
    പൂർണ്ണസംഖ്യ		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	func_nr = 0;	    // number for nameless function
    പൂർണ്ണസംഖ്യ		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_മുകൾ;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    imported_T	*import = ശൂന്യ;

    // ":function" without argument: list functions.
    എങ്കിൽ (ends_excmd2(eap->cmd, eap->arg))
    {
	എങ്കിൽ (!eap->skip)
	    list_functions(ശൂന്യ);
	set_nextcmd(eap, eap->arg);
	തിരികെ ശൂന്യ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    എങ്കിൽ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	തിരികെ ശൂന്യ;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_args);
    generic_func_args_table_init(&gfatab);

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
    എങ്കിൽ (name_arg != ശൂന്യ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (vim9script)
	{
	    എങ്കിൽ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		തിരികെ ശൂന്യ;
	    }
	    p = to_name_end(p, TRUE);
	    എങ്കിൽ (*skipwhite(p) == '.' && vim_strchr(p, '(') != ശൂന്യ)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		തിരികെ ശൂന്യ;
	    }

	    p = eap->arg;
	}

	പൂർണ്ണസംഖ്യ tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ശൂന്യ);
	എങ്കിൽ (name == ശൂന്യ && (fudi.fd_dict == ശൂന്യ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    എങ്കിൽ (!aborting())
	    {
		എങ്കിൽ (!eap->skip && fudi.fd_newkey != ശൂന്യ)
		    semsg(_(e_താക്കോൽ_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		തിരികെ ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	എങ്കിൽ (is_export && name != ശൂന്യ)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    എങ്കിൽ (prefixed != ശൂന്യ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (paren && vim9script && name != ശൂന്യ
				    && vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    പോകുക ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    എങ്കിൽ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	പോകുക ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    എങ്കിൽ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	എങ്കിൽ (p == ശൂന്യ)
	    പോകുക ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    എങ്കിൽ (*p != '(')
    {
	എങ്കിൽ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    പോകുക ret_free;
	}
	// attempt to continue by skipping some text
	എങ്കിൽ (vim_strchr(p, '(') != ശൂന്യ)
	    p = vim_strchr(p, '(');
    }

    എങ്കിൽ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	പോകുക ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    എങ്കിൽ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	പോകുക ret_free;
    }

    ga_init2(&newlines, വലുപ്പം(char_u *), 10);

    എങ്കിൽ (!eap->skip && name_arg == ശൂന്യ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	എങ്കിൽ (name != ശൂന്യ)
	    arg = name;
	അല്ലെങ്കിൽ
	    arg = fudi.fd_newkey;
	എങ്കിൽ (arg != ശൂന്യ && (fudi.fd_di == ശൂന്യ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    പൂർണ്ണസംഖ്യ	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    എങ്കിൽ (arg != fudi.fd_newkey)
	    {
		എങ്കിൽ (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    എങ്കിൽ (name_base == ശൂന്യ)
			name_base = arg + 3;
		    അല്ലെങ്കിൽ
			++name_base;
		}
		വേണ്ടി (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		എങ്കിൽ (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    പോകുക ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		എങ്കിൽ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, ശൂന്യ,
			ശൂന്യ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    പോകുക ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	എങ്കിൽ (fudi.fd_dict != ശൂന്യ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    പോകുക ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    എങ്കിൽ (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ശൂന്യ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ശൂന്യ,
			 ശൂന്യ, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, വരികൾ_to_free) == FAIL)
	പോകുക errret_2;
    whitep = p;

    എങ്കിൽ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	എങ്കിൽ (*skipwhite(p) == ':')
	{
	    എങ്കിൽ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    എങ്കിൽ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ശൂന്യ;
	    }
	}
	p = skipwhite(p);
    }
    അല്ലെങ്കിൽ
	// find extra arguments "range", "dict", "abort" and "closure"
	വേണ്ടി (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    എങ്കിൽ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		എങ്കിൽ (current_funccal == ശൂന്യ)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_മുകൾ_level_str,
			    name == ശൂന്യ ? (char_u *)"" : name);
		    പോകുക erret;
		}
	    }
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    എങ്കിൽ (*p == '\n')
	വരി_arg = p + 1;
    അല്ലെങ്കിൽ എങ്കിൽ (*p != NUL
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
    എങ്കിൽ (താക്കോൽ_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	എങ്കിൽ (!eap->skip && !eap->forceit)
	{
	    എങ്കിൽ (fudi.fd_dict != ശൂന്യ && fudi.fd_newkey == ശൂന്യ)
		emsg(_(e_dictionary_entry_already_exists));
	    അല്ലെങ്കിൽ എങ്കിൽ (name != ശൂന്യ && find_func(name, is_global) != ശൂന്യ)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	എങ്കിൽ (!eap->skip && did_emsg)
	    പോകുക erret;

	സന്ദേശം_putchar('\n');	    // don't overwrite the function name
	cmdline_row = സന്ദേശം_row;
    }

    // Save the starting line number.
    sourcing_lnum_മുകൾ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    എങ്കിൽ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, വരി_arg, വരികൾ_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	പോകുക erret;

    /*
     * If there are no errors, add the function
     */
    എങ്കിൽ (fudi.fd_dict != ശൂന്യ)
    {
	അക്ഷരം	numbuf[NUMBUFLEN];

	fp = ശൂന്യ;
	എങ്കിൽ (fudi.fd_newkey == ശൂന്യ && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    പോകുക erret;
	}
	എങ്കിൽ (fudi.fd_di == ശൂന്യ)
	{
	    // Can't add a function to a locked dictionary
	    എങ്കിൽ (മൂല്യം_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		പോകുക erret;
	}
	    // Can't change an existing function if it is locked
	അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    പോകുക erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, വലുപ്പം(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	എങ്കിൽ (name == ശൂന്യ)
	    പോകുക erret;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	പൂർണ്ണസംഖ്യ		var_conflict = FALSE;
	പൂർണ്ണസംഖ്യ		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	എങ്കിൽ (v != ശൂന്യ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	എങ്കിൽ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ)
	    {
		എങ്കിൽ (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    എങ്കിൽ (v != ശൂന്യ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		അല്ലെങ്കിൽ
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    എങ്കിൽ (prefixed != ശൂന്യ && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			എങ്കിൽ (v != ശൂന്യ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	എങ്കിൽ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    പോകുക erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	എങ്കിൽ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == ശൂന്യ ? name : uname, 0, FALSE);
	}

	എങ്കിൽ (fp != ശൂന്യ || import != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ dead = fp != ശൂന്യ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    എങ്കിൽ (import != ശൂന്യ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_മുകൾ;
		എങ്കിൽ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		അല്ലെങ്കിൽ
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		പോകുക errret_keep;
	    }
	    എങ്കിൽ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		പോകുക errret_keep;
	    }
	    എങ്കിൽ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ശൂന്യ;
		overwrite = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ശൂന്യ;
		func_clear_ഇനങ്ങൾ(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#ifdef FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#endif
		fp->uf_def_status = UF_NOT_COMPILED;
	    }
	}
    }

    എങ്കിൽ (fp == ശൂന്യ)
    {
	എങ്കിൽ (fudi.fd_dict == ശൂന്യ && vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    എങ്കിൽ (SOURCING_NAME != ശൂന്യ)
	    {
		scriptname = autoload_name(name);
		എങ്കിൽ (scriptname != ശൂന്യ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (പൂർണ്ണസംഖ്യ)STRLEN(p);
		    slen = (പൂർണ്ണസംഖ്യ)STRLEN(SOURCING_NAME);
		    എങ്കിൽ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    എങ്കിൽ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_മുകൾ;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		പോകുക erret;
	    }
	}

	എങ്കിൽ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	എങ്കിൽ (fp == ശൂന്യ)
	    പോകുക erret;
	fp_allocated = TRUE;

	എങ്കിൽ (fudi.fd_dict != ശൂന്യ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    എങ്കിൽ (func_name == ശൂന്യ)
	    {
		VIM_CLEAR(fp);
		പോകുക erret;
	    }
	    എങ്കിൽ (fudi.fd_di == ശൂന്യ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		എങ്കിൽ (fudi.fd_di == ശൂന്യ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    പോകുക erret;
		}
		എങ്കിൽ (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    പോകുക erret;
		}
	    }
	    അല്ലെങ്കിൽ
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_args = newargs;
    fp->uf_def_args = default_args;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    എങ്കിൽ (eap->cmdidx == CMD_def)
    {
	പൂർണ്ണസംഖ്യ	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	എങ്കിൽ (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_മുകൾ;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	പൂർണ്ണസംഖ്യ save_is_export = is_export;
	is_export = FALSE;

	എങ്കിൽ (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_എണ്ണം, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    പോകുക erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	എങ്കിൽ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    പോകുക erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    അല്ലെങ്കിൽ
	fp->uf_def_status = UF_NOT_COMPILED;

    എങ്കിൽ (fp_allocated)
    {
	// insert the new function in the function list
	എങ്കിൽ (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_താക്കോൽ = UF2HIKEY(fp);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    പോകുക erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_വരികൾ = newlines;
    newlines.ga_data = ശൂന്യ;
    എങ്കിൽ ((flags & FC_CLOSURE) != 0)
    {
	എങ്കിൽ (register_closure(fp) == FAIL)
	    പോകുക erret;
    }
    അല്ലെങ്കിൽ
	fp->uf_scoped = ശൂന്യ;

#ifdef FEAT_PROFILE
    എങ്കിൽ (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    എങ്കിൽ (sandbox)
	flags |= FC_SANDBOX;
    എങ്കിൽ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_മുകൾ;
    എങ്കിൽ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    എങ്കിൽ (eap->cmdidx == CMD_def)
	set_function_type(fp);
    അല്ലെങ്കിൽ എങ്കിൽ (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    എങ്കിൽ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ശൂന്യ);

    പോകുക ret_free;

erret:
    എങ്കിൽ (fp != ശൂന്യ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    എങ്കിൽ (fp != ശൂന്യ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	എങ്കിൽ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_ഇനങ്ങൾ(fp);
    }
    എങ്കിൽ (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_strings(&newargs);
    ga_clear_strings(&default_args);
    ga_clear_strings(&newlines);
ret_free:
    ga_clear_strings(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_args_table_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    എങ്കിൽ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    തിരികെ fp;
}

/*
 * ":function"
 */
    ശൂന്യം
ex_function(exarg_T *eap)
{
    garray_T വരികൾ_to_free;

    ga_init2(&വരികൾ_to_free, വലുപ്പം(char_u *), 50);
    (ശൂന്യം)define_function(eap, ശൂന്യ, &വരികൾ_to_free, 0, ശൂന്യ, 0, ശൂന്യ);
    ga_clear_strings(&വരികൾ_to_free);
}

    പൂർണ്ണസംഖ്യ
get_func_arity(char_u *name, പൂർണ്ണസംഖ്യ *required, പൂർണ്ണസംഖ്യ *optional, പൂർണ്ണസംഖ്യ *varargs)
{
    ufunc_T	*ufunc = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argcount = 0;
    പൂർണ്ണസംഖ്യ		min_argcount = 0;
    പൂർണ്ണസംഖ്യ		idx;

    idx = find_internal_func(name);
    എങ്കിൽ (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ശൂന്യ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	എങ്കിൽ (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	എങ്കിൽ (ufunc == ശൂന്യ)
	    തിരികെ FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    തിരികെ OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
find_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    char_u	*bracket_start = ശൂന്യ;
    char_u	*generic_func_name = name;

    എങ്കിൽ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    എങ്കിൽ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(ശൂന്യം)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    അല്ലെങ്കിൽ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	എങ്കിൽ (ufunc != ശൂന്യ)
	    തിരികെ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ശൂന്യ, ശൂന്യ, ശൂന്യ, &ufunc);
	എങ്കിൽ (ufunc != ശൂന്യ)
	{
	    vim_free(fname);
	    തിരികെ ufunc;
	}
	എങ്കിൽ (fname != ശൂന്യ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    എങ്കിൽ (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		തിരികെ ശൂന്യ;
	    }
	}
    }
    എങ്കിൽ (fname == ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str), name);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	തിരികെ ശൂന്യ;
    }

    ufunc = find_func(fname, is_global);
    എങ്കിൽ (ufunc == ശൂന്യ)
    {
	char_u *p = untrans_function_name(fname);

	എങ്കിൽ (p != ശൂന്യ)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	എങ്കിൽ (ufunc == ശൂന്യ)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    എങ്കിൽ (bracket_start != ശൂന്യ)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    എങ്കിൽ (ufunc != ശൂന്യ)
    {
	// handle generic functions
	എങ്കിൽ (bracket_start == ശൂന്യ)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    എങ്കിൽ (ufunc == ശൂന്യ)
	semsg(_(e_cannot_find_function_str), name);
    തിരികെ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    ശൂന്യം
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    എങ്കിൽ (func_needs_compiling(ufunc, compile_type))
	(ശൂന്യം)compile_def_function(ufunc, FALSE, compile_type, ശൂന്യ);
    അല്ലെങ്കിൽ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ശൂന്യ ? cl->class_name.string : (char_u *)"",
				cl != ശൂന്യ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    സ്ഥിതം ശൂന്യം
defcompile_funcs_in_script(ശൂന്യം)
{
    long	todo = (long)func_hashtab.ht_used;
    പൂർണ്ണസംഖ്യ		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    വേണ്ടി (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    എങ്കിൽ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(ശൂന്യം)compile_def_function(ufunc, FALSE, CT_NONE, ശൂന്യ);

		എങ്കിൽ (func_hashtab.ht_changed != changed)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (long)func_hashtab.ht_used;
		    changed = func_hashtab.ht_changed;
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
    ശൂന്യം
ex_defcompile(exarg_T *eap)
{
    എങ്കിൽ (*eap->arg != NUL)
    {
	typval_T tv;

	എങ്കിൽ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    എങ്കിൽ (cl != ശൂന്യ)
		defcompile_class(cl);
	}
	അല്ലെങ്കിൽ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    എങ്കിൽ (ufunc != ശൂന്യ)
		defcompile_function(ufunc, ശൂന്യ);
	}
    }
    അല്ലെങ്കിൽ
    {
	defcompile_funcs_in_script();

	// compile all the class defined in the current script
	defcompile_classes_in_script();
    }
}

/*
 * Return 5 if "p" starts with "<SID>" or "<SNR>" (ignoring case).
 * Return 2 if "p" starts with "s:".
 * Return 0 otherwise.
 */
    പൂർണ്ണസംഖ്യ
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    എങ്കിൽ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	തിരികെ 5;
    എങ്കിൽ (p[0] == 's' && p[1] == ':')
	തിരികെ 2;
    തിരികെ 0;
}

    പൂർണ്ണസംഖ്യ
translated_function_exists(char_u *name, പൂർണ്ണസംഖ്യ is_global)
{
    എങ്കിൽ (builtin_function(name, -1))
	തിരികെ has_internal_func(name);
    തിരികെ find_func(name, is_global) != ശൂന്യ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    പൂർണ്ണസംഖ്യ
has_varargs(ufunc_T *ufunc)
{
    തിരികെ ufunc->uf_varargs || ufunc->uf_va_name != ശൂന്യ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    പൂർണ്ണസംഖ്യ
function_exists(char_u *name, പൂർണ്ണസംഖ്യ no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    n = FALSE;
    പൂർണ്ണസംഖ്യ	    flag;
    പൂർണ്ണസംഖ്യ	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    എങ്കിൽ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    എങ്കിൽ (p != ശൂന്യ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    തിരികെ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    പൂർണ്ണസംഖ്യ
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    ufunc_T	*fp = ശൂന്യ;
    list_T	*args;
    പൂർണ്ണസംഖ്യ		is_def;
    അക്ഷരം	*tofree;

    എങ്കിൽ (*name == NUL)
	തിരികെ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    എങ്കിൽ (p != ശൂന്യ && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    എങ്കിൽ (fp == ശൂന്യ)
	തിരികെ FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    എങ്കിൽ (args == ശൂന്യ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	തിരികെ FAIL;
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	എങ്കിൽ (arg == ശൂന്യ || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    തിരികെ FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	എങ്കിൽ (fp->uf_arg_types == ശൂന്യ)
	    dict_add_string(arg, "type", (char_u *)"any");
	അല്ലെങ്കിൽ
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	എങ്കിൽ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    എങ്കിൽ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	എങ്കിൽ (va == ശൂന്യ || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    തിരികെ FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == ശൂന്യ ? (char_u *)"" : fp->uf_va_name);
	എങ്കിൽ (fp->uf_va_type == ശൂന്യ)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	അല്ലെങ്കിൽ
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    എങ്കിൽ (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    അല്ലെങ്കിൽ
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_സംഖ്യ(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_സംഖ്യ(d, "lnum", fp->uf_script_ctx.sc_lnum);
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, പൂർണ്ണസംഖ്യ check)
{
    char_u	*nm = name;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    എങ്കിൽ (p != ശൂന്യ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	തിരികെ p;

    vim_free(p);
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം long_u	done;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		changed;
    സ്ഥിതം hashitem_T	*hi;
    ufunc_T		*fp;

    എങ്കിൽ (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    എങ്കിൽ (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	പൂർണ്ണസംഖ്യ len;

	എങ്കിൽ (done++ > 0)
	    ++hi;
	ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	എങ്കിൽ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    തിരികെ (char_u *)"";

	എങ്കിൽ (fp->uf_namelen + 4 >= IOSIZE)
	    തിരികെ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	എങ്കിൽ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    എങ്കിൽ (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	തിരികെ IObuff;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, പൂർണ്ണസംഖ്യ extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    എങ്കിൽ (ufunc == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ശൂന്യ;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_എണ്ണം = ശൂന്യ;
    ufunc->uf_tml_total = ശൂന്യ;
    ufunc->uf_tml_self = ശൂന്യ;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    എങ്കിൽ (ufunc->uf_arg_types != ശൂന്യ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	എങ്കിൽ (at != ശൂന്യ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     വലുപ്പം(type_T *) * ufunc->uf_args.ga_len);
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

    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = ശൂന്യ;

    ga_copy_strings(&fp->uf_വരികൾ, &ufunc->uf_വരികൾ);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    എങ്കിൽ (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    തിരികെ ufunc;
}

/*
 * ":delfunction {name}"
 */
    ശൂന്യം
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ശൂന്യ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ശൂന്യ, ശൂന്യ, ശൂന്യ);
    vim_free(fudi.fd_newkey);
    എങ്കിൽ (name == ശൂന്യ)
    {
	എങ്കിൽ (fudi.fd_dict != ശൂന്യ && !eap->skip)
	    emsg(_(e_funcref_required));
	തിരികെ;
    }
    എങ്കിൽ (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	തിരികെ;
    }
    set_nextcmd(eap, p);
    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
	*p = NUL;

    എങ്കിൽ (numbered_function(name) && fudi.fd_dict == ശൂന്യ)
    {
	എങ്കിൽ (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	തിരികെ;
    }
    എങ്കിൽ (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    എങ്കിൽ (!eap->skip)
    {
	എങ്കിൽ (fp == ശൂന്യ)
	{
	    എങ്കിൽ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    തിരികെ;
	}
	എങ്കിൽ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    തിരികെ;
	}
	എങ്കിൽ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    തിരികെ;
	}

	എങ്കിൽ (fudi.fd_dict != ശൂന്യ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	അല്ലെങ്കിൽ
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    എങ്കിൽ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		എങ്കിൽ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    അല്ലെങ്കിൽ
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    ശൂന്യം
func_unref(char_u *name)
{
    ufunc_T *fp = ശൂന്യ;

    എങ്കിൽ (name == ശൂന്യ || !func_name_refcount(name))
	തിരികെ;
    fp = find_func(name, FALSE);
    എങ്കിൽ (fp == ശൂന്യ && numbered_function(name))
    {
#ifdef EXITFREE
	എങ്കിൽ (!entered_free_all_mem)
#endif
	    internal_error("func_unref()");
    }
    func_ptr_unref(fp);
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 * Also when it becomes one and uf_partial points to the function.
 */
    ശൂന്യം
func_ptr_unref(ufunc_T *fp)
{
    എങ്കിൽ (fp != ശൂന്യ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ശൂന്യ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	എങ്കിൽ (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    ശൂന്യം
func_ref(char_u *name)
{
    ufunc_T *fp;

    എങ്കിൽ (name == ശൂന്യ || !func_name_refcount(name))
	തിരികെ;
    fp = find_func(name, FALSE);
    എങ്കിൽ (fp != ശൂന്യ)
	++fp->uf_refcount;
    അല്ലെങ്കിൽ എങ്കിൽ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    ശൂന്യം
func_ptr_ref(ufunc_T *fp)
{
    എങ്കിൽ (fp != ശൂന്യ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
can_free_funccal(funccall_T *fc, പൂർണ്ണസംഖ്യ copyID)
{
    തിരികെ (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    ശൂന്യം
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		returning = FALSE;
    evalarg_T	evalarg;

    എങ്കിൽ (current_funccal == ശൂന്യ)
    {
	emsg(_(e_return_not_inside_function));
	തിരികെ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    എങ്കിൽ (eap->skip)
	++emsg_skip;

    eap->nextcmd = ശൂന്യ;
    എങ്കിൽ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	എങ്കിൽ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	അല്ലെങ്കിൽ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    അല്ലെങ്കിൽ എങ്കിൽ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	എങ്കിൽ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ശൂന്യ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    എങ്കിൽ (returning)
	eap->nextcmd = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->nextcmd == ശൂന്യ)	    // no argument
	set_nextcmd(eap, arg);

    എങ്കിൽ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		doesrange;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		failed = FALSE;

    lnum = eap->line1;
    വേണ്ടി ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	എങ്കിൽ (eap->addr_എണ്ണം > 0)
	{
	    എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		നിർത്തുക;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	എങ്കിൽ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    നിർത്തുക;
	}
	എങ്കിൽ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	എങ്കിൽ (handle_subscript(arg, ശൂന്യ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    നിർത്തുക;
	}

	clear_tv(&rettv);
	എങ്കിൽ (doesrange)
	    നിർത്തുക;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	എങ്കിൽ (aborting())
	    നിർത്തുക;
    }
    തിരികെ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    പൂർണ്ണസംഖ്യ		partial_argc = 0;		// number of partial arguments
    പൂർണ്ണസംഖ്യ		argcount = 0;			// number of arguments found
    പൂർണ്ണസംഖ്യ		r;

    എങ്കിൽ (current_funccal == ശൂന്യ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	തിരികെ FAIL;
    }
    എങ്കിൽ (partial != ശൂന്യ)
    {
	എങ്കിൽ (partial->pt_dict != ശൂന്യ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (partial->pt_argc > 0)
	{
	    പൂർണ്ണസംഖ്യ i;

	    partial_argc = partial->pt_argc;
	    വേണ്ടി (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    പൂർണ്ണസംഖ്യ is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    എങ്കിൽ (r == OK)
    {
	എങ്കിൽ (type != ശൂന്യ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, ശൂന്യ, name);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (is_builtin)
	{
	    പൂർണ്ണസംഖ്യ idx = find_internal_func(name);

	    എങ്കിൽ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    എങ്കിൽ (ufunc != ശൂന്യ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		എങ്കിൽ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    എങ്കിൽ (r == FAIL)
    {
	ആകുമ്പോൾ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	തിരികെ FAIL;
    }
    തിരികെ add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    പൂർണ്ണസംഖ്യ
can_add_defer(ശൂന്യം)
{
    എങ്കിൽ (!in_def_function() && get_current_funccal() == ശൂന്യ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
add_defer(char_u *name, പൂർണ്ണസംഖ്യ argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    പൂർണ്ണസംഖ്യ		argcount = argcount_arg;
    defer_T	*dr;
    പൂർണ്ണസംഖ്യ		ret = FAIL;

    എങ്കിൽ (saved_name == ശൂന്യ)
	പോകുക theend;
    എങ്കിൽ (in_def_function())
    {
	എങ്കിൽ (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, വലുപ്പം(defer_T), 10);
	എങ്കിൽ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    പോകുക theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ആകുമ്പോൾ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ആകുമ്പോൾ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    തിരികെ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    സ്ഥിതം ശൂന്യം
handle_defer_one(funccall_T *funccal)
{
    പൂർണ്ണസംഖ്യ	    idx;

    വേണ്ടി (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	എങ്കിൽ (dr->dr_name == ശൂന്യ)
	    // already being called, can happen if function does ":qa"
	    തുടരുക;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ശൂന്യ;

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
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    സ്ഥിതം ശൂന്യം
invoke_funccall_defer(funccall_T *fc)
{
    എങ്കിൽ (fc->fc_ectx != ശൂന്യ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    അല്ലെങ്കിൽ
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    ശൂന്യം
invoke_all_defer(ശൂന്യം)
{
    വേണ്ടി (funccall_T *fc = current_funccal; fc != ശൂന്യ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    വേണ്ടി (funccal_entry_T *fce = funccal_അട്ടി; fce != ശൂന്യ; fce = fce->next)
	വേണ്ടി (funccall_T *fc = fce->മുകൾ_funccal; fc != ശൂന്യ; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    ശൂന്യം
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ശൂന്യ;
    partial_T	*partial = ശൂന്യ;
    evalarg_T	evalarg;
    type_T	*type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		found_var = FALSE;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    എങ്കിൽ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	എങ്കിൽ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	തിരികെ;
    }

    tofree = trans_function_name_ext(&arg, ശൂന്യ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ശൂന്യ, &ufunc);
    എങ്കിൽ (fudi.fd_newkey != ശൂന്യ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    എങ്കിൽ (tofree == ശൂന്യ)
	തിരികെ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    എങ്കിൽ (fudi.fd_dict != ശൂന്യ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (പൂർണ്ണസംഖ്യ)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ശൂന്യ ? ശൂന്യ : &partial,
				vim9script && type == ശൂന്യ ? &type : ശൂന്യ,
						     FALSE, FALSE, &found_var);

    എങ്കിൽ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	എങ്കിൽ (name == ശൂന്യ)
	    പോകുക end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    എങ്കിൽ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	പോകുക end;
    }
    എങ്കിൽ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	പോകുക end;
    }

    എങ്കിൽ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    അല്ലെങ്കിൽ
    {
	funcexe_T funcexe;

	CLEAR_FIELD(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_found_var = found_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    എങ്കിൽ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	എങ്കിൽ (!ends_excmd2(eap->arg, arg))
	{
	    എങ്കിൽ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	അല്ലെങ്കിൽ
	    set_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

end:
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
    പൂർണ്ണസംഖ്യ
do_return(
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		reanimate,
    പൂർണ്ണസംഖ്യ		is_cmd,
    ശൂന്യം	*rettv)
{
    പൂർണ്ണസംഖ്യ		idx;
    cstack_T	*cstack = eap->cstack;

    എങ്കിൽ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    എങ്കിൽ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	എങ്കിൽ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	അല്ലെങ്കിൽ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    എങ്കിൽ (reanimate)
		rettv = current_funccal->fc_rettv;

	    എങ്കിൽ (rettv != ശൂന്യ)
	    {
		// Store the value of the pending return.
		എങ്കിൽ ((cstack->cs_rettv[idx] = alloc_tv()) != ശൂന്യ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		അല്ലെങ്കിൽ
		    emsg(_(e_out_of_memory));
	    }
	    അല്ലെങ്കിൽ
		cstack->cs_rettv[idx] = ശൂന്യ;

	    എങ്കിൽ (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_സംഖ്യ;
		current_funccal->fc_rettv->vval.v_സംഖ്യ = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    അല്ലെങ്കിൽ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	എങ്കിൽ (!reanimate && rettv != ശൂന്യ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    എങ്കിൽ (!is_cmd)
		vim_free(rettv);
	}
    }

    തിരികെ idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    ശൂന്യം
discard_pending_return(ശൂന്യം *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(ശൂന്യം *rettv)
{
    char_u	*s = ശൂന്യ;
    char_u	*tofree = ശൂന്യ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    എങ്കിൽ (rettv != ശൂന്യ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    എങ്കിൽ (s == ശൂന്യ)
	s = (char_u *)"";
    അല്ലെങ്കിൽ
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    എങ്കിൽ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    തിരികെ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_വരി(
    പൂർണ്ണസംഖ്യ	    c UNUSED,
    ശൂന്യം    *cookie,
    പൂർണ്ണസംഖ്യ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    എങ്കിൽ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
	func_വരി_end(cookie);
#endif

    gap = &fp->uf_വരികൾ;
    എങ്കിൽ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	// Skip NULL lines (continuation lines).
	ആകുമ്പോൾ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == ശൂന്യ)
	    ++fcp->fc_linenr;
	എങ്കിൽ (fcp->fc_linenr >= gap->ga_len)
	    retval = ശൂന്യ;
	അല്ലെങ്കിൽ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    എങ്കിൽ (do_profiling == PROF_YES)
		func_വരി_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    എങ്കിൽ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    തിരികെ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    പൂർണ്ണസംഖ്യ
func_has_ended(ശൂന്യം *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    തിരികെ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    പൂർണ്ണസംഖ്യ
func_has_abort(
    ശൂന്യം    *cookie)
{
    തിരികെ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_in, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = ശൂന്യ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    എങ്കിൽ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ശൂന്യ
				     && rettv->vval.v_partial->pt_func != ശൂന്യ)
	fp = rettv->vval.v_partial->pt_func;
    അല്ലെങ്കിൽ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ശൂന്യ ? ശൂന്യ
					      : rettv->vval.v_partial->pt_name;
	എങ്കിൽ (fname == ശൂന്യ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    char_u	*tofree = ശൂന്യ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    എങ്കിൽ (fp != ശൂന്യ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	എങ്കിൽ (pt != ശൂന്യ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ശൂന്യ;
	    എങ്കിൽ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    അല്ലെങ്കിൽ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		പൂർണ്ണസംഖ്യ		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		എങ്കിൽ (ret_pt->pt_name != ശൂന്യ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		അല്ലെങ്കിൽ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		എങ്കിൽ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    എങ്കിൽ (pt->pt_argv == ശൂന്യ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    അല്ലെങ്കിൽ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			വേണ്ടി (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    തിരികെ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(ശൂന്യം *cookie)
{
    തിരികെ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(ശൂന്യം *cookie)
{
    തിരികെ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    പൂർണ്ണസംഖ്യ *
func_dbg_tick(ശൂന്യം *cookie)
{
    തിരികെ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    പൂർണ്ണസംഖ്യ
func_level(ശൂന്യം *cookie)
{
    തിരികെ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    പൂർണ്ണസംഖ്യ
current_func_returned(ശൂന്യം)
{
    തിരികെ current_funccal->fc_returned;
}

    പൂർണ്ണസംഖ്യ
free_unref_funccal(പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ testing)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;
    പൂർണ്ണസംഖ്യ		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    വേണ്ടി (pfc = &previous_funccal; *pfc != ശൂന്യ; )
    {
	എങ്കിൽ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	അല്ലെങ്കിൽ
	    pfc = &(*pfc)->fc_caller;
    }
    എങ്കിൽ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(ശൂന്യം)garbage_collect(testing);

    തിരികെ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    സ്ഥിതം funccall_T *
get_funccal(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    എങ്കിൽ (debug_backtrace_level > 0)
    {
	വേണ്ടി (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    എങ്കിൽ (temp_funccal)
		funccal = temp_funccal;
	    അല്ലെങ്കിൽ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    തിരികെ funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    സ്ഥിതം funccall_T *
get_funccal_for_vars(ശൂന്യം)
{
    funccall_T	*funccal = ശൂന്യ;

    എങ്കിൽ (current_funccal == ശൂന്യ)
	തിരികെ ശൂന്യ;
    funccal = get_funccal();
    എങ്കിൽ (funccal == ശൂന്യ || funccal->fc_l_vars.dv_refcount == 0)
	തിരികെ ശൂന്യ;
    തിരികെ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(ശൂന്യം)
{
    funccall_T	*funccal = get_funccal_for_vars();

    തിരികെ funccal == ശൂന്യ ? ശൂന്യ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(ശൂന്യം)
{
    funccall_T	*funccal = get_funccal_for_vars();

    തിരികെ funccal == ശൂന്യ ? ശൂന്യ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(ശൂന്യം)
{
    funccall_T	*funccal = get_funccal_for_vars();

    തിരികെ funccal == ശൂന്യ ? ശൂന്യ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(ശൂന്യം)
{
    funccall_T	*funccal = get_funccal_for_vars();

    തിരികെ funccal == ശൂന്യ ? ശൂന്യ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    ശൂന്യം
list_func_vars(പൂർണ്ണസംഖ്യ *first)
{
    എങ്കിൽ (current_funccal != ശൂന്യ && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, first);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    എങ്കിൽ (current_funccal != ശൂന്യ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	തിരികെ &current_funccal->fc_l_vars;
    തിരികെ ശൂന്യ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ശൂന്യ;
    char_u	*varname;

    എങ്കിൽ (current_funccal == ശൂന്യ || current_funccal->fc_func->uf_scoped == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ആകുമ്പോൾ (current_funccal != ശൂന്യ)
    {
	ht = find_var_ht(name, &varname);
	എങ്കിൽ (ht != ശൂന്യ && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (current_funccal == current_funccal->fc_func->uf_scoped)
	    നിർത്തുക;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    തിരികെ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, പൂർണ്ണസംഖ്യ no_autoload)
{
    dictitem_T	*v = ശൂന്യ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    എങ്കിൽ (current_funccal == ശൂന്യ || current_funccal->fc_func->uf_scoped == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ആകുമ്പോൾ (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	എങ്കിൽ (ht != ശൂന്യ && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    എങ്കിൽ (v != ശൂന്യ)
		നിർത്തുക;
	}
	എങ്കിൽ (current_funccal == current_funccal->fc_func->uf_scoped)
	    നിർത്തുക;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    തിരികെ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_previous_funccal(പൂർണ്ണസംഖ്യ copyID)
{
    funccall_T	*fc;

    വേണ്ടി (fc = previous_funccal; fc != ശൂന്യ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	എങ്കിൽ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ശൂന്യ, ശൂന്യ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ശൂന്യ, ശൂന്യ)
		|| set_ref_in_list_ഇനങ്ങൾ(&fc->fc_l_varlist, copyID + 1, ശൂന്യ, ശൂന്യ))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_ref_in_funccal(funccall_T *fc, പൂർണ്ണസംഖ്യ copyID)
{
    എങ്കിൽ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	എങ്കിൽ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, ശൂന്യ, ശൂന്യ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, ശൂന്യ, ശൂന്യ)
		|| set_ref_in_list_ഇനങ്ങൾ(&fc->fc_l_varlist, copyID, ശൂന്യ, ശൂന്യ)
		|| set_ref_in_func(ശൂന്യ, fc->fc_func, copyID))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_call_അട്ടി(പൂർണ്ണസംഖ്യ copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    വേണ്ടി (fc = current_funccal; fc != ശൂന്യ; fc = fc->fc_caller)
	എങ്കിൽ (set_ref_in_funccal(fc, copyID))
	    തിരികെ TRUE;

    // Also go through the funccal_stack.
    വേണ്ടി (entry = funccal_അട്ടി; entry != ശൂന്യ; entry = entry->next)
	വേണ്ടി (fc = entry->മുകൾ_funccal; fc != ശൂന്യ; fc = fc->fc_caller)
	    എങ്കിൽ (set_ref_in_funccal(fc, copyID))
		തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_functions(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi = ശൂന്യ;
    ufunc_T	*fp;

    todo = (പൂർണ്ണസംഖ്യ)func_hashtab.ht_used;
    വേണ്ടി (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    എങ്കിൽ (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(ശൂന്യ, fp, copyID))
		തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_func_args(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < funcargs.ga_len; ++i)
	എങ്കിൽ (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_func(char_u *name, ufunc_T *fp_in, പൂർണ്ണസംഖ്യ copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ശൂന്യ;
    char_u	*fname;
    പൂർണ്ണസംഖ്യ		abort = FALSE;

    എങ്കിൽ (name == ശൂന്യ && fp_in == ശൂന്യ)
	തിരികെ FALSE;

    എങ്കിൽ (fp_in == ശൂന്യ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    എങ്കിൽ (fp != ശൂന്യ)
    {
	വേണ്ടി (fc = fp->uf_scoped; fc != ശൂന്യ; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    തിരികെ abort;
}

#endif // FEAT_EVAL
