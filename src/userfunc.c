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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
સ્થિર hashtab_T	func_hashtab;

// Used by get_func_tv()
સ્થિર garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
સ્થિર funccall_T *current_funccal = ખાલી;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
સ્થિર funccall_T *previous_funccal = ખાલી;

સ્થિર શૂન્ય funccal_unref(funccall_T *fc, ufunc_T *fp, પૂર્ણાંક force);
સ્થિર શૂન્ય func_clear(ufunc_T *fp, પૂર્ણાંક force);
સ્થિર પૂર્ણાંક func_free(ufunc_T *fp, પૂર્ણાંક force);
સ્થિર char_u *untrans_function_name(char_u *name);
સ્થિર શૂન્ય handle_defer_one(funccall_T *funccal);
સ્થિર char_u *trans_function_name_ext(char_u **pp, પૂર્ણાંક *is_global, પૂર્ણાંક skip, પૂર્ણાંક flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    શૂન્ય
func_init(શૂન્ય)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(શૂન્ય)
{
    પાછા &func_hashtab;
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
    સ્થિર char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	પૂર્ણાંક	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	પૂર્ણાંક	    is_vararg,
	પૂર્ણાંક	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ખાલી;
    પૂર્ણાંક		is_underscore = FALSE;

    જ્યારે (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    જો (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == ખાલી
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	જો (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	પાછા arg;
    }

    // Extra checks in Vim9 script.
    જો (!skip && argtypes != ખાલી)
    {
	પૂર્ણાંક c = *p;
	*p = NUL;
	પૂર્ણાંક r = check_reserved_name(arg, FALSE);
	*p = c;
	જો (r == FAIL)
	    પાછા arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	જો (check_defined(arg, p - arg,
				   evalarg == ખાલી ? ખાલી : evalarg->eval_cctx,
			       eap == ખાલી ? ખાલી : eap->cstack, TRUE) == FAIL)
	    પાછા arg;
    }

    જો (newargs != ખાલી && ga_grow(newargs, 1) == FAIL)
	પાછા arg;
    જો (newargs != ખાલી)
    {
	પૂર્ણાંક	c;
	પૂર્ણાંક	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	જો (arg_copy == ખાલી)
	{
	    *p = c;
	    પાછા arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	જો (argtypes == ખાલી || !is_underscore)
	    // Check for duplicate argument name.
	    માટે (i = 0; i < newargs->ga_len; ++i)
		જો (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    પાછા arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    જો (argtypes != ખાલી && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ખાલી && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ખાલી;

	જો (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ખાલી ? arg : arg_copy);
	    p = skipwhite(p);
	}
	જો (*p == ':')
	{
	    ++p;
	    જો (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		પાછા arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    જો (!skip)
		type = vim_strnsave(type, p - type);
	}
	નહીંતર જો (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == ખાલી ? arg : arg_copy);
	    પાછા arg;
	}
	જો (!skip)
	{
	    જો (type == ખાલી && types_optional)
	    {
		// lambda arguments default to "any" type
		જો (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		નહીંતર
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    પાછા p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    સ્થિર char_u *
get_function_લીટી(
	exarg_T		*eap,
	garray_T	*લીટીઓ_to_free,
	પૂર્ણાંક		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    જો (eap->ea_getline == ખાલી)
	theline = getcmdline(':', 0L, indent, 0);
    નહીંતર
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    જો (theline != ખાલી)
    {
	જો (લીટીઓ_to_free->ga_len > 0
		&& eap->cmdlinep != ખાલી
		&& *eap->cmdlinep == ((char_u **)લીટીઓ_to_free->ga_data)
						   [લીટીઓ_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(શૂન્ય)ga_add_string(લીટીઓ_to_free, theline);
    }

    પાછા theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    સ્થિર પૂર્ણાંક
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    પૂર્ણાંક		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    પૂર્ણાંક		*varargs,
    garray_T	*default_args,
    પૂર્ણાંક		skip,
    exarg_T	*eap,		// can be NULL
    પૂર્ણાંક		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*લીટીઓ_to_free)
{
    પૂર્ણાંક		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		any_default = FALSE;
    char_u	*whitep = *argp;
    પૂર્ણાંક		need_પદાવલિ = FALSE;

    જો (newargs != ખાલી)
	ga_init2(newargs, કદ(char_u *), 3);
    જો (argtypes != ખાલી)
	ga_init2(argtypes, કદ(char_u *), 3);
    જો (arg_objm != ખાલી)
	ga_init2(arg_objm, કદ(int8_T), 3);
    જો (!skip && default_args != ખાલી)
	ga_init2(default_args, કદ(char_u *), 3);

    જો (varargs != ખાલી)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    જ્યારે (*p != endchar)
    {
	જ્યારે (eap != ખાલી && eap->ea_getline != ખાલી
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_લીટી(eap, લીટીઓ_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    જો (theline == ખાલી)
		તોડો;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	જો (mustend && *p != endchar)
	{
	    જો (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    જાઓ err_ret;
	}
	જો (*p == endchar && !need_પદાવલિ)
	    તોડો;

	જો (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    જો (varargs != ખાલી)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    જો (argtypes != ખાલી)
	    {
		// ...name: list<type>
		જો (!eval_isnamec1(*p))
		{
		    જો (!skip)
			emsg(_(e_missing_name_after_dots));
		    જાઓ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		જો (p == arg)
		    તોડો;
		જો (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    તોડો;
		}
	    }
	}
	નહીંતર જો (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    જ્યારે (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    જો (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		તોડો;
	    }

	    જો (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		જો (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_કિંમત_must_be_vnone_str), p);
		    જાઓ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		જો (ga_grow(default_args, 1) == FAIL)
		    જાઓ err_ret;

		char_u *પદાવલિ = vim_strnsave((char_u *)"v:none", 6);
		જો (પદાવલિ == ખાલી)
		    જાઓ err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = પદાવલિ;
		default_args->ga_len++;
	    }
	    નહીંતર જો (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		જાઓ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    જો (newargs != ખાલી && ga_grow(newargs, 1) == FAIL)
		પાછા FAIL;
	    જો (newargs != ખાલી)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		જો (argtypes != ખાલી && ga_grow(argtypes, 1) == OK
			    && arg_objm != ખાલી && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    જો (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			પૂર્ણાંક len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			જો (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			જો (assignment != ખાલી)
			{
			    c = *argend;
			    *argend = NUL;
			    જો (any_default)
				vim_snprintf((અક્ષર *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    નહીંતર
				vim_snprintf((અક્ષર *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    જો (*p == ',')
		++p;
	}
	નહીંતર
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    જો (p == arg)
		તોડો;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    જો (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != ખાલી)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *પદાવલિ = p;
		જો (eval1(&p, &rettv, ખાલી) != FAIL)
		{
		    જો (!skip)
		    {
			જો (ga_grow(default_args, 1) == FAIL)
			    જાઓ err_ret;

			જો (need_પદાવલિ)
			    need_પદાવલિ = FALSE;
			// trim trailing whitespace
			જ્યારે (p > પદાવલિ && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			પદાવલિ = vim_strsave(પદાવલિ);
			જો (પદાવલિ == ખાલી)
			{
			    *p = c;
			    જાઓ err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = પદાવલિ;
			default_args->ga_len++;
			*p = c;
		    }
		}
		નહીંતર
		{
		    mustend = TRUE;
		    જો (*skipwhite(p) == NUL)
			need_પદાવલિ = TRUE;
		}
	    }
	    નહીંતર જો (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		જાઓ err_ret;
	    }

	    જો (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		જો (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    જાઓ err_ret;
		}
		p = skipwhite(p);
	    }
	    જો (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		જો (!skip && argtypes != ખાલી
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    જાઓ err_ret;
		}
	    }
	    નહીંતર
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    જો (*p != endchar)
	જાઓ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    પાછા OK;

err_ret:
    જો (newargs != ખાલી)
	ga_clear_strings(newargs);
    જો (!skip && default_args != ખાલી)
	ga_clear_strings(default_args);
    પાછા FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    પૂર્ણાંક		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    પૂર્ણાંક		obj_member_ગણતરી,
    cctx_T	*cctx)
{
    પૂર્ણાંક len = 0;

    ga_init2(&fp->uf_type_list, કદ(type_T *), 10);
    જો (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	જો (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	જો (fp->uf_arg_types != ખાલી)
	{
	    પૂર્ણાંક	i;
	    type_T	*type;

	    માટે (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		જો (p == ખાલી)
		    // will get the type from the default value
		    type = &t_unknown;
		નહીંતર
		{
		    જો (arg_objm != ખાલી && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			માટે (પૂર્ણાંક om = 0; om < obj_member_ગણતરી; ++om)
			{
			    જો (obj_members != ખાલી
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				તોડો;
			    }
			}
		    }
		    નહીંતર
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		જો (type == ખાલી || !valid_declaration_type(type))
		    પાછા FAIL;
		fp->uf_arg_types[i] = type;
		જો (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    જો (obj_members != ખાલી && *name == '_')
			// protected object method
			name++;

		    જો (var_wrong_func_name(name, TRUE))
			પાછા FAIL;
		}
	    }
	}
    }

    જો (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = ખાલી;
	p = ((char_u **)argtypes->ga_data)[len];
	જો (p == ખાલી)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	નહીંતર
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    જો (fp->uf_va_type != ખાલી && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		પાછા FAIL;
	    }
	}
	જો (fp->uf_va_type == ખાલી)
	    પાછા FAIL;
    }

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    જો (ret_type == ખાલી)
	fp->uf_ret_type = &t_void;
    નહીંતર
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	જો (fp->uf_ret_type == ખાલી)
	{
	    fp->uf_ret_type = &t_void;
	    પાછા FAIL;
	}
    }
    પાછા OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    સ્થિર પૂર્ણાંક
register_closure(ufunc_T *fp)
{
    જો (fp->uf_scoped == current_funccal)
	// no change
	પાછા OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    જો (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	પાછા FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    પાછા OK;
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

    જો (name[0] != K_SPECIAL)
	પાછા name;
    len = STRLEN(name);
    જો (len + 3 > bufsize)
	પાછા name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    પાછા buf;
}

સ્થિર char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(શૂન્ય)
{
    સ્થિર પૂર્ણાંક	lambda_no = 0;
    પૂર્ણાંક		n;
    string_T	ret;

    n = vim_snprintf((અક્ષર *)lambda_name, કદ(lambda_name), "<lambda>%d", ++lambda_no);
    જો (n < 1)
	ret.length = 0;
    નહીંતર જો (n >= (પૂર્ણાંક)કદ(lambda_name))
	ret.length = કદ(lambda_name) - 1;
    નહીંતર
	ret.length = (size_t)n;

    ret.string = lambda_name;
    પાછા ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    સ્થિર ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < કદ(ufunc_T) ? કદ(ufunc_T) : len);
    જો (fp != ખાલી)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((શૂન્ય *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	જો (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    જો (fp->uf_name_exp != ખાલી)
		vim_snprintf((અક્ષર *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    પાછા fp;
}

#જો defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, શૂન્ય *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    જો (fp == ખાલી)
	પાછા ખાલી;

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

    પાછા name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    સ્થિર char_u *
skip_arrow(
	char_u	*start,
	પૂર્ણાંક	equal_arrow,
	char_u	**ret_type,
	પૂર્ણાંક	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    જો (equal_arrow)
    {
	જો (*s == ':')
	{
	    જો (white_error != ખાલી && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		પાછા ખાલી;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    જો (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		પાછા ખાલી;
	    }
	}
	bef = s;
	s = skipwhite(s);
	જો (*s != '=')
	    પાછા ખાલી;
	++s;
    }
    જો (*s != '>')
	પાછા ખાલી;
    જો (white_error != ખાલી && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	પાછા ખાલી;
    }
    પાછા skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    સ્થિર પૂર્ણાંક
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    જો (checkforcmd(&p, "function", 2))
    {
	જો (*p == '(')
	    પાછા FALSE;
	*cmd = p;
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    સ્થિર શૂન્ય
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    જો (cstack == ખાલી || cstack->cs_idx < 0)
	પાછા;

    પૂર્ણાંક	    ગણતરી = cstack->cs_idx + 1;
    પૂર્ણાંક	    i;

    fp->uf_block_ids = ALLOC_MULT(પૂર્ણાંક, ગણતરી);
    જો (fp->uf_block_ids != ખાલી)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		કદ(પૂર્ણાંક) * ગણતરી);
	fp->uf_block_depth = ગણતરી;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    માટે (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    સ્થિર char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    જ્યારે (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	જો (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	નહીંતર જો (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    તોડો;
	}
	નહીંતર
	    MB_PTR_ADV(p);
    }

    પાછા p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    સ્થિર char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    જ્યારે (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	જો (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	નહીંતર જો (*p == '"')
	{
	    p++;    // skip the ending double quote
	    તોડો;
	}
	નહીંતર
	    MB_PTR_ADV(p);
    }

    પાછા p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    સ્થિર char_u *
find_start_of_vim9_comment(char_u *લીટી)
{
    char_u	*p = લીટી;

    જ્યારે (*p != NUL)
    {
	જો (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	નહીંતર જો (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	નહીંતર
	{
	    જો (*p == '#')
		// Found the start of a Vim9 comment
		તોડો;
	    MB_PTR_ADV(p);
	}
    }

    પાછા p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    સ્થિર પૂર્ણાંક
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *લીટી_arg_in,
	garray_T    *લીટીઓ_to_free)
{
    linenr_T	sourcing_lnum_ટોચ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    પૂર્ણાંક		saved_wait_return = need_wait_return;
    char_u	*લીટી_arg = લીટી_arg_in;
    પૂર્ણાંક		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    અક્ષર	nesting_def[MAX_FUNC_NESTING];
    અક્ષર	nesting_inline[MAX_FUNC_NESTING];
    પૂર્ણાંક		nesting = 0;
    getline_opt_T getline_options;
    પૂર્ણાંક		indent = 2;
    char_u	*skip_until = ખાલી;
    પૂર્ણાંક		ret = FAIL;
    પૂર્ણાંક		is_heredoc = FALSE;
    પૂર્ણાંક		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ખાલી;
    size_t	heredoc_trimmedlen = 0;
    પૂર્ણાંક		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    જો (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	જો (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    જાઓ theend;
	જ્યારે (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ખાલી;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    માટે (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	જો (ચાવી_Typed)
	{
	    સંદેશ_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	જો (લીટી_arg != ખાલી)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = લીટી_arg;
	    p = vim_strchr(theline, '\n');
	    જો (p == ખાલી)
		લીટી_arg += STRLEN(લીટી_arg);
	    નહીંતર
	    {
		*p = NUL;
		લીટી_arg = p + 1;
	    }
	}
	નહીંતર
	{
	    theline = get_function_લીટી(eap, લીટીઓ_to_free, indent,
							      getline_options);
	}
	જો (ચાવી_Typed)
	    લીટીઓ_ડાબું = Rows - 1;
	જો (theline == ખાલી)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_ટોચ;
	    જો (skip_until != ખાલી)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    નહીંતર જો (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    નહીંતર જો (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    નહીંતર
		emsg(_(e_missing_endfunction));
	    જાઓ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	જો (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	નહીંતર
	    sourcing_lnum_off = 0;

	જો (skip_until != ખાલી)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    જો (heredoc_trimmed == ખાલી
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		જો (heredoc_trimmed == ખાલી)
		    p = theline;
		નહીંતર જો (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		નહીંતર
		    p = theline + heredoc_trimmedlen;
		જો (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    જો (heredoc_concat_len > 0)
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
	નહીંતર
	{
	    પૂર્ણાંક	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    માટે (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		જો (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    જો (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		જો (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		જો (nesting-- == 0)
		{
		    char_u *nextcmd = ખાલી;

		    જો (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    નહીંતર જો (લીટી_arg != ખાલી && *skipwhite(લીટી_arg) != NUL)
			nextcmd = લીટી_arg;
		    નહીંતર જો (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_ટોચ
							+ newlines->ga_len + 1;
			જો (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			નહીંતર
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    જો (nextcmd != ખાલી && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			જો (લીટીઓ_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)લીટીઓ_to_free->ga_data)
						   [લીટીઓ_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)લીટીઓ_to_free->ga_data)
						   [લીટીઓ_to_free->ga_len - 1];
			    --લીટીઓ_to_free->ga_len;
			}
		    }
		    તોડો;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    નહીંતર જો (nesting_def[nesting])
	    {
		જો (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    નહીંતર જો (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    જો (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    નહીંતર જો (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    જો (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		જો (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ખાલી, TRUE, 0));
		જો (vim9_function && *p == '<')
		{
		    // skip generic function
		    જો (skip_generic_func_type_args(&p) == FAIL)
			જાઓ theend;
		}
		જો (*skipwhite(p) == '(')
		{
		    જો (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    નહીંતર
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    જો (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		જો (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		નહીંતર
		    end = p + STRLEN(p) - 1;

		જ્યારે (end > p && VIM_ISWHITE(*end))
		    --end;
		જો (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    પૂર્ણાંક	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    જ્યારે (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    જો (!is_block)
			is_block = find_cmd_block_start(p) != ખાલી;

		    જો (is_block)
		    {
			જો (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			નહીંતર
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
	    જો (!vim9_function || colon)
		p = skip_range(p, FALSE, ખાલી);
	    char_u *tp = p;
	    જો (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    નહીંતર
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    જો (arg[0] == '<' && arg[1] =='<'
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
		જો (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    જો (heredoc_trimmed == ખાલી)
			heredoc_trimmedlen = 0;
		}
		જો (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		નહીંતર
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		જો (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    જો (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		જો (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    પૂર્ણાંક		save_sc_version = current_sctx.sc_version;
		    પૂર્ણાંક		var_ગણતરી = 0;
		    પૂર્ણાંક		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_ગણતરી, &semicolon,
									 TRUE);
		    જો (arg != ખાલી)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    જો (arg != ખાલી && STRNCMP(arg, "=<<", 3) == 0)
		    {
			પૂર્ણાંક has_trim = FALSE;

			p = skipwhite(arg + 3);
			જ્યારે (TRUE)
			{
			    જો (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ચાલુ;
			    }
			    જો (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ચાલુ;
			    }
			    તોડો;
			}
			જો (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    જો (heredoc_trimmed == ખાલી)
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
	જો (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    જાઓ theend;

	જો (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	નહીંતર
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	જો (p == ખાલી)
	    જાઓ theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	જ્યારે (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ખાલી;

	// Check for end of eap->arg.
	જો (લીટી_arg != ખાલી && *લીટી_arg == NUL)
	    લીટી_arg = ખાલી;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    જો (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    પાછા ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    સ્થિર પૂર્ણાંક
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	પૂર્ણાંક	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    પૂર્ણાંક		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ખાલી;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ખાલી;
    પૂર્ણાંક		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    પૂર્ણાંક		lnum_save = -1;
    linenr_T	sourcing_lnum_ટોચ = SOURCING_LNUM;
    char_u	*લીટી_arg = ખાલી;

    *arg = skipwhite(*arg + 1);
    જો (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	પાછા FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    જો (**arg == '\n')
	લીટી_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    જો (evalarg->eval_cctx != ખાલી)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    નહીંતર
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, કદ(char_u *), 10);
    જો (get_function_body(&eap, &newlines, લીટી_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	જાઓ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_ગણતરી += newlines.ga_len;
    જો (gap->ga_itemsize > 0)
    {
	પૂર્ણાંક	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	માટે (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    જો (p == ખાલી)
		// comment line in the lambda body
		ચાલુ;

	    p = skipwhite(p);

	    જો (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		જાઓ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    જો (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    નહીંતર
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    જો (pnl != ખાલી)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	જો (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    જાઓ erret;
	જો (eap.nextcmd != ખાલી)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	નહીંતર
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	જો (pnl != ખાલી)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    જો (eap.nextcmd != ખાલી)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	જો (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    નહીંતર
	*arg = (char_u *)"";

    જો (!evaluate)
    {
	ret = OK;
	જાઓ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    જો (ufunc == ખાલી)
	જાઓ erret;
    જો (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	જાઓ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = ખાલી;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = ખાલી;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_ટોચ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    જો (parse_argument_types(ufunc, argtypes, varargs, ખાલી, ખાલી, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	જાઓ erret;
    }

    // parse the return type, if any
    જો (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	જાઓ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    જો (pt == ખાલી)
	જાઓ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_લીટીઓ = newlines;
    newlines.ga_data = ખાલી;
    જો (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    જો (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_ટોચ;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ખાલી;
    ret = OK;

erret:
    જો (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    જો (newargs != ખાલી)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    જો (ufunc != ખાલી)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    પાછા ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    પૂર્ણાંક
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	પૂર્ણાંક	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = ખાલી;
    partial_T   *pt = ખાલી;
    પૂર્ણાંક		varargs;
    char_u	*ret_type = ખાલી;
    પૂર્ણાંક		ret;
    char_u	*s;
    char_u	*start, *end;
    પૂર્ણાંક		*old_eval_lavars = eval_lavars_used;
    પૂર્ણાંક		eval_lavars = FALSE;
    char_u	*tofree2 = ખાલી;
    પૂર્ણાંક		equal_arrow = **arg == '(';
    પૂર્ણાંક		white_error = FALSE;
    પૂર્ણાંક		called_emsg_start = called_emsg;
    પૂર્ણાંક		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    જો (equal_arrow && !vim9script)
	પાછા NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', ખાલી,
	    types_optional ? &argtypes : ખાલી, types_optional,
			types_optional ? &arg_objm : ખાલી, evalarg,
			ખાલી, &default_args, TRUE, ખાલી, FALSE, ખાલી, ખાલી);
    જો (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ખાલી) == ખાલી)
    {
	જો (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	પાછા called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    જો (evaluate)
	pnewargs = &newargs;
    નહીંતર
	pnewargs = ખાલી;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ખાલી, types_optional,
			types_optional ? &arg_objm : ખાલી, evalarg,
					    &varargs, &default_args,
					    FALSE, ખાલી, FALSE, ખાલી, ખાલી);
    જો (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ખાલી)) == ખાલી)
    {
	જો (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	પાછા white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    જો (ret_type != ખાલી)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    જો (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    જો (equal_arrow && **arg == '{')
    {
	જો (evalarg == ખાલી)
	    // cannot happen?
	    જાઓ theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	જો (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ખાલી, varargs,
			   &default_args, ret_type) == FAIL)
	    જાઓ errret;
	જાઓ theend;
    }
    જો (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_કિંમતો_in_lambda));
	જાઓ errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_પદાવલિ_concatenate(arg, &start, &end, evalarg);
    જો (ret == FAIL)
	જાઓ errret;

    જો (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg != '}')
	{
	    semsg(_(e_expected_જમણું_curly_str), *arg);
	    જાઓ errret;
	}
	++*arg;
    }

    જો (evaluate)
    {
	પૂર્ણાંક	    len;
	પૂર્ણાંક	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *લીટી_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	જો (fp == ખાલી)
	    જાઓ errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	જો (pt == ખાલી)
	    જાઓ errret;

	ga_init2(&newlines, કદ(char_u *), 1);
	જો (ga_grow(&newlines, 1) == FAIL)
	    જાઓ errret;

	// If there are line breaks, we need to split up the string.
	લીટી_end = vim_strchr(start, '\n');
	જો (લીટી_end == ખાલી || લીટી_end > end)
	    લીટી_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (પૂર્ણાંક)(લીટી_end - start) + 1;
	p = alloc(len);
	જો (p == ખાલી)
	    જાઓ errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, લીટી_end - start);

	જો (લીટી_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    જ્યારે (લીટી_end < end && *લીટી_end == '\n')
	    {
		જો (ga_grow(&newlines, 1) == FAIL)
		    જાઓ errret;
		start = લીટી_end + 1;
		લીટી_end = vim_strchr(start, '\n');
		જો (લીટી_end == ખાલી || લીટી_end > end)
		    લીટી_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, લીટી_end - start);
	    }
	}

	જો (strstr((અક્ષર *)p + 7, "a:") == ખાલી)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	જો (types_optional)
	{
	    જો (parse_argument_types(fp, &argtypes,
				vim9script && varargs, ખાલી, ખાલી, 0,
				cctx) == FAIL)
		જાઓ errret;
	    જો (ret_type != ખાલી)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ખાલી, cctx, TRUE);
		જો (fp->uf_ret_type == ખાલી)
		    જાઓ errret;
	    }
	    નહીંતર
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_લીટીઓ = newlines;
	જો (current_funccal != ખાલી && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    જો (register_closure(fp) == FAIL)
		જાઓ errret;
	}

#ifdef FEAT_PROFILE
	જો (prof_def_func())
	    func_do_profile(fp);
#endif
	જો (sandbox)
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
    જો (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    પાછા OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    જો (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    જો (fp != ખાલી)
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
    પાછા FAIL;
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
	પૂર્ણાંક	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	પૂર્ણાંક	    no_autoload,
	પૂર્ણાંક	    new_function,
	પૂર્ણાંક	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = ખાલી;
    પૂર્ણાંક		cc;
    char_u	*s = ખાલી;
    hashtab_T	*ht;
    પૂર્ણાંક		did_type = FALSE;

    જો (partialp != ખાલી)
	*partialp = ખાલી;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    જો (v != ખાલી)
    {
	tv = &v->di_tv;
    }
    નહીંતર જો (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	પૂર્ણાંક	    len = *lenp;

	જો (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	જો (import != ખાલી)
	{
	    name[len] = NUL;
	    જો (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    નહીંતર
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    પાછા (char_u *)"";	// just in case
	}
    }

    જો (tv != ખાલી)
    {
	જો (found_var != ખાલી)
	    *found_var = TRUE;
	જો (tv->v_type == VAR_FUNC)
	{
	    જો (tv->vval.v_string == ખાલી)
	    {
		*lenp = 0;
		પાછા (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    જો (p != ખાલી && generic_func_find_open_bracket(name) != ખાલી)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		પાછા (char_u *)"";
	    }
	    જો (p != ખાલી)
		*lenp = p - s;
	    નહીંતર
		*lenp = (પૂર્ણાંક)STRLEN(s);
	}
	નહીંતર જો (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    જો (pt == ખાલી)
	    {
		*lenp = 0;
		પાછા (char_u *)"";	// just in case
	    }
	    જો (partialp != ખાલી)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (પૂર્ણાંક)STRLEN(s);
	}

	જો (s != ખાલી)
	{
	    જો (!did_type && type != ખાલી && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		જો (sv != ખાલી)
		    *type = sv->sv_type;
	    }
	    પાછા s;
	}
    }

    પાછા name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    શૂન્ય
emsg_funcname(અક્ષર *ermsg, char_u *name)
{
    char_u	*p = name;

    જો (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    જો (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    પૂર્ણાંક
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	પૂર્ણાંક	    partial_argc,
	typval_T    *argvars,
	પૂર્ણાંક	    *argcount,
	પૂર્ણાંક	    is_builtin)
{
    char_u	*argp = *arg;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		evaluate = evalarg == ખાલી
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    જ્યારે (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	જો (*argp == ')' || *argp == ',' || *argp == NUL)
	    તોડો;

	પૂર્ણાંક arg_idx = *argcount;
	જો (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    તોડો;
	}
	++*argcount;
	જો (!is_builtin && check_typval_is_કિંમત(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    તોડો;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	જો (vim9script)
	{
	    જો (*argp != ',' && *skipwhite(argp) == ',')
	    {
		જો (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		તોડો;
	    }
	}
	નહીંતર
	    argp = skipwhite(argp);
	જો (*argp != ',')
	    તોડો;
	જો (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    જો (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    તોડો;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    જો (*argp == ')')
	++argp;
    નહીંતર
	ret = FAIL;
    *arg = argp;
    પાછા ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
get_func_tv(
    char_u	*name,		// name of the function
    પૂર્ણાંક		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    પૂર્ણાંક		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    પૂર્ણાંક		argcount = 0;			// number of arguments found
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		evaluate = evalarg == ખાલી
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ખાલી ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    જો (ret == OK)
    {
	પૂર્ણાંક	i = 0;
	પૂર્ણાંક	did_emsg_before = did_emsg;

	જો (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    જો (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, કદ(typval_T *), 50);
	    માટે (i = 0; i < argcount; ++i)
		જો (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	જો (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    નહીંતર જો (!aborting() && evaluate)
    {
	જો (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	નહીંતર
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    જ્યારે (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    જો (vim9script)
	*arg = argp;
    નહીંતર
	*arg = skipwhite(argp);
    પાછા ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    સ્થિર પૂર્ણાંક
eval_fname_sid(char_u *p)
{
    પાછા (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    જો (script_name == name)
	પાછા name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (પૂર્ણાંક)KE_SNR;
    fname_buflen = 3;
    જો (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    નહીંતર
    {
	જો (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	નહીંતર
	{
	    fname_buflen += vim_snprintf((અક્ષર *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    જો (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    નહીંતર
    {
	fname = alloc(fnamelen + 1);
	જો (fname == ખાલી)
	    *error = FCERR_OTHER;
	નહીંતર
	{
	    *tofree = fname;
	    vim_snprintf((અક્ષર *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    પાછા fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    શૂન્ય
func_name_with_sid(char_u *name, પૂર્ણાંક sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (પૂર્ણાંક)KE_SNR;
    vim_snprintf((અક્ષર *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    સ્થિર ufunc_T *
find_func_with_sid(char_u *name, પૂર્ણાંક sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    જો (!SCRIPT_ID_VALID(sid))
	પાછા ખાલી;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    જો (!HASHITEM_EMPTY(hi))
	પાછા HI2UF(hi);
    પાછા ખાલી;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    સ્થિર ufunc_T *
find_func_with_prefix(char_u *name, પૂર્ણાંક sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    જો (vim_strchr(name, AUTOLOAD_CHAR) != ખાલી)
	પાછા ખાલી;	// already has the prefix
    જો (!SCRIPT_ID_VALID(sid))
	પાછા ખાલી;	// not in a script
    si = SCRIPT_ITEM(sid);
    જો (si->sn_autoload_prefix != ખાલી)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	જો (namep == ખાલી)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	જો (len < કદ(buffer))
	    auto_name = buffer;
	નહીંતર
	    auto_name = alloc(len);
	જો (auto_name != ખાલી)
	{
	    vim_snprintf((અક્ષર *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    જો (auto_name != buffer)
		vim_free(auto_name);
	    જો (!HASHITEM_EMPTY(hi))
		પાછા HI2UF(hi);
	}
    }

    પાછા ખાલી;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, પૂર્ણાંક flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    જો ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	જો (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    જો (func != ખાલી)
		પાછા func;
	}
	જો (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    જો (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (પૂર્ણાંક)sid);
		જો (func != ખાલી)
		    પાછા func;
	    }
	}
    }

    જો ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	જો (!HASHITEM_EMPTY(hi))
	    પાછા HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    પાછા find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, પૂર્ણાંક is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    જો (fp != ખાલી && (fp->uf_flags & FC_DEAD) == 0)
	પાછા fp;
    પાછા ખાલી;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    પૂર્ણાંક
func_is_global(ufunc_T *ufunc)
{
    પાછા ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    પૂર્ણાંક
func_requires_g_prefix(ufunc_T *ufunc)
{
    પાછા func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ખાલી
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    સ્થિર પૂર્ણાંક
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    પૂર્ણાંક	len;

    જો (!func_is_global(fp))
	len = vim_snprintf((અક્ષર *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    નહીંતર
	len = vim_snprintf((અક્ષર *)buf, bufsize, "%s", fp->uf_name);

    પાછા (len >= (પૂર્ણાંક)bufsize) ? (પૂર્ણાંક)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    સ્થિર શૂન્ય
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    અક્ષર	*name,
    varnumber_T nr)
{
    STRCPY(v->di_ચાવી, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_સંખ્યા;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_સંખ્યા = nr;
}

/*
 * Free "fc".
 */
    સ્થિર શૂન્ય
free_funccal(funccall_T *fc)
{
    પૂર્ણાંક	i;

    માટે (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	જો (fp != ખાલી && fp->uf_scoped == fc)
	    fp->uf_scoped = ખાલી;
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
   સ્થિર શૂન્ય
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_વસ્તુઓ(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    સ્થિર શૂન્ય
cleanup_function_call(funccall_T *fc)
{
    પૂર્ણાંક	may_free_fc = fc->fc_refcount <= 0;
    પૂર્ણાંક	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    જો (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    નહીંતર
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    જો (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    નહીંતર
    {
	પૂર્ણાંક	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (પૂર્ણાંક)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    જો (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    જો (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = ખાલી;
    નહીંતર
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_વસ્તુઓ(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    જો (free_fc)
	free_funccal(fc);
    નહીંતર
    {
	સ્થિર પૂર્ણાંક made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	જો (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	નહીંતર જો (++made_copy >= (પૂર્ણાંક)((4096 * 1024) / કદ(*fc)))
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
    સ્થિર પૂર્ણાંક
numbered_function(char_u *name)
{
    પાછા SAFE_isdigit(*name)
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
    પૂર્ણાંક
func_name_refcount(char_u *name)
{
    પાછા numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    સ્થિર શૂન્ય
funccal_unref(funccall_T *fc, ufunc_T *fp, પૂર્ણાંક force)
{
    funccall_T	**pfc;
    પૂર્ણાંક		i;

    જો (fc == ખાલી)
	પાછા;

    જો (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	માટે (pfc = &previous_funccal; *pfc != ખાલી; pfc = &(*pfc)->fc_caller)
	{
	    જો (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		પાછા;
	    }
	}
    માટે (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	જો (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = ખાલી;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    સ્થિર પૂર્ણાંક
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    જો (fp->uf_flags & FC_DEAD)
	પાછા FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    જો (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	પાછા FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    જો (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	પાછા FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    પાછા TRUE;
}

    સ્થિર શૂન્ય
func_clear_વસ્તુઓ(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_લીટીઓ));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    જો (IS_GENERIC_FUNC(fp))
	generic_func_clear_વસ્તુઓ(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ખાલી;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    જો (fp->uf_cb_free != ખાલી)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ખાલી;
    }

    fp->uf_cb_state = ખાલી;
    fp->uf_cb = ખાલી;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_ગણતરી);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    સ્થિર શૂન્ય
func_clear(ufunc_T *fp, પૂર્ણાંક force)
{
    જો (fp->uf_cleared)
	પાછા;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_વસ્તુઓ(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    સ્થિર પૂર્ણાંક
func_free(ufunc_T *fp, પૂર્ણાંક force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    જો ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    જો ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	જો (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	પાછા OK;
    }
    પાછા FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    શૂન્ય
func_clear_free(ufunc_T *fp, પૂર્ણાંક force)
{
    func_clear(fp, force);
    જો (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    નહીંતર
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    પૂર્ણાંક
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ખાલી;

    જો (ufunc == ખાલી)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	પાછા FAIL;
    }

    fp = find_func(global, TRUE);
    જો (fp != ખાલી)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	પાછા FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    જો (fp == ખાલી)
	પાછા FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    જો (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_લીટીઓ, &fp->uf_લીટીઓ) == FAIL)
	જાઓ failed;

    જો (ufunc->uf_arg_types != ખાલી)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	જો (fp->uf_arg_types == ખાલી)
	    જાઓ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    કદ(type_T *) * fp->uf_args.ga_len);
    }
    જો (ufunc->uf_va_name != ખાલી)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	જો (fp->uf_va_name == ખાલી)
	    જાઓ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    જો ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == ખાલી)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	જો (pt == ખાલી)
	    જાઓ failed;
	જો (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    જાઓ failed;
	}
	ufunc->uf_partial = pt;
    }

    પાછા OK;

failed:
    func_clear_free(fp, TRUE);
    પાછા FAIL;
}

સ્થિર પૂર્ણાંક	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    પૂર્ણાંક
funcdepth_increment(શૂન્ય)
{
    જો (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	પાછા FAIL;
    }
    ++funcdepth;
    પાછા OK;
}

    શૂન્ય
funcdepth_decrement(શૂન્ય)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    પૂર્ણાંક
funcdepth_get(શૂન્ય)
{
    પાછા funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    શૂન્ય
funcdepth_restore(પૂર્ણાંક depth)
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

    જો (fc == ખાલી)
	પાછા ખાલી;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    પાછા fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    શૂન્ય
remove_funccal(શૂન્ય)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    સ્થિર funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    પૂર્ણાંક		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    પૂર્ણાંક		using_sandbox = FALSE;
    પૂર્ણાંક		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    પૂર્ણાંક		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    પૂર્ણાંક		default_arg_err = FALSE;
    dictitem_T	*v;
    પૂર્ણાંક		fixvar_idx = 0;	// index in fc_fixvar[]
    પૂર્ણાંક		i;
    પૂર્ણાંક		ai;
    પૂર્ણાંક		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    પૂર્ણાંક		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    જો (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = -1;
	પાછા FCERR_FAILED;
    }

    લીટી_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    જો (fc == ખાલી)
	પાછા FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, કદ(ufunc_T *), 1);

    જો (fp->uf_def_status != UF_NOT_COMPILED)
    {
	જો (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ખાલી ? ખાલી : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	જો (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	જો (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	જો (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	જો (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ખાલી && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	પાછા retval;
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
    જો (selfdict != ખાલી)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ચાવી;
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
    જો ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    જો ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ચાવી;
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
    જો ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    માટે (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	પૂર્ણાંક	    addlocal = FALSE;
	typval_T    def_rettv;
	પૂર્ણાંક	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	જો (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    જો (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_સંખ્યા == VVAL_NONE));
	    જો (isdefault)
	    {
		char_u	    *default_પદાવલિ = ખાલી;

		def_rettv.v_type = VAR_સંખ્યા;
		def_rettv.vval.v_સંખ્યા = -1;

		default_પદાવલિ = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		જો (eval1(&default_પદાવલિ, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    તોડો;
		}
	    }

	    namelen = STRLEN(name);
	}
	નહીંતર
	{
	    જો ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		તોડો;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((અક્ષર *)numbuf, કદ(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	જો (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_ચાવી, name);
	}
	નહીંતર
	{
	    v = dictitem_alloc(name);
	    જો (v == ખાલી)
		તોડો;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	જો (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	જો (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	નહીંતર
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	જો (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    જો (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ધકેલો_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    જો (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	જો (p_verbose >= 14)
	{
	    char_u	buf[સંદેશ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    સંદેશ_puts("(");
	    માટે (i = 0; i < argcount; ++i)
	    {
		જો (i > 0)
		    સંદેશ_puts(", ");
		જો (argvars[i].v_type == VAR_સંખ્યા)
		    સંદેશ_outnum((long)argvars[i].vval.v_સંખ્યા);
		નહીંતર
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    જો (s != ખાલી)
		    {
			જો (vim_strsize(s) > સંદેશ_BUF_CLEN)
			{
			    trunc_string(s, buf, સંદેશ_BUF_CLEN, સંદેશ_BUF_LEN);
			    s = buf;
			}
			સંદેશ_puts((અક્ષર *)s);
			vim_free(tofree);
		    }
		}
	    }
	    સંદેશ_puts(")");
	}
	સંદેશ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == ખાલી ? ખાલી : fc->fc_caller->fc_func);
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

    જો (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    નહીંતર જો (islambda)
    {
	char_u *p = *(char_u **)fp->uf_લીટીઓ.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(શૂન્ય)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    નહીંતર
	// call do_cmdline() to execute the lines
	do_cmdline(ખાલી, get_func_લીટી, (શૂન્ય *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    જો ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_સંખ્યા;
	rettv->vval.v_સંખ્યા = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	જો (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ખાલી ? ખાલી : fc->fc_caller->fc_func;

	જો (fp->uf_profiling || (caller != ખાલી && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    જો (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	જો (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	નહીંતર જો (fc->fc_rettv->v_type == VAR_સંખ્યા)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_સંખ્યા);
	નહીંતર
	{
	    char_u	buf[સંદેશ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    જો (s != ખાલી)
	    {
		જો (vim_strsize(s) > સંદેશ_BUF_CLEN)
		{
		    trunc_string(s, buf, સંદેશ_BUF_CLEN, સંદેશ_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	સંદેશ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_કાઢો();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    જો (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    જો (p_verbose >= 12 && SOURCING_NAME != ખાલી)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	સંદેશ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    માટે (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    પાછા retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, પૂર્ણાંક argcount)
{
    પૂર્ણાંક regular_args = fp->uf_args.ga_len;

    જો (argcount < regular_args - fp->uf_def_args.ga_len)
	પાછા FCERR_TOOFEW;
    નહીંતર જો (!has_varargs(fp) && argcount > regular_args)
	પાછા FCERR_TOOMANY;
    પાછા FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	પૂર્ણાંક	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    જો (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	પાછા (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    જો (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ખાલી)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    જો (error != FCERR_UNKNOWN)
	પાછા error;

    જો ((fp->uf_flags & FC_DICT) && selfdict == ખાલી)
    {
	error = FCERR_DICT;
    }
    નહીંતર
    {
	પૂર્ણાંક		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_શોધ_patterns();
	જો (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ખાલી);
	જો (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	જો (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_શોધ_patterns();
    }

    પાછા error;
}

સ્થિર funccal_entry_T *funccal_ઢગલો = ખાલી;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    શૂન્ય
save_funccal(funccal_entry_T *entry)
{
    entry->ટોચ_funccal = current_funccal;
    entry->next = funccal_ઢગલો;
    funccal_ઢગલો = entry;
    current_funccal = ખાલી;
}

    શૂન્ય
restore_funccal(શૂન્ય)
{
    જો (funccal_ઢગલો == ખાલી)
	internal_error("restore_funccal()");
    નહીંતર
    {
	current_funccal = funccal_ઢગલો->ટોચ_funccal;
	funccal_ઢગલો = funccal_ઢગલો->next;
    }
}

    funccall_T *
get_current_funccal(શૂન્ય)
{
    પાછા current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    પૂર્ણાંક
at_script_level(શૂન્ય)
{
    પાછા current_funccal == ખાલી && autocmd_match == ખાલી;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    શૂન્ય
delete_script_functions(પૂર્ણાંક sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (પૂર્ણાંક)KE_SNR;
    len = 3 + vim_snprintf((અક્ષર *)buf + 3, કદ(buf) - 3, "%d_", sid);

    જ્યારે (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(&func_hashtab, hi, todo)
	    જો (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		જો (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    પૂર્ણાંક changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    જો (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			જો (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    નહીંતર
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			જો (changed != func_hashtab.ht_changed)
			    તોડો;
		    }
		}
		--todo;
	    }
    }
}

#જો defined(EXITFREE)
    શૂન્ય
free_all_functions(શૂન્ય)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    પૂર્ણાંક		changed;

    // Clean up the current_funccal chain and the funccal stack.
    જ્યારે (current_funccal != ખાલી)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	જો (current_funccal == ખાલી && funccal_ઢગલો != ખાલી)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    જ્યારે (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(&func_hashtab, hi, todo)
	    જો (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		જો (func_name_refcount(fp->uf_name))
		    ++skipped;
		નહીંતર
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    જો (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			તોડો;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    જ્યારે (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_વસ્તુઓ(&func_hashtab, hi, todo)
	    જો (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		જો (func_name_refcount(fp->uf_name))
		    ++skipped;
		નહીંતર
		{
		    જો (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			તોડો;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    જો (skipped == 0)
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
    પૂર્ણાંક
builtin_function(char_u *name, પૂર્ણાંક len)
{
    પૂર્ણાંક i;

    જો (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	પાછા FALSE;
    માટે (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	જો (name[i] == AUTOLOAD_CHAR)
	    પાછા FALSE;
	જો (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    જો (name[i] == '.')
		પાછા FALSE;
	    તોડો;
	}
    }
    પાછા TRUE;
}

    પૂર્ણાંક
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
    પૂર્ણાંક		argc = 0;
    પૂર્ણાંક		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_વસ્તુઓ(l, item)
    {
	જો (argc == MAX_FUNC_ARGS - (partial == ખાલી ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    તોડો;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    જો (item == ખાલી)
    {
	funcexe_T	funcexe;
	પૂર્ણાંક		namelen = -1;

	જો (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    જો (p != ખાલી)
	    {
		જો (skip_generic_func_type_args(&p) == FAIL)
		    જાઓ done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	જો (partial != ખાલી)
	{
	    funcexe.fe_object = partial->pt_obj;
	    જો (funcexe.fe_object != ખાલી)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    જ્યારે (argc > 0)
	clear_tv(&argv[--argc]);

done:
    પાછા r;
}

સ્થિર પૂર્ણાંક callback_depth = 0;

    પૂર્ણાંક
get_callback_depth(શૂન્ય)
{
    પાછા callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    પૂર્ણાંક
call_callback(
    callback_T	*callback,
    પૂર્ણાંક		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    પૂર્ણાંક		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    પૂર્ણાંક		ret;

    જો (callback->cb_name == ખાલી || *callback->cb_name == NUL)
	પાછા FAIL;

    જો (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	પાછા FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    જો (callback->cb_partial != ખાલી)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	જો (funcexe.fe_object != ખાલી)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    જો (need_rethrow && current_exception != ખાલી && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    પાછા ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    પૂર્ણાંક		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    જો (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	પાછા -2;

    retval = tv_get_સંખ્યા_chk(&rettv, ખાલી);
    clear_tv(&rettv);
    પાછા retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    શૂન્ય
user_func_error(funcerror_T error, char_u *name, પૂર્ણાંક found_var)
{
    પસંદગી (error)
    {
	સ્થિતિ FCERR_UNKNOWN:
		જો (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		નહીંતર
		    emsg_funcname(e_unknown_function_str, name);
		તોડો;
	સ્થિતિ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		તોડો;
	સ્થિતિ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		તોડો;
	સ્થિતિ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		તોડો;
	સ્થિતિ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		તોડો;
	સ્થિતિ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		તોડો;
	સ્થિતિ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		તોડો;
	સ્થિતિ FCERR_OTHER:
	સ્થિતિ FCERR_FAILED:
		// assume the error message was already given
		તોડો;
	સ્થિતિ FCERR_NONE:
		તોડો;
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
    સ્થિર funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	પૂર્ણાંક	    argcount,
	પૂર્ણાંક	    base_included,
	char_u	    *name)
{
    જો (funcexe->fe_check_type != ખાલી && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	જો (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? ખાલી : funcexe->fe_basetv,
			  name) == FAIL)
	    પાછા FCERR_OTHER;
    }
    પાછા FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    પૂર્ણાંક
call_func(
    char_u	*funcname,	// name of the function
    પૂર્ણાંક		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    પૂર્ણાંક		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    પૂર્ણાંક		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    પૂર્ણાંક		i;
    ufunc_T	*fp = ખાલી;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ખાલી;
    char_u	*fname = ખાલી;
    char_u	*name = ખાલી;
    પૂર્ણાંક		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    પૂર્ણાંક		argv_clear = 0;
    પૂર્ણાંક		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    જો (partial != ખાલી)
	fp = partial->pt_func;
    જો (fp == ખાલી)
	fp = funcexe->fe_ufunc;

    જો (fp == ખાલી)
    {
	જો (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    જો (p != ખાલી)
	    {
		len = p - funcname;
		જો (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ખાલી)
		    જાઓ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	જો (name == ખાલી)
	    જાઓ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    જો (funcexe->fe_doesrange != ખાલી)
	*funcexe->fe_doesrange = FALSE;

    જો (partial != ખાલી)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	જો (partial->pt_dict != ખાલી && (selfdict == ખાલી || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	જો (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    માટે (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		જો (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    જાઓ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    માટે (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    જો (funcexe->fe_check_type != ખાલી
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		માટે (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    જો (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != ખાલી) ? name : funcname);

    જો (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	પૂર્ણાંક	is_global = FALSE;

	// Skip "g:" before a function name.
	જો (fp == ખાલી && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_સંખ્યા;	// default rettv is number zero
	rettv->vval.v_સંખ્યા = 0;
	error = FCERR_UNKNOWN;

	જો (fp != ખાલી || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    જો (fp == ખાલી)
	    {
		fp = find_func(rfname, is_global);
		જો (fp != ખાલી && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ખાલી;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    જો (fp == ખાલી
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ખાલી)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    જો (fp == ખાલી && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    જો (fp == ખાલી)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		જો (p != ખાલી && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    જો (partial == ખાલી && fp != ખાલી && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		જો (fp == ખાલી)
		{
		    error = FCERR_FAILED;
		    જાઓ theend;
		}
	    }
	    નહીંતર જો (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != ખાલી ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		જાઓ theend;
	    }

	    જો (fp != ખાલી && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    નહીંતર જો (fp != ખાલી)
	    {
		પૂર્ણાંક need_arg_check = FALSE;
		જો (funcexe->fe_check_type == ખાલી)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		જો (funcexe->fe_argv_func != ખાલી)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		જો (funcexe->fe_basetv != ખાલી)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, કદ(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		જો (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != ખાલી) ? name : funcname);

		જો (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	નહીંતર જો (funcexe->fe_basetv != ખાલી)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	નહીંતર
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
    જો (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    જો (!aborting())
	user_func_error(error, (name != ખાલી) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    જ્યારે (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    પાછા ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    પૂર્ણાંક
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    પૂર્ણાંક		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ખાલી;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    પૂર્ણાંક		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_સંખ્યા;	// default rettv is number zero
    rettv->vval.v_સંખ્યા = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    જો (name == ખાલી)
	પાછા ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    જો (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    નહીંતર
	rfname = fname;
    fp = find_func(rfname, is_global);
    જો (fp != ખાલી && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ખાલી;
    જો (fp == ખાલી)
	ret = NOTDONE;
    નહીંતર જો (fp != ખાલી && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    નહીંતર જો (fp != ખાલી)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ખાલી);
	જો (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    પાછા ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    પાછા fp->uf_name_exp != ખાલી ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    સ્થિર પૂર્ણાંક
function_list_modified(પૂર્ણાંક prev_ht_changed)
{
    જો (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    સ્થિર પૂર્ણાંક
list_func_head(ufunc_T *fp, પૂર્ણાંક indent)
{
    પૂર્ણાંક		prev_ht_changed = func_hashtab.ht_changed;
    પૂર્ણાંક		j;

    સંદેશ_start();

    // a timer at the more prompt may have deleted the function
    જો (function_list_modified(prev_ht_changed))
	પાછા FAIL;

    જો (indent)
	સંદેશ_puts("   ");
    જો (fp->uf_def_status != UF_NOT_COMPILED)
	સંદેશ_puts("def ");
    નહીંતર
	સંદેશ_puts("function ");
    સંદેશ_puts((અક્ષર *)printable_func_name(fp));
    સંદેશ_putchar('(');
    માટે (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	જો (j)
	    સંદેશ_puts(", ");
	સંદેશ_puts((અક્ષર *)FUNCARG(fp, j));
	જો (fp->uf_arg_types != ખાલી)
	{
	    અક્ષર *tofree;

	    સંદેશ_puts(": ");
	    સંદેશ_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	જો (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    સંદેશ_puts(" = ");
	    સંદેશ_puts(((અક્ષર **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    જો (fp->uf_varargs)
    {
	જો (j)
	    સંદેશ_puts(", ");
	સંદેશ_puts("...");
    }
    જો (fp->uf_va_name != ખાલી)
    {
	જો (!fp->uf_varargs)
	{
	    જો (j)
		સંદેશ_puts(", ");
	    સંદેશ_puts("...");
	}
	સંદેશ_puts((અક્ષર *)fp->uf_va_name);
	જો (fp->uf_va_type != ખાલી)
	{
	    અક્ષર *tofree;

	    સંદેશ_puts(": ");
	    સંદેશ_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    સંદેશ_putchar(')');

    જો (fp->uf_def_status != UF_NOT_COMPILED)
    {
	જો (fp->uf_ret_type != &t_void)
	{
	    અક્ષર *tofree;

	    સંદેશ_puts(": ");
	    સંદેશ_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    નહીંતર જો (fp->uf_flags & FC_ABORT)
	સંદેશ_puts(" abort");
    જો (fp->uf_flags & FC_RANGE)
	સંદેશ_puts(" range");
    જો (fp->uf_flags & FC_DICT)
	સંદેશ_puts(" dict");
    જો (fp->uf_flags & FC_CLOSURE)
	સંદેશ_puts(" closure");
    સંદેશ_clr_eos();
    જો (p_verbose > 0)
	last_set_સંદેશ(fp->uf_script_ctx);

    પાછા OK;
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
    પૂર્ણાંક		*is_global,
    પૂર્ણાંક		skip,		// only find the end, don't evaluate
    પૂર્ણાંક		flags)
{
    પાછા trans_function_name_ext(pp, is_global, skip, flags,
	    ખાલી, ખાલી, ખાલી, ખાલી);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    સ્થિર char_u *
trans_function_name_ext(
    char_u	**pp,
    પૂર્ણાંક		*is_global,
    પૂર્ણાંક		skip,		// only find the end, don't evaluate
    પૂર્ણાંક		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ખાલી;
    char_u	*start;
    char_u	*end;
    પૂર્ણાંક		lead;
    char_u	sid_buf[20];
    પૂર્ણાંક		len;
    પૂર્ણાંક		extra = 0;
    પૂર્ણાંક		prefix_g = FALSE;
    lval_T	lv;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		vim9_local;

    જો (fdp != ખાલી)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    જો ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (પૂર્ણાંક)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	પાછા vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    જો (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, ખાલી, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    જો (end == start || (vim9script && end != ખાલી
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	જો (!skip)
	    emsg(_(e_function_name_required));
	જાઓ theend;
    }
    જો (end == ખાલી || (lv.ll_tv != ખાલી && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	જો (!aborting())
	{
	    જો (end != ખાલી)
		semsg(_(e_invalid_argument_str), start);
	}
	નહીંતર
	    *pp = find_name_end(start, ખાલી, ખાલી, FNE_INCL_BR);
	જાઓ theend;
    }

    જો (lv.ll_ufunc != ખાલી)
    {
	જો (ufunc != ખાલી)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	જાઓ theend;
    }

    જો (lv.ll_tv != ખાલી)
    {
	જો (fdp != ખાલી)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ખાલી;
	    fdp->fd_di = lv.ll_di;
	}
	જો (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ખાલી)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	નહીંતર જો (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ખાલી)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	નહીંતર જો (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ખાલી)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    જો (partial != ખાલી)
		*partial = lv.ll_tv->vval.v_partial;
	}
	નહીંતર
	{
	    જો (!skip && !(flags & TFN_QUIET) && (fdp == ખાલી
			     || lv.ll_dict == ખાલી || fdp->fd_newkey == ખાલી))
		emsg(_(e_funcref_required));
	    નહીંતર
		*pp = end;
	    name = ખાલી;
	}
	જાઓ theend;
    }

    જો (lv.ll_name == ખાલી)
    {
	// Error found, but continue after the function name.
	*pp = end;
	જાઓ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    જો (lv.ll_exp_name != ખાલી)
    {
	len = (પૂર્ણાંક)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ખાલી);
	જો (name == lv.ll_exp_name)
	    name = ખાલી;
    }
    નહીંતર જો (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	પૂર્ણાંક		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	જો (si->sn_autoload_prefix != ખાલી)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	નહીંતર
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (પૂર્ણાંક)KE_SNR;
	    vim_snprintf((અક્ષર *)sid_buf + 3, કદ(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	જાઓ theend;
    }
    નહીંતર જો (!(flags & TFN_NO_DEREF))
    {
	len = (પૂર્ણાંક)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ખાલી);
	જો (name == *pp)
	    name = ખાલી;
    }
    જો (name != ખાલી)
    {
	name = vim_strsave(name);
	*pp = end;
	જો (name != ખાલી && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (પૂર્ણાંક)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	જાઓ theend;
    }

    જો (lv.ll_exp_name != ખાલી)
    {
	len = (પૂર્ણાંક)STRLEN(lv.ll_exp_name);
	જો (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    નહીંતર
    {
	// skip over "s:" and "g:"
	જો (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    જો (lv.ll_name[0] == 'g')
	    {
		જો (is_global != ખાલી)
		{
		    *is_global = TRUE;
		}
		નહીંતર
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (પૂર્ણાંક)(end - lv.ll_name);
    }
    જો (len <= 0)
    {
	જો (!skip)
	    emsg(_(e_function_name_required));
	જાઓ theend;
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
    જો (skip)
	lead = 0;	// do nothing
    નહીંતર જો (lead > 0 || vim9_local)
    {
	જો (!vim9_local)
	{
	    જો (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		જાઓ theend;
	    }
	    lead = 3;
	}
	જો (vim9_local || (lv.ll_exp_name != ખાલી
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    જો (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		જાઓ theend;
	    }
	    sid_buflen = vim_snprintf((અક્ષર *)sid_buf, કદ(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    જો (vim9_local)
		extra = 3 + (પૂર્ણાંક)sid_buflen;
	    નહીંતર
		lead += (પૂર્ણાંક)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    નહીંતર જો (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	જાઓ theend;
    }
    જો (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	જો (cp != ખાલી && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    જાઓ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    જો (name != ખાલી)
    {
	જો (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (પૂર્ણાંક)KE_SNR;
	    જો (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	નહીંતર જો (prefix_g)
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
    પાછા name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    સ્થિર char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    જો (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	જો (p != ખાલી)
	    પાછા p + 1;
    }
    પાછા ખાલી;
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
    અક્ષર	sid_buf[25];
    size_t	sid_buflen;
    પૂર્ણાંક		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    જો (funcname == ખાલી)
	પાછા ખાલી;

    જો (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	જો (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    પાછા ખાલી;
	ufunc = find_func(funcname, FALSE);
	જો (ufunc == ખાલી || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ખાલી)
	    પાછા ખાલી;
	++p;
	off = 0;
    }
    નહીંતર
	off = *funcname == 's' ? 2 : 5;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	પાછા ખાલી;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, કદ(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    જો (newname == ખાલી)
	પાછા ખાલી;
    vim_snprintf((અક્ષર *)newname, newnamesize, "%s%s", sid_buf, p + off);

    પાછા newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    જો (n != ખાલી)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    પાછા n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	પૂર્ણાંક	    *is_global,
	પૂર્ણાંક	    skip,
	પૂર્ણાંક	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    જો (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(શૂન્ય)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	જો (fudi != ખાલી)
	    CLEAR_POINTER(fudi);
    }
    નહીંતર
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ખાલી, ખાલી, ખાલી);
    *name = p;
    પાછા saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    શૂન્ય
list_functions(regmatch_T *regmatch)
{
    પૂર્ણાંક		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    માટે (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    જો ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ખાલી
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		જો (list_func_head(fp, FALSE) == FAIL)
		    પાછા;
		જો (function_list_modified(prev_ht_changed))
		    પાછા;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    સ્થિર char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    જો (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	જો (regmatch.regprog != ખાલી)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    જો (*p == '/')
	++p;

    પાછા p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    સ્થિર ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, પૂર્ણાંક is_global)
{
    ufunc_T	*fp = ખાલી;
    પૂર્ણાંક		j;

    જો (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	પાછા ખાલી;
    }

    set_nextcmd(eap, p);

    જો (eap->nextcmd != ખાલી)
	*p = NUL;

    જો (eap->skip || got_int)
	પાછા ખાલી;

    fp = find_func(name, is_global);
    જો (fp == ખાલી && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	જો (up != ખાલી)
	    fp = find_func(up, FALSE);
    }

    જો (fp == ખાલી)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	પાછા ખાલી;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    પૂર્ણાંક prev_ht_changed = func_hashtab.ht_changed;

    જો (list_func_head(fp, TRUE) != OK)
	પાછા fp;

    માટે (j = 0; j < fp->uf_લીટીઓ.ga_len && !got_int; ++j)
    {
	જો (FUNCLINE(fp, j) == ખાલી)
	    ચાલુ;
	સંદેશ_putchar('\n');
	સંદેશ_outnum((long)(j + 1));
	જો (j < 9)
	    સંદેશ_putchar(' ');
	જો (j < 99)
	    સંદેશ_putchar(' ');
	જો (function_list_modified(prev_ht_changed))
	    તોડો;
	સંદેશ_prt_લીટી(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    જો (!got_int)
    {
	સંદેશ_putchar('\n');
	જો (!function_list_modified(prev_ht_changed))
	{
	    જો (fp->uf_def_status != UF_NOT_COMPILED)
		સંદેશ_puts("   enddef");
	    નહીંતર
		સંદેશ_puts("   endfunction");
	}
    }

    પાછા fp;
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
	garray_T    *લીટીઓ_to_free,
	પૂર્ણાંક	    class_flags,
	ocmember_T  *obj_members,
	પૂર્ણાંક         obj_member_ગણતરી,
	cctx_T	    *cctx)
{
    પૂર્ણાંક		j;
    પૂર્ણાંક		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    પૂર્ણાંક		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*લીટી_arg = ખાલી;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    પૂર્ણાંક		varargs = FALSE;
    પૂર્ણાંક		flags = 0;
    char_u	*ret_type = ખાલી;
    ufunc_T	*fp = ખાલી;
    પૂર્ણાંક		fp_allocated = FALSE;
    પૂર્ણાંક		free_fp = FALSE;
    પૂર્ણાંક		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    સ્થિર પૂર્ણાંક	func_nr = 0;	    // number for nameless function
    પૂર્ણાંક		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_ટોચ;
    પૂર્ણાંક		vim9script = in_vim9script();
    imported_T	*import = ખાલી;

    // ":function" without argument: list functions.
    જો (ends_excmd2(eap->cmd, eap->arg))
    {
	જો (!eap->skip)
	    list_functions(ખાલી);
	set_nextcmd(eap, eap->arg);
	પાછા ખાલી;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    જો (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	પાછા ખાલી;
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
    જો (name_arg != ખાલી)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    નહીંતર
    {
	જો (vim9script)
	{
	    જો (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		પાછા ખાલી;
	    }
	    p = to_name_end(p, TRUE);
	    જો (*skipwhite(p) == '.' && vim_strchr(p, '(') != ખાલી)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		પાછા ખાલી;
	    }

	    p = eap->arg;
	}

	પૂર્ણાંક tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ખાલી);
	જો (name == ખાલી && (fudi.fd_dict == ખાલી || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    જો (!aborting())
	    {
		જો (!eap->skip && fudi.fd_newkey != ખાલી)
		    semsg(_(e_ચાવી_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		પાછા ખાલી;
	    }
	    નહીંતર
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	જો (is_export && name != ખાલી)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    જો (prefixed != ખાલી && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	નહીંતર જો (paren && vim9script && name != ખાલી
				    && vim_strchr(name, AUTOLOAD_CHAR) != ખાલી)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    જાઓ ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    જો (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	જાઓ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    જો (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	જો (p == ખાલી)
	    જાઓ ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    જો (*p != '(')
    {
	જો (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    જાઓ ret_free;
	}
	// attempt to continue by skipping some text
	જો (vim_strchr(p, '(') != ખાલી)
	    p = vim_strchr(p, '(');
    }

    જો ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	જાઓ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    જો (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	જાઓ ret_free;
    }

    ga_init2(&newlines, કદ(char_u *), 10);

    જો (!eap->skip && name_arg == ખાલી)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	જો (name != ખાલી)
	    arg = name;
	નહીંતર
	    arg = fudi.fd_newkey;
	જો (arg != ખાલી && (fudi.fd_di == ખાલી
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    પૂર્ણાંક	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    જો (arg != fudi.fd_newkey)
	    {
		જો (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    જો (name_base == ખાલી)
			name_base = arg + 3;
		    નહીંતર
			++name_base;
		}
		માટે (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		જો (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    જાઓ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		જો (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, ખાલી,
			ખાલી, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    જાઓ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	જો (fudi.fd_dict != ખાલી && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    જાઓ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    જો (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ખાલી, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ખાલી,
			 ખાલી, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, લીટીઓ_to_free) == FAIL)
	જાઓ errret_2;
    whitep = p;

    જો (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	જો (*skipwhite(p) == ':')
	{
	    જો (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    નહીંતર જો (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    જો (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    નહીંતર
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ખાલી;
	    }
	}
	p = skipwhite(p);
    }
    નહીંતર
	// find extra arguments "range", "dict", "abort" and "closure"
	માટે (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    જો (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    નહીંતર જો (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    નહીંતર જો (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    નહીંતર જો (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		જો (current_funccal == ખાલી)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_ટોચ_level_str,
			    name == ખાલી ? (char_u *)"" : name);
		    જાઓ erret;
		}
	    }
	    નહીંતર
		તોડો;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    જો (*p == '\n')
	લીટી_arg = p + 1;
    નહીંતર જો (*p != NUL
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
    જો (ચાવી_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	જો (!eap->skip && !eap->forceit)
	{
	    જો (fudi.fd_dict != ખાલી && fudi.fd_newkey == ખાલી)
		emsg(_(e_dictionary_entry_already_exists));
	    નહીંતર જો (name != ખાલી && find_func(name, is_global) != ખાલી)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	જો (!eap->skip && did_emsg)
	    જાઓ erret;

	સંદેશ_putchar('\n');	    // don't overwrite the function name
	cmdline_row = સંદેશ_row;
    }

    // Save the starting line number.
    sourcing_lnum_ટોચ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    જો (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, લીટી_arg, લીટીઓ_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	જાઓ erret;

    /*
     * If there are no errors, add the function
     */
    જો (fudi.fd_dict != ખાલી)
    {
	અક્ષર	numbuf[NUMBUFLEN];

	fp = ખાલી;
	જો (fudi.fd_newkey == ખાલી && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    જાઓ erret;
	}
	જો (fudi.fd_di == ખાલી)
	{
	    // Can't add a function to a locked dictionary
	    જો (કિંમત_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		જાઓ erret;
	}
	    // Can't change an existing function if it is locked
	નહીંતર જો (કિંમત_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    જાઓ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, કદ(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	જો (name == ખાલી)
	    જાઓ erret;
    }
    નહીંતર જો (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	પૂર્ણાંક		var_conflict = FALSE;
	પૂર્ણાંક		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	જો (v != ખાલી && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	જો (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    જો (si->sn_autoload_prefix != ખાલી)
	    {
		જો (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    જો (v != ખાલી)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		નહીંતર
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    જો (prefixed != ખાલી && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			જો (v != ખાલી)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	જો (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    જાઓ erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	જો (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == ખાલી ? name : uname, 0, FALSE);
	}

	જો (fp != ખાલી || import != ખાલી)
	{
	    પૂર્ણાંક dead = fp != ખાલી && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    જો (import != ખાલી
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_ટોચ;
		જો (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		નહીંતર
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		જાઓ errret_keep;
	    }
	    જો (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		જાઓ errret_keep;
	    }
	    જો (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ખાલી;
		overwrite = TRUE;
	    }
	    નહીંતર
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ખાલી;
		func_clear_વસ્તુઓ(fp);
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

    જો (fp == ખાલી)
    {
	જો (fudi.fd_dict == ખાલી && vim_strchr(name, AUTOLOAD_CHAR) != ખાલી)
	{
	    પૂર્ણાંક	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    જો (SOURCING_NAME != ખાલી)
	    {
		scriptname = autoload_name(name);
		જો (scriptname != ખાલી)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (પૂર્ણાંક)STRLEN(p);
		    slen = (પૂર્ણાંક)STRLEN(SOURCING_NAME);
		    જો (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    જો (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_ટોચ;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		જાઓ erret;
	    }
	}

	જો (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	જો (fp == ખાલી)
	    જાઓ erret;
	fp_allocated = TRUE;

	જો (fudi.fd_dict != ખાલી)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    જો (func_name == ખાલી)
	    {
		VIM_CLEAR(fp);
		જાઓ erret;
	    }
	    જો (fudi.fd_di == ખાલી)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		જો (fudi.fd_di == ખાલી)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    જાઓ erret;
		}
		જો (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    જાઓ erret;
		}
	    }
	    નહીંતર
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

    જો (eap->cmdidx == CMD_def)
    {
	પૂર્ણાંક	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	જો (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_ટોચ;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	પૂર્ણાંક save_is_export = is_export;
	is_export = FALSE;

	જો (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_ગણતરી, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    જાઓ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	જો (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    જાઓ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    નહીંતર
	fp->uf_def_status = UF_NOT_COMPILED;

    જો (fp_allocated)
    {
	// insert the new function in the function list
	જો (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_ચાવી = UF2HIKEY(fp);
	}
	નહીંતર જો (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    જાઓ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_લીટીઓ = newlines;
    newlines.ga_data = ખાલી;
    જો ((flags & FC_CLOSURE) != 0)
    {
	જો (register_closure(fp) == FAIL)
	    જાઓ erret;
    }
    નહીંતર
	fp->uf_scoped = ખાલી;

#ifdef FEAT_PROFILE
    જો (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    જો (sandbox)
	flags |= FC_SANDBOX;
    જો (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_ટોચ;
    જો (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    જો (eap->cmdidx == CMD_def)
	set_function_type(fp);
    નહીંતર જો (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    જો (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ખાલી);

    જાઓ ret_free;

erret:
    જો (fp != ખાલી)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    જો (fp != ખાલી)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	જો (IS_GENERIC_FUNC(fp))
	    generic_func_clear_વસ્તુઓ(fp);
    }
    જો (free_fp)
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
    જો (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    પાછા fp;
}

/*
 * ":function"
 */
    શૂન્ય
ex_function(exarg_T *eap)
{
    garray_T લીટીઓ_to_free;

    ga_init2(&લીટીઓ_to_free, કદ(char_u *), 50);
    (શૂન્ય)define_function(eap, ખાલી, &લીટીઓ_to_free, 0, ખાલી, 0, ખાલી);
    ga_clear_strings(&લીટીઓ_to_free);
}

    પૂર્ણાંક
get_func_arity(char_u *name, પૂર્ણાંક *required, પૂર્ણાંક *optional, પૂર્ણાંક *varargs)
{
    ufunc_T	*ufunc = ખાલી;
    પૂર્ણાંક		argcount = 0;
    પૂર્ણાંક		min_argcount = 0;
    પૂર્ણાંક		idx;

    idx = find_internal_func(name);
    જો (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    નહીંતર
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ખાલી;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	જો (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	જો (ufunc == ખાલી)
	    પાછા FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    પાછા OK;
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
    પૂર્ણાંક		is_global = FALSE;
    char_u	*bracket_start = ખાલી;
    char_u	*generic_func_name = name;

    જો (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    નહીંતર જો (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    જો (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(શૂન્ય)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    નહીંતર
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	જો (ufunc != ખાલી)
	    પાછા ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ખાલી, ખાલી, ખાલી, &ufunc);
	જો (ufunc != ખાલી)
	{
	    vim_free(fname);
	    પાછા ufunc;
	}
	જો (fname != ખાલી && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    જો (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		પાછા ખાલી;
	    }
	}
    }
    જો (fname == ખાલી)
    {
	semsg(_(e_invalid_argument_str), name);
	પાછા ખાલી;
    }
    જો (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	પાછા ખાલી;
    }

    ufunc = find_func(fname, is_global);
    જો (ufunc == ખાલી)
    {
	char_u *p = untrans_function_name(fname);

	જો (p != ખાલી)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	જો (ufunc == ખાલી)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    જો (bracket_start != ખાલી)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    જો (ufunc != ખાલી)
    {
	// handle generic functions
	જો (bracket_start == ખાલી)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    જો (ufunc == ખાલી)
	semsg(_(e_cannot_find_function_str), name);
    પાછા ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    શૂન્ય
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    જો (func_needs_compiling(ufunc, compile_type))
	(શૂન્ય)compile_def_function(ufunc, FALSE, compile_type, ખાલી);
    નહીંતર
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ખાલી ? cl->class_name.string : (char_u *)"",
				cl != ખાલી ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    સ્થિર શૂન્ય
defcompile_funcs_in_script(શૂન્ય)
{
    long	todo = (long)func_hashtab.ht_used;
    પૂર્ણાંક		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    માટે (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    જો (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(શૂન્ય)compile_def_function(ufunc, FALSE, CT_NONE, ખાલી);

		જો (func_hashtab.ht_changed != changed)
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
    શૂન્ય
ex_defcompile(exarg_T *eap)
{
    જો (*eap->arg != NUL)
    {
	typval_T tv;

	જો (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    જો (cl != ખાલી)
		defcompile_class(cl);
	}
	નહીંતર
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    જો (ufunc != ખાલી)
		defcompile_function(ufunc, ખાલી);
	}
    }
    નહીંતર
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
    પૂર્ણાંક
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    જો (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	પાછા 5;
    જો (p[0] == 's' && p[1] == ':')
	પાછા 2;
    પાછા 0;
}

    પૂર્ણાંક
translated_function_exists(char_u *name, પૂર્ણાંક is_global)
{
    જો (builtin_function(name, -1))
	પાછા has_internal_func(name);
    પાછા find_func(name, is_global) != ખાલી;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    પૂર્ણાંક
has_varargs(ufunc_T *ufunc)
{
    પાછા ufunc->uf_varargs || ufunc->uf_va_name != ખાલી;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    પૂર્ણાંક
function_exists(char_u *name, પૂર્ણાંક no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    પૂર્ણાંક	    n = FALSE;
    પૂર્ણાંક	    flag;
    પૂર્ણાંક	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    જો (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    જો (p != ખાલી && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    પાછા n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    પૂર્ણાંક
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    પૂર્ણાંક		is_global = FALSE;
    ufunc_T	*fp = ખાલી;
    list_T	*args;
    પૂર્ણાંક		is_def;
    અક્ષર	*tofree;

    જો (*name == NUL)
	પાછા FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    જો (p != ખાલી && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    જો (fp == ખાલી)
	પાછા FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    જો (args == ખાલી || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	પાછા FAIL;
    }
    માટે (પૂર્ણાંક j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	જો (arg == ખાલી || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    પાછા FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	જો (fp->uf_arg_types == ખાલી)
	    dict_add_string(arg, "type", (char_u *)"any");
	નહીંતર
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	જો (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    જો (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	જો (va == ખાલી || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    પાછા FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == ખાલી ? (char_u *)"" : fp->uf_va_name);
	જો (fp->uf_va_type == ખાલી)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	નહીંતર
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    જો (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    નહીંતર
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_સંખ્યા(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_સંખ્યા(d, "lnum", fp->uf_script_ctx.sc_lnum);
    પાછા OK;
}

#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, પૂર્ણાંક check)
{
    char_u	*nm = name;
    char_u	*p;
    પૂર્ણાંક		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    જો (p != ખાલી && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	પાછા p;

    vim_free(p);
    પાછા ખાલી;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, પૂર્ણાંક idx)
{
    સ્થિર long_u	done;
    સ્થિર પૂર્ણાંક		changed;
    સ્થિર hashitem_T	*hi;
    ufunc_T		*fp;

    જો (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    જો (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	પૂર્ણાંક len;

	જો (done++ > 0)
	    ++hi;
	જ્યારે (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	જો ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    પાછા (char_u *)"";

	જો (fp->uf_namelen + 4 >= IOSIZE)
	    પાછા fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	જો (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    જો (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	પાછા IObuff;
    }
    પાછા ખાલી;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, પૂર્ણાંક extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    જો (ufunc == ખાલી)
	પાછા ખાલી;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ખાલી;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_ગણતરી = ખાલી;
    ufunc->uf_tml_total = ખાલી;
    ufunc->uf_tml_self = ખાલી;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    જો (ufunc->uf_arg_types != ખાલી)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	જો (at != ખાલી)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     કદ(type_T *) * ufunc->uf_args.ga_len);
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

    જો (ufunc->uf_va_name != ખાલી)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = ખાલી;

    ga_copy_strings(&fp->uf_લીટીઓ, &ufunc->uf_લીટીઓ);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    જો (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    પાછા ufunc;
}

/*
 * ":delfunction {name}"
 */
    શૂન્ય
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ખાલી;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    પૂર્ણાંક		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ખાલી, ખાલી, ખાલી);
    vim_free(fudi.fd_newkey);
    જો (name == ખાલી)
    {
	જો (fudi.fd_dict != ખાલી && !eap->skip)
	    emsg(_(e_funcref_required));
	પાછા;
    }
    જો (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	પાછા;
    }
    set_nextcmd(eap, p);
    જો (eap->nextcmd != ખાલી)
	*p = NUL;

    જો (numbered_function(name) && fudi.fd_dict == ખાલી)
    {
	જો (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	પાછા;
    }
    જો (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    જો (!eap->skip)
    {
	જો (fp == ખાલી)
	{
	    જો (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    પાછા;
	}
	જો (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    પાછા;
	}
	જો (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    પાછા;
	}

	જો (fudi.fd_dict != ખાલી)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	નહીંતર
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    જો (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		જો (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    નહીંતર
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    શૂન્ય
func_unref(char_u *name)
{
    ufunc_T *fp = ખાલી;

    જો (name == ખાલી || !func_name_refcount(name))
	પાછા;
    fp = find_func(name, FALSE);
    જો (fp == ખાલી && numbered_function(name))
    {
#ifdef EXITFREE
	જો (!entered_free_all_mem)
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
    શૂન્ય
func_ptr_unref(ufunc_T *fp)
{
    જો (fp != ખાલી && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ખાલી
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	જો (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    શૂન્ય
func_ref(char_u *name)
{
    ufunc_T *fp;

    જો (name == ખાલી || !func_name_refcount(name))
	પાછા;
    fp = find_func(name, FALSE);
    જો (fp != ખાલી)
	++fp->uf_refcount;
    નહીંતર જો (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    શૂન્ય
func_ptr_ref(ufunc_T *fp)
{
    જો (fp != ખાલી)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    સ્થિર પૂર્ણાંક
can_free_funccal(funccall_T *fc, પૂર્ણાંક copyID)
{
    પાછા (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    શૂન્ય
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    પૂર્ણાંક		returning = FALSE;
    evalarg_T	evalarg;

    જો (current_funccal == ખાલી)
    {
	emsg(_(e_return_not_inside_function));
	પાછા;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    જો (eap->skip)
	++emsg_skip;

    eap->nextcmd = ખાલી;
    જો ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	જો (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	નહીંતર
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    નહીંતર જો (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	જો (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ખાલી);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    જો (returning)
	eap->nextcmd = ખાલી;
    નહીંતર જો (eap->nextcmd == ખાલી)	    // no argument
	set_nextcmd(eap, arg);

    જો (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    સ્થિર પૂર્ણાંક
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    પૂર્ણાંક		doesrange;
    typval_T	rettv;
    પૂર્ણાંક		failed = FALSE;

    lnum = eap->line1;
    માટે ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	જો (eap->addr_ગણતરી > 0)
	{
	    જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		તોડો;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	જો (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    તોડો;
	}
	જો (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	જો (handle_subscript(arg, ખાલી, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    તોડો;
	}

	clear_tv(&rettv);
	જો (doesrange)
	    તોડો;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	જો (aborting())
	    તોડો;
    }
    પાછા failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    સ્થિર પૂર્ણાંક
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    પૂર્ણાંક		partial_argc = 0;		// number of partial arguments
    પૂર્ણાંક		argcount = 0;			// number of arguments found
    પૂર્ણાંક		r;

    જો (current_funccal == ખાલી)
    {
	semsg(_(e_str_not_inside_function), "defer");
	પાછા FAIL;
    }
    જો (partial != ખાલી)
    {
	જો (partial->pt_dict != ખાલી)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    પાછા FAIL;
	}
	જો (partial->pt_argc > 0)
	{
	    પૂર્ણાંક i;

	    partial_argc = partial->pt_argc;
	    માટે (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    પૂર્ણાંક is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    જો (r == OK)
    {
	જો (type != ખાલી)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, ખાલી, name);
	}
	નહીંતર જો (is_builtin)
	{
	    પૂર્ણાંક idx = find_internal_func(name);

	    જો (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    નહીંતર જો (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	નહીંતર
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    જો (ufunc != ખાલી)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		જો (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    જો (r == FAIL)
    {
	જ્યારે (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	પાછા FAIL;
    }
    પાછા add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    પૂર્ણાંક
can_add_defer(શૂન્ય)
{
    જો (!in_def_function() && get_current_funccal() == ખાલી)
    {
	semsg(_(e_str_not_inside_function), "defer");
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
add_defer(char_u *name, પૂર્ણાંક argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    પૂર્ણાંક		argcount = argcount_arg;
    defer_T	*dr;
    પૂર્ણાંક		ret = FAIL;

    જો (saved_name == ખાલી)
	જાઓ theend;
    જો (in_def_function())
    {
	જો (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    નહીંતર
    {
	જો (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, કદ(defer_T), 10);
	જો (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    જાઓ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	જ્યારે (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    જ્યારે (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    પાછા ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    સ્થિર શૂન્ય
handle_defer_one(funccall_T *funccal)
{
    પૂર્ણાંક	    idx;

    માટે (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	જો (dr->dr_name == ખાલી)
	    // already being called, can happen if function does ":qa"
	    ચાલુ;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ખાલી;

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
	માટે (પૂર્ણાંક i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    સ્થિર શૂન્ય
invoke_funccall_defer(funccall_T *fc)
{
    જો (fc->fc_ectx != ખાલી)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    નહીંતર
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    શૂન્ય
invoke_all_defer(શૂન્ય)
{
    માટે (funccall_T *fc = current_funccal; fc != ખાલી; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    માટે (funccal_entry_T *fce = funccal_ઢગલો; fce != ખાલી; fce = fce->next)
	માટે (funccall_T *fc = fce->ટોચ_funccal; fc != ખાલી; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    શૂન્ય
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    પૂર્ણાંક		len;
    પૂર્ણાંક		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ખાલી;
    partial_T	*partial = ખાલી;
    evalarg_T	evalarg;
    type_T	*type = ખાલી;
    પૂર્ણાંક		found_var = FALSE;
    પૂર્ણાંક		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    જો (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	જો (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	પાછા;
    }

    tofree = trans_function_name_ext(&arg, ખાલી, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ખાલી, &ufunc);
    જો (fudi.fd_newkey != ખાલી)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_ચાવી_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    જો (tofree == ખાલી)
	પાછા;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    જો (fudi.fd_dict != ખાલી)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (પૂર્ણાંક)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ખાલી ? ખાલી : &partial,
				vim9script && type == ખાલી ? &type : ખાલી,
						     FALSE, FALSE, &found_var);

    જો (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	જો (name == ખાલી)
	    જાઓ end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    જો (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	જાઓ end;
    }
    જો (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	જાઓ end;
    }

    જો (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    નહીંતર
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
    જો ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	જો (!ends_excmd2(eap->arg, arg))
	{
	    જો (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	નહીંતર
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
    પૂર્ણાંક
do_return(
    exarg_T	*eap,
    પૂર્ણાંક		reanimate,
    પૂર્ણાંક		is_cmd,
    શૂન્ય	*rettv)
{
    પૂર્ણાંક		idx;
    cstack_T	*cstack = eap->cstack;

    જો (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    જો (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	જો (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	નહીંતર
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    જો (reanimate)
		rettv = current_funccal->fc_rettv;

	    જો (rettv != ખાલી)
	    {
		// Store the value of the pending return.
		જો ((cstack->cs_rettv[idx] = alloc_tv()) != ખાલી)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		નહીંતર
		    emsg(_(e_out_of_memory));
	    }
	    નહીંતર
		cstack->cs_rettv[idx] = ખાલી;

	    જો (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_સંખ્યા;
		current_funccal->fc_rettv->vval.v_સંખ્યા = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    નહીંતર
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	જો (!reanimate && rettv != ખાલી)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    જો (!is_cmd)
		vim_free(rettv);
	}
    }

    પાછા idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    શૂન્ય
discard_pending_return(શૂન્ય *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(શૂન્ય *rettv)
{
    char_u	*s = ખાલી;
    char_u	*tofree = ખાલી;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    જો (rettv != ખાલી)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    જો (s == ખાલી)
	s = (char_u *)"";
    નહીંતર
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    જો (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    પાછા vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_લીટી(
    પૂર્ણાંક	    c UNUSED,
    શૂન્ય    *cookie,
    પૂર્ણાંક	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    જો (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	func_લીટી_end(cookie);
#endif

    gap = &fp->uf_લીટીઓ;
    જો (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = ખાલી;
    નહીંતર
    {
	// Skip NULL lines (continuation lines).
	જ્યારે (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == ખાલી)
	    ++fcp->fc_linenr;
	જો (fcp->fc_linenr >= gap->ga_len)
	    retval = ખાલી;
	નહીંતર
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    જો (do_profiling == PROF_YES)
		func_લીટી_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    જો (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    પાછા retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    પૂર્ણાંક
func_has_ended(શૂન્ય *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    પાછા (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    પૂર્ણાંક
func_has_abort(
    શૂન્ય    *cookie)
{
    પાછા ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = ખાલી;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    જો (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ખાલી
				     && rettv->vval.v_partial->pt_func != ખાલી)
	fp = rettv->vval.v_partial->pt_func;
    નહીંતર
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ખાલી ? ખાલી
					      : rettv->vval.v_partial->pt_name;
	જો (fname == ખાલી)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ખાલી;
	}
	નહીંતર
	{
	    char_u	*tofree = ખાલી;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    જો (fp != ખાલી && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	જો (pt != ખાલી)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ખાલી;
	    જો (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    નહીંતર
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		પૂર્ણાંક		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		જો (ret_pt->pt_name != ખાલી)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		નહીંતર
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		જો (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    જો (pt->pt_argv == ખાલી)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    નહીંતર
		    {
			pt->pt_argc = ret_pt->pt_argc;
			માટે (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    પાછા selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(શૂન્ય *cookie)
{
    પાછા ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(શૂન્ય *cookie)
{
    પાછા &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    પૂર્ણાંક *
func_dbg_tick(શૂન્ય *cookie)
{
    પાછા &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    પૂર્ણાંક
func_level(શૂન્ય *cookie)
{
    પાછા ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    પૂર્ણાંક
current_func_returned(શૂન્ય)
{
    પાછા current_funccal->fc_returned;
}

    પૂર્ણાંક
free_unref_funccal(પૂર્ણાંક copyID, પૂર્ણાંક testing)
{
    પૂર્ણાંક		did_free = FALSE;
    પૂર્ણાંક		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    માટે (pfc = &previous_funccal; *pfc != ખાલી; )
    {
	જો (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	નહીંતર
	    pfc = &(*pfc)->fc_caller;
    }
    જો (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(શૂન્ય)garbage_collect(testing);

    પાછા did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    સ્થિર funccall_T *
get_funccal(શૂન્ય)
{
    પૂર્ણાંક		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    જો (debug_backtrace_level > 0)
    {
	માટે (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    જો (temp_funccal)
		funccal = temp_funccal;
	    નહીંતર
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    પાછા funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    સ્થિર funccall_T *
get_funccal_for_vars(શૂન્ય)
{
    funccall_T	*funccal = ખાલી;

    જો (current_funccal == ખાલી)
	પાછા ખાલી;
    funccal = get_funccal();
    જો (funccal == ખાલી || funccal->fc_l_vars.dv_refcount == 0)
	પાછા ખાલી;
    પાછા funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(શૂન્ય)
{
    funccall_T	*funccal = get_funccal_for_vars();

    પાછા funccal == ખાલી ? ખાલી : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(શૂન્ય)
{
    funccall_T	*funccal = get_funccal_for_vars();

    પાછા funccal == ખાલી ? ખાલી : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(શૂન્ય)
{
    funccall_T	*funccal = get_funccal_for_vars();

    પાછા funccal == ખાલી ? ખાલી : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(શૂન્ય)
{
    funccall_T	*funccal = get_funccal_for_vars();

    પાછા funccal == ખાલી ? ખાલી : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    શૂન્ય
list_func_vars(પૂર્ણાંક *first)
{
    જો (current_funccal != ખાલી && current_funccal->fc_l_vars.dv_refcount > 0)
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
    જો (current_funccal != ખાલી
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	પાછા &current_funccal->fc_l_vars;
    પાછા ખાલી;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ખાલી;
    char_u	*varname;

    જો (current_funccal == ખાલી || current_funccal->fc_func->uf_scoped == ખાલી)
	પાછા ખાલી;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    જ્યારે (current_funccal != ખાલી)
    {
	ht = find_var_ht(name, &varname);
	જો (ht != ખાલી && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    જો (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		તોડો;
	    }
	}
	જો (current_funccal == current_funccal->fc_func->uf_scoped)
	    તોડો;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    પાછા hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, પૂર્ણાંક no_autoload)
{
    dictitem_T	*v = ખાલી;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    જો (current_funccal == ખાલી || current_funccal->fc_func->uf_scoped == ખાલી)
	પાછા ખાલી;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    જ્યારે (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	જો (ht != ખાલી && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    જો (v != ખાલી)
		તોડો;
	}
	જો (current_funccal == current_funccal->fc_func->uf_scoped)
	    તોડો;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    પાછા v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    પૂર્ણાંક
set_ref_in_previous_funccal(પૂર્ણાંક copyID)
{
    funccall_T	*fc;

    માટે (fc = previous_funccal; fc != ખાલી; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	જો (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ખાલી, ખાલી)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ખાલી, ખાલી)
		|| set_ref_in_list_વસ્તુઓ(&fc->fc_l_varlist, copyID + 1, ખાલી, ખાલી))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
set_ref_in_funccal(funccall_T *fc, પૂર્ણાંક copyID)
{
    જો (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	જો (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, ખાલી, ખાલી)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, ખાલી, ખાલી)
		|| set_ref_in_list_વસ્તુઓ(&fc->fc_l_varlist, copyID, ખાલી, ખાલી)
		|| set_ref_in_func(ખાલી, fc->fc_func, copyID))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    પૂર્ણાંક
set_ref_in_call_ઢગલો(પૂર્ણાંક copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    માટે (fc = current_funccal; fc != ખાલી; fc = fc->fc_caller)
	જો (set_ref_in_funccal(fc, copyID))
	    પાછા TRUE;

    // Also go through the funccal_stack.
    માટે (entry = funccal_ઢગલો; entry != ખાલી; entry = entry->next)
	માટે (fc = entry->ટોચ_funccal; fc != ખાલી; fc = fc->fc_caller)
	    જો (set_ref_in_funccal(fc, copyID))
		પાછા TRUE;
    પાછા FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    પૂર્ણાંક
set_ref_in_functions(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		todo;
    hashitem_T	*hi = ખાલી;
    ufunc_T	*fp;

    todo = (પૂર્ણાંક)func_hashtab.ht_used;
    માટે (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    જો (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(ખાલી, fp, copyID))
		પાછા TRUE;
	}
    }
    પાછા FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    પૂર્ણાંક
set_ref_in_func_args(પૂર્ણાંક copyID)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < funcargs.ga_len; ++i)
	જો (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, ખાલી, ખાલી, ખાલી))
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    પૂર્ણાંક
set_ref_in_func(char_u *name, ufunc_T *fp_in, પૂર્ણાંક copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ખાલી;
    char_u	*fname;
    પૂર્ણાંક		abort = FALSE;

    જો (name == ખાલી && fp_in == ખાલી)
	પાછા FALSE;

    જો (fp_in == ખાલી)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    જો (fp != ખાલી)
    {
	માટે (fc = fp->uf_scoped; fc != ખાલી; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    પાછા abort;
}

#endif // FEAT_EVAL
