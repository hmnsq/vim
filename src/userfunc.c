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
স্থির funccall_T *current_funccal = রিক্ত;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
স্থির funccall_T *previous_funccal = রিক্ত;

স্থির শূন্য funccal_unref(funccall_T *fc, ufunc_T *fp, পূর্ণাঙ্ক force);
স্থির শূন্য func_clear(ufunc_T *fp, পূর্ণাঙ্ক force);
স্থির পূর্ণাঙ্ক func_free(ufunc_T *fp, পূর্ণাঙ্ক force);
স্থির char_u *untrans_function_name(char_u *name);
স্থির শূন্য handle_defer_one(funccall_T *funccal);
স্থির char_u *trans_function_name_ext(char_u **pp, পূর্ণাঙ্ক *is_global, পূর্ণাঙ্ক skip, পূর্ণাঙ্ক flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

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
    প্রত্যাগম &func_hashtab;
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
	পূর্ণাঙ্ক	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	পূর্ণাঙ্ক	    is_vararg,
	পূর্ণাঙ্ক	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = রিক্ত;
    পূর্ণাঙ্ক		is_underscore = FALSE;

    যাবত্ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    যদি (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == রিক্ত
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	যদি (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	প্রত্যাগম arg;
    }

    // Extra checks in Vim9 script.
    যদি (!skip && argtypes != রিক্ত)
    {
	পূর্ণাঙ্ক c = *p;
	*p = NUL;
	পূর্ণাঙ্ক r = check_reserved_name(arg, FALSE);
	*p = c;
	যদি (r == FAIL)
	    প্রত্যাগম arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	যদি (check_defined(arg, p - arg,
				   evalarg == রিক্ত ? রিক্ত : evalarg->eval_cctx,
			       eap == রিক্ত ? রিক্ত : eap->cstack, TRUE) == FAIL)
	    প্রত্যাগম arg;
    }

    যদি (newargs != রিক্ত && ga_grow(newargs, 1) == FAIL)
	প্রত্যাগম arg;
    যদি (newargs != রিক্ত)
    {
	পূর্ণাঙ্ক	c;
	পূর্ণাঙ্ক	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	যদি (arg_copy == রিক্ত)
	{
	    *p = c;
	    প্রত্যাগম arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	যদি (argtypes == রিক্ত || !is_underscore)
	    // Check for duplicate argument name.
	    প্রতি (i = 0; i < newargs->ga_len; ++i)
		যদি (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    প্রত্যাগম arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    যদি (argtypes != রিক্ত && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != রিক্ত && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = রিক্ত;

	যদি (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == রিক্ত ? arg : arg_copy);
	    p = skipwhite(p);
	}
	যদি (*p == ':')
	{
	    ++p;
	    যদি (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		প্রত্যাগম arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    যদি (!skip)
		type = vim_strnsave(type, p - type);
	}
	অন্যথা যদি (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == রিক্ত ? arg : arg_copy);
	    প্রত্যাগম arg;
	}
	যদি (!skip)
	{
	    যদি (type == রিক্ত && types_optional)
	    {
		// lambda arguments default to "any" type
		যদি (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		অন্যথা
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    প্রত্যাগম p;
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
	পূর্ণাঙ্ক		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    যদি (eap->ea_getline == রিক্ত)
	theline = getcmdline(':', 0L, indent, 0);
    অন্যথা
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    যদি (theline != রিক্ত)
    {
	যদি (সারিগুলি_to_free->ga_len > 0
		&& eap->cmdlinep != রিক্ত
		&& *eap->cmdlinep == ((char_u **)সারিগুলি_to_free->ga_data)
						   [সারিগুলি_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(শূন্য)ga_add_string(সারিগুলি_to_free, theline);
    }

    প্রত্যাগম theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    স্থির পূর্ণাঙ্ক
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    পূর্ণাঙ্ক		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    পূর্ণাঙ্ক		*varargs,
    garray_T	*default_args,
    পূর্ণাঙ্ক		skip,
    exarg_T	*eap,		// can be NULL
    পূর্ণাঙ্ক		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*সারিগুলি_to_free)
{
    পূর্ণাঙ্ক		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		any_default = FALSE;
    char_u	*whitep = *argp;
    পূর্ণাঙ্ক		need_রাশিমালা = FALSE;

    যদি (newargs != রিক্ত)
	ga_init2(newargs, পরিমাণ(char_u *), 3);
    যদি (argtypes != রিক্ত)
	ga_init2(argtypes, পরিমাণ(char_u *), 3);
    যদি (arg_objm != রিক্ত)
	ga_init2(arg_objm, পরিমাণ(int8_T), 3);
    যদি (!skip && default_args != রিক্ত)
	ga_init2(default_args, পরিমাণ(char_u *), 3);

    যদি (varargs != রিক্ত)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    যাবত্ (*p != endchar)
    {
	যাবত্ (eap != রিক্ত && eap->ea_getline != রিক্ত
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_সারি(eap, সারিগুলি_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    যদি (theline == রিক্ত)
		ভঙ্গ;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	যদি (mustend && *p != endchar)
	{
	    যদি (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    গচ্ছ err_ret;
	}
	যদি (*p == endchar && !need_রাশিমালা)
	    ভঙ্গ;

	যদি (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    যদি (varargs != রিক্ত)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    যদি (argtypes != রিক্ত)
	    {
		// ...name: list<type>
		যদি (!eval_isnamec1(*p))
		{
		    যদি (!skip)
			emsg(_(e_missing_name_after_dots));
		    গচ্ছ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		যদি (p == arg)
		    ভঙ্গ;
		যদি (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ভঙ্গ;
		}
	    }
	}
	অন্যথা যদি (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    যাবত্ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    যদি (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		ভঙ্গ;
	    }

	    যদি (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		যদি (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_মান_must_be_vnone_str), p);
		    গচ্ছ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		যদি (ga_grow(default_args, 1) == FAIL)
		    গচ্ছ err_ret;

		char_u *রাশিমালা = vim_strnsave((char_u *)"v:none", 6);
		যদি (রাশিমালা == রিক্ত)
		    গচ্ছ err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = রাশিমালা;
		default_args->ga_len++;
	    }
	    অন্যথা যদি (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		গচ্ছ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    যদি (newargs != রিক্ত && ga_grow(newargs, 1) == FAIL)
		প্রত্যাগম FAIL;
	    যদি (newargs != রিক্ত)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		যদি (argtypes != রিক্ত && ga_grow(argtypes, 1) == OK
			    && arg_objm != রিক্ত && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    যদি (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			পূর্ণাঙ্ক len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			যদি (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			যদি (assignment != রিক্ত)
			{
			    c = *argend;
			    *argend = NUL;
			    যদি (any_default)
				vim_snprintf((বর্ণ *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    অন্যথা
				vim_snprintf((বর্ণ *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
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
		ভঙ্গ;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    যদি (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != রিক্ত)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *রাশিমালা = p;
		যদি (eval1(&p, &rettv, রিক্ত) != FAIL)
		{
		    যদি (!skip)
		    {
			যদি (ga_grow(default_args, 1) == FAIL)
			    গচ্ছ err_ret;

			যদি (need_রাশিমালা)
			    need_রাশিমালা = FALSE;
			// trim trailing whitespace
			যাবত্ (p > রাশিমালা && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			রাশিমালা = vim_strsave(রাশিমালা);
			যদি (রাশিমালা == রিক্ত)
			{
			    *p = c;
			    গচ্ছ err_ret;
			}
			((char_u **)(default_args->ga_data))
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
		গচ্ছ err_ret;
	    }

	    যদি (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		যদি (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    গচ্ছ err_ret;
		}
		p = skipwhite(p);
	    }
	    যদি (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		যদি (!skip && argtypes != রিক্ত
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    গচ্ছ err_ret;
		}
	    }
	    অন্যথা
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    যদি (*p != endchar)
	গচ্ছ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    প্রত্যাগম OK;

err_ret:
    যদি (newargs != রিক্ত)
	ga_clear_strings(newargs);
    যদি (!skip && default_args != রিক্ত)
	ga_clear_strings(default_args);
    প্রত্যাগম FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    পূর্ণাঙ্ক		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    পূর্ণাঙ্ক		obj_member_গুনতি,
    cctx_T	*cctx)
{
    পূর্ণাঙ্ক len = 0;

    ga_init2(&fp->uf_type_list, পরিমাণ(type_T *), 10);
    যদি (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	যদি (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	যদি (fp->uf_arg_types != রিক্ত)
	{
	    পূর্ণাঙ্ক	i;
	    type_T	*type;

	    প্রতি (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		যদি (p == রিক্ত)
		    // will get the type from the default value
		    type = &t_unknown;
		অন্যথা
		{
		    যদি (arg_objm != রিক্ত && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			প্রতি (পূর্ণাঙ্ক om = 0; om < obj_member_গুনতি; ++om)
			{
			    যদি (obj_members != রিক্ত
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ভঙ্গ;
			    }
			}
		    }
		    অন্যথা
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		যদি (type == রিক্ত || !valid_declaration_type(type))
		    প্রত্যাগম FAIL;
		fp->uf_arg_types[i] = type;
		যদি (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    যদি (obj_members != রিক্ত && *name == '_')
			// protected object method
			name++;

		    যদি (var_wrong_func_name(name, TRUE))
			প্রত্যাগম FAIL;
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
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = রিক্ত;
	p = ((char_u **)argtypes->ga_data)[len];
	যদি (p == রিক্ত)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	অন্যথা
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    যদি (fp->uf_va_type != রিক্ত && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		প্রত্যাগম FAIL;
	    }
	}
	যদি (fp->uf_va_type == রিক্ত)
	    প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

    স্থির পূর্ণাঙ্ক
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    যদি (ret_type == রিক্ত)
	fp->uf_ret_type = &t_void;
    অন্যথা
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	যদি (fp->uf_ret_type == রিক্ত)
	{
	    fp->uf_ret_type = &t_void;
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    স্থির পূর্ণাঙ্ক
register_closure(ufunc_T *fp)
{
    যদি (fp->uf_scoped == current_funccal)
	// no change
	প্রত্যাগম OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    যদি (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	প্রত্যাগম FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    প্রত্যাগম OK;
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
	প্রত্যাগম name;
    len = STRLEN(name);
    যদি (len + 3 > bufsize)
	প্রত্যাগম name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    প্রত্যাগম buf;
}

স্থির char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(শূন্য)
{
    স্থির পূর্ণাঙ্ক	lambda_no = 0;
    পূর্ণাঙ্ক		n;
    string_T	ret;

    n = vim_snprintf((বর্ণ *)lambda_name, পরিমাণ(lambda_name), "<lambda>%d", ++lambda_no);
    যদি (n < 1)
	ret.length = 0;
    অন্যথা যদি (n >= (পূর্ণাঙ্ক)পরিমাণ(lambda_name))
	ret.length = পরিমাণ(lambda_name) - 1;
    অন্যথা
	ret.length = (size_t)n;

    ret.string = lambda_name;
    প্রত্যাগম ret;
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
    fp = alloc_clear(len < পরিমাণ(ufunc_T) ? পরিমাণ(ufunc_T) : len);
    যদি (fp != রিক্ত)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((শূন্য *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	যদি (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    যদি (fp->uf_name_exp != রিক্ত)
		vim_snprintf((বর্ণ *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    প্রত্যাগম fp;
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

    fp = alloc_ufunc(name.string, name.length);
    যদি (fp == রিক্ত)
	প্রত্যাগম রিক্ত;

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

    প্রত্যাগম name.string;
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
	পূর্ণাঙ্ক	equal_arrow,
	char_u	**ret_type,
	পূর্ণাঙ্ক	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    যদি (equal_arrow)
    {
	যদি (*s == ':')
	{
	    যদি (white_error != রিক্ত && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		প্রত্যাগম রিক্ত;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    যদি (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		প্রত্যাগম রিক্ত;
	    }
	}
	bef = s;
	s = skipwhite(s);
	যদি (*s != '=')
	    প্রত্যাগম রিক্ত;
	++s;
    }
    যদি (*s != '>')
	প্রত্যাগম রিক্ত;
    যদি (white_error != রিক্ত && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    স্থির পূর্ণাঙ্ক
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    যদি (checkforcmd(&p, "function", 2))
    {
	যদি (*p == '(')
	    প্রত্যাগম FALSE;
	*cmd = p;
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    স্থির শূন্য
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    যদি (cstack == রিক্ত || cstack->cs_idx < 0)
	প্রত্যাগম;

    পূর্ণাঙ্ক	    গুনতি = cstack->cs_idx + 1;
    পূর্ণাঙ্ক	    i;

    fp->uf_block_ids = ALLOC_MULT(পূর্ণাঙ্ক, গুনতি);
    যদি (fp->uf_block_ids != রিক্ত)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		পরিমাণ(পূর্ণাঙ্ক) * গুনতি);
	fp->uf_block_depth = গুনতি;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    প্রতি (i = 0; i <= cstack->cs_idx; ++i)
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
    যাবত্ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	যদি (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	অন্যথা যদি (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ভঙ্গ;
	}
	অন্যথা
	    MB_PTR_ADV(p);
    }

    প্রত্যাগম p;
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
    যাবত্ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	যদি (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	অন্যথা যদি (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ভঙ্গ;
	}
	অন্যথা
	    MB_PTR_ADV(p);
    }

    প্রত্যাগম p;
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

    যাবত্ (*p != NUL)
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
		ভঙ্গ;
	    MB_PTR_ADV(p);
	}
    }

    প্রত্যাগম p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    স্থির পূর্ণাঙ্ক
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *সারি_arg_in,
	garray_T    *সারিগুলি_to_free)
{
    linenr_T	sourcing_lnum_শীর্ষ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    পূর্ণাঙ্ক		saved_wait_return = need_wait_return;
    char_u	*সারি_arg = সারি_arg_in;
    পূর্ণাঙ্ক		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    বর্ণ	nesting_def[MAX_FUNC_NESTING];
    বর্ণ	nesting_inline[MAX_FUNC_NESTING];
    পূর্ণাঙ্ক		nesting = 0;
    getline_opt_T getline_options;
    পূর্ণাঙ্ক		indent = 2;
    char_u	*skip_until = রিক্ত;
    পূর্ণাঙ্ক		ret = FAIL;
    পূর্ণাঙ্ক		is_heredoc = FALSE;
    পূর্ণাঙ্ক		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = রিক্ত;
    size_t	heredoc_trimmedlen = 0;
    পূর্ণাঙ্ক		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    যদি (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	যদি (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    গচ্ছ theend;
	যাবত্ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = রিক্ত;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    প্রতি (;;)
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

	যদি (সারি_arg != রিক্ত)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = সারি_arg;
	    p = vim_strchr(theline, '\n');
	    যদি (p == রিক্ত)
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
	যদি (theline == রিক্ত)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_শীর্ষ;
	    যদি (skip_until != রিক্ত)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    অন্যথা যদি (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    অন্যথা যদি (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    অন্যথা
		emsg(_(e_missing_endfunction));
	    গচ্ছ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	যদি (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	অন্যথা
	    sourcing_lnum_off = 0;

	যদি (skip_until != রিক্ত)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    যদি (heredoc_trimmed == রিক্ত
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		যদি (heredoc_trimmed == রিক্ত)
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
	অন্যথা
	{
	    পূর্ণাঙ্ক	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    প্রতি (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
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
		    char_u *nextcmd = রিক্ত;

		    যদি (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    অন্যথা যদি (সারি_arg != রিক্ত && *skipwhite(সারি_arg) != NUL)
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
		    যদি (nextcmd != রিক্ত && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			যদি (সারিগুলি_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)সারিগুলি_to_free->ga_data)
						   [সারিগুলি_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)সারিগুলি_to_free->ga_data)
						   [সারিগুলি_to_free->ga_len - 1];
			    --সারিগুলি_to_free->ga_len;
			}
		    }
		    ভঙ্গ;
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
		vim_free(trans_function_name(&p, রিক্ত, TRUE, 0));
		যদি (vim9_function && *p == '<')
		{
		    // skip generic function
		    যদি (skip_generic_func_type_args(&p) == FAIL)
			গচ্ছ theend;
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

		যাবত্ (end > p && VIM_ISWHITE(*end))
		    --end;
		যদি (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    পূর্ণাঙ্ক	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    যাবত্ (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    যদি (!is_block)
			is_block = find_cmd_block_start(p) != রিক্ত;

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
		p = skip_range(p, FALSE, রিক্ত);
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
		    যদি (heredoc_trimmed == রিক্ত)
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
		    পূর্ণাঙ্ক		save_sc_version = current_sctx.sc_version;
		    পূর্ণাঙ্ক		var_গুনতি = 0;
		    পূর্ণাঙ্ক		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_গুনতি, &semicolon,
									 TRUE);
		    যদি (arg != রিক্ত)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    যদি (arg != রিক্ত && STRNCMP(arg, "=<<", 3) == 0)
		    {
			পূর্ণাঙ্ক has_trim = FALSE;

			p = skipwhite(arg + 3);
			যাবত্ (TRUE)
			{
			    যদি (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				অনুবর্ত;
			    }
			    যদি (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				অনুবর্ত;
			    }
			    ভঙ্গ;
			}
			যদি (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    যদি (heredoc_trimmed == রিক্ত)
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
	    গচ্ছ theend;

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
	যদি (p == রিক্ত)
	    গচ্ছ theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	যাবত্ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = রিক্ত;

	// Check for end of eap->arg.
	যদি (সারি_arg != রিক্ত && *সারি_arg == NUL)
	    সারি_arg = রিক্ত;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    যদি (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    প্রত্যাগম ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    স্থির পূর্ণাঙ্ক
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	পূর্ণাঙ্ক	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    পূর্ণাঙ্ক		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = রিক্ত;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = রিক্ত;
    পূর্ণাঙ্ক		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    পূর্ণাঙ্ক		lnum_save = -1;
    linenr_T	sourcing_lnum_শীর্ষ = SOURCING_LNUM;
    char_u	*সারি_arg = রিক্ত;

    *arg = skipwhite(*arg + 1);
    যদি (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	প্রত্যাগম FAIL;
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
    যদি (evalarg->eval_cctx != রিক্ত)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    অন্যথা
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, পরিমাণ(char_u *), 10);
    যদি (get_function_body(&eap, &newlines, সারি_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	গচ্ছ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_গুনতি += newlines.ga_len;
    যদি (gap->ga_itemsize > 0)
    {
	পূর্ণাঙ্ক	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	প্রতি (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    যদি (p == রিক্ত)
		// comment line in the lambda body
		অনুবর্ত;

	    p = skipwhite(p);

	    যদি (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		গচ্ছ erret;

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
	    যদি (pnl != রিক্ত)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	যদি (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    গচ্ছ erret;
	যদি (eap.nextcmd != রিক্ত)
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
	যদি (pnl != রিক্ত)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    যদি (eap.nextcmd != রিক্ত)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	যদি (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    অন্যথা
	*arg = (char_u *)"";

    যদি (!evaluate)
    {
	ret = OK;
	গচ্ছ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    যদি (ufunc == রিক্ত)
	গচ্ছ erret;
    যদি (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	গচ্ছ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = রিক্ত;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = রিক্ত;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_শীর্ষ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    যদি (parse_argument_types(ufunc, argtypes, varargs, রিক্ত, রিক্ত, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	গচ্ছ erret;
    }

    // parse the return type, if any
    যদি (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	গচ্ছ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    যদি (pt == রিক্ত)
	গচ্ছ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_সারিগুলি = newlines;
    newlines.ga_data = রিক্ত;
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
    ufunc = রিক্ত;
    ret = OK;

erret:
    যদি (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    যদি (newargs != রিক্ত)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    যদি (ufunc != রিক্ত)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    প্রত্যাগম ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    পূর্ণাঙ্ক
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	পূর্ণাঙ্ক	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = রিক্ত;
    partial_T   *pt = রিক্ত;
    পূর্ণাঙ্ক		varargs;
    char_u	*ret_type = রিক্ত;
    পূর্ণাঙ্ক		ret;
    char_u	*s;
    char_u	*start, *end;
    পূর্ণাঙ্ক		*old_eval_lavars = eval_lavars_used;
    পূর্ণাঙ্ক		eval_lavars = FALSE;
    char_u	*tofree2 = রিক্ত;
    পূর্ণাঙ্ক		equal_arrow = **arg == '(';
    পূর্ণাঙ্ক		white_error = FALSE;
    পূর্ণাঙ্ক		called_emsg_start = called_emsg;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    যদি (equal_arrow && !vim9script)
	প্রত্যাগম NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', রিক্ত,
	    types_optional ? &argtypes : রিক্ত, types_optional,
			types_optional ? &arg_objm : রিক্ত, evalarg,
			রিক্ত, &default_args, TRUE, রিক্ত, FALSE, রিক্ত, রিক্ত);
    যদি (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, রিক্ত) == রিক্ত)
    {
	যদি (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	প্রত্যাগম called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    যদি (evaluate)
	pnewargs = &newargs;
    অন্যথা
	pnewargs = রিক্ত;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : রিক্ত, types_optional,
			types_optional ? &arg_objm : রিক্ত, evalarg,
					    &varargs, &default_args,
					    FALSE, রিক্ত, FALSE, রিক্ত, রিক্ত);
    যদি (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : রিক্ত)) == রিক্ত)
    {
	যদি (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	প্রত্যাগম white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    যদি (ret_type != রিক্ত)
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
	যদি (evalarg == রিক্ত)
	    // cannot happen?
	    গচ্ছ theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	যদি (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : রিক্ত, varargs,
			   &default_args, ret_type) == FAIL)
	    গচ্ছ errret;
	গচ্ছ theend;
    }
    যদি (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_মানসমূহ_in_lambda));
	গচ্ছ errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_রাশিমালা_concatenate(arg, &start, &end, evalarg);
    যদি (ret == FAIL)
	গচ্ছ errret;

    যদি (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg != '}')
	{
	    semsg(_(e_expected_ডান_curly_str), *arg);
	    গচ্ছ errret;
	}
	++*arg;
    }

    যদি (evaluate)
    {
	পূর্ণাঙ্ক	    len;
	পূর্ণাঙ্ক	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *সারি_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	যদি (fp == রিক্ত)
	    গচ্ছ errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	যদি (pt == রিক্ত)
	    গচ্ছ errret;

	ga_init2(&newlines, পরিমাণ(char_u *), 1);
	যদি (ga_grow(&newlines, 1) == FAIL)
	    গচ্ছ errret;

	// If there are line breaks, we need to split up the string.
	সারি_end = vim_strchr(start, '\n');
	যদি (সারি_end == রিক্ত || সারি_end > end)
	    সারি_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (পূর্ণাঙ্ক)(সারি_end - start) + 1;
	p = alloc(len);
	যদি (p == রিক্ত)
	    গচ্ছ errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, সারি_end - start);

	যদি (সারি_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    যাবত্ (সারি_end < end && *সারি_end == '\n')
	    {
		যদি (ga_grow(&newlines, 1) == FAIL)
		    গচ্ছ errret;
		start = সারি_end + 1;
		সারি_end = vim_strchr(start, '\n');
		যদি (সারি_end == রিক্ত || সারি_end > end)
		    সারি_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, সারি_end - start);
	    }
	}

	যদি (strstr((বর্ণ *)p + 7, "a:") == রিক্ত)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	যদি (types_optional)
	{
	    যদি (parse_argument_types(fp, &argtypes,
				vim9script && varargs, রিক্ত, রিক্ত, 0,
				cctx) == FAIL)
		গচ্ছ errret;
	    যদি (ret_type != রিক্ত)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     রিক্ত, cctx, TRUE);
		যদি (fp->uf_ret_type == রিক্ত)
		    গচ্ছ errret;
	    }
	    অন্যথা
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_সারিগুলি = newlines;
	যদি (current_funccal != রিক্ত && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    যদি (register_closure(fp) == FAIL)
		গচ্ছ errret;
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

    প্রত্যাগম OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    যদি (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    যদি (fp != রিক্ত)
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
    প্রত্যাগম FAIL;
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
	পূর্ণাঙ্ক	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	পূর্ণাঙ্ক	    no_autoload,
	পূর্ণাঙ্ক	    new_function,
	পূর্ণাঙ্ক	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = রিক্ত;
    পূর্ণাঙ্ক		cc;
    char_u	*s = রিক্ত;
    hashtab_T	*ht;
    পূর্ণাঙ্ক		did_type = FALSE;

    যদি (partialp != রিক্ত)
	*partialp = রিক্ত;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    যদি (v != রিক্ত)
    {
	tv = &v->di_tv;
    }
    অন্যথা যদি (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	পূর্ণাঙ্ক	    len = *lenp;

	যদি (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	যদি (import != রিক্ত)
	{
	    name[len] = NUL;
	    যদি (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    অন্যথা
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    প্রত্যাগম (char_u *)"";	// just in case
	}
    }

    যদি (tv != রিক্ত)
    {
	যদি (found_var != রিক্ত)
	    *found_var = TRUE;
	যদি (tv->v_type == VAR_FUNC)
	{
	    যদি (tv->vval.v_string == রিক্ত)
	    {
		*lenp = 0;
		প্রত্যাগম (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    যদি (p != রিক্ত && generic_func_find_open_bracket(name) != রিক্ত)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		প্রত্যাগম (char_u *)"";
	    }
	    যদি (p != রিক্ত)
		*lenp = p - s;
	    অন্যথা
		*lenp = (পূর্ণাঙ্ক)STRLEN(s);
	}
	অন্যথা যদি (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    যদি (pt == রিক্ত)
	    {
		*lenp = 0;
		প্রত্যাগম (char_u *)"";	// just in case
	    }
	    যদি (partialp != রিক্ত)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (পূর্ণাঙ্ক)STRLEN(s);
	}

	যদি (s != রিক্ত)
	{
	    যদি (!did_type && type != রিক্ত && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		যদি (sv != রিক্ত)
		    *type = sv->sv_type;
	    }
	    প্রত্যাগম s;
	}
    }

    প্রত্যাগম name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    শূন্য
emsg_funcname(বর্ণ *ermsg, char_u *name)
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
    পূর্ণাঙ্ক
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	পূর্ণাঙ্ক	    partial_argc,
	typval_T    *argvars,
	পূর্ণাঙ্ক	    *argcount,
	পূর্ণাঙ্ক	    is_builtin)
{
    char_u	*argp = *arg;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    যাবত্ (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	যদি (*argp == ')' || *argp == ',' || *argp == NUL)
	    ভঙ্গ;

	পূর্ণাঙ্ক arg_idx = *argcount;
	যদি (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ভঙ্গ;
	}
	++*argcount;
	যদি (!is_builtin && check_typval_is_মান(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ভঙ্গ;
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
		ভঙ্গ;
	    }
	}
	অন্যথা
	    argp = skipwhite(argp);
	যদি (*argp != ',')
	    ভঙ্গ;
	যদি (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    যদি (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ভঙ্গ;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    যদি (*argp == ')')
	++argp;
    অন্যথা
	ret = FAIL;
    *arg = argp;
    প্রত্যাগম ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    পূর্ণাঙ্ক
get_func_tv(
    char_u	*name,		// name of the function
    পূর্ণাঙ্ক		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    পূর্ণাঙ্ক		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    পূর্ণাঙ্ক		argcount = 0;			// number of arguments found
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == রিক্ত ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    যদি (ret == OK)
    {
	পূর্ণাঙ্ক	i = 0;
	পূর্ণাঙ্ক	did_emsg_before = did_emsg;

	যদি (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    যদি (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, পরিমাণ(typval_T *), 50);
	    প্রতি (i = 0; i < argcount; ++i)
		যদি (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
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

    যাবত্ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    যদি (vim9script)
	*arg = argp;
    অন্যথা
	*arg = skipwhite(argp);
    প্রত্যাগম ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    স্থির পূর্ণাঙ্ক
eval_fname_sid(char_u *p)
{
    প্রত্যাগম (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
	প্রত্যাগম name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (পূর্ণাঙ্ক)KE_SNR;
    fname_buflen = 3;
    যদি (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    অন্যথা
    {
	যদি (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	অন্যথা
	{
	    fname_buflen += vim_snprintf((বর্ণ *)fname_buf + 3,
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
	যদি (fname == রিক্ত)
	    *error = FCERR_OTHER;
	অন্যথা
	{
	    *tofree = fname;
	    vim_snprintf((বর্ণ *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    প্রত্যাগম fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    শূন্য
func_name_with_sid(char_u *name, পূর্ণাঙ্ক sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (পূর্ণাঙ্ক)KE_SNR;
    vim_snprintf((বর্ণ *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    স্থির ufunc_T *
find_func_with_sid(char_u *name, পূর্ণাঙ্ক sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    যদি (!SCRIPT_ID_VALID(sid))
	প্রত্যাগম রিক্ত;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    যদি (!HASHITEM_EMPTY(hi))
	প্রত্যাগম HI2UF(hi);
    প্রত্যাগম রিক্ত;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    স্থির ufunc_T *
find_func_with_prefix(char_u *name, পূর্ণাঙ্ক sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    যদি (vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত)
	প্রত্যাগম রিক্ত;	// already has the prefix
    যদি (!SCRIPT_ID_VALID(sid))
	প্রত্যাগম রিক্ত;	// not in a script
    si = SCRIPT_ITEM(sid);
    যদি (si->sn_autoload_prefix != রিক্ত)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	যদি (namep == রিক্ত)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	যদি (len < পরিমাণ(buffer))
	    auto_name = buffer;
	অন্যথা
	    auto_name = alloc(len);
	যদি (auto_name != রিক্ত)
	{
	    vim_snprintf((বর্ণ *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    যদি (auto_name != buffer)
		vim_free(auto_name);
	    যদি (!HASHITEM_EMPTY(hi))
		প্রত্যাগম HI2UF(hi);
	}
    }

    প্রত্যাগম রিক্ত;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, পূর্ণাঙ্ক flags)
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
	    যদি (func != রিক্ত)
		প্রত্যাগম func;
	}
	যদি (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    যদি (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (পূর্ণাঙ্ক)sid);
		যদি (func != রিক্ত)
		    প্রত্যাগম func;
	    }
	}
    }

    যদি ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	যদি (!HASHITEM_EMPTY(hi))
	    প্রত্যাগম HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    প্রত্যাগম find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, পূর্ণাঙ্ক is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    যদি (fp != রিক্ত && (fp->uf_flags & FC_DEAD) == 0)
	প্রত্যাগম fp;
    প্রত্যাগম রিক্ত;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    পূর্ণাঙ্ক
func_is_global(ufunc_T *ufunc)
{
    প্রত্যাগম ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    পূর্ণাঙ্ক
func_requires_g_prefix(ufunc_T *ufunc)
{
    প্রত্যাগম func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == রিক্ত
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    স্থির পূর্ণাঙ্ক
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    পূর্ণাঙ্ক	len;

    যদি (!func_is_global(fp))
	len = vim_snprintf((বর্ণ *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    অন্যথা
	len = vim_snprintf((বর্ণ *)buf, bufsize, "%s", fp->uf_name);

    প্রত্যাগম (len >= (পূর্ণাঙ্ক)bufsize) ? (পূর্ণাঙ্ক)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    স্থির শূন্য
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    বর্ণ	*name,
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
    পূর্ণাঙ্ক	i;

    প্রতি (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	যদি (fp != রিক্ত && fp->uf_scoped == fc)
	    fp->uf_scoped = রিক্ত;
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
    পূর্ণাঙ্ক	may_free_fc = fc->fc_refcount <= 0;
    পূর্ণাঙ্ক	free_fc = TRUE;

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
	পূর্ণাঙ্ক	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (পূর্ণাঙ্ক)fc->fc_l_avars.dv_hashtab.ht_used;
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
	fc->fc_l_varlist.lv_first = রিক্ত;
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
	স্থির পূর্ণাঙ্ক made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	যদি (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	অন্যথা যদি (++made_copy >= (পূর্ণাঙ্ক)((4096 * 1024) / পরিমাণ(*fc)))
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
    স্থির পূর্ণাঙ্ক
numbered_function(char_u *name)
{
    প্রত্যাগম SAFE_isdigit(*name)
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
    পূর্ণাঙ্ক
func_name_refcount(char_u *name)
{
    প্রত্যাগম numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    স্থির শূন্য
funccal_unref(funccall_T *fc, ufunc_T *fp, পূর্ণাঙ্ক force)
{
    funccall_T	**pfc;
    পূর্ণাঙ্ক		i;

    যদি (fc == রিক্ত)
	প্রত্যাগম;

    যদি (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	প্রতি (pfc = &previous_funccal; *pfc != রিক্ত; pfc = &(*pfc)->fc_caller)
	{
	    যদি (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		প্রত্যাগম;
	    }
	}
    প্রতি (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	যদি (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = রিক্ত;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    স্থির পূর্ণাঙ্ক
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    যদি (fp->uf_flags & FC_DEAD)
	প্রত্যাগম FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    যদি (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	প্রত্যাগম FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    যদি (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	প্রত্যাগম FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    প্রত্যাগম TRUE;
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
    fp->uf_partial = রিক্ত;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    যদি (fp->uf_cb_free != রিক্ত)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = রিক্ত;
    }

    fp->uf_cb_state = রিক্ত;
    fp->uf_cb = রিক্ত;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_গুনতি);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    স্থির শূন্য
func_clear(ufunc_T *fp, পূর্ণাঙ্ক force)
{
    যদি (fp->uf_cleared)
	প্রত্যাগম;
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
    স্থির পূর্ণাঙ্ক
func_free(ufunc_T *fp, পূর্ণাঙ্ক force)
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
	প্রত্যাগম OK;
    }
    প্রত্যাগম FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    শূন্য
func_clear_free(ufunc_T *fp, পূর্ণাঙ্ক force)
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
    পূর্ণাঙ্ক
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = রিক্ত;

    যদি (ufunc == রিক্ত)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	প্রত্যাগম FAIL;
    }

    fp = find_func(global, TRUE);
    যদি (fp != রিক্ত)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	প্রত্যাগম FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    যদি (fp == রিক্ত)
	প্রত্যাগম FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    যদি (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_সারিগুলি, &fp->uf_সারিগুলি) == FAIL)
	গচ্ছ failed;

    যদি (ufunc->uf_arg_types != রিক্ত)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	যদি (fp->uf_arg_types == রিক্ত)
	    গচ্ছ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    পরিমাণ(type_T *) * fp->uf_args.ga_len);
    }
    যদি (ufunc->uf_va_name != রিক্ত)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	যদি (fp->uf_va_name == রিক্ত)
	    গচ্ছ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    যদি ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == রিক্ত)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	যদি (pt == রিক্ত)
	    গচ্ছ failed;
	যদি (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    গচ্ছ failed;
	}
	ufunc->uf_partial = pt;
    }

    প্রত্যাগম OK;

failed:
    func_clear_free(fp, TRUE);
    প্রত্যাগম FAIL;
}

স্থির পূর্ণাঙ্ক	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    পূর্ণাঙ্ক
funcdepth_increment(শূন্য)
{
    যদি (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	প্রত্যাগম FAIL;
    }
    ++funcdepth;
    প্রত্যাগম OK;
}

    শূন্য
funcdepth_decrement(শূন্য)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    পূর্ণাঙ্ক
funcdepth_get(শূন্য)
{
    প্রত্যাগম funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    শূন্য
funcdepth_restore(পূর্ণাঙ্ক depth)
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

    যদি (fc == রিক্ত)
	প্রত্যাগম রিক্ত;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    প্রত্যাগম fc;
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
    পূর্ণাঙ্ক		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    পূর্ণাঙ্ক		using_sandbox = FALSE;
    পূর্ণাঙ্ক		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    পূর্ণাঙ্ক		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    পূর্ণাঙ্ক		default_arg_err = FALSE;
    dictitem_T	*v;
    পূর্ণাঙ্ক		fixvar_idx = 0;	// index in fc_fixvar[]
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ai;
    পূর্ণাঙ্ক		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    পূর্ণাঙ্ক		tv_to_free_len = 0;
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
	প্রত্যাগম FCERR_FAILED;
    }

    সারি_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    যদি (fc == রিক্ত)
	প্রত্যাগম FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, পরিমাণ(ufunc_T *), 1);

    যদি (fp->uf_def_status != UF_NOT_COMPILED)
    {
	যদি (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == রিক্ত ? রিক্ত : fc->fc_caller->fc_func;
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
				  || (caller != রিক্ত && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	প্রত্যাগম retval;
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
    যদি (selfdict != রিক্ত)
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
    প্রতি (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	পূর্ণাঙ্ক	    addlocal = FALSE;
	typval_T    def_rettv;
	পূর্ণাঙ্ক	    isdefault = FALSE;

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
		char_u	    *default_রাশিমালা = রিক্ত;

		def_rettv.v_type = VAR_সংখ্যা;
		def_rettv.vval.v_সংখ্যা = -1;

		default_রাশিমালা = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		যদি (eval1(&default_রাশিমালা, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ভঙ্গ;
		}
	    }

	    namelen = STRLEN(name);
	}
	অন্যথা
	{
	    যদি ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ভঙ্গ;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((বর্ণ *)numbuf, পরিমাণ(numbuf), "%d", ai + 1);
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
	    যদি (v == রিক্ত)
		ভঙ্গ;
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
	    প্রতি (i = 0; i < argcount; ++i)
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
		    যদি (s != রিক্ত)
		    {
			যদি (vim_strsize(s) > বার্তা_BUF_CLEN)
			{
			    trunc_string(s, buf, বার্তা_BUF_CLEN, বার্তা_BUF_LEN);
			    s = buf;
			}
			বার্তা_puts((বর্ণ *)s);
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
			fc->fc_caller == রিক্ত ? রিক্ত : fc->fc_caller->fc_func);
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
	char_u *p = *(char_u **)fp->uf_সারিগুলি.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(শূন্য)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    অন্যথা
	// call do_cmdline() to execute the lines
	do_cmdline(রিক্ত, get_func_সারি, (শূন্য *)fc,
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
	ufunc_T *caller = fc->fc_caller == রিক্ত ? রিক্ত : fc->fc_caller->fc_func;

	যদি (fp->uf_profiling || (caller != রিক্ত && caller->uf_profiling))
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
	    যদি (s != রিক্ত)
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

    যদি (p_verbose >= 12 && SOURCING_NAME != রিক্ত)
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
    প্রতি (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    প্রত্যাগম retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, পূর্ণাঙ্ক argcount)
{
    পূর্ণাঙ্ক regular_args = fp->uf_args.ga_len;

    যদি (argcount < regular_args - fp->uf_def_args.ga_len)
	প্রত্যাগম FCERR_TOOFEW;
    অন্যথা যদি (!has_varargs(fp) && argcount > regular_args)
	প্রত্যাগম FCERR_TOOMANY;
    প্রত্যাগম FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	পূর্ণাঙ্ক	    argcount,
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

	প্রত্যাগম (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    যদি (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != রিক্ত)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    যদি (error != FCERR_UNKNOWN)
	প্রত্যাগম error;

    যদি ((fp->uf_flags & FC_DICT) && selfdict == রিক্ত)
    {
	error = FCERR_DICT;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক		did_save_redo = FALSE;
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
				   (fp->uf_flags & FC_DICT) ? selfdict : রিক্ত);
	যদি (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	যদি (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_অনুসন্ধান_patterns();
    }

    প্রত্যাগম error;
}

স্থির funccal_entry_T *funccal_স্তূপ = রিক্ত;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    শূন্য
save_funccal(funccal_entry_T *entry)
{
    entry->শীর্ষ_funccal = current_funccal;
    entry->next = funccal_স্তূপ;
    funccal_স্তূপ = entry;
    current_funccal = রিক্ত;
}

    শূন্য
restore_funccal(শূন্য)
{
    যদি (funccal_স্তূপ == রিক্ত)
	internal_error("restore_funccal()");
    অন্যথা
    {
	current_funccal = funccal_স্তূপ->শীর্ষ_funccal;
	funccal_স্তূপ = funccal_স্তূপ->next;
    }
}

    funccall_T *
get_current_funccal(শূন্য)
{
    প্রত্যাগম current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    পূর্ণাঙ্ক
at_script_level(শূন্য)
{
    প্রত্যাগম current_funccal == রিক্ত && autocmd_match == রিক্ত;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    শূন্য
delete_script_functions(পূর্ণাঙ্ক sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (পূর্ণাঙ্ক)KE_SNR;
    len = 3 + vim_snprintf((বর্ণ *)buf + 3, পরিমাণ(buf) - 3, "%d_", sid);

    যাবত্ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_বস্তুসমূহ(&func_hashtab, hi, todo)
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		যদি (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    পূর্ণাঙ্ক changed = func_hashtab.ht_changed;

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
			    ভঙ্গ;
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
    পূর্ণাঙ্ক		changed;

    // Clean up the current_funccal chain and the funccal stack.
    যাবত্ (current_funccal != রিক্ত)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	যদি (current_funccal == রিক্ত && funccal_স্তূপ != রিক্ত)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    যাবত্ (todo > 0)
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
			ভঙ্গ;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    যাবত্ (func_hashtab.ht_used > skipped)
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
			ভঙ্গ;
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
    পূর্ণাঙ্ক
builtin_function(char_u *name, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক i;

    যদি (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	প্রত্যাগম FALSE;
    প্রতি (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	যদি (name[i] == AUTOLOAD_CHAR)
	    প্রত্যাগম FALSE;
	যদি (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    যদি (name[i] == '.')
		প্রত্যাগম FALSE;
	    ভঙ্গ;
	}
    }
    প্রত্যাগম TRUE;
}

    পূর্ণাঙ্ক
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
    পূর্ণাঙ্ক		argc = 0;
    পূর্ণাঙ্ক		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_বস্তুসমূহ(l, item)
    {
	যদি (argc == MAX_FUNC_ARGS - (partial == রিক্ত ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ভঙ্গ;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    যদি (item == রিক্ত)
    {
	funcexe_T	funcexe;
	পূর্ণাঙ্ক		namelen = -1;

	যদি (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    যদি (p != রিক্ত)
	    {
		যদি (skip_generic_func_type_args(&p) == FAIL)
		    গচ্ছ done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	যদি (partial != রিক্ত)
	{
	    funcexe.fe_object = partial->pt_obj;
	    যদি (funcexe.fe_object != রিক্ত)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    যাবত্ (argc > 0)
	clear_tv(&argv[--argc]);

done:
    প্রত্যাগম r;
}

স্থির পূর্ণাঙ্ক callback_depth = 0;

    পূর্ণাঙ্ক
get_callback_depth(শূন্য)
{
    প্রত্যাগম callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    পূর্ণাঙ্ক
call_callback(
    callback_T	*callback,
    পূর্ণাঙ্ক		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    পূর্ণাঙ্ক		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    পূর্ণাঙ্ক		ret;

    যদি (callback->cb_name == রিক্ত || *callback->cb_name == NUL)
	প্রত্যাগম FAIL;

    যদি (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	প্রত্যাগম FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    যদি (callback->cb_partial != রিক্ত)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	যদি (funcexe.fe_object != রিক্ত)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    যদি (need_rethrow && current_exception != রিক্ত && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    প্রত্যাগম ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    পূর্ণাঙ্ক		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    যদি (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	প্রত্যাগম -2;

    retval = tv_get_সংখ্যা_chk(&rettv, রিক্ত);
    clear_tv(&rettv);
    প্রত্যাগম retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    শূন্য
user_func_error(funcerror_T error, char_u *name, পূর্ণাঙ্ক found_var)
{
    বিকল্প (error)
    {
	পক্ষ FCERR_UNKNOWN:
		যদি (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		অন্যথা
		    emsg_funcname(e_unknown_function_str, name);
		ভঙ্গ;
	পক্ষ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		ভঙ্গ;
	পক্ষ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ভঙ্গ;
	পক্ষ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ভঙ্গ;
	পক্ষ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ভঙ্গ;
	পক্ষ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		ভঙ্গ;
	পক্ষ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ভঙ্গ;
	পক্ষ FCERR_OTHER:
	পক্ষ FCERR_FAILED:
		// assume the error message was already given
		ভঙ্গ;
	পক্ষ FCERR_NONE:
		ভঙ্গ;
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
	পূর্ণাঙ্ক	    argcount,
	পূর্ণাঙ্ক	    base_included,
	char_u	    *name)
{
    যদি (funcexe->fe_check_type != রিক্ত && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	যদি (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? রিক্ত : funcexe->fe_basetv,
			  name) == FAIL)
	    প্রত্যাগম FCERR_OTHER;
    }
    প্রত্যাগম FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    পূর্ণাঙ্ক
call_func(
    char_u	*funcname,	// name of the function
    পূর্ণাঙ্ক		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    পূর্ণাঙ্ক		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    পূর্ণাঙ্ক		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    পূর্ণাঙ্ক		i;
    ufunc_T	*fp = রিক্ত;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = রিক্ত;
    char_u	*fname = রিক্ত;
    char_u	*name = রিক্ত;
    পূর্ণাঙ্ক		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    পূর্ণাঙ্ক		argv_clear = 0;
    পূর্ণাঙ্ক		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    যদি (partial != রিক্ত)
	fp = partial->pt_func;
    যদি (fp == রিক্ত)
	fp = funcexe->fe_ufunc;

    যদি (fp == রিক্ত)
    {
	যদি (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    যদি (p != রিক্ত)
	    {
		len = p - funcname;
		যদি (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == রিক্ত)
		    গচ্ছ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	যদি (name == রিক্ত)
	    গচ্ছ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    যদি (funcexe->fe_doesrange != রিক্ত)
	*funcexe->fe_doesrange = FALSE;

    যদি (partial != রিক্ত)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	যদি (partial->pt_dict != রিক্ত && (selfdict == রিক্ত || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	যদি (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    প্রতি (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		যদি (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    গচ্ছ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    প্রতি (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    যদি (funcexe->fe_check_type != রিক্ত
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		প্রতি (i = 0; i < check_type.tt_argcount; ++i)
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
					     (name != রিক্ত) ? name : funcname);

    যদি (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	পূর্ণাঙ্ক	is_global = FALSE;

	// Skip "g:" before a function name.
	যদি (fp == রিক্ত && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_সংখ্যা;	// default rettv is number zero
	rettv->vval.v_সংখ্যা = 0;
	error = FCERR_UNKNOWN;

	যদি (fp != রিক্ত || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    যদি (fp == রিক্ত)
	    {
		fp = find_func(rfname, is_global);
		যদি (fp != রিক্ত && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = রিক্ত;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    যদি (fp == রিক্ত
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, রিক্ত)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    যদি (fp == রিক্ত && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    যদি (fp == রিক্ত)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		যদি (p != রিক্ত && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    যদি (partial == রিক্ত && fp != রিক্ত && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		যদি (fp == রিক্ত)
		{
		    error = FCERR_FAILED;
		    গচ্ছ theend;
		}
	    }
	    অন্যথা যদি (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != রিক্ত ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		গচ্ছ theend;
	    }

	    যদি (fp != রিক্ত && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    অন্যথা যদি (fp != রিক্ত)
	    {
		পূর্ণাঙ্ক need_arg_check = FALSE;
		যদি (funcexe->fe_check_type == রিক্ত)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		যদি (funcexe->fe_argv_func != রিক্ত)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		যদি (funcexe->fe_basetv != রিক্ত)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, পরিমাণ(typval_T) * argcount);
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
				       TRUE, (name != রিক্ত) ? name : funcname);

		যদি (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	অন্যথা যদি (funcexe->fe_basetv != রিক্ত)
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
	user_func_error(error, (name != রিক্ত) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    যাবত্ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    প্রত্যাগম ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    পূর্ণাঙ্ক
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    পূর্ণাঙ্ক		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = রিক্ত;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    পূর্ণাঙ্ক		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_সংখ্যা;	// default rettv is number zero
    rettv->vval.v_সংখ্যা = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    যদি (name == রিক্ত)
	প্রত্যাগম ret;

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
    যদি (fp != রিক্ত && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = রিক্ত;
    যদি (fp == রিক্ত)
	ret = NOTDONE;
    অন্যথা যদি (fp != রিক্ত && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    অন্যথা যদি (fp != রিক্ত)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, রিক্ত);
	যদি (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    প্রত্যাগম ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    প্রত্যাগম fp->uf_name_exp != রিক্ত ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    স্থির পূর্ণাঙ্ক
function_list_modified(পূর্ণাঙ্ক prev_ht_changed)
{
    যদি (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    স্থির পূর্ণাঙ্ক
list_func_head(ufunc_T *fp, পূর্ণাঙ্ক indent)
{
    পূর্ণাঙ্ক		prev_ht_changed = func_hashtab.ht_changed;
    পূর্ণাঙ্ক		j;

    বার্তা_start();

    // a timer at the more prompt may have deleted the function
    যদি (function_list_modified(prev_ht_changed))
	প্রত্যাগম FAIL;

    যদি (indent)
	বার্তা_puts("   ");
    যদি (fp->uf_def_status != UF_NOT_COMPILED)
	বার্তা_puts("def ");
    অন্যথা
	বার্তা_puts("function ");
    বার্তা_puts((বর্ণ *)printable_func_name(fp));
    বার্তা_putchar('(');
    প্রতি (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	যদি (j)
	    বার্তা_puts(", ");
	বার্তা_puts((বর্ণ *)FUNCARG(fp, j));
	যদি (fp->uf_arg_types != রিক্ত)
	{
	    বর্ণ *tofree;

	    বার্তা_puts(": ");
	    বার্তা_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	যদি (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    বার্তা_puts(" = ");
	    বার্তা_puts(((বর্ণ **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    যদি (fp->uf_varargs)
    {
	যদি (j)
	    বার্তা_puts(", ");
	বার্তা_puts("...");
    }
    যদি (fp->uf_va_name != রিক্ত)
    {
	যদি (!fp->uf_varargs)
	{
	    যদি (j)
		বার্তা_puts(", ");
	    বার্তা_puts("...");
	}
	বার্তা_puts((বর্ণ *)fp->uf_va_name);
	যদি (fp->uf_va_type != রিক্ত)
	{
	    বর্ণ *tofree;

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
	    বর্ণ *tofree;

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

    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক		*is_global,
    পূর্ণাঙ্ক		skip,		// only find the end, don't evaluate
    পূর্ণাঙ্ক		flags)
{
    প্রত্যাগম trans_function_name_ext(pp, is_global, skip, flags,
	    রিক্ত, রিক্ত, রিক্ত, রিক্ত);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    স্থির char_u *
trans_function_name_ext(
    char_u	**pp,
    পূর্ণাঙ্ক		*is_global,
    পূর্ণাঙ্ক		skip,		// only find the end, don't evaluate
    পূর্ণাঙ্ক		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = রিক্ত;
    char_u	*start;
    char_u	*end;
    পূর্ণাঙ্ক		lead;
    char_u	sid_buf[20];
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		extra = 0;
    পূর্ণাঙ্ক		prefix_g = FALSE;
    lval_T	lv;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		vim9_local;

    যদি (fdp != রিক্ত)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    যদি ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (পূর্ণাঙ্ক)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	প্রত্যাগম vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    যদি (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, রিক্ত, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    যদি (end == start || (vim9script && end != রিক্ত
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	যদি (!skip)
	    emsg(_(e_function_name_required));
	গচ্ছ theend;
    }
    যদি (end == রিক্ত || (lv.ll_tv != রিক্ত && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	যদি (!aborting())
	{
	    যদি (end != রিক্ত)
		semsg(_(e_invalid_argument_str), start);
	}
	অন্যথা
	    *pp = find_name_end(start, রিক্ত, রিক্ত, FNE_INCL_BR);
	গচ্ছ theend;
    }

    যদি (lv.ll_ufunc != রিক্ত)
    {
	যদি (ufunc != রিক্ত)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	গচ্ছ theend;
    }

    যদি (lv.ll_tv != রিক্ত)
    {
	যদি (fdp != রিক্ত)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = রিক্ত;
	    fdp->fd_di = lv.ll_di;
	}
	যদি (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != রিক্ত)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	অন্যথা যদি (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != রিক্ত)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	অন্যথা যদি (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != রিক্ত)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    যদি (partial != রিক্ত)
		*partial = lv.ll_tv->vval.v_partial;
	}
	অন্যথা
	{
	    যদি (!skip && !(flags & TFN_QUIET) && (fdp == রিক্ত
			     || lv.ll_dict == রিক্ত || fdp->fd_newkey == রিক্ত))
		emsg(_(e_funcref_required));
	    অন্যথা
		*pp = end;
	    name = রিক্ত;
	}
	গচ্ছ theend;
    }

    যদি (lv.ll_name == রিক্ত)
    {
	// Error found, but continue after the function name.
	*pp = end;
	গচ্ছ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    যদি (lv.ll_exp_name != রিক্ত)
    {
	len = (পূর্ণাঙ্ক)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, রিক্ত);
	যদি (name == lv.ll_exp_name)
	    name = রিক্ত;
    }
    অন্যথা যদি (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	পূর্ণাঙ্ক		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	যদি (si->sn_autoload_prefix != রিক্ত)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	অন্যথা
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (পূর্ণাঙ্ক)KE_SNR;
	    vim_snprintf((বর্ণ *)sid_buf + 3, পরিমাণ(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	গচ্ছ theend;
    }
    অন্যথা যদি (!(flags & TFN_NO_DEREF))
    {
	len = (পূর্ণাঙ্ক)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, রিক্ত);
	যদি (name == *pp)
	    name = রিক্ত;
    }
    যদি (name != রিক্ত)
    {
	name = vim_strsave(name);
	*pp = end;
	যদি (name != রিক্ত && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (পূর্ণাঙ্ক)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	গচ্ছ theend;
    }

    যদি (lv.ll_exp_name != রিক্ত)
    {
	len = (পূর্ণাঙ্ক)STRLEN(lv.ll_exp_name);
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
		যদি (is_global != রিক্ত)
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
	len = (পূর্ণাঙ্ক)(end - lv.ll_name);
    }
    যদি (len <= 0)
    {
	যদি (!skip)
	    emsg(_(e_function_name_required));
	গচ্ছ theend;
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
		গচ্ছ theend;
	    }
	    lead = 3;
	}
	যদি (vim9_local || (lv.ll_exp_name != রিক্ত
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    যদি (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		গচ্ছ theend;
	    }
	    sid_buflen = vim_snprintf((বর্ণ *)sid_buf, পরিমাণ(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    যদি (vim9_local)
		extra = 3 + (পূর্ণাঙ্ক)sid_buflen;
	    অন্যথা
		lead += (পূর্ণাঙ্ক)sid_buflen;
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
	গচ্ছ theend;
    }
    যদি (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	যদি (cp != রিক্ত && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    গচ্ছ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    যদি (name != রিক্ত)
    {
	যদি (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (পূর্ণাঙ্ক)KE_SNR;
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
    প্রত্যাগম name;
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
	যদি (p != রিক্ত)
	    প্রত্যাগম p + 1;
    }
    প্রত্যাগম রিক্ত;
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
    বর্ণ	sid_buf[25];
    size_t	sid_buflen;
    পূর্ণাঙ্ক		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    যদি (funcname == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	যদি (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    প্রত্যাগম রিক্ত;
	ufunc = find_func(funcname, FALSE);
	যদি (ufunc == রিক্ত || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == রিক্ত)
	    প্রত্যাগম রিক্ত;
	++p;
	off = 0;
    }
    অন্যথা
	off = *funcname == 's' ? 2 : 5;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	প্রত্যাগম রিক্ত;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, পরিমাণ(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    যদি (newname == রিক্ত)
	প্রত্যাগম রিক্ত;
    vim_snprintf((বর্ণ *)newname, newnamesize, "%s%s", sid_buf, p + off);

    প্রত্যাগম newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    যদি (n != রিক্ত)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    প্রত্যাগম n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	পূর্ণাঙ্ক	    *is_global,
	পূর্ণাঙ্ক	    skip,
	পূর্ণাঙ্ক	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    যদি (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(শূন্য)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	যদি (fudi != রিক্ত)
	    CLEAR_POINTER(fudi);
    }
    অন্যথা
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, রিক্ত, রিক্ত, রিক্ত);
    *name = p;
    প্রত্যাগম saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    শূন্য
list_functions(regmatch_T *regmatch)
{
    পূর্ণাঙ্ক		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    প্রতি (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    যদি ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == রিক্ত
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		যদি (list_func_head(fp, FALSE) == FAIL)
		    প্রত্যাগম;
		যদি (function_list_modified(prev_ht_changed))
		    প্রত্যাগম;
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
	যদি (regmatch.regprog != রিক্ত)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    যদি (*p == '/')
	++p;

    প্রত্যাগম p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    স্থির ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, পূর্ণাঙ্ক is_global)
{
    ufunc_T	*fp = রিক্ত;
    পূর্ণাঙ্ক		j;

    যদি (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	প্রত্যাগম রিক্ত;
    }

    set_nextcmd(eap, p);

    যদি (eap->nextcmd != রিক্ত)
	*p = NUL;

    যদি (eap->skip || got_int)
	প্রত্যাগম রিক্ত;

    fp = find_func(name, is_global);
    যদি (fp == রিক্ত && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	যদি (up != রিক্ত)
	    fp = find_func(up, FALSE);
    }

    যদি (fp == রিক্ত)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	প্রত্যাগম রিক্ত;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    পূর্ণাঙ্ক prev_ht_changed = func_hashtab.ht_changed;

    যদি (list_func_head(fp, TRUE) != OK)
	প্রত্যাগম fp;

    প্রতি (j = 0; j < fp->uf_সারিগুলি.ga_len && !got_int; ++j)
    {
	যদি (FUNCLINE(fp, j) == রিক্ত)
	    অনুবর্ত;
	বার্তা_putchar('\n');
	বার্তা_outnum((long)(j + 1));
	যদি (j < 9)
	    বার্তা_putchar(' ');
	যদি (j < 99)
	    বার্তা_putchar(' ');
	যদি (function_list_modified(prev_ht_changed))
	    ভঙ্গ;
	বার্তা_prt_সারি(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    যদি (!got_int)
    {
	বার্তা_putchar('\n');
	যদি (!function_list_modified(prev_ht_changed))
	{
	    যদি (fp->uf_def_status != UF_NOT_COMPILED)
		বার্তা_puts("   enddef");
	    অন্যথা
		বার্তা_puts("   endfunction");
	}
    }

    প্রত্যাগম fp;
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
	পূর্ণাঙ্ক	    class_flags,
	ocmember_T  *obj_members,
	পূর্ণাঙ্ক         obj_member_গুনতি,
	cctx_T	    *cctx)
{
    পূর্ণাঙ্ক		j;
    পূর্ণাঙ্ক		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    পূর্ণাঙ্ক		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*সারি_arg = রিক্ত;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    পূর্ণাঙ্ক		varargs = FALSE;
    পূর্ণাঙ্ক		flags = 0;
    char_u	*ret_type = রিক্ত;
    ufunc_T	*fp = রিক্ত;
    পূর্ণাঙ্ক		fp_allocated = FALSE;
    পূর্ণাঙ্ক		free_fp = FALSE;
    পূর্ণাঙ্ক		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    স্থির পূর্ণাঙ্ক	func_nr = 0;	    // number for nameless function
    পূর্ণাঙ্ক		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_শীর্ষ;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    imported_T	*import = রিক্ত;

    // ":function" without argument: list functions.
    যদি (ends_excmd2(eap->cmd, eap->arg))
    {
	যদি (!eap->skip)
	    list_functions(রিক্ত);
	set_nextcmd(eap, eap->arg);
	প্রত্যাগম রিক্ত;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    যদি (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	প্রত্যাগম রিক্ত;
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
    যদি (name_arg != রিক্ত)
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
		প্রত্যাগম রিক্ত;
	    }
	    p = to_name_end(p, TRUE);
	    যদি (*skipwhite(p) == '.' && vim_strchr(p, '(') != রিক্ত)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		প্রত্যাগম রিক্ত;
	    }

	    p = eap->arg;
	}

	পূর্ণাঙ্ক tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != রিক্ত);
	যদি (name == রিক্ত && (fudi.fd_dict == রিক্ত || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    যদি (!aborting())
	    {
		যদি (!eap->skip && fudi.fd_newkey != রিক্ত)
		    semsg(_(e_চাবি_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		প্রত্যাগম রিক্ত;
	    }
	    অন্যথা
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	যদি (is_export && name != রিক্ত)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    যদি (prefixed != রিক্ত && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	অন্যথা যদি (paren && vim9script && name != রিক্ত
				    && vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    গচ্ছ ret_free;
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
	গচ্ছ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    যদি (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	যদি (p == রিক্ত)
	    গচ্ছ ret_free;
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
	    গচ্ছ ret_free;
	}
	// attempt to continue by skipping some text
	যদি (vim_strchr(p, '(') != রিক্ত)
	    p = vim_strchr(p, '(');
    }

    যদি ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	গচ্ছ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    যদি (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	গচ্ছ ret_free;
    }

    ga_init2(&newlines, পরিমাণ(char_u *), 10);

    যদি (!eap->skip && name_arg == রিক্ত)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	যদি (name != রিক্ত)
	    arg = name;
	অন্যথা
	    arg = fudi.fd_newkey;
	যদি (arg != রিক্ত && (fudi.fd_di == রিক্ত
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    পূর্ণাঙ্ক	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    যদি (arg != fudi.fd_newkey)
	    {
		যদি (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    যদি (name_base == রিক্ত)
			name_base = arg + 3;
		    অন্যথা
			++name_base;
		}
		প্রতি (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		যদি (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    গচ্ছ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		যদি (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, রিক্ত,
			রিক্ত, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    গচ্ছ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	যদি (fudi.fd_dict != রিক্ত && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    গচ্ছ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    যদি (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : রিক্ত, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : রিক্ত,
			 রিক্ত, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, সারিগুলি_to_free) == FAIL)
	গচ্ছ errret_2;
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
		ret_type = রিক্ত;
	    }
	}
	p = skipwhite(p);
    }
    অন্যথা
	// find extra arguments "range", "dict", "abort" and "closure"
	প্রতি (;;)
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
		যদি (current_funccal == রিক্ত)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_শীর্ষ_level_str,
			    name == রিক্ত ? (char_u *)"" : name);
		    গচ্ছ erret;
		}
	    }
	    অন্যথা
		ভঙ্গ;
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
	    যদি (fudi.fd_dict != রিক্ত && fudi.fd_newkey == রিক্ত)
		emsg(_(e_dictionary_entry_already_exists));
	    অন্যথা যদি (name != রিক্ত && find_func(name, is_global) != রিক্ত)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	যদি (!eap->skip && did_emsg)
	    গচ্ছ erret;

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
	গচ্ছ erret;

    /*
     * If there are no errors, add the function
     */
    যদি (fudi.fd_dict != রিক্ত)
    {
	বর্ণ	numbuf[NUMBUFLEN];

	fp = রিক্ত;
	যদি (fudi.fd_newkey == রিক্ত && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    গচ্ছ erret;
	}
	যদি (fudi.fd_di == রিক্ত)
	{
	    // Can't add a function to a locked dictionary
	    যদি (মান_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		গচ্ছ erret;
	}
	    // Can't change an existing function if it is locked
	অন্যথা যদি (মান_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    গচ্ছ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, পরিমাণ(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	যদি (name == রিক্ত)
	    গচ্ছ erret;
    }
    অন্যথা যদি (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	পূর্ণাঙ্ক		var_conflict = FALSE;
	পূর্ণাঙ্ক		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	যদি (v != রিক্ত && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	যদি (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    যদি (si->sn_autoload_prefix != রিক্ত)
	    {
		যদি (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    যদি (v != রিক্ত)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		অন্যথা
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    যদি (prefixed != রিক্ত && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			যদি (v != রিক্ত)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	যদি (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    গচ্ছ erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	যদি (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == রিক্ত ? name : uname, 0, FALSE);
	}

	যদি (fp != রিক্ত || import != রিক্ত)
	{
	    পূর্ণাঙ্ক dead = fp != রিক্ত && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    যদি (import != রিক্ত
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_শীর্ষ;
		যদি (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		অন্যথা
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		গচ্ছ errret_keep;
	    }
	    যদি (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		গচ্ছ errret_keep;
	    }
	    যদি (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = রিক্ত;
		overwrite = TRUE;
	    }
	    অন্যথা
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = রিক্ত;
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

    যদি (fp == রিক্ত)
    {
	যদি (fudi.fd_dict == রিক্ত && vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত)
	{
	    পূর্ণাঙ্ক	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    যদি (SOURCING_NAME != রিক্ত)
	    {
		scriptname = autoload_name(name);
		যদি (scriptname != রিক্ত)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (পূর্ণাঙ্ক)STRLEN(p);
		    slen = (পূর্ণাঙ্ক)STRLEN(SOURCING_NAME);
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
		গচ্ছ erret;
	    }
	}

	যদি (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	যদি (fp == রিক্ত)
	    গচ্ছ erret;
	fp_allocated = TRUE;

	যদি (fudi.fd_dict != রিক্ত)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    যদি (func_name == রিক্ত)
	    {
		VIM_CLEAR(fp);
		গচ্ছ erret;
	    }
	    যদি (fudi.fd_di == রিক্ত)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		যদি (fudi.fd_di == রিক্ত)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    গচ্ছ erret;
		}
		যদি (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    গচ্ছ erret;
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
	পূর্ণাঙ্ক	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	যদি (generic_func_args_table_size(&gfatab) > 0)
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
	পূর্ণাঙ্ক save_is_export = is_export;
	is_export = FALSE;

	যদি (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_গুনতি, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    গচ্ছ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	যদি (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    গচ্ছ erret;
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
	    গচ্ছ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_সারিগুলি = newlines;
    newlines.ga_data = রিক্ত;
    যদি ((flags & FC_CLOSURE) != 0)
    {
	যদি (register_closure(fp) == FAIL)
	    গচ্ছ erret;
    }
    অন্যথা
	fp->uf_scoped = রিক্ত;

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
	defcompile_function(fp, রিক্ত);

    গচ্ছ ret_free;

erret:
    যদি (fp != রিক্ত)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    যদি (fp != রিক্ত)
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

    প্রত্যাগম fp;
}

/*
 * ":function"
 */
    শূন্য
ex_function(exarg_T *eap)
{
    garray_T সারিগুলি_to_free;

    ga_init2(&সারিগুলি_to_free, পরিমাণ(char_u *), 50);
    (শূন্য)define_function(eap, রিক্ত, &সারিগুলি_to_free, 0, রিক্ত, 0, রিক্ত);
    ga_clear_strings(&সারিগুলি_to_free);
}

    পূর্ণাঙ্ক
get_func_arity(char_u *name, পূর্ণাঙ্ক *required, পূর্ণাঙ্ক *optional, পূর্ণাঙ্ক *varargs)
{
    ufunc_T	*ufunc = রিক্ত;
    পূর্ণাঙ্ক		argcount = 0;
    পূর্ণাঙ্ক		min_argcount = 0;
    পূর্ণাঙ্ক		idx;

    idx = find_internal_func(name);
    যদি (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    অন্যথা
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = রিক্ত;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	যদি (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	যদি (ufunc == রিক্ত)
	    প্রত্যাগম FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক		is_global = FALSE;
    char_u	*bracket_start = রিক্ত;
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
	যদি (ufunc != রিক্ত)
	    প্রত্যাগম ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      রিক্ত, রিক্ত, রিক্ত, &ufunc);
	যদি (ufunc != রিক্ত)
	{
	    vim_free(fname);
	    প্রত্যাগম ufunc;
	}
	যদি (fname != রিক্ত && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    যদি (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		প্রত্যাগম রিক্ত;
	    }
	}
    }
    যদি (fname == রিক্ত)
    {
	semsg(_(e_invalid_argument_str), name);
	প্রত্যাগম রিক্ত;
    }
    যদি (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	প্রত্যাগম রিক্ত;
    }

    ufunc = find_func(fname, is_global);
    যদি (ufunc == রিক্ত)
    {
	char_u *p = untrans_function_name(fname);

	যদি (p != রিক্ত)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	যদি (ufunc == রিক্ত)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    যদি (bracket_start != রিক্ত)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    যদি (ufunc != রিক্ত)
    {
	// handle generic functions
	যদি (bracket_start == রিক্ত)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    যদি (ufunc == রিক্ত)
	semsg(_(e_cannot_find_function_str), name);
    প্রত্যাগম ufunc;
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
	(শূন্য)compile_def_function(ufunc, FALSE, compile_type, রিক্ত);
    অন্যথা
	smsg(_("Function %s%s%s does not need compiling"),
				cl != রিক্ত ? cl->class_name.string : (char_u *)"",
				cl != রিক্ত ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    স্থির শূন্য
defcompile_funcs_in_script(শূন্য)
{
    long	todo = (long)func_hashtab.ht_used;
    পূর্ণাঙ্ক		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    প্রতি (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    যদি (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(শূন্য)compile_def_function(ufunc, FALSE, CT_NONE, রিক্ত);

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

	    যদি (cl != রিক্ত)
		defcompile_class(cl);
	}
	অন্যথা
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    যদি (ufunc != রিক্ত)
		defcompile_function(ufunc, রিক্ত);
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
    পূর্ণাঙ্ক
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    যদি (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	প্রত্যাগম 5;
    যদি (p[0] == 's' && p[1] == ':')
	প্রত্যাগম 2;
    প্রত্যাগম 0;
}

    পূর্ণাঙ্ক
translated_function_exists(char_u *name, পূর্ণাঙ্ক is_global)
{
    যদি (builtin_function(name, -1))
	প্রত্যাগম has_internal_func(name);
    প্রত্যাগম find_func(name, is_global) != রিক্ত;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    পূর্ণাঙ্ক
has_varargs(ufunc_T *ufunc)
{
    প্রত্যাগম ufunc->uf_varargs || ufunc->uf_va_name != রিক্ত;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    পূর্ণাঙ্ক
function_exists(char_u *name, পূর্ণাঙ্ক no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    পূর্ণাঙ্ক	    n = FALSE;
    পূর্ণাঙ্ক	    flag;
    পূর্ণাঙ্ক	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    যদি (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    যদি (p != রিক্ত && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    প্রত্যাগম n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    পূর্ণাঙ্ক
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    পূর্ণাঙ্ক		is_global = FALSE;
    ufunc_T	*fp = রিক্ত;
    list_T	*args;
    পূর্ণাঙ্ক		is_def;
    বর্ণ	*tofree;

    যদি (*name == NUL)
	প্রত্যাগম FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    যদি (p != রিক্ত && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    যদি (fp == রিক্ত)
	প্রত্যাগম FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    যদি (args == রিক্ত || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	প্রত্যাগম FAIL;
    }
    প্রতি (পূর্ণাঙ্ক j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	যদি (arg == রিক্ত || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    প্রত্যাগম FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	যদি (fp->uf_arg_types == রিক্ত)
	    dict_add_string(arg, "type", (char_u *)"any");
	অন্যথা
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	যদি (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    যদি (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	যদি (va == রিক্ত || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    প্রত্যাগম FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == রিক্ত ? (char_u *)"" : fp->uf_va_name);
	যদি (fp->uf_va_type == রিক্ত)
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
    প্রত্যাগম OK;
}

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, পূর্ণাঙ্ক check)
{
    char_u	*nm = name;
    char_u	*p;
    পূর্ণাঙ্ক		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    যদি (p != রিক্ত && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	প্রত্যাগম p;

    vim_free(p);
    প্রত্যাগম রিক্ত;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, পূর্ণাঙ্ক idx)
{
    স্থির long_u	done;
    স্থির পূর্ণাঙ্ক		changed;
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
	পূর্ণাঙ্ক len;

	যদি (done++ > 0)
	    ++hi;
	যাবত্ (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	যদি ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    প্রত্যাগম (char_u *)"";

	যদি (fp->uf_namelen + 4 >= IOSIZE)
	    প্রত্যাগম fp->uf_name;	// prevents overflow

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
	প্রত্যাগম IObuff;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, পূর্ণাঙ্ক extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    যদি (ufunc == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = রিক্ত;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_গুনতি = রিক্ত;
    ufunc->uf_tml_total = রিক্ত;
    ufunc->uf_tml_self = রিক্ত;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    যদি (ufunc->uf_arg_types != রিক্ত)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	যদি (at != রিক্ত)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     পরিমাণ(type_T *) * ufunc->uf_args.ga_len);
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

    যদি (ufunc->uf_va_name != রিক্ত)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = রিক্ত;

    ga_copy_strings(&fp->uf_সারিগুলি, &ufunc->uf_সারিগুলি);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    যদি (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    প্রত্যাগম ufunc;
}

/*
 * ":delfunction {name}"
 */
    শূন্য
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = রিক্ত;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    পূর্ণাঙ্ক		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     রিক্ত, রিক্ত, রিক্ত);
    vim_free(fudi.fd_newkey);
    যদি (name == রিক্ত)
    {
	যদি (fudi.fd_dict != রিক্ত && !eap->skip)
	    emsg(_(e_funcref_required));
	প্রত্যাগম;
    }
    যদি (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	প্রত্যাগম;
    }
    set_nextcmd(eap, p);
    যদি (eap->nextcmd != রিক্ত)
	*p = NUL;

    যদি (numbered_function(name) && fudi.fd_dict == রিক্ত)
    {
	যদি (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	প্রত্যাগম;
    }
    যদি (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    যদি (!eap->skip)
    {
	যদি (fp == রিক্ত)
	{
	    যদি (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    প্রত্যাগম;
	}
	যদি (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    প্রত্যাগম;
	}
	যদি (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    প্রত্যাগম;
	}

	যদি (fudi.fd_dict != রিক্ত)
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
    ufunc_T *fp = রিক্ত;

    যদি (name == রিক্ত || !func_name_refcount(name))
	প্রত্যাগম;
    fp = find_func(name, FALSE);
    যদি (fp == রিক্ত && numbered_function(name))
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
    যদি (fp != রিক্ত && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != রিক্ত
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

    যদি (name == রিক্ত || !func_name_refcount(name))
	প্রত্যাগম;
    fp = find_func(name, FALSE);
    যদি (fp != রিক্ত)
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
    যদি (fp != রিক্ত)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    স্থির পূর্ণাঙ্ক
can_free_funccal(funccall_T *fc, পূর্ণাঙ্ক copyID)
{
    প্রত্যাগম (fc->fc_l_varlist.lv_copyID != copyID
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
    পূর্ণাঙ্ক		returning = FALSE;
    evalarg_T	evalarg;

    যদি (current_funccal == রিক্ত)
    {
	emsg(_(e_return_not_inside_function));
	প্রত্যাগম;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    যদি (eap->skip)
	++emsg_skip;

    eap->nextcmd = রিক্ত;
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
	    returning = do_return(eap, FALSE, TRUE, রিক্ত);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    যদি (returning)
	eap->nextcmd = রিক্ত;
    অন্যথা যদি (eap->nextcmd == রিক্ত)	    // no argument
	set_nextcmd(eap, arg);

    যদি (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    স্থির পূর্ণাঙ্ক
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		doesrange;
    typval_T	rettv;
    পূর্ণাঙ্ক		failed = FALSE;

    lnum = eap->line1;
    প্রতি ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	যদি (eap->addr_গুনতি > 0)
	{
	    যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ভঙ্গ;
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
	    ভঙ্গ;
	}
	যদি (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	যদি (handle_subscript(arg, রিক্ত, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ভঙ্গ;
	}

	clear_tv(&rettv);
	যদি (doesrange)
	    ভঙ্গ;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	যদি (aborting())
	    ভঙ্গ;
    }
    প্রত্যাগম failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    স্থির পূর্ণাঙ্ক
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    পূর্ণাঙ্ক		partial_argc = 0;		// number of partial arguments
    পূর্ণাঙ্ক		argcount = 0;			// number of arguments found
    পূর্ণাঙ্ক		r;

    যদি (current_funccal == রিক্ত)
    {
	semsg(_(e_str_not_inside_function), "defer");
	প্রত্যাগম FAIL;
    }
    যদি (partial != রিক্ত)
    {
	যদি (partial->pt_dict != রিক্ত)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    প্রত্যাগম FAIL;
	}
	যদি (partial->pt_argc > 0)
	{
	    পূর্ণাঙ্ক i;

	    partial_argc = partial->pt_argc;
	    প্রতি (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    পূর্ণাঙ্ক is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    যদি (r == OK)
    {
	যদি (type != রিক্ত)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, রিক্ত, name);
	}
	অন্যথা যদি (is_builtin)
	{
	    পূর্ণাঙ্ক idx = find_internal_func(name);

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
	    যদি (ufunc != রিক্ত)
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
	যাবত্ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    পূর্ণাঙ্ক
can_add_defer(শূন্য)
{
    যদি (!in_def_function() && get_current_funccal() == রিক্ত)
    {
	semsg(_(e_str_not_inside_function), "defer");
	প্রত্যাগম FALSE;
    }
    প্রত্যাগম TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
add_defer(char_u *name, পূর্ণাঙ্ক argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    পূর্ণাঙ্ক		argcount = argcount_arg;
    defer_T	*dr;
    পূর্ণাঙ্ক		ret = FAIL;

    যদি (saved_name == রিক্ত)
	গচ্ছ theend;
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
	    ga_init2(&current_funccal->fc_defer, পরিমাণ(defer_T), 10);
	যদি (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    গচ্ছ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	যাবত্ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    যাবত্ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    প্রত্যাগম ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    স্থির শূন্য
handle_defer_one(funccall_T *funccal)
{
    পূর্ণাঙ্ক	    idx;

    প্রতি (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	যদি (dr->dr_name == রিক্ত)
	    // already being called, can happen if function does ":qa"
	    অনুবর্ত;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = রিক্ত;

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
	প্রতি (পূর্ণাঙ্ক i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    স্থির শূন্য
invoke_funccall_defer(funccall_T *fc)
{
    যদি (fc->fc_ectx != রিক্ত)
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
    প্রতি (funccall_T *fc = current_funccal; fc != রিক্ত; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    প্রতি (funccal_entry_T *fce = funccal_স্তূপ; fce != রিক্ত; fce = fce->next)
	প্রতি (funccall_T *fc = fce->শীর্ষ_funccal; fc != রিক্ত; fc = fc->fc_caller)
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
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = রিক্ত;
    partial_T	*partial = রিক্ত;
    evalarg_T	evalarg;
    type_T	*type = রিক্ত;
    পূর্ণাঙ্ক		found_var = FALSE;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

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
	প্রত্যাগম;
    }

    tofree = trans_function_name_ext(&arg, রিক্ত, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : রিক্ত, &ufunc);
    যদি (fudi.fd_newkey != রিক্ত)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_চাবি_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    যদি (tofree == রিক্ত)
	প্রত্যাগম;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    যদি (fudi.fd_dict != রিক্ত)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (পূর্ণাঙ্ক)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != রিক্ত ? রিক্ত : &partial,
				vim9script && type == রিক্ত ? &type : রিক্ত,
						     FALSE, FALSE, &found_var);

    যদি (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	যদি (name == রিক্ত)
	    গচ্ছ end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    যদি (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	গচ্ছ end;
    }
    যদি (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	গচ্ছ end;
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
    পূর্ণাঙ্ক
do_return(
    exarg_T	*eap,
    পূর্ণাঙ্ক		reanimate,
    পূর্ণাঙ্ক		is_cmd,
    শূন্য	*rettv)
{
    পূর্ণাঙ্ক		idx;
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

	    যদি (rettv != রিক্ত)
	    {
		// Store the value of the pending return.
		যদি ((cstack->cs_rettv[idx] = alloc_tv()) != রিক্ত)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		অন্যথা
		    emsg(_(e_out_of_memory));
	    }
	    অন্যথা
		cstack->cs_rettv[idx] = রিক্ত;

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
	যদি (!reanimate && rettv != রিক্ত)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    যদি (!is_cmd)
		vim_free(rettv);
	}
    }

    প্রত্যাগম idx < 0;
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
    char_u	*s = রিক্ত;
    char_u	*tofree = রিক্ত;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    যদি (rettv != রিক্ত)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    যদি (s == রিক্ত)
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
    প্রত্যাগম vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_সারি(
    পূর্ণাঙ্ক	    c UNUSED,
    শূন্য    *cookie,
    পূর্ণাঙ্ক	    indent UNUSED,
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
	retval = রিক্ত;
    অন্যথা
    {
	// Skip NULL lines (continuation lines).
	যাবত্ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == রিক্ত)
	    ++fcp->fc_linenr;
	যদি (fcp->fc_linenr >= gap->ga_len)
	    retval = রিক্ত;
	অন্যথা
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
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

    প্রত্যাগম retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    পূর্ণাঙ্ক
func_has_ended(শূন্য *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    প্রত্যাগম (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    পূর্ণাঙ্ক
func_has_abort(
    শূন্য    *cookie)
{
    প্রত্যাগম ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = রিক্ত;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    যদি (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != রিক্ত
				     && rettv->vval.v_partial->pt_func != রিক্ত)
	fp = rettv->vval.v_partial->pt_func;
    অন্যথা
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == রিক্ত ? রিক্ত
					      : rettv->vval.v_partial->pt_name;
	যদি (fname == রিক্ত)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = রিক্ত;
	}
	অন্যথা
	{
	    char_u	*tofree = রিক্ত;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    যদি (fp != রিক্ত && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	যদি (pt != রিক্ত)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = রিক্ত;
	    যদি (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    অন্যথা
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		পূর্ণাঙ্ক		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		যদি (ret_pt->pt_name != রিক্ত)
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
		    যদি (pt->pt_argv == রিক্ত)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    অন্যথা
		    {
			pt->pt_argc = ret_pt->pt_argc;
			প্রতি (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    প্রত্যাগম selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(শূন্য *cookie)
{
    প্রত্যাগম ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(শূন্য *cookie)
{
    প্রত্যাগম &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    পূর্ণাঙ্ক *
func_dbg_tick(শূন্য *cookie)
{
    প্রত্যাগম &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    পূর্ণাঙ্ক
func_level(শূন্য *cookie)
{
    প্রত্যাগম ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    পূর্ণাঙ্ক
current_func_returned(শূন্য)
{
    প্রত্যাগম current_funccal->fc_returned;
}

    পূর্ণাঙ্ক
free_unref_funccal(পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক testing)
{
    পূর্ণাঙ্ক		did_free = FALSE;
    পূর্ণাঙ্ক		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    প্রতি (pfc = &previous_funccal; *pfc != রিক্ত; )
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

    প্রত্যাগম did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    স্থির funccall_T *
get_funccal(শূন্য)
{
    পূর্ণাঙ্ক		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    যদি (debug_backtrace_level > 0)
    {
	প্রতি (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    যদি (temp_funccal)
		funccal = temp_funccal;
	    অন্যথা
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    প্রত্যাগম funccal;
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
    funccall_T	*funccal = রিক্ত;

    যদি (current_funccal == রিক্ত)
	প্রত্যাগম রিক্ত;
    funccal = get_funccal();
    যদি (funccal == রিক্ত || funccal->fc_l_vars.dv_refcount == 0)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    প্রত্যাগম funccal == রিক্ত ? রিক্ত : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    প্রত্যাগম funccal == রিক্ত ? রিক্ত : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    প্রত্যাগম funccal == রিক্ত ? রিক্ত : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(শূন্য)
{
    funccall_T	*funccal = get_funccal_for_vars();

    প্রত্যাগম funccal == রিক্ত ? রিক্ত : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    শূন্য
list_func_vars(পূর্ণাঙ্ক *first)
{
    যদি (current_funccal != রিক্ত && current_funccal->fc_l_vars.dv_refcount > 0)
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
    যদি (current_funccal != রিক্ত
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	প্রত্যাগম &current_funccal->fc_l_vars;
    প্রত্যাগম রিক্ত;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = রিক্ত;
    char_u	*varname;

    যদি (current_funccal == রিক্ত || current_funccal->fc_func->uf_scoped == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    যাবত্ (current_funccal != রিক্ত)
    {
	ht = find_var_ht(name, &varname);
	যদি (ht != রিক্ত && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    যদি (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		ভঙ্গ;
	    }
	}
	যদি (current_funccal == current_funccal->fc_func->uf_scoped)
	    ভঙ্গ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    প্রত্যাগম hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, পূর্ণাঙ্ক no_autoload)
{
    dictitem_T	*v = রিক্ত;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    যদি (current_funccal == রিক্ত || current_funccal->fc_func->uf_scoped == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    যাবত্ (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	যদি (ht != রিক্ত && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    যদি (v != রিক্ত)
		ভঙ্গ;
	}
	যদি (current_funccal == current_funccal->fc_func->uf_scoped)
	    ভঙ্গ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    প্রত্যাগম v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    পূর্ণাঙ্ক
set_ref_in_previous_funccal(পূর্ণাঙ্ক copyID)
{
    funccall_T	*fc;

    প্রতি (fc = previous_funccal; fc != রিক্ত; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	যদি (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, রিক্ত, রিক্ত)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, রিক্ত, রিক্ত)
		|| set_ref_in_list_বস্তুসমূহ(&fc->fc_l_varlist, copyID + 1, রিক্ত, রিক্ত))
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

    স্থির পূর্ণাঙ্ক
set_ref_in_funccal(funccall_T *fc, পূর্ণাঙ্ক copyID)
{
    যদি (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	যদি (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, রিক্ত, রিক্ত)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, রিক্ত, রিক্ত)
		|| set_ref_in_list_বস্তুসমূহ(&fc->fc_l_varlist, copyID, রিক্ত, রিক্ত)
		|| set_ref_in_func(রিক্ত, fc->fc_func, copyID))
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    পূর্ণাঙ্ক
set_ref_in_call_স্তূপ(পূর্ণাঙ্ক copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    প্রতি (fc = current_funccal; fc != রিক্ত; fc = fc->fc_caller)
	যদি (set_ref_in_funccal(fc, copyID))
	    প্রত্যাগম TRUE;

    // Also go through the funccal_stack.
    প্রতি (entry = funccal_স্তূপ; entry != রিক্ত; entry = entry->next)
	প্রতি (fc = entry->শীর্ষ_funccal; fc != রিক্ত; fc = fc->fc_caller)
	    যদি (set_ref_in_funccal(fc, copyID))
		প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    পূর্ণাঙ্ক
set_ref_in_functions(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi = রিক্ত;
    ufunc_T	*fp;

    todo = (পূর্ণাঙ্ক)func_hashtab.ht_used;
    প্রতি (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    যদি (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(রিক্ত, fp, copyID))
		প্রত্যাগম TRUE;
	}
    }
    প্রত্যাগম FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    পূর্ণাঙ্ক
set_ref_in_func_args(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < funcargs.ga_len; ++i)
	যদি (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, রিক্ত, রিক্ত, রিক্ত))
	    প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    পূর্ণাঙ্ক
set_ref_in_func(char_u *name, ufunc_T *fp_in, পূর্ণাঙ্ক copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = রিক্ত;
    char_u	*fname;
    পূর্ণাঙ্ক		abort = FALSE;

    যদি (name == রিক্ত && fp_in == রিক্ত)
	প্রত্যাগম FALSE;

    যদি (fp_in == রিক্ত)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    যদি (fp != রিক্ত)
    {
	প্রতি (fc = fp->uf_scoped; fc != রিক্ত; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    প্রত্যাগম abort;
}

#endif // FEAT_EVAL
