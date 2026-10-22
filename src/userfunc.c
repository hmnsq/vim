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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)
/*
 * All user-defined functions are found in this hashtable.
 */
ಸ್ಥಿರ hashtab_T	func_hashtab;

// Used by get_func_tv()
ಸ್ಥಿರ garray_T funcargs = GA_EMPTY;

// pointer to funccal for currently active function
ಸ್ಥಿರ funccall_T *current_funccal = ರಿಕ್ತ;

// Pointer to list of previously used funccal, still around because some
// item in it is still being used.
ಸ್ಥಿರ funccall_T *previous_funccal = ರಿಕ್ತ;

ಸ್ಥಿರ ಶೂನ್ಯ funccal_unref(funccall_T *fc, ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force);
ಸ್ಥಿರ ಶೂನ್ಯ func_clear(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ func_free(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force);
ಸ್ಥಿರ char_u *untrans_function_name(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ handle_defer_one(funccall_T *funccal);
ಸ್ಥಿರ char_u *trans_function_name_ext(char_u **pp, ಪೂರ್ಣಾಙ್ಕ *is_global, ಪೂರ್ಣಾಙ್ಕ skip, ಪೂರ್ಣಾಙ್ಕ flags, funcdict_T *fdp, partial_T **partial, type_T **type, ufunc_T **ufunc);

    ಶೂನ್ಯ
func_init(ಶೂನ್ಯ)
{
    hash_init(&func_hashtab);
}

/*
 * Return the function hash table
 */
    hashtab_T *
func_tbl_get(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ &func_hashtab;
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
    ಸ್ಥಿರ char_u *
one_function_arg(
	char_u	    *arg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ಪೂರ್ಣಾಙ್ಕ	    types_optional,
	garray_T    *arg_objm,
	evalarg_T   *evalarg,
	exarg_T	    *eap,
	ಪೂರ್ಣಾಙ್ಕ	    is_vararg,
	ಪೂರ್ಣಾಙ್ಕ	    skip)
{
    char_u	*p = arg;
    char_u	*arg_copy = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		is_underscore = FALSE;

    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '_')
	++p;
    ಯದಿ (arg == p || SAFE_isdigit(*arg)
	    || (argtypes == ರಿಕ್ತ
		&& ((p - arg == 9 && STRNCMP(arg, "firstline", 9) == 0)
		    || (p - arg == 8 && STRNCMP(arg, "lastline", 8) == 0))))
    {
	ಯದಿ (!skip)
	    semsg(_(e_illegal_argument_str), arg);
	ಪ್ರತ್ಯಾಗಮ arg;
    }

    // Extra checks in Vim9 script.
    ಯದಿ (!skip && argtypes != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ c = *p;
	*p = NUL;
	ಪೂರ್ಣಾಙ್ಕ r = check_reserved_name(arg, FALSE);
	*p = c;
	ಯದಿ (r == FAIL)
	    ಪ್ರತ್ಯಾಗಮ arg;

	// Cannot use script var name for argument. In function: also check
	// local vars and arguments.
	ಯದಿ (check_defined(arg, p - arg,
				   evalarg == ರಿಕ್ತ ? ರಿಕ್ತ : evalarg->eval_cctx,
			       eap == ರಿಕ್ತ ? ರಿಕ್ತ : eap->cstack, TRUE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ arg;
    }

    ಯದಿ (newargs != ರಿಕ್ತ && ga_grow(newargs, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ arg;
    ಯದಿ (newargs != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	c;
	ಪೂರ್ಣಾಙ್ಕ	i;

	c = *p;
	*p = NUL;
	arg_copy = vim_strsave(arg);
	ಯದಿ (arg_copy == ರಿಕ್ತ)
	{
	    *p = c;
	    ಪ್ರತ್ಯಾಗಮ arg;
	}
	is_underscore = arg_copy[0] == '_' && arg_copy[1] == NUL;
	ಯದಿ (argtypes == ರಿಕ್ತ || !is_underscore)
	    // Check for duplicate argument name.
	    ಪ್ರತಿ (i = 0; i < newargs->ga_len; ++i)
		ಯದಿ (STRCMP(((char_u **)(newargs->ga_data))[i], arg_copy) == 0)
		{
		    semsg(_(e_duplicate_argument_name_str), arg_copy);
		    vim_free(arg_copy);
		    ಪ್ರತ್ಯಾಗಮ arg;
		}
	((char_u **)(newargs->ga_data))[newargs->ga_len] = arg_copy;
	newargs->ga_len++;

	*p = c;
    }

    // get any type from "arg: type"
    ಯದಿ (argtypes != ರಿಕ್ತ && (skip || ga_grow(argtypes, 1) == OK)
		&& arg_objm != ರಿಕ್ತ && (skip || ga_grow(arg_objm, 1) == OK))
    {
	char_u *type = ರಿಕ್ತ;

	ಯದಿ (VIM_ISWHITE(*p) && *skipwhite(p) == ':')
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str),
					    arg_copy == ರಿಕ್ತ ? arg : arg_copy);
	    p = skipwhite(p);
	}
	ಯದಿ (*p == ':')
	{
	    ++p;
	    ಯದಿ (!skip && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ":", p - 1);
		ಪ್ರತ್ಯಾಗಮ arg;
	    }
	    type = skipwhite(p);
	    p = skip_type(type, TRUE);
	    ಯದಿ (!skip)
		type = vim_strnsave(type, p - type);
	}
	ಅನ್ಯಥಾ ಯದಿ (*skipwhite(p) != '=' && !types_optional && !is_underscore)
	{
	    semsg(_(e_missing_argument_type_for_str),
					    arg_copy == ರಿಕ್ತ ? arg : arg_copy);
	    ಪ್ರತ್ಯಾಗಮ arg;
	}
	ಯದಿ (!skip)
	{
	    ಯದಿ (type == ರಿಕ್ತ && types_optional)
	    {
		// lambda arguments default to "any" type
		ಯದಿ (is_vararg)
		    type = vim_strnsave((char_u *)"list<any>", 9);
		ಅನ್ಯಥಾ
		    type = vim_strnsave((char_u *)"any", 3);
	    }
	    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] = type;
	    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = FALSE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Handle line continuation in function arguments or body.
 * Get a next line, store it in "eap" if appropriate and put the line in
 * "lines_to_free" to free the line later.
 */
    ಸ್ಥಿರ char_u *
get_function_ಸಾಲು(
	exarg_T		*eap,
	garray_T	*ಸಾಲುಗಳು_to_free,
	ಪೂರ್ಣಾಙ್ಕ		indent,
	getline_opt_T	getline_options)
{
    char_u *theline;

    ಯದಿ (eap->ea_getline == ರಿಕ್ತ)
	theline = getcmdline(':', 0L, indent, 0);
    ಅನ್ಯಥಾ
	theline = eap->ea_getline(':', eap->cookie, indent, getline_options);
    ಯದಿ (theline != ರಿಕ್ತ)
    {
	ಯದಿ (ಸಾಲುಗಳು_to_free->ga_len > 0
		&& eap->cmdlinep != ರಿಕ್ತ
		&& *eap->cmdlinep == ((char_u **)ಸಾಲುಗಳು_to_free->ga_data)
						   [ಸಾಲುಗಳು_to_free->ga_len - 1])
	    *eap->cmdlinep = theline;
	(ಶೂನ್ಯ)ga_add_string(ಸಾಲುಗಳು_to_free, theline);
    }

    ಪ್ರತ್ಯಾಗಮ theline;
}

/*
 * Get function arguments.
 * "argp" should point to just after the "(", possibly to white space.
 * "argp" is advanced just after "endchar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_function_args(
    char_u	**argp,
    char_u	endchar,
    garray_T	*newargs,
    garray_T	*argtypes,	// NULL unless using :def
    ಪೂರ್ಣಾಙ್ಕ		types_optional,	// types optional if "argtypes" is not NULL
    garray_T	*arg_objm,	// NULL unless using :def
    evalarg_T	*evalarg,	// context or NULL
    ಪೂರ್ಣಾಙ್ಕ		*varargs,
    garray_T	*default_args,
    ಪೂರ್ಣಾಙ್ಕ		skip,
    exarg_T	*eap,		// can be NULL
    ಪೂರ್ಣಾಙ್ಕ		in_class,	// non-zero when inside a class or interface
    garray_T	*newlines,	// function body lines
    garray_T	*ಸಾಲುಗಳು_to_free)
{
    ಪೂರ್ಣಾಙ್ಕ		mustend = FALSE;
    char_u	*arg;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		any_default = FALSE;
    char_u	*whitep = *argp;
    ಪೂರ್ಣಾಙ್ಕ		need_ಅಭಿವ್ಯಕ್ತಿ = FALSE;

    ಯದಿ (newargs != ರಿಕ್ತ)
	ga_init2(newargs, ಪರಿಮಾಣ(char_u *), 3);
    ಯದಿ (argtypes != ರಿಕ್ತ)
	ga_init2(argtypes, ಪರಿಮಾಣ(char_u *), 3);
    ಯದಿ (arg_objm != ರಿಕ್ತ)
	ga_init2(arg_objm, ಪರಿಮಾಣ(int8_T), 3);
    ಯದಿ (!skip && default_args != ರಿಕ್ತ)
	ga_init2(default_args, ಪರಿಮಾಣ(char_u *), 3);

    ಯದಿ (varargs != ರಿಕ್ತ)
	*varargs = FALSE;

    /*
     * Isolate the arguments: "arg1, arg2, ...)"
     */
    arg = skipwhite(*argp);
    p = arg;
    ಯಾವತ್ (*p != endchar)
    {
	ಯಾವತ್ (eap != ರಿಕ್ತ && eap->ea_getline != ರಿಕ್ತ
			 && (*p == NUL || (VIM_ISWHITE(*whitep) && *p == '#')))
	{
	    // End of the line, get the next one.
	    char_u *theline = get_function_ಸಾಲು(eap, ಸಾಲುಗಳು_to_free, 0,
							  GETLINE_CONCAT_CONT);

	    ಯದಿ (theline == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    whitep = (char_u *)" ";
	    p = skipwhite(theline);
	}

	ಯದಿ (mustend && *p != endchar)
	{
	    ಯದಿ (!skip)
		semsg(_(e_invalid_argument_str), *argp);
	    ಗಚ್ಛ err_ret;
	}
	ಯದಿ (*p == endchar && !need_ಅಭಿವ್ಯಕ್ತಿ)
	    ಭಙ್ಗ;

	ಯದಿ (p[0] == '.' && p[1] == '.' && p[2] == '.')
	{
	    ಯದಿ (varargs != ರಿಕ್ತ)
		*varargs = TRUE;
	    p += 3;
	    mustend = TRUE;

	    ಯದಿ (argtypes != ರಿಕ್ತ)
	    {
		// ...name: list<type>
		ಯದಿ (!eval_isnamec1(*p))
		{
		    ಯದಿ (!skip)
			emsg(_(e_missing_name_after_dots));
		    ಗಚ್ಛ err_ret;
		}

		arg = p;
		p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, TRUE, skip);
		ಯದಿ (p == arg)
		    ಭಙ್ಗ;
		ಯದಿ (*skipwhite(p) == '=')
		{
		    emsg(_(e_cannot_use_default_for_variable_arguments));
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (in_class && STRNCMP(p, "this.", 5) == 0)
	{
	    // this.memberName
	    p += 5;
	    arg = p;
	    ಯಾವತ್ (ASCII_ISALNUM(*p) || *p == '_')
		++p;
	    char_u *argend = p;

	    // object variable this. can be used only in a constructor
	    ಯದಿ (STRNCMP(eap->arg, "new", 3) != 0)
	    {
		c = *argend;
		*argend = NUL;
		semsg(_(e_cannot_use_an_object_variable_except_with_the_new_method_str), arg);
		*argend = c;
		ಭಙ್ಗ;
	    }

	    ಯದಿ (*skipwhite(p) == '=')
	    {
		char_u *defval = skipwhite(skipwhite(p) + 1);
		ಯದಿ (STRNCMP(defval, "v:none", 6) != 0)
		{
		    semsg(_(e_constructor_default_ಮೌಲ್ಯ_must_be_vnone_str), p);
		    ಗಚ್ಛ err_ret;
		}
		any_default = TRUE;
		p = defval + 6;

		ಯದಿ (ga_grow(default_args, 1) == FAIL)
		    ಗಚ್ಛ err_ret;

		char_u *ಅಭಿವ್ಯಕ್ತಿ = vim_strnsave((char_u *)"v:none", 6);
		ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
		    ಗಚ್ಛ err_ret;
		((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = ಅಭಿವ್ಯಕ್ತಿ;
		default_args->ga_len++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		ಗಚ್ಛ err_ret;
	    }

	    // TODO: check the argument is indeed a member
	    ಯದಿ (newargs != ರಿಕ್ತ && ga_grow(newargs, 1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (newargs != ರಿಕ್ತ)
	    {
		((char_u **)(newargs->ga_data))[newargs->ga_len] =
					       vim_strnsave(arg, argend - arg);
		newargs->ga_len++;

		ಯದಿ (argtypes != ರಿಕ್ತ && ga_grow(argtypes, 1) == OK
			    && arg_objm != ರಿಕ್ತ && ga_grow(arg_objm, 1) == OK)
		{
		    // TODO: use the actual type
		    ((char_u **)argtypes->ga_data)[argtypes->ga_len++] =
						  vim_strnsave((char_u *)"any", 3);
		    ((int8_T *)arg_objm->ga_data)[arg_objm->ga_len++] = TRUE;

		    // Add a line to the function body for the assignment.
		    ಯದಿ (ga_grow(newlines, 1) == OK)
		    {
			// "this.name = name"
			ಪೂರ್ಣಾಙ್ಕ len = 5 + (argend - arg) + 3 + (argend - arg) + 1;
			ಯದಿ (any_default)
			    len += 14 + 10;
			char_u *assignment = alloc(len);
			ಯದಿ (assignment != ರಿಕ್ತ)
			{
			    c = *argend;
			    *argend = NUL;
			    ಯದಿ (any_default)
				vim_snprintf((ವರ್ಣ *)assignment, len,
						"ifargisset %d this.%s = %s",
					   default_args->ga_len - 1, arg, arg);
			    ಅನ್ಯಥಾ
				vim_snprintf((ವರ್ಣ *)assignment, len,
						     "this.%s = %s", arg, arg);
			    *argend = c;
			    ((char_u **)(newlines->ga_data))[
					      newlines->ga_len++] = assignment;
			}
		    }
		}
	    }
	    ಯದಿ (*p == ',')
		++p;
	}
	ಅನ್ಯಥಾ
	{
	    char_u *np;

	    arg = p;
	    p = one_function_arg(p, newargs, argtypes, types_optional,
					arg_objm, evalarg, eap, FALSE, skip);
	    ಯದಿ (p == arg)
		ಭಙ್ಗ;

	    // Recognize " = expr" but not " == expr".  A lambda can have
	    // "(a = expr" but "(a == expr" and "(a =~ expr" are not a lambda.
	    np = skipwhite(p);
	    ಯದಿ (*np == '=' && np[1] != '=' && np[1] != '~'
						       && default_args != ರಿಕ್ತ)
	    {
		typval_T	rettv;

		// find the end of the expression (doesn't evaluate it)
		any_default = TRUE;
		p = skipwhite(np + 1);
		char_u *ಅಭಿವ್ಯಕ್ತಿ = p;
		ಯದಿ (eval1(&p, &rettv, ರಿಕ್ತ) != FAIL)
		{
		    ಯದಿ (!skip)
		    {
			ಯದಿ (ga_grow(default_args, 1) == FAIL)
			    ಗಚ್ಛ err_ret;

			ಯದಿ (need_ಅಭಿವ್ಯಕ್ತಿ)
			    need_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
			// trim trailing whitespace
			ಯಾವತ್ (p > ಅಭಿವ್ಯಕ್ತಿ && VIM_ISWHITE(p[-1]))
			    p--;
			c = *p;
			*p = NUL;
			ಅಭಿವ್ಯಕ್ತಿ = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ);
			ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
			{
			    *p = c;
			    ಗಚ್ಛ err_ret;
			}
			((char_u **)(default_args->ga_data))
						 [default_args->ga_len] = ಅಭಿವ್ಯಕ್ತಿ;
			default_args->ga_len++;
			*p = c;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    mustend = TRUE;
		    ಯದಿ (*skipwhite(p) == NUL)
			need_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (any_default)
	    {
		emsg(_(e_non_default_argument_follows_default_argument));
		ಗಚ್ಛ err_ret;
	    }

	    ಯದಿ (VIM_ISWHITE(*p) && *skipwhite(p) == ',')
	    {
		// Be tolerant when skipping
		ಯದಿ (!skip)
		{
		    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		    ಗಚ್ಛ err_ret;
		}
		p = skipwhite(p);
	    }
	    ಯದಿ (*p == ',')
	    {
		++p;
		// Don't give this error when skipping, it makes the "->" not
		// found in "{k,v -> x}" and give a confusing error.
		// Allow missing space after comma in legacy functions.
		ಯದಿ (!skip && argtypes != ರಿಕ್ತ
				      && !IS_WHITE_OR_NUL(*p) && *p != endchar)
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ಗಚ್ಛ err_ret;
		}
	    }
	    ಅನ್ಯಥಾ
		mustend = TRUE;
	}
	whitep = p;
	p = skipwhite(p);
    }

    ಯದಿ (*p != endchar)
	ಗಚ್ಛ err_ret;
    ++p;	// skip "endchar"

    *argp = p;
    ಪ್ರತ್ಯಾಗಮ OK;

err_ret:
    ಯದಿ (newargs != ರಿಕ್ತ)
	ga_clear_strings(newargs);
    ಯದಿ (!skip && default_args != ರಿಕ್ತ)
	ga_clear_strings(default_args);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Parse the argument types, filling "fp->uf_arg_types".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_argument_types(
    ufunc_T	*fp,
    garray_T	*argtypes,
    ಪೂರ್ಣಾಙ್ಕ		varargs,
    garray_T	*arg_objm,
    ocmember_T	*obj_members,
    ಪೂರ್ಣಾಙ್ಕ		obj_member_ಎಣಿಕೆ,
    cctx_T	*cctx)
{
    ಪೂರ್ಣಾಙ್ಕ len = 0;

    ga_init2(&fp->uf_type_list, ಪರಿಮಾಣ(type_T *), 10);
    ಯದಿ (argtypes->ga_len > 0)
    {
	// When "varargs" is set the last name/type goes into uf_va_name
	// and uf_va_type.
	len = argtypes->ga_len - (varargs ? 1 : 0);

	ಯದಿ (len > 0)
	    fp->uf_arg_types = ALLOC_CLEAR_MULT(type_T *, len);
	ಯದಿ (fp->uf_arg_types != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	i;
	    type_T	*type;

	    ಪ್ರತಿ (i = 0; i < len; ++ i)
	    {
		char_u *p = ((char_u **)argtypes->ga_data)[i];

		ಯದಿ (p == ರಿಕ್ತ)
		    // will get the type from the default value
		    type = &t_unknown;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (arg_objm != ರಿಕ್ತ && ((int8_T *)arg_objm->ga_data)[i])
		    {
			char_u *aname = ((char_u **)fp->uf_args.ga_data)[i];

			type = &t_any;
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ om = 0; om < obj_member_ಎಣಿಕೆ; ++om)
			{
			    ಯದಿ (obj_members != ರಿಕ್ತ
				    && STRCMP(aname,
					obj_members[om].ocm_name.string) == 0)
			    {
				type = obj_members[om].ocm_type;
				ಭಙ್ಗ;
			    }
			}
		    }
		    ಅನ್ಯಥಾ
			type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
		}
		ಯದಿ (type == ರಿಕ್ತ || !valid_declaration_type(type))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		fp->uf_arg_types[i] = type;
		ಯದಿ (i < fp->uf_args.ga_len
			&& (type->tt_type == VAR_FUNC
			    || type->tt_type == VAR_PARTIAL))
		{
		    char_u *name = ((char_u **)fp->uf_args.ga_data)[i];
		    ಯದಿ (obj_members != ರಿಕ್ತ && *name == '_')
			// protected object method
			name++;

		    ಯದಿ (var_wrong_func_name(name, TRUE))
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
    }

    ಯದಿ (varargs)
    {
	char_u *p;

	// Move the last argument "...name: type" to uf_va_name and
	// uf_va_type.
	--fp->uf_args.ga_len;
	fp->uf_va_name = ((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len];
	((char_u **)fp->uf_args.ga_data)[fp->uf_args.ga_len] = ರಿಕ್ತ;
	p = ((char_u **)argtypes->ga_data)[len];
	ಯದಿ (p == ರಿಕ್ತ)
	    // TODO: get type from default value
	    fp->uf_va_type = &t_list_any;
	ಅನ್ಯಥಾ
	{
	    fp->uf_va_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	    ಯದಿ (fp->uf_va_type != ರಿಕ್ತ && fp->uf_va_type->tt_type != VAR_LIST)
	    {
		semsg(_(e_variable_arguments_type_must_be_list_str),
					  ((char_u **)argtypes->ga_data)[len]);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಯದಿ (fp->uf_va_type == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_return_type(ufunc_T *fp, char_u *ret_type, cctx_T *cctx)
{
    ಯದಿ (ret_type == ರಿಕ್ತ)
	fp->uf_ret_type = &t_void;
    ಅನ್ಯಥಾ
    {
	char_u *p = ret_type;

	fp->uf_ret_type = parse_type(&p, &fp->uf_type_list, fp, cctx, TRUE);
	ಯದಿ (fp->uf_ret_type == ರಿಕ್ತ)
	{
	    fp->uf_ret_type = &t_void;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Register function "fp" as using "current_funccal" as its scope.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
register_closure(ufunc_T *fp)
{
    ಯದಿ (fp->uf_scoped == current_funccal)
	// no change
	ಪ್ರತ್ಯಾಗಮ OK;
    funccal_unref(fp->uf_scoped, fp, FALSE);
    fp->uf_scoped = current_funccal;
    current_funccal->fc_refcount++;

    ಯದಿ (ga_grow(&current_funccal->fc_ufuncs, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((ufunc_T **)current_funccal->fc_ufuncs.ga_data)
				    [current_funccal->fc_ufuncs.ga_len++] = fp;
    ಪ್ರತ್ಯಾಗಮ OK;
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

    ಯದಿ (name[0] != K_SPECIAL)
	ಪ್ರತ್ಯಾಗಮ name;
    len = STRLEN(name);
    ಯದಿ (len + 3 > bufsize)
	ಪ್ರತ್ಯಾಗಮ name;

    mch_memmove(buf + 5, name + 3, len - 2);  // Include trailing NUL
    mch_memmove(buf, "<SNR>", 5);
    ಪ್ರತ್ಯಾಗಮ buf;
}

ಸ್ಥಿರ char_u	lambda_name[8 + NUMBUFLEN];

/*
 * Get a name for a lambda.  Returned in static memory.
 */
    string_T
get_lambda_name(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	lambda_no = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;
    string_T	ret;

    n = vim_snprintf((ವರ್ಣ *)lambda_name, ಪರಿಮಾಣ(lambda_name), "<lambda>%d", ++lambda_no);
    ಯದಿ (n < 1)
	ret.length = 0;
    ಅನ್ಯಥಾ ಯದಿ (n >= (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(lambda_name))
	ret.length = ಪರಿಮಾಣ(lambda_name) - 1;
    ಅನ್ಯಥಾ
	ret.length = (size_t)n;

    ret.string = lambda_name;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Allocate a "ufunc_T" for a function called "name".
 * Makes sure the size is right.
 */
    ಸ್ಥಿರ ufunc_T *
alloc_ufunc(char_u *name, size_t namelen)
{
    size_t  len;
    ufunc_T *fp;

    // When the name is short we need to make sure we allocate enough bytes for
    // the whole struct, including any padding.
    len = offsetof(ufunc_T, uf_name) + namelen + 1;
    fp = alloc_clear(len < ಪರಿಮಾಣ(ufunc_T) ? ಪರಿಮಾಣ(ufunc_T) : len);
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	// Add a type cast to avoid a warning for an overflow, the uf_name[] array
	// can actually extend beyond the struct.
	STRCPY((ಶೂನ್ಯ *)fp->uf_name, name);
	fp->uf_namelen = namelen;

	ಯದಿ (name[0] == K_SPECIAL)
	{
	    len = namelen + 3;		    // including +1 for NUL
	    fp->uf_name_exp = alloc(len);
	    ಯದಿ (fp->uf_name_exp != ರಿಕ್ತ)
		vim_snprintf((ವರ್ಣ *)fp->uf_name_exp, len, "<SNR>%s", fp->uf_name + 3);
	}
    }

    ಪ್ರತ್ಯಾಗಮ fp;
}

#ಯದಿ defined(FEAT_LUA)
/*
 * Registers a native C callback which can be called from Vim script.
 * Returns the name of the Vim script function.
 */
    char_u *
register_cfunc(cfunc_T cb, cfunc_free_T cb_free, ಶೂನ್ಯ *state)
{
    string_T	name = get_lambda_name();
    ufunc_T	*fp;

    fp = alloc_ufunc(name.string, name.length);
    ಯದಿ (fp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

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

    ಪ್ರತ್ಯಾಗಮ name.string;
}
#endif

/*
 * Skip over "->" or "=>" after the arguments of a lambda.
 * If ": type" is found make "ret_type" point to "type".
 * If "white_error" is not NULL check for correct use of white space and set
 * "white_error" to TRUE if there is an error.
 * Return NULL if no valid arrow found.
 */
    ಸ್ಥಿರ char_u *
skip_arrow(
	char_u	*start,
	ಪೂರ್ಣಾಙ್ಕ	equal_arrow,
	char_u	**ret_type,
	ಪೂರ್ಣಾಙ್ಕ	*white_error)
{
    char_u  *s = start;
    char_u  *bef = start - 2; // "start" points to > of ->

    ಯದಿ (equal_arrow)
    {
	ಯದಿ (*s == ':')
	{
	    ಯದಿ (white_error != ರಿಕ್ತ && !VIM_ISWHITE(s[1]))
	    {
		*white_error = TRUE;
		semsg(_(e_white_space_required_after_str_str), ":", s);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    s = skipwhite(s + 1);
	    *ret_type = s;
	    s = skip_type(s, TRUE);
	    ಯದಿ (s == *ret_type)
	    {
		emsg(_(e_missing_return_type));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	bef = s;
	s = skipwhite(s);
	ಯದಿ (*s != '=')
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	++s;
    }
    ಯದಿ (*s != '>')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (white_error != ರಿಕ್ತ && ((!VIM_ISWHITE(*bef) && *bef != '{')
		|| !IS_WHITE_OR_NUL(s[1])))
    {
	*white_error = TRUE;
	semsg(_(e_white_space_required_before_and_after_str_at_str),
					       equal_arrow ? "=>" : "->", bef);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ skipwhite(s + 1);
}

/*
 * Check if "*cmd" points to a function command and if so advance "*cmd" and
 * return TRUE.
 * Otherwise return FALSE;
 * Do not consider "function(" to be a command.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_function_cmd(char_u **cmd)
{
    char_u *p = *cmd;

    ಯದಿ (checkforcmd(&p, "function", 2))
    {
	ಯದಿ (*p == '(')
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	*cmd = p;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Called when defining a function: The context may be needed for script
 * variables declared in a block that is visible now but not when the function
 * is compiled or called later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
function_using_block_scopes(ufunc_T *fp, cstack_T *cstack)
{
    ಯದಿ (cstack == ರಿಕ್ತ || cstack->cs_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ = cstack->cs_idx + 1;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    fp->uf_block_ids = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, ಎಣಿಕೆ);
    ಯದಿ (fp->uf_block_ids != ರಿಕ್ತ)
    {
	mch_memmove(fp->uf_block_ids, cstack->cs_block_id,
		ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * ಎಣಿಕೆ);
	fp->uf_block_depth = ಎಣಿಕೆ;
    }

    // Set flag in each block to indicate a function was defined.  This
    // is used to keep the variable when leaving the block, see
    // hide_script_var().
    ಪ್ರತಿ (i = 0; i <= cstack->cs_idx; ++i)
	cstack->cs_flags[i] |= CSF_FUNC_DEF;
}

/*
 * Skip over all the characters in a single quoted string starting at "p" and
 * return a pointer to the character following the ending single quote.
 * If the ending single quote is missing, then return a pointer to the NUL
 * character.
 */
    ಸ್ಥಿರ char_u *
skip_single_quote_string(char_u *p)
{
    p++;	    // skip the beginning single quote
    ಯಾವತ್ (*p != NUL)
    {
	// Within the string, a single quote can be escaped by using
	// two single quotes.
	ಯದಿ (*p == '\'' && *(p + 1) == '\'')
	    p += 2;
	ಅನ್ಯಥಾ ಯದಿ (*p == '\'')
	{
	    p++;    // skip the ending single quote
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    MB_PTR_ADV(p);
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Skip over all the characters in a double quoted string starting at "p" and
 * return a pointer to the character following the ending double quote.
 * If the ending double quote is missing, then return a pointer to the NUL
 * character.
 */
    ಸ್ಥಿರ char_u *
skip_double_quote_string(char_u *p)
{
    p++;	    // skip the beginning double quote
    ಯಾವತ್ (*p != NUL)
    {
	// Within the string, a double quote can be escaped by
	// preceding it with a backslash.
	ಯದಿ (*p == '\\' && *(p + 1) == '"')
	    p += 2;
	ಅನ್ಯಥಾ ಯದಿ (*p == '"')
	{
	    p++;    // skip the ending double quote
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    MB_PTR_ADV(p);
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Return the start of a Vim9 comment (#) in the line starting at "line".
 * If a comment is not found, then returns a pointer to the end of the
 * string (NUL).
 */
    ಸ್ಥಿರ char_u *
find_start_of_vim9_comment(char_u *ಸಾಲು)
{
    char_u	*p = ಸಾಲು;

    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (*p == '\'')
	    // Skip a single quoted string.
	    p = skip_single_quote_string(p);
	ಅನ್ಯಥಾ ಯದಿ (*p == '"')
	    // Skip a double quoted string.
	    p = skip_double_quote_string(p);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*p == '#')
		// Found the start of a Vim9 comment
		ಭಙ್ಗ;
	    MB_PTR_ADV(p);
	}
    }

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Read the body of a function, put every line in "newlines".
 * This stops at "}", "endfunction" or "enddef".
 * "newlines" must already have been initialized.
 * "eap->cmdidx" is CMD_function, CMD_def or CMD_block;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_function_body(
	exarg_T	    *eap,
	garray_T    *newlines,
	char_u	    *ಸಾಲು_arg_in,
	garray_T    *ಸಾಲುಗಳು_to_free)
{
    linenr_T	sourcing_lnum_ತುದಿ = SOURCING_LNUM;
    linenr_T	sourcing_lnum_off;
    ಪೂರ್ಣಾಙ್ಕ		saved_wait_return = need_wait_return;
    char_u	*ಸಾಲು_arg = ಸಾಲು_arg_in;
    ಪೂರ್ಣಾಙ್ಕ		vim9_function = eap->cmdidx == CMD_def
						   || eap->cmdidx == CMD_block;
#define MAX_FUNC_NESTING 50
    ವರ್ಣ	nesting_def[MAX_FUNC_NESTING];
    ವರ್ಣ	nesting_inline[MAX_FUNC_NESTING];
    ಪೂರ್ಣಾಙ್ಕ		nesting = 0;
    getline_opt_T getline_options;
    ಪೂರ್ಣಾಙ್ಕ		indent = 2;
    char_u	*skip_until = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		is_heredoc = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		heredoc_concat_len = 0;
    garray_T	heredoc_ga;
    char_u	*heredoc_trimmed = ರಿಕ್ತ;
    size_t	heredoc_trimmedlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_start = did_emsg;

    ga_init2(&heredoc_ga, 1, 500);

    // Detect having skipped over comment lines to find the return
    // type.  Add NULL lines to keep the line count correct.
    sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
    ಯದಿ (SOURCING_LNUM < sourcing_lnum_off)
    {
	sourcing_lnum_off -= SOURCING_LNUM;
	ಯದಿ (ga_grow(newlines, sourcing_lnum_off) == FAIL)
	    ಗಚ್ಛ theend;
	ಯಾವತ್ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ರಿಕ್ತ;
    }

    nesting_def[0] = vim9_function;
    nesting_inline[0] = eap->cmdidx == CMD_block;
    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
    ಪ್ರತಿ (;;)
    {
	char_u	*theline;
	char_u	*p;
	char_u	*arg;

	ಯದಿ (ಕೀಲಿ_Typed)
	{
	    ಸಂದೇಶ_scroll = TRUE;
	    saved_wait_return = FALSE;
	}
	need_wait_return = FALSE;

	ಯದಿ (ಸಾಲು_arg != ರಿಕ್ತ)
	{
	    // Use eap->arg, split up in parts by line breaks.
	    theline = ಸಾಲು_arg;
	    p = vim_strchr(theline, '\n');
	    ಯದಿ (p == ರಿಕ್ತ)
		ಸಾಲು_arg += STRLEN(ಸಾಲು_arg);
	    ಅನ್ಯಥಾ
	    {
		*p = NUL;
		ಸಾಲು_arg = p + 1;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    theline = get_function_ಸಾಲು(eap, ಸಾಲುಗಳು_to_free, indent,
							      getline_options);
	}
	ಯದಿ (ಕೀಲಿ_Typed)
	    ಸಾಲುಗಳು_ಎಡ = Rows - 1;
	ಯದಿ (theline == ರಿಕ್ತ)
	{
	    // Use the start of the function for the line number.
	    SOURCING_LNUM = sourcing_lnum_ತುದಿ;
	    ಯದಿ (skip_until != ರಿಕ್ತ)
		semsg(_(e_missing_heredoc_end_marker_str), skip_until);
	    ಅನ್ಯಥಾ ಯದಿ (nesting_inline[nesting])
		emsg(_(e_missing_end_block));
	    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_def)
		emsg(_(e_missing_enddef));
	    ಅನ್ಯಥಾ
		emsg(_(e_missing_endfunction));
	    ಗಚ್ಛ theend;
	}

	// Detect line continuation: SOURCING_LNUM increased more than one.
	sourcing_lnum_off = get_sourced_lnum(eap->ea_getline, eap->cookie);
	ಯದಿ (SOURCING_LNUM < sourcing_lnum_off)
	    sourcing_lnum_off -= SOURCING_LNUM;
	ಅನ್ಯಥಾ
	    sourcing_lnum_off = 0;

	ಯದಿ (skip_until != ರಿಕ್ತ)
	{
	    // Don't check for ":endfunc"/":enddef" between
	    // * ":append" and "."
	    // * ":python <<EOF" and "EOF"
	    // * ":let {var-name} =<< [trim] {marker}" and "{marker}"
	    ಯದಿ (heredoc_trimmed == ರಿಕ್ತ
		    || (is_heredoc && skipwhite(theline) == theline)
		    || STRNCMP(theline, heredoc_trimmed,
						 heredoc_trimmedlen) == 0)
	    {
		ಯದಿ (heredoc_trimmed == ರಿಕ್ತ)
		    p = theline;
		ಅನ್ಯಥಾ ಯದಿ (is_heredoc)
		    p = skipwhite(theline) == theline
				 ? theline : theline + heredoc_trimmedlen;
		ಅನ್ಯಥಾ
		    p = theline + heredoc_trimmedlen;
		ಯದಿ (STRCMP(p, skip_until) == 0)
		{
		    VIM_CLEAR(skip_until);
		    VIM_CLEAR(heredoc_trimmed);
		    heredoc_trimmedlen = 0;
		    getline_options = vim9_function
				? GETLINE_CONCAT_CONTBAR : GETLINE_CONCAT_CONT;
		    is_heredoc = FALSE;

		    ಯದಿ (heredoc_concat_len > 0)
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
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ	    c;
	    char_u  *end;
	    char_u  *cmd;
	    bool    colon = false;

	    // skip ':' and blanks
	    ಪ್ರತಿ (p = theline; VIM_ISWHITE(*p) || *p == ':'; ++p)
	    {
		ಯದಿ (*p == ':')
		    colon = true;
	    }

	    // Check for "endfunction", "enddef" or "}".
	    // When a ":" follows it must be a dict key; "enddef: value,"
	    cmd = p;
	    ಯದಿ (nesting_inline[nesting]
		    ? *p == '}'
		    : (checkforcmd(&p, nesting_def[nesting]
						? "enddef" : "endfunction", 4)
			&& *p != ':'))
	    {
		ಯದಿ (!nesting_inline[nesting] && nesting_def[nesting]
								&& p < cmd + 6)
		    semsg(_(e_command_cannot_be_shortened_str), cmd);
		ಯದಿ (nesting-- == 0)
		{
		    char_u *nextcmd = ರಿಕ್ತ;

		    ಯದಿ (*p == '|' || *p == '}')
			nextcmd = p + 1;
		    ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_arg != ರಿಕ್ತ && *skipwhite(ಸಾಲು_arg) != NUL)
			nextcmd = ಸಾಲು_arg;
		    ಅನ್ಯಥಾ ಯದಿ (*p != NUL && *p != (vim9_function ? '#' : '"')
					   && (vim9_function || p_verbose > 0))
		    {
			SOURCING_LNUM = sourcing_lnum_ತುದಿ
							+ newlines->ga_len + 1;
			ಯದಿ (eap->cmdidx == CMD_def)
			    semsg(_(e_text_found_after_str_str), "enddef", p);
			ಅನ್ಯಥಾ
			    give_warning2((char_u *)
				   _("W22: Text found after :endfunction: %s"),
				   p, TRUE);
		    }
		    ಯದಿ (nextcmd != ರಿಕ್ತ && *skipwhite(nextcmd) != NUL)
		    {
			// Another command follows. If the line came from "eap"
			// we can simply point into it, otherwise we need to
			// change "eap->cmdlinep" to point to the last fetched
			// line.
			eap->nextcmd = nextcmd;
			ಯದಿ (ಸಾಲುಗಳು_to_free->ga_len > 0
				&& *eap->cmdlinep !=
					    ((char_u **)ಸಾಲುಗಳು_to_free->ga_data)
						   [ಸಾಲುಗಳು_to_free->ga_len - 1])
			{
			    // *cmdlinep will be freed later, thus remove the
			    // line from lines_to_free.
			    vim_free(*eap->cmdlinep);
			    *eap->cmdlinep = ((char_u **)ಸಾಲುಗಳು_to_free->ga_data)
						   [ಸಾಲುಗಳು_to_free->ga_len - 1];
			    --ಸಾಲುಗಳು_to_free->ga_len;
			}
		    }
		    ಭಙ್ಗ;
		}
	    }

	    // Check for mismatched "endfunc" or "enddef".
	    // We don't check for "def" inside "func" thus we also can't check
	    // for "enddef".
	    // We continue to find the end of the function, although we might
	    // not find it.
	    ಅನ್ಯಥಾ ಯದಿ (nesting_def[nesting])
	    {
		ಯದಿ (checkforcmd(&p, "endfunction", 4) && *p != ':')
		    emsg(_(e_mismatched_endfunction));
	    }
	    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_def && checkforcmd(&p, "enddef", 4))
		emsg(_(e_mismatched_enddef));

	    // Increase indent inside "if", "while", "for" and "try", decrease
	    // at "end".
	    ಯದಿ (indent > 2 && (*p == '}' || STRNCMP(p, "end", 3) == 0))
		indent -= 2;
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "if", 2) == 0
		    || STRNCMP(p, "wh", 2) == 0
		    || STRNCMP(p, "for", 3) == 0
		    || STRNCMP(p, "try", 3) == 0)
		indent += 2;

	    // Check for defining a function inside this function.
	    // Only recognize "def" inside "def", not inside "function",
	    // For backwards compatibility, see Test_function_python().
	    c = *p;
	    ಯದಿ (is_function_cmd(&p)
		    || (eap->cmdidx == CMD_def && checkforcmd(&p, "def", 3)))
	    {
		ಯದಿ (*p == '!')
		    p = skipwhite(p + 1);
		p += eval_fname_script(p);
		vim_free(trans_function_name(&p, ರಿಕ್ತ, TRUE, 0));
		ಯದಿ (vim9_function && *p == '<')
		{
		    // skip generic function
		    ಯದಿ (skip_generic_func_type_args(&p) == FAIL)
			ಗಚ್ಛ theend;
		}
		ಯದಿ (*skipwhite(p) == '(')
		{
		    ಯದಿ (nesting == MAX_FUNC_NESTING - 1)
			emsg(_(e_function_nesting_too_deep));
		    ಅನ್ಯಥಾ
		    {
			++nesting;
			nesting_def[nesting] = (c == 'd');
			nesting_inline[nesting] = FALSE;
			indent += 2;
		    }
		}
	    }

	    ಯದಿ (nesting_def[nesting] ? *p != '#' : *p != '"')
	    {
		// Not a comment line: check for nested inline function.

		ಯದಿ (nesting_inline[nesting])
		{
		    // A comment (#) can follow the opening curly brace of a
		    // block statement.  Need to ignore the comment and look
		    // for the opening curly brace before the comment.
		    end = find_start_of_vim9_comment(p) - 1;
		}
		ಅನ್ಯಥಾ
		    end = p + STRLEN(p) - 1;

		ಯಾವತ್ (end > p && VIM_ISWHITE(*end))
		    --end;
		ಯದಿ (end > p + 1 && *end == '{' && VIM_ISWHITE(end[-1]))
		{
		    ಪೂರ್ಣಾಙ್ಕ	    is_block;

		    // check for trailing "=> {": start of an inline function
		    --end;
		    ಯಾವತ್ (end > p && VIM_ISWHITE(*end))
			--end;
		    is_block = end > p + 2 && end[-1] == '=' && end[0] == '>';
		    ಯದಿ (!is_block)
			is_block = find_cmd_block_start(p) != ರಿಕ್ತ;

		    ಯದಿ (is_block)
		    {
			ಯದಿ (nesting == MAX_FUNC_NESTING - 1)
			    emsg(_(e_function_nesting_too_deep));
			ಅನ್ಯಥಾ
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
	    ಯದಿ (!vim9_function || colon)
		p = skip_range(p, FALSE, ರಿಕ್ತ);
	    char_u *tp = p;
	    ಯದಿ (!vim9_function
		&& (checkforcmd(&p, "append", 1)
		    || checkforcmd(&p, "change", 1)
		    || checkforcmd(&p, "insert", 1))
		    && (*p == '!' || *p == '|' || IS_WHITE_NL_OR_NUL(*p)))
		skip_until = vim_strnsave((char_u *)".", 1);
	    ಅನ್ಯಥಾ
		p = tp;

	    // Check for ":python <<EOF", ":tcl <<EOF", etc.
	    arg = skipwhite(skiptowhite(p));
	    ಯದಿ (arg[0] == '<' && arg[1] =='<'
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
		ಯದಿ (STRNCMP(p, "trim", 4) == 0
			&& (p[4] == NUL || VIM_ISWHITE(p[4])))
		{
		    // Ignore leading white space.
		    p = skipwhite(p + 4);
		    heredoc_trimmedlen = skipwhite(theline) - theline;
		    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
		    ಯದಿ (heredoc_trimmed == ರಿಕ್ತ)
			heredoc_trimmedlen = 0;
		}
		ಯದಿ (*p == NUL)
		    skip_until = vim_strnsave((char_u *)".", 1);
		ಅನ್ಯಥಾ
		    skip_until = vim_strnsave(p, skiptowhite(p) - p);
		getline_options = GETLINE_NONE;
		is_heredoc = TRUE;
		ಯದಿ (vim9_function && nesting == 0)
		    heredoc_concat_len = newlines->ga_len + 1;
	    }

	    ಯದಿ (!is_heredoc)
	    {
		// Check for ":cmd v =<< [trim] EOF"
		//       and ":cmd [a, b] =<< [trim] EOF"
		//       and "lines =<< [trim] EOF" for Vim9
		// Where "cmd" can be "let", "var", "final" or "const".
		arg = p;
		ಯದಿ (checkforcmd(&arg, "let", 2)
			|| checkforcmd(&arg, "var", 3)
			|| checkforcmd(&arg, "final", 5)
			|| checkforcmd(&arg, "const", 5)
			|| vim9_function)
		{
		    ಪೂರ್ಣಾಙ್ಕ		save_sc_version = current_sctx.sc_version;
		    ಪೂರ್ಣಾಙ್ಕ		var_ಎಣಿಕೆ = 0;
		    ಪೂರ್ಣಾಙ್ಕ		semicolon = 0;

		    current_sctx.sc_version
				     = vim9_function ? SCRIPT_VERSION_VIM9 : 1;
		    arg = skip_var_list(arg, TRUE, &var_ಎಣಿಕೆ, &semicolon,
									 TRUE);
		    ಯದಿ (arg != ರಿಕ್ತ)
			arg = skipwhite(arg);
		    current_sctx.sc_version = save_sc_version;
		    ಯದಿ (arg != ರಿಕ್ತ && STRNCMP(arg, "=<<", 3) == 0)
		    {
			ಪೂರ್ಣಾಙ್ಕ has_trim = FALSE;

			p = skipwhite(arg + 3);
			ಯಾವತ್ (TRUE)
			{
			    ಯದಿ (STRNCMP(p, "trim", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				has_trim = TRUE;
				ಅನುವರ್ತ;
			    }
			    ಯದಿ (STRNCMP(p, "eval", 4) == 0
				    && (p[4] == NUL || VIM_ISWHITE(p[4])))
			    {
				// Ignore leading white space.
				p = skipwhite(p + 4);
				ಅನುವರ್ತ;
			    }
			    ಭಙ್ಗ;
			}
			ಯದಿ (has_trim)
			{
			    heredoc_trimmedlen = skipwhite(theline) - theline;
			    heredoc_trimmed = vim_strnsave(theline, heredoc_trimmedlen);
			    ಯದಿ (heredoc_trimmed == ರಿಕ್ತ)
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
	ಯದಿ (ga_grow_id(newlines, 1 + sourcing_lnum_off, aid_get_func) == FAIL)
	    ಗಚ್ಛ theend;

	ಯದಿ (heredoc_concat_len > 0)
	{
	    // For a :def function "python << EOF" concatenates all the lines,
	    // to be used for the instruction later.
	    ga_concat(&heredoc_ga, theline);
	    GA_CONCAT_LITERAL(&heredoc_ga, "\n");
	    p = vim_strnsave((char_u *)"", 0);
	}
	ಅನ್ಯಥಾ
	{
	    // Copy the line to newly allocated memory.  get_one_sourceline()
	    // allocates 250 bytes per line, this saves 80% on average.  The
	    // cost is an extra alloc/free.
	    p = vim_strsave(theline);
	}
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	((char_u **)(newlines->ga_data))[newlines->ga_len++] = p;

	// Add NULL lines for continuation lines, so that the line count is
	// equal to the index in the growarray.
	ಯಾವತ್ (sourcing_lnum_off-- > 0)
	    ((char_u **)(newlines->ga_data))[newlines->ga_len++] = ರಿಕ್ತ;

	// Check for end of eap->arg.
	ಯದಿ (ಸಾಲು_arg != ರಿಕ್ತ && *ಸಾಲು_arg == NUL)
	    ಸಾಲು_arg = ರಿಕ್ತ;
    }

    // Return OK when no error was detected here; "did_emsg" may already be
    // set by an earlier error in the same command.
    ಯದಿ (did_emsg == did_emsg_start)
	ret = OK;

theend:
    vim_free(skip_until);
    vim_free(heredoc_trimmed);
    vim_free(heredoc_ga.ga_data);
    need_wait_return |= saved_wait_return;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Handle the body of a lambda.  *arg points to the "{", process statements
 * until the matching "}".
 * When not evaluating "newargs" is NULL.
 * When successful "rettv" is set to a funcref.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lambda_function_body(
	char_u	    **arg,
	typval_T    *rettv,
	evalarg_T   *evalarg,
	garray_T    *newargs,
	garray_T    *argtypes,
	ಪೂರ್ಣಾಙ್ಕ	    varargs,
	garray_T    *default_args,
	char_u	    *ret_type)
{
    char_u	*start = *arg;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	*gap = &evalarg->eval_ga;
    garray_T	*freegap = &evalarg->eval_freega;
    ufunc_T	*ufunc = ರಿಕ್ತ;
    exarg_T	eap;
    garray_T	newlines;
    char_u	*cmdline = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    partial_T	*pt;
    string_T	name;
    ಪೂರ್ಣಾಙ್ಕ		lnum_save = -1;
    linenr_T	sourcing_lnum_ತುದಿ = SOURCING_LNUM;
    char_u	*ಸಾಲು_arg = ರಿಕ್ತ;

    *arg = skipwhite(*arg + 1);
    ಯದಿ (**arg == '|' || !ends_excmd2(start, *arg))
    {
	semsg(_(e_trailing_characters_str), *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // When there is a line break use what follows for the lambda body.
    // Makes lambda body initializers work for object and enum member
    // variables.
    ಯದಿ (**arg == '\n')
	ಸಾಲು_arg = *arg + 1;

    CLEAR_FIELD(eap);
    eap.cmdidx = CMD_block;
    eap.forceit = FALSE;
    eap.cmdlinep = &cmdline;
    eap.skip = !evaluate;
    ಯದಿ (evalarg->eval_cctx != ರಿಕ್ತ)
	fill_exarg_from_cctx(&eap, evalarg->eval_cctx);
    ಅನ್ಯಥಾ
    {
	eap.ea_getline = evalarg->eval_getline;
	eap.cookie = evalarg->eval_cookie;
    }

    ga_init2(&newlines, ಪರಿಮಾಣ(char_u *), 10);
    ಯದಿ (get_function_body(&eap, &newlines, ಸಾಲು_arg,
					     &evalarg->eval_tofree_ga) == FAIL)
	ಗಚ್ಛ erret;

    // When inside a lambda must add the function lines to evalarg.eval_ga.
    evalarg->eval_break_ಎಣಿಕೆ += newlines.ga_len;
    ಯದಿ (gap->ga_itemsize > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	idx;
	char_u	*last;
	size_t  plen;
	char_u  *pnl;

	ಪ್ರತಿ (idx = 0; idx < newlines.ga_len; ++idx)
	{
	    char_u  *p = ((char_u **)newlines.ga_data)[idx];
	    ಯದಿ (p == ರಿಕ್ತ)
		// comment line in the lambda body
		ಅನುವರ್ತ;

	    p = skipwhite(p);

	    ಯದಿ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
		ಗಚ್ಛ erret;

	    // Going to concatenate the lines after parsing.  For an empty or
	    // comment line use an empty string.
	    // Insert NL characters at the start of each line, the string will
	    // be split again later in .get_lambda_tv().
	    ಯದಿ (*p == NUL || vim9_comment_start(p))
	    {
		p = (char_u *)"";
		plen = 0;
	    }
	    ಅನ್ಯಥಾ
		plen = STRLEN(p);
	    pnl = vim_strnsave((char_u *)"\n", plen + 1);
	    ಯದಿ (pnl != ರಿಕ್ತ)
		mch_memmove(pnl + 1, p, plen + 1);
	    ((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	    ((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
	}
	ಯದಿ (ga_grow(gap, 1) == FAIL || ga_grow(freegap, 1) == FAIL)
	    ಗಚ್ಛ erret;
	ಯದಿ (eap.nextcmd != ರಿಕ್ತ)
	{
	    // more is following after the "}", which was skipped
	    last = cmdline;
	    plen = STRLEN(last);
	}
	ಅನ್ಯಥಾ
	{
	    // nothing is following the "}"
	    last = (char_u *)"}";
	    plen = 1;
	}
	pnl = vim_strnsave((char_u *)"\n", plen + 1);
	ಯದಿ (pnl != ರಿಕ್ತ)
	    mch_memmove(pnl + 1, last, plen + 1);
	((char_u **)gap->ga_data)[gap->ga_len++] = pnl;
	((char_u **)freegap->ga_data)[freegap->ga_len++] = pnl;
    }

    ಯದಿ (eap.nextcmd != ರಿಕ್ತ)
    {
	garray_T *tfgap = &evalarg->eval_tofree_ga;

	// Something comes after the "}".
	*arg = eap.nextcmd;

	// "arg" points into cmdline, need to keep the line and free it later.
	ಯದಿ (ga_grow(tfgap, 1) == OK)
	{
	    ((char_u **)(tfgap->ga_data))[tfgap->ga_len++] = cmdline;
	    evalarg->eval_using_cmdline = TRUE;
	}
    }
    ಅನ್ಯಥಾ
	*arg = (char_u *)"";

    ಯದಿ (!evaluate)
    {
	ret = OK;
	ಗಚ್ಛ erret;
    }

    name = get_lambda_name();
    ufunc = alloc_ufunc(name.string, name.length);
    ಯದಿ (ufunc == ರಿಕ್ತ)
	ಗಚ್ಛ erret;
    ಯದಿ (hash_add(&func_hashtab, UF2HIKEY(ufunc), "add function") == FAIL)
	ಗಚ್ಛ erret;
    ufunc->uf_flags = FC_LAMBDA;
    ufunc->uf_refcount = 1;
    ufunc->uf_args = *newargs;
    newargs->ga_data = ರಿಕ್ತ;
    ufunc->uf_def_args = *default_args;
    default_args->ga_data = ರಿಕ್ತ;
    ufunc->uf_func_type = &t_func_any;

    // error messages are for the first function line
    lnum_save = SOURCING_LNUM;
    SOURCING_LNUM = sourcing_lnum_ತುದಿ;

    // parse argument types, the type variables of the function being
    // compiled may be used
    ಯದಿ (parse_argument_types(ufunc, argtypes, varargs, ರಿಕ್ತ, ರಿಕ್ತ, 0,
					    evalarg->eval_cctx) == FAIL)
    {
	SOURCING_LNUM = lnum_save;
	ಗಚ್ಛ erret;
    }

    // parse the return type, if any
    ಯದಿ (parse_return_type(ufunc, ret_type, evalarg->eval_cctx) == FAIL)
	ಗಚ್ಛ erret;

    pt = ALLOC_CLEAR_ONE(partial_T);
    ಯದಿ (pt == ರಿಕ್ತ)
	ಗಚ್ಛ erret;
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ufunc->uf_ಸಾಲುಗಳು = newlines;
    newlines.ga_data = ರಿಕ್ತ;
    ಯದಿ (sandbox)
	ufunc->uf_flags |= FC_SANDBOX;
    ಯದಿ (!ASCII_ISUPPER(*ufunc->uf_name))
	ufunc->uf_flags |= FC_VIM9;
    ufunc->uf_script_ctx = current_sctx;
    ufunc->uf_script_ctx_version = current_sctx.sc_version;
    ufunc->uf_script_ctx.sc_lnum += sourcing_lnum_ತುದಿ;
    set_function_type(ufunc);

    function_using_block_scopes(ufunc, evalarg->eval_cstack);

    rettv->vval.v_partial = pt;
    rettv->v_type = VAR_PARTIAL;
    ufunc = ರಿಕ್ತ;
    ret = OK;

erret:
    ಯದಿ (lnum_save >= 0)
	SOURCING_LNUM = lnum_save;
    ga_clear_strings(&newlines);
    ಯದಿ (newargs != ರಿಕ್ತ)
	ga_clear_strings(newargs);
    ga_clear_strings(default_args);
    ಯದಿ (ufunc != ರಿಕ್ತ)
    {
	func_clear(ufunc, TRUE);
	func_free(ufunc, TRUE);
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Parse a lambda expression and get a Funcref from "*arg" into "rettv".
 * "arg" points to the { in "{arg -> expr}" or the ( in "(arg) => expr"
 * When "types_optional" is TRUE optionally take argument types.
 * Return OK or FAIL.  Returns NOTDONE for dict or {expr}.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_lambda_tv(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    types_optional,
	evalarg_T   *evalarg,
	cctx_T	    *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    garray_T	newargs;
    garray_T	newlines;
    garray_T	*pnewargs;
    garray_T	argtypes;
    garray_T	default_args;
    garray_T	arg_objm;
    ufunc_T	*fp = ರಿಕ್ತ;
    partial_T   *pt = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		varargs;
    char_u	*ret_type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*s;
    char_u	*start, *end;
    ಪೂರ್ಣಾಙ್ಕ		*old_eval_lavars = eval_lavars_used;
    ಪೂರ್ಣಾಙ್ಕ		eval_lavars = FALSE;
    char_u	*tofree2 = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		equal_arrow = **arg == '(';
    ಪೂರ್ಣಾಙ್ಕ		white_error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    long	start_lnum = SOURCING_LNUM;

    ಯದಿ (equal_arrow && !vim9script)
	ಪ್ರತ್ಯಾಗಮ NOTDONE;

    ga_init(&newargs);
    ga_init(&newlines);

    // First, check if this is really a lambda expression. "->" or "=>" must
    // be found after the arguments.
    s = *arg + 1;
    ret = get_function_args(&s, equal_arrow ? ')' : '-', ರಿಕ್ತ,
	    types_optional ? &argtypes : ರಿಕ್ತ, types_optional,
			types_optional ? &arg_objm : ರಿಕ್ತ, evalarg,
			ರಿಕ್ತ, &default_args, TRUE, ರಿಕ್ತ, FALSE, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (ret == FAIL || skip_arrow(s, equal_arrow, &ret_type, ರಿಕ್ತ) == ರಿಕ್ತ)
    {
	ಯದಿ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ಪ್ರತ್ಯಾಗಮ called_emsg == called_emsg_start ? NOTDONE : FAIL;
    }

    // Parse the arguments for real.
    ಯದಿ (evaluate)
	pnewargs = &newargs;
    ಅನ್ಯಥಾ
	pnewargs = ರಿಕ್ತ;
    *arg += 1;
    ret = get_function_args(arg, equal_arrow ? ')' : '-', pnewargs,
	    types_optional ? &argtypes : ರಿಕ್ತ, types_optional,
			types_optional ? &arg_objm : ರಿಕ್ತ, evalarg,
					    &varargs, &default_args,
					    FALSE, ರಿಕ್ತ, FALSE, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (ret == FAIL
		  || (s = skip_arrow(*arg, equal_arrow, &ret_type,
		equal_arrow || vim9script ? &white_error : ರಿಕ್ತ)) == ರಿಕ್ತ)
    {
	ಯದಿ (types_optional)
	{
	    ga_clear_strings(&argtypes);
	    ga_clear(&arg_objm);
	}
	ga_clear_strings(&newargs);
	ಪ್ರತ್ಯಾಗಮ white_error ? FAIL : NOTDONE;
    }
    *arg = s;

    // Skipping over linebreaks may make "ret_type" invalid, make a copy.
    ಯದಿ (ret_type != ರಿಕ್ತ)
    {
	ret_type = vim_strsave(ret_type);
	tofree2 = ret_type;
    }

    // Set up a flag for checking local variables and arguments.
    ಯದಿ (evaluate)
	eval_lavars_used = &eval_lavars;

    *arg = skipwhite_and_linebreak(*arg, evalarg);

    // Recognize "{" as the start of a function body.
    ಯದಿ (equal_arrow && **arg == '{')
    {
	ಯದಿ (evalarg == ರಿಕ್ತ)
	    // cannot happen?
	    ಗಚ್ಛ theend;
	SOURCING_LNUM = start_lnum;  // used for where lambda is defined
	ಯದಿ (lambda_function_body(arg, rettv, evalarg, pnewargs,
			   types_optional ? &argtypes : ರಿಕ್ತ, varargs,
			   &default_args, ret_type) == FAIL)
	    ಗಚ್ಛ errret;
	ಗಚ್ಛ theend;
    }
    ಯದಿ (default_args.ga_len > 0)
    {
	emsg(_(e_cannot_use_default_ಮೌಲ್ಯಗಳು_in_lambda));
	ಗಚ್ಛ errret;
    }

    // Get the start and the end of the expression.
    start = *arg;
    ret = skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(arg, &start, &end, evalarg);
    ಯದಿ (ret == FAIL)
	ಗಚ್ಛ errret;

    ಯದಿ (!equal_arrow)
    {
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg != '}')
	{
	    semsg(_(e_expected_ಬಲ_curly_str), *arg);
	    ಗಚ್ಛ errret;
	}
	++*arg;
    }

    ಯದಿ (evaluate)
    {
	ಪೂರ್ಣಾಙ್ಕ	    len;
	ಪೂರ್ಣಾಙ್ಕ	    flags = FC_LAMBDA;
	char_u	    *p;
	char_u	    *ಸಾಲು_end;
	string_T    name = get_lambda_name();

	fp = alloc_ufunc(name.string, name.length);
	ಯದಿ (fp == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;
	fp->uf_def_status = UF_NOT_COMPILED;
	pt = ALLOC_CLEAR_ONE(partial_T);
	ಯದಿ (pt == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;

	ga_init2(&newlines, ಪರಿಮಾಣ(char_u *), 1);
	ಯದಿ (ga_grow(&newlines, 1) == FAIL)
	    ಗಚ್ಛ errret;

	// If there are line breaks, we need to split up the string.
	ಸಾಲು_end = vim_strchr(start, '\n');
	ಯದಿ (ಸಾಲು_end == ರಿಕ್ತ || ಸಾಲು_end > end)
	    ಸಾಲು_end = end;

	// Add "return " before the expression (or the first line).
	len = 7 + (ಪೂರ್ಣಾಙ್ಕ)(ಸಾಲು_end - start) + 1;
	p = alloc(len);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;
	((char_u **)(newlines.ga_data))[newlines.ga_len++] = p;
	STRCPY(p, "return ");
	vim_strncpy(p + 7, start, ಸಾಲು_end - start);

	ಯದಿ (ಸಾಲು_end != end)
	{
	    // Add more lines, split by line breaks.  Thus is used when a
	    // lambda with { cmds } is encountered.
	    ಯಾವತ್ (ಸಾಲು_end < end && *ಸಾಲು_end == '\n')
	    {
		ಯದಿ (ga_grow(&newlines, 1) == FAIL)
		    ಗಚ್ಛ errret;
		start = ಸಾಲು_end + 1;
		ಸಾಲು_end = vim_strchr(start, '\n');
		ಯದಿ (ಸಾಲು_end == ರಿಕ್ತ || ಸಾಲು_end > end)
		    ಸಾಲು_end = end;
		((char_u **)(newlines.ga_data))[newlines.ga_len++] =
					 vim_strnsave(start, ಸಾಲು_end - start);
	    }
	}

	ಯದಿ (strstr((ವರ್ಣ *)p + 7, "a:") == ರಿಕ್ತ)
	    // No a: variables are used for sure.
	    flags |= FC_NOARGS;

	fp->uf_refcount = 1;
	fp->uf_args = newargs;
	ga_init(&fp->uf_def_args);
	ಯದಿ (types_optional)
	{
	    ಯದಿ (parse_argument_types(fp, &argtypes,
				vim9script && varargs, ರಿಕ್ತ, ರಿಕ್ತ, 0,
				cctx) == FAIL)
		ಗಚ್ಛ errret;
	    ಯದಿ (ret_type != ರಿಕ್ತ)
	    {
		fp->uf_ret_type = parse_type(&ret_type, &fp->uf_type_list,
					     ರಿಕ್ತ, cctx, TRUE);
		ಯದಿ (fp->uf_ret_type == ರಿಕ್ತ)
		    ಗಚ್ಛ errret;
	    }
	    ಅನ್ಯಥಾ
		fp->uf_ret_type = &t_unknown;
	}

	fp->uf_ಸಾಲುಗಳು = newlines;
	ಯದಿ (current_funccal != ರಿಕ್ತ && eval_lavars)
	{
	    flags |= FC_CLOSURE;
	    ಯದಿ (register_closure(fp) == FAIL)
		ಗಚ್ಛ errret;
	}

#ifdef FEAT_PROFILE
	ಯದಿ (prof_def_func())
	    func_do_profile(fp);
#endif
	ಯದಿ (sandbox)
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
    ಯದಿ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }

    ಪ್ರತ್ಯಾಗಮ OK;

errret:
    ga_clear_strings(&newargs);
    ga_clear_strings(&newlines);
    ga_clear_strings(&default_args);
    ಯದಿ (types_optional)
    {
	ga_clear_strings(&argtypes);
	ga_clear(&arg_objm);
    }
    ಯದಿ (fp != ರಿಕ್ತ)
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
    ಪ್ರತ್ಯಾಗಮ FAIL;
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
	ಪೂರ್ಣಾಙ್ಕ	    *lenp,
	partial_T   **partialp,
	type_T	    **type,
	ಪೂರ್ಣಾಙ್ಕ	    no_autoload,
	ಪೂರ್ಣಾಙ್ಕ	    new_function,
	ಪೂರ್ಣಾಙ್ಕ	    *found_var)
{
    dictitem_T	*v;
    typval_T	*tv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    char_u	*s = ರಿಕ್ತ;
    hashtab_T	*ht;
    ಪೂರ್ಣಾಙ್ಕ		did_type = FALSE;

    ಯದಿ (partialp != ರಿಕ್ತ)
	*partialp = ರಿಕ್ತ;

    cc = name[*lenp];
    name[*lenp] = NUL;

    v = find_var_also_in_script(name, &ht, no_autoload);
    name[*lenp] = cc;
    ಯದಿ (v != ರಿಕ್ತ)
    {
	tv = &v->di_tv;
    }
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script() || STRNCMP(name, "s:", 2) == 0)
    {
	imported_T  *import;
	char_u	    *p = name;
	ಪೂರ್ಣಾಙ್ಕ	    len = *lenp;

	ಯದಿ (STRNCMP(name, "s:", 2) == 0)
	{
	    p = name + 2;
	    len -= 2;
	}
	import = find_imported(p, len, FALSE);

	// imported function from another script
	ಯದಿ (import != ರಿಕ್ತ)
	{
	    name[len] = NUL;
	    ಯದಿ (new_function)
		semsg(_(e_redefining_imported_item_str), name);
	    ಅನ್ಯಥಾ
		semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    name[len] = cc;
	    *lenp = 0;
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";	// just in case
	}
    }

    ಯದಿ (tv != ರಿಕ್ತ)
    {
	ಯದಿ (found_var != ರಿಕ್ತ)
	    *found_var = TRUE;
	ಯದಿ (tv->v_type == VAR_FUNC)
	{
	    ಯದಿ (tv->vval.v_string == ರಿಕ್ತ)
	    {
		*lenp = 0;
		ಪ್ರತ್ಯಾಗಮ (char_u *)"";	// just in case
	    }
	    s = tv->vval.v_string;
	    char_u *p = generic_func_find_open_bracket(s);
	    ಯದಿ (p != ರಿಕ್ತ && generic_func_find_open_bracket(name) != ರಿಕ್ತ)
	    {
		// the funcref name already has generic types but types are
		// specified when using the funcref.
		*lenp = 0;
		ಪ್ರತ್ಯಾಗಮ (char_u *)"";
	    }
	    ಯದಿ (p != ರಿಕ್ತ)
		*lenp = p - s;
	    ಅನ್ಯಥಾ
		*lenp = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
	}
	ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_PARTIAL)
	{
	    partial_T *pt = tv->vval.v_partial;

	    ಯದಿ (pt == ರಿಕ್ತ)
	    {
		*lenp = 0;
		ಪ್ರತ್ಯಾಗಮ (char_u *)"";	// just in case
	    }
	    ಯದಿ (partialp != ರಿಕ್ತ)
		*partialp = pt;
	    s = partial_name(pt);
	    *lenp = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
	}

	ಯದಿ (s != ರಿಕ್ತ)
	{
	    ಯದಿ (!did_type && type != ರಿಕ್ತ && ht == get_script_local_ht())
	    {
		svar_T  *sv = find_typval_in_script(tv, 0, TRUE);

		ಯದಿ (sv != ರಿಕ್ತ)
		    *type = sv->sv_type;
	    }
	    ಪ್ರತ್ಯಾಗಮ s;
	}
    }

    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * Give an error message with a function name.  Handle <SNR> things.
 * "ermsg" is to be passed without translation, use N_() instead of _().
 */
    ಶೂನ್ಯ
emsg_funcname(ವರ್ಣ *ermsg, char_u *name)
{
    char_u	*p = name;

    ಯದಿ (name[0] == K_SPECIAL && name[1] != NUL && name[2] != NUL)
	p = concat_str((char_u *)"<SNR>", name + 3);
    semsg(_(ermsg), p);
    ಯದಿ (p != name)
	vim_free(p);
}

/*
 * Get function arguments at "*arg" and advance it.
 * Return them in "*argvars[MAX_FUNC_ARGS + 1]" and the count in "argcount".
 * On failure FAIL is returned but the "argvars[argcount]" are still set.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_func_arguments(
	char_u	    **arg,
	evalarg_T   *evalarg,
	ಪೂರ್ಣಾಙ್ಕ	    partial_argc,
	typval_T    *argvars,
	ಪೂರ್ಣಾಙ್ಕ	    *argcount,
	ಪೂರ್ಣಾಙ್ಕ	    is_builtin)
{
    char_u	*argp = *arg;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ಯಾವತ್ (*argcount < MAX_FUNC_ARGS - partial_argc)
    {
	// skip the '(' or ',' and possibly line breaks
	argp = skipwhite_and_linebreak(argp + 1, evalarg);

	ಯದಿ (*argp == ')' || *argp == ',' || *argp == NUL)
	    ಭಙ್ಗ;

	ಪೂರ್ಣಾಙ್ಕ arg_idx = *argcount;
	ಯದಿ (eval1(&argp, &argvars[arg_idx], evalarg) == FAIL)
	{
	    ret = FAIL;
	    ಭಙ್ಗ;
	}
	++*argcount;
	ಯದಿ (!is_builtin && check_typval_is_ಮೌಲ್ಯ(&argvars[arg_idx]) == FAIL)
	{
	    ret = FAIL;
	    ಭಙ್ಗ;
	}

	// The comma should come right after the argument, but this wasn't
	// checked previously, thus only enforce it in Vim9 script.
	ಯದಿ (vim9script)
	{
	    ಯದಿ (*argp != ',' && *skipwhite(argp) == ',')
	    {
		ಯದಿ (evaluate)
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", argp);
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	    argp = skipwhite(argp);
	ಯದಿ (*argp != ',')
	    ಭಙ್ಗ;
	ಯದಿ (vim9script && !IS_WHITE_NL_OR_NUL(argp[1]))
	{
	    ಯದಿ (evaluate)
		semsg(_(e_white_space_required_after_str_str), ",", argp);
	    ret = FAIL;
	    ಭಙ್ಗ;
	}
    }

    argp = skipwhite_and_linebreak(argp, evalarg);
    ಯದಿ (*argp == ')')
	++argp;
    ಅನ್ಯಥಾ
	ret = FAIL;
    *arg = argp;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Call a function and put the result in "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_func_tv(
    char_u	*name,		// name of the function
    ಪೂರ್ಣಾಙ್ಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,
    char_u	**arg,		// argument, pointing to the '('
    evalarg_T	*evalarg,	// for line continuation
    funcexe_T	*funcexe)	// various values
{
    char_u	*argp;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;			// number of arguments found
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    argp = *arg;
    ret = get_func_arguments(&argp, evalarg,
	    (funcexe->fe_partial == ರಿಕ್ತ ? 0 : funcexe->fe_partial->pt_argc),
			       argvars, &argcount, builtin_function(name, -1));

    ಯದಿ (ret == OK)
    {
	ಪೂರ್ಣಾಙ್ಕ	i = 0;
	ಪೂರ್ಣಾಙ್ಕ	did_emsg_before = did_emsg;

	ಯದಿ (get_vim_var_nr(VV_TESTING))
	{
	    // Prepare for calling test_garbagecollect_now(), need to know
	    // what variables are used on the call stack.
	    ಯದಿ (funcargs.ga_itemsize == 0)
		ga_init2(&funcargs, ಪರಿಮಾಣ(typval_T *), 50);
	    ಪ್ರತಿ (i = 0; i < argcount; ++i)
		ಯದಿ (ga_grow(&funcargs, 1) == OK)
		    ((typval_T **)funcargs.ga_data)[funcargs.ga_len++] =
								  &argvars[i];
	}

	ret = call_func(name, len, rettv, argcount, argvars, funcexe);
	ಯದಿ (vim9script && did_emsg > did_emsg_before)
	{
	    // An error in a builtin function does not return FAIL, but we do
	    // want to abort further processing if an error was given.
	    ret = FAIL;
	    clear_tv(rettv);
	}

	funcargs.ga_len -= i;
    }
    ಅನ್ಯಥಾ ಯದಿ (!aborting() && evaluate)
    {
	ಯದಿ (argcount == MAX_FUNC_ARGS)
	    emsg_funcname(e_too_many_arguments_for_function_str_2, name);
	ಅನ್ಯಥಾ
	    emsg_funcname(e_invalid_arguments_for_function_str, name);
    }

    ಯಾವತ್ (--argcount >= 0)
	clear_tv(&argvars[argcount]);

    ಯದಿ (vim9script)
	*arg = argp;
    ಅನ್ಯಥಾ
	*arg = skipwhite(argp);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return TRUE if "p" starts with "<SID>" or "s:".
 * Only works if eval_fname_script() returned non-zero for "p"!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_fname_sid(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (*p == 's' || TOUPPER_ASC(p[2]) == 'I');
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
    ಯದಿ (script_name == name)
	ಪ್ರತ್ಯಾಗಮ name;  // no prefix

    fname_buf[0] = K_SPECIAL;
    fname_buf[1] = KS_EXTRA;
    fname_buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
    fname_buflen = 3;
    ಯದಿ (!eval_fname_sid(name))	// "<SID>" or "s:"
	fname_buf[fname_buflen] = NUL;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (current_sctx.sc_sid <= 0)
	    *error = FCERR_SCRIPT;
	ಅನ್ಯಥಾ
	{
	    fname_buflen += vim_snprintf((ವರ್ಣ *)fname_buf + 3,
						FLEN_FIXED - 3,
						"%ld_",
						(long)current_sctx.sc_sid);
	}
    }
    fnamelen = fname_buflen + STRLEN(script_name);
    ಯದಿ (fnamelen < FLEN_FIXED)
    {
	STRCPY(fname_buf + fname_buflen, script_name);
	fname = fname_buf;
    }
    ಅನ್ಯಥಾ
    {
	fname = alloc(fnamelen + 1);
	ಯದಿ (fname == ರಿಕ್ತ)
	    *error = FCERR_OTHER;
	ಅನ್ಯಥಾ
	{
	    *tofree = fname;
	    vim_snprintf((ವರ್ಣ *)fname, fnamelen + 1, "%s%s", fname_buf, script_name);
	}
    }
    ಪ್ರತ್ಯಾಗಮ fname;
}

/*
 * Concatenate the script ID and function name into  "<SNR>99_name".
 * "buffer" must have size MAX_FUNC_NAME_LEN.
 */
    ಶೂನ್ಯ
func_name_with_sid(char_u *name, ಪೂರ್ಣಾಙ್ಕ sid, char_u *buffer)
{
    // A script-local function is stored as "<SNR>99_name".
    buffer[0] = K_SPECIAL;
    buffer[1] = KS_EXTRA;
    buffer[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
    vim_snprintf((ವರ್ಣ *)buffer + 3, MAX_FUNC_NAME_LEN - 3, "%ld_%s",
							      (long)sid, name);
}

/*
 * Find a function "name" in script "sid".
 */
    ಸ್ಥಿರ ufunc_T *
find_func_with_sid(char_u *name, ಪೂರ್ಣಾಙ್ಕ sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];

    ಯದಿ (!SCRIPT_ID_VALID(sid))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// not in a script

    func_name_with_sid(name, sid, buffer);
    hi = hash_find(&func_hashtab, buffer);
    ಯದಿ (!HASHITEM_EMPTY(hi))
	ಪ್ರತ್ಯಾಗಮ HI2UF(hi);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find a function "name" in script "sid" prefixing the autoload prefix.
 */
    ಸ್ಥಿರ ufunc_T *
find_func_with_prefix(char_u *name, ಪೂರ್ಣಾಙ್ಕ sid)
{
    hashitem_T	    *hi;
    char_u	    buffer[MAX_FUNC_NAME_LEN];
    scriptitem_T    *si;

    ಯದಿ (vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// already has the prefix
    ಯದಿ (!SCRIPT_ID_VALID(sid))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// not in a script
    si = SCRIPT_ITEM(sid);
    ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ)
    {
	size_t	len = STRLEN(si->sn_autoload_prefix) + STRLEN(name) + 1;
	char_u	*auto_name;
	char_u	*namep;

	// skip a "<SNR>99_" prefix
	namep = untrans_function_name(name);
	ಯದಿ (namep == ರಿಕ್ತ)
	    namep = name;

	// An exported function in an autoload script is stored as
	// "dir#path#name".
	ಯದಿ (len < ಪರಿಮಾಣ(buffer))
	    auto_name = buffer;
	ಅನ್ಯಥಾ
	    auto_name = alloc(len);
	ಯದಿ (auto_name != ರಿಕ್ತ)
	{
	    vim_snprintf((ವರ್ಣ *)auto_name, len, "%s%s",
						si->sn_autoload_prefix, namep);
	    hi = hash_find(&func_hashtab, auto_name);
	    ಯದಿ (auto_name != buffer)
		vim_free(auto_name);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
		ಪ್ರತ್ಯಾಗಮ HI2UF(hi);
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * When "flags" has FFED_IS_GLOBAL don't find script-local or imported
 * functions.
 * When "flags" has "FFED_NO_GLOBAL" don't find global functions.
 * Return NULL for unknown function.
 */
    ufunc_T *
find_func_even_dead(char_u *name, ಪೂರ್ಣಾಙ್ಕ flags)
{
    hashitem_T	*hi;
    ufunc_T	*func;

    ಯದಿ ((flags & FFED_IS_GLOBAL) == 0)
    {
	// Find script-local function before global one.
	ಯದಿ (in_vim9script() && eval_isnamec1(*name)
					   && (name[1] != ':' || *name == 's'))
	{
	    func = find_func_with_sid(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
	    ಯದಿ (func != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ func;
	}
	ಯದಿ (in_vim9script() && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    char_u  *p = name + 5;
	    long    sid;

	    // printable "<SNR>123_Name" form
	    sid = getdigits(&p);
	    ಯದಿ (*p == '_')
	    {
		func = find_func_with_sid(p + 1, (ಪೂರ್ಣಾಙ್ಕ)sid);
		ಯದಿ (func != ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ func;
	    }
	}
    }

    ಯದಿ ((flags & FFED_NO_GLOBAL) == 0)
    {
	hi = hash_find(&func_hashtab,
				STRNCMP(name, "g:", 2) == 0 ? name + 2 : name);
	ಯದಿ (!HASHITEM_EMPTY(hi))
	    ಪ್ರತ್ಯಾಗಮ HI2UF(hi);
    }

    // Find autoload function if this is an autoload script.
    ಪ್ರತ್ಯಾಗಮ find_func_with_prefix(name[0] == 's' && name[1] == ':'
				       ? name + 2 : name, current_sctx.sc_sid);
}

/*
 * Find a function by name, return pointer to it in ufuncs.
 * "cctx" is passed in a :def function to find imported functions.
 * Return NULL for unknown or dead function.
 */
    ufunc_T *
find_func(char_u *name, ಪೂರ್ಣಾಙ್ಕ is_global)
{
    ufunc_T	*fp = find_func_even_dead(name, is_global ? FFED_IS_GLOBAL : 0);

    ಯದಿ (fp != ರಿಕ್ತ && (fp->uf_flags & FC_DEAD) == 0)
	ಪ್ರತ್ಯಾಗಮ fp;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE if "ufunc" is a global function.
 */
    ಪೂರ್ಣಾಙ್ಕ
func_is_global(ufunc_T *ufunc)
{
    ಪ್ರತ್ಯಾಗಮ ufunc->uf_name[0] != K_SPECIAL;
}

/*
 * Return TRUE if "ufunc" must be called with a g: prefix in Vim9 script.
 */
    ಪೂರ್ಣಾಙ್ಕ
func_requires_g_prefix(ufunc_T *ufunc)
{
    ಪ್ರತ್ಯಾಗಮ func_is_global(ufunc)
	    && (ufunc->uf_flags & FC_LAMBDA) == 0
	    && !IS_GENERIC_FUNC(ufunc)
	    && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ರಿಕ್ತ
	    && !SAFE_isdigit(ufunc->uf_name[0]);
}

/*
 * Copy the function name of "fp" to buffer "buf".
 * "buf" must be able to hold the function name plus three bytes.
 * Takes care of script-local function names.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cat_func_name(char_u *buf, size_t bufsize, ufunc_T *fp)
{
    ಪೂರ್ಣಾಙ್ಕ	len;

    ಯದಿ (!func_is_global(fp))
	len = vim_snprintf((ವರ್ಣ *)buf, bufsize, "<SNR>%s", fp->uf_name + 3);
    ಅನ್ಯಥಾ
	len = vim_snprintf((ವರ್ಣ *)buf, bufsize, "%s", fp->uf_name);

    ಪ್ರತ್ಯಾಗಮ (len >= (ಪೂರ್ಣಾಙ್ಕ)bufsize) ? (ಪೂರ್ಣಾಙ್ಕ)bufsize - 1 : len;
}

/*
 * Add a number variable "name" to dict "dp" with value "nr".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_nr_var(
    dict_T	*dp,
    dictitem_T	*v,
    ವರ್ಣ	*name,
    varnumber_T nr)
{
    STRCPY(v->di_ಕೀಲಿ, name);
    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    hash_add(&dp->dv_hashtab, DI2HIKEY(v), "add variable");
    v->di_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    v->di_tv.v_lock = VAR_FIXED;
    v->di_tv.vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * Free "fc".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_funccal(funccall_T *fc)
{
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಪ್ರತಿ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
    {
	ufunc_T *fp = ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i];

	// When garbage collecting a funccall_T may be freed before the
	// function that references it, clear its uf_scoped field.
	// The function may have been redefined and point to another
	// funccall_T, don't clear it then.
	ಯದಿ (fp != ರಿಕ್ತ && fp->uf_scoped == fc)
	    fp->uf_scoped = ರಿಕ್ತ;
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
   ಸ್ಥಿರ ಶೂನ್ಯ
free_funccal_contents(funccall_T *fc)
{
    listitem_T	*li;

    // Free all l: variables.
    vars_clear(&fc->fc_l_vars.dv_hashtab);

    // Free all a: variables.
    vars_clear(&fc->fc_l_avars.dv_hashtab);

    // Free the a:000 variables.
    FOR_ALL_LIST_ವಸ್ತುಗಳು(&fc->fc_l_varlist, li)
	clear_tv(&li->li_tv);

    free_funccal(fc);
}

/*
 * Handle the last part of returning from a function: free the local hashtable.
 * Unless it is still in use by a closure.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_function_call(funccall_T *fc)
{
    ಪೂರ್ಣಾಙ್ಕ	may_free_fc = fc->fc_refcount <= 0;
    ಪೂರ್ಣಾಙ್ಕ	free_fc = TRUE;

    current_funccal = fc->fc_caller;

    // Free all l: variables if not referred.
    ಯದಿ (may_free_fc && fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear(&fc->fc_l_vars.dv_hashtab);
    ಅನ್ಯಥಾ
	free_fc = FALSE;

    // If the a:000 list and the l: and a: dicts are not referenced and
    // there is no closure using it, we can free the funccall_T and what's
    // in it.
    ಯದಿ (may_free_fc && fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)
	vars_clear_ext(&fc->fc_l_avars.dv_hashtab, FALSE);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    todo;
	hashitem_T  *hi;
	dictitem_T  *di;

	free_fc = FALSE;

	// Make a copy of the a: variables, since we didn't do that above.
	todo = (ಪೂರ್ಣಾಙ್ಕ)fc->fc_l_avars.dv_hashtab.ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&fc->fc_l_avars.dv_hashtab, hi, todo)
	{
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		di = HI2DI(hi);
		copy_tv(&di->di_tv, &di->di_tv);
	    }
	}
    }

    ಯದಿ (may_free_fc && fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT)
	fc->fc_l_varlist.lv_first = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	listitem_T *li;

	free_fc = FALSE;

	// Make a copy of the a:000 items, since we didn't do that above.
	FOR_ALL_LIST_ವಸ್ತುಗಳು(&fc->fc_l_varlist, li)
	    copy_tv(&li->li_tv, &li->li_tv);
    }

    ಯದಿ (free_fc)
	free_funccal(fc);
    ಅನ್ಯಥಾ
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ made_copy = 0;

	// "fc" is still in use.  This can happen when returning "a:000",
	// assigning "l:" to a global variable or defining a closure.
	// Link "fc" in the list for garbage collection later.
	fc->fc_caller = previous_funccal;
	previous_funccal = fc;

	ಯದಿ (want_garbage_collect)
	    // If garbage collector is ready, clear count.
	    made_copy = 0;
	ಅನ್ಯಥಾ ಯದಿ (++made_copy >= (ಪೂರ್ಣಾಙ್ಕ)((4096 * 1024) / ಪರಿಮಾಣ(*fc)))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
numbered_function(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ SAFE_isdigit(*name)
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
    ಪೂರ್ಣಾಙ್ಕ
func_name_refcount(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ numbered_function(name) || (name[0] == '<' && name[1] == 'l');
}

/*
 * Unreference "fc": decrement the reference count and free it when it
 * becomes zero.  "fp" is detached from "fc".
 * When "force" is TRUE we are exiting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
funccal_unref(funccall_T *fc, ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force)
{
    funccall_T	**pfc;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (fc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (--fc->fc_refcount <= 0 && (force || (
		fc->fc_l_varlist.lv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_vars.dv_refcount == DO_NOT_FREE_CNT
		&& fc->fc_l_avars.dv_refcount == DO_NOT_FREE_CNT)))
	ಪ್ರತಿ (pfc = &previous_funccal; *pfc != ರಿಕ್ತ; pfc = &(*pfc)->fc_caller)
	{
	    ಯದಿ (fc == *pfc)
	    {
		*pfc = fc->fc_caller;
		free_funccal_contents(fc);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    ಪ್ರತಿ (i = 0; i < fc->fc_ufuncs.ga_len; ++i)
	ಯದಿ (((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] == fp)
	    ((ufunc_T **)(fc->fc_ufuncs.ga_data))[i] = ರಿಕ್ತ;
}

/*
 * Remove the function from the function hashtable.  If the function was
 * deleted while it still has references this was already done.
 * Return TRUE if the entry was deleted, FALSE if it wasn't found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
func_remove(ufunc_T *fp)
{
    hashitem_T	*hi;

    // Return if it was already virtually deleted.
    ಯದಿ (fp->uf_flags & FC_DEAD)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    hi = hash_find(&func_hashtab, UF2HIKEY(fp));
    // Another function with the same name may be in the table, e.g. a global
    // function with the name of a class method, do not remove it.
    ಯದಿ (HASHITEM_EMPTY(hi) || HI2UF(hi) != fp)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // When there is a def-function index do not actually remove the
    // function, so we can find the index when defining the function again.
    // Do remove it when it's a copy.
    ಯದಿ (fp->uf_def_status == UF_COMPILED && (fp->uf_flags & FC_COPY) == 0)
    {
	fp->uf_flags |= FC_DEAD;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    hash_remove(&func_hashtab, hi, "remove function");
    fp->uf_flags |= FC_DELETED;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
func_clear_ವಸ್ತುಗಳು(ufunc_T *fp)
{
    ga_clear_strings(&(fp->uf_args));
    ga_clear_strings(&(fp->uf_def_args));
    ga_clear_strings(&(fp->uf_ಸಾಲುಗಳು));
    VIM_CLEAR(fp->uf_arg_types);
    VIM_CLEAR(fp->uf_block_ids);
    VIM_CLEAR(fp->uf_va_name);
    clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);

    ಯದಿ (IS_GENERIC_FUNC(fp))
	generic_func_clear_ವಸ್ತುಗಳು(fp);

    // Increment the refcount of this function to avoid it being freed
    // recursively when the partial is freed.
    fp->uf_refcount += 3;
    partial_unref(fp->uf_partial);
    fp->uf_partial = ರಿಕ್ತ;
    fp->uf_refcount -= 3;

#ifdef FEAT_LUA
    ಯದಿ (fp->uf_cb_free != ರಿಕ್ತ)
    {
	fp->uf_cb_free(fp->uf_cb_state);
	fp->uf_cb_free = ರಿಕ್ತ;
    }

    fp->uf_cb_state = ರಿಕ್ತ;
    fp->uf_cb = ರಿಕ್ತ;
#endif
#ifdef FEAT_PROFILE
    VIM_CLEAR(fp->uf_tml_ಎಣಿಕೆ);
    VIM_CLEAR(fp->uf_tml_total);
    VIM_CLEAR(fp->uf_tml_self);
#endif
}

/*
 * Free all things that a function contains.  Does not free the function
 * itself, use func_free() for that.
 * When "force" is TRUE we are exiting.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
func_clear(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force)
{
    ಯದಿ (fp->uf_cleared)
	ಪ್ರತ್ಯಾಗಮ;
    fp->uf_cleared = TRUE;

    // clear this function
    func_clear_ವಸ್ತುಗಳು(fp);
    funccal_unref(fp->uf_scoped, fp, force);
    unlink_def_function(fp);
}

/*
 * Free a function and remove it from the list of functions.  Does not free
 * what a function contains, call func_clear() first.
 * When "force" is TRUE we are exiting.
 * Returns OK when the function was actually freed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
func_free(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force)
{
    // Only remove it when not done already, otherwise we would remove a newer
    // version of the function with the same name.
    ಯದಿ ((fp->uf_flags & (FC_DELETED | FC_REMOVED)) == 0)
	func_remove(fp);

    ಯದಿ ((fp->uf_flags & FC_DEAD) == 0 || force)
    {
	ಯದಿ (fp->uf_dfunc_idx > 0)
	    unlink_def_function(fp);
	VIM_CLEAR(fp->uf_name_exp);
	vim_free(fp);
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Free all things that a function contains and free the function itself.
 * When "force" is TRUE we are exiting.
 */
    ಶೂನ್ಯ
func_clear_free(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ force)
{
    func_clear(fp, force);
    ಯದಿ (force || fp->uf_dfunc_idx == 0 || func_name_refcount(fp->uf_name)
						   || (fp->uf_flags & FC_COPY))
	func_free(fp, force);
    ಅನ್ಯಥಾ
	fp->uf_flags |= FC_DEAD;
}

/*
 * Copy already defined function "lambda" to a new function with name "global".
 * This is for when a compiled function defines a global function.
 */
    ಪೂರ್ಣಾಙ್ಕ
copy_lambda_to_global_func(
	char_u		*lambda,
	char_u		*global,
	loopvarinfo_T	*loopvarinfo,
	ectx_T		*ectx)
{
    ufunc_T *ufunc = find_func_even_dead(lambda, FFED_IS_GLOBAL);
    ufunc_T *fp = ರಿಕ್ತ;

    ಯದಿ (ufunc == ರಿಕ್ತ)
    {
	semsg(_(e_lambda_function_not_found_str), lambda);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    fp = find_func(global, TRUE);
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	// TODO: handle ! to overwrite
	semsg(_(e_function_str_already_exists_add_bang_to_replace), global);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    fp = alloc_ufunc(global, STRLEN(global));
    ಯದಿ (fp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    fp->uf_varargs = ufunc->uf_varargs;
    fp->uf_flags = (ufunc->uf_flags & ~FC_VIM9) | FC_COPY;
    fp->uf_def_status = ufunc->uf_def_status;
    fp->uf_dfunc_idx = ufunc->uf_dfunc_idx;
    ಯದಿ (ga_copy_strings(&ufunc->uf_args, &fp->uf_args) == FAIL
	    || ga_copy_strings(&ufunc->uf_def_args, &fp->uf_def_args)
								    == FAIL
	    || ga_copy_strings(&ufunc->uf_ಸಾಲುಗಳು, &fp->uf_ಸಾಲುಗಳು) == FAIL)
	ಗಚ್ಛ failed;

    ಯದಿ (ufunc->uf_arg_types != ರಿಕ್ತ)
    {
	fp->uf_arg_types = ALLOC_MULT(type_T *, fp->uf_args.ga_len);
	ಯದಿ (fp->uf_arg_types == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
	mch_memmove(fp->uf_arg_types, ufunc->uf_arg_types,
				    ಪರಿಮಾಣ(type_T *) * fp->uf_args.ga_len);
    }
    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ)
    {
	fp->uf_va_name = vim_strsave(ufunc->uf_va_name);
	ಯದಿ (fp->uf_va_name == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
    }
    fp->uf_ret_type = ufunc->uf_ret_type;

    fp->uf_refcount = 1;

    hash_add(&func_hashtab, UF2HIKEY(fp), "copy lambda");

    // the referenced dfunc_T is now used one more time
    link_def_function(fp);

    // Create a partial to store the context of the function where it was
    // instantiated.  Only needs to be done once.  Do this on the original
    // function, "dfunc->df_ufunc" will point to it.
    ಯದಿ ((ufunc->uf_flags & FC_CLOSURE) && ufunc->uf_partial == ರಿಕ್ತ)
    {
	partial_T   *pt = ALLOC_CLEAR_ONE(partial_T);

	ಯದಿ (pt == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
	ಯದಿ (fill_partial_and_closure(pt, ufunc, loopvarinfo, ectx) == FAIL)
	{
	    vim_free(pt);
	    ಗಚ್ಛ failed;
	}
	ufunc->uf_partial = pt;
    }

    ಪ್ರತ್ಯಾಗಮ OK;

failed:
    func_clear_free(fp, TRUE);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	funcdepth = 0;

/*
 * Increment the function call depth count.
 * Return FAIL when going over 'maxfuncdepth'.
 * Otherwise return OK, must call funcdepth_decrement() later!
 */
    ಪೂರ್ಣಾಙ್ಕ
funcdepth_increment(ಶೂನ್ಯ)
{
    ಯದಿ (funcdepth >= p_mfd)
    {
	emsg(_(e_function_call_depth_is_higher_than_maxfuncdepth));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++funcdepth;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
funcdepth_decrement(ಶೂನ್ಯ)
{
    --funcdepth;
}

/*
 * Get the current function call depth.
 */
    ಪೂರ್ಣಾಙ್ಕ
funcdepth_get(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ funcdepth;
}

/*
 * Restore the function call depth.  This is for cases where there is no
 * guarantee funcdepth_decrement() can be called exactly the same number of
 * times as funcdepth_increment().
 */
    ಶೂನ್ಯ
funcdepth_restore(ಪೂರ್ಣಾಙ್ಕ depth)
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

    ಯದಿ (fc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    fc->fc_caller = current_funccal;
    current_funccal = fc;
    fc->fc_func = fp;
    func_ptr_ref(fp);
    fc->fc_rettv = rettv;
    ಪ್ರತ್ಯಾಗಮ fc;
}

/*
 * To be called when returning from a compiled function; restores
 * current_funccal.
 */
    ಶೂನ್ಯ
remove_funccal(ಶೂನ್ಯ)
{
    funccall_T *fc = current_funccal;

    current_funccal = fc->fc_caller;
    free_funccal(fc);
}

/*
 * Call a user function.
 */
    ಸ್ಥಿರ funcerror_T
call_user_func(
    ufunc_T	*fp,		// pointer to function
    ಪೂರ್ಣಾಙ್ಕ		argcount,	// nr of args
    typval_T	*argvars,	// arguments
    typval_T	*rettv,		// return value
    funcexe_T	*funcexe,	// context
    dict_T	*selfdict)	// Dictionary for "self"
{
    sctx_T	save_current_sctx;
    ectx_T	*save_current_ectx;
    ಪೂರ್ಣಾಙ್ಕ		using_sandbox = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    funccall_T	*fc;
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg;
    funcerror_T retval = FCERR_NONE;
    ಪೂರ್ಣಾಙ್ಕ		default_arg_err = FALSE;
    dictitem_T	*v;
    ಪೂರ್ಣಾಙ್ಕ		fixvar_idx = 0;	// index in fc_fixvar[]
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ai;
    ಪೂರ್ಣಾಙ್ಕ		islambda = FALSE;
    char_u	numbuf[NUMBUFLEN];
    char_u	*name;
    size_t	namelen;
    typval_T	*tv_to_free[MAX_FUNC_ARGS];
    ಪೂರ್ಣಾಙ್ಕ		tv_to_free_len = 0;
#ifdef FEAT_PROFILE
    profinfo_T	profile_info;
#endif
    ESTACK_CHECK_DECLARATION;

#ifdef FEAT_PROFILE
    CLEAR_FIELD(profile_info);
#endif

    // If depth of calling is getting too high, don't execute the function.
    ಯದಿ (funcdepth_increment() == FAIL)
    {
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಪ್ರತ್ಯಾಗಮ FCERR_FAILED;
    }

    ಸಾಲು_breakcheck();		// check for CTRL-C hit

    fc = create_funccal(fp, rettv);
    ಯದಿ (fc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FCERR_OTHER;
    fc->fc_level = ex_nesting_level;
    // Check if this function has a breakpoint.
    fc->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name, (linenr_T)0);
    fc->fc_dbg_tick = debug_tick;
    // Set up fields for closure.
    ga_init2(&fc->fc_ufuncs, ಪರಿಮಾಣ(ufunc_T *), 1);

    ಯದಿ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ಯದಿ (fp->uf_flags & FC_SANDBOX)
	{
	    using_sandbox = TRUE;
	    ++sandbox;
	}
#ifdef FEAT_PROFILE
	ufunc_T *caller = fc->fc_caller == ರಿಕ್ತ ? ರಿಕ್ತ : fc->fc_caller->fc_func;
#endif
	// Execute the function, possibly compiling it first.
#ifdef FEAT_PROFILE
	ಯದಿ (do_profiling == PROF_YES)
	    profile_may_start_func(&profile_info, fp, caller);
#endif
	sticky_cmdmod_flags = 0;
	ಯದಿ (call_def_function(fp, argcount, argvars, 0,
		   funcexe->fe_partial, funcexe->fe_object, fc, rettv) == FAIL)
	    retval = FCERR_FAILED;
	ಯದಿ (using_sandbox)
	    --sandbox;
	funcdepth_decrement();
#ifdef FEAT_PROFILE
	ಯದಿ (do_profiling == PROF_YES && (fp->uf_profiling
				  || (caller != ರಿಕ್ತ && caller->uf_profiling)))
	    profile_may_end_func(&profile_info, fp, caller);
#endif
	remove_funccal();
	sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	ಪ್ರತ್ಯಾಗಮ retval;
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
    ಯದಿ (selfdict != ರಿಕ್ತ)
    {
	// Set l:self to "selfdict".  Use "name" to avoid a warning from
	// some compiler that checks the destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ಕೀಲಿ;
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
    ಯದಿ ((fp->uf_flags & FC_NOARGS) == 0)
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var, "0",
				(varnumber_T)(argcount >= fp->uf_args.ga_len
				    ? argcount - fp->uf_args.ga_len : 0));
    fc->fc_l_avars.dv_lock = VAR_FIXED;
    ಯದಿ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	// Use "name" to avoid a warning from some compiler that checks the
	// destination size.
	v = &fc->fc_fixvar[fixvar_idx++].var;
	name = v->di_ಕೀಲಿ;
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
    ಯದಿ ((fp->uf_flags & FC_NOARGS) == 0)
    {
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
			      "firstline", (varnumber_T)funcexe->fe_firstline);
	add_nr_var(&fc->fc_l_avars, &fc->fc_fixvar[fixvar_idx++].var,
				"lastline", (varnumber_T)funcexe->fe_lastline);
    }
    ಪ್ರತಿ (i = 0; i < argcount || i < fp->uf_args.ga_len; ++i)
    {
	ಪೂರ್ಣಾಙ್ಕ	    addlocal = FALSE;
	typval_T    def_rettv;
	ಪೂರ್ಣಾಙ್ಕ	    isdefault = FALSE;

	ai = i - fp->uf_args.ga_len;
	ಯದಿ (ai < 0)
	{
	    // named argument a:name
	    name = FUNCARG(fp, i);
	    ಯದಿ (islambda)
		addlocal = TRUE;

	    // evaluate named argument default expression
	    isdefault = ai + fp->uf_def_args.ga_len >= 0
		       && (i >= argcount || (argvars[i].v_type == VAR_SPECIAL
				   && argvars[i].vval.v_ಸಂಖ್ಯೆ == VVAL_NONE));
	    ಯದಿ (isdefault)
	    {
		char_u	    *default_ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;

		def_rettv.v_type = VAR_ಸಂಖ್ಯೆ;
		def_rettv.vval.v_ಸಂಖ್ಯೆ = -1;

		default_ಅಭಿವ್ಯಕ್ತಿ = ((char_u **)(fp->uf_def_args.ga_data))
						 [ai + fp->uf_def_args.ga_len];
		ಯದಿ (eval1(&default_ಅಭಿವ್ಯಕ್ತಿ, &def_rettv, &EVALARG_EVALUATE) == FAIL)
		{
		    default_arg_err = 1;
		    ಭಙ್ಗ;
		}
	    }

	    namelen = STRLEN(name);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ ((fp->uf_flags & FC_NOARGS) != 0)
		// Bail out if no a: arguments used (in lambda).
		ಭಙ್ಗ;

	    // "..." argument a:1, a:2, etc.
	    namelen = vim_snprintf((ವರ್ಣ *)numbuf, ಪರಿಮಾಣ(numbuf), "%d", ai + 1);
	    name = numbuf;
	}
	ಯದಿ (fixvar_idx < FIXVAR_CNT && namelen <= VAR_SHORT_LEN)
	{
	    v = &fc->fc_fixvar[fixvar_idx++].var;
	    v->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	    STRCPY(v->di_ಕೀಲಿ, name);
	}
	ಅನ್ಯಥಾ
	{
	    v = dictitem_alloc(name);
	    ಯದಿ (v == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    v->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
	}

	// Note: the values are copied directly to avoid alloc/free.
	// "argvars" must have VAR_FIXED for v_lock.
	v->di_tv = isdefault ? def_rettv : argvars[i];
	v->di_tv.v_lock = VAR_FIXED;

	ಯದಿ (isdefault)
	    // Need to free this later, no matter where it's stored.
	    tv_to_free[tv_to_free_len++] = &v->di_tv;

	ಯದಿ (addlocal)
	{
	    // Named arguments should be accessed without the "a:" prefix in
	    // lambda expressions.  Add to the l: dict.
	    copy_tv(&v->di_tv, &v->di_tv);
	    hash_add(&fc->fc_l_vars.dv_hashtab, DI2HIKEY(v), "local variable");
	}
	ಅನ್ಯಥಾ
	    hash_add(&fc->fc_l_avars.dv_hashtab, DI2HIKEY(v), "add variable");

	ಯದಿ (ai >= 0 && ai < MAX_FUNC_ARGS)
	{
	    listitem_T *li = &fc->fc_l_listitems[ai];

	    li->li_tv = argvars[i];
	    li->li_tv.v_lock = VAR_FIXED;
	    list_append(&fc->fc_l_varlist, li);
	}
    }

    // Don't redraw while executing the function.
    ++RedrawingDisabled;

    ಯದಿ (fp->uf_flags & FC_SANDBOX)
    {
	using_sandbox = TRUE;
	++sandbox;
    }

    estack_ತಳ್ಳು_ufunc(fp, 1);
    ESTACK_CHECK_SETUP;
    ಯದಿ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("calling %s"), SOURCING_NAME);
	ಯದಿ (p_verbose >= 14)
	{
	    char_u	buf[ಸಂದೇಶ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    ಸಂದೇಶ_puts("(");
	    ಪ್ರತಿ (i = 0; i < argcount; ++i)
	    {
		ಯದಿ (i > 0)
		    ಸಂದೇಶ_puts(", ");
		ಯದಿ (argvars[i].v_type == VAR_ಸಂಖ್ಯೆ)
		    ಸಂದೇಶ_outnum((long)argvars[i].vval.v_ಸಂಖ್ಯೆ);
		ಅನ್ಯಥಾ
		{
		    // Do not want errors such as E724 here.
		    ++emsg_off;
		    s = tv2string(&argvars[i], &tofree, numbuf2, 0);
		    --emsg_off;
		    ಯದಿ (s != ರಿಕ್ತ)
		    {
			ಯದಿ (vim_strsize(s) > ಸಂದೇಶ_BUF_CLEN)
			{
			    trunc_string(s, buf, ಸಂದೇಶ_BUF_CLEN, ಸಂದೇಶ_BUF_LEN);
			    s = buf;
			}
			ಸಂದೇಶ_puts((ವರ್ಣ *)s);
			vim_free(tofree);
		    }
		}
	    }
	    ಸಂದೇಶ_puts(")");
	}
	ಸಂದೇಶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }
#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	profile_may_start_func(&profile_info, fp,
			fc->fc_caller == ರಿಕ್ತ ? ರಿಕ್ತ : fc->fc_caller->fc_func);
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

    ಯದಿ (default_arg_err && (fp->uf_flags & FC_ABORT || trylevel > 0 ))
    {
	did_emsg = TRUE;
	retval = FCERR_FAILED;
    }
    ಅನ್ಯಥಾ ಯದಿ (islambda)
    {
	char_u *p = *(char_u **)fp->uf_ಸಾಲುಗಳು.ga_data + 7;

	// A Lambda always has the command "return {expr}".  It is much faster
	// to evaluate {expr} directly.
	++ex_nesting_level;
	(ಶೂನ್ಯ)eval1(&p, rettv, &EVALARG_EVALUATE);
	--ex_nesting_level;
    }
    ಅನ್ಯಥಾ
	// call do_cmdline() to execute the lines
	do_cmdline(ರಿಕ್ತ, get_func_ಸಾಲು, (ಶೂನ್ಯ *)fc,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

    // Invoke functions added with ":defer".
    handle_defer_one(current_funccal);

    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    // when the function was aborted because of an error, return -1
    ಯದಿ ((did_emsg && (fp->uf_flags & FC_ABORT)) || rettv->v_type == VAR_UNKNOWN)
    {
	clear_tv(rettv);
	rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;

	// In corner cases returning a "failed" value is not backwards
	// compatible.  Only do this for Vim9 script.
	ಯದಿ (in_vim9script())
	    retval = FCERR_FAILED;
    }

#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = fc->fc_caller == ರಿಕ್ತ ? ರಿಕ್ತ : fc->fc_caller->fc_func;

	ಯದಿ (fp->uf_profiling || (caller != ರಿಕ್ತ && caller->uf_profiling))
	    profile_may_end_func(&profile_info, fp, caller);
    }
#endif

    // when being verbose, mention the return value
    ಯದಿ (p_verbose >= 12)
    {
	++no_wait_return;
	verbose_enter_scroll();

	ಯದಿ (aborting())
	    smsg(_("%s aborted"), SOURCING_NAME);
	ಅನ್ಯಥಾ ಯದಿ (fc->fc_rettv->v_type == VAR_ಸಂಖ್ಯೆ)
	    smsg(_("%s returning #%ld"), SOURCING_NAME,
					    (long)fc->fc_rettv->vval.v_ಸಂಖ್ಯೆ);
	ಅನ್ಯಥಾ
	{
	    char_u	buf[ಸಂದೇಶ_BUF_LEN];
	    char_u	numbuf2[NUMBUFLEN];
	    char_u	*tofree;
	    char_u	*s;

	    // The value may be very long.  Skip the middle part, so that we
	    // have some idea how it starts and ends. smsg() would always
	    // truncate it at the end. Don't want errors such as E724 here.
	    ++emsg_off;
	    s = tv2string(fc->fc_rettv, &tofree, numbuf2, 0);
	    --emsg_off;
	    ಯದಿ (s != ರಿಕ್ತ)
	    {
		ಯದಿ (vim_strsize(s) > ಸಂದೇಶ_BUF_CLEN)
		{
		    trunc_string(s, buf, ಸಂದೇಶ_BUF_CLEN, ಸಂದೇಶ_BUF_LEN);
		    s = buf;
		}
		smsg(_("%s returning %s"), SOURCING_NAME, s);
		vim_free(tofree);
	    }
	}
	ಸಂದೇಶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
    current_sctx = save_current_sctx;
    restore_current_ectx(save_current_ectx);

#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	script_prof_restore(&profile_info.pi_wait_start);
#endif
    ಯದಿ (using_sandbox)
	--sandbox;
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;

    ಯದಿ (p_verbose >= 12 && SOURCING_NAME != ರಿಕ್ತ)
    {
	++no_wait_return;
	verbose_enter_scroll();

	smsg(_("continuing in %s"), SOURCING_NAME);
	ಸಂದೇಶ_puts("\n");   // don't overwrite this either

	verbose_leave_scroll();
	--no_wait_return;
    }

    did_emsg |= save_did_emsg;
    funcdepth_decrement();
    ಪ್ರತಿ (i = 0; i < tv_to_free_len; ++i)
	clear_tv(tv_to_free[i]);
    cleanup_function_call(fc);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Check the argument count for user function "fp".
 * Return FCERR_UNKNOWN if OK, FCERR_TOOFEW or FCERR_TOOMANY otherwise.
 */
    funcerror_T
check_user_func_argcount(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ argcount)
{
    ಪೂರ್ಣಾಙ್ಕ regular_args = fp->uf_args.ga_len;

    ಯದಿ (argcount < regular_args - fp->uf_def_args.ga_len)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOFEW;
    ಅನ್ಯಥಾ ಯದಿ (!has_varargs(fp) && argcount > regular_args)
	ಪ್ರತ್ಯಾಗಮ FCERR_TOOMANY;
    ಪ್ರತ್ಯಾಗಮ FCERR_UNKNOWN;
}

/*
 * Call a user function after checking the arguments.
 */
    funcerror_T
call_user_func_check(
	ufunc_T	    *fp,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	typval_T    *argvars,
	typval_T    *rettv,
	funcexe_T   *funcexe,
	dict_T	    *selfdict)
{
    funcerror_T error = FCERR_NONE;

#ifdef FEAT_LUA
    ಯದಿ (fp->uf_flags & FC_CFUNC)
    {
	cfunc_T cb = fp->uf_cb;

	ಪ್ರತ್ಯಾಗಮ (*cb)(argcount, argvars, rettv, fp->uf_cb_state);
    }
#endif

    ಯದಿ (fp->uf_flags & FC_RANGE && funcexe->fe_doesrange != ರಿಕ್ತ)
	*funcexe->fe_doesrange = TRUE;
    error = check_user_func_argcount(fp, argcount);
    ಯದಿ (error != FCERR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ error;

    ಯದಿ ((fp->uf_flags & FC_DICT) && selfdict == ರಿಕ್ತ)
    {
	error = FCERR_DICT;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ		did_save_redo = FALSE;
	save_redo_T	save_redo;

	/*
	 * Call the user function.
	 * Save and restore search patterns, script variables and
	 * redo buffer.
	 */
	save_ಹುಡುಕು_patterns();
	ಯದಿ (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redo = TRUE;
	}
	++fp->uf_calls;
	error = call_user_func(fp, argcount, argvars, rettv, funcexe,
				   (fp->uf_flags & FC_DICT) ? selfdict : ರಿಕ್ತ);
	ಯದಿ (--fp->uf_calls <= 0 && fp->uf_refcount <= 0)
	    // Function was unreferenced while being used, free it now.
	    func_clear_free(fp, FALSE);
	ಯದಿ (did_save_redo)
	    restoreRedobuff(&save_redo);
	restore_ಹುಡುಕು_patterns();
    }

    ಪ್ರತ್ಯಾಗಮ error;
}

ಸ್ಥಿರ funccal_entry_T *funccal_ರಾಶಿ = ರಿಕ್ತ;

/*
 * Save the current function call pointer, and set it to NULL.
 * Used when executing autocommands and for ":source".
 */
    ಶೂನ್ಯ
save_funccal(funccal_entry_T *entry)
{
    entry->ತುದಿ_funccal = current_funccal;
    entry->next = funccal_ರಾಶಿ;
    funccal_ರಾಶಿ = entry;
    current_funccal = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
restore_funccal(ಶೂನ್ಯ)
{
    ಯದಿ (funccal_ರಾಶಿ == ರಿಕ್ತ)
	internal_error("restore_funccal()");
    ಅನ್ಯಥಾ
    {
	current_funccal = funccal_ರಾಶಿ->ತುದಿ_funccal;
	funccal_ರಾಶಿ = funccal_ರಾಶಿ->next;
    }
}

    funccall_T *
get_current_funccal(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_funccal;
}

/*
 * Return TRUE when currently at the script level:
 * - not in a function
 * - not executing an autocommand
 * Note that when an autocommand sources a script the result is FALSE;
 */
    ಪೂರ್ಣಾಙ್ಕ
at_script_level(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_funccal == ರಿಕ್ತ && autocmd_match == ರಿಕ್ತ;
}

/*
 * Mark all functions of script "sid" as deleted.
 */
    ಶೂನ್ಯ
delete_script_functions(ಪೂರ್ಣಾಙ್ಕ sid)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	todo = 1;
    char_u	buf[30];
    size_t	len;

    buf[0] = K_SPECIAL;
    buf[1] = KS_EXTRA;
    buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
    len = 3 + vim_snprintf((ವರ್ಣ *)buf + 3, ಪರಿಮಾಣ(buf) - 3, "%d_", sid);

    ಯಾವತ್ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		fp = HI2UF(hi);
		ಯದಿ (STRNCMP(fp->uf_name, buf, len) == 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ changed = func_hashtab.ht_changed;

		    fp->uf_flags |= FC_DEAD;

		    ಯದಿ (fp->uf_calls > 0)
		    {
			// Function is executing, don't free it but do remove
			// it from the hashtable.
			ಯದಿ (func_remove(fp))
			    fp->uf_refcount--;
		    }
		    ಅನ್ಯಥಾ
		    {
			func_clear(fp, TRUE);
			// When clearing a function another function can be
			// cleared as a side effect.  When that happens start
			// over.
			ಯದಿ (changed != func_hashtab.ht_changed)
			    ಭಙ್ಗ;
		    }
		}
		--todo;
	    }
    }
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_functions(ಶೂನ್ಯ)
{
    hashitem_T	*hi;
    ufunc_T	*fp;
    long_u	skipped = 0;
    long_u	todo = 1;
    ಪೂರ್ಣಾಙ್ಕ		changed;

    // Clean up the current_funccal chain and the funccal stack.
    ಯಾವತ್ (current_funccal != ರಿಕ್ತ)
    {
	clear_tv(current_funccal->fc_rettv);
	cleanup_function_call(current_funccal);
	ಯದಿ (current_funccal == ರಿಕ್ತ && funccal_ರಾಶಿ != ರಿಕ್ತ)
	    restore_funccal();
    }

    // First clear what the functions contain.  Since this may lower the
    // reference count of a function, it may also free a function and change
    // the hash table. Restart if that happens.
    ಯಾವತ್ (todo > 0)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		// clear the def function index now
		fp = HI2UF(hi);
		fp->uf_flags &= ~FC_DEAD;
		fp->uf_def_status = UF_NOT_COMPILED;

		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		ಯದಿ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ಅನ್ಯಥಾ
		{
		    changed = func_hashtab.ht_changed;
		    func_clear(fp, TRUE);
		    ಯದಿ (changed != func_hashtab.ht_changed)
		    {
			skipped = 0;
			ಭಙ್ಗ;
		    }
		}
		--todo;
	    }
    }

    // Now actually free the functions.  Need to start all over every time,
    // because func_free() may change the hash table.
    skipped = 0;
    ಯಾವತ್ (func_hashtab.ht_used > skipped)
    {
	todo = func_hashtab.ht_used;
	FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&func_hashtab, hi, todo)
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		--todo;
		// Only free functions that are not refcounted, those are
		// supposed to be freed when no longer referenced.
		fp = HI2UF(hi);
		ಯದಿ (func_name_refcount(fp->uf_name))
		    ++skipped;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (func_free(fp, FALSE) == OK)
		    {
			skipped = 0;
			ಭಙ್ಗ;
		    }
		    // did not actually free it
		    ++skipped;
		}
	    }
    }
    ಯದಿ (skipped == 0)
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
    ಪೂರ್ಣಾಙ್ಕ
builtin_function(char_u *name, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (!ASCII_ISLOWER(name[0]) || name[1] == ':')
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (i = 0; name[i] != NUL && (len < 0 || i < len); ++i)
    {
	ಯದಿ (name[i] == AUTOLOAD_CHAR)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (!eval_isnamec(name[i]))
	{
	    // "name.something" is not a builtin function
	    ಯದಿ (name[i] == '.')
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಪೂರ್ಣಾಙ್ಕ
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
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;
    ಪೂರ್ಣಾಙ್ಕ		r = 0;

    CHECK_LIST_MATERIALIZE(l);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, item)
    {
	ಯದಿ (argc == MAX_FUNC_ARGS - (partial == ರಿಕ್ತ ? 0 : partial->pt_argc))
	{
	    emsg(_(e_too_many_arguments));
	    ಭಙ್ಗ;
	}
	// Make a copy of each argument.  This is needed to be able to set
	// v_lock to VAR_FIXED in the copy without changing the original list.
	copy_tv(&item->li_tv, &argv[argc++]);
    }

    ಯದಿ (item == ರಿಕ್ತ)
    {
	funcexe_T	funcexe;
	ಪೂರ್ಣಾಙ್ಕ		namelen = -1;

	ಯದಿ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(name);

	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		ಯದಿ (skip_generic_func_type_args(&p) == FAIL)
		    ಗಚ್ಛ done;
		namelen = p - name + 1;
	    }
	}

	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ಯದಿ (partial != ರಿಕ್ತ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ಯದಿ (funcexe.fe_object != ರಿಕ್ತ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_selfdict = selfdict;
	r = call_func(name, namelen, rettv, argc, argv, &funcexe);
    }

    // Free the arguments.
    ಯಾವತ್ (argc > 0)
	clear_tv(&argv[--argc]);

done:
    ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ callback_depth = 0;

    ಪೂರ್ಣಾಙ್ಕ
get_callback_depth(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ callback_depth;
}

/*
 * Invoke call_func() with a callback.
 * Returns FAIL if the callback could not be called.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_callback(
    callback_T	*callback,
    ಪೂರ್ಣಾಙ್ಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ಪೂರ್ಣಾಙ್ಕ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (callback->cb_name == ರಿಕ್ತ || *callback->cb_name == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (callback_depth > p_mfd)
    {
	emsg(_(e_command_too_recursive));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = callback->cb_partial;
    ಯದಿ (callback->cb_partial != ರಿಕ್ತ)
    {
	funcexe.fe_object = callback->cb_partial->pt_obj;
	ಯದಿ (funcexe.fe_object != ರಿಕ್ತ)
	    ++funcexe.fe_object->obj_refcount;
    }
    ++callback_depth;
    ret = call_func(callback->cb_name, len, rettv, argcount, argvars,
							&funcexe);
    --callback_depth;

    // When a :def function was called that uses :try an error would be turned
    // into an exception.  Need to give the error here.
    ಯದಿ (need_rethrow && current_exception != ರಿಕ್ತ && trylevel == 0)
    {
	need_rethrow = FALSE;
	handle_did_throw();
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * call the 'callback' function and return the result as a number.
 * Returns -2 when calling the function fails.  Uses argv[0] to argv[argc - 1]
 * for the function arguments. argv[argc] should have type VAR_UNKNOWN.
 */
    varnumber_T
call_callback_retnr(
    callback_T	*callback,
    ಪೂರ್ಣಾಙ್ಕ		argcount,	// number of "argvars"
    typval_T	*argvars)	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
{
    typval_T	rettv;
    varnumber_T	retval;

    ಯದಿ (call_callback(callback, -1, &rettv, argcount, argvars) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -2;

    retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, ರಿಕ್ತ);
    clear_tv(&rettv);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Give an error message for the result of a function.
 * Nothing if "error" is FCERR_NONE.
 */
    ಶೂನ್ಯ
user_func_error(funcerror_T error, char_u *name, ಪೂರ್ಣಾಙ್ಕ found_var)
{
    ವಿಕಲ್ಪ (error)
    {
	ಪಕ್ಷ FCERR_UNKNOWN:
		ಯದಿ (found_var)
		    emsg_funcname(e_not_callable_type_str, name);
		ಅನ್ಯಥಾ
		    emsg_funcname(e_unknown_function_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_NOTMETHOD:
		emsg_funcname(e_cannot_use_function_as_method_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_DELETED:
		emsg_funcname(e_function_was_deleted_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_TOOMANY:
		emsg_funcname(e_too_many_arguments_for_function_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_TOOFEW:
		emsg_funcname(e_not_enough_arguments_for_function_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_SCRIPT:
		emsg_funcname(e_using_sid_not_in_script_context_str, name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_DICT:
		emsg_funcname(e_calling_dict_function_without_dictionary_str,
									 name);
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_OTHER:
	ಪಕ್ಷ FCERR_FAILED:
		// assume the error message was already given
		ಭಙ್ಗ;
	ಪಕ್ಷ FCERR_NONE:
		ಭಙ್ಗ;
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
    ಸ್ಥಿರ funcerror_T
may_check_argument_types(
	funcexe_T   *funcexe,
	typval_T    *argvars,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	ಪೂರ್ಣಾಙ್ಕ	    base_included,
	char_u	    *name)
{
    ಯದಿ (funcexe->fe_check_type != ರಿಕ್ತ && funcexe->fe_evaluate)
    {
	// Check that the argument types are OK for the types of the funcref.
	ಯದಿ (check_argument_types(funcexe->fe_check_type,
			  argvars, argcount,
			  base_included ? ರಿಕ್ತ : funcexe->fe_basetv,
			  name) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FCERR_OTHER;
    }
    ಪ್ರತ್ಯಾಗಮ FCERR_NONE;
}

/*
 * Call a function with its resolved parameters
 *
 * Return FAIL when the function can't be called,  OK otherwise.
 * Also returns OK when an error was encountered while executing the function.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_func(
    char_u	*funcname,	// name of the function
    ಪೂರ್ಣಾಙ್ಕ		len,		// length of "name" or -1 to use strlen()
    typval_T	*rettv,		// return value goes here
    ಪೂರ್ಣಾಙ್ಕ		argcount_in,	// number of "argvars"
    typval_T	*argvars_in,	// vars for arguments, must have "argcount"
				// PLUS ONE elements!
    funcexe_T	*funcexe)	// more arguments
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ufunc_T	*fp = ರಿಕ್ತ;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ರಿಕ್ತ;
    char_u	*fname = ರಿಕ್ತ;
    char_u	*name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argcount = argcount_in;
    typval_T	*argvars = argvars_in;
    dict_T	*selfdict = funcexe->fe_selfdict;
    typval_T	argv[MAX_FUNC_ARGS + 1]; // used when "partial" or
					 // "funcexe->fe_basetv" is not NULL
    ಪೂರ್ಣಾಙ್ಕ		argv_clear = 0;
    ಪೂರ್ಣಾಙ್ಕ		argv_base = 0;
    partial_T	*partial = funcexe->fe_partial;
    type_T	check_type;
    type_T	*check_type_args[MAX_FUNC_ARGS];
    gfargs_tab_T gfatab;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when call_func() returns FAIL.
    rettv->v_type = VAR_UNKNOWN;

    generic_func_args_table_init(&gfatab);

    ಯದಿ (partial != ರಿಕ್ತ)
	fp = partial->pt_func;
    ಯದಿ (fp == ರಿಕ್ತ)
	fp = funcexe->fe_ufunc;

    ಯದಿ (fp == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	{
	    char_u *p = generic_func_find_open_bracket(funcname);

	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		len = p - funcname;
		ಯದಿ (parse_generic_func_type_args(funcname, len, p, &gfatab,
						funcexe->fe_cctx) == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
	    }
	}

	// Make a copy of the name, if it comes from a funcref variable it
	// could be changed or deleted in the called function.
	name = len > 0 ? vim_strnsave(funcname, len) : vim_strsave(funcname);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
    }

    ಯದಿ (funcexe->fe_doesrange != ರಿಕ್ತ)
	*funcexe->fe_doesrange = FALSE;

    ಯದಿ (partial != ರಿಕ್ತ)
    {
	// When the function has a partial with a dict and there is a dict
	// argument, use the dict argument.  That is backwards compatible.
	// When the dict was bound explicitly use the one from the partial.
	ಯದಿ (partial->pt_dict != ರಿಕ್ತ && (selfdict == ರಿಕ್ತ || !partial->pt_auto))
	    selfdict = partial->pt_dict;
	ಯದಿ (error == FCERR_NONE && partial->pt_argc > 0)
	{
	    ಪ್ರತಿ (argv_clear = 0; argv_clear < partial->pt_argc; ++argv_clear)
	    {
		ಯದಿ (argv_clear + argcount_in >= MAX_FUNC_ARGS)
		{
		    error = FCERR_TOOMANY;
		    ಗಚ್ಛ theend;
		}
		copy_tv(&partial->pt_argv[argv_clear], &argv[argv_clear]);
	    }
	    ಪ್ರತಿ (i = 0; i < argcount_in; ++i)
		argv[i + argv_clear] = argvars_in[i];
	    argvars = argv;
	    argcount = partial->pt_argc + argcount_in;

	    ಯದಿ (funcexe->fe_check_type != ರಿಕ್ತ
				  && funcexe->fe_check_type->tt_argcount != -1)
	    {
		// Now funcexe->fe_check_type is missing the added arguments,
		// make a copy of the type with the correction.
		check_type = *funcexe->fe_check_type;
		funcexe->fe_check_type = &check_type;
		check_type.tt_args = check_type_args;
		CLEAR_FIELD(check_type_args);
		ಪ್ರತಿ (i = 0; i < check_type.tt_argcount; ++i)
		    check_type_args[i + partial->pt_argc] =
							 check_type.tt_args[i];
		check_type.tt_argcount += partial->pt_argc;
		check_type.tt_min_argcount += partial->pt_argc;
	    }
	}
    }

    ಯದಿ (error == FCERR_NONE)
	// check the argument types if possible
	error = may_check_argument_types(funcexe, argvars, argcount, FALSE,
					     (name != ರಿಕ್ತ) ? name : funcname);

    ಯದಿ (error == FCERR_NONE && funcexe->fe_evaluate)
    {
	char_u *rfname = fname;
	ಪೂರ್ಣಾಙ್ಕ	is_global = FALSE;

	// Skip "g:" before a function name.
	ಯದಿ (fp == ರಿಕ್ತ && fname[0] == 'g' && fname[1] == ':')
	{
	    is_global = TRUE;
	    rfname = fname + 2;
	}

	rettv->v_type = VAR_ಸಂಖ್ಯೆ;	// default rettv is number zero
	rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	error = FCERR_UNKNOWN;

	ಯದಿ (fp != ರಿಕ್ತ || !builtin_function(rfname, -1))
	{
	    /*
	     * User defined function.
	     */
	    ಯದಿ (fp == ರಿಕ್ತ)
	    {
		fp = find_func(rfname, is_global);
		ಯದಿ (fp != ರಿಕ್ತ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
		    // In Vim9 script g: is required to find a global
		    // non-autoload function.
		    fp = ರಿಕ್ತ;
	    }

	    // Trigger FuncUndefined event, may load the function.
	    ಯದಿ (fp == ರಿಕ್ತ
		    && apply_autocmds(EVENT_FUNCUNDEFINED,
						    rfname, rfname, TRUE, ರಿಕ್ತ)
		    && !aborting())
	    {
		// executed an autocommand, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    // Try loading a package.
	    ಯದಿ (fp == ರಿಕ್ತ && script_autoload(rfname, TRUE) && !aborting())
	    {
		// loaded a package, search for the function again
		fp = find_func(rfname, is_global);
	    }
	    ಯದಿ (fp == ರಿಕ್ತ)
	    {
		char_u *p = untrans_function_name(rfname);

		// If using Vim9 script try not local to the script.
		// Don't do this if the name starts with "s:".
		ಯದಿ (p != ರಿಕ್ತ && (funcname[0] != 's' || funcname[1] != ':'))
		    fp = find_func(p, is_global);
	    }

	    ಯದಿ (partial == ರಿಕ್ತ && fp != ರಿಕ್ತ && IS_GENERIC_FUNC(fp))
	    {
		// generic function call
		fp = generic_func_get(fp, &gfatab);
		ಯದಿ (fp == ರಿಕ್ತ)
		{
		    error = FCERR_FAILED;
		    ಗಚ್ಛ theend;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (generic_func_args_table_size(&gfatab) > 0)
	    {
		emsg_funcname(fp != ರಿಕ್ತ ? e_not_a_generic_function_str
				: e_unknown_generic_function_str, rfname);
		error = FCERR_FAILED;
		ಗಚ್ಛ theend;
	    }

	    ಯದಿ (fp != ರಿಕ್ತ && (fp->uf_flags & FC_DELETED))
		error = FCERR_DELETED;
	    ಅನ್ಯಥಾ ಯದಿ (fp != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ need_arg_check = FALSE;
		ಯದಿ (funcexe->fe_check_type == ರಿಕ್ತ)
		{
		    funcexe->fe_check_type = fp->uf_func_type;
		    need_arg_check = TRUE;
		}

		ಯದಿ (funcexe->fe_argv_func != ರಿಕ್ತ)
		{
		    // postponed filling in the arguments, do it now
		    argcount = funcexe->fe_argv_func(argcount, argvars,
							       argv_clear, fp);
		    need_arg_check = TRUE;
		}

		ಯದಿ (funcexe->fe_basetv != ರಿಕ್ತ)
		{
		    // Method call: base->Method()
		    mch_memmove(&argv[1], argvars, ಪರಿಮಾಣ(typval_T) * argcount);
		    argv[0] = *funcexe->fe_basetv;
		    argcount++;
		    argvars = argv;
		    argv_base = 1;
		    need_arg_check = TRUE;
		}

		// Check the argument types now that the function type and all
		// argument values are known, if not done above.
		ಯದಿ (need_arg_check)
		    error = may_check_argument_types(funcexe, argvars, argcount,
				       TRUE, (name != ರಿಕ್ತ) ? name : funcname);

		ಯದಿ (error == FCERR_NONE || error == FCERR_UNKNOWN)
		    error = call_user_func_check(fp, argcount, argvars, rettv,
							    funcexe, selfdict);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (funcexe->fe_basetv != ರಿಕ್ತ)
	{
	    /*
	     * expr->method(): Find the method name in the table, call its
	     * implementation with the base as one of the arguments.
	     */
	    error = call_internal_method(fname, argcount, argvars, rettv,
							   funcexe->fe_basetv);
	}
	ಅನ್ಯಥಾ
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
    ಯದಿ (error == FCERR_NONE)
	ret = OK;

theend:
    /*
     * Report an error unless the argument evaluation or function call has been
     * cancelled due to an aborting error, an interrupt, or an exception.
     */
    ಯದಿ (!aborting())
	user_func_error(error, (name != ರಿಕ್ತ) ? name : funcname,
							funcexe->fe_found_var);

    // clear the copies made from the partial
    ಯಾವತ್ (argv_clear > 0)
	clear_tv(&argv[--argv_clear + argv_base]);

    vim_free(tofree);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Call a function without arguments, partial or dict.
 * This is like call_func() when the call is only "FuncName()".
 * To be used by "expr" options.
 * Returns NOTDONE when the function could not be found.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_simple_func(
    char_u	*funcname,	// name of the function
    size_t	len,		// length of "name"
    typval_T	*rettv)		// return value goes here
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ರಿಕ್ತ;
    char_u	*name;
    char_u	*fname;
    char_u	*rfname;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    ufunc_T	*fp;

    rettv->v_type = VAR_ಸಂಖ್ಯೆ;	// default rettv is number zero
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;

    // Make a copy of the name, an option can be changed in the function.
    name = vim_strnsave(funcname, len);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ret;

    fname = fname_trans_sid(name, fname_buf, &tofree, &error);

    // Skip "g:" before a function name.
    ಯದಿ (fname[0] == 'g' && fname[1] == ':')
    {
	is_global = TRUE;
	rfname = fname + 2;
    }
    ಅನ್ಯಥಾ
	rfname = fname;
    fp = find_func(rfname, is_global);
    ಯದಿ (fp != ರಿಕ್ತ && !is_global && in_vim9script()
						 && func_requires_g_prefix(fp))
	// In Vim9 script g: is required to find a global non-autoload
	// function.
	fp = ರಿಕ್ತ;
    ಯದಿ (fp == ರಿಕ್ತ)
	ret = NOTDONE;
    ಅನ್ಯಥಾ ಯದಿ (fp != ರಿಕ್ತ && (fp->uf_flags & FC_DELETED))
	error = FCERR_DELETED;
    ಅನ್ಯಥಾ ಯದಿ (fp != ರಿಕ್ತ)
    {
	typval_T argvars[1];
	funcexe_T	funcexe;

	argvars[0].v_type = VAR_UNKNOWN;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	error = call_user_func_check(fp, 0, argvars, rettv, &funcexe, ರಿಕ್ತ);
	ಯದಿ (error == FCERR_NONE)
	    ret = OK;
    }

    user_func_error(error, name, FALSE);
    vim_free(tofree);
    vim_free(name);

    ಪ್ರತ್ಯಾಗಮ ret;
}

    char_u *
printable_func_name(ufunc_T *fp)
{
    ಪ್ರತ್ಯಾಗಮ fp->uf_name_exp != ರಿಕ್ತ ? fp->uf_name_exp : fp->uf_name;
}

/*
 * When "prev_ht_changed" does not equal "ht_changed" give an error and return
 * TRUE.  Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
function_list_modified(ಪೂರ್ಣಾಙ್ಕ prev_ht_changed)
{
    ಯದಿ (prev_ht_changed != func_hashtab.ht_changed)
    {
	emsg(_(e_function_list_was_modified));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * List the head of the function: "function name(arg1, arg2)".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list_func_head(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ indent)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_ht_changed = func_hashtab.ht_changed;
    ಪೂರ್ಣಾಙ್ಕ		j;

    ಸಂದೇಶ_start();

    // a timer at the more prompt may have deleted the function
    ಯದಿ (function_list_modified(prev_ht_changed))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (indent)
	ಸಂದೇಶ_puts("   ");
    ಯದಿ (fp->uf_def_status != UF_NOT_COMPILED)
	ಸಂದೇಶ_puts("def ");
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts("function ");
    ಸಂದೇಶ_puts((ವರ್ಣ *)printable_func_name(fp));
    ಸಂದೇಶ_putchar('(');
    ಪ್ರತಿ (j = 0; j < fp->uf_args.ga_len; ++j)
    {
	ಯದಿ (j)
	    ಸಂದೇಶ_puts(", ");
	ಸಂದೇಶ_puts((ವರ್ಣ *)FUNCARG(fp, j));
	ಯದಿ (fp->uf_arg_types != ರಿಕ್ತ)
	{
	    ವರ್ಣ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ಯದಿ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	{
	    ಸಂದೇಶ_puts(" = ");
	    ಸಂದೇಶ_puts(((ವರ್ಣ **)(fp->uf_def_args.ga_data))
		       [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
	}
    }
    ಯದಿ (fp->uf_varargs)
    {
	ಯದಿ (j)
	    ಸಂದೇಶ_puts(", ");
	ಸಂದೇಶ_puts("...");
    }
    ಯದಿ (fp->uf_va_name != ರಿಕ್ತ)
    {
	ಯದಿ (!fp->uf_varargs)
	{
	    ಯದಿ (j)
		ಸಂದೇಶ_puts(", ");
	    ಸಂದೇಶ_puts("...");
	}
	ಸಂದೇಶ_puts((ವರ್ಣ *)fp->uf_va_name);
	ಯದಿ (fp->uf_va_type != ರಿಕ್ತ)
	{
	    ವರ್ಣ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಸಂದೇಶ_putchar(')');

    ಯದಿ (fp->uf_def_status != UF_NOT_COMPILED)
    {
	ಯದಿ (fp->uf_ret_type != &t_void)
	{
	    ವರ್ಣ *tofree;

	    ಸಂದೇಶ_puts(": ");
	    ಸಂದೇಶ_puts(type_name(fp->uf_ret_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (fp->uf_flags & FC_ABORT)
	ಸಂದೇಶ_puts(" abort");
    ಯದಿ (fp->uf_flags & FC_RANGE)
	ಸಂದೇಶ_puts(" range");
    ಯದಿ (fp->uf_flags & FC_DICT)
	ಸಂದೇಶ_puts(" dict");
    ಯದಿ (fp->uf_flags & FC_CLOSURE)
	ಸಂದೇಶ_puts(" closure");
    ಸಂದೇಶ_clr_eos();
    ಯದಿ (p_verbose > 0)
	last_set_ಸಂದೇಶ(fp->uf_script_ctx);

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		*is_global,
    ಪೂರ್ಣಾಙ್ಕ		skip,		// only find the end, don't evaluate
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪ್ರತ್ಯಾಗಮ trans_function_name_ext(pp, is_global, skip, flags,
	    ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * trans_function_name() with extra arguments.
 * "fdp", "partial", "type" and "ufunc" can be NULL.
 */
    ಸ್ಥಿರ char_u *
trans_function_name_ext(
    char_u	**pp,
    ಪೂರ್ಣಾಙ್ಕ		*is_global,
    ಪೂರ್ಣಾಙ್ಕ		skip,		// only find the end, don't evaluate
    ಪೂರ್ಣಾಙ್ಕ		flags,
    funcdict_T	*fdp,		// return: info about dictionary used
    partial_T	**partial,	// return: partial of a FuncRef
    type_T	**type,		// return: type of funcref
    ufunc_T	**ufunc)	// return: function
{
    char_u	*name = ರಿಕ್ತ;
    char_u	*start;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		lead;
    char_u	sid_buf[20];
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		prefix_g = FALSE;
    lval_T	lv;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		vim9_local;

    ಯದಿ (fdp != ರಿಕ್ತ)
	CLEAR_POINTER(fdp);
    start = *pp;

    // Check for hard coded <SNR>: already translated function ID (from a user
    // command).
    ಯದಿ ((*pp)[0] == K_SPECIAL && (*pp)[1] == KS_EXTRA
						   && (*pp)[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_SNR)
    {
	*pp += 3;
	len = get_id_len(pp) + 3;
	ಪ್ರತ್ಯಾಗಮ vim_strnsave(start, len);
    }

    // A name starting with "<SID>" or "<SNR>" is local to a script.  But
    // don't skip over "s:", get_lval() needs it for "s:dict.func".
    lead = eval_fname_script(start);
    ಯದಿ (lead > 2)
	start += lead;

    // Note that TFN_ flags use the same values as GLV_ flags.
    end = get_lval(start, ರಿಕ್ತ, &lv, FALSE, skip,
			flags | GLV_READ_ONLY | GLV_PREFER_FUNC,
					      lead > 2 ? 0 : FNE_CHECK_START);
    ಯದಿ (end == start || (vim9script && end != ರಿಕ್ತ
				   && end[-1] == AUTOLOAD_CHAR && *end == '('))
    {
	ಯದಿ (!skip)
	    emsg(_(e_function_name_required));
	ಗಚ್ಛ theend;
    }
    ಯದಿ (end == ರಿಕ್ತ || (lv.ll_tv != ರಿಕ್ತ && (lead > 2 || lv.ll_range)))
    {
	/*
	 * Report an invalid expression in braces, unless the expression
	 * evaluation has been cancelled due to an aborting error, an
	 * interrupt, or an exception.
	 */
	ಯದಿ (!aborting())
	{
	    ಯದಿ (end != ರಿಕ್ತ)
		semsg(_(e_invalid_argument_str), start);
	}
	ಅನ್ಯಥಾ
	    *pp = find_name_end(start, ರಿಕ್ತ, ರಿಕ್ತ, FNE_INCL_BR);
	ಗಚ್ಛ theend;
    }

    ಯದಿ (lv.ll_ufunc != ರಿಕ್ತ)
    {
	ಯದಿ (ufunc != ರಿಕ್ತ)
	    *ufunc = lv.ll_ufunc;
	name = vim_strnsave(lv.ll_ufunc->uf_name, lv.ll_ufunc->uf_namelen);
	*pp = end;
	ಗಚ್ಛ theend;
    }

    ಯದಿ (lv.ll_tv != ರಿಕ್ತ)
    {
	ಯದಿ (fdp != ರಿಕ್ತ)
	{
	    fdp->fd_dict = lv.ll_dict;
	    fdp->fd_newkey = lv.ll_newkey;
	    lv.ll_newkey = ರಿಕ್ತ;
	    fdp->fd_di = lv.ll_di;
	}
	ಯದಿ (lv.ll_tv->v_type == VAR_FUNC && lv.ll_tv->vval.v_string != ರಿಕ್ತ)
	{
	    name = vim_strsave(lv.ll_tv->vval.v_string);
	    *pp = end;
	}
	ಅನ್ಯಥಾ ಯದಿ (lv.ll_tv->v_type == VAR_CLASS
					     && lv.ll_tv->vval.v_class != ರಿಕ್ತ)
	{
	    name = vim_strnsave(lv.ll_tv->vval.v_class->class_name.string,
		lv.ll_tv->vval.v_class->class_name.length);
	    *pp = end;
	}
	ಅನ್ಯಥಾ ಯದಿ (lv.ll_tv->v_type == VAR_PARTIAL
					  && lv.ll_tv->vval.v_partial != ರಿಕ್ತ)
	{
	    name = vim_strsave(partial_name(lv.ll_tv->vval.v_partial));
	    *pp = end;
	    ಯದಿ (partial != ರಿಕ್ತ)
		*partial = lv.ll_tv->vval.v_partial;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!skip && !(flags & TFN_QUIET) && (fdp == ರಿಕ್ತ
			     || lv.ll_dict == ರಿಕ್ತ || fdp->fd_newkey == ರಿಕ್ತ))
		emsg(_(e_funcref_required));
	    ಅನ್ಯಥಾ
		*pp = end;
	    name = ರಿಕ್ತ;
	}
	ಗಚ್ಛ theend;
    }

    ಯದಿ (lv.ll_name == ರಿಕ್ತ)
    {
	// Error found, but continue after the function name.
	*pp = end;
	ಗಚ್ಛ theend;
    }

    // Check if the name is a Funcref.  If so, use the value.
    ಯದಿ (lv.ll_exp_name != ರಿಕ್ತ)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(lv.ll_exp_name);
	name = deref_func_name(lv.ll_exp_name, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ರಿಕ್ತ);
	ಯದಿ (name == lv.ll_exp_name)
	    name = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (lv.ll_sid > 0)
    {
	scriptitem_T	*si = SCRIPT_ITEM(lv.ll_sid);
	ಪೂರ್ಣಾಙ್ಕ		cc = *lv.ll_name_end;

	// function in another script.  Prefix <SNR>99_ or the autoload prefix.
	*lv.ll_name_end = NUL;
	ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ)
	{
	    name = concat_str(si->sn_autoload_prefix, lv.ll_name);
	}
	ಅನ್ಯಥಾ
	{
	    sid_buf[0] = K_SPECIAL;
	    sid_buf[1] = KS_EXTRA;
	    sid_buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
	    vim_snprintf((ವರ್ಣ *)sid_buf + 3, ಪರಿಮಾಣ(sid_buf) - 3,
						      "%ld_", (long)lv.ll_sid);
	    name = concat_str(sid_buf, lv.ll_name);
	}
	*lv.ll_name_end = cc;
	*pp = end;
	ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ ಯದಿ (!(flags & TFN_NO_DEREF))
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)(end - *pp);
	name = deref_func_name(*pp, &len, partial, type,
			  flags & TFN_NO_AUTOLOAD, flags & TFN_NEW_FUNC, ರಿಕ್ತ);
	ಯದಿ (name == *pp)
	    name = ರಿಕ್ತ;
    }
    ಯದಿ (name != ರಿಕ್ತ)
    {
	name = vim_strsave(name);
	*pp = end;
	ಯದಿ (name != ರಿಕ್ತ && STRNCMP(name, "<SNR>", 5) == 0)
	{
	    // Change "<SNR>" to the byte sequence.
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
	    mch_memmove(name + 3, name + 5, STRLEN(name + 5) + 1);
	}
	ಗಚ್ಛ theend;
    }

    ಯದಿ (lv.ll_exp_name != ರಿಕ್ತ)
    {
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(lv.ll_exp_name);
	ಯದಿ (lead <= 2 && lv.ll_name == lv.ll_exp_name
					 && STRNCMP(lv.ll_name, "s:", 2) == 0)
	{
	    // When there was "s:" already or the name expanded to get a
	    // leading "s:" then remove it.
	    lv.ll_name += 2;
	    len -= 2;
	    lead = 2;
	}
    }
    ಅನ್ಯಥಾ
    {
	// skip over "s:" and "g:"
	ಯದಿ (lead == 2 || (lv.ll_name[0] == 'g' && lv.ll_name[1] == ':'))
	{
	    ಯದಿ (lv.ll_name[0] == 'g')
	    {
		ಯದಿ (is_global != ರಿಕ್ತ)
		{
		    *is_global = TRUE;
		}
		ಅನ್ಯಥಾ
		{
		    // dropping "g:" without setting "is_global" won't work in
		    // Vim9 script, put it back later
		    prefix_g = TRUE;
		    extra = 2;
		}
	    }
	    lv.ll_name += 2;
	}
	len = (ಪೂರ್ಣಾಙ್ಕ)(end - lv.ll_name);
    }
    ಯದಿ (len <= 0)
    {
	ಯದಿ (!skip)
	    emsg(_(e_function_name_required));
	ಗಚ್ಛ theend;
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
    ಯದಿ (skip)
	lead = 0;	// do nothing
    ಅನ್ಯಥಾ ಯದಿ (lead > 0 || vim9_local)
    {
	ಯದಿ (!vim9_local)
	{
	    ಯದಿ (vim9script && lead == 2 && !ASCII_ISUPPER(*lv.ll_name)
						   && current_script_is_vim9())
	    {
		semsg(_(e_function_name_must_start_with_capital_str), start);
		ಗಚ್ಛ theend;
	    }
	    lead = 3;
	}
	ಯದಿ (vim9_local || (lv.ll_exp_name != ರಿಕ್ತ
					     && eval_fname_sid(lv.ll_exp_name))
						       || eval_fname_sid(*pp))
	{
	    size_t  sid_buflen;

	    // It's script-local, "s:" or "<SID>"
	    ಯದಿ (current_sctx.sc_sid <= 0)
	    {
		emsg(_(e_using_sid_not_in_script_context));
		ಗಚ್ಛ theend;
	    }
	    sid_buflen = vim_snprintf((ವರ್ಣ *)sid_buf, ಪರಿಮಾಣ(sid_buf), "%ld_", (long)current_sctx.sc_sid);
	    ಯದಿ (vim9_local)
		extra = 3 + (ಪೂರ್ಣಾಙ್ಕ)sid_buflen;
	    ಅನ್ಯಥಾ
		lead += (ಪೂರ್ಣಾಙ್ಕ)sid_buflen;
	}
    }
    // The function name must start with an upper case letter (unless it is a
    // Vim9 class new() function or a Vim9 class private method or one of the
    // supported Vim9 object builtin functions)
    ಅನ್ಯಥಾ ಯದಿ (!(flags & TFN_INT)
	    && (builtin_function(lv.ll_name, len)
				   || (vim9script && *lv.ll_name == '_'))
	    && !((flags & TFN_IN_CLASS)
		&& (is_valid_builtin_obj_methodname(lv.ll_name)
		    || (*lv.ll_name == '_'))))
    {
	semsg(_(vim9script ? e_function_name_must_start_with_capital_str
			   : e_function_name_must_start_with_capital_or_s_str),
									start);
	ಗಚ್ಛ theend;
    }
    ಯದಿ (!skip && !(flags & TFN_QUIET) && !(flags & TFN_NO_DEREF))
    {
	char_u *cp = vim_strchr(lv.ll_name, ':');

	ಯದಿ (cp != ರಿಕ್ತ && cp < end)
	{
	    semsg(_(e_function_name_cannot_contain_colon_str), start);
	    ಗಚ್ಛ theend;
	}
    }

    name = alloc(len + lead + extra + 1);
    ಯದಿ (name != ರಿಕ್ತ)
    {
	ಯದಿ (!skip && (lead > 0 || vim9_local))
	{
	    name[0] = K_SPECIAL;
	    name[1] = KS_EXTRA;
	    name[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
	    ಯದಿ (vim9_local || lead > 3)	// If it's "<SID>"
		STRCPY(name + 3, sid_buf);
	}
	ಅನ್ಯಥಾ ಯದಿ (prefix_g)
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
    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * Assuming "name" is the result of trans_function_name() and it was prefixed
 * to use the script-local name, return the unmodified name (points into
 * "name").  Otherwise return NULL.
 * This can be used to first search for a script-local function and fall back
 * to the global function if not found.
 */
    ಸ್ಥಿರ char_u *
untrans_function_name(char_u *name)
{
    char_u *p;

    ಯದಿ (*name == K_SPECIAL && in_vim9script())
    {
	p = vim_strchr(name, '_');
	ಯದಿ (p != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ p + 1;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ವರ್ಣ	sid_buf[25];
    size_t	sid_buflen;
    ಪೂರ್ಣಾಙ್ಕ		off;
    char_u	*newname;
    size_t	newnamesize;
    char_u	*p = funcname;

    ಯದಿ (funcname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (STRNCMP(funcname, "s:", 2) != 0
		&& STRNCMP(funcname, "<SID>", 5) != 0)
    {
	ufunc_T	    *ufunc;

	// The function name does not have a script-local prefix.  Try finding
	// it when in a Vim9 script and there is no "g:" prefix.
	ಯದಿ (!in_vim9script() || STRNCMP(funcname, "g:", 2) == 0)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ufunc = find_func(funcname, FALSE);
	ಯದಿ (ufunc == ರಿಕ್ತ || func_is_global(ufunc)
			      || (p = vim_strchr(ufunc->uf_name, '_')) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	++p;
	off = 0;
    }
    ಅನ್ಯಥಾ
	off = *funcname == 's' ? 2 : 5;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	emsg(_(e_using_sid_not_in_script_context));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // Expand s: prefix into <SNR>nr_<name>
    sid_buflen = vim_snprintf(sid_buf, ಪರಿಮಾಣ(sid_buf), "<SNR>%ld_",
	    (long)current_sctx.sc_sid);
    newnamesize = sid_buflen + STRLEN(p + off) + 1;
    newname = alloc(newnamesize);
    ಯದಿ (newname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    vim_snprintf((ವರ್ಣ *)newname, newnamesize, "%s%s", sid_buf, p + off);

    ಪ್ರತ್ಯಾಗಮ newname;
}

/*
 * Return script-local "fname" with the 3-byte sequence replaced by
 * printable <SNR> in allocated memory.
 */
    char_u *
alloc_printable_func_name(char_u *fname)
{
    char_u *n = alloc(STRLEN(fname + 3) + 6);

    ಯದಿ (n != ರಿಕ್ತ)
    {
	STRCPY(n, "<SNR>");
	STRCPY(n + 5, fname + 3);
    }
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Call trans_function_name(), except that a lambda is returned as-is.
 * Returns the name in allocated memory.
 */
    char_u *
save_function_name(
	char_u	    **name,
	ಪೂರ್ಣಾಙ್ಕ	    *is_global,
	ಪೂರ್ಣಾಙ್ಕ	    skip,
	ಪೂರ್ಣಾಙ್ಕ	    flags,
	funcdict_T  *fudi)
{
    char_u *p = *name;
    char_u *saved;

    ಯದಿ (STRNCMP(p, "<lambda>", 8) == 0)
    {
	p += 8;
	(ಶೂನ್ಯ)getdigits(&p);
	saved = vim_strnsave(*name, p - *name);
	ಯದಿ (fudi != ರಿಕ್ತ)
	    CLEAR_POINTER(fudi);
    }
    ಅನ್ಯಥಾ
	saved = trans_function_name_ext(&p, is_global, skip,
						flags, fudi, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    *name = p;
    ಪ್ರತ್ಯಾಗಮ saved;
}

/*
 * List functions.  When "regmatch" is NULL all of then.
 * Otherwise functions matching "regmatch".
 */
    ಶೂನ್ಯ
list_functions(regmatch_T *regmatch)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_ht_changed = func_hashtab.ht_changed;
    long_u	todo = func_hashtab.ht_used;
    hashitem_T	*hi;

    ಪ್ರತಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    ufunc_T	*fp = HI2UF(hi);

	    --todo;
	    ಯದಿ ((fp->uf_flags & FC_DEAD) == 0
		    && (regmatch == ರಿಕ್ತ
			? !message_filtered(fp->uf_name)
			    && !func_name_refcount(fp->uf_name)
			: !SAFE_isdigit(*fp->uf_name)
			    && vim_regexec(regmatch, fp->uf_name, 0)))
	    {
		ಯದಿ (list_func_head(fp, FALSE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ;
		ಯದಿ (function_list_modified(prev_ht_changed))
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }
}

/*
 * ":function /pat": list functions matching pattern.
 */
    ಸ್ಥಿರ char_u *
list_functions_matching_pat(exarg_T *eap)
{
    char_u	*p;
    char_u	c;

    p = skip_regexp(eap->arg + 1, '/', TRUE);
    ಯದಿ (!eap->skip)
    {
	regmatch_T	regmatch;

	c = *p;
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg + 1, RE_MAGIC);
	*p = c;
	ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
	{
	    regmatch.rm_ic = p_ic;
	    list_functions(&regmatch);
	    vim_regfree(regmatch.regprog);
	}
    }
    ಯದಿ (*p == '/')
	++p;

    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * List function "name".
 * Returns the function pointer or NULL on failure.
 */
    ಸ್ಥಿರ ufunc_T *
list_one_function(exarg_T *eap, char_u *name, char_u *p, ಪೂರ್ಣಾಙ್ಕ is_global)
{
    ufunc_T	*fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		j;

    ಯದಿ (!ends_excmd(*skipwhite(p)))
    {
	semsg(_(e_trailing_characters_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    set_nextcmd(eap, p);

    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
	*p = NUL;

    ಯದಿ (eap->skip || got_int)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    fp = find_func(name, is_global);
    ಯದಿ (fp == ರಿಕ್ತ && ASCII_ISUPPER(*eap->arg))
    {
	char_u *up = untrans_function_name(name);

	// With Vim9 script the name was made script-local, if not
	// found try again with the original name.
	ಯದಿ (up != ರಿಕ್ತ)
	    fp = find_func(up, FALSE);
    }

    ಯದಿ (fp == ರಿಕ್ತ)
    {
	emsg_funcname(e_undefined_function_str, eap->arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Check no function was added or removed from a timer, e.g. at
    // the more prompt.  "fp" may then be invalid.
    ಪೂರ್ಣಾಙ್ಕ prev_ht_changed = func_hashtab.ht_changed;

    ಯದಿ (list_func_head(fp, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ fp;

    ಪ್ರತಿ (j = 0; j < fp->uf_ಸಾಲುಗಳು.ga_len && !got_int; ++j)
    {
	ಯದಿ (FUNCLINE(fp, j) == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_outnum((long)(j + 1));
	ಯದಿ (j < 9)
	    ಸಂದೇಶ_putchar(' ');
	ಯದಿ (j < 99)
	    ಸಂದೇಶ_putchar(' ');
	ಯದಿ (function_list_modified(prev_ht_changed))
	    ಭಙ್ಗ;
	ಸಂದೇಶ_prt_ಸಾಲು(FUNCLINE(fp, j), FALSE);
	out_flush();	// show a line at a time
	ui_breakcheck();
    }

    ಯದಿ (!got_int)
    {
	ಸಂದೇಶ_putchar('\n');
	ಯದಿ (!function_list_modified(prev_ht_changed))
	{
	    ಯದಿ (fp->uf_def_status != UF_NOT_COMPILED)
		ಸಂದೇಶ_puts("   enddef");
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts("   endfunction");
	}
    }

    ಪ್ರತ್ಯಾಗಮ fp;
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
	garray_T    *ಸಾಲುಗಳು_to_free,
	ಪೂರ್ಣಾಙ್ಕ	    class_flags,
	ocmember_T  *obj_members,
	ಪೂರ್ಣಾಙ್ಕ         obj_member_ಎಣಿಕೆ,
	cctx_T	    *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		j;
    ಪೂರ್ಣಾಙ್ಕ		saved_did_emsg = FALSE;
    char_u	*name = name_arg;
    size_t	namelen = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    char_u	*p;
    char_u	*arg;
    char_u	*whitep;
    char_u	*ಸಾಲು_arg = ರಿಕ್ತ;
    garray_T	newargs;
    garray_T	argtypes;
    garray_T	arg_objm;
    garray_T	default_args;
    garray_T	newlines;
    gfargs_tab_T gfatab;
    ಪೂರ್ಣಾಙ್ಕ		varargs = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    char_u	*ret_type = ರಿಕ್ತ;
    ufunc_T	*fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		fp_allocated = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		free_fp = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		overwrite = FALSE;
    dictitem_T	*v;
    funcdict_T	fudi;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	func_nr = 0;	    // number for nameless function
    ಪೂರ್ಣಾಙ್ಕ		paren;
    hashitem_T	*hi;
    linenr_T	sourcing_lnum_ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    imported_T	*import = ರಿಕ್ತ;

    // ":function" without argument: list functions.
    ಯದಿ (ends_excmd2(eap->cmd, eap->arg))
    {
	ಯದಿ (!eap->skip)
	    list_functions(ರಿಕ್ತ);
	set_nextcmd(eap, eap->arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /*
     * ":function /pat": list functions matching pattern.
     */
    ಯದಿ (*eap->arg == '/')
    {
	p = list_functions_matching_pat(eap);
	set_nextcmd(eap, p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಯದಿ (name_arg != ರಿಕ್ತ)
    {
	// nested function, argument is (args).
	paren = TRUE;
	CLEAR_FIELD(fudi);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (vim9script)
	{
	    ಯದಿ (p[0] == 's' && p[1] == ':')
	    {
		semsg(_(e_cannot_use_s_colon_in_vim9_script_str), p);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    p = to_name_end(p, TRUE);
	    ಯದಿ (*skipwhite(p) == '.' && vim_strchr(p, '(') != ರಿಕ್ತ)
	    {
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->arg);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }

	    p = eap->arg;
	}

	ಪೂರ್ಣಾಙ್ಕ tfn_flags = TFN_NO_AUTOLOAD | TFN_NEW_FUNC
				       | (class_flags != 0 ? TFN_IN_CLASS : 0);
	name = save_function_name(&p, &is_global, eap->skip, tfn_flags, &fudi);
	paren = (vim_strchr(p, '(') != ರಿಕ್ತ);
	ಯದಿ (name == ರಿಕ್ತ && (fudi.fd_dict == ರಿಕ್ತ || !paren) && !eap->skip)
	{
	    /*
	     * Return on an invalid expression in braces, unless the expression
	     * evaluation has been cancelled due to an aborting error, an
	     * interrupt, or an exception.
	     */
	    ಯದಿ (!aborting())
	    {
		ಯದಿ (!eap->skip && fudi.fd_newkey != ರಿಕ್ತ)
		    semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str),
							       fudi.fd_newkey);
		vim_free(fudi.fd_newkey);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		eap->skip = TRUE;
	}

	// For "export def FuncName()" in an autoload script the function name
	// is stored with the legacy autoload name "dir#script#FuncName" so
	// that it can also be found in legacy script.
	ಯದಿ (is_export && name != ರಿಕ್ತ)
	{
	    char_u *prefixed = may_prefix_autoload(name);

	    ಯದಿ (prefixed != ರಿಕ್ತ && prefixed != name)
	    {
		vim_free(name);
		name = prefixed;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (paren && vim9script && name != ರಿಕ್ತ
				    && vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	{
	    emsg(_(e_cannot_use_name_with_hash_in_vim9_script_use_export_instead));
	    ಗಚ್ಛ ret_free;
	}
    }

    // An error in a function call during evaluation of an expression in magic
    // braces should not cause the function not to be defined.
    saved_did_emsg = did_emsg;
    did_emsg = FALSE;

    /*
     * ":function func" with only function name: list function.
     */
    ಯದಿ (!paren)
    {
	fp = list_one_function(eap, name, p, is_global);
	ಗಚ್ಛ ret_free;
    }

    /*
     * :function func<type, type..>() is a generic function
     */
    p = skipwhite(p);
    ಯದಿ (vim9script && eap->cmdidx == CMD_def && *p == '<')
    {
	// generic function
	p = parse_generic_func_type_params(name, p, &gfatab, cctx);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ ret_free;
    }

    /*
     * ":function name(arg1, arg2)" Define function.
     */
    p = skipwhite(p);
    ಯದಿ (*p != '(')
    {
	ಯದಿ (!eap->skip)
	{
	    semsg(_(e_missing_paren_str), eap->arg);
	    ಗಚ್ಛ ret_free;
	}
	// attempt to continue by skipping some text
	ಯದಿ (vim_strchr(p, '(') != ರಿಕ್ತ)
	    p = vim_strchr(p, '(');
    }

    ಯದಿ ((vim9script || eap->cmdidx == CMD_def) && VIM_ISWHITE(p[-1]))
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", p - 1);
	ಗಚ್ಛ ret_free;
    }

    // In Vim9 script only global functions can be redefined.
    ಯದಿ (vim9script && eap->forceit && !is_global)
    {
	emsg(_(e_no_bang_allowed));
	ಗಚ್ಛ ret_free;
    }

    ga_init2(&newlines, ಪರಿಮಾಣ(char_u *), 10);

    ಯದಿ (!eap->skip && name_arg == ರಿಕ್ತ)
    {
	// Check the name of the function.  Unless it's a dictionary function
	// (that we are overwriting).
	ಯದಿ (name != ರಿಕ್ತ)
	    arg = name;
	ಅನ್ಯಥಾ
	    arg = fudi.fd_newkey;
	ಯದಿ (arg != ರಿಕ್ತ && (fudi.fd_di == ರಿಕ್ತ
				     || (fudi.fd_di->di_tv.v_type != VAR_FUNC
				 && fudi.fd_di->di_tv.v_type != VAR_PARTIAL)))
	{
	    char_u  *name_base = arg;
	    ಪೂರ್ಣಾಙ್ಕ	    i;

	    // When defining a dictionary function with bracket notation
	    // (e.g. obj['foo-bar']()), the key is a dictionary key and is not
	    // required to follow function naming rules.  Skip the identifier
	    // check in that case.
	    ಯದಿ (arg != fudi.fd_newkey)
	    {
		ಯದಿ (*arg == K_SPECIAL)
		{
		    name_base = vim_strchr(arg, '_');
		    ಯದಿ (name_base == ರಿಕ್ತ)
			name_base = arg + 3;
		    ಅನ್ಯಥಾ
			++name_base;
		}
		ಪ್ರತಿ (i = 0; name_base[i] != NUL && (i == 0
					    ? eval_isnamec1(name_base[i])
					    : eval_isnamec(name_base[i])); ++i)
		    ;
		ಯದಿ (name_base[i] != NUL)
		{
		    emsg_funcname(e_invalid_argument_str, arg);
		    ಗಚ್ಛ ret_free;
		}

		// In Vim9 script a function cannot have the same name as a
		// variable.
		ಯದಿ (vim9script && *arg == K_SPECIAL
		    && eval_variable(name_base, i, 0, ರಿಕ್ತ,
			ರಿಕ್ತ, EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT
						     + EVAL_VAR_NO_FUNC) == OK)
		{
		    semsg(_(e_redefining_script_item_str), name_base);
		    ಗಚ್ಛ ret_free;
		}
	    }
	}
	// Disallow using the g: dict.
	ಯದಿ (fudi.fd_dict != ರಿಕ್ತ && fudi.fd_dict->dv_scope == VAR_DEF_SCOPE)
	{
	    emsg(_(e_cannot_use_g_here));
	    ಗಚ್ಛ ret_free;
	}
    }

    // This may get more lines and make the pointers into the first line
    // invalid.
    ++p;
    ಯದಿ (get_function_args(&p, ')', &newargs,
			 eap->cmdidx == CMD_def ? &argtypes : ರಿಕ್ತ, FALSE,
			 eap->cmdidx == CMD_def ? &arg_objm : ರಿಕ್ತ,
			 ರಿಕ್ತ, &varargs, &default_args, eap->skip,
			 eap, class_flags, &newlines, ಸಾಲುಗಳು_to_free) == FAIL)
	ಗಚ್ಛ errret_2;
    whitep = p;

    ಯದಿ (eap->cmdidx == CMD_def)
    {
	// find the return type: :def Func(): type
	ಯದಿ (*skipwhite(p) == ':')
	{
	    ಯದಿ (*p != ':')
	    {
		semsg(_(e_no_white_space_allowed_before_colon_str), p);
		p = skipwhite(p);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!IS_WHITE_OR_NUL(p[1]))
		semsg(_(e_white_space_required_after_str_str), ":", p);
	    ret_type = skipwhite(p + 1);
	    p = skip_type(ret_type, FALSE);
	    ಯದಿ (p > ret_type)
	    {
		ret_type = vim_strnsave(ret_type, p - ret_type);
		whitep = p;
		p = skipwhite(p);
	    }
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_expected_type_str), ret_type);
		ret_type = ರಿಕ್ತ;
	    }
	}
	p = skipwhite(p);
    }
    ಅನ್ಯಥಾ
	// find extra arguments "range", "dict", "abort" and "closure"
	ಪ್ರತಿ (;;)
	{
	    whitep = p;
	    p = skipwhite(p);
	    ಯದಿ (STRNCMP(p, "range", 5) == 0)
	    {
		flags |= FC_RANGE;
		p += 5;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "dict", 4) == 0)
	    {
		flags |= FC_DICT;
		p += 4;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "abort", 5) == 0)
	    {
		flags |= FC_ABORT;
		p += 5;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "closure", 7) == 0)
	    {
		flags |= FC_CLOSURE;
		p += 7;
		ಯದಿ (current_funccal == ರಿಕ್ತ)
		{
		    emsg_funcname(e_closure_function_should_not_be_at_ತುದಿ_level_str,
			    name == ರಿಕ್ತ ? (char_u *)"" : name);
		    ಗಚ್ಛ erret;
		}
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}

    // When there is a line break use what follows for the function body.
    // Makes 'exe "func Test()\n...\nendfunc"' work.
    ಯದಿ (*p == '\n')
	ಸಾಲು_arg = p + 1;
    ಅನ್ಯಥಾ ಯದಿ (*p != NUL
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
    ಯದಿ (ಕೀಲಿ_Typed)
    {
	// Check if the function already exists, don't let the user type the
	// whole function before telling him it doesn't work!  For a script we
	// need to skip the body to be able to find what follows.
	ಯದಿ (!eap->skip && !eap->forceit)
	{
	    ಯದಿ (fudi.fd_dict != ರಿಕ್ತ && fudi.fd_newkey == ರಿಕ್ತ)
		emsg(_(e_dictionary_entry_already_exists));
	    ಅನ್ಯಥಾ ಯದಿ (name != ರಿಕ್ತ && find_func(name, is_global) != ರಿಕ್ತ)
		emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
	}

	ಯದಿ (!eap->skip && did_emsg)
	    ಗಚ್ಛ erret;

	ಸಂದೇಶ_putchar('\n');	    // don't overwrite the function name
	cmdline_row = ಸಂದೇಶ_row;
    }

    // Save the starting line number.
    sourcing_lnum_ತುದಿ = SOURCING_LNUM;

    // Do not define the function when getting the body fails, when the header
    // had an error (the body is still read to find the end) and when skipping.
    ಯದಿ (((class_flags & CF_INTERFACE) == 0
		&& (class_flags & CF_ABSTRACT_METHOD) == 0
		&& (get_function_body(eap, &newlines, ಸಾಲು_arg, ಸಾಲುಗಳು_to_free)
							== FAIL || did_emsg))
	    || eap->skip)
	ಗಚ್ಛ erret;

    /*
     * If there are no errors, add the function
     */
    ಯದಿ (fudi.fd_dict != ರಿಕ್ತ)
    {
	ವರ್ಣ	numbuf[NUMBUFLEN];

	fp = ರಿಕ್ತ;
	ಯದಿ (fudi.fd_newkey == ರಿಕ್ತ && !eap->forceit)
	{
	    emsg(_(e_dictionary_entry_already_exists));
	    ಗಚ್ಛ erret;
	}
	ಯದಿ (fudi.fd_di == ರಿಕ್ತ)
	{
	    // Can't add a function to a locked dictionary
	    ಯದಿ (ಮೌಲ್ಯ_check_lock(fudi.fd_dict->dv_lock, eap->arg, FALSE))
		ಗಚ್ಛ erret;
	}
	    // Can't change an existing function if it is locked
	ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ_check_lock(fudi.fd_di->di_tv.v_lock, eap->arg, FALSE))
	    ಗಚ್ಛ erret;

	// Give the function a sequential number.  Can only be used with a
	// Funcref!
	vim_free(name);
	namelen = vim_snprintf(numbuf, ಪರಿಮಾಣ(numbuf), "%d", ++func_nr);
	name = vim_strnsave((char_u *)numbuf, namelen);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಗಚ್ಛ erret;
    }
    ಅನ್ಯಥಾ ಯದಿ (class_flags == 0)
    {
	hashtab_T	*ht;
	char_u		*find_name = name;
	ಪೂರ್ಣಾಙ್ಕ		var_conflict = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		ffed_flags = is_global ? FFED_IS_GLOBAL : 0;

	v = find_var(name, &ht, TRUE);
	ಯದಿ (v != ರಿಕ್ತ && (vim9script || v->di_tv.v_type == VAR_FUNC))
	    var_conflict = TRUE;

	ಯದಿ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ)
	    {
		ಯದಿ (is_export)
		{
		    find_name = name + STRLEN(si->sn_autoload_prefix);
		    v = find_var(find_name, &ht, TRUE);
		    ಯದಿ (v != ರಿಕ್ತ)
			var_conflict = TRUE;
		    // Only check if the function already exists in the script,
		    // global functions can be shadowed.
		    ffed_flags |= FFED_NO_GLOBAL;
		}
		ಅನ್ಯಥಾ
		{
		    char_u *prefixed = may_prefix_autoload(name);

		    ಯದಿ (prefixed != ರಿಕ್ತ && prefixed != name)
		    {
			v = find_var(prefixed, &ht, TRUE);
			ಯದಿ (v != ರಿಕ್ತ)
			    var_conflict = TRUE;
			vim_free(prefixed);
		    }
		}
	    }
	}
	ಯದಿ (var_conflict)
	{
	    emsg_funcname(e_function_name_conflicts_with_variable_str, name);
	    ಗಚ್ಛ erret;
	}

	fp = find_func_even_dead(find_name, ffed_flags);
	ಯದಿ (vim9script)
	{
	    char_u *uname = untrans_function_name(name);

	    import = find_imported(uname == ರಿಕ್ತ ? name : uname, 0, FALSE);
	}

	ಯದಿ (fp != ರಿಕ್ತ || import != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ dead = fp != ರಿಕ್ತ && (fp->uf_flags & FC_DEAD);

	    // Function can be replaced with "function!" and when sourcing the
	    // same script again, but only once.
	    // A name that is used by an import can not be overruled.
	    ಯದಿ (import != ರಿಕ್ತ
		    || (!dead && !eap->forceit
			&& (fp->uf_script_ctx.sc_sid != current_sctx.sc_sid
			  || fp->uf_script_ctx.sc_seq == current_sctx.sc_seq)))
	    {
		SOURCING_LNUM = sourcing_lnum_ತುದಿ;
		ಯದಿ (vim9script)
		    emsg_funcname(e_name_already_defined_str, name);
		ಅನ್ಯಥಾ
		    emsg_funcname(e_function_str_already_exists_add_bang_to_replace, name);
		ಗಚ್ಛ errret_keep;
	    }
	    ಯದಿ (fp->uf_calls > 0)
	    {
		emsg_funcname(
			    e_cannot_redefine_function_str_it_is_in_use, name);
		ಗಚ್ಛ errret_keep;
	    }
	    ಯದಿ (fp->uf_refcount > 1)
	    {
		// This function is referenced somewhere, don't redefine it but
		// create a new one.
		--fp->uf_refcount;
		fp->uf_flags |= FC_REMOVED;
		fp = ರಿಕ್ತ;
		overwrite = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		char_u *exp_name = fp->uf_name_exp;

		// redefine existing function, keep the expanded name
		VIM_CLEAR(name);
		namelen = 0;
		fp->uf_name_exp = ರಿಕ್ತ;
		func_clear_ವಸ್ತುಗಳು(fp);
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

    ಯದಿ (fp == ರಿಕ್ತ)
    {
	ಯದಿ (fudi.fd_dict == ರಿಕ್ತ && vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    slen, plen;
	    char_u  *scriptname;

	    // Check that the autoload name matches the script name.
	    j = FAIL;
	    ಯದಿ (SOURCING_NAME != ರಿಕ್ತ)
	    {
		scriptname = autoload_name(name);
		ಯದಿ (scriptname != ರಿಕ್ತ)
		{
		    p = vim_strchr(scriptname, '/');
		    plen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
		    slen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(SOURCING_NAME);
		    ಯದಿ (slen > plen && fnamecmp(p,
					    SOURCING_NAME + slen - plen) == 0)
			j = OK;
		    vim_free(scriptname);
		}
	    }
	    ಯದಿ (j == FAIL)
	    {
		linenr_T save_lnum = SOURCING_LNUM;

		SOURCING_LNUM = sourcing_lnum_ತುದಿ;
		semsg(_(e_function_name_does_not_match_script_file_name_str),
									 name);
		SOURCING_LNUM = save_lnum;
		ಗಚ್ಛ erret;
	    }
	}

	ಯದಿ (namelen == 0)
	    namelen = STRLEN(name);
	fp = alloc_ufunc(name, namelen);
	ಯದಿ (fp == ರಿಕ್ತ)
	    ಗಚ್ಛ erret;
	fp_allocated = TRUE;

	ಯದಿ (fudi.fd_dict != ರಿಕ್ತ)
	{
	    char_u *func_name = vim_strnsave(name, namelen);

	    ಯದಿ (func_name == ರಿಕ್ತ)
	    {
		VIM_CLEAR(fp);
		ಗಚ್ಛ erret;
	    }
	    ಯದಿ (fudi.fd_di == ರಿಕ್ತ)
	    {
		// add new dict entry
		fudi.fd_di = dictitem_alloc(fudi.fd_newkey);
		ಯದಿ (fudi.fd_di == ರಿಕ್ತ)
		{
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ಗಚ್ಛ erret;
		}
		ಯದಿ (dict_add(fudi.fd_dict, fudi.fd_di) == FAIL)
		{
		    vim_free(fudi.fd_di);
		    vim_free(func_name);
		    VIM_CLEAR(fp);
		    ಗಚ್ಛ erret;
		}
	    }
	    ಅನ್ಯಥಾ
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

    ಯದಿ (eap->cmdidx == CMD_def)
    {
	ಪೂರ್ಣಾಙ್ಕ	    lnum_save = SOURCING_LNUM;
	cstack_T    *cstack = eap->cstack;

	fp->uf_def_status = UF_TO_BE_COMPILED;

	ಯದಿ (generic_func_args_table_size(&gfatab) > 0)
	{
	    // initialize generic function state
	    flags |= FC_GENERIC;
	    generic_func_init(fp, &gfatab);
	}

	// error messages are for the first function line
	SOURCING_LNUM = sourcing_lnum_ತುದಿ;

	// The function may use script variables from the context.
	function_using_block_scopes(fp, cstack);

	// The argument types and the return type may use an imported type.
	// In that case, the imported file will be sourced.  To avoid treating
	// everything in the imported file as exported, temporarily reset
	// is_export.
	ಪೂರ್ಣಾಙ್ಕ save_is_export = is_export;
	is_export = FALSE;

	ಯದಿ (parse_argument_types(fp, &argtypes, varargs, &arg_objm,
				obj_members, obj_member_ಎಣಿಕೆ, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ಗಚ್ಛ erret;
	}
	varargs = FALSE;

	// parse the return type, if any
	ಯದಿ (parse_return_type(fp, ret_type, cctx) == FAIL)
	{
	    SOURCING_LNUM = lnum_save;
	    free_fp = fp_allocated;
	    is_export = save_is_export;
	    ಗಚ್ಛ erret;
	}
	is_export = save_is_export;
	SOURCING_LNUM = lnum_save;
    }
    ಅನ್ಯಥಾ
	fp->uf_def_status = UF_NOT_COMPILED;

    ಯದಿ (fp_allocated)
    {
	// insert the new function in the function list
	ಯದಿ (overwrite)
	{
	    hi = hash_find(&func_hashtab, name);
	    hi->hi_ಕೀಲಿ = UF2HIKEY(fp);
	}
	ಅನ್ಯಥಾ ಯದಿ (class_flags == 0 && hash_add(&func_hashtab,
					 UF2HIKEY(fp), "add function") == FAIL)
	{
	    free_fp = TRUE;
	    ಗಚ್ಛ erret;
	}
	fp->uf_refcount = 1;
    }

    fp->uf_ಸಾಲುಗಳು = newlines;
    newlines.ga_data = ರಿಕ್ತ;
    ಯದಿ ((flags & FC_CLOSURE) != 0)
    {
	ಯದಿ (register_closure(fp) == FAIL)
	    ಗಚ್ಛ erret;
    }
    ಅನ್ಯಥಾ
	fp->uf_scoped = ರಿಕ್ತ;

#ifdef FEAT_PROFILE
    ಯದಿ (prof_def_func())
	func_do_profile(fp);
#endif
    fp->uf_varargs = varargs;
    ಯದಿ (sandbox)
	flags |= FC_SANDBOX;
    ಯದಿ (vim9script && !ASCII_ISUPPER(*fp->uf_name))
	flags |= FC_VIM9;
    fp->uf_flags = flags;
    fp->uf_calls = 0;
    fp->uf_cleared = FALSE;
    fp->uf_script_ctx = current_sctx;
    fp->uf_script_ctx_version = current_sctx.sc_version;
    fp->uf_script_ctx.sc_lnum += sourcing_lnum_ತುದಿ;
    ಯದಿ (is_export)
    {
	fp->uf_flags |= FC_EXPORT;
	// let do_one_cmd() know the export worked.
	is_export = FALSE;
    }

    ಯದಿ (eap->cmdidx == CMD_def)
	set_function_type(fp);
    ಅನ್ಯಥಾ ಯದಿ (fp->uf_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	// :func does not use Vim9 script syntax, even in a Vim9 script file
	fp->uf_script_ctx.sc_version = SCRIPT_VERSION_MAX;

    // If test_override('defcompile' 1) is used, then compile the function now
    ಯದಿ (eap->cmdidx == CMD_def && override_defcompile)
	defcompile_function(fp, ರಿಕ್ತ);

    ಗಚ್ಛ ret_free;

erret:
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	// these were set to "newargs" and "default_args", which are cleared
	// below
	ga_init(&fp->uf_args);
	ga_init(&fp->uf_def_args);
    }
errret_2:
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	VIM_CLEAR(fp->uf_arg_types);
	VIM_CLEAR(fp->uf_va_name);
	VIM_CLEAR(fp->uf_name_exp);
	clear_func_type_list(&fp->uf_type_list, &fp->uf_func_type);
	// the generic types were moved from "gfatab" to "fp"
	ಯದಿ (IS_GENERIC_FUNC(fp))
	    generic_func_clear_ವಸ್ತುಗಳು(fp);
    }
    ಯದಿ (free_fp)
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
    ಯದಿ (name != name_arg)
	vim_free(name);
    vim_free(ret_type);
    did_emsg |= saved_did_emsg;

    ಪ್ರತ್ಯಾಗಮ fp;
}

/*
 * ":function"
 */
    ಶೂನ್ಯ
ex_function(exarg_T *eap)
{
    garray_T ಸಾಲುಗಳು_to_free;

    ga_init2(&ಸಾಲುಗಳು_to_free, ಪರಿಮಾಣ(char_u *), 50);
    (ಶೂನ್ಯ)define_function(eap, ರಿಕ್ತ, &ಸಾಲುಗಳು_to_free, 0, ರಿಕ್ತ, 0, ರಿಕ್ತ);
    ga_clear_strings(&ಸಾಲುಗಳು_to_free);
}

    ಪೂರ್ಣಾಙ್ಕ
get_func_arity(char_u *name, ಪೂರ್ಣಾಙ್ಕ *required, ಪೂರ್ಣಾಙ್ಕ *optional, ಪೂರ್ಣಾಙ್ಕ *varargs)
{
    ufunc_T	*ufunc = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;
    ಪೂರ್ಣಾಙ್ಕ		min_argcount = 0;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    idx = find_internal_func(name);
    ಯದಿ (idx >= 0)
    {
	internal_func_get_argcount(idx, &argcount, &min_argcount);
	*varargs = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	char_u	fname_buf[FLEN_FIXED + 1];
	char_u	*tofree = ರಿಕ್ತ;
	funcerror_T error = FCERR_NONE;
	char_u	*fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ಯದಿ (error == FCERR_NONE)
	    ufunc = find_func(fname, FALSE);
	vim_free(tofree);

	ಯದಿ (ufunc == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	argcount = ufunc->uf_args.ga_len;
	min_argcount = ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len;
	*varargs = has_varargs(ufunc);
    }

    *required = min_argcount;
    *optional = argcount - min_argcount;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    char_u	*bracket_start = ರಿಕ್ತ;
    char_u	*generic_func_name = name;

    ಯದಿ (STRNCMP(arg, "profile", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	*compile_type = CT_PROFILE;
	arg = skipwhite(arg + 7);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "debug", 5) == 0 && VIM_ISWHITE(arg[5]))
    {
	*compile_type = CT_DEBUG;
	arg = skipwhite(arg + 5);
    }

    ಯದಿ (STRNCMP(arg, "<lambda>", 8) == 0)
    {
	arg += 8;
	(ಶೂನ್ಯ)getdigits(&arg);
	fname = vim_strnsave(name, arg - name);
    }
    ಅನ್ಯಥಾ
    {
	// First try finding a method in a class, trans_function_name() will
	// give an error if the function is not found.
	ufunc = find_class_func(&arg);
	ಯದಿ (ufunc != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ufunc;

	fname = trans_function_name_ext(&arg, &is_global, FALSE,
		      TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DECL,
		      ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, &ufunc);
	ಯದಿ (ufunc != ರಿಕ್ತ)
	{
	    vim_free(fname);
	    ಪ್ರತ್ಯಾಗಮ ufunc;
	}
	ಯದಿ (fname != ರಿಕ್ತ && *arg == '<')
	{
	    generic_func_name = name;
	    bracket_start = arg;
	    ಯದಿ (skip_generic_func_type_args(&arg) == FAIL)
	    {
		vim_free(fname);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }
    ಯದಿ (fname == ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str), name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (!ends_excmd2(name, arg))
    {
	vim_free(fname);
	emsg(ex_errmsg(e_trailing_characters_str, arg));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ufunc = find_func(fname, is_global);
    ಯದಿ (ufunc == ರಿಕ್ತ)
    {
	char_u *p = untrans_function_name(fname);

	ಯದಿ (p != ರಿಕ್ತ)
	    // Try again without making it script-local.
	    ufunc = find_func(p, FALSE);

	ಯದಿ (ufunc == ರಿಕ್ತ)
	{
	    bracket_start = generic_func_find_open_bracket(fname);
	    ಯದಿ (bracket_start != ರಿಕ್ತ)
	    {
		generic_func_name = fname;
		*bracket_start = NUL;
		ufunc = find_func(fname, FALSE);
		*bracket_start = '<';
	    }
	}
    }

    ಯದಿ (ufunc != ರಿಕ್ತ)
    {
	// handle generic functions
	ಯದಿ (bracket_start == ರಿಕ್ತ)
	    bracket_start = generic_func_name + STRLEN(generic_func_name);

	ufunc = eval_generic_func(ufunc, generic_func_name, &bracket_start);
    }

    vim_free(fname);

    ಯದಿ (ufunc == ರಿಕ್ತ)
	semsg(_(e_cannot_find_function_str), name);
    ಪ್ರತ್ಯಾಗಮ ufunc;
}

/*
 * Compile the :def function "ufunc".  If "cl" is not NULL, then compile the
 * class or object method "ufunc" in "cl".
 */
    ಶೂನ್ಯ
defcompile_function(ufunc_T *ufunc, class_T *cl)
{
    compiletype_T compile_type = CT_NONE;

    ಯದಿ (func_needs_compiling(ufunc, compile_type))
	(ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, compile_type, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	smsg(_("Function %s%s%s does not need compiling"),
				cl != ರಿಕ್ತ ? cl->class_name.string : (char_u *)"",
				cl != ರಿಕ್ತ ? (char_u *)"." : (char_u *)"",
				ufunc->uf_name);
}

/*
 * Compile all the :def functions defined in the current script
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
defcompile_funcs_in_script(ಶೂನ್ಯ)
{
    long	todo = (long)func_hashtab.ht_used;
    ಪೂರ್ಣಾಙ್ಕ		changed = func_hashtab.ht_changed;
    hashitem_T	*hi;

    ಪ್ರತಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ufunc_T *ufunc = HI2UF(hi);
	    ಯದಿ (ufunc->uf_script_ctx.sc_sid == current_sctx.sc_sid
		    && ufunc->uf_def_status == UF_TO_BE_COMPILED
		    && (ufunc->uf_flags & FC_DEAD) == 0)
	    {
		(ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, CT_NONE, ರಿಕ್ತ);

		ಯದಿ (func_hashtab.ht_changed != changed)
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
    ಶೂನ್ಯ
ex_defcompile(exarg_T *eap)
{
    ಯದಿ (*eap->arg != NUL)
    {
	typval_T tv;

	ಯದಿ (is_class_name(eap->arg, &tv))
	{
	    class_T *cl = tv.vval.v_class;

	    ಯದಿ (cl != ರಿಕ್ತ)
		defcompile_class(cl);
	}
	ಅನ್ಯಥಾ
	{
	    compiletype_T compile_type = CT_NONE;
	    ufunc_T *ufunc = find_func_by_name(eap->arg, &compile_type);
	    ಯದಿ (ufunc != ರಿಕ್ತ)
		defcompile_function(ufunc, ರಿಕ್ತ);
	}
    }
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
eval_fname_script(char_u *p)
{
    // Use MB_STRICMP() because in Turkish comparing the "I" may not work with
    // the standard library function.
    ಯದಿ (p[0] == '<' && (MB_STRNICMP(p + 1, "SID>", 4) == 0
				       || MB_STRNICMP(p + 1, "SNR>", 4) == 0))
	ಪ್ರತ್ಯಾಗಮ 5;
    ಯದಿ (p[0] == 's' && p[1] == ':')
	ಪ್ರತ್ಯಾಗಮ 2;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಪೂರ್ಣಾಙ್ಕ
translated_function_exists(char_u *name, ಪೂರ್ಣಾಙ್ಕ is_global)
{
    ಯದಿ (builtin_function(name, -1))
	ಪ್ರತ್ಯಾಗಮ has_internal_func(name);
    ಪ್ರತ್ಯಾಗಮ find_func(name, is_global) != ರಿಕ್ತ;
}

/*
 * Return TRUE when "ufunc" has old-style "..." varargs
 * or named varargs "...name: type".
 */
    ಪೂರ್ಣಾಙ್ಕ
has_varargs(ufunc_T *ufunc)
{
    ಪ್ರತ್ಯಾಗಮ ufunc->uf_varargs || ufunc->uf_va_name != ರಿಕ್ತ;
}

/*
 * Return TRUE if a function "name" exists.
 * If "no_defef" is TRUE, do not dereference a Funcref.
 */
    ಪೂರ್ಣಾಙ್ಕ
function_exists(char_u *name, ಪೂರ್ಣಾಙ್ಕ no_deref)
{
    char_u  *nm = name;
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    n = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    flag;
    ಪೂರ್ಣಾಙ್ಕ	    is_global = FALSE;

    flag = TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD;
    ಯದಿ (no_deref)
	flag |= TFN_NO_DEREF;
    p = trans_function_name(&nm, &is_global, FALSE, flag);
    nm = skipwhite(nm);

    // Only accept "funcname", "funcname ", "funcname (..." and
    // "funcname(...", not "funcname!...".
    ಯದಿ (p != ರಿಕ್ತ && (*nm == NUL || *nm == '('))
	n = translated_function_exists(p, is_global);
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Add what getinfo() reports for the user defined function "name" to "d".
 * Returns FAIL when there is no such function.
 */
    ಪೂರ್ಣಾಙ್ಕ
user_func_info(char_u *name, dict_T *d)
{
    char_u	*nm = name;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    ufunc_T	*fp = ರಿಕ್ತ;
    list_T	*args;
    ಪೂರ್ಣಾಙ್ಕ		is_def;
    ವರ್ಣ	*tofree;

    ಯದಿ (*name == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    p = trans_function_name(&nm, &is_global, FALSE,
			 TFN_INT | TFN_QUIET | TFN_NO_AUTOLOAD | TFN_NO_DEREF);
    ಯದಿ (p != ರಿಕ್ತ && *skipwhite(nm) == NUL)
	fp = find_func(p, is_global);
    vim_free(p);
    ಯದಿ (fp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    is_def = fp->uf_def_status != UF_NOT_COMPILED;
    dict_add_string(d, "name", printable_func_name(fp));
    dict_add_string(d, "kind", (char_u *)(is_def ? "def" : "function"));
    dict_add_bool(d, "available", TRUE);
    args = list_alloc();
    ಯದಿ (args == ರಿಕ್ತ || dict_add_list(d, "args", args) == FAIL)
    {
	list_unref(args);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < fp->uf_args.ga_len; ++j)
    {
	dict_T	*arg = dict_alloc();

	ಯದಿ (arg == ರಿಕ್ತ || list_append_dict(args, arg) == FAIL)
	{
	    dict_unref(arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	dict_add_string(arg, "name", FUNCARG(fp, j));
	ಯದಿ (fp->uf_arg_types == ರಿಕ್ತ)
	    dict_add_string(arg, "type", (char_u *)"any");
	ಅನ್ಯಥಾ
	{
	    dict_add_string(arg, "type",
			     (char_u *)type_name(fp->uf_arg_types[j], &tofree));
	    vim_free(tofree);
	}
	ಯದಿ (j >= fp->uf_args.ga_len - fp->uf_def_args.ga_len)
	    dict_add_string(arg, "default", ((char_u **)fp->uf_def_args.ga_data)
			     [j - fp->uf_args.ga_len + fp->uf_def_args.ga_len]);
    }
    ಯದಿ (has_varargs(fp))
    {
	dict_T	*va = dict_alloc();

	ಯದಿ (va == ರಿಕ್ತ || dict_add_dict(d, "varargs", va) == FAIL)
	{
	    dict_unref(va);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	dict_add_string(va, "name",
			 fp->uf_va_name == ರಿಕ್ತ ? (char_u *)"" : fp->uf_va_name);
	ಯದಿ (fp->uf_va_type == ರಿಕ್ತ)
	    dict_add_string(va, "type", (char_u *)"list<any>");
	ಅನ್ಯಥಾ
	{
	    dict_add_string(va, "type",
				 (char_u *)type_name(fp->uf_va_type, &tofree));
	    vim_free(tofree);
	}
    }
    ಯದಿ (is_def)
    {
	dict_add_string(d, "returns",
				 (char_u *)type_name(fp->uf_ret_type, &tofree));
	vim_free(tofree);
    }
    ಅನ್ಯಥಾ
	dict_add_string(d, "returns", (char_u *)"any");
    dict_add_bool(d, "abort", (fp->uf_flags & FC_ABORT) != 0);
    dict_add_bool(d, "range", (fp->uf_flags & FC_RANGE) != 0);
    dict_add_bool(d, "dict", (fp->uf_flags & FC_DICT) != 0);
    dict_add_bool(d, "closure", (fp->uf_flags & FC_CLOSURE) != 0);
    dict_add_ಸಂಖ್ಯೆ(d, "sid", fp->uf_script_ctx.sc_sid);
    dict_add_ಸಂಖ್ಯೆ(d, "lnum", fp->uf_script_ctx.sc_lnum);
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    char_u *
get_expanded_name(char_u *name, ಪೂರ್ಣಾಙ್ಕ check)
{
    char_u	*nm = name;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;

    p = trans_function_name(&nm, &is_global, FALSE, TFN_INT|TFN_QUIET);

    ಯದಿ (p != ರಿಕ್ತ && *nm == NUL
		       && (!check || translated_function_exists(p, is_global)))
	ಪ್ರತ್ಯಾಗಮ p;

    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * function names.
 */
    char_u *
get_user_func_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ long_u	done;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		changed;
    ಸ್ಥಿರ hashitem_T	*hi;
    ufunc_T		*fp;

    ಯದಿ (idx == 0)
    {
	done = 0;
	hi = func_hashtab.ht_array;
	changed = func_hashtab.ht_changed;
    }
    ಯದಿ (changed == func_hashtab.ht_changed && done < func_hashtab.ht_used)
    {
	ಪೂರ್ಣಾಙ್ಕ len;

	ಯದಿ (done++ > 0)
	    ++hi;
	ಯಾವತ್ (HASHITEM_EMPTY(hi))
	    ++hi;
	fp = HI2UF(hi);

	// don't show dead, dict and lambda functions
	ಯದಿ ((fp->uf_flags & FC_DEAD) || (fp->uf_flags & FC_DICT)
				|| STRNCMP(fp->uf_name, "<lambda>", 8) == 0)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";

	ಯದಿ (fp->uf_namelen + 4 >= IOSIZE)
	    ಪ್ರತ್ಯಾಗಮ fp->uf_name;	// prevents overflow

	len = cat_func_name(IObuff, IOSIZE, fp);
	ಯದಿ (xp->xp_context != EXPAND_USER_FUNC
				       && xp->xp_context != EXPAND_DISASSEMBLE)
	{
	    STRCPY(IObuff + len, "(");
	    ಯದಿ (!has_varargs(fp) && fp->uf_args.ga_len == 0)
	    {
		++len;
		STRCPY(IObuff + len, ")");
	    }
	}
	ಪ್ರತ್ಯಾಗಮ IObuff;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Make a copy of a function.
 * Intended to be used for a function defined on a base class that has a copy
 * on the child class.
 * The copy has uf_refcount set to one.
 * Returns NULL when out of memory.
 */
    ufunc_T *
copy_function(ufunc_T *fp, ಪೂರ್ಣಾಙ್ಕ extra_namelen)
{
    ufunc_T *ufunc = alloc_ufunc(fp->uf_name, fp->uf_namelen + extra_namelen);
    ಯದಿ (ufunc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Most things can just be copied.
    // The call to alloc_ufunc() above allocates a new uf_name_exp.  So save
    // and restore it.
    char_u *save_uf_name_exp = ufunc->uf_name_exp;
    *ufunc = *fp;
    ufunc->uf_name_exp = save_uf_name_exp;

    ufunc->uf_def_status = UF_TO_BE_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ufunc->uf_class = ರಿಕ್ತ;
#ifdef FEAT_PROFILE
    // The copy gets its own profiling data, see below.
    ufunc->uf_profiling = FALSE;
    ufunc->uf_prof_initialized = FALSE;
    ufunc->uf_tml_ಎಣಿಕೆ = ರಿಕ್ತ;
    ufunc->uf_tml_total = ರಿಕ್ತ;
    ufunc->uf_tml_self = ರಿಕ್ತ;
#endif

    ga_copy_strings(&fp->uf_args, &ufunc->uf_args);
    ga_copy_strings(&fp->uf_def_args, &ufunc->uf_def_args);

    ಯದಿ (ufunc->uf_arg_types != ರಿಕ್ತ)
    {
	// "uf_arg_types" is an allocated array, make a copy.
	type_T **at = ALLOC_CLEAR_MULT(type_T *, ufunc->uf_args.ga_len);
	ಯದಿ (at != ರಿಕ್ತ)
	{
	    mch_memmove(at, ufunc->uf_arg_types,
				     ಪರಿಮಾಣ(type_T *) * ufunc->uf_args.ga_len);
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

    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ)
	ufunc->uf_va_name = vim_strsave(ufunc->uf_va_name);

    // TODO:
    //    type_T	*uf_va_type;
    //    type_T	*uf_func_type;

    ufunc->uf_block_depth = 0;
    ufunc->uf_block_ids = ರಿಕ್ತ;

    ga_copy_strings(&fp->uf_ಸಾಲುಗಳು, &ufunc->uf_ಸಾಲುಗಳು);

#ifdef FEAT_PROFILE
    // Profile the copy when the function is profiled, e.g. after
    // ":profile! file", which only applies when a function is defined.
    ಯದಿ (fp->uf_profiling)
	func_do_profile(ufunc);
#endif

    ufunc->uf_refcount = 1;

    ಪ್ರತ್ಯಾಗಮ ufunc;
}

/*
 * ":delfunction {name}"
 */
    ಶೂನ್ಯ
ex_delfunction(exarg_T *eap)
{
    ufunc_T	*fp = ರಿಕ್ತ;
    char_u	*p;
    char_u	*name;
    funcdict_T	fudi;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;

    p = eap->arg;
    name = trans_function_name_ext(&p, &is_global, eap->skip, TFN_NO_DECL, &fudi,
							     ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    vim_free(fudi.fd_newkey);
    ಯದಿ (name == ರಿಕ್ತ)
    {
	ಯದಿ (fudi.fd_dict != ರಿಕ್ತ && !eap->skip)
	    emsg(_(e_funcref_required));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ends_excmd(*skipwhite(p)))
    {
	vim_free(name);
	semsg(_(e_trailing_characters_str), p);
	ಪ್ರತ್ಯಾಗಮ;
    }
    set_nextcmd(eap, p);
    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
	*p = NUL;

    ಯದಿ (numbered_function(name) && fudi.fd_dict == ರಿಕ್ತ)
    {
	ಯದಿ (!eap->skip)
	    semsg(_(e_invalid_argument_str), eap->arg);
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!eap->skip)
	fp = find_func(name, is_global);
    vim_free(name);

    ಯದಿ (!eap->skip)
    {
	ಯದಿ (fp == ರಿಕ್ತ)
	{
	    ಯದಿ (!eap->forceit)
		semsg(_(e_unknown_function_str), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (fp->uf_calls > 0)
	{
	    semsg(_(e_cannot_delete_function_str_it_is_in_use), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (fp->uf_flags & FC_VIM9)
	{
	    semsg(_(e_cannot_delete_vim9_script_function_str), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (fudi.fd_dict != ರಿಕ್ತ)
	{
	    // Delete the dict item that refers to the function, it will
	    // invoke func_unref() and possibly delete the function.
	    dictitem_remove(fudi.fd_dict, fudi.fd_di, "delfunction");
	}
	ಅನ್ಯಥಾ
	{
	    // A normal function (not a numbered function or lambda) has a
	    // refcount of 1 for the entry in the hashtable.  When deleting
	    // it and the refcount is more than one, it should be kept.
	    // A numbered function and lambda should be kept if the refcount is
	    // one or more.
	    ಯದಿ (fp->uf_refcount > (func_name_refcount(fp->uf_name) ? 0 : 1))
	    {
		// Function is still referenced somewhere.  Don't free it but
		// do remove it from the hashtable.
		ಯದಿ (func_remove(fp))
		    fp->uf_refcount--;
	    }
	    ಅನ್ಯಥಾ
		func_clear_free(fp, FALSE);
	}
    }
}

/*
 * Unreference a Function: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
func_unref(char_u *name)
{
    ufunc_T *fp = ರಿಕ್ತ;

    ಯದಿ (name == ರಿಕ್ತ || !func_name_refcount(name))
	ಪ್ರತ್ಯಾಗಮ;
    fp = find_func(name, FALSE);
    ಯದಿ (fp == ರಿಕ್ತ && numbered_function(name))
    {
#ifdef EXITFREE
	ಯದಿ (!entered_free_all_mem)
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
    ಶೂನ್ಯ
func_ptr_unref(ufunc_T *fp)
{
    ಯದಿ (fp != ರಿಕ್ತ && (--fp->uf_refcount <= 0
		|| (fp->uf_refcount == 1 && fp->uf_partial != ರಿಕ್ತ
					 && fp->uf_partial->pt_refcount <= 1
					 && fp->uf_partial->pt_func == fp)))
    {
	// Only delete it when it's not being used.  Otherwise it's done
	// when "uf_calls" becomes zero.
	ಯದಿ (fp->uf_calls == 0)
	    func_clear_free(fp, FALSE);
    }
}

/*
 * Count a reference to a Function.
 */
    ಶೂನ್ಯ
func_ref(char_u *name)
{
    ufunc_T *fp;

    ಯದಿ (name == ರಿಕ್ತ || !func_name_refcount(name))
	ಪ್ರತ್ಯಾಗಮ;
    fp = find_func(name, FALSE);
    ಯದಿ (fp != ರಿಕ್ತ)
	++fp->uf_refcount;
    ಅನ್ಯಥಾ ಯದಿ (numbered_function(name))
	// Only give an error for a numbered function.
	// Fail silently, when named or lambda function isn't found.
	internal_error("func_ref()");
}

/*
 * Count a reference to a Function.
 */
    ಶೂನ್ಯ
func_ptr_ref(ufunc_T *fp)
{
    ಯದಿ (fp != ರಿಕ್ತ)
	++fp->uf_refcount;
}

/*
 * Return TRUE if items in "fc" do not have "copyID".  That means they are not
 * referenced from anywhere that is in use.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_free_funccal(funccall_T *fc, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪ್ರತ್ಯಾಗಮ (fc->fc_l_varlist.lv_copyID != copyID
	    && fc->fc_l_vars.dv_copyID != copyID
	    && fc->fc_l_avars.dv_copyID != copyID
	    && fc->fc_copyID != copyID);
}

/*
 * ":return [expr]"
 */
    ಶೂನ್ಯ
ex_return(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		returning = FALSE;
    evalarg_T	evalarg;

    ಯದಿ (current_funccal == ರಿಕ್ತ)
    {
	emsg(_(e_return_not_inside_function));
	ಪ್ರತ್ಯಾಗಮ;
    }

    init_evalarg(&evalarg);
    evalarg.eval_flags = eap->skip ? 0 : EVAL_EVALUATE;

    ಯದಿ (eap->skip)
	++emsg_skip;

    eap->nextcmd = ರಿಕ್ತ;
    ಯದಿ ((*arg != NUL && *arg != '|' && *arg != '\n')
				  && eval0(arg, &rettv, eap, &evalarg) != FAIL)
    {
	ಯದಿ (!eap->skip)
	    returning = do_return(eap, FALSE, TRUE, &rettv);
	ಅನ್ಯಥಾ
	    clear_tv(&rettv);
    }
    // It's safer to return also on error.
    ಅನ್ಯಥಾ ಯದಿ (!eap->skip)
    {
	// In return statement, cause_abort should be force_abort.
	update_force_abort();

	/*
	 * Return unless the expression evaluation has been cancelled due to an
	 * aborting error, an interrupt, or an exception.
	 */
	ಯದಿ (!aborting())
	    returning = do_return(eap, FALSE, TRUE, ರಿಕ್ತ);
    }

    // When skipping or the return gets pending, advance to the next command
    // in this line (!returning).  Otherwise, ignore the rest of the line.
    // Following lines will be ignored by get_func_line().
    ಯದಿ (returning)
	eap->nextcmd = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (eap->nextcmd == ರಿಕ್ತ)	    // no argument
	set_nextcmd(eap, arg);

    ಯದಿ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);
}

/*
 * Lower level implementation of "call".  Only called when not skipping.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ex_call_inner(
	exarg_T	    *eap,
	char_u	    *name,
	char_u	    **arg,
	char_u	    *startarg,
	funcexe_T   *funcexe_init,
	evalarg_T   *evalarg)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		doesrange;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;

    lnum = eap->line1;
    ಪ್ರತಿ ( ; lnum <= eap->line2; ++lnum)
    {
	funcexe_T funcexe;

	ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	{
	    ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// If the function deleted lines or switched to another buffer
		// the line number may become invalid.
		emsg(_(e_invalid_range));
		ಭಙ್ಗ;
	    }
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    curwin->w_cursor.coladd = 0;
	}
	*arg = startarg;

	funcexe = *funcexe_init;
	funcexe.fe_doesrange = &doesrange;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this
	ಯದಿ (get_func_tv(name, -1, &rettv, arg, evalarg, &funcexe) == FAIL)
	{
	    failed = TRUE;
	    ಭಙ್ಗ;
	}
	ಯದಿ (has_watchexpr())
	    dbg_check_breakpoint(eap);

	// Handle a function returning a Funcref, Dictionary or List.
	ಯದಿ (handle_subscript(arg, ರಿಕ್ತ, &rettv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
	{
	    failed = TRUE;
	    ಭಙ್ಗ;
	}

	clear_tv(&rettv);
	ಯದಿ (doesrange)
	    ಭಙ್ಗ;

	// Stop when immediately aborting on error, or when an interrupt
	// occurred or an exception was thrown but not caught.
	// get_func_tv() returned OK, so that the check for trailing
	// characters below is executed.
	ಯದಿ (aborting())
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ failed;
}

/*
 * Core part of ":defer func(arg)".  "arg" points to the "(" and is advanced.
 * Returns FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ex_defer_inner(
	char_u	    *name,
	char_u	    **arg,
	type_T	    *type,
	partial_T   *partial,
	evalarg_T   *evalarg)
{
    typval_T	argvars[MAX_FUNC_ARGS + 1];	// vars for arguments
    ಪೂರ್ಣಾಙ್ಕ		partial_argc = 0;		// number of partial arguments
    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;			// number of arguments found
    ಪೂರ್ಣಾಙ್ಕ		r;

    ಯದಿ (current_funccal == ರಿಕ್ತ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (partial != ರಿಕ್ತ)
    {
	ಯದಿ (partial->pt_dict != ರಿಕ್ತ)
	{
	    emsg(_(e_cannot_use_partial_with_dictionary_for_defer));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (partial->pt_argc > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    partial_argc = partial->pt_argc;
	    ಪ್ರತಿ (i = 0; i < partial_argc; ++i)
		copy_tv(&partial->pt_argv[i], &argvars[i]);
	}
    }
    ಪೂರ್ಣಾಙ್ಕ is_builtin = builtin_function(name, -1);
    r = get_func_arguments(arg, evalarg, FALSE,
				argvars + partial_argc, &argcount, is_builtin);
    argcount += partial_argc;

    ಯದಿ (r == OK)
    {
	ಯದಿ (type != ರಿಕ್ತ)
	{
	    // Check that the arguments are OK for the types of the funcref.
	    r = check_argument_types(type, argvars, argcount, ರಿಕ್ತ, name);
	}
	ಅನ್ಯಥಾ ಯದಿ (is_builtin)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx = find_internal_func(name);

	    ಯದಿ (idx < 0)
	    {
		emsg_funcname(e_unknown_function_str, name);
		r = FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (check_internal_func(idx, argcount) == -1)
		r = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    ufunc_T *ufunc = find_func(name, FALSE);

	    // we tolerate an unknown function here, it might be defined later
	    ಯದಿ (ufunc != ರಿಕ್ತ)
	    {
		funcerror_T error = check_user_func_argcount(ufunc, argcount);
		ಯದಿ (error != FCERR_UNKNOWN)
		{
		    user_func_error(error, name, FALSE);
		    r = FAIL;
		}
	    }
	}
    }

    ಯದಿ (r == FAIL)
    {
	ಯಾವತ್ (--argcount >= 0)
	    clear_tv(&argvars[argcount]);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ add_defer(name, argcount, argvars);
}

/*
 * Return TRUE if currently inside a function call.
 * Give an error message and return FALSE when not.
 */
    ಪೂರ್ಣಾಙ್ಕ
can_add_defer(ಶೂನ್ಯ)
{
    ಯದಿ (!in_def_function() && get_current_funccal() == ರಿಕ್ತ)
    {
	semsg(_(e_str_not_inside_function), "defer");
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "argvars[]".
 * Caller must check that in_def_function() returns TRUE or current_funccal is
 * not NULL.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
add_defer(char_u *name, ಪೂರ್ಣಾಙ್ಕ argcount_arg, typval_T *argvars)
{
    char_u	*saved_name = vim_strsave(name);
    ಪೂರ್ಣಾಙ್ಕ		argcount = argcount_arg;
    defer_T	*dr;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

    ಯದಿ (saved_name == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    ಯದಿ (in_def_function())
    {
	ಯದಿ (add_defer_function(saved_name, argcount, argvars) == OK)
	{
	    argcount = 0;
	    ret = OK;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (current_funccal->fc_defer.ga_itemsize == 0)
	    ga_init2(&current_funccal->fc_defer, ಪರಿಮಾಣ(defer_T), 10);
	ಯದಿ (ga_grow_id(&current_funccal->fc_defer, 1, aid_defer) == FAIL)
	{
	    vim_free(saved_name);
	    ಗಚ್ಛ theend;
	}
	dr = ((defer_T *)current_funccal->fc_defer.ga_data)
					  + current_funccal->fc_defer.ga_len++;
	dr->dr_name = saved_name;
	dr->dr_argcount = argcount;
	ಯಾವತ್ (argcount > 0)
	{
	    --argcount;
	    dr->dr_argvars[argcount] = argvars[argcount];
	}
	ret = OK;
    }

theend:
    ಯಾವತ್ (--argcount >= 0)
	clear_tv(&argvars[argcount]);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Invoked after a functions has finished: invoke ":defer" functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_defer_one(funccall_T *funccal)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx;

    ಪ್ರತಿ (idx = funccal->fc_defer.ga_len - 1; idx >= 0; --idx)
    {
	defer_T	    *dr = ((defer_T *)funccal->fc_defer.ga_data) + idx;

	ಯದಿ (dr->dr_name == ರಿಕ್ತ)
	    // already being called, can happen if function does ":qa"
	    ಅನುವರ್ತ;

	funcexe_T   funcexe;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;

	typval_T    rettv;
	rettv.v_type = VAR_UNKNOWN;	// clear_tv() uses this

	char_u *name = dr->dr_name;
	dr->dr_name = ರಿಕ್ತ;

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
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = dr->dr_argcount - 1; i >= 0; --i)
	    clear_tv(&dr->dr_argvars[i]);
    }
    ga_clear(&funccal->fc_defer);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_funccall_defer(funccall_T *fc)
{
    ಯದಿ (fc->fc_ectx != ರಿಕ್ತ)
    {
	// :def function
	unwind_def_callstack(fc->fc_ectx);
	may_invoke_defer_funcs(fc->fc_ectx);
    }
    ಅನ್ಯಥಾ
    {
	// legacy function
	handle_defer_one(fc);
    }
}

/*
 * Called when exiting: call all defer functions.
 */
    ಶೂನ್ಯ
invoke_all_defer(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (funccall_T *fc = current_funccal; fc != ರಿಕ್ತ; fc = fc->fc_caller)
	invoke_funccall_defer(fc);

    ಪ್ರತಿ (funccal_entry_T *fce = funccal_ರಾಶಿ; fce != ರಿಕ್ತ; fce = fce->next)
	ಪ್ರತಿ (funccall_T *fc = fce->ತುದಿ_funccal; fc != ರಿಕ್ತ; fc = fc->fc_caller)
	    invoke_funccall_defer(fc);
}

/*
 * ":1,25call func(arg1, arg2)"	function call.
 * ":defer func(arg1, arg2)"    deferred function call.
 */
    ಶೂನ್ಯ
ex_call(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*startarg;
    char_u	*name;
    char_u	*tofree;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;
    funcdict_T	fudi;
    ufunc_T	*ufunc = ರಿಕ್ತ;
    partial_T	*partial = ರಿಕ್ತ;
    evalarg_T	evalarg;
    type_T	*type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		found_var = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ಯದಿ (eap->skip)
    {
	typval_T	rettv;

	// trans_function_name() doesn't work well when skipping, use eval0()
	// instead to skip to any following command, e.g. for:
	//   :if 0 | call dict.foo().bar() | endif
	++emsg_skip;
	ಯದಿ (eval0(eap->arg, &rettv, eap, &evalarg) != FAIL)
	    clear_tv(&rettv);
	--emsg_skip;
	clear_evalarg(&evalarg, eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    tofree = trans_function_name_ext(&arg, ರಿಕ್ತ, FALSE, TFN_INT | TFN_NO_DECL,
			   &fudi, &partial, vim9script ? &type : ರಿಕ್ತ, &ufunc);
    ಯದಿ (fudi.fd_newkey != ರಿಕ್ತ)
    {
	// Still need to give an error message for missing key.
	semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), fudi.fd_newkey);
	vim_free(fudi.fd_newkey);
    }
    ಯದಿ (tofree == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Increase refcount on dictionary, it could get deleted when evaluating
    // the arguments.
    ಯದಿ (fudi.fd_dict != ರಿಕ್ತ)
	++fudi.fd_dict->dv_refcount;

    // If it is the name of a variable of type VAR_FUNC or VAR_PARTIAL use its
    // contents.  For VAR_PARTIAL get its partial, unless we already have one
    // from trans_function_name().
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(tofree);
    name = deref_func_name(tofree, &len, partial != ರಿಕ್ತ ? ರಿಕ್ತ : &partial,
				vim9script && type == ರಿಕ್ತ ? &type : ರಿಕ್ತ,
						     FALSE, FALSE, &found_var);

    ಯದಿ (*arg == '<')
    {
	// generic function call
	name = append_generic_func_type_args(name, STRLEN(name), &arg);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಗಚ್ಛ end;
	vim_free(tofree);
	tofree = name;
    }

    // Skip white space to allow ":call func ()".  Not good, but required for
    // backward compatibility.
    startarg = skipwhite(arg);
    ಯದಿ (*startarg != '(')
    {
	semsg(_(e_missing_parenthesis_str), eap->arg);
	ಗಚ್ಛ end;
    }
    ಯದಿ (vim9script && startarg > arg)
    {
	semsg(_(e_no_white_space_allowed_before_str_str), "(", eap->arg);
	ಗಚ್ಛ end;
    }

    ಯದಿ (eap->cmdidx == CMD_defer)
    {
	arg = startarg;
	failed = ex_defer_inner(name, &arg, type, partial, &evalarg) == FAIL;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ ((!aborting() || did_throw) && (!failed || eap->cstack->cs_trylevel > 0))
    {
	// Check for trailing illegal characters and a following command.
	arg = skipwhite(arg);
	ಯದಿ (!ends_excmd2(eap->arg, arg))
	{
	    ಯದಿ (!failed && !aborting())
	    {
		emsg_severe = TRUE;
		semsg(_(e_trailing_characters_str), arg);
	    }
	}
	ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
do_return(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		reanimate,
    ಪೂರ್ಣಾಙ್ಕ		is_cmd,
    ಶೂನ್ಯ	*rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    cstack_T	*cstack = eap->cstack;

    ಯದಿ (reanimate)
	// Undo the return.
	current_funccal->fc_returned = FALSE;

    /*
     * Cleanup (and inactivate) conditionals, but stop when a try conditional
     * not in its finally clause (which then is to be executed next) is found.
     * In this case, make the ":return" pending for execution at the ":endtry".
     * Otherwise, return normally.
     */
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ಯದಿ (idx >= 0)
    {
	cstack->cs_pending[idx] = CSTP_RETURN;

	ಯದಿ (!is_cmd && !reanimate)
	    // A pending return again gets pending.  "rettv" points to an
	    // allocated variable with the rettv of the original ":return"'s
	    // argument if present or is NULL else.
	    cstack->cs_rettv[idx] = rettv;
	ಅನ್ಯಥಾ
	{
	    // When undoing a return in order to make it pending, get the stored
	    // return rettv.
	    ಯದಿ (reanimate)
		rettv = current_funccal->fc_rettv;

	    ಯದಿ (rettv != ರಿಕ್ತ)
	    {
		// Store the value of the pending return.
		ಯದಿ ((cstack->cs_rettv[idx] = alloc_tv()) != ರಿಕ್ತ)
		    *(typval_T *)cstack->cs_rettv[idx] = *(typval_T *)rettv;
		ಅನ್ಯಥಾ
		    emsg(_(e_out_of_memory));
	    }
	    ಅನ್ಯಥಾ
		cstack->cs_rettv[idx] = ರಿಕ್ತ;

	    ಯದಿ (reanimate)
	    {
		// The pending return value could be overwritten by a ":return"
		// without argument in a finally clause; reset the default
		// return value.
		current_funccal->fc_rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		current_funccal->fc_rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	    }
	}
	report_make_pending(CSTP_RETURN, rettv);
    }
    ಅನ್ಯಥಾ
    {
	current_funccal->fc_returned = TRUE;

	// If the return is carried out now, store the return value.  For
	// a return immediately after reanimation, the value is already
	// there.
	ಯದಿ (!reanimate && rettv != ರಿಕ್ತ)
	{
	    clear_tv(current_funccal->fc_rettv);
	    *current_funccal->fc_rettv = *(typval_T *)rettv;
	    ಯದಿ (!is_cmd)
		vim_free(rettv);
	}
    }

    ಪ್ರತ್ಯಾಗಮ idx < 0;
}

/*
 * Free the variable with a pending return value.
 */
    ಶೂನ್ಯ
discard_pending_return(ಶೂನ್ಯ *rettv)
{
    free_tv((typval_T *)rettv);
}

/*
 * Generate a return command for producing the value of "rettv".  The result
 * is an allocated string.  Used by report_pending() for verbose messages.
 */
    char_u *
get_return_cmd(ಶೂನ್ಯ *rettv)
{
    char_u	*s = ರಿಕ್ತ;
    char_u	*tofree = ರಿಕ್ತ;
    char_u	numbuf[NUMBUFLEN];
    size_t	slen = 0;
    size_t	IObufflen;

    ಯದಿ (rettv != ರಿಕ್ತ)
	s = echo_string((typval_T *)rettv, &tofree, numbuf, 0);
    ಯದಿ (s == ರಿಕ್ತ)
	s = (char_u *)"";
    ಅನ್ಯಥಾ
	slen = STRLEN(s);

    STRCPY(IObuff, ":return ");
    STRNCPY(IObuff + 8, s, IOSIZE - 8);
    IObufflen = 8 + slen;
    ಯದಿ (IObufflen >= IOSIZE)
    {
	STRCPY(IObuff + IOSIZE - 4, "...");
	IObufflen = IOSIZE - 1;
    }
    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ vim_strnsave(IObuff, IObufflen);
}

/*
 * Get next function line.
 * Called by do_cmdline() to get the next line.
 * Returns allocated string, or NULL for end of function.
 */
    char_u *
get_func_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ	    c UNUSED,
    ಶೂನ್ಯ    *cookie,
    ಪೂರ್ಣಾಙ್ಕ	    indent UNUSED,
    getline_opt_T options UNUSED)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;
    char_u	*retval;
    garray_T	*gap;  // growarray with function lines

    // If breakpoints have been added/deleted need to check for it.
    ಯದಿ (fcp->fc_dbg_tick != debug_tick)
    {
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }
#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	func_ಸಾಲು_end(cookie);
#endif

    gap = &fp->uf_ಸಾಲುಗಳು;
    ಯದಿ (((fp->uf_flags & FC_ABORT) && did_emsg && !aborted_in_try())
	    || fcp->fc_returned)
	retval = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	// Skip NULL lines (continuation lines).
	ಯಾವತ್ (fcp->fc_linenr < gap->ga_len
			&& ((char_u **)(gap->ga_data))[fcp->fc_linenr] == ರಿಕ್ತ)
	    ++fcp->fc_linenr;
	ಯದಿ (fcp->fc_linenr >= gap->ga_len)
	    retval = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    retval = vim_strsave(((char_u **)(gap->ga_data))[fcp->fc_linenr++]);
	    SOURCING_LNUM = fcp->fc_linenr;
#ifdef FEAT_PROFILE
	    ಯದಿ (do_profiling == PROF_YES)
		func_ಸಾಲು_start(cookie, SOURCING_LNUM);
#endif
	}
    }

    // Did we encounter a breakpoint?
    ಯದಿ (fcp->fc_breakpoint != 0 && fcp->fc_breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(fp->uf_name, SOURCING_LNUM);
	// Find next breakpoint.
	fcp->fc_breakpoint = dbg_find_breakpoint(FALSE, fp->uf_name,
							       SOURCING_LNUM);
	fcp->fc_dbg_tick = debug_tick;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if the currently active function should be ended, because a
 * return was encountered or an error occurred.  Used inside a ":while".
 */
    ಪೂರ್ಣಾಙ್ಕ
func_has_ended(ಶೂನ್ಯ *cookie)
{
    funccall_T  *fcp = (funccall_T *)cookie;

    // Ignore the "abort" flag if the abortion behavior has been changed due to
    // an error inside a try conditional.
    ಪ್ರತ್ಯಾಗಮ (((fcp->fc_func->uf_flags & FC_ABORT)
					      && did_emsg && !aborted_in_try())
	    || fcp->fc_returned);
}

/*
 * return TRUE if cookie indicates a function which "abort"s on errors.
 */
    ಪೂರ್ಣಾಙ್ಕ
func_has_abort(
    ಶೂನ್ಯ    *cookie)
{
    ಪ್ರತ್ಯಾಗಮ ((funccall_T *)cookie)->fc_func->uf_flags & FC_ABORT;
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
    ufunc_T	*fp = ರಿಕ್ತ;
    char_u	fname_buf[FLEN_FIXED + 1];
    dict_T	*selfdict = selfdict_in;

    ಯದಿ (rettv->v_type == VAR_PARTIAL  && rettv->vval.v_partial != ರಿಕ್ತ
				     && rettv->vval.v_partial->pt_func != ರಿಕ್ತ)
	fp = rettv->vval.v_partial->pt_func;
    ಅನ್ಯಥಾ
    {
	fname = rettv->v_type == VAR_FUNC ? rettv->vval.v_string
					 : rettv->vval.v_partial == ರಿಕ್ತ ? ರಿಕ್ತ
					      : rettv->vval.v_partial->pt_name;
	ಯದಿ (fname == ರಿಕ್ತ)
	{
	    // There is no point binding a dict to a NULL function, just create
	    // a function reference.
	    rettv->v_type = VAR_FUNC;
	    rettv->vval.v_string = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    char_u	*tofree = ರಿಕ್ತ;
	    funcerror_T	error;

	    // Translate "s:func" to the stored function name.
	    fname = fname_trans_sid(fname, fname_buf, &tofree, &error);
	    fp = find_func(fname, FALSE);
	    vim_free(tofree);
	}
    }

    ಯದಿ (fp != ರಿಕ್ತ && (fp->uf_flags & FC_DICT))
    {
	partial_T	*pt = ALLOC_CLEAR_ONE(partial_T);

	ಯದಿ (pt != ರಿಕ್ತ)
	{
	    pt->pt_refcount = 1;
	    pt->pt_dict = selfdict;
	    pt->pt_auto = TRUE;
	    selfdict = ರಿಕ್ತ;
	    ಯದಿ (rettv->v_type == VAR_FUNC)
	    {
		// Just a function: Take over the function name and use
		// selfdict.
		pt->pt_name = rettv->vval.v_string;
	    }
	    ಅನ್ಯಥಾ
	    {
		partial_T	*ret_pt = rettv->vval.v_partial;
		ಪೂರ್ಣಾಙ್ಕ		i;

		// Partial: copy the function name, use selfdict and copy
		// args.  Can't take over name or args, the partial might
		// be referenced elsewhere.
		ಯದಿ (ret_pt->pt_name != ರಿಕ್ತ)
		{
		    pt->pt_name = vim_strsave(ret_pt->pt_name);
		    func_ref(pt->pt_name);
		}
		ಅನ್ಯಥಾ
		{
		    pt->pt_func = ret_pt->pt_func;
		    func_ptr_ref(pt->pt_func);
		}
		ಯದಿ (ret_pt->pt_argc > 0)
		{
		    pt->pt_argv = ALLOC_MULT(typval_T, ret_pt->pt_argc);
		    ಯದಿ (pt->pt_argv == ರಿಕ್ತ)
			// out of memory: drop the arguments
			pt->pt_argc = 0;
		    ಅನ್ಯಥಾ
		    {
			pt->pt_argc = ret_pt->pt_argc;
			ಪ್ರತಿ (i = 0; i < pt->pt_argc; i++)
			    copy_tv(&ret_pt->pt_argv[i], &pt->pt_argv[i]);
		    }
		}
		partial_unref(ret_pt);
	    }
	    rettv->v_type = VAR_PARTIAL;
	    rettv->vval.v_partial = pt;
	}
    }
    ಪ್ರತ್ಯಾಗಮ selfdict;
}

/*
 * Return the name of the executed function.
 */
    char_u *
func_name(ಶೂನ್ಯ *cookie)
{
    ಪ್ರತ್ಯಾಗಮ ((funccall_T *)cookie)->fc_func->uf_name;
}

/*
 * Return the address holding the next breakpoint line for a funccall cookie.
 */
    linenr_T *
func_breakpoint(ಶೂನ್ಯ *cookie)
{
    ಪ್ರತ್ಯಾಗಮ &((funccall_T *)cookie)->fc_breakpoint;
}

/*
 * Return the address holding the debug tick for a funccall cookie.
 */
    ಪೂರ್ಣಾಙ್ಕ *
func_dbg_tick(ಶೂನ್ಯ *cookie)
{
    ಪ್ರತ್ಯಾಗಮ &((funccall_T *)cookie)->fc_dbg_tick;
}

/*
 * Return the nesting level for a funccall cookie.
 */
    ಪೂರ್ಣಾಙ್ಕ
func_level(ಶೂನ್ಯ *cookie)
{
    ಪ್ರತ್ಯಾಗಮ ((funccall_T *)cookie)->fc_level;
}

/*
 * Return TRUE when a function was ended by a ":return" command.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_func_returned(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_funccal->fc_returned;
}

    ಪೂರ್ಣಾಙ್ಕ
free_unref_funccal(ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ testing)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_free_funccal = FALSE;
    funccall_T	*fc, **pfc;

    ಪ್ರತಿ (pfc = &previous_funccal; *pfc != ರಿಕ್ತ; )
    {
	ಯದಿ (can_free_funccal(*pfc, copyID))
	{
	    fc = *pfc;
	    *pfc = fc->fc_caller;
	    free_funccal_contents(fc);
	    did_free = TRUE;
	    did_free_funccal = TRUE;
	}
	ಅನ್ಯಥಾ
	    pfc = &(*pfc)->fc_caller;
    }
    ಯದಿ (did_free_funccal)
	// When a funccal was freed some more items might be garbage
	// collected, so run again.
	(ಶೂನ್ಯ)garbage_collect(testing);

    ಪ್ರತ್ಯಾಗಮ did_free;
}

/*
 * Get function call environment based on backtrace debug level
 */
    ಸ್ಥಿರ funccall_T *
get_funccal(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    funccall_T	*funccal;
    funccall_T	*temp_funccal;

    funccal = current_funccal;
    ಯದಿ (debug_backtrace_level > 0)
    {
	ಪ್ರತಿ (i = 0; i < debug_backtrace_level; i++)
	{
	    temp_funccal = funccal->fc_caller;
	    ಯದಿ (temp_funccal)
		funccal = temp_funccal;
	    ಅನ್ಯಥಾ
		// backtrace level overflow. reset to max
		debug_backtrace_level = i;
	}
    }
    ಪ್ರತ್ಯಾಗಮ funccal;
}

/*
 * Get the function call environment to use for the l: and a: variables, based
 * on the backtrace debug level.
 * Returns NULL if there is no current funccal or when the selected funccal is
 * for a :def function, which does not have l: and a: dictionaries.
 */
    ಸ್ಥಿರ funccall_T *
get_funccal_for_vars(ಶೂನ್ಯ)
{
    funccall_T	*funccal = ರಿಕ್ತ;

    ಯದಿ (current_funccal == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    funccal = get_funccal();
    ಯದಿ (funccal == ರಿಕ್ತ || funccal->fc_l_vars.dv_refcount == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ funccal;
}

/*
 * Return the hashtable used for local variables in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_local_ht(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಪ್ರತ್ಯಾಗಮ funccal == ರಿಕ್ತ ? ರಿಕ್ತ : &funccal->fc_l_vars.dv_hashtab;
}

/*
 * Return the l: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_local_var(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಪ್ರತ್ಯಾಗಮ funccal == ರಿಕ್ತ ? ರಿಕ್ತ : &funccal->fc_l_vars_var;
}

/*
 * Return the hashtable used for argument in the current funccal.
 * Return NULL if there is no current funccal.
 */
    hashtab_T *
get_funccal_args_ht(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಪ್ರತ್ಯಾಗಮ funccal == ರಿಕ್ತ ? ರಿಕ್ತ : &funccal->fc_l_avars.dv_hashtab;
}

/*
 * Return the a: scope variable.
 * Return NULL if there is no current funccal.
 */
    dictitem_T *
get_funccal_args_var(ಶೂನ್ಯ)
{
    funccall_T	*funccal = get_funccal_for_vars();

    ಪ್ರತ್ಯಾಗಮ funccal == ರಿಕ್ತ ? ರಿಕ್ತ : &funccal->fc_l_avars_var;
}

/*
 * List function variables, if there is a function.
 */
    ಶೂನ್ಯ
list_func_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    ಯದಿ (current_funccal != ರಿಕ್ತ && current_funccal->fc_l_vars.dv_refcount > 0)
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
    ಯದಿ (current_funccal != ರಿಕ್ತ
	    && ht == &current_funccal->fc_l_vars.dv_hashtab)
	ಪ್ರತ್ಯಾಗಮ &current_funccal->fc_l_vars;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Search hashitem in parent scope.
 */
    hashitem_T *
find_hi_in_scoped_ht(char_u *name, hashtab_T **pht)
{
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    hashitem_T	*hi = ರಿಕ್ತ;
    char_u	*varname;

    ಯದಿ (current_funccal == ರಿಕ್ತ || current_funccal->fc_func->uf_scoped == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Search in parent scope, which can be referenced from a lambda.
    current_funccal = current_funccal->fc_func->uf_scoped;
    ಯಾವತ್ (current_funccal != ರಿಕ್ತ)
    {
	ht = find_var_ht(name, &varname);
	ಯದಿ (ht != ರಿಕ್ತ && *varname != NUL)
	{
	    hi = hash_find(ht, varname);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
	    {
		*pht = ht;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ಭಙ್ಗ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ಪ್ರತ್ಯಾಗಮ hi;
}

/*
 * Search variable in parent scope.
 */
    dictitem_T *
find_var_in_scoped_ht(char_u *name, ಪೂರ್ಣಾಙ್ಕ no_autoload)
{
    dictitem_T	*v = ರಿಕ್ತ;
    funccall_T	*old_current_funccal = current_funccal;
    hashtab_T	*ht;
    char_u	*varname;

    ಯದಿ (current_funccal == ರಿಕ್ತ || current_funccal->fc_func->uf_scoped == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Search in parent scope which is possible to reference from lambda
    current_funccal = current_funccal->fc_func->uf_scoped;
    ಯಾವತ್ (current_funccal)
    {
	ht = find_var_ht(name, &varname);
	ಯದಿ (ht != ರಿಕ್ತ && *varname != NUL)
	{
	    v = find_var_in_ht(ht, *name, varname, no_autoload);
	    ಯದಿ (v != ರಿಕ್ತ)
		ಭಙ್ಗ;
	}
	ಯದಿ (current_funccal == current_funccal->fc_func->uf_scoped)
	    ಭಙ್ಗ;
	current_funccal = current_funccal->fc_func->uf_scoped;
    }
    current_funccal = old_current_funccal;

    ಪ್ರತ್ಯಾಗಮ v;
}

/*
 * Set "copyID + 1" in previous_funccal and callers.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_previous_funccal(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    funccall_T	*fc;

    ಪ್ರತಿ (fc = previous_funccal; fc != ರಿಕ್ತ; fc = fc->fc_caller)
    {
	fc->fc_copyID = copyID + 1;
	ಯದಿ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID + 1, ರಿಕ್ತ, ರಿಕ್ತ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID + 1, ರಿಕ್ತ, ರಿಕ್ತ)
		|| set_ref_in_list_ವಸ್ತುಗಳು(&fc->fc_l_varlist, copyID + 1, ರಿಕ್ತ, ರಿಕ್ತ))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_ref_in_funccal(funccall_T *fc, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಯದಿ (fc->fc_copyID != copyID)
    {
	fc->fc_copyID = copyID;
	ಯದಿ (set_ref_in_ht(&fc->fc_l_vars.dv_hashtab, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
		|| set_ref_in_ht(&fc->fc_l_avars.dv_hashtab, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
		|| set_ref_in_list_ವಸ್ತುಗಳು(&fc->fc_l_varlist, copyID, ರಿಕ್ತ, ರಿಕ್ತ)
		|| set_ref_in_func(ರಿಕ್ತ, fc->fc_func, copyID))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set "copyID" in all local vars and arguments in the call stack.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_call_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    funccall_T		*fc;
    funccal_entry_T	*entry;

    ಪ್ರತಿ (fc = current_funccal; fc != ರಿಕ್ತ; fc = fc->fc_caller)
	ಯದಿ (set_ref_in_funccal(fc, copyID))
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    // Also go through the funccal_stack.
    ಪ್ರತಿ (entry = funccal_ರಾಶಿ; entry != ರಿಕ್ತ; entry = entry->next)
	ಪ್ರತಿ (fc = entry->ತುದಿ_funccal; fc != ರಿಕ್ತ; fc = fc->fc_caller)
	    ಯದಿ (set_ref_in_funccal(fc, copyID))
		ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set "copyID" in all functions available by name.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_functions(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi = ರಿಕ್ತ;
    ufunc_T	*fp;

    todo = (ಪೂರ್ಣಾಙ್ಕ)func_hashtab.ht_used;
    ಪ್ರತಿ (hi = func_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ಯದಿ (!func_name_refcount(fp->uf_name)
					  && set_ref_in_func(ರಿಕ್ತ, fp, copyID))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Set "copyID" in all function arguments.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_func_args(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < funcargs.ga_len; ++i)
	ಯದಿ (set_ref_in_item(((typval_T **)funcargs.ga_data)[i],
						copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark all lists and dicts referenced through function "name" with "copyID".
 * Returns TRUE if setting references failed somehow.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_func(char_u *name, ufunc_T *fp_in, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ufunc_T	*fp = fp_in;
    funccall_T	*fc;
    funcerror_T	error = FCERR_NONE;
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ರಿಕ್ತ;
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;

    ಯದಿ (name == ರಿಕ್ತ && fp_in == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (fp_in == ರಿಕ್ತ)
    {
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	fp = find_func(fname, FALSE);
    }
    ಯದಿ (fp != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (fc = fp->uf_scoped; fc != ರಿಕ್ತ; fc = fc->fc_func->uf_scoped)
	    abort = abort || set_ref_in_funccal(fc, copyID);
    }

    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ abort;
}

#endif // FEAT_EVAL
