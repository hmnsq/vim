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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ساكن hashtab_T	func_hashtab;

// Used by get_func_tv()
ساكن garray_T funcargs = GA_خالی;

// pointer to funccal for currently active function
ساكن funccall_T *current_funccal = عدم;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ساكن funccall_T *قبلی_funccal = عدم;

ساكن فراغ funccal_unref(funccall_T *fc, ufunc_T *fp, صحيح force);
ساكن فراغ func_clear(ufunc_T *fp, صحيح force);
ساكن صحيح func_free(ufunc_T *fp, صحيح force);
ساكن char_u *untrans_function_name(char_u *name);
ساكن فراغ handle_defer_one(funccall_T *funccal);
ساكن char_u *trans_function_name_ext(char_u **pp, صحيح *is_global, صحيح skip, صحيح flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    فراغ
func_init(فراغ)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(فراغ)
{
    أرجع &func_hashtab;
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
    ساكن char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	صحيح	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	صحيح	    is_vararg,
	صحيح	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = عدم;
    صحيح		is_underscore = FALSE;

    طالما (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    إذا (arg == p || امن_isdigit(*arg)
	    || (argtypes == عدم
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	إذا (!skip)
	    semsg(_(e_illegal_آرگومان_str), arg);
	أرجع arg;
    }

    // Extra checks in Vim9 script.
    إذا (!skip && argtypes != عدم)
    {
	صحيح c = *p;
	*p = NUL;
	صحيح r = check_reserved_name(arg, FALSE);
	*p = c;
	إذا (r == FAIL)
	    أرجع arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	إذا (check_defined(arg, p - arg,
				   evalarg == عدم ? عدم : evalarg->eval_cctx,
			       eap == عدم ? عدم : eap->cstack, TRUE) == FAIL)
	    أرجع arg;
    }

    إذا (newargs != عدم && ga_grow(newargs, 1) == FAIL)
	أرجع arg;
    إذا (newargs != عدم)
    {
	صحيح	c;
	صحيح	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	إذا (arg_copy == عدم)
	{
	    *p = c;
	    أرجع arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	إذا (argtypes == عدم || !is_underscore)
	    // Check for duplicate argument name.
	    لكل (i = 0; i < newargs->ga_len; ++i)
		إذا (STRCMP(((char_u **)(newargs->ga_داده))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_آرگومان_name_str), arg_copy);
		    vim_free(arg_copy);
		    أرجع arg;
		}
	((char_u **)(newargs->ga_داده))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    إذا (argtypes != عدم && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != عدم && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = عدم;

	إذا (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == عدم ? arg : arg_copy);
	    p = skipwhite(p);
	}
	إذا (*p == ':')
	{
	    ++p;
	    إذا (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		أرجع arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    إذا (!skip)
		type = vim_strnsave(type, p - type);
	}
	وإلا إذا (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_آرگومان_type_for_str),
					    arg_copy == عدم ? arg : arg_copy);
	    أرجع arg;
	}
	إذا (!skip)
	{
	    إذا (type == عدم && types_optional)
	    {
		// lambda arguments default to "any" type
		إذا (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		وإلا
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_داده)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_داده)[arg_objm->ga_len++] = FALSE;
	}
    }

    أرجع p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ساكن char_u *
get_function_سطر(
	exarg_T		*eap,
	garray_T	*سطرها_مقصد_free,
	صحيح		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    إذا (eap->ea_getline == عدم)
	theline = getcmdline(':', 0L, indent, 0);
    وإلا
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    إذا (theline != عدم)
    {
	إذا (سطرها_مقصد_free->ga_len > 0
		&& eap->cmdlinep != عدم
		&& *eap->cmdlinep == ((char_u **)سطرها_مقصد_free->ga_داده)
						   [سطرها_مقصد_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(فراغ)ga_جمع_کن_string(سطرها_مقصد_free, theline);
    }

    أرجع theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ساكن صحيح
get_function_آرگومانها(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    صحيح		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    صحيح		*varargs,
    garray_T	*default_آرگومانها,
    صحيح		skip,
    exarg_T	*eap,		// can be NULL
    صحيح		درون_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*سطرها_مقصد_free)
{
    صحيح		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    صحيح		c;
    صحيح		any_default = FALSE;
    char_u	*whitep = *argp;
    صحيح		need_عبارت = FALSE;

    إذا (newargs != عدم)
	ga_init2(newargs, حجم(char_u *), 3);
    إذا (argtypes != عدم)
	ga_init2(argtypes, حجم(char_u *), 3);
    إذا (arg_objm != عدم)
	ga_init2(arg_objm, حجم(int8_T), 3);
    إذا (!skip && default_آرگومانها != عدم)
	ga_init2(default_آرگومانها, حجم(char_u *), 3);

    إذا (varargs != عدم)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    طالما (*p != endchar)
    {
	طالما (eap != عدم && eap->ea_getline != عدم
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_سطر(eap, سطرها_مقصد_free, 0,
							  GETLINE_CONCAT_CONT);

	    إذا (theline == عدم)
		اكسر;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	إذا (mustend && *p != endchar)
	{
	    إذا (!skip)
		semsg(_(e_invalid_آرگومان_str), *argp);
	    اذهب err_ret;
	}
	إذا (*p == endchar && !need_عبارت)
	    اكسر;

	إذا (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    إذا (varargs != عدم)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    إذا (argtypes != عدم)
	    {
		// ...name: list<type>
		إذا (!eval_isnamec1(*p))
		{
		    إذا (!skip)
			emsg(_(e_missing_name_after_dots));
		    اذهب err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		إذا (p == arg)
		    اكسر;
		إذا (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    اكسر;
		}
	    }
	}
	وإلا إذا (درون_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    طالما (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    إذا (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_روش_str), arg);
		*argend = c;
		اكسر;
	    }

	    إذا (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		إذا (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_مقدار_must_be_vnone_str), p);
		    اذهب err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		إذا (ga_grow(default_آرگومانها, 1) == FAIL)
		    اذهب err_ret;

		char_u *عبارت = vim_strnsave((char_u *)"v:none", 6);
		إذا (عبارت == عدم)
		    اذهب err_ret;
		((char_u **)(default_آرگومانها->ga_داده))
						 [default_آرگومانها->ga_len] = عبارت;
		default_آرگومانها->ga_len++;
	    }
	    وإلا إذا (any_default)
	    {
		emsg(_(e_non_default_آرگومان_follows_default_آرگومان));
		اذهب err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    إذا (newargs != عدم && ga_grow(newargs, 1) == FAIL)
		أرجع FAIL;
	    إذا (newargs != عدم)
	    {
		((char_u **)(newargs->ga_داده))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		إذا (argtypes != عدم && ga_grow(argtypes, 1) == OK
			    && arg_objm != عدم && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_داده)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_داده)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    إذا (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			صحيح len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			إذا (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			إذا (assignment != عدم)
			{
			    c = *argend;
			    *argend = NUL;
			    إذا (any_default)
				vim_snprintf((حرف *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_آرگومانها->ga_len - 1, arg, arg);
			    وإلا
				vim_snprintf((حرف *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_داده))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    إذا (*p == ',')
		++p;
	}
	وإلا
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    إذا (p == arg)
		اكسر;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    إذا (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_آرگومانها != عدم)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *عبارت = p;
		إذا (eval1(&p, &rettv, عدم) != FAIL)
		{
		    إذا (!skip)
		    {
			إذا (ga_grow(default_آرگومانها, 1) == FAIL)
			    اذهب err_ret;

			إذا (need_عبارت)
			    need_عبارت = FALSE;
			// trim trailing whitespace
			طالما (p > عبارت && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			عبارت = vim_strsave(عبارت);
			إذا (عبارت == عدم)
			{
			    *p = c;
			    اذهب err_ret;
			}
			((char_u **)(default_آرگومانها->ga_داده))
						 [default_آرگومانها->ga_len] = عبارت;
			default_آرگومانها->ga_len++;
			*p = c;
		    }
		}
		وإلا
		{
		    mustend = TRUE;
		    إذا (*skipwhite(p) == NUL)
			need_عبارت = TRUE;
		}
	    }
	    وإلا إذا (any_default)
	    {
		emsg(_(e_non_default_آرگومان_follows_default_آرگومان));
		اذهب err_ret;
	    }

	    إذا (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		إذا (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    اذهب err_ret;
		}
		p = skipwhite(p);
	    }
	    إذا (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		إذا (!skip && argtypes != عدم
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    اذهب err_ret;
		}
	    }
	    وإلا
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    إذا (*p != endchar)
	اذهب err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    أرجع OK;

err_ret:
    إذا (newargs != عدم)
	ga_clear_رشتهها(newargs);
    إذا (!skip && default_آرگومانها != عدم)
	ga_clear_رشتهها(default_آرگومانها);
    أرجع FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ساكن صحيح
parse_آرگومان_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    صحيح		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    صحيح		obj_member_شمار,
    cctx_T	*cctx)
{
    صحيح len = 0;

    ga_init2(&fp->uf_type_list, حجم(type_T *), 10);
    إذا (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	إذا (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	إذا (fp->uf_arg_types != عدم)
	{
	    صحيح	i;
	    type_T	*type;

	    لكل (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_داده)[i];

		إذا (p == عدم)
		    // will get the type from the default value
		    type = &t_unknown;
		وإلا
		{
		    إذا (arg_objm != عدم && ((int8_T *)arg_objm->ga_داده)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_آرگومانها.ga_داده)[i];

			type = &t_any;
			لكل (صحيح om = 0; om < obj_member_شمار; ++om)
			{
			    إذا (obj_members != عدم
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				اكسر;
			    }
			}
		    }
		    وإلا
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		إذا (type == عدم || !valid_declaration_type(type))
		    أرجع FAIL;
		fp->uf_arg_types[i] = type;
		إذا (i < fp->uf_آرگومانها.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_آرگومانها.ga_داده)[i];
		    إذا (obj_members != عدم && *name == '_')
			// protected object method
			name++;

		    إذا (var_wrong_func_name(name, TRUE))
			أرجع FAIL;
		}
	    }
	}
    }

    إذا (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_آرگومانها.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_آرگومانها.ga_داده)[fp->uf_آرگومانها.ga_len];
	((char_u **)fp->uf_آرگومانها.ga_داده)[fp->uf_آرگومانها.ga_len] = عدم;
	p = ((char_u **)argtypes->ga_داده)[len];
	إذا (p == عدم)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	وإلا
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    إذا (fp->uf_va_type != عدم && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_داده)[len]);
		أرجع FAIL;
	    }
	}
	إذا (fp->uf_va_type == عدم)
	    أرجع FAIL;
    }

    أرجع OK;
}

    ساكن صحيح
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    إذا (ret_type == عدم)
	fp->uf_ret_type = &t_void;
    وإلا
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	إذا (fp->uf_ret_type == عدم)
	{
	    fp->uf_ret_type = &t_void;
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ساكن صحيح
ثبات_بستگی(ufunc_T *fp)
{
    إذا (fp->uf_scoped == current_funccal)
	// no change
	أرجع OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    إذا (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	أرجع FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_داده)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    أرجع OK;
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

    إذا (name[0] != K_SPECIAL)
	أرجع name;
    len = STRLEN(name);
    إذا (len + 3 > bufsize)
	أرجع name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    أرجع buf;
}

ساكن char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(فراغ)
{
    ساكن صحيح	lambda_no = 0;
    صحيح		n;
    string_T	ret;

    n = vim_snprintf((حرف *)lambda_name, حجم(lambda_name), "<lambda>%d", ++lambda_no);
    إذا (n < 1)
	ret.درازی = 0;
    وإلا إذا (n >= (صحيح)حجم(lambda_name))
	ret.درازی = حجم(lambda_name) - 1;
    وإلا
	ret.درازی = (size_t)n;

    ret.string = lambda_name;
    أرجع ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ساكن ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < حجم(ufunc_T) ? حجم(ufunc_T) : len);
    إذا (fp != عدم)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((فراغ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	إذا (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    إذا (fp->uf_name_exp != عدم)
		vim_snprintf((حرف *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    أرجع fp;
}

#إذا defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
ثبات_cfunc(cfunc_T cb, cfunc_free_T cb_free, فراغ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.درازی);
    إذا (fp == عدم)
	أرجع عدم;

    fp->uf_def_وضعیت = UF_NOT_COMPILED;
    fp->uf_refcount = 1;
    fp->uf_varargs = TRUE;
    fp->uf_flags = FC_CFUNC | FC_LAMBDA;
    fp->uf_calls = 0;
    fp->uf_script_ctx = current_sctx;
    fp->uf_cb = cb;
    fp->uf_cb_free = cb_free;
    fp->uf_cb_state = state;

    hash_جمع_کن(&func_hashtab, UF2HIKEY(fp), "add C function");

    أرجع name.string;
}
#پایان_اگر

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ساكن char_u *
skip_arrow(
	char_u	*شروع,
	صحيح	equal_arrow,
	char_u	**ret_type,
	صحيح	*white_error)
{
    char_u  *s = شروع;
    char_u  *bef = شروع - 2; // "start" points to > of ->

    إذا (equal_arrow)
    {
	إذا (*s == ':')
	{
	    إذا (white_error != عدم && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		أرجع عدم;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    إذا (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		أرجع عدم;
	    }
	}
	bef = s;
	s = skipwhite(s);
	إذا (*s != '=')
	    أرجع عدم;
	++s;
    }
    إذا (*s != '>')
	أرجع عدم;
    إذا (white_error != عدم && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	أرجع عدم;
    }
    أرجع skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ساكن صحيح
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    إذا (checkforcmd(&p, "function", 2))
    {
	إذا (*p == '(')
	    أرجع FALSE;
	*cmd = p;
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ساكن فراغ
function_using_بلوک_scopes(ufunc_T *fp, cstack_T *cstack)
{
    إذا (cstack == عدم || cstack->cs_idx < 0)
	أرجع;

    صحيح	    شمار = cstack->cs_idx + 1;
    صحيح	    i;

    fp->uf_بلوک_ids = ALLOC_MULT(صحيح, شمار);
    إذا (fp->uf_بلوک_ids != عدم)
    {
	mch_memmove(fp->uf_بلوک_ids, cstack->cs_بلوک_id,
		حجم(صحيح) * شمار);
	fp->uf_بلوک_عمق = شمار;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    لكل (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ساكن char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    طالما (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	إذا (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	وإلا إذا (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    اكسر;
	}
	وإلا
	    MB_PTR_ADV(p);
    }

    أرجع p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ساكن char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    طالما (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	إذا (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	وإلا إذا (*p == '"')
	{
	    p++;    // skip the ending double quote
	    اكسر;
	}
	وإلا
	    MB_PTR_ADV(p);
    }

    أرجع p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ساكن char_u *
پیدا_کن_شروع_از_vim9_comment(char_u *سطر)
{
    char_u	*p = سطر;

    طالما (*p != NUL)
    {
	إذا (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	وإلا إذا (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	وإلا
	{
	    إذا (*p == '#')
		// Found the start of a Vim9 comment
		اكسر;
	    MB_PTR_ADV(p);
	}
    }

    أرجع p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ساكن صحيح
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *سطر_arg_درون,
	garray_T    *سطرها_مقصد_free)
{
    linenr_T	sourcing_lnum_بالاترین = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    صحيح		saved_wait_return = need_wait_return;
    char_u	*سطر_arg = سطر_arg_درون;
    صحيح		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_بلوک;
#تعریف_کن MAX_FUNC_NESTING 50
    حرف	nesting_def[MAX_FUNC_NESTING];
    حرف	nesting_inline[MAX_FUNC_NESTING];
    صحيح		nesting = 0;
    getline_opt_T getline_options;
    صحيح		indent = 2;
    char_u	*skip_until = عدم;
    صحيح		ret = FAIL;
    صحيح		is_heredoc = FALSE;
    صحيح		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = عدم;
    size_t	heredoc_trimmedlen = 0;
    صحيح		did_emsg_شروع = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    إذا (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	إذا (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    اذهب theend;
	طالما (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_داده))[newlines->ga_len++] = عدم;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_بلوک;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    لكل (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	إذا (کلید_Typed)
	{
	    پیام_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	إذا (سطر_arg != عدم)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = سطر_arg;
	    p = vim_strchr(theline, '\n');
	    إذا (p == عدم)
		سطر_arg += STRLEN(سطر_arg);
	    وإلا
	    {
		*p = NUL;
		سطر_arg = p + 1;
	    }
	}
	وإلا
	{
	    theline = get_function_سطر(eap, سطرها_مقصد_free, indent,
							      getline_options);
	}
	إذا (کلید_Typed)
	    سطرها_چپ = ردیفها - 1;
	إذا (theline == عدم)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_بالاترین;
	    إذا (skip_until != عدم)
		semsg(_(e_missing_heredoc_انتها_marker_str), skip_until);
	    وإلا إذا (nesting_inline[nesting])
		emsg(_(e_missing_انتها_بلوک));
	    وإلا إذا (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    وإلا
		emsg(_(e_missing_endfunction));
	    اذهب theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	إذا (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	وإلا
	    sourcing_lnum_off = 0;

	إذا (skip_until != عدم)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    إذا (heredoc_trimmed == عدم
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		إذا (heredoc_trimmed == عدم)
		    p = theline;
		وإلا إذا (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		وإلا
		    p = theline + heredoc_trimmedlen;
		إذا (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    إذا (heredoc_concat_len > 0)
		    {
			// Replace the starting line with all the concatenated
			// lines.
			ga_concat(&heredoc_ga, theline);
			vim_free(((char_u **)(newlines->ga_داده))[
						      heredoc_concat_len - 1]);
			((char_u **)(newlines->ga_داده))[
				  heredoc_concat_len - 1] = heredoc_ga.ga_داده;
			ga_init(&heredoc_ga);
			heredoc_concat_len = 0;
			theline += STRLEN(theline);  // skip the "EOF"
		    }
		}
	    }
	}
	وإلا
	{
	    صحيح	    c;
	    char_u  *انتها;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    لكل (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		إذا (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    إذا (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		إذا (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		إذا (nesting-- == 0)
		{
		    char_u *nextcmd = عدم;

		    إذا (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    وإلا إذا (سطر_arg != عدم && *skipwhite(سطر_arg) != NUL)
			nextcmd = سطر_arg;
		    وإلا إذا (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_بالاترین
							+ newlines->ga_len + 1;
			إذا (eap->cmdidx == CMD_def)
			    semsg(_(e_متن_یافت_after_str_str), "enddef", p);
			وإلا
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    إذا (nextcmd != عدم && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			إذا (سطرها_مقصد_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)سطرها_مقصد_free->ga_داده)
						   [سطرها_مقصد_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)سطرها_مقصد_free->ga_داده)
						   [سطرها_مقصد_free->ga_len - 1];
			    --سطرها_مقصد_free->ga_len;
			}
		    }
		    اكسر;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    وإلا إذا (nesting_def[nesting])
	    {
		إذا (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    وإلا إذا (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    إذا (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    وإلا إذا (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    إذا (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		إذا (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, عدم, TRUE, 0));
		إذا (vim9_function && *p == '<')
		{
		    // skip generic function
		    إذا (skip_generic_func_type_آرگومانها(&p) == FAIL)
			اذهب theend;
		}
		إذا (*skipwhite(p) == '(')
		{
		    إذا (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    وإلا
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    إذا (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		إذا (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    انتها = پیدا_کن_شروع_از_vim9_comment(p) - 1;
		}
		وإلا
		    انتها = p + STRLEN(p) - 1;

		طالما (انتها > p && VIM_ISWHITE(*انتها))
		    --انتها;
		إذا (انتها > p + 1 && *انتها == '{' && VIM_ISWHITE(انتها[-1]))
		{
		    صحيح	    is_بلوک;

		    // check for trailing "=> {": start of an inline function
		    --انتها;
		    طالما (انتها > p && VIM_ISWHITE(*انتها))
			--انتها;
		    is_بلوک = انتها > p + 2 && انتها[-1] == '=' && انتها[0] == '>';
		    إذا (!is_بلوک)
			is_بلوک = پیدا_کن_cmd_بلوک_شروع(p) != عدم;

		    إذا (is_بلوک)
		    {
			إذا (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			وإلا
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
	    إذا (!vim9_function || colon)
		p = skip_range(p, FALSE, عدم);
	    char_u *tp = p;
	    إذا (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    وإلا
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    إذا (arg[0] == '<' && arg[1] =='<'
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
		إذا (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    إذا (heredoc_trimmed == عدم)
			heredoc_trimmedlen = 0;
		}
		إذا (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		وإلا
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		إذا (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    إذا (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		إذا (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    صحيح		save_sc_نسخه = current_sctx.sc_نسخه;
		    صحيح		var_شمار = 0;
		    صحيح		semicolon = 0;

		    current_sctx.sc_نسخه
				     = vim9_function ? SCRIPT_نسخه_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_شمار, &semicolon,
									 TRUE);
		    إذا (arg != عدم)
			arg = skipwhite(arg);
		    current_sctx.sc_نسخه = save_sc_نسخه;
		    إذا (arg != عدم && STRNCMP(arg, "=<<", 3) == 0)
		    {
			صحيح has_trim = FALSE;

			p = skipwhite(arg + 3);
			طالما (TRUE)
			{
			    إذا (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				استمر;
			    }
			    إذا (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				استمر;
			    }
			    اكسر;
			}
			إذا (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    إذا (heredoc_trimmed == عدم)
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
	إذا (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    اذهب theend;

	إذا (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	وإلا
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	إذا (p == عدم)
	    اذهب theend;
	((char_u **)(newlines->ga_داده))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	طالما (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_داده))[newlines->ga_len++] = عدم;

	// Check for end of eap->arg.
	إذا (سطر_arg != عدم && *سطر_arg == NUL)
	    سطر_arg = عدم;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    إذا (did_emsg == did_emsg_شروع)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_داده);
    need_wait_return |= saved_wait_return;
    أرجع ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ساكن صحيح
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	صحيح	    varargs,
	garray_T    *default_آرگومانها,
	char_u	    *ret_type)
{
    char_u	*شروع = *arg;
    صحيح		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = عدم;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = عدم;
    صحيح		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    صحيح		lnum_save = -1;
    linenr_T	sourcing_lnum_بالاترین = SOURCING_LNUM;
    char_u	*سطر_arg = عدم;

    *arg = skipwhite(*arg + 1);
    إذا (**arg == '|' || !دوسر_excmd2(شروع, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	أرجع FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    إذا (**arg == '\n')
	سطر_arg = *arg + 1;

    CLEAR_فیلد(eap);
    eap.cmdidx = CMD_بلوک;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    إذا (evalarg->eval_cctx != عدم)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    وإلا
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, حجم(char_u *), 10);
    إذا (get_function_body(&eap, &newlines, سطر_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	اذهب erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_شمار += newlines.ga_len;
    إذا (gap->ga_itemsize > 0)
    {
	صحيح	idx;
	char_u	*آخرین;
	size_t  plen;
	char_u  *pnl;

	لكل (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_داده)[idx];
	    إذا (p == عدم)
		// comment line in the lambda body
		استمر;

	    p = skipwhite(p);

	    إذا (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		اذهب erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    إذا (*p == NUL || vim9_comment_شروع(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    وإلا
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    إذا (pnl != عدم)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_داده)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_داده)[freegap->ga_len++] = pnl;
	}
	إذا (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    اذهب erret;
	إذا (eap.nextcmd != عدم)
	{
	    // more is following after the "}", which was skipped
	    آخرین = cmdline;
	    plen = STRLEN(آخرین);
	}
	وإلا
	{
	    // nothing is following the "}"
	    آخرین = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	إذا (pnl != عدم)
	    mch_memmove(pnl + 1, آخرین, plen + 1);
	((char_u **)gap->ga_داده)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_داده)[freegap->ga_len++] = pnl;
    }

    إذا (eap.nextcmd != عدم)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	إذا (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_داده))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    وإلا
	*arg = (char_u *)"";

    إذا (!evaluate)
    {
	ret = OK;
	اذهب erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.درازی);
    إذا (ufunc == عدم)
	اذهب erret;
    إذا (hash_جمع_کن(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	اذهب erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_آرگومانها = *newargs;
    newargs->ga_داده = عدم;
    ufunc->uf_def_آرگومانها = *default_آرگومانها;
    default_آرگومانها->ga_داده = عدم;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_بالاترین;

    // parse argument types, the type variables of the function being
    // compiled may be used
    إذا (parse_آرگومان_types(ufunc, argtypes, varargs, عدم, عدم, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	اذهب erret;
    }

    // parse the return type, if any
    إذا (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	اذهب erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    إذا (pt == عدم)
	اذهب erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_سطرها = newlines;
    newlines.ga_داده = عدم;
    إذا (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    إذا (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_نسخه = current_sctx.sc_نسخه;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_بالاترین;
    مجموعه_function_type(ufunc);

    function_using_بلوک_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = عدم;
    ret = OK;

erret:
    إذا (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_رشتهها(&newlines);
    إذا (newargs != عدم)
	ga_clear_رشتهها(newargs);
    ga_clear_رشتهها(default_آرگومانها);
    إذا (ufunc != عدم)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    أرجع ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    صحيح
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	صحيح	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_آرگومانها;
    garray_T	arg_objm;
    ufunc_T	*fp = عدم;
    partial_T   *pt = عدم;
    صحيح		varargs;
    char_u	*ret_type = عدم;
    صحيح		ret;
    char_u	*s;
    char_u	*شروع, *انتها;
    صحيح		*old_eval_lavars = eval_lavars_استفاده_شده;
    صحيح		eval_lavars = FALSE;
    char_u	*tofree2 = عدم;
    صحيح		equal_arrow = **arg == '(';
    صحيح		white_error = FALSE;
    صحيح		called_emsg_شروع = called_emsg;
    صحيح		vim9script = درون_vim9script();
    دراز	شروع_lnum = SOURCING_LNUM;

    إذا (equal_arrow && !vim9script)
	أرجع NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_آرگومانها(&s, equal_arrow ? ')' : '-', عدم,
	    types_optional ? &argtypes : عدم, types_optional,
			types_optional ? &arg_objm : عدم, evalarg,
			عدم, &default_آرگومانها, TRUE, عدم, FALSE, عدم, عدم);
    إذا (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, عدم) == عدم)
    {
	إذا (types_optional)
	{
	    ga_clear_رشتهها(&argtypes);
	    ga_clear(&arg_objm);
	}
	أرجع called_emsg == called_emsg_شروع ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    إذا (evaluate)
	pnewargs = &newargs;
    وإلا
	pnewargs = عدم;
    *arg += 1;
    ret = get_function_آرگومانها(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : عدم, types_optional,
			types_optional ? &arg_objm : عدم, evalarg,
					    &varargs, &default_آرگومانها,
					    FALSE, عدم, FALSE, عدم, عدم);
    إذا (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : عدم)) == عدم)
    {
	إذا (types_optional)
	{
	    ga_clear_رشتهها(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_رشتهها(&newargs);
	أرجع white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    إذا (ret_type != عدم)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    إذا (evaluate)
	eval_lavars_استفاده_شده = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    إذا (equal_arrow && **arg == '{')
    {
	إذا (evalarg == عدم)
	    // cannot happen?
	    اذهب theend;
	SOURCING_LNUM = شروع_lnum;  // used for where lambda is defined
	إذا (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : عدم, varargs,
			   &default_آرگومانها, ret_type) == FAIL)
	    اذهب errret;
	اذهب theend;
    }
    إذا (default_آرگومانها.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_مقادیر_درون_lambda));
	اذهب errret;
    }

    // Get the start and the end of the expression.
    شروع = *arg;
    ret = skip_عبارت_concatenate(arg, &شروع, &انتها, evalarg);
    إذا (ret == FAIL)
	اذهب errret;

    إذا (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg != '}')
	{
	    semsg(_(e_expected_راست_curly_str), *arg);
	    اذهب errret;
	}
	++*arg;
    }

    إذا (evaluate)
    {
	صحيح	    len;
	صحيح	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *سطر_انتها;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.درازی);
	إذا (fp == عدم)
	    اذهب errret;
	fp->uf_def_وضعیت = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	إذا (pt == عدم)
	    اذهب errret;

	ga_init2(&newlines, حجم(char_u *), 1);
	إذا (ga_grow(&newlines, 1) == FAIL)
	    اذهب errret;

	// If there are line breaks, we need to split up the string.
	سطر_انتها = vim_strchr(شروع, '\n');
	إذا (سطر_انتها == عدم || سطر_انتها > انتها)
	    سطر_انتها = انتها;

	// Add "return " before the expression (or the first line).
	len = 7 + (صحيح)(سطر_انتها - شروع) + 1;
	p = alloc(len);
	إذا (p == عدم)
	    اذهب errret;
	((char_u **)(newlines.ga_داده))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, شروع, سطر_انتها - شروع);

	إذا (سطر_انتها != انتها)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    طالما (سطر_انتها < انتها && *سطر_انتها == '\n')
	    {
		إذا (ga_grow(&newlines, 1) == FAIL)
		    اذهب errret;
		شروع = سطر_انتها + 1;
		سطر_انتها = vim_strchr(شروع, '\n');
		إذا (سطر_انتها == عدم || سطر_انتها > انتها)
		    سطر_انتها = انتها;
		((char_u **)(newlines.ga_داده))[newlines.ga_len++] =
					 vim_strnsave(شروع, سطر_انتها - شروع);
	    }
	}

	إذا (جستجوی_رشته((حرف *)p + 7, "a:") == عدم)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_آرگومانها = newargs;
	ga_init(&fp->uf_def_آرگومانها);
	إذا (types_optional)
	{
	    إذا (parse_آرگومان_types(fp, &argtypes,
				vim9script && varargs, عدم, عدم, 0,
				cctx) == FAIL)
		اذهب errret;
	    إذا (ret_type != عدم)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     عدم, cctx, TRUE);
		إذا (fp->uf_ret_type == عدم)
		    اذهب errret;
	    }
	    وإلا
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_سطرها = newlines;
	إذا (current_funccal != عدم && eval_lavars)
	{
	    flags |= FC_بستگی;
	    إذا (ثبات_بستگی(fp) == FAIL)
		اذهب errret;
	}

#اگر_تعریف_شده FEAT_PROFILE
	إذا (prof_def_func())
	    func_do_profile(fp);
#پایان_اگر
	إذا (sandbox)
	    flags |= FC_SANDBOX;
	// In legacy script a lambda can be called with more args than
	// uf_args.ga_len.  In Vim9 script "...name" has to be used.
	fp->uf_varargs = !vim9script || varargs;
	fp->uf_flags = flags;
	fp->uf_calls = 0;
	fp->uf_script_ctx = current_sctx;
	// Use the line number of the arguments.
	fp->uf_script_ctx.sc_lnum += شروع_lnum;

	function_using_بلوک_scopes(fp, evalarg->eval_cstack);

	pt->pt_func = fp;
	pt->pt_refcount = 1;
	rettv->vval.v_partial = pt;
	rettv->v_type = VAR_PARTIAL;

	hash_جمع_کن(&func_hashtab, UF2HIKEY(fp), "add lambda");
    }

theend:
    eval_lavars_استفاده_شده = old_eval_lavars;
    vim_free(tofree2);
    إذا (types_optional)
    {
	ga_clear_رشتهها(&argtypes);
	ga_clear(&arg_objm);
    }

    أرجع OK;

errret:
    ga_clear_رشتهها(&newargs);
    ga_clear_رشتهها(&newlines);
    ga_clear_رشتهها(&default_آرگومانها);
    إذا (types_optional)
    {
	ga_clear_رشتهها(&argtypes);
	ga_clear(&arg_objm);
    }
    إذا (fp != عدم)
    {
	vim_free(fp->uf_arg_types);
	vim_free(fp->uf_name_exp);
	vim_free(fp->uf_va_name);
	clear_type_list(&fp->uf_type_list);
	vim_free(fp);
    }
    vim_free(pt);
    vim_free(tofree2);
    eval_lavars_استفاده_شده = old_eval_lavars;
    أرجع FAIL;
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
	صحيح	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	صحيح	    no_autoload,
	صحيح	    new_function,
	صحيح	    *یافت_var)
{
    dictitem_T	*v;
    typval_T	*tv = عدم;
    صحيح		cc;
    char_u	*s = عدم;
    hashtab_T	*ht;
    صحيح		did_type = FALSE;

    إذا (partialp != عدم)
	*partialp = عدم;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = پیدا_کن_var_also_درون_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    إذا (v != عدم)
    {
	tv = &v->di_tv;
    }
    وإلا إذا (درون_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	صحيح	    len = *lenp;

	إذا (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = پیدا_کن_imported(p, len, FALSE);

	// imported function from another script
	إذا (import != عدم)
	{
	    name[len] = NUL;
	    إذا (new_function)
		semsg(_(e_redefining_imported_قلم_str), name);
	    وإلا
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    أرجع (char_u *)"";	// just in case
	}
    }

    إذا (tv != عدم)
    {
	إذا (یافت_var != عدم)
	    *یافت_var = TRUE;
	إذا (tv->v_type == VAR_FUNC)
	{
	    إذا (tv->vval.v_string == عدم)
	    {
		*lenp = 0;
		أرجع (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_پیدا_کن_open_bracket(s);
	    إذا (p != عدم && generic_func_پیدا_کن_open_bracket(name) != عدم)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		أرجع (char_u *)"";
	    }
	    إذا (p != عدم)
		*lenp = p - s;
	    وإلا
		*lenp = (صحيح)STRLEN(s);
	}
	وإلا إذا (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    إذا (pt == عدم)
	    {
		*lenp = 0;
		أرجع (char_u *)"";	// just in case
	    }
	    إذا (partialp != عدم)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (صحيح)STRLEN(s);
	}

	إذا (s != عدم)
	{
	    إذا (!did_type && type != عدم && ht == get_script_local_ht())
	    {
		svar_T  *sv = پیدا_کن_typval_درون_script(tv, 0, TRUE);

		إذا (sv != عدم)
		    *type = sv->sv_type;
	    }
	    أرجع s;
	}
    }

    أرجع name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    فراغ
emsg_funcname(حرف *ermsg, char_u *name)
{
    char_u	*p = name;

    إذا (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    إذا (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    صحيح
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	صحيح	    partial_argc,
	typval_T    *argvars,
	صحيح	    *argcount,
	صحيح	    is_builtin)
{
    char_u	*argp = *arg;
    صحيح		ret = OK;
    صحيح		vim9script = درون_vim9script();
    صحيح		evaluate = evalarg == عدم
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    طالما (*argcount < MAX_FUNC_آرگومانها - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	إذا (*argp == ')' || *argp == ',' || *argp == NUL)
	    اكسر;

	صحيح arg_idx = *argcount;
	إذا (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    اكسر;
	}
	++*argcount;
	إذا (!is_builtin && check_typval_is_مقدار(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    اكسر;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	إذا (vim9script)
	{
	    إذا (*argp != ',' && *skipwhite(argp) == ',')
	    {
		إذا (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		اكسر;
	    }
	}
	وإلا
	    argp = skipwhite(argp);
	إذا (*argp != ',')
	    اكسر;
	إذا (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    إذا (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    اكسر;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    إذا (*argp == ')')
	++argp;
    وإلا
	ret = FAIL;
    *arg = argp;
    أرجع ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    صحيح
get_func_tv(
    char_u	*name,		// name of the function
    صحيح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    صحيح		ret;
    typval_T	argvars[MAX_FUNC_آرگومانها + 1];	// vars for arguments
    صحيح		argcount = 0;			// number of arguments found
    صحيح		vim9script = درون_vim9script();
    صحيح		evaluate = evalarg == عدم
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == عدم ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    إذا (ret == OK)
    {
	صحيح	i = 0;
	صحيح	did_emsg_before = did_emsg;

	إذا (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    إذا (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, حجم(typval_T *), 50);
	    لكل (i = 0; i < argcount; ++i)
		إذا (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_داده)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	إذا (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    وإلا إذا (!aborting() && evaluate)
    {
	إذا (argcount == MAX_FUNC_آرگومانها)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	وإلا
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    طالما (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    إذا (vim9script)
	*arg = argp;
    وإلا
	*arg = skipwhite(argp);
    أرجع ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ساكن صحيح
eval_fname_sid(char_u *p)
{
    أرجع (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    إذا (script_name == name)
	أرجع name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (صحيح)KE_SNR;
    fname_buflen = 3;
    إذا (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    وإلا
    {
	إذا (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	وإلا
	{
	    fname_buflen += vim_snprintf((حرف *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(دراز)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    إذا (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    وإلا
    {
	fname = alloc(fnamelen + 1);
	إذا (fname == عدم)
	    *error = FCERR_OTHER;
	وإلا
	{
	    *tofree = fname;
	    vim_snprintf((حرف *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    أرجع fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    فراغ
func_name_with_sid(char_u *name, صحيح sid, char_u *میانگیر)
{
    // A script-local function is stored as "<SNR>99_name".
    میانگیر[0] = K_SPECIAL;
    میانگیر[1] = KS_EXTRA;
    میانگیر[2] = (صحيح)KE_SNR;
    vim_snprintf((حرف *)میانگیر + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (دراز)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ساكن ufunc_T *
پیدا_کن_func_with_sid(char_u *name, صحيح sid)
{
    hashitem_T	    *hi;
    char_u	    میانگیر[MAX_FUNC_NAME_LEN];

    إذا (!SCRIPT_ID_VALID(sid))
	أرجع عدم;	// not in a script

    func_name_with_sid(name, sid, میانگیر);
    hi = hash_پیدا_کن(&func_hashtab, میانگیر);
    إذا (!HASHITEM_خالی(hi))
	أرجع HI2UF(hi);
    أرجع عدم;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ساكن ufunc_T *
پیدا_کن_func_with_پیشوند(char_u *name, صحيح sid)
{
    hashitem_T	    *hi;
    char_u	    میانگیر[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    إذا (vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	أرجع عدم;	// already has the prefix
    إذا (!SCRIPT_ID_VALID(sid))
	أرجع عدم;	// not in a script
    si = SCRIPT_قلم(sid);
    إذا (si->sn_autoload_پیشوند != عدم)
    {
	size_t	len = STRLEN(si->sn_autoload_پیشوند) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	إذا (namep == عدم)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	إذا (len < حجم(میانگیر))
	    auto_name = میانگیر;
	وإلا
	    auto_name = alloc(len);
	إذا (auto_name != عدم)
	{
	    vim_snprintf((حرف *)auto_name, len, "%s%s",
						si->sn_autoload_پیشوند, namep);
	    hi = hash_پیدا_کن(&func_hashtab, auto_name);
	    إذا (auto_name != میانگیر)
		vim_free(auto_name);
	    إذا (!HASHITEM_خالی(hi))
		أرجع HI2UF(hi);
	}
    }

    أرجع عدم;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
پیدا_کن_func_even_dead(char_u *name, صحيح flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    إذا ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	إذا (درون_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = پیدا_کن_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    إذا (func != عدم)
		أرجع func;
	}
	إذا (درون_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    دراز    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    إذا (*p == '_')
	    {
		func = پیدا_کن_func_with_sid(p + 1, (صحيح)sid);
		إذا (func != عدم)
		    أرجع func;
	    }
	}
    }

    إذا ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_پیدا_کن(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	إذا (!HASHITEM_خالی(hi))
	    أرجع HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    أرجع پیدا_کن_func_with_پیشوند(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
پیدا_کن_func(char_u *name, صحيح is_global)
{
    ufunc_T	*fp = پیدا_کن_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    إذا (fp != عدم && (fp->uf_flags & FC_DEAD) == 0)
	أرجع fp;
    أرجع عدم;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    صحيح
func_is_global(ufunc_T *ufunc)
{
    أرجع ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    صحيح
func_requires_g_پیشوند(ufunc_T *ufunc)
{
    أرجع func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == عدم
	    && !امن_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ساكن صحيح
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    صحيح	len;

    إذا (!func_is_global(fp))
	len = vim_snprintf((حرف *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    وإلا
	len = vim_snprintf((حرف *)buf, bufsize, "%s", fp->uf_name);

    أرجع (len >= (صحيح)bufsize) ? (صحيح)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ساكن فراغ
جمع_کن_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    حرف	*name,
    varnumber_T nr)
{
    STRCPY(v->di_کلید, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_جمع_کن(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_عدد;
    v->di_tv.v_قفل = VAR_FIXED;
    v->di_tv.vval.v_عدد = nr;
}

/*
 * Free "fc".
 */
    ساكن فراغ
free_funccal(funccall_T *fc)
{
    صحيح	i;

    لكل (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_داده))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	إذا (fp != عدم && fp->uf_scoped == fc)
	    fp->uf_scoped = عدم;
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
   ساكن فراغ
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
    ساكن فراغ
cleanup_function_call(funccall_T *fc)
{
    صحيح	may_free_fc = fc->fc_refcount <= 0;
    صحيح	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    إذا (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    وإلا
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    إذا (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    وإلا
    {
	صحيح	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (صحيح)fc->fc_l_avars.dv_hashtab.ht_استفاده_شده;
	FOR_ALL_HASHTAB_اقلام(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    إذا (!HASHITEM_خالی(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    إذا (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_اولی = عدم;
    وإلا
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_اقلام(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    إذا (free_fc)
	free_funccal(fc);
    وإلا
    {
	ساكن صحيح made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = قبلی_funccal;
	قبلی_funccal = fc;

	إذا (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	وإلا إذا (++made_copy >= (صحيح)((4096 * 1024) / حجم(*fc)))
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
    ساكن صحيح
numbered_function(char_u *name)
{
    أرجع امن_isdigit(*name)
	    || (name[0] == 'g' && name[1] == ':' && امن_isdigit(name[2]));
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
    صحيح
func_name_refcount(char_u *name)
{
    أرجع numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ساكن فراغ
funccal_unref(funccall_T *fc, ufunc_T *fp, صحيح force)
{
    funccall_T	**pfc;
    صحيح		i;

    إذا (fc == عدم)
	أرجع;

    إذا (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	لكل (pfc = &قبلی_funccal; *pfc != عدم; pfc = &(*pfc)->fc_caller)
	{
	    إذا (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		أرجع;
	    }
	}
    لكل (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	إذا (((ufunc_T **)(fc->fc_ufuncs.ga_داده))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_داده))[i] = عدم;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ساكن صحيح
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    إذا (fp->uf_flags & FC_DEAD)
	أرجع FALSE;

    hi = hash_پیدا_کن(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    إذا (HASHITEM_خالی(hi) || HI2UF(hi) != fp)
	أرجع FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    إذا (fp->uf_def_وضعیت == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	أرجع FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    أرجع TRUE;
}

    ساكن فراغ
func_clear_اقلام(ufunc_T *fp)
{
    ga_clear_رشتهها(&(fp->uf_آرگومانها));
    ga_clear_رشتهها(&(fp->uf_def_آرگومانها));
    ga_clear_رشتهها(&(fp->uf_سطرها));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_بلوک_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    إذا (IS_GENERIC_FUNC(fp))
	generic_func_clear_اقلام(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = عدم;
    fp->uf_refcount -= 3;

#اگر_تعریف_شده FEAT_LUA
    إذا (fp->uf_cb_free != عدم)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = عدم;
    }

    fp->uf_cb_state = عدم;
    fp->uf_cb = عدم;
#پایان_اگر
#اگر_تعریف_شده FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_شمار);
    VIM_CLEAR(fp->uf_tml_مجموع);
    VIM_CLEAR(fp->uf_tml_self);
#پایان_اگر
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ساكن فراغ
func_clear(ufunc_T *fp, صحيح force)
{
    إذا (fp->uf_cleared)
	أرجع;
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
    ساكن صحيح
func_free(ufunc_T *fp, صحيح force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    إذا ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    إذا ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	إذا (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    فراغ
func_clear_free(ufunc_T *fp, صحيح force)
{
    func_clear(fp, force);
    إذا (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    وإلا
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    صحيح
copy_lambda_مقصد_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = پیدا_کن_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = عدم;

    إذا (ufunc == عدم)
    {
	semsg(_(e_lambda_function_not_یافت_str), lambda);
	أرجع FAIL;
    }

    fp = پیدا_کن_func(global, TRUE);
    إذا (fp != عدم)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_جمع_کن_bang_مقصد_replace), global);
	أرجع FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    إذا (fp == عدم)
	أرجع FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_وضعیت = ufunc->uf_def_وضعیت;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    إذا (ga_copy_رشتهها(&ufunc->uf_آرگومانها, &fp->uf_آرگومانها) == FAIL
	    || ga_copy_رشتهها(&ufunc->uf_def_آرگومانها, &fp->uf_def_آرگومانها)
								    == FAIL
	    || ga_copy_رشتهها(&ufunc->uf_سطرها, &fp->uf_سطرها) == FAIL)
	اذهب failed;

    إذا (ufunc->uf_arg_types != عدم)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_آرگومانها.ga_len);
	إذا (fp->uf_arg_types == عدم)
	    اذهب failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    حجم(type_T *) * fp->uf_آرگومانها.ga_len);
    }
    إذا (ufunc->uf_va_name != عدم)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	إذا (fp->uf_va_name == عدم)
	    اذهب failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_جمع_کن(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    إذا ((ufunc->uf_flags & FC_بستگی) && ufunc->uf_partial == عدم)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	إذا (pt == عدم)
	    اذهب failed;
	إذا (fill_partial_and_بستگی(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    اذهب failed;
	}
	ufunc->uf_partial = pt;
    }

    أرجع OK;

failed:
    func_clear_free(fp, TRUE);
    أرجع FAIL;
}

ساكن صحيح	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    صحيح
funcdepth_increment(فراغ)
{
    إذا (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_عمق_is_higher_than_maxfuncdepth));
	أرجع FAIL;
    }
    ++funcdepth;
    أرجع OK;
}

    فراغ
funcdepth_decrement(فراغ)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    صحيح
funcdepth_get(فراغ)
{
    أرجع funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    فراغ
funcdepth_restore(صحيح عمق)
{
    funcdepth = عمق;
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

    إذا (fc == عدم)
	أرجع عدم;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    أرجع fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    فراغ
remove_funccal(فراغ)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ساكن funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    صحيح		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    صحيح		using_sandbox = FALSE;
    صحيح		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    صحيح		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    صحيح		default_arg_err = FALSE;
    dictitem_T	*v;
    صحيح		fixvar_idx = 0;	// index in fc_fixvar[]
    صحيح		i;
    صحيح		ai;
    صحيح		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_مقصد_free[MAX_FUNC_آرگومانها];
    صحيح		tv_مقصد_free_len = 0;
#اگر_تعریف_شده FEAT_PROFILE
    profinfo_T	profile_info;
#پایان_اگر
    ESTACK_CHECK_DECLARATION;

#اگر_تعریف_شده FEAT_PROFILE
    CLEAR_فیلد(profile_info);
#پایان_اگر

    // If depth of calling is getting too high, don't execute the function.
    إذا (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = -1;
	أرجع FCERR_FAILED;
    }

    سطر_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    إذا (fc == عدم)
	أرجع FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_پیدا_کن_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, حجم(ufunc_T *), 1);

    إذا (fp->uf_def_وضعیت != UF_NOT_COMPILED)
    {
	إذا (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#اگر_تعریف_شده FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func;
#پایان_اگر
	// Execute the function, possibly compiling it first.
#اگر_تعریف_شده FEAT_PROFILE
	إذا (do_profiling == PROF_YES)
	    profile_may_شروع_func(&profile_info, fp, caller);
#پایان_اگر
	sticky_cmdmod_flags = 0;
	إذا (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	إذا (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#اگر_تعریف_شده FEAT_PROFILE
	إذا (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != عدم && caller->uf_profiling)))
	    profile_may_انتها_func(&profile_info, fp, caller);
#پایان_اگر
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	أرجع retval;
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
    إذا (selfdict != عدم)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
	STRCPY(name, "self");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_جمع_کن(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "set self dictionary");
	v->di_tv.v_type = VAR_DICT;
	v->di_tv.v_قفل = 0;
	v->di_tv.vval.v_dict = selfdict;
	++selfdict->dv_refcount;
    }

    /*
     * Init a: variables, unless none found (in lambda).
     * Set a:0 to "argcount" less number of named arguments, if >= 0.
     * Set a:000 to a list with room for the "..." arguments.
     */
    init_var_dict(&fc->fc_l_avars, &fc->fc_l_avars_var, VAR_SCOPE);
    إذا ((fp->uf_flags & FC_NOARGS) == 0)
	جمع_کن_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_آرگومانها.ga_len
				    ? argcount - fp->uf_آرگومانها.ga_len : 0));
    fc->fc_l_avars.dv_قفل = VAR_FIXED;
    إذا ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_کلید;
	STRCPY(name, "000");
	v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	hash_جمع_کن(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "function argument");
	v->di_tv.v_type = VAR_LIST;
	v->di_tv.v_قفل = VAR_FIXED;
	v->di_tv.vval.v_list = &fc->fc_l_varlist;
    }
    CLEAR_فیلد(fc->fc_l_varlist);
    fc->fc_l_varlist.lv_refcount = DO_NOT_FREE_CNT;
    fc->fc_l_varlist.lv_قفل = VAR_FIXED;

    /*
     * Set a:firstline to "firstline" and a:lastline to "lastline".
     * Set a:name to named arguments.
     * Set a:N to the "..." arguments.
     * Skipped when no a: variables used (in lambda).
     */
    إذا ((fp->uf_flags & FC_NOARGS) == 0)
    {
	جمع_کن_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	جمع_کن_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    لكل (i = 0; i < argcount || i < fp->uf_آرگومانها.ga_len; ++i)
    {
	صحيح	    addlocal = FALSE;
	typval_T    def_rettv;
	صحيح	    isdefault = FALSE;

	ai = i - fp->uf_آرگومانها.ga_len;
	إذا (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    إذا (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_آرگومانها.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_عدد == VVAL_NONE));
	    إذا (isdefault)
	    {
		char_u	    *default_عبارت = عدم;

		def_rettv.v_type = VAR_عدد;
		def_rettv.vval.v_عدد = -1;

		default_عبارت = ((char_u **)(fp->uf_def_آرگومانها.ga_داده))
						 [ai + fp->uf_def_آرگومانها.ga_len];
		إذا (eval1(&default_عبارت, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    اكسر;
		}
	    }

	    namelen = STRLEN(name);
	}
	وإلا
	{
	    إذا ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		اكسر;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((حرف *)numbuf, حجم(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	إذا (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_کلید, name);
	}
	وإلا
	{
	    v = dictitem_alloc(name);
	    إذا (v == عدم)
		اكسر;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_قفل = VAR_FIXED;

	إذا (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_مقصد_free[tv_مقصد_free_len++] = &v->di_tv;

	إذا (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_جمع_کن(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	وإلا
	    hash_جمع_کن(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	إذا (ai >= 0 && ai < MAX_FUNC_آرگومانها)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_قفل = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    إذا (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_افزودن_ufunc(fp, 1);
    ESTACK_CHECK_راهاندازی;
    إذا (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	إذا (p_verbose >= 14)
	{
	    char_u	buf[پیام_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    پیام_puts("(");
	    لكل (i = 0; i < argcount; ++i)
	    {
		إذا (i > 0)
		    پیام_puts(", ");
		إذا (argvars[i].v_type == VAR_عدد)
		    پیام_outnum((دراز)argvars[i].vval.v_عدد);
		وإلا
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    إذا (s != عدم)
		    {
			إذا (vim_strsize(s) > پیام_BUF_CLEN)
			{
			    trunc_string(s, buf, پیام_BUF_CLEN, پیام_BUF_LEN);
			    s = buf;
			}
			پیام_puts((حرف *)s);
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
#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	profile_may_شروع_func(&profile_info, fp,
			fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func);
#پایان_اگر

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

    إذا (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    وإلا إذا (islambda)
    {
	char_u *p = *(char_u **)fp->uf_سطرها.ga_داده + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(فراغ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    وإلا
	// call do_cmdline() to execute the lines
	do_cmdline(عدم, get_func_سطر, (فراغ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    إذا ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_عدد;
	rettv->vval.v_عدد = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	إذا (درون_vim9script())
	    retval = FCERR_FAILED;
    }

#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func;

	إذا (fp->uf_profiling || (caller != عدم && caller->uf_profiling))
	    profile_may_انتها_func(&profile_info, fp, caller);
    }
#پایان_اگر

    // when being verbose, mention the return value
    إذا (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	إذا (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	وإلا إذا (fc->fc_rettv->v_type == VAR_عدد)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (دراز)fc->fc_rettv->vval.v_عدد);
	وإلا
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
	    إذا (s != عدم)
	    {
		إذا (vim_strsize(s) > پیام_BUF_CLEN)
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

#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_شروع);
#پایان_اگر
    إذا (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    إذا (p_verbose >= 12 && SOURCING_NAME != عدم)
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
    لكل (i = 0; i < tv_مقصد_free_len; ++i)
	clear_tv(tv_مقصد_free[i]);
    cleanup_function_call(fc);

    أرجع retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, صحيح argcount)
{
    صحيح regular_آرگومانها = fp->uf_آرگومانها.ga_len;

    إذا (argcount < regular_آرگومانها - fp->uf_def_آرگومانها.ga_len)
	أرجع FCERR_TOOFEW;
    وإلا إذا (!has_varargs(fp) && argcount > regular_آرگومانها)
	أرجع FCERR_TOOMANY;
    أرجع FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	صحيح	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#اگر_تعریف_شده FEAT_LUA
    إذا (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	أرجع (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#پایان_اگر

    إذا (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != عدم)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    إذا (error != FCERR_UNKNOWN)
	أرجع error;

    إذا ((fp->uf_flags & FC_DICT) && selfdict == عدم)
    {
	error = FCERR_DICT;
    }
    وإلا
    {
	صحيح		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_جستجو_patterns();
	إذا (!ins_compl_فعال())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : عدم);
	إذا (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	إذا (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_جستجو_patterns();
    }

    أرجع error;
}

ساكن funccal_مدخل_T *funccal_پشته = عدم;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    فراغ
save_funccal(funccal_مدخل_T *مدخل)
{
    مدخل->بالاترین_funccal = current_funccal;
    مدخل->بعدی = funccal_پشته;
    funccal_پشته = مدخل;
    current_funccal = عدم;
}

    فراغ
restore_funccal(فراغ)
{
    إذا (funccal_پشته == عدم)
	داخلی_error("restore_funccal()");
    وإلا
    {
	current_funccal = funccal_پشته->بالاترین_funccal;
	funccal_پشته = funccal_پشته->بعدی;
    }
}

    funccall_T *
get_current_funccal(فراغ)
{
    أرجع current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    صحيح
at_script_level(فراغ)
{
    أرجع current_funccal == عدم && autocmd_تطابق == عدم;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    فراغ
delete_script_functions(صحيح sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (صحيح)KE_SNR;
    len = 3 + vim_snprintf((حرف *)buf + 3, حجم(buf) - 3, "%d_", sid);

    طالما (todo > 0)
    {
	todo = func_hashtab.ht_استفاده_شده;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    إذا (!HASHITEM_خالی(hi))
	    {
		fp = HI2UF(hi);
		إذا (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    صحيح تغییریافته = func_hashtab.ht_تغییریافته;

		    fp->uf_flags |= FC_DEAD;

		    إذا (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			إذا (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    وإلا
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			إذا (تغییریافته != func_hashtab.ht_تغییریافته)
			    اكسر;
		    }
		}
		--todo;
	    }
    }
}

#إذا defined(EXITFREE)
    فراغ
free_all_functions(فراغ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    صحيح		تغییریافته;

    // Clean up the current_funccal chain and the funccal stack.
    طالما (current_funccal != عدم)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	إذا (current_funccal == عدم && funccal_پشته != عدم)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    طالما (todo > 0)
    {
	todo = func_hashtab.ht_استفاده_شده;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    إذا (!HASHITEM_خالی(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_وضعیت = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		إذا (func_name_refcount(fp->uf_name))
		    ++skipped;
		وإلا
		{
		    تغییریافته = func_hashtab.ht_تغییریافته;
		    func_clear(fp, TRUE);
		    إذا (تغییریافته != func_hashtab.ht_تغییریافته)
		    {
			skipped = 0;
			اكسر;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    طالما (func_hashtab.ht_استفاده_شده > skipped)
    {
	todo = func_hashtab.ht_استفاده_شده;
	FOR_ALL_HASHTAB_اقلام(&func_hashtab, hi, todo)
	    إذا (!HASHITEM_خالی(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		إذا (func_name_refcount(fp->uf_name))
		    ++skipped;
		وإلا
		{
		    إذا (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			اكسر;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    إذا (skipped == 0)
	hash_clear(&func_hashtab);

    free_def_functions();
}
#پایان_اگر

/*
 * Return TRUE if "name" looks like a builtin function name: starts with a
 * lower case letter, doesn't contain AUTOLOAD_CHAR or ':', no "." after the
 * name.
 * "len" is the length of "name", or -1 for NUL terminated.
 */
    صحيح
builtin_function(char_u *name, صحيح len)
{
    صحيح i;

    إذا (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	أرجع FALSE;
    لكل (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	إذا (name[i] == AUTOLOAD_CHAR)
	    أرجع FALSE;
	إذا (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    إذا (name[i] == '.')
		أرجع FALSE;
	    اكسر;
	}
    }
    أرجع TRUE;
}

    صحيح
func_call(
    char_u	*name,
    typval_T	*آرگومانها,
    partial_T	*partial,
    dict_T	*selfdict,
    typval_T	*rettv)
{
    list_T	*l = آرگومانها->vval.v_list;
    listitem_T	*قلم;
    typval_T	argv[MAX_FUNC_آرگومانها + 1];
    صحيح		argc = 0;
    صحيح		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_اقلام(l, قلم)
    {
	إذا (argc == MAX_FUNC_آرگومانها - (partial == عدم ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    اكسر;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&قلم->li_tv, &argv[argc++]);
    }

    إذا (قلم == عدم)
    {
	funcexe_T	funcexe;
	صحيح		namelen = -1;

	إذا (درون_vim9script())
	{
	    char_u *p = generic_func_پیدا_کن_open_bracket(name);

	    إذا (p != عدم)
	    {
		إذا (skip_generic_func_type_آرگومانها(&p) == FAIL)
		    اذهب انجامشده;
		namelen = p - name + 1;
	    }
	}

	CLEAR_فیلد(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	إذا (partial != عدم)
	{
	    funcexe.fe_object = partial->pt_obj;
	    إذا (funcexe.fe_object != عدم)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    طالما (argc > 0)
	clear_tv(&argv[--argc]);

انجامشده:
    أرجع r;
}

ساكن صحيح callback_عمق = 0;

    صحيح
get_callback_عمق(فراغ)
{
    أرجع callback_عمق;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    صحيح
call_callback(
    callback_T	*callback,
    صحيح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    صحيح		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    صحيح		ret;

    إذا (callback->cb_name == عدم || *callback->cb_name == NUL)
	أرجع FAIL;

    إذا (callback_عمق > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	أرجع FAIL;
    }

    CLEAR_فیلد(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    إذا (callback->cb_partial != عدم)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	إذا (funcexe.fe_object != عدم)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_عمق;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_عمق;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    إذا (need_rethrow && current_exception != عدم && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    أرجع ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    صحيح		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    إذا (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	أرجع -2;

    retval = tv_get_عدد_chk(&rettv, عدم);
    clear_tv(&rettv);
    أرجع retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    فراغ
user_func_error(funcerror_T error, char_u *name, صحيح یافت_var)
{
    بدل (error)
    {
	حالة FCERR_UNKNOWN:
		إذا (یافت_var)
		    emsg_funcname(e_not_callable_type_str, name);
		وإلا
		    emsg_funcname(e_unknown_function_str, name);
		اكسر;
	حالة FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_روش_str, name);
		اكسر;
	حالة FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		اكسر;
	حالة FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		اكسر;
	حالة FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		اكسر;
	حالة FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_درون_script_context_str, name);
		اكسر;
	حالة FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		اكسر;
	حالة FCERR_OTHER:
	حالة FCERR_FAILED:
		// assume the error message was already given
		اكسر;
	حالة FCERR_NONE:
		اكسر;
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
    ساكن funcerror_T
may_check_آرگومان_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	صحيح	    argcount,
	صحيح	    پایه_included,
	char_u	    *name)
{
    إذا (funcexe->fe_check_type != عدم && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	إذا (check_آرگومان_types(funcexe->fe_check_type,
			  argvars, argcount,
			  پایه_included ? عدم : funcexe->fe_basetv,
			  name) == FAIL)
	    أرجع FCERR_OTHER;
    }
    أرجع FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    صحيح
call_func(
    char_u	*funcname,	// name of the function
    صحيح		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    صحيح		argcount_درون,	// number of "argvars"
    typval_T	*argvars_درون,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    صحيح		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    صحيح		i;
    ufunc_T	*fp = عدم;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = عدم;
    char_u	*fname = عدم;
    char_u	*name = عدم;
    صحيح		argcount = argcount_درون;
    typval_T	*argvars = argvars_درون;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_آرگومانها + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    صحيح		argv_clear = 0;
    صحيح		argv_پایه = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_آرگومانها[MAX_FUNC_آرگومانها];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_آرگومانها_جدول_init(&gfatab);

    إذا (partial != عدم)
	fp = partial->pt_func;
    إذا (fp == عدم)
	fp = funcexe->fe_ufunc;

    إذا (fp == عدم)
    {
	إذا (درون_vim9script())
	{
	    char_u *p = generic_func_پیدا_کن_open_bracket(funcname);

	    إذا (p != عدم)
	    {
		len = p - funcname;
		إذا (parse_generic_func_type_آرگومانها(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == عدم)
		    اذهب theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	إذا (name == عدم)
	    اذهب theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    إذا (funcexe->fe_doesrange != عدم)
	*funcexe->fe_doesrange = FALSE;

    إذا (partial != عدم)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	إذا (partial->pt_dict != عدم && (selfdict == عدم || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	إذا (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    لكل (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		إذا (argv_clear + argcount_درون >= MAX_FUNC_آرگومانها)
		{
		    error = FCERR_TOOMANY;
		    اذهب theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    لكل (i = 0; i < argcount_درون; ++i)
		argv[i + argv_clear] = argvars_درون[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_درون;

	    إذا (funcexe->fe_check_type != عدم
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_آرگومانها = check_type_آرگومانها;
		CLEAR_فیلد(check_type_آرگومانها);
		لكل (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_آرگومانها[i + partial->pt_argc] =
							 check_type.tt_آرگومانها[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    إذا (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_آرگومان_types(funcexe, argvars, argcount, FALSE,
					     (name != عدم) ? name : funcname);

    إذا (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	صحيح	is_global = FALSE;

	// Skip "g:" before a function name.
	إذا (fp == عدم && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_عدد;	// default rettv is number zero
	rettv->vval.v_عدد = 0;
	error = FCERR_UNKNOWN;

	إذا (fp != عدم || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    إذا (fp == عدم)
	    {
		fp = پیدا_کن_func(rfname, is_global);
		إذا (fp != عدم && !is_global && درون_vim9script()
						 && func_requires_g_پیشوند(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = عدم;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    إذا (fp == عدم
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, عدم)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = پیدا_کن_func(rfname, is_global);
	    }
	    // Try loading a package.
	    إذا (fp == عدم && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = پیدا_کن_func(rfname, is_global);
	    }
	    إذا (fp == عدم)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		إذا (p != عدم && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = پیدا_کن_func(p, is_global);
	    }

	    إذا (partial == عدم && fp != عدم && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		إذا (fp == عدم)
		{
		    error = FCERR_FAILED;
		    اذهب theend;
		}
	    }
	    وإلا إذا (generic_func_آرگومانها_جدول_اندازه(&gfatab) > 0)
	    {
		emsg_funcname(fp != عدم ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		اذهب theend;
	    }

	    إذا (fp != عدم && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    وإلا إذا (fp != عدم)
	    {
		صحيح need_arg_check = FALSE;
		إذا (funcexe->fe_check_type == عدم)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		إذا (funcexe->fe_argv_func != عدم)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		إذا (funcexe->fe_basetv != عدم)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, حجم(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_پایه = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		إذا (need_arg_check)
		    error = may_check_آرگومان_types(funcexe, argvars, argcount,
				       TRUE, (name != عدم) ? name : funcname);

		إذا (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	وإلا إذا (funcexe->fe_basetv != عدم)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_داخلی_روش(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	وإلا
	{
	    /*
	     * Find the function name in the table, call its implementation.
	     */
	    error = call_داخلی_func(fname, argcount, argvars, rettv);
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
    إذا (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    إذا (!aborting())
	user_func_error(error, (name != عدم) ? name : funcname,
							funcexe->fe_یافت_var);

    // clear the copies made from the partial
    طالما (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_پایه]);

    vim_free(tofree);
    vim_free(name);
    generic_func_آرگومانها_جدول_clear(&gfatab);

    أرجع ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    صحيح
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    صحيح		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = عدم;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    صحيح		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_عدد;	// default rettv is number zero
    rettv->vval.v_عدد = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    إذا (name == عدم)
	أرجع ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    إذا (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    وإلا
	rfname = fname;
    fp = پیدا_کن_func(rfname, is_global);
    إذا (fp != عدم && !is_global && درون_vim9script()
						 && func_requires_g_پیشوند(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = عدم;
    إذا (fp == عدم)
	ret = NOTDONE;
    وإلا إذا (fp != عدم && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    وإلا إذا (fp != عدم)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_فیلد(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, عدم);
	إذا (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    أرجع ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    أرجع fp->uf_name_exp != عدم ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ساكن صحيح
function_list_modified(صحيح prev_ht_تغییریافته)
{
    إذا (prev_ht_تغییریافته != func_hashtab.ht_تغییریافته)
    {
	emsg(_(e_function_list_was_modified));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ساكن صحيح
list_func_سر(ufunc_T *fp, صحيح indent)
{
    صحيح		prev_ht_تغییریافته = func_hashtab.ht_تغییریافته;
    صحيح		j;

    پیام_شروع();

    // a timer at the more prompt may have deleted the function
    إذا (function_list_modified(prev_ht_تغییریافته))
	أرجع FAIL;

    إذا (indent)
	پیام_puts("   ");
    إذا (fp->uf_def_وضعیت != UF_NOT_COMPILED)
	پیام_puts("def ");
    وإلا
	پیام_puts("function ");
    پیام_puts((حرف *)printable_func_name(fp));
    پیام_putchar('(');
    لكل (j = 0; j < fp->uf_آرگومانها.ga_len; ++j)
    {
	إذا (j)
	    پیام_puts(", ");
	پیام_puts((حرف *)FUNCARG(fp, j));
	إذا (fp->uf_arg_types != عدم)
	{
	    حرف *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	إذا (j >= fp->uf_آرگومانها.ga_len - fp->uf_def_آرگومانها.ga_len)
	{
	    پیام_puts(" = ");
	    پیام_puts(((حرف **)(fp->uf_def_آرگومانها.ga_داده))
		       [j - fp->uf_آرگومانها.ga_len + fp->uf_def_آرگومانها.ga_len]);
	}
    }
    إذا (fp->uf_varargs)
    {
	إذا (j)
	    پیام_puts(", ");
	پیام_puts("...");
    }
    إذا (fp->uf_va_name != عدم)
    {
	إذا (!fp->uf_varargs)
	{
	    إذا (j)
		پیام_puts(", ");
	    پیام_puts("...");
	}
	پیام_puts((حرف *)fp->uf_va_name);
	إذا (fp->uf_va_type != عدم)
	{
	    حرف *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    پیام_putchar(')');

    إذا (fp->uf_def_وضعیت != UF_NOT_COMPILED)
    {
	إذا (fp->uf_ret_type != &t_void)
	{
	    حرف *tofree;

	    پیام_puts(": ");
	    پیام_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    وإلا إذا (fp->uf_flags & FC_ABORT)
	پیام_puts(" abort");
    إذا (fp->uf_flags & FC_RANGE)
	پیام_puts(" range");
    إذا (fp->uf_flags & FC_DICT)
	پیام_puts(" dict");
    إذا (fp->uf_flags & FC_بستگی)
	پیام_puts(" closure");
    پیام_clr_eos();
    إذا (p_verbose > 0)
	آخرین_مجموعه_پیام(fp->uf_script_ctx);

    أرجع OK;
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
    صحيح		*is_global,
    صحيح		skip,		// only find the end, don't evaluate
    صحيح		flags)
{
    أرجع trans_function_name_ext(pp, is_global, skip, flags,
	    عدم, عدم, عدم, عدم);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ساكن char_u *
trans_function_name_ext(
    char_u	**pp,
    صحيح		*is_global,
    صحيح		skip,		// only find the end, don't evaluate
    صحيح		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = عدم;
    char_u	*شروع;
    char_u	*انتها;
    صحيح		lead;
    char_u	sid_buf[20];
    صحيح		len;
    صحيح		extra = 0;
    صحيح		پیشوند_g = FALSE;
    lval_T	lv;
    صحيح		vim9script = درون_vim9script();
    صحيح		vim9_local;

    إذا (fdp != عدم)
	CLEAR_اشاره_گر(fdp);
    شروع = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    إذا ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (صحيح)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	أرجع vim_strnsave(شروع, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(شروع);
    إذا (lead > 2)
	شروع += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    انتها = get_lval(شروع, عدم, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_شروع);
    إذا (انتها == شروع || (vim9script && انتها != عدم
				   && انتها[-1] == AUTOLOAD_CHAR && *انتها == '('))
    {
	إذا (!skip)
	    emsg(_(e_function_name_required));
	اذهب theend;
    }
    إذا (انتها == عدم || (lv.ll_tv != عدم && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	إذا (!aborting())
	{
	    إذا (انتها != عدم)
		semsg(_(e_invalid_آرگومان_str), شروع);
	}
	وإلا
	    *pp = پیدا_کن_name_انتها(شروع, عدم, عدم, FNE_INCL_BR);
	اذهب theend;
    }

    إذا (lv.ll_ufunc != عدم)
    {
	إذا (ufunc != عدم)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = انتها;
	اذهب theend;
    }

    إذا (lv.ll_tv != عدم)
    {
	إذا (fdp != عدم)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = عدم;
	    fdp->fd_di = lv.ll_di;
	}
	إذا (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != عدم)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = انتها;
	}
	وإلا إذا (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != عدم)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.درازی);
	    *pp = انتها;
	}
	وإلا إذا (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != عدم)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = انتها;
	    إذا (partial != عدم)
		*partial = lv.ll_tv->vval.v_partial;
	}
	وإلا
	{
	    إذا (!skip && !(flags & TFN_QUIET) && (fdp == عدم
			     || lv.ll_dict == عدم || fdp->fd_newkey == عدم))
		emsg(_(e_funcref_required));
	    وإلا
		*pp = انتها;
	    name = عدم;
	}
	اذهب theend;
    }

    إذا (lv.ll_name == عدم)
    {
	// Error found, but continue after the function name.
	*pp = انتها;
	اذهب theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    إذا (lv.ll_exp_name != عدم)
    {
	len = (صحيح)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, عدم);
	إذا (name == lv.ll_exp_name)
	    name = عدم;
    }
    وإلا إذا (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_قلم(lv.ll_sid);
	صحيح		cc = *lv.ll_name_انتها;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_انتها = NUL;
	إذا (si->sn_autoload_پیشوند != عدم)
	{
	    name = concat_str(si->sn_autoload_پیشوند, lv.ll_name);
	}
	وإلا
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (صحيح)KE_SNR;
	    vim_snprintf((حرف *)sid_buf + 3, حجم(sid_buf) - 3,
						      "%ld_", (دراز)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_انتها = cc;
	*pp = انتها;
	اذهب theend;
    }
    وإلا إذا (!(flags & TFN_NO_DEREF))
    {
	len = (صحيح)(انتها - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, عدم);
	إذا (name == *pp)
	    name = عدم;
    }
    إذا (name != عدم)
    {
	name = vim_strsave(name);
	*pp = انتها;
	إذا (name != عدم && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحيح)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	اذهب theend;
    }

    إذا (lv.ll_exp_name != عدم)
    {
	len = (صحيح)STRLEN(lv.ll_exp_name);
	إذا (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    وإلا
    {
	// skip over "s:" and "g:"
	إذا (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    إذا (lv.ll_name[0] == 'g')
	    {
		إذا (is_global != عدم)
		{
		    *is_global = TRUE;
		}
		وإلا
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    پیشوند_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (صحيح)(انتها - lv.ll_name);
    }
    إذا (len <= 0)
    {
	إذا (!skip)
	    emsg(_(e_function_name_required));
	اذهب theend;
    }

    // In Vim9 script a user function is script-local by default, unless it
    // starts with a lower case character: dict.func().  Or when in a class.
    vim9_local = ASCII_ISUPPER(*شروع) && vim9script
						&& (flags & TFN_درون_CLASS) == 0;

    /*
     * Copy the function name to allocated memory.
     * Accept <SID>name() inside a script, translate into <SNR>123_name().
     * Accept <SNR>123_name() outside a script.
     */
    إذا (skip)
	lead = 0;	// do nothing
    وإلا إذا (lead > 0 || vim9_local)
    {
	إذا (!vim9_local)
	{
	    إذا (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_شروع_with_capital_str), شروع);
		اذهب theend;
	    }
	    lead = 3;
	}
	إذا (vim9_local || (lv.ll_exp_name != عدم
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    إذا (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_درون_script_context));
		اذهب theend;
	    }
	    sid_buflen = vim_snprintf((حرف *)sid_buf, حجم(sid_buf), "%ld_", (دراز)current_sctx.sc_sid);
	    إذا (vim9_local)
		extra = 3 + (صحيح)sid_buflen;
	    وإلا
		lead += (صحيح)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    وإلا إذا (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_درون_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_شروع_with_capital_str
			   : e_function_name_must_شروع_with_capital_or_s_str),
									شروع);
	اذهب theend;
    }
    إذا (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	إذا (cp != عدم && cp < انتها)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), شروع);
	    اذهب theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    إذا (name != عدم)
    {
	إذا (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحيح)KE_SNR;
	    إذا (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	وإلا إذا (پیشوند_g)
	{
	    name[0] = 'g';
	    name[1] = ':';
	}
	mch_memmove(name + lead + extra, lv.ll_name, (size_t)len);
	name[lead + extra + len] = NUL;
    }
    *pp = انتها;

theend:
    clear_lval(&lv);
    أرجع name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ساكن char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    إذا (*name == K_SPECIAL && درون_vim9script())
    {
	p = vim_strchr(name, '_');
	إذا (p != عدم)
	    أرجع p + 1;
    }
    أرجع عدم;
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
    حرف	sid_buf[25];
    size_t	sid_buflen;
    صحيح		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    إذا (funcname == عدم)
	أرجع عدم;

    إذا (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	إذا (!درون_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    أرجع عدم;
	ufunc = پیدا_کن_func(funcname, FALSE);
	إذا (ufunc == عدم || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == عدم)
	    أرجع عدم;
	++p;
	off = 0;
    }
    وإلا
	off = *funcname == 's' ? 2 : 5;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_درون_script_context));
	أرجع عدم;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, حجم(sid_buf), "<SNR>%ld_",
	    (دراز)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    إذا (newname == عدم)
	أرجع عدم;
    vim_snprintf((حرف *)newname, newnamesize, "%s%s", sid_buf, p + off);

    أرجع newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    إذا (n != عدم)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    أرجع n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	صحيح	    *is_global,
	صحيح	    skip,
	صحيح	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    إذا (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(فراغ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	إذا (fudi != عدم)
	    CLEAR_اشاره_گر(fudi);
    }
    وإلا
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, عدم, عدم, عدم);
    *name = p;
    أرجع saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    فراغ
list_functions(regmatch_T *regmatch)
{
    صحيح		prev_ht_تغییریافته = func_hashtab.ht_تغییریافته;
    long_u	todo = func_hashtab.ht_استفاده_شده;
    hashitem_T	*hi;

    لكل (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	إذا (!HASHITEM_خالی(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    إذا ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == عدم
			? !پیام_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !امن_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		إذا (list_func_سر(fp, FALSE) == FAIL)
		    أرجع;
		إذا (function_list_modified(prev_ht_تغییریافته))
		    أرجع;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ساكن char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    إذا (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	إذا (regmatch.regprog != عدم)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    إذا (*p == '/')
	++p;

    أرجع p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ساكن ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, صحيح is_global)
{
    ufunc_T	*fp = عدم;
    صحيح		j;

    إذا (!دوسر_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	أرجع عدم;
    }

    مجموعه_nextcmd(eap, p);

    إذا (eap->nextcmd != عدم)
	*p = NUL;

    إذا (eap->skip || got_int)
	أرجع عدم;

    fp = پیدا_کن_func(name, is_global);
    إذا (fp == عدم && ASCII_ISUPPER(*eap->arg))
    {
	char_u *به_بالا = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	إذا (به_بالا != عدم)
	    fp = پیدا_کن_func(به_بالا, FALSE);
    }

    إذا (fp == عدم)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	أرجع عدم;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    صحيح prev_ht_تغییریافته = func_hashtab.ht_تغییریافته;

    إذا (list_func_سر(fp, TRUE) != OK)
	أرجع fp;

    لكل (j = 0; j < fp->uf_سطرها.ga_len && !got_int; ++j)
    {
	إذا (FUNCLINE(fp, j) == عدم)
	    استمر;
	پیام_putchar('\n');
	پیام_outnum((دراز)(j + 1));
	إذا (j < 9)
	    پیام_putchar(' ');
	إذا (j < 99)
	    پیام_putchar(' ');
	إذا (function_list_modified(prev_ht_تغییریافته))
	    اكسر;
	پیام_prt_سطر(FUNCLINE(fp, j), FALSE);
	بیرون_flush();	// show a line at a time
	ui_breakcheck();
    }

    إذا (!got_int)
    {
	پیام_putchar('\n');
	إذا (!function_list_modified(prev_ht_تغییریافته))
	{
	    إذا (fp->uf_def_وضعیت != UF_NOT_COMPILED)
		پیام_puts("   enddef");
	    وإلا
		پیام_puts("   endfunction");
	}
    }

    أرجع fp;
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
	garray_T    *سطرها_مقصد_free,
	صحيح	    class_flags,
	ocmember_T  *obj_members,
	صحيح         obj_member_شمار,
	cctx_T	    *cctx)
{
    صحيح		j;
    صحيح		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    صحيح		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*سطر_arg = عدم;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_آرگومانها;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    صحيح		varargs = FALSE;
    صحيح		flags = 0;
    char_u	*ret_type = عدم;
    ufunc_T	*fp = عدم;
    صحيح		fp_allocated = FALSE;
    صحيح		free_fp = FALSE;
    صحيح		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ساكن صحيح	func_nr = 0;	    // number for nameless function
    صحيح		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_بالاترین;
    صحيح		vim9script = درون_vim9script();
    imported_T	*import = عدم;

    // ":function" without argument: list functions.
    إذا (دوسر_excmd2(eap->cmd, eap->arg))
    {
	إذا (!eap->skip)
	    list_functions(عدم);
	مجموعه_nextcmd(eap, eap->arg);
	أرجع عدم;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    إذا (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	مجموعه_nextcmd(eap, p);
	أرجع عدم;
    }

    ga_init(&newargs);
    ga_init(&argtypes);
    ga_init(&arg_objm);
    ga_init(&default_آرگومانها);
    generic_func_آرگومانها_جدول_init(&gfatab);

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
    إذا (name_arg != عدم)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_فیلد(fudi);
    }
    وإلا
    {
	إذا (vim9script)
	{
	    إذا (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_درون_vim9_script_str), p);
		أرجع عدم;
	    }
	    p = مقصد_name_انتها(p, TRUE);
	    إذا (*skipwhite(p) == '.' && vim_strchr(p, '(') != عدم)
	    {
		semsg(_(e_cannot_define_dict_func_درون_vim9_script_str),
								     eap->arg);
		أرجع عدم;
	    }

	    p = eap->arg;
	}

	صحيح tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_درون_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != عدم);
	إذا (name == عدم && (fudi.fd_dict == عدم || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    إذا (!aborting())
	    {
		إذا (!eap->skip && fudi.fd_newkey != عدم)
		    semsg(_(e_کلید_not_present_درون_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		أرجع عدم;
	    }
	    وإلا
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	إذا (is_export && name != عدم)
	{
	    char_u *prefixed = may_پیشوند_autoload(name);

	    إذا (prefixed != عدم && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	وإلا إذا (paren && vim9script && name != عدم
				    && vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    emsg(_(e_cannot_use_name_with_hash_درون_vim9_script_use_export_instead));
	    اذهب ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    إذا (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	اذهب ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    إذا (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	إذا (p == عدم)
	    اذهب ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    إذا (*p != '(')
    {
	إذا (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    اذهب ret_free;
	}
	// attempt to continue by skipping some text
	إذا (vim_strchr(p, '(') != عدم)
	    p = vim_strchr(p, '(');
    }

    إذا ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	اذهب ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    إذا (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	اذهب ret_free;
    }

    ga_init2(&newlines, حجم(char_u *), 10);

    إذا (!eap->skip && name_arg == عدم)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	إذا (name != عدم)
	    arg = name;
	وإلا
	    arg = fudi.fd_newkey;
	إذا (arg != عدم && (fudi.fd_di == عدم
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_پایه = arg;
	    صحيح	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    إذا (arg != fudi.fd_newkey)
	    {
		إذا (*arg == K_SPECIAL)
		{
		    name_پایه = vim_strchr(arg, '_');
		    إذا (name_پایه == عدم)
			name_پایه = arg + 3;
		    وإلا
			++name_پایه;
		}
		لكل (i = 0; name_پایه[i] != NUL && (i == 0
					    ? eval_isnamec1(name_پایه[i])
					    : eval_isnamec(name_پایه[i])); ++i)
		    ;
		إذا (name_پایه[i] != NUL)
		{
		    emsg_funcname(e_invalid_آرگومان_str, arg);
		    اذهب ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		إذا (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_پایه, i, 0, عدم,
			عدم, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_قلم_str), name_پایه);
		    اذهب ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	إذا (fudi.fd_dict != عدم && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_اینجا));
	    اذهب ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    إذا (get_function_آرگومانها(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : عدم, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : عدم,
			 عدم, &varargs, &default_آرگومانها, eap->skip,
			 eap, class_flags, &newlines, سطرها_مقصد_free) == FAIL)
	اذهب errret_2;
    whitep = p;

    إذا (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	إذا (*skipwhite(p) == ':')
	{
	    إذا (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    وإلا إذا (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    إذا (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    وإلا
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = عدم;
	    }
	}
	p = skipwhite(p);
    }
    وإلا
	// find extra arguments "range", "dict", "abort" and "closure"
	لكل (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    إذا (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    وإلا إذا (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    وإلا إذا (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    وإلا إذا (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_بستگی;
		p += 7;
		إذا (current_funccal == عدم)
		{
		    emsg_funcname(e_بستگی_function_should_not_be_at_بالاترین_level_str,
			    name == عدم ? (char_u *)"" : name);
		    اذهب erret;
		}
	    }
	    وإلا
		اكسر;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    إذا (*p == '\n')
	سطر_arg = p + 1;
    وإلا إذا (*p != NUL
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
    إذا (کلید_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	إذا (!eap->skip && !eap->forceit)
	{
	    إذا (fudi.fd_dict != عدم && fudi.fd_newkey == عدم)
		emsg(_(e_dictionary_مدخل_already_exists));
	    وإلا إذا (name != عدم && پیدا_کن_func(name, is_global) != عدم)
		emsg_funcname(e_function_str_already_exists_جمع_کن_bang_مقصد_replace, name);
	}

	إذا (!eap->skip && did_emsg)
	    اذهب erret;

	پیام_putchar('\n');	    // don't overwrite the function name
	cmdline_ردیف = پیام_ردیف;
    }

    // Save the starting line number.
    sourcing_lnum_بالاترین = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    إذا (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_روش) == 0
		&& (get_function_body(eap, &newlines, سطر_arg, سطرها_مقصد_free)
							== FAIL || did_emsg))
	    || eap->skip)
	اذهب erret;

    /*
     * If there are no errors, add the function
     */
    إذا (fudi.fd_dict != عدم)
    {
	حرف	numbuf[NUMBUFLEN];

	fp = عدم;
	إذا (fudi.fd_newkey == عدم && !eap->forceit)
	{
	    emsg(_(e_dictionary_مدخل_already_exists));
	    اذهب erret;
	}
	إذا (fudi.fd_di == عدم)
	{
	    // Can't add a function to a locked dictionary
	    إذا (مقدار_check_قفل(fudi.fd_dict->dv_قفل, eap->arg, FALSE))
		اذهب erret;
	}
	    // Can't change an existing function if it is locked
	وإلا إذا (مقدار_check_قفل(fudi.fd_di->di_tv.v_قفل, eap->arg, FALSE))
	    اذهب erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, حجم(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	إذا (name == عدم)
	    اذهب erret;
    }
    وإلا إذا (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*پیدا_کن_name = name;
	صحيح		var_conflict = FALSE;
	صحيح		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = پیدا_کن_var(name, &ht, TRUE);
	إذا (v != عدم && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	إذا (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_قلم(current_sctx.sc_sid);

	    إذا (si->sn_autoload_پیشوند != عدم)
	    {
		إذا (is_export)
		{
		    پیدا_کن_name = name + STRLEN(si->sn_autoload_پیشوند);
		    v = پیدا_کن_var(پیدا_کن_name, &ht, TRUE);
		    إذا (v != عدم)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		وإلا
		{
		    char_u *prefixed = may_پیشوند_autoload(name);

		    إذا (prefixed != عدم && prefixed != name)
		    {
			v = پیدا_کن_var(prefixed, &ht, TRUE);
			إذا (v != عدم)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	إذا (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    اذهب erret;
	}

	fp = پیدا_کن_func_even_dead(پیدا_کن_name, ffed_flags);
	إذا (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = پیدا_کن_imported(uname == عدم ? name : uname, 0, FALSE);
	}

	إذا (fp != عدم || import != عدم)
	{
	    صحيح dead = fp != عدم && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    إذا (import != عدم
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_بالاترین;
		إذا (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		وإلا
		    emsg_funcname(e_function_str_already_exists_جمع_کن_bang_مقصد_replace, name);
		اذهب errret_keep;
	    }
	    إذا (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_درون_use, name);
		اذهب errret_keep;
	    }
	    إذا (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = عدم;
		overwrite = TRUE;
	    }
	    وإلا
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = عدم;
		func_clear_اقلام(fp);
		fp->uf_name_exp = exp_name;
		fp->uf_flags &= ~FC_DEAD;
#اگر_تعریف_شده FEAT_PROFILE
		fp->uf_profiling = FALSE;
		fp->uf_prof_initialized = FALSE;
#پایان_اگر
		fp->uf_def_وضعیت = UF_NOT_COMPILED;
	    }
	}
    }

    إذا (fp == عدم)
    {
	إذا (fudi.fd_dict == عدم && vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    صحيح	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    إذا (SOURCING_NAME != عدم)
	    {
		scriptname = autoload_name(name);
		إذا (scriptname != عدم)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (صحيح)STRLEN(p);
		    slen = (صحيح)STRLEN(SOURCING_NAME);
		    إذا (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    إذا (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_بالاترین;
		semsg(_(e_function_name_does_not_تطابق_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		اذهب erret;
	    }
	}

	إذا (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	إذا (fp == عدم)
	    اذهب erret;
	fp_allocated = TRUE;

	إذا (fudi.fd_dict != عدم)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    إذا (func_name == عدم)
	    {
		VIM_CLEAR(fp);
		اذهب erret;
	    }
	    إذا (fudi.fd_di == عدم)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		إذا (fudi.fd_di == عدم)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    اذهب erret;
		}
		إذا (dict_جمع_کن(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    اذهب erret;
		}
	    }
	    وإلا
		// overwrite existing dict entry
		clear_tv(&fudi.fd_di->di_tv);
	    fudi.fd_di->di_tv.v_type = VAR_FUNC;
	    fudi.fd_di->di_tv.vval.v_string = func_name;

	    // behave like "dict" was used
	    flags |= FC_DICT;
	}
    }
    fp->uf_آرگومانها = newargs;
    fp->uf_def_آرگومانها = default_آرگومانها;
    fp->uf_ret_type = &t_any;
    fp->uf_func_type = &t_func_any;

    إذا (eap->cmdidx == CMD_def)
    {
	صحيح	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_وضعیت = UF_مقصد_BE_COMPILED;

	إذا (generic_func_آرگومانها_جدول_اندازه(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_بالاترین;

	// The function may use script variables from the context.
	function_using_بلوک_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	صحيح save_is_export = is_export;
	is_export = FALSE;

	إذا (parse_آرگومان_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_شمار, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    اذهب erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	إذا (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    اذهب erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    وإلا
	fp->uf_def_وضعیت = UF_NOT_COMPILED;

    إذا (fp_allocated)
    {
	// insert the new function in the function list
	إذا (overwrite)
	{
	    hi = hash_پیدا_کن(&func_hashtab, name);
	    hi->hi_کلید = UF2HIKEY(fp);
	}
	وإلا إذا (class_flags == 0 && hash_جمع_کن(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    اذهب erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_سطرها = newlines;
    newlines.ga_داده = عدم;
    إذا ((flags & FC_بستگی) != 0)
    {
	إذا (ثبات_بستگی(fp) == FAIL)
	    اذهب erret;
    }
    وإلا
	fp->uf_scoped = عدم;

#اگر_تعریف_شده FEAT_PROFILE
    إذا (prof_def_func())
	func_do_profile(fp);
#پایان_اگر
    fp->uf_varargs = varargs;
    إذا (sandbox)
	flags |= FC_SANDBOX;
    إذا (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_نسخه = current_sctx.sc_نسخه;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_بالاترین;
    إذا (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    إذا (eap->cmdidx == CMD_def)
	مجموعه_function_type(fp);
    وإلا إذا (fp->uf_script_ctx.sc_نسخه == SCRIPT_نسخه_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_نسخه = SCRIPT_نسخه_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    إذا (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, عدم);

    اذهب ret_free;

erret:
    إذا (fp != عدم)
    {
	// these were set to "newargs" and "default_args", which are cleared
	//زیر
	ga_init(&fp->uf_آرگومانها);
	ga_init(&fp->uf_def_آرگومانها);
    }
errret_2:
    إذا (fp != عدم)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	إذا (IS_GENERIC_FUNC(fp))
	    generic_func_clear_اقلام(fp);
    }
    إذا (free_fp)
	VIM_CLEAR(fp);
errret_keep:
    ga_clear_رشتهها(&newargs);
    ga_clear_رشتهها(&default_آرگومانها);
    ga_clear_رشتهها(&newlines);
ret_free:
    ga_clear_رشتهها(&argtypes);
    ga_clear(&arg_objm);
    // The generic types are still in use and should not be freed.  Instead
    // clear the grow array.
    ga_clear(&gfatab.gfat_param_types);
    generic_func_آرگومانها_جدول_clear(&gfatab);
    vim_free(fudi.fd_newkey);
    إذا (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    أرجع fp;
}

/*
 * ":function"
 */
    فراغ
ex_function(exarg_T *eap)
{
    garray_T سطرها_مقصد_free;

    ga_init2(&سطرها_مقصد_free, حجم(char_u *), 50);
    (فراغ)define_function(eap, عدم, &سطرها_مقصد_free, 0, عدم, 0, عدم);
    ga_clear_رشتهها(&سطرها_مقصد_free);
}

    صحيح
get_func_arity(char_u *name, صحيح *required, صحيح *optional, صحيح *varargs)
{
    ufunc_T	*ufunc = عدم;
    صحيح		argcount = 0;
    صحيح		min_argcount = 0;
    صحيح		idx;

    idx = پیدا_کن_داخلی_func(name);
    إذا (idx >= 0)
    {
	داخلی_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    وإلا
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = عدم;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	إذا (error == FCERR_NONE)
	    ufunc = پیدا_کن_func(fname, FALSE);
	vim_free(tofree);

	إذا (ufunc == عدم)
	    أرجع FAIL;

	argcount = ufunc->uf_آرگومانها.ga_len;
	min_argcount = ufunc->uf_آرگومانها.ga_len - ufunc->uf_def_آرگومانها.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    أرجع OK;
}

/*
 * Find a function by name, including "<lambda>123".
 * Check for "profile" and "debug" arguments and set"compile_type".
 * Caller should initialize "compile_type" to CT_NONE.
 * Return NULL if not found.
 */
    ufunc_T *
پیدا_کن_func_by_name(char_u *name, compiletype_T *compile_type)
{
    char_u	*arg = name;
    char_u	*fname;
    ufunc_T	*ufunc;
    صحيح		is_global = FALSE;
    char_u	*bracket_شروع = عدم;
    char_u	*generic_func_name = name;

    إذا (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    وإلا إذا (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    إذا (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(فراغ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    وإلا
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = پیدا_کن_class_func(&arg);
	إذا (ufunc != عدم)
	    أرجع ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      عدم, عدم, عدم, &ufunc);
	إذا (ufunc != عدم)
	{
	    vim_free(fname);
	    أرجع ufunc;
	}
	إذا (fname != عدم && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_شروع = arg;
	    إذا (skip_generic_func_type_آرگومانها(&arg) == FAIL)
	    {
		vim_free(fname);
		أرجع عدم;
	    }
	}
    }
    إذا (fname == عدم)
    {
	semsg(_(e_invalid_آرگومان_str), name);
	أرجع عدم;
    }
    إذا (!دوسر_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	أرجع عدم;
    }

    ufunc = پیدا_کن_func(fname, is_global);
    إذا (ufunc == عدم)
    {
	char_u *p = untrans_function_name(fname);

	إذا (p != عدم)
	    // Try again without making it script-local.
	    ufunc = پیدا_کن_func(p, FALSE);

	إذا (ufunc == عدم)
	{
	    bracket_شروع = generic_func_پیدا_کن_open_bracket(fname);
	    إذا (bracket_شروع != عدم)
	    {
		generic_func_name = fname;
		*bracket_شروع = NUL;
		ufunc = پیدا_کن_func(fname, FALSE);
		*bracket_شروع = '<';
	    }
	}
    }

    إذا (ufunc != عدم)
    {
	// handle generic functions
	إذا (bracket_شروع == عدم)
	    bracket_شروع = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_شروع);
    }

    vim_free(fname);

    إذا (ufunc == عدم)
	semsg(_(e_cannot_پیدا_کن_function_str), name);
    أرجع ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    فراغ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    إذا (func_needs_compiling(ufunc, compile_type))
	(فراغ)compile_def_function(ufunc, FALSE, compile_type, عدم);
    وإلا
	smsg(_("Function %s%s%s does not need compiling"),
				cl != عدم ? cl->class_name.string : (char_u *)"",
				cl != عدم ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ساكن فراغ
defcompile_funcs_درون_script(فراغ)
{
    دراز	todo = (دراز)func_hashtab.ht_استفاده_شده;
    صحيح		تغییریافته = func_hashtab.ht_تغییریافته;
    hashitem_T	*hi;

    لكل (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	إذا (!HASHITEM_خالی(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    إذا (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_وضعیت == UF_مقصد_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(فراغ)compile_def_function(ufunc, FALSE, CT_NONE, عدم);

		إذا (func_hashtab.ht_تغییریافته != تغییریافته)
		{
		    // a function has been added or removed, need to start
		    // over
		    todo = (دراز)func_hashtab.ht_استفاده_شده;
		    تغییریافته = func_hashtab.ht_تغییریافته;
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
    فراغ
ex_defcompile(exarg_T *eap)
{
    إذا (*eap->arg != NUL)
    {
	typval_T tv;

	إذا (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    إذا (cl != عدم)
		defcompile_class(cl);
	}
	وإلا
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = پیدا_کن_func_by_name(eap->arg, &compile_type);
	    إذا (ufunc != عدم)
		defcompile_function(ufunc, عدم);
	}
    }
    وإلا
    {
	defcompile_funcs_درون_script();

	// compile all the class defined in the current script
	defcompile_classes_درون_script();
    }
}

/*
 * Return 5 if "p" starts with "<SID>" or "<SNR>" (ignoring case).
 * Return 2 if "p" starts with "s:".
 * Return 0 otherwise.
 */
    صحيح
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    إذا (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	أرجع 5;
    إذا (p[0] == 's' && p[1] == ':')
	أرجع 2;
    أرجع 0;
}

    صحيح
translated_function_exists(char_u *name, صحيح is_global)
{
    إذا (builtin_function(name, -1))
	أرجع has_داخلی_func(name);
    أرجع پیدا_کن_func(name, is_global) != عدم;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    صحيح
has_varargs(ufunc_T *ufunc)
{
    أرجع ufunc->uf_varargs || ufunc->uf_va_name != عدم;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    صحيح
function_exists(char_u *name, صحيح no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    صحيح	    n = FALSE;
    صحيح	    flag;
    صحيح	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    إذا (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    إذا (p != عدم && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    أرجع n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    صحيح
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    صحيح		is_global = FALSE;
    ufunc_T	*fp = عدم;
    list_T	*آرگومانها;
    صحيح		is_def;
    حرف	*tofree;

    إذا (*name == NUL)
	أرجع FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    إذا (p != عدم && *skipwhite(nm) == NUL)
	fp = پیدا_کن_func(p, is_global);
    vim_free(p);
    إذا (fp == عدم)
	أرجع FAIL;

    is_def = fp->uf_def_وضعیت != UF_NOT_COMPILED;
    dict_جمع_کن_string(d, "name", printable_func_name(fp));
    dict_جمع_کن_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_جمع_کن_bool(d, "available", TRUE);
    آرگومانها = list_alloc();
    إذا (آرگومانها == عدم || dict_جمع_کن_list(d, "args", آرگومانها) == FAIL)
    {
	list_unref(آرگومانها);
	أرجع FAIL;
    }
    لكل (صحيح j = 0; j < fp->uf_آرگومانها.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	إذا (arg == عدم || list_append_dict(آرگومانها, arg) == FAIL)
	{
	    dict_unref(arg);
	    أرجع FAIL;
	}
	dict_جمع_کن_string(arg, "name", FUNCARG(fp, j));
	إذا (fp->uf_arg_types == عدم)
	    dict_جمع_کن_string(arg, "type", (char_u *)"any");
	وإلا
	{
	    dict_جمع_کن_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	إذا (j >= fp->uf_آرگومانها.ga_len - fp->uf_def_آرگومانها.ga_len)
	    dict_جمع_کن_string(arg, "default", ((char_u **)fp->uf_def_آرگومانها.ga_داده)
			     [j - fp->uf_آرگومانها.ga_len + fp->uf_def_آرگومانها.ga_len]);
    }
    إذا (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	إذا (va == عدم || dict_جمع_کن_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    أرجع FAIL;
	}
	dict_جمع_کن_string(va, "name",
			 fp->uf_va_name == عدم ? (char_u *)"" : fp->uf_va_name);
	إذا (fp->uf_va_type == عدم)
	    dict_جمع_کن_string(va, "type", (char_u *)"list<any>");
	وإلا
	{
	    dict_جمع_کن_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    إذا (is_def)
    {
	dict_جمع_کن_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    وإلا
	dict_جمع_کن_string(d, "returns", (char_u *)"any");
    dict_جمع_کن_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_جمع_کن_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_جمع_کن_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_جمع_کن_bool(d, "closure", (fp->uf_flags & FC_بستگی) != 0);
    dict_جمع_کن_عدد(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_جمع_کن_عدد(d, "lnum", fp->uf_script_ctx.sc_lnum);
    أرجع OK;
}

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, صحيح check)
{
    char_u	*nm = name;
    char_u	*p;
    صحيح		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    إذا (p != عدم && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	أرجع p;

    vim_free(p);
    أرجع عدم;
}
#پایان_اگر

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, صحيح idx)
{
    ساكن long_u	انجامشده;
    ساكن صحيح		تغییریافته;
    ساكن hashitem_T	*hi;
    ufunc_T		*fp;

    إذا (idx == 0)
    {
	انجامشده = 0;
	hi = func_hashtab.ht_array;
	تغییریافته = func_hashtab.ht_تغییریافته;
    }
    إذا (تغییریافته == func_hashtab.ht_تغییریافته && انجامشده < func_hashtab.ht_استفاده_شده)
    {
	صحيح len;

	إذا (انجامشده++ > 0)
	    ++hi;
	طالما (HASHITEM_خالی(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	إذا ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    أرجع (char_u *)"";

	إذا (fp->uf_namelen + 4 >= IOSIZE)
	    أرجع fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	إذا (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    إذا (!has_varargs(fp) && fp->uf_آرگومانها.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	أرجع IObuff;
    }
    أرجع عدم;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, صحيح extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    إذا (ufunc == عدم)
	أرجع عدم;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_وضعیت = UF_مقصد_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = عدم;
#اگر_تعریف_شده FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_شمار = عدم;
    ufunc->uf_tml_مجموع = عدم;
    ufunc->uf_tml_self = عدم;
#پایان_اگر

    ga_copy_رشتهها(&fp->uf_آرگومانها, &ufunc->uf_آرگومانها);
    ga_copy_رشتهها(&fp->uf_def_آرگومانها, &ufunc->uf_def_آرگومانها);

    إذا (ufunc->uf_arg_types != عدم)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_آرگومانها.ga_len);
	إذا (at != عدم)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     حجم(type_T *) * ufunc->uf_آرگومانها.ga_len);
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

    إذا (ufunc->uf_va_name != عدم)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_بلوک_عمق = 0;
    ufunc->uf_بلوک_ids = عدم;

    ga_copy_رشتهها(&fp->uf_سطرها, &ufunc->uf_سطرها);

#اگر_تعریف_شده FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    إذا (fp->uf_profiling)
	func_do_profile(ufunc);
#پایان_اگر

    ufunc->uf_refcount = 1;

    أرجع ufunc;
}

/*
 * ":delfunction {name}"
 */
    فراغ
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = عدم;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    صحيح		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     عدم, عدم, عدم);
    vim_free(fudi.fd_newkey);
    إذا (name == عدم)
    {
	إذا (fudi.fd_dict != عدم && !eap->skip)
	    emsg(_(e_funcref_required));
	أرجع;
    }
    إذا (!دوسر_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	أرجع;
    }
    مجموعه_nextcmd(eap, p);
    إذا (eap->nextcmd != عدم)
	*p = NUL;

    إذا (numbered_function(name) && fudi.fd_dict == عدم)
    {
	إذا (!eap->skip)
	    semsg(_(e_invalid_آرگومان_str), eap->arg);
	vim_free(name);
	أرجع;
    }
    إذا (!eap->skip)
	fp = پیدا_کن_func(name, is_global);
    vim_free(name);

    إذا (!eap->skip)
    {
	إذا (fp == عدم)
	{
	    إذا (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    أرجع;
	}
	إذا (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_درون_use), eap->arg);
	    أرجع;
	}
	إذا (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    أرجع;
	}

	إذا (fudi.fd_dict != عدم)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	وإلا
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    إذا (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		إذا (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    وإلا
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
func_unref(char_u *name)
{
    ufunc_T *fp = عدم;

    إذا (name == عدم || !func_name_refcount(name))
	أرجع;
    fp = پیدا_کن_func(name, FALSE);
    إذا (fp == عدم && numbered_function(name))
    {
#اگر_تعریف_شده EXITFREE
	إذا (!entered_free_all_mem)
#پایان_اگر
	    داخلی_error("func_unref()");
    }
    func_ptr_unref(fp);
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 * Also when it becomes one and uf_partial points to the function.
 */
    فراغ
func_ptr_unref(ufunc_T *fp)
{
    إذا (fp != عدم && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != عدم
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	إذا (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    فراغ
func_ref(char_u *name)
{
    ufunc_T *fp;

    إذا (name == عدم || !func_name_refcount(name))
	أرجع;
    fp = پیدا_کن_func(name, FALSE);
    إذا (fp != عدم)
	++fp->uf_refcount;
    وإلا إذا (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	داخلی_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    فراغ
func_ptr_ref(ufunc_T *fp)
{
    إذا (fp != عدم)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ساكن صحيح
can_free_funccal(funccall_T *fc, صحيح copyID)
{
    أرجع (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    فراغ
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    صحيح		returning = FALSE;
    evalarg_T	evalarg;

    إذا (current_funccal == عدم)
    {
	emsg(_(e_return_not_inside_function));
	أرجع;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    إذا (eap->skip)
	++emsg_skip;

    eap->nextcmd = عدم;
    إذا ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	إذا (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	وإلا
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    وإلا إذا (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	إذا (!aborting())
	    returning = do_return(eap, FALSE, TRUE, عدم);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    إذا (returning)
	eap->nextcmd = عدم;
    وإلا إذا (eap->nextcmd == عدم)	    // no argument
	مجموعه_nextcmd(eap, arg);

    إذا (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ساكن صحيح
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    صحيح		doesrange;
    typval_T	rettv;
    صحيح		failed = FALSE;

    lnum = eap->line1;
    لكل ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	إذا (eap->addr_شمار > 0)
	{
	    إذا (lnum > curbuf->b_ml.ml_سطر_شمار)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		اكسر;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.ستون = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	إذا (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    اكسر;
	}
	إذا (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	إذا (handle_subscript(arg, عدم, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    اكسر;
	}

	clear_tv(&rettv);
	إذا (doesrange)
	    اكسر;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	إذا (aborting())
	    اكسر;
    }
    أرجع failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ساكن صحيح
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_آرگومانها + 1];	// vars for arguments
    صحيح		partial_argc = 0;		// number of partial arguments
    صحيح		argcount = 0;			// number of arguments found
    صحيح		r;

    إذا (current_funccal == عدم)
    {
	semsg(_(e_str_not_inside_function), "defer");
	أرجع FAIL;
    }
    إذا (partial != عدم)
    {
	إذا (partial->pt_dict != عدم)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    أرجع FAIL;
	}
	إذا (partial->pt_argc > 0)
	{
	    صحيح i;

	    partial_argc = partial->pt_argc;
	    لكل (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    صحيح is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    إذا (r == OK)
    {
	إذا (type != عدم)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_آرگومان_types(type, argvars, argcount, عدم, name);
	}
	وإلا إذا (is_builtin)
	{
	    صحيح idx = پیدا_کن_داخلی_func(name);

	    إذا (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    وإلا إذا (check_داخلی_func(idx, argcount) == -1)
		r = FAIL;
	}
	وإلا
	{
	    ufunc_T *ufunc = پیدا_کن_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    إذا (ufunc != عدم)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		إذا (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    إذا (r == FAIL)
    {
	طالما (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	أرجع FAIL;
    }
    أرجع جمع_کن_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    صحيح
can_جمع_کن_defer(فراغ)
{
    إذا (!درون_def_function() && get_current_funccal() == عدم)
    {
	semsg(_(e_str_not_inside_function), "defer");
	أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    صحيح
جمع_کن_defer(char_u *name, صحيح argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    صحيح		argcount = argcount_arg;
    defer_T	*dr;
    صحيح		ret = FAIL;

    إذا (saved_name == عدم)
	اذهب theend;
    إذا (درون_def_function())
    {
	إذا (جمع_کن_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    وإلا
    {
	إذا (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, حجم(defer_T), 10);
	إذا (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    اذهب theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_داده)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	طالما (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    طالما (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    أرجع ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ساكن فراغ
handle_defer_one(funccall_T *funccal)
{
    صحيح	    idx;

    لكل (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_داده) + idx;

	إذا (dr->dr_name == عدم)
	    // already being called, can happen if function does ":qa"
	    استمر;

	funcexe_T   funcexe;
	CLEAR_فیلد(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = عدم;

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
	لكل (صحيح i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ساكن فراغ
invoke_funccall_defer(funccall_T *fc)
{
    إذا (fc->fc_ectx != عدم)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    وإلا
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    فراغ
invoke_all_defer(فراغ)
{
    لكل (funccall_T *fc = current_funccal; fc != عدم; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    لكل (funccal_مدخل_T *fce = funccal_پشته; fce != عدم; fce = fce->بعدی)
	لكل (funccall_T *fc = fce->بالاترین_funccal; fc != عدم; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    فراغ
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    صحيح		len;
    صحيح		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = عدم;
    partial_T	*partial = عدم;
    evalarg_T	evalarg;
    type_T	*type = عدم;
    صحيح		یافت_var = FALSE;
    صحيح		vim9script = درون_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    إذا (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	إذا (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	أرجع;
    }

    tofree = trans_function_name_ext(&arg, عدم, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : عدم, &ufunc);
    إذا (fudi.fd_newkey != عدم)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_کلید_not_present_درون_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    إذا (tofree == عدم)
	أرجع;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    إذا (fudi.fd_dict != عدم)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (صحيح)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != عدم ? عدم : &partial,
				vim9script && type == عدم ? &type : عدم,
						     FALSE, FALSE, &یافت_var);

    إذا (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_آرگومانها(name, STRLEN(name), &arg);
	إذا (name == عدم)
	    اذهب انتها;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    إذا (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	اذهب انتها;
    }
    إذا (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	اذهب انتها;
    }

    إذا (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    وإلا
    {
	funcexe_T funcexe;

	CLEAR_فیلد(funcexe);
	funcexe.fe_check_type = type;
	funcexe.fe_ufunc = ufunc;
	funcexe.fe_partial = partial;
	funcexe.fe_selfdict = fudi.fd_dict;
	funcexe.fe_firstline = eap->line1;
	funcexe.fe_lastline = eap->line2;
	funcexe.fe_یافت_var = یافت_var;
	funcexe.fe_evaluate = TRUE;
	failed = ex_call_inner(eap, name, &arg, startarg, &funcexe, &evalarg);
    }

    // When inside :try we need to check for following "| catch" or "| endtry".
    // Not when there was an error, but do check if an exception was thrown.
    إذا ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	إذا (!دوسر_excmd2(eap->arg, arg))
	{
	    إذا (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	وإلا
	    مجموعه_nextcmd(eap, arg);
    }
    // Must be after using "arg", it may point into memory cleared here.
    clear_evalarg(&evalarg, eap);

انتها:
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
    صحيح
do_return(
    exarg_T	*eap,
    صحيح		reanimate,
    صحيح		is_cmd,
    فراغ	*rettv)
{
    صحيح		idx;
    cstack_T	*cstack = eap->cstack;

    إذا (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    إذا (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	إذا (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	وإلا
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    إذا (reanimate)
		rettv = current_funccal->fc_rettv;

	    إذا (rettv != عدم)
	    {
		// Store the value of the pending return.
		إذا ((cstack->cs_rettv[idx] = alloc_tv()) != عدم)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		وإلا
		    emsg(_(e_بیرون_از_حافظه));
	    }
	    وإلا
		cstack->cs_rettv[idx] = عدم;

	    إذا (reanimate)
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
    وإلا
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	إذا (!reanimate && rettv != عدم)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    إذا (!is_cmd)
		vim_free(rettv);
	}
    }

    أرجع idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    فراغ
discard_pending_return(فراغ *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(فراغ *rettv)
{
    char_u	*s = عدم;
    char_u	*tofree = عدم;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    إذا (rettv != عدم)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    إذا (s == عدم)
	s = (char_u *)"";
    وإلا
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    إذا (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    أرجع vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_سطر(
    صحيح	    c استفادهنشده,
    فراغ    *cookie,
    صحيح	    indent استفادهنشده,
    getline_opt_T options استفادهنشده)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    إذا (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_پیدا_کن_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	func_سطر_انتها(cookie);
#پایان_اگر

    gap = &fp->uf_سطرها;
    إذا (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_درون_try())
	    || fcp->fc_returned)
	retval = عدم;
    وإلا
    {
	// Skip NULL lines (continuation lines).
	طالما (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_داده))[fcp->fc_linenr] == عدم)
	    ++fcp->fc_linenr;
	إذا (fcp->fc_linenr >= gap->ga_len)
	    retval = عدم;
	وإلا
	{
	    retval = vim_strsave(((char_u **)(gap->ga_داده))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#اگر_تعریف_شده FEAT_PROFILE
	    إذا (do_profiling == PROF_YES)
		func_سطر_شروع(cookie, SOURCING_LNUM);
#پایان_اگر
	}
    }

    // Did we encounter a breakpoint?
    إذا (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_پیدا_کن_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    أرجع retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    صحيح
func_has_ended(فراغ *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    أرجع (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_درون_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    صحيح
func_has_abort(
    فراغ    *cookie)
{
    أرجع ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
}


/*
 * Turn "dict.Func" into a partial for "Func" bound to "dict".
 * Don't do this when "Func" is already a partial that was bound
 * explicitly (pt_auto is FALSE).
 * Changes "rettv" in-place.
 * Returns the updated "selfdict_in".
 */
    dict_T *
make_partial(dict_T *selfdict_درون, typval_T *rettv)
{
    char_u	*fname;
    ufunc_T	*fp = عدم;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_درون;

    إذا (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != عدم
				     && rettv->vval.v_partial->pt_func != عدم)
	fp = rettv->vval.v_partial->pt_func;
    وإلا
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == عدم ? عدم
					      : rettv->vval.v_partial->pt_name;
	إذا (fname == عدم)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = عدم;
	}
	وإلا
	{
	    char_u	*tofree = عدم;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = پیدا_کن_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    إذا (fp != عدم && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	إذا (pt != عدم)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = عدم;
	    إذا (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    وإلا
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		صحيح		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		إذا (ret_pt->pt_name != عدم)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		وإلا
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		إذا (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    إذا (pt->pt_argv == عدم)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    وإلا
		    {
			pt->pt_argc = ret_pt->pt_argc;
			لكل (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    أرجع selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(فراغ *cookie)
{
    أرجع ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(فراغ *cookie)
{
    أرجع &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    صحيح *
func_dbg_tick(فراغ *cookie)
{
    أرجع &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    صحيح
func_level(فراغ *cookie)
{
    أرجع ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    صحيح
current_func_returned(فراغ)
{
    أرجع current_funccal->fc_returned;
}

    صحيح
free_unref_funccal(صحيح copyID, صحيح testing)
{
    صحيح		did_free = FALSE;
    صحيح		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    لكل (pfc = &قبلی_funccal; *pfc != عدم; )
    {
	إذا (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	وإلا
	    pfc = &(*pfc)->fc_caller;
    }
    إذا (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(فراغ)garbage_collect(testing);

    أرجع did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ساكن funccall_T *
get_funccal(فراغ)
{
    صحيح		i;
    funccall_T	*funccal;
    funccall_T	*موقت_funccal;

    funccal = current_funccal;
    إذا (debug_backtrace_level > 0)
    {
	لكل (i = 0; i < debug_backtrace_level; i++)
	{
	    موقت_funccal = funccal->fc_caller;
	    إذا (موقت_funccal)
		funccal = موقت_funccal;
	    وإلا
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    أرجع funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ساكن funccall_T *
get_funccal_for_vars(فراغ)
{
    funccall_T	*funccal = عدم;

    إذا (current_funccal == عدم)
	أرجع عدم;
    funccal = get_funccal();
    إذا (funccal == عدم || funccal->fc_l_vars.dv_refcount == 0)
	أرجع عدم;
    أرجع funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    أرجع funccal == عدم ? عدم : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    أرجع funccal == عدم ? عدم : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_آرگومانها_ht(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    أرجع funccal == عدم ? عدم : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_آرگومانها_var(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    أرجع funccal == عدم ? عدم : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    فراغ
list_func_vars(صحيح *اولی)
{
    إذا (current_funccal != عدم && current_funccal->fc_l_vars.dv_refcount > 0)
	list_hashtable_vars(&current_funccal->fc_l_vars.dv_hashtab,
							   "l:", FALSE, اولی);
}

/*
 * If "ht" is the hashtable for local variables in the current funccal, return
 * the dict that contains it.
 * Otherwise return NULL.
 */
    dict_T *
get_current_funccal_dict(hashtab_T *ht)
{
    إذا (current_funccal != عدم
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	أرجع &current_funccal->fc_l_vars;
    أرجع عدم;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
پیدا_کن_hi_درون_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = عدم;
    char_u	*varname;

    إذا (current_funccal == عدم || current_funccal->fc_func->uf_scoped == عدم)
	أرجع عدم;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    طالما (current_funccal != عدم)
    {
	ht = پیدا_کن_var_ht(name, &varname);
	إذا (ht != عدم && *varname != NUL)
	{
	    hi = hash_پیدا_کن(ht, varname);
	    إذا (!HASHITEM_خالی(hi))
	    {
		*pht = ht;
		اكسر;
	    }
	}
	إذا (current_funccal == current_funccal->fc_func->uf_scoped)
	    اكسر;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    أرجع hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
پیدا_کن_var_درون_scoped_ht(char_u *name, صحيح no_autoload)
{
    dictitem_T	*v = عدم;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    إذا (current_funccal == عدم || current_funccal->fc_func->uf_scoped == عدم)
	أرجع عدم;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    طالما (current_funccal)
    {
	ht = پیدا_کن_var_ht(name, &varname);
	إذا (ht != عدم && *varname != NUL)
	{
	    v = پیدا_کن_var_درون_ht(ht, *name, varname, no_autoload);
	    إذا (v != عدم)
		اكسر;
	}
	إذا (current_funccal == current_funccal->fc_func->uf_scoped)
	    اكسر;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    أرجع v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    صحيح
مجموعه_ref_درون_قبلی_funccal(صحيح copyID)
{
    funccall_T	*fc;

    لكل (fc = قبلی_funccal; fc != عدم; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	إذا (مجموعه_ref_درون_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, عدم, عدم)
		|| مجموعه_ref_درون_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, عدم, عدم)
		|| مجموعه_ref_درون_list_اقلام(&fc->fc_l_varlist, copyID + 1, عدم, عدم))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن صحيح
مجموعه_ref_درون_funccal(funccall_T *fc, صحيح copyID)
{
    إذا (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	إذا (مجموعه_ref_درون_ht(&fc->fc_l_vars.dv_hashtab, copyID, عدم, عدم)
		|| مجموعه_ref_درون_ht(&fc->fc_l_avars.dv_hashtab, copyID, عدم, عدم)
		|| مجموعه_ref_درون_list_اقلام(&fc->fc_l_varlist, copyID, عدم, عدم)
		|| مجموعه_ref_درون_func(عدم, fc->fc_func, copyID))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    صحيح
مجموعه_ref_درون_call_پشته(صحيح copyID)
{
    funccall_T		*fc;
    funccal_مدخل_T	*مدخل;

    لكل (fc = current_funccal; fc != عدم; fc = fc->fc_caller)
	إذا (مجموعه_ref_درون_funccal(fc, copyID))
	    أرجع TRUE;

    // Also go through the funccal_stack.
    لكل (مدخل = funccal_پشته; مدخل != عدم; مدخل = مدخل->بعدی)
	لكل (fc = مدخل->بالاترین_funccal; fc != عدم; fc = fc->fc_caller)
	    إذا (مجموعه_ref_درون_funccal(fc, copyID))
		أرجع TRUE;
    أرجع FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    صحيح
مجموعه_ref_درون_functions(صحيح copyID)
{
    صحيح		todo;
    hashitem_T	*hi = عدم;
    ufunc_T	*fp;

    todo = (صحيح)func_hashtab.ht_استفاده_شده;
    لكل (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	إذا (!HASHITEM_خالی(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    إذا (!func_name_refcount(fp->uf_name)
					  && مجموعه_ref_درون_func(عدم, fp, copyID))
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    صحيح
مجموعه_ref_درون_func_آرگومانها(صحيح copyID)
{
    صحيح i;

    لكل (i = 0; i < funcargs.ga_len; ++i)
	إذا (مجموعه_ref_درون_قلم(((typval_T **)funcargs.ga_داده)[i],
						copyID, عدم, عدم, عدم))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
مجموعه_ref_درون_func(char_u *name, ufunc_T *fp_درون, صحيح copyID)
{
    ufunc_T	*fp = fp_درون;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = عدم;
    char_u	*fname;
    صحيح		abort = FALSE;

    إذا (name == عدم && fp_درون == عدم)
	أرجع FALSE;

    إذا (fp_درون == عدم)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = پیدا_کن_func(fname, FALSE);
    }
    إذا (fp != عدم)
    {
	لكل (fc = fp->uf_scoped; fc != عدم; fc = fc->fc_func->uf_scoped)
	    abort = abort || مجموعه_ref_درون_funccal(fc, copyID);
    }

    vim_free(tofree);
    أرجع abort;
}

#پایان_اگر // FEAT_EVAL
