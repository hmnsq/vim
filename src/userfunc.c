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

#जइ defined(FEAT_EVAL)
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

स्थिर शून्य funccal_unref(funccall_T *fc, ufunc_T *fp, पूर्णाङ्क force);
स्थिर शून्य func_clear(ufunc_T *fp, पूर्णाङ्क force);
स्थिर पूर्णाङ्क func_free(ufunc_T *fp, पूर्णाङ्क force);
स्थिर char_u *untrans_function_name(char_u *name);
स्थिर शून्य handle_defer_one(funccall_T *funccal);
स्थिर char_u *trans_function_name_ext(char_u **pp, पूर्णाङ्क *is_global, पूर्णाङ्क skip, पूर्णाङ्क flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

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
    प्रत्यागम &func_hashtab;
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
	पूर्णाङ्क	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	पूर्णाङ्क	    is_vararg,
	पूर्णाङ्क	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = रिक्त;
    पूर्णाङ्क		is_underscore = FALSE;

    जाव (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    जइ (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == रिक्त
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	जइ (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	प्रत्यागम arg;
    }

    // Extra checks in Vim9 script.
    जइ (!skip && argtypes != रिक्त)
    {
	पूर्णाङ्क c = *p;
	*p = NUL;
	पूर्णाङ्क r = check_reserved_name(arg, FALSE);
	*p = c;
	जइ (r == FAIL)
	    प्रत्यागम arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	जइ (check_defined(arg, p - arg,
				   evalarg == रिक्त ? रिक्त : evalarg->eval_cctx,
			       eap == रिक्त ? रिक्त : eap->cstack, TRUE) == FAIL)
	    प्रत्यागम arg;
    }

    जइ (newargs != रिक्त && ga_grow(newargs, 1) == FAIL)
	प्रत्यागम arg;
    जइ (newargs != रिक्त)
    {
	पूर्णाङ्क	c;
	पूर्णाङ्क	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	जइ (arg_copy == रिक्त)
	{
	    *p = c;
	    प्रत्यागम arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	जइ (argtypes == रिक्त || !is_underscore)
	    // Check for duplicate argument name.
	    प्रति (i = 0; i < newargs->ga_len; ++i)
		जइ (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    प्रत्यागम arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    जइ (argtypes != रिक्त && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != रिक्त && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = रिक्त;

	जइ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == रिक्त ? arg : arg_copy);
	    p = skipwhite(p);
	}
	जइ (*p == ':')
	{
	    ++p;
	    जइ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		प्रत्यागम arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    जइ (!skip)
		type = vim_strnsave(type, p - type);
	}
	अण्णहा जइ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == रिक्त ? arg : arg_copy);
	    प्रत्यागम arg;
	}
	जइ (!skip)
	{
	    जइ (type == रिक्त && types_optional)
	    {
		// lambda arguments default to "any" type
		जइ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		अण्णहा
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    प्रत्यागम p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    स्थिर char_u *
get_function_पङ्क्तिः(
	exarg_T		*eap,
	garray_T	*पङ्क्तयः_to_free,
	पूर्णाङ्क		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    जइ (eap->ea_getline == रिक्त)
	theline = getcmdline(':', 0L, indent, 0);
    अण्णहा
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    जइ (theline != रिक्त)
    {
	जइ (पङ्क्तयः_to_free->ga_len > 0
		&& eap->cmdlinep != रिक्त
		&& *eap->cmdlinep == ((char_u **)पङ्क्तयः_to_free->ga_data)
						   [पङ्क्तयः_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(शून्य)ga_add_string(पङ्क्तयः_to_free, theline);
    }

    प्रत्यागम theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    स्थिर पूर्णाङ्क
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    पूर्णाङ्क		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    पूर्णाङ्क		*varargs,
    garray_T	*default_args,
    पूर्णाङ्क		skip,
    exarg_T	*eap,		// can be NULL
    पूर्णाङ्क		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*पङ्क्तयः_to_free)
{
    पूर्णाङ्क		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		any_default = FALSE;
    char_u	*whitep = *argp;
    पूर्णाङ्क		need_व्यञ्जकम् = FALSE;

    जइ (newargs != रिक्त)
	ga_init2(newargs, परिमाण(char_u *), 3);
    जइ (argtypes != रिक्त)
	ga_init2(argtypes, परिमाण(char_u *), 3);
    जइ (arg_objm != रिक्त)
	ga_init2(arg_objm, परिमाण(int8_T), 3);
    जइ (!skip && default_args != रिक्त)
	ga_init2(default_args, परिमाण(char_u *), 3);

    जइ (varargs != रिक्त)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    जाव (*p != endchar)
    {
	जाव (eap != रिक्त && eap->ea_getline != रिक्त
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_पङ्क्तिः(eap, पङ्क्तयः_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    जइ (theline == रिक्त)
		भङ्ग;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	जइ (mustend && *p != endchar)
	{
	    जइ (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    गच्छ err_ret;
	}
	जइ (*p == endchar && !need_व्यञ्जकम्)
	    भङ्ग;

	जइ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    जइ (varargs != रिक्त)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    जइ (argtypes != रिक्त)
	    {
		// ...name: list<type>
		जइ (!eval_isnamec1(*p))
		{
		    जइ (!skip)
			emsg(_(e_missing_name_after_dots));
		    गच्छ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		जइ (p == arg)
		    भङ्ग;
		जइ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    भङ्ग;
		}
	    }
	}
	अण्णहा जइ (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    जाव (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    जइ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		भङ्ग;
	    }

	    जइ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		जइ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_मानम्_must_be_vnone_str), p);
		    गच्छ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		जइ (ga_grow(default_args, 1) == FAIL)
		    गच्छ err_ret;

		char_u *व्यञ्जकम् = vim_strnsave((char_u *)"v:none", 6);
		जइ (व्यञ्जकम् == रिक्त)
		    गच्छ err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = व्यञ्जकम्;
		default_args->ga_len++;
	    }
	    अण्णहा जइ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		गच्छ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    जइ (newargs != रिक्त && ga_grow(newargs, 1) == FAIL)
		प्रत्यागम FAIL;
	    जइ (newargs != रिक्त)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		जइ (argtypes != रिक्त && ga_grow(argtypes, 1) == OK
			    && arg_objm != रिक्त && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    जइ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			पूर्णाङ्क len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			जइ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			जइ (assignment != रिक्त)
			{
			    c = *argend;
			    *argend = NUL;
			    जइ (any_default)
				vim_snprintf((वर्ण *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    अण्णहा
				vim_snprintf((वर्ण *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    जइ (*p == ',')
		++p;
	}
	अण्णहा
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    जइ (p == arg)
		भङ्ग;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    जइ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != रिक्त)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *व्यञ्जकम् = p;
		जइ (eval1(&p, &rettv, रिक्त) != FAIL)
		{
		    जइ (!skip)
		    {
			जइ (ga_grow(default_args, 1) == FAIL)
			    गच्छ err_ret;

			जइ (need_व्यञ्जकम्)
			    need_व्यञ्जकम् = FALSE;
			// trim trailing whitespace
			जाव (p > व्यञ्जकम् && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			व्यञ्जकम् = vim_strsave(व्यञ्जकम्);
			जइ (व्यञ्जकम् == रिक्त)
			{
			    *p = c;
			    गच्छ err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = व्यञ्जकम्;
			default_args->ga_len++;
			*p = c;
		    }
		}
		अण्णहा
		{
		    mustend = TRUE;
		    जइ (*skipwhite(p) == NUL)
			need_व्यञ्जकम् = TRUE;
		}
	    }
	    अण्णहा जइ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		गच्छ err_ret;
	    }

	    जइ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		जइ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    गच्छ err_ret;
		}
		p = skipwhite(p);
	    }
	    जइ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		जइ (!skip && argtypes != रिक्त
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    गच्छ err_ret;
		}
	    }
	    अण्णहा
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    जइ (*p != endchar)
	गच्छ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    प्रत्यागम OK;

err_ret:
    जइ (newargs != रिक्त)
	ga_clear_strings(newargs);
    जइ (!skip && default_args != रिक्त)
	ga_clear_strings(default_args);
    प्रत्यागम FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    पूर्णाङ्क		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    पूर्णाङ्क		obj_member_गणनम्,
    cctx_T	*cctx)
{
    पूर्णाङ्क len = 0;

    ga_init2(&fp->uf_type_list, परिमाण(type_T *), 10);
    जइ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	जइ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	जइ (fp->uf_arg_types != रिक्त)
	{
	    पूर्णाङ्क	i;
	    type_T	*type;

	    प्रति (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		जइ (p == रिक्त)
		    // will get the type from the default value
		    type = &t_unknown;
		अण्णहा
		{
		    जइ (arg_objm != रिक्त && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			प्रति (पूर्णाङ्क om = 0; om < obj_member_गणनम्; ++om)
			{
			    जइ (obj_members != रिक्त
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				भङ्ग;
			    }
			}
		    }
		    अण्णहा
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		जइ (type == रिक्त || !valid_declaration_type(type))
		    प्रत्यागम FAIL;
		fp->uf_arg_types[i] = type;
		जइ (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    जइ (obj_members != रिक्त && *name == '_')
			// protected object method
			name++;

		    जइ (var_wrong_func_name(name, TRUE))
			प्रत्यागम FAIL;
		}
	    }
	}
    }

    जइ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = रिक्त;
	p = ((char_u **)argtypes->ga_data)[len];
	जइ (p == रिक्त)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	अण्णहा
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    जइ (fp->uf_va_type != रिक्त && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		प्रत्यागम FAIL;
	    }
	}
	जइ (fp->uf_va_type == रिक्त)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    जइ (ret_type == रिक्त)
	fp->uf_ret_type = &t_void;
    अण्णहा
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	जइ (fp->uf_ret_type == रिक्त)
	{
	    fp->uf_ret_type = &t_void;
	    प्रत्यागम FAIL;
	}
    }
    प्रत्यागम OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    स्थिर पूर्णाङ्क
register_closure(ufunc_T *fp)
{
    जइ (fp->uf_scoped == current_funccal)
	// no change
	प्रत्यागम OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    जइ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	प्रत्यागम FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    प्रत्यागम OK;
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

    जइ (name[0] != K_SPECIAL)
	प्रत्यागम name;
    len = STRLEN(name);
    जइ (len + 3 > bufsize)
	प्रत्यागम name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    प्रत्यागम buf;
}

स्थिर char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(शून्य)
{
    स्थिर पूर्णाङ्क	lambda_no = 0;
    पूर्णाङ्क		n;
    string_T	ret;

    n = vim_snprintf((वर्ण *)lambda_name, परिमाण(lambda_name), "<lambda>%d", ++lambda_no);
    जइ (n < 1)
	ret.length = 0;
    अण्णहा जइ (n >= (पूर्णाङ्क)परिमाण(lambda_name))
	ret.length = परिमाण(lambda_name) - 1;
    अण्णहा
	ret.length = (size_t)n;

    ret.string = lambda_name;
    प्रत्यागम ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    स्थिर ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < परिमाण(ufunc_T) ? परिमाण(ufunc_T) : len);
    जइ (fp != रिक्त)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((शून्य *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	जइ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    जइ (fp->uf_name_exp != रिक्त)
		vim_snprintf((वर्ण *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    प्रत्यागम fp;
}

#जइ defined(FEAT_LUA)
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
    जइ (fp == रिक्त)
	प्रत्यागम रिक्त;

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

    प्रत्यागम name.string;
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
	पूर्णाङ्क	equal_arrow,
	char_u	**ret_type,
	पूर्णाङ्क	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    जइ (equal_arrow)
    {
	जइ (*s == ':')
	{
	    जइ (white_error != रिक्त && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		प्रत्यागम रिक्त;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    जइ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		प्रत्यागम रिक्त;
	    }
	}
	bef = s;
	s = skipwhite(s);
	जइ (*s != '=')
	    प्रत्यागम रिक्त;
	++s;
    }
    जइ (*s != '>')
	प्रत्यागम रिक्त;
    जइ (white_error != रिक्त && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    स्थिर पूर्णाङ्क
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    जइ (checkforcmd(&p, "function", 2))
    {
	जइ (*p == '(')
	    प्रत्यागम FALSE;
	*cmd = p;
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    स्थिर शून्य
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    जइ (cstack == रिक्त || cstack->cs_idx < 0)
	प्रत्यागम;

    पूर्णाङ्क	    गणनम् = cstack->cs_idx + 1;
    पूर्णाङ्क	    i;

    fp->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, गणनम्);
    जइ (fp->uf_block_ids != रिक्त)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		परिमाण(पूर्णाङ्क) * गणनम्);
	fp->uf_block_depth = गणनम्;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    प्रति (i = 0; i <= cstack->cs_idx; ++i)
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
    जाव (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	जइ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	अण्णहा जइ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    भङ्ग;
	}
	अण्णहा
	    MB_PTR_ADV(p);
    }

    प्रत्यागम p;
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
    जाव (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	जइ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	अण्णहा जइ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    भङ्ग;
	}
	अण्णहा
	    MB_PTR_ADV(p);
    }

    प्रत्यागम p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    स्थिर char_u *
find_start_of_vim9_comment(char_u *पङ्क्तिः)
{
    char_u	*p = पङ्क्तिः;

    जाव (*p != NUL)
    {
	जइ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	अण्णहा जइ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	अण्णहा
	{
	    जइ (*p == '#')
		// Found the start of a Vim9 comment
		भङ्ग;
	    MB_PTR_ADV(p);
	}
    }

    प्रत्यागम p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    स्थिर पूर्णाङ्क
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *पङ्क्तिः_arg_in,
	garray_T    *पङ्क्तयः_to_free)
{
    linenr_T	sourcing_lnum_शिखरम् = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    पूर्णाङ्क		saved_wait_return = need_wait_return;
    char_u	*पङ्क्तिः_arg = पङ्क्तिः_arg_in;
    पूर्णाङ्क		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    वर्ण	nesting_def[MAX_FUNC_NESTING];
    वर्ण	nesting_inline[MAX_FUNC_NESTING];
    पूर्णाङ्क		nesting = 0;
    getline_opt_T getline_options;
    पूर्णाङ्क		indent = 2;
    char_u	*skip_until = रिक्त;
    पूर्णाङ्क		ret = FAIL;
    पूर्णाङ्क		is_heredoc = FALSE;
    पूर्णाङ्क		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = रिक्त;
    size_t	heredoc_trimmedlen = 0;
    पूर्णाङ्क		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    जइ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	जइ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    गच्छ theend;
	जाव (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = रिक्त;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    प्रति (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	जइ (कुञ्चिका_Typed)
	{
	    सन्देशः_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	जइ (पङ्क्तिः_arg != रिक्त)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = पङ्क्तिः_arg;
	    p = vim_strchr(theline, '\n');
	    जइ (p == रिक्त)
		पङ्क्तिः_arg += STRLEN(पङ्क्तिः_arg);
	    अण्णहा
	    {
		*p = NUL;
		पङ्क्तिः_arg = p + 1;
	    }
	}
	अण्णहा
	{
	    theline = get_function_पङ्क्तिः(eap, पङ्क्तयः_to_free, indent,
							      getline_options);
	}
	जइ (कुञ्चिका_Typed)
	    पङ्क्तयः_वामः = Rows - 1;
	जइ (theline == रिक्त)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_शिखरम्;
	    जइ (skip_until != रिक्त)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    अण्णहा जइ (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    अण्णहा जइ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    अण्णहा
		emsg(_(e_missing_endfunction));
	    गच्छ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	जइ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	अण्णहा
	    sourcing_lnum_off = 0;

	जइ (skip_until != रिक्त)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    जइ (heredoc_trimmed == रिक्त
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		जइ (heredoc_trimmed == रिक्त)
		    p = theline;
		अण्णहा जइ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		अण्णहा
		    p = theline + heredoc_trimmedlen;
		जइ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    जइ (heredoc_concat_len > 0)
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
	अण्णहा
	{
	    पूर्णाङ्क	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    प्रति (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		जइ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    जइ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		जइ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		जइ (nesting-- == 0)
		{
		    char_u *nextcmd = रिक्त;

		    जइ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    अण्णहा जइ (पङ्क्तिः_arg != रिक्त && *skipwhite(पङ्क्तिः_arg) != NUL)
			nextcmd = पङ्क्तिः_arg;
		    अण्णहा जइ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_शिखरम्
							+ newlines->ga_len + 1;
			जइ (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			अण्णहा
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    जइ (nextcmd != रिक्त && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			जइ (पङ्क्तयः_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)पङ्क्तयः_to_free->ga_data)
						   [पङ्क्तयः_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)पङ्क्तयः_to_free->ga_data)
						   [पङ्क्तयः_to_free->ga_len - 1];
			    --पङ्क्तयः_to_free->ga_len;
			}
		    }
		    भङ्ग;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    अण्णहा जइ (nesting_def[nesting])
	    {
		जइ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    अण्णहा जइ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    जइ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    अण्णहा जइ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    जइ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		जइ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, रिक्त, TRUE, 0));
		जइ (vim9_function && *p == '<')
		{
		    // skip generic function
		    जइ (skip_generic_func_type_args(&p) == FAIL)
			गच्छ theend;
		}
		जइ (*skipwhite(p) == '(')
		{
		    जइ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    अण्णहा
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    जइ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		जइ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		अण्णहा
		    end = p + STRLEN(p) - 1;

		जाव (end > p && VIM_ISWHITE(*end))
		    --end;
		जइ (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    पूर्णाङ्क	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    जाव (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    जइ (!is_block)
			is_block = find_cmd_block_start(p) != रिक्त;

		    जइ (is_block)
		    {
			जइ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			अण्णहा
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
	    जइ (!vim9_function || colon)
		p = skip_range(p, FALSE, रिक्त);
	    char_u *tp = p;
	    जइ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    अण्णहा
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    जइ (arg[0] == '<' && arg[1] =='<'
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
		जइ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    जइ (heredoc_trimmed == रिक्त)
			heredoc_trimmedlen = 0;
		}
		जइ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		अण्णहा
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		जइ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    जइ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		जइ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    पूर्णाङ्क		save_sc_version = current_sctx.sc_version;
		    पूर्णाङ्क		var_गणनम् = 0;
		    पूर्णाङ्क		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_गणनम्, &semicolon,
									 TRUE);
		    जइ (arg != रिक्त)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    जइ (arg != रिक्त && STRNCMP(arg, "=<<", 3) == 0)
		    {
			पूर्णाङ्क has_trim = FALSE;

			p = skipwhite(arg + 3);
			जाव (TRUE)
			{
			    जइ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				अनुवर्त;
			    }
			    जइ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				अनुवर्त;
			    }
			    भङ्ग;
			}
			जइ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    जइ (heredoc_trimmed == रिक्त)
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
	जइ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    गच्छ theend;

	जइ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	अण्णहा
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	जइ (p == रिक्त)
	    गच्छ theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	जाव (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = रिक्त;

	// Check for end of eap->arg.
	जइ (पङ्क्तिः_arg != रिक्त && *पङ्क्तिः_arg == NUL)
	    पङ्क्तिः_arg = रिक्त;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    जइ (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    प्रत्यागम ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    स्थिर पूर्णाङ्क
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	पूर्णाङ्क	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    पूर्णाङ्क		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = रिक्त;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = रिक्त;
    पूर्णाङ्क		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    पूर्णाङ्क		lnum_save = -1;
    linenr_T	sourcing_lnum_शिखरम् = SOURCING_LNUM;
    char_u	*पङ्क्तिः_arg = रिक्त;

    *arg = skipwhite(*arg + 1);
    जइ (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	प्रत्यागम FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    जइ (**arg == '\n')
	पङ्क्तिः_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    जइ (evalarg->eval_cctx != रिक्त)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    अण्णहा
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, परिमाण(char_u *), 10);
    जइ (get_function_body(&eap, &newlines, पङ्क्तिः_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	गच्छ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_गणनम् += newlines.ga_len;
    जइ (gap->ga_itemsize > 0)
    {
	पूर्णाङ्क	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	प्रति (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    जइ (p == रिक्त)
		// comment line in the lambda body
		अनुवर्त;

	    p = skipwhite(p);

	    जइ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		गच्छ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    जइ (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    अण्णहा
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    जइ (pnl != रिक्त)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	जइ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    गच्छ erret;
	जइ (eap.nextcmd != रिक्त)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	अण्णहा
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	जइ (pnl != रिक्त)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    जइ (eap.nextcmd != रिक्त)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	जइ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    अण्णहा
	*arg = (char_u *)"";

    जइ (!evaluate)
    {
	ret = OK;
	गच्छ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    जइ (ufunc == रिक्त)
	गच्छ erret;
    जइ (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	गच्छ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = रिक्त;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = रिक्त;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_शिखरम्;

    // parse argument types, the type variables of the function being
    // compiled may be used
    जइ (parse_argument_types(ufunc, argtypes, varargs, रिक्त, रिक्त, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	गच्छ erret;
    }

    // parse the return type, if any
    जइ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	गच्छ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    जइ (pt == रिक्त)
	गच्छ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_पङ्क्तयः = newlines;
    newlines.ga_data = रिक्त;
    जइ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    जइ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_शिखरम्;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = रिक्त;
    ret = OK;

erret:
    जइ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    जइ (newargs != रिक्त)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    जइ (ufunc != रिक्त)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    प्रत्यागम ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    पूर्णाङ्क
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	पूर्णाङ्क	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = रिक्त;
    partial_T   *pt = रिक्त;
    पूर्णाङ्क		varargs;
    char_u	*ret_type = रिक्त;
    पूर्णाङ्क		ret;
    char_u	*s;
    char_u	*start, *end;
    पूर्णाङ्क		*old_eval_lavars = eval_lavars_used;
    पूर्णाङ्क		eval_lavars = FALSE;
    char_u	*tofree2 = रिक्त;
    पूर्णाङ्क		equal_arrow = **arg == '(';
    पूर्णाङ्क		white_error = FALSE;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    पूर्णाङ्क		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    जइ (equal_arrow && !vim9script)
	प्रत्यागम NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', रिक्त,
	    types_optional ? &argtypes : रिक्त, types_optional,
			types_optional ? &arg_objm : रिक्त, evalarg,
			रिक्त, &default_args, TRUE, रिक्त, FALSE, रिक्त, रिक्त);
    जइ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, रिक्त) == रिक्त)
    {
	जइ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	प्रत्यागम called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    जइ (evaluate)
	pnewargs = &newargs;
    अण्णहा
	pnewargs = रिक्त;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : रिक्त, types_optional,
			types_optional ? &arg_objm : रिक्त, evalarg,
					    &varargs, &default_args,
					    FALSE, रिक्त, FALSE, रिक्त, रिक्त);
    जइ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : रिक्त)) == रिक्त)
    {
	जइ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	प्रत्यागम white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    जइ (ret_type != रिक्त)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    जइ (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    जइ (equal_arrow && **arg == '{')
    {
	जइ (evalarg == रिक्त)
	    // cannot happen?
	    गच्छ theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	जइ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : रिक्त, varargs,
			   &default_args, ret_type) == FAIL)
	    गच्छ errret;
	गच्छ theend;
    }
    जइ (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_मानानि_in_lambda));
	गच्छ errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_व्यञ्जकम्_concatenate(arg, &start, &end, evalarg);
    जइ (ret == FAIL)
	गच्छ errret;

    जइ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जइ (**arg != '}')
	{
	    semsg(_(e_expected_दक्षिणः_curly_str), *arg);
	    गच्छ errret;
	}
	++*arg;
    }

    जइ (evaluate)
    {
	पूर्णाङ्क	    len;
	पूर्णाङ्क	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *पङ्क्तिः_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	जइ (fp == रिक्त)
	    गच्छ errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	जइ (pt == रिक्त)
	    गच्छ errret;

	ga_init2(&newlines, परिमाण(char_u *), 1);
	जइ (ga_grow(&newlines, 1) == FAIL)
	    गच्छ errret;

	// If there are line breaks, we need to split up the string.
	पङ्क्तिः_end = vim_strchr(start, '\n');
	जइ (पङ्क्तिः_end == रिक्त || पङ्क्तिः_end > end)
	    पङ्क्तिः_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (पूर्णाङ्क)(पङ्क्तिः_end - start) + 1;
	p = alloc(len);
	जइ (p == रिक्त)
	    गच्छ errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, पङ्क्तिः_end - start);

	जइ (पङ्क्तिः_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    जाव (पङ्क्तिः_end < end && *पङ्क्तिः_end == '\n')
	    {
		जइ (ga_grow(&newlines, 1) == FAIL)
		    गच्छ errret;
		start = पङ्क्तिः_end + 1;
		पङ्क्तिः_end = vim_strchr(start, '\n');
		जइ (पङ्क्तिः_end == रिक्त || पङ्क्तिः_end > end)
		    पङ्क्तिः_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, पङ्क्तिः_end - start);
	    }
	}

	जइ (strstr((वर्ण *)p + 7, "a:") == रिक्त)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	जइ (types_optional)
	{
	    जइ (parse_argument_types(fp, &argtypes,
				vim9script && varargs, रिक्त, रिक्त, 0,
				cctx) == FAIL)
		गच्छ errret;
	    जइ (ret_type != रिक्त)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     रिक्त, cctx, TRUE);
		जइ (fp->uf_ret_type == रिक्त)
		    गच्छ errret;
	    }
	    अण्णहा
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_पङ्क्तयः = newlines;
	जइ (current_funccal != रिक्त && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    जइ (register_closure(fp) == FAIL)
		गच्छ errret;
	}

#ifdef FEAT_PROFILE
	जइ (prof_def_func())
	    func_do_profile(fp);
#endif
	जइ (sandbox)
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
    जइ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    प्रत्यागम OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    जइ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    जइ (fp != रिक्त)
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
    प्रत्यागम FAIL;
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
	पूर्णाङ्क	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	पूर्णाङ्क	    no_autoload,
	पूर्णाङ्क	    new_function,
	पूर्णाङ्क	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = रिक्त;
    पूर्णाङ्क		cc;
    char_u	*s = रिक्त;
    hashtab_T	*ht;
    पूर्णाङ्क		did_type = FALSE;

    जइ (partialp != रिक्त)
	*partialp = रिक्त;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    जइ (v != रिक्त)
    {
	tv = &v->di_tv;
    }
    अण्णहा जइ (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	पूर्णाङ्क	    len = *lenp;

	जइ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	जइ (import != रिक्त)
	{
	    name[len] = NUL;
	    जइ (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    अण्णहा
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    प्रत्यागम (char_u *)"";	// just in case
	}
    }

    जइ (tv != रिक्त)
    {
	जइ (found_var != रिक्त)
	    *found_var = TRUE;
	जइ (tv->v_type == VAR_FUNC)
	{
	    जइ (tv->vval.v_string == रिक्त)
	    {
		*lenp = 0;
		प्रत्यागम (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    जइ (p != रिक्त && generic_func_find_open_bracket(name) != रिक्त)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		प्रत्यागम (char_u *)"";
	    }
	    जइ (p != रिक्त)
		*lenp = p - s;
	    अण्णहा
		*lenp = (पूर्णाङ्क)STRLEN(s);
	}
	अण्णहा जइ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    जइ (pt == रिक्त)
	    {
		*lenp = 0;
		प्रत्यागम (char_u *)"";	// just in case
	    }
	    जइ (partialp != रिक्त)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (पूर्णाङ्क)STRLEN(s);
	}

	जइ (s != रिक्त)
	{
	    जइ (!did_type && type != रिक्त && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		जइ (sv != रिक्त)
		    *type = sv->sv_type;
	    }
	    प्रत्यागम s;
	}
    }

    प्रत्यागम name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    शून्य
emsg_funcname(वर्ण *ermsg, char_u *name)
{
    char_u	*p = name;

    जइ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    जइ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    पूर्णाङ्क
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	पूर्णाङ्क	    partial_argc,
	typval_T    *argvars,
	पूर्णाङ्क	    *argcount,
	पूर्णाङ्क	    is_builtin)
{
    char_u	*argp = *arg;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		evaluate = evalarg == रिक्त
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    जाव (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	जइ (*argp == ')' || *argp == ',' || *argp == NUL)
	    भङ्ग;

	पूर्णाङ्क arg_idx = *argcount;
	जइ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    भङ्ग;
	}
	++*argcount;
	जइ (!is_builtin && check_typval_is_मानम्(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    भङ्ग;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	जइ (vim9script)
	{
	    जइ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		जइ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		भङ्ग;
	    }
	}
	अण्णहा
	    argp = skipwhite(argp);
	जइ (*argp != ',')
	    भङ्ग;
	जइ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    जइ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    भङ्ग;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    जइ (*argp == ')')
	++argp;
    अण्णहा
	ret = FAIL;
    *arg = argp;
    प्रत्यागम ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
get_func_tv(
    char_u	*name,		// name of the function
    पूर्णाङ्क		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    पूर्णाङ्क		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    पूर्णाङ्क		argcount = 0;			// number of arguments found
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		evaluate = evalarg == रिक्त
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == रिक्त ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    जइ (ret == OK)
    {
	पूर्णाङ्क	i = 0;
	पूर्णाङ्क	did_emsg_before = did_emsg;

	जइ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    जइ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, परिमाण(typval_T *), 50);
	    प्रति (i = 0; i < argcount; ++i)
		जइ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	जइ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    अण्णहा जइ (!aborting() && evaluate)
    {
	जइ (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	अण्णहा
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    जाव (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    जइ (vim9script)
	*arg = argp;
    अण्णहा
	*arg = skipwhite(argp);
    प्रत्यागम ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    स्थिर पूर्णाङ्क
eval_fname_sid(char_u *p)
{
    प्रत्यागम (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    जइ (script_name == name)
	प्रत्यागम name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (पूर्णाङ्क)KE_SNR;
    fname_buflen = 3;
    जइ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    अण्णहा
    {
	जइ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	अण्णहा
	{
	    fname_buflen += vim_snprintf((वर्ण *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    जइ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    अण्णहा
    {
	fname = alloc(fnamelen + 1);
	जइ (fname == रिक्त)
	    *error = FCERR_OTHER;
	अण्णहा
	{
	    *tofree = fname;
	    vim_snprintf((वर्ण *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    प्रत्यागम fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    शून्य
func_name_with_sid(char_u *name, पूर्णाङ्क sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (पूर्णाङ्क)KE_SNR;
    vim_snprintf((वर्ण *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    स्थिर ufunc_T *
find_func_with_sid(char_u *name, पूर्णाङ्क sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    जइ (!SCRIPT_ID_VALID(sid))
	प्रत्यागम रिक्त;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    जइ (!HASHITEM_EMPTY(hi))
	प्रत्यागम HI2UF(hi);
    प्रत्यागम रिक्त;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    स्थिर ufunc_T *
find_func_with_prefix(char_u *name, पूर्णाङ्क sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    जइ (vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	प्रत्यागम रिक्त;	// already has the prefix
    जइ (!SCRIPT_ID_VALID(sid))
	प्रत्यागम रिक्त;	// not in a script
    si = SCRIPT_ITEM(sid);
    जइ (si->sn_autoload_prefix != रिक्त)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	जइ (namep == रिक्त)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	जइ (len < परिमाण(buffer))
	    auto_name = buffer;
	अण्णहा
	    auto_name = alloc(len);
	जइ (auto_name != रिक्त)
	{
	    vim_snprintf((वर्ण *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    जइ (auto_name != buffer)
		vim_free(auto_name);
	    जइ (!HASHITEM_EMPTY(hi))
		प्रत्यागम HI2UF(hi);
	}
    }

    प्रत्यागम रिक्त;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, पूर्णाङ्क flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    जइ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	जइ (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    जइ (func != रिक्त)
		प्रत्यागम func;
	}
	जइ (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    जइ (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (पूर्णाङ्क)sid);
		जइ (func != रिक्त)
		    प्रत्यागम func;
	    }
	}
    }

    जइ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	जइ (!HASHITEM_EMPTY(hi))
	    प्रत्यागम HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    प्रत्यागम find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, पूर्णाङ्क is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    जइ (fp != रिक्त && (fp->uf_flags & FC_DEAD) == 0)
	प्रत्यागम fp;
    प्रत्यागम रिक्त;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    पूर्णाङ्क
func_is_global(ufunc_T *ufunc)
{
    प्रत्यागम ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    पूर्णाङ्क
func_requires_g_prefix(ufunc_T *ufunc)
{
    प्रत्यागम func_is_global(ufunc)
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
    स्थिर पूर्णाङ्क
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    पूर्णाङ्क	len;

    जइ (!func_is_global(fp))
	len = vim_snprintf((वर्ण *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    अण्णहा
	len = vim_snprintf((वर्ण *)buf, bufsize, "%s", fp->uf_name);

    प्रत्यागम (len >= (पूर्णाङ्क)bufsize) ? (पूर्णाङ्क)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    स्थिर शून्य
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    वर्ण	*name,
    varnumber_T nr)
{
    STRCPY(v->di_कुञ्चिका, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_सङ्ख्या;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_सङ्ख्या = nr;
}

/*
 * Free "fc".
 */
    स्थिर शून्य
free_funccal(funccall_T *fc)
{
    पूर्णाङ्क	i;

    प्रति (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	जइ (fp != रिक्त && fp->uf_scoped == fc)
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
    FOR_ALL_LIST_वस्तूनि(&fc->fc_l_varlist, li)
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
    पूर्णाङ्क	may_free_fc = fc->fc_refcount <= 0;
    पूर्णाङ्क	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    जइ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    अण्णहा
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    जइ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    अण्णहा
    {
	पूर्णाङ्क	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (पूर्णाङ्क)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तूनि(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    जइ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    जइ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = रिक्त;
    अण्णहा
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_वस्तूनि(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    जइ (free_fc)
	free_funccal(fc);
    अण्णहा
    {
	स्थिर पूर्णाङ्क made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	जइ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	अण्णहा जइ (++made_copy >= (पूर्णाङ्क)((4096 * 1024) / परिमाण(*fc)))
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
    स्थिर पूर्णाङ्क
numbered_function(char_u *name)
{
    प्रत्यागम SAFE_isdigit(*name)
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
    पूर्णाङ्क
func_name_refcount(char_u *name)
{
    प्रत्यागम numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    स्थिर शून्य
funccal_unref(funccall_T *fc, ufunc_T *fp, पूर्णाङ्क force)
{
    funccall_T	**pfc;
    पूर्णाङ्क		i;

    जइ (fc == रिक्त)
	प्रत्यागम;

    जइ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	प्रति (pfc = &previous_funccal; *pfc != रिक्त; pfc = &(*pfc)->fc_caller)
	{
	    जइ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		प्रत्यागम;
	    }
	}
    प्रति (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	जइ (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = रिक्त;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    स्थिर पूर्णाङ्क
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    जइ (fp->uf_flags & FC_DEAD)
	प्रत्यागम FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    जइ (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	प्रत्यागम FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    जइ (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	प्रत्यागम FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    प्रत्यागम TRUE;
}

    स्थिर शून्य
func_clear_वस्तूनि(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_पङ्क्तयः));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    जइ (IS_GENERIC_FUNC(fp))
	generic_func_clear_वस्तूनि(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = रिक्त;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    जइ (fp->uf_cb_free != रिक्त)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = रिक्त;
    }

    fp->uf_cb_state = रिक्त;
    fp->uf_cb = रिक्त;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_गणनम्);
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
func_clear(ufunc_T *fp, पूर्णाङ्क force)
{
    जइ (fp->uf_cleared)
	प्रत्यागम;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_वस्तूनि(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    स्थिर पूर्णाङ्क
func_free(ufunc_T *fp, पूर्णाङ्क force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    जइ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    जइ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	जइ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	प्रत्यागम OK;
    }
    प्रत्यागम FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    शून्य
func_clear_free(ufunc_T *fp, पूर्णाङ्क force)
{
    func_clear(fp, force);
    जइ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    अण्णहा
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    पूर्णाङ्क
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = रिक्त;

    जइ (ufunc == रिक्त)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	प्रत्यागम FAIL;
    }

    fp = find_func(global, TRUE);
    जइ (fp != रिक्त)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	प्रत्यागम FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    जइ (fp == रिक्त)
	प्रत्यागम FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    जइ (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_पङ्क्तयः, &fp->uf_पङ्क्तयः) == FAIL)
	गच्छ failed;

    जइ (ufunc->uf_arg_types != रिक्त)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	जइ (fp->uf_arg_types == रिक्त)
	    गच्छ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    परिमाण(type_T *) * fp->uf_args.ga_len);
    }
    जइ (ufunc->uf_va_name != रिक्त)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	जइ (fp->uf_va_name == रिक्त)
	    गच्छ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    जइ ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == रिक्त)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	जइ (pt == रिक्त)
	    गच्छ failed;
	जइ (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    गच्छ failed;
	}
	ufunc->uf_partial = pt;
    }

    प्रत्यागम OK;

failed:
    func_clear_free(fp, TRUE);
    प्रत्यागम FAIL;
}

स्थिर पूर्णाङ्क	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    पूर्णाङ्क
funcdepth_increment(शून्य)
{
    जइ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	प्रत्यागम FAIL;
    }
    ++funcdepth;
    प्रत्यागम OK;
}

    शून्य
funcdepth_decrement(शून्य)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    पूर्णाङ्क
funcdepth_get(शून्य)
{
    प्रत्यागम funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    शून्य
funcdepth_restore(पूर्णाङ्क depth)
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

    जइ (fc == रिक्त)
	प्रत्यागम रिक्त;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    प्रत्यागम fc;
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
    पूर्णाङ्क		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    पूर्णाङ्क		using_sandbox = FALSE;
    पूर्णाङ्क		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    पूर्णाङ्क		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    पूर्णाङ्क		default_arg_err = FALSE;
    dictitem_T	*v;
    पूर्णाङ्क		fixvar_idx = 0;	// index in fc_fixvar[]
    पूर्णाङ्क		i;
    पूर्णाङ्क		ai;
    पूर्णाङ्क		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    पूर्णाङ्क		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    जइ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_सङ्ख्या;
	rettv->vval.v_सङ्ख्या = -1;
	प्रत्यागम FCERR_FAILED;
    }

    पङ्क्तिः_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    जइ (fc == रिक्त)
	प्रत्यागम FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, परिमाण(ufunc_T *), 1);

    जइ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	जइ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == रिक्त ? रिक्त : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	जइ (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	जइ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	जइ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	जइ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != रिक्त && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	प्रत्यागम retval;
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
    जइ (selfdict != रिक्त)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_कुञ्चिका;
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
    जइ ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    जइ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_कुञ्चिका;
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
    जइ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    प्रति (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	पूर्णाङ्क	    addlocal = FALSE;
	typval_T    def_rettv;
	पूर्णाङ्क	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	जइ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    जइ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_सङ्ख्या == VVAL_NONE));
	    जइ (isdefault)
	    {
		char_u	    *default_व्यञ्जकम् = रिक्त;

		def_rettv.v_type = VAR_सङ्ख्या;
		def_rettv.vval.v_सङ्ख्या = -1;

		default_व्यञ्जकम् = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		जइ (eval1(&default_व्यञ्जकम्, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    भङ्ग;
		}
	    }

	    namelen = STRLEN(name);
	}
	अण्णहा
	{
	    जइ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		भङ्ग;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((वर्ण *)numbuf, परिमाण(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	जइ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_कुञ्चिका, name);
	}
	अण्णहा
	{
	    v = dictitem_alloc(name);
	    जइ (v == रिक्त)
		भङ्ग;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	जइ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	जइ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	अण्णहा
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	जइ (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    जइ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_प्रक्षिप_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    जइ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	जइ (p_verbose >= 14)
	{
	    char_u	buf[सन्देशः_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    सन्देशः_puts("(");
	    प्रति (i = 0; i < argcount; ++i)
	    {
		जइ (i > 0)
		    सन्देशः_puts(", ");
		जइ (argvars[i].v_type == VAR_सङ्ख्या)
		    सन्देशः_outnum((long)argvars[i].vval.v_सङ्ख्या);
		अण्णहा
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    जइ (s != रिक्त)
		    {
			जइ (vim_strsize(s) > सन्देशः_BUF_CLEN)
			{
			    trunc_string(s, buf, सन्देशः_BUF_CLEN, सन्देशः_BUF_LEN);
			    s = buf;
			}
			सन्देशः_puts((वर्ण *)s);
			vim_free(tofree);
		    }
		}
	    }
	    सन्देशः_puts(")");
	}
	सन्देशः_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    जइ (do_profiling == PROF_YES)
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

    जइ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    अण्णहा जइ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_पङ्क्तयः.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(शून्य)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    अण्णहा
	// call do_cmdline() to execute the lines
	do_cmdline(रिक्त, get_func_पङ्क्तिः, (शून्य *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    जइ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    जइ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_सङ्ख्या;
	rettv->vval.v_सङ्ख्या = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	जइ (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    जइ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == रिक्त ? रिक्त : fc->fc_caller->fc_func;

	जइ (fp->uf_profiling || (caller != रिक्त && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    जइ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	जइ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	अण्णहा जइ (fc->fc_rettv->v_type == VAR_सङ्ख्या)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_सङ्ख्या);
	अण्णहा
	{
	    char_u	buf[सन्देशः_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    जइ (s != रिक्त)
	    {
		जइ (vim_strsize(s) > सन्देशः_BUF_CLEN)
		{
		    trunc_string(s, buf, सन्देशः_BUF_CLEN, सन्देशः_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	सन्देशः_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_निष्कासय();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    जइ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    जइ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    जइ (p_verbose >= 12 && SOURCING_NAME != रिक्त)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	सन्देशः_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    प्रति (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    प्रत्यागम retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, पूर्णाङ्क argcount)
{
    पूर्णाङ्क regular_args = fp->uf_args.ga_len;

    जइ (argcount < regular_args - fp->uf_def_args.ga_len)
	प्रत्यागम FCERR_TOOFEW;
    अण्णहा जइ (!has_varargs(fp) && argcount > regular_args)
	प्रत्यागम FCERR_TOOMANY;
    प्रत्यागम FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	पूर्णाङ्क	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    जइ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	प्रत्यागम (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    जइ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != रिक्त)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    जइ (error != FCERR_UNKNOWN)
	प्रत्यागम error;

    जइ ((fp->uf_flags & FC_DICT) && selfdict == रिक्त)
    {
	error = FCERR_DICT;
    }
    अण्णहा
    {
	पूर्णाङ्क		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_अन्वेषणम्_patterns();
	जइ (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : रिक्त);
	जइ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	जइ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_अन्वेषणम्_patterns();
    }

    प्रत्यागम error;
}

स्थिर funccal_entry_T *funccal_राशिः = रिक्त;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    शून्य
save_funccal(funccal_entry_T *entry)
{
    entry->शिखरम्_funccal = current_funccal;
    entry->next = funccal_राशिः;
    funccal_राशिः = entry;
    current_funccal = रिक्त;
}

    शून्य
restore_funccal(शून्य)
{
    जइ (funccal_राशिः == रिक्त)
	internal_error("restore_funccal()");
    अण्णहा
    {
	current_funccal = funccal_राशिः->शिखरम्_funccal;
	funccal_राशिः = funccal_राशिः->next;
    }
}

    funccall_T *
get_current_funccal(शून्य)
{
    प्रत्यागम current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    पूर्णाङ्क
at_script_level(शून्य)
{
    प्रत्यागम current_funccal == रिक्त && autocmd_match == रिक्त;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    शून्य
delete_script_functions(पूर्णाङ्क sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (पूर्णाङ्क)KE_SNR;
    len = 3 + vim_snprintf((वर्ण *)buf + 3, परिमाण(buf) - 3, "%d_", sid);

    जाव (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तूनि(&func_hashtab, hi, todo)
	    जइ (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		जइ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    पूर्णाङ्क changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    जइ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			जइ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    अण्णहा
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			जइ (changed != func_hashtab.ht_changed)
			    भङ्ग;
		    }
		}
		--todo;
	    }
    }
}

#जइ defined(EXITFREE)
    शून्य
free_all_functions(शून्य)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    पूर्णाङ्क		changed;

    // Clean up the current_funccal chain and the funccal stack.
    जाव (current_funccal != रिक्त)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	जइ (current_funccal == रिक्त && funccal_राशिः != रिक्त)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    जाव (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तूनि(&func_hashtab, hi, todo)
	    जइ (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		जइ (func_name_refcount(fp->uf_name))
		    ++skipped;
		अण्णहा
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    जइ (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			भङ्ग;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    जाव (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_वस्तूनि(&func_hashtab, hi, todo)
	    जइ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		जइ (func_name_refcount(fp->uf_name))
		    ++skipped;
		अण्णहा
		{
		    जइ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			भङ्ग;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    जइ (skipped == 0)
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
    पूर्णाङ्क
builtin_function(char_u *name, पूर्णाङ्क len)
{
    पूर्णाङ्क i;

    जइ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	प्रत्यागम FALSE;
    प्रति (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	जइ (name[i] == AUTOLOAD_CHAR)
	    प्रत्यागम FALSE;
	जइ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    जइ (name[i] == '.')
		प्रत्यागम FALSE;
	    भङ्ग;
	}
    }
    प्रत्यागम TRUE;
}

    पूर्णाङ्क
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
    पूर्णाङ्क		argc = 0;
    पूर्णाङ्क		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_वस्तूनि(l, item)
    {
	जइ (argc == MAX_FUNC_ARGS - (partial == रिक्त ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    भङ्ग;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    जइ (item == रिक्त)
    {
	funcexe_T	funcexe;
	पूर्णाङ्क		namelen = -1;

	जइ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    जइ (p != रिक्त)
	    {
		जइ (skip_generic_func_type_args(&p) == FAIL)
		    गच्छ done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	जइ (partial != रिक्त)
	{
	    funcexe.fe_object = partial->pt_obj;
	    जइ (funcexe.fe_object != रिक्त)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    जाव (argc > 0)
	clear_tv(&argv[--argc]);

done:
    प्रत्यागम r;
}

स्थिर पूर्णाङ्क callback_depth = 0;

    पूर्णाङ्क
get_callback_depth(शून्य)
{
    प्रत्यागम callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    पूर्णाङ्क
call_callback(
    callback_T	*callback,
    पूर्णाङ्क		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    पूर्णाङ्क		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    पूर्णाङ्क		ret;

    जइ (callback->cb_name == रिक्त || *callback->cb_name == NUL)
	प्रत्यागम FAIL;

    जइ (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	प्रत्यागम FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    जइ (callback->cb_partial != रिक्त)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	जइ (funcexe.fe_object != रिक्त)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    जइ (need_rethrow && current_exception != रिक्त && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    प्रत्यागम ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    पूर्णाङ्क		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    जइ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	प्रत्यागम -2;

    retval = tv_get_सङ्ख्या_chk(&rettv, रिक्त);
    clear_tv(&rettv);
    प्रत्यागम retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    शून्य
user_func_error(funcerror_T error, char_u *name, पूर्णाङ्क found_var)
{
    विकल्प (error)
    {
	पक्ष FCERR_UNKNOWN:
		जइ (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		अण्णहा
		    emsg_funcname(e_unknown_function_str, name);
		भङ्ग;
	पक्ष FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		भङ्ग;
	पक्ष FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		भङ्ग;
	पक्ष FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		भङ्ग;
	पक्ष FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		भङ्ग;
	पक्ष FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		भङ्ग;
	पक्ष FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		भङ्ग;
	पक्ष FCERR_OTHER:
	पक्ष FCERR_FAILED:
		// assume the error message was already given
		भङ्ग;
	पक्ष FCERR_NONE:
		भङ्ग;
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
	पूर्णाङ्क	    argcount,
	पूर्णाङ्क	    base_included,
	char_u	    *name)
{
    जइ (funcexe->fe_check_type != रिक्त && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	जइ (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? रिक्त : funcexe->fe_basetv,
			  name) == FAIL)
	    प्रत्यागम FCERR_OTHER;
    }
    प्रत्यागम FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    पूर्णाङ्क
call_func(
    char_u	*funcname,	// name of the function
    पूर्णाङ्क		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    पूर्णाङ्क		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    पूर्णाङ्क		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    पूर्णाङ्क		i;
    ufunc_T	*fp = रिक्त;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*fname = रिक्त;
    char_u	*name = रिक्त;
    पूर्णाङ्क		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    पूर्णाङ्क		argv_clear = 0;
    पूर्णाङ्क		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    जइ (partial != रिक्त)
	fp = partial->pt_func;
    जइ (fp == रिक्त)
	fp = funcexe->fe_ufunc;

    जइ (fp == रिक्त)
    {
	जइ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    जइ (p != रिक्त)
	    {
		len = p - funcname;
		जइ (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == रिक्त)
		    गच्छ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	जइ (name == रिक्त)
	    गच्छ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    जइ (funcexe->fe_doesrange != रिक्त)
	*funcexe->fe_doesrange = FALSE;

    जइ (partial != रिक्त)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	जइ (partial->pt_dict != रिक्त && (selfdict == रिक्त || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	जइ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    प्रति (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		जइ (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    गच्छ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    प्रति (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    जइ (funcexe->fe_check_type != रिक्त
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		प्रति (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    जइ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != रिक्त) ? name : funcname);

    जइ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	पूर्णाङ्क	is_global = FALSE;

	// Skip "g:" before a function name.
	जइ (fp == रिक्त && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_सङ्ख्या;	// default rettv is number zero
	rettv->vval.v_सङ्ख्या = 0;
	error = FCERR_UNKNOWN;

	जइ (fp != रिक्त || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    जइ (fp == रिक्त)
	    {
		fp = find_func(rfname, is_global);
		जइ (fp != रिक्त && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = रिक्त;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    जइ (fp == रिक्त
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, रिक्त)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    जइ (fp == रिक्त && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    जइ (fp == रिक्त)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		जइ (p != रिक्त && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    जइ (partial == रिक्त && fp != रिक्त && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		जइ (fp == रिक्त)
		{
		    error = FCERR_FAILED;
		    गच्छ theend;
		}
	    }
	    अण्णहा जइ (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != रिक्त ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		गच्छ theend;
	    }

	    जइ (fp != रिक्त && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    अण्णहा जइ (fp != रिक्त)
	    {
		पूर्णाङ्क need_arg_check = FALSE;
		जइ (funcexe->fe_check_type == रिक्त)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		जइ (funcexe->fe_argv_func != रिक्त)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		जइ (funcexe->fe_basetv != रिक्त)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, परिमाण(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		जइ (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != रिक्त) ? name : funcname);

		जइ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	अण्णहा जइ (funcexe->fe_basetv != रिक्त)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	अण्णहा
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
    जइ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    जइ (!aborting())
	user_func_error(error, (name != रिक्त) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    जाव (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    प्रत्यागम ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    पूर्णाङ्क
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    पूर्णाङ्क		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    पूर्णाङ्क		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_सङ्ख्या;	// default rettv is number zero
    rettv->vval.v_सङ्ख्या = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    जइ (name == रिक्त)
	प्रत्यागम ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    जइ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    अण्णहा
	rfname = fname;
    fp = find_func(rfname, is_global);
    जइ (fp != रिक्त && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = रिक्त;
    जइ (fp == रिक्त)
	ret = NOTDONE;
    अण्णहा जइ (fp != रिक्त && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    अण्णहा जइ (fp != रिक्त)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, रिक्त);
	जइ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    प्रत्यागम ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    प्रत्यागम fp->uf_name_exp != रिक्त ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    स्थिर पूर्णाङ्क
function_list_modified(पूर्णाङ्क prev_ht_changed)
{
    जइ (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    स्थिर पूर्णाङ्क
list_func_head(ufunc_T *fp, पूर्णाङ्क indent)
{
    पूर्णाङ्क		prev_ht_changed = func_hashtab.ht_changed;
    पूर्णाङ्क		j;

    सन्देशः_start();

    // a timer at the more prompt may have deleted the function
    जइ (function_list_modified(prev_ht_changed))
	प्रत्यागम FAIL;

    जइ (indent)
	सन्देशः_puts("   ");
    जइ (fp->uf_def_status != UF_NOT_COMPILED)
	सन्देशः_puts("def ");
    अण्णहा
	सन्देशः_puts("function ");
    सन्देशः_puts((वर्ण *)printable_func_name(fp));
    सन्देशः_putchar('(');
    प्रति (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	जइ (j)
	    सन्देशः_puts(", ");
	सन्देशः_puts((वर्ण *)FUNCARG(fp, j));
	जइ (fp->uf_arg_types != रिक्त)
	{
	    वर्ण *tofree;

	    सन्देशः_puts(": ");
	    सन्देशः_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	जइ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    सन्देशः_puts(" = ");
	    सन्देशः_puts(((वर्ण **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    जइ (fp->uf_varargs)
    {
	जइ (j)
	    सन्देशः_puts(", ");
	सन्देशः_puts("...");
    }
    जइ (fp->uf_va_name != रिक्त)
    {
	जइ (!fp->uf_varargs)
	{
	    जइ (j)
		सन्देशः_puts(", ");
	    सन्देशः_puts("...");
	}
	सन्देशः_puts((वर्ण *)fp->uf_va_name);
	जइ (fp->uf_va_type != रिक्त)
	{
	    वर्ण *tofree;

	    सन्देशः_puts(": ");
	    सन्देशः_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    सन्देशः_putchar(')');

    जइ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	जइ (fp->uf_ret_type != &t_void)
	{
	    वर्ण *tofree;

	    सन्देशः_puts(": ");
	    सन्देशः_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    अण्णहा जइ (fp->uf_flags & FC_ABORT)
	सन्देशः_puts(" abort");
    जइ (fp->uf_flags & FC_RANGE)
	सन्देशः_puts(" range");
    जइ (fp->uf_flags & FC_DICT)
	सन्देशः_puts(" dict");
    जइ (fp->uf_flags & FC_CLOSURE)
	सन्देशः_puts(" closure");
    सन्देशः_clr_eos();
    जइ (p_verbose > 0)
	last_set_सन्देशः(fp->uf_script_ctx);

    प्रत्यागम OK;
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
    पूर्णाङ्क		*is_global,
    पूर्णाङ्क		skip,		// only find the end, don't evaluate
    पूर्णाङ्क		flags)
{
    प्रत्यागम trans_function_name_ext(pp, is_global, skip, flags,
	    रिक्त, रिक्त, रिक्त, रिक्त);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    स्थिर char_u *
trans_function_name_ext(
    char_u	**pp,
    पूर्णाङ्क		*is_global,
    पूर्णाङ्क		skip,		// only find the end, don't evaluate
    पूर्णाङ्क		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = रिक्त;
    char_u	*start;
    char_u	*end;
    पूर्णाङ्क		lead;
    char_u	sid_buf[20];
    पूर्णाङ्क		len;
    पूर्णाङ्क		extra = 0;
    पूर्णाङ्क		prefix_g = FALSE;
    lval_T	lv;
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		vim9_local;

    जइ (fdp != रिक्त)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    जइ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (पूर्णाङ्क)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	प्रत्यागम vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    जइ (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, रिक्त, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    जइ (end == start || (vim9script && end != रिक्त
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	जइ (!skip)
	    emsg(_(e_function_name_required));
	गच्छ theend;
    }
    जइ (end == रिक्त || (lv.ll_tv != रिक्त && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	जइ (!aborting())
	{
	    जइ (end != रिक्त)
		semsg(_(e_invalid_argument_str), start);
	}
	अण्णहा
	    *pp = find_name_end(start, रिक्त, रिक्त, FNE_INCL_BR);
	गच्छ theend;
    }

    जइ (lv.ll_ufunc != रिक्त)
    {
	जइ (ufunc != रिक्त)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	गच्छ theend;
    }

    जइ (lv.ll_tv != रिक्त)
    {
	जइ (fdp != रिक्त)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = रिक्त;
	    fdp->fd_di = lv.ll_di;
	}
	जइ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != रिक्त)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	अण्णहा जइ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != रिक्त)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	अण्णहा जइ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != रिक्त)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    जइ (partial != रिक्त)
		*partial = lv.ll_tv->vval.v_partial;
	}
	अण्णहा
	{
	    जइ (!skip && !(flags & TFN_QUIET) && (fdp == रिक्त
			     || lv.ll_dict == रिक्त || fdp->fd_newkey == रिक्त))
		emsg(_(e_funcref_required));
	    अण्णहा
		*pp = end;
	    name = रिक्त;
	}
	गच्छ theend;
    }

    जइ (lv.ll_name == रिक्त)
    {
	// Error found, but continue after the function name.
	*pp = end;
	गच्छ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    जइ (lv.ll_exp_name != रिक्त)
    {
	len = (पूर्णाङ्क)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, रिक्त);
	जइ (name == lv.ll_exp_name)
	    name = रिक्त;
    }
    अण्णहा जइ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	पूर्णाङ्क		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	जइ (si->sn_autoload_prefix != रिक्त)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	अण्णहा
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (पूर्णाङ्क)KE_SNR;
	    vim_snprintf((वर्ण *)sid_buf + 3, परिमाण(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	गच्छ theend;
    }
    अण्णहा जइ (!(flags & TFN_NO_DEREF))
    {
	len = (पूर्णाङ्क)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, रिक्त);
	जइ (name == *pp)
	    name = रिक्त;
    }
    जइ (name != रिक्त)
    {
	name = vim_strsave(name);
	*pp = end;
	जइ (name != रिक्त && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (पूर्णाङ्क)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	गच्छ theend;
    }

    जइ (lv.ll_exp_name != रिक्त)
    {
	len = (पूर्णाङ्क)STRLEN(lv.ll_exp_name);
	जइ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    अण्णहा
    {
	// skip over "s:" and "g:"
	जइ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    जइ (lv.ll_name[0] == 'g')
	    {
		जइ (is_global != रिक्त)
		{
		    *is_global = TRUE;
		}
		अण्णहा
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (पूर्णाङ्क)(end - lv.ll_name);
    }
    जइ (len <= 0)
    {
	जइ (!skip)
	    emsg(_(e_function_name_required));
	गच्छ theend;
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
    जइ (skip)
	lead = 0;	// do nothing
    अण्णहा जइ (lead > 0 || vim9_local)
    {
	जइ (!vim9_local)
	{
	    जइ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		गच्छ theend;
	    }
	    lead = 3;
	}
	जइ (vim9_local || (lv.ll_exp_name != रिक्त
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    जइ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		गच्छ theend;
	    }
	    sid_buflen = vim_snprintf((वर्ण *)sid_buf, परिमाण(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    जइ (vim9_local)
		extra = 3 + (पूर्णाङ्क)sid_buflen;
	    अण्णहा
		lead += (पूर्णाङ्क)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    अण्णहा जइ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	गच्छ theend;
    }
    जइ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	जइ (cp != रिक्त && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    गच्छ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    जइ (name != रिक्त)
    {
	जइ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (पूर्णाङ्क)KE_SNR;
	    जइ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	अण्णहा जइ (prefix_g)
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
    प्रत्यागम name;
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

    जइ (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	जइ (p != रिक्त)
	    प्रत्यागम p + 1;
    }
    प्रत्यागम रिक्त;
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
    वर्ण	sid_buf[25];
    size_t	sid_buflen;
    पूर्णाङ्क		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    जइ (funcname == रिक्त)
	प्रत्यागम रिक्त;

    जइ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	जइ (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    प्रत्यागम रिक्त;
	ufunc = find_func(funcname, FALSE);
	जइ (ufunc == रिक्त || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == रिक्त)
	    प्रत्यागम रिक्त;
	++p;
	off = 0;
    }
    अण्णहा
	off = *funcname == 's' ? 2 : 5;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	प्रत्यागम रिक्त;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, परिमाण(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    जइ (newname == रिक्त)
	प्रत्यागम रिक्त;
    vim_snprintf((वर्ण *)newname, newnamesize, "%s%s", sid_buf, p + off);

    प्रत्यागम newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    जइ (n != रिक्त)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    प्रत्यागम n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	पूर्णाङ्क	    *is_global,
	पूर्णाङ्क	    skip,
	पूर्णाङ्क	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    जइ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(शून्य)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	जइ (fudi != रिक्त)
	    CLEAR_POINTER(fudi);
    }
    अण्णहा
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, रिक्त, रिक्त, रिक्त);
    *name = p;
    प्रत्यागम saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    शून्य
list_functions(regmatch_T *regmatch)
{
    पूर्णाङ्क		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    प्रति (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    जइ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == रिक्त
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		जइ (list_func_head(fp, FALSE) == FAIL)
		    प्रत्यागम;
		जइ (function_list_modified(prev_ht_changed))
		    प्रत्यागम;
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
    जइ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	जइ (regmatch.regprog != रिक्त)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    जइ (*p == '/')
	++p;

    प्रत्यागम p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    स्थिर ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, पूर्णाङ्क is_global)
{
    ufunc_T	*fp = रिक्त;
    पूर्णाङ्क		j;

    जइ (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	प्रत्यागम रिक्त;
    }

    set_nextcmd(eap, p);

    जइ (eap->nextcmd != रिक्त)
	*p = NUL;

    जइ (eap->skip || got_int)
	प्रत्यागम रिक्त;

    fp = find_func(name, is_global);
    जइ (fp == रिक्त && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	जइ (up != रिक्त)
	    fp = find_func(up, FALSE);
    }

    जइ (fp == रिक्त)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	प्रत्यागम रिक्त;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    पूर्णाङ्क prev_ht_changed = func_hashtab.ht_changed;

    जइ (list_func_head(fp, TRUE) != OK)
	प्रत्यागम fp;

    प्रति (j = 0; j < fp->uf_पङ्क्तयः.ga_len && !got_int; ++j)
    {
	जइ (FUNCLINE(fp, j) == रिक्त)
	    अनुवर्त;
	सन्देशः_putchar('\n');
	सन्देशः_outnum((long)(j + 1));
	जइ (j < 9)
	    सन्देशः_putchar(' ');
	जइ (j < 99)
	    सन्देशः_putchar(' ');
	जइ (function_list_modified(prev_ht_changed))
	    भङ्ग;
	सन्देशः_prt_पङ्क्तिः(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    जइ (!got_int)
    {
	सन्देशः_putchar('\n');
	जइ (!function_list_modified(prev_ht_changed))
	{
	    जइ (fp->uf_def_status != UF_NOT_COMPILED)
		सन्देशः_puts("   enddef");
	    अण्णहा
		सन्देशः_puts("   endfunction");
	}
    }

    प्रत्यागम fp;
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
	garray_T    *पङ्क्तयः_to_free,
	पूर्णाङ्क	    class_flags,
	ocmember_T  *obj_members,
	पूर्णाङ्क         obj_member_गणनम्,
	cctx_T	    *cctx)
{
    पूर्णाङ्क		j;
    पूर्णाङ्क		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    पूर्णाङ्क		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*पङ्क्तिः_arg = रिक्त;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    पूर्णाङ्क		varargs = FALSE;
    पूर्णाङ्क		flags = 0;
    char_u	*ret_type = रिक्त;
    ufunc_T	*fp = रिक्त;
    पूर्णाङ्क		fp_allocated = FALSE;
    पूर्णाङ्क		free_fp = FALSE;
    पूर्णाङ्क		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    स्थिर पूर्णाङ्क	func_nr = 0;	    // number for nameless function
    पूर्णाङ्क		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_शिखरम्;
    पूर्णाङ्क		vim9script = in_vim9script();
    imported_T	*import = रिक्त;

    // ":function" without argument: list functions.
    जइ (ends_excmd2(eap->cmd, eap->arg))
    {
	जइ (!eap->skip)
	    list_functions(रिक्त);
	set_nextcmd(eap, eap->arg);
	प्रत्यागम रिक्त;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    जइ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	प्रत्यागम रिक्त;
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
    जइ (name_arg != रिक्त)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    अण्णहा
    {
	जइ (vim9script)
	{
	    जइ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		प्रत्यागम रिक्त;
	    }
	    p = to_name_end(p, TRUE);
	    जइ (*skipwhite(p) == '.' && vim_strchr(p, '(') != रिक्त)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		प्रत्यागम रिक्त;
	    }

	    p = eap->arg;
	}

	पूर्णाङ्क tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != रिक्त);
	जइ (name == रिक्त && (fudi.fd_dict == रिक्त || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    जइ (!aborting())
	    {
		जइ (!eap->skip && fudi.fd_newkey != रिक्त)
		    semsg(_(e_कुञ्चिका_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		प्रत्यागम रिक्त;
	    }
	    अण्णहा
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	जइ (is_export && name != रिक्त)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    जइ (prefixed != रिक्त && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	अण्णहा जइ (paren && vim9script && name != रिक्त
				    && vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    गच्छ ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    जइ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	गच्छ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    जइ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	जइ (p == रिक्त)
	    गच्छ ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    जइ (*p != '(')
    {
	जइ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    गच्छ ret_free;
	}
	// attempt to continue by skipping some text
	जइ (vim_strchr(p, '(') != रिक्त)
	    p = vim_strchr(p, '(');
    }

    जइ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	गच्छ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    जइ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	गच्छ ret_free;
    }

    ga_init2(&newlines, परिमाण(char_u *), 10);

    जइ (!eap->skip && name_arg == रिक्त)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	जइ (name != रिक्त)
	    arg = name;
	अण्णहा
	    arg = fudi.fd_newkey;
	जइ (arg != रिक्त && (fudi.fd_di == रिक्त
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    पूर्णाङ्क	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    जइ (arg != fudi.fd_newkey)
	    {
		जइ (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    जइ (name_base == रिक्त)
			name_base = arg + 3;
		    अण्णहा
			++name_base;
		}
		प्रति (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		जइ (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    गच्छ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		जइ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, रिक्त,
			रिक्त, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    गच्छ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	जइ (fudi.fd_dict != रिक्त && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    गच्छ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    जइ (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : रिक्त, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : रिक्त,
			 रिक्त, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, पङ्क्तयः_to_free) == FAIL)
	गच्छ errret_2;
    whitep = p;

    जइ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	जइ (*skipwhite(p) == ':')
	{
	    जइ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    अण्णहा जइ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    जइ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    अण्णहा
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = रिक्त;
	    }
	}
	p = skipwhite(p);
    }
    अण्णहा
	// find extra arguments "range", "dict", "abort" and "closure"
	प्रति (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    जइ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    अण्णहा जइ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    अण्णहा जइ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    अण्णहा जइ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		जइ (current_funccal == रिक्त)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_शिखरम्_level_str,
			    name == रिक्त ? (char_u *)"" : name);
		    गच्छ erret;
		}
	    }
	    अण्णहा
		भङ्ग;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    जइ (*p == '\n')
	पङ्क्तिः_arg = p + 1;
    अण्णहा जइ (*p != NUL
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
    जइ (कुञ्चिका_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	जइ (!eap->skip && !eap->forceit)
	{
	    जइ (fudi.fd_dict != रिक्त && fudi.fd_newkey == रिक्त)
		emsg(_(e_dictionary_entry_already_exists));
	    अण्णहा जइ (name != रिक्त && find_func(name, is_global) != रिक्त)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	जइ (!eap->skip && did_emsg)
	    गच्छ erret;

	सन्देशः_putchar('\n');	    // don't overwrite the function name
	cmdline_row = सन्देशः_row;
    }

    // Save the starting line number.
    sourcing_lnum_शिखरम् = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    जइ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, पङ्क्तिः_arg, पङ्क्तयः_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	गच्छ erret;

    /*
     * If there are no errors, add the function
     */
    जइ (fudi.fd_dict != रिक्त)
    {
	वर्ण	numbuf[NUMBUFLEN];

	fp = रिक्त;
	जइ (fudi.fd_newkey == रिक्त && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    गच्छ erret;
	}
	जइ (fudi.fd_di == रिक्त)
	{
	    // Can't add a function to a locked dictionary
	    जइ (मानम्_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		गच्छ erret;
	}
	    // Can't change an existing function if it is locked
	अण्णहा जइ (मानम्_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    गच्छ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, परिमाण(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	जइ (name == रिक्त)
	    गच्छ erret;
    }
    अण्णहा जइ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	पूर्णाङ्क		var_conflict = FALSE;
	पूर्णाङ्क		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	जइ (v != रिक्त && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	जइ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    जइ (si->sn_autoload_prefix != रिक्त)
	    {
		जइ (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    जइ (v != रिक्त)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		अण्णहा
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    जइ (prefixed != रिक्त && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			जइ (v != रिक्त)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	जइ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    गच्छ erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	जइ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == रिक्त ? name : uname, 0, FALSE);
	}

	जइ (fp != रिक्त || import != रिक्त)
	{
	    पूर्णाङ्क dead = fp != रिक्त && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    जइ (import != रिक्त
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_शिखरम्;
		जइ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		अण्णहा
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		गच्छ errret_keep;
	    }
	    जइ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		गच्छ errret_keep;
	    }
	    जइ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = रिक्त;
		overwrite = TRUE;
	    }
	    अण्णहा
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = रिक्त;
		func_clear_वस्तूनि(fp);
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

    जइ (fp == रिक्त)
    {
	जइ (fudi.fd_dict == रिक्त && vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    पूर्णाङ्क	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    जइ (SOURCING_NAME != रिक्त)
	    {
		scriptname = autoload_name(name);
		जइ (scriptname != रिक्त)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (पूर्णाङ्क)STRLEN(p);
		    slen = (पूर्णाङ्क)STRLEN(SOURCING_NAME);
		    जइ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    जइ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_शिखरम्;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		गच्छ erret;
	    }
	}

	जइ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	जइ (fp == रिक्त)
	    गच्छ erret;
	fp_allocated = TRUE;

	जइ (fudi.fd_dict != रिक्त)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    जइ (func_name == रिक्त)
	    {
		VIM_CLEAR(fp);
		गच्छ erret;
	    }
	    जइ (fudi.fd_di == रिक्त)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		जइ (fudi.fd_di == रिक्त)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    गच्छ erret;
		}
		जइ (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    गच्छ erret;
		}
	    }
	    अण्णहा
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

    जइ (eap->cmdidx == CMD_def)
    {
	पूर्णाङ्क	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	जइ (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_शिखरम्;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	पूर्णाङ्क save_is_export = is_export;
	is_export = FALSE;

	जइ (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_गणनम्, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    गच्छ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	जइ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    गच्छ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    अण्णहा
	fp->uf_def_status = UF_NOT_COMPILED;

    जइ (fp_allocated)
    {
	// insert the new function in the function list
	जइ (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_कुञ्चिका = UF2HIKEY(fp);
	}
	अण्णहा जइ (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    गच्छ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_पङ्क्तयः = newlines;
    newlines.ga_data = रिक्त;
    जइ ((flags & FC_CLOSURE) != 0)
    {
	जइ (register_closure(fp) == FAIL)
	    गच्छ erret;
    }
    अण्णहा
	fp->uf_scoped = रिक्त;

#ifdef FEAT_PROFILE
    जइ (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    जइ (sandbox)
	flags |= FC_SANDBOX;
    जइ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_शिखरम्;
    जइ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    जइ (eap->cmdidx == CMD_def)
	set_function_type(fp);
    अण्णहा जइ (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    जइ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, रिक्त);

    गच्छ ret_free;

erret:
    जइ (fp != रिक्त)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    जइ (fp != रिक्त)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	जइ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_वस्तूनि(fp);
    }
    जइ (free_fp)
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
    जइ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    प्रत्यागम fp;
}

/*
 * ":function"
 */
    शून्य
ex_function(exarg_T *eap)
{
    garray_T पङ्क्तयः_to_free;

    ga_init2(&पङ्क्तयः_to_free, परिमाण(char_u *), 50);
    (शून्य)define_function(eap, रिक्त, &पङ्क्तयः_to_free, 0, रिक्त, 0, रिक्त);
    ga_clear_strings(&पङ्क्तयः_to_free);
}

    पूर्णाङ्क
get_func_arity(char_u *name, पूर्णाङ्क *required, पूर्णाङ्क *optional, पूर्णाङ्क *varargs)
{
    ufunc_T	*ufunc = रिक्त;
    पूर्णाङ्क		argcount = 0;
    पूर्णाङ्क		min_argcount = 0;
    पूर्णाङ्क		idx;

    idx = find_internal_func(name);
    जइ (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    अण्णहा
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = रिक्त;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	जइ (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	जइ (ufunc == रिक्त)
	    प्रत्यागम FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    प्रत्यागम OK;
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
    पूर्णाङ्क		is_global = FALSE;
    char_u	*bracket_start = रिक्त;
    char_u	*generic_func_name = name;

    जइ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    अण्णहा जइ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    जइ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(शून्य)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    अण्णहा
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	जइ (ufunc != रिक्त)
	    प्रत्यागम ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      रिक्त, रिक्त, रिक्त, &ufunc);
	जइ (ufunc != रिक्त)
	{
	    vim_free(fname);
	    प्रत्यागम ufunc;
	}
	जइ (fname != रिक्त && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    जइ (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		प्रत्यागम रिक्त;
	    }
	}
    }
    जइ (fname == रिक्त)
    {
	semsg(_(e_invalid_argument_str), name);
	प्रत्यागम रिक्त;
    }
    जइ (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	प्रत्यागम रिक्त;
    }

    ufunc = find_func(fname, is_global);
    जइ (ufunc == रिक्त)
    {
	char_u *p = untrans_function_name(fname);

	जइ (p != रिक्त)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	जइ (ufunc == रिक्त)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    जइ (bracket_start != रिक्त)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    जइ (ufunc != रिक्त)
    {
	// handle generic functions
	जइ (bracket_start == रिक्त)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    जइ (ufunc == रिक्त)
	semsg(_(e_cannot_find_function_str), name);
    प्रत्यागम ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    शून्य
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    जइ (func_needs_compiling(ufunc, compile_type))
	(शून्य)compile_def_function(ufunc, FALSE, compile_type, रिक्त);
    अण्णहा
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
    long	todo = (long)func_hashtab.ht_used;
    पूर्णाङ्क		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    प्रति (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    जइ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(शून्य)compile_def_function(ufunc, FALSE, CT_NONE, रिक्त);

		जइ (func_hashtab.ht_changed != changed)
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
    शून्य
ex_defcompile(exarg_T *eap)
{
    जइ (*eap->arg != NUL)
    {
	typval_T tv;

	जइ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    जइ (cl != रिक्त)
		defcompile_class(cl);
	}
	अण्णहा
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    जइ (ufunc != रिक्त)
		defcompile_function(ufunc, रिक्त);
	}
    }
    अण्णहा
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
    पूर्णाङ्क
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    जइ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	प्रत्यागम 5;
    जइ (p[0] == 's' && p[1] == ':')
	प्रत्यागम 2;
    प्रत्यागम 0;
}

    पूर्णाङ्क
translated_function_exists(char_u *name, पूर्णाङ्क is_global)
{
    जइ (builtin_function(name, -1))
	प्रत्यागम has_internal_func(name);
    प्रत्यागम find_func(name, is_global) != रिक्त;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    पूर्णाङ्क
has_varargs(ufunc_T *ufunc)
{
    प्रत्यागम ufunc->uf_varargs || ufunc->uf_va_name != रिक्त;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    पूर्णाङ्क
function_exists(char_u *name, पूर्णाङ्क no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    पूर्णाङ्क	    n = FALSE;
    पूर्णाङ्क	    flag;
    पूर्णाङ्क	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    जइ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    जइ (p != रिक्त && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    प्रत्यागम n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    पूर्णाङ्क
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    पूर्णाङ्क		is_global = FALSE;
    ufunc_T	*fp = रिक्त;
    list_T	*args;
    पूर्णाङ्क		is_def;
    वर्ण	*tofree;

    जइ (*name == NUL)
	प्रत्यागम FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    जइ (p != रिक्त && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    जइ (fp == रिक्त)
	प्रत्यागम FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    जइ (args == रिक्त || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	प्रत्यागम FAIL;
    }
    प्रति (पूर्णाङ्क j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	जइ (arg == रिक्त || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    प्रत्यागम FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	जइ (fp->uf_arg_types == रिक्त)
	    dict_add_string(arg, "type", (char_u *)"any");
	अण्णहा
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	जइ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    जइ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	जइ (va == रिक्त || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    प्रत्यागम FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == रिक्त ? (char_u *)"" : fp->uf_va_name);
	जइ (fp->uf_va_type == रिक्त)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	अण्णहा
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    जइ (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    अण्णहा
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_सङ्ख्या(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_सङ्ख्या(d, "lnum", fp->uf_script_ctx.sc_lnum);
    प्रत्यागम OK;
}

#जइ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, पूर्णाङ्क check)
{
    char_u	*nm = name;
    char_u	*p;
    पूर्णाङ्क		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    जइ (p != रिक्त && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	प्रत्यागम p;

    vim_free(p);
    प्रत्यागम रिक्त;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, पूर्णाङ्क idx)
{
    स्थिर long_u	done;
    स्थिर पूर्णाङ्क		changed;
    स्थिर hashitem_T	*hi;
    ufunc_T		*fp;

    जइ (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    जइ (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	पूर्णाङ्क len;

	जइ (done++ > 0)
	    ++hi;
	जाव (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	जइ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    प्रत्यागम (char_u *)"";

	जइ (fp->uf_namelen + 4 >= IOSIZE)
	    प्रत्यागम fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	जइ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    जइ (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	प्रत्यागम IObuff;
    }
    प्रत्यागम रिक्त;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, पूर्णाङ्क extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    जइ (ufunc == रिक्त)
	प्रत्यागम रिक्त;

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
    ufunc->uf_tml_गणनम् = रिक्त;
    ufunc->uf_tml_total = रिक्त;
    ufunc->uf_tml_self = रिक्त;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    जइ (ufunc->uf_arg_types != रिक्त)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	जइ (at != रिक्त)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     परिमाण(type_T *) * ufunc->uf_args.ga_len);
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

    जइ (ufunc->uf_va_name != रिक्त)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = रिक्त;

    ga_copy_strings(&fp->uf_पङ्क्तयः, &ufunc->uf_पङ्क्तयः);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    जइ (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    प्रत्यागम ufunc;
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
    पूर्णाङ्क		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     रिक्त, रिक्त, रिक्त);
    vim_free(fudi.fd_newkey);
    जइ (name == रिक्त)
    {
	जइ (fudi.fd_dict != रिक्त && !eap->skip)
	    emsg(_(e_funcref_required));
	प्रत्यागम;
    }
    जइ (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	प्रत्यागम;
    }
    set_nextcmd(eap, p);
    जइ (eap->nextcmd != रिक्त)
	*p = NUL;

    जइ (numbered_function(name) && fudi.fd_dict == रिक्त)
    {
	जइ (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	प्रत्यागम;
    }
    जइ (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    जइ (!eap->skip)
    {
	जइ (fp == रिक्त)
	{
	    जइ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    प्रत्यागम;
	}
	जइ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    प्रत्यागम;
	}
	जइ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    प्रत्यागम;
	}

	जइ (fudi.fd_dict != रिक्त)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	अण्णहा
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    जइ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		जइ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    अण्णहा
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

    जइ (name == रिक्त || !func_name_refcount(name))
	प्रत्यागम;
    fp = find_func(name, FALSE);
    जइ (fp == रिक्त && numbered_function(name))
    {
#ifdef EXITFREE
	जइ (!entered_free_all_mem)
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
    जइ (fp != रिक्त && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != रिक्त
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	जइ (fp->uf_calls == 0)
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

    जइ (name == रिक्त || !func_name_refcount(name))
	प्रत्यागम;
    fp = find_func(name, FALSE);
    जइ (fp != रिक्त)
	++fp->uf_refcount;
    अण्णहा जइ (numbered_function(name))
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
    जइ (fp != रिक्त)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    स्थिर पूर्णाङ्क
can_free_funccal(funccall_T *fc, पूर्णाङ्क copyID)
{
    प्रत्यागम (fc->fc_l_varlist.lv_copyID != copyID
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
    पूर्णाङ्क		returning = FALSE;
    evalarg_T	evalarg;

    जइ (current_funccal == रिक्त)
    {
	emsg(_(e_return_not_inside_function));
	प्रत्यागम;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    जइ (eap->skip)
	++emsg_skip;

    eap->nextcmd = रिक्त;
    जइ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	जइ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	अण्णहा
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    अण्णहा जइ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	जइ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, रिक्त);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    जइ (returning)
	eap->nextcmd = रिक्त;
    अण्णहा जइ (eap->nextcmd == रिक्त)	    // no argument
	set_nextcmd(eap, arg);

    जइ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    स्थिर पूर्णाङ्क
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    पूर्णाङ्क		doesrange;
    typval_T	rettv;
    पूर्णाङ्क		failed = FALSE;

    lnum = eap->line1;
    प्रति ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	जइ (eap->addr_गणनम् > 0)
	{
	    जइ (lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		भङ्ग;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	जइ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    भङ्ग;
	}
	जइ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	जइ (handle_subscript(arg, रिक्त, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    भङ्ग;
	}

	clear_tv(&rettv);
	जइ (doesrange)
	    भङ्ग;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	जइ (aborting())
	    भङ्ग;
    }
    प्रत्यागम failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    स्थिर पूर्णाङ्क
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    पूर्णाङ्क		partial_argc = 0;		// number of partial arguments
    पूर्णाङ्क		argcount = 0;			// number of arguments found
    पूर्णाङ्क		r;

    जइ (current_funccal == रिक्त)
    {
	semsg(_(e_str_not_inside_function), "defer");
	प्रत्यागम FAIL;
    }
    जइ (partial != रिक्त)
    {
	जइ (partial->pt_dict != रिक्त)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    प्रत्यागम FAIL;
	}
	जइ (partial->pt_argc > 0)
	{
	    पूर्णाङ्क i;

	    partial_argc = partial->pt_argc;
	    प्रति (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    पूर्णाङ्क is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    जइ (r == OK)
    {
	जइ (type != रिक्त)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, रिक्त, name);
	}
	अण्णहा जइ (is_builtin)
	{
	    पूर्णाङ्क idx = find_internal_func(name);

	    जइ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    अण्णहा जइ (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	अण्णहा
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    जइ (ufunc != रिक्त)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		जइ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    जइ (r == FAIL)
    {
	जाव (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	प्रत्यागम FAIL;
    }
    प्रत्यागम add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    पूर्णाङ्क
can_add_defer(शून्य)
{
    जइ (!in_def_function() && get_current_funccal() == रिक्त)
    {
	semsg(_(e_str_not_inside_function), "defer");
	प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
add_defer(char_u *name, पूर्णाङ्क argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    पूर्णाङ्क		argcount = argcount_arg;
    defer_T	*dr;
    पूर्णाङ्क		ret = FAIL;

    जइ (saved_name == रिक्त)
	गच्छ theend;
    जइ (in_def_function())
    {
	जइ (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    अण्णहा
    {
	जइ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, परिमाण(defer_T), 10);
	जइ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    गच्छ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	जाव (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    जाव (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    प्रत्यागम ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    स्थिर शून्य
handle_defer_one(funccall_T *funccal)
{
    पूर्णाङ्क	    idx;

    प्रति (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	जइ (dr->dr_name == रिक्त)
	    // already being called, can happen if function does ":qa"
	    अनुवर्त;

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
	प्रति (पूर्णाङ्क i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    स्थिर शून्य
invoke_funccall_defer(funccall_T *fc)
{
    जइ (fc->fc_ectx != रिक्त)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    अण्णहा
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
    प्रति (funccall_T *fc = current_funccal; fc != रिक्त; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    प्रति (funccal_entry_T *fce = funccal_राशिः; fce != रिक्त; fce = fce->next)
	प्रति (funccall_T *fc = fce->शिखरम्_funccal; fc != रिक्त; fc = fc->fc_caller)
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
    पूर्णाङ्क		len;
    पूर्णाङ्क		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = रिक्त;
    partial_T	*partial = रिक्त;
    evalarg_T	evalarg;
    type_T	*type = रिक्त;
    पूर्णाङ्क		found_var = FALSE;
    पूर्णाङ्क		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    जइ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	जइ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	प्रत्यागम;
    }

    tofree = trans_function_name_ext(&arg, रिक्त, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : रिक्त, &ufunc);
    जइ (fudi.fd_newkey != रिक्त)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    जइ (tofree == रिक्त)
	प्रत्यागम;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    जइ (fudi.fd_dict != रिक्त)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (पूर्णाङ्क)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != रिक्त ? रिक्त : &partial,
				vim9script && type == रिक्त ? &type : रिक्त,
						     FALSE, FALSE, &found_var);

    जइ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	जइ (name == रिक्त)
	    गच्छ end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    जइ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	गच्छ end;
    }
    जइ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	गच्छ end;
    }

    जइ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    अण्णहा
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
    जइ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	जइ (!ends_excmd2(eap->arg, arg))
	{
	    जइ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	अण्णहा
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
    पूर्णाङ्क
do_return(
    exarg_T	*eap,
    पूर्णाङ्क		reanimate,
    पूर्णाङ्क		is_cmd,
    शून्य	*rettv)
{
    पूर्णाङ्क		idx;
    cstack_T	*cstack = eap->cstack;

    जइ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    जइ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	जइ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	अण्णहा
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    जइ (reanimate)
		rettv = current_funccal->fc_rettv;

	    जइ (rettv != रिक्त)
	    {
		// Store the value of the pending return.
		जइ ((cstack->cs_rettv[idx] = alloc_tv()) != रिक्त)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		अण्णहा
		    emsg(_(e_out_of_memory));
	    }
	    अण्णहा
		cstack->cs_rettv[idx] = रिक्त;

	    जइ (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_सङ्ख्या;
		current_funccal->fc_rettv->vval.v_सङ्ख्या = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    अण्णहा
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	जइ (!reanimate && rettv != रिक्त)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    जइ (!is_cmd)
		vim_free(rettv);
	}
    }

    प्रत्यागम idx < 0;
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
    size_t	slen = 0;
    size_t	IObufflen;

    जइ (rettv != रिक्त)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    जइ (s == रिक्त)
	s = (char_u *)"";
    अण्णहा
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    जइ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    प्रत्यागम vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_पङ्क्तिः(
    पूर्णाङ्क	    c UNUSED,
    शून्य    *cookie,
    पूर्णाङ्क	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    जइ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    जइ (do_profiling == PROF_YES)
	func_पङ्क्तिः_end(cookie);
#endif

    gap = &fp->uf_पङ्क्तयः;
    जइ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = रिक्त;
    अण्णहा
    {
	// Skip NULL lines (continuation lines).
	जाव (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == रिक्त)
	    ++fcp->fc_linenr;
	जइ (fcp->fc_linenr >= gap->ga_len)
	    retval = रिक्त;
	अण्णहा
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    जइ (do_profiling == PROF_YES)
		func_पङ्क्तिः_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    जइ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    प्रत्यागम retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    पूर्णाङ्क
func_has_ended(शून्य *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    प्रत्यागम (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    पूर्णाङ्क
func_has_abort(
    शून्य    *cookie)
{
    प्रत्यागम ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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

    जइ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != रिक्त
				     && rettv->vval.v_partial->pt_func != रिक्त)
	fp = rettv->vval.v_partial->pt_func;
    अण्णहा
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == रिक्त ? रिक्त
					      : rettv->vval.v_partial->pt_name;
	जइ (fname == रिक्त)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = रिक्त;
	}
	अण्णहा
	{
	    char_u	*tofree = रिक्त;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    जइ (fp != रिक्त && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	जइ (pt != रिक्त)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = रिक्त;
	    जइ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    अण्णहा
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		पूर्णाङ्क		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		जइ (ret_pt->pt_name != रिक्त)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		अण्णहा
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		जइ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    जइ (pt->pt_argv == रिक्त)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    अण्णहा
		    {
			pt->pt_argc = ret_pt->pt_argc;
			प्रति (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    प्रत्यागम selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(शून्य *cookie)
{
    प्रत्यागम ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(शून्य *cookie)
{
    प्रत्यागम &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    पूर्णाङ्क *
func_dbg_tick(शून्य *cookie)
{
    प्रत्यागम &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    पूर्णाङ्क
func_level(शून्य *cookie)
{
    प्रत्यागम ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    पूर्णाङ्क
current_func_returned(शून्य)
{
    प्रत्यागम current_funccal->fc_returned;
}

    पूर्णाङ्क
free_unref_funccal(पूर्णाङ्क copyID, पूर्णाङ्क testing)
{
    पूर्णाङ्क		did_free = FALSE;
    पूर्णाङ्क		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    प्रति (pfc = &previous_funccal; *pfc != रिक्त; )
    {
	जइ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	अण्णहा
	    pfc = &(*pfc)->fc_caller;
    }
    जइ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(शून्य)garbage_collect(testing);

    प्रत्यागम did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    स्थिर funccall_T *
get_funccal(शून्य)
{
    पूर्णाङ्क		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    जइ (debug_backtrace_level > 0)
    {
	प्रति (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    जइ (temp_funccal)
		funccal = temp_funccal;
	    अण्णहा
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    प्रत्यागम funccal;
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

    जइ (current_funccal == रिक्त)
	प्रत्यागम रिक्त;
    funccal = get_funccal();
    जइ (funccal == रिक्त || funccal->fc_l_vars.dv_refcount == 0)
	प्रत्यागम रिक्त;
    प्रत्यागम funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    प्रत्यागम funccal == रिक्त ? रिक्त : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    प्रत्यागम funccal == रिक्त ? रिक्त : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    प्रत्यागम funccal == रिक्त ? रिक्त : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(शून्य)
{
    funccall_T	*funccal = get_funccal_for_vars();

    प्रत्यागम funccal == रिक्त ? रिक्त : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    शून्य
list_func_vars(पूर्णाङ्क *first)
{
    जइ (current_funccal != रिक्त && current_funccal->fc_l_vars.dv_refcount > 0)
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
    जइ (current_funccal != रिक्त
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	प्रत्यागम &current_funccal->fc_l_vars;
    प्रत्यागम रिक्त;
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

    जइ (current_funccal == रिक्त || current_funccal->fc_func->uf_scoped == रिक्त)
	प्रत्यागम रिक्त;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    जाव (current_funccal != रिक्त)
    {
	ht = find_var_ht(name, &varname);
	जइ (ht != रिक्त && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    जइ (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		भङ्ग;
	    }
	}
	जइ (current_funccal == current_funccal->fc_func->uf_scoped)
	    भङ्ग;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    प्रत्यागम hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, पूर्णाङ्क no_autoload)
{
    dictitem_T	*v = रिक्त;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    जइ (current_funccal == रिक्त || current_funccal->fc_func->uf_scoped == रिक्त)
	प्रत्यागम रिक्त;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    जाव (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	जइ (ht != रिक्त && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    जइ (v != रिक्त)
		भङ्ग;
	}
	जइ (current_funccal == current_funccal->fc_func->uf_scoped)
	    भङ्ग;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    प्रत्यागम v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    पूर्णाङ्क
set_ref_in_previous_funccal(पूर्णाङ्क copyID)
{
    funccall_T	*fc;

    प्रति (fc = previous_funccal; fc != रिक्त; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	जइ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, रिक्त, रिक्त)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, रिक्त, रिक्त)
		|| set_ref_in_list_वस्तूनि(&fc->fc_l_varlist, copyID + 1, रिक्त, रिक्त))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

    स्थिर पूर्णाङ्क
set_ref_in_funccal(funccall_T *fc, पूर्णाङ्क copyID)
{
    जइ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	जइ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, रिक्त, रिक्त)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, रिक्त, रिक्त)
		|| set_ref_in_list_वस्तूनि(&fc->fc_l_varlist, copyID, रिक्त, रिक्त)
		|| set_ref_in_func(रिक्त, fc->fc_func, copyID))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    पूर्णाङ्क
set_ref_in_call_राशिः(पूर्णाङ्क copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    प्रति (fc = current_funccal; fc != रिक्त; fc = fc->fc_caller)
	जइ (set_ref_in_funccal(fc, copyID))
	    प्रत्यागम TRUE;

    // Also go through the funccal_stack.
    प्रति (entry = funccal_राशिः; entry != रिक्त; entry = entry->next)
	प्रति (fc = entry->शिखरम्_funccal; fc != रिक्त; fc = fc->fc_caller)
	    जइ (set_ref_in_funccal(fc, copyID))
		प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    पूर्णाङ्क
set_ref_in_functions(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi = रिक्त;
    ufunc_T	*fp;

    todo = (पूर्णाङ्क)func_hashtab.ht_used;
    प्रति (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    जइ (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(रिक्त, fp, copyID))
		प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    पूर्णाङ्क
set_ref_in_func_args(पूर्णाङ्क copyID)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < funcargs.ga_len; ++i)
	जइ (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, रिक्त, रिक्त, रिक्त))
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    पूर्णाङ्क
set_ref_in_func(char_u *name, ufunc_T *fp_in, पूर्णाङ्क copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*fname;
    पूर्णाङ्क		abort = FALSE;

    जइ (name == रिक्त && fp_in == रिक्त)
	प्रत्यागम FALSE;

    जइ (fp_in == रिक्त)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    जइ (fp != रिक्त)
    {
	प्रति (fc = fp->uf_scoped; fc != रिक्त; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    प्रत्यागम abort;
}

#endif // FEAT_EVAL
