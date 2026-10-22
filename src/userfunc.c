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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
நிலையான hashtab_T	func_hashtab;

// Used by get_func_tv()
நிலையான garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
நிலையான funccall_T *current_funccal = வெற்று;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
நிலையான funccall_T *previous_funccal = வெற்று;

நிலையான வெறுமை funccal_unref(funccall_T *fc, ufunc_T *fp, முழு force);
நிலையான வெறுமை func_clear(ufunc_T *fp, முழு force);
நிலையான முழு func_free(ufunc_T *fp, முழு force);
நிலையான char_u *untrans_function_name(char_u *name);
நிலையான வெறுமை handle_defer_one(funccall_T *funccal);
நிலையான char_u *trans_function_name_ext(char_u **pp, முழு *is_global, முழு skip, முழு flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    வெறுமை
func_init(வெறுமை)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(வெறுமை)
{
    திரும்பு &func_hashtab;
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
    நிலையான char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	முழு	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	முழு	    is_vararg,
	முழு	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = வெற்று;
    முழு		is_underscore = FALSE;

    போது (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    எனில் (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == வெற்று
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	எனில் (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	திரும்பு arg;
    }

    // Extra checks in Vim9 script.
    எனில் (!skip && argtypes != வெற்று)
    {
	முழு c = *p;
	*p = NUL;
	முழு r = check_reserved_name(arg, FALSE);
	*p = c;
	எனில் (r == FAIL)
	    திரும்பு arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	எனில் (check_defined(arg, p - arg,
				   evalarg == வெற்று ? வெற்று : evalarg->eval_cctx,
			       eap == வெற்று ? வெற்று : eap->cstack, TRUE) == FAIL)
	    திரும்பு arg;
    }

    எனில் (newargs != வெற்று && ga_grow(newargs, 1) == FAIL)
	திரும்பு arg;
    எனில் (newargs != வெற்று)
    {
	முழு	c;
	முழு	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	எனில் (arg_copy == வெற்று)
	{
	    *p = c;
	    திரும்பு arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	எனில் (argtypes == வெற்று || !is_underscore)
	    // Check for duplicate argument name.
	    க்கு (i = 0; i < newargs->ga_len; ++i)
		எனில் (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    திரும்பு arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    எனில் (argtypes != வெற்று && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != வெற்று && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = வெற்று;

	எனில் (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == வெற்று ? arg : arg_copy);
	    p = skipwhite(p);
	}
	எனில் (*p == ':')
	{
	    ++p;
	    எனில் (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		திரும்பு arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    எனில் (!skip)
		type = vim_strnsave(type, p - type);
	}
	இல்லையெனில் எனில் (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == வெற்று ? arg : arg_copy);
	    திரும்பு arg;
	}
	எனில் (!skip)
	{
	    எனில் (type == வெற்று && types_optional)
	    {
		// lambda arguments default to "any" type
		எனில் (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		இல்லையெனில்
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    திரும்பு p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    நிலையான char_u *
get_function_வரி(
	exarg_T		*eap,
	garray_T	*வரிகள்_to_free,
	முழு		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    எனில் (eap->ea_getline == வெற்று)
	theline = getcmdline(':', 0L, indent, 0);
    இல்லையெனில்
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    எனில் (theline != வெற்று)
    {
	எனில் (வரிகள்_to_free->ga_len > 0
		&& eap->cmdlinep != வெற்று
		&& *eap->cmdlinep == ((char_u **)வரிகள்_to_free->ga_data)
						   [வரிகள்_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(வெறுமை)ga_add_string(வரிகள்_to_free, theline);
    }

    திரும்பு theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    நிலையான முழு
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    முழு		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    முழு		*varargs,
    garray_T	*default_args,
    முழு		skip,
    exarg_T	*eap,		// can be NULL
    முழு		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*வரிகள்_to_free)
{
    முழு		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    முழு		c;
    முழு		any_default = FALSE;
    char_u	*whitep = *argp;
    முழு		need_கோவை = FALSE;

    எனில் (newargs != வெற்று)
	ga_init2(newargs, அளவு(char_u *), 3);
    எனில் (argtypes != வெற்று)
	ga_init2(argtypes, அளவு(char_u *), 3);
    எனில் (arg_objm != வெற்று)
	ga_init2(arg_objm, அளவு(int8_T), 3);
    எனில் (!skip && default_args != வெற்று)
	ga_init2(default_args, அளவு(char_u *), 3);

    எனில் (varargs != வெற்று)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    போது (*p != endchar)
    {
	போது (eap != வெற்று && eap->ea_getline != வெற்று
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_வரி(eap, வரிகள்_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    எனில் (theline == வெற்று)
		நிறுத்து;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	எனில் (mustend && *p != endchar)
	{
	    எனில் (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    செல் err_ret;
	}
	எனில் (*p == endchar && !need_கோவை)
	    நிறுத்து;

	எனில் (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    எனில் (varargs != வெற்று)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    எனில் (argtypes != வெற்று)
	    {
		// ...name: list<type>
		எனில் (!eval_isnamec1(*p))
		{
		    எனில் (!skip)
			emsg(_(e_missing_name_after_dots));
		    செல் err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		எனில் (p == arg)
		    நிறுத்து;
		எனில் (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில் எனில் (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    போது (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    எனில் (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		நிறுத்து;
	    }

	    எனில் (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		எனில் (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_மதிப்பு_must_be_vnone_str), p);
		    செல் err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		எனில் (ga_grow(default_args, 1) == FAIL)
		    செல் err_ret;

		char_u *கோவை = vim_strnsave((char_u *)"v:none", 6);
		எனில் (கோவை == வெற்று)
		    செல் err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = கோவை;
		default_args->ga_len++;
	    }
	    இல்லையெனில் எனில் (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		செல் err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    எனில் (newargs != வெற்று && ga_grow(newargs, 1) == FAIL)
		திரும்பு FAIL;
	    எனில் (newargs != வெற்று)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		எனில் (argtypes != வெற்று && ga_grow(argtypes, 1) == OK
			    && arg_objm != வெற்று && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    எனில் (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			முழு len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			எனில் (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			எனில் (assignment != வெற்று)
			{
			    c = *argend;
			    *argend = NUL;
			    எனில் (any_default)
				vim_snprintf((எழுத்து *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    இல்லையெனில்
				vim_snprintf((எழுத்து *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    எனில் (*p == ',')
		++p;
	}
	இல்லையெனில்
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    எனில் (p == arg)
		நிறுத்து;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    எனில் (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != வெற்று)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *கோவை = p;
		எனில் (eval1(&p, &rettv, வெற்று) != FAIL)
		{
		    எனில் (!skip)
		    {
			எனில் (ga_grow(default_args, 1) == FAIL)
			    செல் err_ret;

			எனில் (need_கோவை)
			    need_கோவை = FALSE;
			// trim trailing whitespace
			போது (p > கோவை && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			கோவை = vim_strsave(கோவை);
			எனில் (கோவை == வெற்று)
			{
			    *p = c;
			    செல் err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = கோவை;
			default_args->ga_len++;
			*p = c;
		    }
		}
		இல்லையெனில்
		{
		    mustend = TRUE;
		    எனில் (*skipwhite(p) == NUL)
			need_கோவை = TRUE;
		}
	    }
	    இல்லையெனில் எனில் (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		செல் err_ret;
	    }

	    எனில் (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		எனில் (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    செல் err_ret;
		}
		p = skipwhite(p);
	    }
	    எனில் (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		எனில் (!skip && argtypes != வெற்று
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    செல் err_ret;
		}
	    }
	    இல்லையெனில்
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    எனில் (*p != endchar)
	செல் err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    திரும்பு OK;

err_ret:
    எனில் (newargs != வெற்று)
	ga_clear_strings(newargs);
    எனில் (!skip && default_args != வெற்று)
	ga_clear_strings(default_args);
    திரும்பு FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    நிலையான முழு
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    முழு		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    முழு		obj_member_மொத்தம்,
    cctx_T	*cctx)
{
    முழு len = 0;

    ga_init2(&fp->uf_type_list, அளவு(type_T *), 10);
    எனில் (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	எனில் (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	எனில் (fp->uf_arg_types != வெற்று)
	{
	    முழு	i;
	    type_T	*type;

	    க்கு (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		எனில் (p == வெற்று)
		    // will get the type from the default value
		    type = &t_unknown;
		இல்லையெனில்
		{
		    எனில் (arg_objm != வெற்று && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			க்கு (முழு om = 0; om < obj_member_மொத்தம்; ++om)
			{
			    எனில் (obj_members != வெற்று
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				நிறுத்து;
			    }
			}
		    }
		    இல்லையெனில்
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		எனில் (type == வெற்று || !valid_declaration_type(type))
		    திரும்பு FAIL;
		fp->uf_arg_types[i] = type;
		எனில் (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    எனில் (obj_members != வெற்று && *name == '_')
			// protected object method
			name++;

		    எனில் (var_wrong_func_name(name, TRUE))
			திரும்பு FAIL;
		}
	    }
	}
    }

    எனில் (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = வெற்று;
	p = ((char_u **)argtypes->ga_data)[len];
	எனில் (p == வெற்று)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	இல்லையெனில்
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    எனில் (fp->uf_va_type != வெற்று && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		திரும்பு FAIL;
	    }
	}
	எனில் (fp->uf_va_type == வெற்று)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

    நிலையான முழு
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    எனில் (ret_type == வெற்று)
	fp->uf_ret_type = &t_void;
    இல்லையெனில்
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	எனில் (fp->uf_ret_type == வெற்று)
	{
	    fp->uf_ret_type = &t_void;
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    நிலையான முழு
register_closure(ufunc_T *fp)
{
    எனில் (fp->uf_scoped == current_funccal)
	// no change
	திரும்பு OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    எனில் (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	திரும்பு FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    திரும்பு OK;
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

    எனில் (name[0] != K_SPECIAL)
	திரும்பு name;
    len = STRLEN(name);
    எனில் (len + 3 > bufsize)
	திரும்பு name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    திரும்பு buf;
}

நிலையான char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(வெறுமை)
{
    நிலையான முழு	lambda_no = 0;
    முழு		n;
    string_T	ret;

    n = vim_snprintf((எழுத்து *)lambda_name, அளவு(lambda_name), "<lambda>%d", ++lambda_no);
    எனில் (n < 1)
	ret.length = 0;
    இல்லையெனில் எனில் (n >= (முழு)அளவு(lambda_name))
	ret.length = அளவு(lambda_name) - 1;
    இல்லையெனில்
	ret.length = (size_t)n;

    ret.string = lambda_name;
    திரும்பு ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    நிலையான ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < அளவு(ufunc_T) ? அளவு(ufunc_T) : len);
    எனில் (fp != வெற்று)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((வெறுமை *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	எனில் (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    எனில் (fp->uf_name_exp != வெற்று)
		vim_snprintf((எழுத்து *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    திரும்பு fp;
}

#எனில் defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, வெறுமை *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    எனில் (fp == வெற்று)
	திரும்பு வெற்று;

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

    திரும்பு name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    நிலையான char_u *
skip_arrow(
	char_u	*start,
	முழு	equal_arrow,
	char_u	**ret_type,
	முழு	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    எனில் (equal_arrow)
    {
	எனில் (*s == ':')
	{
	    எனில் (white_error != வெற்று && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		திரும்பு வெற்று;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    எனில் (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		திரும்பு வெற்று;
	    }
	}
	bef = s;
	s = skipwhite(s);
	எனில் (*s != '=')
	    திரும்பு வெற்று;
	++s;
    }
    எனில் (*s != '>')
	திரும்பு வெற்று;
    எனில் (white_error != வெற்று && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	திரும்பு வெற்று;
    }
    திரும்பு skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    நிலையான முழு
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    எனில் (checkforcmd(&p, "function", 2))
    {
	எனில் (*p == '(')
	    திரும்பு FALSE;
	*cmd = p;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    நிலையான வெறுமை
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    எனில் (cstack == வெற்று || cstack->cs_idx < 0)
	திரும்பு;

    முழு	    மொத்தம் = cstack->cs_idx + 1;
    முழு	    i;

    fp->uf_block_ids = ALLOC_MULT(முழு, மொத்தம்);
    எனில் (fp->uf_block_ids != வெற்று)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		அளவு(முழு) * மொத்தம்);
	fp->uf_block_depth = மொத்தம்;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    க்கு (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    நிலையான char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    போது (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	எனில் (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	இல்லையெனில் எனில் (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    நிறுத்து;
	}
	இல்லையெனில்
	    MB_PTR_ADV(p);
    }

    திரும்பு p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    நிலையான char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    போது (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	எனில் (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	இல்லையெனில் எனில் (*p == '"')
	{
	    p++;    // skip the ending double quote
	    நிறுத்து;
	}
	இல்லையெனில்
	    MB_PTR_ADV(p);
    }

    திரும்பு p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    நிலையான char_u *
find_start_of_vim9_comment(char_u *வரி)
{
    char_u	*p = வரி;

    போது (*p != NUL)
    {
	எனில் (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	இல்லையெனில் எனில் (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	இல்லையெனில்
	{
	    எனில் (*p == '#')
		// Found the start of a Vim9 comment
		நிறுத்து;
	    MB_PTR_ADV(p);
	}
    }

    திரும்பு p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    நிலையான முழு
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *வரி_arg_in,
	garray_T    *வரிகள்_to_free)
{
    linenr_T	sourcing_lnum_உச்சி = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    முழு		saved_wait_return = need_wait_return;
    char_u	*வரி_arg = வரி_arg_in;
    முழு		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    எழுத்து	nesting_def[MAX_FUNC_NESTING];
    எழுத்து	nesting_inline[MAX_FUNC_NESTING];
    முழு		nesting = 0;
    getline_opt_T getline_options;
    முழு		indent = 2;
    char_u	*skip_until = வெற்று;
    முழு		ret = FAIL;
    முழு		is_heredoc = FALSE;
    முழு		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = வெற்று;
    size_t	heredoc_trimmedlen = 0;
    முழு		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    எனில் (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	எனில் (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    செல் theend;
	போது (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = வெற்று;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    க்கு (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	எனில் (சாவி_Typed)
	{
	    செய்தி_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	எனில் (வரி_arg != வெற்று)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = வரி_arg;
	    p = vim_strchr(theline, '\n');
	    எனில் (p == வெற்று)
		வரி_arg += STRLEN(வரி_arg);
	    இல்லையெனில்
	    {
		*p = NUL;
		வரி_arg = p + 1;
	    }
	}
	இல்லையெனில்
	{
	    theline = get_function_வரி(eap, வரிகள்_to_free, indent,
							      getline_options);
	}
	எனில் (சாவி_Typed)
	    வரிகள்_இடது = Rows - 1;
	எனில் (theline == வெற்று)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_உச்சி;
	    எனில் (skip_until != வெற்று)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    இல்லையெனில் எனில் (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    இல்லையெனில் எனில் (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    இல்லையெனில்
		emsg(_(e_missing_endfunction));
	    செல் theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	எனில் (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	இல்லையெனில்
	    sourcing_lnum_off = 0;

	எனில் (skip_until != வெற்று)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    எனில் (heredoc_trimmed == வெற்று
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		எனில் (heredoc_trimmed == வெற்று)
		    p = theline;
		இல்லையெனில் எனில் (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		இல்லையெனில்
		    p = theline + heredoc_trimmedlen;
		எனில் (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    எனில் (heredoc_concat_len > 0)
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
	இல்லையெனில்
	{
	    முழு	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    க்கு (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		எனில் (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    எனில் (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		எனில் (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		எனில் (nesting-- == 0)
		{
		    char_u *nextcmd = வெற்று;

		    எனில் (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    இல்லையெனில் எனில் (வரி_arg != வெற்று && *skipwhite(வரி_arg) != NUL)
			nextcmd = வரி_arg;
		    இல்லையெனில் எனில் (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_உச்சி
							+ newlines->ga_len + 1;
			எனில் (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			இல்லையெனில்
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    எனில் (nextcmd != வெற்று && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			எனில் (வரிகள்_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)வரிகள்_to_free->ga_data)
						   [வரிகள்_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)வரிகள்_to_free->ga_data)
						   [வரிகள்_to_free->ga_len - 1];
			    --வரிகள்_to_free->ga_len;
			}
		    }
		    நிறுத்து;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    இல்லையெனில் எனில் (nesting_def[nesting])
	    {
		எனில் (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    இல்லையெனில் எனில் (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    எனில் (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    இல்லையெனில் எனில் (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    எனில் (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		எனில் (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, வெற்று, TRUE, 0));
		எனில் (vim9_function && *p == '<')
		{
		    // skip generic function
		    எனில் (skip_generic_func_type_args(&p) == FAIL)
			செல் theend;
		}
		எனில் (*skipwhite(p) == '(')
		{
		    எனில் (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    இல்லையெனில்
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    எனில் (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		எனில் (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		இல்லையெனில்
		    end = p + STRLEN(p) - 1;

		போது (end > p && VIM_ISWHITE(*end))
		    --end;
		எனில் (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    முழு	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    போது (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    எனில் (!is_block)
			is_block = find_cmd_block_start(p) != வெற்று;

		    எனில் (is_block)
		    {
			எனில் (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			இல்லையெனில்
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
	    எனில் (!vim9_function || colon)
		p = skip_range(p, FALSE, வெற்று);
	    char_u *tp = p;
	    எனில் (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    இல்லையெனில்
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    எனில் (arg[0] == '<' && arg[1] =='<'
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
		எனில் (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    எனில் (heredoc_trimmed == வெற்று)
			heredoc_trimmedlen = 0;
		}
		எனில் (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		இல்லையெனில்
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		எனில் (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    எனில் (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		எனில் (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    முழு		save_sc_version = current_sctx.sc_version;
		    முழு		var_மொத்தம் = 0;
		    முழு		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_மொத்தம், &semicolon,
									 TRUE);
		    எனில் (arg != வெற்று)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    எனில் (arg != வெற்று && STRNCMP(arg, "=<<", 3) == 0)
		    {
			முழு has_trim = FALSE;

			p = skipwhite(arg + 3);
			போது (TRUE)
			{
			    எனில் (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				தொடர்;
			    }
			    எனில் (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				தொடர்;
			    }
			    நிறுத்து;
			}
			எனில் (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    எனில் (heredoc_trimmed == வெற்று)
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
	எனில் (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    செல் theend;

	எனில் (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	இல்லையெனில்
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	எனில் (p == வெற்று)
	    செல் theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	போது (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = வெற்று;

	// Check for end of eap->arg.
	எனில் (வரி_arg != வெற்று && *வரி_arg == NUL)
	    வரி_arg = வெற்று;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    எனில் (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    திரும்பு ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    நிலையான முழு
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	முழு	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    முழு		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = வெற்று;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = வெற்று;
    முழு		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    முழு		lnum_save = -1;
    linenr_T	sourcing_lnum_உச்சி = SOURCING_LNUM;
    char_u	*வரி_arg = வெற்று;

    *arg = skipwhite(*arg + 1);
    எனில் (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	திரும்பு FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    எனில் (**arg == '\n')
	வரி_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    எனில் (evalarg->eval_cctx != வெற்று)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    இல்லையெனில்
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, அளவு(char_u *), 10);
    எனில் (get_function_body(&eap, &newlines, வரி_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	செல் erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_மொத்தம் += newlines.ga_len;
    எனில் (gap->ga_itemsize > 0)
    {
	முழு	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	க்கு (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    எனில் (p == வெற்று)
		// comment line in the lambda body
		தொடர்;

	    p = skipwhite(p);

	    எனில் (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		செல் erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    எனில் (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    இல்லையெனில்
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    எனில் (pnl != வெற்று)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	எனில் (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    செல் erret;
	எனில் (eap.nextcmd != வெற்று)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	இல்லையெனில்
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	எனில் (pnl != வெற்று)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    எனில் (eap.nextcmd != வெற்று)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	எனில் (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    இல்லையெனில்
	*arg = (char_u *)"";

    எனில் (!evaluate)
    {
	ret = OK;
	செல் erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    எனில் (ufunc == வெற்று)
	செல் erret;
    எனில் (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	செல் erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = வெற்று;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = வெற்று;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_உச்சி;

    // parse argument types, the type variables of the function being
    // compiled may be used
    எனில் (parse_argument_types(ufunc, argtypes, varargs, வெற்று, வெற்று, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	செல் erret;
    }

    // parse the return type, if any
    எனில் (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	செல் erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    எனில் (pt == வெற்று)
	செல் erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_வரிகள் = newlines;
    newlines.ga_data = வெற்று;
    எனில் (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    எனில் (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_உச்சி;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = வெற்று;
    ret = OK;

erret:
    எனில் (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    எனில் (newargs != வெற்று)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    எனில் (ufunc != வெற்று)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    திரும்பு ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    முழு
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	முழு	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = வெற்று;
    partial_T   *pt = வெற்று;
    முழு		varargs;
    char_u	*ret_type = வெற்று;
    முழு		ret;
    char_u	*s;
    char_u	*start, *end;
    முழு		*old_eval_lavars = eval_lavars_used;
    முழு		eval_lavars = FALSE;
    char_u	*tofree2 = வெற்று;
    முழு		equal_arrow = **arg == '(';
    முழு		white_error = FALSE;
    முழு		called_emsg_start = called_emsg;
    முழு		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    எனில் (equal_arrow && !vim9script)
	திரும்பு NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', வெற்று,
	    types_optional ? &argtypes : வெற்று, types_optional,
			types_optional ? &arg_objm : வெற்று, evalarg,
			வெற்று, &default_args, TRUE, வெற்று, FALSE, வெற்று, வெற்று);
    எனில் (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, வெற்று) == வெற்று)
    {
	எனில் (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	திரும்பு called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    எனில் (evaluate)
	pnewargs = &newargs;
    இல்லையெனில்
	pnewargs = வெற்று;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : வெற்று, types_optional,
			types_optional ? &arg_objm : வெற்று, evalarg,
					    &varargs, &default_args,
					    FALSE, வெற்று, FALSE, வெற்று, வெற்று);
    எனில் (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : வெற்று)) == வெற்று)
    {
	எனில் (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	திரும்பு white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    எனில் (ret_type != வெற்று)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    எனில் (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    எனில் (equal_arrow && **arg == '{')
    {
	எனில் (evalarg == வெற்று)
	    // cannot happen?
	    செல் theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	எனில் (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : வெற்று, varargs,
			   &default_args, ret_type) == FAIL)
	    செல் errret;
	செல் theend;
    }
    எனில் (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_மதிப்புகள்_in_lambda));
	செல் errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_கோவை_concatenate(arg, &start, &end, evalarg);
    எனில் (ret == FAIL)
	செல் errret;

    எனில் (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg != '}')
	{
	    semsg(_(e_expected_வலது_curly_str), *arg);
	    செல் errret;
	}
	++*arg;
    }

    எனில் (evaluate)
    {
	முழு	    len;
	முழு	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *வரி_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	எனில் (fp == வெற்று)
	    செல் errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	எனில் (pt == வெற்று)
	    செல் errret;

	ga_init2(&newlines, அளவு(char_u *), 1);
	எனில் (ga_grow(&newlines, 1) == FAIL)
	    செல் errret;

	// If there are line breaks, we need to split up the string.
	வரி_end = vim_strchr(start, '\n');
	எனில் (வரி_end == வெற்று || வரி_end > end)
	    வரி_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (முழு)(வரி_end - start) + 1;
	p = alloc(len);
	எனில் (p == வெற்று)
	    செல் errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, வரி_end - start);

	எனில் (வரி_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    போது (வரி_end < end && *வரி_end == '\n')
	    {
		எனில் (ga_grow(&newlines, 1) == FAIL)
		    செல் errret;
		start = வரி_end + 1;
		வரி_end = vim_strchr(start, '\n');
		எனில் (வரி_end == வெற்று || வரி_end > end)
		    வரி_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, வரி_end - start);
	    }
	}

	எனில் (strstr((எழுத்து *)p + 7, "a:") == வெற்று)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	எனில் (types_optional)
	{
	    எனில் (parse_argument_types(fp, &argtypes,
				vim9script && varargs, வெற்று, வெற்று, 0,
				cctx) == FAIL)
		செல் errret;
	    எனில் (ret_type != வெற்று)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     வெற்று, cctx, TRUE);
		எனில் (fp->uf_ret_type == வெற்று)
		    செல் errret;
	    }
	    இல்லையெனில்
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_வரிகள் = newlines;
	எனில் (current_funccal != வெற்று && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    எனில் (register_closure(fp) == FAIL)
		செல் errret;
	}

#ifdef FEAT_PROFILE
	எனில் (prof_def_func())
	    func_do_profile(fp);
#endif
	எனில் (sandbox)
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
    எனில் (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    திரும்பு OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    எனில் (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    எனில் (fp != வெற்று)
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
    திரும்பு FAIL;
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
	முழு	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	முழு	    no_autoload,
	முழு	    new_function,
	முழு	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = வெற்று;
    முழு		cc;
    char_u	*s = வெற்று;
    hashtab_T	*ht;
    முழு		did_type = FALSE;

    எனில் (partialp != வெற்று)
	*partialp = வெற்று;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    எனில் (v != வெற்று)
    {
	tv = &v->di_tv;
    }
    இல்லையெனில் எனில் (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	முழு	    len = *lenp;

	எனில் (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	எனில் (import != வெற்று)
	{
	    name[len] = NUL;
	    எனில் (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    இல்லையெனில்
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    திரும்பு (char_u *)"";	// just in case
	}
    }

    எனில் (tv != வெற்று)
    {
	எனில் (found_var != வெற்று)
	    *found_var = TRUE;
	எனில் (tv->v_type == VAR_FUNC)
	{
	    எனில் (tv->vval.v_string == வெற்று)
	    {
		*lenp = 0;
		திரும்பு (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    எனில் (p != வெற்று && generic_func_find_open_bracket(name) != வெற்று)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		திரும்பு (char_u *)"";
	    }
	    எனில் (p != வெற்று)
		*lenp = p - s;
	    இல்லையெனில்
		*lenp = (முழு)STRLEN(s);
	}
	இல்லையெனில் எனில் (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    எனில் (pt == வெற்று)
	    {
		*lenp = 0;
		திரும்பு (char_u *)"";	// just in case
	    }
	    எனில் (partialp != வெற்று)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (முழு)STRLEN(s);
	}

	எனில் (s != வெற்று)
	{
	    எனில் (!did_type && type != வெற்று && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		எனில் (sv != வெற்று)
		    *type = sv->sv_type;
	    }
	    திரும்பு s;
	}
    }

    திரும்பு name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    வெறுமை
emsg_funcname(எழுத்து *ermsg, char_u *name)
{
    char_u	*p = name;

    எனில் (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    எனில் (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    முழு
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	முழு	    partial_argc,
	typval_T    *argvars,
	முழு	    *argcount,
	முழு	    is_builtin)
{
    char_u	*argp = *arg;
    முழு		ret = OK;
    முழு		vim9script = in_vim9script();
    முழு		evaluate = evalarg == வெற்று
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    போது (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	எனில் (*argp == ')' || *argp == ',' || *argp == NUL)
	    நிறுத்து;

	முழு arg_idx = *argcount;
	எனில் (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    நிறுத்து;
	}
	++*argcount;
	எனில் (!is_builtin && check_typval_is_மதிப்பு(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    நிறுத்து;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	எனில் (vim9script)
	{
	    எனில் (*argp != ',' && *skipwhite(argp) == ',')
	    {
		எனில் (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		நிறுத்து;
	    }
	}
	இல்லையெனில்
	    argp = skipwhite(argp);
	எனில் (*argp != ',')
	    நிறுத்து;
	எனில் (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    எனில் (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    நிறுத்து;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    எனில் (*argp == ')')
	++argp;
    இல்லையெனில்
	ret = FAIL;
    *arg = argp;
    திரும்பு ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    முழு
get_func_tv(
    char_u	*name,		// name of the function
    முழு		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    முழு		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    முழு		argcount = 0;			// number of arguments found
    முழு		vim9script = in_vim9script();
    முழு		evaluate = evalarg == வெற்று
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == வெற்று ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    எனில் (ret == OK)
    {
	முழு	i = 0;
	முழு	did_emsg_before = did_emsg;

	எனில் (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    எனில் (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, அளவு(typval_T *), 50);
	    க்கு (i = 0; i < argcount; ++i)
		எனில் (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	எனில் (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    இல்லையெனில் எனில் (!aborting() && evaluate)
    {
	எனில் (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	இல்லையெனில்
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    போது (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    எனில் (vim9script)
	*arg = argp;
    இல்லையெனில்
	*arg = skipwhite(argp);
    திரும்பு ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    நிலையான முழு
eval_fname_sid(char_u *p)
{
    திரும்பு (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    எனில் (script_name == name)
	திரும்பு name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (முழு)KE_SNR;
    fname_buflen = 3;
    எனில் (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    இல்லையெனில்
    {
	எனில் (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	இல்லையெனில்
	{
	    fname_buflen += vim_snprintf((எழுத்து *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    எனில் (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    இல்லையெனில்
    {
	fname = alloc(fnamelen + 1);
	எனில் (fname == வெற்று)
	    *error = FCERR_OTHER;
	இல்லையெனில்
	{
	    *tofree = fname;
	    vim_snprintf((எழுத்து *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    திரும்பு fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    வெறுமை
func_name_with_sid(char_u *name, முழு sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (முழு)KE_SNR;
    vim_snprintf((எழுத்து *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    நிலையான ufunc_T *
find_func_with_sid(char_u *name, முழு sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    எனில் (!SCRIPT_ID_VALID(sid))
	திரும்பு வெற்று;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    எனில் (!HASHITEM_EMPTY(hi))
	திரும்பு HI2UF(hi);
    திரும்பு வெற்று;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    நிலையான ufunc_T *
find_func_with_prefix(char_u *name, முழு sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    எனில் (vim_strchr(name, AUTOLOAD_CHAR) != வெற்று)
	திரும்பு வெற்று;	// already has the prefix
    எனில் (!SCRIPT_ID_VALID(sid))
	திரும்பு வெற்று;	// not in a script
    si = SCRIPT_ITEM(sid);
    எனில் (si->sn_autoload_prefix != வெற்று)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	எனில் (namep == வெற்று)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	எனில் (len < அளவு(buffer))
	    auto_name = buffer;
	இல்லையெனில்
	    auto_name = alloc(len);
	எனில் (auto_name != வெற்று)
	{
	    vim_snprintf((எழுத்து *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    எனில் (auto_name != buffer)
		vim_free(auto_name);
	    எனில் (!HASHITEM_EMPTY(hi))
		திரும்பு HI2UF(hi);
	}
    }

    திரும்பு வெற்று;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, முழு flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    எனில் ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	எனில் (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    எனில் (func != வெற்று)
		திரும்பு func;
	}
	எனில் (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    எனில் (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (முழு)sid);
		எனில் (func != வெற்று)
		    திரும்பு func;
	    }
	}
    }

    எனில் ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	எனில் (!HASHITEM_EMPTY(hi))
	    திரும்பு HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    திரும்பு find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, முழு is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    எனில் (fp != வெற்று && (fp->uf_flags & FC_DEAD) == 0)
	திரும்பு fp;
    திரும்பு வெற்று;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    முழு
func_is_global(ufunc_T *ufunc)
{
    திரும்பு ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    முழு
func_requires_g_prefix(ufunc_T *ufunc)
{
    திரும்பு func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == வெற்று
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    நிலையான முழு
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    முழு	len;

    எனில் (!func_is_global(fp))
	len = vim_snprintf((எழுத்து *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    இல்லையெனில்
	len = vim_snprintf((எழுத்து *)buf, bufsize, "%s", fp->uf_name);

    திரும்பு (len >= (முழு)bufsize) ? (முழு)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    நிலையான வெறுமை
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    எழுத்து	*name,
    varnumber_T nr)
{
    STRCPY(v->di_சாவி, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_எண்;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_எண் = nr;
}

/*
 * Free "fc".
 */
    நிலையான வெறுமை
free_funccal(funccall_T *fc)
{
    முழு	i;

    க்கு (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	எனில் (fp != வெற்று && fp->uf_scoped == fc)
	    fp->uf_scoped = வெற்று;
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
   நிலையான வெறுமை
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_பொருட்கள்(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    நிலையான வெறுமை
cleanup_function_call(funccall_T *fc)
{
    முழு	may_free_fc = fc->fc_refcount <= 0;
    முழு	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    எனில் (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    இல்லையெனில்
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    எனில் (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    இல்லையெனில்
    {
	முழு	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (முழு)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_பொருட்கள்(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    எனில் (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = வெற்று;
    இல்லையெனில்
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_பொருட்கள்(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    எனில் (free_fc)
	free_funccal(fc);
    இல்லையெனில்
    {
	நிலையான முழு made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	எனில் (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	இல்லையெனில் எனில் (++made_copy >= (முழு)((4096 * 1024) / அளவு(*fc)))
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
    நிலையான முழு
numbered_function(char_u *name)
{
    திரும்பு SAFE_isdigit(*name)
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
    முழு
func_name_refcount(char_u *name)
{
    திரும்பு numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    நிலையான வெறுமை
funccal_unref(funccall_T *fc, ufunc_T *fp, முழு force)
{
    funccall_T	**pfc;
    முழு		i;

    எனில் (fc == வெற்று)
	திரும்பு;

    எனில் (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	க்கு (pfc = &previous_funccal; *pfc != வெற்று; pfc = &(*pfc)->fc_caller)
	{
	    எனில் (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		திரும்பு;
	    }
	}
    க்கு (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	எனில் (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = வெற்று;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    நிலையான முழு
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    எனில் (fp->uf_flags & FC_DEAD)
	திரும்பு FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    எனில் (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	திரும்பு FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    எனில் (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	திரும்பு FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    திரும்பு TRUE;
}

    நிலையான வெறுமை
func_clear_பொருட்கள்(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_வரிகள்));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    எனில் (IS_GENERIC_FUNC(fp))
	generic_func_clear_பொருட்கள்(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = வெற்று;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    எனில் (fp->uf_cb_free != வெற்று)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = வெற்று;
    }

    fp->uf_cb_state = வெற்று;
    fp->uf_cb = வெற்று;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_மொத்தம்);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    நிலையான வெறுமை
func_clear(ufunc_T *fp, முழு force)
{
    எனில் (fp->uf_cleared)
	திரும்பு;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_பொருட்கள்(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    நிலையான முழு
func_free(ufunc_T *fp, முழு force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    எனில் ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    எனில் ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	எனில் (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	திரும்பு OK;
    }
    திரும்பு FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    வெறுமை
func_clear_free(ufunc_T *fp, முழு force)
{
    func_clear(fp, force);
    எனில் (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    இல்லையெனில்
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    முழு
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = வெற்று;

    எனில் (ufunc == வெற்று)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	திரும்பு FAIL;
    }

    fp = find_func(global, TRUE);
    எனில் (fp != வெற்று)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	திரும்பு FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    எனில் (fp == வெற்று)
	திரும்பு FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    எனில் (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_வரிகள், &fp->uf_வரிகள்) == FAIL)
	செல் failed;

    எனில் (ufunc->uf_arg_types != வெற்று)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	எனில் (fp->uf_arg_types == வெற்று)
	    செல் failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    அளவு(type_T *) * fp->uf_args.ga_len);
    }
    எனில் (ufunc->uf_va_name != வெற்று)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	எனில் (fp->uf_va_name == வெற்று)
	    செல் failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    எனில் ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == வெற்று)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	எனில் (pt == வெற்று)
	    செல் failed;
	எனில் (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    செல் failed;
	}
	ufunc->uf_partial = pt;
    }

    திரும்பு OK;

failed:
    func_clear_free(fp, TRUE);
    திரும்பு FAIL;
}

நிலையான முழு	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    முழு
funcdepth_increment(வெறுமை)
{
    எனில் (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	திரும்பு FAIL;
    }
    ++funcdepth;
    திரும்பு OK;
}

    வெறுமை
funcdepth_decrement(வெறுமை)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    முழு
funcdepth_get(வெறுமை)
{
    திரும்பு funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    வெறுமை
funcdepth_restore(முழு depth)
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

    எனில் (fc == வெற்று)
	திரும்பு வெற்று;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    திரும்பு fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    வெறுமை
remove_funccal(வெறுமை)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    நிலையான funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    முழு		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    முழு		using_sandbox = FALSE;
    முழு		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    முழு		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    முழு		default_arg_err = FALSE;
    dictitem_T	*v;
    முழு		fixvar_idx = 0;	// index in fc_fixvar[]
    முழு		i;
    முழு		ai;
    முழு		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    முழு		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    எனில் (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = -1;
	திரும்பு FCERR_FAILED;
    }

    வரி_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    எனில் (fc == வெற்று)
	திரும்பு FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, அளவு(ufunc_T *), 1);

    எனில் (fp->uf_def_status != UF_NOT_COMPILED)
    {
	எனில் (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == வெற்று ? வெற்று : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	எனில் (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	எனில் (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	எனில் (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	எனில் (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != வெற்று && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	திரும்பு retval;
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
    எனில் (selfdict != வெற்று)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_சாவி;
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
    எனில் ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    எனில் ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_சாவி;
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
    எனில் ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    க்கு (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	முழு	    addlocal = FALSE;
	typval_T    def_rettv;
	முழு	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	எனில் (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    எனில் (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_எண் == VVAL_NONE));
	    எனில் (isdefault)
	    {
		char_u	    *default_கோவை = வெற்று;

		def_rettv.v_type = VAR_எண்;
		def_rettv.vval.v_எண் = -1;

		default_கோவை = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		எனில் (eval1(&default_கோவை, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    நிறுத்து;
		}
	    }

	    namelen = STRLEN(name);
	}
	இல்லையெனில்
	{
	    எனில் ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		நிறுத்து;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((எழுத்து *)numbuf, அளவு(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	எனில் (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_சாவி, name);
	}
	இல்லையெனில்
	{
	    v = dictitem_alloc(name);
	    எனில் (v == வெற்று)
		நிறுத்து;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	எனில் (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	எனில் (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	இல்லையெனில்
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	எனில் (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    எனில் (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_தள்ளு_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    எனில் (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	எனில் (p_verbose >= 14)
	{
	    char_u	buf[செய்தி_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    செய்தி_puts("(");
	    க்கு (i = 0; i < argcount; ++i)
	    {
		எனில் (i > 0)
		    செய்தி_puts(", ");
		எனில் (argvars[i].v_type == VAR_எண்)
		    செய்தி_outnum((long)argvars[i].vval.v_எண்);
		இல்லையெனில்
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    எனில் (s != வெற்று)
		    {
			எனில் (vim_strsize(s) > செய்தி_BUF_CLEN)
			{
			    trunc_string(s, buf, செய்தி_BUF_CLEN, செய்தி_BUF_LEN);
			    s = buf;
			}
			செய்தி_puts((எழுத்து *)s);
			vim_free(tofree);
		    }
		}
	    }
	    செய்தி_puts(")");
	}
	செய்தி_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == வெற்று ? வெற்று : fc->fc_caller->fc_func);
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

    எனில் (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    இல்லையெனில் எனில் (islambda)
    {
	char_u *p = *(char_u **)fp->uf_வரிகள்.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(வெறுமை)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    இல்லையெனில்
	// call do_cmdline() to execute the lines
	do_cmdline(வெற்று, get_func_வரி, (வெறுமை *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    எனில் ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_எண்;
	rettv->vval.v_எண் = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	எனில் (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == வெற்று ? வெற்று : fc->fc_caller->fc_func;

	எனில் (fp->uf_profiling || (caller != வெற்று && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    எனில் (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	எனில் (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	இல்லையெனில் எனில் (fc->fc_rettv->v_type == VAR_எண்)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_எண்);
	இல்லையெனில்
	{
	    char_u	buf[செய்தி_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    எனில் (s != வெற்று)
	    {
		எனில் (vim_strsize(s) > செய்தி_BUF_CLEN)
		{
		    trunc_string(s, buf, செய்தி_BUF_CLEN, செய்தி_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	செய்தி_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_எடு();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    எனில் (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    எனில் (p_verbose >= 12 && SOURCING_NAME != வெற்று)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	செய்தி_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    க்கு (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    திரும்பு retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, முழு argcount)
{
    முழு regular_args = fp->uf_args.ga_len;

    எனில் (argcount < regular_args - fp->uf_def_args.ga_len)
	திரும்பு FCERR_TOOFEW;
    இல்லையெனில் எனில் (!has_varargs(fp) && argcount > regular_args)
	திரும்பு FCERR_TOOMANY;
    திரும்பு FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	முழு	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    எனில் (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	திரும்பு (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    எனில் (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != வெற்று)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    எனில் (error != FCERR_UNKNOWN)
	திரும்பு error;

    எனில் ((fp->uf_flags & FC_DICT) && selfdict == வெற்று)
    {
	error = FCERR_DICT;
    }
    இல்லையெனில்
    {
	முழு		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_தேடல்_patterns();
	எனில் (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : வெற்று);
	எனில் (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	எனில் (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_தேடல்_patterns();
    }

    திரும்பு error;
}

நிலையான funccal_entry_T *funccal_அடுக்கு = வெற்று;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    வெறுமை
save_funccal(funccal_entry_T *entry)
{
    entry->உச்சி_funccal = current_funccal;
    entry->next = funccal_அடுக்கு;
    funccal_அடுக்கு = entry;
    current_funccal = வெற்று;
}

    வெறுமை
restore_funccal(வெறுமை)
{
    எனில் (funccal_அடுக்கு == வெற்று)
	internal_error("restore_funccal()");
    இல்லையெனில்
    {
	current_funccal = funccal_அடுக்கு->உச்சி_funccal;
	funccal_அடுக்கு = funccal_அடுக்கு->next;
    }
}

    funccall_T *
get_current_funccal(வெறுமை)
{
    திரும்பு current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    முழு
at_script_level(வெறுமை)
{
    திரும்பு current_funccal == வெற்று && autocmd_match == வெற்று;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    வெறுமை
delete_script_functions(முழு sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (முழு)KE_SNR;
    len = 3 + vim_snprintf((எழுத்து *)buf + 3, அளவு(buf) - 3, "%d_", sid);

    போது (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_பொருட்கள்(&func_hashtab, hi, todo)
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		எனில் (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    முழு changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    எனில் (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			எனில் (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    இல்லையெனில்
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			எனில் (changed != func_hashtab.ht_changed)
			    நிறுத்து;
		    }
		}
		--todo;
	    }
    }
}

#எனில் defined(EXITFREE)
    வெறுமை
free_all_functions(வெறுமை)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    முழு		changed;

    // Clean up the current_funccal chain and the funccal stack.
    போது (current_funccal != வெற்று)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	எனில் (current_funccal == வெற்று && funccal_அடுக்கு != வெற்று)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    போது (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_பொருட்கள்(&func_hashtab, hi, todo)
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		எனில் (func_name_refcount(fp->uf_name))
		    ++skipped;
		இல்லையெனில்
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    எனில் (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			நிறுத்து;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    போது (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_பொருட்கள்(&func_hashtab, hi, todo)
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		எனில் (func_name_refcount(fp->uf_name))
		    ++skipped;
		இல்லையெனில்
		{
		    எனில் (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			நிறுத்து;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    எனில் (skipped == 0)
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
    முழு
builtin_function(char_u *name, முழு len)
{
    முழு i;

    எனில் (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	திரும்பு FALSE;
    க்கு (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	எனில் (name[i] == AUTOLOAD_CHAR)
	    திரும்பு FALSE;
	எனில் (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    எனில் (name[i] == '.')
		திரும்பு FALSE;
	    நிறுத்து;
	}
    }
    திரும்பு TRUE;
}

    முழு
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
    முழு		argc = 0;
    முழு		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_பொருட்கள்(l, item)
    {
	எனில் (argc == MAX_FUNC_ARGS - (partial == வெற்று ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    நிறுத்து;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    எனில் (item == வெற்று)
    {
	funcexe_T	funcexe;
	முழு		namelen = -1;

	எனில் (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    எனில் (p != வெற்று)
	    {
		எனில் (skip_generic_func_type_args(&p) == FAIL)
		    செல் done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	எனில் (partial != வெற்று)
	{
	    funcexe.fe_object = partial->pt_obj;
	    எனில் (funcexe.fe_object != வெற்று)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    போது (argc > 0)
	clear_tv(&argv[--argc]);

done:
    திரும்பு r;
}

நிலையான முழு callback_depth = 0;

    முழு
get_callback_depth(வெறுமை)
{
    திரும்பு callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    முழு
call_callback(
    callback_T	*callback,
    முழு		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    முழு		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    முழு		ret;

    எனில் (callback->cb_name == வெற்று || *callback->cb_name == NUL)
	திரும்பு FAIL;

    எனில் (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	திரும்பு FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    எனில் (callback->cb_partial != வெற்று)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	எனில் (funcexe.fe_object != வெற்று)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    எனில் (need_rethrow && current_exception != வெற்று && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    திரும்பு ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    முழு		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    எனில் (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	திரும்பு -2;

    retval = tv_get_எண்_chk(&rettv, வெற்று);
    clear_tv(&rettv);
    திரும்பு retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    வெறுமை
user_func_error(funcerror_T error, char_u *name, முழு found_var)
{
    தேர்வு (error)
    {
	நிலை FCERR_UNKNOWN:
		எனில் (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		இல்லையெனில்
		    emsg_funcname(e_unknown_function_str, name);
		நிறுத்து;
	நிலை FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		நிறுத்து;
	நிலை FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		நிறுத்து;
	நிலை FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		நிறுத்து;
	நிலை FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		நிறுத்து;
	நிலை FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		நிறுத்து;
	நிலை FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		நிறுத்து;
	நிலை FCERR_OTHER:
	நிலை FCERR_FAILED:
		// assume the error message was already given
		நிறுத்து;
	நிலை FCERR_NONE:
		நிறுத்து;
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
    நிலையான funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	முழு	    argcount,
	முழு	    base_included,
	char_u	    *name)
{
    எனில் (funcexe->fe_check_type != வெற்று && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	எனில் (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? வெற்று : funcexe->fe_basetv,
			  name) == FAIL)
	    திரும்பு FCERR_OTHER;
    }
    திரும்பு FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    முழு
call_func(
    char_u	*funcname,	// name of the function
    முழு		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    முழு		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    முழு		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    முழு		i;
    ufunc_T	*fp = வெற்று;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = வெற்று;
    char_u	*fname = வெற்று;
    char_u	*name = வெற்று;
    முழு		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    முழு		argv_clear = 0;
    முழு		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    எனில் (partial != வெற்று)
	fp = partial->pt_func;
    எனில் (fp == வெற்று)
	fp = funcexe->fe_ufunc;

    எனில் (fp == வெற்று)
    {
	எனில் (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    எனில் (p != வெற்று)
	    {
		len = p - funcname;
		எனில் (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == வெற்று)
		    செல் theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	எனில் (name == வெற்று)
	    செல் theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    எனில் (funcexe->fe_doesrange != வெற்று)
	*funcexe->fe_doesrange = FALSE;

    எனில் (partial != வெற்று)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	எனில் (partial->pt_dict != வெற்று && (selfdict == வெற்று || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	எனில் (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    க்கு (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		எனில் (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    செல் theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    க்கு (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    எனில் (funcexe->fe_check_type != வெற்று
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		க்கு (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    எனில் (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != வெற்று) ? name : funcname);

    எனில் (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	முழு	is_global = FALSE;

	// Skip "g:" before a function name.
	எனில் (fp == வெற்று && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_எண்;	// default rettv is number zero
	rettv->vval.v_எண் = 0;
	error = FCERR_UNKNOWN;

	எனில் (fp != வெற்று || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    எனில் (fp == வெற்று)
	    {
		fp = find_func(rfname, is_global);
		எனில் (fp != வெற்று && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = வெற்று;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    எனில் (fp == வெற்று
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, வெற்று)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    எனில் (fp == வெற்று && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    எனில் (fp == வெற்று)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		எனில் (p != வெற்று && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    எனில் (partial == வெற்று && fp != வெற்று && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		எனில் (fp == வெற்று)
		{
		    error = FCERR_FAILED;
		    செல் theend;
		}
	    }
	    இல்லையெனில் எனில் (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != வெற்று ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		செல் theend;
	    }

	    எனில் (fp != வெற்று && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    இல்லையெனில் எனில் (fp != வெற்று)
	    {
		முழு need_arg_check = FALSE;
		எனில் (funcexe->fe_check_type == வெற்று)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		எனில் (funcexe->fe_argv_func != வெற்று)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		எனில் (funcexe->fe_basetv != வெற்று)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, அளவு(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		எனில் (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != வெற்று) ? name : funcname);

		எனில் (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	இல்லையெனில் எனில் (funcexe->fe_basetv != வெற்று)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	இல்லையெனில்
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
    எனில் (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    எனில் (!aborting())
	user_func_error(error, (name != வெற்று) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    போது (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    திரும்பு ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    முழு
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    முழு		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = வெற்று;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    முழு		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_எண்;	// default rettv is number zero
    rettv->vval.v_எண் = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    எனில் (name == வெற்று)
	திரும்பு ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    எனில் (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    இல்லையெனில்
	rfname = fname;
    fp = find_func(rfname, is_global);
    எனில் (fp != வெற்று && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = வெற்று;
    எனில் (fp == வெற்று)
	ret = NOTDONE;
    இல்லையெனில் எனில் (fp != வெற்று && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    இல்லையெனில் எனில் (fp != வெற்று)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, வெற்று);
	எனில் (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    திரும்பு ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    திரும்பு fp->uf_name_exp != வெற்று ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    நிலையான முழு
function_list_modified(முழு prev_ht_changed)
{
    எனில் (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    நிலையான முழு
list_func_head(ufunc_T *fp, முழு indent)
{
    முழு		prev_ht_changed = func_hashtab.ht_changed;
    முழு		j;

    செய்தி_start();

    // a timer at the more prompt may have deleted the function
    எனில் (function_list_modified(prev_ht_changed))
	திரும்பு FAIL;

    எனில் (indent)
	செய்தி_puts("   ");
    எனில் (fp->uf_def_status != UF_NOT_COMPILED)
	செய்தி_puts("def ");
    இல்லையெனில்
	செய்தி_puts("function ");
    செய்தி_puts((எழுத்து *)printable_func_name(fp));
    செய்தி_putchar('(');
    க்கு (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	எனில் (j)
	    செய்தி_puts(", ");
	செய்தி_puts((எழுத்து *)FUNCARG(fp, j));
	எனில் (fp->uf_arg_types != வெற்று)
	{
	    எழுத்து *tofree;

	    செய்தி_puts(": ");
	    செய்தி_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	எனில் (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    செய்தி_puts(" = ");
	    செய்தி_puts(((எழுத்து **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    எனில் (fp->uf_varargs)
    {
	எனில் (j)
	    செய்தி_puts(", ");
	செய்தி_puts("...");
    }
    எனில் (fp->uf_va_name != வெற்று)
    {
	எனில் (!fp->uf_varargs)
	{
	    எனில் (j)
		செய்தி_puts(", ");
	    செய்தி_puts("...");
	}
	செய்தி_puts((எழுத்து *)fp->uf_va_name);
	எனில் (fp->uf_va_type != வெற்று)
	{
	    எழுத்து *tofree;

	    செய்தி_puts(": ");
	    செய்தி_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    செய்தி_putchar(')');

    எனில் (fp->uf_def_status != UF_NOT_COMPILED)
    {
	எனில் (fp->uf_ret_type != &t_void)
	{
	    எழுத்து *tofree;

	    செய்தி_puts(": ");
	    செய்தி_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    இல்லையெனில் எனில் (fp->uf_flags & FC_ABORT)
	செய்தி_puts(" abort");
    எனில் (fp->uf_flags & FC_RANGE)
	செய்தி_puts(" range");
    எனில் (fp->uf_flags & FC_DICT)
	செய்தி_puts(" dict");
    எனில் (fp->uf_flags & FC_CLOSURE)
	செய்தி_puts(" closure");
    செய்தி_clr_eos();
    எனில் (p_verbose > 0)
	last_set_செய்தி(fp->uf_script_ctx);

    திரும்பு OK;
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
    முழு		*is_global,
    முழு		skip,		// only find the end, don't evaluate
    முழு		flags)
{
    திரும்பு trans_function_name_ext(pp, is_global, skip, flags,
	    வெற்று, வெற்று, வெற்று, வெற்று);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    நிலையான char_u *
trans_function_name_ext(
    char_u	**pp,
    முழு		*is_global,
    முழு		skip,		// only find the end, don't evaluate
    முழு		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = வெற்று;
    char_u	*start;
    char_u	*end;
    முழு		lead;
    char_u	sid_buf[20];
    முழு		len;
    முழு		extra = 0;
    முழு		prefix_g = FALSE;
    lval_T	lv;
    முழு		vim9script = in_vim9script();
    முழு		vim9_local;

    எனில் (fdp != வெற்று)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    எனில் ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (முழு)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	திரும்பு vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    எனில் (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, வெற்று, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    எனில் (end == start || (vim9script && end != வெற்று
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	எனில் (!skip)
	    emsg(_(e_function_name_required));
	செல் theend;
    }
    எனில் (end == வெற்று || (lv.ll_tv != வெற்று && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	எனில் (!aborting())
	{
	    எனில் (end != வெற்று)
		semsg(_(e_invalid_argument_str), start);
	}
	இல்லையெனில்
	    *pp = find_name_end(start, வெற்று, வெற்று, FNE_INCL_BR);
	செல் theend;
    }

    எனில் (lv.ll_ufunc != வெற்று)
    {
	எனில் (ufunc != வெற்று)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	செல் theend;
    }

    எனில் (lv.ll_tv != வெற்று)
    {
	எனில் (fdp != வெற்று)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = வெற்று;
	    fdp->fd_di = lv.ll_di;
	}
	எனில் (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != வெற்று)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	இல்லையெனில் எனில் (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != வெற்று)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	இல்லையெனில் எனில் (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != வெற்று)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    எனில் (partial != வெற்று)
		*partial = lv.ll_tv->vval.v_partial;
	}
	இல்லையெனில்
	{
	    எனில் (!skip && !(flags & TFN_QUIET) && (fdp == வெற்று
			     || lv.ll_dict == வெற்று || fdp->fd_newkey == வெற்று))
		emsg(_(e_funcref_required));
	    இல்லையெனில்
		*pp = end;
	    name = வெற்று;
	}
	செல் theend;
    }

    எனில் (lv.ll_name == வெற்று)
    {
	// Error found, but continue after the function name.
	*pp = end;
	செல் theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    எனில் (lv.ll_exp_name != வெற்று)
    {
	len = (முழு)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, வெற்று);
	எனில் (name == lv.ll_exp_name)
	    name = வெற்று;
    }
    இல்லையெனில் எனில் (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	முழு		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	எனில் (si->sn_autoload_prefix != வெற்று)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	இல்லையெனில்
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (முழு)KE_SNR;
	    vim_snprintf((எழுத்து *)sid_buf + 3, அளவு(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	செல் theend;
    }
    இல்லையெனில் எனில் (!(flags & TFN_NO_DEREF))
    {
	len = (முழு)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, வெற்று);
	எனில் (name == *pp)
	    name = வெற்று;
    }
    எனில் (name != வெற்று)
    {
	name = vim_strsave(name);
	*pp = end;
	எனில் (name != வெற்று && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (முழு)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	செல் theend;
    }

    எனில் (lv.ll_exp_name != வெற்று)
    {
	len = (முழு)STRLEN(lv.ll_exp_name);
	எனில் (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    இல்லையெனில்
    {
	// skip over "s:" and "g:"
	எனில் (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    எனில் (lv.ll_name[0] == 'g')
	    {
		எனில் (is_global != வெற்று)
		{
		    *is_global = TRUE;
		}
		இல்லையெனில்
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (முழு)(end - lv.ll_name);
    }
    எனில் (len <= 0)
    {
	எனில் (!skip)
	    emsg(_(e_function_name_required));
	செல் theend;
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
    எனில் (skip)
	lead = 0;	// do nothing
    இல்லையெனில் எனில் (lead > 0 || vim9_local)
    {
	எனில் (!vim9_local)
	{
	    எனில் (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		செல் theend;
	    }
	    lead = 3;
	}
	எனில் (vim9_local || (lv.ll_exp_name != வெற்று
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    எனில் (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		செல் theend;
	    }
	    sid_buflen = vim_snprintf((எழுத்து *)sid_buf, அளவு(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    எனில் (vim9_local)
		extra = 3 + (முழு)sid_buflen;
	    இல்லையெனில்
		lead += (முழு)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    இல்லையெனில் எனில் (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	செல் theend;
    }
    எனில் (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	எனில் (cp != வெற்று && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    செல் theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    எனில் (name != வெற்று)
    {
	எனில் (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (முழு)KE_SNR;
	    எனில் (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	இல்லையெனில் எனில் (prefix_g)
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
    திரும்பு name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    நிலையான char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    எனில் (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	எனில் (p != வெற்று)
	    திரும்பு p + 1;
    }
    திரும்பு வெற்று;
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
    எழுத்து	sid_buf[25];
    size_t	sid_buflen;
    முழு		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    எனில் (funcname == வெற்று)
	திரும்பு வெற்று;

    எனில் (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	எனில் (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    திரும்பு வெற்று;
	ufunc = find_func(funcname, FALSE);
	எனில் (ufunc == வெற்று || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == வெற்று)
	    திரும்பு வெற்று;
	++p;
	off = 0;
    }
    இல்லையெனில்
	off = *funcname == 's' ? 2 : 5;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	திரும்பு வெற்று;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, அளவு(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    எனில் (newname == வெற்று)
	திரும்பு வெற்று;
    vim_snprintf((எழுத்து *)newname, newnamesize, "%s%s", sid_buf, p + off);

    திரும்பு newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    எனில் (n != வெற்று)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    திரும்பு n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	முழு	    *is_global,
	முழு	    skip,
	முழு	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    எனில் (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(வெறுமை)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	எனில் (fudi != வெற்று)
	    CLEAR_POINTER(fudi);
    }
    இல்லையெனில்
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, வெற்று, வெற்று, வெற்று);
    *name = p;
    திரும்பு saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    வெறுமை
list_functions(regmatch_T *regmatch)
{
    முழு		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    க்கு (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    எனில் ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == வெற்று
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		எனில் (list_func_head(fp, FALSE) == FAIL)
		    திரும்பு;
		எனில் (function_list_modified(prev_ht_changed))
		    திரும்பு;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    நிலையான char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    எனில் (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	எனில் (regmatch.regprog != வெற்று)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    எனில் (*p == '/')
	++p;

    திரும்பு p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    நிலையான ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, முழு is_global)
{
    ufunc_T	*fp = வெற்று;
    முழு		j;

    எனில் (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	திரும்பு வெற்று;
    }

    set_nextcmd(eap, p);

    எனில் (eap->nextcmd != வெற்று)
	*p = NUL;

    எனில் (eap->skip || got_int)
	திரும்பு வெற்று;

    fp = find_func(name, is_global);
    எனில் (fp == வெற்று && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	எனில் (up != வெற்று)
	    fp = find_func(up, FALSE);
    }

    எனில் (fp == வெற்று)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	திரும்பு வெற்று;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    முழு prev_ht_changed = func_hashtab.ht_changed;

    எனில் (list_func_head(fp, TRUE) != OK)
	திரும்பு fp;

    க்கு (j = 0; j < fp->uf_வரிகள்.ga_len && !got_int; ++j)
    {
	எனில் (FUNCLINE(fp, j) == வெற்று)
	    தொடர்;
	செய்தி_putchar('\n');
	செய்தி_outnum((long)(j + 1));
	எனில் (j < 9)
	    செய்தி_putchar(' ');
	எனில் (j < 99)
	    செய்தி_putchar(' ');
	எனில் (function_list_modified(prev_ht_changed))
	    நிறுத்து;
	செய்தி_prt_வரி(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    எனில் (!got_int)
    {
	செய்தி_putchar('\n');
	எனில் (!function_list_modified(prev_ht_changed))
	{
	    எனில் (fp->uf_def_status != UF_NOT_COMPILED)
		செய்தி_puts("   enddef");
	    இல்லையெனில்
		செய்தி_puts("   endfunction");
	}
    }

    திரும்பு fp;
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
	garray_T    *வரிகள்_to_free,
	முழு	    class_flags,
	ocmember_T  *obj_members,
	முழு         obj_member_மொத்தம்,
	cctx_T	    *cctx)
{
    முழு		j;
    முழு		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    முழு		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*வரி_arg = வெற்று;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    முழு		varargs = FALSE;
    முழு		flags = 0;
    char_u	*ret_type = வெற்று;
    ufunc_T	*fp = வெற்று;
    முழு		fp_allocated = FALSE;
    முழு		free_fp = FALSE;
    முழு		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    நிலையான முழு	func_nr = 0;	    // number for nameless function
    முழு		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_உச்சி;
    முழு		vim9script = in_vim9script();
    imported_T	*import = வெற்று;

    // ":function" without argument: list functions.
    எனில் (ends_excmd2(eap->cmd, eap->arg))
    {
	எனில் (!eap->skip)
	    list_functions(வெற்று);
	set_nextcmd(eap, eap->arg);
	திரும்பு வெற்று;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    எனில் (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	திரும்பு வெற்று;
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
    எனில் (name_arg != வெற்று)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    இல்லையெனில்
    {
	எனில் (vim9script)
	{
	    எனில் (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		திரும்பு வெற்று;
	    }
	    p = to_name_end(p, TRUE);
	    எனில் (*skipwhite(p) == '.' && vim_strchr(p, '(') != வெற்று)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		திரும்பு வெற்று;
	    }

	    p = eap->arg;
	}

	முழு tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != வெற்று);
	எனில் (name == வெற்று && (fudi.fd_dict == வெற்று || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    எனில் (!aborting())
	    {
		எனில் (!eap->skip && fudi.fd_newkey != வெற்று)
		    semsg(_(e_சாவி_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		திரும்பு வெற்று;
	    }
	    இல்லையெனில்
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	எனில் (is_export && name != வெற்று)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    எனில் (prefixed != வெற்று && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	இல்லையெனில் எனில் (paren && vim9script && name != வெற்று
				    && vim_strchr(name, AUTOLOAD_CHAR) != வெற்று)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    செல் ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    எனில் (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	செல் ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    எனில் (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	எனில் (p == வெற்று)
	    செல் ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    எனில் (*p != '(')
    {
	எனில் (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    செல் ret_free;
	}
	// attempt to continue by skipping some text
	எனில் (vim_strchr(p, '(') != வெற்று)
	    p = vim_strchr(p, '(');
    }

    எனில் ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	செல் ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    எனில் (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	செல் ret_free;
    }

    ga_init2(&newlines, அளவு(char_u *), 10);

    எனில் (!eap->skip && name_arg == வெற்று)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	எனில் (name != வெற்று)
	    arg = name;
	இல்லையெனில்
	    arg = fudi.fd_newkey;
	எனில் (arg != வெற்று && (fudi.fd_di == வெற்று
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    முழு	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    எனில் (arg != fudi.fd_newkey)
	    {
		எனில் (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    எனில் (name_base == வெற்று)
			name_base = arg + 3;
		    இல்லையெனில்
			++name_base;
		}
		க்கு (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		எனில் (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    செல் ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		எனில் (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, வெற்று,
			வெற்று, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    செல் ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	எனில் (fudi.fd_dict != வெற்று && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    செல் ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    எனில் (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : வெற்று, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : வெற்று,
			 வெற்று, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, வரிகள்_to_free) == FAIL)
	செல் errret_2;
    whitep = p;

    எனில் (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	எனில் (*skipwhite(p) == ':')
	{
	    எனில் (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    இல்லையெனில் எனில் (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    எனில் (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    இல்லையெனில்
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = வெற்று;
	    }
	}
	p = skipwhite(p);
    }
    இல்லையெனில்
	// find extra arguments "range", "dict", "abort" and "closure"
	க்கு (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    எனில் (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		எனில் (current_funccal == வெற்று)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_உச்சி_level_str,
			    name == வெற்று ? (char_u *)"" : name);
		    செல் erret;
		}
	    }
	    இல்லையெனில்
		நிறுத்து;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    எனில் (*p == '\n')
	வரி_arg = p + 1;
    இல்லையெனில் எனில் (*p != NUL
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
    எனில் (சாவி_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	எனில் (!eap->skip && !eap->forceit)
	{
	    எனில் (fudi.fd_dict != வெற்று && fudi.fd_newkey == வெற்று)
		emsg(_(e_dictionary_entry_already_exists));
	    இல்லையெனில் எனில் (name != வெற்று && find_func(name, is_global) != வெற்று)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	எனில் (!eap->skip && did_emsg)
	    செல் erret;

	செய்தி_putchar('\n');	    // don't overwrite the function name
	cmdline_row = செய்தி_row;
    }

    // Save the starting line number.
    sourcing_lnum_உச்சி = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    எனில் (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, வரி_arg, வரிகள்_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	செல் erret;

    /*
     * If there are no errors, add the function
     */
    எனில் (fudi.fd_dict != வெற்று)
    {
	எழுத்து	numbuf[NUMBUFLEN];

	fp = வெற்று;
	எனில் (fudi.fd_newkey == வெற்று && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    செல் erret;
	}
	எனில் (fudi.fd_di == வெற்று)
	{
	    // Can't add a function to a locked dictionary
	    எனில் (மதிப்பு_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		செல் erret;
	}
	    // Can't change an existing function if it is locked
	இல்லையெனில் எனில் (மதிப்பு_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    செல் erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, அளவு(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	எனில் (name == வெற்று)
	    செல் erret;
    }
    இல்லையெனில் எனில் (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	முழு		var_conflict = FALSE;
	முழு		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	எனில் (v != வெற்று && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	எனில் (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    எனில் (si->sn_autoload_prefix != வெற்று)
	    {
		எனில் (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    எனில் (v != வெற்று)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		இல்லையெனில்
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    எனில் (prefixed != வெற்று && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			எனில் (v != வெற்று)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	எனில் (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    செல் erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	எனில் (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == வெற்று ? name : uname, 0, FALSE);
	}

	எனில் (fp != வெற்று || import != வெற்று)
	{
	    முழு dead = fp != வெற்று && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    எனில் (import != வெற்று
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_உச்சி;
		எனில் (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		இல்லையெனில்
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		செல் errret_keep;
	    }
	    எனில் (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		செல் errret_keep;
	    }
	    எனில் (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = வெற்று;
		overwrite = TRUE;
	    }
	    இல்லையெனில்
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = வெற்று;
		func_clear_பொருட்கள்(fp);
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

    எனில் (fp == வெற்று)
    {
	எனில் (fudi.fd_dict == வெற்று && vim_strchr(name, AUTOLOAD_CHAR) != வெற்று)
	{
	    முழு	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    எனில் (SOURCING_NAME != வெற்று)
	    {
		scriptname = autoload_name(name);
		எனில் (scriptname != வெற்று)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (முழு)STRLEN(p);
		    slen = (முழு)STRLEN(SOURCING_NAME);
		    எனில் (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    எனில் (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_உச்சி;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		செல் erret;
	    }
	}

	எனில் (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	எனில் (fp == வெற்று)
	    செல் erret;
	fp_allocated = TRUE;

	எனில் (fudi.fd_dict != வெற்று)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    எனில் (func_name == வெற்று)
	    {
		VIM_CLEAR(fp);
		செல் erret;
	    }
	    எனில் (fudi.fd_di == வெற்று)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		எனில் (fudi.fd_di == வெற்று)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    செல் erret;
		}
		எனில் (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    செல் erret;
		}
	    }
	    இல்லையெனில்
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

    எனில் (eap->cmdidx == CMD_def)
    {
	முழு	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	எனில் (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_உச்சி;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	முழு save_is_export = is_export;
	is_export = FALSE;

	எனில் (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_மொத்தம், cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    செல் erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	எனில் (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    செல் erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    இல்லையெனில்
	fp->uf_def_status = UF_NOT_COMPILED;

    எனில் (fp_allocated)
    {
	// insert the new function in the function list
	எனில் (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_சாவி = UF2HIKEY(fp);
	}
	இல்லையெனில் எனில் (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    செல் erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_வரிகள் = newlines;
    newlines.ga_data = வெற்று;
    எனில் ((flags & FC_CLOSURE) != 0)
    {
	எனில் (register_closure(fp) == FAIL)
	    செல் erret;
    }
    இல்லையெனில்
	fp->uf_scoped = வெற்று;

#ifdef FEAT_PROFILE
    எனில் (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    எனில் (sandbox)
	flags |= FC_SANDBOX;
    எனில் (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_உச்சி;
    எனில் (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    எனில் (eap->cmdidx == CMD_def)
	set_function_type(fp);
    இல்லையெனில் எனில் (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    எனில் (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, வெற்று);

    செல் ret_free;

erret:
    எனில் (fp != வெற்று)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    எனில் (fp != வெற்று)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	எனில் (IS_GENERIC_FUNC(fp))
	    generic_func_clear_பொருட்கள்(fp);
    }
    எனில் (free_fp)
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
    எனில் (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    திரும்பு fp;
}

/*
 * ":function"
 */
    வெறுமை
ex_function(exarg_T *eap)
{
    garray_T வரிகள்_to_free;

    ga_init2(&வரிகள்_to_free, அளவு(char_u *), 50);
    (வெறுமை)define_function(eap, வெற்று, &வரிகள்_to_free, 0, வெற்று, 0, வெற்று);
    ga_clear_strings(&வரிகள்_to_free);
}

    முழு
get_func_arity(char_u *name, முழு *required, முழு *optional, முழு *varargs)
{
    ufunc_T	*ufunc = வெற்று;
    முழு		argcount = 0;
    முழு		min_argcount = 0;
    முழு		idx;

    idx = find_internal_func(name);
    எனில் (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    இல்லையெனில்
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = வெற்று;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	எனில் (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	எனில் (ufunc == வெற்று)
	    திரும்பு FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    திரும்பு OK;
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
    முழு		is_global = FALSE;
    char_u	*bracket_start = வெற்று;
    char_u	*generic_func_name = name;

    எனில் (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    எனில் (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(வெறுமை)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    இல்லையெனில்
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	எனில் (ufunc != வெற்று)
	    திரும்பு ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      வெற்று, வெற்று, வெற்று, &ufunc);
	எனில் (ufunc != வெற்று)
	{
	    vim_free(fname);
	    திரும்பு ufunc;
	}
	எனில் (fname != வெற்று && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    எனில் (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		திரும்பு வெற்று;
	    }
	}
    }
    எனில் (fname == வெற்று)
    {
	semsg(_(e_invalid_argument_str), name);
	திரும்பு வெற்று;
    }
    எனில் (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	திரும்பு வெற்று;
    }

    ufunc = find_func(fname, is_global);
    எனில் (ufunc == வெற்று)
    {
	char_u *p = untrans_function_name(fname);

	எனில் (p != வெற்று)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	எனில் (ufunc == வெற்று)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    எனில் (bracket_start != வெற்று)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    எனில் (ufunc != வெற்று)
    {
	// handle generic functions
	எனில் (bracket_start == வெற்று)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    எனில் (ufunc == வெற்று)
	semsg(_(e_cannot_find_function_str), name);
    திரும்பு ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    வெறுமை
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    எனில் (func_needs_compiling(ufunc, compile_type))
	(வெறுமை)compile_def_function(ufunc, FALSE, compile_type, வெற்று);
    இல்லையெனில்
	smsg(_("Function %s%s%s does not need compiling"),
				cl != வெற்று ? cl->class_name.string : (char_u *)"",
				cl != வெற்று ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    நிலையான வெறுமை
defcompile_funcs_in_script(வெறுமை)
{
    long	todo = (long)func_hashtab.ht_used;
    முழு		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    க்கு (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    எனில் (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(வெறுமை)compile_def_function(ufunc, FALSE, CT_NONE, வெற்று);

		எனில் (func_hashtab.ht_changed != changed)
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
    வெறுமை
ex_defcompile(exarg_T *eap)
{
    எனில் (*eap->arg != NUL)
    {
	typval_T tv;

	எனில் (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    எனில் (cl != வெற்று)
		defcompile_class(cl);
	}
	இல்லையெனில்
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    எனில் (ufunc != வெற்று)
		defcompile_function(ufunc, வெற்று);
	}
    }
    இல்லையெனில்
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
    முழு
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    எனில் (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	திரும்பு 5;
    எனில் (p[0] == 's' && p[1] == ':')
	திரும்பு 2;
    திரும்பு 0;
}

    முழு
translated_function_exists(char_u *name, முழு is_global)
{
    எனில் (builtin_function(name, -1))
	திரும்பு has_internal_func(name);
    திரும்பு find_func(name, is_global) != வெற்று;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    முழு
has_varargs(ufunc_T *ufunc)
{
    திரும்பு ufunc->uf_varargs || ufunc->uf_va_name != வெற்று;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    முழு
function_exists(char_u *name, முழு no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    முழு	    n = FALSE;
    முழு	    flag;
    முழு	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    எனில் (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    எனில் (p != வெற்று && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    திரும்பு n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    முழு
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    முழு		is_global = FALSE;
    ufunc_T	*fp = வெற்று;
    list_T	*args;
    முழு		is_def;
    எழுத்து	*tofree;

    எனில் (*name == NUL)
	திரும்பு FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    எனில் (p != வெற்று && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    எனில் (fp == வெற்று)
	திரும்பு FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    எனில் (args == வெற்று || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	திரும்பு FAIL;
    }
    க்கு (முழு j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	எனில் (arg == வெற்று || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    திரும்பு FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	எனில் (fp->uf_arg_types == வெற்று)
	    dict_add_string(arg, "type", (char_u *)"any");
	இல்லையெனில்
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	எனில் (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    எனில் (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	எனில் (va == வெற்று || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    திரும்பு FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == வெற்று ? (char_u *)"" : fp->uf_va_name);
	எனில் (fp->uf_va_type == வெற்று)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	இல்லையெனில்
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    எனில் (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    இல்லையெனில்
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_எண்(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_எண்(d, "lnum", fp->uf_script_ctx.sc_lnum);
    திரும்பு OK;
}

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, முழு check)
{
    char_u	*nm = name;
    char_u	*p;
    முழு		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    எனில் (p != வெற்று && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	திரும்பு p;

    vim_free(p);
    திரும்பு வெற்று;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, முழு idx)
{
    நிலையான long_u	done;
    நிலையான முழு		changed;
    நிலையான hashitem_T	*hi;
    ufunc_T		*fp;

    எனில் (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    எனில் (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	முழு len;

	எனில் (done++ > 0)
	    ++hi;
	போது (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	எனில் ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    திரும்பு (char_u *)"";

	எனில் (fp->uf_namelen + 4 >= IOSIZE)
	    திரும்பு fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	எனில் (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    எனில் (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	திரும்பு IObuff;
    }
    திரும்பு வெற்று;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, முழு extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    எனில் (ufunc == வெற்று)
	திரும்பு வெற்று;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = வெற்று;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_மொத்தம் = வெற்று;
    ufunc->uf_tml_total = வெற்று;
    ufunc->uf_tml_self = வெற்று;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    எனில் (ufunc->uf_arg_types != வெற்று)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	எனில் (at != வெற்று)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     அளவு(type_T *) * ufunc->uf_args.ga_len);
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

    எனில் (ufunc->uf_va_name != வெற்று)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = வெற்று;

    ga_copy_strings(&fp->uf_வரிகள், &ufunc->uf_வரிகள்);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    எனில் (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    திரும்பு ufunc;
}

/*
 * ":delfunction {name}"
 */
    வெறுமை
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = வெற்று;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    முழு		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     வெற்று, வெற்று, வெற்று);
    vim_free(fudi.fd_newkey);
    எனில் (name == வெற்று)
    {
	எனில் (fudi.fd_dict != வெற்று && !eap->skip)
	    emsg(_(e_funcref_required));
	திரும்பு;
    }
    எனில் (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	திரும்பு;
    }
    set_nextcmd(eap, p);
    எனில் (eap->nextcmd != வெற்று)
	*p = NUL;

    எனில் (numbered_function(name) && fudi.fd_dict == வெற்று)
    {
	எனில் (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	திரும்பு;
    }
    எனில் (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    எனில் (!eap->skip)
    {
	எனில் (fp == வெற்று)
	{
	    எனில் (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    திரும்பு;
	}
	எனில் (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    திரும்பு;
	}
	எனில் (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    திரும்பு;
	}

	எனில் (fudi.fd_dict != வெற்று)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	இல்லையெனில்
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    எனில் (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		எனில் (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    இல்லையெனில்
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    வெறுமை
func_unref(char_u *name)
{
    ufunc_T *fp = வெற்று;

    எனில் (name == வெற்று || !func_name_refcount(name))
	திரும்பு;
    fp = find_func(name, FALSE);
    எனில் (fp == வெற்று && numbered_function(name))
    {
#ifdef EXITFREE
	எனில் (!entered_free_all_mem)
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
    வெறுமை
func_ptr_unref(ufunc_T *fp)
{
    எனில் (fp != வெற்று && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != வெற்று
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	எனில் (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    வெறுமை
func_ref(char_u *name)
{
    ufunc_T *fp;

    எனில் (name == வெற்று || !func_name_refcount(name))
	திரும்பு;
    fp = find_func(name, FALSE);
    எனில் (fp != வெற்று)
	++fp->uf_refcount;
    இல்லையெனில் எனில் (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    வெறுமை
func_ptr_ref(ufunc_T *fp)
{
    எனில் (fp != வெற்று)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    நிலையான முழு
can_free_funccal(funccall_T *fc, முழு copyID)
{
    திரும்பு (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    வெறுமை
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    முழு		returning = FALSE;
    evalarg_T	evalarg;

    எனில் (current_funccal == வெற்று)
    {
	emsg(_(e_return_not_inside_function));
	திரும்பு;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    எனில் (eap->skip)
	++emsg_skip;

    eap->nextcmd = வெற்று;
    எனில் ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	எனில் (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	இல்லையெனில்
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    இல்லையெனில் எனில் (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	எனில் (!aborting())
	    returning = do_return(eap, FALSE, TRUE, வெற்று);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    எனில் (returning)
	eap->nextcmd = வெற்று;
    இல்லையெனில் எனில் (eap->nextcmd == வெற்று)	    // no argument
	set_nextcmd(eap, arg);

    எனில் (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    நிலையான முழு
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    முழு		doesrange;
    typval_T	rettv;
    முழு		failed = FALSE;

    lnum = eap->line1;
    க்கு ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	எனில் (eap->addr_மொத்தம் > 0)
	{
	    எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		நிறுத்து;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	எனில் (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    நிறுத்து;
	}
	எனில் (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	எனில் (handle_subscript(arg, வெற்று, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    நிறுத்து;
	}

	clear_tv(&rettv);
	எனில் (doesrange)
	    நிறுத்து;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	எனில் (aborting())
	    நிறுத்து;
    }
    திரும்பு failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    நிலையான முழு
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    முழு		partial_argc = 0;		// number of partial arguments
    முழு		argcount = 0;			// number of arguments found
    முழு		r;

    எனில் (current_funccal == வெற்று)
    {
	semsg(_(e_str_not_inside_function), "defer");
	திரும்பு FAIL;
    }
    எனில் (partial != வெற்று)
    {
	எனில் (partial->pt_dict != வெற்று)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    திரும்பு FAIL;
	}
	எனில் (partial->pt_argc > 0)
	{
	    முழு i;

	    partial_argc = partial->pt_argc;
	    க்கு (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    முழு is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    எனில் (r == OK)
    {
	எனில் (type != வெற்று)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, வெற்று, name);
	}
	இல்லையெனில் எனில் (is_builtin)
	{
	    முழு idx = find_internal_func(name);

	    எனில் (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    இல்லையெனில் எனில் (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	இல்லையெனில்
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    எனில் (ufunc != வெற்று)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		எனில் (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    எனில் (r == FAIL)
    {
	போது (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	திரும்பு FAIL;
    }
    திரும்பு add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    முழு
can_add_defer(வெறுமை)
{
    எனில் (!in_def_function() && get_current_funccal() == வெற்று)
    {
	semsg(_(e_str_not_inside_function), "defer");
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    முழு
add_defer(char_u *name, முழு argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    முழு		argcount = argcount_arg;
    defer_T	*dr;
    முழு		ret = FAIL;

    எனில் (saved_name == வெற்று)
	செல் theend;
    எனில் (in_def_function())
    {
	எனில் (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    இல்லையெனில்
    {
	எனில் (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, அளவு(defer_T), 10);
	எனில் (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    செல் theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	போது (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    போது (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    திரும்பு ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    நிலையான வெறுமை
handle_defer_one(funccall_T *funccal)
{
    முழு	    idx;

    க்கு (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	எனில் (dr->dr_name == வெற்று)
	    // already being called, can happen if function does ":qa"
	    தொடர்;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = வெற்று;

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
	க்கு (முழு i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    நிலையான வெறுமை
invoke_funccall_defer(funccall_T *fc)
{
    எனில் (fc->fc_ectx != வெற்று)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    இல்லையெனில்
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    வெறுமை
invoke_all_defer(வெறுமை)
{
    க்கு (funccall_T *fc = current_funccal; fc != வெற்று; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    க்கு (funccal_entry_T *fce = funccal_அடுக்கு; fce != வெற்று; fce = fce->next)
	க்கு (funccall_T *fc = fce->உச்சி_funccal; fc != வெற்று; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    வெறுமை
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    முழு		len;
    முழு		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = வெற்று;
    partial_T	*partial = வெற்று;
    evalarg_T	evalarg;
    type_T	*type = வெற்று;
    முழு		found_var = FALSE;
    முழு		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    எனில் (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	எனில் (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	திரும்பு;
    }

    tofree = trans_function_name_ext(&arg, வெற்று, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : வெற்று, &ufunc);
    எனில் (fudi.fd_newkey != வெற்று)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_சாவி_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    எனில் (tofree == வெற்று)
	திரும்பு;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    எனில் (fudi.fd_dict != வெற்று)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (முழு)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != வெற்று ? வெற்று : &partial,
				vim9script && type == வெற்று ? &type : வெற்று,
						     FALSE, FALSE, &found_var);

    எனில் (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	எனில் (name == வெற்று)
	    செல் end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    எனில் (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	செல் end;
    }
    எனில் (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	செல் end;
    }

    எனில் (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    இல்லையெனில்
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
    எனில் ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	எனில் (!ends_excmd2(eap->arg, arg))
	{
	    எனில் (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	இல்லையெனில்
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
    முழு
do_return(
    exarg_T	*eap,
    முழு		reanimate,
    முழு		is_cmd,
    வெறுமை	*rettv)
{
    முழு		idx;
    cstack_T	*cstack = eap->cstack;

    எனில் (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    எனில் (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	எனில் (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	இல்லையெனில்
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    எனில் (reanimate)
		rettv = current_funccal->fc_rettv;

	    எனில் (rettv != வெற்று)
	    {
		// Store the value of the pending return.
		எனில் ((cstack->cs_rettv[idx] = alloc_tv()) != வெற்று)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		இல்லையெனில்
		    emsg(_(e_out_of_memory));
	    }
	    இல்லையெனில்
		cstack->cs_rettv[idx] = வெற்று;

	    எனில் (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_எண்;
		current_funccal->fc_rettv->vval.v_எண் = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    இல்லையெனில்
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	எனில் (!reanimate && rettv != வெற்று)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    எனில் (!is_cmd)
		vim_free(rettv);
	}
    }

    திரும்பு idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    வெறுமை
discard_pending_return(வெறுமை *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(வெறுமை *rettv)
{
    char_u	*s = வெற்று;
    char_u	*tofree = வெற்று;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    எனில் (rettv != வெற்று)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    எனில் (s == வெற்று)
	s = (char_u *)"";
    இல்லையெனில்
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    எனில் (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    திரும்பு vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_வரி(
    முழு	    c UNUSED,
    வெறுமை    *cookie,
    முழு	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    எனில் (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	func_வரி_end(cookie);
#endif

    gap = &fp->uf_வரிகள்;
    எனில் (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = வெற்று;
    இல்லையெனில்
    {
	// Skip NULL lines (continuation lines).
	போது (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == வெற்று)
	    ++fcp->fc_linenr;
	எனில் (fcp->fc_linenr >= gap->ga_len)
	    retval = வெற்று;
	இல்லையெனில்
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    எனில் (do_profiling == PROF_YES)
		func_வரி_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    எனில் (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    திரும்பு retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    முழு
func_has_ended(வெறுமை *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    திரும்பு (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    முழு
func_has_abort(
    வெறுமை    *cookie)
{
    திரும்பு ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = வெற்று;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    எனில் (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != வெற்று
				     && rettv->vval.v_partial->pt_func != வெற்று)
	fp = rettv->vval.v_partial->pt_func;
    இல்லையெனில்
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == வெற்று ? வெற்று
					      : rettv->vval.v_partial->pt_name;
	எனில் (fname == வெற்று)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = வெற்று;
	}
	இல்லையெனில்
	{
	    char_u	*tofree = வெற்று;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    எனில் (fp != வெற்று && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	எனில் (pt != வெற்று)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = வெற்று;
	    எனில் (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    இல்லையெனில்
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		முழு		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		எனில் (ret_pt->pt_name != வெற்று)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		இல்லையெனில்
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		எனில் (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    எனில் (pt->pt_argv == வெற்று)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    இல்லையெனில்
		    {
			pt->pt_argc = ret_pt->pt_argc;
			க்கு (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    திரும்பு selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(வெறுமை *cookie)
{
    திரும்பு ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(வெறுமை *cookie)
{
    திரும்பு &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    முழு *
func_dbg_tick(வெறுமை *cookie)
{
    திரும்பு &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    முழு
func_level(வெறுமை *cookie)
{
    திரும்பு ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    முழு
current_func_returned(வெறுமை)
{
    திரும்பு current_funccal->fc_returned;
}

    முழு
free_unref_funccal(முழு copyID, முழு testing)
{
    முழு		did_free = FALSE;
    முழு		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    க்கு (pfc = &previous_funccal; *pfc != வெற்று; )
    {
	எனில் (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	இல்லையெனில்
	    pfc = &(*pfc)->fc_caller;
    }
    எனில் (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(வெறுமை)garbage_collect(testing);

    திரும்பு did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    நிலையான funccall_T *
get_funccal(வெறுமை)
{
    முழு		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    எனில் (debug_backtrace_level > 0)
    {
	க்கு (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    எனில் (temp_funccal)
		funccal = temp_funccal;
	    இல்லையெனில்
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    திரும்பு funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    நிலையான funccall_T *
get_funccal_for_vars(வெறுமை)
{
    funccall_T	*funccal = வெற்று;

    எனில் (current_funccal == வெற்று)
	திரும்பு வெற்று;
    funccal = get_funccal();
    எனில் (funccal == வெற்று || funccal->fc_l_vars.dv_refcount == 0)
	திரும்பு வெற்று;
    திரும்பு funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(வெறுமை)
{
    funccall_T	*funccal = get_funccal_for_vars();

    திரும்பு funccal == வெற்று ? வெற்று : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(வெறுமை)
{
    funccall_T	*funccal = get_funccal_for_vars();

    திரும்பு funccal == வெற்று ? வெற்று : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(வெறுமை)
{
    funccall_T	*funccal = get_funccal_for_vars();

    திரும்பு funccal == வெற்று ? வெற்று : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(வெறுமை)
{
    funccall_T	*funccal = get_funccal_for_vars();

    திரும்பு funccal == வெற்று ? வெற்று : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    வெறுமை
list_func_vars(முழு *first)
{
    எனில் (current_funccal != வெற்று && current_funccal->fc_l_vars.dv_refcount > 0)
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
    எனில் (current_funccal != வெற்று
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	திரும்பு &current_funccal->fc_l_vars;
    திரும்பு வெற்று;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = வெற்று;
    char_u	*varname;

    எனில் (current_funccal == வெற்று || current_funccal->fc_func->uf_scoped == வெற்று)
	திரும்பு வெற்று;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    போது (current_funccal != வெற்று)
    {
	ht = find_var_ht(name, &varname);
	எனில் (ht != வெற்று && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    எனில் (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		நிறுத்து;
	    }
	}
	எனில் (current_funccal == current_funccal->fc_func->uf_scoped)
	    நிறுத்து;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    திரும்பு hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, முழு no_autoload)
{
    dictitem_T	*v = வெற்று;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    எனில் (current_funccal == வெற்று || current_funccal->fc_func->uf_scoped == வெற்று)
	திரும்பு வெற்று;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    போது (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	எனில் (ht != வெற்று && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    எனில் (v != வெற்று)
		நிறுத்து;
	}
	எனில் (current_funccal == current_funccal->fc_func->uf_scoped)
	    நிறுத்து;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    திரும்பு v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    முழு
set_ref_in_previous_funccal(முழு copyID)
{
    funccall_T	*fc;

    க்கு (fc = previous_funccal; fc != வெற்று; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	எனில் (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, வெற்று, வெற்று)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, வெற்று, வெற்று)
		|| set_ref_in_list_பொருட்கள்(&fc->fc_l_varlist, copyID + 1, வெற்று, வெற்று))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    நிலையான முழு
set_ref_in_funccal(funccall_T *fc, முழு copyID)
{
    எனில் (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	எனில் (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, வெற்று, வெற்று)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, வெற்று, வெற்று)
		|| set_ref_in_list_பொருட்கள்(&fc->fc_l_varlist, copyID, வெற்று, வெற்று)
		|| set_ref_in_func(வெற்று, fc->fc_func, copyID))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    முழு
set_ref_in_call_அடுக்கு(முழு copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    க்கு (fc = current_funccal; fc != வெற்று; fc = fc->fc_caller)
	எனில் (set_ref_in_funccal(fc, copyID))
	    திரும்பு TRUE;

    // Also go through the funccal_stack.
    க்கு (entry = funccal_அடுக்கு; entry != வெற்று; entry = entry->next)
	க்கு (fc = entry->உச்சி_funccal; fc != வெற்று; fc = fc->fc_caller)
	    எனில் (set_ref_in_funccal(fc, copyID))
		திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    முழு
set_ref_in_functions(முழு copyID)
{
    முழு		todo;
    hashitem_T	*hi = வெற்று;
    ufunc_T	*fp;

    todo = (முழு)func_hashtab.ht_used;
    க்கு (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    எனில் (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(வெற்று, fp, copyID))
		திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    முழு
set_ref_in_func_args(முழு copyID)
{
    முழு i;

    க்கு (i = 0; i < funcargs.ga_len; ++i)
	எனில் (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, வெற்று, வெற்று, வெற்று))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    முழு
set_ref_in_func(char_u *name, ufunc_T *fp_in, முழு copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = வெற்று;
    char_u	*fname;
    முழு		abort = FALSE;

    எனில் (name == வெற்று && fp_in == வெற்று)
	திரும்பு FALSE;

    எனில் (fp_in == வெற்று)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    எனில் (fp != வெற்று)
    {
	க்கு (fc = fp->uf_scoped; fc != வெற்று; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    திரும்பு abort;
}

#endif // FEAT_EVAL
