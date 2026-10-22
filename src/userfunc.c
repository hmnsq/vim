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

#אם defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ساكن hashtab_T	func_hashtab;

// Used by get_func_tv()
ساكن garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
ساكن funccall_T *current_funccal = عدم;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ساكن funccall_T *previous_funccal = عدم;

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
    החזר &func_hashtab;
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

    כלעוד (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    אם (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == عدم
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	אם (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	החזר arg;
    }

    // Extra checks in Vim9 script.
    אם (!skip && argtypes != عدم)
    {
	صحيح c = *p;
	*p = NUL;
	صحيح r = check_reserved_name(arg, FALSE);
	*p = c;
	אם (r == FAIL)
	    החזר arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	אם (check_defined(arg, p - arg,
				   evalarg == عدم ? عدم : evalarg->eval_cctx,
			       eap == عدم ? عدم : eap->cstack, TRUE) == FAIL)
	    החזר arg;
    }

    אם (newargs != عدم && ga_grow(newargs, 1) == FAIL)
	החזר arg;
    אם (newargs != عدم)
    {
	صحيح	c;
	صحيح	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	אם (arg_copy == عدم)
	{
	    *p = c;
	    החזר arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	אם (argtypes == عدم || !is_underscore)
	    // Check for duplicate argument name.
	    עבור (i = 0; i < newargs->ga_len; ++i)
		אם (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    החזר arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    אם (argtypes != عدم && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != عدم && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = عدم;

	אם (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == عدم ? arg : arg_copy);
	    p = skipwhite(p);
	}
	אם (*p == ':')
	{
	    ++p;
	    אם (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		החזר arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    אם (!skip)
		type = vim_strnsave(type, p - type);
	}
	אחרת אם (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == عدم ? arg : arg_copy);
	    החזר arg;
	}
	אם (!skip)
	{
	    אם (type == عدم && types_optional)
	    {
		// lambda arguments default to "any" type
		אם (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		אחרת
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    החזר p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ساكن char_u *
get_function_line(
	exarg_T		*eap,
	garray_T	*lines_to_free,
	صحيح		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    אם (eap->ea_getline == عدم)
	theline = getcmdline(':', 0L, indent, 0);
    אחרת
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    אם (theline != عدم)
    {
	אם (lines_to_free->ga_len > 0
		&& eap->cmdlinep != عدم
		&& *eap->cmdlinep == ((char_u **)lines_to_free->ga_data)
						   [lines_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(فراغ)ga_add_string(lines_to_free, theline);
    }

    החזר theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ساكن صحيح
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    صحيح		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    صحيح		*varargs,
    garray_T	*default_args,
    صحيح		skip,
    exarg_T	*eap,		// can be NULL
    صحيح		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*lines_to_free)
{
    صحيح		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    صحيح		c;
    صحيح		any_default = FALSE;
    char_u	*whitep = *argp;
    صحيح		need_expr = FALSE;

    אם (newargs != عدم)
	ga_init2(newargs, حجم(char_u *), 3);
    אם (argtypes != عدم)
	ga_init2(argtypes, حجم(char_u *), 3);
    אם (arg_objm != عدم)
	ga_init2(arg_objm, حجم(int8_T), 3);
    אם (!skip && default_args != عدم)
	ga_init2(default_args, حجم(char_u *), 3);

    אם (varargs != عدم)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    כלעוד (*p != endchar)
    {
	כלעוד (eap != عدم && eap->ea_getline != عدم
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_line(eap, lines_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    אם (theline == عدم)
		اكسر;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	אם (mustend && *p != endchar)
	{
	    אם (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    اذهب err_ret;
	}
	אם (*p == endchar && !need_expr)
	    اكسر;

	אם (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    אם (varargs != عدم)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    אם (argtypes != عدم)
	    {
		// ...name: list<type>
		אם (!eval_isnamec1(*p))
		{
		    אם (!skip)
			emsg(_(e_missing_name_after_dots));
		    اذهب err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		אם (p == arg)
		    اكسر;
		אם (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    اكسر;
		}
	    }
	}
	אחרת אם (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    כלעוד (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    אם (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		اكسر;
	    }

	    אם (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		אם (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_value_must_be_vnone_str), p);
		    اذهب err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		אם (ga_grow(default_args, 1) == FAIL)
		    اذهب err_ret;

		char_u *expr = vim_strnsave((char_u *)"v:none", 6);
		אם (expr == عدم)
		    اذهب err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = expr;
		default_args->ga_len++;
	    }
	    אחרת אם (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		اذهب err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    אם (newargs != عدم && ga_grow(newargs, 1) == FAIL)
		החזר FAIL;
	    אם (newargs != عدم)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		אם (argtypes != عدم && ga_grow(argtypes, 1) == OK
			    && arg_objm != عدم && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    אם (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			صحيح len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			אם (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			אם (assignment != عدم)
			{
			    c = *argend;
			    *argend = NUL;
			    אם (any_default)
				vim_snprintf((حرف *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    אחרת
				vim_snprintf((حرف *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    אם (*p == ',')
		++p;
	}
	אחרת
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    אם (p == arg)
		اكسر;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    אם (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != عدم)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *expr = p;
		אם (eval1(&p, &rettv, عدم) != FAIL)
		{
		    אם (!skip)
		    {
			אם (ga_grow(default_args, 1) == FAIL)
			    اذهب err_ret;

			אם (need_expr)
			    need_expr = FALSE;
			// trim trailing whitespace
			כלעוד (p > expr && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			expr = vim_strsave(expr);
			אם (expr == عدم)
			{
			    *p = c;
			    اذهب err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = expr;
			default_args->ga_len++;
			*p = c;
		    }
		}
		אחרת
		{
		    mustend = TRUE;
		    אם (*skipwhite(p) == NUL)
			need_expr = TRUE;
		}
	    }
	    אחרת אם (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		اذهب err_ret;
	    }

	    אם (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		אם (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    اذهب err_ret;
		}
		p = skipwhite(p);
	    }
	    אם (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		אם (!skip && argtypes != عدم
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    اذهب err_ret;
		}
	    }
	    אחרת
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    אם (*p != endchar)
	اذهب err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    החזר OK;

err_ret:
    אם (newargs != عدم)
	ga_clear_strings(newargs);
    אם (!skip && default_args != عدم)
	ga_clear_strings(default_args);
    החזר FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ساكن صحيح
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    صحيح		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    صحيح		obj_member_count,
    cctx_T	*cctx)
{
    صحيح len = 0;

    ga_init2(&fp->uf_type_list, حجم(type_T *), 10);
    אם (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	אם (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	אם (fp->uf_arg_types != عدم)
	{
	    صحيح	i;
	    type_T	*type;

	    עבור (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		אם (p == عدم)
		    // will get the type from the default value
		    type = &t_unknown;
		אחרת
		{
		    אם (arg_objm != عدم && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			עבור (صحيح om = 0; om < obj_member_count; ++om)
			{
			    אם (obj_members != عدم
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				اكسر;
			    }
			}
		    }
		    אחרת
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		אם (type == عدم || !valid_declaration_type(type))
		    החזר FAIL;
		fp->uf_arg_types[i] = type;
		אם (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    אם (obj_members != عدم && *name == '_')
			// protected object method
			name++;

		    אם (var_wrong_func_name(name, TRUE))
			החזר FAIL;
		}
	    }
	}
    }

    אם (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = عدم;
	p = ((char_u **)argtypes->ga_data)[len];
	אם (p == عدم)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	אחרת
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    אם (fp->uf_va_type != عدم && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		החזר FAIL;
	    }
	}
	אם (fp->uf_va_type == عدم)
	    החזר FAIL;
    }

    החזר OK;
}

    ساكن صحيح
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    אם (ret_type == عدم)
	fp->uf_ret_type = &t_void;
    אחרת
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	אם (fp->uf_ret_type == عدم)
	{
	    fp->uf_ret_type = &t_void;
	    החזר FAIL;
	}
    }
    החזר OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ساكن صحيح
register_closure(ufunc_T *fp)
{
    אם (fp->uf_scoped == current_funccal)
	// no change
	החזר OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    אם (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	החזר FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    החזר OK;
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

    אם (name[0] != K_SPECIAL)
	החזר name;
    len = STRLEN(name);
    אם (len + 3 > bufsize)
	החזר name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    החזר buf;
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
    אם (n < 1)
	ret.length = 0;
    אחרת אם (n >= (صحيح)حجم(lambda_name))
	ret.length = حجم(lambda_name) - 1;
    אחרת
	ret.length = (size_t)n;

    ret.string = lambda_name;
    החזר ret;
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
    אם (fp != عدم)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((فراغ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	אם (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    אם (fp->uf_name_exp != عدم)
		vim_snprintf((حرف *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    החזר fp;
}

#אם defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, فراغ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    אם (fp == عدم)
	החזר عدم;

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

    החזר name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ساكن char_u *
skip_arrow(
	char_u	*start,
	صحيح	equal_arrow,
	char_u	**ret_type,
	صحيح	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    אם (equal_arrow)
    {
	אם (*s == ':')
	{
	    אם (white_error != عدم && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		החזר عدم;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    אם (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		החזר عدم;
	    }
	}
	bef = s;
	s = skipwhite(s);
	אם (*s != '=')
	    החזר عدم;
	++s;
    }
    אם (*s != '>')
	החזר عدم;
    אם (white_error != عدم && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	החזר عدم;
    }
    החזר skipwhite(s + 1);
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

    אם (checkforcmd(&p, "function", 2))
    {
	אם (*p == '(')
	    החזר FALSE;
	*cmd = p;
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ساكن فراغ
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    אם (cstack == عدم || cstack->cs_idx < 0)
	החזר;

    صحيح	    count = cstack->cs_idx + 1;
    صحيح	    i;

    fp->uf_block_ids = ALLOC_MULT(صحيح, count);
    אם (fp->uf_block_ids != عدم)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		حجم(صحيح) * count);
	fp->uf_block_depth = count;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    עבור (i = 0; i <= cstack->cs_idx; ++i)
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
    כלעוד (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	אם (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	אחרת אם (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    اكسر;
	}
	אחרת
	    MB_PTR_ADV(p);
    }

    החזר p;
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
    כלעוד (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	אם (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	אחרת אם (*p == '"')
	{
	    p++;    // skip the ending double quote
	    اكسر;
	}
	אחרת
	    MB_PTR_ADV(p);
    }

    החזר p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ساكن char_u *
find_start_of_vim9_comment(char_u *line)
{
    char_u	*p = line;

    כלעוד (*p != NUL)
    {
	אם (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	אחרת אם (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	אחרת
	{
	    אם (*p == '#')
		// Found the start of a Vim9 comment
		اكسر;
	    MB_PTR_ADV(p);
	}
    }

    החזר p;
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
	char_u	    *line_arg_in,
	garray_T    *lines_to_free)
{
    linenr_T	sourcing_lnum_top = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    صحيح		saved_wait_return = need_wait_return;
    char_u	*line_arg = line_arg_in;
    صحيح		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
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
    صحيح		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    אם (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	אם (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    اذهب theend;
	כלעוד (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = عدم;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    עבור (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	אם (KeyTyped)
	{
	    msg_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	אם (line_arg != عدم)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = line_arg;
	    p = vim_strchr(theline, '\n');
	    אם (p == عدم)
		line_arg += STRLEN(line_arg);
	    אחרת
	    {
		*p = NUL;
		line_arg = p + 1;
	    }
	}
	אחרת
	{
	    theline = get_function_line(eap, lines_to_free, indent,
							      getline_options);
	}
	אם (KeyTyped)
	    lines_left = Rows - 1;
	אם (theline == عدم)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_top;
	    אם (skip_until != عدم)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    אחרת אם (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    אחרת אם (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    אחרת
		emsg(_(e_missing_endfunction));
	    اذهب theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	אם (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	אחרת
	    sourcing_lnum_off = 0;

	אם (skip_until != عدم)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    אם (heredoc_trimmed == عدم
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		אם (heredoc_trimmed == عدم)
		    p = theline;
		אחרת אם (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		אחרת
		    p = theline + heredoc_trimmedlen;
		אם (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    אם (heredoc_concat_len > 0)
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
	אחרת
	{
	    صحيح	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    עבור (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		אם (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    אם (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		אם (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		אם (nesting-- == 0)
		{
		    char_u *nextcmd = عدم;

		    אם (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    אחרת אם (line_arg != عدم && *skipwhite(line_arg) != NUL)
			nextcmd = line_arg;
		    אחרת אם (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_top
							+ newlines->ga_len + 1;
			אם (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			אחרת
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    אם (nextcmd != عدم && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			אם (lines_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)lines_to_free->ga_data)
						   [lines_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)lines_to_free->ga_data)
						   [lines_to_free->ga_len - 1];
			    --lines_to_free->ga_len;
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
	    אחרת אם (nesting_def[nesting])
	    {
		אם (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    אחרת אם (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    אם (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    אחרת אם (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    אם (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		אם (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, عدم, TRUE, 0));
		אם (vim9_function && *p == '<')
		{
		    // skip generic function
		    אם (skip_generic_func_type_args(&p) == FAIL)
			اذهب theend;
		}
		אם (*skipwhite(p) == '(')
		{
		    אם (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    אחרת
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    אם (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		אם (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		אחרת
		    end = p + STRLEN(p) - 1;

		כלעוד (end > p && VIM_ISWHITE(*end))
		    --end;
		אם (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    صحيح	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    כלעוד (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    אם (!is_block)
			is_block = find_cmd_block_start(p) != عدم;

		    אם (is_block)
		    {
			אם (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			אחרת
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
	    אם (!vim9_function || colon)
		p = skip_range(p, FALSE, عدم);
	    char_u *tp = p;
	    אם (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    אחרת
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    אם (arg[0] == '<' && arg[1] =='<'
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
		אם (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    אם (heredoc_trimmed == عدم)
			heredoc_trimmedlen = 0;
		}
		אם (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		אחרת
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		אם (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    אם (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		אם (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    صحيح		save_sc_version = current_sctx.sc_version;
		    صحيح		var_count = 0;
		    صحيح		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_count, &semicolon,
									 TRUE);
		    אם (arg != عدم)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    אם (arg != عدم && STRNCMP(arg, "=<<", 3) == 0)
		    {
			صحيح has_trim = FALSE;

			p = skipwhite(arg + 3);
			כלעוד (TRUE)
			{
			    אם (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				استمر;
			    }
			    אם (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				استمر;
			    }
			    اكسر;
			}
			אם (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    אם (heredoc_trimmed == عدم)
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
	אם (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    اذهب theend;

	אם (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	אחרת
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	אם (p == عدم)
	    اذهب theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	כלעוד (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = عدم;

	// Check for end of eap->arg.
	אם (line_arg != عدم && *line_arg == NUL)
	    line_arg = عدم;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    אם (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    החזר ret;
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
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
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
    linenr_T	sourcing_lnum_top = SOURCING_LNUM;
    char_u	*line_arg = عدم;

    *arg = skipwhite(*arg + 1);
    אם (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	החזר FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    אם (**arg == '\n')
	line_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    אם (evalarg->eval_cctx != عدم)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    אחרת
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, حجم(char_u *), 10);
    אם (get_function_body(&eap, &newlines, line_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	اذهب erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_count += newlines.ga_len;
    אם (gap->ga_itemsize > 0)
    {
	صحيح	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	עבור (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    אם (p == عدم)
		// comment line in the lambda body
		استمر;

	    p = skipwhite(p);

	    אם (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		اذهب erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    אם (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    אחרת
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    אם (pnl != عدم)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	אם (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    اذهب erret;
	אם (eap.nextcmd != عدم)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	אחרת
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	אם (pnl != عدم)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    אם (eap.nextcmd != عدم)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	אם (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    אחרת
	*arg = (char_u *)"";

    אם (!evaluate)
    {
	ret = OK;
	اذهب erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    אם (ufunc == عدم)
	اذهب erret;
    אם (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	اذهب erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = عدم;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = عدم;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_top;

    // parse argument types, the type variables of the function being
    // compiled may be used
    אם (parse_argument_types(ufunc, argtypes, varargs, عدم, عدم, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	اذهب erret;
    }

    // parse the return type, if any
    אם (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	اذهب erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    אם (pt == عدم)
	اذهب erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_lines = newlines;
    newlines.ga_data = عدم;
    אם (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    אם (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_top;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = عدم;
    ret = OK;

erret:
    אם (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    אם (newargs != عدم)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    אם (ufunc != عدم)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    החזר ret;
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
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = عدم;
    partial_T   *pt = عدم;
    صحيح		varargs;
    char_u	*ret_type = عدم;
    صحيح		ret;
    char_u	*s;
    char_u	*start, *end;
    صحيح		*old_eval_lavars = eval_lavars_used;
    صحيح		eval_lavars = FALSE;
    char_u	*tofree2 = عدم;
    صحيح		equal_arrow = **arg == '(';
    صحيح		white_error = FALSE;
    صحيح		called_emsg_start = called_emsg;
    صحيح		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    אם (equal_arrow && !vim9script)
	החזר NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', عدم,
	    types_optional ? &argtypes : عدم, types_optional,
			types_optional ? &arg_objm : عدم, evalarg,
			عدم, &default_args, TRUE, عدم, FALSE, عدم, عدم);
    אם (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, عدم) == عدم)
    {
	אם (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	החזר called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    אם (evaluate)
	pnewargs = &newargs;
    אחרת
	pnewargs = عدم;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : عدم, types_optional,
			types_optional ? &arg_objm : عدم, evalarg,
					    &varargs, &default_args,
					    FALSE, عدم, FALSE, عدم, عدم);
    אם (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : عدم)) == عدم)
    {
	אם (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	החזר white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    אם (ret_type != عدم)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    אם (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    אם (equal_arrow && **arg == '{')
    {
	אם (evalarg == عدم)
	    // cannot happen?
	    اذهب theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	אם (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : عدم, varargs,
			   &default_args, ret_type) == FAIL)
	    اذهب errret;
	اذهب theend;
    }
    אם (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_values_in_lambda));
	اذهب errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_expr_concatenate(arg, &start, &end, evalarg);
    אם (ret == FAIL)
	اذهب errret;

    אם (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	אם (**arg != '}')
	{
	    semsg(_(e_expected_right_curly_str), *arg);
	    اذهب errret;
	}
	++*arg;
    }

    אם (evaluate)
    {
	صحيح	    len;
	صحيح	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *line_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	אם (fp == عدم)
	    اذهب errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	אם (pt == عدم)
	    اذهب errret;

	ga_init2(&newlines, حجم(char_u *), 1);
	אם (ga_grow(&newlines, 1) == FAIL)
	    اذهب errret;

	// If there are line breaks, we need to split up the string.
	line_end = vim_strchr(start, '\n');
	אם (line_end == عدم || line_end > end)
	    line_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (صحيح)(line_end - start) + 1;
	p = alloc(len);
	אם (p == عدم)
	    اذهب errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, line_end - start);

	אם (line_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    כלעוד (line_end < end && *line_end == '\n')
	    {
		אם (ga_grow(&newlines, 1) == FAIL)
		    اذهب errret;
		start = line_end + 1;
		line_end = vim_strchr(start, '\n');
		אם (line_end == عدم || line_end > end)
		    line_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, line_end - start);
	    }
	}

	אם (strstr((حرف *)p + 7, "a:") == عدم)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	אם (types_optional)
	{
	    אם (parse_argument_types(fp, &argtypes,
				vim9script && varargs, عدم, عدم, 0,
				cctx) == FAIL)
		اذهب errret;
	    אם (ret_type != عدم)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     عدم, cctx, TRUE);
		אם (fp->uf_ret_type == عدم)
		    اذهب errret;
	    }
	    אחרת
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_lines = newlines;
	אם (current_funccal != عدم && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    אם (register_closure(fp) == FAIL)
		اذهب errret;
	}

#ifdef FEAT_PROFILE
	אם (prof_def_func())
	    func_do_profile(fp);
#endif
	אם (sandbox)
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
    אם (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    החזר OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    אם (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    אם (fp != عدم)
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
    החזר FAIL;
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
	صحيح	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = عدم;
    صحيح		cc;
    char_u	*s = عدم;
    hashtab_T	*ht;
    صحيح		did_type = FALSE;

    אם (partialp != عدم)
	*partialp = عدم;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    אם (v != عدم)
    {
	tv = &v->di_tv;
    }
    אחרת אם (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	صحيح	    len = *lenp;

	אם (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	אם (import != عدم)
	{
	    name[len] = NUL;
	    אם (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    אחרת
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    החזר (char_u *)"";	// just in case
	}
    }

    אם (tv != عدم)
    {
	אם (found_var != عدم)
	    *found_var = TRUE;
	אם (tv->v_type == VAR_FUNC)
	{
	    אם (tv->vval.v_string == عدم)
	    {
		*lenp = 0;
		החזר (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    אם (p != عدم && generic_func_find_open_bracket(name) != عدم)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		החזר (char_u *)"";
	    }
	    אם (p != عدم)
		*lenp = p - s;
	    אחרת
		*lenp = (صحيح)STRLEN(s);
	}
	אחרת אם (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    אם (pt == عدم)
	    {
		*lenp = 0;
		החזר (char_u *)"";	// just in case
	    }
	    אם (partialp != عدم)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (صحيح)STRLEN(s);
	}

	אם (s != عدم)
	{
	    אם (!did_type && type != عدم && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		אם (sv != عدم)
		    *type = sv->sv_type;
	    }
	    החזר s;
	}
    }

    החזר name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    فراغ
emsg_funcname(حرف *ermsg, char_u *name)
{
    char_u	*p = name;

    אם (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    אם (p != name)
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
    صحيح		vim9script = in_vim9script();
    صحيح		evaluate = evalarg == عدم
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    כלעוד (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	אם (*argp == ')' || *argp == ',' || *argp == NUL)
	    اكسر;

	صحيح arg_idx = *argcount;
	אם (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    اكسر;
	}
	++*argcount;
	אם (!is_builtin && check_typval_is_value(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    اكسر;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	אם (vim9script)
	{
	    אם (*argp != ',' && *skipwhite(argp) == ',')
	    {
		אם (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		اكسر;
	    }
	}
	אחרת
	    argp = skipwhite(argp);
	אם (*argp != ',')
	    اكسر;
	אם (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    אם (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    اكسر;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    אם (*argp == ')')
	++argp;
    אחרת
	ret = FAIL;
    *arg = argp;
    החזר ret;
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
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    صحيح		argcount = 0;			// number of arguments found
    صحيح		vim9script = in_vim9script();
    صحيح		evaluate = evalarg == عدم
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == عدم ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    אם (ret == OK)
    {
	صحيح	i = 0;
	صحيح	did_emsg_before = did_emsg;

	אם (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    אם (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, حجم(typval_T *), 50);
	    עבור (i = 0; i < argcount; ++i)
		אם (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	אם (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    אחרת אם (!aborting() && evaluate)
    {
	אם (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	אחרת
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    כלעוד (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    אם (vim9script)
	*arg = argp;
    אחרת
	*arg = skipwhite(argp);
    החזר ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ساكن صحيح
eval_fname_sid(char_u *p)
{
    החזר (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    אם (script_name == name)
	החזר name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (صحيح)KE_SNR;
    fname_buflen = 3;
    אם (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    אחרת
    {
	אם (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	אחרת
	{
	    fname_buflen += vim_snprintf((حرف *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    אם (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    אחרת
    {
	fname = alloc(fnamelen + 1);
	אם (fname == عدم)
	    *error = FCERR_OTHER;
	אחרת
	{
	    *tofree = fname;
	    vim_snprintf((حرف *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    החזר fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    فراغ
func_name_with_sid(char_u *name, صحيح sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (صحيح)KE_SNR;
    vim_snprintf((حرف *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ساكن ufunc_T *
find_func_with_sid(char_u *name, صحيح sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    אם (!SCRIPT_ID_VALID(sid))
	החזר عدم;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    אם (!HASHITEM_EMPTY(hi))
	החזר HI2UF(hi);
    החזר عدم;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ساكن ufunc_T *
find_func_with_prefix(char_u *name, صحيح sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    אם (vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	החזר عدم;	// already has the prefix
    אם (!SCRIPT_ID_VALID(sid))
	החזר عدم;	// not in a script
    si = SCRIPT_ITEM(sid);
    אם (si->sn_autoload_prefix != عدم)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	אם (namep == عدم)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	אם (len < حجم(buffer))
	    auto_name = buffer;
	אחרת
	    auto_name = alloc(len);
	אם (auto_name != عدم)
	{
	    vim_snprintf((حرف *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    אם (auto_name != buffer)
		vim_free(auto_name);
	    אם (!HASHITEM_EMPTY(hi))
		החזר HI2UF(hi);
	}
    }

    החזר عدم;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, صحيح flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    אם ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	אם (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    אם (func != عدم)
		החזר func;
	}
	אם (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    אם (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (صحيح)sid);
		אם (func != عدم)
		    החזר func;
	    }
	}
    }

    אם ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	אם (!HASHITEM_EMPTY(hi))
	    החזר HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    החזר find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, صحيح is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    אם (fp != عدم && (fp->uf_flags & FC_DEAD) == 0)
	החזר fp;
    החזר عدم;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    صحيح
func_is_global(ufunc_T *ufunc)
{
    החזר ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    صحيح
func_requires_g_prefix(ufunc_T *ufunc)
{
    החזר func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == عدم
	    && !SAFE_isdigit(ufunc->uf_name[0]);
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

    אם (!func_is_global(fp))
	len = vim_snprintf((حرف *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    אחרת
	len = vim_snprintf((حرف *)buf, bufsize, "%s", fp->uf_name);

    החזר (len >= (صحيح)bufsize) ? (صحيح)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ساكن فراغ
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    حرف	*name,
    varnumber_T nr)
{
    STRCPY(v->di_key, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_NUMBER;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_number = nr;
}

/*
 * Free "fc".
 */
    ساكن فراغ
free_funccal(funccall_T *fc)
{
    صحيح	i;

    עבור (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	אם (fp != عدم && fp->uf_scoped == fc)
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
    FOR_ALL_LIST_ITEMS(&fc->fc_l_varlist, li)
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
    אם (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    אחרת
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    אם (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    אחרת
    {
	صحيح	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (صحيح)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    אם (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    אם (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = عدم;
    אחרת
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ITEMS(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    אם (free_fc)
	free_funccal(fc);
    אחרת
    {
	ساكن صحيح made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	אם (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	אחרת אם (++made_copy >= (صحيح)((4096 * 1024) / حجم(*fc)))
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
    החזר SAFE_isdigit(*name)
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
    صحيح
func_name_refcount(char_u *name)
{
    החזר numbered_function(name) || (name[0] == '<' && name[1] == 'l');
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

    אם (fc == عدم)
	החזר;

    אם (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	עבור (pfc = &previous_funccal; *pfc != عدم; pfc = &(*pfc)->fc_caller)
	{
	    אם (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		החזר;
	    }
	}
    עבור (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	אם (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = عدم;
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
    אם (fp->uf_flags & FC_DEAD)
	החזר FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    אם (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	החזר FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    אם (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	החזר FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    החזר TRUE;
}

    ساكن فراغ
func_clear_items(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_lines));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    אם (IS_GENERIC_FUNC(fp))
	generic_func_clear_items(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = عدم;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    אם (fp->uf_cb_free != عدم)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = عدم;
    }

    fp->uf_cb_state = عدم;
    fp->uf_cb = عدم;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_count);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ساكن فراغ
func_clear(ufunc_T *fp, صحيح force)
{
    אם (fp->uf_cleared)
	החזר;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_items(fp);
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
    אם ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    אם ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	אם (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	החזר OK;
    }
    החזר FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    فراغ
func_clear_free(ufunc_T *fp, صحيح force)
{
    func_clear(fp, force);
    אם (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    אחרת
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    صحيح
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = عدم;

    אם (ufunc == عدم)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	החזר FAIL;
    }

    fp = find_func(global, TRUE);
    אם (fp != عدم)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	החזר FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    אם (fp == عدم)
	החזר FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    אם (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_lines, &fp->uf_lines) == FAIL)
	اذهب failed;

    אם (ufunc->uf_arg_types != عدم)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	אם (fp->uf_arg_types == عدم)
	    اذهب failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    حجم(type_T *) * fp->uf_args.ga_len);
    }
    אם (ufunc->uf_va_name != عدم)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	אם (fp->uf_va_name == عدم)
	    اذهب failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    אם ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == عدم)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	אם (pt == عدم)
	    اذهب failed;
	אם (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    اذهب failed;
	}
	ufunc->uf_partial = pt;
    }

    החזר OK;

failed:
    func_clear_free(fp, TRUE);
    החזר FAIL;
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
    אם (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	החזר FAIL;
    }
    ++funcdepth;
    החזר OK;
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
    החזר funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    فراغ
funcdepth_restore(صحيح depth)
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

    אם (fc == عدم)
	החזר عدم;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    החזר fc;
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
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    صحيح		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    אם (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = -1;
	החזר FCERR_FAILED;
    }

    line_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    אם (fc == عدم)
	החזר FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, حجم(ufunc_T *), 1);

    אם (fp->uf_def_status != UF_NOT_COMPILED)
    {
	אם (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	אם (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	אם (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	אם (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	אם (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != عدم && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	החזר retval;
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
    אם (selfdict != عدم)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_key;
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
    אם ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    אם ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_key;
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
    אם ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    עבור (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	صحيح	    addlocal = FALSE;
	typval_T    def_rettv;
	صحيح	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	אם (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    אם (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_number == VVAL_NONE));
	    אם (isdefault)
	    {
		char_u	    *default_expr = عدم;

		def_rettv.v_type = VAR_NUMBER;
		def_rettv.vval.v_number = -1;

		default_expr = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		אם (eval1(&default_expr, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    اكسر;
		}
	    }

	    namelen = STRLEN(name);
	}
	אחרת
	{
	    אם ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		اكسر;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((حرف *)numbuf, حجم(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	אם (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_key, name);
	}
	אחרת
	{
	    v = dictitem_alloc(name);
	    אם (v == عدم)
		اكسر;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	אם (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	אם (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	אחרת
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	אם (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    אם (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_push_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    אם (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	אם (p_verbose >= 14)
	{
	    char_u	buf[MSG_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    msg_puts("(");
	    עבור (i = 0; i < argcount; ++i)
	    {
		אם (i > 0)
		    msg_puts(", ");
		אם (argvars[i].v_type == VAR_NUMBER)
		    msg_outnum((long)argvars[i].vval.v_number);
		אחרת
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    אם (s != عدم)
		    {
			אם (vim_strsize(s) > MSG_BUF_CLEN)
			{
			    trunc_string(s, buf, MSG_BUF_CLEN, MSG_BUF_LEN);
			    s = buf;
			}
			msg_puts((حرف *)s);
			vim_free(tofree);
		    }
		}
	    }
	    msg_puts(")");
	}
	msg_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    אם (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func);
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

    אם (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    אחרת אם (islambda)
    {
	char_u *p = *(char_u **)fp->uf_lines.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(فراغ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    אחרת
	// call do_cmdline() to execute the lines
	do_cmdline(عدم, get_func_line, (فراغ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    אם (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    אם ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_NUMBER;
	rettv->vval.v_number = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	אם (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    אם (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == عدم ? عدم : fc->fc_caller->fc_func;

	אם (fp->uf_profiling || (caller != عدم && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    אם (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	אם (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	אחרת אם (fc->fc_rettv->v_type == VAR_NUMBER)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_number);
	אחרת
	{
	    char_u	buf[MSG_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    אם (s != عدم)
	    {
		אם (vim_strsize(s) > MSG_BUF_CLEN)
		{
		    trunc_string(s, buf, MSG_BUF_CLEN, MSG_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	msg_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_pop();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    אם (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    אם (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    אם (p_verbose >= 12 && SOURCING_NAME != عدم)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	msg_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    עבור (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    החזר retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, صحيح argcount)
{
    صحيح regular_args = fp->uf_args.ga_len;

    אם (argcount < regular_args - fp->uf_def_args.ga_len)
	החזר FCERR_TOOFEW;
    אחרת אם (!has_varargs(fp) && argcount > regular_args)
	החזר FCERR_TOOMANY;
    החזר FCERR_UNKNOWN;
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

#ifdef FEAT_LUA
    אם (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	החזר (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    אם (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != عدم)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    אם (error != FCERR_UNKNOWN)
	החזר error;

    אם ((fp->uf_flags & FC_DICT) && selfdict == عدم)
    {
	error = FCERR_DICT;
    }
    אחרת
    {
	صحيح		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_search_patterns();
	אם (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : عدم);
	אם (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	אם (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_search_patterns();
    }

    החזר error;
}

ساكن funccal_entry_T *funccal_stack = عدم;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    فراغ
save_funccal(funccal_entry_T *entry)
{
    entry->top_funccal = current_funccal;
    entry->next = funccal_stack;
    funccal_stack = entry;
    current_funccal = عدم;
}

    فراغ
restore_funccal(فراغ)
{
    אם (funccal_stack == عدم)
	internal_error("restore_funccal()");
    אחרת
    {
	current_funccal = funccal_stack->top_funccal;
	funccal_stack = funccal_stack->next;
    }
}

    funccall_T *
get_current_funccal(فراغ)
{
    החזר current_funccal;
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
    החזר current_funccal == عدم && autocmd_match == عدم;
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

    כלעוד (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    אם (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		אם (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    صحيح changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    אם (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			אם (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    אחרת
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			אם (changed != func_hashtab.ht_changed)
			    اكسر;
		    }
		}
		--todo;
	    }
    }
}

#אם defined(EXITFREE)
    فراغ
free_all_functions(فراغ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    صحيح		changed;

    // Clean up the current_funccal chain and the funccal stack.
    כלעוד (current_funccal != عدم)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	אם (current_funccal == عدم && funccal_stack != عدم)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    כלעוד (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    אם (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		אם (func_name_refcount(fp->uf_name))
		    ++skipped;
		אחרת
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    אם (changed != func_hashtab.ht_changed)
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
    כלעוד (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ITEMS(&func_hashtab, hi, todo)
	    אם (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		אם (func_name_refcount(fp->uf_name))
		    ++skipped;
		אחרת
		{
		    אם (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			اكسر;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    אם (skipped == 0)
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
    صحيح
builtin_function(char_u *name, صحيح len)
{
    صحيح i;

    אם (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	החזר FALSE;
    עבור (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	אם (name[i] == AUTOLOAD_CHAR)
	    החזר FALSE;
	אם (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    אם (name[i] == '.')
		החזר FALSE;
	    اكسر;
	}
    }
    החזר TRUE;
}

    صحيح
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
    صحيح		argc = 0;
    صحيح		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ITEMS(l, item)
    {
	אם (argc == MAX_FUNC_ARGS - (partial == عدم ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    اكسر;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    אם (item == عدم)
    {
	funcexe_T	funcexe;
	صحيح		namelen = -1;

	אם (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    אם (p != عدم)
	    {
		אם (skip_generic_func_type_args(&p) == FAIL)
		    اذهب done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	אם (partial != عدم)
	{
	    funcexe.fe_object = partial->pt_obj;
	    אם (funcexe.fe_object != عدم)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    כלעוד (argc > 0)
	clear_tv(&argv[--argc]);

done:
    החזר r;
}

ساكن صحيح callback_depth = 0;

    صحيح
get_callback_depth(فراغ)
{
    החזר callback_depth;
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

    אם (callback->cb_name == عدم || *callback->cb_name == NUL)
	החזר FAIL;

    אם (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	החזר FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    אם (callback->cb_partial != عدم)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	אם (funcexe.fe_object != عدم)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    אם (need_rethrow && current_exception != عدم && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    החזר ret;
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

    אם (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	החזר -2;

    retval = tv_get_number_chk(&rettv, عدم);
    clear_tv(&rettv);
    החזר retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    فراغ
user_func_error(funcerror_T error, char_u *name, صحيح found_var)
{
    بدل (error)
    {
	حالة FCERR_UNKNOWN:
		אם (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		אחרת
		    emsg_funcname(e_unknown_function_str, name);
		اكسر;
	حالة FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
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
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
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
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	صحيح	    argcount,
	صحيح	    base_included,
	char_u	    *name)
{
    אם (funcexe->fe_check_type != عدم && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	אם (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? عدم : funcexe->fe_basetv,
			  name) == FAIL)
	    החזר FCERR_OTHER;
    }
    החזר FCERR_NONE;
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
    صحيح		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
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
    صحيح		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    صحيح		argv_clear = 0;
    صحيح		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    אם (partial != عدم)
	fp = partial->pt_func;
    אם (fp == عدم)
	fp = funcexe->fe_ufunc;

    אם (fp == عدم)
    {
	אם (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    אם (p != عدم)
	    {
		len = p - funcname;
		אם (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == عدم)
		    اذهب theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	אם (name == عدم)
	    اذهب theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    אם (funcexe->fe_doesrange != عدم)
	*funcexe->fe_doesrange = FALSE;

    אם (partial != عدم)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	אם (partial->pt_dict != عدم && (selfdict == عدم || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	אם (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    עבור (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		אם (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    اذهب theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    עבור (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    אם (funcexe->fe_check_type != عدم
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		עבור (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    אם (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != عدم) ? name : funcname);

    אם (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	صحيح	is_global = FALSE;

	// Skip "g:" before a function name.
	אם (fp == عدم && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_NUMBER;	// default rettv is number zero
	rettv->vval.v_number = 0;
	error = FCERR_UNKNOWN;

	אם (fp != عدم || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    אם (fp == عدم)
	    {
		fp = find_func(rfname, is_global);
		אם (fp != عدم && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = عدم;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    אם (fp == عدم
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, عدم)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    אם (fp == عدم && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    אם (fp == عدم)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		אם (p != عدم && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    אם (partial == عدم && fp != عدم && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		אם (fp == عدم)
		{
		    error = FCERR_FAILED;
		    اذهب theend;
		}
	    }
	    אחרת אם (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != عدم ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		اذهب theend;
	    }

	    אם (fp != عدم && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    אחרת אם (fp != عدم)
	    {
		صحيح need_arg_check = FALSE;
		אם (funcexe->fe_check_type == عدم)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		אם (funcexe->fe_argv_func != عدم)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		אם (funcexe->fe_basetv != عدم)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, حجم(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		אם (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != عدم) ? name : funcname);

		אם (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	אחרת אם (funcexe->fe_basetv != عدم)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	אחרת
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
    אם (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    אם (!aborting())
	user_func_error(error, (name != عدم) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    כלעוד (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    החזר ret;
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

    rettv->v_type = VAR_NUMBER;	// default rettv is number zero
    rettv->vval.v_number = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    אם (name == عدم)
	החזר ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    אם (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    אחרת
	rfname = fname;
    fp = find_func(rfname, is_global);
    אם (fp != عدم && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = عدم;
    אם (fp == عدم)
	ret = NOTDONE;
    אחרת אם (fp != عدم && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    אחרת אם (fp != عدم)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, عدم);
	אם (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    החזר ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    החזר fp->uf_name_exp != عدم ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ساكن صحيح
function_list_modified(صحيح prev_ht_changed)
{
    אם (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ساكن صحيح
list_func_head(ufunc_T *fp, صحيح indent)
{
    صحيح		prev_ht_changed = func_hashtab.ht_changed;
    صحيح		j;

    msg_start();

    // a timer at the more prompt may have deleted the function
    אם (function_list_modified(prev_ht_changed))
	החזר FAIL;

    אם (indent)
	msg_puts("   ");
    אם (fp->uf_def_status != UF_NOT_COMPILED)
	msg_puts("def ");
    אחרת
	msg_puts("function ");
    msg_puts((حرف *)printable_func_name(fp));
    msg_putchar('(');
    עבור (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	אם (j)
	    msg_puts(", ");
	msg_puts((حرف *)FUNCARG(fp, j));
	אם (fp->uf_arg_types != عدم)
	{
	    حرف *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	אם (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    msg_puts(" = ");
	    msg_puts(((حرف **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    אם (fp->uf_varargs)
    {
	אם (j)
	    msg_puts(", ");
	msg_puts("...");
    }
    אם (fp->uf_va_name != عدم)
    {
	אם (!fp->uf_varargs)
	{
	    אם (j)
		msg_puts(", ");
	    msg_puts("...");
	}
	msg_puts((حرف *)fp->uf_va_name);
	אם (fp->uf_va_type != عدم)
	{
	    حرف *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    msg_putchar(')');

    אם (fp->uf_def_status != UF_NOT_COMPILED)
    {
	אם (fp->uf_ret_type != &t_void)
	{
	    حرف *tofree;

	    msg_puts(": ");
	    msg_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    אחרת אם (fp->uf_flags & FC_ABORT)
	msg_puts(" abort");
    אם (fp->uf_flags & FC_RANGE)
	msg_puts(" range");
    אם (fp->uf_flags & FC_DICT)
	msg_puts(" dict");
    אם (fp->uf_flags & FC_CLOSURE)
	msg_puts(" closure");
    msg_clr_eos();
    אם (p_verbose > 0)
	last_set_msg(fp->uf_script_ctx);

    החזר OK;
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
    החזר trans_function_name_ext(pp, is_global, skip, flags,
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
    char_u	*start;
    char_u	*end;
    صحيح		lead;
    char_u	sid_buf[20];
    صحيح		len;
    صحيح		extra = 0;
    صحيح		prefix_g = FALSE;
    lval_T	lv;
    صحيح		vim9script = in_vim9script();
    صحيح		vim9_local;

    אם (fdp != عدم)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    אם ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (صحيح)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	החזר vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    אם (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, عدم, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    אם (end == start || (vim9script && end != عدم
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	אם (!skip)
	    emsg(_(e_function_name_required));
	اذهب theend;
    }
    אם (end == عدم || (lv.ll_tv != عدم && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	אם (!aborting())
	{
	    אם (end != عدم)
		semsg(_(e_invalid_argument_str), start);
	}
	אחרת
	    *pp = find_name_end(start, عدم, عدم, FNE_INCL_BR);
	اذهب theend;
    }

    אם (lv.ll_ufunc != عدم)
    {
	אם (ufunc != عدم)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	اذهب theend;
    }

    אם (lv.ll_tv != عدم)
    {
	אם (fdp != عدم)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = عدم;
	    fdp->fd_di = lv.ll_di;
	}
	אם (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != عدم)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	אחרת אם (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != عدم)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	אחרת אם (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != عدم)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    אם (partial != عدم)
		*partial = lv.ll_tv->vval.v_partial;
	}
	אחרת
	{
	    אם (!skip && !(flags & TFN_QUIET) && (fdp == عدم
			     || lv.ll_dict == عدم || fdp->fd_newkey == عدم))
		emsg(_(e_funcref_required));
	    אחרת
		*pp = end;
	    name = عدم;
	}
	اذهب theend;
    }

    אם (lv.ll_name == عدم)
    {
	// Error found, but continue after the function name.
	*pp = end;
	اذهب theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    אם (lv.ll_exp_name != عدم)
    {
	len = (صحيح)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, عدم);
	אם (name == lv.ll_exp_name)
	    name = عدم;
    }
    אחרת אם (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	صحيح		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	אם (si->sn_autoload_prefix != عدم)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	אחרת
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (صحيح)KE_SNR;
	    vim_snprintf((حرف *)sid_buf + 3, حجم(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	اذهب theend;
    }
    אחרת אם (!(flags & TFN_NO_DEREF))
    {
	len = (صحيح)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, عدم);
	אם (name == *pp)
	    name = عدم;
    }
    אם (name != عدم)
    {
	name = vim_strsave(name);
	*pp = end;
	אם (name != عدم && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحيح)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	اذهب theend;
    }

    אם (lv.ll_exp_name != عدم)
    {
	len = (صحيح)STRLEN(lv.ll_exp_name);
	אם (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    אחרת
    {
	// skip over "s:" and "g:"
	אם (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    אם (lv.ll_name[0] == 'g')
	    {
		אם (is_global != عدم)
		{
		    *is_global = TRUE;
		}
		אחרת
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (صحيح)(end - lv.ll_name);
    }
    אם (len <= 0)
    {
	אם (!skip)
	    emsg(_(e_function_name_required));
	اذهب theend;
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
    אם (skip)
	lead = 0;	// do nothing
    אחרת אם (lead > 0 || vim9_local)
    {
	אם (!vim9_local)
	{
	    אם (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		اذهب theend;
	    }
	    lead = 3;
	}
	אם (vim9_local || (lv.ll_exp_name != عدم
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    אם (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		اذهب theend;
	    }
	    sid_buflen = vim_snprintf((حرف *)sid_buf, حجم(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    אם (vim9_local)
		extra = 3 + (صحيح)sid_buflen;
	    אחרת
		lead += (صحيح)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    אחרת אם (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	اذهب theend;
    }
    אם (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	אם (cp != عدم && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    اذهب theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    אם (name != عدم)
    {
	אם (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (صحيح)KE_SNR;
	    אם (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	אחרת אם (prefix_g)
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
    החזר name;
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

    אם (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	אם (p != عدم)
	    החזר p + 1;
    }
    החזר عدم;
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

    אם (funcname == عدم)
	החזר عدم;

    אם (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	אם (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    החזר عدم;
	ufunc = find_func(funcname, FALSE);
	אם (ufunc == عدم || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == عدم)
	    החזר عدم;
	++p;
	off = 0;
    }
    אחרת
	off = *funcname == 's' ? 2 : 5;

    אם (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	החזר عدم;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, حجم(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    אם (newname == عدم)
	החזר عدم;
    vim_snprintf((حرف *)newname, newnamesize, "%s%s", sid_buf, p + off);

    החזר newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    אם (n != عدم)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    החזר n;
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

    אם (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(فراغ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	אם (fudi != عدم)
	    CLEAR_POINTER(fudi);
    }
    אחרת
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, عدم, عدم, عدم);
    *name = p;
    החזר saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    فراغ
list_functions(regmatch_T *regmatch)
{
    صحيح		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    עבור (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    אם ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == عدم
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		אם (list_func_head(fp, FALSE) == FAIL)
		    החזר;
		אם (function_list_modified(prev_ht_changed))
		    החזר;
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
    אם (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	אם (regmatch.regprog != عدم)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    אם (*p == '/')
	++p;

    החזר p;
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

    אם (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	החזר عدم;
    }

    set_nextcmd(eap, p);

    אם (eap->nextcmd != عدم)
	*p = NUL;

    אם (eap->skip || got_int)
	החזר عدم;

    fp = find_func(name, is_global);
    אם (fp == عدم && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	אם (up != عدم)
	    fp = find_func(up, FALSE);
    }

    אם (fp == عدم)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	החזר عدم;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    صحيح prev_ht_changed = func_hashtab.ht_changed;

    אם (list_func_head(fp, TRUE) != OK)
	החזר fp;

    עבור (j = 0; j < fp->uf_lines.ga_len && !got_int; ++j)
    {
	אם (FUNCLINE(fp, j) == عدم)
	    استمر;
	msg_putchar('\n');
	msg_outnum((long)(j + 1));
	אם (j < 9)
	    msg_putchar(' ');
	אם (j < 99)
	    msg_putchar(' ');
	אם (function_list_modified(prev_ht_changed))
	    اكسر;
	msg_prt_line(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    אם (!got_int)
    {
	msg_putchar('\n');
	אם (!function_list_modified(prev_ht_changed))
	{
	    אם (fp->uf_def_status != UF_NOT_COMPILED)
		msg_puts("   enddef");
	    אחרת
		msg_puts("   endfunction");
	}
    }

    החזר fp;
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
	garray_T    *lines_to_free,
	صحيح	    class_flags,
	ocmember_T  *obj_members,
	صحيح         obj_member_count,
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
    char_u	*line_arg = عدم;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
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
    linenr_T	sourcing_lnum_top;
    صحيح		vim9script = in_vim9script();
    imported_T	*import = عدم;

    // ":function" without argument: list functions.
    אם (ends_excmd2(eap->cmd, eap->arg))
    {
	אם (!eap->skip)
	    list_functions(عدم);
	set_nextcmd(eap, eap->arg);
	החזר عدم;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    אם (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	החזר عدم;
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
    אם (name_arg != عدم)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    אחרת
    {
	אם (vim9script)
	{
	    אם (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		החזר عدم;
	    }
	    p = to_name_end(p, TRUE);
	    אם (*skipwhite(p) == '.' && vim_strchr(p, '(') != عدم)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		החזר عدم;
	    }

	    p = eap->arg;
	}

	صحيح tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != عدم);
	אם (name == عدم && (fudi.fd_dict == عدم || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    אם (!aborting())
	    {
		אם (!eap->skip && fudi.fd_newkey != عدم)
		    semsg(_(e_key_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		החזר عدم;
	    }
	    אחרת
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	אם (is_export && name != عدم)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    אם (prefixed != عدم && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	אחרת אם (paren && vim9script && name != عدم
				    && vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
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
    אם (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	اذهب ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    אם (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	אם (p == عدم)
	    اذهب ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    אם (*p != '(')
    {
	אם (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    اذهب ret_free;
	}
	// attempt to continue by skipping some text
	אם (vim_strchr(p, '(') != عدم)
	    p = vim_strchr(p, '(');
    }

    אם ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	اذهب ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    אם (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	اذهب ret_free;
    }

    ga_init2(&newlines, حجم(char_u *), 10);

    אם (!eap->skip && name_arg == عدم)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	אם (name != عدم)
	    arg = name;
	אחרת
	    arg = fudi.fd_newkey;
	אם (arg != عدم && (fudi.fd_di == عدم
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    صحيح	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    אם (arg != fudi.fd_newkey)
	    {
		אם (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    אם (name_base == عدم)
			name_base = arg + 3;
		    אחרת
			++name_base;
		}
		עבור (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		אם (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    اذهب ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		אם (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, عدم,
			عدم, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    اذهب ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	אם (fudi.fd_dict != عدم && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    اذهب ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    אם (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : عدم, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : عدم,
			 عدم, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, lines_to_free) == FAIL)
	اذهب errret_2;
    whitep = p;

    אם (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	אם (*skipwhite(p) == ':')
	{
	    אם (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    אחרת אם (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    אם (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    אחרת
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = عدم;
	    }
	}
	p = skipwhite(p);
    }
    אחרת
	// find extra arguments "range", "dict", "abort" and "closure"
	עבור (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    אם (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    אחרת אם (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    אחרת אם (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    אחרת אם (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		אם (current_funccal == عدم)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_top_level_str,
			    name == عدم ? (char_u *)"" : name);
		    اذهب erret;
		}
	    }
	    אחרת
		اكسر;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    אם (*p == '\n')
	line_arg = p + 1;
    אחרת אם (*p != NUL
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
    אם (KeyTyped)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	אם (!eap->skip && !eap->forceit)
	{
	    אם (fudi.fd_dict != عدم && fudi.fd_newkey == عدم)
		emsg(_(e_dictionary_entry_already_exists));
	    אחרת אם (name != عدم && find_func(name, is_global) != عدم)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	אם (!eap->skip && did_emsg)
	    اذهب erret;

	msg_putchar('\n');	    // don't overwrite the function name
	cmdline_row = msg_row;
    }

    // Save the starting line number.
    sourcing_lnum_top = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    אם (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, line_arg, lines_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	اذهب erret;

    /*
     * If there are no errors, add the function
     */
    אם (fudi.fd_dict != عدم)
    {
	حرف	numbuf[NUMBUFLEN];

	fp = عدم;
	אם (fudi.fd_newkey == عدم && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    اذهب erret;
	}
	אם (fudi.fd_di == عدم)
	{
	    // Can't add a function to a locked dictionary
	    אם (value_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		اذهب erret;
	}
	    // Can't change an existing function if it is locked
	אחרת אם (value_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    اذهب erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, حجم(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	אם (name == عدم)
	    اذهب erret;
    }
    אחרת אם (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	صحيح		var_conflict = FALSE;
	صحيح		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	אם (v != عدم && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	אם (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    אם (si->sn_autoload_prefix != عدم)
	    {
		אם (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    אם (v != عدم)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		אחרת
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    אם (prefixed != عدم && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			אם (v != عدم)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	אם (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    اذهب erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	אם (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == عدم ? name : uname, 0, FALSE);
	}

	אם (fp != عدم || import != عدم)
	{
	    صحيح dead = fp != عدم && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    אם (import != عدم
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_top;
		אם (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		אחרת
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		اذهب errret_keep;
	    }
	    אם (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		اذهب errret_keep;
	    }
	    אם (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = عدم;
		overwrite = TRUE;
	    }
	    אחרת
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = عدم;
		func_clear_items(fp);
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

    אם (fp == عدم)
    {
	אם (fudi.fd_dict == عدم && vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    صحيح	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    אם (SOURCING_NAME != عدم)
	    {
		scriptname = autoload_name(name);
		אם (scriptname != عدم)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (صحيح)STRLEN(p);
		    slen = (صحيح)STRLEN(SOURCING_NAME);
		    אם (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    אם (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_top;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		اذهب erret;
	    }
	}

	אם (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	אם (fp == عدم)
	    اذهب erret;
	fp_allocated = TRUE;

	אם (fudi.fd_dict != عدم)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    אם (func_name == عدم)
	    {
		VIM_CLEAR(fp);
		اذهب erret;
	    }
	    אם (fudi.fd_di == عدم)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		אם (fudi.fd_di == عدم)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    اذهب erret;
		}
		אם (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    اذهب erret;
		}
	    }
	    אחרת
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

    אם (eap->cmdidx == CMD_def)
    {
	صحيح	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	אם (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_top;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	صحيح save_is_export = is_export;
	is_export = FALSE;

	אם (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_count, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    اذهب erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	אם (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    اذهب erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    אחרת
	fp->uf_def_status = UF_NOT_COMPILED;

    אם (fp_allocated)
    {
	// insert the new function in the function list
	אם (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_key = UF2HIKEY(fp);
	}
	אחרת אם (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    اذهب erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_lines = newlines;
    newlines.ga_data = عدم;
    אם ((flags & FC_CLOSURE) != 0)
    {
	אם (register_closure(fp) == FAIL)
	    اذهب erret;
    }
    אחרת
	fp->uf_scoped = عدم;

#ifdef FEAT_PROFILE
    אם (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    אם (sandbox)
	flags |= FC_SANDBOX;
    אם (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_top;
    אם (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    אם (eap->cmdidx == CMD_def)
	set_function_type(fp);
    אחרת אם (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    אם (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, عدم);

    اذهب ret_free;

erret:
    אם (fp != عدم)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    אם (fp != عدم)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	אם (IS_GENERIC_FUNC(fp))
	    generic_func_clear_items(fp);
    }
    אם (free_fp)
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
    אם (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    החזר fp;
}

/*
 * ":function"
 */
    فراغ
ex_function(exarg_T *eap)
{
    garray_T lines_to_free;

    ga_init2(&lines_to_free, حجم(char_u *), 50);
    (فراغ)define_function(eap, عدم, &lines_to_free, 0, عدم, 0, عدم);
    ga_clear_strings(&lines_to_free);
}

    صحيح
get_func_arity(char_u *name, صحيح *required, صحيح *optional, صحيح *varargs)
{
    ufunc_T	*ufunc = عدم;
    صحيح		argcount = 0;
    صحيح		min_argcount = 0;
    صحيح		idx;

    idx = find_internal_func(name);
    אם (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    אחרת
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = عدم;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	אם (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	אם (ufunc == عدم)
	    החזר FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    החזר OK;
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
    صحيح		is_global = FALSE;
    char_u	*bracket_start = عدم;
    char_u	*generic_func_name = name;

    אם (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    אחרת אם (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    אם (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(فراغ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    אחרת
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	אם (ufunc != عدم)
	    החזר ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      عدم, عدم, عدم, &ufunc);
	אם (ufunc != عدم)
	{
	    vim_free(fname);
	    החזר ufunc;
	}
	אם (fname != عدم && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    אם (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		החזר عدم;
	    }
	}
    }
    אם (fname == عدم)
    {
	semsg(_(e_invalid_argument_str), name);
	החזר عدم;
    }
    אם (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	החזר عدم;
    }

    ufunc = find_func(fname, is_global);
    אם (ufunc == عدم)
    {
	char_u *p = untrans_function_name(fname);

	אם (p != عدم)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	אם (ufunc == عدم)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    אם (bracket_start != عدم)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    אם (ufunc != عدم)
    {
	// handle generic functions
	אם (bracket_start == عدم)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    אם (ufunc == عدم)
	semsg(_(e_cannot_find_function_str), name);
    החזר ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    فراغ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    אם (func_needs_compiling(ufunc, compile_type))
	(فراغ)compile_def_function(ufunc, FALSE, compile_type, عدم);
    אחרת
	smsg(_("Function %s%s%s does not need compiling"),
				cl != عدم ? cl->class_name.string : (char_u *)"",
				cl != عدم ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ساكن فراغ
defcompile_funcs_in_script(فراغ)
{
    long	todo = (long)func_hashtab.ht_used;
    صحيح		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    עבור (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    אם (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(فراغ)compile_def_function(ufunc, FALSE, CT_NONE, عدم);

		אם (func_hashtab.ht_changed != changed)
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
    فراغ
ex_defcompile(exarg_T *eap)
{
    אם (*eap->arg != NUL)
    {
	typval_T tv;

	אם (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    אם (cl != عدم)
		defcompile_class(cl);
	}
	אחרת
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    אם (ufunc != عدم)
		defcompile_function(ufunc, عدم);
	}
    }
    אחרת
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
    صحيح
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    אם (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	החזר 5;
    אם (p[0] == 's' && p[1] == ':')
	החזר 2;
    החזר 0;
}

    صحيح
translated_function_exists(char_u *name, صحيح is_global)
{
    אם (builtin_function(name, -1))
	החזר has_internal_func(name);
    החזר find_func(name, is_global) != عدم;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    صحيح
has_varargs(ufunc_T *ufunc)
{
    החזר ufunc->uf_varargs || ufunc->uf_va_name != عدم;
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
    אם (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    אם (p != عدم && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    החזר n;
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
    list_T	*args;
    صحيح		is_def;
    حرف	*tofree;

    אם (*name == NUL)
	החזר FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    אם (p != عدم && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    אם (fp == عدم)
	החזר FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    אם (args == عدم || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	החזר FAIL;
    }
    עבור (صحيح j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	אם (arg == عدم || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    החזר FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	אם (fp->uf_arg_types == عدم)
	    dict_add_string(arg, "type", (char_u *)"any");
	אחרת
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	אם (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    אם (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	אם (va == عدم || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    החזר FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == عدم ? (char_u *)"" : fp->uf_va_name);
	אם (fp->uf_va_type == عدم)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	אחרת
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    אם (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    אחרת
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_number(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_number(d, "lnum", fp->uf_script_ctx.sc_lnum);
    החזר OK;
}

#אם defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, صحيح check)
{
    char_u	*nm = name;
    char_u	*p;
    صحيح		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    אם (p != عدم && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	החזר p;

    vim_free(p);
    החזר عدم;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, صحيح idx)
{
    ساكن long_u	done;
    ساكن صحيح		changed;
    ساكن hashitem_T	*hi;
    ufunc_T		*fp;

    אם (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    אם (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	صحيح len;

	אם (done++ > 0)
	    ++hi;
	כלעוד (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	אם ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    החזר (char_u *)"";

	אם (fp->uf_namelen + 4 >= IOSIZE)
	    החזר fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	אם (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    אם (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	החזר IObuff;
    }
    החזר عدم;
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
    אם (ufunc == عدم)
	החזר عدم;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = عدم;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_count = عدم;
    ufunc->uf_tml_total = عدم;
    ufunc->uf_tml_self = عدم;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    אם (ufunc->uf_arg_types != عدم)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	אם (at != عدم)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     حجم(type_T *) * ufunc->uf_args.ga_len);
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

    אם (ufunc->uf_va_name != عدم)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = عدم;

    ga_copy_strings(&fp->uf_lines, &ufunc->uf_lines);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    אם (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    החזר ufunc;
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
    אם (name == عدم)
    {
	אם (fudi.fd_dict != عدم && !eap->skip)
	    emsg(_(e_funcref_required));
	החזר;
    }
    אם (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	החזר;
    }
    set_nextcmd(eap, p);
    אם (eap->nextcmd != عدم)
	*p = NUL;

    אם (numbered_function(name) && fudi.fd_dict == عدم)
    {
	אם (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	החזר;
    }
    אם (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    אם (!eap->skip)
    {
	אם (fp == عدم)
	{
	    אם (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    החזר;
	}
	אם (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    החזר;
	}
	אם (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    החזר;
	}

	אם (fudi.fd_dict != عدم)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	אחרת
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    אם (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		אם (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    אחרת
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

    אם (name == عدم || !func_name_refcount(name))
	החזר;
    fp = find_func(name, FALSE);
    אם (fp == عدم && numbered_function(name))
    {
#ifdef EXITFREE
	אם (!entered_free_all_mem)
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
    فراغ
func_ptr_unref(ufunc_T *fp)
{
    אם (fp != عدم && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != عدم
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	אם (fp->uf_calls == 0)
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

    אם (name == عدم || !func_name_refcount(name))
	החזר;
    fp = find_func(name, FALSE);
    אם (fp != عدم)
	++fp->uf_refcount;
    אחרת אם (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    فراغ
func_ptr_ref(ufunc_T *fp)
{
    אם (fp != عدم)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ساكن صحيح
can_free_funccal(funccall_T *fc, صحيح copyID)
{
    החזר (fc->fc_l_varlist.lv_copyID != copyID
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

    אם (current_funccal == عدم)
    {
	emsg(_(e_return_not_inside_function));
	החזר;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    אם (eap->skip)
	++emsg_skip;

    eap->nextcmd = عدم;
    אם ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	אם (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	אחרת
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    אחרת אם (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	אם (!aborting())
	    returning = do_return(eap, FALSE, TRUE, عدم);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    אם (returning)
	eap->nextcmd = عدم;
    אחרת אם (eap->nextcmd == عدم)	    // no argument
	set_nextcmd(eap, arg);

    אם (eap->skip)
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
    עבור ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	אם (eap->addr_count > 0)
	{
	    אם (lnum > curbuf->b_ml.ml_line_count)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		اكسر;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	אם (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    اكسر;
	}
	אם (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	אם (handle_subscript(arg, عدم, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    اكسر;
	}

	clear_tv(&rettv);
	אם (doesrange)
	    اكسر;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	אם (aborting())
	    اكسر;
    }
    החזר failed;
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
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    صحيح		partial_argc = 0;		// number of partial arguments
    صحيح		argcount = 0;			// number of arguments found
    صحيح		r;

    אם (current_funccal == عدم)
    {
	semsg(_(e_str_not_inside_function), "defer");
	החזר FAIL;
    }
    אם (partial != عدم)
    {
	אם (partial->pt_dict != عدم)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    החזר FAIL;
	}
	אם (partial->pt_argc > 0)
	{
	    صحيح i;

	    partial_argc = partial->pt_argc;
	    עבור (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    صحيح is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    אם (r == OK)
    {
	אם (type != عدم)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, عدم, name);
	}
	אחרת אם (is_builtin)
	{
	    صحيح idx = find_internal_func(name);

	    אם (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    אחרת אם (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	אחרת
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    אם (ufunc != عدم)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		אם (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    אם (r == FAIL)
    {
	כלעוד (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	החזר FAIL;
    }
    החזר add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    صحيح
can_add_defer(فراغ)
{
    אם (!in_def_function() && get_current_funccal() == عدم)
    {
	semsg(_(e_str_not_inside_function), "defer");
	החזר FALSE;
    }
    החזר TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    صحيح
add_defer(char_u *name, صحيح argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    صحيح		argcount = argcount_arg;
    defer_T	*dr;
    صحيح		ret = FAIL;

    אם (saved_name == عدم)
	اذهب theend;
    אם (in_def_function())
    {
	אם (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    אחרת
    {
	אם (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, حجم(defer_T), 10);
	אם (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    اذهب theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	כלעוד (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    כלעוד (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    החזר ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ساكن فراغ
handle_defer_one(funccall_T *funccal)
{
    صحيح	    idx;

    עבור (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	אם (dr->dr_name == عدم)
	    // already being called, can happen if function does ":qa"
	    استمر;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
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
	עבור (صحيح i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ساكن فراغ
invoke_funccall_defer(funccall_T *fc)
{
    אם (fc->fc_ectx != عدم)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    אחרת
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
    עבור (funccall_T *fc = current_funccal; fc != عدم; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    עבור (funccal_entry_T *fce = funccal_stack; fce != عدم; fce = fce->next)
	עבור (funccall_T *fc = fce->top_funccal; fc != عدم; fc = fc->fc_caller)
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
    صحيح		found_var = FALSE;
    صحيح		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    אם (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	אם (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	החזר;
    }

    tofree = trans_function_name_ext(&arg, عدم, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : عدم, &ufunc);
    אם (fudi.fd_newkey != عدم)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_key_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    אם (tofree == عدم)
	החזר;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    אם (fudi.fd_dict != عدم)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (صحيح)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != عدم ? عدم : &partial,
				vim9script && type == عدم ? &type : عدم,
						     FALSE, FALSE, &found_var);

    אם (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	אם (name == عدم)
	    اذهب end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    אם (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	اذهب end;
    }
    אם (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	اذهب end;
    }

    אם (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    אחרת
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
    אם ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	אם (!ends_excmd2(eap->arg, arg))
	{
	    אם (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	אחרת
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
    صحيح
do_return(
    exarg_T	*eap,
    صحيح		reanimate,
    صحيح		is_cmd,
    فراغ	*rettv)
{
    صحيح		idx;
    cstack_T	*cstack = eap->cstack;

    אם (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    אם (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	אם (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	אחרת
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    אם (reanimate)
		rettv = current_funccal->fc_rettv;

	    אם (rettv != عدم)
	    {
		// Store the value of the pending return.
		אם ((cstack->cs_rettv[idx] = alloc_tv()) != عدم)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		אחרת
		    emsg(_(e_out_of_memory));
	    }
	    אחרת
		cstack->cs_rettv[idx] = عدم;

	    אם (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_NUMBER;
		current_funccal->fc_rettv->vval.v_number = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    אחרת
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	אם (!reanimate && rettv != عدم)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    אם (!is_cmd)
		vim_free(rettv);
	}
    }

    החזר idx < 0;
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

    אם (rettv != عدم)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    אם (s == عدم)
	s = (char_u *)"";
    אחרת
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    אם (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    החזר vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_line(
    صحيح	    c UNUSED,
    فراغ    *cookie,
    صحيح	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    אם (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    אם (do_profiling == PROF_YES)
	func_line_end(cookie);
#endif

    gap = &fp->uf_lines;
    אם (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = عدم;
    אחרת
    {
	// Skip NULL lines (continuation lines).
	כלעוד (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == عدم)
	    ++fcp->fc_linenr;
	אם (fcp->fc_linenr >= gap->ga_len)
	    retval = عدم;
	אחרת
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    אם (do_profiling == PROF_YES)
		func_line_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    אם (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    החזר retval;
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
    החזר (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    صحيح
func_has_abort(
    فراغ    *cookie)
{
    החזר ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = عدم;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    אם (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != عدم
				     && rettv->vval.v_partial->pt_func != عدم)
	fp = rettv->vval.v_partial->pt_func;
    אחרת
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == عدم ? عدم
					      : rettv->vval.v_partial->pt_name;
	אם (fname == عدم)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = عدم;
	}
	אחרת
	{
	    char_u	*tofree = عدم;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    אם (fp != عدم && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	אם (pt != عدم)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = عدم;
	    אם (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    אחרת
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		صحيح		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		אם (ret_pt->pt_name != عدم)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		אחרת
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		אם (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    אם (pt->pt_argv == عدم)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    אחרת
		    {
			pt->pt_argc = ret_pt->pt_argc;
			עבור (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    החזר selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(فراغ *cookie)
{
    החזר ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(فراغ *cookie)
{
    החזר &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    صحيح *
func_dbg_tick(فراغ *cookie)
{
    החזר &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    صحيح
func_level(فراغ *cookie)
{
    החזר ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    صحيح
current_func_returned(فراغ)
{
    החזר current_funccal->fc_returned;
}

    صحيح
free_unref_funccal(صحيح copyID, صحيح testing)
{
    صحيح		did_free = FALSE;
    صحيح		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    עבור (pfc = &previous_funccal; *pfc != عدم; )
    {
	אם (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	אחרת
	    pfc = &(*pfc)->fc_caller;
    }
    אם (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(فراغ)garbage_collect(testing);

    החזר did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ساكن funccall_T *
get_funccal(فراغ)
{
    صحيح		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    אם (debug_backtrace_level > 0)
    {
	עבור (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    אם (temp_funccal)
		funccal = temp_funccal;
	    אחרת
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    החזר funccal;
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

    אם (current_funccal == عدم)
	החזר عدم;
    funccal = get_funccal();
    אם (funccal == عدم || funccal->fc_l_vars.dv_refcount == 0)
	החזר عدم;
    החזר funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    החזר funccal == عدم ? عدم : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    החזר funccal == عدم ? عدم : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    החזר funccal == عدم ? عدم : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(فراغ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    החזר funccal == عدم ? عدم : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    فراغ
list_func_vars(صحيح *first)
{
    אם (current_funccal != عدم && current_funccal->fc_l_vars.dv_refcount > 0)
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
    אם (current_funccal != عدم
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	החזר &current_funccal->fc_l_vars;
    החזר عدم;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = عدم;
    char_u	*varname;

    אם (current_funccal == عدم || current_funccal->fc_func->uf_scoped == عدم)
	החזר عدم;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    כלעוד (current_funccal != عدم)
    {
	ht = find_var_ht(name, &varname);
	אם (ht != عدم && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    אם (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		اكسر;
	    }
	}
	אם (current_funccal == current_funccal->fc_func->uf_scoped)
	    اكسر;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    החזר hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, صحيح no_autoload)
{
    dictitem_T	*v = عدم;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    אם (current_funccal == عدم || current_funccal->fc_func->uf_scoped == عدم)
	החזר عدم;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    כלעוד (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	אם (ht != عدم && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    אם (v != عدم)
		اكسر;
	}
	אם (current_funccal == current_funccal->fc_func->uf_scoped)
	    اكسر;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    החזר v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    صحيح
set_ref_in_previous_funccal(صحيح copyID)
{
    funccall_T	*fc;

    עבור (fc = previous_funccal; fc != عدم; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	אם (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, عدم, عدم)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, عدم, عدم)
		|| set_ref_in_list_items(&fc->fc_l_varlist, copyID + 1, عدم, عدم))
	    החזר TRUE;
    }
    החזר FALSE;
}

    ساكن صحيح
set_ref_in_funccal(funccall_T *fc, صحيح copyID)
{
    אם (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	אם (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, عدم, عدم)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, عدم, عدم)
		|| set_ref_in_list_items(&fc->fc_l_varlist, copyID, عدم, عدم)
		|| set_ref_in_func(عدم, fc->fc_func, copyID))
	    החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    صحيح
set_ref_in_call_stack(صحيح copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    עבור (fc = current_funccal; fc != عدم; fc = fc->fc_caller)
	אם (set_ref_in_funccal(fc, copyID))
	    החזר TRUE;

    // Also go through the funccal_stack.
    עבור (entry = funccal_stack; entry != عدم; entry = entry->next)
	עבור (fc = entry->top_funccal; fc != عدم; fc = fc->fc_caller)
	    אם (set_ref_in_funccal(fc, copyID))
		החזר TRUE;
    החזר FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    صحيح
set_ref_in_functions(صحيح copyID)
{
    صحيح		todo;
    hashitem_T	*hi = عدم;
    ufunc_T	*fp;

    todo = (صحيح)func_hashtab.ht_used;
    עבור (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    אם (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(عدم, fp, copyID))
		החזר TRUE;
	}
    }
    החזר FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    صحيح
set_ref_in_func_args(صحيح copyID)
{
    صحيح i;

    עבור (i = 0; i < funcargs.ga_len; ++i)
	אם (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, عدم, عدم, عدم))
	    החזר TRUE;
    החזר FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    صحيح
set_ref_in_func(char_u *name, ufunc_T *fp_in, صحيح copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = عدم;
    char_u	*fname;
    صحيح		abort = FALSE;

    אם (name == عدم && fp_in == عدم)
	החזר FALSE;

    אם (fp_in == عدم)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    אם (fp != عدم)
    {
	עבור (fc = fp->uf_scoped; fc != عدم; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    החזר abort;
}

#endif // FEAT_EVAL
