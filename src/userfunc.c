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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ایستا hashtab_T	func_hashtab;

// Used by get_func_tv()
ایستا garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
ایستا funccall_T *current_funccal = پوچ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ایستا funccall_T *previous_funccal = پوچ;

ایستا تهی funccal_unref(funccall_T *fc, ufunc_T *fp, صحیح force);
ایستا تهی func_clear(ufunc_T *fp, صحیح force);
ایستا صحیح func_free(ufunc_T *fp, صحیح force);
ایستا char_u *untrans_function_name(char_u *name);
ایستا تهی handle_defer_one(funccall_T *funccal);
ایستا char_u *trans_function_name_ext(char_u **pp, صحیح *is_global, صحیح skip, صحیح flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    تهی
func_init(تهی)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(تهی)
{
    بازگشت &func_hashtab;
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
    ایستا char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	صحیح	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	صحیح	    is_vararg,
	صحیح	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = پوچ;
    صحیح		is_underscore = FALSE;

    وقتی (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    اگر (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == پوچ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	اگر (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	بازگشت arg;
    }

    // Extra checks in Vim9 script.
    اگر (!skip && argtypes != پوچ)
    {
	صحیح c = *p;
	*p = NUL;
	صحیح r = check_reserved_name(arg, FALSE);
	*p = c;
	اگر (r == FAIL)
	    بازگشت arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	اگر (check_defined(arg, p - arg,
				   evalarg == پوچ ? پوچ : evalarg->eval_cctx,
			       eap == پوچ ? پوچ : eap->cstack, TRUE) == FAIL)
	    بازگشت arg;
    }

    اگر (newargs != پوچ && ga_grow(newargs, 1) == FAIL)
	بازگشت arg;
    اگر (newargs != پوچ)
    {
	صحیح	c;
	صحیح	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	اگر (arg_copy == پوچ)
	{
	    *p = c;
	    بازگشت arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	اگر (argtypes == پوچ || !is_underscore)
	    // Check for duplicate argument name.
	    برای (i = 0; i < newargs->ga_len; ++i)
		اگر (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    بازگشت arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    اگر (argtypes != پوچ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != پوچ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = پوچ;

	اگر (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == پوچ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	اگر (*p == ':')
	{
	    ++p;
	    اگر (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		بازگشت arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    اگر (!skip)
		type = vim_strnsave(type, p - type);
	}
	وگرنه اگر (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == پوچ ? arg : arg_copy);
	    بازگشت arg;
	}
	اگر (!skip)
	{
	    اگر (type == پوچ && types_optional)
	    {
		// lambda arguments default to "any" type
		اگر (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		وگرنه
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    بازگشت p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ایستا char_u *
get_function_سطر(
	exarg_T		*eap,
	garray_T	*سطرها_to_free,
	صحیح		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    اگر (eap->ea_getline == پوچ)
	theline = getcmdline(':', 0L, indent, 0);
    وگرنه
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    اگر (theline != پوچ)
    {
	اگر (سطرها_to_free->ga_len > 0
		&& eap->cmdlinep != پوچ
		&& *eap->cmdlinep == ((char_u **)سطرها_to_free->ga_data)
						   [سطرها_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(تهی)ga_add_string(سطرها_to_free, theline);
    }

    بازگشت theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ایستا صحیح
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    صحیح		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    صحیح		*varargs,
    garray_T	*default_args,
    صحیح		skip,
    exarg_T	*eap,		// can be NULL
    صحیح		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*سطرها_to_free)
{
    صحیح		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    صحیح		c;
    صحیح		any_default = FALSE;
    char_u	*whitep = *argp;
    صحیح		need_عبارت = FALSE;

    اگر (newargs != پوچ)
	ga_init2(newargs, اندازه(char_u *), 3);
    اگر (argtypes != پوچ)
	ga_init2(argtypes, اندازه(char_u *), 3);
    اگر (arg_objm != پوچ)
	ga_init2(arg_objm, اندازه(int8_T), 3);
    اگر (!skip && default_args != پوچ)
	ga_init2(default_args, اندازه(char_u *), 3);

    اگر (varargs != پوچ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    وقتی (*p != endchar)
    {
	وقتی (eap != پوچ && eap->ea_getline != پوچ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_سطر(eap, سطرها_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    اگر (theline == پوچ)
		بشکن;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	اگر (mustend && *p != endchar)
	{
	    اگر (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    برو err_ret;
	}
	اگر (*p == endchar && !need_عبارت)
	    بشکن;

	اگر (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    اگر (varargs != پوچ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    اگر (argtypes != پوچ)
	    {
		// ...name: list<type>
		اگر (!eval_isnamec1(*p))
		{
		    اگر (!skip)
			emsg(_(e_missing_name_after_dots));
		    برو err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		اگر (p == arg)
		    بشکن;
		اگر (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    بشکن;
		}
	    }
	}
	وگرنه اگر (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    وقتی (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    اگر (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		بشکن;
	    }

	    اگر (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		اگر (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_مقدار_must_be_vnone_str), p);
		    برو err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		اگر (ga_grow(default_args, 1) == FAIL)
		    برو err_ret;

		char_u *عبارت = vim_strnsave((char_u *)"v:none", 6);
		اگر (عبارت == پوچ)
		    برو err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = عبارت;
		default_args->ga_len++;
	    }
	    وگرنه اگر (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		برو err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    اگر (newargs != پوچ && ga_grow(newargs, 1) == FAIL)
		بازگشت FAIL;
	    اگر (newargs != پوچ)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		اگر (argtypes != پوچ && ga_grow(argtypes, 1) == OK
			    && arg_objm != پوچ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    اگر (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			صحیح len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			اگر (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			اگر (assignment != پوچ)
			{
			    c = *argend;
			    *argend = NUL;
			    اگر (any_default)
				vim_snprintf((نویسه *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    وگرنه
				vim_snprintf((نویسه *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    اگر (*p == ',')
		++p;
	}
	وگرنه
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    اگر (p == arg)
		بشکن;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    اگر (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != پوچ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *عبارت = p;
		اگر (eval1(&p, &rettv, پوچ) != FAIL)
		{
		    اگر (!skip)
		    {
			اگر (ga_grow(default_args, 1) == FAIL)
			    برو err_ret;

			اگر (need_عبارت)
			    need_عبارت = FALSE;
			// trim trailing whitespace
			وقتی (p > عبارت && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			عبارت = vim_strsave(عبارت);
			اگر (عبارت == پوچ)
			{
			    *p = c;
			    برو err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = عبارت;
			default_args->ga_len++;
			*p = c;
		    }
		}
		وگرنه
		{
		    mustend = TRUE;
		    اگر (*skipwhite(p) == NUL)
			need_عبارت = TRUE;
		}
	    }
	    وگرنه اگر (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		برو err_ret;
	    }

	    اگر (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		اگر (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    برو err_ret;
		}
		p = skipwhite(p);
	    }
	    اگر (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		اگر (!skip && argtypes != پوچ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    برو err_ret;
		}
	    }
	    وگرنه
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    اگر (*p != endchar)
	برو err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    بازگشت OK;

err_ret:
    اگر (newargs != پوچ)
	ga_clear_strings(newargs);
    اگر (!skip && default_args != پوچ)
	ga_clear_strings(default_args);
    بازگشت FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ایستا صحیح
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    صحیح		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    صحیح		obj_member_شمار,
    cctx_T	*cctx)
{
    صحیح len = 0;

    ga_init2(&fp->uf_type_list, اندازه(type_T *), 10);
    اگر (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	اگر (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	اگر (fp->uf_arg_types != پوچ)
	{
	    صحیح	i;
	    type_T	*type;

	    برای (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		اگر (p == پوچ)
		    // will get the type from the default value
		    type = &t_unknown;
		وگرنه
		{
		    اگر (arg_objm != پوچ && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			برای (صحیح om = 0; om < obj_member_شمار; ++om)
			{
			    اگر (obj_members != پوچ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				بشکن;
			    }
			}
		    }
		    وگرنه
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		اگر (type == پوچ || !valid_declaration_type(type))
		    بازگشت FAIL;
		fp->uf_arg_types[i] = type;
		اگر (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    اگر (obj_members != پوچ && *name == '_')
			// protected object method
			name++;

		    اگر (var_wrong_func_name(name, TRUE))
			بازگشت FAIL;
		}
	    }
	}
    }

    اگر (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = پوچ;
	p = ((char_u **)argtypes->ga_data)[len];
	اگر (p == پوچ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	وگرنه
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    اگر (fp->uf_va_type != پوچ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		بازگشت FAIL;
	    }
	}
	اگر (fp->uf_va_type == پوچ)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

    ایستا صحیح
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    اگر (ret_type == پوچ)
	fp->uf_ret_type = &t_void;
    وگرنه
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	اگر (fp->uf_ret_type == پوچ)
	{
	    fp->uf_ret_type = &t_void;
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ایستا صحیح
register_closure(ufunc_T *fp)
{
    اگر (fp->uf_scoped == current_funccal)
	// no change
	بازگشت OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    اگر (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	بازگشت FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    بازگشت OK;
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

    اگر (name[0] != K_SPECIAL)
	بازگشت name;
    len = STRLEN(name);
    اگر (len + 3 > bufsize)
	بازگشت name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    بازگشت buf;
}

ایستا char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(تهی)
{
    ایستا صحیح	lambda_no = 0;
    صحیح		n;
    string_T	ret;

    n = vim_snprintf((نویسه *)lambda_name, اندازه(lambda_name), "<lambda>%d", ++lambda_no);
    اگر (n < 1)
	ret.length = 0;
    وگرنه اگر (n >= (صحیح)اندازه(lambda_name))
	ret.length = اندازه(lambda_name) - 1;
    وگرنه
	ret.length = (size_t)n;

    ret.string = lambda_name;
    بازگشت ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ایستا ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < اندازه(ufunc_T) ? اندازه(ufunc_T) : len);
    اگر (fp != پوچ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((تهی *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	اگر (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    اگر (fp->uf_name_exp != پوچ)
		vim_snprintf((نویسه *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    بازگشت fp;
}

#اگر defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, تهی *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    اگر (fp == پوچ)
	بازگشت پوچ;

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

    بازگشت name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ایستا char_u *
skip_arrow(
	char_u	*start,
	صحیح	equal_arrow,
	char_u	**ret_type,
	صحیح	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    اگر (equal_arrow)
    {
	اگر (*s == ':')
	{
	    اگر (white_error != پوچ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		بازگشت پوچ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    اگر (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		بازگشت پوچ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	اگر (*s != '=')
	    بازگشت پوچ;
	++s;
    }
    اگر (*s != '>')
	بازگشت پوچ;
    اگر (white_error != پوچ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	بازگشت پوچ;
    }
    بازگشت skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ایستا صحیح
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    اگر (checkforcmd(&p, "function", 2))
    {
	اگر (*p == '(')
	    بازگشت FALSE;
	*cmd = p;
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ایستا تهی
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    اگر (cstack == پوچ || cstack->cs_idx < 0)
	بازگشت;

    صحیح	    شمار = cstack->cs_idx + 1;
    صحیح	    i;

    fp->uf_block_ids = ALLOC_MULT(صحیح, شمار);
    اگر (fp->uf_block_ids != پوچ)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		اندازه(صحیح) * شمار);
	fp->uf_block_depth = شمار;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    برای (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ایستا char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    وقتی (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	اگر (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	وگرنه اگر (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    بشکن;
	}
	وگرنه
	    MB_PTR_ADV(p);
    }

    بازگشت p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ایستا char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    وقتی (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	اگر (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	وگرنه اگر (*p == '"')
	{
	    p++;    // skip the ending double quote
	    بشکن;
	}
	وگرنه
	    MB_PTR_ADV(p);
    }

    بازگشت p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ایستا char_u *
find_start_of_vim9_comment(char_u *سطر)
{
    char_u	*p = سطر;

    وقتی (*p != NUL)
    {
	اگر (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	وگرنه اگر (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	وگرنه
	{
	    اگر (*p == '#')
		// Found the start of a Vim9 comment
		بشکن;
	    MB_PTR_ADV(p);
	}
    }

    بازگشت p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ایستا صحیح
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *سطر_arg_in,
	garray_T    *سطرها_to_free)
{
    linenr_T	sourcing_lnum_بالاترین = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    صحیح		saved_wait_return = need_wait_return;
    char_u	*سطر_arg = سطر_arg_in;
    صحیح		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    نویسه	nesting_def[MAX_FUNC_NESTING];
    نویسه	nesting_inline[MAX_FUNC_NESTING];
    صحیح		nesting = 0;
    getline_opt_T getline_options;
    صحیح		indent = 2;
    char_u	*skip_until = پوچ;
    صحیح		ret = FAIL;
    صحیح		is_heredoc = FALSE;
    صحیح		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = پوچ;
    size_t	heredoc_trimmedlen = 0;
    صحیح		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    اگر (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	اگر (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    برو theend;
	وقتی (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = پوچ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    برای (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	اگر (کلید_Typed)
	{
	    پیام_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	اگر (سطر_arg != پوچ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = سطر_arg;
	    p = vim_strchr(theline, '\n');
	    اگر (p == پوچ)
		سطر_arg += STRLEN(سطر_arg);
	    وگرنه
	    {
		*p = NUL;
		سطر_arg = p + 1;
	    }
	}
	وگرنه
	{
	    theline = get_function_سطر(eap, سطرها_to_free, indent,
							      getline_options);
	}
	اگر (کلید_Typed)
	    سطرها_چپ = Rows - 1;
	اگر (theline == پوچ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_بالاترین;
	    اگر (skip_until != پوچ)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    وگرنه اگر (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    وگرنه اگر (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    وگرنه
		emsg(_(e_missing_endfunction));
	    برو theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	اگر (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	وگرنه
	    sourcing_lnum_off = 0;

	اگر (skip_until != پوچ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    اگر (heredoc_trimmed == پوچ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		اگر (heredoc_trimmed == پوچ)
		    p = theline;
		وگرنه اگر (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		وگرنه
		    p = theline + heredoc_trimmedlen;
		اگر (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    اگر (heredoc_concat_len > 0)
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
	وگرنه
	{
	    صحیح	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    برای (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		اگر (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    اگر (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		اگر (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		اگر (nesting-- == 0)
		{
		    char_u *nextcmd = پوچ;

		    اگر (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    وگرنه اگر (سطر_arg != پوچ && *skipwhite(سطر_arg) != NUL)
			nextcmd = سطر_arg;
		    وگرنه اگر (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_بالاترین
							+ newlines->ga_len + 1;
			اگر (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			وگرنه
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    اگر (nextcmd != پوچ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			اگر (سطرها_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)سطرها_to_free->ga_data)
						   [سطرها_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)سطرها_to_free->ga_data)
						   [سطرها_to_free->ga_len - 1];
			    --سطرها_to_free->ga_len;
			}
		    }
		    بشکن;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    وگرنه اگر (nesting_def[nesting])
	    {
		اگر (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    وگرنه اگر (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    اگر (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    وگرنه اگر (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    اگر (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		اگر (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, پوچ, TRUE, 0));
		اگر (vim9_function && *p == '<')
		{
		    // skip generic function
		    اگر (skip_generic_func_type_args(&p) == FAIL)
			برو theend;
		}
		اگر (*skipwhite(p) == '(')
		{
		    اگر (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    وگرنه
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    اگر (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		اگر (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		وگرنه
		    end = p + STRLEN(p) - 1;

		وقتی (end > p && VIM_ISWHITE(*end))
		    --end;
		اگر (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    صحیح	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    وقتی (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    اگر (!is_block)
			is_block = find_cmd_block_start(p) != پوچ;

		    اگر (is_block)
		    {
			اگر (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			وگرنه
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
	    اگر (!vim9_function || colon)
		p = skip_range(p, FALSE, پوچ);
	    char_u *tp = p;
	    اگر (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    وگرنه
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    اگر (arg[0] == '<' && arg[1] =='<'
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
		اگر (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    اگر (heredoc_trimmed == پوچ)
			heredoc_trimmedlen = 0;
		}
		اگر (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		وگرنه
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		اگر (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    اگر (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		اگر (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    صحیح		save_sc_version = current_sctx.sc_version;
		    صحیح		var_شمار = 0;
		    صحیح		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_شمار, &semicolon,
									 TRUE);
		    اگر (arg != پوچ)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    اگر (arg != پوچ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			صحیح has_trim = FALSE;

			p = skipwhite(arg + 3);
			وقتی (TRUE)
			{
			    اگر (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ادامه;
			    }
			    اگر (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ادامه;
			    }
			    بشکن;
			}
			اگر (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    اگر (heredoc_trimmed == پوچ)
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
	اگر (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    برو theend;

	اگر (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	وگرنه
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	اگر (p == پوچ)
	    برو theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	وقتی (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = پوچ;

	// Check for end of eap->arg.
	اگر (سطر_arg != پوچ && *سطر_arg == NUL)
	    سطر_arg = پوچ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    اگر (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    بازگشت ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ایستا صحیح
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	صحیح	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    صحیح		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = پوچ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = پوچ;
    صحیح		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    صحیح		lnum_save = -1;
    linenr_T	sourcing_lnum_بالاترین = SOURCING_LNUM;
    char_u	*سطر_arg = پوچ;

    *arg = skipwhite(*arg + 1);
    اگر (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	بازگشت FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    اگر (**arg == '\n')
	سطر_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    اگر (evalarg->eval_cctx != پوچ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    وگرنه
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, اندازه(char_u *), 10);
    اگر (get_function_body(&eap, &newlines, سطر_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	برو erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_شمار += newlines.ga_len;
    اگر (gap->ga_itemsize > 0)
    {
	صحیح	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	برای (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    اگر (p == پوچ)
		// comment line in the lambda body
		ادامه;

	    p = skipwhite(p);

	    اگر (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		برو erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    اگر (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    وگرنه
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    اگر (pnl != پوچ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	اگر (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    برو erret;
	اگر (eap.nextcmd != پوچ)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	وگرنه
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	اگر (pnl != پوچ)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    اگر (eap.nextcmd != پوچ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	اگر (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    وگرنه
	*arg = (char_u *)"";

    اگر (!evaluate)
    {
	ret = OK;
	برو erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    اگر (ufunc == پوچ)
	برو erret;
    اگر (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	برو erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = پوچ;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = پوچ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_بالاترین;

    // parse argument types, the type variables of the function being
    // compiled may be used
    اگر (parse_argument_types(ufunc, argtypes, varargs, پوچ, پوچ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	برو erret;
    }

    // parse the return type, if any
    اگر (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	برو erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    اگر (pt == پوچ)
	برو erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_سطرها = newlines;
    newlines.ga_data = پوچ;
    اگر (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    اگر (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_بالاترین;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = پوچ;
    ret = OK;

erret:
    اگر (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    اگر (newargs != پوچ)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    اگر (ufunc != پوچ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    بازگشت ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    صحیح
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	صحیح	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = پوچ;
    partial_T   *pt = پوچ;
    صحیح		varargs;
    char_u	*ret_type = پوچ;
    صحیح		ret;
    char_u	*s;
    char_u	*start, *end;
    صحیح		*old_eval_lavars = eval_lavars_used;
    صحیح		eval_lavars = FALSE;
    char_u	*tofree2 = پوچ;
    صحیح		equal_arrow = **arg == '(';
    صحیح		white_error = FALSE;
    صحیح		called_emsg_start = called_emsg;
    صحیح		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    اگر (equal_arrow && !vim9script)
	بازگشت NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', پوچ,
	    types_optional ? &argtypes : پوچ, types_optional,
			types_optional ? &arg_objm : پوچ, evalarg,
			پوچ, &default_args, TRUE, پوچ, FALSE, پوچ, پوچ);
    اگر (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, پوچ) == پوچ)
    {
	اگر (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	بازگشت called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    اگر (evaluate)
	pnewargs = &newargs;
    وگرنه
	pnewargs = پوچ;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : پوچ, types_optional,
			types_optional ? &arg_objm : پوچ, evalarg,
					    &varargs, &default_args,
					    FALSE, پوچ, FALSE, پوچ, پوچ);
    اگر (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : پوچ)) == پوچ)
    {
	اگر (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	بازگشت white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    اگر (ret_type != پوچ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    اگر (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    اگر (equal_arrow && **arg == '{')
    {
	اگر (evalarg == پوچ)
	    // cannot happen?
	    برو theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	اگر (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : پوچ, varargs,
			   &default_args, ret_type) == FAIL)
	    برو errret;
	برو theend;
    }
    اگر (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_مقادیر_in_lambda));
	برو errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_عبارت_concatenate(arg, &start, &end, evalarg);
    اگر (ret == FAIL)
	برو errret;

    اگر (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg != '}')
	{
	    semsg(_(e_expected_راست_curly_str), *arg);
	    برو errret;
	}
	++*arg;
    }

    اگر (evaluate)
    {
	صحیح	    len;
	صحیح	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *سطر_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	اگر (fp == پوچ)
	    برو errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	اگر (pt == پوچ)
	    برو errret;

	ga_init2(&newlines, اندازه(char_u *), 1);
	اگر (ga_grow(&newlines, 1) == FAIL)
	    برو errret;

	// If there are line breaks, we need to split up the string.
	سطر_end = vim_strchr(start, '\n');
	اگر (سطر_end == پوچ || سطر_end > end)
	    سطر_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (صحیح)(سطر_end - start) + 1;
	p = alloc(len);
	اگر (p == پوچ)
	    برو errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, سطر_end - start);

	اگر (سطر_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    وقتی (سطر_end < end && *سطر_end == '\n')
	    {
		اگر (ga_grow(&newlines, 1) == FAIL)
		    برو errret;
		start = سطر_end + 1;
		سطر_end = vim_strchr(start, '\n');
		اگر (سطر_end == پوچ || سطر_end > end)
		    سطر_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, سطر_end - start);
	    }
	}

	اگر (strstr((نویسه *)p + 7, "a:") == پوچ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	اگر (types_optional)
	{
	    اگر (parse_argument_types(fp, &argtypes,
				vim9script && varargs, پوچ, پوچ, 0,
				cctx) == FAIL)
		برو errret;
	    اگر (ret_type != پوچ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     پوچ, cctx, TRUE);
		اگر (fp->uf_ret_type == پوچ)
		    برو errret;
	    }
	    وگرنه
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_سطرها = newlines;
	اگر (current_funccal != پوچ && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    اگر (register_closure(fp) == FAIL)
		برو errret;
	}

#ifdef FEAT_PROFILE
	اگر (prof_def_func())
	    func_do_profile(fp);
#endif
	اگر (sandbox)
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
    اگر (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    بازگشت OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    اگر (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    اگر (fp != پوچ)
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
    بازگشت FAIL;
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
	صحیح	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	صحیح	    no_autoload,
	صحیح	    new_function,
	صحیح	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = پوچ;
    صحیح		cc;
    char_u	*s = پوچ;
    hashtab_T	*ht;
    صحیح		did_type = FALSE;

    اگر (partialp != پوچ)
	*partialp = پوچ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    اگر (v != پوچ)
    {
	tv = &v->di_tv;
    }
    وگرنه اگر (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	صحیح	    len = *lenp;

	اگر (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	اگر (import != پوچ)
	{
	    name[len] = NUL;
	    اگر (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    وگرنه
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    بازگشت (char_u *)"";	// just in case
	}
    }

    اگر (tv != پوچ)
    {
	اگر (found_var != پوچ)
	    *found_var = TRUE;
	اگر (tv->v_type == VAR_FUNC)
	{
	    اگر (tv->vval.v_string == پوچ)
	    {
		*lenp = 0;
		بازگشت (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    اگر (p != پوچ && generic_func_find_open_bracket(name) != پوچ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		بازگشت (char_u *)"";
	    }
	    اگر (p != پوچ)
		*lenp = p - s;
	    وگرنه
		*lenp = (صحیح)STRLEN(s);
	}
	وگرنه اگر (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    اگر (pt == پوچ)
	    {
		*lenp = 0;
		بازگشت (char_u *)"";	// just in case
	    }
	    اگر (partialp != پوچ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (صحیح)STRLEN(s);
	}

	اگر (s != پوچ)
	{
	    اگر (!did_type && type != پوچ && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		اگر (sv != پوچ)
		    *type = sv->sv_type;
	    }
	    بازگشت s;
	}
    }

    بازگشت name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    تهی
emsg_funcname(نویسه *ermsg, char_u *name)
{
    char_u	*p = name;

    اگر (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    اگر (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    صحیح
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	صحیح	    partial_argc,
	typval_T    *argvars,
	صحیح	    *argcount,
	صحیح	    is_builtin)
{
    char_u	*argp = *arg;
    صحیح		ret = OK;
    صحیح		vim9script = in_vim9script();
    صحیح		evaluate = evalarg == پوچ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    وقتی (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	اگر (*argp == ')' || *argp == ',' || *argp == NUL)
	    بشکن;

	صحیح arg_idx = *argcount;
	اگر (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    بشکن;
	}
	++*argcount;
	اگر (!is_builtin && check_typval_is_مقدار(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    بشکن;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	اگر (vim9script)
	{
	    اگر (*argp != ',' && *skipwhite(argp) == ',')
	    {
		اگر (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		بشکن;
	    }
	}
	وگرنه
	    argp = skipwhite(argp);
	اگر (*argp != ',')
	    بشکن;
	اگر (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    اگر (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    بشکن;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    اگر (*argp == ')')
	++argp;
    وگرنه
	ret = FAIL;
    *arg = argp;
    بازگشت ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    صحیح
get_func_tv(
    char_u	*name,		// name of the function
    صحیح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    صحیح		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    صحیح		argcount = 0;			// number of arguments found
    صحیح		vim9script = in_vim9script();
    صحیح		evaluate = evalarg == پوچ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == پوچ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    اگر (ret == OK)
    {
	صحیح	i = 0;
	صحیح	did_emsg_before = did_emsg;

	اگر (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    اگر (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, اندازه(typval_T *), 50);
	    برای (i = 0; i < argcount; ++i)
		اگر (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	اگر (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    وگرنه اگر (!aborting() && evaluate)
    {
	اگر (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	وگرنه
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    وقتی (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    اگر (vim9script)
	*arg = argp;
    وگرنه
	*arg = skipwhite(argp);
    بازگشت ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ایستا صحیح
eval_fname_sid(char_u *p)
{
    بازگشت (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    اگر (script_name == name)
	بازگشت name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (صحیح)KE_SNR;
    fname_buflen = 3;
    اگر (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    وگرنه
    {
	اگر (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	وگرنه
	{
	    fname_buflen += vim_snprintf((نویسه *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    اگر (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    وگرنه
    {
	fname = alloc(fnamelen + 1);
	اگر (fname == پوچ)
	    *error = FCERR_OTHER;
	وگرنه
	{
	    *tofree = fname;
	    vim_snprintf((نویسه *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    بازگشت fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    تهی
func_name_with_sid(char_u *name, صحیح sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (صحیح)KE_SNR;
    vim_snprintf((نویسه *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ایستا ufunc_T *
find_func_with_sid(char_u *name, صحیح sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    اگر (!SCRIPT_ID_VALID(sid))
	بازگشت پوچ;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    اگر (!HASHITEM_EMPTY(hi))
	بازگشت HI2UF(hi);
    بازگشت پوچ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ایستا ufunc_T *
find_func_with_prefix(char_u *name, صحیح sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    اگر (vim_strchr(name, AUTOLOAD_CHAR) != پوچ)
	بازگشت پوچ;	// already has the prefix
    اگر (!SCRIPT_ID_VALID(sid))
	بازگشت پوچ;	// not in a script
    si = SCRIPT_ITEM(sid);
    اگر (si->sn_autoload_prefix != پوچ)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	اگر (namep == پوچ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	اگر (len < اندازه(buffer))
	    auto_name = buffer;
	وگرنه
	    auto_name = alloc(len);
	اگر (auto_name != پوچ)
	{
	    vim_snprintf((نویسه *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    اگر (auto_name != buffer)
		vim_free(auto_name);
	    اگر (!HASHITEM_EMPTY(hi))
		بازگشت HI2UF(hi);
	}
    }

    بازگشت پوچ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, صحیح flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    اگر ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	اگر (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    اگر (func != پوچ)
		بازگشت func;
	}
	اگر (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    اگر (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (صحیح)sid);
		اگر (func != پوچ)
		    بازگشت func;
	    }
	}
    }

    اگر ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	اگر (!HASHITEM_EMPTY(hi))
	    بازگشت HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    بازگشت find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, صحیح is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    اگر (fp != پوچ && (fp->uf_flags & FC_DEAD) == 0)
	بازگشت fp;
    بازگشت پوچ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    صحیح
func_is_global(ufunc_T *ufunc)
{
    بازگشت ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    صحیح
func_requires_g_prefix(ufunc_T *ufunc)
{
    بازگشت func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == پوچ
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ایستا صحیح
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    صحیح	len;

    اگر (!func_is_global(fp))
	len = vim_snprintf((نویسه *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    وگرنه
	len = vim_snprintf((نویسه *)buf, bufsize, "%s", fp->uf_name);

    بازگشت (len >= (صحیح)bufsize) ? (صحیح)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ایستا تهی
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    نویسه	*name,
    varnumber_T nr)
{
    STRCPY(v->di_کلید, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_عدد;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_عدد = nr;
}

/*
 * Free "fc".
 */
    ایستا تهی
free_funccal(funccall_T *fc)
{
    صحیح	i;

    برای (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	اگر (fp != پوچ && fp->uf_scoped == fc)
	    fp->uf_scoped = پوچ;
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
   ایستا تهی
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_اقلام(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    ایستا تهی
cleanup_function_call(funccall_T *fc)
{
    صحیح	may_free_fc = fc->fc_refcount <= 0;
    صحیح	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    اگر (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    وگرنه
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    اگر (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    وگرنه
    {
	صحیح	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (صحیح)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_اقلام(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    اگر (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = پوچ;
    وگرنه
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_اقلام(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    اگر (free_fc)
	free_funccal(fc);
    وگرنه
    {
	ایستا صحیح made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	اگر (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	وگرنه اگر (++made_copy >= (صحیح)((4096 * 1024) / اندازه(*fc)))
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
    ایستا صحیح
numbered_function(char_u *name)
{
    بازگشت SAFE_isdigit(*name)
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
    صحیح
func_name_refcount(char_u *name)
{
    بازگشت numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ایستا تهی
funccal_unref(funccall_T *fc, ufunc_T *fp, صحیح force)
{
    funccall_T	**pfc;
    صحیح		i;

    اگر (fc == پوچ)
	بازگشت;

    اگر (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	برای (pfc = &previous_funccal; *pfc != پوچ; pfc = &(*pfc)->fc_caller)
	{
	    اگر (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		بازگشت;
	    }
	}
    برای (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	اگر (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = پوچ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ایستا صحیح
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    اگر (fp->uf_flags & FC_DEAD)
	بازگشت FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    اگر (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	بازگشت FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    اگر (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	بازگشت FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    بازگشت TRUE;
}

    ایستا تهی
func_clear_اقلام(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_سطرها));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    اگر (IS_GENERIC_FUNC(fp))
	generic_func_clear_اقلام(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = پوچ;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    اگر (fp->uf_cb_free != پوچ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = پوچ;
    }

    fp->uf_cb_state = پوچ;
    fp->uf_cb = پوچ;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_شمار);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ایستا تهی
func_clear(ufunc_T *fp, صحیح force)
{
    اگر (fp->uf_cleared)
	بازگشت;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_اقلام(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    ایستا صحیح
func_free(ufunc_T *fp, صحیح force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    اگر ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    اگر ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	اگر (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	بازگشت OK;
    }
    بازگشت FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    تهی
func_clear_free(ufunc_T *fp, صحیح force)
{
    func_clear(fp, force);
    اگر (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    وگرنه
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    صحیح
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = پوچ;

    اگر (ufunc == پوچ)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	بازگشت FAIL;
    }

    fp = find_func(global, TRUE);
    اگر (fp != پوچ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	بازگشت FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    اگر (fp == پوچ)
	بازگشت FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    اگر (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_سطرها, &fp->uf_سطرها) == FAIL)
	برو failed;

    اگر (ufunc->uf_arg_types != پوچ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	اگر (fp->uf_arg_types == پوچ)
	    برو failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    اندازه(type_T *) * fp->uf_args.ga_len);
    }
    اگر (ufunc->uf_va_name != پوچ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	اگر (fp->uf_va_name == پوچ)
	    برو failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    اگر ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == پوچ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	اگر (pt == پوچ)
	    برو failed;
	اگر (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    برو failed;
	}
	ufunc->uf_partial = pt;
    }

    بازگشت OK;

failed:
    func_clear_free(fp, TRUE);
    بازگشت FAIL;
}

ایستا صحیح	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    صحیح
funcdepth_increment(تهی)
{
    اگر (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	بازگشت FAIL;
    }
    ++funcdepth;
    بازگشت OK;
}

    تهی
funcdepth_decrement(تهی)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    صحیح
funcdepth_get(تهی)
{
    بازگشت funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    تهی
funcdepth_restore(صحیح depth)
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

    اگر (fc == پوچ)
	بازگشت پوچ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    بازگشت fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    تهی
remove_funccal(تهی)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ایستا funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    صحیح		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    صحیح		using_sandbox = FALSE;
    صحیح		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    صحیح		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    صحیح		default_arg_err = FALSE;
    dictitem_T	*v;
    صحیح		fixvar_idx = 0;	// index in fc_fixvar[]
    صحیح		i;
    صحیح		ai;
    صحیح		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    صحیح		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    اگر (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = -1;
	بازگشت FCERR_FAILED;
    }

    سطر_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    اگر (fc == پوچ)
	بازگشت FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, اندازه(ufunc_T *), 1);

    اگر (fp->uf_def_status != UF_NOT_COMPILED)
    {
	اگر (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == پوچ ? پوچ : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	اگر (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	اگر (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	اگر (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	اگر (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != پوچ && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	بازگشت retval;
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
    اگر (selfdict != پوچ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
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
    اگر ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    اگر ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
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
    اگر ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    برای (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	صحیح	    addlocal = FALSE;
	typval_T    def_rettv;
	صحیح	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	اگر (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    اگر (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_عدد == VVAL_NONE));
	    اگر (isdefault)
	    {
		char_u	    *default_عبارت = پوچ;

		def_rettv.v_type = VAR_عدد;
		def_rettv.vval.v_عدد = -1;

		default_عبارت = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		اگر (eval1(&default_عبارت, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    بشکن;
		}
	    }

	    namelen = STRLEN(name);
	}
	وگرنه
	{
	    اگر ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		بشکن;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((نویسه *)numbuf, اندازه(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	اگر (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_کلید, name);
	}
	وگرنه
	{
	    v = dictitem_alloc(name);
	    اگر (v == پوچ)
		بشکن;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	اگر (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	اگر (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	وگرنه
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	اگر (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    اگر (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_افزودن_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    اگر (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	اگر (p_verbose >= 14)
	{
	    char_u	buf[پیام_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    پیام_puts("(");
	    برای (i = 0; i < argcount; ++i)
	    {
		اگر (i > 0)
		    پیام_puts(", ");
		اگر (argvars[i].v_type == VAR_عدد)
		    پیام_outnum((long)argvars[i].vval.v_عدد);
		وگرنه
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    اگر (s != پوچ)
		    {
			اگر (vim_strsize(s) > پیام_BUF_CLEN)
			{
			    trunc_string(s, buf, پیام_BUF_CLEN, پیام_BUF_LEN);
			    s = buf;
			}
			پیام_puts((نویسه *)s);
			vim_free(tofree);
		    }
		}
	    }
	    پیام_puts(")");
	}
	پیام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == پوچ ? پوچ : fc->fc_caller->fc_func);
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

    اگر (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    وگرنه اگر (islambda)
    {
	char_u *p = *(char_u **)fp->uf_سطرها.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(تهی)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    وگرنه
	// call do_cmdline() to execute the lines
	do_cmdline(پوچ, get_func_سطر, (تهی *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    اگر ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	اگر (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == پوچ ? پوچ : fc->fc_caller->fc_func;

	اگر (fp->uf_profiling || (caller != پوچ && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    اگر (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	اگر (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	وگرنه اگر (fc->fc_rettv->v_type == VAR_عدد)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_عدد);
	وگرنه
	{
	    char_u	buf[پیام_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    اگر (s != پوچ)
	    {
		اگر (vim_strsize(s) > پیام_BUF_CLEN)
		{
		    trunc_string(s, buf, پیام_BUF_CLEN, پیام_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	پیام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_برداشت();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    اگر (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    اگر (p_verbose >= 12 && SOURCING_NAME != پوچ)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	پیام_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    برای (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    بازگشت retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, صحیح argcount)
{
    صحیح regular_args = fp->uf_args.ga_len;

    اگر (argcount < regular_args - fp->uf_def_args.ga_len)
	بازگشت FCERR_TOOFEW;
    وگرنه اگر (!has_varargs(fp) && argcount > regular_args)
	بازگشت FCERR_TOOMANY;
    بازگشت FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	صحیح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    اگر (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	بازگشت (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    اگر (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != پوچ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    اگر (error != FCERR_UNKNOWN)
	بازگشت error;

    اگر ((fp->uf_flags & FC_DICT) && selfdict == پوچ)
    {
	error = FCERR_DICT;
    }
    وگرنه
    {
	صحیح		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_جستجو_patterns();
	اگر (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : پوچ);
	اگر (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	اگر (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_جستجو_patterns();
    }

    بازگشت error;
}

ایستا funccal_entry_T *funccal_پشته = پوچ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    تهی
save_funccal(funccal_entry_T *entry)
{
    entry->بالاترین_funccal = current_funccal;
    entry->next = funccal_پشته;
    funccal_پشته = entry;
    current_funccal = پوچ;
}

    تهی
restore_funccal(تهی)
{
    اگر (funccal_پشته == پوچ)
	internal_error("restore_funccal()");
    وگرنه
    {
	current_funccal = funccal_پشته->بالاترین_funccal;
	funccal_پشته = funccal_پشته->next;
    }
}

    funccall_T *
get_current_funccal(تهی)
{
    بازگشت current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    صحیح
at_script_level(تهی)
{
    بازگشت current_funccal == پوچ && autocmd_match == پوچ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    تهی
delete_script_functions(صحیح sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (صحیح)KE_SNR;
    len = 3 + vim_snprintf((نویسه *)buf + 3, اندازه(buf) - 3, "%d_", sid);

    وقتی (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		اگر (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    صحیح changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    اگر (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			اگر (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    وگرنه
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			اگر (changed != func_hashtab.ht_changed)
			    بشکن;
		    }
		}
		--todo;
	    }
    }
}

#اگر defined(EXITFREE)
    تهی
free_all_functions(تهی)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    صحیح		changed;

    // Clean up the current_funccal chain and the funccal stack.
    وقتی (current_funccal != پوچ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	اگر (current_funccal == پوچ && funccal_پشته != پوچ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    وقتی (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		اگر (func_name_refcount(fp->uf_name))
		    ++skipped;
		وگرنه
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    اگر (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			بشکن;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    وقتی (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		اگر (func_name_refcount(fp->uf_name))
		    ++skipped;
		وگرنه
		{
		    اگر (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			بشکن;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    اگر (skipped == 0)
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
    صحیح
builtin_function(char_u *name, صحیح len)
{
    صحیح i;

    اگر (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	بازگشت FALSE;
    برای (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	اگر (name[i] == AUTOLOAD_CHAR)
	    بازگشت FALSE;
	اگر (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    اگر (name[i] == '.')
		بازگشت FALSE;
	    بشکن;
	}
    }
    بازگشت TRUE;
}

    صحیح
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
    صحیح		argc = 0;
    صحیح		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, item)
    {
	اگر (argc == MAX_FUNC_ARGS - (partial == پوچ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    بشکن;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    اگر (item == پوچ)
    {
	funcexe_T	funcexe;
	صحیح		namelen = -1;

	اگر (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    اگر (p != پوچ)
	    {
		اگر (skip_generic_func_type_args(&p) == FAIL)
		    برو done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	اگر (partial != پوچ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    اگر (funcexe.fe_object != پوچ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    وقتی (argc > 0)
	clear_tv(&argv[--argc]);

done:
    بازگشت r;
}

ایستا صحیح callback_depth = 0;

    صحیح
get_callback_depth(تهی)
{
    بازگشت callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    صحیح
call_callback(
    callback_T	*callback,
    صحیح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    صحیح		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    صحیح		ret;

    اگر (callback->cb_name == پوچ || *callback->cb_name == NUL)
	بازگشت FAIL;

    اگر (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	بازگشت FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    اگر (callback->cb_partial != پوچ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	اگر (funcexe.fe_object != پوچ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    اگر (need_rethrow && current_exception != پوچ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    بازگشت ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    صحیح		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    اگر (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	بازگشت -2;

    retval = tv_get_عدد_chk(&rettv, پوچ);
    clear_tv(&rettv);
    بازگشت retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    تهی
user_func_error(funcerror_T error, char_u *name, صحیح found_var)
{
    انتخاب (error)
    {
	حالت FCERR_UNKNOWN:
		اگر (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		وگرنه
		    emsg_funcname(e_unknown_function_str, name);
		بشکن;
	حالت FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		بشکن;
	حالت FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		بشکن;
	حالت FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		بشکن;
	حالت FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		بشکن;
	حالت FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		بشکن;
	حالت FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		بشکن;
	حالت FCERR_OTHER:
	حالت FCERR_FAILED:
		// assume the error message was already given
		بشکن;
	حالت FCERR_NONE:
		بشکن;
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
    ایستا funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	صحیح	    argcount,
	صحیح	    base_included,
	char_u	    *name)
{
    اگر (funcexe->fe_check_type != پوچ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	اگر (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? پوچ : funcexe->fe_basetv,
			  name) == FAIL)
	    بازگشت FCERR_OTHER;
    }
    بازگشت FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    صحیح
call_func(
    char_u	*funcname,	// name of the function
    صحیح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    صحیح		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    صحیح		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    صحیح		i;
    ufunc_T	*fp = پوچ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = پوچ;
    char_u	*fname = پوچ;
    char_u	*name = پوچ;
    صحیح		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    صحیح		argv_clear = 0;
    صحیح		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    اگر (partial != پوچ)
	fp = partial->pt_func;
    اگر (fp == پوچ)
	fp = funcexe->fe_ufunc;

    اگر (fp == پوچ)
    {
	اگر (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    اگر (p != پوچ)
	    {
		len = p - funcname;
		اگر (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == پوچ)
		    برو theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	اگر (name == پوچ)
	    برو theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    اگر (funcexe->fe_doesrange != پوچ)
	*funcexe->fe_doesrange = FALSE;

    اگر (partial != پوچ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	اگر (partial->pt_dict != پوچ && (selfdict == پوچ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	اگر (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    برای (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		اگر (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    برو theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    برای (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    اگر (funcexe->fe_check_type != پوچ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		برای (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    اگر (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != پوچ) ? name : funcname);

    اگر (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	صحیح	is_global = FALSE;

	// Skip "g:" before a function name.
	اگر (fp == پوچ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_عدد;	// default rettv is number zero
	rettv->vval.v_عدد = 0;
	error = FCERR_UNKNOWN;

	اگر (fp != پوچ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    اگر (fp == پوچ)
	    {
		fp = find_func(rfname, is_global);
		اگر (fp != پوچ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = پوچ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    اگر (fp == پوچ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, پوچ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    اگر (fp == پوچ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    اگر (fp == پوچ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		اگر (p != پوچ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    اگر (partial == پوچ && fp != پوچ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		اگر (fp == پوچ)
		{
		    error = FCERR_FAILED;
		    برو theend;
		}
	    }
	    وگرنه اگر (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != پوچ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		برو theend;
	    }

	    اگر (fp != پوچ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    وگرنه اگر (fp != پوچ)
	    {
		صحیح need_arg_check = FALSE;
		اگر (funcexe->fe_check_type == پوچ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		اگر (funcexe->fe_argv_func != پوچ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		اگر (funcexe->fe_basetv != پوچ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, اندازه(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		اگر (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != پوچ) ? name : funcname);

		اگر (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	وگرنه اگر (funcexe->fe_basetv != پوچ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	وگرنه
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
    اگر (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    اگر (!aborting())
	user_func_error(error, (name != پوچ) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    وقتی (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    بازگشت ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    صحیح
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    صحیح		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = پوچ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    صحیح		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_عدد;	// default rettv is number zero
    rettv->vval.v_عدد = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    اگر (name == پوچ)
	بازگشت ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    اگر (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    وگرنه
	rfname = fname;
    fp = find_func(rfname, is_global);
    اگر (fp != پوچ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = پوچ;
    اگر (fp == پوچ)
	ret = NOTDONE;
    وگرنه اگر (fp != پوچ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    وگرنه اگر (fp != پوچ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, پوچ);
	اگر (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    بازگشت ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    بازگشت fp->uf_name_exp != پوچ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ایستا صحیح
function_list_modified(صحیح prev_ht_changed)
{
    اگر (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ایستا صحیح
list_func_head(ufunc_T *fp, صحیح indent)
{
    صحیح		prev_ht_changed = func_hashtab.ht_changed;
    صحیح		j;

    پیام_start();

    // a timer at the more prompt may have deleted the function
    اگر (function_list_modified(prev_ht_changed))
	بازگشت FAIL;

    اگر (indent)
	پیام_puts("   ");
    اگر (fp->uf_def_status != UF_NOT_COMPILED)
	پیام_puts("def ");
    وگرنه
	پیام_puts("function ");
    پیام_puts((نویسه *)printable_func_name(fp));
    پیام_putchar('(');
    برای (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	اگر (j)
	    پیام_puts(", ");
	پیام_puts((نویسه *)FUNCARG(fp, j));
	اگر (fp->uf_arg_types != پوچ)
	{
	    نویسه *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	اگر (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    پیام_puts(" = ");
	    پیام_puts(((نویسه **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    اگر (fp->uf_varargs)
    {
	اگر (j)
	    پیام_puts(", ");
	پیام_puts("...");
    }
    اگر (fp->uf_va_name != پوچ)
    {
	اگر (!fp->uf_varargs)
	{
	    اگر (j)
		پیام_puts(", ");
	    پیام_puts("...");
	}
	پیام_puts((نویسه *)fp->uf_va_name);
	اگر (fp->uf_va_type != پوچ)
	{
	    نویسه *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    پیام_putchar(')');

    اگر (fp->uf_def_status != UF_NOT_COMPILED)
    {
	اگر (fp->uf_ret_type != &t_void)
	{
	    نویسه *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    وگرنه اگر (fp->uf_flags & FC_ABORT)
	پیام_puts(" abort");
    اگر (fp->uf_flags & FC_RANGE)
	پیام_puts(" range");
    اگر (fp->uf_flags & FC_DICT)
	پیام_puts(" dict");
    اگر (fp->uf_flags & FC_CLOSURE)
	پیام_puts(" closure");
    پیام_clr_eos();
    اگر (p_verbose > 0)
	last_set_پیام(fp->uf_script_ctx);

    بازگشت OK;
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
    صحیح		*is_global,
    صحیح		skip,		// only find the end, don't evaluate
    صحیح		flags)
{
    بازگشت trans_function_name_ext(pp, is_global, skip, flags,
	    پوچ, پوچ, پوچ, پوچ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ایستا char_u *
trans_function_name_ext(
    char_u	**pp,
    صحیح		*is_global,
    صحیح		skip,		// only find the end, don't evaluate
    صحیح		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = پوچ;
    char_u	*start;
    char_u	*end;
    صحیح		lead;
    char_u	sid_buf[20];
    صحیح		len;
    صحیح		extra = 0;
    صحیح		prefix_g = FALSE;
    lval_T	lv;
    صحیح		vim9script = in_vim9script();
    صحیح		vim9_local;

    اگر (fdp != پوچ)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    اگر ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (صحیح)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	بازگشت vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    اگر (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, پوچ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    اگر (end == start || (vim9script && end != پوچ
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	اگر (!skip)
	    emsg(_(e_function_name_required));
	برو theend;
    }
    اگر (end == پوچ || (lv.ll_tv != پوچ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	اگر (!aborting())
	{
	    اگر (end != پوچ)
		semsg(_(e_invalid_argument_str), start);
	}
	وگرنه
	    *pp = find_name_end(start, پوچ, پوچ, FNE_INCL_BR);
	برو theend;
    }

    اگر (lv.ll_ufunc != پوچ)
    {
	اگر (ufunc != پوچ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	برو theend;
    }

    اگر (lv.ll_tv != پوچ)
    {
	اگر (fdp != پوچ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = پوچ;
	    fdp->fd_di = lv.ll_di;
	}
	اگر (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != پوچ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	وگرنه اگر (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != پوچ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	وگرنه اگر (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != پوچ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    اگر (partial != پوچ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	وگرنه
	{
	    اگر (!skip && !(flags & TFN_QUIET) && (fdp == پوچ
			     || lv.ll_dict == پوچ || fdp->fd_newkey == پوچ))
		emsg(_(e_funcref_required));
	    وگرنه
		*pp = end;
	    name = پوچ;
	}
	برو theend;
    }

    اگر (lv.ll_name == پوچ)
    {
	// Error found, but continue after the function name.
	*pp = end;
	برو theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    اگر (lv.ll_exp_name != پوچ)
    {
	len = (صحیح)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, پوچ);
	اگر (name == lv.ll_exp_name)
	    name = پوچ;
    }
    وگرنه اگر (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	صحیح		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	اگر (si->sn_autoload_prefix != پوچ)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	وگرنه
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (صحیح)KE_SNR;
	    vim_snprintf((نویسه *)sid_buf + 3, اندازه(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	برو theend;
    }
    وگرنه اگر (!(flags & TFN_NO_DEREF))
    {
	len = (صحیح)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, پوچ);
	اگر (name == *pp)
	    name = پوچ;
    }
    اگر (name != پوچ)
    {
	name = vim_strsave(name);
	*pp = end;
	اگر (name != پوچ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحیح)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	برو theend;
    }

    اگر (lv.ll_exp_name != پوچ)
    {
	len = (صحیح)STRLEN(lv.ll_exp_name);
	اگر (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    وگرنه
    {
	// skip over "s:" and "g:"
	اگر (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    اگر (lv.ll_name[0] == 'g')
	    {
		اگر (is_global != پوچ)
		{
		    *is_global = TRUE;
		}
		وگرنه
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (صحیح)(end - lv.ll_name);
    }
    اگر (len <= 0)
    {
	اگر (!skip)
	    emsg(_(e_function_name_required));
	برو theend;
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
    اگر (skip)
	lead = 0;	// do nothing
    وگرنه اگر (lead > 0 || vim9_local)
    {
	اگر (!vim9_local)
	{
	    اگر (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		برو theend;
	    }
	    lead = 3;
	}
	اگر (vim9_local || (lv.ll_exp_name != پوچ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    اگر (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		برو theend;
	    }
	    sid_buflen = vim_snprintf((نویسه *)sid_buf, اندازه(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    اگر (vim9_local)
		extra = 3 + (صحیح)sid_buflen;
	    وگرنه
		lead += (صحیح)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    وگرنه اگر (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	برو theend;
    }
    اگر (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	اگر (cp != پوچ && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    برو theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    اگر (name != پوچ)
    {
	اگر (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحیح)KE_SNR;
	    اگر (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	وگرنه اگر (prefix_g)
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
    بازگشت name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ایستا char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    اگر (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	اگر (p != پوچ)
	    بازگشت p + 1;
    }
    بازگشت پوچ;
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
    نویسه	sid_buf[25];
    size_t	sid_buflen;
    صحیح		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    اگر (funcname == پوچ)
	بازگشت پوچ;

    اگر (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	اگر (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    بازگشت پوچ;
	ufunc = find_func(funcname, FALSE);
	اگر (ufunc == پوچ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == پوچ)
	    بازگشت پوچ;
	++p;
	off = 0;
    }
    وگرنه
	off = *funcname == 's' ? 2 : 5;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	بازگشت پوچ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, اندازه(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    اگر (newname == پوچ)
	بازگشت پوچ;
    vim_snprintf((نویسه *)newname, newnamesize, "%s%s", sid_buf, p + off);

    بازگشت newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    اگر (n != پوچ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    بازگشت n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	صحیح	    *is_global,
	صحیح	    skip,
	صحیح	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    اگر (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(تهی)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	اگر (fudi != پوچ)
	    CLEAR_POINTER(fudi);
    }
    وگرنه
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, پوچ, پوچ, پوچ);
    *name = p;
    بازگشت saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    تهی
list_functions(regmatch_T *regmatch)
{
    صحیح		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    برای (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    اگر ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == پوچ
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		اگر (list_func_head(fp, FALSE) == FAIL)
		    بازگشت;
		اگر (function_list_modified(prev_ht_changed))
		    بازگشت;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ایستا char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    اگر (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	اگر (regmatch.regprog != پوچ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    اگر (*p == '/')
	++p;

    بازگشت p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ایستا ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, صحیح is_global)
{
    ufunc_T	*fp = پوچ;
    صحیح		j;

    اگر (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	بازگشت پوچ;
    }

    set_nextcmd(eap, p);

    اگر (eap->nextcmd != پوچ)
	*p = NUL;

    اگر (eap->skip || got_int)
	بازگشت پوچ;

    fp = find_func(name, is_global);
    اگر (fp == پوچ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	اگر (up != پوچ)
	    fp = find_func(up, FALSE);
    }

    اگر (fp == پوچ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	بازگشت پوچ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    صحیح prev_ht_changed = func_hashtab.ht_changed;

    اگر (list_func_head(fp, TRUE) != OK)
	بازگشت fp;

    برای (j = 0; j < fp->uf_سطرها.ga_len && !got_int; ++j)
    {
	اگر (FUNCLINE(fp, j) == پوچ)
	    ادامه;
	پیام_putchar('\n');
	پیام_outnum((long)(j + 1));
	اگر (j < 9)
	    پیام_putchar(' ');
	اگر (j < 99)
	    پیام_putchar(' ');
	اگر (function_list_modified(prev_ht_changed))
	    بشکن;
	پیام_prt_سطر(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    اگر (!got_int)
    {
	پیام_putchar('\n');
	اگر (!function_list_modified(prev_ht_changed))
	{
	    اگر (fp->uf_def_status != UF_NOT_COMPILED)
		پیام_puts("   enddef");
	    وگرنه
		پیام_puts("   endfunction");
	}
    }

    بازگشت fp;
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
	garray_T    *سطرها_to_free,
	صحیح	    class_flags,
	ocmember_T  *obj_members,
	صحیح         obj_member_شمار,
	cctx_T	    *cctx)
{
    صحیح		j;
    صحیح		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    صحیح		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*سطر_arg = پوچ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    صحیح		varargs = FALSE;
    صحیح		flags = 0;
    char_u	*ret_type = پوچ;
    ufunc_T	*fp = پوچ;
    صحیح		fp_allocated = FALSE;
    صحیح		free_fp = FALSE;
    صحیح		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ایستا صحیح	func_nr = 0;	    // number for nameless function
    صحیح		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_بالاترین;
    صحیح		vim9script = in_vim9script();
    imported_T	*import = پوچ;

    // ":function" without argument: list functions.
    اگر (ends_excmd2(eap->cmd, eap->arg))
    {
	اگر (!eap->skip)
	    list_functions(پوچ);
	set_nextcmd(eap, eap->arg);
	بازگشت پوچ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    اگر (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	بازگشت پوچ;
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
    اگر (name_arg != پوچ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    وگرنه
    {
	اگر (vim9script)
	{
	    اگر (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		بازگشت پوچ;
	    }
	    p = to_name_end(p, TRUE);
	    اگر (*skipwhite(p) == '.' && vim_strchr(p, '(') != پوچ)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		بازگشت پوچ;
	    }

	    p = eap->arg;
	}

	صحیح tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != پوچ);
	اگر (name == پوچ && (fudi.fd_dict == پوچ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    اگر (!aborting())
	    {
		اگر (!eap->skip && fudi.fd_newkey != پوچ)
		    semsg(_(e_کلید_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		بازگشت پوچ;
	    }
	    وگرنه
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	اگر (is_export && name != پوچ)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    اگر (prefixed != پوچ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	وگرنه اگر (paren && vim9script && name != پوچ
				    && vim_strchr(name, AUTOLOAD_CHAR) != پوچ)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    برو ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    اگر (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	برو ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    اگر (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	اگر (p == پوچ)
	    برو ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    اگر (*p != '(')
    {
	اگر (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    برو ret_free;
	}
	// attempt to continue by skipping some text
	اگر (vim_strchr(p, '(') != پوچ)
	    p = vim_strchr(p, '(');
    }

    اگر ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	برو ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    اگر (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	برو ret_free;
    }

    ga_init2(&newlines, اندازه(char_u *), 10);

    اگر (!eap->skip && name_arg == پوچ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	اگر (name != پوچ)
	    arg = name;
	وگرنه
	    arg = fudi.fd_newkey;
	اگر (arg != پوچ && (fudi.fd_di == پوچ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    صحیح	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    اگر (arg != fudi.fd_newkey)
	    {
		اگر (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    اگر (name_base == پوچ)
			name_base = arg + 3;
		    وگرنه
			++name_base;
		}
		برای (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		اگر (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    برو ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		اگر (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, پوچ,
			پوچ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    برو ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	اگر (fudi.fd_dict != پوچ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    برو ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    اگر (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : پوچ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : پوچ,
			 پوچ, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, سطرها_to_free) == FAIL)
	برو errret_2;
    whitep = p;

    اگر (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	اگر (*skipwhite(p) == ':')
	{
	    اگر (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    وگرنه اگر (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    اگر (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    وگرنه
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = پوچ;
	    }
	}
	p = skipwhite(p);
    }
    وگرنه
	// find extra arguments "range", "dict", "abort" and "closure"
	برای (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    اگر (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    وگرنه اگر (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    وگرنه اگر (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    وگرنه اگر (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		اگر (current_funccal == پوچ)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_بالاترین_level_str,
			    name == پوچ ? (char_u *)"" : name);
		    برو erret;
		}
	    }
	    وگرنه
		بشکن;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    اگر (*p == '\n')
	سطر_arg = p + 1;
    وگرنه اگر (*p != NUL
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
    اگر (کلید_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	اگر (!eap->skip && !eap->forceit)
	{
	    اگر (fudi.fd_dict != پوچ && fudi.fd_newkey == پوچ)
		emsg(_(e_dictionary_entry_already_exists));
	    وگرنه اگر (name != پوچ && find_func(name, is_global) != پوچ)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	اگر (!eap->skip && did_emsg)
	    برو erret;

	پیام_putchar('\n');	    // don't overwrite the function name
	cmdline_row = پیام_row;
    }

    // Save the starting line number.
    sourcing_lnum_بالاترین = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    اگر (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, سطر_arg, سطرها_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	برو erret;

    /*
     * If there are no errors, add the function
     */
    اگر (fudi.fd_dict != پوچ)
    {
	نویسه	numbuf[NUMBUFLEN];

	fp = پوچ;
	اگر (fudi.fd_newkey == پوچ && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    برو erret;
	}
	اگر (fudi.fd_di == پوچ)
	{
	    // Can't add a function to a locked dictionary
	    اگر (مقدار_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		برو erret;
	}
	    // Can't change an existing function if it is locked
	وگرنه اگر (مقدار_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    برو erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, اندازه(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	اگر (name == پوچ)
	    برو erret;
    }
    وگرنه اگر (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	صحیح		var_conflict = FALSE;
	صحیح		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	اگر (v != پوچ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    اگر (si->sn_autoload_prefix != پوچ)
	    {
		اگر (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    اگر (v != پوچ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		وگرنه
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    اگر (prefixed != پوچ && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			اگر (v != پوچ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	اگر (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    برو erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	اگر (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == پوچ ? name : uname, 0, FALSE);
	}

	اگر (fp != پوچ || import != پوچ)
	{
	    صحیح dead = fp != پوچ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    اگر (import != پوچ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_بالاترین;
		اگر (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		وگرنه
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		برو errret_keep;
	    }
	    اگر (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		برو errret_keep;
	    }
	    اگر (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = پوچ;
		overwrite = TRUE;
	    }
	    وگرنه
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = پوچ;
		func_clear_اقلام(fp);
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

    اگر (fp == پوچ)
    {
	اگر (fudi.fd_dict == پوچ && vim_strchr(name, AUTOLOAD_CHAR) != پوچ)
	{
	    صحیح	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    اگر (SOURCING_NAME != پوچ)
	    {
		scriptname = autoload_name(name);
		اگر (scriptname != پوچ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (صحیح)STRLEN(p);
		    slen = (صحیح)STRLEN(SOURCING_NAME);
		    اگر (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    اگر (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_بالاترین;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		برو erret;
	    }
	}

	اگر (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	اگر (fp == پوچ)
	    برو erret;
	fp_allocated = TRUE;

	اگر (fudi.fd_dict != پوچ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    اگر (func_name == پوچ)
	    {
		VIM_CLEAR(fp);
		برو erret;
	    }
	    اگر (fudi.fd_di == پوچ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		اگر (fudi.fd_di == پوچ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    برو erret;
		}
		اگر (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    برو erret;
		}
	    }
	    وگرنه
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

    اگر (eap->cmdidx == CMD_def)
    {
	صحیح	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	اگر (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_بالاترین;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	صحیح save_is_export = is_export;
	is_export = FALSE;

	اگر (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_شمار, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    برو erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	اگر (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    برو erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    وگرنه
	fp->uf_def_status = UF_NOT_COMPILED;

    اگر (fp_allocated)
    {
	// insert the new function in the function list
	اگر (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_کلید = UF2HIKEY(fp);
	}
	وگرنه اگر (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    برو erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_سطرها = newlines;
    newlines.ga_data = پوچ;
    اگر ((flags & FC_CLOSURE) != 0)
    {
	اگر (register_closure(fp) == FAIL)
	    برو erret;
    }
    وگرنه
	fp->uf_scoped = پوچ;

#ifdef FEAT_PROFILE
    اگر (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    اگر (sandbox)
	flags |= FC_SANDBOX;
    اگر (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_بالاترین;
    اگر (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    اگر (eap->cmdidx == CMD_def)
	set_function_type(fp);
    وگرنه اگر (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    اگر (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, پوچ);

    برو ret_free;

erret:
    اگر (fp != پوچ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    اگر (fp != پوچ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	اگر (IS_GENERIC_FUNC(fp))
	    generic_func_clear_اقلام(fp);
    }
    اگر (free_fp)
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
    اگر (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    بازگشت fp;
}

/*
 * ":function"
 */
    تهی
ex_function(exarg_T *eap)
{
    garray_T سطرها_to_free;

    ga_init2(&سطرها_to_free, اندازه(char_u *), 50);
    (تهی)define_function(eap, پوچ, &سطرها_to_free, 0, پوچ, 0, پوچ);
    ga_clear_strings(&سطرها_to_free);
}

    صحیح
get_func_arity(char_u *name, صحیح *required, صحیح *optional, صحیح *varargs)
{
    ufunc_T	*ufunc = پوچ;
    صحیح		argcount = 0;
    صحیح		min_argcount = 0;
    صحیح		idx;

    idx = find_internal_func(name);
    اگر (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    وگرنه
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = پوچ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	اگر (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	اگر (ufunc == پوچ)
	    بازگشت FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    بازگشت OK;
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
    صحیح		is_global = FALSE;
    char_u	*bracket_start = پوچ;
    char_u	*generic_func_name = name;

    اگر (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    وگرنه اگر (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    اگر (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(تهی)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    وگرنه
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	اگر (ufunc != پوچ)
	    بازگشت ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      پوچ, پوچ, پوچ, &ufunc);
	اگر (ufunc != پوچ)
	{
	    vim_free(fname);
	    بازگشت ufunc;
	}
	اگر (fname != پوچ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    اگر (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		بازگشت پوچ;
	    }
	}
    }
    اگر (fname == پوچ)
    {
	semsg(_(e_invalid_argument_str), name);
	بازگشت پوچ;
    }
    اگر (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	بازگشت پوچ;
    }

    ufunc = find_func(fname, is_global);
    اگر (ufunc == پوچ)
    {
	char_u *p = untrans_function_name(fname);

	اگر (p != پوچ)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	اگر (ufunc == پوچ)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    اگر (bracket_start != پوچ)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    اگر (ufunc != پوچ)
    {
	// handle generic functions
	اگر (bracket_start == پوچ)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    اگر (ufunc == پوچ)
	semsg(_(e_cannot_find_function_str), name);
    بازگشت ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    تهی
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    اگر (func_needs_compiling(ufunc, compile_type))
	(تهی)compile_def_function(ufunc, FALSE, compile_type, پوچ);
    وگرنه
	smsg(_("Function %s%s%s does not need compiling"),
				cl != پوچ ? cl->class_name.string : (char_u *)"",
				cl != پوچ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ایستا تهی
defcompile_funcs_in_script(تهی)
{
    long	todo = (long)func_hashtab.ht_used;
    صحیح		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    برای (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    اگر (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(تهی)compile_def_function(ufunc, FALSE, CT_NONE, پوچ);

		اگر (func_hashtab.ht_changed != changed)
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
    تهی
ex_defcompile(exarg_T *eap)
{
    اگر (*eap->arg != NUL)
    {
	typval_T tv;

	اگر (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    اگر (cl != پوچ)
		defcompile_class(cl);
	}
	وگرنه
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    اگر (ufunc != پوچ)
		defcompile_function(ufunc, پوچ);
	}
    }
    وگرنه
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
    صحیح
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    اگر (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	بازگشت 5;
    اگر (p[0] == 's' && p[1] == ':')
	بازگشت 2;
    بازگشت 0;
}

    صحیح
translated_function_exists(char_u *name, صحیح is_global)
{
    اگر (builtin_function(name, -1))
	بازگشت has_internal_func(name);
    بازگشت find_func(name, is_global) != پوچ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    صحیح
has_varargs(ufunc_T *ufunc)
{
    بازگشت ufunc->uf_varargs || ufunc->uf_va_name != پوچ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    صحیح
function_exists(char_u *name, صحیح no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    صحیح	    n = FALSE;
    صحیح	    flag;
    صحیح	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    اگر (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    اگر (p != پوچ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    بازگشت n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    صحیح
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    صحیح		is_global = FALSE;
    ufunc_T	*fp = پوچ;
    list_T	*args;
    صحیح		is_def;
    نویسه	*tofree;

    اگر (*name == NUL)
	بازگشت FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    اگر (p != پوچ && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    اگر (fp == پوچ)
	بازگشت FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    اگر (args == پوچ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	بازگشت FAIL;
    }
    برای (صحیح j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	اگر (arg == پوچ || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    بازگشت FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	اگر (fp->uf_arg_types == پوچ)
	    dict_add_string(arg, "type", (char_u *)"any");
	وگرنه
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	اگر (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    اگر (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	اگر (va == پوچ || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    بازگشت FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == پوچ ? (char_u *)"" : fp->uf_va_name);
	اگر (fp->uf_va_type == پوچ)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	وگرنه
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    اگر (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    وگرنه
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_عدد(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_عدد(d, "lnum", fp->uf_script_ctx.sc_lnum);
    بازگشت OK;
}

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, صحیح check)
{
    char_u	*nm = name;
    char_u	*p;
    صحیح		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    اگر (p != پوچ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	بازگشت p;

    vim_free(p);
    بازگشت پوچ;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, صحیح idx)
{
    ایستا long_u	done;
    ایستا صحیح		changed;
    ایستا hashitem_T	*hi;
    ufunc_T		*fp;

    اگر (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    اگر (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	صحیح len;

	اگر (done++ > 0)
	    ++hi;
	وقتی (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	اگر ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    بازگشت (char_u *)"";

	اگر (fp->uf_namelen + 4 >= IOSIZE)
	    بازگشت fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	اگر (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    اگر (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	بازگشت IObuff;
    }
    بازگشت پوچ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, صحیح extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    اگر (ufunc == پوچ)
	بازگشت پوچ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = پوچ;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_شمار = پوچ;
    ufunc->uf_tml_total = پوچ;
    ufunc->uf_tml_self = پوچ;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    اگر (ufunc->uf_arg_types != پوچ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	اگر (at != پوچ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     اندازه(type_T *) * ufunc->uf_args.ga_len);
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

    اگر (ufunc->uf_va_name != پوچ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = پوچ;

    ga_copy_strings(&fp->uf_سطرها, &ufunc->uf_سطرها);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    اگر (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    بازگشت ufunc;
}

/*
 * ":delfunction {name}"
 */
    تهی
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = پوچ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    صحیح		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     پوچ, پوچ, پوچ);
    vim_free(fudi.fd_newkey);
    اگر (name == پوچ)
    {
	اگر (fudi.fd_dict != پوچ && !eap->skip)
	    emsg(_(e_funcref_required));
	بازگشت;
    }
    اگر (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	بازگشت;
    }
    set_nextcmd(eap, p);
    اگر (eap->nextcmd != پوچ)
	*p = NUL;

    اگر (numbered_function(name) && fudi.fd_dict == پوچ)
    {
	اگر (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	بازگشت;
    }
    اگر (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    اگر (!eap->skip)
    {
	اگر (fp == پوچ)
	{
	    اگر (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    بازگشت;
	}
	اگر (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    بازگشت;
	}
	اگر (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    بازگشت;
	}

	اگر (fudi.fd_dict != پوچ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	وگرنه
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    اگر (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		اگر (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    وگرنه
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    تهی
func_unref(char_u *name)
{
    ufunc_T *fp = پوچ;

    اگر (name == پوچ || !func_name_refcount(name))
	بازگشت;
    fp = find_func(name, FALSE);
    اگر (fp == پوچ && numbered_function(name))
    {
#ifdef EXITFREE
	اگر (!entered_free_all_mem)
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
    تهی
func_ptr_unref(ufunc_T *fp)
{
    اگر (fp != پوچ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != پوچ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	اگر (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    تهی
func_ref(char_u *name)
{
    ufunc_T *fp;

    اگر (name == پوچ || !func_name_refcount(name))
	بازگشت;
    fp = find_func(name, FALSE);
    اگر (fp != پوچ)
	++fp->uf_refcount;
    وگرنه اگر (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    تهی
func_ptr_ref(ufunc_T *fp)
{
    اگر (fp != پوچ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ایستا صحیح
can_free_funccal(funccall_T *fc, صحیح copyID)
{
    بازگشت (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    تهی
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    صحیح		returning = FALSE;
    evalarg_T	evalarg;

    اگر (current_funccal == پوچ)
    {
	emsg(_(e_return_not_inside_function));
	بازگشت;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    اگر (eap->skip)
	++emsg_skip;

    eap->nextcmd = پوچ;
    اگر ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	اگر (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	وگرنه
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    وگرنه اگر (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	اگر (!aborting())
	    returning = do_return(eap, FALSE, TRUE, پوچ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    اگر (returning)
	eap->nextcmd = پوچ;
    وگرنه اگر (eap->nextcmd == پوچ)	    // no argument
	set_nextcmd(eap, arg);

    اگر (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ایستا صحیح
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    صحیح		doesrange;
    typval_T	rettv;
    صحیح		failed = FALSE;

    lnum = eap->line1;
    برای ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	اگر (eap->addr_شمار > 0)
	{
	    اگر (lnum > curbuf->b_ml.ml_سطر_شمار)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		بشکن;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	اگر (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    بشکن;
	}
	اگر (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	اگر (handle_subscript(arg, پوچ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    بشکن;
	}

	clear_tv(&rettv);
	اگر (doesrange)
	    بشکن;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	اگر (aborting())
	    بشکن;
    }
    بازگشت failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ایستا صحیح
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    صحیح		partial_argc = 0;		// number of partial arguments
    صحیح		argcount = 0;			// number of arguments found
    صحیح		r;

    اگر (current_funccal == پوچ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	بازگشت FAIL;
    }
    اگر (partial != پوچ)
    {
	اگر (partial->pt_dict != پوچ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    بازگشت FAIL;
	}
	اگر (partial->pt_argc > 0)
	{
	    صحیح i;

	    partial_argc = partial->pt_argc;
	    برای (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    صحیح is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    اگر (r == OK)
    {
	اگر (type != پوچ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, پوچ, name);
	}
	وگرنه اگر (is_builtin)
	{
	    صحیح idx = find_internal_func(name);

	    اگر (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    وگرنه اگر (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	وگرنه
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    اگر (ufunc != پوچ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		اگر (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    اگر (r == FAIL)
    {
	وقتی (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	بازگشت FAIL;
    }
    بازگشت add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    صحیح
can_add_defer(تهی)
{
    اگر (!in_def_function() && get_current_funccal() == پوچ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    صحیح
add_defer(char_u *name, صحیح argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    صحیح		argcount = argcount_arg;
    defer_T	*dr;
    صحیح		ret = FAIL;

    اگر (saved_name == پوچ)
	برو theend;
    اگر (in_def_function())
    {
	اگر (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    وگرنه
    {
	اگر (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, اندازه(defer_T), 10);
	اگر (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    برو theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	وقتی (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    وقتی (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    بازگشت ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ایستا تهی
handle_defer_one(funccall_T *funccal)
{
    صحیح	    idx;

    برای (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	اگر (dr->dr_name == پوچ)
	    // already being called, can happen if function does ":qa"
	    ادامه;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = پوچ;

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
	برای (صحیح i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ایستا تهی
invoke_funccall_defer(funccall_T *fc)
{
    اگر (fc->fc_ectx != پوچ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    وگرنه
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    تهی
invoke_all_defer(تهی)
{
    برای (funccall_T *fc = current_funccal; fc != پوچ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    برای (funccal_entry_T *fce = funccal_پشته; fce != پوچ; fce = fce->next)
	برای (funccall_T *fc = fce->بالاترین_funccal; fc != پوچ; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    تهی
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    صحیح		len;
    صحیح		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = پوچ;
    partial_T	*partial = پوچ;
    evalarg_T	evalarg;
    type_T	*type = پوچ;
    صحیح		found_var = FALSE;
    صحیح		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    اگر (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	اگر (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	بازگشت;
    }

    tofree = trans_function_name_ext(&arg, پوچ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : پوچ, &ufunc);
    اگر (fudi.fd_newkey != پوچ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_کلید_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    اگر (tofree == پوچ)
	بازگشت;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    اگر (fudi.fd_dict != پوچ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (صحیح)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != پوچ ? پوچ : &partial,
				vim9script && type == پوچ ? &type : پوچ,
						     FALSE, FALSE, &found_var);

    اگر (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	اگر (name == پوچ)
	    برو end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    اگر (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	برو end;
    }
    اگر (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	برو end;
    }

    اگر (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    وگرنه
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
    اگر ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	اگر (!ends_excmd2(eap->arg, arg))
	{
	    اگر (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	وگرنه
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
    صحیح
do_return(
    exarg_T	*eap,
    صحیح		reanimate,
    صحیح		is_cmd,
    تهی	*rettv)
{
    صحیح		idx;
    cstack_T	*cstack = eap->cstack;

    اگر (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    اگر (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	اگر (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	وگرنه
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    اگر (reanimate)
		rettv = current_funccal->fc_rettv;

	    اگر (rettv != پوچ)
	    {
		// Store the value of the pending return.
		اگر ((cstack->cs_rettv[idx] = alloc_tv()) != پوچ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		وگرنه
		    emsg(_(e_out_of_memory));
	    }
	    وگرنه
		cstack->cs_rettv[idx] = پوچ;

	    اگر (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_عدد;
		current_funccal->fc_rettv->vval.v_عدد = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    وگرنه
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	اگر (!reanimate && rettv != پوچ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    اگر (!is_cmd)
		vim_free(rettv);
	}
    }

    بازگشت idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    تهی
discard_pending_return(تهی *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(تهی *rettv)
{
    char_u	*s = پوچ;
    char_u	*tofree = پوچ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    اگر (rettv != پوچ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    اگر (s == پوچ)
	s = (char_u *)"";
    وگرنه
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    اگر (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    بازگشت vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_سطر(
    صحیح	    c UNUSED,
    تهی    *cookie,
    صحیح	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    اگر (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	func_سطر_end(cookie);
#endif

    gap = &fp->uf_سطرها;
    اگر (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = پوچ;
    وگرنه
    {
	// Skip NULL lines (continuation lines).
	وقتی (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == پوچ)
	    ++fcp->fc_linenr;
	اگر (fcp->fc_linenr >= gap->ga_len)
	    retval = پوچ;
	وگرنه
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    اگر (do_profiling == PROF_YES)
		func_سطر_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    اگر (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    بازگشت retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    صحیح
func_has_ended(تهی *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    بازگشت (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    صحیح
func_has_abort(
    تهی    *cookie)
{
    بازگشت ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = پوچ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    اگر (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != پوچ
				     && rettv->vval.v_partial->pt_func != پوچ)
	fp = rettv->vval.v_partial->pt_func;
    وگرنه
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == پوچ ? پوچ
					      : rettv->vval.v_partial->pt_name;
	اگر (fname == پوچ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = پوچ;
	}
	وگرنه
	{
	    char_u	*tofree = پوچ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    اگر (fp != پوچ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	اگر (pt != پوچ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = پوچ;
	    اگر (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    وگرنه
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		صحیح		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		اگر (ret_pt->pt_name != پوچ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		وگرنه
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		اگر (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    اگر (pt->pt_argv == پوچ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    وگرنه
		    {
			pt->pt_argc = ret_pt->pt_argc;
			برای (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    بازگشت selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(تهی *cookie)
{
    بازگشت ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(تهی *cookie)
{
    بازگشت &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    صحیح *
func_dbg_tick(تهی *cookie)
{
    بازگشت &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    صحیح
func_level(تهی *cookie)
{
    بازگشت ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    صحیح
current_func_returned(تهی)
{
    بازگشت current_funccal->fc_returned;
}

    صحیح
free_unref_funccal(صحیح copyID, صحیح testing)
{
    صحیح		did_free = FALSE;
    صحیح		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    برای (pfc = &previous_funccal; *pfc != پوچ; )
    {
	اگر (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	وگرنه
	    pfc = &(*pfc)->fc_caller;
    }
    اگر (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(تهی)garbage_collect(testing);

    بازگشت did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ایستا funccall_T *
get_funccal(تهی)
{
    صحیح		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    اگر (debug_backtrace_level > 0)
    {
	برای (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    اگر (temp_funccal)
		funccal = temp_funccal;
	    وگرنه
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    بازگشت funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ایستا funccall_T *
get_funccal_for_vars(تهی)
{
    funccall_T	*funccal = پوچ;

    اگر (current_funccal == پوچ)
	بازگشت پوچ;
    funccal = get_funccal();
    اگر (funccal == پوچ || funccal->fc_l_vars.dv_refcount == 0)
	بازگشت پوچ;
    بازگشت funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(تهی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    بازگشت funccal == پوچ ? پوچ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(تهی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    بازگشت funccal == پوچ ? پوچ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(تهی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    بازگشت funccal == پوچ ? پوچ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(تهی)
{
    funccall_T	*funccal = get_funccal_for_vars();

    بازگشت funccal == پوچ ? پوچ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    تهی
list_func_vars(صحیح *first)
{
    اگر (current_funccal != پوچ && current_funccal->fc_l_vars.dv_refcount > 0)
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
    اگر (current_funccal != پوچ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	بازگشت &current_funccal->fc_l_vars;
    بازگشت پوچ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = پوچ;
    char_u	*varname;

    اگر (current_funccal == پوچ || current_funccal->fc_func->uf_scoped == پوچ)
	بازگشت پوچ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    وقتی (current_funccal != پوچ)
    {
	ht = find_var_ht(name, &varname);
	اگر (ht != پوچ && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    اگر (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		بشکن;
	    }
	}
	اگر (current_funccal == current_funccal->fc_func->uf_scoped)
	    بشکن;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    بازگشت hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, صحیح no_autoload)
{
    dictitem_T	*v = پوچ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    اگر (current_funccal == پوچ || current_funccal->fc_func->uf_scoped == پوچ)
	بازگشت پوچ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    وقتی (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	اگر (ht != پوچ && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    اگر (v != پوچ)
		بشکن;
	}
	اگر (current_funccal == current_funccal->fc_func->uf_scoped)
	    بشکن;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    بازگشت v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    صحیح
set_ref_in_previous_funccal(صحیح copyID)
{
    funccall_T	*fc;

    برای (fc = previous_funccal; fc != پوچ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	اگر (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, پوچ, پوچ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, پوچ, پوچ)
		|| set_ref_in_list_اقلام(&fc->fc_l_varlist, copyID + 1, پوچ, پوچ))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

    ایستا صحیح
set_ref_in_funccal(funccall_T *fc, صحیح copyID)
{
    اگر (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	اگر (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, پوچ, پوچ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, پوچ, پوچ)
		|| set_ref_in_list_اقلام(&fc->fc_l_varlist, copyID, پوچ, پوچ)
		|| set_ref_in_func(پوچ, fc->fc_func, copyID))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    صحیح
set_ref_in_call_پشته(صحیح copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    برای (fc = current_funccal; fc != پوچ; fc = fc->fc_caller)
	اگر (set_ref_in_funccal(fc, copyID))
	    بازگشت TRUE;

    // Also go through the funccal_stack.
    برای (entry = funccal_پشته; entry != پوچ; entry = entry->next)
	برای (fc = entry->بالاترین_funccal; fc != پوچ; fc = fc->fc_caller)
	    اگر (set_ref_in_funccal(fc, copyID))
		بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    صحیح
set_ref_in_functions(صحیح copyID)
{
    صحیح		todo;
    hashitem_T	*hi = پوچ;
    ufunc_T	*fp;

    todo = (صحیح)func_hashtab.ht_used;
    برای (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    اگر (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(پوچ, fp, copyID))
		بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    صحیح
set_ref_in_func_args(صحیح copyID)
{
    صحیح i;

    برای (i = 0; i < funcargs.ga_len; ++i)
	اگر (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, پوچ, پوچ, پوچ))
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحیح
set_ref_in_func(char_u *name, ufunc_T *fp_in, صحیح copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = پوچ;
    char_u	*fname;
    صحیح		abort = FALSE;

    اگر (name == پوچ && fp_in == پوچ)
	بازگشت FALSE;

    اگر (fp_in == پوچ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    اگر (fp != پوچ)
    {
	برای (fc = fp->uf_scoped; fc != پوچ; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    بازگشت abort;
}

#endif // FEAT_EVAL
