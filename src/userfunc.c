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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
স্থির hashtab_T	func_hashtab;

// Used by get_func_tv()
স্থির garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
স্থির funccall_T *current_funccal = ফাঁকা;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
স্থির funccall_T *previous_funccal = ফাঁকা;

স্থির শূন্য funccal_unref(funccall_T *fc, ufunc_T *fp, পূর্ণসংখ্যা force);
স্থির শূন্য func_clear(ufunc_T *fp, পূর্ণসংখ্যা force);
স্থির পূর্ণসংখ্যা func_free(ufunc_T *fp, পূর্ণসংখ্যা force);
স্থির char_u *untrans_function_name(char_u *name);
স্থির শূন্য handle_defer_one(funccall_T *funccal);
স্থির char_u *trans_function_name_ext(char_u **pp, পূর্ণসংখ্যা *is_global, পূর্ণসংখ্যা skip, পূর্ণসংখ্যা flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    শূন্য
func_init(শূন্য)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(শূন্য)
{
    ফেরত &func_hashtab;
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
    স্থির char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	পূর্ণসংখ্যা	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	পূর্ণসংখ্যা	    is_vararg,
	পূর্ণসংখ্যা	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ফাঁকা;
    পূর্ণসংখ্যা		is_underscore = FALSE;

    যখন (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    যদি (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == ফাঁকা
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	যদি (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	ফেরত arg;
    }

    // Extra checks in Vim9 script.
    যদি (!skip && argtypes != ফাঁকা)
    {
	পূর্ণসংখ্যা c = *p;
	*p = NUL;
	পূর্ণসংখ্যা r = check_reserved_name(arg, FALSE);
	*p = c;
	যদি (r == FAIL)
	    ফেরত arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	যদি (check_defined(arg, p - arg,
				   evalarg == ফাঁকা ? ফাঁকা : evalarg->eval_cctx,
			       eap == ফাঁকা ? ফাঁকা : eap->cstack, TRUE) == FAIL)
	    ফেরত arg;
    }

    যদি (newargs != ফাঁকা && ga_grow(newargs, 1) == FAIL)
	ফেরত arg;
    যদি (newargs != ফাঁকা)
    {
	পূর্ণসংখ্যা	c;
	পূর্ণসংখ্যা	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	যদি (arg_copy == ফাঁকা)
	{
	    *p = c;
	    ফেরত arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	যদি (argtypes == ফাঁকা || !is_underscore)
	    // Check for duplicate argument name.
	    জন্য (i = 0; i < newargs->ga_len; ++i)
		যদি (STRCMP(((char_u **)(newargs->ga_তথ্য))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    ফেরত arg;
		}
	((char_u **)(newargs->ga_তথ্য))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    যদি (argtypes != ফাঁকা && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ফাঁকা && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ফাঁকা;

	যদি (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ফাঁকা ? arg : arg_copy);
	    p = skipwhite(p);
	}
	যদি (*p == ':')
	{
	    ++p;
	    যদি (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ফেরত arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    যদি (!skip)
		type = vim_strnsave(type, p - type);
	}
	অন্যথা যদি (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == ফাঁকা ? arg : arg_copy);
	    ফেরত arg;
	}
	যদি (!skip)
	{
	    যদি (type == ফাঁকা && types_optional)
	    {
		// lambda arguments default to "any" type
		যদি (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		অন্যথা
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_তথ্য)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_তথ্য)[arg_objm->ga_len++] = FALSE;
	}
    }

    ফেরত p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    স্থির char_u *
get_function_সারি(
	exarg_T		*eap,
	garray_T	*সারিগুলি_to_free,
	পূর্ণসংখ্যা		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    যদি (eap->ea_getline == ফাঁকা)
	theline = getcmdline(':', 0L, indent, 0);
    অন্যথা
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    যদি (theline != ফাঁকা)
    {
	যদি (সারিগুলি_to_free->ga_len > 0
		&& eap->cmdlinep != ফাঁকা
		&& *eap->cmdlinep == ((char_u **)সারিগুলি_to_free->ga_তথ্য)
						   [সারিগুলি_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(শূন্য)ga_add_string(সারিগুলি_to_free, theline);
    }

    ফেরত theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    স্থির পূর্ণসংখ্যা
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    পূর্ণসংখ্যা		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    পূর্ণসংখ্যা		*varargs,
    garray_T	*default_args,
    পূর্ণসংখ্যা		skip,
    exarg_T	*eap,		// can be NULL
    পূর্ণসংখ্যা		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*সারিগুলি_to_free)
{
    পূর্ণসংখ্যা		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		any_default = FALSE;
    char_u	*whitep = *argp;
    পূর্ণসংখ্যা		need_রাশিমালা = FALSE;

    যদি (newargs != ফাঁকা)
	ga_init2(newargs, আকার(char_u *), 3);
    যদি (argtypes != ফাঁকা)
	ga_init2(argtypes, আকার(char_u *), 3);
    যদি (arg_objm != ফাঁকা)
	ga_init2(arg_objm, আকার(int8_T), 3);
    যদি (!skip && default_args != ফাঁকা)
	ga_init2(default_args, আকার(char_u *), 3);

    যদি (varargs != ফাঁকা)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    যখন (*p != endchar)
    {
	যখন (eap != ফাঁকা && eap->ea_getline != ফাঁকা
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_সারি(eap, সারিগুলি_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    যদি (theline == ফাঁকা)
		বিরতি;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	যদি (mustend && *p != endchar)
	{
	    যদি (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    যাও err_ret;
	}
	যদি (*p == endchar && !need_রাশিমালা)
	    বিরতি;

	যদি (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    যদি (varargs != ফাঁকা)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    যদি (argtypes != ফাঁকা)
	    {
		// ...name: list<type>
		যদি (!eval_isnamec1(*p))
		{
		    যদি (!skip)
			emsg(_(e_missing_name_after_dots));
		    যাও err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		যদি (p == arg)
		    বিরতি;
		যদি (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    বিরতি;
		}
	    }
	}
	অন্যথা যদি (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    যখন (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    যদি (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		বিরতি;
	    }

	    যদি (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		যদি (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_মান_must_be_vnone_str), p);
		    যাও err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		যদি (ga_grow(default_args, 1) == FAIL)
		    যাও err_ret;

		char_u *রাশিমালা = vim_strnsave((char_u *)"v:none", 6);
		যদি (রাশিমালা == ফাঁকা)
		    যাও err_ret;
		((char_u **)(default_args->ga_তথ্য))
						 [default_args->ga_len] = রাশিমালা;
		default_args->ga_len++;
	    }
	    অন্যথা যদি (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		যাও err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    যদি (newargs != ফাঁকা && ga_grow(newargs, 1) == FAIL)
		ফেরত FAIL;
	    যদি (newargs != ফাঁকা)
	    {
		((char_u **)(newargs->ga_তথ্য))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		যদি (argtypes != ফাঁকা && ga_grow(argtypes, 1) == OK
			    && arg_objm != ফাঁকা && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_তথ্য)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_তথ্য)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    যদি (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			পূর্ণসংখ্যা len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			যদি (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			যদি (assignment != ফাঁকা)
			{
			    c = *argend;
			    *argend = NUL;
			    যদি (any_default)
				vim_snprintf((অক্ষর *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    অন্যথা
				vim_snprintf((অক্ষর *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_তথ্য))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    যদি (*p == ',')
		++p;
	}
	অন্যথা
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    যদি (p == arg)
		বিরতি;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    যদি (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != ফাঁকা)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *রাশিমালা = p;
		যদি (eval1(&p, &rettv, ফাঁকা) != FAIL)
		{
		    যদি (!skip)
		    {
			যদি (ga_grow(default_args, 1) == FAIL)
			    যাও err_ret;

			যদি (need_রাশিমালা)
			    need_রাশিমালা = FALSE;
			// trim trailing whitespace
			যখন (p > রাশিমালা && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			রাশিমালা = vim_strsave(রাশিমালা);
			যদি (রাশিমালা == ফাঁকা)
			{
			    *p = c;
			    যাও err_ret;
			}
			((char_u **)(default_args->ga_তথ্য))
						 [default_args->ga_len] = রাশিমালা;
			default_args->ga_len++;
			*p = c;
		    }
		}
		অন্যথা
		{
		    mustend = TRUE;
		    যদি (*skipwhite(p) == NUL)
			need_রাশিমালা = TRUE;
		}
	    }
	    অন্যথা যদি (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		যাও err_ret;
	    }

	    যদি (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		যদি (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    যাও err_ret;
		}
		p = skipwhite(p);
	    }
	    যদি (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		যদি (!skip && argtypes != ফাঁকা
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    যাও err_ret;
		}
	    }
	    অন্যথা
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    যদি (*p != endchar)
	যাও err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ফেরত OK;

err_ret:
    যদি (newargs != ফাঁকা)
	ga_clear_strings(newargs);
    যদি (!skip && default_args != ফাঁকা)
	ga_clear_strings(default_args);
    ফেরত FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    পূর্ণসংখ্যা		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    পূর্ণসংখ্যা		obj_member_গুনতি,
    cctx_T	*cctx)
{
    পূর্ণসংখ্যা len = 0;

    ga_init2(&fp->uf_type_list, আকার(type_T *), 10);
    যদি (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	যদি (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	যদি (fp->uf_arg_types != ফাঁকা)
	{
	    পূর্ণসংখ্যা	i;
	    type_T	*type;

	    জন্য (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_তথ্য)[i];

		যদি (p == ফাঁকা)
		    // will get the type from the default value
		    type = &t_unknown;
		অন্যথা
		{
		    যদি (arg_objm != ফাঁকা && ((int8_T *)arg_objm->ga_তথ্য)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_তথ্য)[i];

			type = &t_any;
			জন্য (পূর্ণসংখ্যা om = 0; om < obj_member_গুনতি; ++om)
			{
			    যদি (obj_members != ফাঁকা
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				বিরতি;
			    }
			}
		    }
		    অন্যথা
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		যদি (type == ফাঁকা || !valid_declaration_type(type))
		    ফেরত FAIL;
		fp->uf_arg_types[i] = type;
		যদি (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_তথ্য)[i];
		    যদি (obj_members != ফাঁকা && *name == '_')
			// protected object method
			name++;

		    যদি (var_wrong_func_name(name, TRUE))
			ফেরত FAIL;
		}
	    }
	}
    }

    যদি (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_তথ্য)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_তথ্য)[fp->uf_args.ga_len] = ফাঁকা;
	p = ((char_u **)argtypes->ga_তথ্য)[len];
	যদি (p == ফাঁকা)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	অন্যথা
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    যদি (fp->uf_va_type != ফাঁকা && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_তথ্য)[len]);
		ফেরত FAIL;
	    }
	}
	যদি (fp->uf_va_type == ফাঁকা)
	    ফেরত FAIL;
    }

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    যদি (ret_type == ফাঁকা)
	fp->uf_ret_type = &t_void;
    অন্যথা
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	যদি (fp->uf_ret_type == ফাঁকা)
	{
	    fp->uf_ret_type = &t_void;
	    ফেরত FAIL;
	}
    }
    ফেরত OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    স্থির পূর্ণসংখ্যা
register_closure(ufunc_T *fp)
{
    যদি (fp->uf_scoped == current_funccal)
	// no change
	ফেরত OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    যদি (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ফেরত FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_তথ্য)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ফেরত OK;
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

    যদি (name[0] != K_SPECIAL)
	ফেরত name;
    len = STRLEN(name);
    যদি (len + 3 > bufsize)
	ফেরত name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ফেরত buf;
}

স্থির char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(শূন্য)
{
    স্থির পূর্ণসংখ্যা	lambda_no = 0;
    পূর্ণসংখ্যা		n;
    string_T	ret;

    n = vim_snprintf((অক্ষর *)lambda_name, আকার(lambda_name), "<lambda>%d", ++lambda_no);
    যদি (n < 1)
	ret.দৈর্ঘ্য = 0;
    অন্যথা যদি (n >= (পূর্ণসংখ্যা)আকার(lambda_name))
	ret.দৈর্ঘ্য = আকার(lambda_name) - 1;
    অন্যথা
	ret.দৈর্ঘ্য = (size_t)n;

    ret.string = lambda_name;
    ফেরত ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    স্থির ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < আকার(ufunc_T) ? আকার(ufunc_T) : len);
    যদি (fp != ফাঁকা)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((শূন্য *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	যদি (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    যদি (fp->uf_name_exp != ফাঁকা)
		vim_snprintf((অক্ষর *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ফেরত fp;
}

#যদি defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, শূন্য *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.দৈর্ঘ্য);
    যদি (fp == ফাঁকা)
	ফেরত ফাঁকা;

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

    ফেরত name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    স্থির char_u *
skip_arrow(
	char_u	*start,
	পূর্ণসংখ্যা	equal_arrow,
	char_u	**ret_type,
	পূর্ণসংখ্যা	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    যদি (equal_arrow)
    {
	যদি (*s == ':')
	{
	    যদি (white_error != ফাঁকা && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ফেরত ফাঁকা;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    যদি (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ফেরত ফাঁকা;
	    }
	}
	bef = s;
	s = skipwhite(s);
	যদি (*s != '=')
	    ফেরত ফাঁকা;
	++s;
    }
    যদি (*s != '>')
	ফেরত ফাঁকা;
    যদি (white_error != ফাঁকা && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ফেরত ফাঁকা;
    }
    ফেরত skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    স্থির পূর্ণসংখ্যা
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    যদি (checkforcmd(&p, "function", 2))
    {
	যদি (*p == '(')
	    ফেরত FALSE;
	*cmd = p;
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    স্থির শূন্য
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    যদি (cstack == ফাঁকা || cstack->cs_idx < 0)
	ফেরত;

    পূর্ণসংখ্যা	    গুনতি = cstack->cs_idx + 1;
    পূর্ণসংখ্যা	    i;

    fp->uf_block_ids = ALLOC_MULT(পূর্ণসংখ্যা, গুনতি);
    যদি (fp->uf_block_ids != ফাঁকা)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		আকার(পূর্ণসংখ্যা) * গুনতি);
	fp->uf_block_depth = গুনতি;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    জন্য (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    স্থির char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    যখন (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	যদি (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	অন্যথা যদি (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    বিরতি;
	}
	অন্যথা
	    MB_PTR_ADV(p);
    }

    ফেরত p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    স্থির char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    যখন (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	যদি (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	অন্যথা যদি (*p == '"')
	{
	    p++;    // skip the ending double quote
	    বিরতি;
	}
	অন্যথা
	    MB_PTR_ADV(p);
    }

    ফেরত p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    স্থির char_u *
find_start_of_vim9_comment(char_u *সারি)
{
    char_u	*p = সারি;

    যখন (*p != NUL)
    {
	যদি (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	অন্যথা যদি (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	অন্যথা
	{
	    যদি (*p == '#')
		// Found the start of a Vim9 comment
		বিরতি;
	    MB_PTR_ADV(p);
	}
    }

    ফেরত p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    স্থির পূর্ণসংখ্যা
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *সারি_arg_in,
	garray_T    *সারিগুলি_to_free)
{
    linenr_T	sourcing_lnum_শীর্ষ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    পূর্ণসংখ্যা		saved_wait_return = need_wait_return;
    char_u	*সারি_arg = সারি_arg_in;
    পূর্ণসংখ্যা		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    অক্ষর	nesting_def[MAX_FUNC_NESTING];
    অক্ষর	nesting_inline[MAX_FUNC_NESTING];
    পূর্ণসংখ্যা		nesting = 0;
    getline_opt_T getline_options;
    পূর্ণসংখ্যা		indent = 2;
    char_u	*skip_until = ফাঁকা;
    পূর্ণসংখ্যা		ret = FAIL;
    পূর্ণসংখ্যা		is_heredoc = FALSE;
    পূর্ণসংখ্যা		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ফাঁকা;
    size_t	heredoc_trimmedlen = 0;
    পূর্ণসংখ্যা		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    যদি (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	যদি (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    যাও theend;
	যখন (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_তথ্য))[newlines->ga_len++] = ফাঁকা;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    জন্য (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	যদি (চাবি_Typed)
	{
	    বার্তা_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	যদি (সারি_arg != ফাঁকা)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = সারি_arg;
	    p = vim_strchr(theline, '\n');
	    যদি (p == ফাঁকা)
		সারি_arg += STRLEN(সারি_arg);
	    অন্যথা
	    {
		*p = NUL;
		সারি_arg = p + 1;
	    }
	}
	অন্যথা
	{
	    theline = get_function_সারি(eap, সারিগুলি_to_free, indent,
							      getline_options);
	}
	যদি (চাবি_Typed)
	    সারিগুলি_বাম = Rows - 1;
	যদি (theline == ফাঁকা)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_শীর্ষ;
	    যদি (skip_until != ফাঁকা)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    অন্যথা যদি (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    অন্যথা যদি (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    অন্যথা
		emsg(_(e_missing_endfunction));
	    যাও theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	যদি (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	অন্যথা
	    sourcing_lnum_off = 0;

	যদি (skip_until != ফাঁকা)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    যদি (heredoc_trimmed == ফাঁকা
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		যদি (heredoc_trimmed == ফাঁকা)
		    p = theline;
		অন্যথা যদি (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		অন্যথা
		    p = theline + heredoc_trimmedlen;
		যদি (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    যদি (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_তথ্য))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_তথ্য))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_তথ্য;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    জন্য (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		যদি (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    যদি (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		যদি (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		যদি (nesting-- == 0)
		{
		    char_u *nextcmd = ফাঁকা;

		    যদি (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    অন্যথা যদি (সারি_arg != ফাঁকা && *skipwhite(সারি_arg) != NUL)
			nextcmd = সারি_arg;
		    অন্যথা যদি (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_শীর্ষ
							+ newlines->ga_len + 1;
			যদি (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			অন্যথা
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    যদি (nextcmd != ফাঁকা && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			যদি (সারিগুলি_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)সারিগুলি_to_free->ga_তথ্য)
						   [সারিগুলি_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)সারিগুলি_to_free->ga_তথ্য)
						   [সারিগুলি_to_free->ga_len - 1];
			    --সারিগুলি_to_free->ga_len;
			}
		    }
		    বিরতি;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    অন্যথা যদি (nesting_def[nesting])
	    {
		যদি (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    অন্যথা যদি (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    যদি (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    অন্যথা যদি (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    যদি (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		যদি (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ফাঁকা, TRUE, 0));
		যদি (vim9_function && *p == '<')
		{
		    // skip generic function
		    যদি (skip_generic_func_type_args(&p) == FAIL)
			যাও theend;
		}
		যদি (*skipwhite(p) == '(')
		{
		    যদি (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    অন্যথা
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    যদি (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		যদি (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		অন্যথা
		    end = p + STRLEN(p) - 1;

		যখন (end > p && VIM_ISWHITE(*end))
		    --end;
		যদি (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    পূর্ণসংখ্যা	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    যখন (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    যদি (!is_block)
			is_block = find_cmd_block_start(p) != ফাঁকা;

		    যদি (is_block)
		    {
			যদি (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			অন্যথা
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
	    যদি (!vim9_function || colon)
		p = skip_range(p, FALSE, ফাঁকা);
	    char_u *tp = p;
	    যদি (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    অন্যথা
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    যদি (arg[0] == '<' && arg[1] =='<'
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
		যদি (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    যদি (heredoc_trimmed == ফাঁকা)
			heredoc_trimmedlen = 0;
		}
		যদি (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		অন্যথা
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		যদি (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    যদি (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		যদি (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    পূর্ণসংখ্যা		save_sc_version = current_sctx.sc_version;
		    পূর্ণসংখ্যা		var_গুনতি = 0;
		    পূর্ণসংখ্যা		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_গুনতি, &semicolon,
									 TRUE);
		    যদি (arg != ফাঁকা)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    যদি (arg != ফাঁকা && STRNCMP(arg, "=<<", 3) == 0)
		    {
			পূর্ণসংখ্যা has_trim = FALSE;

			p = skipwhite(arg + 3);
			যখন (TRUE)
			{
			    যদি (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				চালু;
			    }
			    যদি (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				চালু;
			    }
			    বিরতি;
			}
			যদি (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    যদি (heredoc_trimmed == ফাঁকা)
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
	যদি (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    যাও theend;

	যদি (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	অন্যথা
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	যদি (p == ফাঁকা)
	    যাও theend;
	((char_u **)(newlines->ga_তথ্য))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	যখন (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_তথ্য))[newlines->ga_len++] = ফাঁকা;

	// Check for end of eap->arg.
	যদি (সারি_arg != ফাঁকা && *সারি_arg == NUL)
	    সারি_arg = ফাঁকা;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    যদি (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_তথ্য);
    need_wait_return |= saved_wait_return;
    ফেরত ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    স্থির পূর্ণসংখ্যা
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	পূর্ণসংখ্যা	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    পূর্ণসংখ্যা		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ফাঁকা;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ফাঁকা;
    পূর্ণসংখ্যা		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    পূর্ণসংখ্যা		lnum_save = -1;
    linenr_T	sourcing_lnum_শীর্ষ = SOURCING_LNUM;
    char_u	*সারি_arg = ফাঁকা;

    *arg = skipwhite(*arg + 1);
    যদি (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ফেরত FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    যদি (**arg == '\n')
	সারি_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    যদি (evalarg->eval_cctx != ফাঁকা)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    অন্যথা
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, আকার(char_u *), 10);
    যদি (get_function_body(&eap, &newlines, সারি_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	যাও erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_গুনতি += newlines.ga_len;
    যদি (gap->ga_itemsize > 0)
    {
	পূর্ণসংখ্যা	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	জন্য (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_তথ্য)[idx];
	    যদি (p == ফাঁকা)
		// comment line in the lambda body
		চালু;

	    p = skipwhite(p);

	    যদি (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		যাও erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    যদি (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    অন্যথা
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    যদি (pnl != ফাঁকা)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_তথ্য)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_তথ্য)[freegap->ga_len++] = pnl;
	}
	যদি (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    যাও erret;
	যদি (eap.nextcmd != ফাঁকা)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	অন্যথা
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	যদি (pnl != ফাঁকা)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_তথ্য)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_তথ্য)[freegap->ga_len++] = pnl;
    }

    যদি (eap.nextcmd != ফাঁকা)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	যদি (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_তথ্য))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    অন্যথা
	*arg = (char_u *)"";

    যদি (!evaluate)
    {
	ret = OK;
	যাও erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.দৈর্ঘ্য);
    যদি (ufunc == ফাঁকা)
	যাও erret;
    যদি (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	যাও erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_তথ্য = ফাঁকা;
    ufunc->uf_def_args = *default_args;
    default_args->ga_তথ্য = ফাঁকা;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_শীর্ষ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    যদি (parse_argument_types(ufunc, argtypes, varargs, ফাঁকা, ফাঁকা, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	যাও erret;
    }

    // parse the return type, if any
    যদি (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	যাও erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    যদি (pt == ফাঁকা)
	যাও erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_সারিগুলি = newlines;
    newlines.ga_তথ্য = ফাঁকা;
    যদি (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    যদি (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_শীর্ষ;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ফাঁকা;
    ret = OK;

erret:
    যদি (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    যদি (newargs != ফাঁকা)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    যদি (ufunc != ফাঁকা)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ফেরত ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    পূর্ণসংখ্যা
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	পূর্ণসংখ্যা	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    পূর্ণসংখ্যা		evaluate = evalarg != ফাঁকা
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = ফাঁকা;
    partial_T   *pt = ফাঁকা;
    পূর্ণসংখ্যা		varargs;
    char_u	*ret_type = ফাঁকা;
    পূর্ণসংখ্যা		ret;
    char_u	*s;
    char_u	*start, *end;
    পূর্ণসংখ্যা		*old_eval_lavars = eval_lavars_used;
    পূর্ণসংখ্যা		eval_lavars = FALSE;
    char_u	*tofree2 = ফাঁকা;
    পূর্ণসংখ্যা		equal_arrow = **arg == '(';
    পূর্ণসংখ্যা		white_error = FALSE;
    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    যদি (equal_arrow && !vim9script)
	ফেরত NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', ফাঁকা,
	    types_optional ? &argtypes : ফাঁকা, types_optional,
			types_optional ? &arg_objm : ফাঁকা, evalarg,
			ফাঁকা, &default_args, TRUE, ফাঁকা, FALSE, ফাঁকা, ফাঁকা);
    যদি (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ফাঁকা) == ফাঁকা)
    {
	যদি (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ফেরত called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    যদি (evaluate)
	pnewargs = &newargs;
    অন্যথা
	pnewargs = ফাঁকা;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ফাঁকা, types_optional,
			types_optional ? &arg_objm : ফাঁকা, evalarg,
					    &varargs, &default_args,
					    FALSE, ফাঁকা, FALSE, ফাঁকা, ফাঁকা);
    যদি (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ফাঁকা)) == ফাঁকা)
    {
	যদি (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	ফেরত white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    যদি (ret_type != ফাঁকা)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    যদি (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    যদি (equal_arrow && **arg == '{')
    {
	যদি (evalarg == ফাঁকা)
	    // cannot happen?
	    যাও theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	যদি (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ফাঁকা, varargs,
			   &default_args, ret_type) == FAIL)
	    যাও errret;
	যাও theend;
    }
    যদি (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_মানসমূহ_in_lambda));
	যাও errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_রাশিমালা_concatenate(arg, &start, &end, evalarg);
    যদি (ret == FAIL)
	যাও errret;

    যদি (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg != '}')
	{
	    semsg(_(e_expected_ডান_curly_str), *arg);
	    যাও errret;
	}
	++*arg;
    }

    যদি (evaluate)
    {
	পূর্ণসংখ্যা	    len;
	পূর্ণসংখ্যা	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *সারি_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.দৈর্ঘ্য);
	যদি (fp == ফাঁকা)
	    যাও errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	যদি (pt == ফাঁকা)
	    যাও errret;

	ga_init2(&newlines, আকার(char_u *), 1);
	যদি (ga_grow(&newlines, 1) == FAIL)
	    যাও errret;

	// If there are line breaks, we need to split up the string.
	সারি_end = vim_strchr(start, '\n');
	যদি (সারি_end == ফাঁকা || সারি_end > end)
	    সারি_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (পূর্ণসংখ্যা)(সারি_end - start) + 1;
	p = alloc(len);
	যদি (p == ফাঁকা)
	    যাও errret;
	((char_u **)(newlines.ga_তথ্য))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, সারি_end - start);

	যদি (সারি_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    যখন (সারি_end < end && *সারি_end == '\n')
	    {
		যদি (ga_grow(&newlines, 1) == FAIL)
		    যাও errret;
		start = সারি_end + 1;
		সারি_end = vim_strchr(start, '\n');
		যদি (সারি_end == ফাঁকা || সারি_end > end)
		    সারি_end = end;
		((char_u **)(newlines.ga_তথ্য))[newlines.ga_len++] =
					 vim_strnsave(start, সারি_end - start);
	    }
	}

	যদি (strstr((অক্ষর *)p + 7, "a:") == ফাঁকা)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	যদি (types_optional)
	{
	    যদি (parse_argument_types(fp, &argtypes,
				vim9script && varargs, ফাঁকা, ফাঁকা, 0,
				cctx) == FAIL)
		যাও errret;
	    যদি (ret_type != ফাঁকা)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ফাঁকা, cctx, TRUE);
		যদি (fp->uf_ret_type == ফাঁকা)
		    যাও errret;
	    }
	    অন্যথা
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_সারিগুলি = newlines;
	যদি (current_funccal != ফাঁকা && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    যদি (register_closure(fp) == FAIL)
		যাও errret;
	}

#ifdef FEAT_PROFILE
	যদি (prof_def_func())
	    func_do_profile(fp);
#endif
	যদি (sandbox)
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
    যদি (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    ফেরত OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    যদি (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    যদি (fp != ফাঁকা)
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
    ফেরত FAIL;
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
	পূর্ণসংখ্যা	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	পূর্ণসংখ্যা	    no_autoload,
	পূর্ণসংখ্যা	    new_function,
	পূর্ণসংখ্যা	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = ফাঁকা;
    পূর্ণসংখ্যা		cc;
    char_u	*s = ফাঁকা;
    hashtab_T	*ht;
    পূর্ণসংখ্যা		did_type = FALSE;

    যদি (partialp != ফাঁকা)
	*partialp = ফাঁকা;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    যদি (v != ফাঁকা)
    {
	tv = &v->di_tv;
    }
    অন্যথা যদি (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	পূর্ণসংখ্যা	    len = *lenp;

	যদি (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	যদি (import != ফাঁকা)
	{
	    name[len] = NUL;
	    যদি (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    অন্যথা
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ফেরত (char_u *)"";	// just in case
	}
    }

    যদি (tv != ফাঁকা)
    {
	যদি (found_var != ফাঁকা)
	    *found_var = TRUE;
	যদি (tv->v_type == VAR_FUNC)
	{
	    যদি (tv->vval.v_string == ফাঁকা)
	    {
		*lenp = 0;
		ফেরত (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    যদি (p != ফাঁকা && generic_func_find_open_bracket(name) != ফাঁকা)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ফেরত (char_u *)"";
	    }
	    যদি (p != ফাঁকা)
		*lenp = p - s;
	    অন্যথা
		*lenp = (পূর্ণসংখ্যা)STRLEN(s);
	}
	অন্যথা যদি (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    যদি (pt == ফাঁকা)
	    {
		*lenp = 0;
		ফেরত (char_u *)"";	// just in case
	    }
	    যদি (partialp != ফাঁকা)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (পূর্ণসংখ্যা)STRLEN(s);
	}

	যদি (s != ফাঁকা)
	{
	    যদি (!did_type && type != ফাঁকা && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		যদি (sv != ফাঁকা)
		    *type = sv->sv_type;
	    }
	    ফেরত s;
	}
    }

    ফেরত name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    শূন্য
emsg_funcname(অক্ষর *ermsg, char_u *name)
{
    char_u	*p = name;

    যদি (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    যদি (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    পূর্ণসংখ্যা
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	পূর্ণসংখ্যা	    partial_argc,
	typval_T    *argvars,
	পূর্ণসংখ্যা	    *argcount,
	পূর্ণসংখ্যা	    is_builtin)
{
    char_u	*argp = *arg;
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		evaluate = evalarg == ফাঁকা
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    যখন (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	যদি (*argp == ')' || *argp == ',' || *argp == NUL)
	    বিরতি;

	পূর্ণসংখ্যা arg_idx = *argcount;
	যদি (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    বিরতি;
	}
	++*argcount;
	যদি (!is_builtin && check_typval_is_মান(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    বিরতি;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	যদি (vim9script)
	{
	    যদি (*argp != ',' && *skipwhite(argp) == ',')
	    {
		যদি (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		বিরতি;
	    }
	}
	অন্যথা
	    argp = skipwhite(argp);
	যদি (*argp != ',')
	    বিরতি;
	যদি (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    যদি (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    বিরতি;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    যদি (*argp == ')')
	++argp;
    অন্যথা
	ret = FAIL;
    *arg = argp;
    ফেরত ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
get_func_tv(
    char_u	*name,		// name of the function
    পূর্ণসংখ্যা		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    পূর্ণসংখ্যা		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    পূর্ণসংখ্যা		argcount = 0;			// number of arguments found
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		evaluate = evalarg == ফাঁকা
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ফাঁকা ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    যদি (ret == OK)
    {
	পূর্ণসংখ্যা	i = 0;
	পূর্ণসংখ্যা	did_emsg_before = did_emsg;

	যদি (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    যদি (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, আকার(typval_T *), 50);
	    জন্য (i = 0; i < argcount; ++i)
		যদি (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_তথ্য)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	যদি (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    অন্যথা যদি (!aborting() && evaluate)
    {
	যদি (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	অন্যথা
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    যখন (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    যদি (vim9script)
	*arg = argp;
    অন্যথা
	*arg = skipwhite(argp);
    ফেরত ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    স্থির পূর্ণসংখ্যা
eval_fname_sid(char_u *p)
{
    ফেরত (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    যদি (script_name == name)
	ফেরত name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (পূর্ণসংখ্যা)KE_SNR;
    fname_buflen = 3;
    যদি (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    অন্যথা
    {
	যদি (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	অন্যথা
	{
	    fname_buflen += vim_snprintf((অক্ষর *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    যদি (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    অন্যথা
    {
	fname = alloc(fnamelen + 1);
	যদি (fname == ফাঁকা)
	    *error = FCERR_OTHER;
	অন্যথা
	{
	    *tofree = fname;
	    vim_snprintf((অক্ষর *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ফেরত fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    শূন্য
func_name_with_sid(char_u *name, পূর্ণসংখ্যা sid, char_u *বাফার)
{
    // A script-local function is stored as "<SNR>99_name".
    বাফার[0] = K_SPECIAL;
    বাফার[1] = KS_EXTRA;
    বাফার[2] = (পূর্ণসংখ্যা)KE_SNR;
    vim_snprintf((অক্ষর *)বাফার + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    স্থির ufunc_T *
find_func_with_sid(char_u *name, পূর্ণসংখ্যা sid)
{
    hashitem_T	    *hi;
    char_u	    বাফার[MAX_FUNC_NAME_LEN];

    যদি (!SCRIPT_ID_VALID(sid))
	ফেরত ফাঁকা;	// not in a script

    func_name_with_sid(name, sid, বাফার);
    hi = hash_find(&func_hashtab, বাফার);
    যদি (!HASHITEM_EMPTY(hi))
	ফেরত HI2UF(hi);
    ফেরত ফাঁকা;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    স্থির ufunc_T *
find_func_with_prefix(char_u *name, পূর্ণসংখ্যা sid)
{
    hashitem_T	    *hi;
    char_u	    বাফার[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    যদি (vim_strchr(name, AUTOLOAD_CHAR) != ফাঁকা)
	ফেরত ফাঁকা;	// already has the prefix
    যদি (!SCRIPT_ID_VALID(sid))
	ফেরত ফাঁকা;	// not in a script
    si = SCRIPT_ITEM(sid);
    যদি (si->sn_autoload_prefix != ফাঁকা)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	যদি (namep == ফাঁকা)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	যদি (len < আকার(বাফার))
	    auto_name = বাফার;
	অন্যথা
	    auto_name = alloc(len);
	যদি (auto_name != ফাঁকা)
	{
	    vim_snprintf((অক্ষর *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    যদি (auto_name != বাফার)
		vim_free(auto_name);
	    যদি (!HASHITEM_EMPTY(hi))
		ফেরত HI2UF(hi);
	}
    }

    ফেরত ফাঁকা;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, পূর্ণসংখ্যা flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    যদি ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	যদি (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    যদি (func != ফাঁকা)
		ফেরত func;
	}
	যদি (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    যদি (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (পূর্ণসংখ্যা)sid);
		যদি (func != ফাঁকা)
		    ফেরত func;
	    }
	}
    }

    যদি ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	যদি (!HASHITEM_EMPTY(hi))
	    ফেরত HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ফেরত find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, পূর্ণসংখ্যা is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    যদি (fp != ফাঁকা && (fp->uf_flags & FC_DEAD) == 0)
	ফেরত fp;
    ফেরত ফাঁকা;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    পূর্ণসংখ্যা
func_is_global(ufunc_T *ufunc)
{
    ফেরত ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    পূর্ণসংখ্যা
func_requires_g_prefix(ufunc_T *ufunc)
{
    ফেরত func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ফাঁকা
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    স্থির পূর্ণসংখ্যা
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    পূর্ণসংখ্যা	len;

    যদি (!func_is_global(fp))
	len = vim_snprintf((অক্ষর *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    অন্যথা
	len = vim_snprintf((অক্ষর *)buf, bufsize, "%s", fp->uf_name);

    ফেরত (len >= (পূর্ণসংখ্যা)bufsize) ? (পূর্ণসংখ্যা)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    স্থির শূন্য
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    অক্ষর	*name,
    varnumber_T nr)
{
    STRCPY(v->di_চাবি, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_সংখ্যা;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_সংখ্যা = nr;
}

/*
 * Free "fc".
 */
    স্থির শূন্য
free_funccal(funccall_T *fc)
{
    পূর্ণসংখ্যা	i;

    জন্য (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_তথ্য))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	যদি (fp != ফাঁকা && fp->uf_scoped == fc)
	    fp->uf_scoped = ফাঁকা;
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
   স্থির শূন্য
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_বস্তুসমূহ(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    স্থির শূন্য
cleanup_function_call(funccall_T *fc)
{
    পূর্ণসংখ্যা	may_free_fc = fc->fc_refcount <= 0;
    পূর্ণসংখ্যা	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    যদি (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    অন্যথা
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    যদি (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    অন্যথা
    {
	পূর্ণসংখ্যা	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (পূর্ণসংখ্যা)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    যদি (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = ফাঁকা;
    অন্যথা
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_বস্তুসমূহ(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    যদি (free_fc)
	free_funccal(fc);
    অন্যথা
    {
	স্থির পূর্ণসংখ্যা made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	যদি (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	অন্যথা যদি (++made_copy >= (পূর্ণসংখ্যা)((4096 * 1024) / আকার(*fc)))
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
    স্থির পূর্ণসংখ্যা
numbered_function(char_u *name)
{
    ফেরত SAFE_isdigit(*name)
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
    পূর্ণসংখ্যা
func_name_refcount(char_u *name)
{
    ফেরত numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    স্থির শূন্য
funccal_unref(funccall_T *fc, ufunc_T *fp, পূর্ণসংখ্যা force)
{
    funccall_T	**pfc;
    পূর্ণসংখ্যা		i;

    যদি (fc == ফাঁকা)
	ফেরত;

    যদি (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	জন্য (pfc = &previous_funccal; *pfc != ফাঁকা; pfc = &(*pfc)->fc_caller)
	{
	    যদি (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ফেরত;
	    }
	}
    জন্য (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	যদি (((ufunc_T **)(fc->fc_ufuncs.ga_তথ্য))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_তথ্য))[i] = ফাঁকা;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    স্থির পূর্ণসংখ্যা
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    যদি (fp->uf_flags & FC_DEAD)
	ফেরত FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    যদি (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	ফেরত FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    যদি (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ফেরত FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ফেরত TRUE;
}

    স্থির শূন্য
func_clear_বস্তুসমূহ(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_সারিগুলি));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    যদি (IS_GENERIC_FUNC(fp))
	generic_func_clear_বস্তুসমূহ(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ফাঁকা;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    যদি (fp->uf_cb_free != ফাঁকা)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ফাঁকা;
    }

    fp->uf_cb_state = ফাঁকা;
    fp->uf_cb = ফাঁকা;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_গুনতি);
    VIM_CLEAR(fp->uf_tml_যোগ);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    স্থির শূন্য
func_clear(ufunc_T *fp, পূর্ণসংখ্যা force)
{
    যদি (fp->uf_cleared)
	ফেরত;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_বস্তুসমূহ(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    স্থির পূর্ণসংখ্যা
func_free(ufunc_T *fp, পূর্ণসংখ্যা force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    যদি ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    যদি ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	যদি (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ফেরত OK;
    }
    ফেরত FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    শূন্য
func_clear_free(ufunc_T *fp, পূর্ণসংখ্যা force)
{
    func_clear(fp, force);
    যদি (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    অন্যথা
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    পূর্ণসংখ্যা
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ফাঁকা;

    যদি (ufunc == ফাঁকা)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	ফেরত FAIL;
    }

    fp = find_func(global, TRUE);
    যদি (fp != ফাঁকা)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	ফেরত FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    যদি (fp == ফাঁকা)
	ফেরত FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    যদি (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_সারিগুলি, &fp->uf_সারিগুলি) == FAIL)
	যাও failed;

    যদি (ufunc->uf_arg_types != ফাঁকা)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	যদি (fp->uf_arg_types == ফাঁকা)
	    যাও failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    আকার(type_T *) * fp->uf_args.ga_len);
    }
    যদি (ufunc->uf_va_name != ফাঁকা)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	যদি (fp->uf_va_name == ফাঁকা)
	    যাও failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    যদি ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == ফাঁকা)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	যদি (pt == ফাঁকা)
	    যাও failed;
	যদি (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    যাও failed;
	}
	ufunc->uf_partial = pt;
    }

    ফেরত OK;

failed:
    func_clear_free(fp, TRUE);
    ফেরত FAIL;
}

স্থির পূর্ণসংখ্যা	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    পূর্ণসংখ্যা
funcdepth_increment(শূন্য)
{
    যদি (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	ফেরত FAIL;
    }
    ++funcdepth;
    ফেরত OK;
}

    শূন্য
funcdepth_decrement(শূন্য)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    পূর্ণসংখ্যা
funcdepth_get(শূন্য)
{
    ফেরত funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    শূন্য
funcdepth_restore(পূর্ণসংখ্যা depth)
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

    যদি (fc == ফাঁকা)
	ফেরত ফাঁকা;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ফেরত fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    শূন্য
remove_funccal(শূন্য)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    স্থির funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    পূর্ণসংখ্যা		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    পূর্ণসংখ্যা		using_sandbox = FALSE;
    পূর্ণসংখ্যা		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    পূর্ণসংখ্যা		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    পূর্ণসংখ্যা		default_arg_err = FALSE;
    dictitem_T	*v;
    পূর্ণসংখ্যা		fixvar_idx = 0;	// index in fc_fixvar[]
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		ai;
    পূর্ণসংখ্যা		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    পূর্ণসংখ্যা		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    যদি (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = -1;
	ফেরত FCERR_FAILED;
    }

    সারি_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    যদি (fc == ফাঁকা)
	ফেরত FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, আকার(ufunc_T *), 1);

    যদি (fp->uf_def_status != UF_NOT_COMPILED)
    {
	যদি (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ফাঁকা ? ফাঁকা : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	যদি (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	যদি (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	যদি (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	যদি (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ফাঁকা && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ফেরত retval;
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
    যদি (selfdict != ফাঁকা)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_চাবি;
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
    যদি ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    যদি ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_চাবি;
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
    যদি ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    জন্য (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	পূর্ণসংখ্যা	    addlocal = FALSE;
	typval_T    def_rettv;
	পূর্ণসংখ্যা	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	যদি (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    যদি (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_সংখ্যা == VVAL_NONE));
	    যদি (isdefault)
	    {
		char_u	    *default_রাশিমালা = ফাঁকা;

		def_rettv.v_type = VAR_সংখ্যা;
		def_rettv.vval.v_সংখ্যা = -1;

		default_রাশিমালা = ((char_u **)(fp->uf_def_args.ga_তথ্য))
						 [ai + fp->uf_def_args.ga_len];
		যদি (eval1(&default_রাশিমালা, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    বিরতি;
		}
	    }

	    namelen = STRLEN(name);
	}
	অন্যথা
	{
	    যদি ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		বিরতি;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((অক্ষর *)numbuf, আকার(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	যদি (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_চাবি, name);
	}
	অন্যথা
	{
	    v = dictitem_alloc(name);
	    যদি (v == ফাঁকা)
		বিরতি;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	যদি (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	যদি (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	অন্যথা
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	যদি (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    যদি (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ঠেলো_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    যদি (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	যদি (p_verbose >= 14)
	{
	    char_u	buf[বার্তা_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    বার্তা_puts("(");
	    জন্য (i = 0; i < argcount; ++i)
	    {
		যদি (i > 0)
		    বার্তা_puts(", ");
		যদি (argvars[i].v_type == VAR_সংখ্যা)
		    বার্তা_outnum((long)argvars[i].vval.v_সংখ্যা);
		অন্যথা
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    যদি (s != ফাঁকা)
		    {
			যদি (vim_strsize(s) > বার্তা_BUF_CLEN)
			{
			    trunc_string(s, buf, বার্তা_BUF_CLEN, বার্তা_BUF_LEN);
			    s = buf;
			}
			বার্তা_puts((অক্ষর *)s);
			vim_free(tofree);
		    }
		}
	    }
	    বার্তা_puts(")");
	}
	বার্তা_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == ফাঁকা ? ফাঁকা : fc->fc_caller->fc_func);
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

    যদি (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    অন্যথা যদি (islambda)
    {
	char_u *p = *(char_u **)fp->uf_সারিগুলি.ga_তথ্য + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(শূন্য)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    অন্যথা
	// call do_cmdline() to execute the lines
	do_cmdline(ফাঁকা, get_func_সারি, (শূন্য *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    যদি ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_সংখ্যা;
	rettv->vval.v_সংখ্যা = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	যদি (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ফাঁকা ? ফাঁকা : fc->fc_caller->fc_func;

	যদি (fp->uf_profiling || (caller != ফাঁকা && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    যদি (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	যদি (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	অন্যথা যদি (fc->fc_rettv->v_type == VAR_সংখ্যা)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_সংখ্যা);
	অন্যথা
	{
	    char_u	buf[বার্তা_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    যদি (s != ফাঁকা)
	    {
		যদি (vim_strsize(s) > বার্তা_BUF_CLEN)
		{
		    trunc_string(s, buf, বার্তা_BUF_CLEN, বার্তা_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	বার্তা_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_বের_করো();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    যদি (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    যদি (p_verbose >= 12 && SOURCING_NAME != ফাঁকা)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	বার্তা_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    জন্য (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    ফেরত retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, পূর্ণসংখ্যা argcount)
{
    পূর্ণসংখ্যা regular_args = fp->uf_args.ga_len;

    যদি (argcount < regular_args - fp->uf_def_args.ga_len)
	ফেরত FCERR_TOOFEW;
    অন্যথা যদি (!has_varargs(fp) && argcount > regular_args)
	ফেরত FCERR_TOOMANY;
    ফেরত FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	পূর্ণসংখ্যা	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    যদি (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ফেরত (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    যদি (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ফাঁকা)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    যদি (error != FCERR_UNKNOWN)
	ফেরত error;

    যদি ((fp->uf_flags & FC_DICT) && selfdict == ফাঁকা)
    {
	error = FCERR_DICT;
    }
    অন্যথা
    {
	পূর্ণসংখ্যা		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_অনুসন্ধান_patterns();
	যদি (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ফাঁকা);
	যদি (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	যদি (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_অনুসন্ধান_patterns();
    }

    ফেরত error;
}

স্থির funccal_entry_T *funccal_স্তূপ = ফাঁকা;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    শূন্য
save_funccal(funccal_entry_T *entry)
{
    entry->শীর্ষ_funccal = current_funccal;
    entry->পরবর্তী = funccal_স্তূপ;
    funccal_স্তূপ = entry;
    current_funccal = ফাঁকা;
}

    শূন্য
restore_funccal(শূন্য)
{
    যদি (funccal_স্তূপ == ফাঁকা)
	internal_error("restore_funccal()");
    অন্যথা
    {
	current_funccal = funccal_স্তূপ->শীর্ষ_funccal;
	funccal_স্তূপ = funccal_স্তূপ->পরবর্তী;
    }
}

    funccall_T *
get_current_funccal(শূন্য)
{
    ফেরত current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    পূর্ণসংখ্যা
at_script_level(শূন্য)
{
    ফেরত current_funccal == ফাঁকা && autocmd_match == ফাঁকা;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    শূন্য
delete_script_functions(পূর্ণসংখ্যা sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (পূর্ণসংখ্যা)KE_SNR;
    len = 3 + vim_snprintf((অক্ষর *)buf + 3, আকার(buf) - 3, "%d_", sid);

    যখন (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(&func_hashtab, hi, todo)
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		যদি (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    পূর্ণসংখ্যা changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    যদি (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			যদি (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    অন্যথা
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			যদি (changed != func_hashtab.ht_changed)
			    বিরতি;
		    }
		}
		--todo;
	    }
    }
}

#যদি defined(EXITFREE)
    শূন্য
free_all_functions(শূন্য)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    পূর্ণসংখ্যা		changed;

    // Clean up the current_funccal chain and the funccal stack.
    যখন (current_funccal != ফাঁকা)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	যদি (current_funccal == ফাঁকা && funccal_স্তূপ != ফাঁকা)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    যখন (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(&func_hashtab, hi, todo)
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		যদি (func_name_refcount(fp->uf_name))
		    ++skipped;
		অন্যথা
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    যদি (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			বিরতি;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    যখন (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(&func_hashtab, hi, todo)
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		যদি (func_name_refcount(fp->uf_name))
		    ++skipped;
		অন্যথা
		{
		    যদি (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			বিরতি;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    যদি (skipped == 0)
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
    পূর্ণসংখ্যা
builtin_function(char_u *name, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা i;

    যদি (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ফেরত FALSE;
    জন্য (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	যদি (name[i] == AUTOLOAD_CHAR)
	    ফেরত FALSE;
	যদি (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    যদি (name[i] == '.')
		ফেরত FALSE;
	    বিরতি;
	}
    }
    ফেরত TRUE;
}

    পূর্ণসংখ্যা
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
    পূর্ণসংখ্যা		argc = 0;
    পূর্ণসংখ্যা		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, item)
    {
	যদি (argc == MAX_FUNC_ARGS - (partial == ফাঁকা ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    বিরতি;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    যদি (item == ফাঁকা)
    {
	funcexe_T	funcexe;
	পূর্ণসংখ্যা		namelen = -1;

	যদি (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    যদি (p != ফাঁকা)
	    {
		যদি (skip_generic_func_type_args(&p) == FAIL)
		    যাও done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	যদি (partial != ফাঁকা)
	{
	    funcexe.fe_object = partial->pt_obj;
	    যদি (funcexe.fe_object != ফাঁকা)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    যখন (argc > 0)
	clear_tv(&argv[--argc]);

done:
    ফেরত r;
}

স্থির পূর্ণসংখ্যা callback_depth = 0;

    পূর্ণসংখ্যা
get_callback_depth(শূন্য)
{
    ফেরত callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    পূর্ণসংখ্যা
call_callback(
    callback_T	*callback,
    পূর্ণসংখ্যা		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    পূর্ণসংখ্যা		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    পূর্ণসংখ্যা		ret;

    যদি (callback->cb_name == ফাঁকা || *callback->cb_name == NUL)
	ফেরত FAIL;

    যদি (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ফেরত FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    যদি (callback->cb_partial != ফাঁকা)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	যদি (funcexe.fe_object != ফাঁকা)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    যদি (need_rethrow && current_exception != ফাঁকা && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ফেরত ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    পূর্ণসংখ্যা		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    যদি (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ফেরত -2;

    retval = tv_get_সংখ্যা_chk(&rettv, ফাঁকা);
    clear_tv(&rettv);
    ফেরত retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    শূন্য
user_func_error(funcerror_T error, char_u *name, পূর্ণসংখ্যা found_var)
{
    নির্বাচন (error)
    {
	ক্ষেত্র FCERR_UNKNOWN:
		যদি (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		অন্যথা
		    emsg_funcname(e_unknown_function_str, name);
		বিরতি;
	ক্ষেত্র FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		বিরতি;
	ক্ষেত্র FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		বিরতি;
	ক্ষেত্র FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		বিরতি;
	ক্ষেত্র FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		বিরতি;
	ক্ষেত্র FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		বিরতি;
	ক্ষেত্র FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		বিরতি;
	ক্ষেত্র FCERR_OTHER:
	ক্ষেত্র FCERR_FAILED:
		// assume the error message was already given
		বিরতি;
	ক্ষেত্র FCERR_NONE:
		বিরতি;
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
    স্থির funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	পূর্ণসংখ্যা	    argcount,
	পূর্ণসংখ্যা	    base_included,
	char_u	    *name)
{
    যদি (funcexe->fe_check_type != ফাঁকা && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	যদি (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? ফাঁকা : funcexe->fe_basetv,
			  name) == FAIL)
	    ফেরত FCERR_OTHER;
    }
    ফেরত FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    পূর্ণসংখ্যা
call_func(
    char_u	*funcname,	// name of the function
    পূর্ণসংখ্যা		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    পূর্ণসংখ্যা		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    পূর্ণসংখ্যা		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    পূর্ণসংখ্যা		i;
    ufunc_T	*fp = ফাঁকা;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ফাঁকা;
    char_u	*fname = ফাঁকা;
    char_u	*name = ফাঁকা;
    পূর্ণসংখ্যা		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    পূর্ণসংখ্যা		argv_clear = 0;
    পূর্ণসংখ্যা		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    যদি (partial != ফাঁকা)
	fp = partial->pt_func;
    যদি (fp == ফাঁকা)
	fp = funcexe->fe_ufunc;

    যদি (fp == ফাঁকা)
    {
	যদি (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    যদি (p != ফাঁকা)
	    {
		len = p - funcname;
		যদি (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ফাঁকা)
		    যাও theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	যদি (name == ফাঁকা)
	    যাও theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    যদি (funcexe->fe_doesrange != ফাঁকা)
	*funcexe->fe_doesrange = FALSE;

    যদি (partial != ফাঁকা)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	যদি (partial->pt_dict != ফাঁকা && (selfdict == ফাঁকা || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	যদি (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    জন্য (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		যদি (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    যাও theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    জন্য (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    যদি (funcexe->fe_check_type != ফাঁকা
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		জন্য (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    যদি (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != ফাঁকা) ? name : funcname);

    যদি (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	পূর্ণসংখ্যা	is_global = FALSE;

	// Skip "g:" before a function name.
	যদি (fp == ফাঁকা && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_সংখ্যা;	// default rettv is number zero
	rettv->vval.v_সংখ্যা = 0;
	error = FCERR_UNKNOWN;

	যদি (fp != ফাঁকা || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    যদি (fp == ফাঁকা)
	    {
		fp = find_func(rfname, is_global);
		যদি (fp != ফাঁকা && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ফাঁকা;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    যদি (fp == ফাঁকা
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ফাঁকা)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    যদি (fp == ফাঁকা && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    যদি (fp == ফাঁকা)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		যদি (p != ফাঁকা && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    যদি (partial == ফাঁকা && fp != ফাঁকা && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		যদি (fp == ফাঁকা)
		{
		    error = FCERR_FAILED;
		    যাও theend;
		}
	    }
	    অন্যথা যদি (generic_func_args_table_আকার(&gfatab) > 0)
	    {
		emsg_funcname(fp != ফাঁকা ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		যাও theend;
	    }

	    যদি (fp != ফাঁকা && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    অন্যথা যদি (fp != ফাঁকা)
	    {
		পূর্ণসংখ্যা need_arg_check = FALSE;
		যদি (funcexe->fe_check_type == ফাঁকা)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		যদি (funcexe->fe_argv_func != ফাঁকা)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		যদি (funcexe->fe_basetv != ফাঁকা)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, আকার(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		যদি (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != ফাঁকা) ? name : funcname);

		যদি (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	অন্যথা যদি (funcexe->fe_basetv != ফাঁকা)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	অন্যথা
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
    যদি (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    যদি (!aborting())
	user_func_error(error, (name != ফাঁকা) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    যখন (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    ফেরত ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    পূর্ণসংখ্যা
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    পূর্ণসংখ্যা		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ফাঁকা;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    পূর্ণসংখ্যা		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_সংখ্যা;	// default rettv is number zero
    rettv->vval.v_সংখ্যা = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    যদি (name == ফাঁকা)
	ফেরত ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    যদি (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    অন্যথা
	rfname = fname;
    fp = find_func(rfname, is_global);
    যদি (fp != ফাঁকা && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ফাঁকা;
    যদি (fp == ফাঁকা)
	ret = NOTDONE;
    অন্যথা যদি (fp != ফাঁকা && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    অন্যথা যদি (fp != ফাঁকা)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ফাঁকা);
	যদি (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ফেরত ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ফেরত fp->uf_name_exp != ফাঁকা ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    স্থির পূর্ণসংখ্যা
function_list_modified(পূর্ণসংখ্যা পূর্ব_ht_changed)
{
    যদি (পূর্ব_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    স্থির পূর্ণসংখ্যা
list_func_শীর্ষ(ufunc_T *fp, পূর্ণসংখ্যা indent)
{
    পূর্ণসংখ্যা		পূর্ব_ht_changed = func_hashtab.ht_changed;
    পূর্ণসংখ্যা		j;

    বার্তা_start();

    // a timer at the more prompt may have deleted the function
    যদি (function_list_modified(পূর্ব_ht_changed))
	ফেরত FAIL;

    যদি (indent)
	বার্তা_puts("   ");
    যদি (fp->uf_def_status != UF_NOT_COMPILED)
	বার্তা_puts("def ");
    অন্যথা
	বার্তা_puts("function ");
    বার্তা_puts((অক্ষর *)printable_func_name(fp));
    বার্তা_putchar('(');
    জন্য (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	যদি (j)
	    বার্তা_puts(", ");
	বার্তা_puts((অক্ষর *)FUNCARG(fp, j));
	যদি (fp->uf_arg_types != ফাঁকা)
	{
	    অক্ষর *tofree;

	    বার্তা_puts(": ");
	    বার্তা_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	যদি (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    বার্তা_puts(" = ");
	    বার্তা_puts(((অক্ষর **)(fp->uf_def_args.ga_তথ্য))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    যদি (fp->uf_varargs)
    {
	যদি (j)
	    বার্তা_puts(", ");
	বার্তা_puts("...");
    }
    যদি (fp->uf_va_name != ফাঁকা)
    {
	যদি (!fp->uf_varargs)
	{
	    যদি (j)
		বার্তা_puts(", ");
	    বার্তা_puts("...");
	}
	বার্তা_puts((অক্ষর *)fp->uf_va_name);
	যদি (fp->uf_va_type != ফাঁকা)
	{
	    অক্ষর *tofree;

	    বার্তা_puts(": ");
	    বার্তা_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    বার্তা_putchar(')');

    যদি (fp->uf_def_status != UF_NOT_COMPILED)
    {
	যদি (fp->uf_ret_type != &t_void)
	{
	    অক্ষর *tofree;

	    বার্তা_puts(": ");
	    বার্তা_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    অন্যথা যদি (fp->uf_flags & FC_ABORT)
	বার্তা_puts(" abort");
    যদি (fp->uf_flags & FC_RANGE)
	বার্তা_puts(" range");
    যদি (fp->uf_flags & FC_DICT)
	বার্তা_puts(" dict");
    যদি (fp->uf_flags & FC_CLOSURE)
	বার্তা_puts(" closure");
    বার্তা_clr_eos();
    যদি (p_verbose > 0)
	last_set_বার্তা(fp->uf_script_ctx);

    ফেরত OK;
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
    পূর্ণসংখ্যা		*is_global,
    পূর্ণসংখ্যা		skip,		// only find the end, don't evaluate
    পূর্ণসংখ্যা		flags)
{
    ফেরত trans_function_name_ext(pp, is_global, skip, flags,
	    ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    স্থির char_u *
trans_function_name_ext(
    char_u	**pp,
    পূর্ণসংখ্যা		*is_global,
    পূর্ণসংখ্যা		skip,		// only find the end, don't evaluate
    পূর্ণসংখ্যা		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ফাঁকা;
    char_u	*start;
    char_u	*end;
    পূর্ণসংখ্যা		lead;
    char_u	sid_buf[20];
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		extra = 0;
    পূর্ণসংখ্যা		prefix_g = FALSE;
    lval_T	lv;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		vim9_local;

    যদি (fdp != ফাঁকা)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    যদি ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (পূর্ণসংখ্যা)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ফেরত vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    যদি (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, ফাঁকা, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    যদি (end == start || (vim9script && end != ফাঁকা
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	যদি (!skip)
	    emsg(_(e_function_name_required));
	যাও theend;
    }
    যদি (end == ফাঁকা || (lv.ll_tv != ফাঁকা && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	যদি (!aborting())
	{
	    যদি (end != ফাঁকা)
		semsg(_(e_invalid_argument_str), start);
	}
	অন্যথা
	    *pp = find_name_end(start, ফাঁকা, ফাঁকা, FNE_INCL_BR);
	যাও theend;
    }

    যদি (lv.ll_ufunc != ফাঁকা)
    {
	যদি (ufunc != ফাঁকা)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	যাও theend;
    }

    যদি (lv.ll_tv != ফাঁকা)
    {
	যদি (fdp != ফাঁকা)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ফাঁকা;
	    fdp->fd_di = lv.ll_di;
	}
	যদি (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ফাঁকা)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	অন্যথা যদি (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ফাঁকা)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.দৈর্ঘ্য);
	    *pp = end;
	}
	অন্যথা যদি (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ফাঁকা)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    যদি (partial != ফাঁকা)
		*partial = lv.ll_tv->vval.v_partial;
	}
	অন্যথা
	{
	    যদি (!skip && !(flags & TFN_QUIET) && (fdp == ফাঁকা
			     || lv.ll_dict == ফাঁকা || fdp->fd_newkey == ফাঁকা))
		emsg(_(e_funcref_required));
	    অন্যথা
		*pp = end;
	    name = ফাঁকা;
	}
	যাও theend;
    }

    যদি (lv.ll_name == ফাঁকা)
    {
	// Error found, but continue after the function name.
	*pp = end;
	যাও theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    যদি (lv.ll_exp_name != ফাঁকা)
    {
	len = (পূর্ণসংখ্যা)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ফাঁকা);
	যদি (name == lv.ll_exp_name)
	    name = ফাঁকা;
    }
    অন্যথা যদি (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	পূর্ণসংখ্যা		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	যদি (si->sn_autoload_prefix != ফাঁকা)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	অন্যথা
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (পূর্ণসংখ্যা)KE_SNR;
	    vim_snprintf((অক্ষর *)sid_buf + 3, আকার(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	যাও theend;
    }
    অন্যথা যদি (!(flags & TFN_NO_DEREF))
    {
	len = (পূর্ণসংখ্যা)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ফাঁকা);
	যদি (name == *pp)
	    name = ফাঁকা;
    }
    যদি (name != ফাঁকা)
    {
	name = vim_strsave(name);
	*pp = end;
	যদি (name != ফাঁকা && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (পূর্ণসংখ্যা)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	যাও theend;
    }

    যদি (lv.ll_exp_name != ফাঁকা)
    {
	len = (পূর্ণসংখ্যা)STRLEN(lv.ll_exp_name);
	যদি (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    অন্যথা
    {
	// skip over "s:" and "g:"
	যদি (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    যদি (lv.ll_name[0] == 'g')
	    {
		যদি (is_global != ফাঁকা)
		{
		    *is_global = TRUE;
		}
		অন্যথা
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (পূর্ণসংখ্যা)(end - lv.ll_name);
    }
    যদি (len <= 0)
    {
	যদি (!skip)
	    emsg(_(e_function_name_required));
	যাও theend;
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
    যদি (skip)
	lead = 0;	// do nothing
    অন্যথা যদি (lead > 0 || vim9_local)
    {
	যদি (!vim9_local)
	{
	    যদি (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		যাও theend;
	    }
	    lead = 3;
	}
	যদি (vim9_local || (lv.ll_exp_name != ফাঁকা
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    যদি (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		যাও theend;
	    }
	    sid_buflen = vim_snprintf((অক্ষর *)sid_buf, আকার(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    যদি (vim9_local)
		extra = 3 + (পূর্ণসংখ্যা)sid_buflen;
	    অন্যথা
		lead += (পূর্ণসংখ্যা)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    অন্যথা যদি (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	যাও theend;
    }
    যদি (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	যদি (cp != ফাঁকা && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    যাও theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    যদি (name != ফাঁকা)
    {
	যদি (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (পূর্ণসংখ্যা)KE_SNR;
	    যদি (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	অন্যথা যদি (prefix_g)
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
    ফেরত name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    স্থির char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    যদি (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	যদি (p != ফাঁকা)
	    ফেরত p + 1;
    }
    ফেরত ফাঁকা;
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
    অক্ষর	sid_buf[25];
    size_t	sid_buflen;
    পূর্ণসংখ্যা		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    যদি (funcname == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	যদি (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ফেরত ফাঁকা;
	ufunc = find_func(funcname, FALSE);
	যদি (ufunc == ফাঁকা || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ফাঁকা)
	    ফেরত ফাঁকা;
	++p;
	off = 0;
    }
    অন্যথা
	off = *funcname == 's' ? 2 : 5;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	ফেরত ফাঁকা;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, আকার(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    যদি (newname == ফাঁকা)
	ফেরত ফাঁকা;
    vim_snprintf((অক্ষর *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ফেরত newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    যদি (n != ফাঁকা)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ফেরত n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	পূর্ণসংখ্যা	    *is_global,
	পূর্ণসংখ্যা	    skip,
	পূর্ণসংখ্যা	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    যদি (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(শূন্য)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	যদি (fudi != ফাঁকা)
	    CLEAR_POINTER(fudi);
    }
    অন্যথা
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ফাঁকা, ফাঁকা, ফাঁকা);
    *name = p;
    ফেরত saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    শূন্য
list_functions(regmatch_T *regmatch)
{
    পূর্ণসংখ্যা		পূর্ব_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    জন্য (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    যদি ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ফাঁকা
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		যদি (list_func_শীর্ষ(fp, FALSE) == FAIL)
		    ফেরত;
		যদি (function_list_modified(পূর্ব_ht_changed))
		    ফেরত;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    স্থির char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    যদি (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	যদি (regmatch.regprog != ফাঁকা)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    যদি (*p == '/')
	++p;

    ফেরত p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    স্থির ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, পূর্ণসংখ্যা is_global)
{
    ufunc_T	*fp = ফাঁকা;
    পূর্ণসংখ্যা		j;

    যদি (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ফেরত ফাঁকা;
    }

    set_nextcmd(eap, p);

    যদি (eap->nextcmd != ফাঁকা)
	*p = NUL;

    যদি (eap->skip || got_int)
	ফেরত ফাঁকা;

    fp = find_func(name, is_global);
    যদি (fp == ফাঁকা && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	যদি (up != ফাঁকা)
	    fp = find_func(up, FALSE);
    }

    যদি (fp == ফাঁকা)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ফেরত ফাঁকা;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    পূর্ণসংখ্যা পূর্ব_ht_changed = func_hashtab.ht_changed;

    যদি (list_func_শীর্ষ(fp, TRUE) != OK)
	ফেরত fp;

    জন্য (j = 0; j < fp->uf_সারিগুলি.ga_len && !got_int; ++j)
    {
	যদি (FUNCLINE(fp, j) == ফাঁকা)
	    চালু;
	বার্তা_putchar('\n');
	বার্তা_outnum((long)(j + 1));
	যদি (j < 9)
	    বার্তা_putchar(' ');
	যদি (j < 99)
	    বার্তা_putchar(' ');
	যদি (function_list_modified(পূর্ব_ht_changed))
	    বিরতি;
	বার্তা_prt_সারি(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    যদি (!got_int)
    {
	বার্তা_putchar('\n');
	যদি (!function_list_modified(পূর্ব_ht_changed))
	{
	    যদি (fp->uf_def_status != UF_NOT_COMPILED)
		বার্তা_puts("   enddef");
	    অন্যথা
		বার্তা_puts("   endfunction");
	}
    }

    ফেরত fp;
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
	garray_T    *সারিগুলি_to_free,
	পূর্ণসংখ্যা	    class_flags,
	ocmember_T  *obj_members,
	পূর্ণসংখ্যা         obj_member_গুনতি,
	cctx_T	    *cctx)
{
    পূর্ণসংখ্যা		j;
    পূর্ণসংখ্যা		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    পূর্ণসংখ্যা		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*সারি_arg = ফাঁকা;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    পূর্ণসংখ্যা		varargs = FALSE;
    পূর্ণসংখ্যা		flags = 0;
    char_u	*ret_type = ফাঁকা;
    ufunc_T	*fp = ফাঁকা;
    পূর্ণসংখ্যা		fp_allocated = FALSE;
    পূর্ণসংখ্যা		free_fp = FALSE;
    পূর্ণসংখ্যা		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    স্থির পূর্ণসংখ্যা	func_nr = 0;	    // number for nameless function
    পূর্ণসংখ্যা		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_শীর্ষ;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    imported_T	*import = ফাঁকা;

    // ":function" without argument: list functions.
    যদি (ends_excmd2(eap->cmd, eap->arg))
    {
	যদি (!eap->skip)
	    list_functions(ফাঁকা);
	set_nextcmd(eap, eap->arg);
	ফেরত ফাঁকা;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    যদি (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	ফেরত ফাঁকা;
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
    যদি (name_arg != ফাঁকা)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    অন্যথা
    {
	যদি (vim9script)
	{
	    যদি (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		ফেরত ফাঁকা;
	    }
	    p = to_name_end(p, TRUE);
	    যদি (*skipwhite(p) == '.' && vim_strchr(p, '(') != ফাঁকা)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		ফেরত ফাঁকা;
	    }

	    p = eap->arg;
	}

	পূর্ণসংখ্যা tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ফাঁকা);
	যদি (name == ফাঁকা && (fudi.fd_dict == ফাঁকা || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    যদি (!aborting())
	    {
		যদি (!eap->skip && fudi.fd_newkey != ফাঁকা)
		    semsg(_(e_চাবি_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ফেরত ফাঁকা;
	    }
	    অন্যথা
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	যদি (is_export && name != ফাঁকা)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    যদি (prefixed != ফাঁকা && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	অন্যথা যদি (paren && vim9script && name != ফাঁকা
				    && vim_strchr(name, AUTOLOAD_CHAR) != ফাঁকা)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    যাও ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    যদি (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	যাও ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    যদি (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	যদি (p == ফাঁকা)
	    যাও ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    যদি (*p != '(')
    {
	যদি (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    যাও ret_free;
	}
	// attempt to continue by skipping some text
	যদি (vim_strchr(p, '(') != ফাঁকা)
	    p = vim_strchr(p, '(');
    }

    যদি ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	যাও ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    যদি (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	যাও ret_free;
    }

    ga_init2(&newlines, আকার(char_u *), 10);

    যদি (!eap->skip && name_arg == ফাঁকা)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	যদি (name != ফাঁকা)
	    arg = name;
	অন্যথা
	    arg = fudi.fd_newkey;
	যদি (arg != ফাঁকা && (fudi.fd_di == ফাঁকা
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    পূর্ণসংখ্যা	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    যদি (arg != fudi.fd_newkey)
	    {
		যদি (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    যদি (name_base == ফাঁকা)
			name_base = arg + 3;
		    অন্যথা
			++name_base;
		}
		জন্য (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		যদি (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    যাও ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		যদি (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, ফাঁকা,
			ফাঁকা, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    যাও ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	যদি (fudi.fd_dict != ফাঁকা && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    যাও ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    যদি (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ফাঁকা, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ফাঁকা,
			 ফাঁকা, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, সারিগুলি_to_free) == FAIL)
	যাও errret_2;
    whitep = p;

    যদি (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	যদি (*skipwhite(p) == ':')
	{
	    যদি (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    অন্যথা যদি (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    যদি (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    অন্যথা
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ফাঁকা;
	    }
	}
	p = skipwhite(p);
    }
    অন্যথা
	// find extra arguments "range", "dict", "abort" and "closure"
	জন্য (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    যদি (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    অন্যথা যদি (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    অন্যথা যদি (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    অন্যথা যদি (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		যদি (current_funccal == ফাঁকা)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_শীর্ষ_level_str,
			    name == ফাঁকা ? (char_u *)"" : name);
		    যাও erret;
		}
	    }
	    অন্যথা
		বিরতি;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    যদি (*p == '\n')
	সারি_arg = p + 1;
    অন্যথা যদি (*p != NUL
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
    যদি (চাবি_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	যদি (!eap->skip && !eap->forceit)
	{
	    যদি (fudi.fd_dict != ফাঁকা && fudi.fd_newkey == ফাঁকা)
		emsg(_(e_dictionary_entry_already_exists));
	    অন্যথা যদি (name != ফাঁকা && find_func(name, is_global) != ফাঁকা)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	যদি (!eap->skip && did_emsg)
	    যাও erret;

	বার্তা_putchar('\n');	    // don't overwrite the function name
	cmdline_row = বার্তা_row;
    }

    // Save the starting line number.
    sourcing_lnum_শীর্ষ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    যদি (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, সারি_arg, সারিগুলি_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	যাও erret;

    /*
     * If there are no errors, add the function
     */
    যদি (fudi.fd_dict != ফাঁকা)
    {
	অক্ষর	numbuf[NUMBUFLEN];

	fp = ফাঁকা;
	যদি (fudi.fd_newkey == ফাঁকা && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    যাও erret;
	}
	যদি (fudi.fd_di == ফাঁকা)
	{
	    // Can't add a function to a locked dictionary
	    যদি (মান_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		যাও erret;
	}
	    // Can't change an existing function if it is locked
	অন্যথা যদি (মান_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    যাও erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, আকার(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	যদি (name == ফাঁকা)
	    যাও erret;
    }
    অন্যথা যদি (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	পূর্ণসংখ্যা		var_conflict = FALSE;
	পূর্ণসংখ্যা		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	যদি (v != ফাঁকা && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	যদি (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    যদি (si->sn_autoload_prefix != ফাঁকা)
	    {
		যদি (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    যদি (v != ফাঁকা)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		অন্যথা
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    যদি (prefixed != ফাঁকা && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			যদি (v != ফাঁকা)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	যদি (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    যাও erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	যদি (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == ফাঁকা ? name : uname, 0, FALSE);
	}

	যদি (fp != ফাঁকা || import != ফাঁকা)
	{
	    পূর্ণসংখ্যা dead = fp != ফাঁকা && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    যদি (import != ফাঁকা
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_শীর্ষ;
		যদি (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		অন্যথা
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		যাও errret_keep;
	    }
	    যদি (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		যাও errret_keep;
	    }
	    যদি (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ফাঁকা;
		overwrite = TRUE;
	    }
	    অন্যথা
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ফাঁকা;
		func_clear_বস্তুসমূহ(fp);
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

    যদি (fp == ফাঁকা)
    {
	যদি (fudi.fd_dict == ফাঁকা && vim_strchr(name, AUTOLOAD_CHAR) != ফাঁকা)
	{
	    পূর্ণসংখ্যা	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    যদি (SOURCING_NAME != ফাঁকা)
	    {
		scriptname = autoload_name(name);
		যদি (scriptname != ফাঁকা)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (পূর্ণসংখ্যা)STRLEN(p);
		    slen = (পূর্ণসংখ্যা)STRLEN(SOURCING_NAME);
		    যদি (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    যদি (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_শীর্ষ;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		যাও erret;
	    }
	}

	যদি (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	যদি (fp == ফাঁকা)
	    যাও erret;
	fp_allocated = TRUE;

	যদি (fudi.fd_dict != ফাঁকা)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    যদি (func_name == ফাঁকা)
	    {
		VIM_CLEAR(fp);
		যাও erret;
	    }
	    যদি (fudi.fd_di == ফাঁকা)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		যদি (fudi.fd_di == ফাঁকা)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    যাও erret;
		}
		যদি (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    যাও erret;
		}
	    }
	    অন্যথা
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

    যদি (eap->cmdidx == CMD_def)
    {
	পূর্ণসংখ্যা	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	যদি (generic_func_args_table_আকার(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_শীর্ষ;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	পূর্ণসংখ্যা save_is_export = is_export;
	is_export = FALSE;

	যদি (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_গুনতি, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    যাও erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	যদি (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    যাও erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    অন্যথা
	fp->uf_def_status = UF_NOT_COMPILED;

    যদি (fp_allocated)
    {
	// insert the new function in the function list
	যদি (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_চাবি = UF2HIKEY(fp);
	}
	অন্যথা যদি (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    যাও erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_সারিগুলি = newlines;
    newlines.ga_তথ্য = ফাঁকা;
    যদি ((flags & FC_CLOSURE) != 0)
    {
	যদি (register_closure(fp) == FAIL)
	    যাও erret;
    }
    অন্যথা
	fp->uf_scoped = ফাঁকা;

#ifdef FEAT_PROFILE
    যদি (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    যদি (sandbox)
	flags |= FC_SANDBOX;
    যদি (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_শীর্ষ;
    যদি (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    যদি (eap->cmdidx == CMD_def)
	set_function_type(fp);
    অন্যথা যদি (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    যদি (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ফাঁকা);

    যাও ret_free;

erret:
    যদি (fp != ফাঁকা)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    যদি (fp != ফাঁকা)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	যদি (IS_GENERIC_FUNC(fp))
	    generic_func_clear_বস্তুসমূহ(fp);
    }
    যদি (free_fp)
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
    যদি (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ফেরত fp;
}

/*
 * ":function"
 */
    শূন্য
ex_function(exarg_T *eap)
{
    garray_T সারিগুলি_to_free;

    ga_init2(&সারিগুলি_to_free, আকার(char_u *), 50);
    (শূন্য)define_function(eap, ফাঁকা, &সারিগুলি_to_free, 0, ফাঁকা, 0, ফাঁকা);
    ga_clear_strings(&সারিগুলি_to_free);
}

    পূর্ণসংখ্যা
get_func_arity(char_u *name, পূর্ণসংখ্যা *required, পূর্ণসংখ্যা *optional, পূর্ণসংখ্যা *varargs)
{
    ufunc_T	*ufunc = ফাঁকা;
    পূর্ণসংখ্যা		argcount = 0;
    পূর্ণসংখ্যা		min_argcount = 0;
    পূর্ণসংখ্যা		idx;

    idx = find_internal_func(name);
    যদি (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    অন্যথা
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ফাঁকা;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	যদি (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	যদি (ufunc == ফাঁকা)
	    ফেরত FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ফেরত OK;
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
    পূর্ণসংখ্যা		is_global = FALSE;
    char_u	*bracket_start = ফাঁকা;
    char_u	*generic_func_name = name;

    যদি (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    অন্যথা যদি (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    যদি (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(শূন্য)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    অন্যথা
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	যদি (ufunc != ফাঁকা)
	    ফেরত ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ফাঁকা, ফাঁকা, ফাঁকা, &ufunc);
	যদি (ufunc != ফাঁকা)
	{
	    vim_free(fname);
	    ফেরত ufunc;
	}
	যদি (fname != ফাঁকা && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    যদি (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		ফেরত ফাঁকা;
	    }
	}
    }
    যদি (fname == ফাঁকা)
    {
	semsg(_(e_invalid_argument_str), name);
	ফেরত ফাঁকা;
    }
    যদি (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ফেরত ফাঁকা;
    }

    ufunc = find_func(fname, is_global);
    যদি (ufunc == ফাঁকা)
    {
	char_u *p = untrans_function_name(fname);

	যদি (p != ফাঁকা)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	যদি (ufunc == ফাঁকা)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    যদি (bracket_start != ফাঁকা)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    যদি (ufunc != ফাঁকা)
    {
	// handle generic functions
	যদি (bracket_start == ফাঁকা)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    যদি (ufunc == ফাঁকা)
	semsg(_(e_cannot_find_function_str), name);
    ফেরত ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    শূন্য
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    যদি (func_needs_compiling(ufunc, compile_type))
	(শূন্য)compile_def_function(ufunc, FALSE, compile_type, ফাঁকা);
    অন্যথা
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ফাঁকা ? cl->class_name.string : (char_u *)"",
				cl != ফাঁকা ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    স্থির শূন্য
defcompile_funcs_in_script(শূন্য)
{
    long	todo = (long)func_hashtab.ht_used;
    পূর্ণসংখ্যা		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    জন্য (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    যদি (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(শূন্য)compile_def_function(ufunc, FALSE, CT_NONE, ফাঁকা);

		যদি (func_hashtab.ht_changed != changed)
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
    শূন্য
ex_defcompile(exarg_T *eap)
{
    যদি (*eap->arg != NUL)
    {
	typval_T tv;

	যদি (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    যদি (cl != ফাঁকা)
		defcompile_class(cl);
	}
	অন্যথা
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    যদি (ufunc != ফাঁকা)
		defcompile_function(ufunc, ফাঁকা);
	}
    }
    অন্যথা
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
    পূর্ণসংখ্যা
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    যদি (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ফেরত 5;
    যদি (p[0] == 's' && p[1] == ':')
	ফেরত 2;
    ফেরত 0;
}

    পূর্ণসংখ্যা
translated_function_exists(char_u *name, পূর্ণসংখ্যা is_global)
{
    যদি (builtin_function(name, -1))
	ফেরত has_internal_func(name);
    ফেরত find_func(name, is_global) != ফাঁকা;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    পূর্ণসংখ্যা
has_varargs(ufunc_T *ufunc)
{
    ফেরত ufunc->uf_varargs || ufunc->uf_va_name != ফাঁকা;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    পূর্ণসংখ্যা
function_exists(char_u *name, পূর্ণসংখ্যা no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    পূর্ণসংখ্যা	    n = FALSE;
    পূর্ণসংখ্যা	    flag;
    পূর্ণসংখ্যা	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    যদি (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    যদি (p != ফাঁকা && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ফেরত n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    পূর্ণসংখ্যা
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    পূর্ণসংখ্যা		is_global = FALSE;
    ufunc_T	*fp = ফাঁকা;
    list_T	*args;
    পূর্ণসংখ্যা		is_def;
    অক্ষর	*tofree;

    যদি (*name == NUL)
	ফেরত FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    যদি (p != ফাঁকা && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    যদি (fp == ফাঁকা)
	ফেরত FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    যদি (args == ফাঁকা || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	ফেরত FAIL;
    }
    জন্য (পূর্ণসংখ্যা j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	যদি (arg == ফাঁকা || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    ফেরত FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	যদি (fp->uf_arg_types == ফাঁকা)
	    dict_add_string(arg, "type", (char_u *)"any");
	অন্যথা
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	যদি (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_তথ্য)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    যদি (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	যদি (va == ফাঁকা || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ফেরত FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == ফাঁকা ? (char_u *)"" : fp->uf_va_name);
	যদি (fp->uf_va_type == ফাঁকা)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	অন্যথা
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    যদি (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    অন্যথা
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_সংখ্যা(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_সংখ্যা(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ফেরত OK;
}

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, পূর্ণসংখ্যা check)
{
    char_u	*nm = name;
    char_u	*p;
    পূর্ণসংখ্যা		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    যদি (p != ফাঁকা && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ফেরত p;

    vim_free(p);
    ফেরত ফাঁকা;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, পূর্ণসংখ্যা idx)
{
    স্থির long_u	done;
    স্থির পূর্ণসংখ্যা		changed;
    স্থির hashitem_T	*hi;
    ufunc_T		*fp;

    যদি (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    যদি (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	পূর্ণসংখ্যা len;

	যদি (done++ > 0)
	    ++hi;
	যখন (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	যদি ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ফেরত (char_u *)"";

	যদি (fp->uf_namelen + 4 >= IOSIZE)
	    ফেরত fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	যদি (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    যদি (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ফেরত IObuff;
    }
    ফেরত ফাঁকা;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, পূর্ণসংখ্যা extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    যদি (ufunc == ফাঁকা)
	ফেরত ফাঁকা;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ফাঁকা;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_গুনতি = ফাঁকা;
    ufunc->uf_tml_যোগ = ফাঁকা;
    ufunc->uf_tml_self = ফাঁকা;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    যদি (ufunc->uf_arg_types != ফাঁকা)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	যদি (at != ফাঁকা)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     আকার(type_T *) * ufunc->uf_args.ga_len);
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

    যদি (ufunc->uf_va_name != ফাঁকা)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = ফাঁকা;

    ga_copy_strings(&fp->uf_সারিগুলি, &ufunc->uf_সারিগুলি);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    যদি (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    ফেরত ufunc;
}

/*
 * ":delfunction {name}"
 */
    শূন্য
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ফাঁকা;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    পূর্ণসংখ্যা		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ফাঁকা, ফাঁকা, ফাঁকা);
    vim_free(fudi.fd_newkey);
    যদি (name == ফাঁকা)
    {
	যদি (fudi.fd_dict != ফাঁকা && !eap->skip)
	    emsg(_(e_funcref_required));
	ফেরত;
    }
    যদি (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ফেরত;
    }
    set_nextcmd(eap, p);
    যদি (eap->nextcmd != ফাঁকা)
	*p = NUL;

    যদি (numbered_function(name) && fudi.fd_dict == ফাঁকা)
    {
	যদি (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	ফেরত;
    }
    যদি (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    যদি (!eap->skip)
    {
	যদি (fp == ফাঁকা)
	{
	    যদি (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ফেরত;
	}
	যদি (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    ফেরত;
	}
	যদি (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ফেরত;
	}

	যদি (fudi.fd_dict != ফাঁকা)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	অন্যথা
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    যদি (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		যদি (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    অন্যথা
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    শূন্য
func_unref(char_u *name)
{
    ufunc_T *fp = ফাঁকা;

    যদি (name == ফাঁকা || !func_name_refcount(name))
	ফেরত;
    fp = find_func(name, FALSE);
    যদি (fp == ফাঁকা && numbered_function(name))
    {
#ifdef EXITFREE
	যদি (!entered_free_all_mem)
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
    শূন্য
func_ptr_unref(ufunc_T *fp)
{
    যদি (fp != ফাঁকা && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ফাঁকা
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	যদি (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    শূন্য
func_ref(char_u *name)
{
    ufunc_T *fp;

    যদি (name == ফাঁকা || !func_name_refcount(name))
	ফেরত;
    fp = find_func(name, FALSE);
    যদি (fp != ফাঁকা)
	++fp->uf_refcount;
    অন্যথা যদি (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    শূন্য
func_ptr_ref(ufunc_T *fp)
{
    যদি (fp != ফাঁকা)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    স্থির পূর্ণসংখ্যা
can_free_funccal(funccall_T *fc, পূর্ণসংখ্যা copyID)
{
    ফেরত (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    শূন্য
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    পূর্ণসংখ্যা		returning = FALSE;
    evalarg_T	evalarg;

    যদি (current_funccal == ফাঁকা)
    {
	emsg(_(e_return_not_inside_function));
	ফেরত;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    যদি (eap->skip)
	++emsg_skip;

    eap->nextcmd = ফাঁকা;
    যদি ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	যদি (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	অন্যথা
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    অন্যথা যদি (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	যদি (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ফাঁকা);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    যদি (returning)
	eap->nextcmd = ফাঁকা;
    অন্যথা যদি (eap->nextcmd == ফাঁকা)	    // no argument
	set_nextcmd(eap, arg);

    যদি (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    স্থির পূর্ণসংখ্যা
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    পূর্ণসংখ্যা		doesrange;
    typval_T	rettv;
    পূর্ণসংখ্যা		failed = FALSE;

    lnum = eap->line1;
    জন্য ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	যদি (eap->addr_গুনতি > 0)
	{
	    যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		বিরতি;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	যদি (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    বিরতি;
	}
	যদি (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	যদি (handle_subscript(arg, ফাঁকা, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    বিরতি;
	}

	clear_tv(&rettv);
	যদি (doesrange)
	    বিরতি;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	যদি (aborting())
	    বিরতি;
    }
    ফেরত failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    স্থির পূর্ণসংখ্যা
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    পূর্ণসংখ্যা		partial_argc = 0;		// number of partial arguments
    পূর্ণসংখ্যা		argcount = 0;			// number of arguments found
    পূর্ণসংখ্যা		r;

    যদি (current_funccal == ফাঁকা)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ফেরত FAIL;
    }
    যদি (partial != ফাঁকা)
    {
	যদি (partial->pt_dict != ফাঁকা)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ফেরত FAIL;
	}
	যদি (partial->pt_argc > 0)
	{
	    পূর্ণসংখ্যা i;

	    partial_argc = partial->pt_argc;
	    জন্য (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    পূর্ণসংখ্যা is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    যদি (r == OK)
    {
	যদি (type != ফাঁকা)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, ফাঁকা, name);
	}
	অন্যথা যদি (is_builtin)
	{
	    পূর্ণসংখ্যা idx = find_internal_func(name);

	    যদি (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    অন্যথা যদি (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	অন্যথা
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    যদি (ufunc != ফাঁকা)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		যদি (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    যদি (r == FAIL)
    {
	যখন (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ফেরত FAIL;
    }
    ফেরত add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    পূর্ণসংখ্যা
can_add_defer(শূন্য)
{
    যদি (!in_def_function() && get_current_funccal() == ফাঁকা)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
add_defer(char_u *name, পূর্ণসংখ্যা argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    পূর্ণসংখ্যা		argcount = argcount_arg;
    defer_T	*dr;
    পূর্ণসংখ্যা		ret = FAIL;

    যদি (saved_name == ফাঁকা)
	যাও theend;
    যদি (in_def_function())
    {
	যদি (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    অন্যথা
    {
	যদি (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, আকার(defer_T), 10);
	যদি (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    যাও theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_তথ্য)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	যখন (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    যখন (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ফেরত ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    স্থির শূন্য
handle_defer_one(funccall_T *funccal)
{
    পূর্ণসংখ্যা	    idx;

    জন্য (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_তথ্য) + idx;

	যদি (dr->dr_name == ফাঁকা)
	    // already being called, can happen if function does ":qa"
	    চালু;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ফাঁকা;

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
	জন্য (পূর্ণসংখ্যা i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    স্থির শূন্য
invoke_funccall_defer(funccall_T *fc)
{
    যদি (fc->fc_ectx != ফাঁকা)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    অন্যথা
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    শূন্য
invoke_all_defer(শূন্য)
{
    জন্য (funccall_T *fc = current_funccal; fc != ফাঁকা; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    জন্য (funccal_entry_T *fce = funccal_স্তূপ; fce != ফাঁকা; fce = fce->পরবর্তী)
	জন্য (funccall_T *fc = fce->শীর্ষ_funccal; fc != ফাঁকা; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    শূন্য
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ফাঁকা;
    partial_T	*partial = ফাঁকা;
    evalarg_T	evalarg;
    type_T	*type = ফাঁকা;
    পূর্ণসংখ্যা		found_var = FALSE;
    পূর্ণসংখ্যা		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    যদি (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	যদি (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ফেরত;
    }

    tofree = trans_function_name_ext(&arg, ফাঁকা, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ফাঁকা, &ufunc);
    যদি (fudi.fd_newkey != ফাঁকা)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_চাবি_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    যদি (tofree == ফাঁকা)
	ফেরত;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    যদি (fudi.fd_dict != ফাঁকা)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (পূর্ণসংখ্যা)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ফাঁকা ? ফাঁকা : &partial,
				vim9script && type == ফাঁকা ? &type : ফাঁকা,
						     FALSE, FALSE, &found_var);

    যদি (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	যদি (name == ফাঁকা)
	    যাও end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    যদি (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	যাও end;
    }
    যদি (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	যাও end;
    }

    যদি (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    অন্যথা
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
    যদি ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	যদি (!ends_excmd2(eap->arg, arg))
	{
	    যদি (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	অন্যথা
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
    পূর্ণসংখ্যা
do_return(
    exarg_T	*eap,
    পূর্ণসংখ্যা		reanimate,
    পূর্ণসংখ্যা		is_cmd,
    শূন্য	*rettv)
{
    পূর্ণসংখ্যা		idx;
    cstack_T	*cstack = eap->cstack;

    যদি (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    যদি (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	যদি (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	অন্যথা
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    যদি (reanimate)
		rettv = current_funccal->fc_rettv;

	    যদি (rettv != ফাঁকা)
	    {
		// Store the value of the pending return.
		যদি ((cstack->cs_rettv[idx] = alloc_tv()) != ফাঁকা)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		অন্যথা
		    emsg(_(e_out_of_memory));
	    }
	    অন্যথা
		cstack->cs_rettv[idx] = ফাঁকা;

	    যদি (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_সংখ্যা;
		current_funccal->fc_rettv->vval.v_সংখ্যা = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    অন্যথা
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	যদি (!reanimate && rettv != ফাঁকা)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    যদি (!is_cmd)
		vim_free(rettv);
	}
    }

    ফেরত idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    শূন্য
discard_pending_return(শূন্য *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(শূন্য *rettv)
{
    char_u	*s = ফাঁকা;
    char_u	*tofree = ফাঁকা;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    যদি (rettv != ফাঁকা)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    যদি (s == ফাঁকা)
	s = (char_u *)"";
    অন্যথা
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    যদি (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ফেরত vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_সারি(
    পূর্ণসংখ্যা	    c UNUSED,
    শূন্য    *cookie,
    পূর্ণসংখ্যা	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    যদি (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	func_সারি_end(cookie);
#endif

    gap = &fp->uf_সারিগুলি;
    যদি (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = ফাঁকা;
    অন্যথা
    {
	// Skip NULL lines (continuation lines).
	যখন (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_তথ্য))[fcp->fc_linenr] == ফাঁকা)
	    ++fcp->fc_linenr;
	যদি (fcp->fc_linenr >= gap->ga_len)
	    retval = ফাঁকা;
	অন্যথা
	{
	    retval = vim_strsave(((char_u **)(gap->ga_তথ্য))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    যদি (do_profiling == PROF_YES)
		func_সারি_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    যদি (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ফেরত retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    পূর্ণসংখ্যা
func_has_ended(শূন্য *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ফেরত (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    পূর্ণসংখ্যা
func_has_abort(
    শূন্য    *cookie)
{
    ফেরত ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = ফাঁকা;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    যদি (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ফাঁকা
				     && rettv->vval.v_partial->pt_func != ফাঁকা)
	fp = rettv->vval.v_partial->pt_func;
    অন্যথা
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ফাঁকা ? ফাঁকা
					      : rettv->vval.v_partial->pt_name;
	যদি (fname == ফাঁকা)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ফাঁকা;
	}
	অন্যথা
	{
	    char_u	*tofree = ফাঁকা;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    যদি (fp != ফাঁকা && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	যদি (pt != ফাঁকা)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ফাঁকা;
	    যদি (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    অন্যথা
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		পূর্ণসংখ্যা		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		যদি (ret_pt->pt_name != ফাঁকা)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		অন্যথা
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		যদি (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    যদি (pt->pt_argv == ফাঁকা)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    অন্যথা
		    {
			pt->pt_argc = ret_pt->pt_argc;
			জন্য (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ফেরত selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(শূন্য *cookie)
{
    ফেরত ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(শূন্য *cookie)
{
    ফেরত &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    পূর্ণসংখ্যা *
func_dbg_tick(শূন্য *cookie)
{
    ফেরত &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    পূর্ণসংখ্যা
func_level(শূন্য *cookie)
{
    ফেরত ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    পূর্ণসংখ্যা
current_func_returned(শূন্য)
{
    ফেরত current_funccal->fc_returned;
}

    পূর্ণসংখ্যা
free_unref_funccal(পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা testing)
{
    পূর্ণসংখ্যা		did_free = FALSE;
    পূর্ণসংখ্যা		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    জন্য (pfc = &previous_funccal; *pfc != ফাঁকা; )
    {
	যদি (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	অন্যথা
	    pfc = &(*pfc)->fc_caller;
    }
    যদি (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(শূন্য)garbage_collect(testing);

    ফেরত did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    স্থির funccall_T *
get_funccal(শূন্য)
{
    পূর্ণসংখ্যা		i;
    funccall_T	*funccal;
    funccall_T	*অস্থায়ী_funccal;

    funccal = current_funccal;
    যদি (debug_backtrace_level > 0)
    {
	জন্য (i = 0; i < debug_backtrace_level; i++)
	{
	    অস্থায়ী_funccal = funccal->fc_caller;
	    যদি (অস্থায়ী_funccal)
		funccal = অস্থায়ী_funccal;
	    অন্যথা
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ফেরত funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    স্থির funccall_T *
get_funccal_for_vars(শূন্য)
{
    funccall_T	*funccal = ফাঁকা;

    যদি (current_funccal == ফাঁকা)
	ফেরত ফাঁকা;
    funccal = get_funccal();
    যদি (funccal == ফাঁকা || funccal->fc_l_vars.dv_refcount == 0)
	ফেরত ফাঁকা;
    ফেরত funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ফেরত funccal == ফাঁকা ? ফাঁকা : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ফেরত funccal == ফাঁকা ? ফাঁকা : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ফেরত funccal == ফাঁকা ? ফাঁকা : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ফেরত funccal == ফাঁকা ? ফাঁকা : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    শূন্য
list_func_vars(পূর্ণসংখ্যা *first)
{
    যদি (current_funccal != ফাঁকা && current_funccal->fc_l_vars.dv_refcount > 0)
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
    যদি (current_funccal != ফাঁকা
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ফেরত &current_funccal->fc_l_vars;
    ফেরত ফাঁকা;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ফাঁকা;
    char_u	*varname;

    যদি (current_funccal == ফাঁকা || current_funccal->fc_func->uf_scoped == ফাঁকা)
	ফেরত ফাঁকা;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    যখন (current_funccal != ফাঁকা)
    {
	ht = find_var_ht(name, &varname);
	যদি (ht != ফাঁকা && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		বিরতি;
	    }
	}
	যদি (current_funccal == current_funccal->fc_func->uf_scoped)
	    বিরতি;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ফেরত hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, পূর্ণসংখ্যা no_autoload)
{
    dictitem_T	*v = ফাঁকা;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    যদি (current_funccal == ফাঁকা || current_funccal->fc_func->uf_scoped == ফাঁকা)
	ফেরত ফাঁকা;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    যখন (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	যদি (ht != ফাঁকা && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    যদি (v != ফাঁকা)
		বিরতি;
	}
	যদি (current_funccal == current_funccal->fc_func->uf_scoped)
	    বিরতি;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ফেরত v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    পূর্ণসংখ্যা
set_ref_in_previous_funccal(পূর্ণসংখ্যা copyID)
{
    funccall_T	*fc;

    জন্য (fc = previous_funccal; fc != ফাঁকা; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	যদি (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ফাঁকা, ফাঁকা)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ফাঁকা, ফাঁকা)
		|| set_ref_in_list_বস্তুসমূহ(&fc->fc_l_varlist, copyID + 1, ফাঁকা, ফাঁকা))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

    স্থির পূর্ণসংখ্যা
set_ref_in_funccal(funccall_T *fc, পূর্ণসংখ্যা copyID)
{
    যদি (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	যদি (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, ফাঁকা, ফাঁকা)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, ফাঁকা, ফাঁকা)
		|| set_ref_in_list_বস্তুসমূহ(&fc->fc_l_varlist, copyID, ফাঁকা, ফাঁকা)
		|| set_ref_in_func(ফাঁকা, fc->fc_func, copyID))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    পূর্ণসংখ্যা
set_ref_in_call_স্তূপ(পূর্ণসংখ্যা copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    জন্য (fc = current_funccal; fc != ফাঁকা; fc = fc->fc_caller)
	যদি (set_ref_in_funccal(fc, copyID))
	    ফেরত TRUE;

    // Also go through the funccal_stack.
    জন্য (entry = funccal_স্তূপ; entry != ফাঁকা; entry = entry->পরবর্তী)
	জন্য (fc = entry->শীর্ষ_funccal; fc != ফাঁকা; fc = fc->fc_caller)
	    যদি (set_ref_in_funccal(fc, copyID))
		ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    পূর্ণসংখ্যা
set_ref_in_functions(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		todo;
    hashitem_T	*hi = ফাঁকা;
    ufunc_T	*fp;

    todo = (পূর্ণসংখ্যা)func_hashtab.ht_used;
    জন্য (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    যদি (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(ফাঁকা, fp, copyID))
		ফেরত TRUE;
	}
    }
    ফেরত FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    পূর্ণসংখ্যা
set_ref_in_func_args(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < funcargs.ga_len; ++i)
	যদি (set_ref_in_item(((typval_T **)funcargs.ga_তথ্য)[i],
						copyID, ফাঁকা, ফাঁকা, ফাঁকা))
	    ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণসংখ্যা
set_ref_in_func(char_u *name, ufunc_T *fp_in, পূর্ণসংখ্যা copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ফাঁকা;
    char_u	*fname;
    পূর্ণসংখ্যা		abort = FALSE;

    যদি (name == ফাঁকা && fp_in == ফাঁকা)
	ফেরত FALSE;

    যদি (fp_in == ফাঁকা)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    যদি (fp != ফাঁকা)
    {
	জন্য (fc = fp->uf_scoped; fc != ফাঁকা; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    ফেরত abort;
}

#endif // FEAT_EVAL
