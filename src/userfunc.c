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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
स्थिर hashtab_T	func_hashtab;

// Used by get_func_tv()
स्थिर garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
स्थिर funccall_T *current_funccal = रिक्त;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
स्थिर funccall_T *previous_funccal = रिक्त;

स्थिर शून्य funccal_unref(funccall_T *fc, ufunc_T *fp, पूर्णांक force);
स्थिर शून्य func_clear(ufunc_T *fp, पूर्णांक force);
स्थिर पूर्णांक func_free(ufunc_T *fp, पूर्णांक force);
स्थिर char_u *untrans_function_name(char_u *name);
स्थिर शून्य handle_defer_one(funccall_T *funccal);
स्थिर char_u *trans_function_name_ext(char_u **pp, पूर्णांक *is_global, पूर्णांक skip, पूर्णांक flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    शून्य
func_init(शून्य)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(शून्य)
{
    परत &func_hashtab;
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
    स्थिर char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	पूर्णांक	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	पूर्णांक	    is_vararg,
	पूर्णांक	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = रिक्त;
    पूर्णांक		is_underscore = FALSE;

    पर्यंत (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    जर (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == रिक्त
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	जर (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	परत arg;
    }

    // Extra checks in Vim9 script.
    जर (!skip && argtypes != रिक्त)
    {
	पूर्णांक c = *p;
	*p = NUL;
	पूर्णांक r = check_reserved_name(arg, FALSE);
	*p = c;
	जर (r == FAIL)
	    परत arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	जर (check_defined(arg, p - arg,
				   evalarg == रिक्त ? रिक्त : evalarg->eval_cctx,
			       eap == रिक्त ? रिक्त : eap->cstack, TRUE) == FAIL)
	    परत arg;
    }

    जर (newargs != रिक्त && ga_grow(newargs, 1) == FAIL)
	परत arg;
    जर (newargs != रिक्त)
    {
	पूर्णांक	c;
	पूर्णांक	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	जर (arg_copy == रिक्त)
	{
	    *p = c;
	    परत arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	जर (argtypes == रिक्त || !is_underscore)
	    // Check for duplicate argument name.
	    साठी (i = 0; i < newargs->ga_len; ++i)
		जर (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    परत arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    जर (argtypes != रिक्त && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != रिक्त && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = रिक्त;

	जर (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == रिक्त ? arg : arg_copy);
	    p = skipwhite(p);
	}
	जर (*p == ':')
	{
	    ++p;
	    जर (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		परत arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    जर (!skip)
		type = vim_strnsave(type, p - type);
	}
	अन्यथा जर (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == रिक्त ? arg : arg_copy);
	    परत arg;
	}
	जर (!skip)
	{
	    जर (type == रिक्त && types_optional)
	    {
		// lambda arguments default to "any" type
		जर (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		अन्यथा
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    परत p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    स्थिर char_u *
get_function_ओळ(
	exarg_T		*eap,
	garray_T	*ओळी_to_free,
	पूर्णांक		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    जर (eap->ea_getline == रिक्त)
	theline = getcmdline(':', 0L, indent, 0);
    अन्यथा
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    जर (theline != रिक्त)
    {
	जर (ओळी_to_free->ga_len > 0
		&& eap->cmdlinep != रिक्त
		&& *eap->cmdlinep == ((char_u **)ओळी_to_free->ga_data)
						   [ओळी_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(शून्य)ga_add_string(ओळी_to_free, theline);
    }

    परत theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    स्थिर पूर्णांक
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    पूर्णांक		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    पूर्णांक		*varargs,
    garray_T	*default_args,
    पूर्णांक		skip,
    exarg_T	*eap,		// can be NULL
    पूर्णांक		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*ओळी_to_free)
{
    पूर्णांक		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		any_default = FALSE;
    char_u	*whitep = *argp;
    पूर्णांक		need_अभिव्यक्ती = FALSE;

    जर (newargs != रिक्त)
	ga_init2(newargs, आकार(char_u *), 3);
    जर (argtypes != रिक्त)
	ga_init2(argtypes, आकार(char_u *), 3);
    जर (arg_objm != रिक्त)
	ga_init2(arg_objm, आकार(int8_T), 3);
    जर (!skip && default_args != रिक्त)
	ga_init2(default_args, आकार(char_u *), 3);

    जर (varargs != रिक्त)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    पर्यंत (*p != endchar)
    {
	पर्यंत (eap != रिक्त && eap->ea_getline != रिक्त
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_ओळ(eap, ओळी_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    जर (theline == रिक्त)
		थांबा;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	जर (mustend && *p != endchar)
	{
	    जर (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    जा err_ret;
	}
	जर (*p == endchar && !need_अभिव्यक्ती)
	    थांबा;

	जर (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    जर (varargs != रिक्त)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    जर (argtypes != रिक्त)
	    {
		// ...name: list<type>
		जर (!eval_isnamec1(*p))
		{
		    जर (!skip)
			emsg(_(e_missing_name_after_dots));
		    जा err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		जर (p == arg)
		    थांबा;
		जर (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    थांबा;
		}
	    }
	}
	अन्यथा जर (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    पर्यंत (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    जर (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		थांबा;
	    }

	    जर (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		जर (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_मूल्य_must_be_vnone_str), p);
		    जा err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		जर (ga_grow(default_args, 1) == FAIL)
		    जा err_ret;

		char_u *अभिव्यक्ती = vim_strnsave((char_u *)"v:none", 6);
		जर (अभिव्यक्ती == रिक्त)
		    जा err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = अभिव्यक्ती;
		default_args->ga_len++;
	    }
	    अन्यथा जर (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		जा err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    जर (newargs != रिक्त && ga_grow(newargs, 1) == FAIL)
		परत FAIL;
	    जर (newargs != रिक्त)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		जर (argtypes != रिक्त && ga_grow(argtypes, 1) == OK
			    && arg_objm != रिक्त && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    जर (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			पूर्णांक len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			जर (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			जर (assignment != रिक्त)
			{
			    c = *argend;
			    *argend = NUL;
			    जर (any_default)
				vim_snprintf((अक्षर *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    अन्यथा
				vim_snprintf((अक्षर *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    जर (*p == ',')
		++p;
	}
	अन्यथा
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    जर (p == arg)
		थांबा;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    जर (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != रिक्त)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *अभिव्यक्ती = p;
		जर (eval1(&p, &rettv, रिक्त) != FAIL)
		{
		    जर (!skip)
		    {
			जर (ga_grow(default_args, 1) == FAIL)
			    जा err_ret;

			जर (need_अभिव्यक्ती)
			    need_अभिव्यक्ती = FALSE;
			// trim trailing whitespace
			पर्यंत (p > अभिव्यक्ती && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			अभिव्यक्ती = vim_strsave(अभिव्यक्ती);
			जर (अभिव्यक्ती == रिक्त)
			{
			    *p = c;
			    जा err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = अभिव्यक्ती;
			default_args->ga_len++;
			*p = c;
		    }
		}
		अन्यथा
		{
		    mustend = TRUE;
		    जर (*skipwhite(p) == NUL)
			need_अभिव्यक्ती = TRUE;
		}
	    }
	    अन्यथा जर (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		जा err_ret;
	    }

	    जर (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		जर (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    जा err_ret;
		}
		p = skipwhite(p);
	    }
	    जर (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		जर (!skip && argtypes != रिक्त
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    जा err_ret;
		}
	    }
	    अन्यथा
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    जर (*p != endchar)
	जा err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    परत OK;

err_ret:
    जर (newargs != रिक्त)
	ga_clear_strings(newargs);
    जर (!skip && default_args != रिक्त)
	ga_clear_strings(default_args);
    परत FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    पूर्णांक		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    पूर्णांक		obj_member_मोजणी,
    cctx_T	*cctx)
{
    पूर्णांक len = 0;

    ga_init2(&fp->uf_type_list, आकार(type_T *), 10);
    जर (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	जर (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	जर (fp->uf_arg_types != रिक्त)
	{
	    पूर्णांक	i;
	    type_T	*type;

	    साठी (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		जर (p == रिक्त)
		    // will get the type from the default value
		    type = &t_unknown;
		अन्यथा
		{
		    जर (arg_objm != रिक्त && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			साठी (पूर्णांक om = 0; om < obj_member_मोजणी; ++om)
			{
			    जर (obj_members != रिक्त
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				थांबा;
			    }
			}
		    }
		    अन्यथा
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		जर (type == रिक्त || !valid_declaration_type(type))
		    परत FAIL;
		fp->uf_arg_types[i] = type;
		जर (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    जर (obj_members != रिक्त && *name == '_')
			// protected object method
			name++;

		    जर (var_wrong_func_name(name, TRUE))
			परत FAIL;
		}
	    }
	}
    }

    जर (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = रिक्त;
	p = ((char_u **)argtypes->ga_data)[len];
	जर (p == रिक्त)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	अन्यथा
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    जर (fp->uf_va_type != रिक्त && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		परत FAIL;
	    }
	}
	जर (fp->uf_va_type == रिक्त)
	    परत FAIL;
    }

    परत OK;
}

    स्थिर पूर्णांक
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    जर (ret_type == रिक्त)
	fp->uf_ret_type = &t_void;
    अन्यथा
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	जर (fp->uf_ret_type == रिक्त)
	{
	    fp->uf_ret_type = &t_void;
	    परत FAIL;
	}
    }
    परत OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    स्थिर पूर्णांक
register_closure(ufunc_T *fp)
{
    जर (fp->uf_scoped == current_funccal)
	// no change
	परत OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    जर (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	परत FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    परत OK;
}

/*
 * If "name" starts with K_SPECIAL and "buf[bufsize]" is big enough
 * return "buf" filled with a readable function name.
 * Otherwise just return "name", thus the return value can always be used.
 * "name" and "buf" may be equal.
 */
    char_u *
make_ufunc_name_readable(char_u *name, char_u *buf, माप_प्रकार bufsize)
{
    माप_प्रकार len;

    जर (name[0] != K_SPECIAL)
	परत name;
    len = STRLEN(name);
    जर (len + 3 > bufsize)
	परत name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    परत buf;
}

स्थिर char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(शून्य)
{
    स्थिर पूर्णांक	lambda_no = 0;
    पूर्णांक		n;
    string_T	ret;

    n = vim_snprintf((अक्षर *)lambda_name, आकार(lambda_name), "<lambda>%d", ++lambda_no);
    जर (n < 1)
	ret.length = 0;
    अन्यथा जर (n >= (पूर्णांक)आकार(lambda_name))
	ret.length = आकार(lambda_name) - 1;
    अन्यथा
	ret.length = (माप_प्रकार)n;

    ret.string = lambda_name;
    परत ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    स्थिर ufunc_T *
alloc_ufunc(char_u *name, माप_प्रकार namelen)
{
    माप_प्रकार  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = दुरत्व(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < आकार(ufunc_T) ? आकार(ufunc_T) : len);
    जर (fp != रिक्त)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((शून्य *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	जर (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    जर (fp->uf_name_exp != रिक्त)
		vim_snprintf((अक्षर *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    परत fp;
}

#जर defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, शून्य *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    जर (fp == रिक्त)
	परत रिक्त;

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

    परत name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    स्थिर char_u *
skip_arrow(
	char_u	*start,
	पूर्णांक	equal_arrow,
	char_u	**ret_type,
	पूर्णांक	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    जर (equal_arrow)
    {
	जर (*s == ':')
	{
	    जर (white_error != रिक्त && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		परत रिक्त;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    जर (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		परत रिक्त;
	    }
	}
	bef = s;
	s = skipwhite(s);
	जर (*s != '=')
	    परत रिक्त;
	++s;
    }
    जर (*s != '>')
	परत रिक्त;
    जर (white_error != रिक्त && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	परत रिक्त;
    }
    परत skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    स्थिर पूर्णांक
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    जर (checkforcmd(&p, "function", 2))
    {
	जर (*p == '(')
	    परत FALSE;
	*cmd = p;
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    स्थिर शून्य
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    जर (cstack == रिक्त || cstack->cs_idx < 0)
	परत;

    पूर्णांक	    मोजणी = cstack->cs_idx + 1;
    पूर्णांक	    i;

    fp->uf_block_ids = ALLOC_MULT(पूर्णांक, मोजणी);
    जर (fp->uf_block_ids != रिक्त)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		आकार(पूर्णांक) * मोजणी);
	fp->uf_block_depth = मोजणी;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    साठी (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    स्थिर char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    पर्यंत (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	जर (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	अन्यथा जर (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    थांबा;
	}
	अन्यथा
	    MB_PTR_ADV(p);
    }

    परत p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    स्थिर char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    पर्यंत (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	जर (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	अन्यथा जर (*p == '"')
	{
	    p++;    // skip the ending double quote
	    थांबा;
	}
	अन्यथा
	    MB_PTR_ADV(p);
    }

    परत p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    स्थिर char_u *
find_start_of_vim9_comment(char_u *ओळ)
{
    char_u	*p = ओळ;

    पर्यंत (*p != NUL)
    {
	जर (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	अन्यथा जर (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	अन्यथा
	{
	    जर (*p == '#')
		// Found the start of a Vim9 comment
		थांबा;
	    MB_PTR_ADV(p);
	}
    }

    परत p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    स्थिर पूर्णांक
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *ओळ_arg_in,
	garray_T    *ओळी_to_free)
{
    linenr_T	sourcing_lnum_शीर्ष = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    पूर्णांक		saved_wait_return = need_wait_return;
    char_u	*ओळ_arg = ओळ_arg_in;
    पूर्णांक		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    अक्षर	nesting_def[MAX_FUNC_NESTING];
    अक्षर	nesting_inline[MAX_FUNC_NESTING];
    पूर्णांक		nesting = 0;
    getline_opt_T getline_options;
    पूर्णांक		indent = 2;
    char_u	*skip_until = रिक्त;
    पूर्णांक		ret = FAIL;
    पूर्णांक		is_heredoc = FALSE;
    पूर्णांक		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = रिक्त;
    माप_प्रकार	heredoc_trimmedlen = 0;
    पूर्णांक		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    जर (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	जर (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    जा theend;
	पर्यंत (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = रिक्त;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    साठी (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	जर (किल्ली_Typed)
	{
	    संदेश_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	जर (ओळ_arg != रिक्त)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = ओळ_arg;
	    p = vim_strchr(theline, '\n');
	    जर (p == रिक्त)
		ओळ_arg += STRLEN(ओळ_arg);
	    अन्यथा
	    {
		*p = NUL;
		ओळ_arg = p + 1;
	    }
	}
	अन्यथा
	{
	    theline = get_function_ओळ(eap, ओळी_to_free, indent,
							      getline_options);
	}
	जर (किल्ली_Typed)
	    ओळी_डावा = Rows - 1;
	जर (theline == रिक्त)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_शीर्ष;
	    जर (skip_until != रिक्त)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    अन्यथा जर (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    अन्यथा जर (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    अन्यथा
		emsg(_(e_missing_endfunction));
	    जा theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	जर (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	अन्यथा
	    sourcing_lnum_off = 0;

	जर (skip_until != रिक्त)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    जर (heredoc_trimmed == रिक्त
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		जर (heredoc_trimmed == रिक्त)
		    p = theline;
		अन्यथा जर (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		अन्यथा
		    p = theline + heredoc_trimmedlen;
		जर (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    जर (heredoc_concat_len > 0)
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
	अन्यथा
	{
	    पूर्णांक	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    साठी (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		जर (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    जर (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		जर (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		जर (nesting-- == 0)
		{
		    char_u *nextcmd = रिक्त;

		    जर (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    अन्यथा जर (ओळ_arg != रिक्त && *skipwhite(ओळ_arg) != NUL)
			nextcmd = ओळ_arg;
		    अन्यथा जर (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_शीर्ष
							+ newlines->ga_len + 1;
			जर (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			अन्यथा
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    जर (nextcmd != रिक्त && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			जर (ओळी_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)ओळी_to_free->ga_data)
						   [ओळी_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)ओळी_to_free->ga_data)
						   [ओळी_to_free->ga_len - 1];
			    --ओळी_to_free->ga_len;
			}
		    }
		    थांबा;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    अन्यथा जर (nesting_def[nesting])
	    {
		जर (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    अन्यथा जर (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    जर (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    अन्यथा जर (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    जर (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		जर (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, रिक्त, TRUE, 0));
		जर (vim9_function && *p == '<')
		{
		    // skip generic function
		    जर (skip_generic_func_type_args(&p) == FAIL)
			जा theend;
		}
		जर (*skipwhite(p) == '(')
		{
		    जर (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    अन्यथा
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    जर (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		जर (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		अन्यथा
		    end = p + STRLEN(p) - 1;

		पर्यंत (end > p && VIM_ISWHITE(*end))
		    --end;
		जर (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    पूर्णांक	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    पर्यंत (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    जर (!is_block)
			is_block = find_cmd_block_start(p) != रिक्त;

		    जर (is_block)
		    {
			जर (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			अन्यथा
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
	    जर (!vim9_function || colon)
		p = skip_range(p, FALSE, रिक्त);
	    char_u *tp = p;
	    जर (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    अन्यथा
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    जर (arg[0] == '<' && arg[1] =='<'
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
		जर (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    जर (heredoc_trimmed == रिक्त)
			heredoc_trimmedlen = 0;
		}
		जर (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		अन्यथा
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		जर (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    जर (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		जर (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    पूर्णांक		save_sc_version = current_sctx.sc_version;
		    पूर्णांक		var_मोजणी = 0;
		    पूर्णांक		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_मोजणी, &semicolon,
									 TRUE);
		    जर (arg != रिक्त)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    जर (arg != रिक्त && STRNCMP(arg, "=<<", 3) == 0)
		    {
			पूर्णांक has_trim = FALSE;

			p = skipwhite(arg + 3);
			पर्यंत (TRUE)
			{
			    जर (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				सुरूठेवा;
			    }
			    जर (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				सुरूठेवा;
			    }
			    थांबा;
			}
			जर (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    जर (heredoc_trimmed == रिक्त)
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
	जर (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    जा theend;

	जर (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	अन्यथा
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	जर (p == रिक्त)
	    जा theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	पर्यंत (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = रिक्त;

	// Check for end of eap->arg.
	जर (ओळ_arg != रिक्त && *ओळ_arg == NUL)
	    ओळ_arg = रिक्त;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    जर (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    परत ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    स्थिर पूर्णांक
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	पूर्णांक	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    पूर्णांक		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = रिक्त;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = रिक्त;
    पूर्णांक		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    पूर्णांक		lnum_save = -1;
    linenr_T	sourcing_lnum_शीर्ष = SOURCING_LNUM;
    char_u	*ओळ_arg = रिक्त;

    *arg = skipwhite(*arg + 1);
    जर (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	परत FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    जर (**arg == '\n')
	ओळ_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    जर (evalarg->eval_cctx != रिक्त)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    अन्यथा
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, आकार(char_u *), 10);
    जर (get_function_body(&eap, &newlines, ओळ_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	जा erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_मोजणी += newlines.ga_len;
    जर (gap->ga_itemsize > 0)
    {
	पूर्णांक	idx;
	char_u	*last;
	माप_प्रकार  plen;
	char_u  *pnl;

	साठी (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    जर (p == रिक्त)
		// comment line in the lambda body
		सुरूठेवा;

	    p = skipwhite(p);

	    जर (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		जा erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    जर (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    अन्यथा
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    जर (pnl != रिक्त)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	जर (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    जा erret;
	जर (eap.nextcmd != रिक्त)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	अन्यथा
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	जर (pnl != रिक्त)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    जर (eap.nextcmd != रिक्त)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	जर (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    अन्यथा
	*arg = (char_u *)"";

    जर (!evaluate)
    {
	ret = OK;
	जा erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    जर (ufunc == रिक्त)
	जा erret;
    जर (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	जा erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = रिक्त;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = रिक्त;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_शीर्ष;

    // parse argument types, the type variables of the function being
    // compiled may be used
    जर (parse_argument_types(ufunc, argtypes, varargs, रिक्त, रिक्त, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	जा erret;
    }

    // parse the return type, if any
    जर (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	जा erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    जर (pt == रिक्त)
	जा erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_ओळी = newlines;
    newlines.ga_data = रिक्त;
    जर (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    जर (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_शीर्ष;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = रिक्त;
    ret = OK;

erret:
    जर (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    जर (newargs != रिक्त)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    जर (ufunc != रिक्त)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    परत ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    पूर्णांक
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	पूर्णांक	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    पूर्णांक		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = रिक्त;
    partial_T   *pt = रिक्त;
    पूर्णांक		varargs;
    char_u	*ret_type = रिक्त;
    पूर्णांक		ret;
    char_u	*s;
    char_u	*start, *end;
    पूर्णांक		*old_eval_lavars = eval_lavars_used;
    पूर्णांक		eval_lavars = FALSE;
    char_u	*tofree2 = रिक्त;
    पूर्णांक		equal_arrow = **arg == '(';
    पूर्णांक		white_error = FALSE;
    पूर्णांक		called_emsg_start = called_emsg;
    पूर्णांक		vim9script = in_vim9script();
    दीर्घ	start_lnum = SOURCING_LNUM;

    जर (equal_arrow && !vim9script)
	परत NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', रिक्त,
	    types_optional ? &argtypes : रिक्त, types_optional,
			types_optional ? &arg_objm : रिक्त, evalarg,
			रिक्त, &default_args, TRUE, रिक्त, FALSE, रिक्त, रिक्त);
    जर (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, रिक्त) == रिक्त)
    {
	जर (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	परत called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    जर (evaluate)
	pnewargs = &newargs;
    अन्यथा
	pnewargs = रिक्त;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : रिक्त, types_optional,
			types_optional ? &arg_objm : रिक्त, evalarg,
					    &varargs, &default_args,
					    FALSE, रिक्त, FALSE, रिक्त, रिक्त);
    जर (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : रिक्त)) == रिक्त)
    {
	जर (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	परत white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    जर (ret_type != रिक्त)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    जर (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    जर (equal_arrow && **arg == '{')
    {
	जर (evalarg == रिक्त)
	    // cannot happen?
	    जा theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	जर (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : रिक्त, varargs,
			   &default_args, ret_type) == FAIL)
	    जा errret;
	जा theend;
    }
    जर (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_मूल्ये_in_lambda));
	जा errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_अभिव्यक्ती_concatenate(arg, &start, &end, evalarg);
    जर (ret == FAIL)
	जा errret;

    जर (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जर (**arg != '}')
	{
	    semsg(_(e_expected_उजवा_curly_str), *arg);
	    जा errret;
	}
	++*arg;
    }

    जर (evaluate)
    {
	पूर्णांक	    len;
	पूर्णांक	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *ओळ_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	जर (fp == रिक्त)
	    जा errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	जर (pt == रिक्त)
	    जा errret;

	ga_init2(&newlines, आकार(char_u *), 1);
	जर (ga_grow(&newlines, 1) == FAIL)
	    जा errret;

	// If there are line breaks, we need to split up the string.
	ओळ_end = vim_strchr(start, '\n');
	जर (ओळ_end == रिक्त || ओळ_end > end)
	    ओळ_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (पूर्णांक)(ओळ_end - start) + 1;
	p = alloc(len);
	जर (p == रिक्त)
	    जा errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, ओळ_end - start);

	जर (ओळ_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    पर्यंत (ओळ_end < end && *ओळ_end == '\n')
	    {
		जर (ga_grow(&newlines, 1) == FAIL)
		    जा errret;
		start = ओळ_end + 1;
		ओळ_end = vim_strchr(start, '\n');
		जर (ओळ_end == रिक्त || ओळ_end > end)
		    ओळ_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, ओळ_end - start);
	    }
	}

	जर (म_माला((अक्षर *)p + 7, "a:") == रिक्त)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	जर (types_optional)
	{
	    जर (parse_argument_types(fp, &argtypes,
				vim9script && varargs, रिक्त, रिक्त, 0,
				cctx) == FAIL)
		जा errret;
	    जर (ret_type != रिक्त)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     रिक्त, cctx, TRUE);
		जर (fp->uf_ret_type == रिक्त)
		    जा errret;
	    }
	    अन्यथा
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_ओळी = newlines;
	जर (current_funccal != रिक्त && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    जर (register_closure(fp) == FAIL)
		जा errret;
	}

#ifdef FEAT_PROFILE
	जर (prof_def_func())
	    func_do_profile(fp);
#endif
	जर (sandbox)
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
    जर (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    परत OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    जर (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    जर (fp != रिक्त)
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
    परत FAIL;
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
	पूर्णांक	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	पूर्णांक	    no_autoload,
	पूर्णांक	    new_function,
	पूर्णांक	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = रिक्त;
    पूर्णांक		cc;
    char_u	*s = रिक्त;
    hashtab_T	*ht;
    पूर्णांक		did_type = FALSE;

    जर (partialp != रिक्त)
	*partialp = रिक्त;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    जर (v != रिक्त)
    {
	tv = &v->di_tv;
    }
    अन्यथा जर (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	पूर्णांक	    len = *lenp;

	जर (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	जर (import != रिक्त)
	{
	    name[len] = NUL;
	    जर (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    अन्यथा
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    परत (char_u *)"";	// just in case
	}
    }

    जर (tv != रिक्त)
    {
	जर (found_var != रिक्त)
	    *found_var = TRUE;
	जर (tv->v_type == VAR_FUNC)
	{
	    जर (tv->vval.v_string == रिक्त)
	    {
		*lenp = 0;
		परत (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    जर (p != रिक्त && generic_func_find_open_bracket(name) != रिक्त)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		परत (char_u *)"";
	    }
	    जर (p != रिक्त)
		*lenp = p - s;
	    अन्यथा
		*lenp = (पूर्णांक)STRLEN(s);
	}
	अन्यथा जर (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    जर (pt == रिक्त)
	    {
		*lenp = 0;
		परत (char_u *)"";	// just in case
	    }
	    जर (partialp != रिक्त)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (पूर्णांक)STRLEN(s);
	}

	जर (s != रिक्त)
	{
	    जर (!did_type && type != रिक्त && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		जर (sv != रिक्त)
		    *type = sv->sv_type;
	    }
	    परत s;
	}
    }

    परत name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    शून्य
emsg_funcname(अक्षर *ermsg, char_u *name)
{
    char_u	*p = name;

    जर (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    जर (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    पूर्णांक
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	पूर्णांक	    partial_argc,
	typval_T    *argvars,
	पूर्णांक	    *argcount,
	पूर्णांक	    is_builtin)
{
    char_u	*argp = *arg;
    पूर्णांक		ret = OK;
    पूर्णांक		vim9script = in_vim9script();
    पूर्णांक		evaluate = evalarg == रिक्त
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    पर्यंत (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	जर (*argp == ')' || *argp == ',' || *argp == NUL)
	    थांबा;

	पूर्णांक arg_idx = *argcount;
	जर (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    थांबा;
	}
	++*argcount;
	जर (!is_builtin && check_typval_is_मूल्य(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    थांबा;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	जर (vim9script)
	{
	    जर (*argp != ',' && *skipwhite(argp) == ',')
	    {
		जर (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		थांबा;
	    }
	}
	अन्यथा
	    argp = skipwhite(argp);
	जर (*argp != ',')
	    थांबा;
	जर (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    जर (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    थांबा;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    जर (*argp == ')')
	++argp;
    अन्यथा
	ret = FAIL;
    *arg = argp;
    परत ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    पूर्णांक
get_func_tv(
    char_u	*name,		// name of the function
    पूर्णांक		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    पूर्णांक		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    पूर्णांक		argcount = 0;			// number of arguments found
    पूर्णांक		vim9script = in_vim9script();
    पूर्णांक		evaluate = evalarg == रिक्त
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == रिक्त ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    जर (ret == OK)
    {
	पूर्णांक	i = 0;
	पूर्णांक	did_emsg_before = did_emsg;

	जर (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    जर (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, आकार(typval_T *), 50);
	    साठी (i = 0; i < argcount; ++i)
		जर (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	जर (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    अन्यथा जर (!aborting() && evaluate)
    {
	जर (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	अन्यथा
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    पर्यंत (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    जर (vim9script)
	*arg = argp;
    अन्यथा
	*arg = skipwhite(argp);
    परत ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    स्थिर पूर्णांक
eval_fname_sid(char_u *p)
{
    परत (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    माप_प्रकार	fnamelen;
    माप_प्रकार	fname_buflen;

    script_name = name + eval_fname_script(name);
    जर (script_name == name)
	परत name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (पूर्णांक)KE_SNR;
    fname_buflen = 3;
    जर (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    अन्यथा
    {
	जर (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	अन्यथा
	{
	    fname_buflen += vim_snprintf((अक्षर *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(दीर्घ)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    जर (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    अन्यथा
    {
	fname = alloc(fnamelen + 1);
	जर (fname == रिक्त)
	    *error = FCERR_OTHER;
	अन्यथा
	{
	    *tofree = fname;
	    vim_snprintf((अक्षर *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    परत fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    शून्य
func_name_with_sid(char_u *name, पूर्णांक sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (पूर्णांक)KE_SNR;
    vim_snprintf((अक्षर *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (दीर्घ)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    स्थिर ufunc_T *
find_func_with_sid(char_u *name, पूर्णांक sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    जर (!SCRIPT_ID_VALID(sid))
	परत रिक्त;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    जर (!HASHITEM_EMPTY(hi))
	परत HI2UF(hi);
    परत रिक्त;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    स्थिर ufunc_T *
find_func_with_prefix(char_u *name, पूर्णांक sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    जर (vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	परत रिक्त;	// already has the prefix
    जर (!SCRIPT_ID_VALID(sid))
	परत रिक्त;	// not in a script
    si = SCRIPT_ITEM(sid);
    जर (si->sn_autoload_prefix != रिक्त)
    {
	माप_प्रकार	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	जर (namep == रिक्त)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	जर (len < आकार(buffer))
	    auto_name = buffer;
	अन्यथा
	    auto_name = alloc(len);
	जर (auto_name != रिक्त)
	{
	    vim_snprintf((अक्षर *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    जर (auto_name != buffer)
		vim_free(auto_name);
	    जर (!HASHITEM_EMPTY(hi))
		परत HI2UF(hi);
	}
    }

    परत रिक्त;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, पूर्णांक flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    जर ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	जर (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    जर (func != रिक्त)
		परत func;
	}
	जर (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    दीर्घ    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    जर (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (पूर्णांक)sid);
		जर (func != रिक्त)
		    परत func;
	    }
	}
    }

    जर ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	जर (!HASHITEM_EMPTY(hi))
	    परत HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    परत find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, पूर्णांक is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    जर (fp != रिक्त && (fp->uf_flags & FC_DEAD) == 0)
	परत fp;
    परत रिक्त;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    पूर्णांक
func_is_global(ufunc_T *ufunc)
{
    परत ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    पूर्णांक
func_requires_g_prefix(ufunc_T *ufunc)
{
    परत func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == रिक्त
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    स्थिर पूर्णांक
cat_func_name(char_u *buf, माप_प्रकार bufsize, ufunc_T *fp)
{
    पूर्णांक	len;

    जर (!func_is_global(fp))
	len = vim_snprintf((अक्षर *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    अन्यथा
	len = vim_snprintf((अक्षर *)buf, bufsize, "%s", fp->uf_name);

    परत (len >= (पूर्णांक)bufsize) ? (पूर्णांक)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    स्थिर शून्य
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    अक्षर	*name,
    varnumber_T nr)
{
    STRCPY(v->di_किल्ली, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_संख्या;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_संख्या = nr;
}

/*
 * Free "fc".
 */
    स्थिर शून्य
free_funccal(funccall_T *fc)
{
    पूर्णांक	i;

    साठी (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	जर (fp != रिक्त && fp->uf_scoped == fc)
	    fp->uf_scoped = रिक्त;
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
   स्थिर शून्य
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_वस्तुसंच(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    स्थिर शून्य
cleanup_function_call(funccall_T *fc)
{
    पूर्णांक	may_free_fc = fc->fc_refcount <= 0;
    पूर्णांक	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    जर (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    अन्यथा
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    जर (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    अन्यथा
    {
	पूर्णांक	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (पूर्णांक)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    जर (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    जर (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = रिक्त;
    अन्यथा
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_वस्तुसंच(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    जर (free_fc)
	free_funccal(fc);
    अन्यथा
    {
	स्थिर पूर्णांक made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	जर (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	अन्यथा जर (++made_copy >= (पूर्णांक)((4096 * 1024) / आकार(*fc)))
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
    स्थिर पूर्णांक
numbered_function(char_u *name)
{
    परत SAFE_isdigit(*name)
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
    पूर्णांक
func_name_refcount(char_u *name)
{
    परत numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    स्थिर शून्य
funccal_unref(funccall_T *fc, ufunc_T *fp, पूर्णांक force)
{
    funccall_T	**pfc;
    पूर्णांक		i;

    जर (fc == रिक्त)
	परत;

    जर (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	साठी (pfc = &previous_funccal; *pfc != रिक्त; pfc = &(*pfc)->fc_caller)
	{
	    जर (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		परत;
	    }
	}
    साठी (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	जर (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = रिक्त;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    स्थिर पूर्णांक
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    जर (fp->uf_flags & FC_DEAD)
	परत FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    जर (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	परत FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    जर (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	परत FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    परत TRUE;
}

    स्थिर शून्य
func_clear_वस्तुसंच(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_ओळी));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    जर (IS_GENERIC_FUNC(fp))
	generic_func_clear_वस्तुसंच(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = रिक्त;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    जर (fp->uf_cb_free != रिक्त)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = रिक्त;
    }

    fp->uf_cb_state = रिक्त;
    fp->uf_cb = रिक्त;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_मोजणी);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    स्थिर शून्य
func_clear(ufunc_T *fp, पूर्णांक force)
{
    जर (fp->uf_cleared)
	परत;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_वस्तुसंच(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    स्थिर पूर्णांक
func_free(ufunc_T *fp, पूर्णांक force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    जर ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    जर ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	जर (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	परत OK;
    }
    परत FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    शून्य
func_clear_free(ufunc_T *fp, पूर्णांक force)
{
    func_clear(fp, force);
    जर (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    अन्यथा
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    पूर्णांक
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = रिक्त;

    जर (ufunc == रिक्त)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	परत FAIL;
    }

    fp = find_func(global, TRUE);
    जर (fp != रिक्त)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	परत FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    जर (fp == रिक्त)
	परत FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    जर (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_ओळी, &fp->uf_ओळी) == FAIL)
	जा failed;

    जर (ufunc->uf_arg_types != रिक्त)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	जर (fp->uf_arg_types == रिक्त)
	    जा failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    आकार(type_T *) * fp->uf_args.ga_len);
    }
    जर (ufunc->uf_va_name != रिक्त)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	जर (fp->uf_va_name == रिक्त)
	    जा failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    जर ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == रिक्त)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	जर (pt == रिक्त)
	    जा failed;
	जर (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    जा failed;
	}
	ufunc->uf_partial = pt;
    }

    परत OK;

failed:
    func_clear_free(fp, TRUE);
    परत FAIL;
}

स्थिर पूर्णांक	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    पूर्णांक
funcdepth_increment(शून्य)
{
    जर (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	परत FAIL;
    }
    ++funcdepth;
    परत OK;
}

    शून्य
funcdepth_decrement(शून्य)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    पूर्णांक
funcdepth_get(शून्य)
{
    परत funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    शून्य
funcdepth_restore(पूर्णांक depth)
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

    जर (fc == रिक्त)
	परत रिक्त;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    परत fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    शून्य
remove_funccal(शून्य)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    स्थिर funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    पूर्णांक		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    पूर्णांक		using_sandbox = FALSE;
    पूर्णांक		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    पूर्णांक		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    पूर्णांक		default_arg_err = FALSE;
    dictitem_T	*v;
    पूर्णांक		fixvar_idx = 0;	// index in fc_fixvar[]
    पूर्णांक		i;
    पूर्णांक		ai;
    पूर्णांक		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    माप_प्रकार	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    पूर्णांक		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    जर (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = -1;
	परत FCERR_FAILED;
    }

    ओळ_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    जर (fc == रिक्त)
	परत FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, आकार(ufunc_T *), 1);

    जर (fp->uf_def_status != UF_NOT_COMPILED)
    {
	जर (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == रिक्त ? रिक्त : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	जर (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	जर (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	जर (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	जर (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != रिक्त && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	परत retval;
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
    जर (selfdict != रिक्त)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_किल्ली;
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
    जर ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    जर ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_किल्ली;
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
    जर ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    साठी (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	पूर्णांक	    addlocal = FALSE;
	typval_T    def_rettv;
	पूर्णांक	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	जर (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    जर (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_संख्या == VVAL_NONE));
	    जर (isdefault)
	    {
		char_u	    *default_अभिव्यक्ती = रिक्त;

		def_rettv.v_type = VAR_संख्या;
		def_rettv.vval.v_संख्या = -1;

		default_अभिव्यक्ती = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		जर (eval1(&default_अभिव्यक्ती, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    थांबा;
		}
	    }

	    namelen = STRLEN(name);
	}
	अन्यथा
	{
	    जर ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		थांबा;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((अक्षर *)numbuf, आकार(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	जर (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_किल्ली, name);
	}
	अन्यथा
	{
	    v = dictitem_alloc(name);
	    जर (v == रिक्त)
		थांबा;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	जर (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	जर (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	अन्यथा
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	जर (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    जर (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ढकला_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    जर (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	जर (p_verbose >= 14)
	{
	    char_u	buf[संदेश_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    संदेश_puts("(");
	    साठी (i = 0; i < argcount; ++i)
	    {
		जर (i > 0)
		    संदेश_puts(", ");
		जर (argvars[i].v_type == VAR_संख्या)
		    संदेश_outnum((दीर्घ)argvars[i].vval.v_संख्या);
		अन्यथा
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    जर (s != रिक्त)
		    {
			जर (vim_strsize(s) > संदेश_BUF_CLEN)
			{
			    trunc_string(s, buf, संदेश_BUF_CLEN, संदेश_BUF_LEN);
			    s = buf;
			}
			संदेश_puts((अक्षर *)s);
			vim_free(tofree);
		    }
		}
	    }
	    संदेश_puts(")");
	}
	संदेश_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == रिक्त ? रिक्त : fc->fc_caller->fc_func);
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

    जर (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    अन्यथा जर (islambda)
    {
	char_u *p = *(char_u **)fp->uf_ओळी.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(शून्य)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    अन्यथा
	// call do_cmdline() to execute the lines
	do_cmdline(रिक्त, get_func_ओळ, (शून्य *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    जर ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_संख्या;
	rettv->vval.v_संख्या = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	जर (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == रिक्त ? रिक्त : fc->fc_caller->fc_func;

	जर (fp->uf_profiling || (caller != रिक्त && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    जर (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	जर (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	अन्यथा जर (fc->fc_rettv->v_type == VAR_संख्या)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (दीर्घ)fc->fc_rettv->vval.v_संख्या);
	अन्यथा
	{
	    char_u	buf[संदेश_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    जर (s != रिक्त)
	    {
		जर (vim_strsize(s) > संदेश_BUF_CLEN)
		{
		    trunc_string(s, buf, संदेश_BUF_CLEN, संदेश_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	संदेश_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_काढा();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    जर (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    जर (p_verbose >= 12 && SOURCING_NAME != रिक्त)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	संदेश_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    साठी (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    परत retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, पूर्णांक argcount)
{
    पूर्णांक regular_args = fp->uf_args.ga_len;

    जर (argcount < regular_args - fp->uf_def_args.ga_len)
	परत FCERR_TOOFEW;
    अन्यथा जर (!has_varargs(fp) && argcount > regular_args)
	परत FCERR_TOOMANY;
    परत FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	पूर्णांक	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    जर (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	परत (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    जर (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != रिक्त)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    जर (error != FCERR_UNKNOWN)
	परत error;

    जर ((fp->uf_flags & FC_DICT) && selfdict == रिक्त)
    {
	error = FCERR_DICT;
    }
    अन्यथा
    {
	पूर्णांक		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_शोध_patterns();
	जर (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : रिक्त);
	जर (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	जर (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_शोध_patterns();
    }

    परत error;
}

स्थिर funccal_entry_T *funccal_चळत = रिक्त;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    शून्य
save_funccal(funccal_entry_T *entry)
{
    entry->शीर्ष_funccal = current_funccal;
    entry->next = funccal_चळत;
    funccal_चळत = entry;
    current_funccal = रिक्त;
}

    शून्य
restore_funccal(शून्य)
{
    जर (funccal_चळत == रिक्त)
	internal_error("restore_funccal()");
    अन्यथा
    {
	current_funccal = funccal_चळत->शीर्ष_funccal;
	funccal_चळत = funccal_चळत->next;
    }
}

    funccall_T *
get_current_funccal(शून्य)
{
    परत current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    पूर्णांक
at_script_level(शून्य)
{
    परत current_funccal == रिक्त && autocmd_match == रिक्त;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    शून्य
delete_script_functions(पूर्णांक sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    माप_प्रकार	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (पूर्णांक)KE_SNR;
    len = 3 + vim_snprintf((अक्षर *)buf + 3, आकार(buf) - 3, "%d_", sid);

    पर्यंत (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(&func_hashtab, hi, todo)
	    जर (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		जर (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    पूर्णांक changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    जर (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			जर (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    अन्यथा
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			जर (changed != func_hashtab.ht_changed)
			    थांबा;
		    }
		}
		--todo;
	    }
    }
}

#जर defined(EXITFREE)
    शून्य
free_all_functions(शून्य)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    पूर्णांक		changed;

    // Clean up the current_funccal chain and the funccal stack.
    पर्यंत (current_funccal != रिक्त)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	जर (current_funccal == रिक्त && funccal_चळत != रिक्त)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    पर्यंत (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(&func_hashtab, hi, todo)
	    जर (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		जर (func_name_refcount(fp->uf_name))
		    ++skipped;
		अन्यथा
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    जर (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			थांबा;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    पर्यंत (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तुसंच(&func_hashtab, hi, todo)
	    जर (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		जर (func_name_refcount(fp->uf_name))
		    ++skipped;
		अन्यथा
		{
		    जर (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			थांबा;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    जर (skipped == 0)
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
    पूर्णांक
builtin_function(char_u *name, पूर्णांक len)
{
    पूर्णांक i;

    जर (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	परत FALSE;
    साठी (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	जर (name[i] == AUTOLOAD_CHAR)
	    परत FALSE;
	जर (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    जर (name[i] == '.')
		परत FALSE;
	    थांबा;
	}
    }
    परत TRUE;
}

    पूर्णांक
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
    पूर्णांक		argc = 0;
    पूर्णांक		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तुसंच(l, item)
    {
	जर (argc == MAX_FUNC_ARGS - (partial == रिक्त ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    थांबा;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    जर (item == रिक्त)
    {
	funcexe_T	funcexe;
	पूर्णांक		namelen = -1;

	जर (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    जर (p != रिक्त)
	    {
		जर (skip_generic_func_type_args(&p) == FAIL)
		    जा done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	जर (partial != रिक्त)
	{
	    funcexe.fe_object = partial->pt_obj;
	    जर (funcexe.fe_object != रिक्त)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    पर्यंत (argc > 0)
	clear_tv(&argv[--argc]);

done:
    परत r;
}

स्थिर पूर्णांक callback_depth = 0;

    पूर्णांक
get_callback_depth(शून्य)
{
    परत callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    पूर्णांक
call_callback(
    callback_T	*callback,
    पूर्णांक		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    पूर्णांक		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    पूर्णांक		ret;

    जर (callback->cb_name == रिक्त || *callback->cb_name == NUL)
	परत FAIL;

    जर (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	परत FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    जर (callback->cb_partial != रिक्त)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	जर (funcexe.fe_object != रिक्त)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    जर (need_rethrow && current_exception != रिक्त && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    परत ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    पूर्णांक		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    जर (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	परत -2;

    retval = tv_get_संख्या_chk(&rettv, रिक्त);
    clear_tv(&rettv);
    परत retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    शून्य
user_func_error(funcerror_T error, char_u *name, पूर्णांक found_var)
{
    निवड (error)
    {
	स्थिती FCERR_UNKNOWN:
		जर (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		अन्यथा
		    emsg_funcname(e_unknown_function_str, name);
		थांबा;
	स्थिती FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		थांबा;
	स्थिती FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		थांबा;
	स्थिती FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		थांबा;
	स्थिती FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		थांबा;
	स्थिती FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		थांबा;
	स्थिती FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		थांबा;
	स्थिती FCERR_OTHER:
	स्थिती FCERR_FAILED:
		// assume the error message was already given
		थांबा;
	स्थिती FCERR_NONE:
		थांबा;
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
    स्थिर funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	पूर्णांक	    argcount,
	पूर्णांक	    base_included,
	char_u	    *name)
{
    जर (funcexe->fe_check_type != रिक्त && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	जर (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? रिक्त : funcexe->fe_basetv,
			  name) == FAIL)
	    परत FCERR_OTHER;
    }
    परत FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    पूर्णांक
call_func(
    char_u	*funcname,	// name of the function
    पूर्णांक		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    पूर्णांक		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    पूर्णांक		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    पूर्णांक		i;
    ufunc_T	*fp = रिक्त;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*fname = रिक्त;
    char_u	*name = रिक्त;
    पूर्णांक		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    पूर्णांक		argv_clear = 0;
    पूर्णांक		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    जर (partial != रिक्त)
	fp = partial->pt_func;
    जर (fp == रिक्त)
	fp = funcexe->fe_ufunc;

    जर (fp == रिक्त)
    {
	जर (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    जर (p != रिक्त)
	    {
		len = p - funcname;
		जर (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == रिक्त)
		    जा theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	जर (name == रिक्त)
	    जा theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    जर (funcexe->fe_doesrange != रिक्त)
	*funcexe->fe_doesrange = FALSE;

    जर (partial != रिक्त)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	जर (partial->pt_dict != रिक्त && (selfdict == रिक्त || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	जर (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    साठी (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		जर (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    जा theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    साठी (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    जर (funcexe->fe_check_type != रिक्त
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		साठी (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    जर (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != रिक्त) ? name : funcname);

    जर (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	पूर्णांक	is_global = FALSE;

	// Skip "g:" before a function name.
	जर (fp == रिक्त && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_संख्या;	// default rettv is number zero
	rettv->vval.v_संख्या = 0;
	error = FCERR_UNKNOWN;

	जर (fp != रिक्त || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    जर (fp == रिक्त)
	    {
		fp = find_func(rfname, is_global);
		जर (fp != रिक्त && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = रिक्त;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    जर (fp == रिक्त
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, रिक्त)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    जर (fp == रिक्त && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    जर (fp == रिक्त)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		जर (p != रिक्त && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    जर (partial == रिक्त && fp != रिक्त && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		जर (fp == रिक्त)
		{
		    error = FCERR_FAILED;
		    जा theend;
		}
	    }
	    अन्यथा जर (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != रिक्त ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		जा theend;
	    }

	    जर (fp != रिक्त && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    अन्यथा जर (fp != रिक्त)
	    {
		पूर्णांक need_arg_check = FALSE;
		जर (funcexe->fe_check_type == रिक्त)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		जर (funcexe->fe_argv_func != रिक्त)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		जर (funcexe->fe_basetv != रिक्त)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, आकार(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		जर (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != रिक्त) ? name : funcname);

		जर (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	अन्यथा जर (funcexe->fe_basetv != रिक्त)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	अन्यथा
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
    जर (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    जर (!aborting())
	user_func_error(error, (name != रिक्त) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    पर्यंत (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    परत ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    पूर्णांक
call_simple_func(
    char_u	*funcname,	// name of the function
    माप_प्रकार	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    पूर्णांक		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    पूर्णांक		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_संख्या;	// default rettv is number zero
    rettv->vval.v_संख्या = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    जर (name == रिक्त)
	परत ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    जर (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    अन्यथा
	rfname = fname;
    fp = find_func(rfname, is_global);
    जर (fp != रिक्त && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = रिक्त;
    जर (fp == रिक्त)
	ret = NOTDONE;
    अन्यथा जर (fp != रिक्त && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    अन्यथा जर (fp != रिक्त)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, रिक्त);
	जर (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    परत ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    परत fp->uf_name_exp != रिक्त ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    स्थिर पूर्णांक
function_list_modified(पूर्णांक prev_ht_changed)
{
    जर (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	परत TRUE;
    }
    परत FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    स्थिर पूर्णांक
list_func_head(ufunc_T *fp, पूर्णांक indent)
{
    पूर्णांक		prev_ht_changed = func_hashtab.ht_changed;
    पूर्णांक		j;

    संदेश_start();

    // a timer at the more prompt may have deleted the function
    जर (function_list_modified(prev_ht_changed))
	परत FAIL;

    जर (indent)
	संदेश_puts("   ");
    जर (fp->uf_def_status != UF_NOT_COMPILED)
	संदेश_puts("def ");
    अन्यथा
	संदेश_puts("function ");
    संदेश_puts((अक्षर *)printable_func_name(fp));
    संदेश_putchar('(');
    साठी (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	जर (j)
	    संदेश_puts(", ");
	संदेश_puts((अक्षर *)FUNCARG(fp, j));
	जर (fp->uf_arg_types != रिक्त)
	{
	    अक्षर *tofree;

	    संदेश_puts(": ");
	    संदेश_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	जर (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    संदेश_puts(" = ");
	    संदेश_puts(((अक्षर **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    जर (fp->uf_varargs)
    {
	जर (j)
	    संदेश_puts(", ");
	संदेश_puts("...");
    }
    जर (fp->uf_va_name != रिक्त)
    {
	जर (!fp->uf_varargs)
	{
	    जर (j)
		संदेश_puts(", ");
	    संदेश_puts("...");
	}
	संदेश_puts((अक्षर *)fp->uf_va_name);
	जर (fp->uf_va_type != रिक्त)
	{
	    अक्षर *tofree;

	    संदेश_puts(": ");
	    संदेश_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    संदेश_putchar(')');

    जर (fp->uf_def_status != UF_NOT_COMPILED)
    {
	जर (fp->uf_ret_type != &t_void)
	{
	    अक्षर *tofree;

	    संदेश_puts(": ");
	    संदेश_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    अन्यथा जर (fp->uf_flags & FC_ABORT)
	संदेश_puts(" abort");
    जर (fp->uf_flags & FC_RANGE)
	संदेश_puts(" range");
    जर (fp->uf_flags & FC_DICT)
	संदेश_puts(" dict");
    जर (fp->uf_flags & FC_CLOSURE)
	संदेश_puts(" closure");
    संदेश_clr_eos();
    जर (p_verbose > 0)
	last_set_संदेश(fp->uf_script_ctx);

    परत OK;
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
    पूर्णांक		*is_global,
    पूर्णांक		skip,		// only find the end, don't evaluate
    पूर्णांक		flags)
{
    परत trans_function_name_ext(pp, is_global, skip, flags,
	    रिक्त, रिक्त, रिक्त, रिक्त);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    स्थिर char_u *
trans_function_name_ext(
    char_u	**pp,
    पूर्णांक		*is_global,
    पूर्णांक		skip,		// only find the end, don't evaluate
    पूर्णांक		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = रिक्त;
    char_u	*start;
    char_u	*end;
    पूर्णांक		lead;
    char_u	sid_buf[20];
    पूर्णांक		len;
    पूर्णांक		extra = 0;
    पूर्णांक		prefix_g = FALSE;
    lval_T	lv;
    पूर्णांक		vim9script = in_vim9script();
    पूर्णांक		vim9_local;

    जर (fdp != रिक्त)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    जर ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (पूर्णांक)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	परत vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    जर (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, रिक्त, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    जर (end == start || (vim9script && end != रिक्त
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	जर (!skip)
	    emsg(_(e_function_name_required));
	जा theend;
    }
    जर (end == रिक्त || (lv.ll_tv != रिक्त && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	जर (!aborting())
	{
	    जर (end != रिक्त)
		semsg(_(e_invalid_argument_str), start);
	}
	अन्यथा
	    *pp = find_name_end(start, रिक्त, रिक्त, FNE_INCL_BR);
	जा theend;
    }

    जर (lv.ll_ufunc != रिक्त)
    {
	जर (ufunc != रिक्त)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	जा theend;
    }

    जर (lv.ll_tv != रिक्त)
    {
	जर (fdp != रिक्त)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = रिक्त;
	    fdp->fd_di = lv.ll_di;
	}
	जर (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != रिक्त)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	अन्यथा जर (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != रिक्त)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	अन्यथा जर (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != रिक्त)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    जर (partial != रिक्त)
		*partial = lv.ll_tv->vval.v_partial;
	}
	अन्यथा
	{
	    जर (!skip && !(flags & TFN_QUIET) && (fdp == रिक्त
			     || lv.ll_dict == रिक्त || fdp->fd_newkey == रिक्त))
		emsg(_(e_funcref_required));
	    अन्यथा
		*pp = end;
	    name = रिक्त;
	}
	जा theend;
    }

    जर (lv.ll_name == रिक्त)
    {
	// Error found, but continue after the function name.
	*pp = end;
	जा theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    जर (lv.ll_exp_name != रिक्त)
    {
	len = (पूर्णांक)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, रिक्त);
	जर (name == lv.ll_exp_name)
	    name = रिक्त;
    }
    अन्यथा जर (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	पूर्णांक		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	जर (si->sn_autoload_prefix != रिक्त)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	अन्यथा
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (पूर्णांक)KE_SNR;
	    vim_snprintf((अक्षर *)sid_buf + 3, आकार(sid_buf) - 3,
						      "%ld_", (दीर्घ)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	जा theend;
    }
    अन्यथा जर (!(flags & TFN_NO_DEREF))
    {
	len = (पूर्णांक)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, रिक्त);
	जर (name == *pp)
	    name = रिक्त;
    }
    जर (name != रिक्त)
    {
	name = vim_strsave(name);
	*pp = end;
	जर (name != रिक्त && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (पूर्णांक)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	जा theend;
    }

    जर (lv.ll_exp_name != रिक्त)
    {
	len = (पूर्णांक)STRLEN(lv.ll_exp_name);
	जर (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    अन्यथा
    {
	// skip over "s:" and "g:"
	जर (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    जर (lv.ll_name[0] == 'g')
	    {
		जर (is_global != रिक्त)
		{
		    *is_global = TRUE;
		}
		अन्यथा
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (पूर्णांक)(end - lv.ll_name);
    }
    जर (len <= 0)
    {
	जर (!skip)
	    emsg(_(e_function_name_required));
	जा theend;
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
    जर (skip)
	lead = 0;	// do nothing
    अन्यथा जर (lead > 0 || vim9_local)
    {
	जर (!vim9_local)
	{
	    जर (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		जा theend;
	    }
	    lead = 3;
	}
	जर (vim9_local || (lv.ll_exp_name != रिक्त
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    माप_प्रकार  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    जर (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		जा theend;
	    }
	    sid_buflen = vim_snprintf((अक्षर *)sid_buf, आकार(sid_buf), "%ld_", (दीर्घ)current_sctx.sc_sid);
	    जर (vim9_local)
		extra = 3 + (पूर्णांक)sid_buflen;
	    अन्यथा
		lead += (पूर्णांक)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    अन्यथा जर (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	जा theend;
    }
    जर (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	जर (cp != रिक्त && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    जा theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    जर (name != रिक्त)
    {
	जर (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (पूर्णांक)KE_SNR;
	    जर (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	अन्यथा जर (prefix_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (माप_प्रकार)len);
	name[lead + extra + len] = NUL;
    }
    *pp = end;

theend:
    clear_lval(&lv);
    परत name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    स्थिर char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    जर (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	जर (p != रिक्त)
	    परत p + 1;
    }
    परत रिक्त;
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
    अक्षर	sid_buf[25];
    माप_प्रकार	sid_buflen;
    पूर्णांक		off;
    char_u	*newname;
    माप_प्रकार	newnamesize;
    char_u	*p = funcname;

    जर (funcname == रिक्त)
	परत रिक्त;

    जर (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	जर (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    परत रिक्त;
	ufunc = find_func(funcname, FALSE);
	जर (ufunc == रिक्त || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == रिक्त)
	    परत रिक्त;
	++p;
	off = 0;
    }
    अन्यथा
	off = *funcname == 's' ? 2 : 5;

    जर (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	परत रिक्त;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, आकार(sid_buf), "<SNR>%ld_",
	    (दीर्घ)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    जर (newname == रिक्त)
	परत रिक्त;
    vim_snprintf((अक्षर *)newname, newnamesize, "%s%s", sid_buf, p + off);

    परत newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    जर (n != रिक्त)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    परत n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	पूर्णांक	    *is_global,
	पूर्णांक	    skip,
	पूर्णांक	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    जर (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(शून्य)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	जर (fudi != रिक्त)
	    CLEAR_POINTER(fudi);
    }
    अन्यथा
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, रिक्त, रिक्त, रिक्त);
    *name = p;
    परत saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    शून्य
list_functions(regmatch_T *regmatch)
{
    पूर्णांक		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    साठी (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जर (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    जर ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == रिक्त
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		जर (list_func_head(fp, FALSE) == FAIL)
		    परत;
		जर (function_list_modified(prev_ht_changed))
		    परत;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    स्थिर char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    जर (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	जर (regmatch.regprog != रिक्त)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    जर (*p == '/')
	++p;

    परत p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    स्थिर ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, पूर्णांक is_global)
{
    ufunc_T	*fp = रिक्त;
    पूर्णांक		j;

    जर (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	परत रिक्त;
    }

    set_nextcmd(eap, p);

    जर (eap->nextcmd != रिक्त)
	*p = NUL;

    जर (eap->skip || got_int)
	परत रिक्त;

    fp = find_func(name, is_global);
    जर (fp == रिक्त && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	जर (up != रिक्त)
	    fp = find_func(up, FALSE);
    }

    जर (fp == रिक्त)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	परत रिक्त;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    पूर्णांक prev_ht_changed = func_hashtab.ht_changed;

    जर (list_func_head(fp, TRUE) != OK)
	परत fp;

    साठी (j = 0; j < fp->uf_ओळी.ga_len && !got_int; ++j)
    {
	जर (FUNCLINE(fp, j) == रिक्त)
	    सुरूठेवा;
	संदेश_putchar('\n');
	संदेश_outnum((दीर्घ)(j + 1));
	जर (j < 9)
	    संदेश_putchar(' ');
	जर (j < 99)
	    संदेश_putchar(' ');
	जर (function_list_modified(prev_ht_changed))
	    थांबा;
	संदेश_prt_ओळ(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    जर (!got_int)
    {
	संदेश_putchar('\n');
	जर (!function_list_modified(prev_ht_changed))
	{
	    जर (fp->uf_def_status != UF_NOT_COMPILED)
		संदेश_puts("   enddef");
	    अन्यथा
		संदेश_puts("   endfunction");
	}
    }

    परत fp;
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
	garray_T    *ओळी_to_free,
	पूर्णांक	    class_flags,
	ocmember_T  *obj_members,
	पूर्णांक         obj_member_मोजणी,
	cctx_T	    *cctx)
{
    पूर्णांक		j;
    पूर्णांक		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    माप_प्रकार	namelen = 0;
    पूर्णांक		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*ओळ_arg = रिक्त;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    पूर्णांक		varargs = FALSE;
    पूर्णांक		flags = 0;
    char_u	*ret_type = रिक्त;
    ufunc_T	*fp = रिक्त;
    पूर्णांक		fp_allocated = FALSE;
    पूर्णांक		free_fp = FALSE;
    पूर्णांक		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    स्थिर पूर्णांक	func_nr = 0;	    // number for nameless function
    पूर्णांक		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_शीर्ष;
    पूर्णांक		vim9script = in_vim9script();
    imported_T	*import = रिक्त;

    // ":function" without argument: list functions.
    जर (ends_excmd2(eap->cmd, eap->arg))
    {
	जर (!eap->skip)
	    list_functions(रिक्त);
	set_nextcmd(eap, eap->arg);
	परत रिक्त;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    जर (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	परत रिक्त;
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
    जर (name_arg != रिक्त)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    अन्यथा
    {
	जर (vim9script)
	{
	    जर (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		परत रिक्त;
	    }
	    p = to_name_end(p, TRUE);
	    जर (*skipwhite(p) == '.' && vim_strchr(p, '(') != रिक्त)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		परत रिक्त;
	    }

	    p = eap->arg;
	}

	पूर्णांक tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != रिक्त);
	जर (name == रिक्त && (fudi.fd_dict == रिक्त || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    जर (!aborting())
	    {
		जर (!eap->skip && fudi.fd_newkey != रिक्त)
		    semsg(_(e_किल्ली_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		परत रिक्त;
	    }
	    अन्यथा
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	जर (is_export && name != रिक्त)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    जर (prefixed != रिक्त && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	अन्यथा जर (paren && vim9script && name != रिक्त
				    && vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    जा ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    जर (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	जा ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    जर (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	जर (p == रिक्त)
	    जा ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    जर (*p != '(')
    {
	जर (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    जा ret_free;
	}
	// attempt to continue by skipping some text
	जर (vim_strchr(p, '(') != रिक्त)
	    p = vim_strchr(p, '(');
    }

    जर ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	जा ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    जर (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	जा ret_free;
    }

    ga_init2(&newlines, आकार(char_u *), 10);

    जर (!eap->skip && name_arg == रिक्त)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	जर (name != रिक्त)
	    arg = name;
	अन्यथा
	    arg = fudi.fd_newkey;
	जर (arg != रिक्त && (fudi.fd_di == रिक्त
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    पूर्णांक	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    जर (arg != fudi.fd_newkey)
	    {
		जर (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    जर (name_base == रिक्त)
			name_base = arg + 3;
		    अन्यथा
			++name_base;
		}
		साठी (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		जर (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    जा ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		जर (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, रिक्त,
			रिक्त, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    जा ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	जर (fudi.fd_dict != रिक्त && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    जा ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    जर (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : रिक्त, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : रिक्त,
			 रिक्त, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, ओळी_to_free) == FAIL)
	जा errret_2;
    whitep = p;

    जर (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	जर (*skipwhite(p) == ':')
	{
	    जर (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    अन्यथा जर (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    जर (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    अन्यथा
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = रिक्त;
	    }
	}
	p = skipwhite(p);
    }
    अन्यथा
	// find extra arguments "range", "dict", "abort" and "closure"
	साठी (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    जर (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    अन्यथा जर (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    अन्यथा जर (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    अन्यथा जर (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		जर (current_funccal == रिक्त)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_शीर्ष_level_str,
			    name == रिक्त ? (char_u *)"" : name);
		    जा erret;
		}
	    }
	    अन्यथा
		थांबा;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    जर (*p == '\n')
	ओळ_arg = p + 1;
    अन्यथा जर (*p != NUL
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
    जर (किल्ली_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	जर (!eap->skip && !eap->forceit)
	{
	    जर (fudi.fd_dict != रिक्त && fudi.fd_newkey == रिक्त)
		emsg(_(e_dictionary_entry_already_exists));
	    अन्यथा जर (name != रिक्त && find_func(name, is_global) != रिक्त)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	जर (!eap->skip && did_emsg)
	    जा erret;

	संदेश_putchar('\n');	    // don't overwrite the function name
	cmdline_row = संदेश_row;
    }

    // Save the starting line number.
    sourcing_lnum_शीर्ष = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    जर (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, ओळ_arg, ओळी_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	जा erret;

    /*
     * If there are no errors, add the function
     */
    जर (fudi.fd_dict != रिक्त)
    {
	अक्षर	numbuf[NUMBUFLEN];

	fp = रिक्त;
	जर (fudi.fd_newkey == रिक्त && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    जा erret;
	}
	जर (fudi.fd_di == रिक्त)
	{
	    // Can't add a function to a locked dictionary
	    जर (मूल्य_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		जा erret;
	}
	    // Can't change an existing function if it is locked
	अन्यथा जर (मूल्य_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    जा erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, आकार(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	जर (name == रिक्त)
	    जा erret;
    }
    अन्यथा जर (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	पूर्णांक		var_conflict = FALSE;
	पूर्णांक		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	जर (v != रिक्त && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	जर (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    जर (si->sn_autoload_prefix != रिक्त)
	    {
		जर (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    जर (v != रिक्त)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		अन्यथा
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    जर (prefixed != रिक्त && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			जर (v != रिक्त)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	जर (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    जा erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	जर (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == रिक्त ? name : uname, 0, FALSE);
	}

	जर (fp != रिक्त || import != रिक्त)
	{
	    पूर्णांक dead = fp != रिक्त && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    जर (import != रिक्त
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_शीर्ष;
		जर (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		अन्यथा
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		जा errret_keep;
	    }
	    जर (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		जा errret_keep;
	    }
	    जर (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = रिक्त;
		overwrite = TRUE;
	    }
	    अन्यथा
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = रिक्त;
		func_clear_वस्तुसंच(fp);
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

    जर (fp == रिक्त)
    {
	जर (fudi.fd_dict == रिक्त && vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    पूर्णांक	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    जर (SOURCING_NAME != रिक्त)
	    {
		scriptname = autoload_name(name);
		जर (scriptname != रिक्त)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (पूर्णांक)STRLEN(p);
		    slen = (पूर्णांक)STRLEN(SOURCING_NAME);
		    जर (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    जर (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_शीर्ष;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		जा erret;
	    }
	}

	जर (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	जर (fp == रिक्त)
	    जा erret;
	fp_allocated = TRUE;

	जर (fudi.fd_dict != रिक्त)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    जर (func_name == रिक्त)
	    {
		VIM_CLEAR(fp);
		जा erret;
	    }
	    जर (fudi.fd_di == रिक्त)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		जर (fudi.fd_di == रिक्त)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    जा erret;
		}
		जर (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    जा erret;
		}
	    }
	    अन्यथा
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

    जर (eap->cmdidx == CMD_def)
    {
	पूर्णांक	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	जर (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_शीर्ष;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	पूर्णांक save_is_export = is_export;
	is_export = FALSE;

	जर (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_मोजणी, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    जा erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	जर (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    जा erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    अन्यथा
	fp->uf_def_status = UF_NOT_COMPILED;

    जर (fp_allocated)
    {
	// insert the new function in the function list
	जर (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_किल्ली = UF2HIKEY(fp);
	}
	अन्यथा जर (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    जा erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_ओळी = newlines;
    newlines.ga_data = रिक्त;
    जर ((flags & FC_CLOSURE) != 0)
    {
	जर (register_closure(fp) == FAIL)
	    जा erret;
    }
    अन्यथा
	fp->uf_scoped = रिक्त;

#ifdef FEAT_PROFILE
    जर (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    जर (sandbox)
	flags |= FC_SANDBOX;
    जर (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_शीर्ष;
    जर (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    जर (eap->cmdidx == CMD_def)
	set_function_type(fp);
    अन्यथा जर (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    जर (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, रिक्त);

    जा ret_free;

erret:
    जर (fp != रिक्त)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    जर (fp != रिक्त)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	जर (IS_GENERIC_FUNC(fp))
	    generic_func_clear_वस्तुसंच(fp);
    }
    जर (free_fp)
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
    जर (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    परत fp;
}

/*
 * ":function"
 */
    शून्य
ex_function(exarg_T *eap)
{
    garray_T ओळी_to_free;

    ga_init2(&ओळी_to_free, आकार(char_u *), 50);
    (शून्य)define_function(eap, रिक्त, &ओळी_to_free, 0, रिक्त, 0, रिक्त);
    ga_clear_strings(&ओळी_to_free);
}

    पूर्णांक
get_func_arity(char_u *name, पूर्णांक *required, पूर्णांक *optional, पूर्णांक *varargs)
{
    ufunc_T	*ufunc = रिक्त;
    पूर्णांक		argcount = 0;
    पूर्णांक		min_argcount = 0;
    पूर्णांक		idx;

    idx = find_internal_func(name);
    जर (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    अन्यथा
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = रिक्त;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	जर (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	जर (ufunc == रिक्त)
	    परत FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    परत OK;
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
    पूर्णांक		is_global = FALSE;
    char_u	*bracket_start = रिक्त;
    char_u	*generic_func_name = name;

    जर (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    अन्यथा जर (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    जर (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(शून्य)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    अन्यथा
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	जर (ufunc != रिक्त)
	    परत ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      रिक्त, रिक्त, रिक्त, &ufunc);
	जर (ufunc != रिक्त)
	{
	    vim_free(fname);
	    परत ufunc;
	}
	जर (fname != रिक्त && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    जर (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		परत रिक्त;
	    }
	}
    }
    जर (fname == रिक्त)
    {
	semsg(_(e_invalid_argument_str), name);
	परत रिक्त;
    }
    जर (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	परत रिक्त;
    }

    ufunc = find_func(fname, is_global);
    जर (ufunc == रिक्त)
    {
	char_u *p = untrans_function_name(fname);

	जर (p != रिक्त)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	जर (ufunc == रिक्त)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    जर (bracket_start != रिक्त)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    जर (ufunc != रिक्त)
    {
	// handle generic functions
	जर (bracket_start == रिक्त)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    जर (ufunc == रिक्त)
	semsg(_(e_cannot_find_function_str), name);
    परत ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    शून्य
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    जर (func_needs_compiling(ufunc, compile_type))
	(शून्य)compile_def_function(ufunc, FALSE, compile_type, रिक्त);
    अन्यथा
	smsg(_("Function %s%s%s does not need compiling"),
				cl != रिक्त ? cl->class_name.string : (char_u *)"",
				cl != रिक्त ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    स्थिर शून्य
defcompile_funcs_in_script(शून्य)
{
    दीर्घ	todo = (दीर्घ)func_hashtab.ht_used;
    पूर्णांक		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    साठी (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जर (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    जर (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(शून्य)compile_def_function(ufunc, FALSE, CT_NONE, रिक्त);

		जर (func_hashtab.ht_changed != changed)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (दीर्घ)func_hashtab.ht_used;
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
    शून्य
ex_defcompile(exarg_T *eap)
{
    जर (*eap->arg != NUL)
    {
	typval_T tv;

	जर (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    जर (cl != रिक्त)
		defcompile_class(cl);
	}
	अन्यथा
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    जर (ufunc != रिक्त)
		defcompile_function(ufunc, रिक्त);
	}
    }
    अन्यथा
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
    पूर्णांक
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    जर (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	परत 5;
    जर (p[0] == 's' && p[1] == ':')
	परत 2;
    परत 0;
}

    पूर्णांक
translated_function_exists(char_u *name, पूर्णांक is_global)
{
    जर (builtin_function(name, -1))
	परत has_internal_func(name);
    परत find_func(name, is_global) != रिक्त;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    पूर्णांक
has_varargs(ufunc_T *ufunc)
{
    परत ufunc->uf_varargs || ufunc->uf_va_name != रिक्त;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    पूर्णांक
function_exists(char_u *name, पूर्णांक no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    पूर्णांक	    n = FALSE;
    पूर्णांक	    flag;
    पूर्णांक	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    जर (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    जर (p != रिक्त && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    परत n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    पूर्णांक
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    पूर्णांक		is_global = FALSE;
    ufunc_T	*fp = रिक्त;
    list_T	*args;
    पूर्णांक		is_def;
    अक्षर	*tofree;

    जर (*name == NUL)
	परत FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    जर (p != रिक्त && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    जर (fp == रिक्त)
	परत FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    जर (args == रिक्त || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	परत FAIL;
    }
    साठी (पूर्णांक j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	जर (arg == रिक्त || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    परत FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	जर (fp->uf_arg_types == रिक्त)
	    dict_add_string(arg, "type", (char_u *)"any");
	अन्यथा
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	जर (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    जर (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	जर (va == रिक्त || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    परत FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == रिक्त ? (char_u *)"" : fp->uf_va_name);
	जर (fp->uf_va_type == रिक्त)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	अन्यथा
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    जर (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    अन्यथा
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_संख्या(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_संख्या(d, "lnum", fp->uf_script_ctx.sc_lnum);
    परत OK;
}

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, पूर्णांक check)
{
    char_u	*nm = name;
    char_u	*p;
    पूर्णांक		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    जर (p != रिक्त && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	परत p;

    vim_free(p);
    परत रिक्त;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, पूर्णांक idx)
{
    स्थिर long_u	done;
    स्थिर पूर्णांक		changed;
    स्थिर hashitem_T	*hi;
    ufunc_T		*fp;

    जर (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    जर (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	पूर्णांक len;

	जर (done++ > 0)
	    ++hi;
	पर्यंत (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	जर ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    परत (char_u *)"";

	जर (fp->uf_namelen + 4 >= IOSIZE)
	    परत fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	जर (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    जर (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	परत IObuff;
    }
    परत रिक्त;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, पूर्णांक extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    जर (ufunc == रिक्त)
	परत रिक्त;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = रिक्त;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_मोजणी = रिक्त;
    ufunc->uf_tml_total = रिक्त;
    ufunc->uf_tml_self = रिक्त;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    जर (ufunc->uf_arg_types != रिक्त)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	जर (at != रिक्त)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     आकार(type_T *) * ufunc->uf_args.ga_len);
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

    जर (ufunc->uf_va_name != रिक्त)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = रिक्त;

    ga_copy_strings(&fp->uf_ओळी, &ufunc->uf_ओळी);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    जर (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    परत ufunc;
}

/*
 * ":delfunction {name}"
 */
    शून्य
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = रिक्त;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    पूर्णांक		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     रिक्त, रिक्त, रिक्त);
    vim_free(fudi.fd_newkey);
    जर (name == रिक्त)
    {
	जर (fudi.fd_dict != रिक्त && !eap->skip)
	    emsg(_(e_funcref_required));
	परत;
    }
    जर (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	परत;
    }
    set_nextcmd(eap, p);
    जर (eap->nextcmd != रिक्त)
	*p = NUL;

    जर (numbered_function(name) && fudi.fd_dict == रिक्त)
    {
	जर (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	परत;
    }
    जर (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    जर (!eap->skip)
    {
	जर (fp == रिक्त)
	{
	    जर (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    परत;
	}
	जर (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    परत;
	}
	जर (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    परत;
	}

	जर (fudi.fd_dict != रिक्त)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	अन्यथा
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    जर (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		जर (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    अन्यथा
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
func_unref(char_u *name)
{
    ufunc_T *fp = रिक्त;

    जर (name == रिक्त || !func_name_refcount(name))
	परत;
    fp = find_func(name, FALSE);
    जर (fp == रिक्त && numbered_function(name))
    {
#ifdef EXITFREE
	जर (!entered_free_all_mem)
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
    शून्य
func_ptr_unref(ufunc_T *fp)
{
    जर (fp != रिक्त && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != रिक्त
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	जर (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    शून्य
func_ref(char_u *name)
{
    ufunc_T *fp;

    जर (name == रिक्त || !func_name_refcount(name))
	परत;
    fp = find_func(name, FALSE);
    जर (fp != रिक्त)
	++fp->uf_refcount;
    अन्यथा जर (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    शून्य
func_ptr_ref(ufunc_T *fp)
{
    जर (fp != रिक्त)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    स्थिर पूर्णांक
can_free_funccal(funccall_T *fc, पूर्णांक copyID)
{
    परत (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    शून्य
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    पूर्णांक		returning = FALSE;
    evalarg_T	evalarg;

    जर (current_funccal == रिक्त)
    {
	emsg(_(e_return_not_inside_function));
	परत;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    जर (eap->skip)
	++emsg_skip;

    eap->nextcmd = रिक्त;
    जर ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	जर (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	अन्यथा
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    अन्यथा जर (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	जर (!aborting())
	    returning = do_return(eap, FALSE, TRUE, रिक्त);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    जर (returning)
	eap->nextcmd = रिक्त;
    अन्यथा जर (eap->nextcmd == रिक्त)	    // no argument
	set_nextcmd(eap, arg);

    जर (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    स्थिर पूर्णांक
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    पूर्णांक		doesrange;
    typval_T	rettv;
    पूर्णांक		failed = FALSE;

    lnum = eap->line1;
    साठी ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	जर (eap->addr_मोजणी > 0)
	{
	    जर (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		थांबा;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	जर (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    थांबा;
	}
	जर (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	जर (handle_subscript(arg, रिक्त, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    थांबा;
	}

	clear_tv(&rettv);
	जर (doesrange)
	    थांबा;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	जर (aborting())
	    थांबा;
    }
    परत failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    स्थिर पूर्णांक
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    पूर्णांक		partial_argc = 0;		// number of partial arguments
    पूर्णांक		argcount = 0;			// number of arguments found
    पूर्णांक		r;

    जर (current_funccal == रिक्त)
    {
	semsg(_(e_str_not_inside_function), "defer");
	परत FAIL;
    }
    जर (partial != रिक्त)
    {
	जर (partial->pt_dict != रिक्त)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    परत FAIL;
	}
	जर (partial->pt_argc > 0)
	{
	    पूर्णांक i;

	    partial_argc = partial->pt_argc;
	    साठी (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    पूर्णांक is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    जर (r == OK)
    {
	जर (type != रिक्त)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, रिक्त, name);
	}
	अन्यथा जर (is_builtin)
	{
	    पूर्णांक idx = find_internal_func(name);

	    जर (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    अन्यथा जर (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	अन्यथा
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    जर (ufunc != रिक्त)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		जर (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    जर (r == FAIL)
    {
	पर्यंत (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	परत FAIL;
    }
    परत add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    पूर्णांक
can_add_defer(शून्य)
{
    जर (!in_def_function() && get_current_funccal() == रिक्त)
    {
	semsg(_(e_str_not_inside_function), "defer");
	परत FALSE;
    }
    परत TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    पूर्णांक
add_defer(char_u *name, पूर्णांक argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    पूर्णांक		argcount = argcount_arg;
    defer_T	*dr;
    पूर्णांक		ret = FAIL;

    जर (saved_name == रिक्त)
	जा theend;
    जर (in_def_function())
    {
	जर (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    अन्यथा
    {
	जर (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, आकार(defer_T), 10);
	जर (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    जा theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	पर्यंत (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    पर्यंत (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    परत ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    स्थिर शून्य
handle_defer_one(funccall_T *funccal)
{
    पूर्णांक	    idx;

    साठी (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	जर (dr->dr_name == रिक्त)
	    // already being called, can happen if function does ":qa"
	    सुरूठेवा;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = रिक्त;

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
	साठी (पूर्णांक i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    स्थिर शून्य
invoke_funccall_defer(funccall_T *fc)
{
    जर (fc->fc_ectx != रिक्त)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    अन्यथा
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    शून्य
invoke_all_defer(शून्य)
{
    साठी (funccall_T *fc = current_funccal; fc != रिक्त; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    साठी (funccal_entry_T *fce = funccal_चळत; fce != रिक्त; fce = fce->next)
	साठी (funccall_T *fc = fce->शीर्ष_funccal; fc != रिक्त; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    शून्य
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    पूर्णांक		len;
    पूर्णांक		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = रिक्त;
    partial_T	*partial = रिक्त;
    evalarg_T	evalarg;
    type_T	*type = रिक्त;
    पूर्णांक		found_var = FALSE;
    पूर्णांक		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    जर (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	जर (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	परत;
    }

    tofree = trans_function_name_ext(&arg, रिक्त, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : रिक्त, &ufunc);
    जर (fudi.fd_newkey != रिक्त)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_किल्ली_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    जर (tofree == रिक्त)
	परत;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    जर (fudi.fd_dict != रिक्त)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (पूर्णांक)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != रिक्त ? रिक्त : &partial,
				vim9script && type == रिक्त ? &type : रिक्त,
						     FALSE, FALSE, &found_var);

    जर (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	जर (name == रिक्त)
	    जा end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    जर (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	जा end;
    }
    जर (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	जा end;
    }

    जर (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    अन्यथा
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
    जर ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	जर (!ends_excmd2(eap->arg, arg))
	{
	    जर (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	अन्यथा
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
    पूर्णांक
do_return(
    exarg_T	*eap,
    पूर्णांक		reanimate,
    पूर्णांक		is_cmd,
    शून्य	*rettv)
{
    पूर्णांक		idx;
    cstack_T	*cstack = eap->cstack;

    जर (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    जर (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	जर (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	अन्यथा
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    जर (reanimate)
		rettv = current_funccal->fc_rettv;

	    जर (rettv != रिक्त)
	    {
		// Store the value of the pending return.
		जर ((cstack->cs_rettv[idx] = alloc_tv()) != रिक्त)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		अन्यथा
		    emsg(_(e_out_of_memory));
	    }
	    अन्यथा
		cstack->cs_rettv[idx] = रिक्त;

	    जर (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_संख्या;
		current_funccal->fc_rettv->vval.v_संख्या = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    अन्यथा
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	जर (!reanimate && rettv != रिक्त)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    जर (!is_cmd)
		vim_free(rettv);
	}
    }

    परत idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    शून्य
discard_pending_return(शून्य *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(शून्य *rettv)
{
    char_u	*s = रिक्त;
    char_u	*tofree = रिक्त;
    char_u	numbuf[NUMBUFLEN];
    माप_प्रकार	slen = 0;
    माप_प्रकार	IObufflen;

    जर (rettv != रिक्त)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    जर (s == रिक्त)
	s = (char_u *)"";
    अन्यथा
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    जर (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    परत vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_ओळ(
    पूर्णांक	    c UNUSED,
    शून्य    *cookie,
    पूर्णांक	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    जर (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	func_ओळ_end(cookie);
#endif

    gap = &fp->uf_ओळी;
    जर (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = रिक्त;
    अन्यथा
    {
	// Skip NULL lines (continuation lines).
	पर्यंत (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == रिक्त)
	    ++fcp->fc_linenr;
	जर (fcp->fc_linenr >= gap->ga_len)
	    retval = रिक्त;
	अन्यथा
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    जर (do_profiling == PROF_YES)
		func_ओळ_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    जर (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    परत retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    पूर्णांक
func_has_ended(शून्य *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    परत (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    पूर्णांक
func_has_abort(
    शून्य    *cookie)
{
    परत ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = रिक्त;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    जर (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != रिक्त
				     && rettv->vval.v_partial->pt_func != रिक्त)
	fp = rettv->vval.v_partial->pt_func;
    अन्यथा
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == रिक्त ? रिक्त
					      : rettv->vval.v_partial->pt_name;
	जर (fname == रिक्त)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = रिक्त;
	}
	अन्यथा
	{
	    char_u	*tofree = रिक्त;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    जर (fp != रिक्त && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	जर (pt != रिक्त)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = रिक्त;
	    जर (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    अन्यथा
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		पूर्णांक		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		जर (ret_pt->pt_name != रिक्त)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		अन्यथा
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		जर (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    जर (pt->pt_argv == रिक्त)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    अन्यथा
		    {
			pt->pt_argc = ret_pt->pt_argc;
			साठी (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    परत selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(शून्य *cookie)
{
    परत ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(शून्य *cookie)
{
    परत &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    पूर्णांक *
func_dbg_tick(शून्य *cookie)
{
    परत &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    पूर्णांक
func_level(शून्य *cookie)
{
    परत ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    पूर्णांक
current_func_returned(शून्य)
{
    परत current_funccal->fc_returned;
}

    पूर्णांक
free_unref_funccal(पूर्णांक copyID, पूर्णांक testing)
{
    पूर्णांक		did_free = FALSE;
    पूर्णांक		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    साठी (pfc = &previous_funccal; *pfc != रिक्त; )
    {
	जर (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	अन्यथा
	    pfc = &(*pfc)->fc_caller;
    }
    जर (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(शून्य)garbage_collect(testing);

    परत did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    स्थिर funccall_T *
get_funccal(शून्य)
{
    पूर्णांक		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    जर (debug_backtrace_level > 0)
    {
	साठी (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    जर (temp_funccal)
		funccal = temp_funccal;
	    अन्यथा
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    परत funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    स्थिर funccall_T *
get_funccal_for_vars(शून्य)
{
    funccall_T	*funccal = रिक्त;

    जर (current_funccal == रिक्त)
	परत रिक्त;
    funccal = get_funccal();
    जर (funccal == रिक्त || funccal->fc_l_vars.dv_refcount == 0)
	परत रिक्त;
    परत funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    परत funccal == रिक्त ? रिक्त : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    परत funccal == रिक्त ? रिक्त : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    परत funccal == रिक्त ? रिक्त : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    परत funccal == रिक्त ? रिक्त : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    शून्य
list_func_vars(पूर्णांक *first)
{
    जर (current_funccal != रिक्त && current_funccal->fc_l_vars.dv_refcount > 0)
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
    जर (current_funccal != रिक्त
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	परत &current_funccal->fc_l_vars;
    परत रिक्त;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = रिक्त;
    char_u	*varname;

    जर (current_funccal == रिक्त || current_funccal->fc_func->uf_scoped == रिक्त)
	परत रिक्त;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    पर्यंत (current_funccal != रिक्त)
    {
	ht = find_var_ht(name, &varname);
	जर (ht != रिक्त && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    जर (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		थांबा;
	    }
	}
	जर (current_funccal == current_funccal->fc_func->uf_scoped)
	    थांबा;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    परत hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, पूर्णांक no_autoload)
{
    dictitem_T	*v = रिक्त;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    जर (current_funccal == रिक्त || current_funccal->fc_func->uf_scoped == रिक्त)
	परत रिक्त;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    पर्यंत (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	जर (ht != रिक्त && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    जर (v != रिक्त)
		थांबा;
	}
	जर (current_funccal == current_funccal->fc_func->uf_scoped)
	    थांबा;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    परत v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    पूर्णांक
set_ref_in_previous_funccal(पूर्णांक copyID)
{
    funccall_T	*fc;

    साठी (fc = previous_funccal; fc != रिक्त; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	जर (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, रिक्त, रिक्त)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, रिक्त, रिक्त)
		|| set_ref_in_list_वस्तुसंच(&fc->fc_l_varlist, copyID + 1, रिक्त, रिक्त))
	    परत TRUE;
    }
    परत FALSE;
}

    स्थिर पूर्णांक
set_ref_in_funccal(funccall_T *fc, पूर्णांक copyID)
{
    जर (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	जर (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, रिक्त, रिक्त)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, रिक्त, रिक्त)
		|| set_ref_in_list_वस्तुसंच(&fc->fc_l_varlist, copyID, रिक्त, रिक्त)
		|| set_ref_in_func(रिक्त, fc->fc_func, copyID))
	    परत TRUE;
    }
    परत FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    पूर्णांक
set_ref_in_call_चळत(पूर्णांक copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    साठी (fc = current_funccal; fc != रिक्त; fc = fc->fc_caller)
	जर (set_ref_in_funccal(fc, copyID))
	    परत TRUE;

    // Also go through the funccal_stack.
    साठी (entry = funccal_चळत; entry != रिक्त; entry = entry->next)
	साठी (fc = entry->शीर्ष_funccal; fc != रिक्त; fc = fc->fc_caller)
	    जर (set_ref_in_funccal(fc, copyID))
		परत TRUE;
    परत FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    पूर्णांक
set_ref_in_functions(पूर्णांक copyID)
{
    पूर्णांक		todo;
    hashitem_T	*hi = रिक्त;
    ufunc_T	*fp;

    todo = (पूर्णांक)func_hashtab.ht_used;
    साठी (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जर (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    जर (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(रिक्त, fp, copyID))
		परत TRUE;
	}
    }
    परत FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    पूर्णांक
set_ref_in_func_args(पूर्णांक copyID)
{
    पूर्णांक i;

    साठी (i = 0; i < funcargs.ga_len; ++i)
	जर (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, रिक्त, रिक्त, रिक्त))
	    परत TRUE;
    परत FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णांक
set_ref_in_func(char_u *name, ufunc_T *fp_in, पूर्णांक copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*fname;
    पूर्णांक		पात = FALSE;

    जर (name == रिक्त && fp_in == रिक्त)
	परत FALSE;

    जर (fp_in == रिक्त)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    जर (fp != रिक्त)
    {
	साठी (fc = fp->uf_scoped; fc != रिक्त; fc = fc->fc_func->uf_scoped)
	    पात = पात || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    परत पात;
}

#endif // FEAT_EVAL
